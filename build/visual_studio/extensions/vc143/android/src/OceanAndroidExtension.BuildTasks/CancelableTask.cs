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
using System.Threading;
using Microsoft.Build.Framework;
using Microsoft.Build.Utilities;

namespace OceanAndroidExtension.BuildTasks
{

/**
 * This class implements the base for cancelable MSBuild tasks.
 * It provides thread-safe process registration so that Cancel() can kill all active child processes, plus a CancellationToken for cooperative cancellation of parallel operations.
 *
 * MSBuild calls Cancel() on a different thread than the one running Execute().
 * Thread safety is achieved via a volatile flag for lock-free checking and a lock around the active-process set for register/unregister/kill-all.
 *
 * @ingroup oceanandroidextension
 */
public abstract class CancelableTask : Task, ICancelableTask
{
	/// The cancellation flag, set by Cancel().
	private volatile bool isCancelled_;

	/// Lock protecting the active-process set.
	private readonly object processLock_ = new object();

	/// The set of currently running child processes.
	private readonly HashSet<Process> activeProcesses_ = new HashSet<Process>();

	/// The cancellation token source for cooperative cancellation.
	private CancellationTokenSource? cancellationTokenSource_;

	/**
	 * Returns whether cancellation has been requested.
	 */
	public bool IsCancelled => isCancelled_;

	/**
	 * Returns a CancellationToken that is triggered when Cancel() is called.
	 * Lazily creates the CancellationTokenSource on first access.
	 * If Cancel() was already called, the new source is immediately cancelled.
	 */
	public CancellationToken CancellationToken
	{
		get
		{
			if (cancellationTokenSource_ == null)
			{
				var newCts = new CancellationTokenSource();

				if (Interlocked.CompareExchange(ref cancellationTokenSource_, newCts, null) != null)
				{
					newCts.Dispose();
				}
				else if (isCancelled_)
				{
					newCts.Cancel();
				}
			}

			return cancellationTokenSource_!.Token;
		}
	}

	/**
	 * Called by MSBuild on a separate thread to request cancellation.
	 * Sets the cancellation flag, triggers the CancellationToken, and kills all registered child processes.
	 */
	public void Cancel()
	{
		isCancelled_ = true;

		try
		{
			cancellationTokenSource_?.Cancel();
		}
		catch (ObjectDisposedException)
		{
		}
		catch (AggregateException)
		{
		}

		// Snapshot the set and release the lock before killing so that
		// UnregisterProcess() calls from Execute() threads are not blocked.
		Process[] processesToKill;
		lock (processLock_)
		{
			processesToKill = new Process[activeProcesses_.Count];
			activeProcesses_.CopyTo(processesToKill);
		}

		if (processesToKill.Length > 0)
		{
			try
			{
				Log.LogMessage(MessageImportance.High, $"Cancelling {processesToKill.Length} running process(es)...");
			}
			catch (InvalidOperationException)
			{
				// BuildEngine may be null in unit tests or during teardown.
			}

			// Kill all processes in parallel to avoid sequential taskkill waits.
			System.Threading.Tasks.Parallel.ForEach(processesToKill, TryKillProcess);
		}
	}

	/**
	 * Registers a running process so that Cancel() can kill it.
	 * If Cancel() has already been called, the process is killed immediately.
	 * @param process The process to register
	 */
	protected void RegisterProcess(Process process)
	{
		lock (processLock_)
		{
			activeProcesses_.Add(process);
		}

		if (isCancelled_)
		{
			TryKillProcess(process);
		}
	}

	/**
	 * Unregisters a process after it has exited.
	 * @param process The process to unregister
	 */
	protected void UnregisterProcess(Process process)
	{
		lock (processLock_)
		{
			activeProcesses_.Remove(process);
		}
	}

	/**
	 * Attempts to kill a process and its entire process tree, ignoring exceptions if the process has already exited or is otherwise inaccessible.
	 *
	 * Process.Kill() only terminates the immediate process, leaving child processes (Gradle JVM, clang workers, Ninja, etc.) running.
	 * Process.Kill(bool) is not available on netstandard2.0, so on Windows we shell out to "taskkill /T /F" which recursively kills the tree.
	 * @param process The process to kill
	 */
	private static void TryKillProcess(Process process)
	{
		try
		{
			if (process.HasExited)
			{
				return;
			}

			try
			{
				using var taskkill = new Process();
				taskkill.StartInfo = new ProcessStartInfo
				{
					FileName = "taskkill",
					Arguments = $"/T /F /PID {process.Id}",
					UseShellExecute = false,
					CreateNoWindow = true,
					RedirectStandardOutput = true,
					RedirectStandardError = true
				};
				taskkill.Start();
				taskkill.WaitForExit(5000);
			}
			catch (System.ComponentModel.Win32Exception)
			{
				// taskkill unavailable (non-Windows), fall back to direct kill
				if (!process.HasExited)
				{
					process.Kill();
				}
			}
		}
		catch (InvalidOperationException)
		{
			// covers InvalidOperationException (process already exited) and ObjectDisposedException (process already disposed), since ObjectDisposedException derives from InvalidOperationException.
		}
	}
}

}
