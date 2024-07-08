---
title: Code Examples
sidebar_position: 9
---

## Creating images

### Image with one plane

First, we define the type of the new image. The `FrameType` is specified by the image resolution, the pixel format, and the origin of the pixel data (either top-left corner, or bottom-left corner). Afterwards the new `Frame` object can be created which then will allocate the necessary memory. The memory will be uninitialized.

```cpp
#include "ocean/base/Frame.h"

using namespace Ocean;

FrameType frameType(1280u, 720u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT);
Frame frameOwningTheMemory(frameType);
```

### Image with two planes
Creating an image with different pixel format (e.g., a pixel format with two planes) is similar.

```cpp
FrameType frameType(1920u, 1080u, FrameType::FORMAT_Y_UV12, FrameType::ORIGIN_UPPER_LEFT);
Frame frameOwningTheMemory(frameType);
```

### Wrapping existing memory with *Frame*
Now, we create a new `Frame` object, we already have the memory and we do not want to make a copy of the memory. The pixel format has only one image plane.

```cpp
uint8_t* existingMemory = ...;
unsigned int paddingElements = ...;

FrameType frameType(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT);
Frame frameOwningTheMemory(frameType, existingMemory, Frame::CM_USE_KEEP_LAYOUT, paddingElements);
```


### Initializers for image with multiple planes

Now, we create a `Frame` object with a pixel format composed of several image planes.

```cpp
FrameType frameType(1920u, 1080u, FrameType::FORMAT_Y_U_V24, FrameType::ORIGIN_UPPER_LEFT);

Frame::PlaneInitializers<uint8_t> planeInitializers;

uint8_t* existingMemoryPlaneY = ...;
unsigned int paddingMemoryPlaneY = ...;

// the first plane will not be the owner of the given memory
planeInitializers.emplace_back(existingMemoryPlaneY, CM_USE_KEEP_LAYOUT, paddingMemoryPlaneY);

uint8_t* existingMemoryPlaneU = ...;
unsigned int paddingMemoryPlaneU = ...;

// the second plane will be the owner of the memory as a copy will be created
planeInitializers.emplace_back(existingMemoryPlaneU,
      CM_COPY_REMOVE_PADDING_LAYOUT, paddingMemoryPlaneU);

unsigned int paddingMemoryPlaneV = ...;

// the memory of the third plane does not exist,
// we create a third plane with own (un-initialized) memory
planeInitializers.emplace_back(paddingMemoryPlaneV);

Frame newFrame(frameType, planeInitializers);
```

A more compact way to express the above could be the following:

```cpp
Frame newFrame(frameType,
  Frame::PlaneInitializers<uint8_t>{
    Frame::PlaneInitializer<uint8_t>(planeY, CM_USE_KEEP_LAYOUT, paddingY),
    Frame::PlaneInitializer<uint8_t>(planeU, CM_COPY_REMOVE_PADDING_LAYOUT, paddingU),
    Frame::PlaneInitializer<uint8_t>(paddingV)
  });
```

### Custom pixel formats

At the time of writing, Ocean support 45+ [pre-defined pixel formats](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/base/Frame.h#L180) which commonly define the precise layout of the color channel (e.g., `FORMAT_RGB24` vs. `FORMAT_BGR24`). Adding a new pre-defined pixel format is straight forward. However, often a generic pixel format can be used on-the-fly in case the user does not need to keep track of e.g., the layout of the color channels.

This is an example of a generic pixel format for an image with two channels of `float` numbers on one plane (the properties of the pixel format are known at compile time):
```cpp
// generic pixel format, float, 2 channels, one plane
FrameType::PixelFormat customPixelFormat = FrameType::genericPixelFormat<float, 2u>();
FrameType frameType(1920u, 1080u, customPixelFormat, FrameType::ORIGIN_UPPER_LEFT);

Frame newFrame(frameType);
```

This is another example of a generic pixel format for an image with three `uint16_t` channels on one plane (the properties of the pixel format are known at run time):

```cpp
// generic pixel format, uint16_t, 3 channels, one plane

FrameType::DataType dataType = FrameType::DT_UNSIGNED_INTEGER_16;
FrameType::PixelFormat customPixelFormat = FrameType::genericPixelFormat(dataType, 3u);
FrameType frameType(1920u, 1080u, customPixelFormat, FrameType::ORIGIN_UPPER_LEFT);

Frame newFrame(frameType);
```

## Initializing image content

By default, the memory of a new `Frame` object will not be initialized (due to performance reasons).
However, the image content can be initialized with ease.

For an image with pixel format `RGB24`

```cpp
Frame rgbFrame(FrameType(1920u, 1080u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));

// set all elements of the frame to 0 (black)
rgbFrame.setValue(0x00u);

// set all elements of the frame to 255 (white)
rgbFrame.setValue(0xFFu);

// set all pixels of the frame to red
rgbFrame.setValue({0xFFu, 0x00u, 0x00u});

// set all pixels of the frame to yellow
rgbFrame.setValue({0xFFu, 0xFFu, 0x00u});

// setting the pixels of an inner rectangle to blue
rgbFrame.subFrame(100u, 100u, 400u, 400u).setValue({0x00u, 0x00u, 0xFFu});
```

For an image containing an alpha channel (e.g., `RGBA32`)

```cpp
Frame rgbaFrame(FrameType(1920u, 1080u, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_UPPER_LEFT));

// set all pixels to fully opaque green
constexpr uint8_t opaqueAlpha = 0xFFu;
rgbaFrame.setValue({0x00u, 0xFF, 0x00u, opaqueAlpha});

// set all pixels to fully transparent gray
constexpr uint8_t transparentAlpha = 0x00u;
rgbaFrame.setValue({0x80u, 0x80, 0x80u, transparentAlpha});
```

For an image with floating point data (e.g., `F64`)
```cpp
Frame depthFrame(FrameType(1920u, 1080u, FrameType::FORMAT_F64, FrameType::ORIGIN_UPPER_LEFT));

// setting all depth values to 99.0
depthFrame.setValue({99.0});
```

## Creating an image pyramid

Ocean's Computer Vision framework includes a robust image pyramid implementation, ideal for enhancing performance in various computer vision algorithms.
Below is an example demonstrating how to create an image pyramid [`CV::FramePyramid`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FramePyramid.h#L36) and access its individual layers:

```cpp
#include "ocean/base/Frame.h"
#include "ocean/base/Messenger.h"
#include "ocean/cv/FramePyramid.h"

Frame inputFrame = ...;

unsigned int layers = 4u; // the resulting pyramid will have four layers
bool copyFirstLayer = true; // the finest layer of the pyramid will be a copy of the input frame

FramePyramid framePyramid(inputFrame, layers, copyFirstLayer);

// let's iterate over all pyramid layers
for (unsigned int layerIndex = 0u; layerIndex < framePyramid.layers(); ++layerIndex)
{
    Frame& pyramidLayer = framePyramid[layerIndex];

    Log::info() << "Pyramid layer resolution " << pyramidLayer.width() << "x" << pyramidLayer.height();
}
```


## Checking properties of an image

Often functions need to ensure that a given image has a specific pixel format, is composed of several channels, planes or holds a specific element data type. Here are just some examples of functions the `Frame` class provides.

```cpp
void function(Frame& frame)
{
  if (frame.isValid() == false) // the frame does not contain any image data
    return;

  if (frame.pixelFormat() == FrameType::FORMAT_Y_UV12) // check specific pixel format
    return;

  if (frame.channels() != 3u) // check number of color channels
    return;

  if (frame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8) // check the data type
    return;

  if (frame.numberPlanes() != 1) // check number of image planes
     return;

  if (frame.isOwner() == false) // the Frame is not the owner of the memory
    return;

  if (frame.isReadOnly()) // the Frame's memory cannot be modified
    return;

  // check for general compatibility between pixel formats in terms of memory layout
  if (FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_RGB24) == false)
    return;

  uint8_t* firstPixel = frame.pixel(0u, 0u);
  firstPixel[0] = 0x00;
  firstPixel[1] = 0x80;
  firstPixel[2] = 0xFF;
}
```

## Resizing images

Ocean provides a simple method for image resizing and scaling.

```cpp
#include "ocean/cv/FrameInterpolator.h"

using namespace Ocean;

Frame sourceFrame = ...;

unsigned int newWidth = sourceFrame.width() * 3u / 4u;
unsigned int newHeight = sourceFrame.height();

FrameType resizedFrameType(sourceFrame.frameType(), newWidth, newHeight);
Frame resizedFrame(resizedFrameType);

CV::FrameInterpolator::resize(sourceFrame, resizedFrame);
```

For more details and a visual comparisons of the available image resizing methods, see [Image Resizing](resizing.md).

## Reading and writing images

Ocean supports several third-party image encoders and decoders. The [IO::Image](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/io/image/Image.h#L30) library wraps the third-party code and provides a unified and platform independent interface.

### Load from file

To load an image from a file on disk, do the following:

```cpp
#include "ocean/io/image/Image.h"

using namespace Ocean;

Frame loadedFrame = IO::Image::readImage("/path/to/my/image.png");

if (loadedFrame.isValid())
{
  // let's convert the PNG image to a JPG image
  IO::Image::writeImage(loadedFrame, "/path/to/my/image.jpg"))
}
```

### Load from memory

To load (and decode) an image from a buffer that is stored in memory, do the following:

```cpp
#include "ocean/io/image/Image.h"

using namespace Ocean;

const void* sourceImageBuffer = ...;
size_t sourceImageBufferSizeInBytes = ...;

Frame decodedFrame = IO::Image::decodeImage(sourceImageBuffer, sourceImageBufferSizeInBytes);

if (decodedFrame.isValid())
{
  // let's convert the image to a JPG image and store the data in a buffer

   std::vector<uint8_t> targetImageBuffer;
   IO::Image::encodeImage(decodedFrame, "jpg", targetImageBuffer))
}
```

## Debugging an image on iOS

Ocean provides simple access to use Apple's standard sharing capabilities via [`shareImages()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/platform/apple/ios/Utilities.h#L95). You can send images to your Mac via AirDrop, you can save an image locally or you can save an image in your Dropbox folder.

```cpp
#include "ocean/base/Frame.h"
#include "ocean/cv/Canvas.h"
#include "ocean/platform/apple/ios/Utilities.h"

using namespace Ocean;

void function(Frame& anyImage)
{
  // do something with the image, e.g., paint into the image

  // share the image
  Platform::Apple::IOS::Utilities::shareImage(anyImage);
}
```
