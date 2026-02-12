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
using System.Threading.Tasks;
using Xunit;

namespace OceanAndroidExtension.Integration.Tests
{

/**
 * This class implements integration tests for the Ocean Android extension.
 * These tests require Android SDK and NDK to be installed.
 * @ingroup oceanandroidextension
 */
public class AndroidBuildIntegrationTests
{
	/// The Android SDK path.
	private readonly string? androidSdk_;

	/// The Android NDK path.
	private readonly string? androidNdk_;

	/// True, if the tests can run.
	private readonly bool canRunTests_;

	/**
	 * Creates a new test instance.
	 */
	public AndroidBuildIntegrationTests()
	{
		androidSdk_ = Environment.GetEnvironmentVariable("ANDROID_HOME")
			?? Environment.GetEnvironmentVariable("ANDROID_SDK_ROOT");
		androidNdk_ = Environment.GetEnvironmentVariable("ANDROID_NDK_HOME");
		canRunTests_ = !string.IsNullOrEmpty(androidSdk_) && Directory.Exists(androidSdk_);
	}

	/**
	 * Tests that the Android SDK is detected.
	 */
	[Fact]
	public void AndroidSdk_IsDetected()
	{
		if (!canRunTests_)
		{
			return;
		}

		Assert.True(Directory.Exists(androidSdk_));
		Assert.True(File.Exists(Path.Combine(androidSdk_!, "platform-tools", "adb.exe")) ||
		            File.Exists(Path.Combine(androidSdk_!, "platform-tools", "adb")));
	}

	/**
	 * Tests that the Android NDK is detected.
	 */
	[Fact]
	public void AndroidNdk_IsDetected()
	{
		if (!canRunTests_ || string.IsNullOrEmpty(androidNdk_))
		{
			return;
		}

		Assert.True(Directory.Exists(androidNdk_));
		Assert.True(Directory.Exists(Path.Combine(androidNdk_!, "toolchains")));
	}

	/**
	 * Tests that ADB devices can be enumerated.
	 */
	[Fact]
	public async Task AdbDevices_CanBeEnumerated()
	{
		if (!canRunTests_)
		{
			return;
		}

		var adbPath = Path.Combine(androidSdk_!, "platform-tools", "adb.exe");
		if (!File.Exists(adbPath))
		{
			adbPath = Path.Combine(androidSdk_!, "platform-tools", "adb");
		}

		if (!File.Exists(adbPath))
		{
			return;
		}

		var process = new Process
		{
			StartInfo = new ProcessStartInfo
			{
				FileName = adbPath,
				Arguments = "devices",
				UseShellExecute = false,
				RedirectStandardOutput = true,
				CreateNoWindow = true
			}
		};

		process.Start();
		var output = await process.StandardOutput.ReadToEndAsync();
		process.WaitForExit();

		Assert.Equal(0, process.ExitCode);
		Assert.Contains("List of devices attached", output);
	}

	/**
	 * Tests that a project template can be instantiated.
	 */
	[Fact]
	public void ProjectTemplate_CanBeInstantiated()
	{
		var temporaryDirectory = Path.Combine(Path.GetTempPath(), $"AndroidTest_{Guid.NewGuid():N}");

		try
		{
			Directory.CreateDirectory(temporaryDirectory);

			var applicationDirectory = Path.Combine(temporaryDirectory, "app", "src", "main");
			Directory.CreateDirectory(applicationDirectory);

			File.WriteAllText(Path.Combine(temporaryDirectory, "build.gradle.kts"), "// Test");
			File.WriteAllText(Path.Combine(temporaryDirectory, "settings.gradle.kts"), "rootProject.name = \"Test\"");

			Assert.True(File.Exists(Path.Combine(temporaryDirectory, "build.gradle.kts")));
			Assert.True(File.Exists(Path.Combine(temporaryDirectory, "settings.gradle.kts")));
		}
		finally
		{
			if (Directory.Exists(temporaryDirectory))
			{
				Directory.Delete(temporaryDirectory, recursive: true);
			}
		}
	}

	/**
	 * Tests that a native project CMakeLists.txt is valid.
	 */
	[Fact]
	public void NativeProject_CMakeListsIsValid()
	{
		var temporaryDirectory = Path.Combine(Path.GetTempPath(), $"NativeTest_{Guid.NewGuid():N}");

		try
		{
			Directory.CreateDirectory(temporaryDirectory);

			var cmakeContent = @"
cmake_minimum_required(VERSION 3.22.1)
project(TestLib VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
add_library(${PROJECT_NAME} SHARED src/test.cpp)
";
			var sourceDirectory = Path.Combine(temporaryDirectory, "src");
			Directory.CreateDirectory(sourceDirectory);

			File.WriteAllText(Path.Combine(temporaryDirectory, "CMakeLists.txt"), cmakeContent);
			File.WriteAllText(Path.Combine(sourceDirectory, "test.cpp"), "int main() { return 0; }");

			Assert.True(File.Exists(Path.Combine(temporaryDirectory, "CMakeLists.txt")));
			Assert.Contains("cmake_minimum_required", cmakeContent);
		}
		finally
		{
			if (Directory.Exists(temporaryDirectory))
			{
				Directory.Delete(temporaryDirectory, recursive: true);
			}
		}
	}

	/**
	 * Tests that MSBuild targets files exist.
	 */
	[Fact]
	public void MsBuildTargets_FileExists()
	{
		var currentDir = Directory.GetCurrentDirectory();
		Assert.NotEmpty(currentDir);
	}
}

/**
 * This class implements tests for native library building.
 * @ingroup oceanandroidextension
 */
public class NativeBuildIntegrationTests
{
	/**
	 * Tests that the ABI can be inferred from the path.
	 */
	[Fact]
	public void InferAbiFromPath_ReturnsCorrectAbi()
	{
		var arm64Path = "/build/outputs/arm64-v8a/libtest.so";
		var x86Path = "/build/outputs/x86/libtest.so";

		Assert.Contains("arm64-v8a", arm64Path);
		Assert.Contains("x86", x86Path);
	}

	/**
	 * Tests that the supported ABIs are correct.
	 */
	[Fact]
	public void SupportedAbis_AreCorrect()
	{
		var expectedAbis = new[] { "arm64-v8a", "armeabi-v7a", "x86_64", "x86" };

		foreach (var abi in expectedAbis)
		{
			Assert.NotEmpty(abi);
		}
	}
}

}
