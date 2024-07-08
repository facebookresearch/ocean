---
title: Accessing the Image Content
sidebar_position: 4
---

The `Frame` class provides several accessors for the actual image information.

## Plane access

The memory of individual image planes with `planeIndex`, can be accessed with [`constdata()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L1945) and [`data()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L1953):

```cpp
template <typename T>
const T* Frame::constdata(const unsigned int planeIndex = 0u) const;

template <typename T>
T* Frame::data(const unsigned int planeIndex = 0u);
```

Here, the type `T` denotes the underlying type of the pixel elements. For example, for `FORMAT_RGB24` it will be `T = uint8_t`, and for `FORMAT_RGB48` it will be `T = uint16_t`.

## Row access

An individual row of an image plane, `planeIndex`, can be accessed with [`constrow()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L2787) and [`row()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L2769):

```cpp
template <typename T>
const T* Frame::constrow(const unsigned int y, const unsigned int planeIndex = 0u) const;

template <typename T>
T* Frame::row(const unsigned int y, const unsigned int planeIndex = 0u);
```

Below is a code fragment demonstrating how to iterate over all pixels in an image and check whether all pixels are black.

```cpp
#include "ocean/base/Frame.h"

bool isFrameBlack(const Frame& rgbFrame)
{
    ocean_assert(rgbFrame.pixelFormat() == FrameType::FORMAT_RGB24);

    for (unsigned int y = 0u; y < rgbFrame.height(); ++y)
    {
        const uint8_t* row = rgbFrame.constrow<uint8_t>(y);

        for (unsigned int x = 0u; x < rgbFrame.width(); ++x)
        {
            if (row[0] != 0x00u || row[1] != 0x00u || row[2] != 0x00u)
                return false;

            row += 3;
        }
    }

    return true;
}
```

## Pixel access

In case one pixel values need to be accessed, use [`constpixel()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L2867) and [`pixel()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L2842):

```cpp
template <typename T>
const T* Frame::constpixel(const unsigned int x, const unsigned int y, const unsigned int planeIndex = 0u) const;

template <typename T>
T* Frame::pixel(const unsigned int x, const unsigned int y, const unsigned int planeIndex = 0u);
```

As a rule of thumb, it is recommended to avoid accessing individual pixels and to operate on an entire row instead.
This will improve the execution performance (see example above).

Below is a code fragment demonstrating how to access a specific pixel in an image.

```cpp
#include "ocean/base/Frame.h"

bool isPixelBlack(const Frame& rgbFrame, unsigned int x, unsigned int y)
{
    ocean_assert(rgbFrame.pixelFormat() == FrameType::FORMAT_RGB24);
    ocean_assert(x < rgbFrame.width() && y < rgbFrame.height());

    const uint8_t* pixel = rgbFrame.constpixel<uint8_t>(x, y);

    return pixel[0] == 0x00u && pixel[1] == 0x00u && pixel[2] == 0x00u;
}
```
