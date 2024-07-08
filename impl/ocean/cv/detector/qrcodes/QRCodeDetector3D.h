/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/QRCodeDetector.h"

#if defined(OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
	#include "ocean/cv/detector/qrcodes/QRCodeDebugElements.h"
#endif

#include "ocean/base/Frame.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/**
 * This class implements a 6-DOF detector for QR codes.
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT QRCodeDetector3D : public QRCodeDetector
{
	public:

		/**
		 * Detects QR codes their 6-DOF poses in two synchronized 8-bit grayscale images
		 * @param sharedAnyCameras The cameras that produced the input images, must have 2 elements, all elements must be valid
		 * @param yFrames The frames in which QR codes will be detected, must be valid, must have 2 elements, origin must be in the upper left corner, and have a pixel format that is compatible with Y8, minimum size is 29 x 29 pixels
		 * @param world_T_device The transformation that maps points in the device coordinate system points to world points, must be valid
		 * @param device_T_cameras The transformation that converts points in the camera coordinate systems to device coordinates, `devicePoint = device_T_cameras[i] * cameraPoint`, must have the same number of elements as `yFrames`, all elements must be valid
		 * @param codes The resulting list of detected QR codes
		 * @param world_T_codes The resulting 6-DOF poses the detected QR codes, number of elements will be identical to `codes`
		 * @param codeSizes The resulting edge lengths of the detected QR codes in meters, number of elements will be identical to `codes`
		 * @param worker Optional worker instance for parallelization
		 * @param allow2DCodes If true, the detector will also report back QR codes which have been detected in only one camera (2D detections), however, their 6-DOF pose and size (edge length) will not be accurate; otherwise only 3D detection will be reported
		 * @return True if one or more QR code has been detected, otherwise false
		 */
		static bool detectQRCodes(const SharedAnyCameras& sharedAnyCameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, QRCodes& codes, HomogenousMatrices4& world_T_codes, Scalars& codeSizes, Worker* worker = nullptr, const bool allow2DCodes = false);

		/**
		 * Detects QR codes their 6-DOF poses in two synchronized 8-bit grayscale images
		 * @param sharedAnyCameras The cameras that produced the input images, must have 2 elements, all elements must be valid
		 * @param yFrames The frames in which QR codes will be detected, must be valid, must have 2 elements, origin must be in the upper left corner, and have a pixel format that is compatible with Y8, minimum size is 29 x 29 pixels
		 * @param world_T_device The transformation that maps points in the device coordinate system points to world points, must be valid
		 * @param device_T_cameras The transformation that converts points in the camera coordinate systems to device coordinates, `devicePoint = device_T_cameras[i] * cameraPoint`, must have the same number of elements as `yFrames`, all elements must be valid
		 * @param codes The resulting list of detected QR codes
		 * @param world_T_codes The resulting 6-DOF poses the detected QR codes, number of elements will be identical to `codes`
		 * @param codeSizes The resulting edge lengths of the detected QR codes in meters, number of elements will be identical to `codes`
		 * @param worker Optional worker instance for parallelization
		 * @param allow2DCodes If true, the detector will also report back QR codes which have been detected in only one camera (2D detections), however, their 6-DOF pose and size (edge length) will not be accurate; otherwise only 3D detection will be reported
		 * @return True if one or more QR code has been detected, otherwise false
		 */
		static bool detectQRCodesWithPyramids(const SharedAnyCameras& sharedAnyCameras, const Frames& yFrames, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, QRCodes& codes, HomogenousMatrices4& world_T_codes, Scalars& codeSizes, Worker* worker = nullptr, const bool allow2DCodes = false);

		/**
		 * Returns an invalid size for QR codes
		 * @return The invalid size value
		 */
		static inline Scalar getInvalidCodeSize();

		/**
		 * Returns an invalid 6-DOF pose for a QR codes
		 * @return The invalid size value
		 */
		static inline const HomogenousMatrix4& getInvalidWorld_T_code();

	protected:

		/**
		 * Triangulates the centers of corresponding observations of finder patterns from two different views
		 * @param sharedAnyCameraA The camera that produced the observation of the first finder pattern, must be valid
		 * @param sharedAnyCameraB The camera that produced the observation of the second finder pattern, must be valid
		 * @param world_T_device The transformation that maps points in the device coordinate system points to world points, must be valid
		 * @param device_T_cameraA The transformation that converts points in the coordinate systems of the first camera to device coordinates, must be valid
		 * @param device_T_cameraB The transformation that converts points in the coordinate systems of the first camera to device coordinates, must be valid
		 * @param finderPatternTripletA The first finder pattern of the corresponding pair, must be valid
		 * @param finderPatternTripletB The second finder pattern of the corresponding pair, must be valid
		 * @param worldPoints The resulting locations of the finder pattern centers in world coordinates, will have three elements
		 * @param averageFinderPatternCenterDistance The average distance between the centers of the top-left and top-right as well as the top-left and the bottom-left finder patterns
		 * @return True if the centers of the finder patterns have been triangulated successfully, otherwise false
		 */
		static bool triangulateFinderPatternsStereo(const SharedAnyCamera& sharedAnyCameraA, const SharedAnyCamera& sharedAnyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const FinderPatternTriplet& finderPatternTripletA, const FinderPatternTriplet& finderPatternTripletB, Vectors3& worldPoints, Scalar& averageFinderPatternCenterDistance);

		/**
		 * Computes the pose of a QR code and extracts it payload given a pair of corresponding finder patterns
		 * @param yFrameA The frame that contain the observation of the first finder pattern, must be valid
		 * @param yFrameB The frame that contain the observation of the second finder pattern, must be valid
		 * @param sharedAnyCameraA The camera that produced the observation of the first finder pattern, must be valid
		 * @param sharedAnyCameraB The camera that produced the observation of the second finder pattern, must be valid
		 * @param world_T_device The transformation that maps points in the device coordinate system points to world points, must be valid
		 * @param device_T_cameraA The transformation that converts points in the coordinate systems of the first camera to device coordinates, must be valid
		 * @param device_T_cameraB The transformation that converts points in the coordinate systems of the first camera to device coordinates, must be valid
		 * @param finderPatternTripletA The first finder pattern of the corresponding pair, must be valid
		 * @param finderPatternTripletB The second finder pattern of the corresponding pair, must be valid
		 * @param worldPoints The locations of the finder pattern centers in world coordinates, will have three elements
		 * @param averageFinderPatternCenterDistance The average distance between the centers of the top-left and top-right as well as the top-left and the bottom-left finder patterns
		 * @param code The resulting QR code that was extracted
		 * @param world_T_code The resulting pose of the code in world coordinates
		 * @param codeSize The resulting (display) size of the extracted code in the physical world (in meters)
		 * @param version Optional indicator of the version number that the code might have (e.g. if it has been observed before), range: [1, 40] or -1 to determine it automatically
		 * @return True if the extraction of the code and computation its pose was successful, otherwise false
		 * @sa triangulateFinderPatternsStereo()
		 */
		static bool computePoseAndExtractQRCodeStereo(const Frame& yFrameA, const Frame& yFrameB, const SharedAnyCamera& sharedAnyCameraA, const SharedAnyCamera& sharedAnyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const FinderPatternTriplet& finderPatternTripletA, const FinderPatternTriplet& finderPatternTripletB, const Vectors3& worldPoints, const Scalar& averageFinderPatternCenterDistance, CV::Detector::QRCodes::QRCode& code, HomogenousMatrix4& world_T_code, Scalar& codeSize, const unsigned int version = (unsigned int)(-1));

		/**
		 * Computes the 6DOF pose of a QR code from a pair of cameras
		 * @param sharedAnyCameraA The first camera that will be used to compute the pose of a code, must be valid
		 * @param sharedAnyCameraB The second camera that will be used to compute the pose of a code, must be valid
		 * @param yFrameA The first image frame that contains an image of the code, must be valid
		 * @param yFrameB The second image frame that contains an image of the code, must be valid
		 * @param world_T_device The transformation that maps points in the device coordinate system points to world points, must be valid
		 * @param device_T_cameraA The transformation that converts points in the coordinate systems of the first camera to device coordinates, must be valid
		 * @param device_T_cameraB The transformation that converts points in the coordinate systems of the second camera to device coordinates, must be valid
		 * @param finderPatternTripletA The triplets of finder patterns of the code in the first image, must be valid
		 * @param finderPatternTripletB The triplets of finder patterns of the code in the second image, must be valid
		 * @param worldPoints The world points from the triangulation of the two finder pattern triplets, must have three elements and be in front of the camera
		 * @param version The version number that the code is believed to have, range: [1, 40]
		 * @param codeSize The code size (in meters) that the code is believed to have, range: (0, infinity)
		 * @param world_T_code The resulting 6DOF pose of the code; in the code space the x- and y-axis range from [-codeSize/2, codeSize/2] and z = 0 (> 0 will be above the code, < 0 will be below the code)
		 * @return True if a pose was computed successfully, otherwise false
		 */
		static bool computePoseStereo(const SharedAnyCamera& sharedAnyCameraA, const SharedAnyCamera& sharedAnyCameraB, const Frame& yFrameA, const Frame& yFrameB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const FinderPatternTriplet& finderPatternTripletA, const FinderPatternTriplet& finderPatternTripletB, const Vector3* worldPoints, const unsigned int version, const Scalar codeSize, HomogenousMatrix4& world_T_code);

		/**
		 * Computes the square projection error given a 6DOF pose of a code
		 * @param anyCamera The camera into which the code will be projected, must be valid
		 * @param world_T_camera The 6DOF pose of the camera in world coordinates, must be valid
		 * @param world_T_code The 6DOF pose of the code that will be used to project the code, must be valid
		 * @param finderPatternTriplet The triplet of finder patterns of which the center points will be used to compute the projection error, must be valid
		 * @param version The version number of the code that is project, range: [1, 40]
		 * @param codeSize_2 The half value of the code (in meters) that is projected, range: (0, infinity)
		 * @return The square sum of the projection error
		 */
		static Scalar computeSquaredProjectionError(const SharedAnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_code, const FinderPatternTriplet& finderPatternTriplet, const unsigned int version, const Scalar codeSize_2);
};

Scalar QRCodeDetector3D::getInvalidCodeSize()
{
	return Scalar(-1.0);
}

inline const HomogenousMatrix4& QRCodeDetector3D::getInvalidWorld_T_code()
{
	const static HomogenousMatrix4 invalidWorld_T_code(false);

	return invalidWorld_T_code;
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
