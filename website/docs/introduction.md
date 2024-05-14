---
title: Introduction
sidebar_position: 1

description: Introduction to Ocean
image: ../static/img/ocean-social-card.jpg
---

The Ocean framework is a software package composed of several individual (mainly platform independent) libraries with focus on Computer Vision and Computer Graphics, e.g. to develop Augmented Reality applications.
Ocean comes with a wide variety of functions for optimization problems like 6-DOF camera pose optimization, 3D object point optimization, camera profile optimization or Bundle Adjustment.

## Overview

The entire framework consists of thousands of source files which are distributed over hundreds of targets.

- **Libraries:** The targets for all Ocean libraries begin with `ocean_` followed by the name of the library: `ocean_NAME`, for example:
  - `ocean_base`
  - `ocean_cv`
  - `ocean_geometry`
  - `ocean_media`
  - `ocean_...`

- **Sub-libraries:** Several main libraries come with sub-libraries covering specialized functionalities or platform specific implementations. Sub-libraries have an additional suffix: `ocean_NAME_SUBNAME`, for example:
  - `ocean_cv_advanced` (Computer Vision library for advanced problems)
  - `ocean_cv_detector` (Computer Vision library with feature detectors)
  - `ocean_cv_...`
  - `ocean_media_avfoundation` (Media library using AVFoundation)
  - `ocean_media_...`

- **Test-libraries:** The libraries implementing the tests start with `ocean_test_`, for example:
  - `ocean_test_base` (Containing all tests for the Base library)
  - `ocean_test_cv` (The tests for the main Computer Vision library)
  - `ocean_test_cv_detector` (Tests for the detectors)
  - `ocean_test_...`

- **Demo applications:** Ocean comes with a large number of demo applications showing how to use Ocean and giving good start points to investigate the code. All demo applications have the same prefix `application_ocean_demo_`. Additionally, applications may have a suffix identifying the platform `_ios`, `_osx`, `_...`, for example:
  - `application_ocean_demo_media_videopreview_ios` (Simple app for iOS)
  - `application_ocean_demo_media_videopreview_osx` (Simple app for OSX)
  - `application_ocean_demo_tracking_featuretracker_ios`
  - `application_ocean_demo_...`

- **Test applications:** All test applications begin with `application_ocean_test_`, for example:
- `application_ocean_test_base` (Test app for the Base library - mainly a wrapper for the corrsponding test library `ocean_test_base`)
- `application_ocean_test_cv` (Test app for the Computer Vision library)
- `application_ocean_test_...`
