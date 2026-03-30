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
    /// Splits a frame into individual channel views.
    /// Handles both interleaved multi-channel formats (RGB24, BGRA32) and
    /// multi-plane YUV formats (Y_UV12, Y_U_V12, Y_VU12).
    /// </summary>
    public static class ChannelSplitter
    {
        /// <summary>
        /// Returns the available channel names for the given frame.
        /// </summary>
        public static string[] GetAvailableChannels(FrameData frame)
        {
            if (frame == null || frame.Planes.Length == 0) return Array.Empty<string>();

            string[]? multiPlaneChannels = GetMultiPlaneChannels(frame.PixelFormat.PredefinedId);
            if (multiPlaneChannels != null) return multiPlaneChannels;

            int channels = (int)frame.Planes[0].Channels;
            if (channels <= 1) return Array.Empty<string>();

            return GetInterleavedChannelNames(frame.PixelFormat.Name, channels);
        }

        private static string[]? GetMultiPlaneChannels(ushort id)
        {
            if (IsNv12Like(id) || IsNv21Like(id) || IsI420Like(id) || IsYv12Like(id))
                return new[] { "Y", "U", "V" };
            if (id == 39 || id == 40) return new[] { "Y", "U", "V" }; // Y_U_V24
            if (id == 48) return new[] { "R", "G", "B" }; // R_G_B24
            if (id == 49) return new[] { "B", "G", "R" }; // B_G_R24
            return null;
        }

        private static readonly (string pattern, string[] names3, string[]? names4)[] InterleavedPatterns =
        {
            ("BGRA", new[] { "B", "G", "R" }, new[] { "B", "G", "R", "A" }),
            ("RGBA", new[] { "R", "G", "B" }, new[] { "R", "G", "B", "A" }),
            ("RGBT", new[] { "R", "G", "B" }, new[] { "R", "G", "B", "T" }),
            ("ABGR", new[] { "A", "B", "G" }, new[] { "A", "B", "G", "R" }),
            ("ARGB", new[] { "A", "R", "G" }, new[] { "A", "R", "G", "B" }),
            ("BGR",  new[] { "B", "G", "R" }, new[] { "B", "G", "R", "X" }),
            ("RGB",  new[] { "R", "G", "B" }, new[] { "R", "G", "B", "X" }),
            ("YUVA", new[] { "Y", "U", "V" }, new[] { "Y", "U", "V", "A" }),
            ("YUVT", new[] { "Y", "U", "V" }, new[] { "Y", "U", "V", "T" }),
            ("YUV",  new[] { "Y", "U", "V" }, null),
            ("YVU",  new[] { "Y", "V", "U" }, null),
            ("YA",   new[] { "Y", "A" },      null),
        };

        private static string[] GetInterleavedChannelNames(string formatName, int channels)
        {
            foreach (var (pattern, names3, names4) in InterleavedPatterns)
            {
                if (!formatName.Contains(pattern)) continue;
                if (channels >= 4 && names4 != null) return names4;
                return names3;
            }

            string[] generic = new string[channels];
            for (int i = 0; i < channels; i++) generic[i] = $"Ch{i}";
            return generic;
        }

        /// <summary>
        /// Extracts a single channel by name from the frame.
        /// Returns a single-channel grayscale FrameData, or null if the channel is not available.
        /// </summary>
        public static FrameData? ExtractChannel(FrameData frame, string channelName)
        {
            if (frame == null || frame.Planes.Length == 0) return null;

            string[] available = GetAvailableChannels(frame);
            int channelIndex = Array.IndexOf(available, channelName);
            if (channelIndex < 0) return null;

            ushort id = frame.PixelFormat.PredefinedId;

            // Multi-plane YUV: extract from the appropriate plane
            if (IsNv12Like(id))
                return ExtractNv12Channel(frame, channelIndex, uvOrder: "UV");
            if (IsNv21Like(id))
                return ExtractNv12Channel(frame, channelIndex, uvOrder: "VU");
            if (IsI420Like(id))
                return ExtractPlanarChannel(frame, channelIndex, planeOrder: new[] { 0, 1, 2 });
            if (IsYv12Like(id))
                return ExtractPlanarChannel(frame, channelIndex, planeOrder: new[] { 0, 2, 1 }); // Y, V, U -> Y, U, V
            if (id == 39 || id == 40) // Y_U_V24
                return ExtractPlanarChannel(frame, channelIndex, planeOrder: new[] { 0, 1, 2 });

            // Planar RGB/BGR
            if (id == 48 || id == 49) // R_G_B24, B_G_R24
                return ExtractPlanarChannel(frame, channelIndex, planeOrder: new[] { 0, 1, 2 });

            // Single-plane interleaved
            return ExtractInterleavedChannel(frame, channelIndex);
        }

        /// <summary>
        /// Legacy API: splits all channels. Prefer ExtractChannel() for single-channel extraction.
        /// </summary>
        public static FrameData[] Split(FrameData frame)
        {
            string[] channels = GetAvailableChannels(frame);
            if (channels.Length == 0) return new[] { frame };

            List<FrameData> results = new List<FrameData>();
            foreach (string ch in channels)
            {
                FrameData? extracted = ExtractChannel(frame, ch);
                results.Add(extracted ?? frame);
            }
            return results.ToArray();
        }

        // --- Multi-plane YUV extraction ---

        /// <summary>
        /// Extracts Y, U, or V from NV12/NV21-like formats (2 planes: Y + interleaved UV/VU).
        /// </summary>
        private static FrameData? ExtractNv12Channel(FrameData frame, int channelIndex, string uvOrder)
        {
            if (frame.Planes.Length < 2) return null;

            PlaneData yPlane = frame.Planes[0];
            PlaneData uvPlane = frame.Planes[1];

            if (channelIndex == 0)
            {
                // Y channel: return plane 0 directly
                return MakeSingleChannelFrame(yPlane, frame);
            }

            // U or V from the interleaved UV/VU plane
            // channelIndex 1 = U, 2 = V
            // For NV12 (UV order): U is at even indices, V at odd
            // For NV21 (VU order): V is at even indices, U at odd
            int uvIdx = (channelIndex == 1) // requesting U
                ? (uvOrder == "UV" ? 0 : 1)   // U is at 0 for NV12, 1 for NV21
                : (uvOrder == "UV" ? 1 : 0);  // V is at 1 for NV12, 0 for NV21

            uint halfW = uvPlane.Width; // UV plane width (already half for NV12)
            uint halfH = uvPlane.Height;

            // The UV plane has 2 interleaved channels per pixel position
            byte[] channelData = new byte[halfW * halfH];
            uint dstStride = halfW;

            for (uint y = 0; y < halfH; y++)
            {
                int srcOffset = (int)(y * uvPlane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < halfW; x++)
                {
                    channelData[dstOffset + x] = uvPlane.Data[srcOffset + x * 2 + uvIdx];
                }
            }

            PlaneData channelPlane = new PlaneData(channelData, halfW, halfH, 1, 1, 0, dstStride, 1);
            return MakeSingleChannelFrame(channelPlane, halfW, halfH, frame);
        }

        /// <summary>
        /// Extracts a channel from fully planar formats (Y_U_V12, Y_V_U12, Y_U_V24, R_G_B24, B_G_R24).
        /// Each channel is a separate plane.
        /// </summary>
        private static FrameData? ExtractPlanarChannel(FrameData frame, int channelIndex, int[] planeOrder)
        {
            // channelIndex maps to Y=0, U=1, V=2 (or R=0, G=1, B=2)
            // planeOrder maps logical channel index to actual plane index
            if (channelIndex >= planeOrder.Length) return null;
            int planeIdx = planeOrder[channelIndex];
            if (planeIdx >= frame.Planes.Length) return null;

            PlaneData plane = frame.Planes[planeIdx];
            return MakeSingleChannelFrame(plane, frame);
        }

        /// <summary>
        /// Extracts a channel from interleaved single-plane formats (RGB24, BGRA32, etc.).
        /// </summary>
        private static FrameData? ExtractInterleavedChannel(FrameData frame, int channelIndex)
        {
            PlaneData plane = frame.Planes[0];
            int channels = (int)plane.Channels;
            int bpe = frame.PixelFormat.BytesPerElement;

            if (channelIndex >= channels) return null;

            uint dstStride = plane.Width * (uint)bpe;
            byte[] channelData = new byte[dstStride * plane.Height];

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int srcIdx = srcOffset + ((int)(x * channels) + channelIndex) * bpe;
                    int dstIdx = dstOffset + (int)(x * bpe);
                    for (int b = 0; b < bpe; b++)
                        channelData[dstIdx + b] = plane.Data[srcIdx + b];
                }
            }

            PlaneData channelPlane = new PlaneData(channelData, plane.Width, plane.Height, 1, (uint)bpe, 0, dstStride, (uint)bpe);
            return MakeSingleChannelFrame(channelPlane, frame);
        }

        // --- Helpers ---

        private static FrameData MakeSingleChannelFrame(PlaneData plane, FrameData source)
        {
            return MakeSingleChannelFrame(plane, plane.Width, plane.Height, source);
        }

        private static FrameData MakeSingleChannelFrame(PlaneData plane, uint width, uint height, FrameData source)
        {
            ulong genericFormat = ((ulong)1 << 48) | // heightMultiple=1
                                  ((ulong)1 << 40) | // widthMultiple=1
                                  ((ulong)1 << 32) | // 1 plane
                                  ((ulong)source.PixelFormat.DataType << 24) |
                                  ((ulong)1 << 16);  // 1 channel

            return new FrameData(width, height, new PixelFormatInfo(genericFormat), source.PixelOrigin, new[] { plane });
        }

        private static bool IsNv12Like(ushort id) => id == 26 || id == 42; // Y_UV12, Y_UV12_FULL_RANGE
        private static bool IsNv21Like(ushort id) => id == 27 || id == 43; // Y_VU12, Y_VU12_FULL_RANGE
        private static bool IsI420Like(ushort id) => id == 20 || id == 44; // Y_U_V12, Y_U_V12_FULL_RANGE
        private static bool IsYv12Like(ushort id) => id == 24 || id == 45; // Y_V_U12, Y_V_U12_FULL_RANGE
    }
}
