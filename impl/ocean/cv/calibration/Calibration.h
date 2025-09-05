/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_CALIBRATION_H
#define META_OCEAN_CV_CALIBRATION_CALIBRATION_H

#include "ocean/cv/CV.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/**
 * @ingroup cv
 * @defgroup cvcalibration Ocean CV Calibration Library
 * @{
 * The Ocean CV Calibration Library provides a comprehensive framework for camera calibration using marker-based calibration boards.
 *
 * ## Overview
 *
 * Camera calibration is the process of determining the intrinsic and extrinsic parameters of a camera.
 * This library uses a pattern-based approach where a calibration board containing uniquely identifiable
 * markers is used to establish 2D-3D correspondences needed for calibration.
 *
 * ## Core Components
 *
 * ### 1. Marker System
 *
 * **Markers** are the fundamental building blocks of the calibration system. Each marker is a 5x5 grid
 * of points (25 points total) that can be either black dots on a white background (positive markers)
 * or white dots on a black background (negative markers). The marker system provides several key features:
 *
 * - **Unique Identification**: Each marker has a unique layout pattern that distinguishes it from others
 * - **Orientation Detection**: Markers can be detected and identified regardless of their orientation (0°, 90°, 180°, 270°)
 * - **Sign Variants**: Each marker can exist in positive (black on white) or negative (white on black) form
 * - **Layout Management**: The system manages a collection of unique marker layouts that are rotation-invariant
 *
 * The marker layout is defined as a 5x5 grid:
 * ```
 *  ----------------
 * |  0  1  2  3  4 |
 * |  5  6  7  8  9 |
 * | 10 11 12 13 14 |
 * | 15 16 17 18 19 |
 * | 20 21 22 23 24 |
 *  ----------------
 * ```
 *
 * The border points (used for initial detection) are arranged as:
 * ```
 *  ----------------
 * |  0  1  2  3  4 |
 * | 15           5 |
 * | 14           6 |
 * | 13           7 |
 * | 12  11 10 9  8 |
 *  ----------------
 * ```
 *
 * ### 2. Calibration Boards
 *
 * **Calibration Boards** are regular grids of markers arranged in a rectangular pattern. Each board:
 *
 * - Contains multiple markers arranged in rows and columns
 * - Has a unique board ID that determines the specific marker arrangement
 * - Ensures that each marker's 4-neighborhood (adjacent markers) is unique within the board
 * - Supports different physical sizes through the MetricCalibrationBoard class
 *
 * ### 3. Point Detection Pipeline
 *
 * The marker point detection follows a multi-stage process:
 *
 * #### Stage 1: Point Detection
 * The **PointDetector** uses pattern matching to identify potential marker points in the image:
 *
 * - **Pattern Matching**: Uses circular or rectangular patterns of varying radii to detect blob-like structures
 * - **Intensity Analysis**: Distinguishes between dark points (black dots) and bright points (white dots)
 * - **Strength Calculation**: Computes detection strength based on intensity differences between center and surrounding pixels
 * - **Non-Maximum Suppression**: Eliminates duplicate detections in overlapping regions
 * - **Sign Detection**: Determines whether each point is positive (black) or negative (white)
 *
 * #### Stage 2: Marker Candidate Formation
 * The **MarkerCandidate** system groups detected points into potential markers:
 *
 * - **Line Detection**: Finds continuous lines of 5 points with the same sign
 * - **Rectangle Formation**: Extends lines to form closed rectangles of 16 border points
 * - **Validation**: Ensures the detected rectangle has the correct geometric properties
 * - **Point Classification**: Associates the 16 border points with the full 25-point marker layout
 *
 * #### Stage 3: Marker Identification
 * Once all 25 points of a marker candidate are known:
 *
 * - **Layout Matching**: Compares the detected point pattern against known marker layouts
 * - **Orientation Determination**: Identifies the marker's orientation (0°, 90°, 180°, 270°)
 * - **ID Assignment**: Assigns the unique marker ID based on the matched layout
 * - **Sign Confirmation**: Verifies the marker's sign (positive or negative)
 *
 * #### Stage 4: Neighborhood Analysis
 * The **CalibrationBoardDetector** establishes spatial relationships:
 *
 * - **Neighbor Detection**: Identifies adjacent markers based on geometric proximity
 * - **Orientation Consistency**: Ensures neighboring markers have consistent relative orientations
 * - **Board Matching**: Compares detected marker neighborhoods against known calibration boards
 * - **Coordinate Assignment**: Assigns board coordinates to identified markers
 *
 * ### 4. Camera Calibration Process
 *
 * The **CameraCalibrator** orchestrates the complete calibration workflow:
 *
 * #### Multi-Image Processing
 * - Processes multiple images of the calibration board from different viewpoints
 * - Accumulates 2D-3D correspondences from detected markers
 * - Handles various camera types (pinhole, fisheye, etc.)
 *
 * #### Initial Parameter Estimation
 * - Estimates initial camera field-of-view based on marker geometry
 * - Determines initial camera poses using detected markers
 * - Handles different camera models through the AnyCamera interface
 *
 * #### Optimization
 * - Performs non-linear optimization to refine camera parameters
 * - Uses robust estimation techniques to handle outliers
 * - Optimizes both intrinsic parameters (focal length, distortion) and extrinsic parameters (poses)
 *
 * The library is platform independent and provides comprehensive debugging and visualization capabilities through the CalibrationDebugElements class.
 * @}
 */

/**
 * @namespace Ocean::CV::Calibration Namespace of the CV Calibration library.<p>
 * The Namespace Ocean::CV::Calibration is used in the entire Ocean CV Calibration Library.
 *
 * This namespace contains all classes and functions related to marker-based camera calibration,
 * including marker detection, calibration board management, and camera parameter estimation.
 */

// Defines OCEAN_CV_CALIBRATION_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_CV_CALIBRATION_EXPORT
		#define OCEAN_CV_CALIBRATION_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_CV_CALIBRATION_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_CV_CALIBRATION_EXPORT
#endif

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_CALIBRATION_H
