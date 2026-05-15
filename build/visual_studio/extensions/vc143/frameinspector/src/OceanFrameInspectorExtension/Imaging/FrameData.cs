/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;
using System.Linq;

namespace OceanFrameInspectorExtension.Imaging
{
    /// <summary>
    /// C# representation of a captured Ocean::Frame, holding copied pixel data.
    /// </summary>
    public sealed class FrameData
    {
        public uint Width { get; }
        public uint Height { get; }
        public PixelFormatInfo PixelFormat { get; }
        public PixelOriginType PixelOrigin { get; }
        public PlaneData[] Planes { get; }

        public FrameData(uint width, uint height, PixelFormatInfo pixelFormat, PixelOriginType pixelOrigin, PlaneData[] planes)
        {
            Width = width;
            Height = height;
            PixelFormat = pixelFormat;
            PixelOrigin = pixelOrigin;
            Planes = planes ?? throw new ArgumentNullException(nameof(planes));
        }

        /// <summary>
        /// Total bytes across all planes.
        /// </summary>
        public long TotalBytes => Planes.Sum(p => (long)p.Data.Length);
    }

    /// <summary>
    /// Data for a single plane, copied from the debuggee.
    /// </summary>
    public sealed class PlaneData
    {
        public byte[] Data { get; }
        public uint Width { get; }
        public uint Height { get; }
        public uint Channels { get; }
        public uint ElementTypeSize { get; }
        public uint PaddingElements { get; }
        public uint StrideBytes { get; }
        public uint BytesPerPixel { get; }

        public PlaneData(byte[] data, uint width, uint height, uint channels, uint elementTypeSize, uint paddingElements, uint strideBytes, uint bytesPerPixel)
        {
            Data = data ?? throw new ArgumentNullException(nameof(data));
            Width = width;
            Height = height;
            Channels = channels;
            ElementTypeSize = elementTypeSize;
            PaddingElements = paddingElements;
            StrideBytes = strideBytes;
            BytesPerPixel = bytesPerPixel;
        }

        public bool IsContinuous => PaddingElements == 0;
    }

    public enum PixelOriginType : uint
    {
        Invalid = 0,
        UpperLeft = 1,
        LowerLeft = 2
    }
}
