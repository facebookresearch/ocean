/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include "ocean/cv/detector/qrcodes/FinderPatternDetector.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/**
 * This class implements common functionality of QR code detectors but is not a stand-alone detector
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT QRCodeDetector
{
	protected:

		/**
		 * Estimates the range of version numbers based on pixel distances of pairs of finder patterns
		 * Estimates the version numbers along the line segments that connect the top-left and the top-right finder patterns (horizontal) as well as the top-left and bottom-left finder patterns (vertical).
		 * @param anyCamera The camera profile that produced the input image, must be valid
		 * @param finderPatterns The pointer to the triplet of finder patterns; must be valid and have 3 elements, the order must be top-left, bottom-left, top-right finder pattern
		 * @param maxAllowedVersionDifference The maximum difference between the vertical and horizontal estimate of the version numbers, range: [0, infinity)
		 * @param versionLow The returning minimum of all estimated version numbers
		 * @param versionHigh The returning maximum of all estimated version numbers
		 * @return True if the minimum and maximum version numbers do not exceed the maximum allowed difference, otherwise false
		 */
		static bool computeProvisionalVersionRange(const AnyCamera& anyCamera, const FinderPattern* finderPatterns, const unsigned int maxAllowedVersionDifference, unsigned int& versionLow, unsigned int& versionHigh);

		/**
		 * Estimates the version numbers based on pixel distances of a pair of finder patterns
		 * @param centerA The center of the first finder pattern; this must be the undistorted location that has been projected into a plane
		 * @param centerB The center of the second finder pattern; this must be the undistorted location that has been projected into the same plane as the first center
		 * @param moduleSizeA The module size of the first finder pattern, range: (0, infinity)
		 * @param moduleSizeB The module size of the second finder pattern, range: (0, infinity)
		 * @param maxAllowedVersionDifference The maximum difference between the vertical and horizontal estimate of the version numbers, range: [0, infinity)
		 * @param versionLow The returning minimum of two estimated version numbers
		 * @param versionHigh The returning maximum of two estimated version numbers
		 * @return True if the minimum and maximum version numbers do not exceed the maximum allowed difference, otherwise false
		 */
		static bool computeProvisionalVersion(const Vector3& centerA, const Vector3& centerB, const Scalar moduleSizeA, const Scalar moduleSizeB, const unsigned int maxAllowedVersionDifference, unsigned int& versionLow, unsigned int& versionHigh);

		/**
		 * Convert the module size in pixels of a finder pattern to the size in a given plane that contains undistorted points
		 * @param anyCamera The camera profile that produced the input image, must be valid
		 * @param plane The plane that contains undistorted points, must be valid
		 * @param imagePoint The location of the center of a finder pattern in the distorted image
		 * @param planePoint The location of the center of a finder pattern in the plane with undistorted points
		 * @param moduleSizeInImage The module size of the finder pattern that was measured in the distorted image, range: (0, infinity)
		 * @param moduleSizeInPlane The resulting module size of the finder pattern in the plane with undistorted points
		 * @return True on success, otherwise false
		 */
		static bool computeModuleSizeInPlane(const AnyCamera& anyCamera, const Plane3& plane, const Vector2& imagePoint, const Vector3& planePoint, const Scalar moduleSizeInImage, Scalar& moduleSizeInPlane);

		/**
		 * Determines the version of the QR code symbols from the size and distance of its three finder patterns
		 * The version, `versionX`, is determined in the horizontal direction between centers of the top-left and the top-right finder patterns and their average module size. The version in the vertical direction is determined similary between the top-left and the bottom-left finder patterns, `versionY`.
		 * QR code versions 1-7 do not store version information in dedicated bit fields, only versions 7-40 do. Because of that both versions need to be identical for versions 1-7 in order to be considered as valid, but it is alright to be a little more lenient for the second category (allowing `versionX` and `versionY` to differ by up to 1 version).
		 * @note Make sure that the triplet is valid, otherwise the result will be undefined.
		 * @param topLeft The finder pattern in the top-left corner of the QR code, must be valid
		 * @param bottomLeft The finder pattern in the bottom-left corner of the QR code, must be valid
		 * @param topRight The finder pattern in the top-right corner of the QR code, must be valid
		 * @param versionX The resulting version determined from the top-left and top-right finder patterns
		 * @param versionY The resulting version determined from the top-left and bottom-left finder patterns
		 * @return True if both versions are below 7 and have the same value or if both versions are in the range [7, 40] and do not differ by more than 1, otherwise false
		 */
		static bool computeProvisionalVersionRange(const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight, unsigned int& versionX, unsigned int& versionY);

		/**
		 * Extracts all modules of a QR code from an image
		 * @param anyCamera The camera profile that produced the input image, must be valid
		 * @param yFrame The frame in which QR codes will be detected, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with Y8
		 * @param width The width of the input frame, range: [29, infinity)
		 * @param height The height of the input frame, range: [29, infinity)
		 * @param paddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param version The version number that the QR code candidate, range: [1, 40]
		 * @param code_T_camera The pose of the QR code candidate
		 * @param isNormalReflectance Indicates whether alignment patterns with normal or inverted reflectance are searched
		 * @param grayThreshold The gray value that has been determined as the separation between foreground and background modules (cf. `FinderPattern::grayThreshold()`), range: [0, 255]
		 * @param modules The resulting list of all extracted modules, will have `QRCode::modulesPerSide(version) * QRCode::modulesPerSide(version)` elements
		 * @param scale Optional scaling factor for the coordinates in the object space, range: (0, infinity)
		 * @return True if the extraction of all modules was successful, otherwise false
		 */
		static bool extractModulesFromImage(const AnyCamera& anyCamera, const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int version, const HomogenousMatrix4& code_T_camera, const bool isNormalReflectance, const unsigned int grayThreshold, std::vector<uint8_t>& modules, const Scalar scale = Scalar(1));

		/**
		 * Computes poses of a QR code given a specific version number
		 * Given the locations of the three finder patterns up to 4 poses are possible (P3P). To reduce the number of possible poses, this function then tries to find an additional correspondence (an alignment pattern), if it exists. If no additional correspondence was found, all possible poses will be returned.
		 * @param anyCamera The camera profile that produced the input image, must be valid
		 * @param yFrame The frame in which QR codes will be detected, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with Y8
		 * @param width The width of the input frame, range: [29, infinity)
		 * @param height The height of the input frame, range: [29, infinity)
		 * @param paddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param finderPatterns The pointer to the triplet of finder patterns; must be valid and have 3 elements, the order must be top-left, bottom-left, top-right finder pattern
		 * @param version The version number that the QR code is estimated to have, range: [1, 40]
		 * @param code_T_cameras The returning possible poses that have been identified, possible size: [0, 4] (optimally it is only 1)
		 * @param scale Optional scaling factor for the coordinates in the object space, range: (0, infinity)
		 * @return True if one or more poses have been found, otherwise false
		 */
		static bool computePoses(const AnyCamera& anyCamera, const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const FinderPattern* finderPatterns, const unsigned int version, HomogenousMatrices4& code_T_cameras, const Scalar scale = Scalar(1));
};

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
