/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFramePyramid.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFramePyramid::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Test Frame Pyramid:   ---";
	Log::info() << " ";

	allSucceeded = testIdealLayers(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testIsOwner(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCalculateMemorySize(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCreationFramePyramidWithConstructor(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCreationFramePyramidWithReplace(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCreateFramePyramidExtreme() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConstructFromFrameMultiLayer(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConstructFromPyramid(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReplaceWithFrameType(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReplaceWithFrame(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReplace11(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConstructor11(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testReduceLayers(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "FramePyramid test succeeded.";
	}
	else
	{
		Log::info() << "FramePyramid test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFramePyramid, IdealLayers)
{
	EXPECT_TRUE(TestFramePyramid::testIdealLayers(GTEST_TEST_DURATION));
}


TEST(TestFramePyramid, IsOwner)
{
	EXPECT_TRUE(TestFramePyramid::testIsOwner(GTEST_TEST_DURATION));
}


TEST(TestFramePyramid, CalculateMemorySize)
{
	EXPECT_TRUE(TestFramePyramid::testCalculateMemorySize(GTEST_TEST_DURATION));
}


TEST(TestFramePyramid, CreationFramePyramidWithConstructor_1920x1080_1Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramidWithConstructor(1920u, 1080u, 1u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramidWithConstructor_1920x1080_2Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramidWithConstructor(1920u, 1080u, 2u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramidWithConstructor_1920x1080_3Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramidWithConstructor(1920u, 1080u, 3u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramidWithConstructor_1920x1080_4Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramidWithConstructor(1920u, 1080u, 4u, 5u, GTEST_TEST_DURATION, worker));
}


TEST(TestFramePyramid, CreationFramePyramidWithReplace_1920x1080_1Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramidWithReplace(1920u, 1080u, 1u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramidWithReplace_1920x1080_2Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramidWithReplace(1920u, 1080u, 2u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramidWithReplace_1920x1080_3Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramidWithReplace(1920u, 1080u, 3u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramidWithReplace_1920x1080_4Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramidWithReplace(1920u, 1080u, 4u, 5u, GTEST_TEST_DURATION, worker));
}


TEST(TestFramePyramid, CreateFramePyramidExtreme)
{
	EXPECT_TRUE(TestFramePyramid::testCreateFramePyramidExtreme());
}


TEST(TestFramePyramid, ConstructFromFrameMultiLayer_1920x1080_1Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testConstructFromFrameMultiLayer(1920u, 1080u, 1u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, ConstructFromFrameMultiLayer_1920x1080_2Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testConstructFromFrameMultiLayer(1920u, 1080u, 2u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, ConstructFromFrameMultiLayer_1920x1080_3Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testConstructFromFrameMultiLayer(1920u, 1080u, 3u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, ConstructFromFrameMultiLayer_1920x1080_4Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testConstructFromFrameMultiLayer(1920u, 1080u, 4u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, ConstructFromFrameMultiLayer_1920x1080_AllLayers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testConstructFromFrameMultiLayer(1920u, 1080u, (unsigned int)(-1), GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, ConstructFromFrameMultiLayer_RandomResolution_AllLayers)
{
	const unsigned int width = RandomI::random(1u, 1920u);
	const unsigned int height = RandomI::random(1u, 1080u);

	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testConstructFromFrameMultiLayer(width, height, (unsigned int)(-1), GTEST_TEST_DURATION, worker));
}


TEST(TestFramePyramid, ConstructFromPyramid)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testConstructFromPyramid(GTEST_TEST_DURATION, worker));
}


TEST(TestFramePyramid, ReplaceWithFrame)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testReplaceWithFrame(GTEST_TEST_DURATION, worker));
}


TEST(TestFramePyramid, ReplaceWithFrameType)
{
	EXPECT_TRUE(TestFramePyramid::testReplaceWithFrameType(GTEST_TEST_DURATION));
}


TEST(TestFramePyramid, Replace11)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testReplace11(GTEST_TEST_DURATION, worker));
}


TEST(TestFramePyramid, Constructor11)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testConstructor11(GTEST_TEST_DURATION, worker));
}


TEST(TestFramePyramid, ReduceLayers)
{
	EXPECT_TRUE(TestFramePyramid::testReduceLayers(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFramePyramid::testIdealLayers(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing ideal layers:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestampWorker(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

		{
			// testing maximal number of layers

			unsigned int coarsestLayerWidth = RandomI::random32(randomGenerator);
			unsigned int coarsestLayerHeight = RandomI::random32(randomGenerator);

			unsigned int* useCoarsestLayerWidth = RandomI::boolean(randomGenerator) ? &coarsestLayerWidth : nullptr;
			unsigned int* useCoarsestLayerHeight = RandomI::boolean(randomGenerator) ? &coarsestLayerHeight : nullptr;

			const unsigned int layers = CV::FramePyramid::idealLayers(width, height, 0u /*invalidCoarsestWidthOrHeight*/, useCoarsestLayerWidth, useCoarsestLayerHeight);

			if (layers == 0u)
			{
				allSucceeded = false;
			}

			unsigned int expectedLayers = 1u;
			unsigned int layerWidth = width;
			unsigned int layerHeight = height;

			while (true)
			{
				ocean_assert(layerWidth >= 1u && layerHeight >= 1u);

				if (layerWidth == 1u || layerHeight == 1u)
				{
					// we have reached the last layer
					break;
				}

				layerWidth /= 2u;
				layerHeight /= 2u;

				++expectedLayers;
			}

			if (layers != expectedLayers)
			{
				allSucceeded = false;
			}

			if (useCoarsestLayerWidth != nullptr)
			{
				if (*useCoarsestLayerWidth != layerWidth)
				{
					allSucceeded = false;
				}
			}

			if (useCoarsestLayerHeight != nullptr)
			{
				if (*useCoarsestLayerHeight != layerHeight)
				{
					allSucceeded = false;
				}
			}
		}

		const unsigned int invalidWidth = RandomI::random(randomGenerator, 0u, width - 1u);
		const unsigned int invalidHeight = RandomI::random(randomGenerator, 0u, height - 1u);

		{
			// testing function defining invalid size

			const unsigned int invalidSize = std::min(invalidWidth, invalidHeight);

			unsigned int coarsestLayerWidth = RandomI::random32(randomGenerator);
			unsigned int coarsestLayerHeight = RandomI::random32(randomGenerator);

			unsigned int* useCoarsestLayerWidth = RandomI::boolean(randomGenerator) ? &coarsestLayerWidth : nullptr;
			unsigned int* useCoarsestLayerHeight = RandomI::boolean(randomGenerator) ? &coarsestLayerHeight : nullptr;

			const unsigned int layers = CV::FramePyramid::idealLayers(width, height, invalidSize, useCoarsestLayerWidth, useCoarsestLayerHeight);

			if (layers == 0u)
			{
				allSucceeded = false;
			}

			unsigned int testWidth = width;
			unsigned int testHeight = height;

			for (unsigned int n = 1u; n < layers; ++n)
			{
				testWidth /= 2u;
				testHeight /= 2u;
			}

			if (testWidth <= invalidSize || testHeight <= invalidSize)
			{
				allSucceeded = false;
			}

			if (testWidth / 2u > invalidSize && testHeight / 2u > invalidSize)
			{
				allSucceeded = false;
			}

			if (useCoarsestLayerWidth != nullptr)
			{
				if (*useCoarsestLayerWidth != testWidth)
				{
					allSucceeded = false;
				}
			}

			if (useCoarsestLayerHeight != nullptr)
			{
				if (*useCoarsestLayerHeight != testHeight)
				{
					allSucceeded = false;
				}
			}
		}

		{
			// testing function defining invalid width and height

			unsigned int coarsestLayerWidth = RandomI::random32(randomGenerator);
			unsigned int coarsestLayerHeight = RandomI::random32(randomGenerator);

			unsigned int* useCoarsestLayerWidth = RandomI::boolean(randomGenerator) ? &coarsestLayerWidth : nullptr;
			unsigned int* useCoarsestLayerHeight = RandomI::boolean(randomGenerator) ? &coarsestLayerHeight : nullptr;

			const unsigned int layers = CV::FramePyramid::idealLayers(width, height, invalidWidth, invalidHeight, useCoarsestLayerWidth, useCoarsestLayerHeight);

			if (layers == 0u)
			{
				allSucceeded = false;
			}

			unsigned int testWidth = width;
			unsigned int testHeight = height;

			for (unsigned int n = 1u; n < layers; ++n)
			{
				testWidth /= 2u;
				testHeight /= 2u;
			}

			if (testWidth <= invalidWidth || testHeight <= invalidHeight)
			{
				allSucceeded = false;
			}

			if (testWidth / 2u > invalidWidth && testHeight / 2u > invalidHeight)
			{
				allSucceeded = false;
			}

			if (useCoarsestLayerWidth != nullptr)
			{
				if (*useCoarsestLayerWidth != testWidth)
				{
					allSucceeded = false;
				}
			}

			if (useCoarsestLayerHeight != nullptr)
			{
				if (*useCoarsestLayerHeight != testHeight)
				{
					allSucceeded = false;
				}
			}
		}

		{
			// testing function using factor and radius

			const unsigned int layerFactor = RandomI::random(randomGenerator, 2u, 20u);

			unsigned int maximalRadius = (unsigned int)(-1);

			if (RandomI::boolean(randomGenerator))
			{
				maximalRadius = RandomI::random(randomGenerator, 1u, std::max(width, height));
			}

			const unsigned int coarsestLayerRadius = RandomI::random(randomGenerator, 2u, 20u);

			unsigned int coarsestLayerWidth = RandomI::random32(randomGenerator);
			unsigned int coarsestLayerHeight = RandomI::random32(randomGenerator);

			unsigned int* useCoarsestLayerWidth = RandomI::boolean(randomGenerator) ? &coarsestLayerWidth : nullptr;
			unsigned int* useCoarsestLayerHeight = RandomI::boolean(randomGenerator) ? &coarsestLayerHeight : nullptr;

			const unsigned int layers = CV::FramePyramid::idealLayers(width, height, invalidWidth, invalidHeight, layerFactor, maximalRadius, coarsestLayerRadius, useCoarsestLayerWidth, useCoarsestLayerHeight);

			unsigned int testWidth = width;
			unsigned int testHeight = height;
			unsigned int testRadius = coarsestLayerRadius;

			for (unsigned int n = 1u; n < layers; ++n)
			{
				testWidth /= layerFactor;
				testHeight /= layerFactor;
				testRadius *= layerFactor;
			}

			if (testWidth <= invalidWidth || testHeight <= invalidHeight)
			{
				allSucceeded = false;
			}

			if (testRadius < maximalRadius)
			{
				// in case the resulting radius is not large enough, we must ensure that we could not have gone down another layer

				const unsigned int nextTestWidth = testWidth / layerFactor;
				const unsigned int nextTestHeight = testHeight / layerFactor;

				if (nextTestWidth > invalidWidth && nextTestHeight > invalidHeight)
				{
					allSucceeded = false;
				}
			}

			if (testRadius / layerFactor > maximalRadius && layers > 1u)
			{
				// we should not add more layers than necessary

				allSucceeded = false;
			}

			if (useCoarsestLayerWidth != nullptr)
			{
				if (*useCoarsestLayerWidth != testWidth)
				{
					allSucceeded = false;
				}
			}

			if (useCoarsestLayerHeight != nullptr)
			{
				if (*useCoarsestLayerHeight != testHeight)
				{
					allSucceeded = false;
				}
			}
		}
	}
	while (startTimestampWorker + testDuration > Timestamp(true));

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

bool TestFramePyramid::testIsOwner(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing isOwner:";

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_RGBA32,
		FrameType::FORMAT_YUV24
	};

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);
		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		{
			// testing pyramid from FrameType

			CV::FramePyramid framePyramid(CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE, FrameType(width, height, pixelFormat, pixelOrigin));

			if (!verifyPyramidOwnership(framePyramid, true /*isValid*/, true /*isOwner*/))
			{
				allSucceeded = false;
			}

			CV::FramePyramid movedFramePyramid(std::move(framePyramid));

			if (!verifyPyramidOwnership(movedFramePyramid, true /*isValid*/, true /*isOwner*/))
			{
				allSucceeded = false;
			}

			if (!verifyPyramidOwnership(framePyramid, false /*isValid*/, false /*isOwner*/)) // NOLINT(bugprone-use-after-move)
			{
				allSucceeded = false;
			}
		}

		{
			// testing pyramid from pyramid and making a copy

			CV::FramePyramid framePyramidSource(CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE, FrameType(width, height, pixelFormat, pixelOrigin));

			CV::FramePyramid framePyramid(framePyramidSource, 0u, CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE, true /*copyData*/);

			if (!verifyPyramidOwnership(framePyramid, true /*isValid*/, true /*isOwner*/))
			{
				allSucceeded = false;
			}

			CV::FramePyramid movedFramePyramid(std::move(framePyramid));

			if (!verifyPyramidOwnership(movedFramePyramid, true /*isValid*/, true /*isOwner*/))
			{
				allSucceeded = false;
			}

			if (!verifyPyramidOwnership(framePyramid, false /*isValid*/, false /*isOwner*/)) // NOLINT(bugprone-use-after-move)
			{
				allSucceeded = false;
			}
		}

		{
			// testing pyramid from pyramid and using the memory only

			CV::FramePyramid framePyramidSource(CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE, FrameType(width, height, pixelFormat, pixelOrigin));

			CV::FramePyramid framePyramid(framePyramidSource, 0u, CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE, false /*copyData*/);

			if (!verifyPyramidOwnership(framePyramid, true /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}

			CV::FramePyramid movedFramePyramid(std::move(framePyramid));

			if (!verifyPyramidOwnership(movedFramePyramid, true /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}

			if (!verifyPyramidOwnership(framePyramid, false /*isValid*/, false /*isOwner*/)) // NOLINT(bugprone-use-after-move)
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

bool TestFramePyramid::testCalculateMemorySize(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing calculateMemorySize:";

	FrameType::PixelFormats pixelFormats = FrameType::definedPixelFormats();

	// we test all pixel formats with one plane, and without dimension restrictions

	for (size_t n = 0; n < pixelFormats.size(); /*noop*/)
	{
		const FrameType::PixelFormat pixelFormat = pixelFormats[n];

		if (FrameType::numberPlanes(pixelFormat) != 1u || !FrameType::formatIsGeneric(pixelFormat))
		{
			pixelFormats[n] = pixelFormats.back();
			pixelFormats.pop_back();
		}
		else
		{
			++n;
		}
	}

	for (unsigned int channels = 1u; channels <= 4u; ++channels)
	{
		pixelFormats.emplace_back(FrameType::genericPixelFormat<uint8_t>(channels));
		pixelFormats.emplace_back(FrameType::genericPixelFormat<int16_t>(channels));
		pixelFormats.emplace_back(FrameType::genericPixelFormat<uint32_t>(channels));
		pixelFormats.emplace_back(FrameType::genericPixelFormat<int64_t>(channels));
		pixelFormats.emplace_back(FrameType::genericPixelFormat<float>(channels));
		pixelFormats.emplace_back(FrameType::genericPixelFormat<double>(channels));
	}

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 0u, 1920u); // 0 is a valid input
		const unsigned int height = RandomI::random(randomGenerator, 0u, 1080u);

		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);
		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		unsigned int layers = CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE;

		if (RandomI::boolean(randomGenerator))
		{
			layers = RandomI::random(randomGenerator, 0u, 20u); // 0 is a valid input
		}

		const bool includeFirstLayer = RandomI::boolean(randomGenerator);

		unsigned int totalLayers = 0u;
		unsigned int* useTotalLayers = RandomI::boolean(randomGenerator) ? &totalLayers : nullptr;

		const size_t size = CV::FramePyramid::calculateMemorySize(width, height, pixelFormat, layers, includeFirstLayer, useTotalLayers);

		unsigned int layerWidth = width;
		unsigned int layerHeight = height;

		size_t testSize = 0;
		unsigned int testLayers = 0u;

		for (unsigned int layerIndex = 0u; layerIndex < layers; ++layerIndex)
		{
			if (layerWidth == 0u || layerHeight == 0u)
			{
				break;
			}

			++testLayers;

			const FrameType layerFrameType(layerWidth, layerHeight, pixelFormat, pixelOrigin);

			if (!layerFrameType.isValid())
			{
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
			}

			const unsigned int layerSize = layerFrameType.frameTypeSize();

			if (layerSize == 0u)
			{
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
			}

			if (size_t(layerSize) >= NumericT<size_t>::maxValue() / 2u)
			{
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
			}

			if (layerIndex == 0u)
			{
				if (includeFirstLayer)
				{
					testSize += size_t(layerSize);
				}
			}
			else
			{
				testSize += size_t(layerSize);
			}

			layerWidth /= 2u;
			layerHeight /= 2u;

			if (testSize >= NumericT<size_t>::maxValue() / 2)
			{
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
			}
		}

		if (testSize != size)
		{
			allSucceeded = false;
		}

		if (useTotalLayers != nullptr)
		{
			if (*useTotalLayers != testLayers)
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

bool TestFramePyramid::testCreationFramePyramidWithConstructor(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int layers = 5u;

	Log::info() << "Testing creation of " << layers << " pyramid layers with constructor:";
	Log::info() << " ";

	bool allSucceeded = true;

	const Indices32 widths =  {640u, 800u, 1280u, 1920u, 3840u};
	const Indices32 heights = {480u, 640u,  720u, 1080u, 2160u};

	for (unsigned int n = 0u; n < widths.size(); ++n)
	{
		Log::info().newLine(n != 0u);

		ocean_assert(widths.size() == heights.size());
		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			allSucceeded = testCreationFramePyramidWithConstructor(width, height, channel, layers, testDuration, worker) && allSucceeded;
			Log::info() << " ";
		}
	}

	return allSucceeded;
}

bool TestFramePyramid::testCreationFramePyramidWithConstructor(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int layers, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u && channels >= 1u);
	ocean_assert(layers >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";
	Log::info() << " ";

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const CV::FramePyramid::DownsamplingMode downsamplingMode : {CV::FramePyramid::DM_FILTER_11, CV::FramePyramid::DM_FILTER_14641})
	{
		if (downsamplingMode == CV::FramePyramid::DM_FILTER_11)
		{
			Log::info() << "With 1-1 filter:";
		}
		else
		{
			Log::info() << "With 1-4-6-4-1 filter:";
		}

		for (const bool copyFirstLayer : {true, false})
		{
			if (copyFirstLayer)
			{
				Log::info() << "Copying first layer:";
			}
			else
			{
				Log::info() << "Using first layer:";
			}

			HighPerformanceStatistic performanceSinglecore;
			HighPerformanceStatistic performanceMulticore;

			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
				HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

				const Timestamp startTimestamp(true);

				do
				{
					for (const bool benchmarkIteration : {true, false})
					{
						const unsigned int testWidth = benchmarkIteration ? width : RandomI::random(randomGenerator, 1u, width);
						const unsigned int testHeight = benchmarkIteration ? height : RandomI::random(randomGenerator, 1u, height);

						const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

						Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, pixelFormat, pixelOrigin), &randomGenerator);

						const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

						const unsigned int testLayers = benchmarkIteration ? layers : RandomI::random(1u, 0xFFFFFFFFu);

						const bool useDownsamplingFunction = RandomI::boolean(randomGenerator);

						const CV::FramePyramid::DownsamplingFunction downsamplingFunction = useDownsamplingFunction ? CV::FramePyramid::downsamplingFunction(downsamplingMode, frame.pixelFormat()) : nullptr;

						const unsigned int expectedLayers = std::min(testLayers, determineMaxLayerCount(testWidth, testHeight));

						UnorderedIndexSet32 expectedReadOnlyLayers;
						UnorderedIndexSet32 expectedOwnerLayers;
						UnorderedIndexSet32 expectedOutsideMemoryBlockLayers;

						for (unsigned int layerIndex = 1u; layerIndex < expectedLayers; ++layerIndex)
						{
							expectedOwnerLayers.emplace(layerIndex);
						}

						CV::FramePyramid framePyramid;

						bool localResult = false;

						performance.start();

							if (useDownsamplingFunction)
							{
								if (copyFirstLayer)
								{
									framePyramid = CV::FramePyramid(frame, downsamplingFunction, testLayers, true /*copyFirstLayer*/, useWorker);

									expectedOwnerLayers.emplace(0u);
								}
								else
								{
									// we can either move the frame, or we can only use the memory

									if (RandomI::boolean(randomGenerator))
									{
										if (frame.isOwner())
										{
											expectedOwnerLayers.emplace(0u);
										}

										framePyramid = CV::FramePyramid(downsamplingFunction, std::move(frame), testLayers, useWorker);
									}
									else
									{
										framePyramid = CV::FramePyramid(frame, downsamplingFunction, testLayers, false /*copyFirstLayer*/, useWorker);
									}

									expectedOutsideMemoryBlockLayers.emplace(0u);
								}
							}
							else
							{
								if (copyFirstLayer)
								{
									framePyramid = CV::FramePyramid(frame, downsamplingMode, testLayers, true /*copyFirstLayer*/, useWorker);
								}
								else
								{
									framePyramid = CV::FramePyramid(downsamplingMode, std::move(frame), testLayers, useWorker);

									expectedOutsideMemoryBlockLayers.emplace(0u);
								}

								expectedOwnerLayers.emplace(0u);
							}

							localResult = framePyramid.isValid();

						performance.stop();

						if (!localResult)
						{
							allSucceeded = false;
						}

						const size_t expectedMemorySize = CV::FramePyramid::calculateMemorySize(testWidth, testHeight, pixelFormat, expectedLayers, copyFirstLayer);

						if (framePyramid.memory().size() != expectedMemorySize)
						{
							allSucceeded = false;
						}

						ocean_assert(framePyramid.layers() == expectedLayers);

						if (!validateConstructFromFrame(framePyramid, downsamplingMode, copyFrame, expectedLayers, expectedReadOnlyLayers, expectedOwnerLayers, expectedOutsideMemoryBlockLayers))
						{
							allSucceeded = false;
						}
					}
				}
				while (startTimestamp + testDuration > Timestamp(true));
			}

			Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms, first: " << String::toAString(performanceSinglecore.firstMseconds(), 2u) << "ms";

			if (performanceMulticore.measurements() != 0u)
			{
				Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms, first: " << String::toAString(performanceMulticore.firstMseconds(), 2u) << "ms";

				Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
			}
		}

		Log::info() << " ";
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

bool TestFramePyramid::testCreationFramePyramidWithReplace(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	constexpr unsigned int layers = 5u;

	Log::info() << "Testing creation of " << layers << " pyramid layers with replace:";
	Log::info() << " ";

	bool allSucceeded = true;

	const Indices32 widths =  {640u, 800u, 1280u, 1920u, 3840u};
	const Indices32 heights = {480u, 640u,  720u, 1080u, 2160u};

	for (unsigned int n = 0u; n < widths.size(); ++n)
	{
		Log::info().newLine(n != 0u);

		ocean_assert(widths.size() == heights.size());
		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			allSucceeded = testCreationFramePyramidWithReplace(width, height, channel, layers, testDuration, worker) && allSucceeded;
			Log::info() << " ";
		}
	}

	return allSucceeded;
}

bool TestFramePyramid::testCreationFramePyramidWithReplace(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int layers, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u && channels >= 1u);
	ocean_assert(layers >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";
	Log::info() << " ";

	const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(channels);

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (const CV::FramePyramid::DownsamplingMode downsamplingMode : {CV::FramePyramid::DM_FILTER_11, CV::FramePyramid::DM_FILTER_14641})
	{
		if (downsamplingMode == CV::FramePyramid::DM_FILTER_11)
		{
			Log::info() << "With 1-1 filter:";
		}
		else
		{
			Log::info() << "With 1-4-6-4-1 filter:";
		}

		for (const bool copyFirstLayer : {true, false})
		{
			if (copyFirstLayer)
			{
				Log::info() << "Copying first layer:";
			}
			else
			{
				Log::info() << "Using first layer:";
			}

			HighPerformanceStatistic performanceSinglecore;
			HighPerformanceStatistic performanceMulticore;

			for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
			{
				Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
				HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

				const Timestamp startTimestamp(true);

				do
				{
					for (const bool benchmarkIteration : {true, false})
					{
						const unsigned int testWidth = benchmarkIteration ? width : RandomI::random(randomGenerator, 1u, width);
						const unsigned int testHeight = benchmarkIteration ? height : RandomI::random(randomGenerator, 1u, height);

						const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

						Frame frame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, pixelFormat, pixelOrigin), &randomGenerator);

						const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

						const unsigned int testLayers = benchmarkIteration ? layers : RandomI::random(1u, 0xFFFFFFFFu);

						const bool useDownsamplingFunction = RandomI::boolean(randomGenerator);

						const CV::FramePyramid::DownsamplingFunction downsamplingFunction = useDownsamplingFunction ? CV::FramePyramid::downsamplingFunction(downsamplingMode, frame.pixelFormat()) : nullptr;

						const unsigned int expectedLayers = std::min(testLayers, determineMaxLayerCount(testWidth, testHeight));

						UnorderedIndexSet32 expectedReadOnlyLayers;
						UnorderedIndexSet32 expectedOwnerLayers;
						UnorderedIndexSet32 expectedOutsideMemoryBlockLayers;

						for (unsigned int layerIndex = 1u; layerIndex < expectedLayers; ++layerIndex)
						{
							expectedOwnerLayers.emplace(layerIndex);
						}

						CV::FramePyramid framePyramid;

						bool localResult = false;

						performance.start();

							if (useDownsamplingFunction)
							{
								if (copyFirstLayer)
								{
									localResult = framePyramid.replace(frame, downsamplingFunction, testLayers, true /*copyFirstLayer*/, useWorker);

									expectedOwnerLayers.emplace(0u);
								}
								else
								{
									// we can either move the frame, or we can only use the memory

									if (RandomI::boolean(randomGenerator))
									{
										if (frame.isOwner())
										{
											expectedOwnerLayers.emplace(0u);
										}

										localResult = framePyramid.replace(downsamplingFunction, std::move(frame), testLayers, useWorker);
									}
									else
									{
										localResult = framePyramid.replace(frame, downsamplingFunction, testLayers, false /*copyFirstLayer*/, useWorker);
									}

									expectedOutsideMemoryBlockLayers.emplace(0u);
								}
							}
							else
							{
								if (copyFirstLayer)
								{
									localResult = framePyramid.replace(frame, downsamplingMode, testLayers, true /*copyFirstLayer*/, useWorker);

									expectedOwnerLayers.emplace(0u);
								}
								else
								{
									if (RandomI::boolean(randomGenerator))
									{
										if (frame.isReadOnly())
										{
											expectedReadOnlyLayers.emplace(0u);
										}

										if (frame.isOwner())
										{
											expectedOwnerLayers.emplace(0u);
										}

										localResult = framePyramid.replace(downsamplingMode, std::move(frame), testLayers, useWorker);
									}
									else
									{
										if (frame.isReadOnly() || (downsamplingMode == CV::FramePyramid::DM_FILTER_11 && !frame.hasAlphaChannel()))
										{
											expectedReadOnlyLayers.emplace(0u);
										}

										localResult = framePyramid.replace(frame, downsamplingMode, testLayers, false /*copyFirstLayer*/, useWorker);
									}

									expectedOutsideMemoryBlockLayers.emplace(0u);
								}
							}

						performance.stop();

						if (!localResult)
						{
							allSucceeded = false;
						}

						const size_t expectedMemorySize = CV::FramePyramid::calculateMemorySize(testWidth, testHeight, pixelFormat, expectedLayers, copyFirstLayer);

						if (framePyramid.memory().size() != expectedMemorySize)
						{
							allSucceeded = false;
						}

						ocean_assert(framePyramid.layers() == expectedLayers);

						if (!validateConstructFromFrame(framePyramid, downsamplingMode, copyFrame, expectedLayers, expectedReadOnlyLayers, expectedOwnerLayers, expectedOutsideMemoryBlockLayers))
						{
							allSucceeded = false;
						}
					}
				}
				while (startTimestamp + testDuration > Timestamp(true));
			}

			Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms, first: " << String::toAString(performanceSinglecore.firstMseconds(), 2u) << "ms";

			if (performanceMulticore.measurements() != 0u)
			{
				Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms, first: " << String::toAString(performanceMulticore.firstMseconds(), 2u) << "ms";

				Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
			}
		}

		Log::info() << " ";
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

bool TestFramePyramid::testCreateFramePyramidExtreme()
{
	Log::info() << "Testing creation with extreme parameters";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Indices32 widths =  {640u, 641u, 640u, 641u, 800u, 1280u, 1920u, 3840u, 3840u};
	const Indices32 heights = {480u, 480u, 481u, 481u, 640u,  720u, 1080u, 2048u, 2160u};

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_Y8, FrameType::FORMAT_YA16, FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32,
		FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::genericPixelFormat<uint8_t, 2u>(), FrameType::genericPixelFormat<uint8_t, 3u>(), FrameType::genericPixelFormat<uint8_t, 4u>()
	};

	for (const unsigned int threads : {1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 12u, 32u, 33u, 48u, 64u})
	{
		Worker extremeWorker(threads, Worker::TYPE_CUSTOM);

		for (unsigned int n = 0u; n < widths.size(); ++n)
		{
			ocean_assert(widths.size() == heights.size());

			const unsigned int width = widths[n];
			const unsigned int height = heights[n];

			for (const FrameType::PixelFormat pixelFormat : pixelFormats)
			{
				for (const CV::FramePyramid::DownsamplingMode downsamplingMode : {CV::FramePyramid::DM_FILTER_11, CV::FramePyramid::DM_FILTER_14641})
				{
					const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

					const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin), &randomGenerator);

					const bool copyFirstLayer = RandomI::boolean(randomGenerator);

					const unsigned int layers = CV::FramePyramid::idealLayers(width, height, 0u, 0u);

					for (unsigned int layerIndex = 1u; layerIndex <= layers; ++layerIndex)
					{
						const CV::FramePyramid framePyramid(frame, downsamplingMode, layerIndex, copyFirstLayer, &extremeWorker);

						if (!validateFramePyramid(frame, framePyramid, downsamplingMode, layerIndex))
						{
							allSucceeded = false;
						}

						if (framePyramid.coarsestLayer().width() >= 2u && framePyramid.coarsestLayer().height() >= 2u)
						{
							break;
						}
					}
				}
			}
		}
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

bool TestFramePyramid::testConstructFromFrameMultiLayer(const double testDuration, Worker& worker)
{
	Log::info() << "Testing multi layer construction/replacement from frame with 24 bit and 3 channels:";
	Log::info() << " ";

	const IndexPairs32 sizes =
	{
		IndexPair32(640u, 480u),
		IndexPair32(656u, 480u),
		IndexPair32(641u, 481u),
		IndexPair32(1024u, 513u),
		IndexPair32(1025u, 512u),
		IndexPair32(1920u, 1080u),
		IndexPair32(3840u, 2048u),
		IndexPair32(3840u, 2160u)
	};

	bool allSucceeded = true;

	for (size_t i = 0; i < sizes.size(); i++)
	{
		if (i != 0)
		{
			Log::info().newLine();
			Log::info().newLine();
		}

		const unsigned int width = sizes[i].first;
		const unsigned int height = sizes[i].second;

		allSucceeded = testConstructFromFrameMultiLayer(width, height, 1u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromFrameMultiLayer(width, height, 2u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromFrameMultiLayer(width, height, 4u, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromFrameMultiLayer(width, height, (unsigned int)(-1), testDuration, worker) && allSucceeded;
	}

	Log::info() << " ";

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

bool TestFramePyramid::testConstructFromFrameMultiLayer(const unsigned int width, unsigned int height, const unsigned int layerCount, const double testDuration, Worker& worker)
{
	Log::info() << "... with size " << width << "x" << height << " and " << (layerCount == ALL_LAYERS ? "all" : String::toAString(layerCount)) << " layers:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int channels = 1u; channels <= 4; ++channels)
	{
		Log::info() << "... with " << channels << " channels:";

		HighPerformanceStatistic performanceSinglecore;
		HighPerformanceStatistic performanceMulticore;

		for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
		{
			Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
			HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

			Timestamp startTimestamp(true);

			do
			{
				const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

				const CV::FramePyramid::DownsamplingMode dowsamplingMode = RandomI::random(randomGenerator, {CV::FramePyramid::DM_FILTER_11, CV::FramePyramid::DM_FILTER_14641});

				const bool copyFirstLayer = RandomI::boolean(randomGenerator);

				UnorderedIndexSet32 expectedReadOnlyLayers;
				UnorderedIndexSet32 expectedOwnerLayers;
				UnorderedIndexSet32 expectedOutsideMemoryBlockLayers;

				const unsigned int expectedNumberLayers = std::min(determineMaxLayerCount(width, height), layerCount);

				for (unsigned int layerIndex = 1u; layerIndex < expectedNumberLayers; ++layerIndex)
				{
					expectedOwnerLayers.emplace(layerIndex);
				}

				if (copyFirstLayer)
				{
					expectedOwnerLayers.emplace(0u);
				}
				else
				{
					expectedOutsideMemoryBlockLayers.emplace(0u);

					if (dowsamplingMode == CV::FramePyramid::DM_FILTER_11 && !frame.hasAlphaChannel())
					{
						expectedReadOnlyLayers.emplace(0u);
					}
				}

				performance.start();
					const CV::FramePyramid framePyramid(frame, dowsamplingMode, layerCount, copyFirstLayer, useWorker);
				performance.stop();

				if (!validateConstructFromFrame(framePyramid, dowsamplingMode, frame, expectedNumberLayers, expectedReadOnlyLayers, expectedOwnerLayers, expectedOutsideMemoryBlockLayers))
				{
					allSucceeded = false;
				}

				if (dowsamplingMode == CV::FramePyramid::DM_FILTER_11)
				{

					if (!copyFirstLayer)
					{
						expectedReadOnlyLayers.emplace(0u);
						expectedOutsideMemoryBlockLayers.emplace(0u);
					}

					if (!validateConstructFromFrame(CV::FramePyramid(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.channels(), frame.pixelOrigin(), layerCount, frame.paddingElements(), copyFirstLayer, useWorker, FrameType::FORMAT_UNDEFINED, frame.timestamp()), CV::FramePyramid::DM_FILTER_11, frame, expectedNumberLayers, expectedReadOnlyLayers, expectedOwnerLayers, expectedOutsideMemoryBlockLayers))
					{
						allSucceeded = false;
					}

					if (!validateConstructFromFrame(CV::FramePyramid(frame, layerCount, copyFirstLayer, useWorker), CV::FramePyramid::DM_FILTER_11, frame, expectedNumberLayers, expectedReadOnlyLayers, expectedOwnerLayers, expectedOutsideMemoryBlockLayers))
					{
						allSucceeded = false;
					}
				}
			}
			while (startTimestamp + testDuration > Timestamp(true));
		}

		Log::info() << "Singlecore performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms, first: " << String::toAString(performanceSinglecore.firstMseconds(), 2u) << "ms";

		if (performanceMulticore.measurements() != 0u)
		{
			Log::info() << "Multicore performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms, first: " << String::toAString(performanceMulticore.firstMseconds(), 2u) << "ms";
			Log::info() << "Multicore boost: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x";
		}
	}

	Log::info() << " ";

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

bool TestFramePyramid::testConstructFromPyramid(const double testDuration, Worker& worker)
{
	Log::info() << "Testing construction from pyramid:";
	Log::info() << " ";

	bool allSucceeded = true;

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_Y8, FrameType::FORMAT_YA16, FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32,
		FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::genericPixelFormat<uint8_t, 2u>(), FrameType::genericPixelFormat<uint8_t, 3u>(), FrameType::genericPixelFormat<uint8_t, 4u>()
	};

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 2000u);

		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);
		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin), &randomGenerator);

		const CV::FramePyramid::DownsamplingMode downsamplingMode = RandomI::random(randomGenerator, {CV::FramePyramid::DM_FILTER_11, CV::FramePyramid::DM_FILTER_14641});

		unsigned int layers = CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE;

		if (RandomI::boolean(randomGenerator))
		{
			layers = RandomI::random(randomGenerator, 1u, 20u);
		}

		Worker* useWorker = RandomI::boolean(randomGenerator) ? &worker : nullptr;

		const CV::FramePyramid framePyramid(frame, downsamplingMode, layers, true /*copyFirstLayer*/, useWorker);

		if (!framePyramid.isValid())
		{
			ocean_assert(false && "This should never happen!");
			allSucceeded = false;
		}

		ocean_assert(validateFramePyramid(frame, framePyramid, downsamplingMode, framePyramid.layers()));

		const unsigned int firstSourceLayerIndex = RandomI::random(randomGenerator, framePyramid.layers() - 1u);

		unsigned int sourceLayers = CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE;

		if (RandomI::boolean(randomGenerator))
		{
			sourceLayers = RandomI::random(randomGenerator, 1u, 20u);
		}

		const unsigned int expectedNumberLayers = std::min(sourceLayers, framePyramid.layers() - firstSourceLayerIndex);

		const bool copyData = RandomI::boolean(randomGenerator);

		UnorderedIndexSet32 ownerLayers;
		UnorderedIndexSet32 outsideMemoryBlockLayers;

		if (copyData)
		{
			for (unsigned int n = 0u; n < expectedNumberLayers; ++n)
			{
				ownerLayers.emplace(n);
			}
		}
		else
		{
			for (unsigned int n = 0u; n < expectedNumberLayers; ++n)
			{
				outsideMemoryBlockLayers.emplace(n);
			}
		}

		const CV::FramePyramid newPyramid(framePyramid, firstSourceLayerIndex, sourceLayers, copyData);

		if (!verifyPyramidOwnership(newPyramid, true, copyData))
		{
			allSucceeded = false;
		}

		UnorderedIndexSet32 readOnlyLayers;

		if (!validateConstructFromFrame(newPyramid, downsamplingMode, framePyramid[firstSourceLayerIndex], expectedNumberLayers, readOnlyLayers, ownerLayers, outsideMemoryBlockLayers))
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << " ";

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

bool TestFramePyramid::testReplaceWithFrameType(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing replace with frame type:";

	bool allSucceeded = true;

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_Y8, FrameType::FORMAT_YA16, FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32,
		FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::genericPixelFormat<uint8_t, 2u>(), FrameType::genericPixelFormat<uint8_t, 3u>(), FrameType::genericPixelFormat<uint8_t, 4u>()
	};

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		CV::FramePyramid framePyramid;

		FrameType previousFrameType;

		for (unsigned int nIteration = 0u; nIteration < 10u; ++nIteration)
		{
			const size_t previousPyramidMemorySize = framePyramid.memory().size();
			const void* previousPyramidMemory = framePyramid.memory().constdata();

			FrameType frameType = previousFrameType;

			unsigned int layers = framePyramid.layers();

			if (nIteration == 0u || RandomI::boolean(randomGenerator))
			{
				const unsigned int width = RandomI::random(randomGenerator, 1u, 2000u);
				const unsigned int height = RandomI::random(randomGenerator, 1u, 2000u);

				const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);
				const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

				frameType = FrameType(width, height, pixelFormat, pixelOrigin);

				layers = RandomI::random(randomGenerator, 1u, 100u);
			}
			else
			{
				// request a similar pyramid as before

				if (RandomI::random(randomGenerator, 1u == 0u))
				{
					const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(previousFrameType.channels());
					const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

					frameType = FrameType(previousFrameType, pixelFormat, pixelOrigin);

					layers = RandomI::random(randomGenerator, 1u, framePyramid.layers());
				}
			}

			const unsigned int expectedLayers = std::min(layers, determineMaxLayerCount(frameType.width(), frameType.height()));

			const bool forceOwner = RandomI::boolean(randomGenerator);

			if (framePyramid.replace(frameType, forceOwner, layers))
			{
				if (framePyramid.layers() != expectedLayers)
				{
					allSucceeded = false;
				}

				if (!framePyramid.isOwner())
				{
					allSucceeded = false;
				}

				if (framePyramid.finestLayer().frameType() != frameType)
				{
					allSucceeded = false;
				}

				unsigned int layerWidth = frameType.width();
				unsigned int layerHeight = frameType.height();
				size_t memoryOffset = 0;

				for (unsigned int layerIndex = 0u; layerIndex < framePyramid.layers(); ++layerIndex)
				{
					if (layerWidth == 0u || layerHeight == 0u)
					{
						allSucceeded = false;
					}

					if (framePyramid[layerIndex].width() != layerWidth || framePyramid[layerIndex].height() != layerHeight)
					{
						allSucceeded = false;
					}

					if (framePyramid[layerIndex].pixelFormat() != frameType.pixelFormat() || framePyramid[layerIndex].pixelOrigin() != frameType.pixelOrigin())
					{
						allSucceeded = false;
					}

					if (framePyramid[layerIndex].constdata<void>() != framePyramid.memory().constdata<uint8_t>() + memoryOffset)
					{
						allSucceeded = false;
					}

					const size_t layerSize = size_t(layerWidth * layerHeight * frameType.channels()) * sizeof(uint8_t);

					layerWidth /= 2u;
					layerHeight /= 2u;

					memoryOffset += layerSize;
				}

				if (memoryOffset > framePyramid.memory().size())
				{
					allSucceeded = false;
				}

				if (nIteration != 0u)
				{
					unsigned int testTotalLayers = 0u;
					const size_t newMemorySize = CV::FramePyramid::calculateMemorySize(frameType.width(), frameType.height(), frameType.pixelFormat(), expectedLayers, true, &testTotalLayers);

					ocean_assert(testTotalLayers == expectedLayers);

					const bool expectUpdatedMemory = previousPyramidMemorySize < newMemorySize;

					if (expectUpdatedMemory)
					{
						if (framePyramid.memory().constdata() == previousPyramidMemory)
						{
							allSucceeded = false;
						}
					}
					else
					{
						if (framePyramid.memory().constdata() != previousPyramidMemory)
						{
							allSucceeded = false;
						}
					}
				}
			}
			else
			{
				allSucceeded = false;
			}

			previousFrameType = frameType;
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

bool TestFramePyramid::testReplaceWithFrame(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing replace with frame:";

	bool allSucceeded = true;

	const FrameType::PixelFormats pixelFormats =
	{
		FrameType::FORMAT_Y8, FrameType::FORMAT_YA16, FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32,
		FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::genericPixelFormat<uint8_t, 2u>(), FrameType::genericPixelFormat<uint8_t, 3u>(), FrameType::genericPixelFormat<uint8_t, 4u>()
	};

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		CV::FramePyramid framePyramid;

		FrameType previousFrameType;
		unsigned int previousPyramidLayers = 0u;
		size_t previousMemorySize = 0;

		bool finestLayerWasOwner = false;

		for (unsigned int nIteration = 0u; nIteration < 10u; ++nIteration)
		{
			const size_t previousPyramidMemorySize = framePyramid.memory().size();
			const void* previousPyramidMemory = framePyramid.memory().constdata();

			FrameType newFrameType = previousFrameType;

			unsigned int layers = framePyramid.layers();

			if (nIteration == 0u || RandomI::boolean(randomGenerator))
			{
				const unsigned int width = RandomI::random(randomGenerator, 1u, 2000u);
				const unsigned int height = RandomI::random(randomGenerator, 1u, 2000u);

				const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);
				const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

				newFrameType = FrameType(width, height, pixelFormat, pixelOrigin);

				layers = RandomI::random(randomGenerator, 1u, 100u);
			}
			else
			{
				// request a similar pyramid as before

				if (RandomI::random(randomGenerator, 1u == 0u))
				{
					const FrameType::PixelFormat pixelFormat = FrameType::genericPixelFormat<uint8_t>(previousFrameType.channels());
					const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

					newFrameType = FrameType(previousFrameType, pixelFormat, pixelOrigin);

					layers = RandomI::random(randomGenerator, 1u, framePyramid.layers());
				}
			}

			Frame frame = CV::CVUtilities::randomizedFrame(newFrameType, &randomGenerator);

			const Frame copyFrame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

			const CV::FramePyramid::DownsamplingMode downsamplingMode = RandomI::random(randomGenerator, {CV::FramePyramid::DM_FILTER_11, CV::FramePyramid::DM_FILTER_14641});

			Worker* useWorker = RandomI::boolean(randomGenerator) ? &worker : nullptr;

			const unsigned int expectedLayers = std::min(layers, determineMaxLayerCount(frame.width(), frame.height()));

			const bool copyFirstLayer = RandomI::boolean(randomGenerator);
			const bool useFrameFunction = RandomI::boolean(randomGenerator);

			UnorderedIndexSet32 expectedReadOnlyLayers;
			UnorderedIndexSet32 expectedOwnerLayers;
			UnorderedIndexSet32 expectedOutsideMemoryBlockLayers;

			for (unsigned int layerIndex = 1u; layerIndex < expectedLayers; ++layerIndex)
			{
				expectedOwnerLayers.emplace(layerIndex);
			}

			bool localResult = false;

			if (useFrameFunction)
			{
				if (copyFirstLayer)
				{
					localResult = framePyramid.replace(frame, downsamplingMode, layers, true /*copyFirstLayer*/, useWorker);

					expectedOwnerLayers.emplace(0u);
				}
				else
				{
					if (RandomI::boolean(randomGenerator))
					{
						localResult = framePyramid.replace(downsamplingMode, std::move(frame), layers, useWorker);

						expectedOwnerLayers.emplace(0u);
						expectedOutsideMemoryBlockLayers.emplace(0u);
					}
					else
					{
						localResult = framePyramid.replace(frame, downsamplingMode, layers, false /*copyFirstLayer*/, useWorker);

						if (frame.isReadOnly() || (downsamplingMode == CV::FramePyramid::DM_FILTER_11 && !frame.hasAlphaChannel()))
						{
							expectedReadOnlyLayers.emplace(0u);
						}

						expectedOutsideMemoryBlockLayers.emplace(0u);
					}
				}
			}
			else
			{
				localResult = framePyramid.replace8BitPerChannel(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.channels(), frame.pixelOrigin(), downsamplingMode, layers, frame.paddingElements(), copyFirstLayer, useWorker, frame.pixelFormat(), frame.timestamp());

				if (copyFirstLayer)
				{
					expectedOwnerLayers.emplace(0u);
				}
				else
				{
					expectedReadOnlyLayers.emplace(0u);
					expectedOutsideMemoryBlockLayers.emplace(0u);
				}
			}

			if (localResult)
			{
				if (framePyramid.layers() != expectedLayers)
				{
					allSucceeded = false;
				}

				if (framePyramid.finestLayer().frameType() != newFrameType)
				{
					allSucceeded = false;
				}

				unsigned int testTotalLayers = 0u;
				const size_t newMemorySize = CV::FramePyramid::calculateMemorySize(copyFrame.width(), copyFrame.height(), copyFrame.pixelFormat(), expectedLayers, copyFirstLayer, &testTotalLayers);

				ocean_assert(testTotalLayers == expectedLayers);

				if (nIteration == 0u)
				{
					if (framePyramid.memory().size() != newMemorySize)
					{
						allSucceeded = false;
					}
				}
				else
				{
					if (framePyramid.memory().size() < newMemorySize)
					{
						allSucceeded = false;
					}
				}

				if (nIteration != 0u && finestLayerWasOwner)
				{
					// special case, the finest layer in the previous pyramid owned the memory, and the new pyramid perfectly fits

					if (previousFrameType == newFrameType && expectedLayers <= previousPyramidLayers && newMemorySize <= previousMemorySize)
					{
						// further, the downsampling is applied layer by layer, this could be optimized further in FramePyramid::replace8BitPerChannel11()

						if (downsamplingMode != CV::FramePyramid::DM_FILTER_11 || FrameType::formatHasAlphaChannel(copyFrame.pixelFormat()))
						{
							expectedOutsideMemoryBlockLayers.emplace(0u);
						}
					}
				}

				if (!validateConstructFromFrame(framePyramid, downsamplingMode, copyFrame, expectedLayers, expectedReadOnlyLayers, expectedOwnerLayers, expectedOutsideMemoryBlockLayers))
				{
					allSucceeded = false;
				}

				if (nIteration != 0u)
				{
					const bool expectUpdatedMemory = previousPyramidMemorySize < newMemorySize;

					if (expectUpdatedMemory)
					{
						if (framePyramid.memory().constdata() == previousPyramidMemory)
						{
							allSucceeded = false;
						}
					}
					else
					{
						if (framePyramid.memory().constdata() != previousPyramidMemory)
						{
							allSucceeded = false;
						}
					}
				}
			}
			else
			{
				allSucceeded = false;
			}

			previousFrameType = newFrameType;
			previousPyramidLayers = framePyramid.layers();
			previousMemorySize = framePyramid.memory().size();

			finestLayerWasOwner = framePyramid.isValid() && framePyramid.finestLayer().isOwner();
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

bool TestFramePyramid::testReplace11(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing replace8BitPerChannel11()";

	// the (for binary size optimized) replace8BitPerChannel11() function does not contain a special downsampling for alpha channels
	// therefore, testing pixel formats without alpha channel only

	const FrameType::PixelFormats pixelFormats = {FrameType::FORMAT_Y8, FrameType::genericPixelFormat<uint8_t, 2u>(), FrameType::FORMAT_RGB24, FrameType::genericPixelFormat<uint8_t, 4u>()};

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 2000u);

		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);

		const unsigned int layers = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int expectedLayers = std::min(layers, determineMaxLayerCount(width, height));

		const bool copyFirstLayer = RandomI::boolean(randomGenerator);

		CV::FramePyramid framePyramid;

		const void* initialPyramidMemory = nullptr;

		for (unsigned int nIteration = 0u; nIteration < 2u; ++nIteration)
		{
			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin), &randomGenerator);

			Worker* useWorker = RandomI::boolean(randomGenerator) ? &worker : nullptr;

			const FrameType::PixelFormat usePixelFormat = RandomI::random(randomGenerator, {FrameType::FORMAT_UNDEFINED, pixelFormat});

			bool replaceResult = false;

			if (RandomI::boolean(randomGenerator))
			{
				replaceResult = framePyramid.replace8BitPerChannel11(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.channels(), pixelOrigin, layers, frame.paddingElements(), copyFirstLayer, useWorker, usePixelFormat, frame.timestamp());
			}
			else
			{
				replaceResult = framePyramid.replace8BitPerChannel11(frame, layers, copyFirstLayer, useWorker);
			}

			if (replaceResult)
			{
				if (framePyramid.layers() != expectedLayers)
				{
					allSucceeded = false;
				}

				if (copyFirstLayer)
				{
					if (frame.constdata<void>() == framePyramid.finestLayer().constdata<void>())
					{
						allSucceeded = false;
					}
				}
				else
				{
					if (frame.constdata<void>() != framePyramid.finestLayer().constdata<void>())
					{
						allSucceeded = false;
					}
				}

				const bool allowCompatibleFrameType = usePixelFormat == FrameType::FORMAT_UNDEFINED ? true : false;

				if (!validateFramePyramid(frame, framePyramid, CV::FramePyramid::DM_FILTER_11, expectedLayers, allowCompatibleFrameType))
				{
					allSucceeded = false;
				}

				if (nIteration == 0u)
				{
					ocean_assert(initialPyramidMemory == nullptr);
					initialPyramidMemory = framePyramid.memory().constdata();
				}
				else
				{
					ocean_assert(initialPyramidMemory != nullptr || (framePyramid.layers() == 1u && !copyFirstLayer));

					if (initialPyramidMemory != framePyramid.memory().constdata())
					{
						// the pyramid should not have allocated a new memory
						allSucceeded = false;
					}
				}
			}
			else
			{
				allSucceeded = false;
			}
		}

		// now, we replace the pyramid with a larger frame

		const unsigned int newWidth = width + 2u;
		const unsigned int newHeight = height + 2u;

		const unsigned int newExpectedLayers = std::min(layers, determineMaxLayerCount(newWidth, newHeight));

		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(newWidth, newHeight, pixelFormat, pixelOrigin), &randomGenerator);

		Worker* useWorker = RandomI::boolean(randomGenerator) ? &worker : nullptr;

		const FrameType::PixelFormat usePixelFormat = RandomI::random(randomGenerator, {FrameType::FORMAT_UNDEFINED, pixelFormat});

		if (framePyramid.replace8BitPerChannel11(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.channels(), pixelOrigin, layers, frame.paddingElements(), copyFirstLayer, useWorker, usePixelFormat, frame.timestamp()))
		{
			if (framePyramid.layers() != newExpectedLayers)
			{
				allSucceeded = false;
			}

			if (copyFirstLayer)
			{
				if (frame.constdata<void>() == framePyramid.finestLayer().constdata<void>())
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (frame.constdata<void>() != framePyramid.finestLayer().constdata<void>())
				{
					allSucceeded = false;
				}
			}

			const bool allowCompatibleFrameType = usePixelFormat == FrameType::FORMAT_UNDEFINED ? true : false;

			if (!validateFramePyramid(frame, framePyramid, CV::FramePyramid::DM_FILTER_11, newExpectedLayers, allowCompatibleFrameType))
			{
				allSucceeded = false;
			}

			if (framePyramid.layers() != 1u || copyFirstLayer)
			{
				ocean_assert(initialPyramidMemory != nullptr);

				if (initialPyramidMemory == framePyramid.memory().constdata())
				{
					// the pyramid should have allocated a new memory
					allSucceeded = false;
				}
			}
		}
		else
		{
			allSucceeded = false;
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

bool TestFramePyramid::testConstructor11(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing restrictive constructor applying 1-1 downsampling:";

	// the (for binary size optimized) replace8BitPerChannel11() function does not contain a special downsampling for alpha channels
	// therefore, testing pixel formats without alpha channel only

	const FrameType::PixelFormats pixelFormats = {FrameType::FORMAT_Y8, FrameType::genericPixelFormat<uint8_t, 2u>(), FrameType::FORMAT_RGB24, FrameType::genericPixelFormat<uint8_t, 4u>()};

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 2000u);

		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);

		const unsigned int layers = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int expectedLayers = std::min(layers, determineMaxLayerCount(width, height));

		const bool copyFirstLayer = RandomI::boolean(randomGenerator);

		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin), &randomGenerator);

		Worker* useWorker = RandomI::boolean(randomGenerator) ? &worker : nullptr;

		FrameType::PixelFormat usePixelFormat = RandomI::random(randomGenerator, {FrameType::FORMAT_UNDEFINED, pixelFormat});

		const bool useFrameObjectConstructor = RandomI::boolean(randomGenerator);

		CV::FramePyramid framePyramid;

		if (useFrameObjectConstructor)
		{
			framePyramid = CV::FramePyramid(frame, layers, copyFirstLayer, useWorker);
		}
		else
		{
			framePyramid = CV::FramePyramid(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.channels(), pixelOrigin, layers, frame.paddingElements(), copyFirstLayer, useWorker, usePixelFormat, frame.timestamp());
		}

		if (framePyramid.isValid())
		{
			const void* initialPyramidMemory = framePyramid.memory().constdata();

			if (framePyramid.layers() != expectedLayers)
			{
				allSucceeded = false;
			}

			if (copyFirstLayer)
			{
				if (frame.constdata<void>() == framePyramid.finestLayer().constdata<void>())
				{
					allSucceeded = false;
				}
			}
			else
			{
				if (frame.constdata<void>() != framePyramid.finestLayer().constdata<void>())
				{
					allSucceeded = false;
				}
			}

			bool allowCompatibleFrameType = usePixelFormat == FrameType::FORMAT_UNDEFINED ? true : false;

			if (!validateFramePyramid(frame, framePyramid, CV::FramePyramid::DM_FILTER_11, expectedLayers, allowCompatibleFrameType))
			{
				allSucceeded = false;
			}


			// now, re replace the pyramid

			frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin), &randomGenerator);

			useWorker = RandomI::boolean(randomGenerator) ? &worker : nullptr;

			usePixelFormat = RandomI::random(randomGenerator, {FrameType::FORMAT_UNDEFINED, pixelFormat});

			if (framePyramid.replace8BitPerChannel11(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.channels(), pixelOrigin, layers, frame.paddingElements(), copyFirstLayer, useWorker, usePixelFormat, frame.timestamp()))
			{
				if (framePyramid.layers() != expectedLayers)
				{
					allSucceeded = false;
				}

				if (copyFirstLayer)
				{
					if (frame.constdata<void>() == framePyramid.finestLayer().constdata<void>())
					{
						allSucceeded = false;
					}
				}
				else
				{
					if (frame.constdata<void>() != framePyramid.finestLayer().constdata<void>())
					{
						allSucceeded = false;
					}
				}

				allowCompatibleFrameType = usePixelFormat == FrameType::FORMAT_UNDEFINED ? true : false;

				if (!validateFramePyramid(frame, framePyramid, CV::FramePyramid::DM_FILTER_11, expectedLayers, allowCompatibleFrameType))
				{
					allSucceeded = false;
				}

				ocean_assert(initialPyramidMemory != nullptr || (framePyramid.layers() == 1u && !copyFirstLayer));

				if (initialPyramidMemory != framePyramid.memory().constdata())
				{
					// the pyramid should not have allocated a new memory
					allSucceeded = false;
				}
			}
			else
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
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

bool TestFramePyramid::testReduceLayers(const double testDuration)
{
	Log::info() << "Testing reduce layers:";

	bool allSucceeded = true;

	const FrameType::PixelFormats pixelFormats = {FrameType::FORMAT_Y8, FrameType::FORMAT_Y16, FrameType::FORMAT_YA16, FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32, FrameType::FORMAT_F32, FrameType::FORMAT_Y64, FrameType::FORMAT_RGBA64};

	RandomGenerator randomGenerator;

	{
		CV::FramePyramid framePyramid;

		framePyramid.reduceLayers(0);

		if (framePyramid.layers() != 0 || framePyramid.isValid())
		{
			allSucceeded = false;
		}
	}

	Timestamp startTimestamp(true);

	do
	{
		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, pixelFormats);

		const unsigned int width = RandomI::random(randomGenerator, 1u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 2000u);

		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		unsigned int layers = CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE;

		if (RandomI::boolean(randomGenerator))
		{
			layers = RandomI::random(1u, 10u);
		}

		CV::FramePyramid framePyramid(layers, FrameType(width, height, pixelFormat, pixelOrigin));

		if (!framePyramid.isValid())
		{
			allSucceeded = false;
		}

		std::vector<const void*> layerPoints;

		for (size_t layerIndex = 0; layerIndex < framePyramid.layers(); ++layerIndex)
		{
			layerPoints.emplace_back(framePyramid[(unsigned int)(layerIndex)].constdata<void>());
		}

		const size_t newLayers = size_t(RandomI::random(randomGenerator, 0u, framePyramid.layers()));

		framePyramid.reduceLayers(newLayers);

		if (framePyramid.layers() == newLayers)
		{
			for (size_t layerIndex = 0; layerIndex < framePyramid.layers(); ++layerIndex)
			{
				if (layerPoints[layerIndex] != framePyramid[(unsigned int)(layerIndex)].constdata<void>())
				{
					allSucceeded = false;
				}
			}
		}
		else
		{
			allSucceeded = false;
		}

		if (newLayers == 0)
		{
			if (framePyramid.isValid())
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

bool TestFramePyramid::validateFramePyramid(const Frame& frame, const CV::FramePyramid& framePyramid, const CV::FramePyramid::DownsamplingMode downsamplingMode, const unsigned int layers, const bool allowCompatibleFrameType)
{
	ocean_assert(frame && framePyramid && layers >= 1u);

	const CV::FramePyramid::DownsamplingFunction downsamplingFunction = CV::FramePyramid::downsamplingFunction(downsamplingMode, frame.pixelFormat());
	ocean_assert(downsamplingFunction);

	return validateFramePyramid(frame, framePyramid, downsamplingFunction, layers, allowCompatibleFrameType);
}

bool TestFramePyramid::validateFramePyramid(const Frame& frame, const CV::FramePyramid& framePyramid, const CV::FramePyramid::DownsamplingFunction& downsamplingFunction, const unsigned int layers, const bool allowCompatibleFrameType)
{
	ocean_assert(frame && framePyramid && layers >= 1u);
	ocean_assert(downsamplingFunction);

	if (!downsamplingFunction)
	{
		return false;
	}

	if (layers > framePyramid.layers())
	{
		return false;
	}

	if (allowCompatibleFrameType && !frame.isFrameTypeCompatible(framePyramid[0], false))
	{
		return false;
	}

	if (!allowCompatibleFrameType && frame.frameType() != framePyramid[0].frameType())
	{
		return false;
	}

	// we check the first layer

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		ocean_assert(frame.numberPlanes() == 1u);
		const unsigned int frameWidthBytes = frame.planeWidthBytes(0u);

		if (memcmp(frame.constrow<void>(y), framePyramid[0].constrow<void>(y), frameWidthBytes) != 0)
		{
			return false;
		}
	}

	// for the remaining layers we do not need to consider padding elements

	Frame finerLayer(frame, Frame::ACM_USE_KEEP_LAYOUT);

	for (unsigned int n = 1u; n < layers; ++n)
	{
		Frame coarserLayer;

		if (!downsamplingFunction(finerLayer, coarserLayer, nullptr /*worker*/))
		{
			return false;
		}

		if (allowCompatibleFrameType && !coarserLayer.isFrameTypeCompatible(framePyramid[n], false))
		{
			return false;
		}

		if (!allowCompatibleFrameType && coarserLayer.frameType() != framePyramid[n].frameType())
		{
			return false;
		}

		const Frame& pyramidLayer = framePyramid[n];

		const size_t planeWidthBytes = size_t(coarserLayer.planeWidthBytes(0u));
		ocean_assert(planeWidthBytes != 0);

		for (unsigned int y = 0u; y < coarserLayer.height(); ++y)
		{
			if (memcmp(coarserLayer.constrow<void>(y), pyramidLayer.constrow<void>(y), planeWidthBytes) != 0)
			{
				return false;
			}
		}

		finerLayer = std::move(coarserLayer);
	}

	for (unsigned int layerIndex = 0u; layerIndex < framePyramid.layers(); ++layerIndex)
	{
		if (framePyramid[layerIndex].timestamp() != frame.timestamp())
		{
			return false;
		}

		if (framePyramid[layerIndex].relativeTimestamp().isValid() && framePyramid[layerIndex].relativeTimestamp() != frame.relativeTimestamp())
		{
			return false;
		}
	}

	return true;
}

bool TestFramePyramid::validateConstructFromFrame(const CV::FramePyramid& framePyramid, const CV::FramePyramid::DownsamplingMode downsamplingMode, const Frame& frame, const unsigned int numberLayers, const UnorderedIndexSet32& readOnlyLayers, const UnorderedIndexSet32& ownerLayers, const UnorderedIndexSet32& outsideMemoryBlockLayers)
{
	ocean_assert(framePyramid.isValid());
	ocean_assert(frame.isValid());
	ocean_assert(numberLayers >= 1u);

	const CV::FramePyramid::DownsamplingFunction downsamplingFunction = CV::FramePyramid::downsamplingFunction(downsamplingMode, frame.pixelFormat());
	ocean_assert(downsamplingFunction);

	return validateConstructFromFrame(framePyramid, downsamplingFunction, frame, numberLayers, readOnlyLayers, ownerLayers, outsideMemoryBlockLayers);
}

bool TestFramePyramid::validateConstructFromFrame(const CV::FramePyramid& framePyramid, const CV::FramePyramid::DownsamplingFunction& downsamplingFunction, const Frame& frame, const unsigned int numberLayers, const UnorderedIndexSet32& readOnlyLayers, const UnorderedIndexSet32& ownerLayers, const UnorderedIndexSet32& outsideMemoryBlockLayers)
{
	ocean_assert(framePyramid.isValid());
	ocean_assert(frame.isValid());
	ocean_assert(numberLayers >= 1u);

	if (!framePyramid.isValid() || !frame.isValid() || numberLayers == 0u)
	{
		return false;
	}

	if (framePyramid.layers() != numberLayers)
	{
		return false;
	}

	if (!validateFramePyramid(frame, framePyramid, downsamplingFunction, numberLayers))
	{
		return false;
	}

	for (unsigned int layerIndex = 0u; layerIndex < numberLayers; ++layerIndex)
	{
		const Frame& layer = framePyramid[layerIndex];

		{
			// testing read-only vs. writable

			if (readOnlyLayers.find(layerIndex) == readOnlyLayers.cend())
			{
				// we expect a writable layer

				if (layer.isReadOnly())
				{
					return false;
				}
			}
			else
			{
				// the layer must not be writable

				if (!layer.isReadOnly())
				{
					return false;
				}
			}
		}

		const bool isOwnedByMemoryBlock = framePyramid.memory().isInside(layer.constdata<void>(), (const uint8_t*)(layer.constdata<void>()) + layer.size());

		{
			// testing is owned

			const bool isOwned = framePyramid.isOwner(layerIndex);

			const bool testIsOwned = layer.isOwner() || isOwnedByMemoryBlock;

			if (isOwned != testIsOwned)
			{
				return false;
			}

			if (ownerLayers.find(layerIndex) == ownerLayers.cend())
			{
				if (isOwned)
				{
					return false;
				}
			}
			else
			{
				if (!isOwned)
				{
					return false;
				}
			}
		}

		{
			// testing outside memory block situation

			if (outsideMemoryBlockLayers.find(layerIndex) != outsideMemoryBlockLayers.cend())
			{
				if (isOwnedByMemoryBlock)
				{
					return false;
				}
			}
			else
			{
				if (!isOwnedByMemoryBlock)
				{
					return false;
				}
			}
		}
	}

	if (ownerLayers.size() == numberLayers)
	{
		if (!framePyramid.isOwner())
		{
			return false;
		}
	}
	else
	{
		if (framePyramid.isOwner())
		{
			return false;
		}
	}

	return true;
}

unsigned int TestFramePyramid::determineMaxLayerCount(unsigned int width, unsigned int height)
{
	unsigned int layerCount = 1u;

	while (width > 1u && height > 1u)
	{
		layerCount++;

		width /= 2u;
		height /= 2u;
	}

	return layerCount;
}

bool TestFramePyramid::verifyPyramidOwnership(const CV::FramePyramid& framePyramid, const bool isValid, const bool isOwner, const Indices32& layerIsOwner)
{
	if (isValid != framePyramid.isValid())
	{
		return false;
	}

	if (isOwner != framePyramid.isOwner())
	{
		return false;
	}

	if (isOwner)
	{
		for (unsigned int layerIndex = 0u; layerIndex < framePyramid.layers(); ++layerIndex)
		{
			if (!framePyramid.isOwner(layerIndex))
			{
				return false;
			}
		}
	}

	if (framePyramid.isValid())
	{
		if (framePyramid.isOwner(framePyramid.layers()))
		{
			return false;
		}
	}

	if (!layerIsOwner.empty())
	{
		const UnorderedIndexSet32 layerSet(layerIsOwner.cbegin(), layerIsOwner.cend());

		for (unsigned int layerIndex = 0u; layerIndex < framePyramid.layers(); ++layerIndex)
		{
			if (layerSet.find(layerIndex) != layerSet.cend())
			{
				if (!framePyramid.isOwner(layerIndex))
				{
					return false;
				}
			}
			else
			{
				if (framePyramid.isOwner(layerIndex))
				{
					return false;
				}
			}
		}
	}
	else
	{
		for (unsigned int layerIndex = 0u; layerIndex < framePyramid.layers(); ++layerIndex)
		{
			if (framePyramid.isOwner(layerIndex) != isOwner)
			{
				return false;
			}
		}
	}

	return true;
}

}

}

}
