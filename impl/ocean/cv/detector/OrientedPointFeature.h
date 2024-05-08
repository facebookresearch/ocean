/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_ORIENTED_POINT_FEATURE_H
#define META_OCEAN_CV_DETECTOR_ORIENTED_POINT_FEATURE_H

#include "ocean/cv/detector/Detector.h"
#include "ocean/cv/detector/PointFeature.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

// Forward declaration.
class OrientedPointFeature;

/**
 * Definition of a vector holding oriented point features.
 * @ingroup cvdetector
 */
typedef std::vector<OrientedPointFeature> OrientedPointFeatures;

/**
 * This class implements the base class for all computer vision features mainly basing on points width an orientation.
 * The feature observation is the 2D position the feature can observed e.g. on an image plane.<br>
 * In contrast the feature position is the real 3D position of the feature.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT OrientedPointFeature : public PointFeature
{
	public:

		/**
		 * Creates a new feature object
		 */
		inline OrientedPointFeature();

		/**
		 * Creates a new feature object by a given 2D observation point.
		 * @param observation Feature 2D observation position
		 * @param distortionState Distortion state of the 2D feature position
		 * @param strength Feature strength
		 * @param orientation Feature orientation in radian, range [0, 2*PI)
		 */
		inline explicit OrientedPointFeature(const Vector2& observation, const DistortionState distortionState = DS_UNKNOWN, const Scalar strength = 0, const Scalar orientation = 0);

		/**
		 * Creates a new feature object by a given position.
		 * @param position Feature 3D position
		 * @param strength Feature strength
		 * @param orientation Feature orientation in radian, range [0, 2*PI)
		 */
		inline explicit OrientedPointFeature(const Vector3& position, const Scalar strength = 0, const Scalar orientation = 0);

		/**
		 * Returns the orientation of this feature in radian.
		 * @return Feature orientation
		 */
		inline Scalar orientation() const;

		/**
		 * Sets the orientation of this features in radian.
		 * @param orientation Feature orientation to set
		 */
		inline void setOrientation(const Scalar orientation);

	protected:

		// Feature orientation in radian, range [0, 2*PI)
		Scalar featureOrientation;

};

inline OrientedPointFeature::OrientedPointFeature() :
	PointFeature(),
	featureOrientation(0)
{
	// nothing to do here
}

inline OrientedPointFeature::OrientedPointFeature(const Vector2& observation, const DistortionState distortionState, const Scalar strength, const Scalar orientation) :
	PointFeature(observation, distortionState, strength)
{
	ocean_assert_accuracy(orientation >= 0 && orientation < Numeric::pi2());
	featureOrientation = orientation;
}

inline OrientedPointFeature::OrientedPointFeature(const Vector3& position, const Scalar strength, const Scalar orientation) :
	PointFeature(position, strength)
{
	ocean_assert_accuracy(orientation >= 0 && orientation < Numeric::pi2());
	featureOrientation = orientation;

	featureDistortionState = DS_INVALID;
}

inline Scalar OrientedPointFeature::orientation() const
{
	return featureOrientation;
}

inline void OrientedPointFeature::setOrientation(const Scalar orientation)
{
	ocean_assert_accuracy(orientation >= 0 && orientation < Numeric::pi2());
	featureOrientation = orientation;
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_ORIENTED_POINT_FEATURE_H
