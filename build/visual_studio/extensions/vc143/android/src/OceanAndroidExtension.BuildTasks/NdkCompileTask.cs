/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace OceanAndroidExtension.BuildTasks
{

/**
 * This class implements an MSBuild task that compiles native C++ code using the Android NDK.
 * @ingroup oceanandroidextension
 */
public class NdkCompileTask : CancelableTask
{
	/// The path to the Android NDK.
	[Required]
	public string NdkPath { get; set; } = string.Empty;

	/// The path to the CMake executable.
	[Required]
	public string CmakePath { get; set; } = string.Empty;

	/// The project directory containing CMakeLists.txt.
	[Required]
	public string ProjectDirectory { get; set; } = string.Empty;

	/// The target ABIs to build for (semicolon-separated).
	public string TargetAbis { get; set; } = "arm64-v8a;armeabi-v7a;x86_64;x86";

	/// The minimum Android API level.
	public int MinApiLevel { get; set; } = 24;

	/// The C++ STL to use (c++_shared, c++_static, none).
	public string Stl { get; set; } = "c++_shared";

	/// The build type (Debug/Release).
	public string BuildType { get; set; } = "Debug";

	/// The output directory for compiled libraries.
	public string? OutputDirectory { get; set; }

	/// Additional CMake arguments.
	public string? AdditionalCmakeArguments { get; set; }

	/// Output: The compiled .so files.
	[Output]
	public ITaskItem[]? OutputFiles { get; set; }

	/**
	 * Executes the NDK compile task.
	 * @return True, if the compilation succeeded
	 */
	public override bool Execute()
	{
		try
		{
			if (!ValidateInputs())
			{
				return false;
			}

			var abis = TargetAbis.Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries);
			var outputFiles = new List<ITaskItem>();

			foreach (var abi in abis)
			{
				if (IsCancelled)
				{
					Log.LogMessage(MessageImportance.High, "Build cancelled.");
					return false;
				}

				Log.LogMessage(MessageImportance.High, $"Building for ABI: {abi}");

				if (!ConfigureAndBuild(abi, out var outputs))
				{
					return false;
				}

				foreach (var output in outputs)
				{
					var item = new TaskItem(output);
					item.SetMetadata("AndroidAbi", abi);
					item.SetMetadata("CopyToOutputDirectory", "PreserveNewest");
					outputFiles.Add(item);
				}
			}

			if (IsCancelled)
			{
				Log.LogMessage(MessageImportance.High, "Build cancelled.");
				return false;
			}

			OutputFiles = outputFiles.ToArray();

			Log.LogMessage(MessageImportance.High, $"NDK build completed: {outputFiles.Count} libraries built");
			return true;
		}
		catch (Exception exception)
		{
			Log.LogErrorFromException(exception);
			return false;
		}
	}

	/**
	 * Validates the task inputs.
	 * @return True, if all inputs are valid
	 */
	private bool ValidateInputs()
	{
		if (string.IsNullOrEmpty(NdkPath) || !Directory.Exists(NdkPath))
		{
			Log.LogError($"NDK not found: {NdkPath}");
			return false;
		}

		if (string.IsNullOrEmpty(CmakePath))
		{
			Log.LogError("CMake path is required");
			return false;
		}

		if (!CmakePath.Equals("cmake", StringComparison.OrdinalIgnoreCase) && !File.Exists(CmakePath))
		{
			Log.LogError($"CMake not found: {CmakePath}");
			return false;
		}

		var cmakeLists = Path.Combine(ProjectDirectory, "CMakeLists.txt");
		if (!File.Exists(cmakeLists))
		{
			Log.LogError($"CMakeLists.txt not found in: {ProjectDirectory}");
			return false;
		}

		var toolchain = Path.Combine(NdkPath, "build", "cmake", "android.toolchain.cmake");
		if (!File.Exists(toolchain))
		{
			Log.LogError($"Android toolchain file not found: {toolchain}");
			return false;
		}

		return true;
	}

	/**
	 * Configures and builds for a specific ABI.
	 * @param abi The target ABI
	 * @param outputs The output file list
	 * @return True, if the build succeeded
	 */
	private bool ConfigureAndBuild(string abi, out List<string> outputs)
	{
		var outputList = new List<string>();
		outputs = outputList;

		var buildDirectory = Path.Combine(ProjectDirectory, "build", abi);
		var outputDirectory = OutputDirectory ?? Path.Combine(ProjectDirectory, "build", "outputs", abi);

		Directory.CreateDirectory(buildDirectory);
		Directory.CreateDirectory(outputDirectory);

		var toolchainFile = Path.Combine(NdkPath, "build", "cmake", "android.toolchain.cmake");

		var ninjaPath = FindNinja();

		var configureArgs = BuildConfigureArguments(abi, buildDirectory, outputDirectory, toolchainFile, ninjaPath);

		Log.LogMessage(MessageImportance.Normal, $"CMake configure: {configureArgs}");

		if (RunProcess(CmakePath, configureArgs, ProjectDirectory) != 0)
		{
			if (!IsCancelled)
			{
				Log.LogError($"CMake configuration failed for {abi}");
			}
			return false;
		}

		if (IsCancelled)
		{
			return false;
		}

		var buildArgs = $"--build \"{buildDirectory}\" --config {BuildType}";

		Log.LogMessage(MessageImportance.Normal, $"CMake build: {buildArgs}");

		if (RunProcess(CmakePath, buildArgs, ProjectDirectory) != 0)
		{
			if (!IsCancelled)
			{
				Log.LogError($"CMake build failed for {abi}");
			}
			return false;
		}

		if (Directory.Exists(outputDirectory))
		{
			outputList.AddRange(Directory.GetFiles(outputDirectory, "*.so", SearchOption.AllDirectories));
		}

		outputList.AddRange(Directory.GetFiles(buildDirectory, "*.so", SearchOption.AllDirectories).Where(file => !outputList.Contains(file)));

		Log.LogMessage(MessageImportance.Normal, $"Built {outputList.Count} libraries for {abi}");

		return true;
	}

	/**
	 * Builds the CMake configure arguments.
	 * @param abi The target ABI
	 * @param buildDirectory The build directory
	 * @param outputDirectory The output directory
	 * @param toolchainFile The toolchain file path
	 * @param ninjaPath The Ninja executable path (nullptr to use system Ninja)
	 * @return The CMake configure arguments
	 */
	private string BuildConfigureArguments(string abi, string buildDirectory, string outputDirectory, string toolchainFile, string? ninjaPath)
	{
		var arguments = new List<string>
		{
			"-G Ninja",
			$"-DCMAKE_TOOLCHAIN_FILE=\"{toolchainFile}\"",
			$"-DANDROID_ABI={abi}",
			$"-DANDROID_PLATFORM=android-{MinApiLevel}",
			$"-DANDROID_STL={Stl}",
			$"-DCMAKE_BUILD_TYPE={BuildType}",
			$"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=\"{outputDirectory}\""
		};

		if (!string.IsNullOrEmpty(ninjaPath))
		{
			arguments.Add($"-DCMAKE_MAKE_PROGRAM=\"{ninjaPath}\"");
		}

		if (!string.IsNullOrEmpty(AdditionalCmakeArguments))
		{
			arguments.Add(AdditionalCmakeArguments!);
		}

		arguments.Add($"-S \"{ProjectDirectory}\"");
		arguments.Add($"-B \"{buildDirectory}\"");

		return string.Join(" ", arguments);
	}

	/**
	 * Finds the Ninja executable.
	 * @return The path to Ninja, nullptr to use system Ninja
	 */
	private string? FindNinja()
	{
		var sdkPath = Environment.GetEnvironmentVariable("ANDROID_HOME") ?? Environment.GetEnvironmentVariable("ANDROID_SDK_ROOT");

		if (!string.IsNullOrEmpty(sdkPath))
		{
			var cmakeDirectory = Path.Combine(sdkPath, "cmake");
			if (Directory.Exists(cmakeDirectory))
			{
				var versions = Directory.GetDirectories(cmakeDirectory);
				foreach (var version in versions.OrderByDescending(versionDirectory => versionDirectory))
				{
					var ninjaPath = Path.Combine(version, "bin", "ninja.exe");
					if (File.Exists(ninjaPath))
					{
						return ninjaPath;
					}
				}
			}
		}

		return null;
	}

	/**
	 * Runs a process and returns the exit code.
	 * @param fileName The executable path
	 * @param arguments The command line arguments
	 * @param workingDirectory The working directory
	 * @return The process exit code
	 */
	private int RunProcess(string fileName, string arguments, string workingDirectory)
	{
		var startInfo = new ProcessStartInfo
		{
			FileName = fileName,
			Arguments = arguments,
			WorkingDirectory = workingDirectory,
			UseShellExecute = false,
			RedirectStandardOutput = true,
			RedirectStandardError = true,
			CreateNoWindow = true
		};

		using var process = new Process { StartInfo = startInfo };

		process.OutputDataReceived += (sender, eventArgs) =>
		{
			if (!string.IsNullOrEmpty(eventArgs.Data))
			{
				Log.LogMessage(MessageImportance.Low, eventArgs.Data);
			}
		};

		process.ErrorDataReceived += (sender, eventArgs) =>
		{
			if (!string.IsNullOrEmpty(eventArgs.Data))
			{
				if (eventArgs.Data.Contains("error:") || eventArgs.Data.Contains("fatal:"))
				{
					Log.LogError(eventArgs.Data);
				}
				else
				{
					Log.LogWarning(eventArgs.Data);
				}
			}
		};

		process.Start();
		RegisterProcess(process);
		process.BeginOutputReadLine();
		process.BeginErrorReadLine();

		try
		{
			process.WaitForExit();
		}
		finally
		{
			UnregisterProcess(process);
		}

		if (IsCancelled)
		{
			return -1;
		}

		return process.ExitCode;
	}
}

}
