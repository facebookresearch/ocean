/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;
using System.Windows.Media.Imaging;
using OceanFrameInspectorExtension.Imaging;
using Xunit;
using PixelFormatConverter = OceanFrameInspectorExtension.Imaging.PixelFormatConverter;

namespace OceanFrameInspectorExtension.Tests
{
    public class PixelFormatConverterTests
    {
        private const ulong FORMAT_Y8 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (1UL << 24) | (1UL << 16) | 30UL;
        private const ulong FORMAT_BGR24 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (1UL << 24) | (3UL << 16) | 3UL;
        private const ulong FORMAT_BGRA32 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (1UL << 24) | (4UL << 16) | 8UL;
        private const ulong FORMAT_RGB24 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (1UL << 24) | (3UL << 16) | 11UL;
        private const ulong FORMAT_RGBA32 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (1UL << 24) | (4UL << 16) | 16UL;

        private static FrameData CreateGray8Frame(uint width, uint height, byte fillValue)
        {
            byte[] data = new byte[width * height];
            for (int i = 0; i < data.Length; i++)
                data[i] = fillValue;

            PlaneData plane = new PlaneData(data, width, height, 1, 1, 0, width, 1);
            return new FrameData(width, height, new PixelFormatInfo(FORMAT_Y8), PixelOriginType.UpperLeft, new[] { plane });
        }

        private static FrameData CreateBgr24Frame(uint width, uint height, byte b, byte g, byte r)
        {
            uint stride = width * 3;
            byte[] data = new byte[stride * height];
            for (uint y = 0; y < height; y++)
            {
                for (uint x = 0; x < width; x++)
                {
                    int offset = (int)(y * stride + x * 3);
                    data[offset + 0] = b;
                    data[offset + 1] = g;
                    data[offset + 2] = r;
                }
            }

            PlaneData plane = new PlaneData(data, width, height, 3, 1, 0, stride, 3);
            return new FrameData(width, height, new PixelFormatInfo(FORMAT_BGR24), PixelOriginType.UpperLeft, new[] { plane });
        }

        private static FrameData CreateRgb24Frame(uint width, uint height, byte r, byte g, byte b)
        {
            uint stride = width * 3;
            byte[] data = new byte[stride * height];
            for (uint y = 0; y < height; y++)
            {
                for (uint x = 0; x < width; x++)
                {
                    int offset = (int)(y * stride + x * 3);
                    data[offset + 0] = r;
                    data[offset + 1] = g;
                    data[offset + 2] = b;
                }
            }

            PlaneData plane = new PlaneData(data, width, height, 3, 1, 0, stride, 3);
            return new FrameData(width, height, new PixelFormatInfo(FORMAT_RGB24), PixelOriginType.UpperLeft, new[] { plane });
        }

        [Fact]
        public void ConvertGray8ProducesBitmapSource()
        {
            FrameData frame = CreateGray8Frame(4, 4, 128);
            BitmapSource result = PixelFormatConverter.Convert(frame);

            Assert.NotNull(result);
            Assert.Equal(4, result.PixelWidth);
            Assert.Equal(4, result.PixelHeight);
        }

        [Fact]
        public void ConvertBgr24ProducesBitmapSource()
        {
            FrameData frame = CreateBgr24Frame(8, 8, 255, 0, 0); // Blue
            BitmapSource result = PixelFormatConverter.Convert(frame);

            Assert.NotNull(result);
            Assert.Equal(8, result.PixelWidth);
            Assert.Equal(8, result.PixelHeight);
        }

        [Fact]
        public void ConvertRgb24SwapsChannels()
        {
            // Create a 1x1 red pixel in RGB format
            FrameData frame = CreateRgb24Frame(1, 1, 255, 0, 0); // Red
            BitmapSource result = PixelFormatConverter.Convert(frame);

            Assert.NotNull(result);
            Assert.Equal(1, result.PixelWidth);

            // Read back the pixel - should be in BGR format
            byte[] pixels = new byte[4]; // Bgr24 uses 3 bytes, but stride may be padded
            int stride = (result.Format.BitsPerPixel * result.PixelWidth + 7) / 8;
            byte[] buffer = new byte[stride];
            result.CopyPixels(buffer, stride, 0);

            // BGR order: Blue=0, Green=0, Red=255
            Assert.Equal(0, buffer[0]);   // B
            Assert.Equal(0, buffer[1]);   // G
            Assert.Equal(255, buffer[2]); // R
        }

        [Fact]
        public void ConvertGray8ConstantValueMatchesInput()
        {
            FrameData frame = CreateGray8Frame(2, 2, 200);
            BitmapSource result = PixelFormatConverter.Convert(frame);

            int stride = (result.Format.BitsPerPixel * result.PixelWidth + 7) / 8;
            byte[] buffer = new byte[stride * result.PixelHeight];
            result.CopyPixels(buffer, stride, 0);

            // All pixels should be 200
            for (int i = 0; i < 4; i++)
            {
                Assert.Equal(200, buffer[i]);
            }
        }

        [Fact]
        public void ConvertLowerLeftOriginFlipsVertically()
        {
            // Create a 2x2 grayscale with top-row=100, bottom-row=200
            byte[] data = { 100, 100, 200, 200 };
            PlaneData plane = new PlaneData(data, 2, 2, 1, 1, 0, 2, 1);
            FrameData frame = new FrameData(2, 2, new PixelFormatInfo(FORMAT_Y8), PixelOriginType.LowerLeft, new[] { plane });

            BitmapSource result = PixelFormatConverter.Convert(frame);

            Assert.NotNull(result);
            Assert.Equal(2, result.PixelWidth);
            Assert.Equal(2, result.PixelHeight);
        }

        [Fact]
        public void ConvertHandlesPadding()
        {
            // Create a 3x2 grayscale with stride=4 (1 byte padding per row)
            byte[] data = { 10, 20, 30, 0, 40, 50, 60, 0 };
            PlaneData plane = new PlaneData(data, 3, 2, 1, 1, 1, 4, 1);
            FrameData frame = new FrameData(3, 2, new PixelFormatInfo(FORMAT_Y8), PixelOriginType.UpperLeft, new[] { plane });

            BitmapSource result = PixelFormatConverter.Convert(frame);

            Assert.NotNull(result);
            Assert.Equal(3, result.PixelWidth);
            Assert.Equal(2, result.PixelHeight);
        }

        [Fact]
        public void ConvertBgra32DirectMapping()
        {
            byte[] data = { 255, 0, 0, 128 }; // BGRA: Blue=255, Green=0, Red=0, Alpha=128
            PlaneData plane = new PlaneData(data, 1, 1, 4, 1, 0, 4, 4);
            FrameData frame = new FrameData(1, 1, new PixelFormatInfo(FORMAT_BGRA32), PixelOriginType.UpperLeft, new[] { plane });

            BitmapSource result = PixelFormatConverter.Convert(frame);
            Assert.NotNull(result);
            Assert.Equal(1, result.PixelWidth);
        }

        [Fact]
        public void ConvertRgba32ToSwizzledBgra()
        {
            // RGBA: R=100, G=150, B=200, A=255
            byte[] data = { 100, 150, 200, 255 };
            PlaneData plane = new PlaneData(data, 1, 1, 4, 1, 0, 4, 4);
            FrameData frame = new FrameData(1, 1, new PixelFormatInfo(FORMAT_RGBA32), PixelOriginType.UpperLeft, new[] { plane });

            BitmapSource result = PixelFormatConverter.Convert(frame);
            Assert.NotNull(result);

            // Read back - should be BGRA order
            byte[] buffer = new byte[4];
            result.CopyPixels(buffer, 4, 0);
            Assert.Equal(200, buffer[0]); // B
            Assert.Equal(150, buffer[1]); // G
            Assert.Equal(100, buffer[2]); // R
            Assert.Equal(255, buffer[3]); // A
        }
    }
}
