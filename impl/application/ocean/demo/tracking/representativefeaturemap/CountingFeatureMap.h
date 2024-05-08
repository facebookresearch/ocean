/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_PATTERNRECOGNITION_COUNTING_FEATURE_MAP_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_PATTERNRECOGNITION_COUNTING_FEATURE_MAP_H

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

using namespace Ocean;

/**
 * Definition of feature points.
 * @ingroup applicationdemotrackingrepresentativefeaturemap
 */
typedef CV::Detector::Blob::BlobFeatures FeaturePoints;

/**
 * Definition of a map mapping feature ids to recognition counts.
 * @ingroup applicationdemotrackingrepresentativefeaturemap
 */
typedef std::map<unsigned int, unsigned int> FeatureId2CountMap;

/**
 * Definition of a multimap mapping recognition counts to feature ids.
 * @ingroup applicationdemotrackingrepresentativefeaturemap
 */
typedef std::multimap<unsigned int, unsigned int> FeatureCount2IdMap;

// Forward declaration.
class CountingFeatureMap;

/**
 * Definition of a vector holding feature map objects.
 * @ingroup applicationdemotrackingrepresentativefeaturemap
 */
typedef std::vector<CountingFeatureMap> CountingFeatureMaps;

/**
 * This class realizes a feature map holding feature points of e.g., a tracking pattern and stores information about the reliability of each individual feature point (counts the number of matches for each feature).
 * @ingroup applicationdemotrackingrepresentativefeaturemap
 */
class CountingFeatureMap
{
	public:

		/**
		 * Creates a new empty feature map object.
		 */
		inline CountingFeatureMap();

		/**
		 * Creates a new feature map by a given pattern and corresponding features.
		 * @param patternName The name of the pattern (the filename of the image not including the prefix path), must be valid
		 * @param patternFrame The frame of the pattern in which the features have been determined, must be valid
		 * @param features The features which have been determined in the pattern, will be moved
		 */
		inline CountingFeatureMap(const std::string& patternName, const Frame& patternFrame, FeaturePoints&& features);

		/**
		 * Returns the name of the pattern (the filename of the image not including the path).
		 * @return The pattern's name
		 */
		inline const std::string& patternName() const;

		/**
		 * Returns the pattern frame for which the feature map is created.
		 * @return The frame providing the features
		 */
		inline const Frame& patternFrame() const;

		/**
		 * Returns the number of frames which have been used to determine the counts of the used feature points for correspondences/matches.
		 * @return The number of frames, with range [0, infinity)
		 */
		inline unsigned int frames() const;

		/**
		 * Returns the number of frames which have been used to determine the counts of the used feature points for correspondences/matches.
		 * @return The number of frames, with range [0, infinity)
		 */
		inline unsigned int& frames();

		/**
		 * Returns the features of this map.
		 * @return The map's features
		 */
		inline const FeaturePoints& features() const;

		/**
		 * Returns the number of counts each feature point has been assigned a valid correspondence.
		 * @return The counts for each feature point
		 */
		inline const FeatureId2CountMap& featureId2Count() const;

		/**
		 * Returns the number of counts each feature point has been assigned a valid correspondence.
		 * @return The counts for each feature point
		 */
		inline FeatureId2CountMap& featureId2Count();

		/**
		 * Returns the number of counts each feature point has been assigned a wrong correspondence.
		 * @return The counts for each feature point
		 */
		inline const FeatureId2CountMap& featureId2Ambiguous() const;

		/**
		 * Returns the number of counts each feature point has been assigned a wrong correspondence.
		 * @return The counts for each feature point
		 */
		inline FeatureId2CountMap& featureId2Ambiguous();

		/**
		 * Returns the ids of all features sorted due to their number of valid correspondences.
		 * @return The features sorted by the number of valid correspondences
		 */
		const FeatureCount2IdMap& featureCount2Id();

		/**
		 * Determines correspondences between feature points stored in this map and feature points from a given frame.
		 * Optional, the given frame can be modified regarding rotation and shrinking.<br>
		 * A homography is used to verify correspondence candidates.<br>
		 * This function can create up to eight different versions of the given frame for which correspondences will be determined.
		 * @param frame The frame in which feature points will be determined and matched against those from the feature map
		 * @param randomGenerator Random number generator to be used
		 * @param rotateFrame True, to rotate the frame four times by 90 degree, (and to determine correspondences for all these images)
		 * @param shrinkFrame True, to shrink the frame by halving the dimension of the frame, (and to apply the correspondences determination for two individual dimensions)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool modifyFrameAndDetermineCorrespondences(const Frame& frame, RandomGenerator& randomGenerator, const bool rotateFrame = true, const bool shrinkFrame = true, Worker* worker = nullptr);

		/**
		 * Checks whether enough feature point correspondences between (already existing) feature points from a feature map and feature points from a given image can be detected/determined.
		 * A homography is used to verify correspondence candidates.<br>
		 * This function can create up to eight different versions of the given frame and each frame is checked individually.
		 * @param mapFeatures The already existing features from the feature map
		 * @param frame The frame in which new feature points will be detected, described an than used for matching
		 * @param randomGenerator Random number generator to be used
		 * @param minimalCorrespondences Minimal number of valid point correspondences to succeed, with range [4, infinity)
		 * @param rotateFrame True, to rotate the frame four times by 90 degree, (and to determine correspondences for all these images)
		 * @param shrinkFrame True, to shrink the frame by halving the dimension of the frame, (and to apply the correspondences determination for two individual dimensions)
		 * @param numberFrames Optional resulting number of frames that have been used for testing, with range [0, 8], nullptr if not of interest
		 * @param worker Optional worker object to distribute the computation
		 * @return The number of frames for which valid correspondences could be determined, with range [0, *numberFrames]
		 */
		static unsigned int modifyFrameAndCheckCorrespondences(const FeaturePoints& mapFeatures, const Frame& frame, RandomGenerator& randomGenerator, unsigned int minimalCorrespondences = 10u, const bool rotateFrame = true, const bool shrinkFrame = true, unsigned int* numberFrames = nullptr, Worker* worker = nullptr);

		/**
		 * Determines and describes feature points in a given frame.
		 * @param frame The frame in which the feature points will be detected
		 * @param threshold The strength threshold of detected feature points, with range (0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static FeaturePoints determineBlobFeatures(const Frame& frame, const Scalar threshold = Scalar(6.5), Worker* worker = nullptr);

		/**
		 * Determines correspondences between feature points stored in a map and feature points in a given camera frame.
		 * First, candidates of correspondences will be determined.<br>
		 * Second, the candidates will be separated into good and bad correspondences based on a homograph transformation.
		 * @param mapFeatures The feature points stored in a (feature) map
		 * @param frame The frame in which new feature points will be detected and matched with the feature points from the map, must be valid
		 * @param randomGenerator Random number generator to be used
		 * @param foundMapFeatures The indices of the feature points from the (feature) map for which valid/correct corresponding feature points in the frame could be found
		 * @param ambiguousMapFeatures The indices of the feature points from the (feature) map for which wrong corresponding feature points in the frame could be found
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool determineCorrespondences(const FeaturePoints& mapFeatures, const Frame& frame, RandomGenerator& randomGenerator, Indices32& foundMapFeatures, Indices32& ambiguousMapFeatures, Worker* worker = nullptr);

		/**
		 * Checks whether enough feature point correspondences between (already existing) feature points from a feature map and feature points from a given image can be detected/determined.
		 * A homography is used to verify correspondence candidates.
		 * @param mapFeatures The already existing features from the feature map
		 * @param frame The frame in which new feature points will be detected, described an than used for matching
		 * @param randomGenerator Random number generator to be used
		 * @param minimalCorrespondences Minimal number of valid point correspondences to succeed, with range [4, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool checkCorrespondences(const FeaturePoints& mapFeatures, const Frame& frame, RandomGenerator& randomGenerator, unsigned int minimalCorrespondences = 10u, Worker* worker = nullptr);

		/**
		 * Determines representative features from and for several (counting) feature maps.
		 * Representative features have a high number of usage counts and do not interfere with features from other feature maps.
		 * @param countingFeatureMaps The (counting) feature maps for which the representative features will be determined
		 * @param representativeFeatureIndexGroups The resulting groups of indices of the representative features, one group for each feature map
		 * @param idealFeatureNumber Ideal number of representative feature points for each feature map, with range [5, infinity)
		 * @param uniformDistributed True, to determine feature points uniform distributed over the pattern; False, to take any features
		 * @param worker Optional worker to distribute the computation
		 */
		static void determineRepresentativeFeatures(CountingFeatureMaps& countingFeatureMaps, std::vector<Indices32>& representativeFeatureIndexGroups, const unsigned int idealFeatureNumber = 50u, const bool uniformDistributed = false, Worker* worker = nullptr);

		/**
		 * Determines reliable features from and for a (counting) feature map.
		 * Reliable features have a high number of usage counts.
		 * @param countingFeatureMap The (counting) feature map for which the reliable features will be determined
		 * @param reliableFeatureIndices The resulting indices of the reliable features
		 * @param idealFeatureNumber Ideal number of representative feature points for each feature map, with range [5, infinity)
		 * @param uniformDistributed True, to determine feature points uniform distributed over the pattern; False, to take any features
		 * @param worker Optional worker to distribute the computation
		 */
		void determineReliableFeatures(CountingFeatureMap& countingFeatureMap, Indices32& reliableFeatureIndices, const unsigned int idealFeatureNumber = 500u, const bool uniformDistributed = false, Worker* worker = nullptr);

	protected:

		/// The name of the pattern (the filename of the image, without prefix path).
		std::string mapPatternName;

		/// The pattern in which the features of this map are detected.
		Frame mapPatternFrame;

		/// The features of this feature map, all features must have been detected in the pattern frame.
		FeaturePoints mapFeatures;

		/// The number of frames which has been used to determine the reliability information, with range [0, infinity)
		unsigned int mapFrames;

		/// The map mapping ids of features to counts of valid assigned correspondences.
		FeatureId2CountMap mapFeatureId2Count;

		/// The map mapping ids of features to counts of wrong assigned correspondences.
		FeatureId2CountMap mapFeatureId2Ambiguous;

		/// The map mapping counts of valid assigned correspondences to feature ids.
		FeatureCount2IdMap mapFeatureCount2Ids;
};

inline CountingFeatureMap::CountingFeatureMap()
{
	// nothing to do here
};

inline CountingFeatureMap::CountingFeatureMap(const std::string& patternName, const Frame& patternFrame, FeaturePoints&& features) :
	mapPatternName(patternName),
	mapPatternFrame(patternFrame),
	mapFeatures(std::move(features)),
	mapFrames(0u)
{
	// nothing to do here
}

inline const std::string& CountingFeatureMap::patternName() const
{
	return mapPatternName;
}

inline const Frame& CountingFeatureMap::patternFrame() const
{
	return mapPatternFrame;
}

inline unsigned int CountingFeatureMap::frames() const
{
	return mapFrames;
}

inline unsigned int& CountingFeatureMap::frames()
{
	return mapFrames;
}

inline const FeaturePoints& CountingFeatureMap::features() const
{
	return mapFeatures;
}

inline const FeatureId2CountMap& CountingFeatureMap::featureId2Count() const
{
	return mapFeatureId2Count;
}

inline FeatureId2CountMap& CountingFeatureMap::featureId2Count()
{
	return mapFeatureId2Count;
}

inline const FeatureId2CountMap& CountingFeatureMap::featureId2Ambiguous() const
{
	return mapFeatureId2Ambiguous;
}

inline FeatureId2CountMap& CountingFeatureMap::featureId2Ambiguous()
{
	return mapFeatureId2Ambiguous;
}

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_PATTERNRECOGNITION_COUNTING_FEATURE_MAP_H
