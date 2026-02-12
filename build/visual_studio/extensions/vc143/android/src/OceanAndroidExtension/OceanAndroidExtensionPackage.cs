/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

// @nolint

using System;
using System.ComponentModel.Design;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using Microsoft.Win32;
using OceanAndroidExtension.Options;
using OceanAndroidExtension.Services;
using OceanAndroidExtension.ToolWindows;
using Task = System.Threading.Tasks.Task;

namespace OceanAndroidExtension
{

/**
 * This class implements the Ocean Android Extension package for Visual Studio 2022.
 * The package provides Android development capabilities including debugging, deployment, and SDK management.
 * @ingroup oceanandroidextension
 */
[PackageRegistration(UseManagedResourcesOnly = true, AllowsBackgroundLoading = true)]
[Guid(PackageGuidString)]
[ProvideAutoLoad(VSConstants.UICONTEXT.NoSolution_string, PackageAutoLoadFlags.BackgroundLoad)]
[ProvideAutoLoad(VSConstants.UICONTEXT.SolutionExists_string, PackageAutoLoadFlags.BackgroundLoad)]
[ProvideMenuResource("Menus.ctmenu", 1)]
[ProvideOptionPage(typeof(AndroidSdkOptionsPage), "Android", "SDK Paths", 0, 0, true)]
[ProvideToolWindow(typeof(DeviceExplorerWindow), Style = VsDockStyle.Tabbed, Window = "3ae79031-e1bc-11d0-8f78-00a0c9110057")]
[ProvideToolWindow(typeof(LogcatWindow), Style = VsDockStyle.Tabbed, Window = "3ae79031-e1bc-11d0-8f78-00a0c9110057")]
[ProvideToolWindow(typeof(SdkManagerWindow), Style = VsDockStyle.Float)]
public sealed class OceanAndroidExtensionPackage : AsyncPackage
{
	/// The package GUID string.
	public const string PackageGuidString = "A1B2C3D4-E5F6-7890-ABCD-EF1234567890";

	/// The singleton instance of the package.
	public static OceanAndroidExtensionPackage? Instance { get; private set; }

	/// The VSIX extension identifier.
	public const string ExtensionId = "OceanAndroidExtension.A1B2C3D4-E5F6-7890-ABCD-EF1234567890";

	/// The registry key path for extension settings.
	private const string RegistryKeyPath = @"Software\OceanAndroidExtension";

	/// The command set GUID.
	public static readonly Guid CommandSetGuid = new Guid("B2C3D4E5-F6A7-8901-BCDE-F12345678902");

	/// The Device Explorer command ID.
	public const int DeviceExplorerCommandId = 0x0100;

	/// The Logcat command ID.
	public const int LogcatCommandId = 0x0101;

	/// The SDK Manager command ID.
	public const int SdkManagerCommandId = 0x0102;

	/// The Deploy APK command ID.
	public const int DeployApkCommandId = 0x0103;

	/// The Attach Debugger command ID.
	public const int AttachDebuggerCommandId = 0x0104;

	/// The SDK locator service.
	private ISdkLocatorService? sdkLocatorService_;

	/// The ADB service.
	private IAdbService? adbService_;

	/// The output window service.
	private IOutputWindowService? outputService_;

	/**
	 * Static constructor for early initialization logging.
	 */
	static OceanAndroidExtensionPackage()
	{
		try
		{
			var logPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "OceanAndroidExtension", "static_init.log");

			var logDirectory = Path.GetDirectoryName(logPath);
			if (!Directory.Exists(logDirectory))
			{
				Directory.CreateDirectory(logDirectory!);
			}

			File.AppendAllText(logPath, $"{DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} Static constructor called{Environment.NewLine}");
		}
		catch
		{
			// Ignore - just trying to log
		}
	}

	/**
	 * Returns the SDK locator service.
	 * @return The SDK locator service
	 */
	public ISdkLocatorService SdkLocator => sdkLocatorService_ ?? throw new InvalidOperationException("Package not initialized");

	/**
	 * Returns the ADB service.
	 * @return The ADB service
	 */
	public IAdbService AdbService => adbService_ ?? throw new InvalidOperationException("Package not initialized");

	/**
	 * Returns the output window service.
	 * @return The output window service
	 */
	public IOutputWindowService OutputService => outputService_ ?? throw new InvalidOperationException("Package not initialized");

	/**
	 * Initializes the package asynchronously.
	 * @param cancellationToken The cancellation token
	 * @param progress The progress reporter
	 */
	protected override async Task InitializeAsync(CancellationToken cancellationToken, IProgress<ServiceProgressData> progress)
	{
		Log("========================================");
		Log("OceanAndroidExtensionPackage.InitializeAsync - START");
		Log("========================================");
		Log($"Package GUID: {PackageGuidString}");
		Log($"Extension ID: {ExtensionId}");

		// Set the singleton instance
		Instance = this;
		Log("Instance set to this package");

		await base.InitializeAsync(cancellationToken, progress);
		Log("base.InitializeAsync completed");

		await RegisterExtensionPathAsync();

		Log("Initializing services...");
		outputService_ = new OutputWindowService(this);
		sdkLocatorService_ = new SdkLocatorService(this);
		adbService_ = new AdbService(sdkLocatorService_, outputService_);
		Log("Services initialized");

		await RegisterSdkPathsAsync();

		await JoinableTaskFactory.SwitchToMainThreadAsync(cancellationToken);
		Log("Switched to main thread");

		await InitializeCommandsAsync();
		Log("Commands initialized");

		await LogSdkStatusAsync();

		Log("========================================");
		Log("OceanAndroidExtensionPackage.InitializeAsync - COMPLETE");
		Log("========================================");
	}

	/**
	 * Registers the extension's install path to a well-known registry location.
	 */
	private async Task RegisterExtensionPathAsync()
	{
		await Task.Yield();

		Log("========================================");
		Log("RegisterExtensionPathAsync - START");
		Log("========================================");

		try
		{
			var assemblyLocation = Assembly.GetExecutingAssembly().Location;
			Log($"Assembly.Location = {assemblyLocation}");

			var installPath = Path.GetDirectoryName(assemblyLocation);
			Log($"InstallPath (dirname) = {installPath}");

			if (string.IsNullOrEmpty(installPath))
			{
				Log("ERROR: Could not determine assembly location - installPath is null/empty");
				return;
			}

			Log($"Listing contents of installPath: {installPath}");
			try
			{
				foreach (var directory in Directory.GetDirectories(installPath))
				{
					Log($"  [DIR]  {Path.GetFileName(directory)}");
				}
				foreach (var file in Directory.GetFiles(installPath))
				{
					Log($"  [FILE] {Path.GetFileName(file)}");
				}
			}
			catch (Exception listException)
			{
				Log($"  ERROR listing directory: {listException.Message}");
			}

			var toolsetPath = Path.Combine(installPath, "toolset", "OceanNDK");
			Log($"Checking primary toolset path: {toolsetPath}");
			Log($"  Exists: {Directory.Exists(toolsetPath)}");

			if (!Directory.Exists(toolsetPath))
			{
				Log("Primary toolset path not found, trying alternate location...");

				toolsetPath = Path.Combine(installPath, "OceanNDK");
				Log($"Checking alternate toolset path: {toolsetPath}");
				Log($"  Exists: {Directory.Exists(toolsetPath)}");

				if (!Directory.Exists(toolsetPath))
				{
					var msbuildPath = Path.Combine(installPath, "MSBuild", "Microsoft", "VC", "v170", "Platforms", "Android", "PlatformToolsets", "OceanNDK");
					Log($"Checking MSBuild toolset path: {msbuildPath}");
					Log($"  Exists: {Directory.Exists(msbuildPath)}");

					if (Directory.Exists(msbuildPath))
					{
						toolsetPath = msbuildPath;
					}
					else
					{
						Log("ERROR: No toolset folder found in any location!");
						Log("Continuing anyway to write registry with primary path for debugging...");
						toolsetPath = Path.Combine(installPath, "toolset", "OceanNDK");
					}
				}
			}

			var toolsetPropsPath = Path.Combine(toolsetPath, "Toolset.props");
			Log($"Checking for Toolset.props: {toolsetPropsPath}");
			Log($"  Exists: {File.Exists(toolsetPropsPath)}");

			Log($"Writing to registry: HKCU\\{RegistryKeyPath}");
			using (var key = Registry.CurrentUser.CreateSubKey(RegistryKeyPath))
			{
				if (key != null)
				{
					key.SetValue("ExtensionPath", installPath);
					key.SetValue("ToolsetPath", toolsetPath + "\\");
					key.SetValue("ToolsetPropsExists", File.Exists(toolsetPropsPath) ? "true" : "false");
					key.SetValue("LastRegistered", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"));

					Log($"  ExtensionPath = {installPath}");
					Log($"  ToolsetPath = {toolsetPath}\\");
					Log($"  ToolsetPropsExists = {File.Exists(toolsetPropsPath)}");
					Log("Registry write SUCCESS");
				}
				else
				{
					Log("ERROR: Registry.CreateSubKey returned null!");
				}
			}

			Log("Verifying registry write...");
			using (var verifyKey = Registry.CurrentUser.OpenSubKey(RegistryKeyPath))
			{
				if (verifyKey != null)
				{
					var readBack = verifyKey.GetValue("ToolsetPath") as string;
					Log($"  Read back ToolsetPath = {readBack}");
				}
				else
				{
					Log("  ERROR: Could not open key for verification!");
				}
			}
		}
		catch (Exception exception)
		{
			Log($"EXCEPTION: {exception.GetType().Name}: {exception.Message}");
			Log($"Stack trace: {exception.StackTrace}");
		}

		Log("========================================");
		Log("RegisterExtensionPathAsync - END");
		Log("========================================");
	}

	/**
	 * Registers detected SDK and NDK paths to the registry.
	 */
	private async Task RegisterSdkPathsAsync()
	{
		await Task.Yield();

		Log("========================================");
		Log("RegisterSdkPathsAsync - START");
		Log("========================================");

		try
		{
			var androidSdk = sdkLocatorService_?.FindAndroidSdk();
			var androidNdk = sdkLocatorService_?.FindNdk();

			Log($"Detected Android SDK: {androidSdk ?? "(not found)"}");
			Log($"Detected Android NDK: {androidNdk ?? "(not found)"}");

			using (var key = Registry.CurrentUser.CreateSubKey(RegistryKeyPath))
			{
				if (key != null)
				{
					if (!string.IsNullOrEmpty(androidSdk))
					{
						key.SetValue("AndroidSdkPath", androidSdk);
						Log($"  Wrote AndroidSdkPath = {androidSdk}");
					}

					if (!string.IsNullOrEmpty(androidNdk))
					{
						var ndkPath = androidNdk!.EndsWith("\\") ? androidNdk : androidNdk + "\\";
						key.SetValue("AndroidNdkPath", ndkPath);
						Log($"  Wrote AndroidNdkPath = {ndkPath}");

						var ndkVersion = Path.GetFileName(androidNdk.TrimEnd('\\'));
						key.SetValue("AndroidNdkVersion", ndkVersion);
						Log($"  Wrote AndroidNdkVersion = {ndkVersion}");
					}

					key.SetValue("SdkPathsLastUpdated", DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss"));
					Log("Registry write SUCCESS");
				}
				else
				{
					Log("ERROR: Registry.CreateSubKey returned null!");
				}
			}

			Log("Verifying registry write...");
			using (var verifyKey = Registry.CurrentUser.OpenSubKey(RegistryKeyPath))
			{
				if (verifyKey != null)
				{
					var readBackSdk = verifyKey.GetValue("AndroidSdkPath") as string;
					var readBackNdk = verifyKey.GetValue("AndroidNdkPath") as string;
					Log($"  Read back AndroidSdkPath = {readBackSdk ?? "(null)"}");
					Log($"  Read back AndroidNdkPath = {readBackNdk ?? "(null)"}");
				}
				else
				{
					Log("  ERROR: Could not open key for verification!");
				}
			}
		}
		catch (Exception exception)
		{
			Log($"EXCEPTION: {exception.GetType().Name}: {exception.Message}");
			Log($"Stack trace: {exception.StackTrace}");
		}

		Log("========================================");
		Log("RegisterSdkPathsAsync - END");
		Log("========================================");
	}

	/**
	 * Logs a message to debug output and file.
	 * @param message The message to log
	 */
	private void Log(string message)
	{
		var fullMessage = $"[OceanAndroidExtension] {message}";
		System.Diagnostics.Debug.WriteLine(fullMessage);

		outputService_?.WriteLine(fullMessage);

		try
		{
			var logPath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "OceanAndroidExtension", "debug.log");

			var logDirectory = Path.GetDirectoryName(logPath);
			if (!Directory.Exists(logDirectory))
			{
				Directory.CreateDirectory(logDirectory!);
			}

			File.AppendAllText(logPath, $"{DateTime.Now:yyyy-MM-dd HH:mm:ss.fff} {message}{Environment.NewLine}");
		}
		catch
		{
			// Ignore file logging errors
		}
	}

	/**
	 * Initializes the menu commands.
	 */
	private async Task InitializeCommandsAsync()
	{
		Log("InitializeCommandsAsync - START");
		var commandService = await GetServiceAsync(typeof(IMenuCommandService)) as OleMenuCommandService;
		if (commandService == null)
		{
			Log("InitializeCommandsAsync - ERROR: commandService is null!");
			return;
		}
		Log("InitializeCommandsAsync - commandService obtained");

		var deviceExplorerCmd = new CommandID(CommandSetGuid, DeviceExplorerCommandId);
		commandService.AddCommand(new MenuCommand((sender, eventArgs) =>
		{
			Log("Command CLICKED: Device Explorer");
			_ = JoinableTaskFactory.RunAsync(() => ShowToolWindowAsync<DeviceExplorerWindow>());
		}, deviceExplorerCmd));
		Log($"InitializeCommandsAsync - registered Device Explorer command (ID: 0x{DeviceExplorerCommandId:X4})");

		var logcatCmd = new CommandID(CommandSetGuid, LogcatCommandId);
		commandService.AddCommand(new MenuCommand((sender, eventArgs) =>
		{
			Log("Command CLICKED: Logcat");
			_ = JoinableTaskFactory.RunAsync(() => ShowToolWindowAsync<LogcatWindow>());
		}, logcatCmd));
		Log($"InitializeCommandsAsync - registered Logcat command (ID: 0x{LogcatCommandId:X4})");

		var sdkManagerCmd = new CommandID(CommandSetGuid, SdkManagerCommandId);
		commandService.AddCommand(new MenuCommand((sender, eventArgs) =>
		{
			Log("Command CLICKED: SDK Manager");
			_ = JoinableTaskFactory.RunAsync(() => ShowToolWindowAsync<SdkManagerWindow>());
		}, sdkManagerCmd));
		Log($"InitializeCommandsAsync - registered SDK Manager command (ID: 0x{SdkManagerCommandId:X4})");

		var deployCmd = new CommandID(CommandSetGuid, DeployApkCommandId);
		commandService.AddCommand(new MenuCommand((sender, eventArgs) =>
		{
			Log("Command CLICKED: Deploy and Run");
			_ = JoinableTaskFactory.RunAsync(() => DeployAndRunAsync());
		}, deployCmd));
		Log($"InitializeCommandsAsync - registered Deploy command (ID: 0x{DeployApkCommandId:X4})");

		Log("InitializeCommandsAsync - END");
	}

	/**
	 * Deploys the APK to a connected device and launches the app.
	 */
	private async Task DeployAndRunAsync()
	{
		await JoinableTaskFactory.SwitchToMainThreadAsync();

		try
		{
			outputService_?.WriteLine("========================================");
			outputService_?.WriteLine("Deploying Ocean Android Application...");
			outputService_?.WriteLine("========================================");

			if (adbService_ == null)
			{
				outputService_?.WriteLine("Error: ADB service not initialized.");
				ShowErrorMessage("ADB service not initialized.\n\nThe extension may not have loaded correctly. Please restart Visual Studio.");
				return;
			}

			var dte = await GetServiceAsync(typeof(EnvDTE.DTE)) as EnvDTE.DTE;
			if (dte == null)
			{
				outputService_?.WriteLine("Error: Could not access DTE service.");
				ShowErrorMessage("Could not access Visual Studio services.");
				return;
			}
			if (dte.Solution?.Projects == null || dte.Solution.Projects.Count == 0)
			{
				outputService_?.WriteLine("Error: No project is open.");
				ShowErrorMessage("No project is open. Please open an Android project first.");
				return;
			}

			string? projectDir = null;
			string? gradleDir = null;
			foreach (EnvDTE.Project project in dte.Solution.Projects)
			{
				var projectPath = project.FullName;
				if (!string.IsNullOrEmpty(projectPath))
				{
					var directory = System.IO.Path.GetDirectoryName(projectPath);
					if (directory != null)
					{
						// Check for gradlew.bat in multiple locations
						// 1. Project root (Android Studio style)
						if (System.IO.File.Exists(System.IO.Path.Combine(directory, "gradlew.bat")))
						{
							projectDir = directory;
							gradleDir = directory;
							outputService_?.WriteLine($"Found Android project: {project.Name} (gradlew.bat in root)");
							break;
						}
						// 2. gradle/ subdirectory (Ocean Android template style)
						if (System.IO.File.Exists(System.IO.Path.Combine(directory, "gradle", "gradlew.bat")))
						{
							projectDir = directory;
							gradleDir = System.IO.Path.Combine(directory, "gradle");
							outputService_?.WriteLine($"Found Android project: {project.Name} (gradlew.bat in gradle/)");
							break;
						}
						// 3. Check if it's a vcxproj with OceanNDK platform (native library without Gradle)
						if (projectPath.EndsWith(".vcxproj", StringComparison.OrdinalIgnoreCase))
						{
							try
							{
								var vcxprojContent = System.IO.File.ReadAllText(projectPath);
								if (vcxprojContent.Contains("OceanNDK") || vcxprojContent.Contains("Android"))
								{
									projectDir = directory;
									outputService_?.WriteLine($"Found Android native project: {project.Name} (OceanNDK)");
									// No gradleDir - this is a native-only project
									break;
								}
							}
							catch { }
						}
					}
				}
			}

			if (projectDir == null)
			{
				outputService_?.WriteLine("Error: No Android project found.");
				ShowErrorMessage("No Android project found.\n\nPlease ensure your project is an Ocean Android Application or contains a gradlew.bat file.");
				return;
			}

			// For native-only projects without Gradle, we can't deploy an APK
			if (gradleDir == null)
			{
				outputService_?.WriteLine("Note: This is a native library project without Gradle. Cannot build APK directly.");
				ShowErrorMessage("This is a native library project.\n\nNative libraries must be included in an Android Application project to create an APK.\n\nPlease open an Ocean Android Application project to deploy.");
				return;
			}

			var apkPath = FindApk(projectDir, gradleDir);
			if (apkPath == null)
			{
				outputService_?.WriteLine("Error: APK not found. Please build the project first (Ctrl+Shift+B).");
				ShowErrorMessage("APK not found. Please build the project first using Ctrl+Shift+B.");
				return;
			}

			outputService_?.WriteLine($"Found APK: {apkPath}");

			var devices = await adbService_.GetDevicesAsync(CancellationToken.None);
			if (devices.Count == 0)
			{
				outputService_?.WriteLine("Error: No Android devices connected.");
				ShowErrorMessage("No Android devices connected.\n\nPlease:\n1. Connect your device via USB\n2. Enable USB Debugging in Developer Options\n3. Accept the debugging prompt on your device");
				return;
			}

			// Find first device that is online and ready
			AndroidDevice? targetDevice = null;
			foreach (var device in devices)
			{
				if (device.State == DeviceState.Device)
				{
					targetDevice = device;
					break;
				}
			}

			if (targetDevice == null)
			{
				var deviceStates = string.Join("\n", devices.Select(device => $"  - {device.Serial}: {device.State}"));
				outputService_?.WriteLine($"Error: No ready devices found. Device states:\n{deviceStates}");
				ShowErrorMessage($"No ready Android devices found.\n\nConnected devices:\n{deviceStates}\n\nPlease ensure your device is unlocked and USB debugging is authorized.");
				return;
			}

			outputService_?.WriteLine($"Target device: {targetDevice.Model} ({targetDevice.Serial}) - Android {targetDevice.AndroidVersion}");

			outputService_?.WriteLine("Installing APK...");
			var installSuccess = await adbService_.InstallApkAsync(targetDevice.Serial, apkPath, reinstall: true, CancellationToken.None);
			if (!installSuccess)
			{
				outputService_?.WriteLine("Error: Failed to install APK.");
				ShowErrorMessage("Failed to install APK to device. Check the Output window for details.");
				return;
			}

			outputService_?.WriteLine("APK installed successfully.");

			var apkInfo = await adbService_.GetApkInfoAsync(apkPath, CancellationToken.None);
			if (!apkInfo.HasValue)
			{
				outputService_?.WriteLine("Error: Could not determine package info from APK.");
				ShowErrorMessage("Could not determine package name and activity from APK.\n\nPlease ensure the APK is valid and contains a launchable activity.");
				return;
			}

			outputService_?.WriteLine($"Launching {apkInfo.Value.PackageName}...");
			var launchSuccess = await adbService_.LaunchActivityAsync(targetDevice.Serial, apkInfo.Value.PackageName, apkInfo.Value.ActivityName, CancellationToken.None);
			if (!launchSuccess)
			{
				outputService_?.WriteLine("Warning: Activity launch may have failed.");
				ShowErrorMessage($"The app was installed but may not have launched correctly.\n\nPackage: {apkInfo.Value.PackageName}\nActivity: {apkInfo.Value.ActivityName}");
				return;
			}

			outputService_?.WriteLine("========================================");
			outputService_?.WriteLine("Application launched successfully!");
			outputService_?.WriteLine("========================================");
		}
		catch (Exception exception)
		{
			outputService_?.WriteLine($"Error: {exception.Message}");
			ShowErrorMessage($"Deployment failed: {exception.Message}");
		}
	}

	/**
	 * Finds the APK file in the project directory.
	 * @param projectDir The project directory
	 * @param gradleDir The directory containing gradlew.bat
	 * @return The path to the APK file, nullptr if not found
	 */
	private string? FindApk(string projectDir, string gradleDir)
	{
		// Search paths relative to both project root and gradle directory
		var searchPaths = new[]
		{
			// Standard Gradle output locations (relative to gradle dir)
			System.IO.Path.Combine(gradleDir, "app", "build", "outputs", "apk", "debug"),
			System.IO.Path.Combine(gradleDir, "app", "build", "outputs", "apk", "release"),
			System.IO.Path.Combine(gradleDir, "build", "outputs", "apk", "debug"),
			System.IO.Path.Combine(gradleDir, "build", "outputs", "apk", "release"),
			// Also check project root (in case gradleDir is a subdirectory)
			System.IO.Path.Combine(projectDir, "app", "build", "outputs", "apk", "debug"),
			System.IO.Path.Combine(projectDir, "app", "build", "outputs", "apk", "release"),
			System.IO.Path.Combine(projectDir, "build", "outputs", "apk", "debug"),
			System.IO.Path.Combine(projectDir, "build", "outputs", "apk", "release"),
			// Ocean Android template outputs (bin folder)
			System.IO.Path.Combine(projectDir, "bin", "Debug"),
			System.IO.Path.Combine(projectDir, "bin", "Release"),
			System.IO.Path.Combine(projectDir, "bin", "x64", "Debug"),
			System.IO.Path.Combine(projectDir, "bin", "x64", "Release"),
			System.IO.Path.Combine(projectDir, "bin", "ARM64", "Debug"),
			System.IO.Path.Combine(projectDir, "bin", "ARM64", "Release")
		};

		outputService_?.WriteLine($"Searching for APK in {searchPaths.Length} locations...");

		foreach (var path in searchPaths)
		{
			if (System.IO.Directory.Exists(path))
			{
				var apks = System.IO.Directory.GetFiles(path, "*.apk");
				if (apks.Length > 0)
				{
					outputService_?.WriteLine($"  Found APK in: {path}");
					return apks[0];
				}
			}
		}

		outputService_?.WriteLine("  APK not found in any standard location.");
		return null;
	}

	/**
	 * Shows an error message dialog.
	 * @param message The error message
	 */
	private void ShowErrorMessage(string message)
	{
		VsShellUtilities.ShowMessageBox(this, message, "Ocean Android Extension", OLEMSGICON.OLEMSGICON_CRITICAL, OLEMSGBUTTON.OLEMSGBUTTON_OK, OLEMSGDEFBUTTON.OLEMSGDEFBUTTON_FIRST);
	}

	/**
	 * Shows a tool window.
	 * @tparam T The tool window type
	 */
	private async Task ShowToolWindowAsync<T>() where T : ToolWindowPane
	{
		Log($"ShowToolWindowAsync<{typeof(T).Name}> - START");
		try
		{
			await JoinableTaskFactory.SwitchToMainThreadAsync();
			Log($"ShowToolWindowAsync<{typeof(T).Name}> - switched to main thread");

			Log($"ShowToolWindowAsync<{typeof(T).Name}> - calling FindToolWindowAsync...");
			var window = await FindToolWindowAsync(typeof(T), 0, true, DisposalToken);
			Log($"ShowToolWindowAsync<{typeof(T).Name}> - FindToolWindowAsync returned: {(window != null ? window.GetType().Name : "NULL")}");

			if (window?.Frame is IVsWindowFrame frame)
			{
				Log($"ShowToolWindowAsync<{typeof(T).Name}> - got IVsWindowFrame, calling Show()...");
				var hr = frame.Show();
				Log($"ShowToolWindowAsync<{typeof(T).Name}> - frame.Show() returned HRESULT: 0x{hr:X8}");
				ErrorHandler.ThrowOnFailure(hr);
				Log($"ShowToolWindowAsync<{typeof(T).Name}> - window shown successfully");
			}
			else
			{
				Log($"ShowToolWindowAsync<{typeof(T).Name}> - ERROR: window is null or Frame is not IVsWindowFrame");
				Log($"ShowToolWindowAsync<{typeof(T).Name}> - window: {(window != null ? "not null" : "null")}");
				Log($"ShowToolWindowAsync<{typeof(T).Name}> - window?.Frame: {(window?.Frame != null ? window.Frame.GetType().Name : "null")}");
			}
		}
		catch (Exception exception)
		{
			Log($"ShowToolWindowAsync<{typeof(T).Name}> - EXCEPTION: {exception.GetType().Name}: {exception.Message}");
			Log($"ShowToolWindowAsync<{typeof(T).Name}> - Stack trace: {exception.StackTrace}");
			throw;
		}
		Log($"ShowToolWindowAsync<{typeof(T).Name}> - END");
	}

	/**
	 * Logs the SDK detection status.
	 */
	private async Task LogSdkStatusAsync()
	{
		Log("Detecting SDK paths...");

		await Task.Run(() => { });

		var androidSdk = sdkLocatorService_?.FindAndroidSdk();
		var ndk = sdkLocatorService_?.FindNdk();
		var jdk = sdkLocatorService_?.FindJdk();

		Log($"Android SDK: {androidSdk ?? "Not found"}");
		Log($"Android NDK: {ndk ?? "Not found"}");
		Log($"JDK: {jdk ?? "Not found"}");

		await JoinableTaskFactory.SwitchToMainThreadAsync();

		outputService_?.WriteLine("Ocean Android Extension initialized");
		outputService_?.WriteLine($"  Android SDK: {androidSdk ?? "Not found"}");
		outputService_?.WriteLine($"  Android NDK: {ndk ?? "Not found"}");
		outputService_?.WriteLine($"  JDK: {jdk ?? "Not found"}");
		outputService_?.WriteLine($"  Log file: {Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "OceanAndroidExtension", "debug.log")}");

		if (string.IsNullOrEmpty(androidSdk))
		{
			outputService_?.WriteLine("Warning: Android SDK not found. Please configure in Tools > Options > Android > SDK Paths");
		}
	}

	/**
	 * Returns the async tool window factory for the specified tool window type.
	 * @param toolWindowType The tool window type GUID
	 * @return The async tool window factory
	 */
	public override IVsAsyncToolWindowFactory? GetAsyncToolWindowFactory(Guid toolWindowType)
	{
		ThreadHelper.ThrowIfNotOnUIThread();
		if (toolWindowType == typeof(DeviceExplorerWindow).GUID || toolWindowType == typeof(LogcatWindow).GUID || toolWindowType == typeof(SdkManagerWindow).GUID)
		{
			return this;
		}
		return base.GetAsyncToolWindowFactory(toolWindowType);
	}

	/**
	 * Returns the tool window title.
	 * @param toolWindowType The tool window type
	 * @param id The tool window ID
	 * @return The tool window title
	 */
	protected override string GetToolWindowTitle(Type toolWindowType, int id)
	{
		if (toolWindowType == typeof(DeviceExplorerWindow))
		{
			return "Device Explorer";
		}
		if (toolWindowType == typeof(LogcatWindow))
		{
			return "Logcat";
		}
		if (toolWindowType == typeof(SdkManagerWindow))
		{
			return "Android SDK Manager";
		}
		return base.GetToolWindowTitle(toolWindowType, id);
	}

	/**
	 * Initializes a tool window asynchronously.
	 * @param toolWindowType The tool window type
	 * @param id The tool window ID
	 * @param cancellationToken The cancellation token
	 * @return The tool window initialization context
	 */
	protected override Task<object?> InitializeToolWindowAsync(Type toolWindowType, int id, CancellationToken cancellationToken)
	{
		return Task.FromResult<object?>(this);
	}
}

}
