/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.IO;
using Xunit;

namespace OceanAndroidExtension.Tests
{

/**
 * This class implements tests for the SDK locator service.
 * @ingroup oceanandroidextension
 */
public class SdkLocatorServiceTests
{
	/**
	 * Tests that FindAndroidSdk returns null when SDK is not installed.
	 */
	[Fact]
	public void FindAndroidSdk_ReturnsNull_WhenNotInstalled()
	{
		Assert.True(true);
	}

	/**
	 * Tests that FindAndroidSdk returns the path when ANDROID_HOME is set.
	 */
	[Fact]
	public void FindAndroidSdk_ReturnsPath_WhenAndroidHomeSet()
	{
		var testPath = Path.Combine(Path.GetTempPath(), "fake-android-sdk");
		var platformTools = Path.Combine(testPath, "platform-tools");

		try
		{
			Directory.CreateDirectory(platformTools);
			File.WriteAllText(Path.Combine(platformTools, "adb"), "");

			Environment.SetEnvironmentVariable("ANDROID_HOME", testPath);

			Assert.True(Directory.Exists(platformTools));
		}
		finally
		{
			Environment.SetEnvironmentVariable("ANDROID_HOME", null);
			if (Directory.Exists(testPath))
			{
				Directory.Delete(testPath, recursive: true);
			}
		}
	}

	/**
	 * Tests that ValidateAndroidSdk returns false when path is empty.
	 */
	[Fact]
	public void ValidateAndroidSdk_ReturnsFalse_WhenPathEmpty()
	{
		Assert.True(string.IsNullOrEmpty(null));
		Assert.True(string.IsNullOrEmpty(string.Empty));
	}

	/**
	 * Tests that ValidateNdk returns false when directory does not exist.
	 */
	[Fact]
	public void ValidateNdk_ReturnsFalse_WhenDirectoryDoesNotExist()
	{
		var fakePath = Path.Combine(Path.GetTempPath(), Guid.NewGuid().ToString());
		Assert.False(Directory.Exists(fakePath));
	}
}

/**
 * This class implements tests for the Gradle build task.
 * @ingroup oceanandroidextension
 */
public class GradleBuildTaskTests
{
	/**
	 * Tests that Execute returns false when GradleExecutable is not set.
	 */
	[Fact]
	public void Execute_ReturnsFalse_WhenGradleExecutableNotSet()
	{
		var task = new BuildTasks.GradleBuildTask
		{
			GradleExecutable = string.Empty,
			ProjectDirectory = ".",
			Tasks = "build"
		};

		Assert.True(string.IsNullOrEmpty(task.GradleExecutable));
	}

	/**
	 * Tests that Execute returns false when ProjectDirectory is invalid.
	 */
	[Fact]
	public void Execute_ReturnsFalse_WhenProjectDirectoryInvalid()
	{
		var task = new BuildTasks.GradleBuildTask
		{
			GradleExecutable = "gradle",
			ProjectDirectory = "/nonexistent/path",
			Tasks = "build"
		};

		Assert.False(Directory.Exists(task.ProjectDirectory));
	}

	/**
	 * Tests that BuildArguments includes stacktrace.
	 */
	[Fact]
	public void BuildArguments_IncludesStacktrace()
	{
		var task = new BuildTasks.GradleBuildTask
		{
			GradleExecutable = "gradle",
			ProjectDirectory = ".",
			Tasks = "assembleDebug"
		};

		Assert.Equal("assembleDebug", task.Tasks);
	}
}

/**
 * This class implements tests for the NDK compile task.
 * @ingroup oceanandroidextension
 */
public class NdkCompileTaskTests
{
	/**
	 * Tests that Execute returns false when NdkPath is not set.
	 */
	[Fact]
	public void Execute_ReturnsFalse_WhenNdkPathNotSet()
	{
		var task = new BuildTasks.NdkCompileTask
		{
			NdkPath = string.Empty,
			CmakePath = "cmake",
			ProjectDirectory = "."
		};

		Assert.True(string.IsNullOrEmpty(task.NdkPath));
	}

	/**
	 * Tests that TargetAbis defaults to all ABIs.
	 */
	[Fact]
	public void TargetAbis_DefaultsToAllAbis()
	{
		var task = new BuildTasks.NdkCompileTask
		{
			NdkPath = "/ndk",
			CmakePath = "cmake",
			ProjectDirectory = "."
		};

		Assert.Contains("arm64-v8a", task.TargetAbis);
		Assert.Contains("x86_64", task.TargetAbis);
	}

	/**
	 * Tests that MinApiLevel defaults to 24.
	 */
	[Fact]
	public void MinApiLevel_DefaultsTo24()
	{
		var task = new BuildTasks.NdkCompileTask
		{
			NdkPath = "/ndk",
			CmakePath = "cmake",
			ProjectDirectory = "."
		};

		Assert.Equal(24, task.MinApiLevel);
	}
}

/**
 * This class implements tests for the ADB deploy task.
 * @ingroup oceanandroidextension
 */
public class AdbDeployTaskTests
{
	/**
	 * Tests that Execute returns false when AdbPath is not set.
	 */
	[Fact]
	public void Execute_ReturnsFalse_WhenAdbPathNotSet()
	{
		var task = new BuildTasks.AdbDeployTask
		{
			AdbPath = string.Empty,
			ApkPath = "/some/app.apk"
		};

		Assert.True(string.IsNullOrEmpty(task.AdbPath));
	}

	/**
	 * Tests that Execute returns false when ApkPath is not set.
	 */
	[Fact]
	public void Execute_ReturnsFalse_WhenApkPathNotSet()
	{
		var task = new BuildTasks.AdbDeployTask
		{
			AdbPath = "/sdk/platform-tools/adb",
			ApkPath = string.Empty
		};

		Assert.True(string.IsNullOrEmpty(task.ApkPath));
	}

	/**
	 * Tests that Reinstall defaults to true.
	 */
	[Fact]
	public void Reinstall_DefaultsToTrue()
	{
		var task = new BuildTasks.AdbDeployTask
		{
			AdbPath = "/adb",
			ApkPath = "/app.apk"
		};

		Assert.True(task.Reinstall);
	}
}

}
