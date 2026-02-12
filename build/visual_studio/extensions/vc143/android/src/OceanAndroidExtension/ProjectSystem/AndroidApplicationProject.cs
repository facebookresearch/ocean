/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.ComponentModel.Composition;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.ProjectSystem;
using Microsoft.VisualStudio.ProjectSystem.Properties;

namespace OceanAndroidExtension.ProjectSystem
{

/**
 * This class defines project capabilities for Android Application projects.
 * @ingroup oceanandroidextension
 */
[Export]
[AppliesTo(AndroidApplicationCapabilities)]
internal class AndroidApplicationProjectCapabilities
{
	/// The capability string for Android Application projects.
	public const string AndroidApplicationCapabilities = "AndroidApplication";
}

/**
 * This class handles deployment of Android applications.
 * @ingroup oceanandroidextension
 */
[Export]
[AppliesTo("AndroidApplication")]
internal class AndroidDeployHandler
{
	/// The unconfigured project.
	[Import]
	private UnconfiguredProject UnconfiguredProject { get; set; } = null!;

	/// True, if deployment is supported.
	public bool IsDeploySupported => true;

	/**
	 * Deploys the Android application.
	 * @param cancellationToken The cancellation token
	 * @param outputPaneWriter The output pane writer for logging
	 */
	public async Task DeployAsync(CancellationToken cancellationToken, TextWriter outputPaneWriter)
	{
		await outputPaneWriter.WriteLineAsync("Deploying Android application...");

		var projectDirectory = Path.GetDirectoryName(UnconfiguredProject.FullPath);
		if (projectDirectory == null)
		{
			await outputPaneWriter.WriteLineAsync("Error: Could not determine project directory.");
			return;
		}

		var apkPath = FindApk(projectDirectory);
		if (apkPath == null)
		{
			await outputPaneWriter.WriteLineAsync("Error: APK not found. Please build the project first.");
			return;
		}

		await outputPaneWriter.WriteLineAsync($"Found APK: {apkPath}");

		var package = OceanAndroidExtensionPackage.GetGlobalService(typeof(OceanAndroidExtensionPackage)) as OceanAndroidExtensionPackage;
		if (package == null)
		{
			await outputPaneWriter.WriteLineAsync("Error: Ocean Android Tools package not available.");
			return;
		}

		var devices = await package.AdbService.GetDevicesAsync(cancellationToken);
		if (devices.Count == 0)
		{
			await outputPaneWriter.WriteLineAsync("Error: No devices connected. Please connect a device or start an emulator.");
			return;
		}

		var device = devices[0];
		await outputPaneWriter.WriteLineAsync($"Deploying to {device.Model} ({device.Serial})...");

		var success = await package.AdbService.InstallApkAsync(device.Serial, apkPath, reinstall: true, cancellationToken);
		if (success)
		{
			await outputPaneWriter.WriteLineAsync("Deployment successful.");

			var info = await package.AdbService.GetApkInfoAsync(apkPath, cancellationToken);
			if (info.HasValue)
			{
				await outputPaneWriter.WriteLineAsync($"Launching {info.Value.PackageName}...");
				await package.AdbService.LaunchActivityAsync(device.Serial, info.Value.PackageName, info.Value.ActivityName, cancellationToken);
			}
		}
		else
		{
			await outputPaneWriter.WriteLineAsync("Error: Deployment failed.");
		}
	}

	/**
	 * Finds the APK file for the project.
	 * @param projectDirectory The project directory
	 * @return The path to the APK file, nullptr if not found
	 */
	private string? FindApk(string projectDirectory)
	{
		var searchPaths = new[]
		{
			Path.Combine(projectDirectory, "app", "build", "outputs", "apk", "debug"),
			Path.Combine(projectDirectory, "app", "build", "outputs", "apk", "release"),
			Path.Combine(projectDirectory, "build", "outputs", "apk", "debug"),
			Path.Combine(projectDirectory, "build", "outputs", "apk", "release")
		};

		foreach (var searchPath in searchPaths)
		{
			if (Directory.Exists(searchPath))
			{
				var apkFiles = Directory.GetFiles(searchPath, "*.apk");
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
