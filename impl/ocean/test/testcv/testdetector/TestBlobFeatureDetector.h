// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_BLOB_FEATURE_DETECTOR_H
#define META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_BLOB_FEATURE_DETECTOR_H

#include "ocean/test/testcv/testdetector/TestCVDetector.h"

#include "ocean/cv/detector/blob/BlobFeatureDetector.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

/**
 * This class implements a Blob feature detector test.
 * @ingroup testcvdetector
 */
class OCEAN_TEST_CV_DETECTOR_EXPORT TestBlobFeatureDetector
{
	protected:

		/// Definition of a detection threshold for read images.
		static constexpr Scalar detectionThresholdStrong_ = Scalar(65);

		/// Definition of a detection threshold for random images.
		static constexpr Scalar detectionThresholdWeak_ = Scalar(10);

	public:

		/**
		 * Tests all Blob detector and descriptor functions.
		 * @param frame Test frame to be used for feature detection, must be valid with a dimension larger than 195x195
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool test(const Frame& frame, const double testDuration, Worker& worker);

		/**
		 * Tests the response maps.
		 * @param yFrame The original frame to filter, must be valid
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testResponseMaps(const Frame& yFrame, const double testDuration, Worker& worker);

		/**
		 * Tests a response map with specified filter index.
		 * @param filterIndex The index of the filter to test, possible values are {1, 2, 3, 4, 6, 8, 12, 16, 24, 32}
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param yFrameTest Optional explicit frame to be used for testing, otherwise a random image will be used
		 * @return True, if succeeded
		 */
		static bool testResponseMap(const unsigned int filterIndex, const double testDuration, Worker& worker, const Frame& yFrameTest = Frame());

		/**
		 * Tests the performance of feature detection using all response layers.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param yFrameTest Optional explicit frame to be used for testing, otherwise a random image will be used
		 * @return True, if succeeded
		 */
		static bool testFeatureDetectionPerformance(const double testDuration, Worker& worker, const Frame& yFrameTest = Frame());

		/**
		 * Tests the maximum suppression of all response layers.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param yFrameTest Optional explicit frame to be used for testing, otherwise a random image will be used
		 * @return True, if succeeded
		 */
		static bool testMaximumSuppression(const double testDuration, Worker& worker, const Frame& yFrameTest = Frame());

		/**
		 * Tests the performance of the orientation calculation of all given features.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param yFrameTest Optional explicit frame to be used for testing, otherwise a random image will be used
		 * @return True, if succeeded
		 */
		static bool testOrientationCalculationPerformance(const double testDuration, Worker& worker, const Frame& yFrameTest = Frame());

		/**
		 * Tests the descriptor calculation of all given features.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @param yFrameTest Optional explicit frame to be used for testing, otherwise a random image will be used
		 * @return True, if succeeded
		 */
		static bool testDescriptorCalculationPerformance(const double testDuration, Worker& worker, const Frame& yFrameTest = Frame());

		/**
		 * Tests the orientation and descriptor calculation of all given features.
		 * @param linedIntegral The lined integral image of the original frame, must be valid
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param testFeatures Features to be used for testing
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testOrientationAndDescriptorCalculation(const uint32_t* linedIntegral, const unsigned int width, const unsigned int height, const CV::Detector::Blob::BlobFeatures& testFeatures, const double testDuration, Worker& worker);

		/**
		 * Tests the matching performance for all given features.
		 * @param linedIntegral The lined integral image of the original frame, must be valid
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param testFeatures Features to be used for testing
		 * @param threshold Maximal distance threshold
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testDescriptorMatching(const uint32_t* linedIntegral, const unsigned int width, const unsigned int height, const CV::Detector::Blob::BlobFeatures& testFeatures, const Scalar threshold, const double testDuration, Worker& worker);

		/**
		 * Tests the performance of the SSD calculations.
		 * @param numberDescriptors The number of descriptors to be tested
		 * @param threshold Maximal distance threshold
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testSSDPerformance(const size_t numberDescriptors, const Scalar threshold, const double testDuration, Worker& worker);

		/**
		 * Tests the performance of the SSD calculations.
		 * @param numberDescriptors The number of descriptors to be tested
		 * @param threshold Maximal distance threshold
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 * @tparam TElement The data type of the descriptor's elements
		 * @tparam tElements The number of elements a descriptor has, possible values are 36 or 64
		 */
		template <typename TElement, unsigned int tElements>
		static bool testSSDPerformance(const size_t numberDescriptors, const Scalar threshold, const double testDuration, Worker& worker);

		/**
	     * Tests the determination of unidirectional correspondences for all given features.
		 * @param linedIntegral The lined integral image of the original frame, must be valid
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param testFeatures Features to be used for testing
		 * @param threshold Maximal distance threshold
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testUnidirectionalCorrespondences(const uint32_t* linedIntegral, const unsigned int width, const unsigned int height, const CV::Detector::Blob::BlobFeatures& testFeatures, const Scalar threshold, const double testDuration, Worker& worker);

		/**
		 * Tests the overall performance of the detection and description of all features.
		 * @param yFrame The frame which is used for the test, with pixel format FORMAT_Y8, must be valid
		 * @param threshold Minimum response threshold
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @param worker The worker object
		 * @return True, if succeeded
		 */
		static bool testOverallPerformance(const Frame& yFrame, const Scalar threshold, const double testDuration, Worker& worker);

		/**
		 * Tests the validation of a response map.
		 * @param yFrame The original frame to filter, must be valid
		 * @param responseMap Response map to validate
		 * @return True, if succeeded
		 */
		static bool validateResponseMap(const Frame& yFrame, const CV::Detector::Blob::BlobFeatureDetector::ResponseMap& responseMap);

		/**
		 * Tests the validation of the un-oriented descriptor calculation.
		 * @param frame Original frame to used for descriptor calculation
		 * @param width The width of the original frame in pixel
		 * @param height The height of the original frame in pixel
		 * @param feature Blob features to be checked
		 * @return True, if succeeded
		 */
		static bool validateDescriptor(const uint8_t* frame, const unsigned int width, const unsigned int height, const CV::Detector::Blob::BlobFeature& feature);

		/**
		 * Tests the validation of the maximum suppression of three response maps.
		 * @param low The low response map
		 * @param middle The middle response map
		 * @param high the high response map
		 * @param threshold Minimum response threshold
		 * @param features The features to validate
		 * @return True, if succeeded
		 */
		static bool validateMaximumSuppression(const CV::Detector::Blob::BlobFeatureDetector::ResponseMap& low, const CV::Detector::Blob::BlobFeatureDetector::ResponseMap& middle, const CV::Detector::Blob::BlobFeatureDetector::ResponseMap& high, const Scalar threshold, const CV::Detector::Blob::BlobFeatures& features);

		/**
		 * Performs the brute force matching for a subset of two sets of features.
		 * @param forwardFeatures The first set of features to be used for matching, must be valid
		 * @param backwardFeatures The second set of features to be used for matching, must be valid
		 * @param backwardSize The number of backward feature points, with range [1, infinity)
		 * @param normalizedThreshold The normalized threshold identifying valid correspondences, with range (0, infinity)
		 * @param bestMatchingBackwards The resulting indices of best matching candidates from the set of backward features, one for each forward feature
		 * @param bestSSDs The resulting best ssd which has been calculated for each valid match
		 * @param firstForward The first forward feature to be handled, with range [0, infinity)
		 * @param numberForward The number of forward features to be handled, with range [1, infinity)
		 * @tparam tEarlyReject True, to apply the early reject SSD strategy; False, to apply a normal SSD strategy
		 */
		template <bool tEarlyReject>
		static void matchDescriptorsSubset(const CV::Detector::Blob::BlobFeature* forwardFeatures, const CV::Detector::Blob::BlobFeature* backwardFeatures, const size_t backwardSize, const CV::Detector::Blob::BlobDescriptor::DescriptorSSD normalizedThreshold, uint32_t* bestMatchingBackwards, CV::Detector::Blob::BlobDescriptor::DescriptorSSD* bestSSDs, const unsigned int firstForward, const unsigned int numberForward);

		/**
		 * Performs the brute force SSD calculation for two sets of feature descriptors.
		 * @param forwardDescriptors The first set of descriptors, must be valid
		 * @param backwardDescriptors The second set of descriptors, must be valid
		 * @param backwardSize The number of backward descriptors, with range [1, infinity)
		 * @param normalizedThreshold The normalized threshold identifying valid correspondences, with range (0, infinity)
		 * @param bestMatchingBackwards The resulting indices of best matching candidates from the set of backward features, one for each forward feature
		 * @param bestSSDs The resulting best SSD which has been calculated for each valid match
		 * @param firstForward The first forward descriptor to be handled, with range [0, infinity)
		 * @param numberForward The number of forward descriptor to be handled, with range [1, infinity)
		 * @tparam TElement The data type of the descriptor's elements
		 * @tparam tElements The number of elements a descriptor has, possible values are 36 or 64
		 * @tparam tUseSIMD True, to apply SIMD instructions for the matching; False, to avoid the explicit usage of SIMD instructions
		 * @tparam tEarlyReject True, to apply the early reject SSD strategy; False, to apply a normal SSD strategy
		 */
		template <typename TElement, unsigned int tElements, bool tUseSIMD, bool tEarlyReject>
		static void matchDescriptorsSubset(const TElement* forwardDescriptors, const TElement* backwardDescriptors, const size_t backwardSize, const typename SquareValueTyper<TElement>::Type normalizedThreshold, uint32_t* bestMatchingBackwards, typename SquareValueTyper<TElement>::Type* bestSSDs, const unsigned int firstForward, const unsigned int numberForward);

		/**
		 * Calculates the summed squared differences between two given vectors.
		 * @param first The vector storing the first elements
		 * @param second The vector storing the second elements
		 * @param size The number of elements, with range [0, infinity)
		 * @return The summed squared differences
		 * @tparam T The data type of each element
		 */
		template <typename T>
		static T summedSquaredDifferences(const T* first, const T* second, const size_t size);
};

template <typename T>
T TestBlobFeatureDetector::summedSquaredDifferences(const T* first, const T* second, const size_t size)
{
	if (size == 0)
	{
		return T(0);
	}

	T result = (first[0] - second[0]) * (first[0] - second[0]);

	for (size_t n = 1; n < size; ++n)
	{
		result += (first[n] - second[n]) * (first[n] - second[n]);
	}

	return result;
}

}

}

}

}

#endif // META_OCEAN_TEST_TESTCV_TESTDETECTOR_TEST_BLOB_FEATURE_DETECTOR_H
