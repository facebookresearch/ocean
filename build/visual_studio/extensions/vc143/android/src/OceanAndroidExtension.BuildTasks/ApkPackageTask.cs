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
using System.IO.Compression;
using System.Linq;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace OceanAndroidExtension.BuildTasks
{

/**
 * This class implements an MSBuild task that packages native libraries into an APK-ready structure.
 * @ingroup oceanandroidextension
 */
public class ApkPackageTask : CancelableTask
{
	/// The native library files to include.
	[Required]
	public ITaskItem[] NativeLibraries { get; set; } = Array.Empty<ITaskItem>();

	/// The output directory for jniLibs structure.
	[Required]
	public string OutputDirectory { get; set; } = string.Empty;

	/// The Android project's jniLibs path.
	public string? JniLibsPath { get; set; }

	/// True, to strip debug symbols from libraries.
	public bool StripDebugSymbols { get; set; } = false;

	/// The path to the strip tool (from NDK).
	public string? StripToolPath { get; set; }

	/// Output: The files that were copied.
	[Output]
	public ITaskItem[]? CopiedFiles { get; set; }

	/**
	 * Executes the APK package task.
	 * @return True, if the packaging succeeded
	 */
	public override bool Execute()
	{
		try
		{
			if (NativeLibraries.Length == 0)
			{
				Log.LogMessage(MessageImportance.Normal, "No native libraries to package");
				return true;
			}

			var copiedFiles = new List<ITaskItem>();
			var targetDirectory = JniLibsPath ?? OutputDirectory;

			Log.LogMessage(MessageImportance.High, $"Packaging {NativeLibraries.Length} native libraries");

			foreach (var library in NativeLibraries)
			{
				if (IsCancelled)
				{
					Log.LogMessage(MessageImportance.High, "Build cancelled.");
					return false;
				}

				var sourcePath = library.ItemSpec;
				if (!File.Exists(sourcePath))
				{
					Log.LogWarning($"Native library not found: {sourcePath}");
					continue;
				}

				var abi = library.GetMetadata("AndroidAbi");
				if (string.IsNullOrEmpty(abi))
				{
					abi = InferAbiFromPath(sourcePath);
				}

				if (string.IsNullOrEmpty(abi))
				{
					Log.LogWarning($"Could not determine ABI for: {sourcePath}");
					continue;
				}

				var abiDirectory = Path.Combine(targetDirectory, abi);
				Directory.CreateDirectory(abiDirectory);

				var fileName = Path.GetFileName(sourcePath);
				var targetPath = Path.Combine(abiDirectory, fileName);

				if (StripDebugSymbols && !string.IsNullOrEmpty(StripToolPath) && File.Exists(StripToolPath))
				{
					StripAndCopy(sourcePath, targetPath, abi!);
				}
				else
				{
					File.Copy(sourcePath, targetPath, overwrite: true);
				}

				var item = new TaskItem(targetPath);
				item.SetMetadata("AndroidAbi", abi);
				copiedFiles.Add(item);

				Log.LogMessage(MessageImportance.Normal, $"  {abi}/{fileName}");
			}

			CopiedFiles = copiedFiles.ToArray();

			Log.LogMessage(MessageImportance.High, $"Packaged {copiedFiles.Count} native libraries to {targetDirectory}");
			return true;
		}
		catch (Exception exception)
		{
			Log.LogErrorFromException(exception);
			return false;
		}
	}

	/**
	 * Infers the ABI from the file path.
	 * @param path The file path
	 * @return The ABI, nullptr if not found
	 */
	private string? InferAbiFromPath(string path)
	{
		var knownAbis = new[] { "arm64-v8a", "armeabi-v7a", "x86_64", "x86" };
		var pathParts = path.Replace('\\', '/').Split('/');

		foreach (var part in pathParts)
		{
			if (knownAbis.Contains(part))
			{
				return part;
			}
		}

		return null;
	}

	/**
	 * Strips debug symbols and copies the file.
	 * @param sourcePath The source file path
	 * @param targetPath The target file path
	 * @param abi The target ABI
	 */
	private void StripAndCopy(string sourcePath, string targetPath, string abi)
	{
		var stripTool = GetStripToolForAbi(abi);
		if (stripTool == null || !File.Exists(stripTool))
		{
			Log.LogMessage(MessageImportance.Low, $"Strip tool not found for {abi}, copying without stripping");
			File.Copy(sourcePath, targetPath, overwrite: true);
			return;
		}

		File.Copy(sourcePath, targetPath, overwrite: true);

		try
		{
			var startInfo = new ProcessStartInfo
			{
				FileName = stripTool,
				Arguments = $"--strip-unneeded \"{targetPath}\"",
				UseShellExecute = false,
				CreateNoWindow = true
			};

			using var process = Process.Start(startInfo);

			if (process != null)
			{
				RegisterProcess(process);

				try
				{
					process.WaitForExit();
				}
				finally
				{
					UnregisterProcess(process);
				}
			}

			if (process?.ExitCode == 0)
			{
				Log.LogMessage(MessageImportance.Low, $"Stripped debug symbols from {Path.GetFileName(targetPath)}");
			}
		}
		catch (Exception exception)
		{
			Log.LogWarning($"Failed to strip {Path.GetFileName(targetPath)}: {exception.Message}");
		}
	}

	/**
	 * Returns the strip tool path for the specified ABI.
	 * @param abi The target ABI
	 * @return The strip tool path, nullptr if not found
	 */
	private string? GetStripToolForAbi(string abi)
	{
		if (string.IsNullOrEmpty(StripToolPath))
		{
			return null;
		}

		var ndkPath = Path.GetDirectoryName(Path.GetDirectoryName(Path.GetDirectoryName(Path.GetDirectoryName(StripToolPath))));
		if (ndkPath == null)
		{
			return null;
		}

		var triple = abi switch
		{
			"arm64-v8a" => "aarch64-linux-android",
			"armeabi-v7a" => "arm-linux-androideabi",
			"x86_64" => "x86_64-linux-android",
			"x86" => "i686-linux-android",
			_ => null
		};

		if (triple == null)
		{
			return null;
		}

		var llvmBin = Path.Combine(ndkPath, "toolchains", "llvm", "prebuilt", "windows-x86_64", "bin");
		var stripPath = Path.Combine(llvmBin, "llvm-strip.exe");

		return File.Exists(stripPath) ? stripPath : null;
	}
}

}
