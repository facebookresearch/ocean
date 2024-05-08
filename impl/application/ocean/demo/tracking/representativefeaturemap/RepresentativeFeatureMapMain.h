// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_REPRESENTATIVEFEATUREMAP_REPRESENTATIVE_FEATURE_MAP_MAIN_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_REPRESENTATIVEFEATUREMAP_REPRESENTATIVE_FEATURE_MAP_MAIN_H

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

#include "application/ocean/demo/tracking/representativefeaturemap/CountingFeatureMap.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

using namespace Ocean;

/**
 * @ingroup applicationdemotracking
 * @defgroup applicationdemotrackingrepresentativefeaturemap Representative Feature Map
 * @{
 * The demo application implements an pattern recognition training tool.
 * This application is platform independent.
 * @}
 */

/**
 * Creates a feature map object for one pattern, and with help of corresponding video sequences.
 * @param patternFile The file of the pattern for which the feature map will be created
 * @param randomGenerator Random number generator to be used
 * @param countingFeatureMap The resulting feature map
 * @param rotateFrame True, to rotate the frame four times by 90 degree, (and to determine correspondences for all these images)
 * @param shrinkFrame True, to shrink the frame by halving the dimension of the frame, (and to apply the correspondences determination for two individual dimensions)
 * @return True, if succeeded
 * @ingroup applicationdemotrackingrepresentativefeaturemap
 */
bool createCountingFeatureMap(const IO::File& patternFile, RandomGenerator& randomGenerator, CountingFeatureMap& countingFeatureMap, const bool rotateFrame = true, const bool shrinkFrame = true);

/**
 * Creates feature map objects for all patterns, and with help of corresponding video sequences, in a specified directory.
 * @param directory The directory in which all patterns will be handled
 * @param patternBaseName The base name of all patterns
 * @param rotateFrame True, to rotate frames four times by 90 degree, (and to determine correspondences for all these images)
 * @param shrinkFrame True, to shrink frames by halving the dimension of the frame, (and to apply the correspondences determination for two individual dimensions)
 * @return The resulting feature maps
 * @ingroup applicationdemotrackingrepresentativefeaturemap
 */
CountingFeatureMaps createCountingFeatureMaps(const IO::Directory& directory, const std::string& patternBaseName, const bool rotateFrame = true, const bool shrinkFrame = true);

/**
 * Checks whether a given pattern can be recognized in each frame of video sequences based on a set of features.
 * @param patternFile The file of the pattern, must be valid
 * @param patternFeatures The features representing the pattern
 * @param minimalCorrespondences Minimal number of valid point correspondences to succeed, with range [4, infinity)
 * @param rotateFrame True, to rotate the frame four times by 90 degree, (and to determine correspondences for all these images)
 * @param shrinkFrame True, to shrink the frame by halving the dimension of the frame, (and to apply the correspondences determination for two individual dimensions)
 * @return True, if succeeded
 * @ingroup applicationdemotrackingrepresentativefeaturemap
 */
bool checkFeatures(const IO::File& patternFile, const CV::Detector::Blob::BlobFeatures& patternFeatures, const unsigned int minimalCorrespondences = 15u, const bool rotateFrame = true, const bool shrinkFrame = true);

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_REPRESENTATIVEFEATUREMAP_REPRESENTATIVE_FEATURE_MAP_MAIN_H
