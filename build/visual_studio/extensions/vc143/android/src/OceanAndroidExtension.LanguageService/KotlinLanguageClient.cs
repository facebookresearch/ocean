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

namespace OceanAndroidExtension.LanguageService
{

/**
 * This class defines the content type for Kotlin files.
 * @ingroup oceanandroidextension
 */
public static class KotlinContentTypeDefinition
{
	/// The Kotlin content type definition.
	[Export]
	[Name("kotlin")]
	[BaseDefinition(CodeRemoteContentDefinition.CodeRemoteContentTypeName)]
	public static ContentTypeDefinition? KotlinContentType;

	/// The Kotlin file extension mapping.
	[Export]
	[FileExtension(".kt")]
	[ContentType("kotlin")]
	public static FileExtensionToContentTypeDefinition? KotlinFileExtension;

	/// The Kotlin script file extension mapping.
	[Export]
	[FileExtension(".kts")]
	[ContentType("kotlin")]
	public static FileExtensionToContentTypeDefinition? KotlinScriptFileExtension;
}

/**
 * This class implements the Language Server Protocol client for Kotlin using the Kotlin Language Server.
 * @ingroup oceanandroidextension
 */
[Export(typeof(ILanguageClient))]
[ContentType("kotlin")]
public class KotlinLanguageClient : ILanguageClient
{
	/// The display name of the language client.
	public string Name => "Kotlin Language Client";

	/// The configuration sections.
	public IEnumerable<string>? ConfigurationSections => new[] { "kotlin" };

	/// The initialization options.
	public object? InitializationOptions => GetInitializationOptions();

	/// The files to watch.
	public IEnumerable<string>? FilesToWatch => new[] { "**/*.kt", "**/*.kts", "**/build.gradle", "**/build.gradle.kts" };

	/// True, to show a notification on initialization failure.
	public bool ShowNotificationOnInitializeFailed => true;

#pragma warning disable CS0067 // Events required by ILanguageClient interface but not used directly
	/// Event raised when starting the language client.
	public event AsyncEventHandler<EventArgs>? StartAsync;

	/// Event raised when stopping the language client.
	public event AsyncEventHandler<EventArgs>? StopAsync;
#pragma warning restore CS0067

	/// The path to the Kotlin Language Server.
	private string? kotlinLsPath_;

	/// The Java home path.
	private string? javaHome_;

	/**
	 * Creates a new Kotlin language client.
	 */
	[ImportingConstructor]
	public KotlinLanguageClient()
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
		if (string.IsNullOrEmpty(kotlinLsPath_))
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
		var message = $"Kotlin language server failed to initialize: {initializationState.StatusMessage}";
		return Task.FromResult<InitializationFailureContext?>(new InitializationFailureContext { FailureMessage = message });
	}

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

		var serverJar = FindKotlinServerJar();
		if (serverJar == null)
		{
			return null;
		}

		var arguments = new List<string>
		{
			"-Xmx1G",
			"-jar", $"\"{serverJar}\""
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

		startInfo.Environment["JAVA_HOME"] = javaHome_;

		var process = Process.Start(startInfo);
		if (process == null)
		{
			return null;
		}

		return new Connection(process.StandardOutput.BaseStream, process.StandardInput.BaseStream);
	}

	/**
	 * Finds the Kotlin Language Server JAR.
	 * @return The path to the server JAR, nullptr if not found
	 */
	private string? FindKotlinServerJar()
	{
		if (string.IsNullOrEmpty(kotlinLsPath_))
		{
			return null;
		}

		var serverDirectory = Path.Combine(kotlinLsPath_, "server", "lib");
		if (Directory.Exists(serverDirectory))
		{
			var jars = Directory.GetFiles(serverDirectory, "kotlin-language-server*.jar");
			if (jars.Length > 0)
			{
				return jars[0];
			}
		}

		var singleJar = Path.Combine(kotlinLsPath_, "kotlin-language-server.jar");
		if (File.Exists(singleJar))
		{
			return singleJar;
		}

		return null;
	}

	/**
	 * Returns the initialization options.
	 * @return The initialization options object
	 */
	private object GetInitializationOptions()
	{
		return new
		{
			storagePath = Path.Combine(Path.GetTempPath(), "kotlin-ls"),
			compiler = new
			{
				jvm = new
				{
					target = "17"
				}
			}
		};
	}

	/**
	 * Detects the Kotlin Language Server and Java paths.
	 */
	private void DetectPaths()
	{
		javaHome_ = Environment.GetEnvironmentVariable("JAVA_HOME");

		var extensionDirectory = Path.GetDirectoryName(typeof(KotlinLanguageClient).Assembly.Location);
		if (extensionDirectory != null)
		{
			kotlinLsPath_ = Path.Combine(extensionDirectory, "kotlin-language-server");
			if (!Directory.Exists(kotlinLsPath_))
			{
				var programFiles = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);
				kotlinLsPath_ = Path.Combine(programFiles, "kotlin-language-server");
			}
		}
	}
}

}
