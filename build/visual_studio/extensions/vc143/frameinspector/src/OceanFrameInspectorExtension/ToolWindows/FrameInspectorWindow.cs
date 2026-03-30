/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System.Runtime.InteropServices;
using Microsoft.VisualStudio.Shell;

namespace OceanFrameInspectorExtension.ToolWindows
{
    /// <summary>
    /// Tool window for the Ocean Frame Inspector.
    /// </summary>
    [Guid("F4C5D6E7-F8A9-0123-ABCD-234567890123")]
    public sealed class FrameInspectorWindow : ToolWindowPane
    {
        public FrameInspectorWindow() : base(null)
        {
            Caption = "Ocean FrameInspector";

            // Package may be null at construction time — pass null and let the control handle it.
            // The package can be set later via Initialize() or by the control looking it up.
            Content = new FrameInspectorControl(null);
        }

        protected override void Initialize()
        {
            base.Initialize();

            // Now Package is available — pass it to the control
            if (Content is FrameInspectorControl control)
            {
                control.SetPackage(Package as OceanFrameInspectorExtensionPackage);
            }
        }
    }
}
