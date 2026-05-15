/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;
using System.Collections.Generic;
using Microsoft.VisualStudio.Shell;
using OceanFrameInspectorExtension.Imaging;

namespace OceanFrameInspectorExtension.Debug
{
    /// <summary>
    /// Reads an Ocean::Frame from the debuggee process by evaluating member expressions
    /// via DTE and reading raw pixel memory via ReadProcessMemory.
    /// </summary>
    public sealed class FrameReader : IDisposable
    {
        private readonly MemoryReader _memoryReader;
        private readonly int _maxMemoryBytes;

        public FrameReader(EnvDTE.Debugger debugger, int processId, int maxMemoryMB = 100)
        {
            _memoryReader = new MemoryReader(debugger, processId);
            _maxMemoryBytes = maxMemoryMB * 1024 * 1024;
        }

        /// <summary>
        /// Reads an Ocean::Frame from the debuggee, given a C++ expression that evaluates to a Frame.
        /// </summary>
        public FrameData? ReadFrame(string expression)
        {
            ThreadHelper.ThrowIfNotOnUIThread();

            // Read FrameType metadata
            uint? width = _memoryReader.EvaluateUInt($"({expression}).width_");
            uint? height = _memoryReader.EvaluateUInt($"({expression}).height_");
            ulong? pixelFormat = _memoryReader.EvaluateUInt64($"(unsigned long long)({expression}).pixelFormat_.pixelFormat_");
            uint? pixelOrigin = _memoryReader.EvaluateUInt($"(unsigned int)({expression}).pixelOrigin_");

            if (width == null || height == null || pixelFormat == null || pixelOrigin == null)
            {
                return null;
            }

            PixelFormatInfo formatInfo = new PixelFormatInfo(pixelFormat.Value);
            PixelOriginType origin = (PixelOriginType)pixelOrigin.Value;

            // Read plane count from StackHeapVector
            uint? planeCount = EvaluateSizeT($"({expression}).planes_.size_");
            if (planeCount == null || planeCount.Value == 0)
            {
                return null;
            }

            // Read each plane
            List<PlaneData> planes = new List<PlaneData>();

            for (uint i = 0; i < planeCount.Value && i < 4; i++)
            {
                PlaneData? plane = ReadPlane(expression, i);
                if (plane != null)
                {
                    planes.Add(plane);
                }
            }

            if (planes.Count == 0)
            {
                return null;
            }

            return new FrameData(width.Value, height.Value, formatInfo, origin, planes.ToArray());
        }

        private PlaneData? ReadPlane(string frameExpr, uint planeIndex)
        {
            ThreadHelper.ThrowIfNotOnUIThread();

            string planeExpr = $"({frameExpr}).planes_.stackElements_[{planeIndex}]";

            PlaneMetadata? meta = ReadPlaneMetadata(planeExpr);
            if (meta == null) return null;

            long totalBytes = (long)meta.StrideBytes * meta.Height;
            if (totalBytes <= 0 || totalBytes > _maxMemoryBytes) return null;

            byte[]? pixelData = _memoryReader.ReadMemory(meta.ConstDataPtr, (int)totalBytes);
            if (pixelData == null) return null;

            return new PlaneData(pixelData, meta.Width, meta.Height, meta.Channels,
                meta.ElementTypeSize, meta.PaddingElements, meta.StrideBytes, meta.BytesPerPixel);
        }

        private PlaneMetadata? ReadPlaneMetadata(string planeExpr)
        {
            ThreadHelper.ThrowIfNotOnUIThread();

            ulong? constDataPtr = _memoryReader.EvaluatePointer($"{planeExpr}.constData_");
            uint? width = _memoryReader.EvaluateUInt($"{planeExpr}.width_");
            uint? height = _memoryReader.EvaluateUInt($"{planeExpr}.height_");
            uint? channels = _memoryReader.EvaluateUInt($"{planeExpr}.channels_");
            uint? strideBytes = _memoryReader.EvaluateUInt($"{planeExpr}.strideBytes_");

            if (constDataPtr == null || constDataPtr.Value == 0 ||
                width == null || height == null || channels == null || strideBytes == null)
            {
                return null;
            }

            uint ets = _memoryReader.EvaluateUInt($"{planeExpr}.elementTypeSize_") ?? 1;
            uint pe = _memoryReader.EvaluateUInt($"{planeExpr}.paddingElements_") ?? 0;
            uint bpp = _memoryReader.EvaluateUInt($"{planeExpr}.bytesPerPixel_") ?? (channels.Value * ets);

            return new PlaneMetadata(constDataPtr.Value, width.Value, height.Value,
                channels.Value, ets, pe, strideBytes.Value, bpp);
        }

        private sealed class PlaneMetadata
        {
            public ulong ConstDataPtr { get; }
            public uint Width { get; }
            public uint Height { get; }
            public uint Channels { get; }
            public uint ElementTypeSize { get; }
            public uint PaddingElements { get; }
            public uint StrideBytes { get; }
            public uint BytesPerPixel { get; }

            public PlaneMetadata(ulong constDataPtr, uint width, uint height, uint channels,
                uint elementTypeSize, uint paddingElements, uint strideBytes, uint bytesPerPixel)
            {
                ConstDataPtr = constDataPtr;
                Width = width;
                Height = height;
                Channels = channels;
                ElementTypeSize = elementTypeSize;
                PaddingElements = paddingElements;
                StrideBytes = strideBytes;
                BytesPerPixel = bytesPerPixel;
            }
        }

        private uint? EvaluateSizeT(string expression)
        {
            ThreadHelper.ThrowIfNotOnUIThread();
            ulong? val64 = _memoryReader.EvaluateUInt64($"(unsigned long long)({expression})");
            if (val64 != null)
            {
                return (uint)Math.Min(val64.Value, uint.MaxValue);
            }

            return _memoryReader.EvaluateUInt(expression);
        }

        public void Dispose()
        {
            _memoryReader.Dispose();
        }
    }
}
