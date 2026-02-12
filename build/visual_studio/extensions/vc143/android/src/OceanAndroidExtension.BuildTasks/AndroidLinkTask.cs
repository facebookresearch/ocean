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
using System.Text.RegularExpressions;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace OceanAndroidExtension.BuildTasks
{

/**
 * This class implements an MSBuild task that links object files into an Android shared library (.so) using clang++.
 * This provides incremental build support via .tlog file generation.
 * @ingroup oceanandroidextension
 */
public class AndroidLinkTask : Task
{
	/// The regex pattern for parsing linker errors.
	private static readonly Regex ErrorRegex = new Regex(@"^(?<file>.+?):(?<line>\d+)?:?\s*(?<type>error|warning|undefined reference):\s*(?<msg>.+)$", RegexOptions.Compiled);

	/// The object files to link.
	[Required]
	public ITaskItem[] ObjectFiles { get; set; } = Array.Empty<ITaskItem>();

	/// The path to the clang++ linker.
	[Required]
	public string LinkerPath { get; set; } = string.Empty;

	/// The path to the NDK sysroot.
	[Required]
	public string Sysroot { get; set; } = string.Empty;

	/// The target triple.
	[Required]
	public string TargetTriple { get; set; } = string.Empty;

	/// The target ABI.
	[Required]
	public string AndroidAbi { get; set; } = string.Empty;

	/// The output file path.
	[Required]
	public string OutputFile { get; set; } = string.Empty;

	/// The C++ STL to link against.
	public string CppStl { get; set; } = "c++_shared";

	/// The architecture-specific linker flags.
	public string ArchitectureFlags { get; set; } = string.Empty;

	/// Additional library directories (semicolon-separated).
	public string AdditionalLibraryDirectories { get; set; } = string.Empty;

	/// Additional libraries to link (semicolon-separated).
	public string AdditionalDependencies { get; set; } = string.Empty;

	/// True, to strip debug symbols from output.
	public bool StripSymbols { get; set; } = false;

	/// The path to llvm-strip tool.
	public string StripToolPath { get; set; } = string.Empty;

	/// True, to generate a map file.
	public bool GenerateMapFile { get; set; } = false;

	/// The map file name.
	public string MapFileName { get; set; } = string.Empty;

	/// True, to enable link-time optimization.
	public bool LinkTimeOptimization { get; set; } = false;

	/// True, to remove unreferenced sections.
	public bool GarbageCollectSections { get; set; } = true;

	/// The shared object name (soname).
	public string SoName { get; set; } = string.Empty;

	/// The version script file for symbol visibility.
	public string VersionScript { get; set; } = string.Empty;

	/// Additional linker options.
	public string AdditionalOptions { get; set; } = string.Empty;

	/// The directory for .tlog files.
	public string TLogDirectory { get; set; } = string.Empty;

	/// Alternative name for TLog directory.
	public string TrackerLogDirectory { get; set; } = string.Empty;

	/// The path to the project file.
	public string ProjectFile { get; set; } = string.Empty;

	/// The tool executable name.
	public string ToolExe { get; set; } = "clang++";

	/// The tool path.
	public string ToolPath { get; set; } = string.Empty;

	/// Output: The path to the output library.
	[Output]
	public string OutputLibrary { get; set; } = string.Empty;

	/**
	 * Executes the Android link task.
	 * @return True, if the linking succeeded
	 */
	public override bool Execute()
	{
		try
		{
			if (!File.Exists(LinkerPath))
			{
				Log.LogError($"Linker not found: {LinkerPath}");
				return false;
			}

			if (ObjectFiles == null || ObjectFiles.Length == 0)
			{
				Log.LogError("No object files to link.");
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

			Log.LogMessage(MessageImportance.Normal, $"Linking {Path.GetFileName(OutputFile)} for {AndroidAbi} ({ObjectFiles.Length} object file(s))");

			var arguments = BuildLinkerArguments();

			Log.LogMessage(MessageImportance.Low, $"{LinkerPath} {arguments}");

			if (!RunLinker(arguments, outputDirectory ?? "."))
			{
				Log.LogError("Linking failed. See errors above.");
				return false;
			}

			if (StripSymbols && File.Exists(OutputFile))
			{
				StripOutputSymbols();
			}

			OutputLibrary = OutputFile;

			WriteTlogFiles(tlogDirectory);

			var projectName = !string.IsNullOrEmpty(ProjectFile) ? Path.GetFileName(ProjectFile) : Path.GetFileNameWithoutExtension(OutputFile) + ".vcxproj";
			Log.LogMessage(MessageImportance.High, $"{projectName} -> {OutputFile}");

			return true;
		}
		catch (Exception exception)
		{
			Log.LogErrorFromException(exception, showStackTrace: true);
			return false;
		}
	}

	/**
	 * Builds the linker arguments.
	 * @return The linker arguments
	 */
	private string BuildLinkerArguments()
	{
		var arguments = new StringBuilder();

		arguments.Append($"-target {TargetTriple} ");
		arguments.Append($"--sysroot=\"{Sysroot}\" ");

		arguments.Append("-shared ");

		if (!string.IsNullOrEmpty(ArchitectureFlags))
		{
			arguments.Append($"{ArchitectureFlags} ");
		}

		if (LinkTimeOptimization)
		{
			arguments.Append("-flto ");
		}

		if (GarbageCollectSections)
		{
			arguments.Append("-Wl,--gc-sections ");
		}

		if (!string.IsNullOrEmpty(SoName))
		{
			arguments.Append($"-Wl,-soname,{SoName} ");
		}
		else
		{
			var defaultSoName = Path.GetFileName(OutputFile);
			arguments.Append($"-Wl,-soname,{defaultSoName} ");
		}

		if (!string.IsNullOrEmpty(VersionScript) && File.Exists(VersionScript))
		{
			arguments.Append($"-Wl,--version-script=\"{VersionScript}\" ");
		}

		if (GenerateMapFile)
		{
			var mapFile = !string.IsNullOrEmpty(MapFileName) ? MapFileName : Path.ChangeExtension(OutputFile, ".map");
			arguments.Append($"-Wl,-Map,\"{mapFile}\" ");
		}

		if (!string.IsNullOrEmpty(AdditionalLibraryDirectories))
		{
			foreach (var directory in SplitSemicolon(AdditionalLibraryDirectories))
			{
				arguments.Append($"-L\"{directory}\" ");
			}
		}

		var stlLibraryDirectory = GetStlLibraryPath();
		if (!string.IsNullOrEmpty(stlLibraryDirectory) && Directory.Exists(stlLibraryDirectory))
		{
			arguments.Append($"-L\"{stlLibraryDirectory}\" ");
		}

		foreach (var obj in ObjectFiles)
		{
			var objPath = obj.GetMetadata("FullPath");
			if (string.IsNullOrEmpty(objPath))
			{
				objPath = obj.ItemSpec;
			}

			arguments.Append($"\"{objPath}\" ");
		}

		arguments.Append($"-o \"{OutputFile}\" ");

		switch (CppStl?.ToLowerInvariant())
		{
			case "c++_shared":
				arguments.Append("-lc++_shared ");
				break;
			case "c++_static":
				arguments.Append("-lc++_static -lc++abi ");
				break;
			case "system":
				arguments.Append("-lstdc++ ");
				break;
			case "none":
				break;
			default:
				arguments.Append("-lc++_shared ");
				break;
		}

		arguments.Append("-llog ");
		arguments.Append("-landroid ");
		arguments.Append("-lm ");
		arguments.Append("-ldl ");

		if (!string.IsNullOrEmpty(AdditionalDependencies))
		{
			foreach (var lib in SplitSemicolon(AdditionalDependencies))
			{
				if (lib.StartsWith("-l") || lib.Contains(Path.DirectorySeparatorChar) || lib.Contains('/'))
				{
					arguments.Append($"{lib} ");
				}
				else
				{
					arguments.Append($"-l{lib} ");
				}
			}
		}

		if (!string.IsNullOrEmpty(AdditionalOptions))
		{
			arguments.Append($"{AdditionalOptions} ");
		}

		return arguments.ToString();
	}

	/**
	 * Returns the STL library path.
	 * @return The STL library path
	 */
	private string GetStlLibraryPath()
	{
		var targetDirectory = AndroidAbi switch
		{
			"arm64-v8a" => "aarch64-linux-android",
			"armeabi-v7a" => "arm-linux-androideabi",
			"x86_64" => "x86_64-linux-android",
			"x86" => "i686-linux-android",
			_ => ""
		};

		if (string.IsNullOrEmpty(targetDirectory))
		{
			return string.Empty;
		}

		return Path.Combine(Sysroot, "usr", "lib", targetDirectory);
	}

	/**
	 * Splits a semicolon-separated string.
	 * @param value The string to split
	 * @return The individual values
	 */
	private IEnumerable<string> SplitSemicolon(string value)
	{
		if (string.IsNullOrEmpty(value))
		{
			yield break;
		}

		foreach (var item in value.Split(new[] { ';' }, StringSplitOptions.RemoveEmptyEntries))
		{
			var trimmed = item.Trim();
			if (!string.IsNullOrEmpty(trimmed))
			{
				yield return trimmed;
			}
		}
	}

	/**
	 * Runs the linker.
	 * @param arguments The linker arguments
	 * @param workingDirectory The working directory
	 * @return True, if the linker succeeded
	 */
	private bool RunLinker(string arguments, string workingDirectory)
	{
		var startInfo = new ProcessStartInfo
		{
			FileName = LinkerPath,
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
				if (ParseAndLogOutput(eventArgs.Data))
				{
					hasErrors = true;
				}
			}
		};

		process.ErrorDataReceived += (sender, eventArgs) =>
		{
			if (!string.IsNullOrEmpty(eventArgs.Data))
			{
				if (ParseAndLogOutput(eventArgs.Data))
				{
					hasErrors = true;
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
	 * Parses and logs linker output.
	 * @param line The output line
	 * @return True, if the line contains an error
	 */
	private bool ParseAndLogOutput(string line)
	{
		bool isError = false;

		if (line.Contains("undefined reference"))
		{
			Log.LogError(line);
			return true;
		}

		var match = ErrorRegex.Match(line);
		if (match.Success)
		{
			var file = match.Groups["file"].Value;
			var lineStr = match.Groups["line"].Value;
			var lineNum = string.IsNullOrEmpty(lineStr) ? 0 : int.Parse(lineStr);
			var type = match.Groups["type"].Value;
			var message = match.Groups["msg"].Value;

			switch (type.ToLowerInvariant())
			{
				case "error":
				case "undefined reference":
					Log.LogError(null, null, null, file, lineNum, 0, 0, 0, message);
					isError = true;
					break;
				case "warning":
					Log.LogWarning(null, null, null, file, lineNum, 0, 0, 0, message);
					break;
			}
		}
		else if (line.Contains("error:"))
		{
			Log.LogError(line);
			isError = true;
		}
		else if (line.Contains("warning:"))
		{
			Log.LogWarning(line);
		}
		else
		{
			Log.LogMessage(MessageImportance.Low, line);
		}

		return isError;
	}

	/**
	 * Strips debug symbols from the output.
	 */
	private void StripOutputSymbols()
	{
		if (string.IsNullOrEmpty(StripToolPath) || !File.Exists(StripToolPath))
		{
			Log.LogWarning("Strip tool not found, skipping symbol stripping.");
			return;
		}

		Log.LogMessage(MessageImportance.Normal, $"Stripping symbols from {Path.GetFileName(OutputFile)}...");

		var startInfo = new ProcessStartInfo
		{
			FileName = StripToolPath,
			Arguments = $"--strip-unneeded \"{OutputFile}\"",
			UseShellExecute = false,
			RedirectStandardOutput = true,
			RedirectStandardError = true,
			CreateNoWindow = true
		};

		using var process = new Process { StartInfo = startInfo };
		process.Start();
		process.WaitForExit();

		if (process.ExitCode != 0)
		{
			Log.LogWarning($"Symbol stripping failed with exit code {process.ExitCode}");
		}
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

			var readTlog = Path.Combine(tlogDirectory, "AndroidLink.read.1.tlog");
			File.WriteAllLines(readTlog, readEntries);

			var writeTlog = Path.Combine(tlogDirectory, "AndroidLink.write.1.tlog");
			File.WriteAllLines(writeTlog, writeEntries);

			var cmdTlog = Path.Combine(tlogDirectory, "AndroidLink.command.1.tlog");
			var cmdEntries = new List<string>
			{
				$"^{OutputFile.ToUpperInvariant()}",
				$"{LinkerPath} -target {TargetTriple} -shared"
			};
			File.WriteAllLines(cmdTlog, cmdEntries);
		}
		catch (Exception exception)
		{
			Log.LogWarning($"Failed to write link .tlog files: {exception.Message}");
		}
	}
}

}
