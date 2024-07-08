/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_RELOCALIZER_STEREO_H
#define META_OCEAN_TRACKING_MAPBUILDING_RELOCALIZER_STEREO_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/Relocalizer.h"

#include "ocean/base/DebugElements.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements a relocalizer for stereo cameras.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT RelocalizerStereo : public Relocalizer
{
	public:

		/**
		 * This class implements a container for debug elements.
		 */
		class RelocalizerDebugElements final :
			public DebugElements,
			public Singleton<RelocalizerDebugElements>
		{
			friend class Singleton<RelocalizerDebugElements>;

			public:

				/**
				 * Definition of several debug elements.
				 */
				enum ElementId : uint32_t
				{
					/// An invalid element id.
					EI_INVALID = 0u,

					/// A camera images with image features.
					EI_CAMERA_IMAGES_WITH_FEATURES,

					/// A camera images with feature correspondences.
					EI_CAMERA_IMAGES_WITH_FEATURE_CORRESPONDENCES,
				};
		};

	public:

		/**
		 * Creates a new relocalizer object.
		 */
		RelocalizerStereo() = default;

		/**
		 * Move constructor.
		 * @param relocalizerStereo The relocalizer to be moved
		 */
		inline RelocalizerStereo(RelocalizerStereo&& relocalizerStereo);

		/**
		 * Creates a new relocalizer object.
		 * @param imageFeaturePointDetectorFunction The feature detection and description function to be used
		 */
		explicit RelocalizerStereo(ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction);

		/**
		 * Relocalizes a given frame.
		 * @param cameraA The camera profile defining the projection of the first camera, must be valid
		 * @param cameraB The camera profile defining the projection of the second camera, must be valid
		 * @param device_T_cameraA The transformation between the first camera and the device, with default camera pose pointing towards the negative z-space and y-axis upwards, must be valid
		 * @param device_T_cameraB The transformation between the second camera and the device, with default camera pose pointing towards the negative z-space and y-axis upwards, must be valid
		 * @param yFrameA The first frame with pixel format FORMAT_Y8, must be valid
		 * @param yFrameB The second frame with pixel format FORMAT_Y8, must be valid
		 * @param world_T_device The resulting device pose transforming device to world
		 * @param minimalNumberCorrespondences The minimal number of 2D/3D correspondences so that a camera pose counts as valid, with range [4, infinity)
		 * @param maximalProjectionError The maximal projection error between 3D object points and their 2D observations, in pixels, with range [0, infinity)
		 * @param inlierRate The rate of correspondence inliers within the entire set of correspondences, e.g., 0.15 means that 15% of matched features are correct and can be used to determine a valid pose, with range (0, 1]
		 * @param world_T_roughDevice Optional rough device pose to speedup the relocalization, if known, invalid otherwise
		 * @param worker Optional worker to distribute the computation
		 * @param usedFeatureCorrespondences Optional resulting number of feature correspondences which have been used, nullptr if not of interest
		 * @param usedObjectPointIdsA Optional resulting ids of the 3D object points which have been used during relocalization in the first camera, nullptr if not of interest
		 * @param usedObjectPointIdsB Optional resulting ids of the 3D object points which have been used during relocalization in the second camera, nullptr if not of interest
		 * @param usedImagePointsA Optional resulting 2D image points that have been used during relocalization in the first camera, nullptr if not of interest
		 * @param usedImagePointsB Optional resulting 2D image points that have been used during relocalization in the second camera, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool relocalize(const AnyCamera& cameraA, const AnyCamera& cameraB, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const Frame& yFrameA, const Frame& yFrameB, HomogenousMatrix4& world_T_device, const unsigned int minimalNumberCorrespondences, const Scalar maximalProjectionError, const Scalar inlierRate = Scalar(0.15), const HomogenousMatrix4& world_T_roughDevice = HomogenousMatrix4(false), Worker* worker = nullptr, size_t* usedFeatureCorrespondences = nullptr, Indices32* usedObjectPointIdsA = nullptr, Indices32* usedObjectPointIdsB = nullptr, Vectors2* usedImagePointsA = nullptr, Vectors2* usedImagePointsB = nullptr);

		/**
		 * Move operator.
		 * @param relocalizerStereo The relocalizer to be moved
		 * @return Reference to this object
		 */
		inline RelocalizerStereo& operator=(RelocalizerStereo&& relocalizerStereo);
};

inline RelocalizerStereo::RelocalizerStereo(RelocalizerStereo&& relocalizerStereo)
{
	*this = std::move(relocalizerStereo);
}

inline RelocalizerStereo& RelocalizerStereo::operator=(RelocalizerStereo&& relocalizerStereo)
{
	Relocalizer::operator=(std::move(relocalizerStereo));

	return *this;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_RELOCALIZER_STEREO_H
