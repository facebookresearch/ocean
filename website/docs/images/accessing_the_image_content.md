---
title: Accessing the Image Content
sidebar_position: 4
---

The `Frame` class provides several accessors for the actual image information.

## Plane access

The memory of individual image planes with `planeIndex`, can be accessed with [`constdata()`](https://github.com/facebookresearch/ocean/blob/main/impl/ocean/base/Frame.h#L2706-L2707) and [`data()`](https://github.com/facebookresearch/ocean/blob/main/impl/ocean/base/Frame.h#L2696-L2697):

```cpp
template <typename T>
const T* Frame::constdata(const unsigned int planeIndex = 0u) const;

template <typename T>
T* Frame::data(const unsigned int planeIndex = 0u);
```

Here, the type `T` denotes the underlying type of the pixel elements. For example, for `FORMAT_RGB24` it will be `T = uint8_t`, and for `FORMAT_RGB48` it will be `T = uint16_t`.

## Row access

An individual row of an image plane, `planeIndex`, can be accessed with [`constrow()`](https://github.com/facebookresearch/ocean/blob/main/impl/ocean/base/Frame.h#L2742-L2743) and [`row()`](https://github.com/facebookresearch/ocean/blob/main/impl/ocean/base/Frame.h#L2724-L2725):

```cpp
template <typename T>
const T* Frame::constrow(const unsigned int y, const unsigned int planeIndex = 0u) const;

template <typename T>
T* Frame::row(const unsigned int y, const unsigned int planeIndex = 0u);
```

## Pixel access

In case one pixel values need to be accessed, use [`constpixel()`](https://github.com/facebookresearch/ocean/blob/main/impl/ocean/base/Frame.h#L2822-L2823) and [`pixel()`](https://github.com/facebookresearch/ocean/blob/main/impl/ocean/base/Frame.h#L2797-L2798):

```cpp
template <typename T>
const T* Frame::constpixel(const unsigned int x, const unsigned int y, const unsigned int planeIndex = 0u) const;

template <typename T>
T* Frame::pixel(const unsigned int x, const unsigned int y, const unsigned int planeIndex = 0u);
```

As a rule of thumb, it is recommended to avoid accessing individual pixels and to operate on an entire row instead. This will improve the execution performance.
