---
title: Padding and Stride
sidebar_position: 3
---

import React from 'react';
import styles from './images.css';

This page outlines how the image class in Ocean is structured, provides a list of best practices, and illustrates some common operations on images using examples.

Each plane can have optional padding memory at the end of each plane row. Planes may use different amounts of padding. The actual memory covered by a plane can be determined with the following equation

```cpp
plane_width_in_elements  = plane_width_in_pixels * plane_channels
plane_stride_in_elements = plane_width_in_elements + padding_in_elements
plane_stride_in_bytes    = plane_stride_in_elements * bytes_per_element
plane_memory_in_bytes    = plane_stride_in_bytes * plane_height_in_rows
```

## Bytes vs. Elements

Why does Ocean define padding memory in elements and not in bytes? When defining padding w.r.t. elements, a potential memory alignment pitfall can be avoided automatically without any additional effort.

Imagine an image with pixel format [`Y32`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L637) which is based on elements with data type `uint32_t`. On some platforms, e.g., `armv7`, each pixel value needs to be located in memory with a 32 bit aligned address - otherwise the process will crash when accessing the pixel values. If the plane would allow to define padding in bytes, the user could easily define a padding size which is not a multiple of 4 which would cause the app to crash.

<img src={require('@site/static/img/docs/images/Frame_Y32_InvalidPadding.png').default} alt="Image: A plane with 4 bytes per element and padding in bytes which is not a multiple of 4. This can cause apps to crash." width="700" className="center-image"/>

This kind of bug is actually very hard to find as it may not be reproducible. However, this can be easily prevented by defining padding **in elements**, i.e., the padding size in bytes is always and automatically a multiple of the element size (a multiple of 4 in case of `uint32_t` as in the example above).

<img src={require('@site/static/img/docs/images/Frame_Y32_ValidPadding.png').default} alt="Image: A plane which defines the padding in elements" width="700" className="center-image"/>

## Helper functions

Ocean has several helper functions to simplify the definition and usage of padding/stride. Often third-party projects specify padding/strides in bytes (and not in elements). For a safe transition, `Frame::strideBytes2paddingElements()` can be used.

```cpp
// the pixel format of the external source
FrameType::PixelFormat pixelFormat = ...

// the external source's image width in pixel
unsigned int imageWidth = ...;

// the external source's image stride in bytes
unsigned int strideInBytes = ...;

unsigned int paddingInElements;
if (Frame::strideBytes2paddingElements(pixelFormat, imageWidth, strideInBytes, paddingInElements))
{
    std::cout << "The source's padding elements: " << paddingInElements;
}
else
{
   throw "The external source has an invalid stride which leads to memory corruption";
}
```
