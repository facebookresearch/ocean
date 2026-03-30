/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;

namespace OceanFrameInspectorExtension.Imaging
{
    /// <summary>
    /// Computes per-channel 256-bin histograms for frame data.
    /// </summary>
    public static class HistogramCalculator
    {
        /// <summary>
        /// Calculates histogram data for a frame.
        /// </summary>
        public static HistogramResult Calculate(FrameData frame)
        {
            if (frame == null) throw new ArgumentNullException(nameof(frame));
            if (frame.Planes.Length == 0) throw new ArgumentException("Frame has no planes.");

            PlaneData plane = frame.Planes[0];
            int channels = (int)plane.Channels;
            int bpe = frame.PixelFormat.BytesPerElement;

            if (channels == 0) channels = 1;

            uint[][] bins = new uint[channels][];
            double[] mins = new double[channels];
            double[] maxs = new double[channels];
            double[] sums = new double[channels];
            long[] counts = new long[channels];

            for (int c = 0; c < channels; c++)
            {
                bins[c] = new uint[256];
                mins[c] = double.MaxValue;
                maxs[c] = double.MinValue;
            }

            for (uint y = 0; y < plane.Height; y++)
            {
                int rowOffset = (int)(y * plane.StrideBytes);

                for (uint x = 0; x < plane.Width; x++)
                {
                    int pixelOffset = rowOffset + (int)(x * plane.BytesPerPixel);

                    for (int c = 0; c < channels; c++)
                    {
                        int elementOffset = pixelOffset + c * bpe;
                        if (elementOffset >= plane.Data.Length) break;

                        ReadElement(plane.Data, elementOffset, bpe, frame.PixelFormat, out double value, out byte binIndex);

                        bins[c][binIndex]++;
                        if (value < mins[c]) mins[c] = value;
                        if (value > maxs[c]) maxs[c] = value;
                        sums[c] += value;
                        counts[c]++;
                    }
                }
            }

            ChannelHistogram[] channelHistograms = new ChannelHistogram[channels];
            for (int c = 0; c < channels; c++)
            {
                double mean = counts[c] > 0 ? sums[c] / counts[c] : 0;
                channelHistograms[c] = new ChannelHistogram(bins[c], mins[c], maxs[c], mean);
            }

            return new HistogramResult(channelHistograms);
        }

        private static void ReadElement(byte[] data, int offset, int bpe, PixelFormatInfo format, out double value, out byte binIndex)
        {
            if (bpe == 1)
            {
                byte val = data[offset];
                value = val;
                binIndex = val;
            }
            else if (bpe == 2 && !format.IsFloatingPoint)
            {
                ushort val = BitConverter.ToUInt16(data, offset);
                value = val;
                binIndex = (byte)(val >> 8);
            }
            else if (bpe == 4 && format.DataType == OceanDataType.SignedFloat32)
            {
                float val = BitConverter.ToSingle(data, offset);
                value = val;
                binIndex = float.IsNaN(val) || float.IsInfinity(val) ? (byte)0 : (byte)Math.Max(0, Math.Min(255, val * 255.0f));
            }
            else if (bpe == 4)
            {
                uint val = BitConverter.ToUInt32(data, offset);
                value = val;
                binIndex = (byte)(val >> 24);
            }
            else if (bpe == 8 && format.DataType == OceanDataType.SignedFloat64)
            {
                double val = BitConverter.ToDouble(data, offset);
                value = val;
                binIndex = double.IsNaN(val) || double.IsInfinity(val) ? (byte)0 : (byte)Math.Max(0, Math.Min(255, val * 255.0));
            }
            else
            {
                value = data[offset];
                binIndex = data[offset];
            }
        }
    }

    /// <summary>
    /// Histogram data for all channels.
    /// </summary>
    public sealed class HistogramResult
    {
        public ChannelHistogram[] Channels { get; }

        public HistogramResult(ChannelHistogram[] channels)
        {
            Channels = channels ?? throw new ArgumentNullException(nameof(channels));
        }
    }

    /// <summary>
    /// Histogram data for a single channel.
    /// </summary>
    public sealed class ChannelHistogram
    {
        public uint[] Bins { get; }
        public double Min { get; }
        public double Max { get; }
        public double Mean { get; }

        public ChannelHistogram(uint[] bins, double min, double max, double mean)
        {
            Bins = bins ?? throw new ArgumentNullException(nameof(bins));
            Min = min;
            Max = max;
            Mean = mean;
        }
    }
}
