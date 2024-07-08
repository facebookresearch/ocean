---
title: Images in Ocean
sidebar_position: 1
---

This page outlines how the image class in Ocean is structured, provides a list of best practices, and illustrates some common operations on images using examples.

In Ocean, the [`Frame`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L1766) class is the official container for images. It is a lightweight object for images with arbitrary pixel formats. The class supports all common pixel formats. This includes pixel formats like `RGB24` or `Y8`, frames with multiple planes like `Y_UV12` (aka `NV12`), packed pixels formats like `Y10_Packed` (aka `RAW10`), and Bayer mosaic pixel formats encoding image information from a sensor. More than 45+ [pre-defined pixel formats](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L180) exist.

Each frame is composed of at least one plane containing the image information. Each plane can have its own *width*, *height*, and *channel number*. Planes support padding memory at the end of each plane row. Each plane can either own the memory or just use a memory pointer from an external source.
The plane memory is composed of elements of a specific data type, e.g., `uint8_t`, `uint16_t`, `float`, or similar. Depending on the pixel format, a pixel is formed by one or more elements. All planes have the same data type. In case an image has several planes, the value of an image pixel is distributed over several plane pixels, e.g., `Y_UV24`. Otherwise all channels are stored in a single plane, e.g., `RGB24`. This will be explained in details in the examples below.
