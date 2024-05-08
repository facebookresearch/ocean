/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_GEOMETRY_GEOMETRY_H
#define META_OCEAN_GEOMETRY_GEOMETRY_H

#include "ocean/base/Base.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include <set>
#include <vector>

namespace Ocean
{

namespace Geometry
{

/**
 * @defgroup geometry Ocean Geometry Library
 * @{
 * The Ocean Geometry Library provides geometry functionalities e.g., projective algorithms, optimization approaches or support for advanced transformations.
 * The library is platform independent.
 *
 * In general, the Geometry library is an direct extension to the Math library offering a wide variety of mathematic functions mostly related with geometric tasks.<br>
 * The entire framework is using a right-handed coordinate system (as long as not explicitly documented differently).<br>
 * Please be aware that often coordinate systems are defined in different ways depending on the area of expertise or traditional decisions.<br>
 * In Computer Graphics, most coordinate systems are defined/interpreted in a way that the observer/camera is looking into the negative z-space.<br>
 * In such a case, the x-axis points to the right of the camera, and the y-axis is identical to the up vector of the camera (as long as the camera is located in the origin of the coordinate system).<br>
 * In the following, such a coordinate system is depicted:
 * <pre>
 *     ^
 *   Y |
 *     |
 *     |
 *     O --------->
 *    /        X
 *   / Z
 *  v
 * </pre>
 * Therefore, each 3D object point lying in front of the camera has a negative z-value (as long as the camera is located in the origin of the coordinate system).<br>
 * On the contrary, each object point lying behind the camera then has a positive z-value.<br>
 * By default, the Ocean framework uses this kind of representation/interpretation for !any! coordinate system and for any algorithm as long as not explicitly stated/documented differently.
 *
 * In Computer Vision, often a slightly different representation/interpretation is used.<br>
 * Computer Vision algorithms or/and equations are mostly defined in relation to a coordinate system in which the observer/camera is looking into the positive z-space.<br>
 * While the x-axis is still pointing to the right of the camera, the y-axis is equivalent to the down vector of the camera.<br>
 * In the following, such a coordinate system is depicted:
 * <pre>
 *      ^
 *   Z /
 *    /
 *   O --------->
 *   |        X
 *   |
 *   | Y
 *   v
 * </pre>
 * In such a coordinate system, 3D object points lying in front of the camera have a positive z-value (as long as the camera is located in the origin of the coordinate system).<br>
 * The reason (at least one of several other) for this coordinate system is that an image/frame quite often is defined to have the origin in the upper left corner of the image/frame (while the x-axis is pointing horizontally to the right and the y-axis is pointing vertically to the bottom of the image/frame).
 *
 * Obviously, both coordinate systems can simply be transformed from one to the other and vice versa (by simply rotating around the x-axis).<br>
 * However, it is crucial to understand that mixing up both coordinate systems can result in totally different results (sometimes not directly noticeable - depending on the situation etc.).<br>
 * As the Ocean framework offers Computer Graphics as well as Computer Vision functionalities it was important to define one coordinate system as default coordinate system.<br>
 * Without specific reasons, the Computer Graphics coordinate system (camera is looking into the negative z-space) is the default coordinate system while the Computer Vision coordinate system (camera is looking into the positive z-space) is the non-default case.<br>
 * Therefore, the Computer Vision coordinate system is called the 'flipped' coordinate system as (compared to the default coordinate system) the Computer Vision coordinate system is flipped around the x-axis by 180 degrees.
 *
 * Whenever a coordinate system is defined, e.g., for a camera pose the name of this parameter contains the information in which coordinate system the transformation is defined.<br>
 * The parameter name for a default camera pose (defined in the default coordinate system) can be e.g., "cameraPose", 'extrinsicMatrix', 'pose', 'transformation', and so on.<br>
 * While the camera pose defined in the flipped coordinate system (the Computer Vision coordinate system) can be e.g., 'cameraPoseFlipped', 'flippedPose', 'flippedTransformation', 'cameraPoseF', 'transformationF', and so on.<br>
 * Either the prefix or suffix 'flipped/Flipped' or 'f/F' is used to identify a transformation defined in the flipped coordinate system.<br>
 * Similar to this naming convention every function expecting (or returning) a transformation defined in the flipped coordinate system will contain the suffix 'F' like e.g., determineCameraPoseF(), projectPointF().<br>
 * As long as this naming convention is preserved within the entire Framework no issues regarding mixed coordinate system can happen and a lot of time can be saved for debugging.<br>
 * The framework has a lot of helper functions allowing to transform coordinate systems from one interpretation to the other.<br>
 * Further, for reasons of convenience often a function is implemented twice (one function holds the actual implementation e.g., for the Computer Vision coordinate system, while the other function is implemented for the Computer Graphic coordinate system and simply transforms input and output).<br>
 * For better understanding take a look on NonLinearOptimizationPose::optimizePose() and NonLinearOptimizationPose::optimizePoseIF().
 *
 * Another decision has been made that a camera pose (the coordinate system respectively), in general, can be located at any position and can have any orientation compared to the world coordinate system.<br>
 * That means that camera poses must not necessarily be located in the origin of the world coordinate system which offers a lot of flexibilities.<br>
 * In this framework, a camera pose (the extrinsic camera matrix) transforms 3D object points, defined in the coordinate system of the camera, into 3D object points defined in the world coordinate system.<br>
 * In Computer Graphics, the well known Model-View-Matrix is often defined in the opposite way (the matrix is simply inverted) - here 3D object points defined in the world coordinate system would be transformed into the camera coordinate system.<br>
 * However, as a wide variety of functions in this framework also expect the inverted camera pose as input, another naming convention exists adding an 'inverted/Inverted' or 'i/I' to any parameter name of function name if the inverted matrix/transformation is expected.<br>
 * For better understanding take a look on PinholeCamera::projectToImage() and PinholeCamera::projectToImageIF().
 *
 * As coordinate systems (and their transformations between each) are very often the reason for bugs, errors and time consuming debugging sessions it is highly recommended to follow the above described rules and to document which kind of transformation is expected in every function.<br>
 * In this framework, the documentation often provides equations showing which kind of transformations are expected or returned e.g., like:
 * <pre>
 * Pw = wTc * Pc,     or      wP = wTc * cP
 * </pre>
 * For a transformation wTc (e.g., a 3x3 or 4x4 square matrix) transforming a point 'Pc/cP' defined in the coordinate system of the camera (c) into the coordinate system of the world (w) - as long as points are multiplied on the right side of the transformation.<br>
 * Due to the notation of the subscripts 'w' and 'c' it's obvious how the transformation is to be interpreted and in which coordinate system a point must be defined so that the transformation can be applied.
 * @}
 */

/**
 * @namespace Ocean::Geometry Namespace of the Geometry library.<p>
 * The Namespace Ocean::Geometry is used in the entire Ocean Geometry Library.
 */

/**
 * Definition of a 2D image point.
 * @ingroup geometry
 */
typedef Vector2 ImagePoint;

/**
 * Definition of a 3D object point.
 * @ingroup geometry
 */
typedef Vector3 ObjectPoint;

/**
 * Definition of a vector holding 2D image points.
 * @ingroup geometry
 */
typedef std::vector<ImagePoint> ImagePoints;

/**
 * Definition of a vector holding 3D object points.
 * @ingroup geometry
 */
typedef std::vector<ObjectPoint> ObjectPoints;

/**
 * Definition of a vector holding object points, so we have groups of object points.
 * @ingroup geometry
 */
typedef std::vector<ObjectPoints> ObjectPointGroups;

/**
 * Definition of a vector holding image points, so we have groups of image points.
 * @ingroup geometry
 */
typedef std::vector<ImagePoints> ImagePointGroups;


// Defines OCEAN_GEOMETRY_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_GEOMETRY_EXPORT
		#define OCEAN_GEOMETRY_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_GEOMETRY_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_GEOMETRY_EXPORT
#endif

}

}

#endif // META_OCEAN_GEOMETRY_GEOMETRY_H
