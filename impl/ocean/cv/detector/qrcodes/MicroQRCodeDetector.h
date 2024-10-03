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
 * This class implements common functionality of Micro QR code detectors but is not a stand-alone detector
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT MicroQRCodeDetector
{
	protected:

		/**
		 * Locates the modules of one timing pattern in a Micro QR code
		 * @param yFrame The frame in which Micro QR code will be detected, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with Y8
		 * @param width The width of the input frame, range: [15, infinity)
		 * @param height The height of the input frame, range: [15, infinity)
		 * @param paddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param finderPattern The finder pattern; must be valid and have known corners
		 * @param topLeftCorner The index of the finder pattern corner considered to be the top-left one, range: [0, 3]
		 * @param timingAdjacentFinderCorner The index of the finder pattern corner adjacent to the timing pattern, must be top-right or bottom-left, range: [0, 3]
		 * @param moduleCenters The centers of the modules that were found in the timing pattern, will be empty if the timing pattern was not found
		 * @return True if the timing pattern was found, otherwise false
		 */
		static bool getTimingPatternModules(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const FinderPattern& finderPattern, const unsigned int topLeftCorner, const unsigned int timingAdjacentFinderCorner, Vectors2& moduleCenters);

		/**
		 * Computes potential versions and poses of a Micro QR code given its finder pattern
		 * @param anyCamera The camera profile that produced the input image, must be valid
		 * @param yFrame The frame in which Micro QR code will be detected, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with Y8
		 * @param width The width of the input frame, range: [15, infinity)
		 * @param height The height of the input frame, range: [15, infinity)
		 * @param paddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param finderPattern The finder pattern; must be valid and have known corners
		 * @param code_T_cameras The returning possible poses that have been identified, possible size: [0, 4] (optimally it is only 1)
		 * @param provisionalVersions The version numbers that the Micro QR code is estimated to have at each potential pose, range: [1, 4]
		 * @param scale Optional scaling factor for the coordinates in the object space, range: (0, infinity)
		 * @return True if one or more poses have been found, otherwise false
		 */
		static bool computePosesAndProvisionalVersions(const AnyCamera& anyCamera, const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const FinderPattern& finderPattern, HomogenousMatrices4& code_T_cameras, std::vector<unsigned int>& provisionalVersions, const Scalar scale = Scalar(1));

		/**
		 * Extracts all modules of a Micro QR code from an image
		 * @param anyCamera The camera profile that produced the input image, must be valid
		 * @param yFrame The frame in which Micro QR code will be detected, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with Y8
		 * @param width The width of the input frame, range: [15, infinity)
		 * @param height The height of the input frame, range: [15, infinity)
		 * @param paddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param version The version number that the Micro QR code candidate, range: [1, 4]
		 * @param code_T_camera The pose of the Micro QR code candidate
		 * @param isNormalReflectance Indicates whether alignment patterns with normal or inverted reflectance are searched
		 * @param grayThreshold The gray value that has been determined as the separation between foreground and background modules (cf. `FinderPattern::grayThreshold()`), range: [0, 255]
		 * @param modules The resulting list of all extracted modules, will have `MicroQRCode::modulesPerSide(version) * MicroQRCode::modulesPerSide(version)` elements
		 * @param scale Optional scaling factor for the coordinates in the object space, range: (0, infinity)
		 * @return True if the extraction of all modules was successful, otherwise false
		 */
		static bool extractModulesFromImage(const AnyCamera& anyCamera, const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const unsigned int version, const HomogenousMatrix4& code_T_camera, const bool isNormalReflectance, const unsigned int grayThreshold, std::vector<uint8_t>& modules, const Scalar scale = Scalar(1));

		/**
			* Extracts the version information from the modules of a Micro QR code
			* @param provisionalVersion The version number that was used to extract the modules, range: [1, 4]
			* @param modules The modules of the Micro QR code, should have `MicroQRCode::modulesPerSide(provisionalVersion) * MicroQRCode::modulesPerSide(provisionalVersion)` elements
			* @return The version number as it was encoded in the Micro QR code, range: [1, 4], or 0 if the version information could not be extracted
			*/
		static unsigned int extractVersionFromModules(const unsigned int provisionalVersion, const std::vector<uint8_t>& modules);

};

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
