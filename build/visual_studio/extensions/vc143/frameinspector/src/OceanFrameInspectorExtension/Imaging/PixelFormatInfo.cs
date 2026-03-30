/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;
using System.Collections.Generic;

namespace OceanFrameInspectorExtension.Imaging
{
    /// <summary>
    /// Decodes Ocean's 64-bit PixelFormat value into its component parts.
    /// </summary>
    public sealed class PixelFormatInfo
    {
        /// <summary>
        /// The raw 64-bit pixel format value from Ocean.
        /// </summary>
        public ulong RawValue { get; }

        /// <summary>
        /// Predefined format ID (lower 16 bits).
        /// </summary>
        public ushort PredefinedId { get; }

        /// <summary>
        /// Number of channels (byte 2).
        /// </summary>
        public byte Channels { get; }

        /// <summary>
        /// Data type enum value (byte 3).
        /// </summary>
        public OceanDataType DataType { get; }

        /// <summary>
        /// Number of planes (byte 4).
        /// </summary>
        public byte PlaneCount { get; }

        /// <summary>
        /// Width must be a multiple of this value (byte 5).
        /// </summary>
        public byte WidthMultiple { get; }

        /// <summary>
        /// Height must be a multiple of this value (byte 6).
        /// </summary>
        public byte HeightMultiple { get; }

        public PixelFormatInfo(ulong rawValue)
        {
            RawValue = rawValue;
            PredefinedId = (ushort)(rawValue & 0xFFFF);
            Channels = (byte)((rawValue >> 16) & 0xFF);
            DataType = (OceanDataType)((rawValue >> 24) & 0xFF);
            PlaneCount = (byte)((rawValue >> 32) & 0xFF);
            WidthMultiple = (byte)((rawValue >> 40) & 0xFF);
            HeightMultiple = (byte)((rawValue >> 48) & 0xFF);
        }

        /// <summary>
        /// Bytes per element for the data type.
        /// </summary>
        public int BytesPerElement => DataType switch
        {
            OceanDataType.UnsignedInteger8 => 1,
            OceanDataType.SignedInteger8 => 1,
            OceanDataType.UnsignedInteger16 => 2,
            OceanDataType.SignedInteger16 => 2,
            OceanDataType.UnsignedInteger32 => 4,
            OceanDataType.SignedInteger32 => 4,
            OceanDataType.UnsignedInteger64 => 8,
            OceanDataType.SignedInteger64 => 8,
            OceanDataType.SignedFloat16 => 2,
            OceanDataType.SignedFloat32 => 4,
            OceanDataType.SignedFloat64 => 8,
            _ => 1
        };

        public bool IsFloatingPoint => DataType == OceanDataType.SignedFloat16 ||
                                       DataType == OceanDataType.SignedFloat32 ||
                                       DataType == OceanDataType.SignedFloat64;

        public bool IsMultiPlane => PlaneCount > 1;

        public bool IsYuv
        {
            get
            {
                if (PredefinedFormatNames.TryGetValue(PredefinedId, out string? name))
                {
                    return name.Contains("Y_U") || name.Contains("Y_V") ||
                           name.Contains("YUV") || name.Contains("YVU") ||
                           name.Contains("YUYV") || name.Contains("UYVY") ||
                           name.Contains("Y_UV") || name.Contains("Y_VU") ||
                           name.Contains("YUVA") || name.Contains("YUVT");
                }
                return false;
            }
        }

        public bool IsPacked
        {
            get
            {
                if (PredefinedFormatNames.TryGetValue(PredefinedId, out string? name))
                {
                    return name.Contains("PACKED") || name.Contains("565") ||
                           name.Contains("5551") || name.Contains("4444") ||
                           name.Contains("YUYV") || name.Contains("UYVY");
                }
                return false;
            }
        }

        /// <summary>
        /// Human-readable name for the predefined format.
        /// </summary>
        public string Name => PredefinedFormatNames.TryGetValue(PredefinedId, out string? name)
            ? name
            : $"GENERIC_{Channels}CH_{DataType}";

        /// <summary>
        /// Human-readable name for the data type.
        /// </summary>
        public string DataTypeName => DataType switch
        {
            OceanDataType.Undefined => "undefined",
            OceanDataType.UnsignedInteger8 => "uint8",
            OceanDataType.SignedInteger8 => "int8",
            OceanDataType.UnsignedInteger16 => "uint16",
            OceanDataType.SignedInteger16 => "int16",
            OceanDataType.UnsignedInteger32 => "uint32",
            OceanDataType.SignedInteger32 => "int32",
            OceanDataType.UnsignedInteger64 => "uint64",
            OceanDataType.SignedInteger64 => "int64",
            OceanDataType.SignedFloat16 => "float16",
            OceanDataType.SignedFloat32 => "float32",
            OceanDataType.SignedFloat64 => "float64",
            _ => "unknown"
        };

        public static readonly Dictionary<ushort, string> PredefinedFormatNames = new Dictionary<ushort, string>
        {
            { 0, "FORMAT_UNDEFINED" },
            { 1, "FORMAT_ABGR32" },
            { 2, "FORMAT_ARGB32" },
            { 3, "FORMAT_BGR24" },
            { 4, "FORMAT_BGR32" },
            { 5, "FORMAT_BGR4444" },
            { 6, "FORMAT_BGR5551" },
            { 7, "FORMAT_BGR565" },
            { 8, "FORMAT_BGRA32" },
            { 9, "FORMAT_BGRA4444" },
            { 10, "FORMAT_BGGR10_PACKED" },
            { 11, "FORMAT_RGB24" },
            { 12, "FORMAT_RGB32" },
            { 13, "FORMAT_RGB4444" },
            { 14, "FORMAT_RGB5551" },
            { 15, "FORMAT_RGB565" },
            { 16, "FORMAT_RGBA32" },
            { 17, "FORMAT_RGBA4444" },
            { 18, "FORMAT_RGBT32" },
            { 19, "FORMAT_RGGB10_PACKED" },
            { 20, "FORMAT_Y_U_V12" },
            { 21, "FORMAT_YUV24" },
            { 22, "FORMAT_YUVA32" },
            { 23, "FORMAT_YUVT32" },
            { 24, "FORMAT_Y_V_U12" },
            { 25, "FORMAT_YVU24" },
            { 26, "FORMAT_Y_UV12" },
            { 27, "FORMAT_Y_VU12" },
            { 28, "FORMAT_YUYV16" },
            { 29, "FORMAT_UYVY16" },
            { 30, "FORMAT_Y8" },
            { 31, "FORMAT_Y10" },
            { 32, "FORMAT_Y10_PACKED" },
            { 33, "FORMAT_Y16" },
            { 34, "FORMAT_Y32" },
            { 35, "FORMAT_Y64" },
            { 36, "FORMAT_YA16" },
            { 37, "FORMAT_RGB48" },
            { 38, "FORMAT_RGBA64" },
            { 39, "FORMAT_Y_U_V24" },
            { 40, "FORMAT_Y_U_V24_FULL_RANGE" },
            { 41, "FORMAT_Y8_LIMITED_RANGE" },
            { 42, "FORMAT_Y_UV12_FULL_RANGE" },
            { 43, "FORMAT_Y_VU12_FULL_RANGE" },
            { 44, "FORMAT_Y_U_V12_FULL_RANGE" },
            { 45, "FORMAT_Y_V_U12_FULL_RANGE" },
            { 46, "FORMAT_F32" },
            { 47, "FORMAT_F64" },
            { 48, "FORMAT_R_G_B24" },
            { 49, "FORMAT_B_G_R24" }
        };
    }

    /// <summary>
    /// Maps to Ocean::FrameType::DataType enum values.
    /// </summary>
    public enum OceanDataType : byte
    {
        Undefined = 0,
        UnsignedInteger8 = 1,
        SignedInteger8 = 2,
        UnsignedInteger16 = 3,
        SignedInteger16 = 4,
        UnsignedInteger32 = 5,
        SignedInteger32 = 6,
        UnsignedInteger64 = 7,
        SignedInteger64 = 8,
        SignedFloat16 = 9,
        SignedFloat32 = 10,
        SignedFloat64 = 11
    }
}
