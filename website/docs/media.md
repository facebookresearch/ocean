---
title: Media
sidebar_position: 5
---

Ocean's media system is a cornerstone feature, providing developers with a unified and platform-independent interface to access a wide range of media types.
This powerful feature allows developers to write code once and deploy it across various platforms without the need for platform-specific adjustments.


## Location and Structure

The media libraries are located within the [ocean/media/...](https://github.com/facebookresearch/ocean/tree/v1.0.0/impl/ocean/media) directory.
This directory contains sub-folders with platform-specific implementations, which are generally abstracted away from the developers.
Instead, developers interact with Ocean's main media library, which offers platform-independent interfaces that encapsulate any platform-specific code.


## Supported Media Types

Ocean's media libraries provide access to common media types, including:

 - [**Image**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/media/Image.h#L36): Access the content of an image file.
 - [**ImageSequence**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/media/ImageFileSequence.h#L27): Access a sequence of images, such as frames extracted from a video.
 - [**Movie**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/media/Movie.h#L36): Playback movies from files.
 - [**LiveVideo**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/media/LiveVideo.h#L35): Access live video from devices like built-in cameras or external USB webcams.
 - [**Audio**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/media/Audio.h#L35): Play audio files, such as mp3 or wav.
 - [**Microphone**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/media/Microphone.h#L36): Access a device's microphone for live capture or video conferencing.
 - [**Recorders**](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/media/Recorder.h#L36): Record movies from rendered 3D scenes or augmented live video streams.

## Simplified Usage

The media classes are designed with inheritance to simplify development.
For instance, all media objects delivering image content (images, image sequences, movies, live videos, etc.) derive from [`Media::FrameMedium`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/media/FrameMedium.h#L52).
This design allows developers to implement code based on a **FrameMedium** object, automatically supporting a variety of media inputs.


### Example: Accessing Image Content

Here's how developers can implement an application to work with any media object that delivers image content:

```cpp
#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

// the url of the medium, e.g., an image file, a movie, the name of a Webcam etc.
std::string url = ...;

Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(url);

if (!frameMedium)
{
    // error handling: the provided url does not lead to a supported medium type
}

frameMedium->start();

while (true)
{
    FrameRef frame = frameMedium->frame();

    if (frame)
    {
        Log::info() << "Frame with resolution " << frame->width() << "x" << frame->height();

        /// access and use the image content here
    }
}
```

This example demonstrates the platform-independent nature of Ocean's media libraries, where the [`Media::Manager`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/media/Manager.h#L33) handles platform-specific details internally.


## Plugin Mechanism

The media system is implemented with a plugin mechanism, allowing developers to choose which media formats to support in their applications.
Some plugins, like [`Media::OpenImageLibraries`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/media/openimagelibraries/OpenImageLibraries.h#L24), are available on all platforms, providing access to images through well-known open-source libraries.
Others, such as [`Media::MediaFoundation`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/media/mediafoundation/MediaFoundation.h#L46) and [`Media::AVFoundation`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/media/avfoundation/AVFoundation.h#L33), are platform-specific.

### Registering Media Libraries

Developers must register the media libraries before accessing media objects.
Here's how to register some media libraries for multi-platform support:

```cpp
#include "ocean/media/Manager.h"

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS
    #include "ocean/media/mediafoundation/MediaFoundation.h
#endif

#ifdef OCEAN_PLATFORM_BUILD_APPLE
    #include "ocean/media/avfoundation/AVFoundation.h
#endif

using namespace Ocean::Media;

int main()
{
    // let's register some media plugins

    OpenImageLibraries::registerOpenImageLibrariesLibrary();

#ifdef OCEAN_PLATFORM_BUILD_WINDOWS
    MediaFoundation::registerMediaFoundationLibrary();
#endif

#ifdef OCEAN_PLATFORM_BUILD_APPLE
    AVFoundation::registerAVFLibrary();
#endif

    Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium("/path/to/media");

    // ...
}
```


## Conclusion

Ocean's media libraries are integral to nearly every module and application requiring media access within the Ocean ecosystem.
Their platform-independent design significantly eases the development process, enabling efficient and flexible media handling across different operating systems and devices.
