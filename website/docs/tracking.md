---
title: Tracking
sidebar_position: 6
---

import React from 'react';
import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

Ocean's Computer Vision framework offers a comprehensive suite of tools for tracking and motion estimation, designed to cater to a variety of applications and performance needs.
Below, we detail the two primary tracking methods available: **Image Patch-based Point Tracking** and **Feature Descriptor-based Tracking**.

### Image Patch-based Point Tracking

Ocean's framework excels in patch-based point tracking, which is ideal when the visual content between two images does not change drastically. This method is particularly effective in scenarios with moderate camera motion or in movie sequences.
Key Features:

    - **Optimized Algorithms**: Utilizes metrics such as [*absolute difference*](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/SumAbsoluteDifferences.h#L30), [*sum square difference*](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/SumSquareDifferences.h#L28), and [*zero-mean sum square differences*](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/ZeroMeanSumSquareDifferences.h#L26) between two image patches.
    - **Adjustable Patch Size**: Allows users to balance execution performance and tracking precision.
    - **Variants of Patch-based Tracking**:
        - [Pixel Accurate Patch Tracking](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/Motion.h#L129): Tracks image patches from one frame to the next with pixel-precise accuracy, offering high speed but rough location details.
        - [Sub-pixel Accurate Patch Tracking](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/advanced/AdvancedMotion.h#L406): Achieves sub-pixel accuracy to pinpoint precise locations of image patches across frames.
        - [Pyramid-based Tracking](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/advanced/AdvancedMotion.h#L152): Enhances speed and covers larger baselines by tracking points on coarser image pyramid levels and refining on finer levels.
        - [Unidirectional Tracking](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/advanced/AdvancedMotion.h#L152): Tracks points from the first to the second image, suitable for applications where post-processing for outlier handling is planned.
        - [Bi-directional Tracking](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/advanced/AdvancedMotion.h#L270): Enhances reliability by tracking points from the first image to the second and back, rejecting points that do not return close to their origins.

Explore these capabilities through the [**Point Tracker**](demoapps/crossplatformapps/point_tracker.md) demo app, which utilizes patch-based point tracking.


### Feature Descriptor-based Tracking
For scenarios requiring robustness against more significant variations in camera motion or perspective, Ocean provides powerful descriptor-based tracking using [ORB](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/detector/ORBFeature.h#L39) and [FREAK](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/detector/FREAKDescriptor.h#L83) feature descriptors.

- **Advantages**

    - **Flexibility**: Suitable for matching features in images that are not sequentially captured or that undergo different camera rotations.
    - **Comprehensive Matching**: Although typically slower than patch-based methods, descriptor-based tracking offers enhanced accuracy in complex scenarios.

Explore these capabilities through the [**Feature Tracker**](demoapps/crossplatformapps/feature_tracker.md) demo app, which utilizes patch-based point tracking.


### Feature Detection

In addition to its robust tracking capabilities, Ocean's Computer Vision framework also includes feature detection algorithms that are essential for identifying unique points in images, which can be crucial for tasks such as motion analysis and camera pose optimization.

    - **Harris Corner Detector:**
    The [Harris Corner Detector](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/detector/HarrisCornerDetector.h#L38) is a highly optimized implementation provided by Ocean.
    This detector is renowned for its ability to identify corners within an image, which are points where the gradient changes direction significantly.
    These corners are considered stable and unique features in the image, making them ideal for precise tracking and camera pose estimation tasks.

    - **FAST Feature Detector:**
Ocean also offers an implementation of the [FAST Feature Detector](https://github.com/facebookresearch/ocean/blob/v1.0.0/impl/ocean/cv/detector/FASTFeatureDetector.h#L32) (Features from Accelerated Segment Test).
FAST is designed for speed and efficiency, making it suitable for real-time applications where quick feature detection is crucial.
It works by examining a circle of pixels around a potential feature point and determining if there is a sufficient number of pixels that are significantly brighter or darker than the central pixel, which indicates a feature.
These feature detection capabilities complement the tracking methods provided by Ocean, allowing users to implement comprehensive and effective computer vision solutions.


---

To implement and explore these tracking methods, refer to the code base folders: [`ocean/cv`](https://github.com/facebookresearch/ocean/tree/v1.0.0/impl/ocean/cv), [`ocean/cv/advanced`](https://github.com/facebookresearch/ocean/tree/v1.0.0/impl/ocean/cv/advanced), [`ocean/cv/detector`](https://github.com/facebookresearch/ocean/tree/v1.0.0/impl/ocean/cv/detector) and [`ocean/tracking`](https://github.com/facebookresearch/ocean/tree/v1.0.0/impl/ocean/tracking).
