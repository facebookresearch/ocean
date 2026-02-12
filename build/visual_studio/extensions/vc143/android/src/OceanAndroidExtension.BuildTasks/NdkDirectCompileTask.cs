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
using System.Text;
using System.Text.RegularExpressions;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace OceanAndroidExtension.BuildTasks
{

/**
 * This class implements an MSBuild task that compiles native C++ code by directly invoking the NDK's clang compiler.
 * This approach provides better Visual Studio integration than CMake, with native error parsing and proper incremental build support through MSBuild's dependency tracking.
 * @ingroup oceanandroidextension
 */
public class NdkDirectCompileTask : Task
{
	/// The regex pattern for parsing compiler errors.
	private static readonly Regex ErrorRegex = new Regex(@"^(?<file>.+?):(?<line>\d+):(?<col>\d+):\s*(?<type>error|warning|note):\s*(?<msg>.+)$", RegexOptions.Compiled);

	/// The path to the Android NDK root directory.
	[Required]
	public string NdkPath { get; set; } = string.Empty;

	/// The source files to compile.
	[Required]
	public ITaskItem[] SourceFiles { get; set; } = Array.Empty<ITaskItem>();

	/// The output directory for object files and the final library.
	[Required]
	public string OutputDirectory { get; set; } = string.Empty;

	/// The name of the output library (without lib prefix or .so suffix).
	[Required]
	public string LibraryName { get; set; } = string.Empty;

	/// The target ABI.
	public string TargetAbi { get; set; } = "arm64-v8a";

	/// The minimum Android API level.
	public int MinApiLevel { get; set; } = 24;

	/// The C++ standard.
	public string CppStandard { get; set; } = "c++17";

	/// The C++ STL to use.
	public string Stl { get; set; } = "c++_shared";

	/// The build configuration.
	public string Configuration { get; set; } = "Debug";

	/// Additional include directories (semicolon-separated).
	public string AdditionalIncludeDirectories { get; set; } = string.Empty;

	/// Preprocessor definitions (semicolon-separated).
	public string PreprocessorDefinitions { get; set; } = string.Empty;

	/// Additional compiler flags.
	public string AdditionalCompilerFlags { get; set; } = string.Empty;

	/// Additional linker flags.
	public string AdditionalLinkerFlags { get; set; } = string.Empty;

	/// Additional libraries to link (semicolon-separated, without -l prefix).
	public string AdditionalLibraries { get; set; } = string.Empty;

	/// True, to enable position-independent code.
	public bool PositionIndependentCode { get; set; } = true;

	/// True, to enable debug symbols.
	public bool GenerateDebugInfo { get; set; } = true;

	/// The optimization level.
	public string OptimizationLevel { get; set; } = string.Empty;

	/// The number of parallel compilation jobs.
	public int ParallelJobs { get; set; } = 0;

	/// Output: The path to the compiled shared library.
	[Output]
	public string OutputLibrary { get; set; } = string.Empty;

	/// Output: The compiled object files.
	[Output]
	public ITaskItem[] ObjectFiles { get; set; } = Array.Empty<ITaskItem>();

	/**
	 * Executes the NDK direct compile task.
	 * @return True, if the build succeeded
	 */
	public override bool Execute()
	{
		try
		{
			if (!ValidateNdk())
			{
				return false;
			}

			var toolchainPath = GetToolchainPath();
			var clangPath = Path.Combine(toolchainPath, "bin", "clang++.exe");
			var sysrootPath = Path.Combine(toolchainPath, "sysroot");

			if (!File.Exists(clangPath))
			{
				clangPath = Path.Combine(toolchainPath, "bin", "clang++");
				if (!File.Exists(clangPath))
				{
					Log.LogError($"Clang compiler not found at: {clangPath}");
					return false;
				}
			}

			var targetTriple = GetTargetTriple();
			var objectDirectory = Path.Combine(OutputDirectory, "obj", TargetAbi);
			Directory.CreateDirectory(objectDirectory);
			Directory.CreateDirectory(OutputDirectory);

			Log.LogMessage(MessageImportance.High, "========================================");
			Log.LogMessage(MessageImportance.High, $"NDK Direct Compile: {LibraryName}");
			Log.LogMessage(MessageImportance.High, $"  ABI: {TargetAbi}");
			Log.LogMessage(MessageImportance.High, $"  Target: {targetTriple}");
			Log.LogMessage(MessageImportance.High, $"  Sources: {SourceFiles.Length} files");
			Log.LogMessage(MessageImportance.High, "========================================");

			var objectFiles = new List<ITaskItem>();
			var compileTasks = new List<(ITaskItem Source, string ObjPath)>();

			foreach (var source in SourceFiles)
			{
				var sourcePath = source.GetMetadata("FullPath");
				if (string.IsNullOrEmpty(sourcePath))
				{
					sourcePath = source.ItemSpec;
				}

				var objFileName = Path.GetFileNameWithoutExtension(sourcePath) + ".o";
				var objPath = Path.Combine(objectDirectory, objFileName);
				compileTasks.Add((source, objPath));
			}

			bool allSucceeded = true;
			foreach (var (source, objPath) in compileTasks)
			{
				var sourcePath = source.GetMetadata("FullPath");
				if (string.IsNullOrEmpty(sourcePath))
				{
					sourcePath = source.ItemSpec;
				}

				if (!CompileSource(clangPath, sysrootPath, targetTriple, sourcePath, objPath))
				{
					allSucceeded = false;
				}
				else
				{
					var item = new TaskItem(objPath);
					item.SetMetadata("SourceFile", sourcePath);
					item.SetMetadata("AndroidAbi", TargetAbi);
					objectFiles.Add(item);
				}
			}

			if (!allSucceeded)
			{
				Log.LogError("Compilation failed. See errors above.");
				return false;
			}

			ObjectFiles = objectFiles.ToArray();

			var libName = $"lib{LibraryName}.so";
			var libPath = Path.Combine(OutputDirectory, TargetAbi, libName);
			Directory.CreateDirectory(Path.GetDirectoryName(libPath)!);

			if (!LinkLibrary(clangPath, sysrootPath, targetTriple, objectFiles, libPath))
			{
				return false;
			}

			OutputLibrary = libPath;

			Log.LogMessage(MessageImportance.High, "========================================");
			Log.LogMessage(MessageImportance.High, $"Build succeeded: {libPath}");
			Log.LogMessage(MessageImportance.High, "========================================");

			return true;
		}
		catch (Exception exception)
		{
			Log.LogErrorFromException(exception, showStackTrace: true);
			return false;
		}
	}

	/**
	 * Validates the NDK installation.
	 * @return True, if the NDK is valid
	 */
	private bool ValidateNdk()
	{
		if (string.IsNullOrEmpty(NdkPath))
		{
			Log.LogError("NdkPath is required.");
			return false;
		}

		if (!Directory.Exists(NdkPath))
		{
			Log.LogError($"NDK directory not found: {NdkPath}");
			return false;
		}

		var toolchainPath = GetToolchainPath();
		if (!Directory.Exists(toolchainPath))
		{
			Log.LogError($"NDK toolchain not found: {toolchainPath}");
			return false;
		}

		return true;
	}

	/**
	 * Returns the toolchain path.
	 * @return The toolchain path
	 */
	private string GetToolchainPath()
	{
		var hostTag = GetHostTag();
		return Path.Combine(NdkPath, "toolchains", "llvm", "prebuilt", hostTag);
	}

	/**
	 * Returns the host tag.
	 * @return The host tag
	 */
	private string GetHostTag()
	{
		if (Environment.OSVersion.Platform == PlatformID.Win32NT)
		{
			return "windows-x86_64";
		}
		else if (Environment.OSVersion.Platform == PlatformID.Unix)
		{
			if (Directory.Exists("/Applications"))
			{
				return "darwin-x86_64";
			}
			return "linux-x86_64";
		}
		return "windows-x86_64";
	}

	/**
	 * Returns the target triple.
	 * @return The target triple
	 */
	private string GetTargetTriple()
	{
		return TargetAbi switch
		{
			"arm64-v8a" => $"aarch64-linux-android{MinApiLevel}",
			"armeabi-v7a" => $"armv7a-linux-androideabi{MinApiLevel}",
			"x86_64" => $"x86_64-linux-android{MinApiLevel}",
			"x86" => $"i686-linux-android{MinApiLevel}",
			_ => throw new ArgumentException($"Unknown ABI: {TargetAbi}")
		};
	}

	/**
	 * Returns the architecture-specific flags.
	 * @return The architecture flags
	 */
	private string GetArchitectureFlags()
	{
		return TargetAbi switch
		{
			"armeabi-v7a" => "-march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16",
			"arm64-v8a" => "",
			"x86" => "-mstackrealign",
			"x86_64" => "",
			_ => ""
		};
	}

	/**
	 * Compiles a source file.
	 * @param clangPath The clang path
	 * @param sysrootPath The sysroot path
	 * @param targetTriple The target triple
	 * @param sourcePath The source file path
	 * @param objPath The output object file path
	 * @return True, if the compilation succeeded
	 */
	private bool CompileSource(string clangPath, string sysrootPath, string targetTriple, string sourcePath, string objPath)
	{
		var arguments = new StringBuilder();

		arguments.Append($"-target {targetTriple} ");
		arguments.Append($"--sysroot=\"{sysrootPath}\" ");

		var archFlags = GetArchitectureFlags();
		if (!string.IsNullOrEmpty(archFlags))
		{
			arguments.Append($"{archFlags} ");
		}

		arguments.Append($"-std={CppStandard} ");

		if (PositionIndependentCode)
		{
			arguments.Append("-fPIC ");
		}

		if (Configuration.Equals("Debug", StringComparison.OrdinalIgnoreCase))
		{
			if (GenerateDebugInfo)
			{
				arguments.Append("-g ");
			}
			arguments.Append("-O0 ");
			arguments.Append("-D_DEBUG ");
			arguments.Append("-DDEBUG=1 ");
		}
		else
		{
			var optLevel = string.IsNullOrEmpty(OptimizationLevel) ? "2" : OptimizationLevel;
			arguments.Append($"-O{optLevel} ");
			arguments.Append("-DNDEBUG ");
		}

		arguments.Append("-DANDROID ");
		arguments.Append($"-D__ANDROID_API__={MinApiLevel} ");

		if (!string.IsNullOrEmpty(AdditionalIncludeDirectories))
		{
			foreach (var directory in AdditionalIncludeDirectories.Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries))
			{
				arguments.Append($"-I\"{directory.Trim()}\" ");
			}
		}

		if (!string.IsNullOrEmpty(PreprocessorDefinitions))
		{
			foreach (var def in PreprocessorDefinitions.Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries))
			{
				arguments.Append($"-D{def.Trim()} ");
			}
		}

		if (!string.IsNullOrEmpty(AdditionalCompilerFlags))
		{
			arguments.Append($"{AdditionalCompilerFlags} ");
		}

		arguments.Append("-c ");
		arguments.Append($"\"{sourcePath}\" ");
		arguments.Append($"-o \"{objPath}\"");

		Log.LogMessage(MessageImportance.Low, $"Compiling: {Path.GetFileName(sourcePath)}");
		Log.LogMessage(MessageImportance.Low, $"  {clangPath} {arguments}");

		return RunCompiler(clangPath, arguments.ToString(), Path.GetDirectoryName(sourcePath)!);
	}

	/**
	 * Links object files into a shared library.
	 * @param clangPath The clang path
	 * @param sysrootPath The sysroot path
	 * @param targetTriple The target triple
	 * @param objectFiles The object files
	 * @param outputPath The output library path
	 * @return True, if the linking succeeded
	 */
	private bool LinkLibrary(string clangPath, string sysrootPath, string targetTriple, List<ITaskItem> objectFiles, string outputPath)
	{
		var arguments = new StringBuilder();

		arguments.Append($"-target {targetTriple} ");
		arguments.Append($"--sysroot=\"{sysrootPath}\" ");

		arguments.Append("-shared ");

		var archFlags = GetArchitectureFlags();
		if (!string.IsNullOrEmpty(archFlags))
		{
			arguments.Append($"{archFlags} ");
		}

		var toolchainPath = GetToolchainPath();
		if (Stl == "c++_shared" || Stl == "c++_static")
		{
			var stlLibraryDirectory = GetStlLibraryPath(toolchainPath);
			if (!string.IsNullOrEmpty(stlLibraryDirectory) && Directory.Exists(stlLibraryDirectory))
			{
				arguments.Append($"-L\"{stlLibraryDirectory}\" ");
			}
			arguments.Append(Stl == "c++_shared" ? "-lc++_shared " : "-lc++_static -lc++abi ");
		}

		foreach (var obj in objectFiles)
		{
			arguments.Append($"\"{obj.ItemSpec}\" ");
		}

		arguments.Append($"-o \"{outputPath}\" ");

		arguments.Append("-llog ");
		arguments.Append("-landroid ");

		if (!string.IsNullOrEmpty(AdditionalLibraries))
		{
			foreach (var lib in AdditionalLibraries.Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries))
			{
				arguments.Append($"-l{lib.Trim()} ");
			}
		}

		if (!string.IsNullOrEmpty(AdditionalLinkerFlags))
		{
			arguments.Append($"{AdditionalLinkerFlags} ");
		}

		Log.LogMessage(MessageImportance.Normal, $"Linking: {Path.GetFileName(outputPath)}");
		Log.LogMessage(MessageImportance.Low, $"  {clangPath} {arguments}");

		return RunCompiler(clangPath, arguments.ToString(), Path.GetDirectoryName(outputPath)!);
	}

	/**
	 * Returns the STL library path.
	 * @param toolchainPath The toolchain path
	 * @return The STL library path
	 */
	private string GetStlLibraryPath(string toolchainPath)
	{
		var targetDirectory = TargetAbi switch
		{
			"arm64-v8a" => "aarch64-linux-android",
			"armeabi-v7a" => "arm-linux-androideabi",
			"x86_64" => "x86_64-linux-android",
			"x86" => "i686-linux-android",
			_ => ""
		};

		return Path.Combine(toolchainPath, "sysroot", "usr", "lib", targetDirectory);
	}

	/**
	 * Runs the compiler.
	 * @param compiler The compiler path
	 * @param arguments The compiler arguments
	 * @param workingDirectory The working directory
	 * @return True, if the compilation succeeded
	 */
	private bool RunCompiler(string compiler, string arguments, string workingDirectory)
	{
		var startInfo = new ProcessStartInfo
		{
			FileName = compiler,
			Arguments = arguments,
			WorkingDirectory = workingDirectory,
			UseShellExecute = false,
			RedirectStandardOutput = true,
			RedirectStandardError = true,
			CreateNoWindow = true,
			StandardErrorEncoding = Encoding.UTF8,
			StandardOutputEncoding = Encoding.UTF8
		};

		using var process = new Process { StartInfo = startInfo };

		var outputBuilder = new StringBuilder();
		var errorBuilder = new StringBuilder();

		process.OutputDataReceived += (sender, eventArgs) =>
		{
			if (!string.IsNullOrEmpty(eventArgs.Data))
			{
				outputBuilder.AppendLine(eventArgs.Data);
				ParseAndLogCompilerOutput(eventArgs.Data);
			}
		};

		process.ErrorDataReceived += (sender, eventArgs) =>
		{
			if (!string.IsNullOrEmpty(eventArgs.Data))
			{
				errorBuilder.AppendLine(eventArgs.Data);
				ParseAndLogCompilerOutput(eventArgs.Data);
			}
		};

		process.Start();
		process.BeginOutputReadLine();
		process.BeginErrorReadLine();
		process.WaitForExit();

		return process.ExitCode == 0;
	}

	/**
	 * Parses and logs compiler output.
	 * @param line The output line
	 */
	private void ParseAndLogCompilerOutput(string line)
	{
		var match = ErrorRegex.Match(line);
		if (match.Success)
		{
			var file = match.Groups["file"].Value;
			var lineNum = int.Parse(match.Groups["line"].Value);
			var col = int.Parse(match.Groups["col"].Value);
			var type = match.Groups["type"].Value;
			var message = match.Groups["msg"].Value;

			switch (type.ToLowerInvariant())
			{
				case "error":
					Log.LogError(null, null, null, file, lineNum, col, 0, 0, message);
					break;
				case "warning":
					Log.LogWarning(null, null, null, file, lineNum, col, 0, 0, message);
					break;
				case "note":
					Log.LogMessage(MessageImportance.Normal, $"{file}({lineNum},{col}): note: {message}");
					break;
			}
		}
		else if (line.Contains("error:"))
		{
			Log.LogError(line);
		}
		else if (line.Contains("warning:"))
		{
			Log.LogWarning(line);
		}
		else
		{
			Log.LogMessage(MessageImportance.Low, line);
		}
	}
}

}
