/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;
using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Shell;

namespace OceanFrameInspectorExtension.Debug
{
    /// <summary>
    /// Evaluates debug expressions via DTE and reads raw memory via Win32 ReadProcessMemory.
    /// </summary>
    public sealed class MemoryReader : IDisposable
    {
        private readonly EnvDTE.Debugger _debugger;
        private readonly int _processId;
        private IntPtr _processHandle;

        public MemoryReader(EnvDTE.Debugger debugger, int processId)
        {
            _debugger = debugger ?? throw new ArgumentNullException(nameof(debugger));
            _processId = processId;
        }

        /// <summary>
        /// Evaluates a C++ expression and returns its string value.
        /// </summary>
        public string? EvaluateExpression(string expression)
        {
            ThreadHelper.ThrowIfNotOnUIThread();

            try
            {
                EnvDTE.Expression result = _debugger.GetExpression(expression, false, 5000);
                if (result != null && result.IsValidValue)
                {
                    return result.Value;
                }
            }
            catch
            {
                // Expression evaluation can throw for invalid expressions
            }

            return null;
        }

        /// <summary>
        /// Evaluates an expression to get a numeric value (uint).
        /// </summary>
        public uint? EvaluateUInt(string expression)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            string? value = EvaluateExpression(expression);
            return ParseUInt(value);
        }

        /// <summary>
        /// Evaluates an expression to get a 64-bit unsigned value.
        /// </summary>
        public ulong? EvaluateUInt64(string expression)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            string? value = EvaluateExpression(expression);
            return ParseUInt64(value);
        }

        /// <summary>
        /// Evaluates an expression to get a pointer address.
        /// </summary>
        public ulong? EvaluatePointer(string expression)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            string? value = EvaluateExpression(expression);
            if (value == null) return null;

            value = value.Trim();
            int spaceIdx = value.IndexOf(' ');
            if (spaceIdx > 0)
                value = value.Substring(0, spaceIdx);

            return ParseUInt64(value);
        }

        /// <summary>
        /// Reads raw bytes from the debuggee process using Win32 ReadProcessMemory.
        /// </summary>
        public byte[]? ReadMemory(ulong address, int byteCount)
        {
            if (byteCount <= 0 || address == 0) return null;

            EnsureProcessHandle();
            if (_processHandle == IntPtr.Zero) return null;

            byte[] buffer = new byte[byteCount];

            bool success = NativeMethods.ReadProcessMemory(
                _processHandle,
                new IntPtr((long)address),
                buffer,
                new IntPtr(byteCount),
                out IntPtr bytesRead);

            if (!success || bytesRead.ToInt64() == 0)
                return null;

            if (bytesRead.ToInt64() < byteCount)
            {
                byte[] trimmed = new byte[bytesRead.ToInt64()];
                Array.Copy(buffer, trimmed, bytesRead.ToInt64());
                return trimmed;
            }

            return buffer;
        }

        public void Dispose()
        {
            if (_processHandle != IntPtr.Zero)
            {
                NativeMethods.CloseHandle(_processHandle);
                _processHandle = IntPtr.Zero;
            }
        }

        private void EnsureProcessHandle()
        {
            if (_processHandle != IntPtr.Zero) return;

            _processHandle = NativeMethods.OpenProcess(
                NativeMethods.PROCESS_VM_READ | NativeMethods.PROCESS_QUERY_INFORMATION,
                false,
                _processId);
        }

        private static uint? ParseUInt(string? value)
        {
            if (value == null) return null;
            value = value.Trim();

            if (value.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
            {
                if (uint.TryParse(value.Substring(2), System.Globalization.NumberStyles.HexNumber, null, out uint hexResult))
                    return hexResult;
            }

            if (uint.TryParse(value, out uint result))
                return result;

            return null;
        }

        private static ulong? ParseUInt64(string? value)
        {
            if (value == null) return null;
            value = value.Trim();

            if (value.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
            {
                if (ulong.TryParse(value.Substring(2), System.Globalization.NumberStyles.HexNumber, null, out ulong hexResult))
                    return hexResult;
            }

            if (ulong.TryParse(value, out ulong result))
                return result;

            return null;
        }

        private static class NativeMethods
        {
            public const int PROCESS_VM_READ = 0x0010;
            public const int PROCESS_QUERY_INFORMATION = 0x0400;

            [DllImport("kernel32.dll", SetLastError = true)]
            public static extern IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);

            [DllImport("kernel32.dll", SetLastError = true)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool ReadProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, IntPtr nSize, out IntPtr lpNumberOfBytesRead);

            [DllImport("kernel32.dll", SetLastError = true)]
            [return: MarshalAs(UnmanagedType.Bool)]
            public static extern bool CloseHandle(IntPtr hObject);
        }
    }
}
