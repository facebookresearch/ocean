/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_STEREO_IMAGE_PROVIDER_INTERFACE_H
#define META_OCEAN_MEDIA_STEREO_IMAGE_PROVIDER_INTERFACE_H

#include "ocean/media/Media.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/FisheyeCamera.h"
#include "ocean/math/HomogenousMatrix4.h"

namespace Ocean
{

namespace Media
{

/**
 * This class is the abstract base class for all stereo image providers.
 * The provider allows access to stereo images, their corresponding camera calibration data, and optional device poses.
 * @ingroup media
 */
class StereoImageProviderInterface
{
	public:

		/**
		 * Destructs this interface.
		 */
		virtual inline ~StereoImageProviderInterface();

		/**
		 * Returns the next stereo images from the provider, further optional transformations between device and world, and between cameras and device will be provided.
		 * @param fisheyeCameraA The resulting camera profile of the first stereo image, will be valid
		 * @param fisheyeCameraB The resulting camera profile of the second stereo image, will be valid
		 * @param imageA The resulting first stereo image, will be valid
		 * @param imageB The resulting second stereo image, will be valid
		 * @param world_T_device Optional resulting transformation transforming device to world, if known, otherwise invalid
		 * @param device_T_cameraA Optional resulting transformation transforming the first camera to device, if known, otherwise invalid
		 * @param device_T_cameraB Optional resulting transformation transforming the second camera to device, if known, otherwise invalid
		 * @param world_T_leftHandJoints Optional resulting transformations to map from the left hand joints to world coordinates, if known, otherwise invalid
		 * @param world_T_rightHandJoints Optional resulting transformations to map from the right hand joints to world coordinates, if known, otherwise invalid
		 * @param abort Optional abort statement allowing to abort the pending request at any time; set the value True to abort the request
		 * @return True, if the next stereo images could be provided; False, if no further stereo images exist
		 * @see hasDeviceTransformations(), hasCameraTransformations().
		 */
		virtual bool nextStereoImages(FisheyeCamera& fisheyeCameraA, FisheyeCamera& fisheyeCameraB, Frame& imageA, Frame& imageB, HomogenousMatrixD4* world_T_device = nullptr, HomogenousMatrixD4* device_T_cameraA = nullptr, HomogenousMatrixD4* device_T_cameraB = nullptr, HomogenousMatricesD4* world_T_leftHandJoints = nullptr, HomogenousMatricesD4* world_T_rightHandJoints = nullptr, bool* abort = nullptr) = 0;

		/**
		 * Restarts with the first pair of stereo images.
		 * @return True, if succeeded
		 */
		virtual bool restart() = 0;

		/**
		 * Returns whether the provider has access to the transformations between device and world (world_T_device).
		 * @return True, if so
		 */
		virtual bool hasDeviceTransformations() const = 0;

		/**
		 * Returns whether the provider has access to the transformations between camerans and device (device_T_cameraA and device_T_cameraB).
		 * @return True, if so
		 */
		virtual bool hasCameraTransformations() const = 0;

		/**
		 * Returns whether this provider can provide valid stereo images (e.g., whether the underlying resources are initialized and whether the source exists).
		 * @return True, if so
		 */
		virtual bool isValid() const = 0;

		/**
		 * Releases all owned resources like e.g., the image sequence objects.
		 */
		virtual void release() = 0;

	protected:

		/**
		 * Protected default constructor.
		 */
		inline StereoImageProviderInterface();
};

StereoImageProviderInterface::StereoImageProviderInterface()
{
	// nothing to do here
}

StereoImageProviderInterface::~StereoImageProviderInterface()
{
	// nothing to do here
}

}

}

#endif // META_OCEAN_MEDIA_STEREO_IMAGE_PROVIDER_INTERFACE_H
