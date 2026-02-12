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
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using OceanAndroidExtension.Services;

namespace OceanAndroidExtension.ToolWindows
{

/**
 * This class implements the SDK Manager tool window for managing Android SDK components.
 * @ingroup oceanandroidextension
 */
[Guid("B9C0D1E2-F3A4-5678-9ABC-DEF012345678")]
public class SdkManagerWindow : ToolWindowPane
{
	/// The WPF control for the SDK Manager.
	private readonly SdkManagerControl control_;

	/// The extension package.
	private OceanAndroidExtensionPackage? package_;

	/**
	 * Creates a new SDK Manager window.
	 * @param context The tool window context (passed from InitializeToolWindowAsync)
	 */
	public SdkManagerWindow(object? context) : base(null)
	{
		Log($"SdkManagerWindow constructor - START (context type: {context?.GetType().Name ?? "null"})");
		Caption = "Android SDK Manager";
		control_ = new SdkManagerControl();
		Content = control_;

		// Get package from context or Instance
		package_ = context as OceanAndroidExtensionPackage ?? OceanAndroidExtensionPackage.Instance;
		Log($"SdkManagerWindow constructor - package_ = {(package_ != null ? "obtained" : "NULL")}");

		control_.RefreshRequested += OnRefreshRequested;
		control_.OpenSdkManagerRequested += OnOpenSdkManagerRequested;
		Log("SdkManagerWindow constructor - END (event handlers registered)");
	}

	/**
	 * Creates a new SDK Manager window (parameterless for fallback).
	 */
	public SdkManagerWindow() : this(null)
	{
	}

	/**
	 * Called when the tool window is created.
	 */
	public override void OnToolWindowCreated()
	{
		Log("SdkManagerWindow.OnToolWindowCreated - START");
		base.OnToolWindowCreated();
		Log("SdkManagerWindow.OnToolWindowCreated - base.OnToolWindowCreated() completed");

		if (package_ == null)
		{
			Log("SdkManagerWindow.OnToolWindowCreated - ERROR: package_ is null");
			control_.SetError("Extension package not available. Please restart Visual Studio.");
			return;
		}

		Log("SdkManagerWindow.OnToolWindowCreated - starting RefreshSdkComponentsAsync");
		_ = RefreshSdkComponentsAsync();
		Log("SdkManagerWindow.OnToolWindowCreated - END");
	}

	/**
	 * Called when the user requests a refresh.
	 * @param sender The event sender
	 * @param eventArgs The event arguments
	 */
	private void OnRefreshRequested(object? sender, EventArgs eventArgs)
	{
		Log("SdkManagerWindow.OnRefreshRequested - user clicked Refresh button");
		_ = RefreshSdkComponentsAsync();
	}

	/**
	 * Called when the user requests to open the SDK Manager.
	 * @param sender The event sender
	 * @param eventArgs The event arguments
	 */
	private void OnOpenSdkManagerRequested(object? sender, EventArgs eventArgs)
	{
		Log("SdkManagerWindow.OnOpenSdkManagerRequested - user clicked Open SDK Manager button");
		_ = OpenSdkManagerAsync();
	}

	/**
	 * Opens the Android SDK Manager.
	 */
	private async Task OpenSdkManagerAsync()
	{
		Log("SdkManagerWindow.OpenSdkManagerAsync - START");

		if (package_ == null)
		{
			Log("SdkManagerWindow.OpenSdkManagerAsync - ERROR: package_ is null");
			ShowErrorMessage("Extension package not available.");
			return;
		}

		await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
		Log("SdkManagerWindow.OpenSdkManagerAsync - switched to main thread");

		try
		{
			var sdkPath = package_.SdkLocator.FindAndroidSdk();
			Log($"SdkManagerWindow.OpenSdkManagerAsync - SDK path: {sdkPath ?? "NULL"}");

			if (string.IsNullOrEmpty(sdkPath))
			{
				Log("SdkManagerWindow.OpenSdkManagerAsync - ERROR: SDK path is empty");
				ShowErrorMessage("Android SDK not found.\n\nPlease configure the SDK path in Tools > Options > Android > SDK Paths.");
				return;
			}

			// Try to find the SDK Manager command line tool
			var sdkManagerPath = Path.Combine(sdkPath, "cmdline-tools", "latest", "bin", "sdkmanager.bat");
			Log($"SdkManagerWindow.OpenSdkManagerAsync - checking SDK manager path: {sdkManagerPath}");
			Log($"SdkManagerWindow.OpenSdkManagerAsync - exists: {File.Exists(sdkManagerPath)}");

			if (!File.Exists(sdkManagerPath))
			{
				// Try older location
				sdkManagerPath = Path.Combine(sdkPath, "tools", "bin", "sdkmanager.bat");
				Log($"SdkManagerWindow.OpenSdkManagerAsync - checking alternate SDK manager path: {sdkManagerPath}");
				Log($"SdkManagerWindow.OpenSdkManagerAsync - exists: {File.Exists(sdkManagerPath)}");
			}

			if (File.Exists(sdkManagerPath))
			{
				Log($"SdkManagerWindow.OpenSdkManagerAsync - opening SDK manager at: {sdkManagerPath}");
				// Open command prompt with SDK manager
				Process.Start(new ProcessStartInfo
				{
					FileName = "cmd.exe",
					Arguments = $"/k \"\"{sdkManagerPath}\" --list\"",
					UseShellExecute = true
				});
				package_.OutputService.WriteLine("Opened SDK Manager command line.");
				Log("SdkManagerWindow.OpenSdkManagerAsync - SDK manager opened successfully");
			}
			else
			{
				// Try to open Android Studio's SDK Manager
				var studioPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles), "Android", "Android Studio", "bin", "studio64.exe");
				Log($"SdkManagerWindow.OpenSdkManagerAsync - checking Android Studio path: {studioPath}");
				Log($"SdkManagerWindow.OpenSdkManagerAsync - exists: {File.Exists(studioPath)}");

				if (File.Exists(studioPath))
				{
					Log("SdkManagerWindow.OpenSdkManagerAsync - opening Android Studio");
					package_.OutputService.WriteLine("Opening Android Studio SDK Manager...");
					ShowInfoMessage("Opening Android Studio...\n\nNavigate to Tools > SDK Manager to manage SDK components.");
					Process.Start(new ProcessStartInfo
					{
						FileName = studioPath,
						UseShellExecute = true
					});
					Log("SdkManagerWindow.OpenSdkManagerAsync - Android Studio opened");
				}
				else
				{
					Log("SdkManagerWindow.OpenSdkManagerAsync - ERROR: Neither SDK Manager nor Android Studio found");
					ShowErrorMessage("SDK Manager not found.\n\nPlease install Android Studio or the Android SDK Command-line Tools to manage SDK components.");
				}
			}

			Log("SdkManagerWindow.OpenSdkManagerAsync - END");
		}
		catch (Exception exception)
		{
			Log($"SdkManagerWindow.OpenSdkManagerAsync - EXCEPTION: {exception.GetType().Name}: {exception.Message}");
			Log($"SdkManagerWindow.OpenSdkManagerAsync - Stack trace: {exception.StackTrace}");
			package_.OutputService.WriteLine($"Error opening SDK Manager: {exception.Message}");
			ShowErrorMessage($"Failed to open SDK Manager: {exception.Message}");
		}
	}

	/**
	 * Refreshes the SDK components list.
	 */
	private async Task RefreshSdkComponentsAsync()
	{
		Log("SdkManagerWindow.RefreshSdkComponentsAsync - START");

		if (package_ == null)
		{
			Log("SdkManagerWindow.RefreshSdkComponentsAsync - ERROR: package_ is null, aborting");
			return;
		}

		await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
		Log("SdkManagerWindow.RefreshSdkComponentsAsync - switched to main thread");

		try
		{
			control_.ClearError();
			Log("SdkManagerWindow.RefreshSdkComponentsAsync - cleared error");

			var sdkPath = package_.SdkLocator.FindAndroidSdk();
			Log($"SdkManagerWindow.RefreshSdkComponentsAsync - SDK path: {sdkPath ?? "NULL"}");

			if (string.IsNullOrEmpty(sdkPath))
			{
				Log("SdkManagerWindow.RefreshSdkComponentsAsync - ERROR: SDK path is empty");
				control_.SetError("Android SDK not found. Please configure SDK path in Tools > Options > Android.");
				return;
			}

			control_.SdkPath = sdkPath!;
			Log($"SdkManagerWindow.RefreshSdkComponentsAsync - set SDK path display: {sdkPath}");

			// Load components on background thread, then update UI
			Log("SdkManagerWindow.RefreshSdkComponentsAsync - loading components on background thread");
			var components = await Task.Run(() => LoadInstalledComponents(sdkPath!));
			Log($"SdkManagerWindow.RefreshSdkComponentsAsync - loaded {components.Count} components");

			// Back on UI thread - update the control
			await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
			Log("SdkManagerWindow.RefreshSdkComponentsAsync - back on main thread, updating UI");

			control_.Components.Clear();
			Log("SdkManagerWindow.RefreshSdkComponentsAsync - cleared existing components");

			foreach (var component in components.OrderBy(c => c.Type).ThenBy(c => c.Name))
			{
				Log($"SdkManagerWindow.RefreshSdkComponentsAsync - adding component: Type={component.Type}, Name={component.Name}, Path={component.Path}");
				control_.Components.Add(component);
			}

			Log($"SdkManagerWindow.RefreshSdkComponentsAsync - added {components.Count} components to UI");

			if (components.Count == 0)
			{
				Log("SdkManagerWindow.RefreshSdkComponentsAsync - no components found, showing info message");
				control_.SetInfo("No SDK components found. Click 'Open SDK Manager' to install components.");
			}

			Log("SdkManagerWindow.RefreshSdkComponentsAsync - END (success)");
		}
		catch (InvalidOperationException exception)
		{
			Log($"SdkManagerWindow.RefreshSdkComponentsAsync - InvalidOperationException: {exception.Message}");
			control_.SetError($"Service error: {exception.Message}");
			ShowErrorMessage($"Failed to access SDK locator: {exception.Message}\n\nPlease ensure the extension loaded correctly.");
		}
		catch (Exception exception)
		{
			Log($"SdkManagerWindow.RefreshSdkComponentsAsync - EXCEPTION: {exception.GetType().Name}: {exception.Message}");
			Log($"SdkManagerWindow.RefreshSdkComponentsAsync - Stack trace: {exception.StackTrace}");
			package_.OutputService.WriteLine($"Error refreshing SDK components: {exception.Message}");
			control_.SetError($"Error: {exception.Message}");
		}
	}

	/**
	 * Loads the installed SDK components.
	 * @param sdkPath The path to the Android SDK
	 * @return The list of SDK components
	 */
	private ObservableCollection<SdkComponent> LoadInstalledComponents(string sdkPath)
	{
		Log($"SdkManagerWindow.LoadInstalledComponents - START, sdkPath={sdkPath}");
		var components = new ObservableCollection<SdkComponent>();

		var platformsDirectory = Path.Combine(sdkPath, "platforms");
		Log($"SdkManagerWindow.LoadInstalledComponents - checking platforms: {platformsDirectory}");
		Log($"SdkManagerWindow.LoadInstalledComponents - platforms exists: {Directory.Exists(platformsDirectory)}");

		if (Directory.Exists(platformsDirectory))
		{
			foreach (var directory in Directory.GetDirectories(platformsDirectory))
			{
				var name = Path.GetFileName(directory);
				if (name.StartsWith("android-"))
				{
					Log($"SdkManagerWindow.LoadInstalledComponents - found platform: {name}");
					components.Add(new SdkComponent
					{
						Name = name,
						Type = "Platform",
						Path = directory,
						IsInstalled = true
					});
				}
			}
		}

		var buildToolsDirectory = Path.Combine(sdkPath, "build-tools");
		Log($"SdkManagerWindow.LoadInstalledComponents - checking build-tools: {buildToolsDirectory}");
		Log($"SdkManagerWindow.LoadInstalledComponents - build-tools exists: {Directory.Exists(buildToolsDirectory)}");

		if (Directory.Exists(buildToolsDirectory))
		{
			foreach (var directory in Directory.GetDirectories(buildToolsDirectory))
			{
				var name = $"build-tools;{Path.GetFileName(directory)}";
				Log($"SdkManagerWindow.LoadInstalledComponents - found build-tools: {name}");
				components.Add(new SdkComponent
				{
					Name = name,
					Type = "Build Tools",
					Path = directory,
					IsInstalled = true
				});
			}
		}

		var ndkDirectory = Path.Combine(sdkPath, "ndk");
		Log($"SdkManagerWindow.LoadInstalledComponents - checking ndk: {ndkDirectory}");
		Log($"SdkManagerWindow.LoadInstalledComponents - ndk exists: {Directory.Exists(ndkDirectory)}");

		if (Directory.Exists(ndkDirectory))
		{
			foreach (var directory in Directory.GetDirectories(ndkDirectory))
			{
				var name = $"ndk;{Path.GetFileName(directory)}";
				Log($"SdkManagerWindow.LoadInstalledComponents - found NDK: {name}");
				components.Add(new SdkComponent
				{
					Name = name,
					Type = "NDK",
					Path = directory,
					IsInstalled = true
				});
			}
		}

		var cmakeDirectory = Path.Combine(sdkPath, "cmake");
		Log($"SdkManagerWindow.LoadInstalledComponents - checking cmake: {cmakeDirectory}");
		Log($"SdkManagerWindow.LoadInstalledComponents - cmake exists: {Directory.Exists(cmakeDirectory)}");

		if (Directory.Exists(cmakeDirectory))
		{
			foreach (var directory in Directory.GetDirectories(cmakeDirectory))
			{
				var name = $"cmake;{Path.GetFileName(directory)}";
				Log($"SdkManagerWindow.LoadInstalledComponents - found CMake: {name}");
				components.Add(new SdkComponent
				{
					Name = name,
					Type = "CMake",
					Path = directory,
					IsInstalled = true
				});
			}
		}

		var platformToolsDirectory = Path.Combine(sdkPath, "platform-tools");
		Log($"SdkManagerWindow.LoadInstalledComponents - checking platform-tools: {platformToolsDirectory}");
		Log($"SdkManagerWindow.LoadInstalledComponents - platform-tools exists: {Directory.Exists(platformToolsDirectory)}");

		if (Directory.Exists(platformToolsDirectory))
		{
			Log("SdkManagerWindow.LoadInstalledComponents - found platform-tools");
			components.Add(new SdkComponent
			{
				Name = "platform-tools",
				Type = "Platform Tools",
				Path = platformToolsDirectory,
				IsInstalled = true
			});
		}

		var emulatorDirectory = Path.Combine(sdkPath, "emulator");
		Log($"SdkManagerWindow.LoadInstalledComponents - checking emulator: {emulatorDirectory}");
		Log($"SdkManagerWindow.LoadInstalledComponents - emulator exists: {Directory.Exists(emulatorDirectory)}");

		if (Directory.Exists(emulatorDirectory))
		{
			Log("SdkManagerWindow.LoadInstalledComponents - found emulator");
			components.Add(new SdkComponent
			{
				Name = "emulator",
				Type = "Emulator",
				Path = emulatorDirectory,
				IsInstalled = true
			});
		}

		Log($"SdkManagerWindow.LoadInstalledComponents - END, total components: {components.Count}");
		return components;
	}

	/**
	 * Shows an error message dialog.
	 * @param message The error message
	 */
	private void ShowErrorMessage(string message)
	{
		Log($"SdkManagerWindow.ShowErrorMessage - displaying: {message}");
		ThreadHelper.ThrowIfNotOnUIThread();
		VsShellUtilities.ShowMessageBox(
			this,
			message,
			"Android SDK Manager",
			OLEMSGICON.OLEMSGICON_CRITICAL,
			OLEMSGBUTTON.OLEMSGBUTTON_OK,
			OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
	}

	/**
	 * Shows an informational message dialog.
	 * @param message The info message
	 */
	private void ShowInfoMessage(string message)
	{
		Log($"SdkManagerWindow.ShowInfoMessage - displaying: {message}");
		ThreadHelper.ThrowIfNotOnUIThread();
		VsShellUtilities.ShowMessageBox(
			this,
			message,
			"Android SDK Manager",
			OLEMSGICON.OLEMSGICON_INFO,
			OLEMSGBUTTON.OLEMSGBUTTON_OK,
			OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
	}

	/**
	 * Logs a message to debug output and file.
	 * @param message The message to log
	 */
	private static void Log(string message)
	{
		var fullMessage = $"[SdkManagerWindow] {message}";
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
 * This class represents an SDK component.
 * @ingroup oceanandroidextension
 */
public class SdkComponent : INotifyPropertyChanged
{
	/// The component name.
	public string Name { get; set; } = string.Empty;

	/// The component type.
	public string Type { get; set; } = string.Empty;

	/// The component path.
	public string Path { get; set; } = string.Empty;

	/// True, if the component is installed.
	public bool IsInstalled { get; set; }

	/// True, if the component is selected.
	public bool IsSelected { get; set; }

	/// The component version.
	public string Version { get; set; } = string.Empty;

#pragma warning disable CS0067 // Required for INotifyPropertyChanged but not used in this simple implementation
	/// Event raised when a property changes.
	public event PropertyChangedEventHandler? PropertyChanged;
#pragma warning restore CS0067
}

/**
 * This class implements the WPF control for the SDK Manager.
 * @ingroup oceanandroidextension
 */
public class SdkManagerControl : UserControl
{
	/// The SDK path text block.
	private readonly TextBlock sdkPathText_;

	/// The status text block.
	private readonly TextBlock statusText_;

	/// The components list view.
	private readonly ListView componentsList_;

	/// The collection of SDK components.
	public ObservableCollection<SdkComponent> Components { get; } = new ObservableCollection<SdkComponent>();

	/// Event raised when the user requests a refresh.
	public event EventHandler? RefreshRequested;

	/// Event raised when the user requests to open the SDK Manager.
	public event EventHandler? OpenSdkManagerRequested;

	/// Sets the SDK path display.
	public string SdkPath
	{
		set => sdkPathText_.Text = $"SDK Location: {value}";
	}

	/**
	 * Creates a new SDK Manager control.
	 */
	public SdkManagerControl()
	{
		var grid = new Grid();
		grid.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
		grid.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });
		grid.RowDefinitions.Add(new RowDefinition { Height = new GridLength(1, GridUnitType.Star) });
		grid.RowDefinitions.Add(new RowDefinition { Height = GridLength.Auto });

		sdkPathText_ = new TextBlock
		{
			Margin = new Thickness(10),
			FontWeight = FontWeights.Bold
		};
		Grid.SetRow(sdkPathText_, 0);
		grid.Children.Add(sdkPathText_);

		statusText_ = new TextBlock
		{
			Margin = new Thickness(10),
			TextWrapping = TextWrapping.Wrap,
			Visibility = Visibility.Collapsed
		};
		Grid.SetRow(statusText_, 1);
		grid.Children.Add(statusText_);

		componentsList_ = new ListView
		{
			ItemsSource = Components,
			Margin = new Thickness(10)
		};

		var gridView = new GridView();
		gridView.Columns.Add(new GridViewColumn
		{
			Header = "Type",
			DisplayMemberBinding = new System.Windows.Data.Binding("Type"),
			Width = 120
		});
		gridView.Columns.Add(new GridViewColumn
		{
			Header = "Name",
			DisplayMemberBinding = new System.Windows.Data.Binding("Name"),
			Width = 250
		});
		gridView.Columns.Add(new GridViewColumn
		{
			Header = "Status",
			Width = 80,
			DisplayMemberBinding = new System.Windows.Data.Binding("IsInstalled")
			{
				Converter = new InstalledConverter()
			}
		});
		gridView.Columns.Add(new GridViewColumn
		{
			Header = "Path",
			DisplayMemberBinding = new System.Windows.Data.Binding("Path"),
			Width = 300
		});

		componentsList_.View = gridView;
		Grid.SetRow(componentsList_, 2);
		grid.Children.Add(componentsList_);

		var buttonPanel = new StackPanel
		{
			Orientation = Orientation.Horizontal,
			HorizontalAlignment = HorizontalAlignment.Right,
			Margin = new Thickness(10)
		};

		var refreshButton = new Button
		{
			Content = "Refresh",
			Padding = new Thickness(20, 5, 20, 5),
			Margin = new Thickness(5)
		};
		refreshButton.Click += (sender, e) => RefreshRequested?.Invoke(this, EventArgs.Empty);
		buttonPanel.Children.Add(refreshButton);

		var openSdkManagerButton = new Button
		{
			Content = "Open SDK Manager",
			Padding = new Thickness(20, 5, 20, 5),
			Margin = new Thickness(5)
		};
		openSdkManagerButton.Click += (sender, e) => OpenSdkManagerRequested?.Invoke(this, EventArgs.Empty);
		buttonPanel.Children.Add(openSdkManagerButton);

		Grid.SetRow(buttonPanel, 3);
		grid.Children.Add(buttonPanel);

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

	/**
	 * This class converts a boolean installed status to a display string.
	 */
	private class InstalledConverter : System.Windows.Data.IValueConverter
	{
		/**
		 * Converts a boolean to a status string.
		 * @param value The boolean value
		 * @param targetType The target type
		 * @param parameter The converter parameter
		 * @param culture The culture info
		 * @return "Installed" or "Not Installed"
		 */
		public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			return (bool)value ? "Installed" : "Not Installed";
		}

		/**
		 * Converts back (not implemented).
		 * @param value The value
		 * @param targetType The target type
		 * @param parameter The converter parameter
		 * @param culture The culture info
		 * @return Not implemented
		 */
		public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
		{
			throw new NotImplementedException();
		}
	}
}

}
