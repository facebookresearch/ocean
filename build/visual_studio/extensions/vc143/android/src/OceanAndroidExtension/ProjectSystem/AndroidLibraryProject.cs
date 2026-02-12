/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System.ComponentModel.Composition;
using Microsoft.VisualStudio.ProjectSystem;

namespace OceanAndroidExtension.ProjectSystem
{

/**
 * This class defines project capabilities for Android Library projects.
 * @ingroup oceanandroidextension
 */
[Export]
[AppliesTo(AndroidLibraryCapabilities)]
internal class AndroidLibraryProjectCapabilities
{
	/// The capability string for Android Library projects.
	public const string AndroidLibraryCapabilities = "AndroidLibrary";
}

/**
 * This class handles Android Library projects.
 * @ingroup oceanandroidextension
 */
[Export]
[AppliesTo("AndroidLibrary")]
internal class AndroidLibraryProjectHandler
{
	/// The configured project.
	[Import]
	private ConfiguredProject ConfiguredProject { get; set; } = null!;

	/// True, if this is a library project.
	public bool IsLibraryProject => true;
}

}
