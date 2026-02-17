/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testdetector/TestLineDetectorHough.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/ValidationPrecision.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameFilterGaussian.h"
#include "ocean/cv/CVUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestDetector
{

bool TestLineDetectorHough::test(const double testDuration, Worker& worker, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("Line detector test");
	Log::info() << " ";

	constexpr unsigned int width = 800u;
	constexpr unsigned int height = 640u;

	if (selector.shouldRun("accumulatorjoin"))
	{
		testResult = testAccumulatorJoin(width, height, worker);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("linedetectorrandomframe"))
	{
		testResult = testLineDetectorRandomFrame(testDuration, worker);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("linedetectorartificialframe"))
	{
		testResult = testLineDetectorArtificialFrame(width, height, testDuration, worker);
	}

	Log::info() << " ";
	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestLineDetectorHough, AccumulatorJoin_800x640)
{
	Worker worker;
	EXPECT_TRUE(TestLineDetectorHough::testAccumulatorJoin(800u, 640u, worker));
}

TEST(TestLineDetectorHough, LineDetectorRandomFrame)
{
	Worker worker;
	EXPECT_TRUE(TestLineDetectorHough::testLineDetectorRandomFrame(GTEST_TEST_DURATION, worker));
}

TEST(TestLineDetectorHough, LineDetectorArtificialFrame_800x640)
{
	Worker worker;
	EXPECT_TRUE(TestLineDetectorHough::testLineDetectorArtificialFrame(800u, 640u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestLineDetectorHough::testAccumulatorJoin(const unsigned int width, const unsigned int height, Worker& /*worker*/)
{
	ocean_assert(width >= 7u && height >= 7u);

	Log::info() << "Accumulator join function:";

	const unsigned int diagonalHalf = (unsigned int)(Numeric::sqrt(Scalar(width * width + height * height))) / 2u;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	{
		CV::Detector::LineDetectorHough::Accumulator accumulators[] =
		{
			CV::Detector::LineDetectorHough::Accumulator(width, height, diagonalHalf, 180u, 0u, true),
			CV::Detector::LineDetectorHough::Accumulator(width, height, diagonalHalf, 180u, 0u, true)
		};

		const unsigned int accumulatorElements = accumulators->distanceBins() * accumulators->angleBins();

		for (unsigned int n = 0; n < accumulatorElements; ++n)
		{
			accumulators[0].accumulatorFrame_.data<uint32_t>()[n] = RandomI::random(randomGenerator, 10000u);
			accumulators[1].accumulatorFrame_.data<uint32_t>()[n] = RandomI::random(randomGenerator, 10000u);
		}

		CV::Detector::LineDetectorHough::Accumulator copy[] =
		{
			CV::Detector::LineDetectorHough::Accumulator(width, height, diagonalHalf, 180u, 0u, true),
			CV::Detector::LineDetectorHough::Accumulator(width, height, diagonalHalf, 180u, 0u, true)
		};

		copy[0].accumulatorFrame_.copy(0, 0, accumulators[0].accumulatorFrame_);
		copy[1].accumulatorFrame_.copy(0, 0, accumulators[1].accumulatorFrame_);

		CV::Detector::LineDetectorHough::Accumulator::joinTwo(accumulators);

		OCEAN_EXPECT_TRUE(validation, validateJoin(copy, accumulators, 2));
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestLineDetectorHough::testLineDetectorRandomFrame(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Detector test on random frame:";

	const FrameType::PixelFormats pixelFormats = {FrameType::FORMAT_Y8, FrameType::FORMAT_RGB24};

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 50u, 1000u);
		const unsigned int height = RandomI::random(randomGenerator, 50u, 1000u);

		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const CV::Detector::LineDetectorHough::FilterType filterType = RandomI::boolean(randomGenerator) ? CV::Detector::LineDetectorHough::FT_SOBEL : CV::Detector::LineDetectorHough::FT_SCHARR;
		CV::Detector::LineDetectorHough::FilterResponse filterResponse = CV::Detector::LineDetectorHough::FR_INVALID;

		switch (RandomI::random(randomGenerator, 1u))
		{
			case 0u:
				filterResponse = CV::Detector::LineDetectorHough::FR_HORIZONTAL_VERTICAL;
				break;

			case 1u:
				filterResponse = CV::Detector::LineDetectorHough::FR_HORIZONTAL_VERTICAL_DIAGONAL;
				break;

			/*case 2u: // activate, once fully supported in the entire pipeline
				filterResponse = CV::Detector::LineDetectorHough::FR_DIAGONAL;
				break;*/

			default:
				ocean_assert(false && "Invalid value!");
				break;
		}

		CV::Detector::LineDetectorHough::InfiniteLines infiniteLines;
		FiniteLines2 optionalFiniteLines;

		const bool useFiniteLines = RandomI::boolean(randomGenerator);
		FiniteLines2* finiteLines = useFiniteLines ? &optionalFiniteLines : nullptr;

		const bool optimizeLines = RandomI::boolean(randomGenerator);

		const bool useWorkerBool = RandomI::boolean(randomGenerator);
		Worker* useWorker = useWorkerBool ? &worker : nullptr;

		OCEAN_EXPECT_TRUE(validation, CV::Detector::LineDetectorHough::detectLines(frame, filterType, filterResponse, infiniteLines, finiteLines, optimizeLines, 100u, 16u, 2u, true, useWorker));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestLineDetectorHough::testLineDetectorArtificialFrame(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 7u && height >= 7u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Detector test on artificial frame:";

	HighPerformanceStatistic performance0;
	HighPerformanceStatistic performance1;

	RandomGenerator randomGenerator;

	constexpr uint8_t dark = 0x40u;

	constexpr double threshold = std::is_same<Scalar, float>::value ? 0.85 : 0.95;

	ValidationPrecision validation(threshold, randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		ValidationPrecision::ScopedIteration scopedIteration(validation);

		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		frame.setValue(0xFF);

		const unsigned int horizontalTop = RandomI::random(randomGenerator, 6u, width - 7u);
		const unsigned int horizontalBottom = RandomI::random(randomGenerator, 6u, width - 7u);

		const unsigned int verticalLeft = RandomI::random(randomGenerator, 6u, height - 7u);
		const unsigned int verticalRight = RandomI::random(randomGenerator, 6u, height - 7u);

		// create two 11 pixel thick lines
		for (int n = -5; n <= 5; ++n)
		{
			CV::Canvas::line<1u>(frame, Scalar(int(horizontalTop) + n), Scalar(0), Scalar(int(horizontalBottom) + n), Scalar(int(height - 1u)), &dark);
			CV::Canvas::line<1u>(frame, Scalar(0), Scalar(int(verticalLeft) + n), Scalar(int(width - 1u)), Scalar(int(verticalRight) + n), &dark);
		}

		Lines2 testLines =
		{
			Line2(Vector2(Scalar(horizontalTop - 5), 0), (Vector2(Scalar(horizontalBottom - 5), Scalar(height - 1u)) - Vector2(Scalar(horizontalTop - 5), 0)).normalized()),
			Line2(Vector2(Scalar(horizontalTop + 5), 0), (Vector2(Scalar(horizontalBottom + 5), Scalar(height - 1u)) - Vector2(Scalar(horizontalTop + 5), 0)).normalized()),
			Line2(Vector2(0, Scalar(verticalLeft - 5)), (Vector2(Scalar(width - 1u), Scalar(verticalRight - 5)) - Vector2(0, Scalar(verticalLeft - 5))).normalized()),
			Line2(Vector2(0, Scalar(verticalLeft + 5)), (Vector2(Scalar(width - 1u), Scalar(verticalRight + 5)) - Vector2(0, Scalar(verticalLeft + 5))).normalized())
		};

		// apply some image blur
		CV::FrameFilterGaussian::filter(frame, 7u, &worker);

		CV::Detector::LineDetectorHough::InfiniteLines infiniteLines0;

		performance0.start();
			const bool result0 = CV::Detector::LineDetectorHough::detectLines(frame, CV::Detector::LineDetectorHough::FT_SOBEL, CV::Detector::LineDetectorHough::FR_HORIZONTAL_VERTICAL, infiniteLines0, nullptr, true, 80u, 8u, 5u, true, &worker, 360u, (unsigned int)(-1), false, Scalar(10), Numeric::deg2rad(5));
		performance0.stop();

		OCEAN_EXPECT_TRUE(validation, result0);

		CV::Detector::LineDetectorHough::InfiniteLines infiniteLines1;

		performance1.start();
			const bool result1 = CV::Detector::LineDetectorHough::detectLinesWithAdaptiveThreshold(frame, CV::Detector::LineDetectorHough::FT_SOBEL, CV::Detector::LineDetectorHough::FR_HORIZONTAL_VERTICAL, infiniteLines1, nullptr, true, Scalar(10), 61u, 8u, 5u, true, &worker, 360u, (unsigned int)(-1), false, Scalar(10), Numeric::deg2rad(5));
		performance1.stop();

		OCEAN_EXPECT_TRUE(validation, result1);

		std::sort(infiniteLines0.rbegin(), infiniteLines0.rend());
		std::sort(infiniteLines1.rbegin(), infiniteLines1.rend());

		unsigned int foundLines = 0u;

		for (const CV::Detector::LineDetectorHough::InfiniteLines& infiniteLines : {infiniteLines0, infiniteLines1})
		{
			for (size_t n = 0; n < infiniteLines.size() && n < testLines.size(); ++n)
			{
				const Line2& line = infiniteLines[n];
				const Line2 transformedLine(line.point() + Vector2(Scalar(width / 2u), Scalar(height / 2u)), line.direction());

				bool lineFound = false;

				for (size_t m = 0; !lineFound && m < testLines.size(); ++m)
				{
					const Scalar angle = transformedLine.direction().angle(testLines[m].direction());

					constexpr Scalar angleThreshold = Scalar(2.5);
					constexpr Scalar distanceThreshold = Scalar(3.5);

					if ((angle <= Numeric::deg2rad(angleThreshold) || angle >= Numeric::deg2rad(Scalar(180) - angleThreshold)) && Numeric::abs(transformedLine.distance(testLines[m].point())) <= distanceThreshold)
					{
						lineFound = true;
					}
				}

				if (lineFound)
				{
					++foundLines;
				}
			}
		}

		if (foundLines != 8u)
		{
			scopedIteration.setInaccurate();
		}
	}
	while (validation.needMoreIterations() || !startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Performance static threshold: " << performance0.averageMseconds() << "ms";
	Log::info() << "Performance dynamic threshold: " << performance1.averageMseconds() << "ms";

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestLineDetectorHough::validateSmooth(const unsigned int* original, const unsigned int* smoothAccumulator, const unsigned int width, const unsigned int height)
{
	// center
	for (unsigned int x = 1; x < width - 1; ++x)
	{
		for (unsigned int y = 1; y < height - 1; ++y)
		{
			// Filter mask:
			// 1 2 1
			// 2 4 2
			// 1 2 1

			const unsigned int topRow = original[(y - 1) * width + x - 1] * 1 + original[(y - 1) * width + x - 0] * 2 + original[(y - 1) * width + x + 1] * 1;
			const unsigned int middleRow = original[(y - 0) * width + x - 1] * 2 + original[(y - 0) * width + x - 0] * 4 + original[(y - 0) * width + x + 1] * 2;
			const unsigned int bottomRow = original[(y + 1) * width + x - 1] * 1 + original[(y + 1) * width + x - 0] * 2 + original[(y + 1) * width + x + 1] * 1;

			const unsigned int result = (topRow + middleRow + bottomRow + 8) / 16;

			if (result != smoothAccumulator[y * width + x])
			{
				return false;
			}
		}
	}

	{
		// top left corner

		// Top left filter mask:
		// 4 2
		// 2 1
		// Bottom right filter mask:
		// 1 2
		const unsigned int topRow = original[width * height - 1] * 2 + original[width * height - 2] * 1;
		const unsigned int middleRow = original[0] * 4 + original[1] * 2;
		const unsigned int bottomRow = original[width] * 2 + original[width + 1] * 1;

		const unsigned int result = (topRow + middleRow + bottomRow + 6) / 12;

		if (result != smoothAccumulator[0])
		{
			return false;
		}
	}

	{
		// top right corner

		// Top right filter mask:
		// 2 4
		// 1 2
		// Bottom left filter mask:
		// 2 1
		const unsigned int topRow = original[width * (height - 1)] * 2 + original[width * (height - 1) + 1] * 1;
		const unsigned int middleRow = original[width - 1] * 4 + original[width - 2] * 2;
		const unsigned int bottomRow = original[2 * width - 1] * 2 + original[2 * width - 2] * 1;

		const unsigned int result = (topRow + middleRow + bottomRow + 6) / 12;

		if (result != smoothAccumulator[width - 1])
		{
			return false;
		}
	}

	{
		// bottom left corner

		// Bottom left filter mask:
		// 2 1
		// 4 2
		// Top right filter mask:
		// 1 2
		const unsigned int topRow = original[width * (height - 2)] * 2 + original[width * (height - 2) + 1] * 1;
		const unsigned int middleRow = original[width * (height - 1)] * 4 + original[width * (height - 1) + 1] * 2;
		const unsigned int bottomRow = original[width - 1] * 2 + original[width - 2] * 1;

		const unsigned int result = (topRow + middleRow + bottomRow + 6) / 12;

		if (result != smoothAccumulator[width * (height - 1)])
		{
			return false;
		}
	}

	{
		// bottom right corner

		// Bottom right filter mask:
		// 1 2
		// 2 4
		// Top left filter mask:
		// 2 1
		const unsigned int topRow = original[width * (height - 2) + width - 2] * 1 + original[width * (height - 2) + width - 1] * 2;
		const unsigned int middleRow = original[width * height - 2] * 2 + original[width * height - 1] * 4;
		const unsigned int bottomRow = original[0] * 2 + original[1] * 1;

		const unsigned int result = (topRow + middleRow + bottomRow + 6) / 12;

		if (result != smoothAccumulator[width * height - 1])
		{
			return false;
		}
	}

	// top line
	// top filter mask:
	// 2 4 2
	// 1 2 1
	// bottom filter mask:
	// 1 2 1

	for (unsigned int x = 1; x < width - 1; ++x)
	{
		const unsigned int topRow = original[width * height - x - 1 - 1] * 1 + original[width * height - x - 0 - 1] * 2 + original[width * height - x + 1 - 1] * 1;
		const unsigned int middleRow = original[x - 1] * 2 + original[x] * 4 + original[x + 1] * 2;
		const unsigned int bottomRow = original[width + x - 1] * 1 + original[width + x] * 2 + original[width + x + 1] * 1;

		const unsigned int result = (topRow + middleRow + bottomRow + 8) / 16;

		if (result != smoothAccumulator[x])
		{
			return false;
		}
	}

	// bottom line
	// top filter mask:
	// 1 2 1
	// bottom filter mask:
	// 1 2 1
	// 2 4 2

	for (unsigned int x = 1; x < width - 1; ++x)
	{
		const unsigned int topRow = original[width * (height - 2) + x - 1] * 1 + original[width * (height - 2) + x - 0] * 2 + original[width * (height - 2) + x + 1] * 1;
		const unsigned int middleRow = original[width * (height - 1) + x - 1] * 2 + original[width * (height - 1) + x - 0] * 4 + original[width * (height - 1) + x + 1] * 2;
		const unsigned int bottomRow = original[width - x - 1 - 1] * 1 + original[width - x - 0 - 1] * 2 + original[width - x + 1 - 1] * 1;

		const unsigned int result = (topRow + middleRow + bottomRow + 8) / 16;

		if (result != smoothAccumulator[width * (height - 1) + x])
		{
			return false;
		}
	}

	return true;
}

bool TestLineDetectorHough::validateJoin(const CV::Detector::LineDetectorHough::Accumulator* original, const CV::Detector::LineDetectorHough::Accumulator* joined, const unsigned int accumulators)
{
	ocean_assert(original && joined);

	const unsigned int elements = original->distanceBins() * original->angleBins();

	for (unsigned int n = 0; n < elements; ++n)
	{
		uint32_t total = 0u;

		for (unsigned int a = 0u; a < accumulators; ++a)
		{
			total += original[a].accumulatorFrame_.constdata<uint32_t>()[n];
		}

		if (total != joined->accumulatorFrame_.constdata<uint32_t>()[n])
		{
			return false;
		}
	}

	return true;
}

}

}

}

}
