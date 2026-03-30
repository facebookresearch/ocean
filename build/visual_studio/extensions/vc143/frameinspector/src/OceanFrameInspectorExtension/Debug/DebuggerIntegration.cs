/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;
using System.Threading.Tasks;
using Microsoft.VisualStudio;
using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;

namespace OceanFrameInspectorExtension.Debug
{
    /// <summary>
    /// Tracks debugger state (design/break/run) and provides access to debug services.
    /// Uses DTE automation model for expression evaluation and Win32 ReadProcessMemory for pixel data.
    /// </summary>
    public sealed class DebuggerIntegration : IVsDebuggerEvents
    {
        private readonly AsyncPackage _package;
        private IVsDebugger? _debugger;
        private uint _debuggerEventsCookie;
        private EnvDTE.DTE? _dte;

        public bool IsInBreakMode { get; private set; }
        public bool IsDebugging { get; private set; }

        public event EventHandler? EnterBreakMode;
        public event EventHandler? LeaveBreakMode;

        public DebuggerIntegration(AsyncPackage package)
        {
            _package = package ?? throw new ArgumentNullException(nameof(package));
        }

        public async Task InitializeAsync()
        {
            await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();

            _debugger = await _package.GetServiceAsync(typeof(SVsShellDebugger)) as IVsDebugger;
            _dte = await _package.GetServiceAsync(typeof(EnvDTE.DTE)) as EnvDTE.DTE;

            if (_debugger != null)
            {
                _debugger.AdviseDebuggerEvents(this, out _debuggerEventsCookie);

                DBGMODE[] mode = new DBGMODE[1];
                if (_debugger.GetMode(mode) == VSConstants.S_OK)
                {
                    UpdateMode(mode[0]);
                }
            }
        }

        public int OnModeChange(DBGMODE dbgmodeNew)
        {
            UpdateMode(dbgmodeNew);
            return VSConstants.S_OK;
        }

        /// <summary>
        /// Gets the DTE debugger for expression evaluation.
        /// </summary>
        public EnvDTE.Debugger? GetDebugger()
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            return _dte?.Debugger;
        }

        /// <summary>
        /// Gets the process ID of the debuggee process.
        /// </summary>
        public int? GetDebuggeeProcessId()
        {
            ThreadHelper.ThrowIfNotOnUIThread();

            try
            {
                EnvDTE.Process? process = _dte?.Debugger?.CurrentProcess;
                return process?.ProcessID;
            }
            catch
            {
                return null;
            }
        }

        private void UpdateMode(DBGMODE mode)
        {
            bool wasInBreak = IsInBreakMode;

            switch (mode)
            {
                case DBGMODE.DBGMODE_Design:
                    IsDebugging = false;
                    IsInBreakMode = false;
                    break;
                case DBGMODE.DBGMODE_Break:
                    IsDebugging = true;
                    IsInBreakMode = true;
                    break;
                case DBGMODE.DBGMODE_Run:
                    IsDebugging = true;
                    IsInBreakMode = false;
                    break;
            }

            if (!wasInBreak && IsInBreakMode)
                EnterBreakMode?.Invoke(this, EventArgs.Empty);
            else if (wasInBreak && !IsInBreakMode)
                LeaveBreakMode?.Invoke(this, EventArgs.Empty);
        }

        public void Dispose()
        {
            ThreadHelper.ThrowIfNotOnUIThread();

            if (_debugger != null && _debuggerEventsCookie != 0)
            {
                _debugger.UnadviseDebuggerEvents(_debuggerEventsCookie);
                _debuggerEventsCookie = 0;
            }
        }
    }
}
