/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

using System;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace OceanFrameInspectorExtension.Imaging
{
    /// <summary>
    /// Generates color map lookup tables (Jet, Turbo, Inferno) and applies them to single-channel frames.
    /// </summary>
    public static class ColorMapGenerator
    {
        /// <summary>
        /// Generates a 256-entry RGB LUT for the specified color map.
        /// </summary>
        public static byte[,] GenerateLut(ColorMapType colorMap)
        {
            return colorMap switch
            {
                ColorMapType.Jet => GenerateJetLut(),
                ColorMapType.Turbo => GenerateTurboLut(),
                ColorMapType.Inferno => GenerateInfernoLut(),
                ColorMapType.Spiral10 => GenerateSpiralLut(10),
                ColorMapType.Spiral20 => GenerateSpiralLut(20),
                ColorMapType.Linear => GenerateGrayLut(),
                _ => GenerateGrayLut()
            };
        }

        /// <summary>
        /// Applies a color map to a single-channel frame, producing a BGR24 BitmapSource.
        /// </summary>
        public static BitmapSource Apply(FrameData frame, ColorMapType colorMap)
        {
            if (frame == null) throw new ArgumentNullException(nameof(frame));

            byte[,] lut = GenerateLut(colorMap);
            PlaneData plane = frame.Planes[0];
            int bpe = frame.PixelFormat.BytesPerElement;

            // First normalize to 0-255 range
            byte[] normalized = NormalizeToBytes(plane, bpe, frame.PixelFormat);

            byte[] bgr = new byte[plane.Width * plane.Height * 3];
            int dstStride = (int)(plane.Width * 3);

            for (int i = 0; i < normalized.Length; i++)
            {
                byte val = normalized[i];
                int di = i * 3;
                bgr[di + 0] = lut[val, 2]; // B
                bgr[di + 1] = lut[val, 1]; // G
                bgr[di + 2] = lut[val, 0]; // R
            }

            BitmapSource result = BitmapSource.Create(
                (int)plane.Width, (int)plane.Height,
                96, 96, PixelFormats.Bgr24, null, bgr, dstStride);

            if (frame.PixelOrigin == PixelOriginType.LowerLeft)
            {
                result = new TransformedBitmap(result, new ScaleTransform(1, -1));
            }

            result.Freeze();
            return result;
        }

        private static byte[] NormalizeToBytes(PlaneData plane, int bpe, PixelFormatInfo format)
        {
            byte[] result = new byte[plane.Width * plane.Height];

            int pixelStride = (int)plane.BytesPerPixel;
            if (pixelStride == 0) pixelStride = (int)plane.Channels * bpe;
            if (pixelStride == 0) pixelStride = bpe;

            if (bpe == 4 && format.DataType == OceanDataType.SignedFloat32)
                NormalizeFloat32(plane, pixelStride, result);
            else
                NormalizeInteger(plane, bpe, pixelStride, result);

            return result;
        }

        private static void NormalizeInteger(PlaneData plane, int bpe, int pixelStride, byte[] result)
        {
            int shift = bpe == 2 ? 8 : 0;
            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * plane.Width);
                for (uint x = 0; x < plane.Width; x++)
                {
                    int srcIdx = srcOffset + (int)(x * pixelStride);
                    if (bpe == 1)
                        result[dstOffset + x] = plane.Data[srcIdx];
                    else if (bpe == 2)
                        result[dstOffset + x] = (byte)(BitConverter.ToUInt16(plane.Data, srcIdx) >> shift);
                    else
                        result[dstOffset + x] = plane.Data[srcIdx];
                }
            }
        }

        private static void NormalizeFloat32(PlaneData plane, int pixelStride, byte[] result)
        {
            float min = float.MaxValue, max = float.MinValue;
            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                for (uint x = 0; x < plane.Width; x++)
                {
                    float val = BitConverter.ToSingle(plane.Data, srcOffset + (int)(x * pixelStride));
                    if (!float.IsNaN(val) && !float.IsInfinity(val))
                    {
                        if (val < min) min = val;
                        if (val > max) max = val;
                    }
                }
            }

            float range = max - min;
            if (range < float.Epsilon) range = 1.0f;

            for (uint y = 0; y < plane.Height; y++)
            {
                int srcOffset = (int)(y * plane.StrideBytes);
                int dstOffset = (int)(y * plane.Width);
                for (uint x = 0; x < plane.Width; x++)
                {
                    float val = BitConverter.ToSingle(plane.Data, srcOffset + (int)(x * pixelStride));
                    result[dstOffset + x] = (float.IsNaN(val) || float.IsInfinity(val))
                        ? (byte)0
                        : (byte)Math.Max(0, Math.Min(255, (val - min) / range * 255.0f));
                }
            }
        }

        private static byte Clamp(double v) => (byte)Math.Max(0, Math.Min(255, (int)(v * 255.0 + 0.5)));

        private static byte[,] GenerateGrayLut()
        {
            byte[,] lut = new byte[256, 3];
            for (int i = 0; i < 256; i++)
            {
                lut[i, 0] = (byte)i;
                lut[i, 1] = (byte)i;
                lut[i, 2] = (byte)i;
            }
            return lut;
        }

        private static byte[,] GenerateJetLut()
        {
            byte[,] lut = new byte[256, 3];
            for (int i = 0; i < 256; i++)
            {
                double t = i / 255.0;
                double r, g, b;

                if (t < 0.125)
                {
                    r = 0; g = 0; b = 0.5 + t * 4.0;
                }
                else if (t < 0.375)
                {
                    r = 0; g = (t - 0.125) * 4.0; b = 1.0;
                }
                else if (t < 0.625)
                {
                    r = (t - 0.375) * 4.0; g = 1.0; b = 1.0 - (t - 0.375) * 4.0;
                }
                else if (t < 0.875)
                {
                    r = 1.0; g = 1.0 - (t - 0.625) * 4.0; b = 0;
                }
                else
                {
                    r = 1.0 - (t - 0.875) * 4.0; g = 0; b = 0;
                }

                lut[i, 0] = Clamp(r);
                lut[i, 1] = Clamp(g);
                lut[i, 2] = Clamp(b);
            }
            return lut;
        }

        private static byte[,] GenerateTurboLut()
        {
            // Turbo colormap approximation based on Google's Turbo colormap
            byte[,] lut = new byte[256, 3];
            for (int i = 0; i < 256; i++)
            {
                double t = i / 255.0;

                double r = Math.Max(0, Math.Min(1, 0.13572138 + t * (4.61539260 + t * (-42.66032258 + t * (132.13108234 + t * (-152.94239396 + t * 59.28637943))))));
                double g = Math.Max(0, Math.Min(1, 0.09140261 + t * (2.19418839 + t * (4.84296658 + t * (-14.18503333 + t * (4.27729857 + t * 2.82956604))))));
                double b = Math.Max(0, Math.Min(1, 0.10667330 + t * (12.64194608 + t * (-60.58204836 + t * (110.36276771 + t * (-89.90310912 + t * 27.34824973))))));

                lut[i, 0] = Clamp(r);
                lut[i, 1] = Clamp(g);
                lut[i, 2] = Clamp(b);
            }
            return lut;
        }

        private static byte[,] GenerateInfernoLut()
        {
            // Inferno colormap approximation
            byte[,] lut = new byte[256, 3];
            for (int i = 0; i < 256; i++)
            {
                double t = i / 255.0;

                double r = Math.Max(0, Math.Min(1, t < 0.5
                    ? -0.0155 + t * (8.7425 + t * (-33.2624 + t * 52.1768))
                    : 0.9656 + t * (1.1789 + t * (-3.5765 + t * 2.3204))));

                double g = Math.Max(0, Math.Min(1, t < 0.5
                    ? 0.0016 + t * (-0.2987 + t * (7.7327 + t * (-12.0498)))
                    : -1.3914 + t * (7.5966 + t * (-10.7570 + t * 5.3970))));

                double b = Math.Max(0, Math.Min(1, t < 0.5
                    ? 0.0144 + t * (5.8945 + t * (-20.7445 + t * 25.6376))
                    : 3.0040 + t * (-10.7862 + t * (13.1882 + t * (-5.4710)))));

                lut[i, 0] = Clamp(r);
                lut[i, 1] = Clamp(g);
                lut[i, 2] = Clamp(b);
            }
            return lut;
        }

        /// <summary>
        /// Generates a spiral (HSV hue rotation) LUT matching Ocean's FrameConverterColorMap::CM_SPIRAL.
        /// The hue cycles through the full spectrum 'rounds' times, with brightness modulation
        /// (dark at low values, bright at high values).
        /// </summary>
        private static byte[,] GenerateSpiralLut(int rounds)
        {
            byte[,] lut = new byte[256, 3];
            for (int i = 0; i < 256; i++)
            {
                double t = i / 255.0; // normalized value [0, 1]

                // Hue position: cycles 'rounds' times through [0, 2*PI]
                double hue = (t * rounds % 1.0) * 2.0 * Math.PI;

                // Brightness modulation: maps t from [-0.8, 0.8]
                // Below 0: fade to black; Above 0: fade to white
                double brightness = t * 1.6 - 0.8;

                // HSV to RGB (saturation=1, value=1)
                double h6 = hue / (Math.PI / 3.0); // hue in [0, 6)
                int sector = (int)h6 % 6;
                if (sector < 0) sector += 6;
                double frac = h6 - Math.Floor(h6);

                double r, g, b;
                switch (sector)
                {
                    case 0: r = 1; g = frac; b = 0; break;
                    case 1: r = 1 - frac; g = 1; b = 0; break;
                    case 2: r = 0; g = 1; b = frac; break;
                    case 3: r = 0; g = 1 - frac; b = 1; break;
                    case 4: r = frac; g = 0; b = 1; break;
                    default: r = 1; g = 0; b = 1 - frac; break;
                }

                // Apply brightness: negative darkens (toward black), positive lightens (toward white)
                if (brightness < 0)
                {
                    double factor = 1.0 + brightness; // [0.2, 1.0]
                    r *= factor;
                    g *= factor;
                    b *= factor;
                }
                else
                {
                    r += (1.0 - r) * brightness;
                    g += (1.0 - g) * brightness;
                    b += (1.0 - b) * brightness;
                }

                lut[i, 0] = Clamp(r);
                lut[i, 1] = Clamp(g);
                lut[i, 2] = Clamp(b);
            }
            return lut;
        }
    }

    public enum ColorMapType
    {
        None,
        Jet,
        Turbo,
        Inferno,
        Spiral10,
        Spiral20,
        Linear
    }
}
