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

### Limited and full value range

Several pixel formats exist in a `_LIMITED_RANGE` and a `_FULL_RANGE` variant, as described in [Pixel formats and plane layout](pixel_formats_and_plane_layout.md). The value range is part of the frame type, so the converters treat the two variants as genuinely different source and target formats: `ConversionFunctionMap` is keyed on the exact pixel format, and the registered coverage is not identical for both variants.

Two practical consequences:

* Converting a `Y_UV12` image to `FORMAT_Y8` can re-use the memory of the source image's first plane, but only when the value ranges match. Because `FORMAT_Y8` is an alias for `FORMAT_Y8_FULL_RANGE`, requesting it for a limited range source forces an allocation and a rescale. Use [`FrameType::formatGrayscalePixelFormat()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L1592) to pick the grayscale format that matches the source, which keeps the zero-copy path available:

  ```cpp
  Frame yFrame;
  CV::FrameConverter::Comfort::convert(inputFrame, FrameType::formatGrayscalePixelFormat(inputFrame.pixelFormat()), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE);
  ```

* Converting between the two range variants of the same format is an explicit conversion, not a re-interpretation. `CV::FrameConverterY8` provides `convertY8LimitedRangeToY8FullRange()` and `convertY8FullRangeToY8LimitedRange()`, which apply the `255/219` rescale in fixed-point arithmetic.

### Color spaces and transformation matrices

Converting between RGB and Y'CbCr is defined by four independent attributes. Mixing them up is the usual cause of subtly wrong colors:

| Attribute | Examples | Encoded in Ocean's `PixelFormat`? |
|-----------|----------|-----------------------------------|
| Value range | limited, full | **Yes** |
| Matrix coefficients (Kr / Kb) | BT.601, BT.709, BT.2020 | No |
| Color primaries | BT.601, BT.709, BT.2020 | No |
| Transfer function (gamma) | sRGB, BT.1886, PQ | No |

Only the value range is part of the frame type. Everything else is implied by *which converter function you call*, not by the pixel format. This is why two conversions with the same source and target pixel format can still produce different results.

Ocean's matrices are exposed explicitly on `CV::FrameConverter`, and the name states both the range and the standard:

| Matrix | Purpose |
|--------|---------|
| `transformationMatrix_FullRangeRGB24_To_FullRangeYUV24_BT601()` | full range RGB to full range Y'CbCr; these are the digitized YPbPr equations, the same matrix JPEG/JFIF uses |
| `transformationMatrix_FullRangeRGB24_To_LimitedRangeYUV24_BT601()` | full range RGB to studio-swing Y'CbCr |
| `transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601()` | studio-swing Y'CbCr back to full range RGB |
| `transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_BT601()` | full range Y'CbCr to full range RGB |
| `transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_Android()` | the variant Android uses for `Y'UV420sp` (NV21); close to BT.601 but not identical |

Corresponding `BGR24` and `YVU24` variants exist for each.

The difference between the range variants is not a rounding detail. The full range luma row is `{0.299, 0.587, 0.114}` with no offset, while the limited range row is `{0.2578, 0.5039, 0.0977}` with an offset of `16`. In the 7-bit fixed-point form used by the integer implementations, that is `{38, 75, 15} + 0` versus `{33, 64, 13} + 16`.

**Ocean currently implements BT.601 only.** There are no BT.709 or BT.2020 matrices in `ocean/cv`. Since HD video is normally BT.709 and UHD is normally BT.2020, converting such content with Ocean applies BT.601 coefficients, which introduces a small hue and saturation error on saturated colors. Nothing in the type system flags this, so it is worth being aware of when the source is HD or UHD video.

Note also that all conversions operate on gamma-encoded values. Ocean's `Y` is luma (`Y'`), not CIE luminance; no linearization is performed.

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
