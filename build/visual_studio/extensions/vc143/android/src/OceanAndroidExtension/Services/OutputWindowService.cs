/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;

namespace OceanAndroidExtension.Services
{

/**
 * This interface defines the Visual Studio output window service functionality.
 * @ingroup oceanandroidextension
 */
public interface IOutputWindowService
{
	/**
	 * Writes a line to the Android output pane.
	 * @param message The message to write
	 */
	void WriteLine(string message);

	/**
	 * Writes text to the Android output pane without a newline.
	 * @param message The message to write
	 */
	void Write(string message);

	/**
	 * Clears the Android output pane.
	 */
	void Clear();

	/**
	 * Shows and activates the Android output pane.
	 */
	void Show();
}

/**
 * This class implements the Visual Studio output window service.
 * @ingroup oceanandroidextension
 */
public class OutputWindowService : IOutputWindowService
{
	/// The GUID for the Android output pane.
	private static readonly Guid AndroidOutputPaneGuid = new Guid("C4D5E6F7-A8B9-0123-4567-89ABCDEF0123");

	/// The Visual Studio package.
	private readonly AsyncPackage package_;

	/// The cached output pane.
	private IVsOutputWindowPane? pane_;

	/**
	 * Creates a new output window service.
	 * @param package The Visual Studio package
	 */
	public OutputWindowService(AsyncPackage package)
	{
		package_ = package;
	}

	/**
	 * Returns the Android output pane, creating it if necessary.
	 * @return The output pane, nullptr if not available
	 */
	private IVsOutputWindowPane? GetPane()
	{
		if (pane_ != null)
		{
			return pane_;
		}

		ThreadHelper.ThrowIfNotOnUIThread();

		var outputWindow = package_.GetService<SVsOutputWindow, IVsOutputWindow>();
		if (outputWindow == null)
		{
			return null;
		}

		if (outputWindow.GetPane(AndroidOutputPaneGuid, out pane_) != VSConstants.S_OK)
		{
			outputWindow.CreatePane(AndroidOutputPaneGuid, "Ocean Android", fInitVisible: 1, fClearWithSolution: 0);
			outputWindow.GetPane(AndroidOutputPaneGuid, out pane_);
		}

		return pane_;
	}

	/**
	 * Writes a line to the Android output pane.
	 * @param message The message to write
	 */
	public void WriteLine(string message)
	{
#pragma warning disable VSTHRD010 // Write handles thread switching internally
		Write(message + Environment.NewLine);
#pragma warning restore VSTHRD010
	}

	/**
	 * Writes text to the Android output pane without a newline.
	 * @param message The message to write
	 */
	public void Write(string message)
	{
		try
		{
			ThreadHelper.JoinableTaskFactory.Run(async () =>
			{
				await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
				GetPane()?.OutputStringThreadSafe(message);
			});
		}
		catch
		{
			// Ignore output errors
		}
	}

	/**
	 * Clears the Android output pane.
	 */
	public void Clear()
	{
		try
		{
			ThreadHelper.JoinableTaskFactory.Run(async () =>
			{
				await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
				GetPane()?.Clear();
			});
		}
		catch
		{
			// Ignore errors
		}
	}

	/**
	 * Shows and activates the Android output pane.
	 */
	public void Show()
	{
		try
		{
			ThreadHelper.JoinableTaskFactory.Run(async () =>
			{
				await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
				GetPane()?.Activate();
			});
		}
		catch
		{
			// Ignore errors
		}
	}
}

}
