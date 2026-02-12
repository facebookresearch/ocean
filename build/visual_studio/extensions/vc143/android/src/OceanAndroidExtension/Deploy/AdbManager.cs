/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using OceanAndroidExtension.Services;

namespace OceanAndroidExtension.Deploy
{

/**
 * This interface defines the ADB device management and APK deployment functionality.
 * @ingroup oceanandroidextension
 */
public interface IAdbManager
{
	/**
	 * Returns the currently selected device for deployment.
	 * @return The selected device, nullptr if no device is selected
	 */
	AndroidDevice? SelectedDevice { get; }

	/**
	 * Sets the target device for deployment.
	 * @param serial The serial number of the device to select
	 * @param cancellationToken The cancellation token
	 * @return True, if the device was found and selected
	 */
	Task<bool> SelectDeviceAsync(string serial, CancellationToken cancellationToken = default);

	/**
	 * Deploys an APK to the selected device.
	 * @param apkPath The path to the APK file
	 * @param options The deployment options
	 * @param cancellationToken The cancellation token
	 * @return The deployment result
	 */
	Task<DeployResult> DeployApkAsync(string apkPath, DeployOptions options, CancellationToken cancellationToken = default);

	/**
	 * Launches an installed application.
	 * @param packageName The package name of the application to launch
	 * @param cancellationToken The cancellation token
	 * @return True, if the application was launched successfully
	 */
	Task<bool> LaunchAppAsync(string packageName, CancellationToken cancellationToken = default);

	/**
	 * Returns the installed packages on the device.
	 * @param cancellationToken The cancellation token
	 * @return The list of installed package names
	 */
	Task<IReadOnlyList<string>> GetInstalledPackagesAsync(CancellationToken cancellationToken = default);

	/**
	 * Clears application data for a package.
	 * @param packageName The package name to clear data for
	 * @param cancellationToken The cancellation token
	 * @return True, if the data was cleared successfully
	 */
	Task<bool> ClearAppDataAsync(string packageName, CancellationToken cancellationToken = default);
}

/**
 * This class defines deployment options for APK installation.
 * @ingroup oceanandroidextension
 */
public class DeployOptions
{
	/// True, to reinstall an existing application.
	public bool Reinstall { get; set; } = true;

	/// True, to allow downgrading an existing application.
	public bool AllowDowngrade { get; set; } = false;

	/// True, to grant all requested permissions automatically.
	public bool GrantPermissions { get; set; } = true;

	/// True, to launch the application after deployment.
	public bool LaunchAfterDeploy { get; set; } = true;

	/// True, to wait for debugger attachment before launching.
	public bool WaitForDebugger { get; set; } = false;
}

/**
 * This class contains the result of an APK deployment operation.
 * @ingroup oceanandroidextension
 */
public class DeployResult
{
	/// True, if the deployment succeeded.
	public bool Success { get; set; }

	/// The package name of the deployed application.
	public string? PackageName { get; set; }

	/// The main activity name of the deployed application.
	public string? ActivityName { get; set; }

	/// The error message if deployment failed.
	public string? ErrorMessage { get; set; }

	/// The duration of the deployment operation.
	public TimeSpan Duration { get; set; }
}

/**
 * This class implements the ADB device management and APK deployment functionality.
 * @ingroup oceanandroidextension
 */
public class AdbManager : IAdbManager
{
	/// The ADB service for device communication.
	private readonly IAdbService adbService_;

	/// The SDK locator service for finding tools.
	private readonly ISdkLocatorService sdkLocator_;

	/// The output window service for logging.
	private readonly IOutputWindowService outputService_;

	/// The currently selected device.
	private AndroidDevice? selectedDevice_;

	/**
	 * Creates a new ADB manager.
	 * @param adbService The ADB service for device communication
	 * @param sdkLocator The SDK locator service for finding tools
	 * @param outputService The output window service for logging
	 */
	public AdbManager(IAdbService adbService, ISdkLocatorService sdkLocator, IOutputWindowService outputService)
	{
		adbService_ = adbService;
		sdkLocator_ = sdkLocator;
		outputService_ = outputService;
	}

	/**
	 * Returns the currently selected device.
	 * @return The selected device, nullptr if no device is selected
	 */
	public AndroidDevice? SelectedDevice => selectedDevice_;

	/**
	 * Sets the target device for deployment.
	 * @param serial The serial number of the device to select
	 * @param cancellationToken The cancellation token
	 * @return True, if the device was found and selected
	 */
	public async Task<bool> SelectDeviceAsync(string serial, CancellationToken cancellationToken = default)
	{
		var devices = await adbService_.GetDevicesAsync(cancellationToken);
		selectedDevice_ = devices.FirstOrDefault(device => device.Serial == serial);
		return selectedDevice_ != null;
	}

	/**
	 * Deploys an APK to the selected device.
	 * @param apkPath The path to the APK file
	 * @param options The deployment options
	 * @param cancellationToken The cancellation token
	 * @return The deployment result
	 */
	public async Task<DeployResult> DeployApkAsync(string apkPath, DeployOptions options, CancellationToken cancellationToken = default)
	{
		var result = new DeployResult();
		var startTime = DateTime.UtcNow;

		try
		{
			if (selectedDevice_ == null)
			{
				var devices = await adbService_.GetDevicesAsync(cancellationToken);
				selectedDevice_ = devices.FirstOrDefault(device => device.State == DeviceState.Device);

				if (selectedDevice_ == null)
				{
					result.ErrorMessage = "No devices available";
					return result;
				}

				outputService_.WriteLine($"Auto-selected device: {selectedDevice_.Model} ({selectedDevice_.Serial})");
			}

			if (!File.Exists(apkPath))
			{
				result.ErrorMessage = $"APK not found: {apkPath}";
				return result;
			}

			var apkInfo = await adbService_.GetApkInfoAsync(apkPath, cancellationToken);
			if (apkInfo.HasValue)
			{
				result.PackageName = apkInfo.Value.PackageName;
				result.ActivityName = apkInfo.Value.ActivityName;
			}

			outputService_.WriteLine($"Deploying to {selectedDevice_.Model}...");
			outputService_.WriteLine($"APK: {Path.GetFileName(apkPath)}");

			var installationSucceeded = await adbService_.InstallApkAsync(selectedDevice_.Serial, apkPath, options.Reinstall, cancellationToken);

			if (!installationSucceeded)
			{
				result.ErrorMessage = "Installation failed";
				return result;
			}

			outputService_.WriteLine("Installation successful");

			if (options.LaunchAfterDeploy && result.PackageName != null && result.ActivityName != null)
			{
				await LaunchAppInternalAsync(result.PackageName, result.ActivityName, options.WaitForDebugger, cancellationToken);
			}

			result.Success = true;
		}
		catch (Exception exception)
		{
			result.ErrorMessage = exception.Message;
			outputService_.WriteLine($"Deploy error: {exception.Message}");
		}

		result.Duration = DateTime.UtcNow - startTime;
		return result;
	}

	/**
	 * Launches an installed application.
	 * @param packageName The package name of the application to launch
	 * @param cancellationToken The cancellation token
	 * @return True, if the application was launched successfully
	 */
	public async Task<bool> LaunchAppAsync(string packageName, CancellationToken cancellationToken = default)
	{
		if (selectedDevice_ == null)
		{
			return false;
		}

		var output = await adbService_.ShellAsync(selectedDevice_.Serial, $"cmd package resolve-activity --brief {packageName}", cancellationToken);

		if (string.IsNullOrEmpty(output))
		{
			return false;
		}

		var lines = output.Split('\n');
		var activityLine = lines.FirstOrDefault(line => line.Contains("/"));

		if (activityLine == null)
		{
			return false;
		}

		var parts = activityLine.Trim().Split('/');
		if (parts.Length != 2)
		{
			return false;
		}

		return await adbService_.LaunchActivityAsync(selectedDevice_.Serial, packageName, parts[1], cancellationToken);
	}

	/**
	 * Launches an application with the specified activity.
	 * @param packageName The package name of the application
	 * @param activityName The activity name to launch
	 * @param waitForDebugger True, to wait for debugger attachment
	 * @param cancellationToken The cancellation token
	 * @return True, if the application was launched successfully
	 */
	private async Task<bool> LaunchAppInternalAsync(string packageName, string activityName, bool waitForDebugger, CancellationToken cancellationToken)
	{
		if (selectedDevice_ == null)
		{
			return false;
		}

		var activity = activityName.StartsWith(".") ? $"{packageName}{activityName}" : activityName;

		var command = waitForDebugger ? $"am start -D -n {packageName}/{activity}" : $"am start -n {packageName}/{activity}";

		var output = await adbService_.ShellAsync(selectedDevice_.Serial, command, cancellationToken);

		if (output?.Contains("Error") == true)
		{
			outputService_.WriteLine($"Launch warning: {output}");
			return false;
		}

		outputService_.WriteLine($"Launched: {packageName}");
		return true;
	}

	/**
	 * Returns the installed packages on the device.
	 * @param cancellationToken The cancellation token
	 * @return The list of installed package names
	 */
	public async Task<IReadOnlyList<string>> GetInstalledPackagesAsync(CancellationToken cancellationToken = default)
	{
		if (selectedDevice_ == null)
		{
			return Array.Empty<string>();
		}

		var output = await adbService_.ShellAsync(selectedDevice_.Serial, "pm list packages -3", cancellationToken);

		if (string.IsNullOrEmpty(output))
		{
			return Array.Empty<string>();
		}

		return output
			.Split('\n')
			.Where(line => line.StartsWith("package:"))
			.Select(line => line.Substring("package:".Length).Trim())
			.OrderBy(packageName => packageName)
			.ToList();
	}

	/**
	 * Clears application data for a package.
	 * @param packageName The package name to clear data for
	 * @param cancellationToken The cancellation token
	 * @return True, if the data was cleared successfully
	 */
	public async Task<bool> ClearAppDataAsync(string packageName, CancellationToken cancellationToken = default)
	{
		if (selectedDevice_ == null)
		{
			return false;
		}

		var output = await adbService_.ShellAsync(selectedDevice_.Serial, $"pm clear {packageName}", cancellationToken);

		return output?.Contains("Success") == true;
	}
}

}
