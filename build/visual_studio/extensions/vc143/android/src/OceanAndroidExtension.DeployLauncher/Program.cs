/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.Diagnostics;

namespace OceanAndroidExtension.DeployLauncher
{

/**
 * This class implements a launcher exe for F5/Start in Visual Studio.
 * WindowsLocalDebugger attaches a native Win32 debugger to the launched process.
 * Batch files cannot run under a native debugger (cmd.exe dies immediately).
 * This exe is the debugger target; it spawns deploy.bat in a separate detached
 * process that is not under the debugger, waits for it, and returns its exit code.
 * @ingroup oceanandroidextension
 */
internal static class Program
{
	/**
	 * Main entry point for the deploy launcher.
	 * @param args Command line arguments: deploy.bat, projectDir, packageName, apkPath
	 * @return Exit code from the deploy script, or 1 on error
	 */
	static int Main(string[] args)
	{
		if (args.Length < 4)
		{
			Console.Error.WriteLine("Usage: OceanDeployLauncher.exe <deploy.bat> <projectDir> <packageName> <apkPath>");
			return 1;
		}

		var deployScript = args[0];
		var projectDir = args[1];
		var packageName = args[2];
		var apkPath = args[3];

		try
		{
			var psi = new ProcessStartInfo
			{
				FileName = deployScript,
				Arguments = string.Format("\"{0}\" \"{1}\" \"{2}\"", projectDir, packageName, apkPath),
				UseShellExecute = true,
				WorkingDirectory = projectDir
			};

			using (var process = Process.Start(psi))
			{
				if (process == null)
				{
					Console.Error.WriteLine("Failed to start deploy script: " + deployScript);
					return 1;
				}

				process.WaitForExit();
				return process.ExitCode;
			}
		}
		catch (Exception ex)
		{
			Console.Error.WriteLine("Error launching deploy script: " + ex.Message);
			return 1;
		}
	}
}

}
