/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestPointTracking.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"
#include "ocean/math/Numeric.h"

#include <opencv2/core/version.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

#include <vector>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

bool TestPointTracking::testPointTracking(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Point tracking test with random images:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	const Indices32 widths = {640u, 800u, 1280u, 1920u};
	const Indices32 heights = {480u, 640u, 720u, 1080u};
	ocean_assert(widths.size() == heights.size());

	for (const unsigned int window : {7u, 15u, 31u})
	{
		for (unsigned int n = 0u; n < widths.size(); ++n)
		{
			allSucceeded = testSparseOpticalFlow(widths[n], heights[n], window, testDuration) && allSucceeded;

			Log::info() << " ";
		}
	}

	if (allSucceeded)
	{
		Log::info() << "Point tracking test succeeded.";
	}
	else
	{
		Log::info() << "Point tracking test FAILED!";
	}

	return allSucceeded;

}

bool TestPointTracking::testPointTracking(const cv::Mat & frame, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Point tracking test with provided image:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	for (const unsigned int window : { 7u, 15u, 31u })
	{
		allSucceeded = testSparseOpticalFlow(frame, window, testDuration) && allSucceeded;

		Log::info() << " ";
	}

	if (allSucceeded)
	{
		Log::info() << "Point tracking test succeeded.";
	}
	else
	{
		Log::info() << "Point tracking test FAILED!";
	}

	return allSucceeded;
}

bool TestPointTracking::testSparseOpticalFlow(int width, int height, int window, const double testDuration)
{
	ocean_assert(width > 0);
	ocean_assert(height > 0);
	ocean_assert(testDuration > 0.0);

	cv::Size size(width, height);

	// get random images
	cv::Mat1b prevImg(size);
	cv::Mat1b nextImg(size);
	cv::randu(prevImg, 0, 255);
	cv::randu(nextImg, 0, 255);

	Log::info() << "Sparse optical flow test for frame size: " << width << "x" << height << ", channels " << prevImg.channels() << ", depth " << prevImg.depth() << ", window " << window << ":";

	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);
	do
	{
		// get random points
		std::vector<cv::Point2f> prevPoints;
		cv::RNG rng;
		cv::Point2f point;
		int P = 1000;
		for (int p = 0; p < P; ++p)
		{
			point.x = static_cast<float>(rng.uniform(0, size.width));
			point.y = static_cast<float>(rng.uniform(0, size.height));
			prevPoints.push_back(point);
		}

		// track points
		std::vector<cv::Point2f> nextPoints;
		std::vector<uchar> status;
		std::vector<float> error;
		cv::Size winSize(window, window);
		int maxLevel = 3;
		cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::COUNT+cv::TermCriteria::EPS, 1000000, 0.0);
		int flags = 0; // cv::OPTFLOW_LK_GET_MIN_EIGENVALS;
		double minEigThreshold = 0; //1e-4;
		{
			HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);
			cv::calcOpticalFlowPyrLK(prevImg, nextImg, prevPoints, nextPoints, status, error, winSize, maxLevel, criteria, flags, minEigThreshold);
		}

#ifdef OCEAN_DEBUG
		for (int p = 0; p < P; ++p)
			cv::circle(prevImg, prevPoints[p], 3, cv::Scalar(255), -1);
		cv::imshow("Input", prevImg);

		for (int p = 0; p < P; ++p)
			cv::circle(nextImg, nextPoints[p], 3, cv::Scalar(255), -1);
		cv::imshow("Output", nextImg);

		cv::waitKey();
		cv::destroyAllWindows();
#endif
	} while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << "ms";

	return true;
}

bool TestPointTracking::testSparseOpticalFlow(const cv::Mat & frame, int window, const double testDuration)
{

	// generate target frame by rotation around center
	cv::Size size = frame.size();
	cv::Mat frame0 = frame.clone();
	unsigned int w = size.width;
	unsigned int h = size.height;
	double angle = Ocean::Numeric::deg2rad(2);
	double a = Ocean::Numeric::cos(angle);
	double b = Ocean::Numeric::sin(angle);
	double x = (1 - a) * w / 2 - b * h / 2;
	double y = b * w / 2 - (1 - a) * h / 2;
	cv::Mat transformation = (cv::Mat_<double>(2, 3) << a, b, x, -b, a, y);
	cv::Mat frame1;
	cv::warpAffine(frame0, frame1, transformation, size);

	// convert to grayscale
	cv::Mat1b gray0;
	cv::cvtColor(frame0, gray0, cv::COLOR_BGR2GRAY);
	cv::Mat1b gray1;
	cv::cvtColor(frame1, gray1, cv::COLOR_BGR2GRAY);
	Log::info() << "Sparse optical flow test for frame size: " << size.width << "x" << size.height << ", channels " << gray0.channels() << ", depth " << gray0.depth() << ", window " << window << ":";

	// detect points
	std::vector<cv::Point2f> points0;
	int maxCorners = 1000;
	double qualityLevel = 0.00001;
	double minDistance = 0.0;
	cv::goodFeaturesToTrack(gray0, points0, maxCorners, qualityLevel, minDistance);
	size_t P = points0.size();
	Log::info() << "Number of points = " << P;

	// track points
	std::vector<cv::Point2f> points1;
	std::vector<uchar> status;
	std::vector<float> error;
	cv::Size winSize(window, window);
	int maxLevel = 3;
	cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, 1000000, 0.0);
	int flags = 0; //cv::OPTFLOW_LK_GET_MIN_EIGENVALS;
	double minEigThreshold = 0; // 1e-4;
	HighPerformanceStatistic performance;
	const Timestamp startTimestamp(true);
	do
	{
		{
			HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);
			cv::calcOpticalFlowPyrLK(gray0, gray1, points0, points1, status, error, winSize, maxLevel, criteria, flags, minEigThreshold);
		}
	} while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance: " << String::toAString(performance.averageMseconds()) << "ms";

	// validate points
	double maxDist = 1.0;
	std::vector<cv::Point2f> points2;
	cv::transform(points0, points2, transformation);
	double dist = 0;
	double avgDist = 0;
	double matchRatio = 0;
	std::vector<bool> matches(P);
	for (size_t p = 0; p < P; p++)
	{
		dist = cv::norm(points2[p] - points1[p]);
		avgDist += dist;
		if (dist < maxDist)
		{
			matches[p] = true;
			matchRatio++;
		}
	}
	avgDist /= P;
	matchRatio /= P;
	Log::info() << "avgDist = " << avgDist;
	Log::info() << "matchRatio = " << matchRatio;

#ifdef OCEAN_DEBUG
	for (size_t p = 0; p < P; ++p) {
		cv::circle(frame1, points2[p], 3, cv::Scalar(255, 0, 0), -1);
		if (matches[p]) {
			cv::circle(frame0, points0[p], 3, cv::Scalar(0, 255, 0), -1);
			cv::circle(frame1, points1[p], 3, cv::Scalar(0, 255, 0), -1);
		}
		else {
			cv::circle(frame0, points0[p], 3, cv::Scalar(0, 0, 255), -1);
			cv::circle(frame1, points1[p], 3, cv::Scalar(0, 0, 255), -1);
		}
	}
	cv::imshow("Previous", frame0);
	cv::imshow("Next", frame1);

	cv::waitKey();
	cv::destroyAllWindows();
#endif

	if (matchRatio == 1.0)
		return true;
	else
		return false;
}

bool TestPointTracking::testTemplateMatching(const cv::Mat & frame)
{
	cv::Size size = frame.size();

	// get random position
	cv::RNG rng;
	cv::Point position;
	position.x = rng.uniform(0, size.width);
	position.y = rng.uniform(0, size.height);

	// define patch
	cv::Size patchSize(20, 20);
	cv::Rect rect(position, patchSize);
	cv::Scalar color(0, 255, 0);
	cv::Mat patch = frame(rect);

#ifdef OCEAN_DEBUG
	cv::Mat input = frame.clone();
	cv::circle(input, position, 5, cv::Scalar(255, 0, 0), -1);
	cv::rectangle(input, rect, cv::Scalar(255, 0, 0));
	cv::imshow("Input", input);
#endif

	// match template
	cv::Mat dist;
	cv::matchTemplate(frame, patch, dist, cv::TM_SQDIFF);

#ifdef OCEAN_DEBUG
	cv::Mat dist1;
	cv::normalize(dist, dist1, 1, 0, cv::NORM_MINMAX);
	cv::imshow("Dist", dist1);
#endif

	// find best match
	double minVal, maxVal;
	cv::Point minLoc, maxLoc;
	cv::minMaxLoc(dist, &minVal, &maxVal, &minLoc, &maxLoc);

#ifdef OCEAN_DEBUG
	cv::Mat output = frame.clone();
	cv::circle(output, position, 5, cv::Scalar(0, 255, 0), -1);
	cv::rectangle(output, rect, cv::Scalar(0, 255, 0));
	cv::imshow("Output", output);
	cv::waitKey();
	cv::destroyAllWindows();
#endif

	return false;
}

}

}

}

}
