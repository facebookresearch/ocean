/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPBUILDING_RELOCALIZER_H
#define META_OCEAN_TRACKING_MAPBUILDING_RELOCALIZER_H

#include "ocean/tracking/mapbuilding/MapBuilding.h"
#include "ocean/tracking/mapbuilding/UnifiedDescriptor.h"
#include "ocean/tracking/mapbuilding/UnifiedFeatureMap.h"

#include <functional>

namespace Ocean
{

namespace Tracking
{

namespace MapBuilding
{

/**
 * This class base class for all relocalizers.
 * @ingroup trackingmapbuilding
 */
class OCEAN_TRACKING_MAPBUILDING_EXPORT Relocalizer
{
	public:

		/**
		 * Definition of a function which detects and describes feature points in a given image.
		 * @param camera The camera profile associated with the image
		 * @param yFrame The image in which the feature will be detected, with pixel format FORMAT_Y8
		 * @param imagePoints The resulting image points within the pixel domain
		 * @param imagePointDescriptors The resulting descriptors, one for each resulting image point
		 * @return True, if succeeded
		 */
		using ImageFeaturePointDetectorFunction = std::function<bool(const AnyCamera& camera, const Frame& yFrame, Vectors2& imagePoints, SharedUnifiedDescriptors& imagePointDescriptors)>;

	public:

		/**
		 * Destructs this relocalizer.
		 */
		virtual ~Relocalizer() = default;

		/**
		 * Sets or updates the function to detect and describe feature points in an image.
		 * @param imageFeaturePointDetectorFunction The function to be used
		 * @return True, if succeeded
		 */
		virtual bool setImageFeaturePointDetectorFunction(ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction);

		/**
		 * Sets or updates the feature map to be used for relocalization.
		 * @param featureMap The feature map to be set, must be valid
		 * @return True, if succeeded
		 */
		virtual bool setFeatureMap(SharedUnifiedFeatureMap featureMap);

		/**
		 * Returns the object points of this relocalizer.
		 * This function is not thread-safe.
		 * @return The relocalizer's 3D object points
		 */
		inline const Vectors3& objectPoints() const;

		/**
		 * Returns the ids of the object points of this relocalizer.
		 * This function is not thread-safe.
		 * @return The relocalizer's object point ids
		 */
		inline const Indices32& objectPointIds() const;

		/**
		 * Returns whether this relocalizer holds a valid map.
		 * @return True, if so
		 */
		virtual bool isValid() const;

		/**
		 * Helper function allowing to detect and to describe multi-level FREAK features with 32 bytes per descriptor in an image.
		 * @param camera The camera profile to be used, must be valid
		 * @param yFrame The image in which the feature will be detected and described, with pixel format FORMAT_Y8, must be valid
		 * @param imagePoints The resulting image points of all detected features
		 * @param imagePointDescriptors The resulting descriptors, one for each resulting image point
		 * @return True, if succeeded
		 */
		static bool detectFreakFeatures(const AnyCamera& camera, const Frame& yFrame, Vectors2& imagePoints, SharedUnifiedDescriptors& imagePointDescriptors);

	protected:

		/**
		 * Default constructor.
		 */
		Relocalizer() = default;

		/**
		 * Creates a new relocalizer object.
		 * @param imageFeaturePointDetectorFunction The feature detection and description function to be used
		 */
		explicit Relocalizer(ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction);

		/**
		 * Move operator.
		 * @param relocalizer The relocalizer to be moved
		 * @return Reference to this object
		 */
		Relocalizer& operator=(Relocalizer&& relocalizer);

	protected:

		///  The function which detects and describes feature points in a given image.
		ImageFeaturePointDetectorFunction imageFeaturePointDetectorFunction_;

		/// The feature map to be used when relocalizing.
		SharedUnifiedFeatureMap featureMap_;

		/// The random generator object to be used.
		RandomGenerator randomGenerator_;

		/// The relocalizer's lock.
		mutable Lock lock_;
};

inline const Vectors3& Relocalizer::objectPoints() const
{
	ocean_assert(isValid());

	return featureMap_->objectPoints();
}

inline const Indices32& Relocalizer::objectPointIds() const
{
	ocean_assert(isValid());

	return featureMap_->objectPointIds();
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPBUILDING_RELOCALIZER_H
