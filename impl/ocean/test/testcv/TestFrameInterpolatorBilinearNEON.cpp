/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameInterpolatorBilinearNEON.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/math/Random.h"

#include "ocean/test/Validation.h"

#include <array>
#include <iostream>

#ifdef OCEAN_USE_GTEST
#include "ocean/test/Test.h"

TEST(TestFrameInterpolatorBilinearNEON, Interpolate4Pixels8BitPerChannel4NEON)
{
	EXPECT_TRUE(Ocean::Test::TestCV::TestFrameInterpolatorBilinearNEON::testInterpolate4Pixels8BitPerChannel4NEON(GTEST_TEST_DURATION));
}

TEST(TestFrameInterpolatorBilinearNEON, Interpolate4Pixels4Channel8BitPerChannelNEON)
{
	EXPECT_TRUE(Ocean::Test::TestCV::TestFrameInterpolatorBilinearNEON::testInterpolate4Pixels4Channel8BitPerChannelNEON(GTEST_TEST_DURATION));
}

TEST(TestFrameInterpolatorBilinearNEON, Interpolate4Pixels4ChannelNEONSpeedComparison)
{
	EXPECT_TRUE(Ocean::Test::TestCV::TestFrameInterpolatorBilinearNEON::testInterpolate4Pixels4ChannelNEONSpeedComparison(GTEST_TEST_DURATION));
}

TEST(TestFrameInterpolatorBilinearNEON, Lookup4ChannelOptimizedNEONConsistency)
{
	EXPECT_TRUE(Ocean::Test::TestCV::TestFrameInterpolatorBilinearNEON::testLookup4ChannelOptimizedNEONConsistency(GTEST_TEST_DURATION));
}

#endif

namespace Ocean
{

namespace Test
{

namespace TestCV
{

using namespace CV;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

namespace
{

/**
 * Scalar reference implementation of 4-pixel bilinear interpolation for 4 channels.
 * Uses the same fixed-point arithmetic (7-bit precision, add 8192, shift 14) as the NEON implementations.
 */
void interpolate4Pixels4ChannelScalarReference(const uint8_t* source, const unsigned int offsetsTopLeftElements[4], const unsigned int offsetsTopRightElements[4], const unsigned int offsetsBottomLeftElements[4], const unsigned int offsetsBottomRightElements[4], const unsigned int validPixels[4], const DataType<uint8_t, 4u>::Type& borderColor, const unsigned int factorsRight[4], const unsigned int factorsBottom[4], DataType<uint8_t, 4u>::Type* targetPositionPixels)
{
	constexpr unsigned int tChannels = 4u;

	for (unsigned int i = 0u; i < 4u; ++i)
	{
		const unsigned int factorRight = factorsRight[i];
		const unsigned int factorBottom = factorsBottom[i];
		const unsigned int factorLeft = 128u - factorRight;
		const unsigned int factorTop = 128u - factorBottom;

		const unsigned int factorTopLeft = factorTop * factorLeft;
		const unsigned int factorTopRight = factorTop * factorRight;
		const unsigned int factorBottomLeft = factorBottom * factorLeft;
		const unsigned int factorBottomRight = factorBottom * factorRight;

		if (validPixels[i])
		{
			const uint8_t* topLeft = source + offsetsTopLeftElements[i];
			const uint8_t* topRight = source + offsetsTopRightElements[i];
			const uint8_t* bottomLeft = source + offsetsBottomLeftElements[i];
			const uint8_t* bottomRight = source + offsetsBottomRightElements[i];

			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				((uint8_t*)(targetPositionPixels + i))[n] = (uint8_t)((topLeft[n] * factorTopLeft + topRight[n] * factorTopRight + bottomLeft[n] * factorBottomLeft + bottomRight[n] * factorBottomRight + 8192u) >> 14u);
			}
		}
		else
		{
			*(targetPositionPixels + i) = borderColor;
		}
	}
}

} // anonymous namespace

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

bool TestFrameInterpolatorBilinearNEON::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   NEON bilinear interpolation tests:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testInterpolate4Pixels8BitPerChannel4NEON(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolate4Pixels4Channel8BitPerChannelNEON(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolate4Pixels4ChannelNEONSpeedComparison(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testLookup4ChannelOptimizedNEONConsistency(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "NEON bilinear interpolation test succeeded.";
	}
	else
	{
		Log::info() << "NEON bilinear interpolation test FAILED!";
	}

	return allSucceeded;
}

bool TestFrameInterpolatorBilinearNEON::testInterpolate4Pixels8BitPerChannel4NEON(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing interpolate4Pixels8BitPerChannelNEON<4u> correctness:";

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int tChannels = 4u;
	using PixelType = DataType<uint8_t, tChannels>::Type;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	// Create a source image large enough for random offsets
	constexpr unsigned int sourceWidth = 100u;
	constexpr unsigned int sourceHeight = 100u;
	constexpr unsigned int sourceStrideElements = sourceWidth * tChannels;

	std::vector<uint8_t> sourceData(sourceStrideElements * sourceHeight);

	const Timestamp startTimestamp(true);

	do
	{
		// Fill source with random data
		for (size_t i = 0; i < sourceData.size(); ++i)
		{
			sourceData[i] = (uint8_t)RandomI::random(randomGenerator, 255u);
		}

		// Generate random interpolation positions within the image (keeping 1-pixel border for bilinear)
		unsigned int offsetsTopLeft[4];
		unsigned int offsetsTopRight[4];
		unsigned int offsetsBottomLeft[4];
		unsigned int offsetsBottomRight[4];
		unsigned int validPixels[4];
		unsigned int factorsRight[4];
		unsigned int factorsBottom[4];

		for (unsigned int i = 0u; i < 4u; ++i)
		{
			const bool isValid = RandomI::random(randomGenerator, 1u) == 1u;
			validPixels[i] = isValid ? 0xFFFFFFFFu : 0x00000000u;

			if (isValid)
			{
				const unsigned int x = RandomI::random(randomGenerator, sourceWidth - 2u);
				const unsigned int y = RandomI::random(randomGenerator, sourceHeight - 2u);

				offsetsTopLeft[i] = (y * sourceWidth + x) * tChannels;
				offsetsTopRight[i] = (y * sourceWidth + (x + 1u)) * tChannels;
				offsetsBottomLeft[i] = ((y + 1u) * sourceWidth + x) * tChannels;
				offsetsBottomRight[i] = ((y + 1u) * sourceWidth + (x + 1u)) * tChannels;
			}
			else
			{
				offsetsTopLeft[i] = 0u;
				offsetsTopRight[i] = 0u;
				offsetsBottomLeft[i] = 0u;
				offsetsBottomRight[i] = 0u;
			}

			factorsRight[i] = RandomI::random(randomGenerator, 128u);
			factorsBottom[i] = RandomI::random(randomGenerator, 128u);
		}

		PixelType borderColor;
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			borderColor.values_[n] = (uint8_t)RandomI::random(randomGenerator, 255u);
		}

		// Compute reference result
		PixelType referenceResult[4];
		interpolate4Pixels4ChannelScalarReference(sourceData.data(), offsetsTopLeft, offsetsTopRight, offsetsBottomLeft, offsetsBottomRight, validPixels, borderColor, factorsRight, factorsBottom, referenceResult);

		// Compute NEON result
		PixelType neonResult[4];

		const uint32x4_t m128_factorsRight = vld1q_u32(factorsRight);
		const uint32x4_t m128_factorsBottom = vld1q_u32(factorsBottom);

		FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelNEON<tChannels>(sourceData.data(), offsetsTopLeft, offsetsTopRight, offsetsBottomLeft, offsetsBottomRight, validPixels, borderColor, m128_factorsRight, m128_factorsBottom, neonResult);

		// Compare results
		for (unsigned int i = 0u; i < 4u; ++i)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				const int diff = int(((const uint8_t*)(neonResult + i))[n]) - int(((const uint8_t*)(referenceResult + i))[n]);

				if (std::abs(diff) > 1)
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();

#else

	Log::info() << "Skipped (no NEON support).";
	return true;

#endif
}

bool TestFrameInterpolatorBilinearNEON::testInterpolate4Pixels4Channel8BitPerChannelNEON(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing interpolate4Pixels4Channel8BitPerChannelNEON correctness:";

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int tChannels = 4u;
	using PixelType = DataType<uint8_t, tChannels>::Type;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	// Create a source image large enough for random offsets
	constexpr unsigned int sourceWidth = 100u;
	constexpr unsigned int sourceHeight = 100u;
	constexpr unsigned int sourceStrideElements = sourceWidth * tChannels;

	std::vector<uint8_t> sourceData(sourceStrideElements * sourceHeight);

	const Timestamp startTimestamp(true);

	do
	{
		// Fill source with random data
		for (size_t i = 0; i < sourceData.size(); ++i)
		{
			sourceData[i] = (uint8_t)RandomI::random(randomGenerator, 255u);
		}

		unsigned int offsetsTopLeft[4];
		unsigned int offsetsTopRight[4];
		unsigned int offsetsBottomLeft[4];
		unsigned int offsetsBottomRight[4];
		unsigned int factorsRight[4];
		unsigned int factorsBottom[4];

		PixelType topLeftPixels[4];
		PixelType topRightPixels[4];
		PixelType bottomLeftPixels[4];
		PixelType bottomRightPixels[4];

		for (unsigned int i = 0u; i < 4u; ++i)
		{
			const unsigned int x = RandomI::random(randomGenerator, sourceWidth - 2u);
			const unsigned int y = RandomI::random(randomGenerator, sourceHeight - 2u);

			offsetsTopLeft[i] = (y * sourceWidth + x) * tChannels;
			offsetsTopRight[i] = (y * sourceWidth + (x + 1u)) * tChannels;
			offsetsBottomLeft[i] = ((y + 1u) * sourceWidth + x) * tChannels;
			offsetsBottomRight[i] = ((y + 1u) * sourceWidth + (x + 1u)) * tChannels;

			factorsRight[i] = RandomI::random(randomGenerator, 128u);
			factorsBottom[i] = RandomI::random(randomGenerator, 128u);

			// Gather pixel data (what the caller would do before calling the new function)
			topLeftPixels[i] = *((const PixelType*)(sourceData.data() + offsetsTopLeft[i]));
			topRightPixels[i] = *((const PixelType*)(sourceData.data() + offsetsTopRight[i]));
			bottomLeftPixels[i] = *((const PixelType*)(sourceData.data() + offsetsBottomLeft[i]));
			bottomRightPixels[i] = *((const PixelType*)(sourceData.data() + offsetsBottomRight[i]));
		}

		// Compute reference result (all pixels valid)
		unsigned int allValid[4] = {0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu};
		PixelType borderColor;
		memset(&borderColor, 0, sizeof(borderColor));

		PixelType referenceResult[4];
		interpolate4Pixels4ChannelScalarReference(sourceData.data(), offsetsTopLeft, offsetsTopRight, offsetsBottomLeft, offsetsBottomRight, allValid, borderColor, factorsRight, factorsBottom, referenceResult);

		// Compute NEON result using the new function
		PixelType neonResult[4];

		const uint32x4_t m128_factorsRight = vld1q_u32(factorsRight);
		const uint32x4_t m128_factorsBottom = vld1q_u32(factorsBottom);

		const uint8x16_t topLeftPixels_u8x16 = vld1q_u8((const uint8_t*)topLeftPixels);
		const uint8x16_t topRightPixels_u8x16 = vld1q_u8((const uint8_t*)topRightPixels);
		const uint8x16_t bottomLeftPixels_u8x16 = vld1q_u8((const uint8_t*)bottomLeftPixels);
		const uint8x16_t bottomRightPixels_u8x16 = vld1q_u8((const uint8_t*)bottomRightPixels);

		FrameInterpolatorBilinear::interpolate4Pixels4Channel8BitPerChannelNEON(topLeftPixels_u8x16, topRightPixels_u8x16, bottomLeftPixels_u8x16, bottomRightPixels_u8x16, m128_factorsRight, m128_factorsBottom, neonResult);

		// Compare results
		for (unsigned int i = 0u; i < 4u; ++i)
		{
			for (unsigned int n = 0u; n < tChannels; ++n)
			{
				const int diff = int(((const uint8_t*)(neonResult + i))[n]) - int(((const uint8_t*)(referenceResult + i))[n]);

				if (std::abs(diff) > 1)
				{
					OCEAN_SET_FAILED(validation);
				}
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();

#else

	Log::info() << "Skipped (no NEON support).";
	return true;

#endif
}

bool TestFrameInterpolatorBilinearNEON::testInterpolate4Pixels4ChannelNEONSpeedComparison(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Speed comparison: interpolate4Pixels8BitPerChannelNEON<4u> vs interpolate4Pixels4Channel8BitPerChannelNEON (4-channel RGBA):";

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int tChannels = 4u;
	using PixelType = DataType<uint8_t, tChannels>::Type;

	RandomGenerator randomGenerator;

	// Create a source image
	constexpr unsigned int sourceWidth = 1920u;
	constexpr unsigned int sourceHeight = 1080u;

	std::vector<uint8_t> sourceData(sourceWidth * sourceHeight * tChannels);
	for (size_t i = 0; i < sourceData.size(); ++i)
	{
		sourceData[i] = (uint8_t)RandomI::random(randomGenerator, 255u);
	}

	// Prepare a large batch of random test data so each measurement takes measurable time
	constexpr unsigned int batchSize = 10000u;

	std::vector<std::array<unsigned int, 4>> allOffsetsTopLeft(batchSize);
	std::vector<std::array<unsigned int, 4>> allOffsetsTopRight(batchSize);
	std::vector<std::array<unsigned int, 4>> allOffsetsBottomLeft(batchSize);
	std::vector<std::array<unsigned int, 4>> allOffsetsBottomRight(batchSize);
	std::vector<std::array<unsigned int, 4>> allValidPixels(batchSize);
	std::vector<std::array<unsigned int, 4>> allFactorsRight(batchSize);
	std::vector<std::array<unsigned int, 4>> allFactorsBottom(batchSize);
	std::vector<PixelType> allBorderColors(batchSize);

	// Pre-gathered pixel data for the optimized path
	std::vector<std::array<PixelType, 4>> allTopLeftPixels(batchSize);
	std::vector<std::array<PixelType, 4>> allTopRightPixels(batchSize);
	std::vector<std::array<PixelType, 4>> allBottomLeftPixels(batchSize);
	std::vector<std::array<PixelType, 4>> allBottomRightPixels(batchSize);

	for (unsigned int b = 0u; b < batchSize; ++b)
	{
		for (unsigned int i = 0u; i < 4u; ++i)
		{
			const unsigned int x = RandomI::random(randomGenerator, sourceWidth - 2u);
			const unsigned int y = RandomI::random(randomGenerator, sourceHeight - 2u);

			allOffsetsTopLeft[b][i] = (y * sourceWidth + x) * tChannels;
			allOffsetsTopRight[b][i] = (y * sourceWidth + (x + 1u)) * tChannels;
			allOffsetsBottomLeft[b][i] = ((y + 1u) * sourceWidth + x) * tChannels;
			allOffsetsBottomRight[b][i] = ((y + 1u) * sourceWidth + (x + 1u)) * tChannels;
			allValidPixels[b][i] = 0xFFFFFFFFu;

			allFactorsRight[b][i] = RandomI::random(randomGenerator, 128u);
			allFactorsBottom[b][i] = RandomI::random(randomGenerator, 128u);

			// Pre-gather pixels for the optimized function (done outside timing loop for fairness)
			allTopLeftPixels[b][i] = *((const PixelType*)(sourceData.data() + allOffsetsTopLeft[b][i]));
			allTopRightPixels[b][i] = *((const PixelType*)(sourceData.data() + allOffsetsTopRight[b][i]));
			allBottomLeftPixels[b][i] = *((const PixelType*)(sourceData.data() + allOffsetsBottomLeft[b][i]));
			allBottomRightPixels[b][i] = *((const PixelType*)(sourceData.data() + allOffsetsBottomRight[b][i]));
		}

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			allBorderColors[b].values_[n] = (uint8_t)RandomI::random(randomGenerator, 255u);
		}
	}

	PixelType targetPixels[4];

	// --- Benchmark the original function: interpolate4Pixels8BitPerChannelNEON<4u> ---
	// This function takes source pointer + offsets, gathers pixels internally, then interpolates

	HighPerformanceStatistic performanceOriginal;

	const Timestamp startTimestampOriginal(true);

	do
	{
		performanceOriginal.start();

		for (unsigned int b = 0u; b < batchSize; ++b)
		{
			const uint32x4_t m128_factorsRight = vld1q_u32(allFactorsRight[b].data());
			const uint32x4_t m128_factorsBottom = vld1q_u32(allFactorsBottom[b].data());

			FrameInterpolatorBilinear::interpolate4Pixels8BitPerChannelNEON<tChannels>(sourceData.data(), allOffsetsTopLeft[b].data(), allOffsetsTopRight[b].data(), allOffsetsBottomLeft[b].data(), allOffsetsBottomRight[b].data(), allValidPixels[b].data(), allBorderColors[b], m128_factorsRight, m128_factorsBottom, targetPixels);
		}

		performanceOriginal.stop();
	}
	while (startTimestampOriginal + testDuration > Timestamp(true));

	// --- Benchmark the optimized function: interpolate4Pixels4Channel8BitPerChannelNEON ---
	// This function takes pre-gathered pixel data in NEON registers (no source+offset gather needed)
	// The pixel gather is done outside the timing loop for a fair comparison of the interpolation core

	HighPerformanceStatistic performanceOptimized;

	const Timestamp startTimestampOptimized(true);

	do
	{
		performanceOptimized.start();

		for (unsigned int b = 0u; b < batchSize; ++b)
		{
			const uint8x16_t topLeftPixels_u8x16 = vld1q_u8((const uint8_t*)allTopLeftPixels[b].data());
			const uint8x16_t topRightPixels_u8x16 = vld1q_u8((const uint8_t*)allTopRightPixels[b].data());
			const uint8x16_t bottomLeftPixels_u8x16 = vld1q_u8((const uint8_t*)allBottomLeftPixels[b].data());
			const uint8x16_t bottomRightPixels_u8x16 = vld1q_u8((const uint8_t*)allBottomRightPixels[b].data());

			const uint32x4_t m128_factorsRight = vld1q_u32(allFactorsRight[b].data());
			const uint32x4_t m128_factorsBottom = vld1q_u32(allFactorsBottom[b].data());

			FrameInterpolatorBilinear::interpolate4Pixels4Channel8BitPerChannelNEON(topLeftPixels_u8x16, topRightPixels_u8x16, bottomLeftPixels_u8x16, bottomRightPixels_u8x16, m128_factorsRight, m128_factorsBottom, targetPixels);
		}

		performanceOptimized.stop();
	}
	while (startTimestampOptimized + testDuration > Timestamp(true));

	Log::info() << "Iterations: " << batchSize << " interpolations per measurement";
	Log::info() << "Original  (interpolate4Pixels8BitPerChannelNEON<4u>):    " << performanceOriginal;
	Log::info() << "Optimized (interpolate4Pixels4Channel8BitPerChannelNEON): " << performanceOptimized;

	const double avgOriginal = performanceOriginal.averageMseconds();
	const double avgOptimized = performanceOptimized.averageMseconds();

	Log::info() << "Average original:  " << String::toAString(avgOriginal, 4u) << " ms";
	Log::info() << "Average optimized: " << String::toAString(avgOptimized, 4u) << " ms";

	if (avgOptimized > 0.0)
	{
		const double speedup = avgOriginal / avgOptimized;
		Log::info() << "Speedup: " << String::toAString(speedup, 2u) << "x";
	}
	else
	{
		Log::info() << "Speedup: both functions completed too fast to measure; increase batchSize or testDuration";
	}

	Log::info() << "Measurements original: " << performanceOriginal.measurements() << ", optimized: " << performanceOptimized.measurements();

	return true;

#else

	Log::info() << "Skipped (no NEON support on this platform).";
	return true;

#endif
}

bool TestFrameInterpolatorBilinearNEON::testLookup4ChannelOptimizedNEONConsistency(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing lookup8BitPerChannelSubsetNEON<4u>: useOptimizedNEON=false vs useOptimizedNEON=true:";

	constexpr unsigned int tChannels = 4u;
	constexpr unsigned int width = 1920u;
	constexpr unsigned int height = 1080u;

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	// Create a fixed source frame and lookup table so all measurements have the same workload
	const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(tChannels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

	CV::FrameInterpolatorBilinear::LookupTable lookupTable(width, height, 20u, 20u);

	const bool offset = true;

	for (unsigned int yBin = 0u; yBin <= lookupTable.binsY(); ++yBin)
	{
		for (unsigned int xBin = 0u; xBin <= lookupTable.binsX(); ++xBin)
		{
			const Vector2 value = Random::vector2(randomGenerator, -10, 10);
			lookupTable.setBinTopLeftCornerValue(xBin, yBin, value);
		}
	}

	uint8_t borderColor[tChannels];
	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		borderColor[n] = (uint8_t)RandomI::random(randomGenerator, 255u);
	}

	Frame targetStandard = CV::CVUtilities::randomizedFrame(sourceFrame.frameType(), &randomGenerator);
	Frame targetOptimized = CV::CVUtilities::randomizedFrame(sourceFrame.frameType(), &randomGenerator);

	// Benchmark standard path
	HighPerformanceStatistic performanceStandard;

	const Timestamp startStandard(true);

	do
	{
		performanceStandard.start();
		CV::FrameInterpolatorBilinear::lookup<uint8_t, tChannels>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), lookupTable, offset, borderColor, targetStandard.data<uint8_t>(), sourceFrame.paddingElements(), targetStandard.paddingElements(), nullptr, false /*useOptimizedNEON*/);
		performanceStandard.stop();
	}
	while (startStandard + testDuration > Timestamp(true));

	// Benchmark optimized path
	HighPerformanceStatistic performanceOptimized;

	const Timestamp startOptimized(true);

	do
	{
		performanceOptimized.start();
		CV::FrameInterpolatorBilinear::lookup<uint8_t, tChannels>(sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), lookupTable, offset, borderColor, targetOptimized.data<uint8_t>(), sourceFrame.paddingElements(), targetOptimized.paddingElements(), nullptr, true /*useOptimizedNEON*/);
		performanceOptimized.stop();
	}
	while (startOptimized + testDuration > Timestamp(true));

	// Validate correctness: both paths must produce identical output
	for (unsigned int y = 0u; y < targetStandard.height(); ++y)
	{
		const uint8_t* rowStandard = targetStandard.constrow<uint8_t>(y);
		const uint8_t* rowOptimized = targetOptimized.constrow<uint8_t>(y);

		for (unsigned int x = 0u; x < targetStandard.width() * tChannels; ++x)
		{
			if (rowStandard[x] != rowOptimized[x])
			{
				OCEAN_SET_FAILED(validation);
			}
		}
	}

	const double avgStandard = performanceStandard.averageMseconds();
	const double avgOptimized = performanceOptimized.averageMseconds();

	std::cout << std::endl;
	std::cout << "Frame: " << width << "x" << height << ", " << tChannels << " channels" << std::endl;
	std::cout << "Standard  (useOptimizedNEON=false): " << performanceStandard.toString() << std::endl;
	std::cout << "Optimized (useOptimizedNEON=true):  " << performanceOptimized.toString() << std::endl;
	std::cout << "Average standard:  " << String::toAString(avgStandard, 4u) << " ms" << std::endl;
	std::cout << "Average optimized: " << String::toAString(avgOptimized, 4u) << " ms" << std::endl;

	if (avgOptimized > 0.0 && avgStandard > 0.0)
	{
		const double speedup = avgStandard / avgOptimized;
		std::cout << "Speedup: " << String::toAString(speedup, 2u) << "x" << std::endl;
	}

	std::cout << "Measurements standard: " << performanceStandard.measurements() << ", optimized: " << performanceOptimized.measurements() << std::endl;
	std::cout << "Validation: " << (validation.succeeded() ? "succeeded" : "FAILED") << std::endl;

	return validation.succeeded();
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
