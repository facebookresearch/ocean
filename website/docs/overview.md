---
title: Overview
sidebar_position: 2

description: Ocean structure - an overview.
image: ../static/img/ocean-social-card.jpg
---

The Ocean framework is a large, organized collection of components and libraries for various tasks,
including computer vision, geometry, media handling, networking, and rendering. This page provides
a general overview of Ocean and explains the structure of the code and how that relates to target
names.

## Main Directories

The framework is quite large, encompassing various components organized into main directories. At
the top-level these include:

* `build/` - build configurations for various build systems, for example: Visual Studio, XCode,
  CMake, or Gradle
* `doc/` - a collection miscellaneous documents
* `impl/` - contains all the source code; more details below
* `res/` - contains all the resource files. This is important mostly for building apps
* `website/` - the location where the source of website that you're reading right now is stored and
  managed.

The `impl/` directory, in particular, is divided into subdirectories, including `application/`,
which holds the source code for test and demo apps, as well as app-specific libraries like native
libraries for Android apps. Another key subdirectory is `ocean/`, containing the source code for
Ocean libraries and GTest executables.

## Code structure And Naming Conventions

The source code is organized in a hierarchical structure of directories and targets grouped by
topic, such as computer vision or rendering. Typically, each directory corresponds to one build
target, with more specialized uses in deeper directories. The naming of build targets matches the
directory name, for example, code in `ocean/cv/detector/qrcodes` belongs to the target
`ocean_cv_detector_qrcodes`.

### Libraries (`impl/ocean`)

The targets for all Ocean libraries begin with `ocean_*` followed by the name of the library,
`ocean_NAME`, for example:

* `ocean_base`
* `ocean_cv`
* `ocean_geometry`
* `ocean_media`
* `ocean_...`

Several main libraries come with sub-libraries which provide specialized functionalities or platform
specific implementations. These sub-libraries have an additional suffix: `ocean_NAME_SUBNAME`, for
example:

* `ocean_cv_advanced`
* `ocean_cv_detector`
* `ocean_cv_detector_qrcodes`
* `ocean_cv_...`
* `ocean_media_avfoundation`
* `ocean_media_...`

This hierarchy or nesting can go down several levels, `ocean_NAME_SUBNAME0_SUBNAME1_...`, depending
on the need of specialization.

Libraries containing unit tests use the fixed prefix `ocean_test_*`. The remainder of that name then
follows the same convention described above. For example:

* `ocean_test_base` - containing tests for `ocean_base`
* `ocean_test_cv` - containing tests for `ocean_cv`
* `ocean_test_cv_detector_qrcodes` - containing tests for `ocean_cv_detector_qrcodes`
* `ocean_test_...`

A more detailed description of the Ocean libraries will be provided in the next section.

### Applications (`impl/applications`)

Ocean comes with a large number of demo applications showing how to use Ocean and giving good start
points to investigate the code. All demo applications have the same prefix
`application_ocean_demo_`. Additionally, applications may have a suffix identifying the platform
`_ios`, `_osx`, `_android`, `_quest`, .... For example:

* `application_ocean_demo_media_videopreview_ios` - an iOS app
* `application_ocean_demo_media_videopreview_osx` - same app but for macOS
* `application_ocean_demo_tracking_featuretracker_ios`
* `application_ocean_demo_...`

All test applications begin with `application_ocean_test_*`. for example:

* `application_ocean_test_base` - application that runs the tests in `ocean_test_base` for
  `ocean_base`
* `application_ocean_test_cv` - application that runs the tests in `ocean_test_cv` for `ocean_cv`
* `application_ocean_test_...`

In both cases above does the remainder of that target name then follow the same convention described
for the libraries above.

It's worth noting that *Ocean tests can be run two different ways*. Either use the type of test
application described above, or run the GTests inside the library directories. The reason to have
both is that the latter is required for the Meta CI systems, while the former allows us developers
to test and debug algorithms more easily using standard debuggers (such as Visual Studio, XCode,
etc.).

The GTests can be identified by the suffix `*_gtest` in their target names. For example:

* `ocean_test_testbase_gtest`
* `ocean_test_testcv_gtest`
* `ocean_test_testdevices_gtest`
* `ocean_test_testgeometry_gtest`
* `ocean_test_...`

## The Ocean Libraries

The following provides a coarse overview of features and components in Ocean libraries. For detailed
information either check the [reference]
(https://facebookresearch.github.io/ocean/doxygen/index.html) or directly in the source code
(links below):

* [`ocean/base`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/base): Commonly used
  functionality, for example:
  * **Multithreading:** thread pools, workers, locks
  * **Data structures:** smart objects, hash maps, specialized vectors/buffers, **frames**,...
  * **Utilities:** strings, command-line parsers, ...
  * **Timing and scheduling:** high-performance timers and time stamps

* [`ocean/cv/*`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/cv): a collection of
  libraries for Computer Vision algorithms, for example:
  * **Image processing:** frame conversion, frame filtering, image interpolation, image
      enhancements, histogram calculation, frequency analysis, image quality assessment
  * **Feature detection:** corner detection, line detection , object detection, barcode detection,
      QR code detection
  * **Segmentation:** contour analysis, contour tracking, mask creation, seed segmentation
  * **Synthesis:** inpainting, layer management , mapping and optimization
  * **Advanced functionality:** Panorama stitching Poisson blending White and black point detection

* [`ocean/devices/*`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/devices): a
  collection of libraries with abstractions for many kinds of devices (or data sources), for
  example:
  * **Sensor support:** acceleration sensors, gyro sensors, magnetic trackers, GPS trackers
  * **Tracker functionality:** orientation trackers, position trackers, scene trackers, object
      trackers
  * **Platform support:** Android, ARCore, ARKit, iOS
  * **Map building and SLAM:** map building devices on-device map creators SLAM devices SLAM
      trackers
  * **Pattern tracking:** using images to determine the camera pose, see the [demo of the feature
      tracker](demoapps/crossplatformapps/feature_tracker.md)

* [`ocean/geometry`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/geometry): contains algorithms
  for Computer Vision, Photogrammetry, and geometry processing, for example:
  * **Transformation and optimization:** includes absolute transformation, various non-linear
      optimization techniques (for camera, homography, line, object point, orientation, plane,
      pose, transformation), and universal optimizations (dense and sparse).
  * **Calibration and pose estimation:** files for camera calibration, perspective pose, P3P, P4P,
      and PnP which are related to estimating the camera pose from points.
  * **Geometry and algorithms:** basic geometry, multiple view geometry, stereoscopic geometry, and
      utilities for general geometric computations.
  * **Error and estimation:** error handling, estimators, and normalization processes.
  * **Computational geometry:** Delaunay triangulation, homography, quadric surfaces, and octree
      structures.
  * **Robust estimation:** RANSAC algorithm for robust outlier detection.
  * **Spatial and structural analysis:** Spatial distribution, grid structures, and vanishing
      projection analysis.
  * **Advanced linkage and distribution:** J-linkage for clustering and Jacobian matrices for
      transformations.

* [`ocean/interaction/*`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/interaction): contains code for
  interactive, JavaScript-based functionalities, for example:
  * **Interaction components:** basic interaction management and user interface components
  * **Javascript integration:** extensive JavaScript integration for device management, media
      handling, and rendering . Also includes JavaScript representations of mathematical constructs
      like vectors, matrices, quaternions, and geometric shapes.
  * **Scene and script management:** handling of scene descriptions and scripting, facilitating
      complex scene setups and interactions in a JavaScript context.

* [`ocean/io/*`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/io): a comprehensive
  set of input/output functionalities, for example:
  * **Basic I/O operations:** core I/O functionalities including file handling, directory
      management, and base I/O operations.
  * **Data encoding and compression:** handling of data encoding (Base64) and compression
      techniques.
  * **Configuration management:** management of various configurations through files like JSON, XML
      and other data formats for flexible configuration options (camera calibration).
  * **Utilities:** specific functionalities like bitstream handling and digital signatures.
  * **Image and Maps I/O:** dedicated support for image and maps handling, including basic image
      operations, base maps, and map rendering.

* [`ocean/math`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/math): a
  comprehensive set of mathematical and geometric functionalities, for example:
  * **Basic mathematical operations and utilities:** general mathematical operations, numerical
      operations, and random number generation.
  * **Geometric shapes and transformations:** handling of various geometric shapes in 2D and/or 3D
      like boxes, spheres, triangles, and more complex shapes like cones, cylinders, and frustums.
      Transformations include matrices, quaternions, and rotations.
  * **Camera models and color representations:** different camera models and camera abstractions for
      external camera models and color representations.
  * **Advanced mathematical concepts:** clustering algorithms, Fourier and cosine transformations,
      and interpolation techniques.
  * **Utilities and specific algorithms:** utilities for handling subsets, variance calculations,
      rate calculations, and specific utilities for the external libraries, e.g., Sophus and Toon.

* [`ocean/media/*`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/media): contains
  a comprehensive set of media handling functionalities, for example:
  * **Core media handling:** basic media operations including audio, video, and image sequences.
  * **Recording and playback:** various recording functionalities and specialized recorders for
      different media types like audio, video, and images.
  * **Platform-specific implementations:** for example Android and AVFoundation specific media
      handling, Windows Imaging Component (WIC).
  * **Image handling:** Extensive image handling capabilities including different image formats
      (JPEG/JPG, PNG, GIF, TIFF, and more) and image operations.
  * **Utilities and plugins:** general utilities and plugins for extending media functionalities,
      e.g., to add support for other image formats.
  * **Specialized media types:** Handling of specialized media types and formats, such as USB
      media.

* [`ocean/network`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/network): contains a set of
  networking functionalities for a software system, for example:
  * **Basic network components:** core networking operations including general network management,
      data handling, and network resources.
  * **Client and server models:** various types of clients and servers including TCP, UDP, HTTP, and
      HTTPS. Also includes specialized clients and servers for streaming.
  * **Connection Types:** support for both connection-oriented and connectionless networking, along
      with packaged versions for easier management.
  * **Networking Utilities:** additional utilities such as address management, port handling, name
      resolver, and socket management.
  * **Queuing and Buffering:** Mechanisms for managing data flow and messages through buffer queues
      and message queues.

* [`ocean/platform/*`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/platform): contains a set of
  platform-specific functionalities and utilities, for example:
  * **Core platform components:** basic platform operations including system management, resource
      management, and utilities.
  * **Input Devices:** handling of input devices like keyboards and mice.
  * **Platform-specific implementations:** Android-, macOS-, iOS-, Windows-, and Quest-specific
      implementations.
  * **Resource and system management:** Management of system resources and configurations, including
      unique identifiers and platform-specific resource managers, e.g., for app assets on Android
      and iOS.
  * **Graphics and rendering:** handling of graphics contexts and rendering operations, particularly
      for OpenGL.
  * **Extended platform functionalities:** Extended functionalities for specific platforms, such as
      VR and AR capabilities for Meta Quest.
  * **JNI and native interfaces:** Java Native Interface and other native interfaces for integrating
      with platform-specific features.

* [`ocean/rendering/`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/rendering): provides a robust
  framework for handling complex rendering operations and is capable of supporting a wide range of
  graphics and rendering scenarios across different platforms. Features include:
  * **Core rendering components:** basic rendering operations including rendering engines,
      framebuffers, and textures.
  * **Scene and object management:** management of scenes, objects, and nodes for complex graphics
      scenes.
  * **Geometric shapes and attributes:** handling of various geometric shapes like boxes, spheres,
      and more complex shapes like cones and cylinders, and meshes. Attributes and attribute sets
      for defining properties of these shapes.
  * **Light Sources and illumination:** different types of light sources and global illumination
      techniques for realistic lighting effects.
  * **Platform-specific implementations:** GLES and platform-specific implementations.
  * **Advanced rendering techniques:** support for advanced rendering techniques like stereo views,
      textures, and global illumination components.

* [`ocean/scenedescription/*`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/scenedescription): contains a set
  of scene description functionalities, for example:
  * **Core scene components:** basic scene management operations including scene handling, node
      management, and field management.
  * **Scene description and management:** advanced scene description and management functionalities
      and wrappers for libraries like Assimp.
  * **Platform-specific implementations:** JNI and other platform-specific implementations.

* [`ocean_system`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/system): contains
  system-level functionalities and USB device management, for example:
  * **Core system components:** basic system operations including memory management, operating
      system interactions, performance tracking, and process management.
  * **USB device management:** various USB-related functionalities including USB context handling,
      device management, and other USB utilities.
  * **Platform-specific usb implementations:** Android-specific USB management.

* [`ocean_tracking`](https://github.com/facebookresearch/ocean/tree/main/impl/ocean/tracking): various tracking
  algorithms, for example:
  * **Core tracking components:** basic tracking operations including general tracking, visual
      tracking, and utilities.
  * **Image alignment:** various functionalities for image alignment and homography calculations.
  * **Plane finding and pose estimation:** plane finding and pose estimation techniques for accurate
      tracking in 3D environments.
  * **Map building and texturing:** map building and texturing operations for creating and managing
      detailed environmental maps.
  * **Advanced tracking techniques:** advanced tracking techniques including SLAM tracking, pattern
      tracking, and point tracking.
  * **Platform-specific implementations:** implementations for fiducial markers code
      (currently requires specialized hardware).
