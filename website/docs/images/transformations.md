---
title: Transformations
sidebar_position: 8
---

Ocean provides a suite of highly optimized image transformation functions designed to meet various needs in image processing and computer vision.
Below, we introduce some of the commonly used functionalities available in our [`ocean/cv/...`](https://github.com/facebookresearch/ocean/tree/v1.0.0/impl/ocean/cv) code folder.

## Basic Transformations

    1. Transposing an image:
        - [`CV::FrameTransposer::transpose()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameTransposer.h#L203)
        - Allows transposing images with arbitrary pixel formats and data types. This low-level function operates on memory pointers and uses template parameters for the element type and the number of frame channels.
        - Helper functions for Frame objects are also available [here](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameTransposer.h#L179).
    2. Rotating an image with 90 degree steps:
        - [`CV::FrameTransposer::rotate()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameTransposer.h#L251)
        - Enables rotation of images in 90-degree steps (e.g., +/- 90, 180, 270) for any pixel format and data type.
        - Additional helper functions for Frame objects can be found [here](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameTransposer.h#L86).
    3. Rotating an image with arbitrary angle:
        - [`CV::FrameInterpolatorBilinear::rotate8BitPerChannel()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameInterpolatorBilinear.h#L425)
        - Provides rotation based on bi-linear interpolation for arbitrary angles, allowing the definition of a pivot-point for rotation.
        - A Frame-based function is also available [here](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameInterpolatorBilinear.h#L278).


## Advanced Transformations

    1. Affine Transformation:
        - [`CV::FrameInterpolatorBilinear::affine8BitPerChannel()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameInterpolatorBilinear.h#L455)
        - Highly optimized function for affine transformations, which can include translation, rotation, scale, aspect ratio, and shear. Utilizes SIMD instructions based on the platform (NEON on ARM architectures and SSE on x86 architectures).
        - Helper functions for bi-linear interpolation and runtime interpolation algorithm selection are available
        [here](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameInterpolatorBilinear.h#L263) and [here](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameInterpolator.h#L141).
    2. Image Warping with Homography:
        - [`CV::FrameInterpolatorBilinear::homography()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameInterpolatorBilinear.h#L109)
        - Supports image warping based on a perspective transformation using homography, which includes perspective distortion in addition to affine transformations.
    3. Lookup Table-Based Interpolation:
        - [`CV::FrameInterpolatorBilinear::lookup()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameInterpolatorBilinear.h#L221)
        - Allows image warping without geometric constraints using a lookup table that defines a source pixel for each target pixel. Lookup tables can be sparse or dense.
    4. Resampling camera images:
        - [`CV::FrameInterpolatorBilinear::resampleCameraImage()`](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/FrameInterpolatorBilinear.h#L295)
        - This interpolation function is designed to transform an image captured with a specific type of camera (e.g., a fisheye camera) into an image as if captured with a different type of camera (e.g., a pinhole camera). It utilizes an image and corresponding camera profile/model as input, along with a specified output camera model. The function resamples the provided image to create a new output image that may serve various purposes such as rectifying a fisheye image, removing camera distortion, or generating a new image with a different field of view.
        - Internally, this function leverages the lookup-based image interpolation method to achieve the transformation, ensuring high fidelity and accuracy in the resampling process.
