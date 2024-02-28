// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_VRS_STEREO_IMAGE_PROVIDER_H
#define META_OCEAN_MEDIA_VRS_STEREO_IMAGE_PROVIDER_H

#include "ocean/media/vrs/VRS.h"
#include "ocean/media/vrs/VRSImageSequence.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SampleMap.h"

#include "ocean/media/StereoImageProviderInterface.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

/**
 * This class implements a VRS-based provider for stereo images.
 * @ingroup mediavrs
 */
class OCEAN_MEDIA_VRS_EXPORT StereoImageProvider : public StereoImageProviderInterface
{
	protected:

		/**
		 * Definition of a map mapping timestamps to poses.
		 */
		typedef SampleMap<HomogenousMatrixD4> TransformationMap;

		/**
		 * Definition of a map mapping timestamps to poses.
		 */
		typedef std::map<double, HomogenousMatricesD4> HandPoseMap;

	public:

		/**
		 * Creates a new invalid provider object.
		 */
		StereoImageProvider() = default;

		/**
		 * Move contructor.
		 * @param stereoImageProvider The provider to be moved
		 */
		StereoImageProvider(StereoImageProvider&& stereoImageProvider);

		/**
		 * Creates a new provider object for a given VRS file.
		 * @param vrsFilename The filename of the VRS file from which the stereo images will be provided, must valid
		 * @param posesFilename The filename of the poses file which contains the transformations between device and camera, an empty string to try to read the transformation from the VRS file
		 */
		StereoImageProvider(const std::string& vrsFilename, const std::string& posesFilename = std::string());

		/**
		 * Returns the next stereo images from the provider, further optional transformations between device and world, and between cameras and device will be provided.
		 * @see StereoImageProviderInterface::nextStereoImages(), hasDeviceTransformations(), hasCameraTransformations().
		 */
		bool nextStereoImages(FisheyeCamera& fisheyeCameraA, FisheyeCamera& fisheyeCameraB, Frame& imageA, Frame& imageB, HomogenousMatrixD4* world_T_device = nullptr, HomogenousMatrixD4* device_T_cameraA = nullptr, HomogenousMatrixD4* device_T_cameraB = nullptr, HomogenousMatricesD4* world_T_leftHandJoints = nullptr, HomogenousMatricesD4* world_T_rightHandJoints = nullptr, bool* abort = nullptr) override;

		/**
		 * Restarts with the first pair of stereo images.
		 * @return True, if succeeded
		 */
		bool restart() override;

		/**
		 * Returns whether the provider has access to the transformations between device and world (world_T_device).
		 * @return True, if so
		 */
		bool hasDeviceTransformations() const override;

		/**
		 * Returns whether the provider has access to the transformations between camerans and device (device_T_cameraA and device_T_cameraB).
		 * @return True, if so
		 */
		bool hasCameraTransformations() const override;

		/**
		 * Returns whether this provider can provide valid stereo images (e.g., whether the VRS file exists and contains stereo images).
		 * @return True, if so
		 */
		bool isValid() const override;

		/**
		 * Releases all owned resources like e.g., the image sequence objects.
		 */
		void release() override;

		/**
		 * Move operator.
		 * @param stereoImageProvider The provider to be moved
		 * @return Reference to this object
		 */
		StereoImageProvider& operator=(StereoImageProvider&& stereoImageProvider);

		/**
		 * Determines the (two) lower stereo cameras, their corresponding image sequences respctively.
		 * @param vrsFile The VRS file from which the stereo cameras will be determined
		 * @param recordableA The resulting recordable of the first stere camera
		 * @param recordableB The resulting recordable of the second stereo camera
		 * @return True, if succeeded
		 */
		static bool determineLowerStereoCameraRecordables(const std::string& vrsFile, std::string& recordableA, std::string& recordableB);

	protected:

		/**
		 * Disabled copy contructor.
		 * @param stereoImageProvider The provider which would be copied
		 */
		StereoImageProvider(const StereoImageProvider& stereoImageProvider) = delete;

		/**
		 * Disabled assign operator.
		 * @param stereoImageProvider The provider which would be assigned
		 * @return Reference to this object
		 */
		StereoImageProvider& operator=(const StereoImageProvider& stereoImageProvider) = delete;

	protected:

		/// The image sequence for the first camera.
		ImageSequenceRef imageSequenceA_;

		/// The image sequence for the second camera.
		ImageSequenceRef imageSequenceB_;

		/// The transformations between device and world, if known.
		TransformationMap map_world_T_device_;

		/// The hand poses of the left hand
		HandPoseMap map_leftHandPoses_;

		/// The hand poses of the right hand
		HandPoseMap map_rightHandPoses_;

		/// Timestamp of the most recent image of the first camera.
		Timestamp frameTimestampA_;

		/// Timestamp of the most recent image of the second camera.
		Timestamp frameTimestampB_;

		/// Number of dropped frames from the first camra.
		size_t droppedFrameA_ = 0;

		/// Number of dropped frames from the second camra.
		size_t droppedFrameB_ = 0;

		/// True, if the provider holds valid data.
		bool isValid_ = false;

		/// The lock of this provider.
		mutable Lock lock_;
};

}

}

}

#endif // META_OCEAN_MEDIA_VRS_STEREO_IMAGE_PROVIDER_H
