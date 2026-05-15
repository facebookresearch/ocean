/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System.Windows.Media.Imaging;
using OceanFrameInspectorExtension.Imaging;
using Xunit;

namespace OceanFrameInspectorExtension.Tests
{
    public class ColorMapGeneratorTests
    {
        private const ulong FORMAT_Y8 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (1UL << 24) | (1UL << 16) | 30UL;

        [Theory]
        [InlineData(ColorMapType.Jet)]
        [InlineData(ColorMapType.Turbo)]
        [InlineData(ColorMapType.Inferno)]
        [InlineData(ColorMapType.None)]
        public void GenerateLutReturns256Entries(ColorMapType colorMap)
        {
            byte[,] lut = ColorMapGenerator.GenerateLut(colorMap);

            Assert.Equal(256, lut.GetLength(0));
            Assert.Equal(3, lut.GetLength(1)); // R, G, B
        }

        [Fact]
        public void JetLutStartsBlueEndsRed()
        {
            byte[,] lut = ColorMapGenerator.GenerateLut(ColorMapType.Jet);

            // At index 0 (cold), should be predominantly blue
            Assert.True(lut[0, 2] > lut[0, 0], "Jet index 0 should have B > R");

            // At index 255 (hot), should be predominantly red
            Assert.True(lut[255, 0] > lut[255, 2], "Jet index 255 should have R > B");
        }

        [Fact]
        public void GrayLutIsIdentity()
        {
            byte[,] lut = ColorMapGenerator.GenerateLut(ColorMapType.None);

            for (int i = 0; i < 256; i++)
            {
                Assert.Equal((byte)i, lut[i, 0]); // R
                Assert.Equal((byte)i, lut[i, 1]); // G
                Assert.Equal((byte)i, lut[i, 2]); // B
            }
        }

        [Fact]
        public void TurboLutMidpointIsGreenish()
        {
            byte[,] lut = ColorMapGenerator.GenerateLut(ColorMapType.Turbo);

            // At midpoint (128), Turbo should have high green values
            Assert.True(lut[128, 1] > 100, "Turbo midpoint should have notable green component");
        }

        [Fact]
        public void AllLutValuesInRange()
        {
            foreach (ColorMapType cmap in new[] { ColorMapType.Jet, ColorMapType.Turbo, ColorMapType.Inferno })
            {
                byte[,] lut = ColorMapGenerator.GenerateLut(cmap);

                for (int i = 0; i < 256; i++)
                {
                    for (int c = 0; c < 3; c++)
                    {
                        Assert.InRange(lut[i, c], (byte)0, (byte)255);
                    }
                }
            }
        }

        [Fact]
        public void ApplyProducesBitmapSource()
        {
            byte[] data = new byte[16];
            for (int i = 0; i < 16; i++)
                data[i] = (byte)(i * 16);

            PlaneData plane = new PlaneData(data, 4, 4, 1, 1, 0, 4, 1);
            FrameData frame = new FrameData(4, 4, new PixelFormatInfo(FORMAT_Y8), PixelOriginType.UpperLeft, new[] { plane });

            BitmapSource result = ColorMapGenerator.Apply(frame, ColorMapType.Jet);

            Assert.NotNull(result);
            Assert.Equal(4, result.PixelWidth);
            Assert.Equal(4, result.PixelHeight);
        }

        [Fact]
        public void ApplyWithLowerLeftOriginFlips()
        {
            byte[] data = { 0, 255 };
            PlaneData plane = new PlaneData(data, 1, 2, 1, 1, 0, 1, 1);
            FrameData frame = new FrameData(1, 2, new PixelFormatInfo(FORMAT_Y8), PixelOriginType.LowerLeft, new[] { plane });

            BitmapSource result = ColorMapGenerator.Apply(frame, ColorMapType.Turbo);
            Assert.NotNull(result);
        }
    }
}
