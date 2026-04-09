/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Shapes;
using OceanFrameInspectorExtension.Imaging;
using OceanFrameInspectorExtension.ViewModels;

namespace OceanFrameInspectorExtension.ToolWindows
{
    /// <summary>
    /// Code-behind for the FrameInspectorControl.
    /// Handles mouse interactions for zoom/pan and pixel inspection.
    ///
    /// Zoom uses LayoutTransform so the ScrollViewer knows the scaled content size
    /// and scrollbars work correctly. Panning is done by scrolling the ScrollViewer
    /// via Preview (tunneling) mouse events to prevent the ScrollViewer from swallowing them.
    /// </summary>
    public partial class FrameInspectorControl : UserControl
    {
        private readonly InspectorViewModel _viewModel;
        private bool _isPanning;
        private Point _panStart;
        private double _scrollStartH;
        private double _scrollStartV;

        public FrameInspectorControl(OceanFrameInspectorExtensionPackage? package)
        {
            InitializeComponent();

            _viewModel = new InspectorViewModel(package);
            _viewModel.PropertyChanged += OnViewModelPropertyChanged;
            _viewModel.FitToWindowRequested += (s, e) => FitImageToWindow();
            DataContext = _viewModel;
        }

        private void OnViewModelPropertyChanged(object? sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(InspectorViewModel.HistogramResult))
            {
                DrawHistogram();
            }
            else if (e.PropertyName == nameof(InspectorViewModel.ZoomLevel))
            {
                UpdateBitmapScalingMode();
            }
        }

        /// <summary>
        /// At zoom > 1x use NearestNeighbor so individual pixels are sharp.
        /// At zoom &lt;= 1x use HighQuality to avoid nearest-neighbor snapping
        /// artifacts caused by any sub-pixel offset in the layout.
        /// </summary>
        private void UpdateBitmapScalingMode()
        {
            var mode = _viewModel.ZoomLevel > 1.0
                ? BitmapScalingMode.NearestNeighbor
                : BitmapScalingMode.HighQuality;
            RenderOptions.SetBitmapScalingMode(FrameImage, mode);
        }

        /// <summary>
        /// Called after the ToolWindowPane is fully initialized and Package is available.
        /// </summary>
        public void SetPackage(OceanFrameInspectorExtensionPackage? package)
        {
            _viewModel.SetPackage(package);
        }

        /// <summary>
        /// Sets the expression and triggers an inspect — called from the editor context menu.
        /// </summary>
        public void InspectExpression(string expression)
        {
            _viewModel.Expression = expression;
            if (_viewModel.InspectCommand.CanExecute(null))
            {
                _viewModel.InspectCommand.Execute(null);
            }
        }

        /// <summary>
        /// Mouse wheel zoom — zooms toward the mouse position by adjusting scroll offsets.
        /// </summary>
        private void ImageScrollViewer_PreviewMouseWheel(object sender, MouseWheelEventArgs e)
        {
            if (_viewModel.BitmapSource == null) return;

            double oldZoom = _viewModel.ZoomLevel;
            double factor = e.Delta > 0 ? 1.25 : 1.0 / 1.25;
            double newZoom = Math.Max(0.05, Math.Min(50.0, oldZoom * factor));

            if (Math.Abs(newZoom - oldZoom) < 0.001) return;

            Point mouseInView = e.GetPosition(ImageScrollViewer);

            double scrollH = ImageScrollViewer.HorizontalOffset;
            double scrollV = ImageScrollViewer.VerticalOffset;

            double contentX = scrollH + mouseInView.X;
            double contentY = scrollV + mouseInView.Y;

            _viewModel.ZoomLevel = newZoom;
            ImageScrollViewer.UpdateLayout();

            double scale = newZoom / oldZoom;
            ImageScrollViewer.ScrollToHorizontalOffset(Math.Max(0, contentX * scale - mouseInView.X));
            ImageScrollViewer.ScrollToVerticalOffset(Math.Max(0, contentY * scale - mouseInView.Y));

            e.Handled = true;
        }

        /// <summary>
        /// Calculates and applies the zoom level to fit the image within the viewport,
        /// filling either width or height depending on the aspect ratio.
        /// Called by the ViewModel's FitToWindowCommand via the FitToWindow event.
        /// </summary>
        public void FitImageToWindow()
        {
            if (_viewModel.BitmapSource == null) return;

            double viewportW = ImageScrollViewer.ViewportWidth;
            double viewportH = ImageScrollViewer.ViewportHeight;

            if (viewportW <= 0 || viewportH <= 0) return;

            double imageW = _viewModel.BitmapSource.PixelWidth;
            double imageH = _viewModel.BitmapSource.PixelHeight;

            if (imageW <= 0 || imageH <= 0) return;

            // Fit: zoom so the image fills the viewport (touching left/right or top/bottom)
            double zoomX = viewportW / imageW;
            double zoomY = viewportH / imageH;
            _viewModel.ZoomLevel = Math.Min(zoomX, zoomY);

            ImageScrollViewer.UpdateLayout();
            ImageScrollViewer.ScrollToHorizontalOffset(0);
            ImageScrollViewer.ScrollToVerticalOffset(0);
        }

        /// <summary>
        /// Preview tunneling event: start panning before ScrollViewer handles the click.
        /// </summary>
        private void ImageScrollViewer_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            if (_viewModel.BitmapSource == null) return;

            _isPanning = true;
            _panStart = e.GetPosition(ImageScrollViewer);
            _scrollStartH = ImageScrollViewer.HorizontalOffset;
            _scrollStartV = ImageScrollViewer.VerticalOffset;
            ImageScrollViewer.CaptureMouse();
            ImageScrollViewer.Cursor = Cursors.Hand;
            e.Handled = true;
        }

        private void ImageScrollViewer_PreviewMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (_isPanning)
            {
                _isPanning = false;
                ImageScrollViewer.ReleaseMouseCapture();
                ImageScrollViewer.Cursor = Cursors.Arrow;
                e.Handled = true;
            }
        }

        /// <summary>
        /// Preview tunneling event: drag to pan by scrolling the ScrollViewer.
        /// The pixel under the cursor stays fixed (grab-and-drag behavior).
        /// </summary>
        private void ImageScrollViewer_PreviewMouseMove(object sender, MouseEventArgs e)
        {
            if (_isPanning && e.LeftButton == MouseButtonState.Pressed)
            {
                Point current = e.GetPosition(ImageScrollViewer);
                double dx = current.X - _panStart.X;
                double dy = current.Y - _panStart.Y;

                ImageScrollViewer.ScrollToHorizontalOffset(_scrollStartH - dx);
                ImageScrollViewer.ScrollToVerticalOffset(_scrollStartV - dy);
                e.Handled = true;
            }
        }

        /// <summary>
        /// Mouse move over image — updates pixel inspector with coordinates and channel values.
        /// </summary>
        private void FrameImage_MouseMove(object sender, MouseEventArgs e)
        {
            if (_isPanning) return;
            if (_viewModel.BitmapSource == null || _viewModel.CurrentFrameData == null) return;

            Point pos = e.GetPosition(FrameImage);

            int px = (int)(pos.X / _viewModel.ZoomLevel);
            int py = (int)(pos.Y / _viewModel.ZoomLevel);

            _viewModel.UpdatePixelInspector(px, py);
        }

        /// <summary>
        /// Draws histogram polylines on the HistogramCanvas for each channel.
        /// Uses theme-aware colors: for single-channel, picks a color visible on both
        /// dark and light VS themes.
        /// </summary>
        private void DrawHistogram()
        {
            HistogramCanvas.Children.Clear();

            HistogramResult? result = _viewModel.HistogramResult;
            if (result == null || result.Channels.Length == 0) return;

            double canvasWidth = HistogramCanvas.ActualWidth;
            double canvasHeight = HistogramCanvas.ActualHeight;
            if (canvasWidth <= 0 || canvasHeight <= 0)
            {
                canvasWidth = 220;
                canvasHeight = 100;
            }

            Color[] channelColors = GetHistogramColors(result.Channels.Length);
            uint globalMax = GetGlobalMaxBin(result);
            if (globalMax == 0) return;

            double logMax = Math.Log(globalMax + 1);

            for (int c = 0; c < result.Channels.Length && c < channelColors.Length; c++)
            {
                Polyline polyline = CreateHistogramPolyline(
                    result.Channels[c], channelColors[c], canvasWidth, canvasHeight,
                    logMax, result.Channels.Length > 1);
                HistogramCanvas.Children.Add(polyline);
            }
        }

        private Color[] GetHistogramColors(int channelCount)
        {
            Color fgColor = Colors.Black;
            if (TryFindResource(Microsoft.VisualStudio.Shell.VsBrushes.ToolWindowTextKey) is SolidColorBrush scb)
                fgColor = scb.Color;

            if (channelCount == 1)
                return new[] { fgColor };

            if (channelCount == 3)
            {
                string[] names = _viewModel.CurrentFrameData != null
                    ? ChannelSplitter.GetAvailableChannels(_viewModel.CurrentFrameData)
                    : Array.Empty<string>();

                if (names.Length >= 3 && names[0] == "B")
                    return new[] { Colors.DodgerBlue, Colors.LimeGreen, Colors.Red };
                if (names.Length >= 3 && names[0] == "Y")
                    return new[] { Colors.Silver, Colors.Cyan, Colors.Magenta };
                return new[] { Colors.Red, Colors.LimeGreen, Colors.DodgerBlue };
            }

            return new[] { Colors.Red, Colors.LimeGreen, Colors.DodgerBlue, Colors.Yellow };
        }

        private static uint GetGlobalMaxBin(HistogramResult result)
        {
            uint max = 0;
            foreach (ChannelHistogram ch in result.Channels)
                foreach (uint bin in ch.Bins)
                    if (bin > max) max = bin;
            return max;
        }

        private static Polyline CreateHistogramPolyline(ChannelHistogram ch, Color color,
            double width, double height, double logMax, bool multiChannel)
        {
            Polyline polyline = new Polyline
            {
                Stroke = new SolidColorBrush(color),
                StrokeThickness = 1.2,
                Opacity = multiChannel ? 0.7 : 1.0
            };

            double binWidth = width / 256.0;
            for (int i = 0; i < 256; i++)
            {
                double x = i * binWidth;
                double normalizedHeight = Math.Log(ch.Bins[i] + 1) / logMax;
                double y = height - normalizedHeight * height;
                polyline.Points.Add(new Point(x, y));
            }

            return polyline;
        }
    }
}
