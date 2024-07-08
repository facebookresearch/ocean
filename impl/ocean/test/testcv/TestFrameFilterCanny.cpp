/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterCanny.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Memory.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterCanny.h"
#include "ocean/cv/FrameFilterScharr.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

/**
 * Definition of individual edge directions for validation purposes only.
 */
enum EdgeDirection
{
	/// Unknown edge direction.
	ED_UNKNOWN = 0,
	/// Horizontal edge with 0 degree.
	ED_HORIZONTAL,
	/// Diagonal edge with 45 degree
	ED_DIAGONAL_45,
	/// Vertical edge with 90 degree.
	ED_VERTICAL,
	/// Diagonal edge with 135 degree.
	ED_DIAGONAL_135,
	/// No edge.
	ED_NO_EDGE
};

bool TestFrameFilterCanny::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	ocean_assert(width >= 3u && height >= 3u);

	Log::info() << "---   Canny filter test with frame size " << width << "x" << height << ":   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	Log::info() << " ";

	allSucceeded = testFilterCannyScharr<int8_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testFilterCannyScharr<int16_t>(width, height, testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Canny filter test succeeded.";
	}
	else
	{
		Log::info() << "Canny filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterCanny, FilterCannyScharrNormalized)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterCanny::testFilterCannyScharr<int8_t>(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterCanny, FilterCannyScharr)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterCanny::testFilterCannyScharr<int16_t>(1920u, 1080u, GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

template <typename TFilter>
bool TestFrameFilterCanny::testFilterCannyScharr(const unsigned int performanceWidth, const unsigned int performanceHeight, const double testDuration, Worker& worker)
{
	static_assert(std::is_same<TFilter, int8_t>::value || std::is_same<TFilter, int16_t>::value, "Invalid type for TFilter");

	ocean_assert(performanceWidth != 0u && performanceHeight != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing Canny edge detector with Scharr filter" << (std::is_same<TFilter, int8_t>::value ? " (normalized)" : "") << ":";

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	constexpr unsigned int maxThreshold = TFilter(std::is_same<TFilter, int8_t>::value ? 127u : 4080u);
	constexpr unsigned int maxThreshold_2 = maxThreshold / 2u;

	for (const bool performanceIteration : {true, false})
	{
		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			const Timestamp startTimestamp(true);

			do
			{
				const unsigned int lowThreshold = RandomI::random(randomGenerator, 0u, maxThreshold_2 - 5u);
				const unsigned int highThreshold = RandomI::random(randomGenerator, lowThreshold + 5u, maxThreshold);
				ocean_assert(lowThreshold < highThreshold);

				const unsigned int width = performanceIteration ? performanceWidth : RandomI::random(randomGenerator, 3u, 2048u);
				const unsigned int height  = performanceIteration ? performanceHeight : RandomI::random(randomGenerator, 3u, 2048u);

				const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);
				const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 256u) * RandomI::random(randomGenerator, 1u);

				Frame source(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), sourcePaddingElements);
				Frame target(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), targetPaddingElements);

				CV::CVUtilities::randomizeFrame(source, /* skipPaddingArea */ false, &randomGenerator);
				CV::CVUtilities::randomizeFrame(target, /* skipPaddingArea */ false, &randomGenerator);

				const Frame clonedTarget(target, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				if (std::is_same<TFilter, int8_t>::value)
				{
					ocean_assert(highThreshold <= 127u);

					performance.startIf(performanceIteration);
					CV::FrameFilterCanny::filterCannyScharrNormalized(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), int8_t(lowThreshold), int8_t(highThreshold), useWorker);
					performance.stopIf(performanceIteration);
				}
				else
				{
					ocean_assert(highThreshold <= 4080u);

					performance.startIf(performanceIteration);
					CV::FrameFilterCanny::filterCannyScharr(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), int16_t(lowThreshold), int16_t(highThreshold), useWorker);
					performance.stopIf(performanceIteration);
				}

				if (!CV::CVUtilities::isPaddingMemoryIdentical(target, clonedTarget))
				{
					Log::error() << "Padding area has been changed - potential memory access violation. Aborting immediately!";

					return false;
				}

				if (!validationCannyFilterScharr(source, target, TFilter(lowThreshold), TFilter(highThreshold)))
				{
					allSucceeded = false;
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}
	}

	Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms";
		Log::info() << "Multi-core boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
	}

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

template <typename TFilter>
bool TestFrameFilterCanny::validationCannyFilterScharr(const Frame& original, const Frame& filtered, const TFilter lowThreshold, const TFilter highThreshold)
{
	static_assert((std::is_same<uint8_t, uint8_t>::value && std::is_same<TFilter, int8_t>::value) || (std::is_same<uint8_t, uint8_t>::value && std::is_same<TFilter, int16_t>::value), "Invalid data types");

	ocean_assert(original.isValid() && filtered.isValid());
	ocean_assert(original.isPixelFormatCompatible(FrameType::FORMAT_Y8) && original.isFrameTypeCompatible(filtered, /* allowDifferentPixelOrigins */ false));

	const unsigned int width = original.width();
	const unsigned int height = original.height();

	// Filtering: 0, 45, 90, 135 degrees
	Memory filterResponses(sizeof(TFilter) * width * height * /* number of filter directions */ 4u);
	memset(filterResponses.data(), 0x00, filterResponses.size());

	constexpr unsigned int filterResponsePaddingElements = 0u;
	CV::FrameFilterScharr::filter8BitPerChannel<TFilter, 1u>(original.constdata<uint8_t>(), (TFilter*)filterResponses.data(), width, height, original.paddingElements(), filterResponsePaddingElements, nullptr);

	const TFilter* filterResponsesData = (const TFilter*)filterResponses.data();

	// Determine the magnitudes and directions
	Memory directionsMemory(sizeof(uint8_t) * width * height);
	memset(directionsMemory.data(), int(ED_UNKNOWN), directionsMemory.size());

	Memory magnitudesMemory(sizeof(TFilter) * width * height);
	memset(magnitudesMemory.data(), 0x00, magnitudesMemory.size());

	uint8_t* directionsData = (uint8_t*)(directionsMemory.data());
	TFilter* magnitudesData = (TFilter*)(magnitudesMemory.data());

	for (unsigned int y = 0u; y < height; y++)
	{
		for (unsigned int x = 0u; x < width; x++)
		{
			const unsigned int filterResponseIndex = y * (4u * width) + (4u * x);
			const unsigned int pixelIndex = y * width + x;

			constexpr EdgeDirection magnitudeDirections[4] =
			{
				ED_VERTICAL,
				ED_HORIZONTAL,
				ED_DIAGONAL_45,
				ED_DIAGONAL_135
			};

			const TFilter magnitudes[4] =
			{
				TFilter(abs(filterResponsesData[filterResponseIndex + 0])), // ED_VERTICAL
				TFilter(abs(filterResponsesData[filterResponseIndex + 1])), // ED_HORIZONTAL
				TFilter(abs(filterResponsesData[filterResponseIndex + 2])), // ED_DIAGONAL_45
				TFilter(abs(filterResponsesData[filterResponseIndex + 3])), // ED_DIAGONAL_135
			};

			TFilter maxMagnitudes[2] = { 0u, 0u };
			unsigned int maxIndex = 0u;

			for (unsigned int i = 0u; i < 4u; ++i)
			{
				if (magnitudes[i] >= maxMagnitudes[0])
				{
					maxMagnitudes[1] = maxMagnitudes[0];
					maxMagnitudes[0] = magnitudes[i];

					maxIndex = i;
				}
			}

			ocean_assert(maxIndex < 4u);

			if (maxMagnitudes[0] > maxMagnitudes[1] && maxMagnitudes[0] > lowThreshold)
			{
				directionsData[pixelIndex] = uint8_t(magnitudeDirections[maxIndex]);
				magnitudesData[pixelIndex] = maxMagnitudes[0];
			}
			else
			{
				directionsData[pixelIndex] = uint8_t(ED_NO_EDGE);
				magnitudesData[pixelIndex] = uint8_t(0);
			}
		}
	}


	// Apply non-maximum suppression using the two adjacent neighbors in direction of the gradient (aka. double thresholding)
	Memory edgeCandidates(sizeof(uint8_t) * width * height);
	memset(edgeCandidates.data(), 0, edgeCandidates.size());

	Memory edges(sizeof(uint8_t) * width * height);
	memset(edges.data(), 0, edges.size());

	uint8_t* edgeCandidatesData = (uint8_t*)edgeCandidates.data();
	uint8_t* edgesData = (uint8_t*)edges.data();

	CV::PixelPositions edgeLocations;

	for (unsigned int y = 1u; y < (height - 1u); y++)
	{
		for (unsigned int x = 1u; x < (width - 1u); x++)
		{
			const unsigned int pixelIndex = y * width + x;

			if (magnitudesData[pixelIndex] > lowThreshold)
			{

				TFilter magnitude0 = TFilter(0);
				TFilter magnitude1 = TFilter(0);

				switch (directionsData[pixelIndex])
				{
					case ED_HORIZONTAL:
						magnitude0 = magnitudesData[(y - 1u) * width + x];
						magnitude1 = magnitudesData[(y + 1u) * width + x];
						break;

					case ED_VERTICAL:
						magnitude0 = magnitudesData[y * width + x - 1u];
						magnitude1 = magnitudesData[y * width + x + 1u];
						break;

					case ED_DIAGONAL_45:
						magnitude0 = magnitudesData[(y - 1u) * width + x - 1u];
						magnitude1 = magnitudesData[(y + 1u) * width + x + 1u];
						break;

					case ED_DIAGONAL_135:
						magnitude0 = magnitudesData[(y - 1u) * width + x + 1u];
						magnitude1 = magnitudesData[(y + 1u) * width + x - 1u];
						break;

					default:
						ocean_assert(false && "Never be here as all magnitudes should exceed the minimum threshold");
						break;
				}

				if (magnitudesData[pixelIndex] > magnitude0 && magnitudesData[pixelIndex] >= magnitude1)
				{
					// The current pixel is a local maximum.

					if (magnitudesData[pixelIndex] > highThreshold)
					{
						// Strong edge
						edgeCandidatesData[pixelIndex] = 255u;
						edgesData[pixelIndex] = 255u;

						edgeLocations.emplace_back(x, y);
					}
					else
					{
						// Weak edge
						edgeCandidatesData[pixelIndex] = 128u;
						edgesData[pixelIndex] = 0u;
					}
				}
			}
		}
	}

	// Apply hysteresis
	while (!edgeLocations.empty())
	{
		const CV::PixelPosition edgeLocation = edgeLocations.back();
		ocean_assert(edgeLocation.x() > 0u && edgeLocation.x() < width && edgeLocation.y() > 0u && edgeLocation.y() < height);


		edgeLocations.pop_back();

		const CV::PixelPosition neighbors[8] =
		{
			edgeLocation.northWest(),
			edgeLocation.north(),
			edgeLocation.northEast(),

			edgeLocation.west(),
			edgeLocation.east(),

			edgeLocation.southWest(),
			edgeLocation.south(),
			edgeLocation.southEast(),
		};

		for (unsigned int i = 0u; i < 8u; ++i)
		{
			const CV::PixelPosition& candidate = neighbors[i];
			ocean_assert(candidate.x() < width && candidate.y() < height);

			const unsigned int pixelIndex = candidate.y() * width + candidate.x();

			if (edgeCandidatesData[pixelIndex] == 128u)
			{
				edgeCandidatesData[pixelIndex] = 255u;
				edgesData[pixelIndex] = 255u;

				edgeLocations.emplace_back(candidate);
			}
		}
	}

	unsigned int numberIncorrect = 0u;
	for (unsigned int y = 1u; y < height - 1u; y++)
	{
		for (unsigned int x = 1u; x < width - 1u; x++)
		{
			const unsigned int pixelIndex = y * width + x;

			if (*filtered.constpixel<uint8_t>(x, y) != edgesData[pixelIndex])
			{
				numberIncorrect++;
			}
		}
	}

	const double error = double(numberIncorrect) / double(width * height);

	if (error > 0.001) // 0.1%
	{
		return false;
	}

	for (unsigned int x = 0u; x < width; ++x)
	{
		if (*filtered.constpixel<uint8_t>(x, 0u) != 0u)
		{
			return false;
		}

		if (*filtered.constpixel<uint8_t>(x, height - 1u) != 0u)
		{
			return false;
		}
	}

	for (unsigned int y = 0u; y < height; ++y)
	{
		if (*filtered.constpixel<uint8_t>(0u, y) != 0u)
		{
			return false;
		}

		if (*filtered.constpixel<uint8_t>(width - 1u, y) != 0u)
		{
			return false;
		}
	}

	return true;
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
