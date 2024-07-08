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
| Y (Gray)     | **FORMAT_Y8**                          | uint8_t          |       | Grayscale image with 8 bits                                      |
|              | FORMAT_Y16                             | uint16_t         |       | Grayscale image with 16 bits                                     |
|              | **FORMAT_Y32**                         | uint32_t         |       |                                                                  |
|              | FORMAT_Y64                             | uint64_t         |       |                                                                  |
|              | **FORMAT_YA16**                        | uint8_t          | Yes   | Grayscale image with 8 bits and alpha                            |
|              | FORMAT_Y10                             | uint16_t         |       | Grayscale image, 10 bits, 6 bits unused                          |
|              | FORMAT_Y10_PACKED                      | uint8_t          |       | Packed Bayer, 10 bits per pixel                                  |
|              |                                        |                  |       |                                                                  |
| Color (YUV)  | **FORMAT_YUV24**                       | uint8_t          |       | Luminance (Y), Chrominance Blue (U, V)                           |
|              | FORMAT_YVU24                           | uint8_t          |       | Flipped chrominance channels                                     |
|              | FORMAT_YUVA32                          | uint8_t          | Yes   |                                                                  |
|              | FORMAT_YUVT32                          | uint8_t          |       | Contains a custom *texture* channel                              |
|              | FORMAT_YUYV16                          | uint8_t          |       | 16 bits per pixel, U/V every second pixel, aka YUY2              |
|              | FORMAT_UYVY16                          | uint8_t          |       | Known as UYVY                                                    |
|              | **FORMAT_Y_UV12**                      | uint8_t          |       | One gray, one chrominance plane, aka NV12                        |
|              | FORMAT_Y_VU12                          | uint8_t          |       | Known as NV21                                                    |
|              | FORMAT_Y_U_V24                         | uint8_t          |       | Known as I444                                                    |
|              | FORMAT_Y_U_V12                         | uint8_t          |       | Known as I420                                                    |
|              | FORMAT_Y_V_U12                         | uint8_t          |       | Known as YV12                                                    |
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
