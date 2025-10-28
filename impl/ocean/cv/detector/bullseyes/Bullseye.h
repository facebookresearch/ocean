/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYE_H
#define OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYE_H

#include "ocean/cv/detector/bullseyes/Bullseyes.h"

#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Bullseyes
{

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
		 */
		Bullseye(const Vector2& position, const Scalar& radius, const unsigned int grayThreshold);

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

} // namespace Bullseyes

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // OCEAN_CV_DETECTOR_BULLSEYES_BULLSEYE_H
