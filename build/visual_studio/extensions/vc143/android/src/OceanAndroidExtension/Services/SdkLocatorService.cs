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
using Microsoft.VisualStudio.Shell;
using Microsoft.Win32;

namespace OceanAndroidExtension.Services
{

/**
 * This interface defines the SDK locator service for finding Android SDK, NDK, and JDK installations.
 * @ingroup oceanandroidextension
 */
public interface ISdkLocatorService
{
	/**
	 * Finds the Android SDK installation path.
	 * @return The path to the Android SDK, nullptr if not found
	 */
	string? FindAndroidSdk();

	/**
	 * Finds the Android NDK installation path.
	 * @return The path to the Android NDK, nullptr if not found
	 */
	string? FindNdk();

	/**
	 * Finds the JDK installation path.
	 * @return The path to the JDK, nullptr if not found
	 */
	string? FindJdk();

	/**
	 * Returns the path to the ADB executable.
	 * @return The path to adb, nullptr if not found
	 */
	string? GetAdbPath();

	/**
	 * Returns the path to a specific build-tools version.
	 * @param version The version of build-tools to find, nullptr for the latest version
	 * @return The path to build-tools, nullptr if not found
	 */
	string? GetBuildToolsPath(string? version = null);

	/**
	 * Returns the available NDK versions installed in the SDK.
	 * @return The list of NDK version strings
	 */
	IEnumerable<string> GetAvailableNdkVersions();

	/**
	 * Returns the available platform versions installed in the SDK.
	 * @return The list of API level integers
	 */
	IEnumerable<int> GetAvailablePlatformVersions();
}

/**
 * This class implements the SDK locator service for finding Android SDK, NDK, and JDK installations.
 * @ingroup oceanandroidextension
 */
public class SdkLocatorService : ISdkLocatorService
{
	/// The Visual Studio package.
	private readonly AsyncPackage package_;

	/// Cached Android SDK path.
	private string? cachedAndroidSdk_;

	/// Cached Android NDK path.
	private string? cachedNdk_;

	/// Cached JDK path.
	private string? cachedJdk_;

	/**
	 * Creates a new SDK locator service.
	 * @param package The Visual Studio package
	 */
	public SdkLocatorService(AsyncPackage package)
	{
		package_ = package;
	}

	/**
	 * Finds the Android SDK installation path.
	 * @return The path to the Android SDK, nullptr if not found
	 */
	public string? FindAndroidSdk()
	{
		if (cachedAndroidSdk_ != null)
		{
			return cachedAndroidSdk_;
		}

		var environmentPaths = new[]
		{
			Environment.GetEnvironmentVariable("ANDROID_HOME"),
			Environment.GetEnvironmentVariable("ANDROID_SDK_ROOT"),
			Environment.GetEnvironmentVariable("ANDROID_SDK")
		};

		foreach (var path in environmentPaths.Where(path => !string.IsNullOrEmpty(path)))
		{
			if (ValidateAndroidSdk(path!))
			{
				cachedAndroidSdk_ = path;
				return path;
			}
		}

		var defaultPaths = GetDefaultAndroidSdkPaths();
		foreach (var path in defaultPaths)
		{
			if (ValidateAndroidSdk(path))
			{
				cachedAndroidSdk_ = path;
				return path;
			}
		}

		var studioSdk = FindAndroidStudioSdk();
		if (studioSdk != null && ValidateAndroidSdk(studioSdk))
		{
			cachedAndroidSdk_ = studioSdk;
			return studioSdk;
		}

		return null;
	}

	/**
	 * Finds the Android NDK installation path.
	 * @return The path to the Android NDK, nullptr if not found
	 */
	public string? FindNdk()
	{
		if (cachedNdk_ != null)
		{
			return cachedNdk_;
		}

		var oceanNdk = Environment.GetEnvironmentVariable("OCEAN_ANDROID_NDK_HOME");
		if (!string.IsNullOrEmpty(oceanNdk) && ValidateNdk(oceanNdk))
		{
			cachedNdk_ = oceanNdk;
			return oceanNdk;
		}

		var environmentNdk = Environment.GetEnvironmentVariable("ANDROID_NDK_HOME") ?? Environment.GetEnvironmentVariable("ANDROID_NDK_ROOT") ?? Environment.GetEnvironmentVariable("NDK_HOME");

		if (!string.IsNullOrEmpty(environmentNdk) && ValidateNdk(environmentNdk))
		{
			cachedNdk_ = environmentNdk;
			return environmentNdk;
		}

		var sdk = FindAndroidSdk();
		if (sdk != null)
		{
			var ndkDirectory = Path.Combine(sdk, "ndk");
			if (Directory.Exists(ndkDirectory))
			{
				var latestVersion = Directory.GetDirectories(ndkDirectory).OrderByDescending(directory => Path.GetFileName(directory)).FirstOrDefault();
				if (latestVersion != null && ValidateNdk(latestVersion))
				{
					cachedNdk_ = latestVersion;
					return latestVersion;
				}
			}

			var ndkBundle = Path.Combine(sdk, "ndk-bundle");
			if (ValidateNdk(ndkBundle))
			{
				cachedNdk_ = ndkBundle;
				return ndkBundle;
			}
		}

		return null;
	}

	/**
	 * Finds the JDK installation path.
	 * @return The path to the JDK, nullptr if not found
	 */
	public string? FindJdk()
	{
		if (cachedJdk_ != null)
		{
			return cachedJdk_;
		}

		var javaHome = Environment.GetEnvironmentVariable("JAVA_HOME");
		if (!string.IsNullOrEmpty(javaHome) && ValidateJdk(javaHome))
		{
			cachedJdk_ = javaHome;
			return javaHome;
		}

		var studioJdk = FindAndroidStudioJdk();
		if (studioJdk != null && ValidateJdk(studioJdk))
		{
			cachedJdk_ = studioJdk;
			return studioJdk;
		}

		var defaultPaths = GetDefaultJdkPaths();
		foreach (var path in defaultPaths)
		{
			if (ValidateJdk(path))
			{
				cachedJdk_ = path;
				return path;
			}
		}

		return null;
	}

	/**
	 * Returns the path to the ADB executable.
	 * @return The path to adb, nullptr if not found
	 */
	public string? GetAdbPath()
	{
		var sdk = FindAndroidSdk();
		if (sdk == null)
		{
			return null;
		}

		var adbPath = Path.Combine(sdk, "platform-tools", "adb.exe");
		return File.Exists(adbPath) ? adbPath : null;
	}

	/**
	 * Returns the path to a specific build-tools version.
	 * @param version The version of build-tools to find, nullptr for the latest version
	 * @return The path to build-tools, nullptr if not found
	 */
	public string? GetBuildToolsPath(string? version = null)
	{
		var sdk = FindAndroidSdk();
		if (sdk == null)
		{
			return null;
		}

		var buildToolsDirectory = Path.Combine(sdk, "build-tools");
		if (!Directory.Exists(buildToolsDirectory))
		{
			return null;
		}

		if (version != null)
		{
			var specificPath = Path.Combine(buildToolsDirectory, version);
			return Directory.Exists(specificPath) ? specificPath : null;
		}

		return Directory.GetDirectories(buildToolsDirectory).OrderByDescending(directory => Path.GetFileName(directory)).FirstOrDefault();
	}

	/**
	 * Returns the available NDK versions installed in the SDK.
	 * @return The list of NDK version strings
	 */
	public IEnumerable<string> GetAvailableNdkVersions()
	{
		var sdk = FindAndroidSdk();
		if (sdk == null)
		{
			yield break;
		}

		var ndkDirectory = Path.Combine(sdk, "ndk");
		if (!Directory.Exists(ndkDirectory))
		{
			yield break;
		}

		foreach (var directory in Directory.GetDirectories(ndkDirectory).OrderByDescending(directory => directory))
		{
			yield return Path.GetFileName(directory);
		}
	}

	/**
	 * Returns the available platform versions installed in the SDK.
	 * @return The list of API level integers
	 */
	public IEnumerable<int> GetAvailablePlatformVersions()
	{
		var sdk = FindAndroidSdk();
		if (sdk == null)
		{
			yield break;
		}

		var platformsDirectory = Path.Combine(sdk, "platforms");
		if (!Directory.Exists(platformsDirectory))
		{
			yield break;
		}

		foreach (var directory in Directory.GetDirectories(platformsDirectory))
		{
			var name = Path.GetFileName(directory);
			if (name.StartsWith("android-") && int.TryParse(name.Substring(8), out var version))
			{
				yield return version;
			}
		}
	}

	/**
	 * Validates an Android SDK path.
	 * @param path The path to validate
	 * @return True, if the path contains a valid Android SDK
	 */
	private bool ValidateAndroidSdk(string path)
	{
		if (string.IsNullOrEmpty(path) || !Directory.Exists(path))
		{
			return false;
		}

		var adb = Path.Combine(path, "platform-tools", "adb.exe");
		if (!File.Exists(adb))
		{
			adb = Path.Combine(path, "platform-tools", "adb");
			if (!File.Exists(adb))
			{
				return false;
			}
		}

		return true;
	}

	/**
	 * Validates an Android NDK path.
	 * @param path The path to validate
	 * @return True, if the path contains a valid Android NDK
	 */
	private bool ValidateNdk(string path)
	{
		if (string.IsNullOrEmpty(path) || !Directory.Exists(path))
		{
			return false;
		}

		var ndkBuild = Path.Combine(path, "ndk-build.cmd");
		var toolchains = Path.Combine(path, "toolchains");

		return File.Exists(ndkBuild) || Directory.Exists(toolchains);
	}

	/**
	 * Validates a JDK path.
	 * @param path The path to validate
	 * @return True, if the path contains a valid JDK
	 */
	private bool ValidateJdk(string path)
	{
		if (string.IsNullOrEmpty(path) || !Directory.Exists(path))
		{
			return false;
		}

		var javac = Path.Combine(path, "bin", "javac.exe");
		if (!File.Exists(javac))
		{
			javac = Path.Combine(path, "bin", "javac");
		}

		return File.Exists(javac);
	}

	/**
	 * Returns the default Android SDK search paths.
	 * @return The list of default SDK paths to search
	 */
	private IEnumerable<string> GetDefaultAndroidSdkPaths()
	{
		var localAppData = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
		var userProfile = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);

		yield return Path.Combine(localAppData, "Android", "Sdk");
		yield return Path.Combine(userProfile, "Android", "Sdk");
		yield return Path.Combine(userProfile, "AppData", "Local", "Android", "Sdk");
		yield return @"C:\Android\sdk";
		yield return @"C:\Android\android-sdk";
	}

	/**
	 * Returns the default JDK search paths.
	 * @return The list of default JDK paths to search
	 */
	private IEnumerable<string> GetDefaultJdkPaths()
	{
		var programFiles = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);

		var javaDirectory = Path.Combine(programFiles, "Java");
		if (Directory.Exists(javaDirectory))
		{
			foreach (var directory in Directory.GetDirectories(javaDirectory).Where(directory => Path.GetFileName(directory).StartsWith("jdk")).OrderByDescending(directory => directory))
			{
				yield return directory;
			}
		}

		var microsoftJdkDirectory = Path.Combine(programFiles, "Microsoft", "jdk");
		if (Directory.Exists(microsoftJdkDirectory))
		{
			foreach (var directory in Directory.GetDirectories(microsoftJdkDirectory).OrderByDescending(directory => directory))
			{
				yield return directory;
			}
		}

		var adoptiumDirectory = Path.Combine(programFiles, "Eclipse Adoptium");
		if (Directory.Exists(adoptiumDirectory))
		{
			foreach (var directory in Directory.GetDirectories(adoptiumDirectory).Where(directory => Path.GetFileName(directory).StartsWith("jdk")).OrderByDescending(directory => directory))
			{
				yield return directory;
			}
		}
	}

	/**
	 * Finds the Android SDK path from Android Studio configuration.
	 * @return The SDK path from Android Studio, nullptr if not found
	 */
	private string? FindAndroidStudioSdk()
	{
		try
		{
			using var key = Registry.CurrentUser.OpenSubKey(@"Software\Google\AndroidStudio");
			var studioPath = key?.GetValue("Path") as string;

			if (!string.IsNullOrEmpty(studioPath))
			{
				var configDirectory = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), ".android");

				var localAppData = Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData);
				return Path.Combine(localAppData, "Android", "Sdk");
			}
		}
		catch
		{
			// Ignore registry access errors
		}

		return null;
	}

	/**
	 * Finds the JDK bundled with Android Studio.
	 * @return The JDK path from Android Studio, nullptr if not found
	 */
	private string? FindAndroidStudioJdk()
	{
		var programFiles = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);

		var studioPath = Path.Combine(programFiles, "Android", "Android Studio");
		if (Directory.Exists(studioPath))
		{
			var jbrPath = Path.Combine(studioPath, "jbr");
			if (Directory.Exists(jbrPath))
			{
				return jbrPath;
			}

			var jrePath = Path.Combine(studioPath, "jre");
			if (Directory.Exists(jrePath))
			{
				return jrePath;
			}
		}

		return null;
	}

	/**
	 * Clears cached paths, forcing re-detection on next call.
	 */
	public void ClearCache()
	{
		cachedAndroidSdk_ = null;
		cachedNdk_ = null;
		cachedJdk_ = null;
	}
}

}
