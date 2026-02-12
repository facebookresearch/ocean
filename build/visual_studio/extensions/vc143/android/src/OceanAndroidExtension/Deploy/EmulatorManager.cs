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
using System.Threading;
using System.Threading.Tasks;
using OceanAndroidExtension.Services;

namespace OceanAndroidExtension.Deploy
{

/**
 * This interface defines the Android emulator management functionality.
 * @ingroup oceanandroidextension
 */
public interface IEmulatorManager
{
	/**
	 * Returns the available AVDs (Android Virtual Devices).
	 * @param cancellationToken The cancellation token
	 * @return The list of available AVDs
	 */
	Task<IReadOnlyList<AvdInfo>> GetAvailableAvdsAsync(CancellationToken cancellationToken = default);

	/**
	 * Starts an emulator with the specified AVD.
	 * @param avdName The name of the AVD to start
	 * @param cancellationToken The cancellation token
	 * @return True, if the emulator started successfully
	 */
	Task<bool> StartEmulatorAsync(string avdName, CancellationToken cancellationToken = default);

	/**
	 * Stops a running emulator.
	 * @param emulatorSerial The serial number of the emulator to stop
	 * @param cancellationToken The cancellation token
	 * @return True, if the emulator was stopped successfully
	 */
	Task<bool> StopEmulatorAsync(string emulatorSerial, CancellationToken cancellationToken = default);

	/**
	 * Creates a new AVD.
	 * @param name The name for the new AVD
	 * @param systemImage The system image to use
	 * @param device The device definition to use
	 * @param cancellationToken The cancellation token
	 * @return True, if the AVD was created successfully
	 */
	Task<bool> CreateAvdAsync(string name, string systemImage, string device, CancellationToken cancellationToken = default);
}

/**
 * This class contains information about an Android Virtual Device.
 * @ingroup oceanandroidextension
 */
public class AvdInfo
{
	/// The name of the AVD.
	public string Name { get; set; } = string.Empty;

	/// The device definition name.
	public string Device { get; set; } = string.Empty;

	/// The path to the AVD configuration.
	public string Path { get; set; } = string.Empty;

	/// The target system image.
	public string Target { get; set; } = string.Empty;

	/// The ABI of the AVD (e.g., x86_64, arm64-v8a).
	public string Abi { get; set; } = string.Empty;

	/// The API level of the AVD.
	public int ApiLevel { get; set; }

	/// True, if the emulator is currently running.
	public bool IsRunning { get; set; }
}

/**
 * This class implements the Android emulator management functionality.
 * @ingroup oceanandroidextension
 */
public class EmulatorManager : IEmulatorManager
{
	/// The SDK locator service for finding SDK paths.
	private readonly ISdkLocatorService sdkLocator_;

	/// The output window service for logging.
	private readonly IOutputWindowService outputService_;

	/// The ADB service for device communication.
	private readonly IAdbService adbService_;

	/**
	 * Creates a new emulator manager.
	 * @param sdkLocator The SDK locator service for finding SDK paths
	 * @param outputService The output window service for logging
	 * @param adbService The ADB service for device communication
	 */
	public EmulatorManager(ISdkLocatorService sdkLocator, IOutputWindowService outputService, IAdbService adbService)
	{
		sdkLocator_ = sdkLocator;
		outputService_ = outputService;
		adbService_ = adbService;
	}

	/**
	 * Returns the available AVDs (Android Virtual Devices).
	 * @param cancellationToken The cancellation token
	 * @return The list of available AVDs
	 */
	public async Task<IReadOnlyList<AvdInfo>> GetAvailableAvdsAsync(CancellationToken cancellationToken = default)
	{
		var emulatorPath = GetEmulatorPath();
		if (emulatorPath == null)
		{
			return Array.Empty<AvdInfo>();
		}

		var output = await RunProcessAsync(emulatorPath, "-list-avds", cancellationToken);
		if (string.IsNullOrEmpty(output))
		{
			return Array.Empty<AvdInfo>();
		}

		var availableAvds = new List<AvdInfo>();
		var runningEmulators = await adbService_.GetDevicesAsync(cancellationToken);
		var runningSerials = runningEmulators.Where(device => device.IsEmulator).Select(device => device.Serial).ToHashSet();

		foreach (var line in output!.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries))
		{
			var avdName = line.Trim();
			if (string.IsNullOrEmpty(avdName))
			{
				continue;
			}

			var avdInfo = await GetAvdInfoAsync(avdName, cancellationToken);
			if (avdInfo != null)
			{
				avdInfo.IsRunning = runningSerials.Any(serial => serial.Contains(avdName));
				availableAvds.Add(avdInfo);
			}
		}

		return availableAvds;
	}

	/**
	 * Returns detailed information about an AVD.
	 * @param avdName The name of the AVD
	 * @param cancellationToken The cancellation token
	 * @return The AVD information, nullptr if not found
	 */
	private Task<AvdInfo?> GetAvdInfoAsync(string avdName, CancellationToken cancellationToken)
	{
		var avdPath = GetAvdPath(avdName);
		if (avdPath == null || !File.Exists(avdPath))
		{
			return Task.FromResult<AvdInfo?>(new AvdInfo { Name = avdName });
		}

		var avdInfo = new AvdInfo { Name = avdName, Path = avdPath };

		try
		{
			var iniContent = File.ReadAllText(avdPath);
			foreach (var line in iniContent.Split('\n'))
			{
				var parts = line.Split('=');
				if (parts.Length != 2)
				{
					continue;
				}

				var key = parts[0].Trim();
				var value = parts[1].Trim();

				switch (key)
				{
					case "hw.device.name":
						avdInfo.Device = value;
						break;
					case "image.sysdir.1":
						avdInfo.Target = value;
						break;
					case "abi.type":
						avdInfo.Abi = value;
						break;
				}
			}

			if (!string.IsNullOrEmpty(avdInfo.Target) && avdInfo.Target.Contains("android-"))
			{
				var match = System.Text.RegularExpressions.Regex.Match(avdInfo.Target, @"android-(\d+)");
				if (match.Success && int.TryParse(match.Groups[1].Value, out var apiLevel))
				{
					avdInfo.ApiLevel = apiLevel;
				}
			}
		}
		catch
		{
			// Ignore parsing errors
		}

		return Task.FromResult<AvdInfo?>(avdInfo);
	}

	/**
	 * Starts an emulator with the specified AVD.
	 * @param avdName The name of the AVD to start
	 * @param cancellationToken The cancellation token
	 * @return True, if the emulator started successfully
	 */
	public async Task<bool> StartEmulatorAsync(string avdName, CancellationToken cancellationToken = default)
	{
		var emulatorPath = GetEmulatorPath();
		if (emulatorPath == null)
		{
			outputService_.WriteLine("Error: Emulator not found");
			return false;
		}

		outputService_.WriteLine($"Starting emulator: {avdName}");

		var arguments = $"-avd {avdName}";

		var startInfo = new ProcessStartInfo
		{
			FileName = emulatorPath,
			Arguments = arguments,
			UseShellExecute = false,
			CreateNoWindow = true
		};

		try
		{
			Process.Start(startInfo);

			for (int i = 0; i < 60 && !cancellationToken.IsCancellationRequested; i++)
			{
				await Task.Delay(1000, cancellationToken);

				var devices = await adbService_.GetDevicesAsync(cancellationToken);
				var emulator = devices.FirstOrDefault(device => device.IsEmulator && device.State == DeviceState.Device);

				if (emulator != null)
				{
					outputService_.WriteLine($"Emulator started: {emulator.Serial}");
					return true;
				}
			}

			outputService_.WriteLine("Warning: Emulator started but device not yet available");
			return true;
		}
		catch (Exception exception)
		{
			outputService_.WriteLine($"Error starting emulator: {exception.Message}");
			return false;
		}
	}

	/**
	 * Stops a running emulator.
	 * @param emulatorSerial The serial number of the emulator to stop
	 * @param cancellationToken The cancellation token
	 * @return True, if the emulator was stopped successfully
	 */
	public async Task<bool> StopEmulatorAsync(string emulatorSerial, CancellationToken cancellationToken = default)
	{
		outputService_.WriteLine($"Stopping emulator: {emulatorSerial}");

		var result = await adbService_.ShellAsync(emulatorSerial, "reboot -p", cancellationToken);
		return true;
	}

	/**
	 * Creates a new AVD.
	 * @param name The name for the new AVD
	 * @param systemImage The system image to use
	 * @param device The device definition to use
	 * @param cancellationToken The cancellation token
	 * @return True, if the AVD was created successfully
	 */
	public async Task<bool> CreateAvdAsync(string name, string systemImage, string device, CancellationToken cancellationToken = default)
	{
		var avdManagerPath = GetAvdManagerPath();
		if (avdManagerPath == null)
		{
			outputService_.WriteLine("Error: avdmanager not found");
			return false;
		}

		var arguments = $"create avd -n {name} -k \"{systemImage}\" -d \"{device}\" --force";

		outputService_.WriteLine($"Creating AVD: {name}");

		var output = await RunProcessAsync(avdManagerPath, arguments, cancellationToken);
		return output?.Contains("Error") != true;
	}

	/**
	 * Returns the path to the emulator executable.
	 * @return The path to emulator, nullptr if not found
	 */
	private string? GetEmulatorPath()
	{
		var sdkPath = sdkLocator_.FindAndroidSdk();
		if (sdkPath == null)
		{
			return null;
		}

		var emulatorPath = Path.Combine(sdkPath, "emulator", "emulator.exe");
		if (!File.Exists(emulatorPath))
		{
			emulatorPath = Path.Combine(sdkPath, "emulator", "emulator");
		}

		return File.Exists(emulatorPath) ? emulatorPath : null;
	}

	/**
	 * Returns the path to the AVD manager executable.
	 * @return The path to avdmanager, nullptr if not found
	 */
	private string? GetAvdManagerPath()
	{
		var sdkPath = sdkLocator_.FindAndroidSdk();
		if (sdkPath == null)
		{
			return null;
		}

		var avdManagerPath = Path.Combine(sdkPath, "cmdline-tools", "latest", "bin", "avdmanager.bat");
		if (!File.Exists(avdManagerPath))
		{
			avdManagerPath = Path.Combine(sdkPath, "tools", "bin", "avdmanager.bat");
		}

		return File.Exists(avdManagerPath) ? avdManagerPath : null;
	}

	/**
	 * Returns the path to an AVD configuration file.
	 * @param avdName The name of the AVD
	 * @return The path to the AVD configuration, nullptr if not found
	 */
	private string? GetAvdPath(string avdName)
	{
		var avdHome = Environment.GetEnvironmentVariable("ANDROID_AVD_HOME");
		if (string.IsNullOrEmpty(avdHome))
		{
			avdHome = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), ".android", "avd");
		}

		var avdIni = Path.Combine(avdHome, $"{avdName}.ini");
		var avdDirectory = Path.Combine(avdHome, $"{avdName}.avd");

		if (Directory.Exists(avdDirectory))
		{
			return Path.Combine(avdDirectory, "config.ini");
		}

		return File.Exists(avdIni) ? avdIni : null;
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

			var output = await process.StandardOutput.ReadToEndAsync();
			await Task.Run(() => process.WaitForExit(), cancellationToken);

			return output;
		}
		catch
		{
			return null;
		}
	}
}

}
