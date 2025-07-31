/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestPerformance.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Worker.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/OpenCVUtilities.h"

#include "ocean/math/Random.h"
#include "ocean/math/SquareMatrix3.h"

#include <opencv2/imgproc.hpp>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

bool TestPerformance::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	const std::string testName = "OpenCV performance test";
	Log::info() << "---   " << testName << ":   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	const std::vector<unsigned int> channels = {1u, 2u, 3u, 4u};
	const std::vector<std::pair<unsigned int, unsigned int>> sizes =  {{640u, 480u}, {1280u, 720u}, {1440u, 1080u}, {1920u, 1080u}};

	for (const unsigned int channel : channels)
	{
		for (const std::pair<unsigned int, unsigned int>& size : sizes)
		{
			allSucceeded = performancePerspectiveWarp(size.first, size.second, channel, testDuration);
			Log::info() << " ";

		}
	}

	if (allSucceeded)
		Log::info() << testName << " succeeded.";
	else
		Log::info() << testName << " FAILED!";

	return allSucceeded;

}

bool TestPerformance::performancePerspectiveWarp(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	Log::info() << "Warp perspective test for frame size: " << width << "x" << height << ", channels " << channels;

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels);

	const SquareMatrix3 transformation(Scalar(0.95), Scalar(-0.05), Scalar(0.05), Scalar(1.05), Scalar(1), Scalar(0), Scalar(35.8), Scalar(-20.4), Scalar(1));
	ocean_assert(!transformation.isSingular());
	const cv::Matx33d cvTransformationInv = CV::OpenCVUtilities::toCvMatx33(transformation.inverted()); // Invert here or add cv::WARP_INVERSE_MAP to cv::warpPerspective()

	HighPerformanceStatistic timerOcean, timerOpenCV;

	const Timestamp startTimestamp(true);

	do
	{
		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		Frame outputFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));

		cv::Mat cvFrame(height, width, CV_MAKETYPE(CV_8U, channels));
		cv::Mat cvOutputFrame(height, width, CV_MAKETYPE(CV_8U, channels));

		CV::OpenCVUtilities::toCvMat(frame).copyTo(cvFrame);

		timerOcean.start();

			switch (channels)
			{
				case 1u:
					CV::FrameInterpolatorBilinear::homography<uint8_t, 1u>(frame.constdata<uint8_t>(), width, height, transformation, nullptr, outputFrame.data<uint8_t>(), CV::PixelPositionI(0, 0), width, height, frame.paddingElements(), outputFrame.paddingElements(), WorkerPool::get().scopedWorker()());
					break;

				case 3u:
					CV::FrameInterpolatorBilinear::homography<uint8_t, 3u>(frame.constdata<uint8_t>(), width, height, transformation, nullptr, outputFrame.data<uint8_t>(), CV::PixelPositionI(0, 0), width, height, frame.paddingElements(), outputFrame.paddingElements(), WorkerPool::get().scopedWorker()());
					break;

				case 4u:
					CV::FrameInterpolatorBilinear::homography<uint8_t, 4u>(frame.constdata<uint8_t>(), width, height, transformation, nullptr, outputFrame.data<uint8_t>(), CV::PixelPositionI(0, 0), width, height, frame.paddingElements(), outputFrame.paddingElements(), WorkerPool::get().scopedWorker()());
					break;

				default:
					ocean_assert(false && "This should never happen!");
					return false;
			}

		timerOcean.stop();

		// Run OpenCV
		timerOpenCV.start();
			cv::warpPerspective(cvFrame, cvOutputFrame, cvTransformationInv, cvOutputFrame.size(), cv::INTER_LINEAR);
		timerOpenCV.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Ocean, best: " << String::toAString(timerOcean.bestMseconds(), 2u) + "ms, avg: " + String::toAString(timerOcean.averageMseconds(), 2u) << "ms, median: " << String::toAString(timerOcean.medianMseconds(), 2u) << "ms";
	Log::info() << "OpenCV, best: " << String::toAString(timerOpenCV.bestMseconds(), 2u) + "ms, avg: " + String::toAString(timerOpenCV.averageMseconds(), 2u) << "ms, median: " << String::toAString(timerOpenCV.medianMseconds(), 2u) << "ms";
	Log::info() << "Ratio best: " << String::toAString(timerOcean.bestMseconds() / timerOpenCV.bestMseconds(), 2u) + ", avg: " + String::toAString(timerOcean.averageMseconds() / timerOpenCV.averageMseconds(), 2u) << ", median: " << String::toAString(timerOcean.medianMseconds() / timerOpenCV.medianMseconds(), 2u);
	Log::info() << "Test duration: " << testDuration;

	return true;
}

}

}

}

}
