/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using OceanFrameInspectorExtension.Imaging;
using Xunit;

namespace OceanFrameInspectorExtension.Tests
{
    public class PixelFormatInfoTests
    {
        // GenericPixelFormat value for FORMAT_RGB24:
        // predefinedId=11, channels=3, dataType=1 (uint8), planes=1, widthMult=1, heightMult=1
        // = (1 << 48) | (1 << 40) | (1 << 32) | (1 << 24) | (3 << 16) | 11
        private const ulong FORMAT_RGB24 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (1UL << 24) | (3UL << 16) | 11UL;

        // FORMAT_BGRA32: predefinedId=8, channels=4, dataType=1, planes=1, widthMult=1, heightMult=1
        private const ulong FORMAT_BGRA32 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (1UL << 24) | (4UL << 16) | 8UL;

        // FORMAT_Y8: predefinedId=30, channels=1, dataType=1, planes=1, widthMult=1, heightMult=1
        private const ulong FORMAT_Y8 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (1UL << 24) | (1UL << 16) | 30UL;

        // FORMAT_Y_U_V12: predefinedId=20, channels=0 (non-generic), dataType=1, planes=3, widthMult=2, heightMult=2
        private const ulong FORMAT_Y_U_V12 = (2UL << 48) | (2UL << 40) | (3UL << 32) | (1UL << 24) | (0UL << 16) | 20UL;

        // FORMAT_F32: predefinedId=46, channels=1, dataType=10 (float32), planes=1, widthMult=1, heightMult=1
        private const ulong FORMAT_F32 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (10UL << 24) | (1UL << 16) | 46UL;

        // FORMAT_RGBA64: predefinedId=38, channels=4, dataType=3 (uint16), planes=1, widthMult=1, heightMult=1
        private const ulong FORMAT_RGBA64 = (1UL << 48) | (1UL << 40) | (1UL << 32) | (3UL << 24) | (4UL << 16) | 38UL;

        [Fact]
        public void DecodesRgb24()
        {
            var info = new PixelFormatInfo(FORMAT_RGB24);

            Assert.Equal(11, info.PredefinedId);
            Assert.Equal(3, info.Channels);
            Assert.Equal(OceanDataType.UnsignedInteger8, info.DataType);
            Assert.Equal(1, info.PlaneCount);
            Assert.Equal(1, info.WidthMultiple);
            Assert.Equal(1, info.HeightMultiple);
            Assert.Equal(1, info.BytesPerElement);
            Assert.False(info.IsFloatingPoint);
            Assert.False(info.IsMultiPlane);
            Assert.Equal("FORMAT_RGB24", info.Name);
        }

        [Fact]
        public void DecodesBgra32()
        {
            var info = new PixelFormatInfo(FORMAT_BGRA32);

            Assert.Equal(8, info.PredefinedId);
            Assert.Equal(4, info.Channels);
            Assert.Equal(OceanDataType.UnsignedInteger8, info.DataType);
            Assert.Equal(1, info.PlaneCount);
            Assert.Equal(1, info.BytesPerElement);
            Assert.Equal("FORMAT_BGRA32", info.Name);
        }

        [Fact]
        public void DecodesY8()
        {
            var info = new PixelFormatInfo(FORMAT_Y8);

            Assert.Equal(30, info.PredefinedId);
            Assert.Equal(1, info.Channels);
            Assert.Equal(OceanDataType.UnsignedInteger8, info.DataType);
            Assert.Equal(1, info.PlaneCount);
            Assert.Equal("FORMAT_Y8", info.Name);
        }

        [Fact]
        public void DecodesMultiPlaneYuv()
        {
            var info = new PixelFormatInfo(FORMAT_Y_U_V12);

            Assert.Equal(20, info.PredefinedId);
            Assert.Equal(0, info.Channels); // non-generic
            Assert.Equal(OceanDataType.UnsignedInteger8, info.DataType);
            Assert.Equal(3, info.PlaneCount);
            Assert.Equal(2, info.WidthMultiple);
            Assert.Equal(2, info.HeightMultiple);
            Assert.True(info.IsMultiPlane);
            Assert.True(info.IsYuv);
            Assert.Equal("FORMAT_Y_U_V12", info.Name);
        }

        [Fact]
        public void DecodesFloat32()
        {
            var info = new PixelFormatInfo(FORMAT_F32);

            Assert.Equal(46, info.PredefinedId);
            Assert.Equal(1, info.Channels);
            Assert.Equal(OceanDataType.SignedFloat32, info.DataType);
            Assert.Equal(1, info.PlaneCount);
            Assert.Equal(4, info.BytesPerElement);
            Assert.True(info.IsFloatingPoint);
            Assert.Equal("FORMAT_F32", info.Name);
        }

        [Fact]
        public void DecodesRgba64()
        {
            var info = new PixelFormatInfo(FORMAT_RGBA64);

            Assert.Equal(38, info.PredefinedId);
            Assert.Equal(4, info.Channels);
            Assert.Equal(OceanDataType.UnsignedInteger16, info.DataType);
            Assert.Equal(1, info.PlaneCount);
            Assert.Equal(2, info.BytesPerElement);
            Assert.False(info.IsFloatingPoint);
            Assert.Equal("FORMAT_RGBA64", info.Name);
        }

        [Fact]
        public void DecodesGenericFormat()
        {
            // A generic format with no predefined ID: 2 channels, float64, 1 plane
            ulong generic = (1UL << 48) | (1UL << 40) | (1UL << 32) | (11UL << 24) | (2UL << 16) | 0UL;
            var info = new PixelFormatInfo(generic);

            Assert.Equal(0, info.PredefinedId);
            Assert.Equal(2, info.Channels);
            Assert.Equal(OceanDataType.SignedFloat64, info.DataType);
            Assert.Equal(8, info.BytesPerElement);
            Assert.True(info.IsFloatingPoint);
            Assert.Contains("GENERIC", info.Name);
        }

        [Fact]
        public void DataTypeNameMappings()
        {
            Assert.Equal("uint8", new PixelFormatInfo(FORMAT_RGB24).DataTypeName);
            Assert.Equal("float32", new PixelFormatInfo(FORMAT_F32).DataTypeName);
            Assert.Equal("uint16", new PixelFormatInfo(FORMAT_RGBA64).DataTypeName);
        }

        [Fact]
        public void PredefinedFormatNamesContainsAllExpected()
        {
            // Verify the map has all 50 entries (0-49)
            Assert.True(PixelFormatInfo.PredefinedFormatNames.ContainsKey(0));
            Assert.True(PixelFormatInfo.PredefinedFormatNames.ContainsKey(49));
            Assert.Equal(50, PixelFormatInfo.PredefinedFormatNames.Count);
        }
    }
}
