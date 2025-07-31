/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestFeatureDetectors.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Messenger.h"

#include <opencv2/core.hpp>
#include <opencv2/core/version.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

//#define DEBUG_OUTPUT

namespace
{

/// Typedef for Harris corners
typedef std::vector<cv::Point2f> HarrisCorners;

/// Typedef for FAST keypoints
typedef std::vector<cv::KeyPoint> KeyPoints;

/**
 * Helper (base) class for the determination of detector thresholds
 *
 * @sa  class Guesstimator
 */
struct Guesstimator
{
	virtual ~Guesstimator() = default;

	/**
	 * @brief Estimate a threshold value for the detectors
	 *
	 * This function finds a threshold value which (approx.) produces the specified
	 * number of Harris corners using bisection.
	 *
	 * @param guesstimator    An instance of a specific functor, e.g. Harris or FAST
	 * @param src             The input image for which the threshold will be estimated
	 * @param keypointsCount  Number of desired keypoints that will be extracted using the estimated threshold (+/- 5%)
	 * @param min_threshold   Minimum of the search range (default: 0)
	 * @param max_threshold   Maximum of the search range (default: 1e4)
	 * @param max_iterations  Maximum number of bisections before the search is aborted (default: 200)
	 *
	 * @return The estimated threshold
	 */
	double estimate(const cv::Mat1b& grayFrame, size_t keypointsCount, double min_threshold = 0., double max_threshold = 1e4, size_t max_iterations = 200)
	{
		const size_t max_difference = static_cast<size_t>(static_cast<double>(keypointsCount) * 0.05);
		ocean_assert(min_threshold >= 0 && min_threshold < max_threshold);
		double threshold         = (min_threshold + max_threshold) / 2.;

		for (size_t i = 0; i < max_iterations; ++i)
		{
			const size_t actualKeypointsCount = determineKeypointsCount(grayFrame, threshold);
			const size_t diff                 = std::max(actualKeypointsCount, keypointsCount) - std::min(actualKeypointsCount, keypointsCount);

			if (diff <= max_difference)
			{
				break;
			}
			else if (actualKeypointsCount < keypointsCount)
			{
				max_threshold = threshold;
				threshold     = (min_threshold + threshold) * 0.5;
			}
			else
			{
				min_threshold = threshold;
				threshold     = (max_threshold + threshold) * 0.5;
			}
		}

		return threshold;
	}

protected:
	/**
	 * Function to copmute the number of extracted keypoints.
	 *
	 * @param grayFrame  The input frame in which feature points are detected
	 * @param threshold  The value to be used as threshold for feature candidates
	 *
	 * @return The number of extracted feature points/corners.
	 */
	virtual size_t determineKeypointsCount(const cv::Mat1b& grayFrame, double threshold) const = 0;
};

/**
 * Guesstimator class for the OpenCV Harris corner detector
 *
 * @sa  class Guesstimator
 */
struct HarrisGuesstimator : public Guesstimator
{
	/**
	 * Default constructor
	 *
	 * @param maxCorners   Maximum number of extracted corners
	 * @param minDistance  Minimum distance of corners
	 */
	HarrisGuesstimator(int maxCorners = Ocean::NumericT<int>::maxValue(), double minDistance = 0.)
	: maxCorners(maxCorners)
	, minDistance(minDistance)
	{
		// Nothing to do.
	}

protected:
	/**
	 * Function to copmute the number of extracted keypoints.
	 *
	 * @param grayFrame  The input frame in which feature points are detected
	 * @param threshold  The value to be used as threshold for feature candidates
	 *
	 * @return The number of extracted Harris corners.
	 */
	size_t determineKeypointsCount(const cv::Mat1b& grayFrame, double threshold) const override
	{
		ocean_assert(!grayFrame.empty());
		HarrisCorners corners;

		cv::goodFeaturesToTrack(grayFrame, corners, maxCorners, threshold, minDistance, cv::noArray(), 3, true);

		return corners.size();
	};

protected:
	const int maxCorners;
	const double minDistance;
};

/**
 * Guesstimator class for the OpenCV FAST feature detector
 *
 * @sa  class Guesstimator
 */
struct FASTGuesstimator : public Guesstimator
{
	/**
	 * Default constructor
	 */
	FASTGuesstimator(bool useNonMaximumSuppression = true)
	: useNonMaximumSuppression(useNonMaximumSuppression)
	{
		// Nothing to do.
	}

protected:
	/**
	 * Function to copmute the number of extracted keypoints.
	 *
	 * @param grayFrame  The input frame in which feature points are detected
	 * @param threshold  The value to be used as threshold for feature candidates
	 *
	 * @return The number of extracted FAST feature points.
	 */
	size_t determineKeypointsCount(const cv::Mat1b& grayFrame, double threshold) const override
	{
		KeyPoints keypoints;

		cv::FAST(grayFrame, keypoints, static_cast<int>(threshold), useNonMaximumSuppression);

		return keypoints.size();
	};

protected:
	const bool useNonMaximumSuppression;
};

/**
 * Guesstimator class for the OpenCV SURF feature detector
 *
 * @sa  class Guesstimator
 */
struct SURFGuesstimator : public Guesstimator
{
protected:
	/**
	 * Function to copmute the number of extracted keypoints.
	 *
	 * @param grayFrame  The input frame in which feature points are detected
	 * @param threshold  The value to be used as threshold for feature candidates
	 *
	 * @return The number of extracted FAST feature points.
	 */
	size_t determineKeypointsCount(const cv::Mat1b& grayFrame, double threshold) const override
	{
		cv::Ptr<cv::xfeatures2d::SurfFeatureDetector> surf = cv::xfeatures2d::SurfFeatureDetector::create(threshold);
		KeyPoints keypoints;

		surf->detect(grayFrame, keypoints);

		return keypoints.size();
	}
};

} // namespace

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

bool TestFeatureDetectors::testFeatureDetectors(const cv::Mat& frame, const double testDuration)
{
	ocean_assert(!frame.empty());
	ocean_assert(testDuration > 0.0);

	Log::info() << "--- OpenCV feature detectors test ---";
	Log::info() << "  ";

	bool allSucceeded = true;

	allSucceeded = testHarrisCornerDetector(frame, testDuration, 500) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testHarrisCornerDetector(frame, testDuration, 1000) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testFASTFeatureDectector(frame, testDuration, 500) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testFASTFeatureDectector(frame, testDuration, 1000) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSURFFeatureDectector(frame, testDuration, 500) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testSURFFeatureDectector(frame, testDuration, 1000) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "OpenCV feature detectors succeeded.";
	else
		Log::info() << "OpenCV feature detectors FAILED!";

	return allSucceeded;
}

bool TestFeatureDetectors::testHarrisCornerDetector(const cv::Mat& frame, const double testDuration, size_t cornersCount)
{
	ocean_assert(!frame.empty());
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing Harris corner detector with approx. " << cornersCount << " feature points:";

	cv::Mat1b grayFrame;
	cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

	const int maxCorners     = Ocean::NumericT<int>::maxValue(); // NOTE NP Don't enforce a hard cut-off limit here, in order to find a true threshold that produces the desired number of corners.
	const double minDistance = 0.;                              // NOTE NP Can't find a min. distance in void HarrisCornerDetector::detectCornerCandidatesSubset() so setting this value to zero.
	const double threshold   = HarrisGuesstimator(maxCorners, minDistance).estimate(grayFrame, cornersCount);
	Log::info() << "Using threshold: " << threshold;
	Log::info() << "Min. distance between corners: " << minDistance;

	HarrisCorners corners;

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		corners.clear();

		HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

		cv::goodFeaturesToTrack(grayFrame, corners, static_cast<int>(cornersCount), threshold, minDistance, cv::noArray(), 3, true);

	} while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Actually detected: " << corners.size() << " feature points";
	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << "ms";

#ifdef DEBUG_OUTPUT
	cv::Mat result = frame.clone();

	for (size_t i = 0; i < corners.size(); ++i)
		cv::circle(result, cv::Point(corners[i]), 5, cv::Scalar(0), 2, 8, 0);

	cv::namedWindow("Harris corners", cv::WINDOW_AUTOSIZE);
	cv::imshow("Harris corners", result);
	cv::waitKey(0);
#endif // DEBUG_OUTPUT

	return true;
}

bool TestFeatureDetectors::testFASTFeatureDectector(const cv::Mat& frame, const double testDuration, size_t cornersCount)
{
	ocean_assert(!frame.empty());
	ocean_assert(testDuration > 0.);

	Log::info() << "Testing FAST feature detector with approx. " << cornersCount << " feature points:";

	cv::Mat1b grayFrame;
	cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

	KeyPoints keypoints;
	const bool useNonMaximumSuppression = true;
	const int threshold                 = static_cast<int>(FASTGuesstimator(useNonMaximumSuppression).estimate(grayFrame, cornersCount));
	Log::info() << "Using threshold: " << threshold;
	Log::info() << "Non-maximum suppression: " << (useNonMaximumSuppression ? "enabled" : "disabled");

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	do
	{
		keypoints.clear();

		HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

		cv::FAST(grayFrame, keypoints, threshold, useNonMaximumSuppression);
	} while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Actually detected: " << keypoints.size() << " feature points";
	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << "ms";

#ifdef DEBUG_OUTPUT
	cv::Mat result = frame.clone();

	cv::drawKeypoints(frame, keypoints, result);

	cv::namedWindow("FAST keypoints", cv::WINDOW_AUTOSIZE);
	cv::imshow("FAST keypoints", result);
	cv::waitKey(0);
#endif // DEBUG_OUTPUT

	return true;
}

bool TestFeatureDetectors::testSURFFeatureDectector(const cv::Mat& frame, const double testDuration, size_t cornersCount, bool computeDescriptors)
{
	ocean_assert(!frame.empty());
	ocean_assert(testDuration > 0.);

	Log::info() << "Testing SURF feature detector with approx. " << cornersCount << " feature points:";

	cv::Mat1b grayFrame;
	cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

	KeyPoints keypoints;
	cv::Mat descriptors;
	const int threshold = static_cast<int>(SURFGuesstimator().estimate(grayFrame, cornersCount));
	Log::info() << "Using threshold: " << threshold;
	Log::info() << "Computation of descriptors: " << (computeDescriptors ? "enabled" : "DISABLED");

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);

	if (computeDescriptors)
	{
		do
		{
			keypoints.clear();
			descriptors.release();

			cv::Ptr<cv::xfeatures2d::SurfFeatureDetector> surf = cv::xfeatures2d::SurfFeatureDetector::create(threshold);

			HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

			surf->detectAndCompute(grayFrame, cv::noArray(), keypoints, descriptors);
			ocean_assert(size_t(descriptors.size().height) == keypoints.size());
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}
	else
	{
		do
		{
			keypoints.clear();
			cv::Ptr<cv::xfeatures2d::SurfFeatureDetector> surf = cv::xfeatures2d::SurfFeatureDetector::create(threshold);

			HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

			surf->detect(grayFrame, keypoints);
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Actually detected: " << keypoints.size() << " feature points";
	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << "ms";
	Log::info() << "Length of descriptor: " << descriptors.size().width;

#ifdef DEBUG_OUTPUT
	cv::Mat result = frame.clone();

	cv::drawKeypoints(frame, keypoints, result);

	cv::namedWindow("SURF keypoints", cv::WINDOW_AUTOSIZE);
	cv::imshow("SURF keypoints", result);
	cv::waitKey(0);
#endif // DEBUG_OUTPUT

	return true;
}

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean
