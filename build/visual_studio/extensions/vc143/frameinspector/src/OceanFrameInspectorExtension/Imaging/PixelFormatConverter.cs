/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace OceanFrameInspectorExtension.Imaging
{
    /// <summary>
    /// Converts raw pixel data from Ocean frame formats to WPF BitmapSource for display.
    /// </summary>
    public static class PixelFormatConverter
    {
        /// <summary>
        /// Converts a FrameData to a displayable BitmapSource.
        /// </summary>
        public static BitmapSource Convert(FrameData frame)
        {
            if (frame == null) throw new ArgumentNullException(nameof(frame));
            if (frame.Planes.Length == 0) throw new ArgumentException("Frame has no planes.");

            BitmapSource result = ConvertByFormat(frame);

            if (frame.PixelOrigin == PixelOriginType.LowerLeft)
            {
                result = new TransformedBitmap(result, new ScaleTransform(1, -1));
            }

            result.Freeze();
            return result;
        }

        private static BitmapSource ConvertByFormat(FrameData frame)
        {
            ushort id = frame.PixelFormat.PredefinedId;

            return id switch
            {
                // Direct BGRA32
                8 => CreateBitmap(frame, PixelFormats.Bgra32),   // FORMAT_BGRA32
                // Direct BGR24
                3 => CreateBitmap(frame, PixelFormats.Bgr24),    // FORMAT_BGR24
                // RGB24 -> swap to BGR24
                11 => ConvertRgb24ToBgr24(frame),                // FORMAT_RGB24
                // RGBA32 -> swizzle to BGRA32
                16 => ConvertRgba32ToBgra32(frame),              // FORMAT_RGBA32
                // ABGR32 -> swizzle to BGRA32
                1 => ConvertAbgr32ToBgra32(frame),               // FORMAT_ABGR32
                // ARGB32 -> swizzle to BGRA32
                2 => ConvertArgb32ToBgra32(frame),               // FORMAT_ARGB32
                // BGR32 -> BGRA32 (set alpha=255)
                4 => ConvertBgrx32ToBgra32(frame),               // FORMAT_BGR32
                // RGB32 -> BGRA32 (swap + set alpha=255)
                12 => ConvertRgbx32ToBgra32(frame),              // FORMAT_RGB32
                // Grayscale 8-bit
                30 or 41 => CreateBitmap(frame, PixelFormats.Gray8),  // FORMAT_Y8, FORMAT_Y8_LIMITED_RANGE
                // Grayscale 16-bit
                33 => CreateBitmap(frame, PixelFormats.Gray16),  // FORMAT_Y16
                // Grayscale 32-bit int -> normalize to 8-bit
                34 => NormalizeUint32ToGray8(frame),              // FORMAT_Y32
                // Grayscale 64-bit int -> normalize to 8-bit
                35 => NormalizeUint64ToGray8(frame),              // FORMAT_Y64
                // Float32 -> normalize to 8-bit grayscale
                46 => NormalizeFloat32ToGray8(frame),             // FORMAT_F32
                // Float64 -> normalize to 8-bit grayscale
                47 => NormalizeFloat64ToGray8(frame),             // FORMAT_F64
                // YUV24 -> BT.601 to RGB
                21 => ConvertYuv24ToRgb(frame),                  // FORMAT_YUV24
                // YVU24 -> BT.601 to RGB
                25 => ConvertYvu24ToRgb(frame),                  // FORMAT_YVU24
                // YA16 -> grayscale (ignore alpha for display)
                36 => ConvertYa16ToGray8(frame),                 // FORMAT_YA16
                // YUVA32 -> convert YUV to RGB, keep alpha
                22 => ConvertYuva32ToBgra32(frame),              // FORMAT_YUVA32
                // RGBT32 -> treat T as alpha
                18 => ConvertRgba32ToBgra32(frame),              // FORMAT_RGBT32
                // RGB48 -> normalize to BGR24
                37 => NormalizeRgb48ToBgr24(frame),              // FORMAT_RGB48
                // RGBA64 -> normalize to BGRA32
                38 => NormalizeRgba64ToBgra32(frame),            // FORMAT_RGBA64
                // NV12 (Y_UV12) -> BT.601
                26 or 42 => ConvertNv12ToRgb(frame),             // FORMAT_Y_UV12, FORMAT_Y_UV12_FULL_RANGE
                // NV21 (Y_VU12) -> BT.601
                27 or 43 => ConvertNv21ToRgb(frame),             // FORMAT_Y_VU12, FORMAT_Y_VU12_FULL_RANGE
                // I420 (Y_U_V12) -> BT.601
                20 or 44 => ConvertI420ToRgb(frame),             // FORMAT_Y_U_V12, FORMAT_Y_U_V12_FULL_RANGE
                // YV12 (Y_V_U12) -> similar to I420 with swapped U/V planes
                24 or 45 => ConvertYv12ToRgb(frame),             // FORMAT_Y_V_U12, FORMAT_Y_V_U12_FULL_RANGE
                // YUYV16
                28 => ConvertYuyv16ToRgb(frame),                 // FORMAT_YUYV16
                // UYVY16
                29 => ConvertUyvy16ToRgb(frame),                 // FORMAT_UYVY16
                // BGR565 / RGB565
                7 => ConvertBgr565ToBgr24(frame),                // FORMAT_BGR565
                15 => ConvertRgb565ToBgr24(frame),               // FORMAT_RGB565
                // Y_U_V24 (planar 4:4:4)
                39 or 40 => ConvertYuv444PlanarToRgb(frame),     // FORMAT_Y_U_V24, FORMAT_Y_U_V24_FULL_RANGE
                // R_G_B24 (planar RGB)
                48 => ConvertPlanarRgbToBgr24(frame),            // FORMAT_R_G_B24
                // B_G_R24 (planar BGR)
                49 => ConvertPlanarBgrToBgr24(frame),            // FORMAT_B_G_R24
                // Y10 (16-bit with 10 useful bits)
                31 => NormalizeY10ToGray8(frame),                 // FORMAT_Y10
                // Fallback: try generic conversion
                _ => ConvertGeneric(frame)
            };
        }

        private static BitmapSource CreateBitmap(FrameData frame, System.Windows.Media.PixelFormat format)
        {
            PlaneData plane = frame.Planes[0];
            return BitmapSource.Create(
                (int)plane.Width, (int)plane.Height,
                96, 96, format, null,
                plane.Data, (int)plane.StrideBytes);
        }

        private static BitmapSource ConvertRgb24ToBgr24(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgr = new byte[plane.Width * plane.Height * 3];
            int dstStride = (int)(plane.Width * 3);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int si = srcOffset + (int)(x * 3);
                    int di = dstOffset + (int)(x * 3);
                    bgr[di + 0] = plane.Data[si + 2]; // B
                    bgr[di + 1] = plane.Data[si + 1]; // G
                    bgr[di + 2] = plane.Data[si + 0]; // R
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertRgba32ToBgra32(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgra = new byte[plane.Width * plane.Height * 4];
            int dstStride = (int)(plane.Width * 4);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int si = srcOffset + (int)(x * 4);
                    int di = dstOffset + (int)(x * 4);
                    bgra[di + 0] = plane.Data[si + 2]; // B
                    bgra[di + 1] = plane.Data[si + 1]; // G
                    bgra[di + 2] = plane.Data[si + 0]; // R
                    bgra[di + 3] = plane.Data[si + 3]; // A
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgra32, null, bgra, dstStride);
        }

        private static BitmapSource ConvertAbgr32ToBgra32(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgra = new byte[plane.Width * plane.Height * 4];
            int dstStride = (int)(plane.Width * 4);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int si = srcOffset + (int)(x * 4);
                    int di = dstOffset + (int)(x * 4);
                    // ABGR -> BGRA: A=src[0], B=src[1], G=src[2], R=src[3]
                    bgra[di + 0] = plane.Data[si + 1]; // B
                    bgra[di + 1] = plane.Data[si + 2]; // G
                    bgra[di + 2] = plane.Data[si + 3]; // R
                    bgra[di + 3] = plane.Data[si + 0]; // A
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgra32, null, bgra, dstStride);
        }

        private static BitmapSource ConvertArgb32ToBgra32(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgra = new byte[plane.Width * plane.Height * 4];
            int dstStride = (int)(plane.Width * 4);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int si = srcOffset + (int)(x * 4);
                    int di = dstOffset + (int)(x * 4);
                    // ARGB -> BGRA: A=src[0], R=src[1], G=src[2], B=src[3]
                    bgra[di + 0] = plane.Data[si + 3]; // B
                    bgra[di + 1] = plane.Data[si + 2]; // G
                    bgra[di + 2] = plane.Data[si + 1]; // R
                    bgra[di + 3] = plane.Data[si + 0]; // A
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgra32, null, bgra, dstStride);
        }

        private static BitmapSource ConvertBgrx32ToBgra32(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgra = new byte[plane.Width * plane.Height * 4];
            int dstStride = (int)(plane.Width * 4);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int si = srcOffset + (int)(x * 4);
                    int di = dstOffset + (int)(x * 4);
                    bgra[di + 0] = plane.Data[si + 0]; // B
                    bgra[di + 1] = plane.Data[si + 1]; // G
                    bgra[di + 2] = plane.Data[si + 2]; // R
                    bgra[di + 3] = 255;                // A
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgra32, null, bgra, dstStride);
        }

        private static BitmapSource ConvertRgbx32ToBgra32(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgra = new byte[plane.Width * plane.Height * 4];
            int dstStride = (int)(plane.Width * 4);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int si = srcOffset + (int)(x * 4);
                    int di = dstOffset + (int)(x * 4);
                    bgra[di + 0] = plane.Data[si + 2]; // B
                    bgra[di + 1] = plane.Data[si + 1]; // G
                    bgra[di + 2] = plane.Data[si + 0]; // R
                    bgra[di + 3] = 255;                // A
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgra32, null, bgra, dstStride);
        }

        private static byte ClampToByte(int value) => (byte)Math.Max(0, Math.Min(255, value));

        private static void YuvToRgb(byte yVal, byte uVal, byte vVal, out byte r, out byte g, out byte b)
        {
            // BT.601 conversion
            int c = yVal - 16;
            int d = uVal - 128;
            int e = vVal - 128;
            r = ClampToByte((298 * c + 409 * e + 128) >> 8);
            g = ClampToByte((298 * c - 100 * d - 208 * e + 128) >> 8);
            b = ClampToByte((298 * c + 516 * d + 128) >> 8);
        }

        private static BitmapSource ConvertYuv24ToRgb(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgr = new byte[plane.Width * plane.Height * 3];
            int dstStride = (int)(plane.Width * 3);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int si = srcOffset + (int)(x * 3);
                    YuvToRgb(plane.Data[si], plane.Data[si + 1], plane.Data[si + 2], out byte r, out byte g, out byte b);
                    int di = dstOffset + (int)(x * 3);
                    bgr[di + 0] = b;
                    bgr[di + 1] = g;
                    bgr[di + 2] = r;
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertYvu24ToRgb(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgr = new byte[plane.Width * plane.Height * 3];
            int dstStride = (int)(plane.Width * 3);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int si = srcOffset + (int)(x * 3);
                    // YVU order: Y=src[0], V=src[1], U=src[2]
                    YuvToRgb(plane.Data[si], plane.Data[si + 2], plane.Data[si + 1], out byte r, out byte g, out byte b);
                    int di = dstOffset + (int)(x * 3);
                    bgr[di + 0] = b;
                    bgr[di + 1] = g;
                    bgr[di + 2] = r;
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertYa16ToGray8(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] gray = new byte[plane.Width * plane.Height];
            int dstStride = (int)plane.Width;

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    gray[dstOffset + (int)x] = plane.Data[srcOffset + (int)(x * 2)];
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Gray8, null, gray, dstStride);
        }

        private static BitmapSource ConvertYuva32ToBgra32(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgra = new byte[plane.Width * plane.Height * 4];
            int dstStride = (int)(plane.Width * 4);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int si = srcOffset + (int)(x * 4);
                    YuvToRgb(plane.Data[si], plane.Data[si + 1], plane.Data[si + 2], out byte r, out byte g, out byte b);
                    int di = dstOffset + (int)(x * 4);
                    bgra[di + 0] = b;
                    bgra[di + 1] = g;
                    bgra[di + 2] = r;
                    bgra[di + 3] = plane.Data[si + 3]; // A
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgra32, null, bgra, dstStride);
        }

        private static BitmapSource NormalizeFloat32ToGray8(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            int pixelCount = (int)(plane.Width * plane.Height);
            float min = float.MaxValue, max = float.MinValue;

            for (int i = 0; i < pixelCount; i++)
            {
                float val = BitConverter.ToSingle(plane.Data, (int)(i / plane.Width * plane.StrideBytes + (i % plane.Width) * 4));
                if (!float.IsNaN(val) && !float.IsInfinity(val))
                {
                    if (val < min) min = val;
                    if (val > max) max = val;
                }
            }

            float range = max - min;
            if (range < float.Epsilon) range = 1.0f;

            byte[] gray = new byte[plane.Width * plane.Height];
            int dstStride = (int)plane.Width;

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    float val = BitConverter.ToSingle(plane.Data, srcOffset + (int)(x * 4));
                    byte normalized = (float.IsNaN(val) || float.IsInfinity(val))
                        ? (byte)0
                        : ClampToByte((int)((val - min) / range * 255.0f));
                    gray[dstOffset + (int)x] = normalized;
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Gray8, null, gray, dstStride);
        }

        private static BitmapSource NormalizeFloat64ToGray8(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            double min = double.MaxValue, max = double.MinValue;

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                for (uint x = 0; x < plane.Width; x++)
                {
                    double val = BitConverter.ToDouble(plane.Data, srcOffset + (int)(x * 8));
                    if (!double.IsNaN(val) && !double.IsInfinity(val))
                    {
                        if (val < min) min = val;
                        if (val > max) max = val;
                    }
                }
            }

            double range = max - min;
            if (range < double.Epsilon) range = 1.0;

            byte[] gray = new byte[plane.Width * plane.Height];
            int dstStride = (int)plane.Width;

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    double val = BitConverter.ToDouble(plane.Data, srcOffset + (int)(x * 8));
                    byte normalized = (double.IsNaN(val) || double.IsInfinity(val))
                        ? (byte)0
                        : ClampToByte((int)((val - min) / range * 255.0));
                    gray[dstOffset + (int)x] = normalized;
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Gray8, null, gray, dstStride);
        }

        private static BitmapSource NormalizeUint32ToGray8(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            uint min = uint.MaxValue, max = uint.MinValue;

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                for (uint x = 0; x < plane.Width; x++)
                {
                    uint val = BitConverter.ToUInt32(plane.Data, srcOffset + (int)(x * 4));
                    if (val < min) min = val;
                    if (val > max) max = val;
                }
            }

            double range = max - min;
            if (range < 1.0) range = 1.0;

            byte[] gray = new byte[plane.Width * plane.Height];
            int dstStride = (int)plane.Width;

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    uint val = BitConverter.ToUInt32(plane.Data, srcOffset + (int)(x * 4));
                    gray[dstOffset + (int)x] = ClampToByte((int)((val - min) / range * 255.0));
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Gray8, null, gray, dstStride);
        }

        private static BitmapSource NormalizeUint64ToGray8(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            ulong min = ulong.MaxValue, max = ulong.MinValue;

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                for (uint x = 0; x < plane.Width; x++)
                {
                    ulong val = BitConverter.ToUInt64(plane.Data, srcOffset + (int)(x * 8));
                    if (val < min) min = val;
                    if (val > max) max = val;
                }
            }

            double range = max - min;
            if (range < 1.0) range = 1.0;

            byte[] gray = new byte[plane.Width * plane.Height];
            int dstStride = (int)plane.Width;

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    ulong val = BitConverter.ToUInt64(plane.Data, srcOffset + (int)(x * 8));
                    gray[dstOffset + (int)x] = ClampToByte((int)((val - min) / range * 255.0));
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Gray8, null, gray, dstStride);
        }

        private static BitmapSource NormalizeRgb48ToBgr24(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgr = new byte[plane.Width * plane.Height * 3];
            int dstStride = (int)(plane.Width * 3);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int si = srcOffset + (int)(x * 6);
                    int di = dstOffset + (int)(x * 3);
                    // Take high byte of each 16-bit channel, swap R/B
                    bgr[di + 0] = plane.Data[si + 5]; // B high byte
                    bgr[di + 1] = plane.Data[si + 3]; // G high byte
                    bgr[di + 2] = plane.Data[si + 1]; // R high byte
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource NormalizeRgba64ToBgra32(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgra = new byte[plane.Width * plane.Height * 4];
            int dstStride = (int)(plane.Width * 4);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int si = srcOffset + (int)(x * 8);
                    int di = dstOffset + (int)(x * 4);
                    bgra[di + 0] = plane.Data[si + 5]; // B high byte
                    bgra[di + 1] = plane.Data[si + 3]; // G high byte
                    bgra[di + 2] = plane.Data[si + 1]; // R high byte
                    bgra[di + 3] = plane.Data[si + 7]; // A high byte
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgra32, null, bgra, dstStride);
        }

        private static BitmapSource NormalizeY10ToGray8(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] gray = new byte[plane.Width * plane.Height];
            int dstStride = (int)plane.Width;

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    ushort val = BitConverter.ToUInt16(plane.Data, srcOffset + (int)(x * 2));
                    gray[dstOffset + (int)x] = (byte)(val >> 2); // 10-bit to 8-bit
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Gray8, null, gray, dstStride);
        }

        private static BitmapSource ConvertNv12ToRgb(FrameData frame)
        {
            if (frame.Planes.Length < 2) return ConvertGeneric(frame);

            PlaneData yPlane = frame.Planes[0];
            PlaneData uvPlane = frame.Planes[1];
            byte[] bgr = new byte[yPlane.Width * yPlane.Height * 3];
            int dstStride = (int)(yPlane.Width * 3);

            for (uint y = 0; y < yPlane.Height; y++)
            {
                int yOffset = (int)(y * yPlane.StrideBytes);
                int uvOffset = (int)((y / 2) * uvPlane.StrideBytes);
                int dstOffset = (int)(y * dstStride);

                for (uint x = 0; x < yPlane.Width; x++)
                {
                    byte yVal = yPlane.Data[yOffset + (int)x];
                    int uvIdx = uvOffset + (int)((x / 2) * 2);
                    byte uVal = uvPlane.Data[uvIdx];
                    byte vVal = uvPlane.Data[uvIdx + 1];

                    YuvToRgb(yVal, uVal, vVal, out byte r, out byte g, out byte b);
                    int di = dstOffset + (int)(x * 3);
                    bgr[di + 0] = b;
                    bgr[di + 1] = g;
                    bgr[di + 2] = r;
                }
            }

            return BitmapSource.Create((int)yPlane.Width, (int)yPlane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertNv21ToRgb(FrameData frame)
        {
            if (frame.Planes.Length < 2) return ConvertGeneric(frame);

            PlaneData yPlane = frame.Planes[0];
            PlaneData vuPlane = frame.Planes[1];
            byte[] bgr = new byte[yPlane.Width * yPlane.Height * 3];
            int dstStride = (int)(yPlane.Width * 3);

            for (uint y = 0; y < yPlane.Height; y++)
            {
                int yOffset = (int)(y * yPlane.StrideBytes);
                int vuOffset = (int)((y / 2) * vuPlane.StrideBytes);
                int dstOffset = (int)(y * dstStride);

                for (uint x = 0; x < yPlane.Width; x++)
                {
                    byte yVal = yPlane.Data[yOffset + (int)x];
                    int vuIdx = vuOffset + (int)((x / 2) * 2);
                    byte vVal = vuPlane.Data[vuIdx];
                    byte uVal = vuPlane.Data[vuIdx + 1];

                    YuvToRgb(yVal, uVal, vVal, out byte r, out byte g, out byte b);
                    int di = dstOffset + (int)(x * 3);
                    bgr[di + 0] = b;
                    bgr[di + 1] = g;
                    bgr[di + 2] = r;
                }
            }

            return BitmapSource.Create((int)yPlane.Width, (int)yPlane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertI420ToRgb(FrameData frame)
        {
            if (frame.Planes.Length < 3) return ConvertGeneric(frame);

            PlaneData yPlane = frame.Planes[0];
            PlaneData uPlane = frame.Planes[1];
            PlaneData vPlane = frame.Planes[2];
            byte[] bgr = new byte[yPlane.Width * yPlane.Height * 3];
            int dstStride = (int)(yPlane.Width * 3);

            for (uint y = 0; y < yPlane.Height; y++)
            {
                int yOffset = (int)(y * yPlane.StrideBytes);
                int uOffset = (int)((y / 2) * uPlane.StrideBytes);
                int vOffset = (int)((y / 2) * vPlane.StrideBytes);
                int dstOffset = (int)(y * dstStride);

                for (uint x = 0; x < yPlane.Width; x++)
                {
                    byte yVal = yPlane.Data[yOffset + (int)x];
                    byte uVal = uPlane.Data[uOffset + (int)(x / 2)];
                    byte vVal = vPlane.Data[vOffset + (int)(x / 2)];

                    YuvToRgb(yVal, uVal, vVal, out byte r, out byte g, out byte b);
                    int di = dstOffset + (int)(x * 3);
                    bgr[di + 0] = b;
                    bgr[di + 1] = g;
                    bgr[di + 2] = r;
                }
            }

            return BitmapSource.Create((int)yPlane.Width, (int)yPlane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertYv12ToRgb(FrameData frame)
        {
            if (frame.Planes.Length < 3) return ConvertGeneric(frame);

            PlaneData yPlane = frame.Planes[0];
            PlaneData vPlane = frame.Planes[1]; // V first for YV12
            PlaneData uPlane = frame.Planes[2];
            byte[] bgr = new byte[yPlane.Width * yPlane.Height * 3];
            int dstStride = (int)(yPlane.Width * 3);

            for (uint y = 0; y < yPlane.Height; y++)
            {
                int yOffset = (int)(y * yPlane.StrideBytes);
                int uOffset = (int)((y / 2) * uPlane.StrideBytes);
                int vOffset = (int)((y / 2) * vPlane.StrideBytes);
                int dstOffset = (int)(y * dstStride);

                for (uint x = 0; x < yPlane.Width; x++)
                {
                    byte yVal = yPlane.Data[yOffset + (int)x];
                    byte uVal = uPlane.Data[uOffset + (int)(x / 2)];
                    byte vVal = vPlane.Data[vOffset + (int)(x / 2)];

                    YuvToRgb(yVal, uVal, vVal, out byte r, out byte g, out byte b);
                    int di = dstOffset + (int)(x * 3);
                    bgr[di + 0] = b;
                    bgr[di + 1] = g;
                    bgr[di + 2] = r;
                }
            }

            return BitmapSource.Create((int)yPlane.Width, (int)yPlane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertYuyv16ToRgb(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgr = new byte[plane.Width * plane.Height * 3];
            int dstStride = (int)(plane.Width * 3);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);

                for (uint x = 0; x < plane.Width; x += 2)
                {
                    int si = srcOffset + (int)(x * 2);
                    byte y0 = plane.Data[si + 0];
                    byte u = plane.Data[si + 1];
                    byte y1 = plane.Data[si + 2];
                    byte v = plane.Data[si + 3];

                    YuvToRgb(y0, u, v, out byte r0, out byte g0, out byte b0);
                    int di0 = dstOffset + (int)(x * 3);
                    bgr[di0 + 0] = b0;
                    bgr[di0 + 1] = g0;
                    bgr[di0 + 2] = r0;

                    if (x + 1 < plane.Width)
                    {
                        YuvToRgb(y1, u, v, out byte r1, out byte g1, out byte b1);
                        int di1 = dstOffset + (int)((x + 1) * 3);
                        bgr[di1 + 0] = b1;
                        bgr[di1 + 1] = g1;
                        bgr[di1 + 2] = r1;
                    }
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertUyvy16ToRgb(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgr = new byte[plane.Width * plane.Height * 3];
            int dstStride = (int)(plane.Width * 3);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);

                for (uint x = 0; x < plane.Width; x += 2)
                {
                    int si = srcOffset + (int)(x * 2);
                    byte u = plane.Data[si + 0];
                    byte y0 = plane.Data[si + 1];
                    byte v = plane.Data[si + 2];
                    byte y1 = plane.Data[si + 3];

                    YuvToRgb(y0, u, v, out byte r0, out byte g0, out byte b0);
                    int di0 = dstOffset + (int)(x * 3);
                    bgr[di0 + 0] = b0;
                    bgr[di0 + 1] = g0;
                    bgr[di0 + 2] = r0;

                    if (x + 1 < plane.Width)
                    {
                        YuvToRgb(y1, u, v, out byte r1, out byte g1, out byte b1);
                        int di1 = dstOffset + (int)((x + 1) * 3);
                        bgr[di1 + 0] = b1;
                        bgr[di1 + 1] = g1;
                        bgr[di1 + 2] = r1;
                    }
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertBgr565ToBgr24(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgr = new byte[plane.Width * plane.Height * 3];
            int dstStride = (int)(plane.Width * 3);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    ushort pixel = BitConverter.ToUInt16(plane.Data, srcOffset + (int)(x * 2));
                    // BGR565: BBBBBGGGGGGRRRRR
                    int b5 = (pixel >> 11) & 0x1F;
                    int g6 = (pixel >> 5) & 0x3F;
                    int r5 = pixel & 0x1F;

                    int di = dstOffset + (int)(x * 3);
                    bgr[di + 0] = (byte)((b5 << 3) | (b5 >> 2));
                    bgr[di + 1] = (byte)((g6 << 2) | (g6 >> 4));
                    bgr[di + 2] = (byte)((r5 << 3) | (r5 >> 2));
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertRgb565ToBgr24(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            byte[] bgr = new byte[plane.Width * plane.Height * 3];
            int dstStride = (int)(plane.Width * 3);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    ushort pixel = BitConverter.ToUInt16(plane.Data, srcOffset + (int)(x * 2));
                    // RGB565: RRRRRGGGGGGBBBBB
                    int r5 = (pixel >> 11) & 0x1F;
                    int g6 = (pixel >> 5) & 0x3F;
                    int b5 = pixel & 0x1F;

                    int di = dstOffset + (int)(x * 3);
                    bgr[di + 0] = (byte)((b5 << 3) | (b5 >> 2));
                    bgr[di + 1] = (byte)((g6 << 2) | (g6 >> 4));
                    bgr[di + 2] = (byte)((r5 << 3) | (r5 >> 2));
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertYuv444PlanarToRgb(FrameData frame)
        {
            if (frame.Planes.Length < 3) return ConvertGeneric(frame);

            PlaneData yPlane = frame.Planes[0];
            PlaneData uPlane = frame.Planes[1];
            PlaneData vPlane = frame.Planes[2];
            byte[] bgr = new byte[yPlane.Width * yPlane.Height * 3];
            int dstStride = (int)(yPlane.Width * 3);

            for (uint y = 0; y < yPlane.Height; y++)
            {
                int yOffset = (int)(y * yPlane.StrideBytes);
                int uOffset = (int)(y * uPlane.StrideBytes);
                int vOffset = (int)(y * vPlane.StrideBytes);
                int dstOffset = (int)(y * dstStride);

                for (uint x = 0; x < yPlane.Width; x++)
                {
                    byte yVal = yPlane.Data[yOffset + (int)x];
                    byte uVal = uPlane.Data[uOffset + (int)x];
                    byte vVal = vPlane.Data[vOffset + (int)x];

                    YuvToRgb(yVal, uVal, vVal, out byte r, out byte g, out byte b);
                    int di = dstOffset + (int)(x * 3);
                    bgr[di + 0] = b;
                    bgr[di + 1] = g;
                    bgr[di + 2] = r;
                }
            }

            return BitmapSource.Create((int)yPlane.Width, (int)yPlane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertPlanarRgbToBgr24(FrameData frame)
        {
            if (frame.Planes.Length < 3) return ConvertGeneric(frame);

            PlaneData rPlane = frame.Planes[0];
            PlaneData gPlane = frame.Planes[1];
            PlaneData bPlane = frame.Planes[2];
            byte[] bgr = new byte[rPlane.Width * rPlane.Height * 3];
            int dstStride = (int)(rPlane.Width * 3);

            for (uint y = 0; y < rPlane.Height; y++)
            {
                int rOffset = (int)(y * rPlane.StrideBytes);
                int gOffset = (int)(y * gPlane.StrideBytes);
                int bOffset = (int)(y * bPlane.StrideBytes);
                int dstOffset = (int)(y * dstStride);

                for (uint x = 0; x < rPlane.Width; x++)
                {
                    int di = dstOffset + (int)(x * 3);
                    bgr[di + 0] = bPlane.Data[bOffset + (int)x];
                    bgr[di + 1] = gPlane.Data[gOffset + (int)x];
                    bgr[di + 2] = rPlane.Data[rOffset + (int)x];
                }
            }

            return BitmapSource.Create((int)rPlane.Width, (int)rPlane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        private static BitmapSource ConvertPlanarBgrToBgr24(FrameData frame)
        {
            if (frame.Planes.Length < 3) return ConvertGeneric(frame);

            PlaneData bPlane = frame.Planes[0];
            PlaneData gPlane = frame.Planes[1];
            PlaneData rPlane = frame.Planes[2];
            byte[] bgr = new byte[bPlane.Width * bPlane.Height * 3];
            int dstStride = (int)(bPlane.Width * 3);

            for (uint y = 0; y < bPlane.Height; y++)
            {
                int bOffset = (int)(y * bPlane.StrideBytes);
                int gOffset = (int)(y * gPlane.StrideBytes);
                int rOffset = (int)(y * rPlane.StrideBytes);
                int dstOffset = (int)(y * dstStride);

                for (uint x = 0; x < bPlane.Width; x++)
                {
                    int di = dstOffset + (int)(x * 3);
                    bgr[di + 0] = bPlane.Data[bOffset + (int)x];
                    bgr[di + 1] = gPlane.Data[gOffset + (int)x];
                    bgr[di + 2] = rPlane.Data[rOffset + (int)x];
                }
            }

            return BitmapSource.Create((int)bPlane.Width, (int)bPlane.Height, 96, 96, PixelFormats.Bgr24, null, bgr, dstStride);
        }

        /// <summary>
        /// Generic fallback: attempt to display as grayscale from first channel of first plane.
        /// </summary>
        private static BitmapSource ConvertGeneric(FrameData frame)
        {
            PlaneData plane = frame.Planes[0];
            int bpe = frame.PixelFormat.BytesPerElement;

            if (bpe == 1 && plane.Channels == 1)
            {
                return CreateBitmap(frame, PixelFormats.Gray8);
            }

            // Fallback: extract first byte of each pixel as grayscale
            byte[] gray = new byte[plane.Width * plane.Height];
            int dstStride = (int)plane.Width;
            int srcBpp = (int)plane.BytesPerPixel;
            if (srcBpp == 0) srcBpp = (int)(plane.Channels * bpe);

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * dstStride);
                for (uint x = 0; x < plane.Width; x++)
                {
                    gray[dstOffset + (int)x] = plane.Data[srcOffset + (int)(x * srcBpp)];
                }
            }

            return BitmapSource.Create((int)plane.Width, (int)plane.Height, 96, 96, PixelFormats.Gray8, null, gray, dstStride);
        }
    }
}
