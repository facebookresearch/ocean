/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTTACKING_TEST_MATCHING_BLOB_H
#define META_OCEAN_TEST_TESTTACKING_TEST_MATCHING_BLOB_H

#include "ocean/test/testtracking/TestTracking.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/detector/blob/BlobFeature.h"

namespace Ocean
{

namespace Test
{

namespace TestTracking
{

/**
 * This class implements a test for matching blob features.
 * @ingroup testtracking
 */
class OCEAN_TEST_TRACKING_EXPORT TestMatchingBlob
{
	protected:

		/**
		 * Definition a map mapping forward indices to backward indices.
		 */
		typedef std::map<Index32, Index32> CorrespondenceMap;

	public:

		/**
		 * Starts all existing tests for blob matching.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration, Worker& worker);

		/**
		 * Tests the determination of bijective feature correspondences via brute force search.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetermineBijectiveFeaturesBruteForce(const double testDuration, Worker& worker);

		/**
		 * Tests the determination of bijective feature correspondences via brute force search.
		 * @param numberForwardFeatures The number of forward features, with range [1, infinity)
		 * @param numberBackwardFeatures The number of backward features, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetermineBijectiveFeaturesBruteForce(const unsigned int numberForwardFeatures, const unsigned int numberBackwardFeatures, const double testDuration, Worker& worker);

		/**
		 * Tests the determination of bijective feature correspondences via brute force search with quality.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetermineBijectiveFeaturesBruteForceWithQuality(const double testDuration, Worker& worker);

		/**
		 * Tests the determination of bijective feature correspondences via brute force search with quality.
		 * @param numberForwardFeatures The number of forward features, with range [1, infinity)
		 * @param numberBackwardFeatures The number of backward features, with range [1, infinity)
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDetermineBijectiveFeaturesBruteForceWithQuality(const unsigned int numberForwardFeatures, const unsigned int numberBackwardFeatures, const double testDuration, Worker& worker);

	protected:

		/**
		 * Normalizes a given descriptor.
		 * @param descriptor The descriptor to be normalized
		 */
		static void normalizeDescriptor(CV::Detector::Blob::BlobDescriptor& descriptor);

		/**
		 * Creates and sets a random descriptor.
		 * @param descriptor The descriptor to be set
		 * @param randomGenerator The random generator object to be used
		 */
		static void randomizeDescriptor(CV::Detector::Blob::BlobDescriptor& descriptor, RandomGenerator& randomGenerator);

		/**
		 * Creates a slightly randomized version of a already existing descriptor.
		 * @param sourceDescriptor The source descriptor which will be used as a source
		 * @param targetDescriptor the target descriptor which will be a slightly modified version of `sourceDescriptor`
		 * @param randomGenerator The random generator object to be used
		 * @param maxElementDelta The maximal elemente-wise delta between the source descriptor and the target descriptor
		 */
		static void slightlyModifiedDescriptor(const CV::Detector::Blob::BlobDescriptor& sourceDescriptor, CV::Detector::Blob::BlobDescriptor& targetDescriptor, RandomGenerator& randomGenerator, const CV::Detector::Blob::BlobDescriptor::DescriptorElement maxElementDelta);

		/**
		 * Creates a set of forward and backward correspondences.
		 * @param numberForwardFeatures The number of forward features, with range [1, infinity)
		 * @param numberBackwardFeatures The number of backward features, with range [1, infinity)
		 * @param randomGenerator Random generator object to be used
		 * @param forwardFeatures The resulting forward features
		 * @param backwardFeatures The resulting backward features
		 * @param correspondenceMap The resulting map of valid correspondences
		 */
		static void createCorrespondences(const unsigned int numberForwardFeatures, const unsigned int numberBackwardFeatures, RandomGenerator& randomGenerator, CV::Detector::Blob::BlobFeatures& forwardFeatures, CV::Detector::Blob::BlobFeatures& backwardFeatures, CorrespondenceMap& correspondenceMap);
};

}

}

}

#endif // META_OCEAN_TEST_TESTTACKING_TEST_MATCHING_BLOB_H
