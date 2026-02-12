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
using System.Threading;
using System.Threading.Tasks;

namespace OceanAndroidExtension.Services
{

/**
 * This class represents an Android device or emulator.
 * @ingroup oceanandroidextension
 */
public class AndroidDevice
{
	/// The device serial number.
	public string Serial { get; set; } = string.Empty;

	/// The device model name.
	public string Model { get; set; } = string.Empty;

	/// The product name.
	public string Product { get; set; } = string.Empty;

	/// The device codename.
	public string Device { get; set; } = string.Empty;

	/// The device connection state.
	public DeviceState State { get; set; }

	/// True, if the device is an emulator.
	public bool IsEmulator => Serial.StartsWith("emulator-");

	/// The Android API level.
	public int ApiLevel { get; set; }

	/// The Android version string (e.g., "13", "14").
	public string AndroidVersion { get; set; } = string.Empty;
}

/**
 * This enum defines the possible device connection states.
 * @ingroup oceanandroidextension
 */
public enum DeviceState
{
	/// Unknown device state.
	Unknown,

	/// Device is online and ready.
	Device,

	/// Device is offline.
	Offline,

	/// Device has not authorized USB debugging.
	Unauthorized,

	/// Device is in recovery mode.
	Recovery,

	/// Device is in sideload mode.
	Sideload
}

/**
 * This interface defines the ADB service for interacting with Android devices.
 * @ingroup oceanandroidextension
 */
public interface IAdbService
{
	/**
	 * Returns the connected devices.
	 * @param cancellationToken The cancellation token
	 * @return The list of connected devices
	 */
	Task<IReadOnlyList<AndroidDevice>> GetDevicesAsync(CancellationToken cancellationToken = default);

	/**
	 * Installs an APK to a device.
	 * @param deviceSerial The serial number of the target device
	 * @param apkPath The path to the APK file
	 * @param reinstall True, to reinstall the application
	 * @param cancellationToken The cancellation token
	 * @return True, if the installation succeeded
	 */
	Task<bool> InstallApkAsync(string deviceSerial, string apkPath, bool reinstall = false, CancellationToken cancellationToken = default);

	/**
	 * Uninstalls a package from a device.
	 * @param deviceSerial The serial number of the target device
	 * @param packageName The package name to uninstall
	 * @param cancellationToken The cancellation token
	 * @return True, if the uninstallation succeeded
	 */
	Task<bool> UninstallPackageAsync(string deviceSerial, string packageName, CancellationToken cancellationToken = default);

	/**
	 * Launches an activity on a device.
	 * @param deviceSerial The serial number of the target device
	 * @param packageName The package name containing the activity
	 * @param activityName The name of the activity to launch
	 * @param cancellationToken The cancellation token
	 * @return True, if the activity was launched successfully
	 */
	Task<bool> LaunchActivityAsync(string deviceSerial, string packageName, string activityName, CancellationToken cancellationToken = default);

	/**
	 * Runs a shell command on a device.
	 * @param deviceSerial The serial number of the target device
	 * @param command The shell command to execute
	 * @param cancellationToken The cancellation token
	 * @return The command output
	 */
	Task<string> ShellAsync(string deviceSerial, string command, CancellationToken cancellationToken = default);

	/**
	 * Pulls a file from a device.
	 * @param deviceSerial The serial number of the target device
	 * @param remotePath The path on the device
	 * @param localPath The local destination path
	 * @param cancellationToken The cancellation token
	 * @return True, if the file was pulled successfully
	 */
	Task<bool> PullFileAsync(string deviceSerial, string remotePath, string localPath, CancellationToken cancellationToken = default);

	/**
	 * Pushes a file to a device.
	 * @param deviceSerial The serial number of the target device
	 * @param localPath The local source path
	 * @param remotePath The destination path on the device
	 * @param cancellationToken The cancellation token
	 * @return True, if the file was pushed successfully
	 */
	Task<bool> PushFileAsync(string deviceSerial, string localPath, string remotePath, CancellationToken cancellationToken = default);

	/**
	 * Starts logcat streaming from a device.
	 * @param deviceSerial The serial number of the target device
	 * @param onOutput Callback for each log line
	 * @param cancellationToken The cancellation token
	 * @return The logcat process, nullptr on error
	 */
	Task<Process?> StartLogcatAsync(string deviceSerial, Action<string> onOutput, CancellationToken cancellationToken = default);

	/**
	 * Forwards a TCP port from a device to local host.
	 * @param deviceSerial The serial number of the target device
	 * @param localPort The local port number
	 * @param remotePort The remote port number on the device
	 * @param cancellationToken The cancellation token
	 * @return True, if port forwarding was established
	 */
	Task<bool> ForwardPortAsync(string deviceSerial, int localPort, int remotePort, CancellationToken cancellationToken = default);

	/**
	 * Returns package information from an APK file.
	 * @param apkPath The path to the APK file
	 * @param cancellationToken The cancellation token
	 * @return A tuple containing the package name and main activity, nullptr on error
	 */
	Task<(string PackageName, string ActivityName)?> GetApkInfoAsync(string apkPath, CancellationToken cancellationToken = default);

	/**
	 * Returns the process ID of a running application.
	 * @param deviceSerial The serial number of the target device
	 * @param packageName The package name of the application
	 * @param cancellationToken The cancellation token
	 * @return The process ID, nullptr if not found
	 */
	Task<int?> GetProcessIdAsync(string deviceSerial, string packageName, CancellationToken cancellationToken = default);

	/**
	 * Forwards a JDWP port for debugging.
	 * @param deviceSerial The serial number of the target device
	 * @param processId The process ID of the application
	 * @param localPort The local port number for JDWP
	 * @param cancellationToken The cancellation token
	 * @return The local port number, nullptr on error
	 */
	Task<int?> JdwpForwardAsync(string deviceSerial, int processId, int localPort, CancellationToken cancellationToken = default);
}

/**
 * This class implements the ADB service for interacting with Android devices.
 * @ingroup oceanandroidextension
 */
public class AdbService : IAdbService
{
	/// The SDK locator service for finding ADB.
	private readonly ISdkLocatorService sdkLocator_;

	/// The output window service for logging.
	private readonly IOutputWindowService outputService_;

	/**
	 * Creates a new ADB service.
	 * @param sdkLocator The SDK locator service for finding ADB
	 * @param outputService The output window service for logging
	 */
	public AdbService(ISdkLocatorService sdkLocator, IOutputWindowService outputService)
	{
		sdkLocator_ = sdkLocator;
		outputService_ = outputService;
	}

	/**
	 * Returns the path to the ADB executable.
	 * @return The path to adb, nullptr if not found
	 */
	private string? GetAdbPath()
	{
		return sdkLocator_.GetAdbPath();
	}

	/**
	 * Returns the connected devices.
	 * @param cancellationToken The cancellation token
	 * @return The list of connected devices
	 */
	public async Task<IReadOnlyList<AndroidDevice>> GetDevicesAsync(CancellationToken cancellationToken = default)
	{
		var result = await RunAdbAsync("devices -l", cancellationToken);
		if (result == null)
		{
			return Array.Empty<AndroidDevice>();
		}

		var devices = new List<AndroidDevice>();
		var lines = result.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);

		foreach (var line in lines.Skip(1))
		{
			var device = ParseDeviceLine(line);
			if (device != null)
			{
				await EnrichDeviceInfoAsync(device, cancellationToken);
				devices.Add(device);
			}
		}

		return devices;
	}

	/**
	 * Parses a device line from ADB devices output.
	 * @param line The line to parse
	 * @return The parsed device, nullptr if parsing failed
	 */
	private AndroidDevice? ParseDeviceLine(string line)
	{
		var parts = line.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
		if (parts.Length < 2)
		{
			return null;
		}

		var device = new AndroidDevice
		{
			Serial = parts[0],
			State = ParseState(parts[1])
		};

		foreach (var part in parts.Skip(2))
		{
			var keyValuePair = part.Split(':');
			if (keyValuePair.Length == 2)
			{
				switch (keyValuePair[0].ToLowerInvariant())
				{
					case "model":
						device.Model = keyValuePair[1];
						break;
					case "device":
						device.Device = keyValuePair[1];
						break;
					case "product":
						device.Product = keyValuePair[1];
						break;
				}
			}
		}

		return device;
	}

	/**
	 * Parses a device state string.
	 * @param state The state string
	 * @return The parsed device state
	 */
	private DeviceState ParseState(string state)
	{
		return state.ToLowerInvariant() switch
		{
			"device" => DeviceState.Device,
			"offline" => DeviceState.Offline,
			"unauthorized" => DeviceState.Unauthorized,
			"recovery" => DeviceState.Recovery,
			"sideload" => DeviceState.Sideload,
			_ => DeviceState.Unknown
		};
	}

	/**
	 * Enriches device information with additional properties.
	 * @param device The device to enrich
	 * @param cancellationToken The cancellation token
	 */
	private async Task EnrichDeviceInfoAsync(AndroidDevice device, CancellationToken cancellationToken)
	{
		if (device.State != DeviceState.Device)
		{
			return;
		}

		try
		{
			var apiLevel = await ShellAsync(device.Serial, "getprop ro.build.version.sdk", cancellationToken);
			if (int.TryParse(apiLevel?.Trim(), out var level))
			{
				device.ApiLevel = level;
			}

			var version = await ShellAsync(device.Serial, "getprop ro.build.version.release", cancellationToken);
			device.AndroidVersion = version?.Trim() ?? string.Empty;

			if (string.IsNullOrEmpty(device.Model))
			{
				var model = await ShellAsync(device.Serial, "getprop ro.product.model", cancellationToken);
				device.Model = model?.Trim() ?? string.Empty;
			}
		}
		catch
		{
			// Ignore enrichment errors
		}
	}

	/**
	 * Installs an APK to a device.
	 * @param deviceSerial The serial number of the target device
	 * @param apkPath The path to the APK file
	 * @param reinstall True, to reinstall the application
	 * @param cancellationToken The cancellation token
	 * @return True, if the installation succeeded
	 */
	public async Task<bool> InstallApkAsync(string deviceSerial, string apkPath, bool reinstall = false, CancellationToken cancellationToken = default)
	{
		var arguments = reinstall ? $"-s {deviceSerial} install -r \"{apkPath}\"" : $"-s {deviceSerial} install \"{apkPath}\"";
		var result = await RunAdbAsync(arguments, cancellationToken);
		return result?.Contains("Success") == true;
	}

	/**
	 * Uninstalls a package from a device.
	 * @param deviceSerial The serial number of the target device
	 * @param packageName The package name to uninstall
	 * @param cancellationToken The cancellation token
	 * @return True, if the uninstallation succeeded
	 */
	public async Task<bool> UninstallPackageAsync(string deviceSerial, string packageName, CancellationToken cancellationToken = default)
	{
		var result = await RunAdbAsync($"-s {deviceSerial} uninstall {packageName}", cancellationToken);
		return result?.Contains("Success") == true;
	}

	/**
	 * Launches an activity on a device.
	 * @param deviceSerial The serial number of the target device
	 * @param packageName The package name containing the activity
	 * @param activityName The name of the activity to launch
	 * @param cancellationToken The cancellation token
	 * @return True, if the activity was launched successfully
	 */
	public async Task<bool> LaunchActivityAsync(string deviceSerial, string packageName, string activityName, CancellationToken cancellationToken = default)
	{
		var activity = activityName.StartsWith(".") ? $"{packageName}{activityName}" : activityName;
		var result = await ShellAsync(deviceSerial, $"am start -n {packageName}/{activity}", cancellationToken);
		return result?.Contains("Error") != true;
	}

	/**
	 * Runs a shell command on a device.
	 * @param deviceSerial The serial number of the target device
	 * @param command The shell command to execute
	 * @param cancellationToken The cancellation token
	 * @return The command output
	 */
	public async Task<string> ShellAsync(string deviceSerial, string command, CancellationToken cancellationToken = default)
	{
		return await RunAdbAsync($"-s {deviceSerial} shell {command}", cancellationToken) ?? string.Empty;
	}

	/**
	 * Pulls a file from a device.
	 * @param deviceSerial The serial number of the target device
	 * @param remotePath The path on the device
	 * @param localPath The local destination path
	 * @param cancellationToken The cancellation token
	 * @return True, if the file was pulled successfully
	 */
	public async Task<bool> PullFileAsync(string deviceSerial, string remotePath, string localPath, CancellationToken cancellationToken = default)
	{
		var result = await RunAdbAsync($"-s {deviceSerial} pull \"{remotePath}\" \"{localPath}\"", cancellationToken);
		return result?.Contains("error") != true;
	}

	/**
	 * Pushes a file to a device.
	 * @param deviceSerial The serial number of the target device
	 * @param localPath The local source path
	 * @param remotePath The destination path on the device
	 * @param cancellationToken The cancellation token
	 * @return True, if the file was pushed successfully
	 */
	public async Task<bool> PushFileAsync(string deviceSerial, string localPath, string remotePath, CancellationToken cancellationToken = default)
	{
		var result = await RunAdbAsync($"-s {deviceSerial} push \"{localPath}\" \"{remotePath}\"", cancellationToken);
		return result?.Contains("error") != true;
	}

	/**
	 * Starts logcat streaming from a device.
	 * @param deviceSerial The serial number of the target device
	 * @param onOutput Callback for each log line
	 * @param cancellationToken The cancellation token
	 * @return The logcat process, nullptr on error
	 */
	public Task<Process?> StartLogcatAsync(string deviceSerial, Action<string> onOutput, CancellationToken cancellationToken = default)
	{
		var adbPath = GetAdbPath();
		if (adbPath == null)
		{
			return Task.FromResult<Process?>(null);
		}

		var process = new Process
		{
			StartInfo = new ProcessStartInfo
			{
				FileName = adbPath,
				Arguments = $"-s {deviceSerial} logcat -v threadtime",
				UseShellExecute = false,
				RedirectStandardOutput = true,
				RedirectStandardError = true,
				CreateNoWindow = true
			},
			EnableRaisingEvents = true
		};

		process.OutputDataReceived += (sender, eventArgs) =>
		{
			if (eventArgs.Data != null)
			{
				onOutput(eventArgs.Data);
			}
		};

		process.Start();
		process.BeginOutputReadLine();

		cancellationToken.Register(() =>
		{
			try
			{
				process.Kill();
			}
			catch
			{
			}
		});

		return Task.FromResult<Process?>(process);
	}

	/**
	 * Forwards a TCP port from a device to local host.
	 * @param deviceSerial The serial number of the target device
	 * @param localPort The local port number
	 * @param remotePort The remote port number on the device
	 * @param cancellationToken The cancellation token
	 * @return True, if port forwarding was established
	 */
	public async Task<bool> ForwardPortAsync(string deviceSerial, int localPort, int remotePort, CancellationToken cancellationToken = default)
	{
		var result = await RunAdbAsync($"-s {deviceSerial} forward tcp:{localPort} tcp:{remotePort}", cancellationToken);
		return result != null;
	}

	/**
	 * Returns package information from an APK file.
	 * @param apkPath The path to the APK file
	 * @param cancellationToken The cancellation token
	 * @return A tuple containing the package name and main activity, nullptr on error
	 */
	public async Task<(string PackageName, string ActivityName)?> GetApkInfoAsync(string apkPath, CancellationToken cancellationToken = default)
	{
		var buildTools = sdkLocator_.GetBuildToolsPath();
		if (buildTools == null)
		{
			return null;
		}

		var aapt2Path = Path.Combine(buildTools, "aapt2.exe");
		if (!File.Exists(aapt2Path))
		{
			return null;
		}

		var result = await RunProcessAsync(aapt2Path, $"dump badging \"{apkPath}\"", cancellationToken);
		if (result == null)
		{
			return null;
		}

		string? packageName = null;
		string? activityName = null;

		foreach (var line in result.Split('\n'))
		{
			if (line.StartsWith("package:"))
			{
				var match = System.Text.RegularExpressions.Regex.Match(line, @"name='([^']+)'");
				if (match.Success)
				{
					packageName = match.Groups[1].Value;
				}
			}
			else if (line.StartsWith("launchable-activity:"))
			{
				var match = System.Text.RegularExpressions.Regex.Match(line, @"name='([^']+)'");
				if (match.Success)
				{
					activityName = match.Groups[1].Value;
				}
			}
		}

		if (packageName != null && activityName != null)
		{
			return (packageName, activityName);
		}

		return null;
	}

	/**
	 * Returns the process ID of a running application.
	 * @param deviceSerial The serial number of the target device
	 * @param packageName The package name of the application
	 * @param cancellationToken The cancellation token
	 * @return The process ID, nullptr if not found
	 */
	public async Task<int?> GetProcessIdAsync(string deviceSerial, string packageName, CancellationToken cancellationToken = default)
	{
		var result = await ShellAsync(deviceSerial, $"pidof {packageName}", cancellationToken);
		if (int.TryParse(result?.Trim(), out var processId))
		{
			return processId;
		}

		return null;
	}

	/**
	 * Forwards a JDWP port for debugging.
	 * @param deviceSerial The serial number of the target device
	 * @param processId The process ID of the application
	 * @param localPort The local port number for JDWP
	 * @param cancellationToken The cancellation token
	 * @return The local port number, nullptr on error
	 */
	public async Task<int?> JdwpForwardAsync(string deviceSerial, int processId, int localPort, CancellationToken cancellationToken = default)
	{
		var result = await RunAdbAsync($"-s {deviceSerial} forward tcp:{localPort} jdwp:{processId}", cancellationToken);
		if (result != null)
		{
			return localPort;
		}

		return null;
	}

	/**
	 * Runs an ADB command and returns the output.
	 * @param arguments The ADB command arguments
	 * @param cancellationToken The cancellation token
	 * @return The command output, nullptr on error
	 */
	private async Task<string?> RunAdbAsync(string arguments, CancellationToken cancellationToken)
	{
		var adbPath = GetAdbPath();
		if (adbPath == null)
		{
			outputService_.WriteLine("Error: ADB not found. Please configure Android SDK path.");
			return null;
		}

		return await RunProcessAsync(adbPath, arguments, cancellationToken);
	}

	/**
	 * Runs a process and returns its output.
	 * @param fileName The path to the executable
	 * @param arguments The command line arguments
	 * @param cancellationToken The cancellation token
	 * @return The process output, nullptr on error
	 */
	private async Task<string?> RunProcessAsync(string fileName, string arguments, CancellationToken cancellationToken)
	{
		try
		{
			var process = new Process
			{
				StartInfo = new ProcessStartInfo
				{
					FileName = fileName,
					Arguments = arguments,
					UseShellExecute = false,
					RedirectStandardOutput = true,
					RedirectStandardError = true,
					CreateNoWindow = true
				}
			};

			var output = new StringBuilder();
			var error = new StringBuilder();

			process.OutputDataReceived += (sender, eventArgs) =>
			{
				if (eventArgs.Data != null)
				{
					output.AppendLine(eventArgs.Data);
				}
			};

			process.ErrorDataReceived += (sender, eventArgs) =>
			{
				if (eventArgs.Data != null)
				{
					error.AppendLine(eventArgs.Data);
				}
			};

			process.Start();
			process.BeginOutputReadLine();
			process.BeginErrorReadLine();

			await Task.Run(() => process.WaitForExit(), cancellationToken);

			if (error.Length > 0)
			{
				outputService_.WriteLine($"ADB Error: {error}");
			}

			return output.ToString();
		}
		catch (Exception exception)
		{
			outputService_.WriteLine($"Error running {Path.GetFileName(fileName)}: {exception.Message}");
			return null;
		}
	}
}

}
