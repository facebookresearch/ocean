/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Media;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using OceanAndroidExtension.Services;

namespace OceanAndroidExtension.ToolWindows
{

/**
 * This class implements the Logcat tool window for viewing Android device logs.
 * @ingroup oceanandroidextension
 */
[Guid("A8B9C0D1-E2F3-4567-89AB-CDEF01234567")]
public class LogcatWindow : ToolWindowPane
{
	/// The WPF control for the Logcat window.
	private readonly LogcatControl control_;

	/// The extension package.
	private OceanAndroidExtensionPackage? package_;

	/// The logcat process.
	private Process? logcatProcess_;

	/// The cancellation token source for logcat.
	private CancellationTokenSource? logcatCancellationTokenSource_;

	/**
	 * Creates a new Logcat window.
	 * @param context The tool window context (passed from InitializeToolWindowAsync)
	 */
	public LogcatWindow(object? context) : base(null)
	{
		Log($"LogcatWindow constructor - START (context type: {context?.GetType().Name ?? "null"})");
		Caption = "Logcat";
		control_ = new LogcatControl();
		Content = control_;

		// Get package from context or Instance
		package_ = context as OceanAndroidExtensionPackage ?? OceanAndroidExtensionPackage.Instance;
		Log($"LogcatWindow constructor - package_ = {(package_ != null ? "obtained" : "NULL")}");

		control_.DeviceChanged += OnDeviceChanged;
		control_.FilterChanged += OnFilterChanged;
		control_.ClearRequested += OnClearRequested;
		Log("LogcatWindow constructor - END (event handlers registered)");
	}

	/**
	 * Creates a new Logcat window (parameterless for fallback).
	 */
	public LogcatWindow() : this(null)
	{
	}

	/**
	 * Called when the tool window is created.
	 */
	public override void OnToolWindowCreated()
	{
		Log("LogcatWindow.OnToolWindowCreated - START");
		base.OnToolWindowCreated();
		Log("LogcatWindow.OnToolWindowCreated - base.OnToolWindowCreated() completed");

		if (package_ == null)
		{
			Log("LogcatWindow.OnToolWindowCreated - ERROR: package_ is null");
			control_.SetError("Extension package not available. Please restart Visual Studio.");
			return;
		}

		Log("LogcatWindow.OnToolWindowCreated - starting RefreshDevicesAsync");
		_ = RefreshDevicesAsync();
		Log("LogcatWindow.OnToolWindowCreated - END");
	}

	/**
	 * Refreshes the device list.
	 */
	private async Task RefreshDevicesAsync()
	{
		Log("LogcatWindow.RefreshDevicesAsync - START");

		if (package_ == null)
		{
			Log("LogcatWindow.RefreshDevicesAsync - ERROR: package_ is null, aborting");
			return;
		}

		try
		{
			Log("LogcatWindow.RefreshDevicesAsync - calling AdbService.GetDevicesAsync");
			var devices = await package_.AdbService.GetDevicesAsync(CancellationToken.None);
			Log($"LogcatWindow.RefreshDevicesAsync - received {devices.Count} devices");

			await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
			Log("LogcatWindow.RefreshDevicesAsync - switched to main thread");

			control_.AvailableDevices.Clear();
			Log("LogcatWindow.RefreshDevicesAsync - cleared available devices");

			foreach (var device in devices)
			{
				Log($"LogcatWindow.RefreshDevicesAsync - adding device: {device.Serial}");
				control_.AvailableDevices.Add(device.Serial);
			}

			if (control_.AvailableDevices.Count > 0 && string.IsNullOrEmpty(control_.SelectedDevice))
			{
				Log($"LogcatWindow.RefreshDevicesAsync - auto-selecting first device: {control_.AvailableDevices[0]}");
				control_.SelectedDevice = control_.AvailableDevices[0];
			}
			else if (control_.AvailableDevices.Count == 0)
			{
				Log("LogcatWindow.RefreshDevicesAsync - no devices, showing info message");
				control_.SetInfo("No devices connected. Connect a device to view logs.");
			}

			Log("LogcatWindow.RefreshDevicesAsync - END (success)");
		}
		catch (InvalidOperationException exception)
		{
			Log($"LogcatWindow.RefreshDevicesAsync - InvalidOperationException: {exception.Message}");
			await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
			control_.SetError($"Service error: {exception.Message}");
			ShowErrorMessage($"Failed to access ADB service: {exception.Message}\n\nPlease ensure the Android SDK is properly configured.");
		}
		catch (Exception exception)
		{
			Log($"LogcatWindow.RefreshDevicesAsync - EXCEPTION: {exception.GetType().Name}: {exception.Message}");
			Log($"LogcatWindow.RefreshDevicesAsync - Stack trace: {exception.StackTrace}");
			await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
			package_.OutputService.WriteLine($"Error refreshing devices: {exception.Message}");
			control_.SetError($"Error: {exception.Message}");
		}
	}

	/**
	 * Called when the selected device changes.
	 * @param sender The event sender
	 * @param deviceSerial The new device serial
	 */
	private void OnDeviceChanged(object? sender, string deviceSerial)
	{
		Log($"LogcatWindow.OnDeviceChanged - device changed to: {deviceSerial}");
		_ = StartLogcatAsync(deviceSerial);
	}

	/**
	 * Called when the log filter changes.
	 * @param sender The event sender
	 * @param filter The new filter
	 */
	private void OnFilterChanged(object? sender, LogcatFilter filter)
	{
		Log($"LogcatWindow.OnFilterChanged - filter changed: MinLevel={filter.MinLevel}, Tag={filter.Tag ?? "null"}, SearchText={filter.SearchText ?? "null"}");
		control_.ApplyFilter(filter);
	}

	/**
	 * Called when the user requests to clear the logs.
	 * @param sender The event sender
	 * @param eventArgs The event arguments
	 */
	private void OnClearRequested(object? sender, EventArgs eventArgs)
	{
		Log("LogcatWindow.OnClearRequested - user clicked Clear button");
		control_.ClearLogs();
	}

	/**
	 * Starts logcat for the specified device.
	 * @param deviceSerial The device serial
	 */
	private async Task StartLogcatAsync(string deviceSerial)
	{
		Log($"LogcatWindow.StartLogcatAsync - START for device: {deviceSerial}");

		if (package_ == null || string.IsNullOrEmpty(deviceSerial))
		{
			Log($"LogcatWindow.StartLogcatAsync - aborting: package_={package_ != null}, deviceSerial={deviceSerial ?? "NULL"}");
			return;
		}

		Log("LogcatWindow.StartLogcatAsync - stopping previous logcat");
		await StopLogcatAsync();

		logcatCancellationTokenSource_ = new CancellationTokenSource();
		Log("LogcatWindow.StartLogcatAsync - created new cancellation token");

		try
		{
			control_.ClearError();
			Log("LogcatWindow.StartLogcatAsync - calling AdbService.StartLogcatAsync");
			logcatProcess_ = await package_.AdbService.StartLogcatAsync(deviceSerial, line => AddLogLine(line), logcatCancellationTokenSource_.Token);

			if (logcatProcess_ == null)
			{
				Log("LogcatWindow.StartLogcatAsync - ERROR: logcatProcess_ is null");
				await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
				control_.SetError("Failed to start logcat. ADB may not be available.");
				ShowErrorMessage("Failed to start logcat.\n\nPlease ensure ADB is available and the device is properly connected.");
			}
			else
			{
				Log($"LogcatWindow.StartLogcatAsync - logcat process started, PID: {logcatProcess_.Id}");
			}

			Log("LogcatWindow.StartLogcatAsync - END (success)");
		}
		catch (Exception exception)
		{
			Log($"LogcatWindow.StartLogcatAsync - EXCEPTION: {exception.GetType().Name}: {exception.Message}");
			Log($"LogcatWindow.StartLogcatAsync - Stack trace: {exception.StackTrace}");
			await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
			package_.OutputService.WriteLine($"Error starting logcat: {exception.Message}");
			control_.SetError($"Error: {exception.Message}");
		}
	}

	/**
	 * Adds a log line to the display.
	 * @param line The log line
	 */
	private void AddLogLine(string line)
	{
		_ = ThreadHelper.JoinableTaskFactory.RunAsync(async () =>
		{
			await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
			var entry = ParseLogLine(line);
			control_.AddLogEntry(entry);
		});
	}

	/**
	 * Parses a logcat log line.
	 * @param line The raw log line
	 * @return The parsed log entry
	 */
	private LogEntry ParseLogLine(string line)
	{
		var entry = new LogEntry { RawLine = line };

		if (line.Length < 20)
		{
			return entry;
		}

		try
		{
			var parts = line.Split(new[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
			if (parts.Length >= 6)
			{
				entry.Timestamp = $"{parts[0]} {parts[1]}";
				entry.ProcessId = parts[2];
				entry.ThreadId = parts[3];
				entry.Level = ParseLogLevel(parts[4]);

				var colonIndex = line.IndexOf(':');
				if (colonIndex > 0 && colonIndex < line.Length - 1)
				{
					var beforeColon = line.Substring(0, colonIndex);
					var tagStart = beforeColon.LastIndexOf(' ');
					if (tagStart > 0)
					{
						entry.Tag = beforeColon.Substring(tagStart + 1);
						entry.Message = line.Substring(colonIndex + 1).Trim();
					}
				}
			}
		}
		catch
		{
			entry.Message = line;
		}

		return entry;
	}

	/**
	 * Parses a log level character.
	 * @param level The log level character
	 * @return The parsed log level
	 */
	private LogLevel ParseLogLevel(string level)
	{
		return level.ToUpperInvariant() switch
		{
			"V" => LogLevel.Verbose,
			"D" => LogLevel.Debug,
			"I" => LogLevel.Info,
			"W" => LogLevel.Warning,
			"E" => LogLevel.Error,
			"F" => LogLevel.Fatal,
			"A" => LogLevel.Assert,
			_ => LogLevel.Unknown
		};
	}

	/**
	 * Stops the logcat process asynchronously with proper cleanup.
	 */
	private async Task StopLogcatAsync()
	{
		Log("LogcatWindow.StopLogcatAsync - START");
		logcatCancellationTokenSource_?.Cancel();
		Log("LogcatWindow.StopLogcatAsync - cancellation requested");

		if (logcatProcess_ != null)
		{
			try
			{
				if (!logcatProcess_.HasExited)
				{
					Log($"LogcatWindow.StopLogcatAsync - killing process PID: {logcatProcess_.Id}");
					logcatProcess_.Kill();
					// Wait for process to exit with timeout
					await Task.Run(() => logcatProcess_.WaitForExit(3000));
					Log("LogcatWindow.StopLogcatAsync - process killed");
				}
				else
				{
					Log("LogcatWindow.StopLogcatAsync - process already exited");
				}
			}
			catch (Exception exception)
			{
				Log($"LogcatWindow.StopLogcatAsync - exception while killing: {exception.Message}");
			}
			finally
			{
				logcatProcess_.Dispose();
				logcatProcess_ = null;
				Log("LogcatWindow.StopLogcatAsync - process disposed");
			}
		}
		else
		{
			Log("LogcatWindow.StopLogcatAsync - no process to stop");
		}

		Log("LogcatWindow.StopLogcatAsync - END");
	}

	/**
	 * Shows an error message dialog.
	 * @param message The error message
	 */
	private void ShowErrorMessage(string message)
	{
		Log($"LogcatWindow.ShowErrorMessage - displaying: {message}");
		ThreadHelper.ThrowIfNotOnUIThread();
		VsShellUtilities.ShowMessageBox(
			this,
			message,
			"Logcat",
			OLEMSGICON.OLEMSGICON_CRITICAL,
			OLEMSGBUTTON.OLEMSGBUTTON_OK,
			OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
	}

	/**
	 * Disposes the tool window.
	 * @param disposing True, if disposing managed resources
	 */
	protected override void Dispose(bool disposing)
	{
		Log($"LogcatWindow.Dispose - START, disposing={disposing}");

		if (disposing)
		{
			Log("LogcatWindow.Dispose - stopping logcat process");
			logcatCancellationTokenSource_?.Cancel();
			if (logcatProcess_ != null)
			{
				try
				{
					if (!logcatProcess_.HasExited)
					{
						Log($"LogcatWindow.Dispose - killing process PID: {logcatProcess_.Id}");
						logcatProcess_.Kill();
						logcatProcess_.WaitForExit(1000);
					}
				}
				catch (Exception exception)
				{
					Log($"LogcatWindow.Dispose - exception while killing: {exception.Message}");
				}
				logcatProcess_.Dispose();
				logcatProcess_ = null;
				Log("LogcatWindow.Dispose - process disposed");
			}
			logcatCancellationTokenSource_?.Dispose();
			Log("LogcatWindow.Dispose - cancellation token disposed");
		}

		base.Dispose(disposing);
		Log("LogcatWindow.Dispose - END");
	}

	/**
	 * Logs a message to debug output and file.
	 * @param message The message to log
	 */
	private static void Log(string message)
	{
		var fullMessage = $"[LogcatWindow] {message}";
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
 * This class represents a log entry.
 * @ingroup oceanandroidextension
 */
public class LogEntry
{
	/// The raw log line.
	public string RawLine { get; set; } = string.Empty;

	/// The timestamp.
	public string Timestamp { get; set; } = string.Empty;

	/// The process ID.
	public string ProcessId { get; set; } = string.Empty;

	/// The thread ID.
	public string ThreadId { get; set; } = string.Empty;

	/// The log level.
	public LogLevel Level { get; set; } = LogLevel.Unknown;

	/// The log tag.
	public string Tag { get; set; } = string.Empty;

	/// The log message.
	public string Message { get; set; } = string.Empty;

	/// The brush color for the log level.
	public Brush LevelBrush => Level switch
	{
		LogLevel.Verbose => Brushes.Gray,
		LogLevel.Debug => Brushes.DarkCyan,
		LogLevel.Info => Brushes.Green,
		LogLevel.Warning => Brushes.Orange,
		LogLevel.Error => Brushes.Red,
		LogLevel.Fatal => Brushes.DarkRed,
		LogLevel.Assert => Brushes.Purple,
		_ => Brushes.Black
	};
}

/**
 * This enum defines the log levels.
 * @ingroup oceanandroidextension
 */
public enum LogLevel
{
	/// Unknown log level.
	Unknown,

	/// Verbose log level.
	Verbose,

	/// Debug log level.
	Debug,

	/// Info log level.
	Info,

	/// Warning log level.
	Warning,

	/// Error log level.
	Error,

	/// Fatal log level.
	Fatal,

	/// Assert log level.
	Assert
}

/**
 * This class defines a logcat filter.
 * @ingroup oceanandroidextension
 */
public class LogcatFilter
{
	/// The minimum log level to display.
	public LogLevel MinLevel { get; set; } = LogLevel.Verbose;

	/// The tag filter (nullptr for no filter).
	public string? Tag { get; set; }

	/// The search text filter (nullptr for no filter).
	public string? SearchText { get; set; }

	/// The package name filter (nullptr for no filter).
	public string? PackageName { get; set; }
}

/**
 * This class implements the WPF control for the Logcat window.
 * @ingroup oceanandroidextension
 */
public class LogcatControl : UserControl
{
	/// The log list control.
	private readonly ListBox logList_;

	/// All log entries.
	private readonly List<LogEntry> allLogs_ = new List<LogEntry>();

	/// Filtered log entries.
	private readonly ObservableCollection<LogEntry> filteredLogs_ = new ObservableCollection<LogEntry>();

	/// The current filter.
	private LogcatFilter currentFilter_ = new LogcatFilter();

	/// Maximum number of log entries.
	private const int MaxLogEntries = 10000;

	/// Number of entries to trim when at capacity.
	private const int TrimCount = 1000;

	/// The status text block.
	private readonly TextBlock statusText_;

	/// The available devices.
	public ObservableCollection<string> AvailableDevices { get; } = new ObservableCollection<string>();

	/// The selected device.
	public string? SelectedDevice { get; set; }

	/// Event raised when the device changes.
	public event EventHandler<string>? DeviceChanged;

	/// Event raised when the filter changes.
	public event EventHandler<LogcatFilter>? FilterChanged;

	/// Event raised when the user requests to clear the logs.
	public event EventHandler? ClearRequested;

	/**
	 * Creates a new Logcat control.
	 */
	public LogcatControl()
	{
		var grid = new Grid();
		grid.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
		grid.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
		grid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Star) });

		var toolbar = CreateToolbar();
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

		logList_ = new ListBox
		{
			ItemsSource = filteredLogs_,
			FontFamily = new FontFamily("Consolas"),
			FontSize = 12
		};

		VirtualizingStackPanel.SetIsVirtualizing(logList_, true);
		VirtualizingStackPanel.SetVirtualizationMode(logList_, VirtualizationMode.Recycling);

		var itemTemplate = new DataTemplate();
		var factory = new FrameworkElementFactory(typeof(TextBlock));
		factory.SetBinding(TextBlock.TextProperty, new Binding("RawLine"));
		factory.SetBinding(TextBlock.ForegroundProperty, new Binding("LevelBrush"));
		itemTemplate.VisualTree = factory;
		logList_.ItemTemplate = itemTemplate;

		Grid.SetRow(logList_, 2);
		grid.Children.Add(logList_);

		Content = grid;
	}

	/**
	 * Creates the toolbar.
	 * @return The toolbar panel
	 */
	private StackPanel CreateToolbar()
	{
		var toolbar = new StackPanel
		{
			Orientation = Orientation.Horizontal,
			Margin = new Thickness(5)
		};

		var deviceCombo = new ComboBox
		{
			ItemsSource = AvailableDevices,
			Width = 200,
			Margin = new Thickness(0, 0, 10, 0)
		};
		deviceCombo.SetBinding(ComboBox.SelectedItemProperty, new Binding("SelectedDevice")
		{
			Source = this,
			Mode = BindingMode.TwoWay
		});
		deviceCombo.SelectionChanged += (sender, eventArgs) =>
		{
			if (deviceCombo.SelectedItem is string device)
			{
				DeviceChanged?.Invoke(this, device);
			}
		};
		toolbar.Children.Add(deviceCombo);

		var levelCombo = new ComboBox
		{
			Width = 100,
			Margin = new Thickness(0, 0, 10, 0)
		};
		levelCombo.Items.Add("Verbose");
		levelCombo.Items.Add("Debug");
		levelCombo.Items.Add("Info");
		levelCombo.Items.Add("Warning");
		levelCombo.Items.Add("Error");
		levelCombo.SelectedIndex = 0;
		levelCombo.SelectionChanged += (sender, eventArgs) =>
		{
			currentFilter_.MinLevel = (LogLevel)(levelCombo.SelectedIndex + 1);
			ApplyFilter(currentFilter_);
			FilterChanged?.Invoke(this, currentFilter_);
		};
		toolbar.Children.Add(levelCombo);

		var searchBox = new TextBox
		{
			Width = 200,
			Margin = new Thickness(0, 0, 10, 0)
		};
		searchBox.TextChanged += (sender, eventArgs) =>
		{
			currentFilter_.SearchText = searchBox.Text;
			ApplyFilter(currentFilter_);
			FilterChanged?.Invoke(this, currentFilter_);
		};
		toolbar.Children.Add(searchBox);

		var clearButton = new Button { Content = "Clear" };
		clearButton.Click += (sender, eventArgs) => ClearRequested?.Invoke(this, EventArgs.Empty);
		toolbar.Children.Add(clearButton);

		return toolbar;
	}

	/**
	 * Adds a log entry to the display.
	 * @param entry The log entry
	 */
	public void AddLogEntry(LogEntry entry)
	{
		allLogs_.Add(entry);

		// Batch removal for better performance - remove TrimCount entries at once
		if (allLogs_.Count > MaxLogEntries)
		{
			allLogs_.RemoveRange(0, TrimCount);
			// Rebuild filtered list after batch removal
			RebuildFilteredLogs();
		}
		else if (MatchesFilter(entry, currentFilter_))
		{
			filteredLogs_.Add(entry);

			if (logList_.Items.Count > 0)
			{
				logList_.ScrollIntoView(logList_.Items[logList_.Items.Count - 1]);
			}
		}
	}

	/**
	 * Rebuilds the filtered logs collection from allLogs_.
	 */
	private void RebuildFilteredLogs()
	{
		filteredLogs_.Clear();
		foreach (var entry in allLogs_)
		{
			if (MatchesFilter(entry, currentFilter_))
			{
				filteredLogs_.Add(entry);
			}
		}
	}

	/**
	 * Applies a filter to the log entries.
	 * @param filter The filter to apply
	 */
	public void ApplyFilter(LogcatFilter filter)
	{
		currentFilter_ = filter;
		RebuildFilteredLogs();
	}

	/**
	 * Clears all log entries.
	 */
	public void ClearLogs()
	{
		allLogs_.Clear();
		filteredLogs_.Clear();
	}

	/**
	 * Sets an error message.
	 * @param message The error message to display
	 */
	public void SetError(string message)
	{
		statusText_.Text = message;
		statusText_.Foreground = Brushes.Red;
		statusText_.Visibility = Visibility.Visible;
	}

	/**
	 * Sets an informational message.
	 * @param message The info message to display
	 */
	public void SetInfo(string message)
	{
		statusText_.Text = message;
		statusText_.Foreground = Brushes.Gray;
		statusText_.Visibility = Visibility.Visible;
	}

	/**
	 * Clears the status message.
	 */
	public void ClearError()
	{
		statusText_.Visibility = Visibility.Collapsed;
	}

	/**
	 * Checks if a log entry matches a filter.
	 * @param entry The log entry
	 * @param filter The filter
	 * @return True, if the entry matches the filter
	 */
	private bool MatchesFilter(LogEntry entry, LogcatFilter filter)
	{
		if (entry.Level < filter.MinLevel && entry.Level != LogLevel.Unknown)
		{
			return false;
		}

		if (!string.IsNullOrEmpty(filter.Tag) && entry.Tag.IndexOf(filter.Tag, StringComparison.OrdinalIgnoreCase) < 0)
		{
			return false;
		}

		if (!string.IsNullOrEmpty(filter.SearchText) && entry.RawLine.IndexOf(filter.SearchText, StringComparison.OrdinalIgnoreCase) < 0)
		{
			return false;
		}

		return true;
	}
}

}
