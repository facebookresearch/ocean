---
title: Pixel formats and plane layout
sidebar_position: 2
---

import React from 'react';
import styles from './images.css';

This section will discuss some examples of important pixel formats and explain their differences as well as their similarities.

## Format: *RGB24*

An image with pixel format [`RGB24`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L313) is composed of one plane and the plane has three channels (for red, green, and blue color values). The image memory is based on elements with data type `uint8_t`, with three elements representing a pixel so that each pixel needs 24 bits in memory. The image width in pixels is identical to the plane width in pixels. The plane width in elements is three times the width in pixels. The plane may contain padding elements at the end of each row which then increases the plane’s stride accordingly.

<img src={require('@site/static/img/docs/images/Frame_RGB24.png').default} alt="Image: The pixel format FORMAT_RGB24" width="700" className="center-image"/>

## Format: *Y8*

An image with format [`Y8`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L592) is very similar to `RGB24` but the plane has only one channel. Therefore, the width of the image in pixels is identical to the plane's width in elements.

<img src={require('@site/static/img/docs/images/Frame_Y8.png').default} alt="Image: The pixel format FORMAT_Y8" width="700" className="center-image"/>

## Format: *Y_UV12*

A common pixel format with two planes is e.g., [`Y_UV12`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L525). In Ocean, the  underscore (`_`) between `Y` and `UV` is used to denote that the image information is separated into two planes. The first plane contains the luminance channel of the image, while the second plane contains the two chrominance channels of the image. As a way to reduce bandwidth, the second plane does not define chrominance values for each pixel in the first plane. Instead it defines one for every second pixel only. Thus, the height of the first plane is two times the height of the second plane. In average, the image data is stored with 12 bits per pixels. It's important to note that due to the 2x2 downsampling of the second plane, this pixel format does not allow image dimensions with odd values.

<img src={require('@site/static/img/docs/images/Frame_Y_UV12.png').default} alt="Image: The pixel format FORMAT_Y_UV12" width="700" className="center-image"/>

## Format: *Y_U_V24*

Images with the format `Y_U_V24` are composed of three planes. Each plane holds one image channel without any sub-sampling.

<img src={require('@site/static/img/docs/images/Frame_Y_U_V24.png').default} alt="Image: The pixel format FORMAT_Y_U_V24" width="700" className="center-image"/>


## Value ranges: limited and full

Some pixel formats exist twice, once with a `_LIMITED_RANGE` suffix and once with a `_FULL_RANGE` suffix. The suffix does not change the memory layout or the plane layout at all — both variants are byte-for-byte identical in structure. It only states which numerical range the channel values are allowed to use:

| Variant | Y channel | U / V channels |
|---------|-----------|----------------|
| Full range | [0, 255] | [0, 255] |
| Limited range | [16, 235] | [16, 240] |

Limited range (also called *studio swing* or *video range*) comes from digital video transport rather than from color science. ITU-R BT.601 reserves the byte values `0x00` and `0xFF` for timing reference codes, and it reserves footroom below 16 and headroom above 235 so that filter overshoot survives the pipeline instead of being clipped. Cameras and video codecs therefore usually deliver limited range data, while computer graphics content and JPEG/JFIF use full range.

It is important to understand what the suffix is *for*. It does not change the pixel values; it records where the pixels came from, so that the converters can decide whether the values need to be rescaled. Getting it wrong is a roughly 9% level error in both directions:

* Limited range data treated as full range renders black at 16/255 and looks washed out.
* Full range data treated as limited range clips everything below 16 and above 235, which crushes blacks and blows out highlights.

Ocean provides these range variants for the following formats:

| Limited range | Full range |
|---------------|------------|
| `FORMAT_Y8_LIMITED_RANGE` | `FORMAT_Y8_FULL_RANGE` |
| `FORMAT_YUV24_LIMITED_RANGE` | `FORMAT_YUV24_FULL_RANGE` |
| `FORMAT_YVU24_LIMITED_RANGE` | `FORMAT_YVU24_FULL_RANGE` |
| `FORMAT_Y_UV12_LIMITED_RANGE` | `FORMAT_Y_UV12_FULL_RANGE` |
| `FORMAT_Y_VU12_LIMITED_RANGE` | `FORMAT_Y_VU12_FULL_RANGE` |
| `FORMAT_Y_U_V12_LIMITED_RANGE` | `FORMAT_Y_U_V12_FULL_RANGE` |
| `FORMAT_Y_V_U12_LIMITED_RANGE` | `FORMAT_Y_V_U12_FULL_RANGE` |
| `FORMAT_Y_U_V24_LIMITED_RANGE` | `FORMAT_Y_U_V24_FULL_RANGE` |

### Watch out for the short names

Every format above also has a short name without a suffix, and those short names are **not** consistent with each other:

* `FORMAT_Y8` is an alias for `FORMAT_Y8_FULL_RANGE`.
* All other short names (`FORMAT_YUV24`, `FORMAT_YVU24`, `FORMAT_Y_UV12`, `FORMAT_Y_VU12`, `FORMAT_Y_U_V12`, `FORMAT_Y_V_U12`, `FORMAT_Y_U_V24`) are aliases for their **limited** range variant.

So writing `FORMAT_Y8` and `FORMAT_Y_UV12` in the same piece of code silently mixes two different conventions. Prefer the explicit `_LIMITED_RANGE` and `_FULL_RANGE` names in new code.

`FrameType::formatIsLimitedRange()` reports which convention a pixel format follows. Note that Ocean's RGB formats are always full range; there is no limited range RGB pixel format.

### The range is not the color space

The value range is only one of four attributes that define a Y'CbCr encoding. The other three are the matrix coefficients (BT.601, BT.709, BT.2020), the color primaries, and the transfer function. Only the value range is encoded in the pixel format; the matrix is chosen by the converter you call.

So `FORMAT_YUV24_LIMITED_RANGE` tells you the channels are in [16, 235] and [16, 240], and nothing more. It does not say whether the data is BT.601 or BT.709. Ocean's converters currently implement BT.601 only. See [Color Conversion](color_conversion.md) for the available transformation matrices and what that restriction means for HD and UHD content.

## Pre-defined pixel formats

Below, you will find all currently defined pixel formats in Ocean (some formats are left out).
Please refer to the accompanying documentation for detailed information about memory layout and value ranges.
The most common pixel formats are highlighted for easy reference.

| Type         | Name                                   | Element Type     | Alpha | Comment                                                          |
|--------------|----------------------------------------|------------------|-------|------------------------------------------------------------------|
| Color (RGB)  | **FORMAT_RGB24**                       | uint8_t          |       | Standard RGB format with 8 bits per channel                      |
|              | **FORMAT_RGBA32**                      | uint8_t          | Yes   | Standard RGB format with 8 bits and alpha                        |
|              | FORMAT_RGB32                           | uint8_t          |       | Includes 8 unused bits                                           |
|              | FORMAT_RGB48                           | uint16_t         |       | 16 bits per channel                                              |
|              | FORMAT_RGBA64                          | uint16_t         | Yes   |                                                                  |
|              | FORMAT_ARGB32                          | uint8_t          | Yes   | First alpha channel, then color channels                         |
|              | FORMAT_RGB4444                         | uint16_t         |       | 4 bits per channels, includes 4 unused bits                      |
|              | FORMAT_RGBA4444                        | uint16_t         | Yes   |                                                                  |
|              | FORMAT_RGB5551                         | uint16_t         |       | 5 bits per channel, includes 1 unused bit                        |
|              | FORMAT_RGB565                          | uint16_t         |       | 5 bits for red/blue, 6 bits for green                            |
|              | FORMAT_RGBT32                          | uint8_t          |       | Contains a custom *texture* channel                              |
|              | FORMAT_RGGB10_PACKED                   | uint8_t          |       | Packed Bayer, 10 bits per channel                                |
|              |                                        |                  |       |                                                                  |
| Color (BGR)  | **FORMAT_BGR24**                       | uint8_t          |       | as RGB24 but with reverse channel order                          |
|              | **FORMAT_BGRA32**                      | uint8_t          | Yes   |                                                                  |
|              | FORMAT_BGR32                           | uint8_t          |       |                                                                  |
|              | FORMAT_BGR4444                         | uint16_t         |       |                                                                  |
|              | FORMAT_BGRA4444                        | uint8_t          | Yes   |                                                                  |
|              | FORMAT_BGR5551                         | uint16           |       |                                                                  |
|              | FORMAT_BGR565                          | uint16_t         |       |                                                                  |
|              | FORMAT_ABGR32                          | uint8_t          | Yes   | First alpha channel, then color channels                         |
|              | FORMAT_BGGR10_PACKED                   | uint8_t          |       | Packed Bayer, 10 bits per channel                                |
|              |                                        |                  |       |                                                                  |
| Y (Gray)     | **FORMAT_Y8**                          | uint8_t          |       | Grayscale image with 8 bits, alias for `FORMAT_Y8_FULL_RANGE`    |
|              | FORMAT_Y8_FULL_RANGE                   | uint8_t          |       | Y in [0, 255]                                                    |
|              | FORMAT_Y8_LIMITED_RANGE                | uint8_t          |       | Y in [16, 235]                                                   |
|              | FORMAT_Y16                             | uint16_t         |       | Grayscale image with 16 bits                                     |
|              | **FORMAT_Y32**                         | uint32_t         |       |                                                                  |
|              | FORMAT_Y64                             | uint64_t         |       |                                                                  |
|              | **FORMAT_YA16**                        | uint8_t          | Yes   | Grayscale image with 8 bits and alpha                            |
|              | FORMAT_Y10                             | uint16_t         |       | Grayscale image, 10 bits, 6 bits unused                          |
|              | FORMAT_Y10_PACKED                      | uint8_t          |       | Packed Bayer, 10 bits per pixel                                  |
|              |                                        |                  |       |                                                                  |
| Color (YUV)  | **FORMAT_YUV24**                       | uint8_t          |       | Luminance (Y), Chrominance Blue (U, V), alias for the limited range variant |
|              | FORMAT_YUV24_LIMITED_RANGE             | uint8_t          |       | Y in [16, 235], U/V in [16, 240]                                 |
|              | FORMAT_YUV24_FULL_RANGE                | uint8_t          |       | All channels in [0, 255]                                         |
|              | FORMAT_YVU24                           | uint8_t          |       | Flipped chrominance channels, alias for the limited range variant |
|              | FORMAT_YVU24_LIMITED_RANGE             | uint8_t          |       | Y in [16, 235], V/U in [16, 240]                                 |
|              | FORMAT_YVU24_FULL_RANGE                | uint8_t          |       | All channels in [0, 255]                                         |
|              | FORMAT_YUVA32                          | uint8_t          | Yes   |                                                                  |
|              | FORMAT_YUVT32                          | uint8_t          |       | Contains a custom *texture* channel                              |
|              | FORMAT_YUYV16                          | uint8_t          |       | 16 bits per pixel, U/V every second pixel, aka YUY2              |
|              | FORMAT_UYVY16                          | uint8_t          |       | Known as UYVY                                                    |
|              | **FORMAT_Y_UV12**                      | uint8_t          |       | One gray, one chrominance plane, aka NV12, alias for the limited range variant |
|              | FORMAT_Y_UV12_LIMITED_RANGE            | uint8_t          |       | Y in [16, 235], U/V in [16, 240]                                 |
|              | FORMAT_Y_UV12_FULL_RANGE               | uint8_t          |       | All channels in [0, 255]                                         |
|              | FORMAT_Y_VU12                          | uint8_t          |       | Known as NV21, alias for the limited range variant               |
|              | FORMAT_Y_VU12_LIMITED_RANGE            | uint8_t          |       | Y in [16, 235], V/U in [16, 240]                                 |
|              | FORMAT_Y_VU12_FULL_RANGE               | uint8_t          |       | All channels in [0, 255]                                         |
|              | FORMAT_Y_U_V24                         | uint8_t          |       | Known as I444, alias for the limited range variant               |
|              | FORMAT_Y_U_V24_LIMITED_RANGE           | uint8_t          |       | Y in [16, 235], U/V in [16, 240]                                 |
|              | FORMAT_Y_U_V24_FULL_RANGE              | uint8_t          |       | All channels in [0, 255]                                         |
|              | FORMAT_Y_U_V12                         | uint8_t          |       | Known as I420, alias for the limited range variant               |
|              | FORMAT_Y_U_V12_LIMITED_RANGE           | uint8_t          |       | Y in [16, 235], U/V in [16, 240]                                 |
|              | FORMAT_Y_U_V12_FULL_RANGE              | uint8_t          |       | All channels in [0, 255]                                         |
|              | FORMAT_Y_V_U12                         | uint8_t          |       | Known as YV12, alias for the limited range variant               |
|              | FORMAT_Y_V_U12_LIMITED_RANGE           | uint8_t          |       | Y in [16, 235], V/U in [16, 240]                                 |
|              | FORMAT_Y_V_U12_FULL_RANGE              | uint8_t          |       | All channels in [0, 255]                                         |
|              |                                        |                  |       |                                                                  |
| Depth        | **FORMAT_F32**                         | float            |       | Floats with 32 bit precision, e.g., for depth                    |
|              | FORMAT_F64                             | double           |       | Floats with 64 bit precision                                     |

## Custom pixel formats

In addition to the predefined pixel formats, Ocean allows the definition of custom pixel formats both at runtime and compile time.
Below are examples demonstrating how to easily define such generic pixel formats:

Pixel format for single-plane images with two channels, each represented by a `float`:
```cpp
FrameType::genericPixelFormat<float, 2u>();
```

Pixel format for single-plane images with three channels, each represented by a `uint32_t`:
```cpp
FrameType::genericPixelFormat<uint32_t, 3u>();
```
