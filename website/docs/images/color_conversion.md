---
title: Color Conversion
sidebar_position: 7
---

import React from 'react';
import styles from '/docs/docs.css';

Ocean features a robust unit designed to convert images between various pixel formats. This unit is backed by highly optimized converters, offering options to balance between precision and performance.

These converters are part of Ocean's [Computer Vision (CV)](https://github.com/facebookresearch/ocean/tree/v1.0.0/impl/ocean/cv) library. Within this library, you'll encounter numerous converter classes prefixed with CV::FrameConverter.... The class names conclude with the source pixel format, indicating the starting format for conversion. For instance, [`CV::FrameConverterRGB24`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameConverterRGB24.h#L27) facilitates the conversion of RGB24 images to formats like BGR24, Y8, YUV24, or Y_VU12, among others. These classes primarily serve as wrappers around sophisticated conversion functions that utilize [color space transformation matrices](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameConverterRGB24.h#L576-L589) for efficient processing.

Ocean also provides straightforward helper functions for conversion, designed to simplify the process. An example of such a function is:

[`CV::FrameConverter::Comfort::convert(const Frame& source, const FrameType::PixelFormat targetPixelFormat, Frame& target, ...)`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameConverter.h#L725)

These functions include additional parameters that allow customization of the conversion process. For example, you can specify whether the converted image should own its memory (holding a copy of the image data) or if it can reuse the memory from the input image to enhance performance. Memory reuse is feasible when the input and output pixel formats are compatible, such as when both are identical or when converting from a Y_UV12 format to a Y8 format.

Additional options include the ability to flip the image upside-down, specify the target alpha value when the source image lacks an alpha channel, or set the gamma correction factor.

The supported conversion types are listed in the [`ConversionFunctionMap`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameConverter.cpp#L166). Additionally, the [`isSupported()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameConverter.h#L631) function can be utilized to verify if a specific conversion type is supported. If a required conversion from a source pixel format to a target pixel format is not available, it is straightforward to add missing converters thanks to Ocean's robust support for color space converters. This flexibility ensures that users can extend functionality to meet specific needs without significant hurdles.

### Code Example

```cpp
#include "ocean/cv/FrameConverter.h"

Frame sourceFrame = ...;

FrameType::PixelFormat targetPixelFormat = FrameType::FORMAT_RGB24;
bool forceCopy = FrameType::CP_AVOID_COPY_IF_POSSIBLE;

Frame targetFrame;
if (!CV::FrameConverter::Comfort::convert(sourceFrame, targetPixelFormat, targetFrame, forceCopy))
{
   Log::info() << "The source image could not be converted to RGB24";
   return;
}

assert(targetFrame.pixelFormat() == FrameType::FORMAT_RGB24);
```
