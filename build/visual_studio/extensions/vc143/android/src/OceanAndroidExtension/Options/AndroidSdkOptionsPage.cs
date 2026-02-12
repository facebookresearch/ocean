/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Shell;
using OceanAndroidExtension.Services;

namespace OceanAndroidExtension.Options
{

/**
 * This class implements the options page for Android SDK configuration in Visual Studio.
 * @ingroup oceanandroidextension
 */
[Guid("D4E5F6A7-B8C9-0123-4567-89ABCDEF0123")]
public class AndroidSdkOptionsPage : DialogPage
{
	/// The Android SDK path.
	private string androidSdkPath_ = string.Empty;

	/// The Android NDK path.
	private string androidNdkPath_ = string.Empty;

	/// The JDK path.
	private string jdkPath_ = string.Empty;

	/// True, to auto-detect SDK paths.
	private bool autoDetect_ = true;

	/// True, to auto-detect SDK paths from environment variables and common locations.
	[Category("SDK Paths")]
	[DisplayName("Auto-detect SDK paths")]
	[Description("Automatically detect Android SDK, NDK, and JDK paths from environment variables and common locations.")]
	public bool AutoDetect
	{
		get => autoDetect_;
		set
		{
			autoDetect_ = value;
			if (value)
			{
				DetectPaths();
			}
		}
	}

	/// The path to the Android SDK installation directory.
	[Category("SDK Paths")]
	[DisplayName("Android SDK Path")]
	[Description("Path to the Android SDK installation directory.")]
	public string AndroidSdkPath
	{
		get => androidSdkPath_;
		set => androidSdkPath_ = value ?? string.Empty;
	}

	/// The path to the Android NDK installation directory.
	[Category("SDK Paths")]
	[DisplayName("Android NDK Path")]
	[Description("Path to the Android NDK installation directory.")]
	public string AndroidNdkPath
	{
		get => androidNdkPath_;
		set => androidNdkPath_ = value ?? string.Empty;
	}

	/// The path to the Java Development Kit installation directory.
	[Category("SDK Paths")]
	[DisplayName("JDK Path")]
	[Description("Path to the Java Development Kit installation directory.")]
	public string JdkPath
	{
		get => jdkPath_;
		set => jdkPath_ = value ?? string.Empty;
	}

	/// The default minimum Android SDK version for new projects.
	[Category("Build")]
	[DisplayName("Default Min SDK Version")]
	[Description("Default minimum Android SDK version for new projects.")]
	[DefaultValue(24)]
	public int DefaultMinSdkVersion { get; set; } = 24;

	/// The default target Android SDK version for new projects.
	[Category("Build")]
	[DisplayName("Default Target SDK Version")]
	[Description("Default target Android SDK version for new projects.")]
	[DefaultValue(34)]
	public int DefaultTargetSdkVersion { get; set; } = 34;

	/// The default NDK version for native projects (empty for latest).
	[Category("Build")]
	[DisplayName("Default NDK Version")]
	[Description("Default NDK version for native projects. Leave empty for latest.")]
	public string DefaultNdkVersion { get; set; } = string.Empty;

	/// True, to enable native C++ debugging for Android applications.
	[Category("Debugging")]
	[DisplayName("Enable Native Debugging")]
	[Description("Enable native C++ debugging for Android applications.")]
	[DefaultValue(true)]
	public bool EnableNativeDebugging { get; set; } = true;

	/// Custom path to LLDB debugger (empty to use NDK's bundled LLDB).
	[Category("Debugging")]
	[DisplayName("LLDB Path")]
	[Description("Custom path to LLDB debugger. Leave empty to use NDK's bundled LLDB.")]
	public string LldbPath { get; set; } = string.Empty;

	/// Additional command-line arguments for the Android Emulator.
	[Category("Emulator")]
	[DisplayName("Emulator Arguments")]
	[Description("Additional command-line arguments for the Android Emulator.")]
	public string EmulatorArguments { get; set; } = string.Empty;

	/// True, to enable hardware acceleration for emulators.
	[Category("Emulator")]
	[DisplayName("Use Hardware Acceleration")]
	[Description("Enable hardware acceleration for emulators (requires Hyper-V or HAXM).")]
	[DefaultValue(true)]
	public bool UseHardwareAcceleration { get; set; } = true;

	/**
	 * Called when the options page is activated.
	 * @param eventArgs The cancel event arguments
	 */
	protected override void OnActivate(CancelEventArgs eventArgs)
	{
		base.OnActivate(eventArgs);

		if (autoDetect_ && string.IsNullOrEmpty(androidSdkPath_))
		{
			DetectPaths();
		}
	}

	/**
	 * Detects SDK paths from environment variables and common locations.
	 */
	private void DetectPaths()
	{
		try
		{
			var locator = new SdkLocatorService(null!);

			if (string.IsNullOrEmpty(androidSdkPath_))
			{
				androidSdkPath_ = locator.FindAndroidSdk() ?? string.Empty;
			}

			if (string.IsNullOrEmpty(androidNdkPath_))
			{
				androidNdkPath_ = locator.FindNdk() ?? string.Empty;
			}

			if (string.IsNullOrEmpty(jdkPath_))
			{
				jdkPath_ = locator.FindJdk() ?? string.Empty;
			}
		}
		catch
		{
			// Ignore detection errors
		}
	}

	/**
	 * Saves the settings to storage and updates environment variables.
	 */
	public override void SaveSettingsToStorage()
	{
		base.SaveSettingsToStorage();

		if (!string.IsNullOrEmpty(androidSdkPath_))
		{
			Environment.SetEnvironmentVariable("ANDROID_HOME", androidSdkPath_);
		}

		if (!string.IsNullOrEmpty(androidNdkPath_))
		{
			Environment.SetEnvironmentVariable("ANDROID_NDK_HOME", androidNdkPath_);
		}

		if (!string.IsNullOrEmpty(jdkPath_))
		{
			Environment.SetEnvironmentVariable("JAVA_HOME", jdkPath_);
		}
	}
}

}
