/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;
using Microsoft.Build.Framework;
using Xunit;

namespace OceanAndroidExtension.Tests
{

/**
 * This class implements tests for the CancelableTask base class.
 * @ingroup oceanandroidextension
 */
public class CancelableTaskTests
{
	/**
	 * A concrete implementation of CancelableTask for testing.
	 * Exposes protected members for test access.
	 */
	private class TestCancelableTask : BuildTasks.CancelableTask
	{
		public Func<bool>? ExecuteFunc { get; set; }

		public override bool Execute()
		{
			return ExecuteFunc?.Invoke() ?? true;
		}

		public void TestRegisterProcess(Process process)
		{
			RegisterProcess(process);
		}

		public void TestUnregisterProcess(Process process)
		{
			UnregisterProcess(process);
		}
	}

	/**
	 * Tests that IsCancelled is false initially.
	 */
	[Fact]
	public void IsCancelled_IsFalseInitially()
	{
		var task = new TestCancelableTask();
		Assert.False(task.IsCancelled);
	}

	/**
	 * Tests that Cancel sets IsCancelled to true.
	 */
	[Fact]
	public void Cancel_SetsIsCancelledToTrue()
	{
		var task = new TestCancelableTask();

		task.Cancel();

		Assert.True(task.IsCancelled);
	}

	/**
	 * Tests that calling Cancel multiple times does not throw.
	 */
	[Fact]
	public void Cancel_CalledMultipleTimes_DoesNotThrow()
	{
		var task = new TestCancelableTask();

		task.Cancel();
		task.Cancel();
		task.Cancel();

		Assert.True(task.IsCancelled);
	}

	/**
	 * Tests that Cancel triggers the CancellationToken.
	 */
	[Fact]
	public void Cancel_TriggersCancellationToken()
	{
		var task = new TestCancelableTask();
		var token = task.CancellationToken;

		Assert.False(token.IsCancellationRequested);

		task.Cancel();

		Assert.True(token.IsCancellationRequested);
	}

	/**
	 * Tests that CancellationToken returns the same token on repeated access.
	 */
	[Fact]
	public void CancellationToken_ReturnsSameTokenOnRepeatedAccess()
	{
		var task = new TestCancelableTask();

		var token1 = task.CancellationToken;
		var token2 = task.CancellationToken;

		Assert.Equal(token1, token2);
	}

	/**
	 * Tests that all 9 task classes implement ICancelableTask.
	 */
	[Theory]
	[InlineData(typeof(BuildTasks.GradleBuildTask))]
	[InlineData(typeof(BuildTasks.AndroidClCompileTask))]
	[InlineData(typeof(BuildTasks.AndroidLinkTask))]
	[InlineData(typeof(BuildTasks.AndroidLibTask))]
	[InlineData(typeof(BuildTasks.NdkCompileTask))]
	[InlineData(typeof(BuildTasks.NdkDirectCompileTask))]
	[InlineData(typeof(BuildTasks.AdbDeployTask))]
	[InlineData(typeof(BuildTasks.ApkPackageTask))]
	[InlineData(typeof(BuildTasks.StageJavaSourcesTask))]
	public void AllTaskClasses_ImplementICancelableTask(Type taskType)
	{
		Assert.True(typeof(ICancelableTask).IsAssignableFrom(taskType),
			$"{taskType.Name} should implement ICancelableTask");
	}

	/**
	 * Tests that all 9 task classes derive from CancelableTask.
	 */
	[Theory]
	[InlineData(typeof(BuildTasks.GradleBuildTask))]
	[InlineData(typeof(BuildTasks.AndroidClCompileTask))]
	[InlineData(typeof(BuildTasks.AndroidLinkTask))]
	[InlineData(typeof(BuildTasks.AndroidLibTask))]
	[InlineData(typeof(BuildTasks.NdkCompileTask))]
	[InlineData(typeof(BuildTasks.NdkDirectCompileTask))]
	[InlineData(typeof(BuildTasks.AdbDeployTask))]
	[InlineData(typeof(BuildTasks.ApkPackageTask))]
	[InlineData(typeof(BuildTasks.StageJavaSourcesTask))]
	public void AllTaskClasses_DeriveFromCancelableTask(Type taskType)
	{
		Assert.True(typeof(BuildTasks.CancelableTask).IsAssignableFrom(taskType),
			$"{taskType.Name} should derive from CancelableTask");
	}

	/**
	 * Tests that a cancelled task returns false from Execute when it checks IsCancelled.
	 */
	[Fact]
	public void Execute_ReturnsFalse_WhenCancelledBeforeExecution()
	{
		var task = new TestCancelableTask();
		task.ExecuteFunc = () => !task.IsCancelled;

		task.Cancel();

		var result = task.Execute();
		Assert.False(result);
	}

	/**
	 * Tests that Cancel can be called from a different thread.
	 */
	[Fact]
	public void Cancel_FromDifferentThread_SetsIsCancelled()
	{
		var task = new TestCancelableTask();

		var thread = new Thread(() => task.Cancel());
		thread.Start();
		thread.Join();

		Assert.True(task.IsCancelled);
	}

	/**
	 * Tests that CancellationToken accessed after Cancel still reports cancelled.
	 */
	[Fact]
	public void CancellationToken_AccessedAfterCancel_ReportsCancelled()
	{
		var task = new TestCancelableTask();

		task.Cancel();

		Assert.True(task.CancellationToken.IsCancellationRequested);
	}

	/**
	 * Creates a ProcessStartInfo for a long-running process that works on both Windows and Unix.
	 */
	private static ProcessStartInfo CreateLongRunningProcessStartInfo()
	{
		if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
		{
			return new ProcessStartInfo
			{
				FileName = "ping",
				Arguments = "-n 60 127.0.0.1",
				UseShellExecute = false,
				CreateNoWindow = true
			};
		}

		return new ProcessStartInfo
		{
			FileName = "sleep",
			Arguments = "60",
			UseShellExecute = false,
			CreateNoWindow = true
		};
	}

	/**
	 * Tests that RegisterProcess immediately kills a process when already cancelled.
	 */
	[Fact]
	public void RegisterProcess_KillsProcess_WhenAlreadyCancelled()
	{
		var task = new TestCancelableTask();
		task.Cancel();

		using var process = new Process
		{
			StartInfo = CreateLongRunningProcessStartInfo()
		};
		process.Start();
		task.TestRegisterProcess(process);

		// The process should be killed almost immediately
		var exited = process.WaitForExit(5000);
		Assert.True(exited, "Process should have been killed by RegisterProcess");
	}

	/**
	 * Tests that Cancel kills a previously registered running process.
	 */
	[Fact]
	public void Cancel_KillsRegisteredProcess()
	{
		var task = new TestCancelableTask();

		using var process = new Process
		{
			StartInfo = CreateLongRunningProcessStartInfo()
		};
		process.Start();
		task.TestRegisterProcess(process);

		task.Cancel();

		var exited = process.WaitForExit(5000);
		Assert.True(exited, "Process should have been killed by Cancel");
	}

	/**
	 * Tests that Cancel does not affect an unregistered process.
	 */
	[Fact]
	public void Cancel_DoesNotKill_UnregisteredProcess()
	{
		var task = new TestCancelableTask();

		using var process = new Process
		{
			StartInfo = CreateLongRunningProcessStartInfo()
		};
		process.Start();
		task.TestRegisterProcess(process);
		task.TestUnregisterProcess(process);

		task.Cancel();

		// Process should still be running since it was unregistered
		Assert.False(process.HasExited, "Unregistered process should still be running");

		process.Kill();
		process.WaitForExit(5000);
	}
}

}
