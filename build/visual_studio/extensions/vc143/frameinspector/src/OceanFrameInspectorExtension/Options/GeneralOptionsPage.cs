/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System.ComponentModel;
using Microsoft.VisualStudio.Shell;

namespace OceanFrameInspectorExtension.Options
{
    /// <summary>
    /// Options page under Tools > Options > Frame Inspector > General.
    /// </summary>
    public class GeneralOptionsPage : DialogPage
    {
        [Category("Memory")]
        [DisplayName("Max Memory Read Size (MB)")]
        [Description("Maximum number of megabytes to read from the debuggee process for a single frame. Default: 100 MB.")]
        [DefaultValue(100)]
        public int MaxMemoryReadSizeMB { get; set; } = 100;

        [Category("Display")]
        [DisplayName("Default Color Map")]
        [Description("The default color map applied to single-channel and floating-point frames.")]
        [DefaultValue(DefaultColorMap.None)]
        public DefaultColorMap DefaultColorMap { get; set; } = DefaultColorMap.None;

        [Category("Behavior")]
        [DisplayName("Auto-Refresh on Break")]
        [Description("Automatically re-read the current expression when the debugger breaks.")]
        [DefaultValue(false)]
        public bool AutoRefreshOnBreak { get; set; } = false;

        [Category("Histogram")]
        [DisplayName("Histogram Bin Count")]
        [Description("Number of bins for the histogram display.")]
        [DefaultValue(256)]
        public int HistogramBinCount { get; set; } = 256;
    }

    public enum DefaultColorMap
    {
        None,
        Jet,
        Turbo,
        Inferno
    }
}
