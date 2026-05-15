/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using Microsoft.VisualStudio.Shell;
using OceanFrameInspectorExtension.Debug;
using PixelFormatConverter = OceanFrameInspectorExtension.Imaging.PixelFormatConverter;
using OceanFrameInspectorExtension.Imaging;

namespace OceanFrameInspectorExtension.ViewModels
{
    /// <summary>
    /// Main ViewModel for the Frame Inspector tool window.
    /// </summary>
    public sealed class InspectorViewModel : INotifyPropertyChanged
    {
        private OceanFrameInspectorExtensionPackage? _package;

        private string _expression = "";
        private FrameData? _currentFrameData;
        private BitmapSource? _bitmapSource;
        private double _zoomLevel = 1.0;
        private string _statusMessage = "Ready. Enter a C++ expression and click Inspect.";
        private bool _isLoading;
        private string _selectedColorMap = "None";
        private string _selectedChannel = "All";
        private string _pixelCoordinateText = "";
        private string _pixelValueText = "";
        private string _pixelHexText = "";
        private Color _pixelColor = Colors.Transparent;
        private bool _isHistogramExpanded;

        public InspectorViewModel(OceanFrameInspectorExtensionPackage? package)
        {
            _package = package;

            InspectCommand = new RelayCommand(ExecuteInspect, CanInspect);
            RefreshCommand = new RelayCommand(ExecuteRefresh, CanRefresh);
            ZoomInCommand = new RelayCommand(_ => ZoomLevel *= 1.25);
            ZoomOutCommand = new RelayCommand(_ => ZoomLevel /= 1.25);
            FitToWindowCommand = new RelayCommand(_ => FitToWindowRequested?.Invoke(this, EventArgs.Empty));
            ResetZoomCommand = new RelayCommand(_ => ZoomLevel = 1.0);

            // Subscribe to auto-refresh
            if (_package?.DebuggerIntegration != null)
            {
                _package.DebuggerIntegration.EnterBreakMode += OnEnterBreakMode;
            }
        }

        /// <summary>
        /// Called when the Package becomes available after ToolWindowPane.Initialize().
        /// </summary>
        public void SetPackage(OceanFrameInspectorExtensionPackage? package)
        {
            if (_package != null || package == null) return;
            _package = package;

            if (_package.DebuggerIntegration != null)
            {
                _package.DebuggerIntegration.EnterBreakMode += OnEnterBreakMode;
            }
        }

        public event PropertyChangedEventHandler? PropertyChanged;

        /// <summary>
        /// Fired when the Fit button is clicked — the view should calculate the correct zoom.
        /// </summary>
        public event EventHandler? FitToWindowRequested;

        // Properties
        public string Expression
        {
            get => _expression;
            set { _expression = value; OnPropertyChanged(); }
        }

        public FrameData? CurrentFrameData
        {
            get => _currentFrameData;
            private set { _currentFrameData = value; OnPropertyChanged(); }
        }

        public BitmapSource? BitmapSource
        {
            get => _bitmapSource;
            private set { _bitmapSource = value; OnPropertyChanged(); OnPropertyChanged(nameof(HasNoImage)); }
        }

        public bool HasNoImage => _bitmapSource == null;

        public double ZoomLevel
        {
            get => _zoomLevel;
            set
            {
                _zoomLevel = Math.Max(0.05, Math.Min(50.0, value));
                OnPropertyChanged();
                OnPropertyChanged(nameof(ZoomPercentText));
            }
        }

        public string ZoomPercentText => $"{ZoomLevel * 100:F0}%";

        public string StatusMessage
        {
            get => _statusMessage;
            set { _statusMessage = value; OnPropertyChanged(); }
        }

        public bool IsLoading
        {
            get => _isLoading;
            set { _isLoading = value; OnPropertyChanged(); }
        }

        public string SelectedColorMap
        {
            get => _selectedColorMap;
            set
            {
                _selectedColorMap = value;
                OnPropertyChanged();
                ReapplyConversion();
            }
        }

        public string SelectedChannel
        {
            get => _selectedChannel;
            set
            {
                _selectedChannel = value;
                OnPropertyChanged();
                ReapplyConversion();
            }
        }

        public string PixelCoordinateText
        {
            get => _pixelCoordinateText;
            set { _pixelCoordinateText = value; OnPropertyChanged(); }
        }

        public string PixelValueText
        {
            get => _pixelValueText;
            set { _pixelValueText = value; OnPropertyChanged(); }
        }

        public string PixelHexText
        {
            get => _pixelHexText;
            set { _pixelHexText = value; OnPropertyChanged(); }
        }

        public Color PixelColor
        {
            get => _pixelColor;
            set { _pixelColor = value; OnPropertyChanged(); }
        }

        public bool IsHistogramExpanded
        {
            get => _isHistogramExpanded;
            set { _isHistogramExpanded = value; OnPropertyChanged(); }
        }

        public ObservableCollection<KeyValuePair<string, string>> MetadataItems { get; } = new ObservableCollection<KeyValuePair<string, string>>();
        public ObservableCollection<string> HistogramStats { get; } = new ObservableCollection<string>();

        public string[] ColorMapOptions { get; } = { "None", "Jet", "Turbo", "Inferno", "Spiral10", "Spiral20", "Linear" };
        private string[] _channelOptions = { "All" };
        public string[] ChannelOptions
        {
            get => _channelOptions;
            private set { _channelOptions = value; OnPropertyChanged(); }
        }

        // Commands
        public ICommand InspectCommand { get; }
        public ICommand RefreshCommand { get; }
        public ICommand ZoomInCommand { get; }
        public ICommand ZoomOutCommand { get; }
        public ICommand FitToWindowCommand { get; }
        public ICommand ResetZoomCommand { get; }

        public void UpdatePixelInspector(int x, int y)
        {
            if (_currentFrameData == null || _currentFrameData.Planes.Length == 0) return;

            PlaneData plane = _currentFrameData.Planes[0];
            if (x < 0 || x >= plane.Width || y < 0 || y >= plane.Height) return;

            PixelCoordinateText = $"({x}, {y})";

            int bpe = _currentFrameData.PixelFormat.BytesPerElement;
            int channels = (int)plane.Channels;
            int pixelOffset = (int)(y * plane.StrideBytes + x * plane.BytesPerPixel);

            if (pixelOffset + channels * bpe > plane.Data.Length) return;

            // Read channel values
            string[] channelNames = GetChannelNames(_currentFrameData.PixelFormat);
            List<string> values = new List<string>();
            byte r = 0, g = 0, b = 0, a = 255;

            for (int c = 0; c < channels && c < channelNames.Length; c++)
            {
                int offset = pixelOffset + c * bpe;
                if (bpe == 1)
                {
                    byte val = plane.Data[offset];
                    values.Add($"{channelNames[c]}={val}");
                    AssignChannelColor(channelNames[c], val, ref r, ref g, ref b, ref a);
                }
                else if (bpe == 2)
                {
                    ushort val = BitConverter.ToUInt16(plane.Data, offset);
                    values.Add($"{channelNames[c]}={val}");
                    AssignChannelColor(channelNames[c], (byte)(val >> 8), ref r, ref g, ref b, ref a);
                }
                else if (bpe == 4 && _currentFrameData.PixelFormat.DataType == OceanDataType.SignedFloat32)
                {
                    float val = BitConverter.ToSingle(plane.Data, offset);
                    values.Add($"{channelNames[c]}={val:F4}");
                    AssignChannelColor(channelNames[c], (byte)Math.Max(0, Math.Min(255, val * 255)), ref r, ref g, ref b, ref a);
                }
                else
                {
                    byte val = plane.Data[offset];
                    values.Add($"{channelNames[c]}={val}");
                    AssignChannelColor(channelNames[c], val, ref r, ref g, ref b, ref a);
                }
            }

            PixelValueText = string.Join("  ", values);
            PixelHexText = $"Hex: #{r:X2}{g:X2}{b:X2}";
            PixelColor = Color.FromArgb(a, r, g, b);
        }

        private static string[] GetChannelNames(PixelFormatInfo format)
        {
            string name = format.Name;

            if (name.Contains("BGRA")) return new[] { "B", "G", "R", "A" };
            if (name.Contains("BGR")) return new[] { "B", "G", "R" };
            if (name.Contains("RGBA")) return new[] { "R", "G", "B", "A" };
            if (name.Contains("RGB")) return new[] { "R", "G", "B" };
            if (name.Contains("ABGR")) return new[] { "A", "B", "G", "R" };
            if (name.Contains("ARGB")) return new[] { "A", "R", "G", "B" };
            if (name.Contains("YUVA")) return new[] { "Y", "U", "V", "A" };
            if (name.Contains("YUV") || name.Contains("YVU")) return new[] { "Y", "U", "V" };
            if (name.Contains("YA")) return new[] { "Y", "A" };
            if (name.Contains("Y")) return new[] { "Y" };
            if (name.Contains("F32") || name.Contains("F64")) return new[] { "V" };

            // Generic
            string[] generic = new string[format.Channels];
            for (int i = 0; i < format.Channels; i++)
                generic[i] = $"Ch{i}";
            return generic;
        }

        private static void AssignChannelColor(string name, byte value, ref byte r, ref byte g, ref byte b, ref byte a)
        {
            switch (name)
            {
                case "R": r = value; break;
                case "G": g = value; break;
                case "B": b = value; break;
                case "A": a = value; break;
                case "Y": r = g = b = value; break;
                case "V":
                case "Ch0": r = g = b = value; break;
            }
        }

        private void ExecuteInspect(object? parameter)
        {
            if (string.IsNullOrWhiteSpace(Expression)) return;

            ThreadHelper.ThrowIfNotOnUIThread();

            try
            {
                IsLoading = true;
                StatusMessage = $"Reading frame from expression: {Expression}...";

                DebuggerIntegration? debugIntegration = _package?.DebuggerIntegration;
                if (debugIntegration == null || !debugIntegration.IsInBreakMode)
                {
                    StatusMessage = "Error: Not in break mode. Pause the debugger first.";
                    return;
                }

                EnvDTE.Debugger? dteDebugger = debugIntegration.GetDebugger();
                int? processId = debugIntegration.GetDebuggeeProcessId();
                if (dteDebugger == null || processId == null)
                {
                    StatusMessage = "Error: Could not access debugger services.";
                    return;
                }

                int maxMB = _package?.GetOptionsPage()?.MaxMemoryReadSizeMB ?? 100;
                FrameData? frameData;
                using (FrameReader reader = new FrameReader(dteDebugger, processId.Value, maxMB))
                {
                    frameData = reader.ReadFrame(Expression);
                }

                if (frameData == null)
                {
                    StatusMessage = "Error: Could not read frame data. Verify the expression evaluates to an Ocean::Frame.";
                    return;
                }

                CurrentFrameData = frameData;
                UpdateDisplay();
                StatusMessage = $"Loaded: {frameData.Width}x{frameData.Height} {frameData.PixelFormat.Name} ({frameData.TotalBytes:N0} bytes)";
            }
            catch (Exception ex)
            {
                StatusMessage = $"Error: {ex.Message}";
            }
            finally
            {
                IsLoading = false;
            }
        }

        private bool CanInspect(object? parameter) => !string.IsNullOrWhiteSpace(Expression);

        private void ExecuteRefresh(object? parameter)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            ExecuteInspect(parameter);
        }

        private bool CanRefresh(object? parameter) => !string.IsNullOrWhiteSpace(Expression);

        private void OnEnterBreakMode(object? sender, EventArgs e)
        {
            if (_package?.GetOptionsPage()?.AutoRefreshOnBreak == true && !string.IsNullOrWhiteSpace(Expression))
            {
                _ = _package.JoinableTaskFactory.RunAsync(async () =>
                {
                    await ThreadHelper.JoinableTaskFactory.SwitchToMainThreadAsync();
                    ExecuteInspect(null);
                });
            }
        }

        private void UpdateDisplay()
        {
            if (_currentFrameData == null) return;

            // Apply color map or channel selection
            ReapplyConversion();

            // Update channel options based on the format
            UpdateChannelOptions();

            // Update metadata
            UpdateMetadata();

            // Update histogram
            UpdateHistogram();
        }

        private void UpdateChannelOptions()
        {
            if (_currentFrameData == null) return;

            string[] available = ChannelSplitter.GetAvailableChannels(_currentFrameData);
            string[] options = new string[available.Length + 1];
            options[0] = "All";
            Array.Copy(available, 0, options, 1, available.Length);

            ChannelOptions = options;
            if (Array.IndexOf(options, _selectedChannel) < 0)
            {
                SelectedChannel = "All";
            }
        }

        private void ReapplyConversion()
        {
            if (_currentFrameData == null) return;

            try
            {
                FrameData displayFrame = _currentFrameData;

                // Channel extraction — uses format-aware splitter for both interleaved and multi-plane
                if (_selectedChannel != "All")
                {
                    FrameData? extracted = ChannelSplitter.ExtractChannel(_currentFrameData, _selectedChannel);
                    if (extracted != null)
                    {
                        displayFrame = extracted;
                    }
                }

                // Color map
                if (_selectedColorMap != "None" && Enum.TryParse(_selectedColorMap, out ColorMapType cmap))
                {
                    BitmapSource = ColorMapGenerator.Apply(displayFrame, cmap);
                }
                else
                {
                    BitmapSource = PixelFormatConverter.Convert(displayFrame);
                }
            }
            catch (Exception ex)
            {
                StatusMessage = $"Conversion error: {ex.Message}";
            }
        }

        private void UpdateMetadata()
        {
            if (_currentFrameData == null) return;

            MetadataItems.Clear();
            MetadataItems.Add(new KeyValuePair<string, string>("Size", $"{_currentFrameData.Width} x {_currentFrameData.Height}"));
            MetadataItems.Add(new KeyValuePair<string, string>("Format", _currentFrameData.PixelFormat.Name));
            MetadataItems.Add(new KeyValuePair<string, string>("Origin", _currentFrameData.PixelOrigin.ToString()));
            MetadataItems.Add(new KeyValuePair<string, string>("Planes", _currentFrameData.Planes.Length.ToString()));
            MetadataItems.Add(new KeyValuePair<string, string>("Channels", _currentFrameData.PixelFormat.Channels.ToString()));
            MetadataItems.Add(new KeyValuePair<string, string>("Data Type", _currentFrameData.PixelFormat.DataTypeName));
            MetadataItems.Add(new KeyValuePair<string, string>("Bytes/Element", _currentFrameData.PixelFormat.BytesPerElement.ToString()));

            if (_currentFrameData.Planes.Length > 0)
            {
                PlaneData p = _currentFrameData.Planes[0];
                MetadataItems.Add(new KeyValuePair<string, string>("Stride", $"{p.StrideBytes} bytes"));
                MetadataItems.Add(new KeyValuePair<string, string>("Padding", p.PaddingElements.ToString()));
                MetadataItems.Add(new KeyValuePair<string, string>("Continuous", p.IsContinuous ? "Yes" : "No"));
                MetadataItems.Add(new KeyValuePair<string, string>("Bytes/Pixel", p.BytesPerPixel.ToString()));
            }

            MetadataItems.Add(new KeyValuePair<string, string>("Total Bytes", $"{_currentFrameData.TotalBytes:N0}"));
        }

        private HistogramResult? _histogramResult;
        public HistogramResult? HistogramResult
        {
            get => _histogramResult;
            private set { _histogramResult = value; OnPropertyChanged(); }
        }

        private void UpdateHistogram()
        {
            if (_currentFrameData == null) return;

            HistogramStats.Clear();
            HistogramResult = null;

            try
            {
                HistogramResult histogram = HistogramCalculator.Calculate(_currentFrameData);

                string[] channelNames = ChannelSplitter.GetAvailableChannels(_currentFrameData);
                if (channelNames.Length == 0) channelNames = new[] { "V" };

                for (int c = 0; c < histogram.Channels.Length && c < channelNames.Length; c++)
                {
                    ChannelHistogram ch = histogram.Channels[c];
                    HistogramStats.Add($"{channelNames[c]}: Min={ch.Min:F1} Max={ch.Max:F1} Mean={ch.Mean:F1}");
                }

                HistogramResult = histogram;
            }
            catch
            {
                HistogramStats.Add("Could not compute histogram.");
            }
        }

        private void OnPropertyChanged([CallerMemberName] string? propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    /// <summary>
    /// Simple ICommand implementation for the ViewModel.
    /// </summary>
    public sealed class RelayCommand : ICommand
    {
        private readonly Action<object?> _execute;
        private readonly Func<object?, bool>? _canExecute;

        public RelayCommand(Action<object?> execute, Func<object?, bool>? canExecute = null)
        {
            _execute = execute ?? throw new ArgumentNullException(nameof(execute));
            _canExecute = canExecute;
        }

        public event EventHandler? CanExecuteChanged
        {
            add => CommandManager.RequerySuggested += value;
            remove => CommandManager.RequerySuggested -= value;
        }

        public bool CanExecute(object? parameter) => _canExecute?.Invoke(parameter) ?? true;
        public void Execute(object? parameter) => _execute(parameter);
    }
}
