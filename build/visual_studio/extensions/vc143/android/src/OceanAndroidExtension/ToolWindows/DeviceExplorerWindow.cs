/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using OceanAndroidExtension.Services;

namespace OceanAndroidExtension.ToolWindows
{

/**
 * This class implements the Device Explorer tool window for managing Android devices and emulators.
 * @ingroup oceanandroidextension
 */
[Guid("F7A8B9C0-D1E2-3456-789A-BCDEF0123456")]
public class DeviceExplorerWindow : ToolWindowPane
{
	/// The WPF control for the Device Explorer.
	private readonly DeviceExplorerControl control_;

	/// The extension package.
	private OceanAndroidExtensionPackage? package_;

	/// The cancellation token source for device refresh.
	private CancellationTokenSource? refreshCancellationTokenSource_;

	/**
	 * Creates a new Device Explorer window.
	 * @param context The tool window context (passed from InitializeToolWindowAsync)
	 */
	public DeviceExplorerWindow(object? context) : base(null)
	{
		Log($"DeviceExplorerWindow constructor - START (context type: {context?.GetType().Name ?? "null"})");
		Caption = "Device Explorer";
		control_ = new DeviceExplorerControl();
		Content = control_;

		// Get package from context or Instance
		package_ = context as OceanAndroidExtensionPackage ?? OceanAndroidExtensionPackage.Instance;
		Log($"DeviceExplorerWindow constructor - package_ = {(package_ != null ? "obtained" : "NULL")}");

		control_.RefreshRequested += OnRefreshRequested;
		control_.StartEmulatorRequested += OnStartEmulatorRequested;
		Log("DeviceExplorerWindow constructor - END (event handlers registered)");
	}

	/**
	 * Creates a new Device Explorer window (parameterless for fallback).
	 */
	public DeviceExplorerWindow() : this(null)
	{
	}

	/**
	 * Called when the tool window is created.
	 */
	public override void OnToolWindowCreated()
	{
		Log("DeviceExplorerWindow.OnToolWindowCreated - START");
		base.OnToolWindowCreated();
		Log("DeviceExplorerWindow.OnToolWindowCreated - base.OnToolWindowCreated() completed");

		if (package_ == null)
		{
			Log("DeviceExplorerWindow.OnToolWindowCreated - ERROR: package_ is null");
			control_.SetError("Extension package not available. Please restart Visual Studio.");
			return;
		}

		Log("DeviceExplorerWindow.OnToolWindowCreated - starting RefreshDevicesAsync");
		_ = RefreshDevicesAsync();
		Log("DeviceExplorerWindow.OnToolWindowCreated - END");
	}

	/**
	 * Called when the user requests a refresh.
	 * @param sender The event sender
	 * @param eventArgs The event arguments
	 */
	private void OnRefreshRequested(object? sender, EventArgs eventArgs)
	{
		Log("DeviceExplorerWindow.OnRefreshRequested - user clicked Refresh button");
		_ = RefreshDevicesAsync();
	}

	/**
	 * Called when the user requests to start an emulator.
	 * @param sender The event sender
	 * @param eventArgs The event arguments
	 */
	private void OnStartEmulatorRequested(object? sender, EventArgs eventArgs)
	{
		Log("DeviceExplorerWindow.OnStartEmulatorRequested - user clicked Start Emulator button");
		_ = StartEmulatorAsync();
	}

	/**
	 * Starts an Android emulator.
	 */
	private async Task StartEmulatorAsync()
	{
		Log("DeviceExplorerWindow.StartEmulatorAsync - START");
		await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
		Log("DeviceExplorerWindow.StartEmulatorAsync - switched to main thread");

		if (package_ == null)
		{
			Log("DeviceExplorerWindow.StartEmulatorAsync - ERROR: package_ is null");
			ShowErrorMessage("Extension package not available.");
			return;
		}

		try
		{
			var sdkPath = package_.SdkLocator.FindAndroidSdk();
			Log($"DeviceExplorerWindow.StartEmulatorAsync - SDK path: {sdkPath ?? "NULL"}");

			if (string.IsNullOrEmpty(sdkPath))
			{
				Log("DeviceExplorerWindow.StartEmulatorAsync - ERROR: SDK path is empty");
				ShowErrorMessage("Android SDK not found.\n\nPlease configure the SDK path in Tools > Options > Android > SDK Paths.");
				return;
			}

			var emulatorPath = Path.Combine(sdkPath, "emulator", "emulator.exe");
			Log($"DeviceExplorerWindow.StartEmulatorAsync - emulator path: {emulatorPath}");
			Log($"DeviceExplorerWindow.StartEmulatorAsync - emulator exists: {File.Exists(emulatorPath)}");

			if (!File.Exists(emulatorPath))
			{
				Log("DeviceExplorerWindow.StartEmulatorAsync - ERROR: emulator.exe not found");
				ShowErrorMessage("Android Emulator not found.\n\nPlease install the Android Emulator via the SDK Manager.");
				return;
			}

			Log("DeviceExplorerWindow.StartEmulatorAsync - listing AVDs");
			var process = new System.Diagnostics.Process
			{
				StartInfo = new System.Diagnostics.ProcessStartInfo
				{
					FileName = emulatorPath,
					Arguments = "-list-avds",
					UseShellExecute = false,
					RedirectStandardOutput = true,
					CreateNoWindow = true
				}
			};

			process.Start();
			Log("DeviceExplorerWindow.StartEmulatorAsync - started emulator -list-avds process");
			var avdList = await process.StandardOutput.ReadToEndAsync();
			process.WaitForExit();
			Log($"DeviceExplorerWindow.StartEmulatorAsync - AVD list output:\n{avdList}");

			var avds = avdList.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
			Log($"DeviceExplorerWindow.StartEmulatorAsync - found {avds.Length} AVDs");

			if (avds.Length == 0)
			{
				Log("DeviceExplorerWindow.StartEmulatorAsync - ERROR: no AVDs found");
				ShowErrorMessage("No Android Virtual Devices (AVDs) found.\n\nPlease create an AVD using Android Studio's AVD Manager.");
				return;
			}

			var avdName = avds[0];
			Log($"DeviceExplorerWindow.StartEmulatorAsync - starting AVD: {avdName}");
			package_.OutputService.WriteLine($"Starting emulator: {avdName}");

			System.Diagnostics.Process.Start(new System.Diagnostics.ProcessStartInfo
			{
				FileName = emulatorPath,
				Arguments = $"-avd {avdName}",
				UseShellExecute = false,
				CreateNoWindow = true
			});

			Log($"DeviceExplorerWindow.StartEmulatorAsync - emulator process started for {avdName}");
			package_.OutputService.WriteLine($"Emulator '{avdName}' is starting...");

			Log("DeviceExplorerWindow.StartEmulatorAsync - waiting 3 seconds before refresh");
			await Task.Delay(3000);
			Log("DeviceExplorerWindow.StartEmulatorAsync - triggering device refresh");
			_ = RefreshDevicesAsync();
			Log("DeviceExplorerWindow.StartEmulatorAsync - END");
		}
		catch (Exception exception)
		{
			Log($"DeviceExplorerWindow.StartEmulatorAsync - EXCEPTION: {exception.GetType().Name}: {exception.Message}");
			Log($"DeviceExplorerWindow.StartEmulatorAsync - Stack trace: {exception.StackTrace}");
			package_.OutputService.WriteLine($"Error starting emulator: {exception.Message}");
			ShowErrorMessage($"Failed to start emulator: {exception.Message}");
		}
	}

	/**
	 * Refreshes the device list.
	 */
	private async Task RefreshDevicesAsync()
	{
		Log("DeviceExplorerWindow.RefreshDevicesAsync - START");

		if (package_ == null)
		{
			Log("DeviceExplorerWindow.RefreshDevicesAsync - ERROR: package_ is null, aborting");
			return;
		}

		Log("DeviceExplorerWindow.RefreshDevicesAsync - cancelling previous refresh if any");
		refreshCancellationTokenSource_?.Cancel();
		refreshCancellationTokenSource_ = new CancellationTokenSource();
		Log("DeviceExplorerWindow.RefreshDevicesAsync - created new cancellation token");

		try
		{
			control_.ClearError();
			Log("DeviceExplorerWindow.RefreshDevicesAsync - calling AdbService.GetDevicesAsync");
			var devices = await package_.AdbService.GetDevicesAsync(refreshCancellationTokenSource_.Token);
			Log($"DeviceExplorerWindow.RefreshDevicesAsync - received {devices.Count} devices from AdbService");

			await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
			Log("DeviceExplorerWindow.RefreshDevicesAsync - switched to main thread");

			control_.Devices.Clear();
			Log("DeviceExplorerWindow.RefreshDevicesAsync - cleared existing devices");

			foreach (var device in devices)
			{
				Log($"DeviceExplorerWindow.RefreshDevicesAsync - adding device: Serial={device.Serial}, Model={device.Model}, State={device.State}, API={device.ApiLevel}, Android={device.AndroidVersion}, IsEmulator={device.IsEmulator}");
				control_.Devices.Add(new DeviceViewModel
				{
					Serial = device.Serial,
					Model = device.Model,
					State = device.State.ToString(),
					ApiLevel = device.ApiLevel,
					AndroidVersion = device.AndroidVersion,
					IsEmulator = device.IsEmulator
				});
			}

			Log($"DeviceExplorerWindow.RefreshDevicesAsync - added {devices.Count} devices to control");

			if (devices.Count == 0)
			{
				Log("DeviceExplorerWindow.RefreshDevicesAsync - no devices, showing info message");
				control_.SetInfo("No devices connected. Connect a device or start an emulator.");
			}

			Log("DeviceExplorerWindow.RefreshDevicesAsync - END (success)");
		}
		catch (OperationCanceledException)
		{
			Log("DeviceExplorerWindow.RefreshDevicesAsync - operation was cancelled");
		}
		catch (InvalidOperationException exception)
		{
			Log($"DeviceExplorerWindow.RefreshDevicesAsync - InvalidOperationException: {exception.Message}");
			await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
			control_.SetError($"Service error: {exception.Message}");
			ShowErrorMessage($"Failed to access ADB service: {exception.Message}\n\nPlease ensure the Android SDK is properly configured.");
		}
		catch (Exception exception)
		{
			Log($"DeviceExplorerWindow.RefreshDevicesAsync - EXCEPTION: {exception.GetType().Name}: {exception.Message}");
			Log($"DeviceExplorerWindow.RefreshDevicesAsync - Stack trace: {exception.StackTrace}");
			await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
			package_.OutputService.WriteLine($"Error refreshing devices: {exception.Message}");
			control_.SetError($"Error: {exception.Message}");
		}
	}

	/**
	 * Shows an error message dialog.
	 * @param message The error message
	 */
	private void ShowErrorMessage(string message)
	{
		Log($"DeviceExplorerWindow.ShowErrorMessage - displaying: {message}");
		ThreadHelper.ThrowIfNotOnUIThread();
		VsShellUtilities.ShowMessageBox(
			this,
			message,
			"Device Explorer",
			OLEMSGICON.OLEMSGICON_CRITICAL,
			OLEMSGBUTTON.OLEMSGBUTTON_OK,
			OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
	}

	/**
	 * Logs a message to debug output and file.
	 * @param message The message to log
	 */
	private static void Log(string message)
	{
		var fullMessage = $"[DeviceExplorerWindow] {message}";
		System.Diagnostics.Debug.WriteLine(fullMessage);

		try
		{
			var logPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "OceanAndroidExtension", "debug.log");

			var logDirectory = Path.GetDirectoryName(logPath);
			if (!Directory.Exists(logDirectory))
			{
				Directory.CreateDirectory(logDirectory!);
			}

			File.AppendAllText(logPath, $"{DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} {fullMessage}{Environment.NewLine}");
		}
		catch
		{
			// Ignore file logging errors
		}
	}
}

/**
 * This class represents a device in the Device Explorer view model.
 * @ingroup oceanandroidextension
 */
public class DeviceViewModel : INotifyPropertyChanged
{
	/// The device serial number.
	public string Serial { get; set; } = string.Empty;

	/// The device model name.
	public string Model { get; set; } = string.Empty;

	/// The device state.
	public string State { get; set; } = string.Empty;

	/// The API level.
	public int ApiLevel { get; set; }

	/// The Android version.
	public string AndroidVersion { get; set; } = string.Empty;

	/// True, if the device is an emulator.
	public bool IsEmulator { get; set; }

	/// The display name for the device.
	public string DisplayName => string.IsNullOrEmpty(Model) ? Serial : $"{Model} ({Serial})";

	/// The device icon type.
	public string DeviceIcon => IsEmulator ? "Emulator" : "Device";

#pragma warning disable CS0067 // Required for INotifyPropertyChanged but not used in this simple implementation
	/// Event raised when a property changes.
	public event PropertyChangedEventHandler? PropertyChanged;
#pragma warning restore CS0067
}

/**
 * This class implements the WPF control for the Device Explorer.
 * @ingroup oceanandroidextension
 */
public class DeviceExplorerControl : UserControl
{
	/// The collection of devices.
	public ObservableCollection<DeviceViewModel> Devices { get; } = new ObservableCollection<DeviceViewModel>();

	/// The status text block.
	private readonly TextBlock statusText_;

	/// Event raised when the user requests a refresh.
	public event EventHandler? RefreshRequested;

	/// Event raised when the user requests to start an emulator.
	public event EventHandler? StartEmulatorRequested;

	/**
	 * Creates a new Device Explorer control.
	 */
	public DeviceExplorerControl()
	{
		var grid = new Grid();
		grid.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
		grid.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
		grid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Star) });

		var toolbar = new StackPanel { Orientation = Orientation.Horizontal, Margin = new Thickness(5) };
		var refreshButton = new Button { Content = "Refresh", Margin = new Thickness(0, 0, 5, 0) };
		refreshButton.Click += (sender, e) => RefreshRequested?.Invoke(this, EventArgs.Empty);
		var startEmulatorButton = new Button { Content = "Start Emulator" };
		startEmulatorButton.Click += (sender, e) => StartEmulatorRequested?.Invoke(this, EventArgs.Empty);
		toolbar.Children.Add(refreshButton);
		toolbar.Children.Add(startEmulatorButton);
		Grid.SetRow(toolbar, 0);
		grid.Children.Add(toolbar);

		statusText_ = new TextBlock
		{
			Margin = new Thickness(5),
			TextWrapping = TextWrapping.Wrap,
			Visibility = Visibility.Collapsed
		};
		Grid.SetRow(statusText_, 1);
		grid.Children.Add(statusText_);

		var listView = new ListView
		{
			ItemsSource = Devices,
			Margin = new Thickness(5)
		};

		var gridView = new GridView();
		gridView.Columns.Add(new GridViewColumn
		{
			Header = "Device",
			DisplayMemberBinding = new Binding("DisplayName"),
			Width = 200
		});
		gridView.Columns.Add(new GridViewColumn
		{
			Header = "State",
			DisplayMemberBinding = new Binding("State"),
			Width = 80
		});
		gridView.Columns.Add(new GridViewColumn
		{
			Header = "Android",
			DisplayMemberBinding = new Binding("AndroidVersion"),
			Width = 80
		});
		gridView.Columns.Add(new GridViewColumn
		{
			Header = "API",
			DisplayMemberBinding = new Binding("ApiLevel"),
			Width = 50
		});

		listView.View = gridView;
		Grid.SetRow(listView, 2);
		grid.Children.Add(listView);

		Content = grid;
	}

	/**
	 * Sets an error message.
	 * @param message The error message to display
	 */
	public void SetError(string message)
	{
		statusText_.Text = message;
		statusText_.Foreground = System.Windows.Media.Brushes.Red;
		statusText_.Visibility = Visibility.Visible;
	}

	/**
	 * Sets an informational message.
	 * @param message The info message to display
	 */
	public void SetInfo(string message)
	{
		statusText_.Text = message;
		statusText_.Foreground = System.Windows.Media.Brushes.Gray;
		statusText_.Visibility = Visibility.Visible;
	}

	/**
	 * Clears the status message.
	 */
	public void ClearError()
	{
		statusText_.Visibility = Visibility.Collapsed;
	}
}

}
