/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYE_H
#define OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYE_H

#include "ocean/cv/detector/bullseyes/Bullseyes.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/Vector2.h"

#include <array>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

/**
 * Structure to hold a single half-ray result from radial consistency checking.
 * A half-ray extends from the center of a bullseye candidate in one direction.
 */
class OCEAN_CV_DETECTOR_BULLSEYES_EXPORT HalfRay
{
	public:

		/// Array of 3 transition points along the half-ray: [0] B->W (center to first ring), [1] W->B (first to second ring), [2] B->W (second ring to background)
		using TransitionPoints = std::array<Vector2, 3>;

	public:

		/// Default constructor.
		HalfRay() = default;

		/**
		 * Returns whether this half-ray found all 3 transition points.
		 * @return True if all 3 transition points are valid and angle is within range [0, 2*PI)
		 */
		inline bool isValid() const;

		/**
		 * Returns whether all intensity check points have the expected values.
		 * @return True if all 3 intensity checks passed (background/foreground validation)
		 */
		inline bool areIntensitiesValid() const;

		/**
		 * Returns an invalid transition point.
		 * @return Invalid transition point
		 */
		static inline const Vector2& invalidTransitionPoint();

		/**
		 * Returns an invalid array of transition points.
		 * @return Array of 3 invalid transition points
		 */
		static inline const TransitionPoints& invalidTransitionPoints();

	public:

		/// Transition points for this half-ray
		TransitionPoints transitionPoints = invalidTransitionPoints();

		/// Points sampled for intensity validation: [0] between transitions 0-1 (white ring), [1] between transitions 1-2 (black ring), [2] beyond transition 2 (background)
		Vector2 intensityCheckPoints[3];

		/// Flags indicating whether each intensity check passed: [0] white ring, [1] black ring, [2] background
		bool isIntensityValid[3] = {false, false, false};

		/// Angle of this half-ray in radians
		Scalar angle = Scalar(-1);
};

/// Definition of a vector holding half-rays.
using HalfRays = std::vector<HalfRay>;

/**
 * Structure to hold diameter results (positive + negative half-rays).
 * A diameter consists of two half-rays extending in opposite directions from the center.
 */
class OCEAN_CV_DETECTOR_BULLSEYES_EXPORT Diameter
{
	public:

		/// Default constructor.
		Diameter() = default;

		/**
		 * Returns whether both half-rays found all 3 transition points.
		 * @return True if both half-rays are valid
		 */
		inline bool areHalfRaysValid() const;

		/**
		 * Returns whether all intensity checks passed in both half-rays.
		 * @return True if both positive and negative half-rays have valid intensities (background and foreground values match expectations)
		 */
		inline bool areIntensitiesValid() const;

	public:

		/// True if distances match within symmetry tolerance
		bool isSymmetryValid = false;

		/// Positive direction half-ray (angle a)
		HalfRay halfRayPositive;

		/// Negative direction half-ray (angle a + PI)
		HalfRay halfRayNegative;
};

/// Definition of a vector holding diameters.
using Diameters = std::vector<Diameter>;

/**
 * Definition of a bullseye composed of a location and a radius.
 * @ingroup cvdetectorbullseyes
 */
class OCEAN_CV_DETECTOR_BULLSEYES_EXPORT Bullseye
{
	public:

		/**
		 * Creates an invalid bullseye object.
		 */
		Bullseye() = default;

		/**
		 * Creates a new bullseye object by a given position and radius.
		 * @param position The (center) position of the bullseye within the camera frame
		 * @param radius The radius of the bullseye in pixels, with range (0, infinity)
		 * @param grayThreshold Threshold that was used during the detection, range [1, 255]
		 * @param pyramidLayer The pyramid layer at which this bullseye was detected, with range [0, infinity)
		 */
		Bullseye(const Vector2& position, const Scalar& radius, const unsigned int grayThreshold, const unsigned int pyramidLayer = 0u);

		/**
		 * Returns whether this bullseye is valid.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns the (center) position of the bullseye.
		 * @return The Bullseye's position within the camera frame
		 */
		const Vector2& position() const;

		/**
		 * Returns the radius of the bullseye.
		 * @return The Bullseye's radius, with range (0, infinity), 0 for an invalid object
		 */
		Scalar radius() const;

		/**
		 * Returns the threshold that was used for the detection of this bullseye
		 * @return The threshold value
		 */
		unsigned int grayThreshold() const;

		/**
		 * Returns the pyramid layer at which this bullseye was detected.
		 * @return The pyramid layer index, with range [0, infinity)
		 */
		unsigned int pyramidLayer() const;

		/**
		 * Returns an invalid bullseye position.
		 * @return Invalid bullseye position
		 */
		static Vector2 invalidPosition();

		/**
		 * Returns an invalid bullseye radius.
		 * @return Invalid bullseye radius
		 */
		static constexpr Scalar invalidRadius();

		/**
		 * Returns an invalid bullseye threshold.
		 * @return Invalid bullseye threshold
		 */
		static constexpr unsigned int invalidGrayThreshold();

	protected:

		/// The (center) position of the bullseye within the camera frame.
		Vector2 position_ = invalidPosition();

		/// The radius of the bullseye in pixels, with range (0, infinity).
		Scalar radius_ = invalidRadius();

		/// The threshold that was used during the detection of this bullseye
		unsigned int grayThreshold_ = invalidGrayThreshold();

		/// The pyramid layer at which this bullseye was detected, with range [0, infinity).
		unsigned int pyramidLayer_ = 0u;
};

/// Definition of a vector holding bullseyes.
using Bullseyes = std::vector<Bullseye>;

constexpr Scalar Bullseye::invalidRadius()
{
	return Scalar(-1);
}

constexpr unsigned int Bullseye::invalidGrayThreshold()
{
	return (unsigned int)(-1);
}

inline const Vector2& HalfRay::invalidTransitionPoint()
{
	static const Vector2 invalidPoint(Numeric::minValue(), Numeric::minValue());
	return invalidPoint;
}

inline const HalfRay::TransitionPoints& HalfRay::invalidTransitionPoints()
{
	static const TransitionPoints invalidPoints = {invalidTransitionPoint(), invalidTransitionPoint(), invalidTransitionPoint()};
	return invalidPoints;
}

inline bool HalfRay::isValid() const
{
	for (const Vector2& transitionPoint : transitionPoints)
	{
		if (transitionPoint == invalidTransitionPoint())
		{
			return false;
		}
	}

	if (angle < 0 || angle >= Numeric::pi2())
	{
		return false;
	}

	return true;
}

inline bool HalfRay::areIntensitiesValid() const
{
	return isIntensityValid[0] && isIntensityValid[1] && isIntensityValid[2];
}

inline bool Diameter::areHalfRaysValid() const
{
	return halfRayPositive.isValid() && halfRayNegative.isValid();
}

inline bool Diameter::areIntensitiesValid() const
{
	return halfRayPositive.areIntensitiesValid() && halfRayNegative.areIntensitiesValid();
}

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYE_H
