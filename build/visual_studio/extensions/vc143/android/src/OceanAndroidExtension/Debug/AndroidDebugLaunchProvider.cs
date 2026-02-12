/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.ProjectSystem;
using Microsoft.VisualStudio.ProjectSystem.Debug;
using Microsoft.VisualStudio.ProjectSystem.VS.Debug;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;

namespace OceanAndroidExtension.Debug
{

/**
 * This class provides debug launch functionality for Android applications.
 * The class implements IDebugLaunchProvider to handle F5/Ctrl+F5 in Visual Studio.
 * @ingroup oceanandroidextension
 */
[Export(ExportContractNames.Scopes.ConfiguredProject, typeof(IDebugLaunchProvider))]
[AppliesTo("AndroidApplication")]
internal class AndroidDebugLaunchProvider : DebugLaunchProviderBase
{
	[ImportingConstructor]
	public AndroidDebugLaunchProvider(ConfiguredProject configuredProject)
		: base(configuredProject)
	{
	}

	/**
	 * Returns whether this provider can launch for the given launch options.
	 * @param launchOptions The launch options to check
	 * @return True, always; this provider can handle both debug and non-debug launches
	 */
	public override Task<bool> CanLaunchAsync(DebugLaunchOptions launchOptions)
	{
		return Task.FromResult(true);
	}

	/**
	 * Performs the debug launch for Android applications.
	 * This function is called when the user presses F5 or Ctrl+F5.
	 * @param launchOptions The launch options specifying debug or non-debug launch
	 * @return The list of debug launch settings
	 */
	public override async Task<IReadOnlyList<IDebugLaunchSettings>> QueryDebugTargetsAsync(DebugLaunchOptions launchOptions)
	{
		var settings = new List<IDebugLaunchSettings>();

		await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();

		var projectPath = ConfiguredProject.UnconfiguredProject.FullPath;
		var projectDirectory = Path.GetDirectoryName(projectPath);

		if (projectDirectory == null)
		{
			throw new InvalidOperationException("Could not determine project directory.");
		}

		var package = await GetPackageAsync();
		if (package == null)
		{
			throw new InvalidOperationException("Ocean Android Tools package not available.");
		}

		var apkPath = await FindApkAsync(projectDirectory);
		if (string.IsNullOrEmpty(apkPath))
		{
			package.OutputService.WriteLine("APK not found. Building project first...");
			throw new InvalidOperationException("APK not found. Please build the project first.");
		}

		var devices = await package.AdbService.GetDevicesAsync(CancellationToken.None);
		if (devices.Count == 0)
		{
			package.OutputService.WriteLine("No Android devices connected.");
			throw new InvalidOperationException("No Android devices connected. Please connect a device or start an emulator.");
		}

		var device = devices[0];
		package.OutputService.WriteLine($"Deploying to {device.Model}...");

		var deploymentSucceeded = await package.AdbService.InstallApkAsync(device.Serial, apkPath!, reinstall: true, CancellationToken.None);
		if (!deploymentSucceeded)
		{
			package.OutputService.WriteLine("Deployment failed.");
			throw new InvalidOperationException("Failed to deploy APK to device.");
		}

		var apkInfo = await package.AdbService.GetApkInfoAsync(apkPath!, CancellationToken.None);
		if (!apkInfo.HasValue)
		{
			package.OutputService.WriteLine("Could not determine package name.");
			throw new InvalidOperationException("Could not determine package name from APK.");
		}

		bool isDebugLaunch = (launchOptions & DebugLaunchOptions.NoDebug) == 0;

		if (isDebugLaunch)
		{
			package.OutputService.WriteLine($"Launching {apkInfo.Value.PackageName} in debug mode...");
			await package.AdbService.ShellAsync(device.Serial, $"am set-debug-app -w {apkInfo.Value.PackageName}", CancellationToken.None);
		}
		else
		{
			package.OutputService.WriteLine($"Launching {apkInfo.Value.PackageName}...");
		}

		await package.AdbService.LaunchActivityAsync(device.Serial, apkInfo.Value.PackageName, apkInfo.Value.ActivityName, CancellationToken.None);

		package.OutputService.WriteLine("Application launched successfully.");

		return settings;
	}

	/**
	 * Returns the Ocean Android Extension package.
	 * @return The package instance, nullptr if not available
	 */
	private async Task<OceanAndroidExtensionPackage?> GetPackageAsync()
	{
		await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
		return OceanAndroidExtensionPackage.GetGlobalService(typeof(OceanAndroidExtensionPackage)) as OceanAndroidExtensionPackage;
	}

	/**
	 * Finds the APK file for the project.
	 * @param projectDirectory The project directory to search in
	 * @return The path to the APK file, nullptr if not found
	 */
	private async Task<string?> FindApkAsync(string projectDirectory)
	{
		var properties = ConfiguredProject.Services.ProjectPropertiesProvider?.GetCommonProperties();
		var buildType = "debug";

		if (properties != null)
		{
			buildType = await properties.GetEvaluatedPropertyValueAsync("AndroidBuildType") ?? "debug";
		}

		var searchPaths = new[]
		{
			Path.Combine(projectDirectory, "app", "build", "outputs", "apk", buildType),
			Path.Combine(projectDirectory, "build", "outputs", "apk", buildType),
			Path.Combine(projectDirectory, "app", "build", "outputs", "apk", "debug"),
			Path.Combine(projectDirectory, "build", "outputs", "apk", "debug")
		};

		foreach (var path in searchPaths)
		{
			if (Directory.Exists(path))
			{
				var apkFiles = Directory.GetFiles(path, "*.apk");
				if (apkFiles.Length > 0)
				{
					return apkFiles[0];
				}
			}
		}

		return null;
	}
}

}
