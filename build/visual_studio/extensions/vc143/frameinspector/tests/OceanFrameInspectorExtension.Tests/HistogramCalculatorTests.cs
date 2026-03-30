/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;
using OceanFrameInspectorExtension.Imaging;
using Xunit;

namespace OceanFrameInspectorExtension.Tests
{
    public class HistogramCalculatorTests
    {
        private const ulong FORMAT_Y8 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (1UL << 24) | (1UL << 16) | 30UL;
        private const ulong FORMAT_RGB24 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (1UL << 24) | (3UL << 16) | 11UL;

        [Fact]
        public void UniformGray8HasSingleBinPeak()
        {
            byte fillValue = 128;
            uint width = 4, height = 4;
            byte[] data = new byte[width * height];
            for (int i = 0; i < data.Length; i++)
                data[i] = fillValue;

            PlaneData plane = new PlaneData(data, width, height, 1, 1, 0, width, 1);
            FrameData frame = new FrameData(width, height, new PixelFormatInfo(FORMAT_Y8), PixelOriginType.UpperLeft, new[] { plane });

            HistogramResult result = HistogramCalculator.Calculate(frame);

            Assert.Single(result.Channels);
            Assert.Equal(16u, result.Channels[0].Bins[128]); // All 16 pixels in bin 128
            Assert.Equal(128.0, result.Channels[0].Min);
            Assert.Equal(128.0, result.Channels[0].Max);
            Assert.Equal(128.0, result.Channels[0].Mean);
        }

        [Fact]
        public void LinearRampHasUniformDistribution()
        {
            uint width = 256, height = 1;
            byte[] data = new byte[256];
            for (int i = 0; i < 256; i++)
                data[i] = (byte)i;

            PlaneData plane = new PlaneData(data, width, height, 1, 1, 0, width, 1);
            FrameData frame = new FrameData(width, height, new PixelFormatInfo(FORMAT_Y8), PixelOriginType.UpperLeft, new[] { plane });

            HistogramResult result = HistogramCalculator.Calculate(frame);

            Assert.Single(result.Channels);

            // Each bin should have exactly 1 entry
            for (int i = 0; i < 256; i++)
            {
                Assert.Equal(1u, result.Channels[0].Bins[i]);
            }

            Assert.Equal(0.0, result.Channels[0].Min);
            Assert.Equal(255.0, result.Channels[0].Max);
            Assert.Equal(127.5, result.Channels[0].Mean);
        }

        [Fact]
        public void MultiChannelProducesMultipleHistograms()
        {
            uint width = 2, height = 2;
            uint stride = width * 3;
            // RGB24: fill R=100, G=200, B=50
            byte[] data = new byte[stride * height];
            for (uint y = 0; y < height; y++)
            {
                for (uint x = 0; x < width; x++)
                {
                    int offset = (int)(y * stride + x * 3);
                    data[offset + 0] = 100; // R
                    data[offset + 1] = 200; // G
                    data[offset + 2] = 50;  // B
                }
            }

            PlaneData plane = new PlaneData(data, width, height, 3, 1, 0, stride, 3);
            FrameData frame = new FrameData(width, height, new PixelFormatInfo(FORMAT_RGB24), PixelOriginType.UpperLeft, new[] { plane });

            HistogramResult result = HistogramCalculator.Calculate(frame);

            Assert.Equal(3, result.Channels.Length);

            // Channel 0 (R): all at bin 100
            Assert.Equal(4u, result.Channels[0].Bins[100]);
            Assert.Equal(100.0, result.Channels[0].Mean);

            // Channel 1 (G): all at bin 200
            Assert.Equal(4u, result.Channels[1].Bins[200]);
            Assert.Equal(200.0, result.Channels[1].Mean);

            // Channel 2 (B): all at bin 50
            Assert.Equal(4u, result.Channels[2].Bins[50]);
            Assert.Equal(50.0, result.Channels[2].Mean);
        }

        [Fact]
        public void MinMaxCorrectForTwoValues()
        {
            byte[] data = { 10, 240 };
            PlaneData plane = new PlaneData(data, 2, 1, 1, 1, 0, 2, 1);
            FrameData frame = new FrameData(2, 1, new PixelFormatInfo(FORMAT_Y8), PixelOriginType.UpperLeft, new[] { plane });

            HistogramResult result = HistogramCalculator.Calculate(frame);

            Assert.Equal(10.0, result.Channels[0].Min);
            Assert.Equal(240.0, result.Channels[0].Max);
            Assert.Equal(125.0, result.Channels[0].Mean);
        }

        [Fact]
        public void EmptyFrameThrows()
        {
            Assert.Throws<ArgumentException>(() =>
            {
                FrameData frame = new FrameData(0, 0, new PixelFormatInfo(FORMAT_Y8), PixelOriginType.UpperLeft, Array.Empty<PlaneData>());
                HistogramCalculator.Calculate(frame);
            });
        }
    }
}
