/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestPointTracking.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/Motion.h"

#include "ocean/cv/detector/HarrisCornerDetector.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

bool TestPointTracking::test(const Frame& frame, const double testDuration, Worker& worker)
{
	ocean_assert(frame && testDuration > 0.0);
	ocean_assert(frame.width() >= 80u && frame.height() >= 80u);

	Log::info() << "---   Point tracking test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		if (CV::FrameConverter::Comfort::isSupported(frame.frameType(), FrameType::findPixelFormat(channels * 8u)))
		{
			if (channels > 1u)
			{
				Log::info() << " ";
				Log::info() << "-";
				Log::info() << " ";
			}

			allSucceeded = testMotion(frame, channels, testDuration, worker) && allSucceeded;
		}
	}

	Log::info() << " ";

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

bool TestPointTracking::testMotion(const Frame& frame, const unsigned int channels, const double testDuration, Worker& worker)
{
	ocean_assert(frame && channels >= 1u && testDuration > 0.0);

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, &worker))
	{
		return false;
	}

	Frame frame0;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::findPixelFormat(channels * 8u), FrameType::ORIGIN_UPPER_LEFT, frame0, CV::FrameConverter::CP_ALWAYS_COPY, &worker))
	{
		ocean_assert(false && "This must never happen!");
		return false;
	}

	Log::info() << "Testing pixel-accurate point tracking between two " << channels << " channel frames:";

	const SquareMatrix3 translationTransformation(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(Scalar(frame.width()) * Scalar(0.5), Scalar(frame.height()) * Scalar(0.5), 1));
	const SquareMatrix3 rotationTransformation(Rotation(0, 0, 1, Numeric::deg2rad(2)));
	const SquareMatrix3 frame0_H_frame1(translationTransformation * rotationTransformation * translationTransformation.inverted());

	Frame frame1(frame0.frameType());
	if (!CV::FrameInterpolatorBilinear::Comfort::homography(frame0, frame1, frame0_H_frame1, nullptr, &worker))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	CV::Detector::HarrisCorners harrisCorners;
	if (!CV::Detector::HarrisCornerDetector::detectCorners(yFrame, 0, true, harrisCorners, true))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	std::sort(harrisCorners.begin(), harrisCorners.end());

	// we want strong Harris corners not directly located at the boundary of the frame

	const Box2 boundingBox(Scalar(40), Scalar(40), Scalar(frame.width() - 40u), Scalar(frame.height() - 40u));

	Vectors2 points0;
	points0.reserve(1000);
	for (size_t n = 0; n < harrisCorners.size() && points0.size() < 1000; ++n)
	{
		if (boundingBox.isInside(harrisCorners[n].observation()))
		{
			points0.emplace_back(harrisCorners[n].observation());
		}
	}

	if (points0.empty())
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	constexpr unsigned int maximalOffset = 64u;
	constexpr unsigned int coarestLayerRadius = 2u;

	bool allSucceeded = true;

	for (const unsigned int patchSize : {7u, 15u, 31u})
	{
		Log::info() << " ";
		Log::info() << "... for " << String::insertCharacter(String::toAString(points0.size()), ',', 3, false) <<  " points with patch size " << patchSize << ":";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		const unsigned int maxWorkerIterations = worker ? 2u : 1u;

		double minPercent = NumericD::maxValue();

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				bool trackResult = false;

				const CV::PixelPositions positions0 = CV::PixelPosition::vectors2pixelPositions(points0);
				CV::PixelPositions positions1;

				performance.start();

				switch (patchSize)
				{
					case 7u:
						trackResult = CV::MotionSSD::trackPointsInPyramidMirroredBorder<7u>(frame0, frame1, positions0, positions0, positions1, maximalOffset, coarestLayerRadius, coarestLayerRadius, CV::FramePyramid::DM_FILTER_14641, useWorker);
						break;

					case 15u:
						trackResult = CV::MotionSSD::trackPointsInPyramidMirroredBorder<15u>(frame0, frame1, positions0, positions0, positions1, maximalOffset, coarestLayerRadius, coarestLayerRadius,CV::FramePyramid::DM_FILTER_14641, useWorker);
						break;

					case 31u:
						trackResult = CV::MotionSSD::trackPointsInPyramidMirroredBorder<31u>(frame0, frame1, positions0, positions0, positions1, maximalOffset, coarestLayerRadius, coarestLayerRadius,CV::FramePyramid::DM_FILTER_14641, useWorker);
						break;
				}

				performance.stop();

				if (trackResult)
				{
					const Vectors2 points1 = CV::PixelPosition::pixelPositions2vectors(positions1);

					const double percent = validateAccuracy(points0, points1, frame0_H_frame1);

					minPercent = std::min(minPercent, percent);
				}
				else
				{
					allSucceeded = false;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Performance: " << String::toAString(performanceSinglecore.averageMseconds()) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: " << String::toAString(performanceMulticore.averageMseconds()) << "ms";
			Log::info() << "Multicore boost factor: " << String::toAString(NumericD::ratio(performanceSinglecore.averageMseconds(), performanceMulticore.averageMseconds()), 1u) << "x";
		}

		if (minPercent < 0.90 || minPercent == NumericD::maxValue())
		{
			allSucceeded = false;

			Log::info() << "Validation: FAILED!";
		}
		else
		{
			Log::info() << "Validation: " << String::toAString(minPercent * 100.0, 1u) << "% succeeded.";
		}
	}

	return allSucceeded;
}

double TestPointTracking::validateAccuracy(const Vectors2& points0, const Vectors2& points1, const SquareMatrix3& frame0_H_frame1, const Scalar maxDistance)
{
	ocean_assert(points0.size() >= 1 && points0.size() == points1.size());
	ocean_assert(maxDistance >= 0);

	size_t validCorresponences = 0;

	for (size_t n = 0; n < points0.size(); ++n)
	{
		const Scalar distance = points0[n].distance(frame0_H_frame1 * points1[n]);

		if (distance <= maxDistance)
		{
			validCorresponences++;
		}
	}

	return double(validCorresponences) / double(points0.size());
}

}

}

}

}
