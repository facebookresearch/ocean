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
using System.Threading;
using System.Threading.Tasks;
using OceanAndroidExtension.Services;

namespace OceanAndroidExtension.Debug
{

/**
 * This class implements a debug adapter for Java/Kotlin debugging using JDWP (Java Debug Wire Protocol).
 * @ingroup oceanandroidextension
 */
public class JdwpDebugAdapter
{
	/// The ADB service for device communication.
	private readonly IAdbService adbService_;

	/// The output window service for logging.
	private readonly IOutputWindowService outputService_;

	/// The forward process for port forwarding.
	private Process? forwardProcess_;

	/// The local port for JDWP connection.
	private int localPort_;

	/// The device serial number.
	private string deviceSerial_ = string.Empty;

	/// The package name of the application being debugged.
	private string packageName_ = string.Empty;

	/**
	 * Creates a new JDWP debug adapter.
	 * @param adbService The ADB service for device communication
	 * @param outputService The output window service for logging
	 */
	public JdwpDebugAdapter(IAdbService adbService, IOutputWindowService outputService)
	{
		adbService_ = adbService;
		outputService_ = outputService;
	}

	/**
	 * Attaches to a running Android application for debugging.
	 * @param deviceSerial The serial number of the target device
	 * @param packageName The package name of the application to debug
	 * @param cancellationToken The cancellation token
	 * @return True, if the attachment succeeded
	 */
	public async Task<bool> AttachAsync(string deviceSerial, string packageName, CancellationToken cancellationToken)
	{
		deviceSerial_ = deviceSerial;
		packageName_ = packageName;

		outputService_.WriteLine($"Attaching debugger to {packageName} on {deviceSerial}...");

		var processId = await WaitForProcessAsync(cancellationToken);
		if (processId == null)
		{
			outputService_.WriteLine("Error: Could not find debuggable process");
			return false;
		}

		outputService_.WriteLine($"Found process with PID {processId}");

		localPort_ = FindAvailablePort();

		var forwardResult = await adbService_.JdwpForwardAsync(deviceSerial, processId.Value, localPort_, cancellationToken);
		if (forwardResult == null)
		{
			outputService_.WriteLine("Error: Failed to forward JDWP port");
			return false;
		}

		outputService_.WriteLine($"JDWP forwarded to localhost:{localPort_}");

		return true;
	}

	/**
	 * Returns the local port for JDWP connection.
	 * @return The local port number
	 */
	public int GetLocalPort() => localPort_;

	/**
	 * Waits for the target process to become available.
	 * @param cancellationToken The cancellation token
	 * @return The process ID, nullptr if not found
	 */
	private async Task<int?> WaitForProcessAsync(CancellationToken cancellationToken)
	{
		const int maxAttempts = 30;
		const int delayMilliseconds = 500;

		for (int i = 0; i < maxAttempts && !cancellationToken.IsCancellationRequested; i++)
		{
			var processId = await adbService_.GetProcessIdAsync(deviceSerial_, packageName_, cancellationToken);
			if (processId.HasValue)
			{
				return processId;
			}

			await Task.Delay(delayMilliseconds, cancellationToken);
		}

		return null;
	}

	/**
	 * Finds an available TCP port for JDWP forwarding.
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
	 * Sends a handshake to the JDWP agent.
	 * @param cancellationToken The cancellation token
	 * @return True, if the handshake succeeded
	 */
	public async Task<bool> HandshakeAsync(CancellationToken cancellationToken)
	{
		try
		{
			using var client = new TcpClient();
			await client.ConnectAsync(IPAddress.Loopback, localPort_);

			var stream = client.GetStream();

			var handshake = System.Text.Encoding.ASCII.GetBytes("JDWP-Handshake");
			await stream.WriteAsync(handshake, 0, handshake.Length, cancellationToken);

			var response = new byte[14];
			var bytesRead = await stream.ReadAsync(response, 0, response.Length, cancellationToken);

			if (bytesRead == 14 && System.Text.Encoding.ASCII.GetString(response) == "JDWP-Handshake")
			{
				outputService_.WriteLine("JDWP handshake successful");
				return true;
			}
		}
		catch (Exception exception)
		{
			outputService_.WriteLine($"JDWP handshake failed: {exception.Message}");
		}

		return false;
	}

	/**
	 * Disconnects from the debug session.
	 */
	public void Disconnect()
	{
		forwardProcess_?.Kill();
		forwardProcess_?.Dispose();
		forwardProcess_ = null;
	}
}

/**
 * This struct defines the JDWP packet structure.
 * @ingroup oceanandroidextension
 */
public struct JdwpPacket
{
	/// The packet length.
	public int Length;

	/// The packet ID.
	public int Id;

	/// The packet flags.
	public byte Flags;

	/// The command set.
	public byte CommandSet;

	/// The command.
	public byte Command;

	/// The packet data.
	public byte[]? Data;

	/// Returns whether this packet is a reply.
	public bool IsReply => (Flags & 0x80) != 0;

	/// Returns the error code from the packet.
	public short ErrorCode => (short)((Data?[0] ?? 0) << 8 | (Data?[1] ?? 0));
}

/**
 * This class defines JDWP command sets and commands.
 * @ingroup oceanandroidextension
 */
public static class JdwpCommands
{
	/// Virtual Machine command set (1).
	public const byte VirtualMachine = 1;

	/// VM_Version command.
	public const byte VM_Version = 1;

	/// VM_ClassesBySignature command.
	public const byte VM_ClassesBySignature = 2;

	/// VM_AllClasses command.
	public const byte VM_AllClasses = 3;

	/// VM_AllThreads command.
	public const byte VM_AllThreads = 4;

	/// VM_TopLevelThreadGroups command.
	public const byte VM_TopLevelThreadGroups = 5;

	/// VM_Dispose command.
	public const byte VM_Dispose = 6;

	/// VM_IDSizes command.
	public const byte VM_IDSizes = 7;

	/// VM_Suspend command.
	public const byte VM_Suspend = 8;

	/// VM_Resume command.
	public const byte VM_Resume = 9;

	/// VM_Exit command.
	public const byte VM_Exit = 10;

	/// Reference Type command set (2).
	public const byte ReferenceType = 2;

	/// RT_Signature command.
	public const byte RT_Signature = 1;

	/// RT_ClassLoader command.
	public const byte RT_ClassLoader = 2;

	/// RT_Modifiers command.
	public const byte RT_Modifiers = 3;

	/// RT_Fields command.
	public const byte RT_Fields = 4;

	/// RT_Methods command.
	public const byte RT_Methods = 5;

	/// Thread Reference command set (11).
	public const byte ThreadReference = 11;

	/// TR_Name command.
	public const byte TR_Name = 1;

	/// TR_Suspend command.
	public const byte TR_Suspend = 2;

	/// TR_Resume command.
	public const byte TR_Resume = 3;

	/// TR_Status command.
	public const byte TR_Status = 4;

	/// TR_Frames command.
	public const byte TR_Frames = 6;

	/// Event Request command set (15).
	public const byte EventRequest = 15;

	/// ER_Set command.
	public const byte ER_Set = 1;

	/// ER_Clear command.
	public const byte ER_Clear = 2;

	/// ER_ClearAllBreakpoints command.
	public const byte ER_ClearAllBreakpoints = 3;
}

}
