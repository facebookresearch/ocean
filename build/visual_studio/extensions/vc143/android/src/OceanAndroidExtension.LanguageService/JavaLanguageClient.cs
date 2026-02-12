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
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio.LanguageServer.Client;
using Microsoft.VisualStudio.Threading;
using Microsoft.VisualStudio.Utilities;
using StreamJsonRpc;

namespace OceanAndroidExtension.LanguageService
{

/**
 * This class defines the content type for Java files.
 * @ingroup oceanandroidextension
 */
public static class JavaContentTypeDefinition
{
	/// The Java content type definition.
	[Export]
	[Name("java")]
	[BaseDefinition(CodeRemoteContentDefinition.CodeRemoteContentTypeName)]
	public static ContentTypeDefinition? JavaContentType;

	/// The Java file extension mapping.
	[Export]
	[FileExtension(".java")]
	[ContentType("java")]
	public static FileExtensionToContentTypeDefinition? JavaFileExtension;
}

/**
 * This class implements the Language Server Protocol client for Java using Eclipse JDT Language Server.
 * @ingroup oceanandroidextension
 */
[Export(typeof(ILanguageClient))]
[ContentType("java")]
public class JavaLanguageClient : ILanguageClient, ILanguageClientCustomMessage2
{
	/// The display name of the language client.
	public string Name => "Java Language Client (JDT-LS)";

	/// The configuration sections.
	public IEnumerable<string>? ConfigurationSections => new[] { "java" };

	/// The initialization options.
	public object? InitializationOptions => GetInitializationOptions();

	/// The files to watch.
	public IEnumerable<string>? FilesToWatch => new[] { "**/*.java", "**/pom.xml", "**/build.gradle", "**/build.gradle.kts" };

	/// True, to show a notification on initialization failure.
	public bool ShowNotificationOnInitializeFailed => true;

#pragma warning disable CS0067 // Events required by ILanguageClient interface but not used directly
	/// Event raised when starting the language client.
	public event AsyncEventHandler<EventArgs>? StartAsync;

	/// Event raised when stopping the language client.
	public event AsyncEventHandler<EventArgs>? StopAsync;
#pragma warning restore CS0067

	/// The path to JDT-LS.
	private string? jdtLsPath_;

	/// The Java home path.
	private string? javaHome_;

	/**
	 * Creates a new Java language client.
	 */
	[ImportingConstructor]
	public JavaLanguageClient()
	{
		DetectPaths();
	}

	/**
	 * Activates the language client.
	 * @param token The cancellation token
	 * @return The language server connection, nullptr if activation failed
	 */
	public Task<Connection?> ActivateAsync(CancellationToken token)
	{
		if (string.IsNullOrEmpty(jdtLsPath_) || !Directory.Exists(jdtLsPath_))
		{
			return Task.FromResult<Connection?>(null);
		}

		var connection = StartLanguageServer();
		return Task.FromResult(connection);
	}

	/**
	 * Called when the language client is loaded.
	 */
	public async Task OnLoadedAsync()
	{
		if (StartAsync != null)
		{
			await StartAsync.InvokeAsync(this, EventArgs.Empty);
		}
	}

	/**
	 * Called when the server is initialized.
	 */
	public Task OnServerInitializedAsync()
	{
		return Task.CompletedTask;
	}

	/**
	 * Called when the server fails to initialize.
	 * @param initializationState The initialization info
	 * @return The initialization failure context
	 */
	public Task<InitializationFailureContext?> OnServerInitializeFailedAsync(ILanguageClientInitializationInfo initializationState)
	{
		var message = $"Java language server failed to initialize: {initializationState.StatusMessage}";
		return Task.FromResult<InitializationFailureContext?>(new InitializationFailureContext { FailureMessage = message });
	}

	/**
	 * Attaches for custom messages.
	 * @param rpc The JSON-RPC instance
	 */
	public Task AttachForCustomMessageAsync(JsonRpc rpc)
	{
		return Task.CompletedTask;
	}

	/// The middle layer (nullptr).
	public object? MiddleLayer => null;

	/// The custom message target (nullptr).
	public object? CustomMessageTarget => null;

	/**
	 * Starts the language server.
	 * @return The language server connection, nullptr on failure
	 */
	private Connection? StartLanguageServer()
	{
		if (string.IsNullOrEmpty(javaHome_))
		{
			return null;
		}

		var javaExe = Path.Combine(javaHome_, "bin", "java.exe");
		if (!File.Exists(javaExe))
		{
			return null;
		}

		var launcherJar = FindLauncherJar();
		if (launcherJar == null)
		{
			return null;
		}

		var configDirectory = Path.Combine(jdtLsPath_!, "config_win");
		if (!Directory.Exists(configDirectory))
		{
			configDirectory = Path.Combine(jdtLsPath_!, "config_linux");
		}

		var workspaceDirectory = Path.Combine(Path.GetTempPath(), "jdt-ls-workspace");
		Directory.CreateDirectory(workspaceDirectory);

		var arguments = new List<string>
		{
			"-Declipse.application=org.eclipse.jdt.ls.core.id1",
			"-Dosgi.bundles.defaultStartLevel=4",
			"-Declipse.product=org.eclipse.jdt.ls.core.product",
			"-Dlog.level=ALL",
			"-Xmx1G",
			"--add-modules=ALL-SYSTEM",
			"--add-opens", "java.base/java.util=ALL-UNNAMED",
			"--add-opens", "java.base/java.lang=ALL-UNNAMED",
			"-jar", $"\"{launcherJar}\"",
			"-configuration", $"\"{configDirectory}\"",
			"-data", $"\"{workspaceDirectory}\""
		};

		var startInfo = new ProcessStartInfo
		{
			FileName = javaExe,
			Arguments = string.Join(" ", arguments),
			UseShellExecute = false,
			RedirectStandardInput = true,
			RedirectStandardOutput = true,
			RedirectStandardError = true,
			CreateNoWindow = true
		};

		var process = Process.Start(startInfo);
		if (process == null)
		{
			return null;
		}

		return new Connection(process.StandardOutput.BaseStream, process.StandardInput.BaseStream);
	}

	/**
	 * Finds the launcher JAR for JDT-LS.
	 * @return The path to the launcher JAR, nullptr if not found
	 */
	private string? FindLauncherJar()
	{
		if (string.IsNullOrEmpty(jdtLsPath_))
		{
			return null;
		}

		var pluginsDirectory = Path.Combine(jdtLsPath_, "plugins");
		if (!Directory.Exists(pluginsDirectory))
		{
			return null;
		}

		var launcherPattern = "org.eclipse.equinox.launcher_*.jar";
		var launchers = Directory.GetFiles(pluginsDirectory, launcherPattern);

		return launchers.Length > 0 ? launchers[0] : null;
	}

	/**
	 * Returns the initialization options.
	 * @return The initialization options object
	 */
	private object GetInitializationOptions()
	{
		return new
		{
			settings = new
			{
				java = new
				{
					home = javaHome_,
					configuration = new
					{
						updateBuildConfiguration = "automatic"
					},
					import = new
					{
						gradle = new { enabled = true },
						maven = new { enabled = true }
					},
					autobuild = new { enabled = true }
				}
			}
		};
	}

	/**
	 * Detects the JDT-LS and Java paths.
	 */
	private void DetectPaths()
	{
		javaHome_ = Environment.GetEnvironmentVariable("JAVA_HOME");

		var extensionDirectory = Path.GetDirectoryName(typeof(JavaLanguageClient).Assembly.Location);
		if (extensionDirectory != null)
		{
			jdtLsPath_ = Path.Combine(extensionDirectory, "jdt-ls");
			if (!Directory.Exists(jdtLsPath_))
			{
				var programFiles = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
				jdtLsPath_ = Path.Combine(programFiles, "jdt-ls");
			}
		}
	}
}

}
