/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAGDETECTOR_H
#define META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAGDETECTOR_H

#include "ocean/tracking/oculustags/OculusTags.h"
#include "ocean/tracking/oculustags/OculusTag.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Worker.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"

#include "ocean/cv/detector/LineDetectorULF.h"
#include "ocean/cv/detector/ShapeDetector.h"

#include "ocean/math/AnyCamera.h"

#include <unordered_map>

namespace Ocean
{

namespace Tracking
{

namespace OculusTags
{

/**
 * Deprecated.
 *
 * This class implements a detector for Oculus tags.
 * @ingroup trackingoculustags
 */
class OCEAN_TRACKING_OCULUSTAGS_EXPORT OculusTagDetector
{
	public:

		/// A data structure to map tag IDs to tag sizes
		typedef std::unordered_map<uint32_t, Scalar> TagSizeMap;

		/// Definition of boundary patterns (quadrilaterals) which are Oculus tag candidates
		typedef std::array<Vector2, 4> BoundaryPattern;

		/// A vector of boundary patterns
		typedef std::vector<BoundaryPattern> BoundaryPatterns;

	public:

		/**
		 * Detects Oculus tags in a single 8-bit grayscale image
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The image in which Oculus tags will be searched, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_camera The transformation that converts camera coordinates to device coordinates, `deviceCoord = device_T_camera * cameraCoord`, must be valid
		 * @param defaultTagSize The edge lengths of all detected tags that are not specified in `tagSizeMap` , range: (0, infinity)
		 * @param tagSizeMap Optional mapping of tag IDs to specific tag sizes, range of tag IDs (key): [0, 1024), range of tag sizes (value): (0, infinity)
		 * @return A vector of detected Oculus tags
		 */
		static OculusTags detectOculusTags(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const Scalar defaultTagSize, const TagSizeMap& tagSizeMap = TagSizeMap());

		/**
		 * Detects Oculus tags in a pair of 8-bit grayscale images
		 * @param anyCameraA The first camera with which the first input image has been recorded, must be valid
		 * @param anyCameraB The second camera with which the second input image has been recorded, must be valid
		 * @param yFrameA The first input image in which Oculus tags will be searched, must be valid
		 * @param yFrameB The first input image in which Oculus tags will be searched, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_cameraA The transformation that converts points in the first camera to device points, must be valid
		 * @param device_T_cameraB The transformation that converts points in the second camera to device points, must be valid
		 * @param defaultTagSize The edge lengths of all detected tags that are not specified in `tagSizeMap` , range: (0, infinity)
		 * @param tagSizeMap Optional mapping of tag IDs to specific tag sizes, range of tag IDs (key): [0, 1024), range of tag sizes (value): (0, infinity)
		 * @return A vector of detected Oculus tags
		 */
		static OculusTags detectOculusTags(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const Frame& yFrameA, const Frame& yFrameB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const Scalar defaultTagSize, const TagSizeMap& tagSizeMap = TagSizeMap());

		/**
		 * Deprecated.
		 *
		 * Detects Oculus tags in a single 8-bit grayscale image
		 * @param fisheyeCamera The fisheye camera with which the input image has been recorded, must be valid
		 * @param yFrame The image in which Oculus tags will be searched, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_camera The transformation that converts camera coordinates to device coordinates, `deviceCoord = device_T_camera * cameraCoord`, must be valid
		 * @param defaultTagSize The edge lengths of all detected tags that are not specified in `tagSizeMap` , range: (0, infinity)
		 * @param tagSizeMap Optional mapping of tag IDs to specific tag sizes, range of tag IDs (key): [0, 1024), range of tag sizes (value): (0, infinity)
		 * @return A vector of detected Oculus tags
		 */
		static inline OculusTags detectOculusTags(const FisheyeCamera& fisheyeCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const Scalar defaultTagSize, const TagSizeMap& tagSizeMap = TagSizeMap());

		/**
		 * Deprecated.
		 *
		 * Detects Oculus tags in a pair of 8-bit grayscale images
		 * @param fisheyeCameraA The first fisheye camera with which the first input image has been recorded, must be valid
		 * @param fisheyeCameraB The second fisheye camera with which the second input image has been recorded, must be valid
		 * @param yFrameA The first input image in which Oculus tags will be searched, must be valid
		 * @param yFrameB The first input image in which Oculus tags will be searched, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_cameraA The transformation that converts points in the first camera to device points, must be valid
		 * @param device_T_cameraB The transformation that converts points in the second camera to device points, must be valid
		 * @param defaultTagSize The edge lengths of all detected tags that are not specified in `tagSizeMap` , range: (0, infinity)
		 * @param tagSizeMap Optional mapping of tag IDs to specific tag sizes, range of tag IDs (key): [0, 1024), range of tag sizes (value): (0, infinity)
		 * @return A vector of detected Oculus tags
		 */
		static inline OculusTags detectOculusTags(const FisheyeCamera& fisheyeCameraA, const FisheyeCamera& fisheyeCameraB, const Frame& yFrameA, const Frame& yFrameB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const Scalar defaultTagSize, const TagSizeMap& tagSizeMap = TagSizeMap());

		/**
		 * Checks if the locations of tag can be confirmed in an image by comparing the tag ID that is read from an image
		 * @note If the closest distance between the tag and the frame is below `OculusTagDetector::yFrameBorder` pixels this function will return false immediately
		 * @param anyCamera The camera that recorded the input frame, must be valid
		 * @param yFrame The image in which the location of the tag will be confirmed, must be valid
		 * @param world_T_device The transformation that maps device coordinates to world coordinates, must be valid
		 * @param device_T_camera The transformation that maps camera coordinates to device coordinates, must be valid
		 * @param tag The tag for which the location will be confirmed, must be valid
		 * @return True if the tag data could be read, decoded, and if the tag ID is identical, otherwise false
		 */
		static bool confirmDetectionInFrame(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const OculusTag& tag);

	protected:

		/**
		 * Determines the reflectance type of an tag candidate and the intensity threshold between foreground and background
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The grayscale image in which the tag candidate has been found, must be valid
		 * @param tag_T_camera The 6DOF pose of the tag relative to the camera coordinates at detection time
		 * @param tagSize The size of the corresponding tag, range: (0, infinity)
		 * @param reflectanceType The reflectance type of the tag candidate that has been determined
		 * @param intensityThreshold The intensity value that can be used to separate (binarize) foreground and background pixel values
		 * @param moduleValueDark The binary value of dark modules, can be `1` or `0`
		 * @return True if the computation was successful, otherwise false
		 */
		static bool determineReflectanceTypeAndIntensityThreshold(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& tag_T_camera, const Scalar tagSize, OculusTag::ReflectanceType& reflectanceType, uint8_t& intensityThreshold, uint8_t& moduleValueDark);

		/**
		 * Determines the top-left corner of the tag candidate
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The grayscale image in which the tag candidate has been found, must be valid
		 * @param unorientedBoundaryPattern The boundary pattern/tag candidate
		 * @param unorientedTag_T_camera The 6DOF pose of the tag relative to the camera coordinates at detection time
		 * @param tagSize The size of the corresponding tag, range: (0, infinity)
		 * @param orientedBoundaryPattern Rotated version of `boundaryPattern` where the first element corresponds to the top-left corner of the tag candidate
		 * @param orientedTag_T_camera The correctly rotated (oriented) 6DOF pose of the tag relative to the camera so that the origin is the in top-left corner of the tag
		 * @param intensityThreshold The intensity value that can be used to separate (binarize) foreground and background pixel values
		 * @param binaryModuleValueDark The binary value of dark modules, can be `1` or `0`
		 * @return True if the computation was successful, otherwise false
		 */
		static bool determineOrientation(const AnyCamera& anyCamera, const Frame& yFrame, const BoundaryPattern& unorientedBoundaryPattern, const HomogenousMatrix4& unorientedTag_T_camera, const Scalar tagSize, BoundaryPattern& orientedBoundaryPattern, HomogenousMatrix4& orientedTag_T_camera, const uint8_t& intensityThreshold, const uint8_t& binaryModuleValueDark);

		/**
		 * Reads the modules from the data matrix of a tag
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The grayscale image in which the tag candidate has been found, must be valid
		 * @param tag_T_camera The 6DOF pose of the tag relative to the camera coordinates at detection time
		 * @param tagSize The size of the corresponding tag, range: (0, infinity)
		 * @param intensityThreshold The intensity value that can be used to separate (binarize) foreground and background pixel values
		 * @param binaryModuleValueDark The binary value of dark modules, can be `1` or `0`
		 * @param binaryModuleValueLight binary value of light modules, can be `1` or `0`
		 * @param dataMatrix Will store the modules that were read from a tag
		 * @return True if the computation was successful, otherwise false
		 */
		static bool readDataMatrix(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& tag_T_camera, const Scalar tagSize, const uint8_t& intensityThreshold, const uint8_t& binaryModuleValueDark, const uint8_t& binaryModuleValueLight, OculusTag::DataMatrix& dataMatrix);

		/**
		 * Computes the 6DOF pose of the tag relative to the location of the camera using 3D-to-2D point correspondences
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param imagePoints The 2D image points, size: [4, infinity)
		 * @param objectPoints The corresponding 3D object points in the reference frame of the tag, size: must be identical to `imagePoints`
		 * @param object_T_camera The 6DOF pose of the tag relative to the camera
		 * @param minPoints Minimum number of points used in each RANSAC iteration, range: [4, imagePoints.size())
		 * @return True if the pose has been successfully computed, otherwise false
		 */
		static bool computePose(const AnyCamera& anyCamera, const Vectors2& imagePoints, const Vectors3& objectPoints, HomogenousMatrix4& object_T_camera, const uint32_t minPoints = 4u);

		/**
		 * Optimize the 6DOF pose of the tag after initial detection using additional corners
		 * @param anyCamera The camera with which the input image has been recorded, must be valid
		 * @param yFrame The image in which the corners were found, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_camera The transformation that converts points in the camera to device points, must be valid
		 * @param tag_T_camera Homogeneous transformation to map 3D points in the camera coordinate system to the tag coordinate system, must be valid
		 * @param boundaryPattern The boundary pattern that of the candidate tag, must be valid
		 * @param tagSize The size of the corresponding tag, range: (0, infinity)
		 * @param dataMatrix The data matrix of the detected tag from which additional support points will be selected, must be valid
		 * @return True if the optimization was successful, otherwise false
		 */
		static bool optimizePose(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, HomogenousMatrix4& tag_T_camera, const BoundaryPattern& boundaryPattern, const Scalar tagSize, const OculusTag::DataMatrix& dataMatrix);

		/**
		 * Optimize the 6DOF pose of the tag after initial detection using additional corners and a second frame
		 * @param anyCameraA The first camera with which the first input image has been recorded, must be valid
		 * @param anyCameraB The second camera with which the second input image has been recorded, must be valid
		 * @param yFrameA The first image in which the corners were found, must be valid
		 * @param yFrameB The second image in which the corners were found, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_cameraA The transformation that converts points in the first camera to device points, must be valid
		 * @param device_T_cameraB The transformation that converts points in the second camera to device points, must be valid
		 * @param boundaryPattern The boundary pattern that was found in the first camera, must be valid
		 * @param tagSize The size of the corresponding tag, range: (0, infinity)
		 * @param dataMatrix The data matrix of the detected tag from which additional support points will be selected, must be valid
		 * @param tag_T_cameraA Homogeneous transformation to map 3D points in the camera coordinate system to the tag coordinate system, must be valid
		 * @return True if the optimization was successful, otherwise false
		 */
		static bool optimizePose(const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const Frame& yFrameA, const Frame& yFrameB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const BoundaryPattern& boundaryPattern, const Scalar tagSize, const OculusTag::DataMatrix& dataMatrix, HomogenousMatrix4& tag_T_cameraA);

	protected:

		/// Defines an area inside the image along the image border that will be ignored completely (in pixels)
		static constexpr uint32_t yFrameBorder = 10u;

		/// Minimum absolute difference between foreground and background color in order to count as a transition
		static constexpr uint32_t minIntensityThreshold = 10u;
};

inline OculusTags OculusTagDetector::detectOculusTags(const FisheyeCamera& fisheyeCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const Scalar defaultTagSize, const TagSizeMap& tagSizeMap)
{
	ocean_assert(fisheyeCamera.isValid());

	const Ocean::AnyCameraFisheye anyCameraFisheye(fisheyeCamera);

	return detectOculusTags(anyCameraFisheye, yFrame, world_T_device, device_T_camera, defaultTagSize, tagSizeMap);
}

inline OculusTags OculusTagDetector::detectOculusTags(const FisheyeCamera& fisheyeCameraA, const FisheyeCamera& fisheyeCameraB, const Frame& yFrameA, const Frame& yFrameB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const Scalar defaultTagSize, const TagSizeMap& tagSizeMap)
{
	ocean_assert(fisheyeCameraA.isValid() && fisheyeCameraB.isValid());

	const Ocean::AnyCameraFisheye anyCameraFisheyeA(fisheyeCameraA);
	const Ocean::AnyCameraFisheye anyCameraFisheyeB(fisheyeCameraB);

	return detectOculusTags(anyCameraFisheyeA, anyCameraFisheyeB, yFrameA, yFrameB, world_T_device, device_T_cameraA, device_T_cameraB, defaultTagSize, tagSizeMap);
}

} // namespace OculusTags

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_OCULUSTAGS_OCULUSTAGDETECTOR_H
