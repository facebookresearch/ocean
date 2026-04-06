/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;
using System.ComponentModel.Design;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using OceanFrameInspectorExtension.Debug;
using OceanFrameInspectorExtension.ToolWindows;
using Task = System.Threading.Tasks.Task;

namespace OceanFrameInspectorExtension
{
    /// <summary>
    /// The main package for the Ocean Frame Inspector extension.
    /// Auto-loads when a debug session is active.
    /// </summary>
    [PackageRegistration(UseManagedResourcesOnly = true, AllowsBackgroundLoading = true)]
    [Guid(PackageGuidString)]
    [ProvideMenuResource("Menus.ctmenu", 1)]
    [ProvideToolWindow(typeof(FrameInspectorWindow), Style = VsDockStyle.Tabbed, Window = "3ae79031-e1bc-11d0-8f78-00a0c9110057")]
    [ProvideAutoLoad(VSConstants.UICONTEXT.Debugging_string, PackageAutoLoadFlags.BackgroundLoad)]
    [ProvideOptionPage(typeof(Options.GeneralOptionsPage), "Frame Inspector", "General", 0, 0, true)]
    public sealed class OceanFrameInspectorExtensionPackage : AsyncPackage
    {
        public const string PackageGuidString = "F1A2B3C4-D5E6-7890-ABCD-EF1234567890";

        public static readonly Guid CommandSetGuid = new Guid("F2A3B4C5-D6E7-8901-BCDE-F12345678901");

        public const int FrameInspectorWindowCommandId = 0x0100;
        public const int InspectFrameContextCommandId = 0x0101;

        public DebuggerIntegration? DebuggerIntegration { get; private set; }

        /// <summary>
        /// Cached expression from BeforeQueryStatus, so the command handler doesn't re-compute it.
        /// </summary>
        private string? _pendingContextExpression;

        protected override async Task InitializeAsync(CancellationToken cancellationToken, IProgress<ServiceProgressData> progress)
        {
            await base.InitializeAsync(cancellationToken, progress);

            await JoinableTaskFactory.SwitchToMainThreadAsync(cancellationToken);

            DeployNatvisFile();

            DebuggerIntegration = new DebuggerIntegration(this);
            await DebuggerIntegration.InitializeAsync();

            OleMenuCommandService? commandService = await GetServiceAsync(typeof(IMenuCommandService)) as OleMenuCommandService;
            if (commandService != null)
            {
                // Debug > Windows > Ocean FrameInspector
                commandService.AddCommand(new MenuCommand(ShowFrameInspectorWindow,
                    new CommandID(CommandSetGuid, FrameInspectorWindowCommandId)));

                // Code editor context menu: Inspect in Ocean FrameInspector
                OleMenuCommand contextCmd = new OleMenuCommand(InspectFrameFromEditor,
                    new CommandID(CommandSetGuid, InspectFrameContextCommandId));
                contextCmd.BeforeQueryStatus += OnBeforeQueryInspectFrameStatus;
                commandService.AddCommand(contextCmd);
            }
        }

        private void ShowFrameInspectorWindow(object sender, EventArgs e)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            ShowToolWindowAndReturn();
        }

        /// <summary>
        /// Shows the FrameInspector window, optionally setting an expression and auto-inspecting.
        /// </summary>
        private FrameInspectorWindow? ShowToolWindowAndReturn()
        {
            ThreadHelper.ThrowIfNotOnUIThread();

            ToolWindowPane window = FindToolWindow(typeof(FrameInspectorWindow), 0, true);
            if (window?.Frame == null) return null;

            IVsWindowFrame windowFrame = (IVsWindowFrame)window.Frame;
            ErrorHandler.ThrowOnFailure(windowFrame.Show());

            return window as FrameInspectorWindow;
        }

        /// <summary>
        /// Context menu handler: uses the cached expression from BeforeQueryStatus,
        /// opens the FrameInspector, and triggers inspect.
        /// </summary>
        private void InspectFrameFromEditor(object sender, EventArgs e)
        {
            ThreadHelper.ThrowIfNotOnUIThread();

            string? expression = _pendingContextExpression;
            _pendingContextExpression = null;

            if (string.IsNullOrWhiteSpace(expression)) return;

            FrameInspectorWindow? window = ShowToolWindowAndReturn();
            if (window?.Content is ToolWindows.FrameInspectorControl control)
            {
                control.InspectExpression(expression!);
            }
        }

        /// <summary>
        /// Only show "Inspect in Ocean FrameInspector" when:
        /// 1. The debugger is in break mode
        /// 2. The word under cursor (or selection) evaluates to an Ocean::Frame
        ///    (checked by probing for the width_ member via dot and arrow access)
        ///
        /// Supported expression types:
        ///   - Ocean::Frame value or Frame&amp; reference  (dot access)
        ///   - Ocean::Frame* raw pointer                   (arrow access)
        ///   - Ocean::FrameRef (ObjectRef&lt;Frame&gt;)    (arrow access)
        ///   - std::shared_ptr&lt;Ocean::Frame&gt;         (arrow access)
        ///   - std::unique_ptr&lt;Ocean::Frame&gt;         (arrow access)
        /// </summary>
        private void OnBeforeQueryInspectFrameStatus(object sender, EventArgs e)
        {
            ThreadHelper.ThrowIfNotOnUIThread();

            if (!(sender is OleMenuCommand cmd)) return;

            _pendingContextExpression = null;
            cmd.Visible = false;

            if (DebuggerIntegration?.IsInBreakMode != true) return;

            string? expression = GetSelectedTextOrWordUnderCursor();
            if (string.IsNullOrWhiteSpace(expression)) return;

            // Probe: check if the expression has a width_ member (i.e., is an Ocean::Frame/FrameType)
            // Try dot access first (values and references), then arrow access (pointers and smart pointers).
            try
            {
                if (!(GetService(typeof(EnvDTE.DTE)) is EnvDTE.DTE dte)) return;

                // Probe 1: dot access - works for Frame values and Frame& references
                EnvDTE.Expression result = dte.Debugger.GetExpression($"({expression}).width_", false, 1000);
                if (result != null && result.IsValidValue)
                {
                    cmd.Visible = true;
                    _pendingContextExpression = expression;
                    return;
                }

                // Probe 2: arrow access - works for Frame*, FrameRef, shared_ptr<Frame>, unique_ptr<Frame>
                result = dte.Debugger.GetExpression($"({expression})->width_", false, 1000);
                if (result == null || !result.IsValidValue) return;

                // Dereference so FrameReader can use uniform dot access on the underlying Frame
                cmd.Visible = true;
                _pendingContextExpression = $"(*({expression}))";
            }
            catch
            {
                // Not a Frame or evaluation failed - keep hidden
            }
        }

        /// <summary>
        /// Gets the selected text in the active editor, or the word under the cursor.
        /// </summary>
        private string? GetSelectedTextOrWordUnderCursor()
        {
            ThreadHelper.ThrowIfNotOnUIThread();

            try
            {
                if (!(GetService(typeof(EnvDTE.DTE)) is EnvDTE.DTE dte)) return null;
                if (dte.ActiveDocument == null) return null;

                EnvDTE.TextSelection? selection = dte.ActiveDocument.Selection as EnvDTE.TextSelection;
                if (selection == null) return null;

                // If there's a selection, use it
                string text = selection.Text?.Trim() ?? "";
                if (!string.IsNullOrEmpty(text)) return text;

                // Otherwise, select the full word under cursor:
                // 1. Move to the start of the word (no selection)
                // 2. Extend selection to the end of the word
                var anchor = selection.ActivePoint.CreateEditPoint();
                selection.WordLeft(false);  // move cursor to word start
                selection.WordRight(true);  // select to word end
                text = selection.Text?.Trim() ?? "";
                selection.MoveToPoint(anchor); // restore cursor position
                return string.IsNullOrEmpty(text) ? null : text;
            }
            catch
            {
                return null;
            }
        }

        public Options.GeneralOptionsPage GetOptionsPage()
        {
            return (Options.GeneralOptionsPage)GetDialogPage(typeof(Options.GeneralOptionsPage));
        }

        /// <summary>
        /// Copies OceanFrame.natvis from the extension install directory to the user's
        /// VS Visualizers folder so the debugger picks it up automatically.
        /// VS does not reliably auto-discover natvis from VSIX assets, so this
        /// copy-on-init is the recommended deployment pattern.
        /// </summary>
        private void DeployNatvisFile()
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            try
            {
                string extensionDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) ?? "";
                string sourcePath = Path.Combine(extensionDir, "Natvis", "OceanFrame.natvis");

                if (!File.Exists(sourcePath)) return;

                string? visualizersDir = GetVisualizersFolder();
                if (visualizersDir == null) return;

                Directory.CreateDirectory(visualizersDir);

                string targetPath = Path.Combine(visualizersDir, "OceanFrame.natvis");

                if (!File.Exists(targetPath) ||
                    File.GetLastWriteTimeUtc(sourcePath) > File.GetLastWriteTimeUtc(targetPath))
                {
                    File.Copy(sourcePath, targetPath, overwrite: true);
                }
            }
            catch
            {
                // Natvis deployment is best-effort - don't break the extension if it fails
            }
        }

        /// <summary>
        /// Finds the Visualizers folder for the running VS instance.
        ///
        /// Strategy (most robust to least):
        /// 1. Read AppDataFolder from HKCU\Software\Microsoft\VisualStudio\{instanceId}
        ///    - works with side-by-side installs, non-default locations, and all VS versions.
        ///    We get the instance registry root from IVsShell to avoid guessing.
        /// 2. Fallback: scan Documents for "Visual Studio *" folders.
        /// </summary>
        private string? GetVisualizersFolder()
        {
            ThreadHelper.ThrowIfNotOnUIThread();

            // Strategy 1: Registry-based - get AppDataFolder for this VS instance
            try
            {
                if (GetService(typeof(SVsShell)) is IVsShell shell)
                {
                    // Get the registry root for this VS instance, e.g., "Software\Microsoft\VisualStudio\17.0_abc123"
                    if (shell.GetProperty((int)__VSSPROPID.VSSPROPID_VirtualRegistryRoot, out object regRootObj) == VSConstants.S_OK
                        && regRootObj is string regRoot
                        && !string.IsNullOrEmpty(regRoot))
                    {
                        using (var key = Microsoft.Win32.Registry.CurrentUser.OpenSubKey(regRoot))
                        {
                            string? appDataFolder = key?.GetValue("AppDataFolder") as string;
                            if (!string.IsNullOrEmpty(appDataFolder))
                            {
                                return Path.Combine(appDataFolder, "Visualizers");
                            }
                        }
                    }

                }
            }
            catch
            {
                // Fall through
            }

            // Strategy 2: scan Documents for "Visual Studio *" folders
            try
            {
                string docs = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
                string[] vsFolders = Directory.GetDirectories(docs, "Visual Studio *");
                if (vsFolders.Length > 0)
                {
                    Array.Sort(vsFolders);
                    return Path.Combine(vsFolders[vsFolders.Length - 1], "Visualizers");
                }
            }
            catch
            {
                // Ignore
            }

            return null;
        }
    }
}
