/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_FAST_FEATURE_H
#define META_OCEAN_CV_DETECTOR_FAST_FEATURE_H

#include "ocean/cv/detector/Detector.h"
#include "ocean/cv/detector/PointFeature.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

// Forward declaration.
class FASTFeature;

/**
 * Definition of a vector holding FAST features.
 * @ingroup cvdetector
 */
typedef std::vector<FASTFeature> FASTFeatures;

/**
 * This class implements a FAST feature.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT FASTFeature : public PointFeature
{
	public:

		/**
		 * Creates a new empty FAST feature object.
		 */
		inline FASTFeature();

		/**
		 * Creates a new feature object by a given 2D observation position in e.g. an image.
		 * @param observation 2D feature observation
		 * @param distortionState Distortion state of the 2D feature position
		 * @param strength Feature strength
		 */
		inline FASTFeature(const Vector2& observation, const DistortionState distortionState, const Scalar strength);

		/**
		 * Converts FAST features to simple 2D image positions.
		 * Thus, the 2D positions are preserved only.
		 * @param features FAST features to convert
		 * @return Resulting image points
		 */
		static inline Geometry::ImagePoints features2imagePoints(const FASTFeatures& features);

		/**
		 * Converts FAST features to simple 2D image positions.
		 * Thus, the 2D positions are preserved only.
		 * @param features FAST feature to convert
		 * @param number Number of features to be converted
		 * @return Resulting image points
		 */
		static inline Geometry::ImagePoints features2imagePoints(const FASTFeatures& features, const unsigned int number);

		/**
		 * Converts FAST features to point features.
		 * @param features FAST features to convert
		 * @return Resulting point features
		 */
		static inline PointFeatures features2pointFeatures(const FASTFeatures& features);

		/**
		 * Converts FAST features to point features.
		 * @param features FAST features to convert
		 * @param number Number of features to be converted
		 * @return Resulting point features
		 */
		static inline PointFeatures features2pointFeatures(const FASTFeatures& features, const unsigned int number);
};

inline FASTFeature::FASTFeature() :
	PointFeature()
{
	// nothing to do here
}

inline FASTFeature::FASTFeature(const Vector2& observation, const DistortionState distortionState, const Scalar strength) :
	PointFeature(observation, distortionState, strength)
{
	// nothing to do here
}

inline Geometry::ImagePoints FASTFeature::features2imagePoints(const FASTFeatures& features)
{
	Geometry::ImagePoints result;
	result.reserve(features.size());

	for (FASTFeatures::const_iterator i = features.begin(); i != features.end(); ++i)
		result.push_back(i->observation());

	return result;
}

inline Geometry::ImagePoints FASTFeature::features2imagePoints(const FASTFeatures& features, const unsigned int number)
{
	const unsigned int realNumber = min(number, (unsigned int)features.size());

	Geometry::ImagePoints result;
	result.reserve(realNumber);

	for (unsigned int n = 0; n < realNumber; ++n)
		result.push_back(features[n].observation());

	return result;
}

inline PointFeatures FASTFeature::features2pointFeatures(const FASTFeatures& features)
{
	PointFeatures result;
	result.reserve(features.size());

	for (FASTFeatures::const_iterator i = features.begin(); i != features.end(); ++i)
		result.push_back(*i);

	return result;
}

inline PointFeatures FASTFeature::features2pointFeatures(const FASTFeatures& features, const unsigned int number)
{
	const unsigned int realNumber = min(number, (unsigned int)features.size());

	PointFeatures result;
	result.reserve(realNumber);

	for (unsigned int n = 0; n < realNumber; ++n)
		result.push_back(features[n]);

	return result;
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_FAST_FEATURE_H
