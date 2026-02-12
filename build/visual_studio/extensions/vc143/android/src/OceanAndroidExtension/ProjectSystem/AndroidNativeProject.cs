/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.ProjectSystem;
using Microsoft.VisualStudio.ProjectSystem.Build;
using Microsoft.VisualStudio.ProjectSystem.Properties;

namespace OceanAndroidExtension.ProjectSystem
{

/**
 * This class defines project capabilities for Android Native (C++) projects.
 * @ingroup oceanandroidextension
 */
[Export]
[AppliesTo(AndroidNativeCapabilities)]
internal class AndroidNativeProjectCapabilities
{
	/// The capability string for Android Native projects.
	public const string AndroidNativeCapabilities = "AndroidNative";
}

/**
 * This class handles copying native .so libraries to dependent Android projects.
 * @ingroup oceanandroidextension
 */
[Export(typeof(IBuildUpToDateCheckProvider))]
[AppliesTo("AndroidApplication")]
internal class NativeLibraryCopyProvider : IBuildUpToDateCheckProvider
{
	/// The unconfigured project.
	[Import]
	private UnconfiguredProject UnconfiguredProject { get; set; } = null!;

	/// The configured project.
	[Import]
	private ConfiguredProject ConfiguredProject { get; set; } = null!;

	/**
	 * Checks if the build is up to date.
	 * @param buildAction The build action
	 * @param logger The logger for output
	 * @param cancellationToken The cancellation token
	 * @return False, to always let the build proceed and copy native libs
	 */
	public Task<bool> IsUpToDateAsync(BuildAction buildAction, TextWriter logger, CancellationToken cancellationToken = default)
	{
		return Task.FromResult(false);
	}

	/**
	 * Checks if up-to-date checking is enabled.
	 * @param cancellationToken The cancellation token
	 * @return True, always enabled
	 */
	public Task<bool> IsUpToDateCheckEnabledAsync(CancellationToken cancellationToken = default)
	{
		return Task.FromResult(true);
	}
}

/**
 * This class provides output groups for native projects to expose .so files to references.
 * @ingroup oceanandroidextension
 */
[Export(typeof(IOutputGroupsProvider))]
[AppliesTo("AndroidNative")]
internal class NativeOutputGroupsProvider : IOutputGroupsProvider
{
	/// The configured project.
	[Import]
	private ConfiguredProject ConfiguredProject { get; set; } = null!;

	/// The supported Android ABIs.
	private static readonly string[] SupportedAbis = { "arm64-v8a", "armeabi-v7a", "x86", "x86_64" };

	/**
	 * Returns the output groups for the project.
	 * @param cancellationToken The cancellation token
	 * @return The list of output groups
	 */
	public Task<IReadOnlyList<IOutputGroup>> GetOutputGroupsAsync(CancellationToken cancellationToken)
	{
		var groups = new List<IOutputGroup>();

		var projectDirectory = Path.GetDirectoryName(ConfiguredProject.UnconfiguredProject.FullPath);
		if (projectDirectory == null)
		{
			return Task.FromResult<IReadOnlyList<IOutputGroup>>(groups);
		}

		var outputDirectory = Path.Combine(projectDirectory, "build", "outputs");

		foreach (var abi in SupportedAbis)
		{
			var abiOutputDirectory = Path.Combine(outputDirectory, abi);
			if (!Directory.Exists(abiOutputDirectory))
			{
				continue;
			}

			var sharedObjectFiles = Directory.GetFiles(abiOutputDirectory, "*.so");
			foreach (var sharedObjectFile in sharedObjectFiles)
			{
				groups.Add(new NativeOutputGroup(sharedObjectFile, abi));
			}
		}

		return Task.FromResult<IReadOnlyList<IOutputGroup>>(groups);
	}
}

/**
 * This class represents a native output group.
 * @ingroup oceanandroidextension
 */
internal class NativeOutputGroup : IOutputGroup
{
	/// The canonical name of the output group.
	public string CanonicalName => "NativeLibrary";

	/// The display name of the output group.
	public string DisplayName => "Native Library";

	/// The key output path.
	public string KeyOutput { get; }

	/// The target path.
	public string TargetPath { get; }

	/// The Android ABI.
	public string AndroidAbi { get; }

	/**
	 * Creates a new native output group.
	 * @param outputPath The output path
	 * @param abi The Android ABI
	 */
	public NativeOutputGroup(string outputPath, string abi)
	{
		KeyOutput = outputPath;
		TargetPath = outputPath;
		AndroidAbi = abi;
	}

	/**
	 * Returns the output group items.
	 * @param cancellationToken The cancellation token
	 * @return The list of output group items
	 */
	public Task<IReadOnlyList<IOutputGroupItem>> GetOutputGroupItemsAsync(CancellationToken cancellationToken)
	{
		var items = new List<IOutputGroupItem>
		{
			new NativeOutputGroupItem(KeyOutput, AndroidAbi)
		};
		return Task.FromResult<IReadOnlyList<IOutputGroupItem>>(items);
	}
}

/**
 * This class represents a native output group item.
 * @ingroup oceanandroidextension
 */
internal class NativeOutputGroupItem : IOutputGroupItem
{
	/// The target path.
	public string TargetPath { get; }

	/// The Android ABI.
	public string AndroidAbi { get; }

	/**
	 * Creates a new native output group item.
	 * @param targetPath The target path
	 * @param abi The Android ABI
	 */
	public NativeOutputGroupItem(string targetPath, string abi)
	{
		TargetPath = targetPath;
		AndroidAbi = abi;
	}

	/// The item metadata.
	public IReadOnlyDictionary<string, string> Metadata => new Dictionary<string, string>
	{
		["AndroidAbi"] = AndroidAbi,
		["CopyToJniLibs"] = "true"
	};
}

/**
 * This interface defines output groups providers.
 * @ingroup oceanandroidextension
 */
public interface IOutputGroupsProvider
{
	/**
	 * Returns the output groups.
	 * @param cancellationToken The cancellation token
	 * @return The list of output groups
	 */
	Task<IReadOnlyList<IOutputGroup>> GetOutputGroupsAsync(CancellationToken cancellationToken);
}

/**
 * This interface defines an output group.
 * @ingroup oceanandroidextension
 */
public interface IOutputGroup
{
	/// The canonical name.
	string CanonicalName { get; }

	/// The display name.
	string DisplayName { get; }

	/// The key output.
	string KeyOutput { get; }

	/**
	 * Returns the output group items.
	 * @param cancellationToken The cancellation token
	 * @return The list of output group items
	 */
	Task<IReadOnlyList<IOutputGroupItem>> GetOutputGroupItemsAsync(CancellationToken cancellationToken);
}

/**
 * This interface defines an output group item.
 * @ingroup oceanandroidextension
 */
public interface IOutputGroupItem
{
	/// The target path.
	string TargetPath { get; }

	/// The item metadata.
	IReadOnlyDictionary<string, string> Metadata { get; }
}

}
