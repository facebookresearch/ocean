# Ocean FrameInspector Extension — Development Plan

Visual Studio Debug Visualizer for `Ocean::Frame` objects.

**Location:** `xplat/ocean/build/visual_studio/extensions/vc143/frameinspector/`

---

## Status Legend

- [x] Implemented and verified working
- [~] Implemented but needs refinement
- [ ] Not yet implemented

---

## Phase 1: Core Infrastructure (v1.0) — COMPLETE

All Phase 1 goals accomplished and verified working. The extension provides:
- Natvis-based structured display in Watch/Locals/Autos (auto-deployed to Visualizers folder)
- WPF tool window with image viewer, metadata panel, pixel inspector, histogram
- Expression-based and context-menu-based frame inspection
- 30+ pixel format conversions including multi-plane YUV
- 7 color maps (Jet, Turbo, Inferno, Spiral10, Spiral20, Linear, None)
- Format-aware channel splitting for both interleaved and planar formats
- Cursor-centered zoom, aspect-ratio-aware fit, click-drag panning
- VS theme integration (dark/light mode)

### 1.1 Build Scaffolding
- [x] `OceanFrameInspectorExtension.sln` — VS 2022 solution with src/tests folders
- [x] `Directory.Build.props` — Output to `$(OceanRootPath)bin/extensions/frameinspector/`
- [x] `Directory.Packages.props` — Central NuGet versions (VS SDK 17.0, xUnit 2.6.5)
- [x] `src/OceanFrameInspectorExtension/OceanFrameInspectorExtension.csproj` — Old-style, net48, VSIX (modeled on Android extension)
- [x] `src/OceanFrameInspectorExtension/source.extension.vsixmanifest` — VS 2022 [17.0, 18.0)
- [x] `tests/OceanFrameInspectorExtension.Tests/OceanFrameInspectorExtension.Tests.csproj` — SDK-style, xUnit, links imaging sources directly

### 1.2 Package & Commands
- [x] `OceanFrameInspectorExtensionPackage.cs` — AsyncPackage, auto-loads on DebugMode
- [x] `OceanFrameInspectorExtensionPackage.vsct` — Commands:
  - Debug > Windows > **Ocean FrameInspector** — opens the tool window
  - Code editor context menu > **Inspect in Ocean FrameInspector** — only visible when in break mode AND the word under cursor is an `Ocean::Frame` variable (validated by probing `(expression).width_` via DTE). Gets selected text or full word under cursor, opens tool window, auto-inspects.
- [x] `Options/GeneralOptionsPage.cs` — Max memory, default color map, auto-refresh, histogram bins

### 1.3 Natvis
- [x] `Natvis/OceanFrame.natvis` — Watch/Locals/Autos display:
  - `Ocean::FrameType` — `1280x720, FORMAT_RGB24, UpperLeft`
  - `Ocean::FrameType::PixelFormatUnion` — shows enum name for predefined formats, `Generic(Nch, dataType)` for generic formats
  - `Ocean::FrameType::PixelFormatProperties` — channels, dataType, planes
  - `Ocean::Frame` — `1280x720, FORMAT_Y_UV12, UpperLeft, 2 Planes` (singular "1 Plane" when applicable)
  - `Ocean::Frame::Plane` — `1280x720, 3ch, stride=3840B`
  - `Ocean::StackHeapVector` — `{ size=N }` with array expansion
- [x] **Auto-deployment**: Package copies natvis to the user's VS Visualizers folder on init, using `IVsShell(VSSPROPID_VirtualRegistryRoot)` + `AppDataFolder` registry key to find the correct path for any VS version (no hard-coded year numbers). Falls back to scanning `Documents\Visual Studio *\` folders.

### 1.4 Debug Integration
- [x] `Debug/DebuggerIntegration.cs` — IVsDebugger event subscription, mode tracking, DTE debugger + process ID access
- [x] `Debug/MemoryReader.cs` — Expression evaluation via `EnvDTE.Debugger.GetExpression()`, raw memory reads via Win32 `ReadProcessMemory`
- [x] `Debug/FrameReader.cs` — Reads Frame struct: evaluates width/height/pixelFormat/pixelOrigin/planes metadata, then reads per-plane pixel data from debuggee memory

**Architecture note**: The debug layer uses DTE for expression evaluation and Win32 `ReadProcessMemory` for bulk pixel data. An earlier approach using `IDebugStackFrame2` / `IDebugExpression2` / `IDebugMemoryBytes2` was abandoned because obtaining the `IDebugStackFrame2` from `IVsDebugger` is not straightforward in VS 2022 extensions.

### 1.5 Imaging Pipeline
- [x] `Imaging/FrameData.cs` — C# model of captured frame with PlaneData
- [x] `Imaging/PixelFormatInfo.cs` — Decodes 64-bit PixelFormat, all 50 predefined formats mapped
- [x] `Imaging/PixelFormatConverter.cs` — Raw bytes to WPF BitmapSource for all formats:
  - Direct mapping: BGRA32, BGR24
  - Channel reorder: RGBA32, ABGR32, ARGB32, BGR32, RGB32, RGB24
  - Grayscale: Y8, Y16, Y32, Y64, Y10
  - Float: F32, F64 (min/max normalization)
  - YUV single-plane: YUV24, YVU24, YUYV16, UYVY16
  - YUV multi-plane: Y_UV12 (NV12), Y_VU12 (NV21), Y_U_V12 (I420), Y_V_U12 (YV12)
  - YUV 4:4:4 planar: Y_U_V24
  - Planar RGB/BGR: R_G_B24, B_G_R24
  - Packed 16-bit: BGR565, RGB565
  - With alpha: YA16, YUVA32, RGBT32
  - High-bit-depth: RGB48, RGBA64
  - Pixel origin flip for ORIGIN_LOWER_LEFT
- [x] `Imaging/HistogramCalculator.cs` — Per-channel 256-bin histograms with min/max/mean
- [x] `Imaging/ColorMapGenerator.cs` — Color maps matching Ocean's `CV::FrameConverterColorMap`:
  - **Jet** — Analytic piecewise-linear RGB formula
  - **Turbo** — Polynomial approximation of Google's Turbo colormap
  - **Inferno** — Polynomial approximation of Kenneth Moreland's Inferno
  - **Spiral10** — HSV hue rotation (10 cycles) with brightness modulation
  - **Spiral20** — HSV hue rotation (20 cycles) with brightness modulation
  - **Linear** — Grayscale normalization (identity LUT)
- [x] `Imaging/ChannelSplitter.cs` — Format-aware channel extraction supporting:
  - **Interleaved** single-plane (RGB24, BGRA32, etc.): extracts by channel index within pixel
  - **NV12/NV21** (2-plane YUV): Y from plane 0, U/V deinterleaved from plane 1
  - **I420/YV12** (3-plane YUV): each Y/U/V from its own plane
  - **Y_U_V24** (3-plane 4:4:4): each channel from its own plane
  - **Planar RGB/BGR**: each R/G/B from its own plane
  - Dynamic channel name detection (`GetAvailableChannels()`) based on pixel format

### 1.6 WPF Tool Window
- [x] `ToolWindows/FrameInspectorWindow.cs` — ToolWindowPane with deferred package initialization
- [x] `ToolWindows/FrameInspectorControl.xaml` — VS-themed layout (VsBrushes) with:
  - Toolbar: expression input, inspect/refresh, zoom controls, color map (None/Jet/Turbo/Inferno/Spiral10/Spiral20/Linear), dynamic channel selector (adapts to format: Y/U/V for YUV, R/G/B for RGB, etc.)
  - Image viewer with LayoutTransform zoom (mouse wheel zooms toward cursor position) and ScrollViewer-based pan (click+drag via Preview tunneling events, hand cursor, correct scrollbar behavior)
  - Fit-to-Window: calculates aspect-ratio-aware zoom level (`min(viewportW/imageW, viewportH/imageH)`) to fill the viewport edge-to-edge
  - Metadata panel with all frame properties
  - Pixel inspector showing coordinates, channel values, hex, color swatch
  - Collapsible histogram with per-channel polyline visualization (log scale, theme-aware colors via `VsBrushes.ToolWindowTextKey` for single-channel; R/G/B for RGB, B/G/R for BGR, Silver/Cyan/Magenta for YUV) and min/max/mean stats
  - Status bar with loading indicator
  - Empty state placeholder message
- [x] `ToolWindows/FrameInspectorControl.xaml.cs` — Preview mouse event handlers for zoom/pan, histogram polyline drawing, `InspectExpression()` API for context menu integration, `FitImageToWindow()` for aspect-ratio-aware fit

### 1.7 ViewModel
- [x] `ViewModels/InspectorViewModel.cs` — INotifyPropertyChanged with:
  - Expression binding and Inspect/Refresh commands
  - Zoom state (pan handled by ScrollViewer)
  - Color map and dynamic channel selection with live reapplication
  - Channel extraction via `ChannelSplitter.ExtractChannel()` (supports YUV multi-plane)
  - Pixel inspector updates on mouse move
  - Auto-refresh on break mode (optional via SetPackage deferred init)
  - Dynamic channel options based on loaded frame's format
  - Metadata, histogram data, and histogram stat collections

### 1.8 Tests
- [x] `PixelFormatInfoTests.cs` — Decode verification for RGB24, BGRA32, Y8, Y_U_V12, F32, RGBA64, generic
- [x] `PixelFormatConverterTests.cs` — BitmapSource output for Gray8, BGR24, RGB24 swap, BGRA32, RGBA32 swizzle, padding handling, origin flip
- [x] `HistogramCalculatorTests.cs` — Uniform data, linear ramp, multi-channel, min/max/mean accuracy
- [x] `ColorMapGeneratorTests.cs` — LUT shape verification, Jet blue-to-red, identity gray, value range

---

## Known Issues & Lessons Learned

### Natvis VSIX Deployment
VS 2022 does not reliably auto-discover natvis from VSIX assets. Neither `Microsoft.VisualStudio.NativeVisualizers` nor `Microsoft.VisualStudio.Debugger.Visualizer` asset types work. The solution is programmatic copy to the user Visualizers folder on package init, using the registry `AppDataFolder` key for the running VS instance. This is the recommended approach per VS extension experts.

### Natvis Visualizers Folder Discovery
The user Visualizers folder is found via `HKCU\Software\Microsoft\VisualStudio\{instanceId}\AppDataFolder` (read via `IVsShell(VSSPROPID_VirtualRegistryRoot)`). This handles side-by-side installs, non-default paths, and all VS versions without hard-coding year numbers. Fallback: scan `Documents\Visual Studio *\` folders.

### ToolWindowPane Package Lifecycle
`ToolWindowPane.Package` is `null` during the constructor. The control must be created with a null package reference and updated later via `Initialize()`. This is a common VS extension pitfall.

### Old-Style VSIX csproj Requirements
Old-style (non-SDK) VSIX projects require specific setup that differs from SDK-style projects:
- `Microsoft.Common.props` import before PropertyGroups
- `VSToolsPath` property with fallback
- `MinimumVisualStudioVersion` property
- `RuntimeIdentifiers` set to `win`
- `ExcludeAssets="runtime"` on `Microsoft.VisualStudio.SDK` package
- VSSDK targets import condition: `'$(VSToolsPath)' != ''` (not `Exists(...)`)
- `AfterTargets="CreateVsixContainer"` (not `"Build"`) for VSIX copy targets

### Debug API Choice
`IDebugStackFrame2` / `IDebugExpression2` / `IDebugMemoryBytes2` are powerful but require obtaining a stack frame, which is non-trivial from a VS package. `EnvDTE.Debugger.GetExpression()` + Win32 `ReadProcessMemory` is simpler and works reliably.

---

## Phase 2: Refinements (v1.1)

### 2.1 Improved Frame Reading
- [ ] Support for heap-overflow planes in StackHeapVector (planes > 4)
- [ ] Better error messages when expression evaluation fails (show which field failed)
- [ ] Support for reading Frame through pointer indirection (`framePtr->`, `*framePtr`)
- [ ] Handle `const Frame&` references and other common expression forms

### 2.2 UI Improvements
- [ ] Keyboard shortcut for Inspect (Enter key in expression box)
- [ ] Recent expressions dropdown/history
- [ ] Better error display in status bar (red text for errors)
- [ ] Auto-size zoom to fit window on first inspect

### 2.3 Editor Integration Enhancements
- [x] Code editor context menu with type-aware visibility (only shows for Ocean::Frame variables)
- [ ] Watch/Locals context menu: "Inspect in Ocean FrameInspector"
- [ ] DataTip integration: "Inspect" link in hover tooltip for Frame variables

---

## Phase 3: Advanced Imaging (v1.2)

### 3.1 Additional Format Support
- [ ] BGR4444, BGR5551, BGRA4444, RGB4444, RGB5551, RGBA4444 unpacking
- [ ] BGGR10_PACKED and RGGB10_PACKED Bayer demosaic (nearest-neighbor)
- [ ] Y10_PACKED unpacking (5 bytes -> 4 pixels)
- [ ] YUVT32 proper handling
- [ ] Signed integer formats (int8, int16, int32) with proper normalization
- [ ] Float16 support

### 3.2 Visualization Enhancements
- [ ] ROI (Region of Interest) selection with crop statistics
- [ ] Pixel value tooltip overlay on zoomed-in images
- [ ] Grid overlay at high zoom levels showing pixel boundaries
- [ ] Measurement tool (distance between two points)
- [ ] Value range slider for manual min/max normalization
- [ ] Alpha channel visualization (checkerboard pattern)

### 3.3 Histogram Improvements
- [x] Histogram drawn as color-coded polylines in Canvas (log scale, per-channel)
- [ ] Interactive histogram with click-to-select range
- [x] Logarithmic scale (default)
- [ ] Cumulative histogram display
- [ ] Per-plane histograms for multi-plane formats

---

## Phase 4: Multi-Frame & Comparison (v1.3)

### 4.1 Multi-Frame Support
- [ ] Frame comparison mode (side-by-side view of two frames)
- [ ] `std::vector<Frame>` and `Frames` (typedef) navigation
- [ ] Frame history ring buffer (keep last N inspected frames)

### 4.2 Frame Comparison
- [ ] Difference visualization (absolute difference, heatmap)
- [ ] Synchronized zoom/pan between compared frames
- [ ] PSNR, SSIM, MSE quality metrics display
- [ ] Overlay mode (blend two frames with adjustable opacity)

---

## Phase 5: Export (v1.4)

### 5.1 Export
- [ ] Save current view as PNG/BMP/TIFF
- [ ] Copy image to clipboard
- [ ] Export raw pixel data as binary file
- [ ] Export metadata as JSON

---

## Phase 6: Performance & Polish (v1.5)

### 6.1 Performance
- [ ] Progressive loading for large frames (>4K resolution)
- [ ] Background thread for pixel format conversion
- [ ] Memory-mapped reading for very large frames
- [ ] Caching of converted BitmapSource to avoid reconversion

### 6.2 UI Polish
- [ ] Custom toolbar icons
- [ ] Keyboard shortcuts (Ctrl+I for inspect, F5 for refresh, etc.)
- [ ] Drag-and-drop expression from Watch/Locals window
- [ ] Resizable metadata/histogram panels with remembering layout

### 6.3 Documentation
- [ ] README with screenshots and usage guide
- [ ] Inline XML doc comments for all public APIs
- [ ] Contributing guide for adding new pixel format support

---

## Phase 7: Advanced Features (v2.0)

### 7.1 Video/Sequence Support
- [ ] Frame-by-frame stepping with prev/next buttons
- [ ] Timeline scrubber for sequences of frames
- [ ] Recording mode (capture frames as they change)

### 7.2 GPU Frame Support
- [ ] Reading frames from GPU memory (CUDA/OpenGL textures)
- [ ] Display of GPU memory layout information

### 7.3 Func-Eval Conversion
- [ ] Func-eval fallback: call `Ocean::FrameConverter::Comfort::convert()` in debuggee for complex YUV formats
- [ ] Configurable func-eval timeout and safety controls

### 7.4 Custom Expressions
- [ ] User-defined pixel format expressions
- [ ] Scriptable conversions for proprietary formats
- [ ] Plugin system for third-party format support

---

## Architecture Reference

### Project Structure
```
OceanFrameInspectorExtension.sln
├── Directory.Build.props                          # Shared build config
├── Directory.Packages.props                       # Central NuGet versions
├── DEVELOPMENT_PLAN.md                            # This file
├── src/
│   └── OceanFrameInspectorExtension/              # Main VSIX (net48, old-style csproj)
│       ├── source.extension.vsixmanifest
│       ├── OceanFrameInspectorExtensionPackage.cs   # AsyncPackage entry point
│       ├── OceanFrameInspectorExtensionPackage.vsct # Menu commands (window + context menu)
│       ├── Options/
│       │   └── GeneralOptionsPage.cs              # Tools > Options settings
│       ├── ToolWindows/
│       │   ├── FrameInspectorWindow.cs            # ToolWindowPane (deferred init)
│       │   ├── FrameInspectorControl.xaml          # VS-themed WPF layout
│       │   └── FrameInspectorControl.xaml.cs       # Mouse handlers + InspectExpression API
│       ├── Debug/
│       │   ├── DebuggerIntegration.cs             # IVsDebugger events + DTE access
│       │   ├── FrameReader.cs                     # Reads Ocean::Frame via DTE expressions
│       │   └── MemoryReader.cs                    # DTE GetExpression + ReadProcessMemory
│       ├── Imaging/
│       │   ├── FrameData.cs
│       │   ├── PixelFormatInfo.cs
│       │   ├── PixelFormatConverter.cs
│       │   ├── HistogramCalculator.cs
│       │   ├── ColorMapGenerator.cs               # Jet, Turbo, Inferno, Spiral10/20, Linear
│       │   └── ChannelSplitter.cs                 # Format-aware channel extraction
│       ├── ViewModels/
│       │   └── InspectorViewModel.cs
│       └── Natvis/
│           └── OceanFrame.natvis
└── tests/
    └── OceanFrameInspectorExtension.Tests/        # Links imaging sources directly
        ├── PixelFormatInfoTests.cs
        ├── PixelFormatConverterTests.cs
        ├── HistogramCalculatorTests.cs
        └── ColorMapGeneratorTests.cs
```

### Debug Data Flow
```
User types expression        OR    Right-clicks variable in editor
        │                                    │
        │                    ┌───────────────┘
        ▼                    ▼
InspectExpression(expr)  ←── "Inspect in Ocean FrameInspector" context menu
        │
        ▼
DTE.Debugger.GetExpression("(expr).width_")     ─── metadata (uint/ulong values)
DTE.Debugger.GetExpression("(expr).planes_.stackElements_[0].constData_")  ─── pointer
        │
        ▼
Win32 OpenProcess(PROCESS_VM_READ, pid)
Win32 ReadProcessMemory(handle, address, size)    ─── raw pixel bytes
        │
        ▼
PixelFormatConverter.Convert(FrameData)           ─── WPF BitmapSource
  or ColorMapGenerator.Apply(FrameData, colorMap) ─── colorized BitmapSource
  or ChannelSplitter.Split(FrameData)[ch]         ─── single channel view
        │
        ▼
Image control in FrameInspectorControl.xaml       ─── rendered to screen
```

### Key Ocean Types (Memory Layout)

| Type | Field | Offset | Size |
|------|-------|--------|------|
| `FrameType` | `width_` | +0 | 4 (uint) |
| `FrameType` | `height_` | +4 | 4 (uint) |
| `FrameType` | `pixelFormat_` | +8 | 8 (uint64 union) |
| `FrameType` | `pixelOrigin_` | +16 | 4 (uint) |
| `Frame` (extends FrameType) | `planes_` | +24* | StackHeapVector |
| `Frame` | `timestamp_` | varies | Timestamp |
| `Frame::Plane` | `allocatedData_` | +0 | 8 (ptr) |
| `Frame::Plane` | `constData_` | +8 | 8 (ptr) |
| `Frame::Plane` | `data_` | +16 | 8 (ptr) |
| `Frame::Plane` | `width_` | +24 | 4 (uint) |
| `Frame::Plane` | `height_` | +28 | 4 (uint) |
| `Frame::Plane` | `channels_` | +32 | 4 (uint) |
| `Frame::Plane` | `elementTypeSize_` | +36 | 4 (uint) |
| `Frame::Plane` | `paddingElements_` | +40 | 4 (uint) |
| `Frame::Plane` | `strideBytes_` | +44 | 4 (uint) |
| `Frame::Plane` | `bytesPerPixel_` | +48 | 4 (uint) |

*Offsets may vary due to alignment/padding.

### PixelFormat Bit Layout (64-bit)
```
Byte:   |     7      |         6         |         5        |     4      |       3      |       2        |      1      |      0      |
        |   unused   |  height-multiple  |  width-multiple  |   planes   |   data type  | channel number |  predefined pixel format  |
```

### APIs Used
| API | Purpose |
|-----|---------|
| `IVsDebugger` / `IVsDebuggerEvents` | Track debug mode (design/break/run) |
| `IVsShell(VSSPROPID_VirtualRegistryRoot)` | Get VS instance registry root for natvis deployment |
| `EnvDTE.Debugger.GetExpression()` | Evaluate C++ expressions in debuggee |
| `EnvDTE.Debugger.CurrentProcess.ProcessID` | Get debuggee PID for memory reading |
| `EnvDTE.DTE.ActiveDocument.Selection` | Get selected text for context menu inspect |
| Win32 `OpenProcess` + `ReadProcessMemory` | Read raw pixel data from debuggee |
| `AsyncPackage` | Extension entry point |
| `ToolWindowPane` | Tool window hosting |
| `OleMenuCommand` + `BeforeQueryStatus` | Dynamic context menu visibility |
| `VsBrushes` | VS dark/light theme support |

---

## Verification Checklist

All items verified working as of v1.0:

1. [x] **Build**: Open `OceanFrameInspectorExtension.sln` in VS 2022, build Release -> produces `OceanFrameInspectorExtension.vsix`
2. [x] **Install**: Double-click VSIX, restart VS
3. [x] **Natvis auto-deploy**: After first debug session start, `OceanFrame.natvis` appears in the user's VS Visualizers folder automatically (version-gated, any VS version)
4. [x] **Natvis display**: Debug a C++ project using Ocean::Frame, inspect in Watch -> shows `1280x720, FORMAT_RGB24, UpperLeft, 1 Plane`
5. [x] **Generic format display**: Inspect a generic pixel format frame -> shows `Generic(3ch, DT_UNSIGNED_INTEGER_8)`
6. [x] **Tool Window**: Debug > Windows > Ocean FrameInspector -> opens with toolbar, metadata, pixel inspector panels
7. [x] **Inspect via expression**: Type frame variable name, click Inspect -> image renders with metadata
8. [x] **Inspect via context menu**: Right-click on a Frame variable in the editor (in break mode) -> "Inspect in Ocean FrameInspector" -> auto-inspects. Menu hidden for non-Frame variables.
9. [x] **Zoom**: Mouse wheel zooms toward cursor position, Fit button fills viewport (aspect-ratio-aware), 100% resets to pixel-perfect
10. [x] **Pan**: Click+drag pans the image (grab-and-drag with hand cursor), scrollbars work correctly
11. [x] **Histogram**: Expand histogram panel -> per-channel polyline visualization (log scale, theme-aware colors) with min/max/mean stats
12. [x] **Color maps**: Select Jet/Turbo/Inferno/Spiral10/Spiral20/Linear from dropdown -> colorized visualization
13. [x] **Channel view (interleaved)**: Select R/G/B/A for RGB/BGR/RGBA formats -> correct grayscale view of that channel
14. [x] **Channel view (YUV multi-plane)**: Select Y/U/V for NV12/NV21/I420/YV12 formats -> correct grayscale view of each plane/channel
15. [x] **Unit tests**: `dotnet test tests/OceanFrameInspectorExtension.Tests/` -> all pass
