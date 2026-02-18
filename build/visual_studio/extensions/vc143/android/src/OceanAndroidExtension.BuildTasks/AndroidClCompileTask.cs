/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace OceanAndroidExtension.BuildTasks
{

/**
 * This class implements an MSBuild task that compiles C/C++ source files using the Android NDK clang compiler.
 * Provides full incremental build support via .tlog file generation for Visual Studio integration.
 * @ingroup oceanandroidextension
 */
public class AndroidClCompileTask : CancelableTask
{
	/// The regex for parsing compiler error/warning output.
	private static readonly Regex ErrorRegex = new Regex(
		@"^(?<file>.+?):(?<line>\d+):(?<col>\d+):\s*(?<type>error|warning|note|fatal error):\s*(?<msg>.+)$",
		RegexOptions.Compiled);

	/// The regex for parsing dependency files.
	private static readonly Regex DepFileRegex = new Regex(
		@"^\s*(.+?):\s*\\?$|^\s+(.+?)\s*\\?$",
		RegexOptions.Compiled | RegexOptions.Multiline);

	/// Source files to compile.
	[Required]
	public ITaskItem[] Sources { get; set; } = Array.Empty<ITaskItem>();

	/// The path to the clang++ executable.
	[Required]
	public string CompilerPath { get; set; } = string.Empty;

	/// The path to the NDK sysroot.
	[Required]
	public string Sysroot { get; set; } = string.Empty;

	/// The target triple (e.g., aarch64-linux-android24).
	[Required]
	public string TargetTriple { get; set; } = string.Empty;

	/// The target ABI (e.g., arm64-v8a).
	[Required]
	public string AndroidAbi { get; set; } = string.Empty;

	/// The Android API level.
	public int ApiLevel { get; set; } = 24;

	/// The C++ language standard.
	public string CppStandard { get; set; } = "c++17";

	/// The C language standard.
	public string CStandard { get; set; } = "c11";

	/// The optimization level (Disabled, MinSpace, MaxSpeed, Full, etc.).
	public string Optimization { get; set; } = "Disabled";

	/// The debug information format.
	public string DebugInformationFormat { get; set; } = "FullDebug";

	/// The warning level.
	public string WarningLevel { get; set; } = "Level3";

	/// True, to treat warnings as errors.
	public bool TreatWarningAsError { get; set; } = false;

	/// Additional include directories (semicolon-separated).
	public string AdditionalIncludeDirectories { get; set; } = string.Empty;

	/// Preprocessor definitions (semicolon-separated).
	public string PreprocessorDefinitions { get; set; } = string.Empty;

	/// Preprocessor definitions to undefine.
	public string UndefinePreprocessorDefinitions { get; set; } = string.Empty;

	/// True, to enable RTTI.
	public bool RuntimeTypeInfo { get; set; } = true;

	/// True, to enable exceptions.
	public bool ExceptionHandling { get; set; } = true;

	/// True, to generate position-independent code.
	public bool PositionIndependentCode { get; set; } = true;

	/// True, to omit frame pointers.
	public bool OmitFramePointers { get; set; } = false;

	/// Architecture-specific flags.
	public string ArchitectureFlags { get; set; } = string.Empty;

	/// True, to enable Thumb mode for ARM.
	public bool ThumbMode { get; set; } = false;

	/// Additional compiler options.
	public string AdditionalOptions { get; set; } = string.Empty;

	/// Object file output path pattern (used when a single pre-resolved path is passed).
	public string ObjectFileName { get; set; } = string.Empty;

	/// Object file output directory. When set, object paths are computed per-item
	/// as ObjectFileDir + RelativeDir + Filename + ".o", avoiding MSBuild task batching.
	public string ObjectFileDir { get; set; } = string.Empty;

	/// Directory for .tlog files.
	public string TLogDirectory { get; set; } = string.Empty;

	/// Alternative name for TLog directory (compatibility).
	public string TrackerLogDirectory { get; set; } = string.Empty;

	/// The tool executable name.
	public string ToolExe { get; set; } = "clang++";

	/// The tool path.
	public string ToolPath { get; set; } = string.Empty;

	/// Maximum number of parallel compilation jobs (default: number of processors).
	public int MaxParallelJobs { get; set; } = Environment.ProcessorCount;

	/// Compiled object files.
	[Output]
	public ITaskItem[] ObjectFiles { get; set; } = Array.Empty<ITaskItem>();

	/// The error count.
	private int errorCount_ = 0;

	/// The warning count.
	private int warningCount_ = 0;

	/**
	 * Returns a string metadata value from the item, falling back to a default.
	 * @param item The task item
	 * @param name The metadata name
	 * @param fallback The fallback value
	 * @return The metadata value or fallback
	 */
	private static string GetItemMetadata(ITaskItem item, string name, string fallback)
	{
		var value = item.GetMetadata(name);
		return !string.IsNullOrEmpty(value) ? value : fallback;
	}

	/**
	 * Returns a boolean metadata value from the item, falling back to a default.
	 * @param item The task item
	 * @param name The metadata name
	 * @param fallback The fallback value
	 * @return The metadata value or fallback
	 */
	private static bool GetBoolItemMetadata(ITaskItem item, string name, bool fallback)
	{
		var value = item.GetMetadata(name);
		if (string.IsNullOrEmpty(value)) return fallback;
		return bool.TryParse(value, out var result) ? result : fallback;
	}

	/**
	 * Executes the compile task.
	 * @return True, if the compilation succeeded
	 */
	public override bool Execute()
	{
		try
		{
			if (!File.Exists(CompilerPath))
			{
				Log.LogError($"Clang compiler not found: {CompilerPath}");
				return false;
			}

			var tlogDirectory = !string.IsNullOrEmpty(TLogDirectory) ? TLogDirectory :
			              !string.IsNullOrEmpty(TrackerLogDirectory) ? TrackerLogDirectory :
			              Path.GetDirectoryName(Sources.First().GetMetadata("FullPath")) ?? ".";

			if (!Directory.Exists(tlogDirectory))
			{
				Directory.CreateDirectory(tlogDirectory);
			}

			var objectFiles = new ConcurrentBag<ITaskItem>();
			int failureCount = 0;

			var parallelOptions = new System.Threading.Tasks.ParallelOptions
			{
				MaxDegreeOfParallelism = MaxParallelJobs > 0 ? MaxParallelJobs : Environment.ProcessorCount,
				CancellationToken = CancellationToken
			};

			try
			{
				System.Threading.Tasks.Parallel.ForEach(Sources, parallelOptions, source =>
				{
					if (IsCancelled)
					{
						return;
					}

					var sourcePath = source.GetMetadata("FullPath");
					if (string.IsNullOrEmpty(sourcePath))
					{
						sourcePath = source.ItemSpec;
					}

					string objPath;
					if (!string.IsNullOrEmpty(ObjectFileDir))
					{
						var relativeDir = source.GetMetadata("RelativeDir") ?? "";
						// Strip ".." segments so the path stays within IntDir
						relativeDir = string.Join(
							Path.DirectorySeparatorChar.ToString(),
							relativeDir.Split(new[] { Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar }, StringSplitOptions.RemoveEmptyEntries)
								.Where(p => p != ".."));
						var filename = source.GetMetadata("Filename");
						if (string.IsNullOrEmpty(filename))
						{
							filename = Path.GetFileNameWithoutExtension(sourcePath);
						}
						objPath = Path.Combine(ObjectFileDir, relativeDir, filename + ".o");
					}
					else if (!string.IsNullOrEmpty(ObjectFileName))
					{
						objPath = ObjectFileName;
					}
					else
					{
						objPath = Path.Combine(tlogDirectory, Path.GetFileNameWithoutExtension(sourcePath) + ".o");
					}

					var objectDirectory = Path.GetDirectoryName(objPath);
					if (!string.IsNullOrEmpty(objectDirectory) && !Directory.Exists(objectDirectory))
					{
						Directory.CreateDirectory(objectDirectory);
					}

					if (IsUpToDate(sourcePath, objPath, tlogDirectory))
					{
						Log.LogMessage(MessageImportance.Normal, $"Skipping {Path.GetFileName(sourcePath)} (up to date)");
						var item = new TaskItem(objPath);
						item.SetMetadata("SourceFile", sourcePath);
						objectFiles.Add(item);
						return;
					}

					Log.LogMessage(MessageImportance.High, Path.GetFileName(sourcePath));

					if (IsCancelled)
					{
						return;
					}

					if (CompileSource(source, sourcePath, objPath, tlogDirectory))
					{
						var item = new TaskItem(objPath);
						item.SetMetadata("SourceFile", sourcePath);
						item.SetMetadata("AndroidAbi", AndroidAbi);
						objectFiles.Add(item);
					}
					else
					{
						Interlocked.Increment(ref failureCount);
					}
				});
			}
			catch (OperationCanceledException)
			{
				Log.LogMessage(MessageImportance.High, "Build cancelled.");
				return false;
			}

			if (IsCancelled)
			{
				Log.LogMessage(MessageImportance.High, "Build cancelled.");
				return false;
			}

			ObjectFiles = objectFiles.ToArray();

			WriteTlogFiles(tlogDirectory);

			bool allSucceeded = failureCount == 0;

			if (!allSucceeded)
			{
				Log.LogError($"Compilation failed with {errorCount_} error(s) and {warningCount_} warning(s).");
			}
			else if (warningCount_ > 0)
			{
				Log.LogMessage(MessageImportance.High, $"Compilation succeeded with {warningCount_} warning(s).");
			}

			return allSucceeded;
		}
		catch (Exception exception)
		{
			Log.LogErrorFromException(exception, showStackTrace: true);
			return false;
		}
	}

	/**
	 * Checks if the object file is up to date.
	 * @param sourcePath The source file path
	 * @param objPath The object file path
	 * @param tlogDirectory The .tlog directory
	 * @return True, if the object file is up to date
	 */
	private bool IsUpToDate(string sourcePath, string objPath, string tlogDirectory)
	{
		if (!File.Exists(objPath))
		{
			return false;
		}

		var sourceTime = File.GetLastWriteTimeUtc(sourcePath);
		var objTime = File.GetLastWriteTimeUtc(objPath);

		if (sourceTime > objTime)
		{
			return false;
		}

		// Look for the .d file next to the .o file (same directory structure)
		var depFile = Path.ChangeExtension(objPath, ".d");
		if (File.Exists(depFile))
		{
			try
			{
				var deps = ParseDependencyFile(depFile);
				foreach (var dep in deps)
				{
					if (File.Exists(dep) && File.GetLastWriteTimeUtc(dep) > objTime)
					{
						return false;
					}
				}
			}
			catch
			{
				return false;
			}
		}

		return true;
	}

	/**
	 * Parses a Makefile-style dependency file.
	 * @param depFile The dependency file path
	 * @return The list of dependencies
	 */
	private List<string> ParseDependencyFile(string depFile)
	{
		var deps = new List<string>();
		var content = File.ReadAllText(depFile);

		var lines = content.Split(new[] { '\n', '\r' }, StringSplitOptions.RemoveEmptyEntries);
		bool firstLine = true;

		foreach (var line in lines)
		{
			var trimmed = line.Trim();
			if (string.IsNullOrEmpty(trimmed))
			{
				continue;
			}

			if (trimmed.EndsWith("\\"))
			{
				trimmed = trimmed.Substring(0, trimmed.Length - 1).TrimEnd();
			}

			if (firstLine)
			{
				var colonIndex = trimmed.IndexOf(':');
				if (colonIndex >= 0)
				{
					trimmed = trimmed.Substring(colonIndex + 1).Trim();
				}
				firstLine = false;
			}

			var parts = trimmed.Split(new[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
			foreach (var part in parts)
			{
				var dep = part.Replace("\\ ", " ").Trim();
				if (!string.IsNullOrEmpty(dep))
				{
					deps.Add(dep);
				}
			}
		}

		return deps;
	}

	/**
	 * Compiles a single source file.
	 * Per-file settings are read from the ITaskItem metadata (populated by MSBuild
	 * ItemDefinitionGroup defaults and per-file overrides). Task-level properties
	 * are used as fallbacks.
	 * @param source The source task item with metadata
	 * @param sourcePath The resolved source file path
	 * @param objPath The output object file path
	 * @param tlogDirectory The .tlog directory
	 * @return True, if the compilation succeeded
	 */
	private bool CompileSource(ITaskItem source, string sourcePath, string objPath, string tlogDirectory)
	{
		// Read per-item metadata, falling back to task-level properties
		var optimization = GetItemMetadata(source, "Optimization", Optimization);
		var debugInfoFormat = GetItemMetadata(source, "DebugInformationFormat", DebugInformationFormat);
		var warningLevel = GetItemMetadata(source, "WarningLevel", WarningLevel);
		var treatWarningAsError = GetBoolItemMetadata(source, "TreatWarningAsError", TreatWarningAsError);
		var cppStandard = GetItemMetadata(source, "LanguageStandard", CppStandard);
		var cStandard = GetItemMetadata(source, "LanguageStandard_C", CStandard);
		var additionalIncludes = GetItemMetadata(source, "AdditionalIncludeDirectories", AdditionalIncludeDirectories);
		var preprocessorDefs = GetItemMetadata(source, "PreprocessorDefinitions", PreprocessorDefinitions);
		var undefPreprocessorDefs = GetItemMetadata(source, "UndefinePreprocessorDefinitions", UndefinePreprocessorDefinitions);
		var rtti = GetBoolItemMetadata(source, "RuntimeTypeInfo", RuntimeTypeInfo);
		var exceptions = GetBoolItemMetadata(source, "ExceptionHandling", ExceptionHandling);
		var pic = GetBoolItemMetadata(source, "PositionIndependentCode", PositionIndependentCode);
		var omitFramePointers = GetBoolItemMetadata(source, "OmitFramePointers", OmitFramePointers);
		var thumbMode = GetBoolItemMetadata(source, "ThumbMode", ThumbMode);
		var additionalOptions = GetItemMetadata(source, "AdditionalOptions", AdditionalOptions);

		var arguments = new StringBuilder();

		arguments.Append($"-target {TargetTriple} ");
		arguments.Append($"--sysroot=\"{Sysroot}\" ");

		if (!string.IsNullOrEmpty(ArchitectureFlags))
		{
			arguments.Append($"{ArchitectureFlags} ");
		}

		if (thumbMode && AndroidAbi == "armeabi-v7a")
		{
			arguments.Append("-mthumb ");
		}

		var extension = Path.GetExtension(sourcePath).ToLowerInvariant();
		if (extension == ".c")
		{
			var cStd = string.IsNullOrEmpty(cStandard) ? "c11" : cStandard;
			arguments.Append($"-std={cStd} ");
		}
		else
		{
			var cppStd = string.IsNullOrEmpty(cppStandard) ? "c++17" : cppStandard;
			arguments.Append($"-std={cppStd} ");
		}

		if (pic)
		{
			arguments.Append("-fPIC ");
		}

		arguments.Append(GetOptimizationFlag(optimization));
		arguments.Append(GetDebugInfoFlag(debugInfoFormat));
		arguments.Append(GetWarningFlags(warningLevel, treatWarningAsError));

		if (!rtti)
		{
			arguments.Append("-fno-rtti ");
		}

		if (!exceptions)
		{
			arguments.Append("-fno-exceptions ");
		}

		if (omitFramePointers)
		{
			arguments.Append("-fomit-frame-pointer ");
		}

		arguments.Append("-DANDROID ");

		if (!string.IsNullOrEmpty(preprocessorDefs))
		{
			foreach (var def in SplitSemicolon(preprocessorDefs))
			{
				arguments.Append($"-D{def} ");
			}
		}

		if (!string.IsNullOrEmpty(undefPreprocessorDefs))
		{
			foreach (var undef in SplitSemicolon(undefPreprocessorDefs))
			{
				arguments.Append($"-U{undef} ");
			}
		}

		if (!string.IsNullOrEmpty(additionalIncludes))
		{
			foreach (var directory in SplitSemicolon(additionalIncludes))
			{
				arguments.Append($"-I\"{directory}\" ");
			}
		}

		if (!string.IsNullOrEmpty(additionalOptions))
		{
			arguments.Append($"{additionalOptions} ");
		}

		// Write .d file next to .o file to avoid collisions between
		// same-named sources in different directories during parallel compilation
		var depFile = Path.ChangeExtension(objPath, ".d");
		arguments.Append($"-MMD -MF \"{depFile}\" ");

		arguments.Append($"-c \"{sourcePath}\" ");
		arguments.Append($"-o \"{objPath}\"");

		Log.LogMessage(MessageImportance.Low, $"{CompilerPath} {arguments}");

		// Delete the output file before compiling so that a killed compiler
		// leaves no file rather than a truncated .o that the timestamp-based
		// up-to-date check would consider valid on the next build.
		if (File.Exists(objPath))
		{
			try { File.Delete(objPath); }
			catch (IOException) { }
		}

		return RunCompiler(arguments.ToString(), Path.GetDirectoryName(sourcePath) ?? ".");
	}

	/**
	 * Returns the optimization flag for clang.
	 * @param optimization The optimization setting
	 * @return The optimization flag
	 */
	private static string GetOptimizationFlag(string optimization)
	{
		return optimization?.ToLowerInvariant() switch
		{
			"disabled" => "-O0 ",
			"minspace" => "-Os ",
			"maxspeed" => "-O2 ",
			"full" => "-O3 ",
			"minsizeaggressive" => "-Oz ",
			"fast" => "-Ofast ",
			_ => "-O0 "
		};
	}

	/**
	 * Returns the debug info flag for clang.
	 * @param debugInfoFormat The debug information format setting
	 * @return The debug info flag
	 */
	private static string GetDebugInfoFlag(string debugInfoFormat)
	{
		return debugInfoFormat?.ToLowerInvariant() switch
		{
			"none" => "",
			"linenumber" => "-gline-tables-only ",
			"fulldebug" => "-g ",
			"fulldebugdwarf4" => "-g -gdwarf-4 ",
			_ => "-g "
		};
	}

	/**
	 * Returns the warning flags for clang.
	 * @param warningLevel The warning level setting
	 * @param treatWarningAsError True to treat warnings as errors
	 * @return The warning flags
	 */
	private static string GetWarningFlags(string warningLevel, bool treatWarningAsError)
	{
		var flags = new StringBuilder();

		switch (warningLevel?.ToLowerInvariant())
		{
			case "turnoffallwarnings":
				flags.Append("-w ");
				break;
			case "level1":
				flags.Append("-Wall ");
				break;
			case "level2":
				flags.Append("-Wall -Wextra ");
				break;
			case "level3":
				flags.Append("-Wall -Wextra -Wpedantic ");
				break;
			case "level4":
				flags.Append("-Weverything ");
				break;
			default:
				flags.Append("-Wall ");
				break;
		}

		if (treatWarningAsError)
		{
			flags.Append("-Werror ");
		}

		return flags.ToString();
	}

	/**
	 * Splits a semicolon-separated string.
	 * @param value The string to split
	 * @return The split values
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
	 * Runs the compiler process.
	 * @param arguments The command line arguments
	 * @param workingDirectory The working directory
	 * @return True, if the compiler exited with code 0
	 */
	private bool RunCompiler(string arguments, string workingDirectory)
	{
		var startInfo = new ProcessStartInfo
		{
			FileName = CompilerPath,
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

		process.OutputDataReceived += (sender, eventArgs) =>
		{
			if (!string.IsNullOrEmpty(eventArgs.Data))
			{
				ParseAndLogOutput(eventArgs.Data);
			}
		};

		process.ErrorDataReceived += (sender, eventArgs) =>
		{
			if (!string.IsNullOrEmpty(eventArgs.Data))
			{
				ParseAndLogOutput(eventArgs.Data);
			}
		};

		process.Start();
		RegisterProcess(process);
		process.BeginOutputReadLine();
		process.BeginErrorReadLine();

		try
		{
			process.WaitForExit();
		}
		finally
		{
			UnregisterProcess(process);
		}

		if (IsCancelled)
		{
			return false;
		}

		return process.ExitCode == 0;
	}

	/**
	 * Parses compiler output and logs errors/warnings.
	 * @param line The output line to parse
	 */
	private void ParseAndLogOutput(string line)
	{
		var match = ErrorRegex.Match(line);
		if (match.Success)
		{
			var file = match.Groups["file"].Value;
			var lineNum = int.Parse(match.Groups["line"].Value);
			var col = int.Parse(match.Groups["col"].Value);
			var type = match.Groups["type"].Value;
			var message = match.Groups["msg"].Value;

			switch (type.ToLowerInvariant())
			{
				case "error":
				case "fatal error":
					Log.LogError(null, null, null, file, lineNum, col, 0, 0, message);
					Interlocked.Increment(ref errorCount_);
					break;
				case "warning":
					Log.LogWarning(null, null, null, file, lineNum, col, 0, 0, message);
					Interlocked.Increment(ref warningCount_);
					break;
				case "note":
					Log.LogMessage(MessageImportance.Normal, $"{file}({lineNum},{col}): note: {message}");
					break;
			}
		}
		else if (line.Contains("error:") || line.Contains("fatal error:"))
		{
			Log.LogError(line);
			Interlocked.Increment(ref errorCount_);
		}
		else if (line.Contains("warning:"))
		{
			Log.LogWarning(line);
			Interlocked.Increment(ref warningCount_);
		}
		else
		{
			Log.LogMessage(MessageImportance.Low, line);
		}
	}

	/**
	 * Writes the .tlog files for incremental build tracking.
	 * Read entries are built from .d dependency files so that header changes
	 * are detected by VS's incremental build system.
	 * @param tlogDirectory The .tlog directory
	 */
	private void WriteTlogFiles(string tlogDirectory)
	{
		try
		{
			// Read tlog: source files and their header dependencies from .d files.
			// Iterate ObjectFiles (which includes both compiled and up-to-date items)
			// to derive .d file paths from .o paths, matching CompileSource/IsUpToDate.
			var readTlog = Path.Combine(tlogDirectory, "AndroidClCompile.read.1.tlog");
			var readEntries = new List<string>();
			foreach (var objFile in ObjectFiles)
			{
				var sourcePath = objFile.GetMetadata("SourceFile");
				if (string.IsNullOrEmpty(sourcePath))
				{
					continue;
				}

				readEntries.Add($"^{sourcePath.ToUpperInvariant()}");

				var depFile = Path.ChangeExtension(objFile.ItemSpec, ".d");
				if (File.Exists(depFile))
				{
					try
					{
						var deps = ParseDependencyFile(depFile);
						foreach (var dep in deps)
						{
							// Resolve to absolute path — VS's tlog checker expects them.
							// Clang writes paths as resolved during compilation; these are
							// typically absolute but may be relative for #include "../x.h".
							var absPath = Path.IsPathRooted(dep) ? dep : Path.GetFullPath(dep);
							readEntries.Add(absPath.ToUpperInvariant());
						}
					}
					catch
					{
						// If we can't parse the .d file, just record the source
					}
				}
			}
			File.WriteAllLines(readTlog, readEntries);

			// Write tlog
			var writeTlog = Path.Combine(tlogDirectory, "AndroidClCompile.write.1.tlog");
			var writeEntries = new List<string>();
			foreach (var objFile in ObjectFiles)
			{
				var sourcePath = objFile.GetMetadata("SourceFile");
				if (!string.IsNullOrEmpty(sourcePath))
				{
					writeEntries.Add($"^{sourcePath.ToUpperInvariant()}");
					writeEntries.Add(objFile.ItemSpec.ToUpperInvariant());
				}
			}
			File.WriteAllLines(writeTlog, writeEntries);

			// Command tlog
			var cmdTlog = Path.Combine(tlogDirectory, "AndroidClCompile.command.1.tlog");
			var cmdEntries = new List<string>();
			foreach (var objFile in ObjectFiles)
			{
				var sourcePath = objFile.GetMetadata("SourceFile");
				if (!string.IsNullOrEmpty(sourcePath))
				{
					cmdEntries.Add($"^{sourcePath.ToUpperInvariant()}");
					cmdEntries.Add($"{CompilerPath} -target {TargetTriple}");
				}
			}
			File.WriteAllLines(cmdTlog, cmdEntries);
		}
		catch (Exception exception)
		{
			Log.LogWarning($"Failed to write .tlog files: {exception.Message}");
		}
	}
}

}
