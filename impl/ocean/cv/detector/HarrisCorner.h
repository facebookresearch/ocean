/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_HARRIS_CORNER_H
#define META_OCEAN_CV_DETECTOR_HARRIS_CORNER_H

#include "ocean/cv/detector/Detector.h"
#include "ocean/cv/detector/PointFeature.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

// Forward declaration.
class HarrisCorner;

/**
 * Definition of a vector holding Harris corners.
 * @ingroup cvdetector
 */
typedef std::vector<HarrisCorner> HarrisCorners;

/**
 * This class implements a Harris corner.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT HarrisCorner : public PointFeature
{
	public:

		/**
		 * Creates a new empty Harris corner object.
		 */
		inline HarrisCorner();

		/**
		 * Creates a new feature object by a given 2D observation position in e.g. an image.
		 * @param observation 2D feature observation
		 * @param distortionState Distortion state of the 2D feature position
		 * @param strength Feature strength
		 */
		inline HarrisCorner(const Vector2& observation, const DistortionState distortionState, const Scalar strength);

		/**
		 * Converts a Harris corner to a simple 2D image position.
		 * @param corner Harris corner to be converted
		 * @return Resulting image point
		 */
		static inline Geometry::ImagePoint corner2imagePoint(const HarrisCorner& corner);

		/**
		 * Converts Harris corners to simple 2D image positions.
		 * Thus, the 2D positions are preserved only.
		 * @param corners Harris corners to convert
		 * @return Resulting image points
		 */
		static inline Geometry::ImagePoints corners2imagePoints(const HarrisCorners& corners);

		/**
		 * Converts Harris corners to simple 2D image positions.
		 * Thus, the 2D positions are preserved only.
		 * @param corners Harris corners to convert
		 * @param number Number of corners to be converted, with range [0, corners.size()]
		 * @return Resulting image points
		 */
		static inline Geometry::ImagePoints corners2imagePoints(const HarrisCorners& corners, const size_t number);

		/**
		 * Converts Harris corners to point features.
		 * @param corners Harris corners to convert
		 * @return Resulting point features
		 */
		static inline PointFeatures corners2pointFeatures(const HarrisCorners& corners);

		/**
		 * Converts Harris corners to point features.
		 * @param corners Harris coners to convert
		 * @param number Number of corners to be converted, with range [0, corners.size()]
		 * @return Resulting point features
		 */
		static inline PointFeatures corners2pointFeatures(const HarrisCorners& corners, const size_t number);
};

inline HarrisCorner::HarrisCorner() :
	PointFeature()
{
	// nothing to do here
}

inline HarrisCorner::HarrisCorner(const Vector2& observation, const DistortionState distortionState, const Scalar strength) :
	PointFeature(observation, distortionState, strength)
{
	// nothing to do here
}

inline Geometry::ImagePoint HarrisCorner::corner2imagePoint(const HarrisCorner& corner)
{
	return corner.observation();
}

inline Geometry::ImagePoints HarrisCorner::corners2imagePoints(const HarrisCorners& corners)
{
	Geometry::ImagePoints result;
	result.reserve(corners.size());

	for (HarrisCorners::const_iterator i = corners.begin(); i != corners.end(); ++i)
		result.push_back(i->observation());

	return result;
}

inline Geometry::ImagePoints HarrisCorner::corners2imagePoints(const HarrisCorners& corners, const size_t number)
{
	ocean_assert(number < corners.size());
	const size_t realNumber = min(number, corners.size());

	Geometry::ImagePoints result;
	result.reserve(realNumber);

	for (size_t n = 0; n < realNumber; ++n)
		result.push_back(corners[n].observation());

	return result;
}

inline PointFeatures HarrisCorner::corners2pointFeatures(const HarrisCorners& corners)
{
	PointFeatures result;
	result.reserve(corners.size());

	for (HarrisCorners::const_iterator i = corners.begin(); i != corners.end(); ++i)
		result.push_back(*i);

	return result;
}

inline PointFeatures HarrisCorner::corners2pointFeatures(const HarrisCorners& corners, const size_t number)
{
	ocean_assert(number < corners.size());
	const size_t realNumber = min(number, corners.size());

	PointFeatures result;
	result.reserve(realNumber);

	for (size_t n = 0; n < realNumber; ++n)
		result.push_back(corners[n]);

	return result;
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_HARRIS_CORNER_H
