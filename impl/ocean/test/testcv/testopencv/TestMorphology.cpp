/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testopencv/TestMorphology.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterDilation.h"
#include "ocean/cv/FrameFilterErosion.h"
#include "ocean/cv/OpenCVUtilities.h"

#include <opencv2/imgproc.hpp>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestOpenCV
{

bool TestMorphology::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Morphology test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testFrameFilterErosion3x3(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFrameFilterErosion5x5(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFrameFilterDilation3x3(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFrameFilterDilation5x5(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFrameFilterOpen3x3(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFrameFilterOpen5x5(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFrameFilterClose3x3(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testFrameFilterClose5x5(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Morphology test succeeded.";
	}
	else
	{
		Log::info() << "Morphology test FAILED!.";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestMorphology, FrameFilterErosion3x3)
{
	EXPECT_TRUE(TestMorphology::testFrameFilterErosion3x3(GTEST_TEST_DURATION));
}

TEST(TestMorphology, FrameFilterErosion5x5)
{
	EXPECT_TRUE(TestMorphology::testFrameFilterErosion5x5(GTEST_TEST_DURATION));
}

TEST(TestMorphology, FrameFilterDilation3x3)
{
	EXPECT_TRUE(TestMorphology::testFrameFilterDilation3x3(GTEST_TEST_DURATION));
}

TEST(TestMorphology, FrameFilterDilation5x5)
{
	EXPECT_TRUE(TestMorphology::testFrameFilterDilation5x5(GTEST_TEST_DURATION));
}

TEST(TestMorphology, FrameFilterOpen3x3)
{
	EXPECT_TRUE(TestMorphology::testFrameFilterOpen3x3(GTEST_TEST_DURATION));
}

TEST(TestMorphology, FrameFilterOpen5x5)
{
	EXPECT_TRUE(TestMorphology::testFrameFilterOpen5x5(GTEST_TEST_DURATION));
}

TEST(TestMorphology, FrameFilterClose3x3)
{
	EXPECT_TRUE(TestMorphology::testFrameFilterClose3x3(GTEST_TEST_DURATION));
}

TEST(TestMorphology, FrameFilterClose5x5)
{
	EXPECT_TRUE(TestMorphology::testFrameFilterClose5x5(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestMorphology::testFrameFilterErosion3x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing erosion with 3x3 kernel:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		// testing random image resolutions

		const unsigned int width = RandomI::random(4u, 1920u);
		const unsigned int height = RandomI::random(4u, 1080u);

		Frame ocnMask = CV::CVUtilities::randomizedBinaryMask(width, height, 0x00, 0u);
		cv::Mat cvMask = CV::OpenCVUtilities::toCvMat(ocnMask, true);

		CV::FrameFilterErosion::filter1Channel8Bit<CV::FrameFilterErosion::MF_SQUARE_3>(ocnMask.data<uint8_t>(), ocnMask.width(), ocnMask.height(), 1u, 0xFF, ocnMask.paddingElements());

		cv::Mat cvFilterKernel = cv::getStructuringElement(cv::MORPH_RECT, {3, 3});
		cv::erode(cvMask, cvMask, cvFilterKernel);

		for (unsigned int y = 0u; y < height; ++y)
		{
			if (memcmp(ocnMask.constrow<void>(y), cvMask.ptr(y), ocnMask.planeWidthBytes(0u)) != 0)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestMorphology::testFrameFilterErosion5x5(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing erosion with 5x5 kernel:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(4u, 1920u);
		const unsigned int height = RandomI::random(4u, 1080u);

		Frame ocnMask = CV::CVUtilities::randomizedBinaryMask(width, height, 0x00, 0u);
		cv::Mat cvMask = CV::OpenCVUtilities::toCvMat(ocnMask, true);

		CV::FrameFilterErosion::filter1Channel8Bit<CV::FrameFilterErosion::MF_SQUARE_5>(ocnMask.data<uint8_t>(), ocnMask.width(), ocnMask.height(), 1u, 0xFF, ocnMask.paddingElements());

		cv::Mat cvFilterKernel = cv::getStructuringElement(cv::MORPH_RECT, {5, 5});
		cv::erode(cvMask, cvMask, cvFilterKernel);

		for (unsigned int y = 0u; y < height; ++y)
		{
			if (memcmp(ocnMask.constrow<void>(y), cvMask.ptr(y), ocnMask.planeWidthBytes(0u)) != 0)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestMorphology::testFrameFilterDilation3x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing dilation with 3x3 kernel:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(4u, 1920u);
		const unsigned int height = RandomI::random(4u, 1080u);

		Frame ocnMask = CV::CVUtilities::randomizedBinaryMask(width, height, 0x00, 0u);
		cv::Mat cvMask = CV::OpenCVUtilities::toCvMat(ocnMask, true);

		CV::FrameFilterDilation::filter1Channel8Bit<CV::FrameFilterErosion::MF_SQUARE_3>(ocnMask.data<uint8_t>(), ocnMask.width(), ocnMask.height(), 1u, 0xFF, ocnMask.paddingElements());

		cv::Mat cvFilterKernel = cv::getStructuringElement(cv::MORPH_RECT, {3, 3});
		cv::dilate(cvMask, cvMask, cvFilterKernel);

		for (unsigned int y = 0u; y < height; ++y)
		{
			if (memcmp(ocnMask.constrow<void>(y), cvMask.ptr(y), ocnMask.planeWidthBytes(0u)) != 0)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestMorphology::testFrameFilterDilation5x5(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing dilation with 5x5 kernel:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(4u, 1920u);
		const unsigned int height = RandomI::random(4u, 1080u);

		Frame ocnMask = CV::CVUtilities::randomizedBinaryMask(width, height, 0x00, 0u);
		cv::Mat cvMask = CV::OpenCVUtilities::toCvMat(ocnMask, true);

		CV::FrameFilterDilation::filter1Channel8Bit<CV::FrameFilterErosion::MF_SQUARE_5>(ocnMask.data<uint8_t>(), ocnMask.width(), ocnMask.height(), 1u, 0xFF, ocnMask.paddingElements());

		cv::Mat cvFilterKernel = cv::getStructuringElement(cv::MORPH_RECT, {5, 5});
		cv::dilate(cvMask, cvMask, cvFilterKernel);

		for (unsigned int y = 0u; y < height; ++y)
		{
			if (memcmp(ocnMask.constrow<void>(y), cvMask.ptr(y), ocnMask.planeWidthBytes(0u)) != 0)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestMorphology::testFrameFilterOpen3x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing opening with 3x3 kernel:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(4u, 1920u);
		const unsigned int height = RandomI::random(4u, 1080u);

		Frame ocnMask = CV::CVUtilities::randomizedBinaryMask(width, height, 0x00u);
		cv::Mat cvMask = CV::OpenCVUtilities::toCvMat(ocnMask, true);

		CV::FrameFilterMorphology::openMask<CV::FrameFilterMorphology::MF_SQUARE_3>(ocnMask.data<uint8_t>(), ocnMask.width(), ocnMask.height(), ocnMask.paddingElements(), 0xFFu);

		cv::Mat cvFilterKernel = cv::getStructuringElement(cv::MORPH_RECT, {3, 3});
		cv::morphologyEx(cvMask, cvMask, cv::MORPH_OPEN, cvFilterKernel);

		for (unsigned int y = 0u; y < height; ++y)
		{
			if (memcmp(ocnMask.constrow<void>(y), cvMask.ptr(y), ocnMask.planeWidthBytes(0u)) != 0)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestMorphology::testFrameFilterOpen5x5(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing opening with 5x5 kernel:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(4u, 1920u);
		const unsigned int height = RandomI::random(4u, 1080u);

		Frame ocnMask = CV::CVUtilities::randomizedBinaryMask(width, height, 0x00u);
		cv::Mat cvMask = CV::OpenCVUtilities::toCvMat(ocnMask, true);

		CV::FrameFilterMorphology::openMask<CV::FrameFilterMorphology::MF_SQUARE_5>(ocnMask.data<uint8_t>(), ocnMask.width(), ocnMask.height(), ocnMask.paddingElements(), 0xFFu);

		cv::Mat cvFilterKernel = cv::getStructuringElement(cv::MORPH_RECT, {5, 5});
		cv::morphologyEx(cvMask, cvMask, cv::MORPH_OPEN, cvFilterKernel);

		for (unsigned int y = 0u; y < height; ++y)
		{
			if (memcmp(ocnMask.constrow<void>(y), cvMask.ptr(y), ocnMask.planeWidthBytes(0u)) != 0)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestMorphology::testFrameFilterClose3x3(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing closing with 3x3 kernel:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(4u, 1920u);
		const unsigned int height = RandomI::random(4u, 1080u);

		Frame ocnMask = CV::CVUtilities::randomizedBinaryMask(width, height, 0x00u);
		cv::Mat cvMask = CV::OpenCVUtilities::toCvMat(ocnMask, true);

		CV::FrameFilterMorphology::closeMask<CV::FrameFilterMorphology::MF_SQUARE_3>(ocnMask.data<uint8_t>(), ocnMask.width(), ocnMask.height(), ocnMask.paddingElements(), 0xFFu);

		cv::Mat cvFilterKernel = cv::getStructuringElement(cv::MORPH_RECT, {3, 3});
		cv::morphologyEx(cvMask, cvMask, cv::MORPH_CLOSE, cvFilterKernel);

		for (unsigned int y = 0u; y < height; ++y)
		{
			if (memcmp(ocnMask.constrow<void>(y), cvMask.ptr(y), ocnMask.planeWidthBytes(0u)) != 0)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestMorphology::testFrameFilterClose5x5(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing closing with 5x5 kernel:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(4u, 1920u);
		const unsigned int height = RandomI::random(4u, 1080u);

		Frame ocnMask = CV::CVUtilities::randomizedBinaryMask(width, height, 0x00u);
		cv::Mat cvMask = CV::OpenCVUtilities::toCvMat(ocnMask, true);

		CV::FrameFilterMorphology::closeMask<CV::FrameFilterMorphology::MF_SQUARE_5>(ocnMask.data<uint8_t>(), ocnMask.width(), ocnMask.height(), ocnMask.paddingElements(), 0xFFu);

		cv::Mat cvFilterKernel = cv::getStructuringElement(cv::MORPH_RECT, {5, 5});
		cv::morphologyEx(cvMask, cvMask, cv::MORPH_CLOSE, cvFilterKernel);

		for (unsigned int y = 0u; y < height; ++y)
		{
			if (memcmp(ocnMask.constrow<void>(y), cvMask.ptr(y), ocnMask.planeWidthBytes(0u)) != 0)
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

} // namespace TestOpenCV

} // namespace TestCV

} // namespace Test

} // namespace Ocean
