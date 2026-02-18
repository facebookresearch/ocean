/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;
using Microsoft.Win32;

namespace OceanAndroidExtension.BuildTasks
{

/**
 * This class implements an MSBuild task that invokes Gradle for Android builds.
 * @ingroup oceanandroidextension
 */
public class GradleBuildTask : CancelableTask
{
	/// The path to the Gradle executable or wrapper script.
	[Required]
	public string GradleExecutable { get; set; } = string.Empty;

	/// The project directory containing build.gradle.
	[Required]
	public string ProjectDirectory { get; set; } = string.Empty;

	/// The Gradle tasks to execute (e.g., "assembleDebug", "clean").
	[Required]
	public string Tasks { get; set; } = string.Empty;

	/// The path to the Android SDK.
	public string? AndroidSdkPath { get; set; }

	/// The path to the JDK.
	public string? JavaHome { get; set; }

	/// The build configuration (Debug/Release).
	public string Configuration { get; set; } = "Debug";

	/// Extra arguments to pass to Gradle.
	public string? ExtraArguments { get; set; }

	/// True, to use the Gradle daemon.
	public bool UseDaemon { get; set; } = true;

	/// True, to run in offline mode.
	public bool Offline { get; set; } = false;

	/// The maximum heap size for the Gradle JVM (e.g., "4g").
	public string? MaxHeapSize { get; set; }

	/// Output: The path to the generated APK.
	[Output]
	public string? ApkPath { get; set; }

	/**
	 * Executes the Gradle build task.
	 * @return True, if the build succeeded
	 */
	public override bool Execute()
	{
		try
		{
			if (!ValidateInputs())
			{
				return false;
			}

			if (IsCancelled)
			{
				Log.LogMessage(MessageImportance.High, "Build cancelled.");
				return false;
			}

			var arguments = BuildArguments();
			var environment = BuildEnvironment();

			Log.LogMessage(MessageImportance.High, $"Running Gradle: {GradleExecutable} {arguments}");

			if (!VerifyWrapperJar())
			{
				return false;
			}

			var exitCode = RunProcess(GradleExecutable, arguments, environment);

			if (IsCancelled)
			{
				Log.LogMessage(MessageImportance.High, "Build cancelled.");
				return false;
			}

			if (exitCode != 0)
			{
				Log.LogError($"Gradle build failed with exit code {exitCode}");
				return false;
			}

			FindOutputApk();

			Log.LogMessage(MessageImportance.High, "Gradle build completed successfully");
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
		if (string.IsNullOrEmpty(GradleExecutable))
		{
			Log.LogError("GradleExecutable is required");
			return false;
		}

		if (!GradleExecutable.Equals("gradle", StringComparison.OrdinalIgnoreCase) && !File.Exists(GradleExecutable))
		{
			Log.LogError($"Gradle executable not found: {GradleExecutable}");
			return false;
		}

		if (string.IsNullOrEmpty(ProjectDirectory) || !Directory.Exists(ProjectDirectory))
		{
			Log.LogError($"Project directory not found: {ProjectDirectory}");
			return false;
		}

		var buildGradle = Path.Combine(ProjectDirectory, "build.gradle");
		var buildGradleKts = Path.Combine(ProjectDirectory, "build.gradle.kts");

		if (!File.Exists(buildGradle) && !File.Exists(buildGradleKts))
		{
			buildGradle = Path.Combine(ProjectDirectory, "app", "build.gradle");
			buildGradleKts = Path.Combine(ProjectDirectory, "app", "build.gradle.kts");

			if (!File.Exists(buildGradle) && !File.Exists(buildGradleKts))
			{
				Log.LogError("No build.gradle or build.gradle.kts found in project directory");
				return false;
			}
		}

		return true;
	}

	/**
	 * Builds the Gradle command line arguments.
	 * @return The Gradle arguments
	 */
	private string BuildArguments()
	{
		var stringBuilder = new StringBuilder();

		stringBuilder.Append(Tasks);

		if (!UseDaemon)
		{
			stringBuilder.Append(" --no-daemon");
		}

		if (Offline)
		{
			stringBuilder.Append(" --offline");
		}

		stringBuilder.Append(" --stacktrace");

		if (!string.IsNullOrEmpty(ExtraArguments))
		{
			stringBuilder.Append(' ');
			stringBuilder.Append(ExtraArguments);
		}

		return stringBuilder.ToString();
	}

	/**
	 * Builds the environment variables for Gradle.
	 * @return The environment variables dictionary
	 */
	private System.Collections.Generic.Dictionary<string, string> BuildEnvironment()
	{
		var environment = new System.Collections.Generic.Dictionary<string, string>();

		if (!string.IsNullOrEmpty(AndroidSdkPath))
		{
			environment["ANDROID_HOME"] = AndroidSdkPath!;
			environment["ANDROID_SDK_ROOT"] = AndroidSdkPath!;
		}

		if (!string.IsNullOrEmpty(JavaHome))
		{
			environment["JAVA_HOME"] = JavaHome!;
		}

		if (!string.IsNullOrEmpty(MaxHeapSize))
		{
			environment["GRADLE_OPTS"] = $"-Xmx{MaxHeapSize}";
		}

		return environment;
	}

	/**
	 * Runs a process and returns the exit code.
	 * @param fileName The executable path
	 * @param arguments The command line arguments
	 * @param environment The environment variables
	 * @return The process exit code
	 */
	private int RunProcess(string fileName, string arguments, System.Collections.Generic.Dictionary<string, string> environment)
	{
		var startInfo = new ProcessStartInfo
		{
			FileName = fileName,
			Arguments = arguments,
			WorkingDirectory = ProjectDirectory,
			UseShellExecute = false,
			RedirectStandardOutput = true,
			RedirectStandardError = true,
			CreateNoWindow = true
		};

		foreach (var keyValuePair in environment)
		{
			startInfo.Environment[keyValuePair.Key] = keyValuePair.Value;
		}

		using var process = new Process { StartInfo = startInfo };

		process.OutputDataReceived += (sender, eventArgs) =>
		{
			if (!string.IsNullOrEmpty(eventArgs.Data))
			{
				Log.LogMessage(MessageImportance.Normal, eventArgs.Data);
			}
		};

		process.ErrorDataReceived += (sender, eventArgs) =>
		{
			if (!string.IsNullOrEmpty(eventArgs.Data))
			{
				if (eventArgs.Data.Contains("FAILURE") || eventArgs.Data.Contains("ERROR") || eventArgs.Data.StartsWith("e:"))
				{
					Log.LogError(eventArgs.Data);
				}
				else
				{
					Log.LogMessage(MessageImportance.Normal, eventArgs.Data);
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

	/**
	 * Verifies the integrity of gradle-wrapper.jar against a .sha256 checksum file.
	 * Looks for the checksum in the extension's toolset directory (via registry),
	 * falling back to a file next to the JAR itself.
	 * If no .sha256 file is found, logs a warning and returns true.
	 * @return True if verification passed or was skipped, false if the hash mismatched
	 */
	private bool VerifyWrapperJar()
	{
		try
		{
			// Derive the JAR path from GradleExecutable (gradlew.bat -> wrapper/gradle-wrapper.jar)
			var wrapperDir = Path.GetDirectoryName(GradleExecutable);
			if (string.IsNullOrEmpty(wrapperDir))
			{
				return true;
			}

			var jarPath = Path.Combine(wrapperDir, "wrapper", "gradle-wrapper.jar");

			if (!File.Exists(jarPath))
			{
				// JAR not found at expected location; let Gradle handle this
				return true;
			}

			// Look for .sha256 in extension's toolset directory (registry path),
			// then fall back to a file next to the JAR
			string? hashFilePath = null;

			using (var key = Registry.CurrentUser.OpenSubKey(@"Software\OceanAndroidExtension"))
			{
				var extensionPath = key?.GetValue("ExtensionPath") as string;
				if (!string.IsNullOrEmpty(extensionPath))
				{
					var toolsetPath = Path.Combine(extensionPath, "toolset", "GradleWrapper", "gradle-wrapper.jar.sha256");
					if (File.Exists(toolsetPath))
					{
						hashFilePath = toolsetPath;
					}
				}
			}

			if (hashFilePath == null)
			{
				var localPath = jarPath + ".sha256";
				if (File.Exists(localPath))
				{
					hashFilePath = localPath;
				}
			}

			if (hashFilePath == null)
			{
				Log.LogWarning("[OceanAndroid] gradle-wrapper.jar.sha256 not found. Skipping integrity check.");
				return true;
			}

			var expectedHash = File.ReadLines(hashFilePath)
				.FirstOrDefault(line => !string.IsNullOrWhiteSpace(line) && !line.TrimStart().StartsWith("#"));

			if (string.IsNullOrWhiteSpace(expectedHash))
			{
				Log.LogWarning("[OceanAndroid] gradle-wrapper.jar.sha256 contains no hash. Skipping integrity check.");
				return true;
			}

			expectedHash = expectedHash.Trim();

			using (var sha256 = SHA256.Create())
			using (var stream = File.OpenRead(jarPath))
			{
				var hashBytes = sha256.ComputeHash(stream);
				var actualHash = BitConverter.ToString(hashBytes).Replace("-", "").ToLowerInvariant();

				if (!string.Equals(actualHash, expectedHash, StringComparison.OrdinalIgnoreCase))
				{
					Log.LogError($"[OceanAndroid] gradle-wrapper.jar integrity check FAILED.\n  Expected: {expectedHash}\n  Actual:   {actualHash}\n  File:     {jarPath}\nThe JAR may have been tampered with. Replace it with a known-good copy from https://services.gradle.org/distributions/");
					return false;
				}

				Log.LogMessage(MessageImportance.Normal, $"[OceanAndroid] gradle-wrapper.jar integrity verified (SHA-256: {actualHash})");
			}

			return true;
		}
		catch (Exception exception)
		{
			Log.LogWarning($"[OceanAndroid] Failed to verify gradle-wrapper.jar: {exception.Message}");
			return true;
		}
	}

	/**
	 * Finds the output APK file.
	 */
	private void FindOutputApk()
	{
		var buildType = Configuration.ToLowerInvariant();
		var searchPaths = new[]
		{
			Path.Combine(ProjectDirectory, "app", "build", "outputs", "apk", buildType),
			Path.Combine(ProjectDirectory, "build", "outputs", "apk", buildType)
		};

		foreach (var searchPath in searchPaths)
		{
			if (Directory.Exists(searchPath))
			{
				var apkFiles = Directory.GetFiles(searchPath, "*.apk");
				if (apkFiles.Length > 0)
				{
					ApkPath = apkFiles[0];
					Log.LogMessage(MessageImportance.High, $"Generated APK: {ApkPath}");
					return;
				}
			}
		}
	}
}

}
