/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_FEATURE_H
#define META_OCEAN_CV_DETECTOR_FEATURE_H

#include "ocean/cv/detector/Detector.h"

#include <vector>

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * Definition of a vector holding feature indices.
 * @ingroup cv
 */
typedef std::vector<unsigned int> FeatureIndices;

/**
 * This class implements the abstract base class for arbitrary computer vision features.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT Feature
{
	public:

		/**
		 * Creates a new feature object
		 */
		inline Feature();
};

inline Feature::Feature()
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_CV_DETECTOR_FEATURE_H
