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
using System.Text;
using System.Text.RegularExpressions;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace OceanAndroidExtension.BuildTasks
{

/**
 * This class implements an MSBuild task that deploys an APK to an Android device using ADB.
 * @ingroup oceanandroidextension
 */
public class AdbDeployTask : CancelableTask
{
	/// The path to the ADB executable.
	[Required]
	public string AdbPath { get; set; } = string.Empty;

	/// The path to the APK file to deploy.
	[Required]
	public string ApkPath { get; set; } = string.Empty;

	/// The target device serial number (uses first available device if empty).
	public string? DeviceSerial { get; set; }

	/// True, to reinstall the app (replace existing).
	public bool Reinstall { get; set; } = true;

	/// True, to allow downgrade.
	public bool AllowDowngrade { get; set; } = false;

	/// True, to grant all runtime permissions.
	public bool GrantPermissions { get; set; } = true;

	/// True, to launch the main activity after install.
	public bool LaunchActivity { get; set; } = true;

	/// Output: The package name of the deployed app.
	[Output]
	public string? PackageName { get; set; }

	/// Output: The main activity name.
	[Output]
	public string? ActivityName { get; set; }

	/**
	 * Executes the ADB deploy task.
	 * @return True, if the deployment succeeded
	 */
	public override bool Execute()
	{
		try
		{
			if (!ValidateInputs())
			{
				return false;
			}

			if (string.IsNullOrEmpty(DeviceSerial))
			{
				DeviceSerial = GetDefaultDevice();
				if (string.IsNullOrEmpty(DeviceSerial))
				{
					Log.LogError("No Android devices connected");
					return false;
				}
			}

			Log.LogMessage(MessageImportance.High, $"Deploying to device: {DeviceSerial}");

			if (!GetApkInfo())
			{
				Log.LogWarning("Could not determine package info from APK");
			}

			if (IsCancelled)
			{
				Log.LogMessage(MessageImportance.High, "Build cancelled.");
				return false;
			}

			if (!InstallApk())
			{
				return false;
			}

			if (IsCancelled)
			{
				Log.LogMessage(MessageImportance.High, "Build cancelled.");
				return false;
			}

			if (LaunchActivity && !string.IsNullOrEmpty(PackageName) && !string.IsNullOrEmpty(ActivityName))
			{
				LaunchApp();
			}

			Log.LogMessage(MessageImportance.High, "Deployment completed successfully");
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
		if (string.IsNullOrEmpty(AdbPath) || !File.Exists(AdbPath))
		{
			Log.LogError($"ADB not found: {AdbPath}");
			return false;
		}

		if (string.IsNullOrEmpty(ApkPath) || !File.Exists(ApkPath))
		{
			Log.LogError($"APK not found: {ApkPath}");
			return false;
		}

		return true;
	}

	/**
	 * Returns the default device serial.
	 * @return The default device serial, nullptr if no device is connected
	 */
	private string? GetDefaultDevice()
	{
		var output = RunAdb("devices -l");
		if (string.IsNullOrEmpty(output))
		{
			return null;
		}

		var lines = output!.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);

		foreach (var line in lines)
		{
			if (line.StartsWith("List of devices"))
			{
				continue;
			}
			if (string.IsNullOrWhiteSpace(line))
			{
				continue;
			}

			var parts = line.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
			if (parts.Length >= 2 && parts[1] == "device")
			{
				return parts[0];
			}
		}

		return null;
	}

	/**
	 * Extracts package information from the APK.
	 * @return True, if the package info was extracted
	 */
	private bool GetApkInfo()
	{
		var sdkPath = Path.GetDirectoryName(Path.GetDirectoryName(AdbPath));
		if (sdkPath == null)
		{
			return false;
		}

		var buildToolsPath = Path.Combine(sdkPath, "build-tools");
		if (!Directory.Exists(buildToolsPath))
		{
			return false;
		}

		var versions = Directory.GetDirectories(buildToolsPath);
		if (versions.Length == 0)
		{
			return false;
		}

		Array.Sort(versions);
		var latestVersion = versions[versions.Length - 1];
		var aapt2Path = Path.Combine(latestVersion, "aapt2.exe");

		if (!File.Exists(aapt2Path))
		{
			return false;
		}

		var output = RunProcess(aapt2Path, $"dump badging \"{ApkPath}\"");
		if (string.IsNullOrEmpty(output))
		{
			return false;
		}

		var packageMatch = Regex.Match(output, @"package:\s+name='([^']+)'");
		if (packageMatch.Success)
		{
			PackageName = packageMatch.Groups[1].Value;
		}

		var activityMatch = Regex.Match(output, @"launchable-activity:\s+name='([^']+)'");
		if (activityMatch.Success)
		{
			ActivityName = activityMatch.Groups[1].Value;
		}

		return !string.IsNullOrEmpty(PackageName);
	}

	/**
	 * Installs the APK to the device.
	 * @return True, if the installation succeeded
	 */
	private bool InstallApk()
	{
		var installArguments = $"-s {DeviceSerial} install";

		if (Reinstall)
		{
			installArguments += " -r";
		}

		if (AllowDowngrade)
		{
			installArguments += " -d";
		}

		if (GrantPermissions)
		{
			installArguments += " -g";
		}

		installArguments += $" \"{ApkPath}\"";

		Log.LogMessage(MessageImportance.Normal, $"Installing APK: {Path.GetFileName(ApkPath)}");

		var output = RunAdb(installArguments);

		if (output?.Contains("Success") == true)
		{
			Log.LogMessage(MessageImportance.High, "APK installed successfully");
			return true;
		}

		if (output?.Contains("INSTALL_FAILED") == true)
		{
			Log.LogError($"Installation failed: {output}");
			return false;
		}

		Log.LogError($"Installation failed with unknown error: {output}");
		return false;
	}

	/**
	 * Launches the app on the device.
	 */
	private void LaunchApp()
	{
		if (string.IsNullOrEmpty(PackageName) || string.IsNullOrEmpty(ActivityName))
		{
			return;
		}

		var activity = ActivityName!.StartsWith(".") ? $"{PackageName}{ActivityName}" : ActivityName;

		var launchArguments = $"-s {DeviceSerial} shell am start -n {PackageName}/{activity}";

		Log.LogMessage(MessageImportance.Normal, $"Launching: {PackageName}/{activity}");

		var output = RunAdb(launchArguments);

		if (output?.Contains("Error") == true)
		{
			Log.LogWarning($"Failed to launch activity: {output}");
		}
		else
		{
			Log.LogMessage(MessageImportance.High, "Application launched");
		}
	}

	/**
	 * Runs an ADB command.
	 * @param arguments The ADB arguments
	 * @return The command output
	 */
	private string? RunAdb(string arguments)
	{
		return RunProcess(AdbPath, arguments);
	}

	/**
	 * Runs a process and returns the output.
	 * Uses async output reads to avoid deadlocks when the child process
	 * writes to both stdout and stderr concurrently.
	 * @param fileName The executable path
	 * @param arguments The command line arguments
	 * @return The process output
	 */
	private string? RunProcess(string fileName, string arguments)
	{
		try
		{
			var startInfo = new ProcessStartInfo
			{
				FileName = fileName,
				Arguments = arguments,
				UseShellExecute = false,
				RedirectStandardOutput = true,
				RedirectStandardError = true,
				CreateNoWindow = true
			};

			using var process = Process.Start(startInfo);
			if (process == null)
			{
				return null;
			}

			RegisterProcess(process);

			var outputBuilder = new StringBuilder();
			var errorBuilder = new StringBuilder();

			process.OutputDataReceived += (sender, eventArgs) =>
			{
				if (!string.IsNullOrEmpty(eventArgs.Data))
				{
					outputBuilder.AppendLine(eventArgs.Data);
				}
			};

			process.ErrorDataReceived += (sender, eventArgs) =>
			{
				if (!string.IsNullOrEmpty(eventArgs.Data))
				{
					errorBuilder.AppendLine(eventArgs.Data);
				}
			};

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

			var error = errorBuilder.ToString();
			if (!string.IsNullOrEmpty(error))
			{
				Log.LogMessage(MessageImportance.Low, $"stderr: {error}");
			}

			return outputBuilder.ToString() + error;
		}
		catch (Exception exception)
		{
			Log.LogWarning($"Process execution failed: {exception.Message}");
			return null;
		}
	}
}

}
