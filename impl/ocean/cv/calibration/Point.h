/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CALIBRATION_POINT_H
#define META_OCEAN_CV_CALIBRATION_POINT_H

#include "ocean/cv/calibration/Calibration.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Calibration
{

/// Forward declaration.
class Point;

/**
 * Definition of a vector holding points.
 * @ingroup cvcalibration
 */
using Points = std::vector<Point>;

/**
 * This class holds the relevant information of a detected marker point.
 * A point is defined by an 2D observation location in the camera image, an approximated radius of the point, a sign (black vs. white), and a strength value.
 * @ingroup cvcalibration
 */
class Point
{
	public:

		/**
		 * Creates a new invalid point.
		 */
		Point() = default;

		/**
		 * Creates a new point.
		 * @param observation The 2D observation location in the camera image, must be valid
		 * @param radius The approximated radius of the point, in pixel, with range [1, infinity)
		 * @param strength The strength of the point, with range [0, infinity)
		 */
		inline Point(const Vector2& observation, const unsigned int radius, const float strength);

		/**
		 * Returns the 2D observation location in the camera image.
		 * @return The point's observation
		 */
		inline const Vector2& observation() const;

		/**
		 * Returns th strength of the point, positive for black points on white background, negative for white points on black background.
		 * @return The point's strength
		 */
		inline float strength() const;

		/**
		 * Returns the sign of the point, true for black points on white background, false for white points on black background.
		 * @return The point's sign
		 */
		inline bool sign() const;

		/**
		 * Returns the radius of this point (the scale of the point).
		 * @return The point's radius, in pixel
		 */
		inline unsigned int radius() const;

		/**
		 * Returns whether this point is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

	protected:

		/// The 2D observation of the point within the camera image.
		Vector2 observation_ = Vector2::minValue();

		/// The radius of the point (the scale of the point), in pixel, with range [1, infinity)
		unsigned int radius_ = 0u;

		/// The strength of the point, positive for black points on white background, negative for white points on black background, with range (-infinity, infinity)
		float strength_ = 0.0f;
};

inline Point::Point(const Vector2& observation, const unsigned int radius, const float strength) :
	observation_(observation),
	radius_(radius),
	strength_(strength)
{
	// nothing to do here
}

inline const Vector2& Point::observation() const
{
	return observation_;
}

inline float Point::strength() const
{
	return strength_;
}

inline bool Point::sign() const
{
	return strength_ >= 0.0f;
}

inline unsigned int Point::radius() const
{
	return radius_;
}

inline bool Point::isValid() const
{
	ocean_assert((radius_ == 0u && observation_ == Vector2::minValue()) || (radius_ != 0u && observation_ != Vector2::minValue()));

	return radius_ != 0u;
}

}

}

}

#endif // META_OCEAN_CV_CALIBRATION_POINT_H
