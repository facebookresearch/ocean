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
using System.Linq;
using System.Text;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace OceanAndroidExtension.BuildTasks
{

/**
 * This class implements an MSBuild task that creates a static library (.a) from object files using llvm-ar.
 * This provides incremental build support via .tlog file generation.
 * @ingroup oceanandroidextension
 */
public class AndroidLibTask : Task
{
	/// The object files to archive.
	[Required]
	public ITaskItem[] ObjectFiles { get; set; } = Array.Empty<ITaskItem>();

	/// The path to llvm-ar executable.
	[Required]
	public string ArchiverPath { get; set; } = string.Empty;

	/// The output static library file path.
	[Required]
	public string OutputFile { get; set; } = string.Empty;

	/// The archiver flags (default: rcs = replace, create, index).
	public string ArchiverFlags { get; set; } = "rcs";

	/// True, to create a thin archive.
	public bool ThinArchive { get; set; } = false;

	/// Additional archiver options.
	public string AdditionalOptions { get; set; } = string.Empty;

	/// The directory for .tlog files.
	public string TLogDirectory { get; set; } = string.Empty;

	/// Alternative name for TLog directory.
	public string TrackerLogDirectory { get; set; } = string.Empty;

	/// The path to the project file.
	public string ProjectFile { get; set; } = string.Empty;

	/// The tool executable name.
	public string ToolExe { get; set; } = "llvm-ar";

	/// The tool path.
	public string ToolPath { get; set; } = string.Empty;

	/// Output: The path to the output library.
	[Output]
	public string OutputLibrary { get; set; } = string.Empty;

	/**
	 * Executes the Android lib task.
	 * @return True, if the archive creation succeeded
	 */
	public override bool Execute()
	{
		try
		{
			if (!File.Exists(ArchiverPath))
			{
				Log.LogError($"Archiver not found: {ArchiverPath}");
				return false;
			}

			if (ObjectFiles == null || ObjectFiles.Length == 0)
			{
				Log.LogError("No object files to archive.");
				return false;
			}

			var outputDirectory = Path.GetDirectoryName(OutputFile);
			if (!string.IsNullOrEmpty(outputDirectory) && !Directory.Exists(outputDirectory))
			{
				Directory.CreateDirectory(outputDirectory);
			}

			var tlogDirectory = !string.IsNullOrEmpty(TLogDirectory) ? TLogDirectory : !string.IsNullOrEmpty(TrackerLogDirectory) ? TrackerLogDirectory : outputDirectory ?? ".";

			if (!Directory.Exists(tlogDirectory))
			{
				Directory.CreateDirectory(tlogDirectory);
			}

			Log.LogMessage(MessageImportance.Normal, $"Creating static library {Path.GetFileName(OutputFile)} ({ObjectFiles.Length} object file(s))");

			if (File.Exists(OutputFile))
			{
				File.Delete(OutputFile);
			}

			var arguments = BuildArchiverArguments();

			Log.LogMessage(MessageImportance.Low, $"{ArchiverPath} {arguments}");

			if (!RunArchiver(arguments, outputDirectory ?? "."))
			{
				Log.LogError("Static library creation failed. See errors above.");
				return false;
			}

			if (!File.Exists(OutputFile))
			{
				Log.LogError($"Archive was not created: {OutputFile}");
				return false;
			}

			OutputLibrary = OutputFile;

			WriteTlogFiles(tlogDirectory);

			var projectName = !string.IsNullOrEmpty(ProjectFile) ? Path.GetFileName(ProjectFile) : Path.GetFileNameWithoutExtension(OutputFile).Replace("lib", "") + ".vcxproj";
			Log.LogMessage(MessageImportance.High, $"{projectName} -> {OutputFile}");

			var fileInfo = new FileInfo(OutputFile);
			Log.LogMessage(MessageImportance.Normal, $"  Size: {FormatFileSize(fileInfo.Length)}");

			return true;
		}
		catch (Exception exception)
		{
			Log.LogErrorFromException(exception, showStackTrace: true);
			return false;
		}
	}

	/**
	 * Builds the archiver arguments.
	 * @return The archiver arguments
	 */
	private string BuildArchiverArguments()
	{
		var arguments = new StringBuilder();

		var flags = ArchiverFlags;
		if (ThinArchive && !flags.Contains('T'))
		{
			flags = "T" + flags;
		}
		arguments.Append($"{flags} ");

		if (!string.IsNullOrEmpty(AdditionalOptions))
		{
			arguments.Append($"{AdditionalOptions} ");
		}

		arguments.Append($"\"{OutputFile}\" ");

		foreach (var obj in ObjectFiles)
		{
			var objPath = obj.GetMetadata("FullPath");
			if (string.IsNullOrEmpty(objPath))
			{
				objPath = obj.ItemSpec;
			}

			arguments.Append($"\"{objPath}\" ");
		}

		return arguments.ToString();
	}

	/**
	 * Runs the archiver.
	 * @param arguments The archiver arguments
	 * @param workingDirectory The working directory
	 * @return True, if the archiver succeeded
	 */
	private bool RunArchiver(string arguments, string workingDirectory)
	{
		var startInfo = new ProcessStartInfo
		{
			FileName = ArchiverPath,
			Arguments = arguments,
			WorkingDirectory = workingDirectory,
			UseShellExecute = false,
			RedirectStandardOutput = true,
			RedirectStandardError = true,
			CreateNoWindow = true,
			StandardErrorEncoding = Encoding.UTF8,
			StandardOutputEncoding = Encoding.UTF8
		};

		using var process = new Process { StartInfo = startInfo };

		var hasErrors = false;

		process.OutputDataReceived += (sender, eventArgs) =>
		{
			if (!string.IsNullOrEmpty(eventArgs.Data))
			{
				Log.LogMessage(MessageImportance.Low, eventArgs.Data);
			}
		};

		process.ErrorDataReceived += (sender, eventArgs) =>
		{
			if (!string.IsNullOrEmpty(eventArgs.Data))
			{
				if (eventArgs.Data.Contains("error:") || eventArgs.Data.Contains("Error:"))
				{
					Log.LogError(eventArgs.Data);
					hasErrors = true;
				}
				else if (eventArgs.Data.Contains("warning:") || eventArgs.Data.Contains("Warning:"))
				{
					Log.LogWarning(eventArgs.Data);
				}
				else
				{
					Log.LogMessage(MessageImportance.Low, eventArgs.Data);
				}
			}
		};

		process.Start();
		process.BeginOutputReadLine();
		process.BeginErrorReadLine();
		process.WaitForExit();

		return process.ExitCode == 0 && !hasErrors;
	}

	/**
	 * Writes the .tlog files for incremental build tracking.
	 * @param tlogDirectory The .tlog directory
	 */
	private void WriteTlogFiles(string tlogDirectory)
	{
		try
		{
			var readEntries = new List<string>();
			var writeEntries = new List<string>();

			readEntries.Add($"^{OutputFile.ToUpperInvariant()}");
			foreach (var obj in ObjectFiles)
			{
				var objPath = obj.GetMetadata("FullPath");
				if (!string.IsNullOrEmpty(objPath))
				{
					readEntries.Add(objPath.ToUpperInvariant());
				}
			}

			writeEntries.Add($"^{ObjectFiles.First().GetMetadata("FullPath")?.ToUpperInvariant()}");
			writeEntries.Add(OutputFile.ToUpperInvariant());

			var readTlog = Path.Combine(tlogDirectory, "AndroidLib.read.1.tlog");
			File.WriteAllLines(readTlog, readEntries);

			var writeTlog = Path.Combine(tlogDirectory, "AndroidLib.write.1.tlog");
			File.WriteAllLines(writeTlog, writeEntries);

			var cmdTlog = Path.Combine(tlogDirectory, "AndroidLib.command.1.tlog");
			var cmdEntries = new List<string>
			{
				$"^{OutputFile.ToUpperInvariant()}",
				$"{ArchiverPath} {ArchiverFlags}"
			};
			File.WriteAllLines(cmdTlog, cmdEntries);
		}
		catch (Exception exception)
		{
			Log.LogWarning($"Failed to write lib .tlog files: {exception.Message}");
		}
	}

	/**
	 * Formats a file size for display.
	 * @param bytes The file size in bytes
	 * @return The formatted file size
	 */
	private static string FormatFileSize(long bytes)
	{
		string[] sizes = { "B", "KB", "MB", "GB" };
		int order = 0;
		double size = bytes;

		while (size >= 1024 && order < sizes.Length - 1)
		{
			order++;
			size /= 1024;
		}

		return $"{size:0.##} {sizes[order]}";
	}
}

}
