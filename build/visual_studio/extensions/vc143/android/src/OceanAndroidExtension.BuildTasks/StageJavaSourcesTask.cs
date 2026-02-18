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
using System.Text.RegularExpressions;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace OceanAndroidExtension.BuildTasks
{

/**
 * This class implements an MSBuild task that stages Java source files to a directory structure matching their package declarations.
 * This enables flat Java source layouts while maintaining proper compilation by Gradle.
 * @ingroup oceanandroidextension
 */
public class StageJavaSourcesTask : CancelableTask
{
	/// The regex pattern for extracting package declarations.
	private static readonly Regex PackageRegex = new Regex(@"^\s*package\s+([\w.]+)\s*;", RegexOptions.Multiline | RegexOptions.Compiled);

	/// The Java source files to stage.
	[Required]
	public ITaskItem[] JavaSources { get; set; } = Array.Empty<ITaskItem>();

	/// The staging directory where files will be copied with proper package structure.
	[Required]
	public string StagingDirectory { get; set; } = string.Empty;

	/// Output: The staged file items for consumption by subsequent tasks.
	[Output]
	public ITaskItem[] StagedFiles { get; set; } = Array.Empty<ITaskItem>();

	/// True, to clean orphaned files from the staging directory.
	public bool CleanOrphans { get; set; } = true;

	/**
	 * Executes the stage Java sources task.
	 * @return True, if the staging succeeded
	 */
	public override bool Execute()
	{
		try
		{
			if (JavaSources.Length == 0)
			{
				Log.LogMessage(MessageImportance.Normal, "No Java sources to stage.");
				return true;
			}

			if (!Directory.Exists(StagingDirectory))
			{
				Directory.CreateDirectory(StagingDirectory);
				Log.LogMessage(MessageImportance.Low, $"Created staging directory: {StagingDirectory}");
			}

			var stagedFiles = new List<ITaskItem>();
			var stagedPaths = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
			var hasErrors = false;

			foreach (var source in JavaSources)
			{
				if (IsCancelled)
				{
					Log.LogMessage(MessageImportance.High, "Build cancelled.");
					return false;
				}

				var sourcePath = source.GetMetadata("FullPath");
				if (string.IsNullOrEmpty(sourcePath))
				{
					sourcePath = source.ItemSpec;
				}

				if (!File.Exists(sourcePath))
				{
					Log.LogError($"Java source file not found: {sourcePath}");
					hasErrors = true;
					continue;
				}

				var result = StageFile(sourcePath);
				if (result == null)
				{
					hasErrors = true;
					continue;
				}

				stagedFiles.Add(result);
				stagedPaths.Add(result.GetMetadata("FullPath"));
			}

			if (CleanOrphans && !hasErrors)
			{
				CleanOrphanedFiles(stagedPaths);
			}

			StagedFiles = stagedFiles.ToArray();

			if (!hasErrors)
			{
				Log.LogMessage(MessageImportance.High, $"Staged {stagedFiles.Count} Java source file(s) to {StagingDirectory}");
			}

			return !hasErrors;
		}
		catch (Exception exception)
		{
			Log.LogErrorFromException(exception);
			return false;
		}
	}

	/**
	 * Stages a single Java source file.
	 * @param sourcePath The source file path
	 * @return The staged task item, nullptr on failure
	 */
	private ITaskItem? StageFile(string sourcePath)
	{
		var packageName = ExtractPackage(sourcePath);
		if (packageName == null)
		{
			Log.LogError($"No package declaration found in: {sourcePath}. Java files must have a 'package X.Y.Z;' declaration.");
			return null;
		}

		var fileName = Path.GetFileName(sourcePath);
		var packagePath = packageName.Replace('.', Path.DirectorySeparatorChar);
		var targetDirectory = Path.Combine(StagingDirectory, packagePath);
		var targetPath = Path.Combine(targetDirectory, fileName);

		if (File.Exists(targetPath))
		{
			var sourceTime = File.GetLastWriteTimeUtc(sourcePath);
			var targetTime = File.GetLastWriteTimeUtc(targetPath);

			if (sourceTime <= targetTime)
			{
				Log.LogMessage(MessageImportance.Low, $"Skipping up-to-date: {fileName}");
				return CreateTaskItem(sourcePath, targetPath, packageName);
			}
		}

		if (!Directory.Exists(targetDirectory))
		{
			Directory.CreateDirectory(targetDirectory);
		}

		File.Copy(sourcePath, targetPath, overwrite: true);
		Log.LogMessage(MessageImportance.Normal, $"Staged: {fileName} -> {packagePath}/{fileName}");

		return CreateTaskItem(sourcePath, targetPath, packageName);
	}

	/**
	 * Creates a task item for a staged file.
	 * @param sourcePath The source file path
	 * @param targetPath The target file path
	 * @param packageName The package name
	 * @return The task item
	 */
	private static ITaskItem CreateTaskItem(string sourcePath, string targetPath, string packageName)
	{
		var item = new TaskItem(targetPath);
		item.SetMetadata("OriginalPath", sourcePath);
		item.SetMetadata("Package", packageName);
		item.SetMetadata("FullPath", Path.GetFullPath(targetPath));
		return item;
	}

	/**
	 * Extracts the package name from a Java source file.
	 * @param filePath The file path
	 * @return The package name, nullptr if not found
	 */
	private string? ExtractPackage(string filePath)
	{
		try
		{
			var content = File.ReadAllText(filePath);
			var match = PackageRegex.Match(content);
			return match.Success ? match.Groups[1].Value : null;
		}
		catch (Exception exception)
		{
			Log.LogWarning($"Failed to read file {filePath}: {exception.Message}");
			return null;
		}
	}

	/**
	 * Cleans orphaned files from the staging directory.
	 * @param validPaths The set of valid file paths
	 */
	private void CleanOrphanedFiles(HashSet<string> validPaths)
	{
		if (!Directory.Exists(StagingDirectory))
		{
			return;
		}

		try
		{
			var allStagedFiles = Directory.GetFiles(StagingDirectory, "*.java", SearchOption.AllDirectories);
			foreach (var file in allStagedFiles)
			{
				var fullPath = Path.GetFullPath(file);
				if (!validPaths.Contains(fullPath))
				{
					File.Delete(file);
					Log.LogMessage(MessageImportance.Normal, $"Removed orphaned staged file: {file}");
				}
			}

			CleanEmptyDirectories(StagingDirectory);
		}
		catch (Exception exception)
		{
			Log.LogWarning($"Failed to clean orphaned files: {exception.Message}");
		}
	}

	/**
	 * Recursively cleans empty directories.
	 * @param directory The directory to clean
	 */
	private void CleanEmptyDirectories(string directory)
	{
		foreach (var subdirectory in Directory.GetDirectories(directory))
		{
			CleanEmptyDirectories(subdirectory);

			if (Directory.GetFiles(subdirectory).Length == 0 && Directory.GetDirectories(subdirectory).Length == 0)
			{
				try
				{
					Directory.Delete(subdirectory);
					Log.LogMessage(MessageImportance.Low, $"Removed empty directory: {subdirectory}");
				}
				catch
				{
					// Ignore failures to delete directories
				}
			}
		}
	}
}

}
