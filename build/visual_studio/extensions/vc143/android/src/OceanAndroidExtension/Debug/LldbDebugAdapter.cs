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
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using OceanAndroidExtension.Services;

namespace OceanAndroidExtension.Debug
{

/**
 * This class implements a debug adapter for native C++ debugging using LLDB from the Android NDK.
 * @ingroup oceanandroidextension
 */
public class LldbDebugAdapter
{
	/// The SDK locator service for finding NDK paths.
	private readonly ISdkLocatorService sdkLocator_;

	/// The ADB service for device communication.
	private readonly IAdbService adbService_;

	/// The output window service for logging.
	private readonly IOutputWindowService outputService_;

	/// The LLDB server process.
	private Process? lldbServer_;

	/// The local port for LLDB connection.
	private int localPort_;

	/// The device serial number.
	private string deviceSerial_ = string.Empty;

	/**
	 * Creates a new LLDB debug adapter.
	 * @param sdkLocator The SDK locator service for finding NDK paths
	 * @param adbService The ADB service for device communication
	 * @param outputService The output window service for logging
	 */
	public LldbDebugAdapter(ISdkLocatorService sdkLocator, IAdbService adbService, IOutputWindowService outputService)
	{
		sdkLocator_ = sdkLocator;
		adbService_ = adbService;
		outputService_ = outputService;
	}

	/**
	 * Starts the LLDB server on the device and sets up port forwarding.
	 * @param deviceSerial The serial number of the target device
	 * @param packageName The package name of the application to debug
	 * @param abi The target ABI (e.g., arm64-v8a, armeabi-v7a, x86_64, x86)
	 * @param cancellationToken The cancellation token
	 * @return True, if the LLDB server started successfully
	 */
	public async Task<bool> StartAsync(string deviceSerial, string packageName, string abi, CancellationToken cancellationToken)
	{
		deviceSerial_ = deviceSerial;

		outputService_.WriteLine($"Starting native debugger for {packageName}...");

		var lldbServerPath = FindLldbServer(abi);
		if (lldbServerPath == null)
		{
			outputService_.WriteLine("Error: Could not find lldb-server for " + abi);
			return false;
		}

		var remoteLldbPath = $"/data/local/tmp/lldb-server";
		if (!await PushLldbServerAsync(lldbServerPath, remoteLldbPath, cancellationToken))
		{
			return false;
		}

		var processId = await adbService_.GetProcessIdAsync(deviceSerial, packageName, cancellationToken);
		if (!processId.HasValue)
		{
			outputService_.WriteLine("Error: Target process not found");
			return false;
		}

		localPort_ = FindAvailablePort();
		var remotePort = localPort_;

		if (!await adbService_.ForwardPortAsync(deviceSerial, localPort_, remotePort, cancellationToken))
		{
			outputService_.WriteLine("Error: Failed to forward debug port");
			return false;
		}

		var lldbServerCommand = $"run-as {packageName} {remoteLldbPath} platform --listen *:{remotePort} --server";
		_ = Task.Run(() => adbService_.ShellAsync(deviceSerial, lldbServerCommand, cancellationToken));

		await Task.Delay(1000, cancellationToken);

		outputService_.WriteLine($"LLDB server started on port {localPort_}");

		return true;
	}

	/**
	 * Returns the local LLDB connection URL.
	 * @return The connection URL in the format "connect://localhost:port"
	 */
	public string GetConnectionUrl() => $"connect://localhost:{localPort_}";

	/**
	 * Returns the path to the local LLDB executable.
	 * @return The path to lldb.exe, nullptr if not found
	 */
	public string? GetLldbPath()
	{
		var ndkPath = sdkLocator_.FindNdk();
		if (ndkPath == null)
		{
			return null;
		}

		var lldbPath = Path.Combine(ndkPath, "toolchains", "llvm", "prebuilt", "windows-x86_64", "bin", "lldb.exe");

		return File.Exists(lldbPath) ? lldbPath : null;
	}

	/**
	 * Creates LLDB launch configuration.
	 * @param packageName The package name of the application
	 * @param symbolPath The path to symbol files, nullptr if not specified
	 * @param sourceDirectories The source directories for source mapping, nullptr if not specified
	 * @return The launch configuration dictionary
	 */
	public Dictionary<string, object> CreateLaunchConfiguration(string packageName, string? symbolPath = null, string[]? sourceDirectories = null)
	{
		var configuration = new Dictionary<string, object>
		{
			["type"] = "lldb",
			["request"] = "attach",
			["name"] = "Android Native Debug",
			["processName"] = packageName,
			["waitFor"] = false,
			["initCommands"] = new[]
			{
				"platform select remote-android",
				$"platform connect {GetConnectionUrl()}"
			}
		};

		if (symbolPath != null)
		{
			configuration["symbolSearchPaths"] = new[] { symbolPath };
		}

		if (sourceDirectories != null)
		{
			configuration["sourceMap"] = CreateSourceMap(sourceDirectories);
		}

		return configuration;
	}

	/**
	 * Creates a source map for debugging.
	 * @param sourceDirectories The source directories to map
	 * @return The source map dictionary
	 */
	private Dictionary<string, string> CreateSourceMap(string[] sourceDirectories)
	{
		var sourceMap = new Dictionary<string, string>();

		foreach (var directory in sourceDirectories)
		{
			sourceMap["/builddir/"] = directory;
		}

		return sourceMap;
	}

	/**
	 * Finds the lldb-server binary for the specified ABI.
	 * @param abi The target ABI
	 * @return The path to lldb-server, nullptr if not found
	 */
	private string? FindLldbServer(string abi)
	{
		var ndkPath = sdkLocator_.FindNdk();
		if (ndkPath == null)
		{
			return null;
		}

		var architectureDirectory = abi switch
		{
			"arm64-v8a" => "android-arm64",
			"armeabi-v7a" => "android-arm",
			"x86_64" => "android-x86_64",
			"x86" => "android-i386",
			_ => null
		};

		if (architectureDirectory == null)
		{
			return null;
		}

		var possiblePaths = new[]
		{
			Path.Combine(ndkPath, "toolchains", "llvm", "prebuilt", "windows-x86_64", "lib64", "clang", "17", "lib", "linux", architectureDirectory, "lldb-server"),
			Path.Combine(ndkPath, "toolchains", "llvm", "prebuilt", "windows-x86_64", "lib", "clang", "17.0.2", "lib", "linux", architectureDirectory, "lldb-server")
		};

		foreach (var path in possiblePaths)
		{
			if (File.Exists(path))
			{
				return path;
			}
		}

		return null;
	}

	/**
	 * Pushes the lldb-server binary to the device.
	 * @param localPath The local path to lldb-server
	 * @param remotePath The remote path on the device
	 * @param cancellationToken The cancellation token
	 * @return True, if the push succeeded
	 */
	private async Task<bool> PushLldbServerAsync(string localPath, string remotePath, CancellationToken cancellationToken)
	{
		outputService_.WriteLine($"Pushing lldb-server to device...");

		if (!await adbService_.PushFileAsync(deviceSerial_, localPath, remotePath, cancellationToken))
		{
			outputService_.WriteLine("Error: Failed to push lldb-server");
			return false;
		}

		await adbService_.ShellAsync(deviceSerial_, $"chmod +x {remotePath}", cancellationToken);

		return true;
	}

	/**
	 * Finds an available TCP port.
	 * @return An available port number
	 */
	private static int FindAvailablePort()
	{
		var listener = new TcpListener(IPAddress.Loopback, 0);
		listener.Start();
		var port = ((IPEndPoint)listener.LocalEndpoint).Port;
		listener.Stop();
		return port;
	}

	/**
	 * Stops the LLDB debug session.
	 * @param cancellationToken The cancellation token
	 */
	public async Task StopAsync(CancellationToken cancellationToken)
	{
		if (lldbServer_ != null)
		{
			try
			{
				lldbServer_.Kill();
			}
			catch
			{
			}

			lldbServer_.Dispose();
			lldbServer_ = null;
		}

		await adbService_.ShellAsync(deviceSerial_, "pkill lldb-server", cancellationToken);

		outputService_.WriteLine("Native debug session ended");
	}
}

}
