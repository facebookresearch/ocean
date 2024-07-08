/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_POINT_FEATURE_H
#define META_OCEAN_CV_DETECTOR_POINT_FEATURE_H

#include "ocean/cv/detector/Detector.h"
#include "ocean/cv/detector/Feature.h"

#include "ocean/geometry/Geometry.h"

#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

// Forward declaration.
class PointFeature;

/**
 * Definition of a vector holding point features.
 * @ingroup cvdetector
 */
typedef std::vector<PointFeature> PointFeatures;

/**
 * This class implements the base class for all computer vision features mainly basing on points.
 * The feature observation is the 2D position the feature can observed e.g. on an image plane.<br>
 * In contrast the feature position is the real 3D position of the feature.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT PointFeature : public Feature
{
	public:

		/**
		 * Definition of individual distortion states.
		 */
		enum DistortionState
		{
			/// Invalid distortion state.
			DS_INVALID,
			/// Unknown distortion state.
			DS_UNKNOWN,
			/// Distorted position.
			DS_DISTORTED,
			/// Undistorted (distortion free) position.
			DS_UNDISTORTED
		};

	public:

		/**
		 * Creates a new feature object
		 */
		inline PointFeature();

		/**
		 * Creates a new feature object by a given 2D observation point.
		 * @param observation Feature 2D observation position
		 * @param distortionState Distortion state of the 2D feature position
		 * @param strength Feature strength
		 */
		inline explicit PointFeature(const Vector2& observation, const DistortionState distortionState = DS_UNKNOWN, const Scalar strength = 0);

		/**
		 * Creates a new feature object by a given position.
		 * @param position Feature 3D position
		 * @param strength Feature strength
		 */
		inline explicit PointFeature(const Vector3& position, const Scalar strength = 0);

		/**
		 * Returns the 2D observation position of this feature e.g. inside a camera frame
		 * @return 2D feature observation position
		 */
		inline const Vector2& observation() const;

		/**
		 * Returns the 3D position of this feature e.g. in the corresponding coordinate system
		 * @return 3D feature position
		 */
		inline const Vector3& position() const;

		/**
		 * Returns the strength of this feature
		 * @return Feature strength
		 */
		inline Scalar strength() const;

		/**
		 * Returns the distortion state of the feature point.
		 * @return Distortion state.
		 */
		inline DistortionState distortionState() const;

		/**
		 * Sets or changes the 2D observation position of this feature.
		 * @param position 2D feature position to set
		 * @param distortionState Distortion state of the feature position
		 */
		inline void setObservation(const Vector2& position, const DistortionState distortionState);

		/**
		 * Sets or changes the 3D position of this feature.
		 * @param position 3D feature position to set
		 */
		inline void setPosition(const Vector3& position);

		/**
		 * Sets or changes the strength of this feature.
		 * @param strength Feature strength to set
		 */
		inline void setStrength(const Scalar strength);

		/**
		 * Converts a point feature to a simple 2D image position.
		 * Thus, the 2D position is preserved only.
		 * @param feature The point feature to convert
		 * @return Resulting image point
		 */
		static inline const Geometry::ImagePoint& feature2imagePoint(const PointFeature& feature);

		/**
		 * Returns whether two feature objects are equal.
		 * @param feature Right feature to test
		 * @return True, if so
		 */
		inline bool operator==(const PointFeature& feature) const;

		/**
		 * Returns whether two feature objects are not equal.
		 * @param feature Right feature to test
		 * @return True, if so
		 */
		inline bool operator!=(const PointFeature& feature) const;

		/**
		 * Returns whether the this feature has a higher strength than a second feature object.
		 * @param feature Second feature to compare
		 * @return True, if so
		 */
		inline bool operator<(const PointFeature& feature) const;

	protected:

		/// 2D feature observation point.
		Vector2 featureObservation;

		/// 3D feature position.
		Vector3 featurePosition;

		/// Feature strength.
		Scalar featureStrength;

		/// Holds the distortion state of the observation position.
		DistortionState featureDistortionState;
};

inline PointFeature::PointFeature() :
	Feature(),
	featureObservation(0, 0),
	featurePosition(0, 0, 0),
	featureStrength(0)
{
	// nothing to do here
}

inline PointFeature::PointFeature(const Vector2& observation, const DistortionState distortionState, const Scalar strength) :
	Feature(),
	featureObservation(observation),
	featurePosition(0, 0, 0),
	featureStrength(strength),
	featureDistortionState(distortionState)
{
	// nothing to do here
}

inline PointFeature::PointFeature(const Vector3& position, const Scalar strength) :
	Feature(),
	featureObservation(0, 0),
	featurePosition(position),
	featureStrength(strength),
	featureDistortionState(DS_INVALID)
{
	// nothing to do here
}

inline const Vector2& PointFeature::observation() const
{
	return featureObservation;
}

inline const Vector3& PointFeature::position() const
{
	return featurePosition;
}

inline Scalar PointFeature::strength() const
{
	return featureStrength;
}

inline PointFeature::DistortionState PointFeature::distortionState() const
{
	return featureDistortionState;
}

inline void PointFeature::setObservation(const Vector2& observation, const DistortionState distortionState)
{
	featureObservation = observation;
	featureDistortionState = distortionState;
}

inline void PointFeature::setPosition(const Vector3& position)
{
	featurePosition = position;
}

inline void PointFeature::setStrength(const Scalar strength)
{
	featureStrength = strength;
}

inline const Geometry::ImagePoint& PointFeature::feature2imagePoint(const PointFeature& feature)
{
	return feature.observation();
}

inline bool PointFeature::operator==(const PointFeature& feature) const
{
	return featureObservation == feature.featureObservation && featurePosition == feature.featurePosition;
}

inline bool PointFeature::operator!=(const PointFeature& feature) const
{
	return !(*this == feature);
}

inline bool PointFeature::operator<(const PointFeature& feature) const
{
	return featureStrength > feature.featureStrength;
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_POINT_FEATURE_H
