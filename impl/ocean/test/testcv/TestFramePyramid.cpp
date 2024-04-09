// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/testcv/TestFramePyramid.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Processor.h"
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

	allSucceeded = testCreationFramePyramidWithFrame(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCreationFramePyramid(testDuration, worker) && allSucceeded;

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


TEST(TestFramePyramid, CreationFramePyramidWithFrame_1920x1080_1Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramidWithFrame(1920u, 1080u, 1u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramidWithFrame_1920x1080_2Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramidWithFrame(1920u, 1080u, 2u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramidWithFrame_1920x1080_3Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramidWithFrame(1920u, 1080u, 3u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramidWithFrame_1920x1080_4Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramidWithFrame(1920u, 1080u, 4u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramid_1920x1080_1Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramid(1920u, 1080u, 1u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramid_1920x1080_2Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramid(1920u, 1080u, 2u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramid_1920x1080_3Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramid(1920u, 1080u, 3u, 5u, GTEST_TEST_DURATION, worker));
}

TEST(TestFramePyramid, CreationFramePyramid_1920x1080_4Channels_5Layers)
{
	Worker worker;
	EXPECT_TRUE(TestFramePyramid::testCreationFramePyramid(1920u, 1080u, 4u, 5u, GTEST_TEST_DURATION, worker));
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

			const unsigned int layers = CV::FramePyramid::idealLayers(width, height, 0u /*invalidWidthOrHeight*/);

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
		}

		const unsigned int invalidWidth = RandomI::random(randomGenerator, 0u, width - 1u);
		const unsigned int invalidHeight = RandomI::random(randomGenerator, 0u, height - 1u);

		{
			// testing function defining invalid size

			const unsigned int invalidSize = std::min(invalidWidth, invalidHeight);

			const unsigned int layers = CV::FramePyramid::idealLayers(width, height, invalidSize);

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
		}

		{
			// testing function defining invalid width and height

			const unsigned int layers = CV::FramePyramid::idealLayers(width, height, invalidWidth, invalidHeight);

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
		}

		{
			// testing function using factor and radius

			const unsigned int layerFactor = RandomI::random(randomGenerator, 2u, 20u);

			unsigned int maximalRadius = (unsigned int)(-1);

			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				maximalRadius = RandomI::random(randomGenerator, 1u, std::max(width, height));
			}

			const unsigned int coarsestLayerRadius = RandomI::random(randomGenerator, 2u, 20u);

			const unsigned int layers = CV::FramePyramid::idealLayers(width, height, invalidWidth, invalidHeight, layerFactor, maximalRadius, coarsestLayerRadius);

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

			CV::FramePyramid framePyramid(FrameType(width, height, pixelFormat, pixelOrigin));

			if (!verifyPyramidOwnership(framePyramid, true /*isValid*/, true /*isOwner*/))
			{
				allSucceeded = false;
			}

			CV::FramePyramid movedFramePyramid(std::move(framePyramid));

			if (!verifyPyramidOwnership(movedFramePyramid, true /*isValid*/, true /*isOwner*/))
			{
				allSucceeded = false;
			}

			if (!verifyPyramidOwnership(framePyramid, false /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}
		}

#if 0 // **TODO** activate once we have switched to Frame in FramePyramid

		{
			// testing pyramid from Frame owning the memory

			const Frame frame(FrameType(width, height, pixelFormat, pixelOrigin));

			CV::FramePyramid framePyramid(frame, true /*copyData*/);

			if (!verifyPyramidOwnership(framePyramid, true /*isValid*/, true /*isOwner*/))
			{
				allSucceeded = false;
			}

			CV::FramePyramid movedFramePyramid(std::move(framePyramid));

			if (!verifyPyramidOwnership(movedFramePyramid, true /*isValid*/, true /*isOwner*/))
			{
				allSucceeded = false;
			}

			if (!verifyPyramidOwnership(framePyramid, false /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}
		}

		{
			// testing pyramid from Frame owning the memory

			const Frame frame(FrameType(width, height, pixelFormat, pixelOrigin));

			CV::FramePyramid framePyramid(frame, false /*copyData*/);

			if (!verifyPyramidOwnership(framePyramid, true /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}

			CV::FramePyramid movedFramePyramid(std::move(framePyramid));

			if (!verifyPyramidOwnership(movedFramePyramid, true /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}

			if (!verifyPyramidOwnership(framePyramid, false /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}
		}
#endif

		{
			// testing pyramid from pyramid and making a copy

			CV::FramePyramid framePyramidSource(FrameType(width, height, pixelFormat, pixelOrigin));

			CV::FramePyramid framePyramid(framePyramidSource, true /*copyData*/);

			if (!verifyPyramidOwnership(framePyramid, true /*isValid*/, true /*isOwner*/))
			{
				allSucceeded = false;
			}

			CV::FramePyramid movedFramePyramid(std::move(framePyramid));

			if (!verifyPyramidOwnership(movedFramePyramid, true /*isValid*/, true /*isOwner*/))
			{
				allSucceeded = false;
			}

			if (!verifyPyramidOwnership(framePyramid, false /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}
		}

		{
			// testing pyramid from pyramid and using the memory only

			CV::FramePyramid framePyramidSource(FrameType(width, height, pixelFormat, pixelOrigin));

			CV::FramePyramid framePyramid(framePyramidSource, false /*copyData*/);

			if (!verifyPyramidOwnership(framePyramid, true /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}

			CV::FramePyramid movedFramePyramid(std::move(framePyramid));

			if (!verifyPyramidOwnership(movedFramePyramid, true /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}

			if (!verifyPyramidOwnership(framePyramid, false /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}
		}

		{
			// testing pyramid from pyramid and making a copy

			CV::FramePyramid framePyramidSource(FrameType(width, height, pixelFormat, pixelOrigin));

			CV::FramePyramid framePyramid = CV::FramePyramid::create8BitPerChannel<true /*tCopyData*/>(framePyramidSource);

			if (!verifyPyramidOwnership(framePyramid, true /*isValid*/, true /*isOwner*/))
			{
				allSucceeded = false;
			}

			CV::FramePyramid movedFramePyramid(std::move(framePyramid));

			if (!verifyPyramidOwnership(movedFramePyramid, true /*isValid*/, true /*isOwner*/))
			{
				allSucceeded = false;
			}

			if (!verifyPyramidOwnership(framePyramid, false /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}
		}

		{
			// testing pyramid from pyramid and using the memory only

			CV::FramePyramid framePyramidSource(FrameType(width, height, pixelFormat, pixelOrigin));

			CV::FramePyramid framePyramid = CV::FramePyramid::create8BitPerChannel<false /*tCopyData*/>(framePyramidSource);

			if (!verifyPyramidOwnership(framePyramid, true /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}

			CV::FramePyramid movedFramePyramid(std::move(framePyramid));

			if (!verifyPyramidOwnership(movedFramePyramid, true /*isValid*/, false /*isOwner*/))
			{
				allSucceeded = false;
			}

			if (!verifyPyramidOwnership(framePyramid, false /*isValid*/, false /*isOwner*/))
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

bool TestFramePyramid::testCreationFramePyramidWithFrame(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int layers = 5u;

	Log::info() << "Testing creation of " << layers << " pyramid layers from Frame:";
	Log::info() << " ";

	bool allSucceeded = true;

	const Indices32 widths =  {640u, 800u, 1280u, 1920u, 3840u};
	const Indices32 heights = {480u, 640u,  720u, 1080u, 2160u};

	for (unsigned int n = 0u; n < widths.size(); ++n)
	{
		Log::info().newLine(n != 0u);

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			allSucceeded = testCreationFramePyramidWithFrame(width, height, channel, layers, testDuration, worker) && allSucceeded;
			Log::info() << " ";
		}
	}

	return allSucceeded;
}

bool TestFramePyramid::testCreationFramePyramid(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	const unsigned int layers = 5u;

	Log::info() << "Testing creation of " << layers << " pyramid layers:";
	Log::info() << " ";

	bool allSucceeded = true;

	const Indices32 widths =  {640u, 800u, 1280u, 1920u, 3840u};
	const Indices32 heights = {480u, 640u,  720u, 1080u, 2160u};

	for (unsigned int n = 0u; n < widths.size(); ++n)
	{
		Log::info().newLine(n != 0u);

		const unsigned int width = widths[n];
		const unsigned int height = heights[n];

		for (unsigned int channel = 1u; channel <= 4u; ++channel)
		{
			allSucceeded = testCreationFramePyramid(width, height, channel, layers, testDuration, worker) && allSucceeded;
			Log::info() << " ";
		}
	}

	return allSucceeded;
}

bool TestFramePyramid::testCreateFramePyramidExtreme()
{
	Log::info() << "Testing creation with extreme parameters";

	bool allSucceeded = true;

	const Indices32 threads = {1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 12u, 32u, 33u, 48u, 64u};

	const Indices32 widths =  {640u, 641u, 640u, 641u, 800u, 1280u, 1920u, 3840u, 3840u};
	const Indices32 heights = {480u, 480u, 481u, 481u, 640u,  720u, 1080u, 2048u, 2160u};

	const unsigned int previousProcessorCores = Processor::get().cores();

	ocean_assert(sizeof(widths) == sizeof(heights));

	for (const unsigned int thread : threads)
	{
		Processor::get().forceCores(thread);

		Worker extremeWorker(Worker::TYPE_ALL_CORES);

		for (unsigned int n = 0u; n < widths.size(); ++n)
		{
			const unsigned int width = widths[n];
			const unsigned int height = heights[n];

			for (unsigned int c = 1u; c <= 4u; ++c)
			{
				LegacyFrame frame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, c), FrameType::ORIGIN_UPPER_LEFT));
				CV::CVUtilities::randomizeFrame(frame);

				const unsigned int layers = CV::FramePyramid::idealLayers(width, height, 0u, 0u);

				for (unsigned int layerIndex = 1u; layerIndex <= layers; ++layerIndex)
				{
					const CV::FramePyramid framePyramid(frame, layerIndex, &extremeWorker);

					if (!validateFramePyramid(Frame(frame, Frame::temporary_ACM_USE_KEEP_LAYOUT), framePyramid, CV::FramePyramid::DM_FILTER_11, layerIndex))
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

	if (Processor::realCores() == previousProcessorCores)
	{
		Processor::get().forceCores(0u);
	}
	else
	{
		Processor::get().forceCores(previousProcessorCores);
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

bool TestFramePyramid::testCreationFramePyramidWithFrame(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int layers, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u && channels >= 1u);
	ocean_assert(layers >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int paddingElements = RandomI::random(randomGenerator, 256u);
			Frame frame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), paddingElements);

			CV::CVUtilities::randomizeFrame(frame, /* skipPaddingArea */ true, &randomGenerator);

			CV::FramePyramid framePyramid;

			performance.start();

			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				framePyramid = CV::FramePyramid(frame, layers, useWorker);
			}
			else
			{
				framePyramid.replace(frame, layers, useWorker);
			}

			ocean_assert(framePyramid.layers() == layers);

			performance.stop();

			if (!validateFramePyramid(frame, framePyramid, CV::FramePyramid::DM_FILTER_11, layers))
			{
				allSucceeded = false;
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

bool TestFramePyramid::testCreationFramePyramid(const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int layers, const double testDuration, Worker& worker)
{
	ocean_assert(width >= 1u && height >= 1u && channels >= 1u);
	ocean_assert(layers >= 1u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for a " << width << "x" << height << " frame with " << channels << " channels:";

	bool allSucceeded = true;

	LegacyFrame frame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT));

	RandomGenerator randomGenerator;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		const Timestamp startTimestamp(true);

		do
		{
			CV::CVUtilities::randomizeFrame(frame, &randomGenerator);

			CV::FramePyramid framePyramid;

			performance.start();

			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				framePyramid = CV::FramePyramid(frame, layers, useWorker);
			}
			else
			{
				framePyramid.replace(frame, layers, useWorker);
			}

			ocean_assert(framePyramid.layers() == layers);

			performance.stop();

			if (!validateFramePyramid(Frame(frame, Frame::temporary_ACM_USE_KEEP_LAYOUT), framePyramid, CV::FramePyramid::DM_FILTER_11, layers))
			{
				allSucceeded = false;
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

bool TestFramePyramid::testConstructFromPyramid(const double testDuration, Worker& worker)
{
	Log::info() << "Testing construction from pyramid with 24 bit and 3 channels:";
	Log::info() << " ";

	const IndexPairs32 sizes =
	{
		IndexPair32(640u, 480u),
		IndexPair32(1920u, 1080u),
		IndexPair32(3840u, 2160u)
	};

	bool allSucceeded = true;

	for (size_t i = 0; i < sizes.size(); ++i)
	{
		if (i != 0u)
		{
			Log::info().newLine();
			Log::info().newLine();
		}

		LegacyFrame frame = LegacyFrame(FrameType(sizes[i].first, sizes[i].second, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame(frame);

		CV::FramePyramid framePyramid = CV::FramePyramid(frame, 2u);
		allSucceeded = testConstructFromPyramid(framePyramid, true,  0u, ALL_LAYERS, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, false, 0u, ALL_LAYERS, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, true,  1u, ALL_LAYERS, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, false, 1u, ALL_LAYERS, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, true,  1u, 4u,         testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, false, 1u, 4u,         testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, true,  0u, 4u,         testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, false, 0u, 4u,         testDuration, worker) && allSucceeded;

		Log::info() << " ";
		Log::info() << " ";

		framePyramid = CV::FramePyramid(frame, ALL_LAYERS);
		allSucceeded = testConstructFromPyramid(framePyramid, true,  0u, ALL_LAYERS, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, false, 0u, ALL_LAYERS, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, true,  1u, ALL_LAYERS, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, false, 1u, ALL_LAYERS, testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, true,  1u, 4u,         testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, false, 1u, 4u,         testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, true,  0u, 4u,         testDuration, worker) && allSucceeded;
		Log::info() << " ";
		allSucceeded = testConstructFromPyramid(framePyramid, false, 0u, 4u,         testDuration, worker) && allSucceeded;
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
				UnorderedIndexSet32 readOnlyLayers;
				UnorderedIndexSet32 ownerLayers;
				UnorderedIndexSet32 outsideMemoryBlockLayers;

				const unsigned int expectedNumberLayers = std::min(determineMaxLayerCount(width, height), layerCount);

				for (unsigned int layerIndex = 0u; layerIndex < expectedNumberLayers; ++layerIndex)
				{
					ownerLayers.emplace(layerIndex);
				}

				LegacyFrame frame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT));
				CV::CVUtilities::randomizeFrame(frame);

				performance.start();
					const CV::FramePyramid framePyramid(frame, layerCount, useWorker);
				performance.stop();

				if (!validateConstructFromFrame(framePyramid, CV::FramePyramid::DM_FILTER_11, Frame(frame, Frame::temporary_ACM_USE_KEEP_LAYOUT), expectedNumberLayers, readOnlyLayers, ownerLayers, outsideMemoryBlockLayers))
				{
					allSucceeded = false;
				}

				const unsigned int paddingElements = RandomI::random(1u, 100u) * RandomI::random(1u);
				Frame paddingFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT), paddingElements);
				paddingFrame.makeContinuous(); // **TODO** workaround until LegacyFrame is not used anymore

				CV::CVUtilities::randomizeFrame(paddingFrame, false);

				const bool copyFirstLayer = RandomI::random(1u) == 0u;

				if (!copyFirstLayer)
				{
					readOnlyLayers.emplace(0u);
					ownerLayers.erase(0u);
					outsideMemoryBlockLayers.emplace(0u);
				}

				if (!validateConstructFromFrame(CV::FramePyramid(paddingFrame.constdata<uint8_t>(), paddingFrame.width(), paddingFrame.height(), paddingFrame.channels(), paddingFrame.pixelOrigin(), layerCount, paddingFrame.paddingElements(), copyFirstLayer, useWorker), CV::FramePyramid::DM_FILTER_11, paddingFrame, expectedNumberLayers, readOnlyLayers, ownerLayers, outsideMemoryBlockLayers))
				{
					allSucceeded = false;
				}

				if (!validateConstructFromFrame(CV::FramePyramid(paddingFrame, layerCount, copyFirstLayer, useWorker), CV::FramePyramid::DM_FILTER_11, paddingFrame, expectedNumberLayers, readOnlyLayers, ownerLayers, outsideMemoryBlockLayers))
				{
					allSucceeded = false;
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

bool TestFramePyramid::testConstructFromPyramid(const CV::FramePyramid& sourcePyramid, const bool copyData, const unsigned int  layerIndex, const unsigned int layerCount, const double testDuration, Worker& worker)
{
	Log::info() << "... with source " << sourcePyramid.finestWidth() << "x" << sourcePyramid.finestHeight() << " and " << sourcePyramid.layers() << " layers, generating " << (layerCount == ALL_LAYERS ? "all" : String::toAString(layerCount)) << " layers starting at " << layerIndex << ":";

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	bool allSucceeded = true;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		CV::FramePyramid framePyramid;

		Timestamp startTimestamp(true);

		do
		{
			const HighPerformanceStatistic::ScopedStatistic scope(performance);

			framePyramid = CV::FramePyramid(sourcePyramid, copyData, layerIndex, layerCount, useWorker);
		}
		while (startTimestamp + testDuration > Timestamp(true));

		if (!validateConstructFromPyramid(framePyramid, sourcePyramid, copyData, layerIndex, layerCount))
		{
			allSucceeded = false;
		}

		framePyramid.clear();

		if (copyData)
		{
			framePyramid = CV::FramePyramid::create8BitPerChannel<true>(sourcePyramid, layerIndex, layerCount, useWorker);
		}
		else
		{
			framePyramid = CV::FramePyramid::create8BitPerChannel<false>(sourcePyramid, layerIndex, layerCount, useWorker);
		}

		if (!validateConstructFromPyramid(framePyramid, sourcePyramid, copyData, layerIndex, layerCount))
		{
			allSucceeded = false;
		}
	}

	Log::info() << "Singlecore Performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 2u) << "ms, first: " << String::toAString(performanceSinglecore.firstMseconds(), 2u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multicore Performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 2u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 2u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 2u) << "ms, first: " << String::toAString(performanceMulticore.firstMseconds(), 2u) << "ms";
		Log::info() << "Multicore boost: Best: " << String::toAString(NumericD::ratio(performanceSinglecore.best(), performanceMulticore.best()), 1u) << "x, worst: " << String::toAString(NumericD::ratio(performanceSinglecore.worst(), performanceMulticore.worst()), 1u) << "x, average: " << String::toAString(NumericD::ratio(performanceSinglecore.average(), performanceMulticore.average()), 1u) << "x, first: " << String::toAString(NumericD::ratio(performanceSinglecore.first(), performanceMulticore.first()), 1u);
	}

	return allSucceeded;
}

bool TestFramePyramid::testReplace11(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing replace8BitPerChannel11()";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 2000u);

		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, {FrameType::FORMAT_Y8, FrameType::FORMAT_YA16, FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32});

		const unsigned int layers = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int expectedLayers = std::min(layers, determineMaxLayerCount(width, height));

		const bool copyFirstLayer = RandomI::random(randomGenerator, 1u) == 0u;

		CV::FramePyramid framePyramid;

		const void* initialPyramidMemory = nullptr;

		for (unsigned int nIteration = 0u; nIteration < 2u; ++nIteration)
		{
			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin), false, &randomGenerator);
			frame.makeContinuous(); // **TODO** temporary workaround until LegacyFrame is still used in FramePyramid

			Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

			const Timestamp timestamp(Random::scalar(randomGenerator, -1000, 1000));

			const FrameType::PixelFormat usePixelFormat = RandomI::random(randomGenerator, {FrameType::FORMAT_UNDEFINED, pixelFormat});

			if (framePyramid.replace8BitPerChannel11(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.channels(), pixelOrigin, layers, frame.paddingElements(), copyFirstLayer, useWorker, usePixelFormat, timestamp))
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
					initialPyramidMemory = framePyramid.memory_.constdata();
				}
				else
				{
					ocean_assert(initialPyramidMemory != nullptr || (framePyramid.layers() == 1u && !copyFirstLayer));

					if (initialPyramidMemory != framePyramid.memory_.constdata())
					{
						// the pyramid should not have allocated a new memory
						allSucceeded = false;
					}
				}

				for (unsigned int n = 0u; n < framePyramid.layers(); ++n)
				{
					if (framePyramid[n].timestamp() != timestamp)
					{
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

		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(newWidth, newHeight, pixelFormat, pixelOrigin), false, &randomGenerator);
		frame.makeContinuous(); // **TODO** temporary workaround until LegacyFrame is still used in FramePyramid

		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		const FrameType::PixelFormat usePixelFormat = RandomI::random(randomGenerator, {FrameType::FORMAT_UNDEFINED, pixelFormat});

		const Timestamp timestamp(Random::scalar(randomGenerator, -1000, 1000));

		if (framePyramid.replace8BitPerChannel11(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.channels(), pixelOrigin, layers, frame.paddingElements(), copyFirstLayer, useWorker, usePixelFormat, timestamp))
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

				if (initialPyramidMemory == framePyramid.memory_.constdata())
				{
					// the pyramid should have allocated a new memory
					allSucceeded = false;
				}
			}

			for (unsigned int n = 0u; n < framePyramid.layers(); ++n)
			{
				if (framePyramid[n].timestamp() != timestamp)
				{
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

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 2000u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 2000u);

		const FrameType::PixelFormat pixelFormat = RandomI::random(randomGenerator, {FrameType::FORMAT_Y8, FrameType::FORMAT_YA16, FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32});

		const unsigned int layers = RandomI::random(randomGenerator, 1u, 100u);
		const unsigned int expectedLayers = std::min(layers, determineMaxLayerCount(width, height));

		const bool copyFirstLayer = RandomI::random(randomGenerator, 1u) == 0u;

		const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});


		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin), false, &randomGenerator);
		frame.makeContinuous(); // **TODO** temporary workaround until LegacyFrame is still used in FramePyramid

		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		Timestamp timestamp(Random::scalar(randomGenerator, -1000, 1000));

		frame.setTimestamp(timestamp);

		FrameType::PixelFormat usePixelFormat = RandomI::random(randomGenerator, {FrameType::FORMAT_UNDEFINED, pixelFormat});

		const bool useFrameObjectConstructor = RandomI::random(randomGenerator, 1u) == 0u;

		CV::FramePyramid framePyramid;

		if (useFrameObjectConstructor)
		{
			framePyramid = CV::FramePyramid(frame, layers, copyFirstLayer, useWorker);
		}
		else
		{
			framePyramid = CV::FramePyramid(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.channels(), pixelOrigin, layers, frame.paddingElements(), copyFirstLayer, useWorker, usePixelFormat, timestamp);
		}

		if (framePyramid.isValid())
		{
			const void* initialPyramidMemory = framePyramid.memory_.constdata();

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

			for (unsigned int n = 0u; n < framePyramid.layers(); ++n)
			{
				if (framePyramid[n].timestamp() != timestamp)
				{
					allSucceeded = false;
				}
			}


			// now, re replace the pyramid

			frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, pixelFormat, pixelOrigin), false, &randomGenerator);
			frame.makeContinuous(); // **TODO** temporary workaround until LegacyFrame is still used in FramePyramid

			useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

			timestamp = Timestamp(Random::scalar(randomGenerator, -1000, 1000));

			usePixelFormat = RandomI::random(randomGenerator, {FrameType::FORMAT_UNDEFINED, pixelFormat});

			if (framePyramid.replace8BitPerChannel11(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.channels(), pixelOrigin, layers, frame.paddingElements(), copyFirstLayer, useWorker, usePixelFormat, timestamp))
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

				if (initialPyramidMemory != framePyramid.memory_.constdata())
				{
					// the pyramid should not have allocated a new memory
					allSucceeded = false;
				}

				for (unsigned int n = 0u; n < framePyramid.layers(); ++n)
				{
					if (framePyramid[n].timestamp() != timestamp)
					{
						allSucceeded = false;
					}
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

		if (RandomI::random(randomGenerator, 1u) == 0u)
		{
			layers = RandomI::random(1u, 10u);
		}

		CV::FramePyramid framePyramid(FrameType(width, height, pixelFormat, pixelOrigin), layers);

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
		const unsigned int frameWidthBytes = frame.planeWidthElements(0u) * FrameType::bytesPerDataType(frame.dataType());

		if (memcmp(frame.constrow<void>(y), framePyramid[0].constrow(y), frameWidthBytes) != 0)
		{
			return false;
		}
	}

	// for the remaining layers we do not need to consider padding elements

	Frame finerLayer(frame, Frame::ACM_USE_KEEP_LAYOUT);

	for (unsigned int n = 1u; n < layers; ++n)
	{
		Frame coarserLayer;

		switch (downsamplingMode)
		{
			case CV::FramePyramid::DM_FILTER_11:
			{
				if (!CV::FrameShrinker::downsampleByTwo11(finerLayer, coarserLayer))
				{
					return false;
				}

				break;
			}

			case CV::FramePyramid::DM_FILTER_14641:
			{
				if (!CV::FrameShrinker::downsampleByTwo14641(finerLayer, coarserLayer))
				{
					return false;
				}

				break;
			}

			default:
				ocean_assert(false && "This should never happen!");
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

		const Frame pyramidLayer(framePyramid[n], Frame::temporary_ACM_USE_KEEP_LAYOUT);

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

	return true;
}

bool TestFramePyramid::validateConstructFromFrame(const CV::FramePyramid& framePyramid, const CV::FramePyramid::DownsamplingMode downsamplingMode, const Frame& frame, const unsigned int numberLayers, const UnorderedIndexSet32& readOnlyLayers, const UnorderedIndexSet32& ownerLayers, const UnorderedIndexSet32& outsideMemoryBlockLayers)
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

	if (!validateFramePyramid(frame, framePyramid, downsamplingMode, numberLayers))
	{
		return false;
	}

	for (unsigned int layerIndex = 0u; layerIndex < numberLayers; ++layerIndex)
	{
		const LegacyFrame& layer = framePyramid[layerIndex];

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

		const size_t layerSizeBytes = layer.size(); // **TODO** switch to Frame-based function once LegacyFrame is removed
		const bool isOwnedByMemoryBlock = framePyramid.memory_.isInside(layer.constdata<void>(), (const uint8_t*)(layer.constdata<void>()) + layerSizeBytes);

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

bool TestFramePyramid::validateConstructFromPyramid(const CV::FramePyramid& framePyramid, const CV::FramePyramid& sourcePyramid, const bool copyData, const unsigned int  layerIndex, const unsigned int layerCount)
{
	unsigned int layerWidth = sourcePyramid.layer(layerIndex).width();
	unsigned int layerHeight = sourcePyramid.layer(layerIndex).height();

	// Evaluate layer count:
	const unsigned int maxLayerCount = copyData ? determineMaxLayerCount(layerWidth, layerHeight) : sourcePyramid.layers() - layerIndex;
	const unsigned int expectedLayerCount = min(layerCount, maxLayerCount);

	if (expectedLayerCount != framePyramid.layers())
	{
		return false;
	}

	bool allLayersAreOwners = copyData;

	unsigned int totalSize = 0u;

	for (unsigned int l = 0u; l < framePyramid.layers(); l++)
	{
		const LegacyFrame& layer = framePyramid.layer(l);

		// If the layer should not exist the current layer is invalid
		if (layerWidth == 0u || layerHeight == 0u || l >= layerCount)
		{
			return false;
		}

		// Test frame type:
		const bool sourceLayerExists = l + layerIndex < sourcePyramid.layers();

		FrameType expectedFrameType;
		if (sourceLayerExists)
		{
			const LegacyFrame& sourceLayer = sourcePyramid.layer(l + layerIndex);
			expectedFrameType =  sourceLayer.frameType();

			// if the pyramid is owner of the image content it must be writable
			if (copyData && layer.isReadOnly())
			{
				return false;
			}

			// if the pyramid is not owner of the image content it will be a read-only pyramid
			if (!copyData && !layer.isReadOnly())
			{
				return false;
			}

			// ensure that both layers have same memory size
			if (layer.size() != sourceLayer.size())
			{
				return false;
			}

			// in any case, the data (copy or reference) must be identical
			if (memcmp(sourceLayer.constdata(), layer.constdata(), sourceLayer.size()) != 0)
			{
				return false;
			}

			// if the data has not been copied both frames must point to the same memory block
			if (!copyData && sourceLayer.constdata() != layer.constdata())
			{
				return false;
			}
		}
		else
		{
			expectedFrameType = FrameType(layerWidth, layerHeight, sourcePyramid.frameType().pixelFormat(), sourcePyramid.frameType().pixelOrigin());
		}

		if (expectedFrameType != layer.frameType())
		{
			return false;
		}

		if (!layer.isOwner() && copyData)
		{
			if (layer.constdata() != framePyramid.memory_.constdata<uint8_t>() + totalSize || layer.size() != expectedFrameType.frameTypeSize())
			{
				return false;
			}

			allLayersAreOwners = false;
		}

		// if a pyramid has been copied it must be writable
		if (copyData && layer.isReadOnly())
		{
			return false;
		}

		totalSize += expectedFrameType.frameTypeSize();

		layerWidth /= 2u;
		layerHeight /= 2u;
	}

	if (allLayersAreOwners)
	{
		if (!framePyramid.memory_.isNull())
		{
			return false;
		}
	}

	const bool validSize = !allLayersAreOwners || size_t(totalSize) == size_t(framePyramid.memory_.size());
	const bool validOwner = framePyramid.isOwner() == copyData;

	return validSize && validOwner;
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
