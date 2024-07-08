/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_RELOCALIZER_MONO_H
#define META_OCEAN_TRACKING_MAPBUILDING_RELOCALIZER_MONO_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/Relocalizer.h"

#include "ocean/base/DebugElements.h"
#include "ocean/base/Worker.h"

#include "ocean/math/AnyCamera.h"

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class implements a relocalizer for mono cameras.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT RelocalizerMono : public Relocalizer
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

					/// A camera image with image features.
					EI_CAMERA_IMAGE_WITH_FEATURES,

					/// A camera image with feature correspondences.
					EI_CAMERA_IMAGE_WITH_FEATURE_CORRESPONDENCES,
				};
		};

	public:

		/**
		 * Creates a new relocalizer object.
		 */
		RelocalizerMono() = default;

		/**
		 * Move constructor.
		 * @param relocalizerMono The relocalizer to be moved
		 */
		inline RelocalizerMono(RelocalizerMono&& relocalizerMono);

		/**
		 * Creates a new relocalizer object.
		 * @param imageFeaturePointDetectorFunction The feature detection and description function to be used
		 */
		explicit RelocalizerMono(ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction);

		/**
		 * Relocalizes a given frame.
		 * @param camera The camera profile defining the projection, must be valid
		 * @param yFrame The frame with pixel format FORMAT_Y8, must be valid
		 * @param world_T_camera The resulting camera pose transforming camera to world, with default camera pose pointing towards the negative z-space and y-axis upwards
		 * @param minimalNumberCorrespondences The minimal number of 2D/3D correspondences so that a camera pose counts as valid, with range [4, infinity)
		 * @param maximalProjectionError The maximal projection error between 3D object points and their 2D observations, in pixels, with range [0, infinity)
		 * @param inlierRate The rate of correspondence inliers within the entire set of correspondences, e.g., 0.15 means that 15% of matched features are correct and can be used to determine a valid pose, with range (0, 1]
		 * @param world_T_roughCamera Optional rough camera pose to speedup the relocalization, if known, invalid otherwise
		 * @param worker Optional worker to distribute the computation
		 * @param usedObjectPointIds Optional resulting ids of the 3D object points which have been used during relocalization, nullptr if not of interest
		 * @param usedImagePoints Optional resulting 2D image points that have been used during relocalization, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool relocalize(const AnyCamera& camera, const Frame& yFrame, HomogenousMatrix4& world_T_camera, const unsigned int minimalNumberCorrespondences, const Scalar maximalProjectionError, const Scalar inlierRate = Scalar(0.15), const HomogenousMatrix4& world_T_roughCamera = HomogenousMatrix4(false), Worker* worker = nullptr, Indices32* usedObjectPointIds = nullptr, Vectors2* usedImagePoints = nullptr);

		/**
		 * Move operator.
		 * @param relocalizerMono The relocalizer to be moved
		 * @return Reference to this object
		 */
		inline RelocalizerMono& operator=(RelocalizerMono&& relocalizerMono);
};

inline RelocalizerMono::RelocalizerMono(RelocalizerMono&& relocalizerMono)
{
	*this = std::move(relocalizerMono);
}

inline RelocalizerMono& RelocalizerMono::operator=(RelocalizerMono&& relocalizerMono)
{
	Relocalizer::operator=(std::move(relocalizerMono));

	return *this;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_RELOCALIZER_MONO_H
