/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/testadvanced/TestAdvancedFrameChannels.h"

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"

#include "ocean/cv/advanced/AdvancedFrameChannels.h"

#include "ocean/base/RandomI.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

namespace TestAdvanced
{

bool TestAdvancedFrameChannels::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& /*worker*/)
{
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Advanced frame channels test:   ---";
	Log::info() << " ";

	allSucceeded = testSeparateTo1Channel(width, height, testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testZipChannels(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Advanced frame channels test succeeded.";
	}
	else
	{
		Log::info() << "Advanced frame channels testFAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestAdvancedFrameChannels, SeparateTo1Channel_1920x1080)
{
	EXPECT_TRUE(TestAdvancedFrameChannels::testSeparateTo1Channel(1920u, 1080u, GTEST_TEST_DURATION));
}

TEST(TestAdvancedFrameChannels, ZipChannels_1920x1080)
{
	EXPECT_TRUE(TestAdvancedFrameChannels::testZipChannels(1920u, 1080u, GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestAdvancedFrameChannels::testSeparateTo1Channel(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing separate to 1 channel for " << width << "x" << height << " images:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<uint8_t, uint8_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<int16_t, int16_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<float, float>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<float, float>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<uint64_t, uint64_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<float, uint8_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testSeparateTo1Channel<uint8_t, float>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Separate to 1 channel test succeeded.";
	}
	else
	{
		Log::info() << "Separate to 1 channel test FAILED!";
	}

	return allSucceeded;
}

bool TestAdvancedFrameChannels::testZipChannels(const unsigned int width, const unsigned int height, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing zip channels for " << width << "x" << height << " images:";
	Log::info() << " ";

	bool allSucceeded = true;

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testZipChannels<uint8_t, uint8_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testZipChannels<int16_t, int16_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testZipChannels<float, float>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testZipChannels<uint64_t, uint64_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testZipChannels<float, uint8_t>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	for (unsigned int channels = 2u; channels <= 5u; ++channels)
	{
		allSucceeded = testZipChannels<uint8_t, float>(width, height, channels, testDuration) && allSucceeded;
		Log::info() << " ";
	}

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Zip channels test succeeded.";
	}
	else
	{
		Log::info() << "Zip channels test FAILED!";
	}

	return allSucceeded;
}

template <typename TSource, typename TTarget>
bool TestAdvancedFrameChannels::testSeparateTo1Channel(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for data type " << TypeNamer::name<TSource>() << " -> " << TypeNamer::name<TTarget>() << ", with " << channels << " channels:";

	bool allSucceeded = true;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceNaive;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			const unsigned int sourceChannels = channels;
			constexpr unsigned int targetChannels = 1u;

			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<TSource>(sourceChannels);
			const FrameType::PixelFormat targetPixelFormat = FrameType::genericPixelFormat<TTarget>(targetChannels);

			const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 1280u);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 720u);

			const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 0u, 1u);

			Frame sourceFrame(FrameType(testWidth, testHeight, sourcePixelFormat, pixelOrigin), sourcePaddingElements);

			if constexpr (std::is_floating_point<TSource>::value)
			{
				// ensuring that we have a value range of [0, 1] for floating point elements

				TSource* sourceData = sourceFrame.data<TSource>();

				for (unsigned int n = 0u; n < sourceFrame.height() * sourceFrame.strideElements(0u); ++n)
				{
					sourceData[n] = RandomT<TSource>::scalar(randomGenerator, TSource(0), TSource(1));
				}
			}
			else
			{
				CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
			}

			Frames targetFrames(sourceChannels);
			Frames copyTargetFrames(sourceChannels);

			std::vector<TTarget*> targetPointers(sourceChannels);
			Indices32 targetsPaddingElements(sourceChannels);

			bool allTargetPaddingElementsZero = true;

			for (unsigned int c = 0u; c < sourceChannels; ++c)
			{
				const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 0u, 1u);

				targetFrames[c] = Frame(FrameType(sourceFrame, targetPixelFormat), targetPaddingElements);
				CV::CVUtilities::randomizeFrame(targetFrames[c], false, &randomGenerator);

				targetPointers[c] = targetFrames[c].data<TTarget>();
				targetsPaddingElements[c] = targetPaddingElements;

				if (targetPaddingElements != 0u)
				{
					allTargetPaddingElementsZero = false;
				}

				copyTargetFrames[c] = Frame(targetFrames[c], Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);
			}

			if (allTargetPaddingElementsZero)
			{
				allTargetPaddingElementsZero = RandomI::random(randomGenerator, 0u, 1u) == 0u; // we also want to test this case
			}

			TSource sourceFactor = TSource(0);
			TTarget targetFactor = TTarget(0);

			if constexpr (std::is_floating_point<TSource>::value && !std::is_floating_point<TTarget>::value)
			{
				sourceFactor = TSource(255.0);
				targetFactor = TTarget(1u);
			}
			else if constexpr (!std::is_floating_point<TSource>::value && std::is_floating_point<TTarget>::value)
			{
				sourceFactor = TSource(1u);
				targetFactor = TTarget(1.0 / 255.0);
			}
			else if constexpr (std::is_floating_point<TSource>::value && std::is_floating_point<TTarget>::value)
			{
				sourceFactor = RandomT<TSource>::scalar(randomGenerator, TSource(1.001), TSource(2));
				targetFactor = RandomT<TTarget>::scalar(randomGenerator, TTarget(1.001), TTarget(2));
			}
			else
			{
				ocean_assert(!std::is_floating_point<TSource>::value && !std::is_floating_point<TTarget>::value);

				const Indices32 values = {1u, 2u};
				const unsigned int index = RandomI::random(randomGenerator, 1u);

				sourceFactor = TSource(values[index]);
				targetFactor = TTarget(values[(index + 1u) % 2u]);
			}

			ocean_assert(sourceFactor != TSource(0) && targetFactor != TTarget(0));

			if (RandomI::random(randomGenerator, 1u) == 0u)
			{
				CV::Advanced::AdvancedFrameChannels::separateTo1Channel<TSource, TTarget>(sourceFrame.constdata<TSource>(), targetPointers.data(), sourceFrame.width(), sourceFrame.height(), sourceFrame.channels(), sourceFactor, targetFactor, sourcePaddingElements, allTargetPaddingElementsZero ? nullptr : targetsPaddingElements.data());
			}
			else
			{
				performance.startIf(performanceIteration);

				switch (sourceChannels)
				{
					case 1u:
						CV::Advanced::AdvancedFrameChannels::separateTo1Channel<TSource>(sourceFrame.constdata<TSource>(), {targetFrames[0].data<TTarget>()}, sourceFrame.width(), sourceFrame.height(), sourceFactor, targetFactor, sourcePaddingElements, {targetsPaddingElements[0]});
						break;

					case 2u:
						CV::Advanced::AdvancedFrameChannels::separateTo1Channel<TSource>(sourceFrame.constdata<TSource>(), {targetFrames[0].data<TTarget>(), targetFrames[1].data<TTarget>()}, sourceFrame.width(), sourceFrame.height(), sourceFactor, targetFactor, sourcePaddingElements, {targetsPaddingElements[0], targetsPaddingElements[1]});
						break;

					case 3u:
						CV::Advanced::AdvancedFrameChannels::separateTo1Channel<TSource>(sourceFrame.constdata<TSource>(), {targetFrames[0].data<TTarget>(), targetFrames[1].data<TTarget>(), targetFrames[2].data<TTarget>()}, sourceFrame.width(), sourceFrame.height(), sourceFactor, targetFactor, sourcePaddingElements, {targetsPaddingElements[0], targetsPaddingElements[1], targetsPaddingElements[2]});
						break;

					case 4u:
						CV::Advanced::AdvancedFrameChannels::separateTo1Channel<TSource>(sourceFrame.constdata<TSource>(), {targetFrames[0].data<TTarget>(), targetFrames[1].data<TTarget>(), targetFrames[2].data<TTarget>(), targetFrames[3].data<TTarget>()}, sourceFrame.width(), sourceFrame.height(), sourceFactor, targetFactor,  sourcePaddingElements, {targetsPaddingElements[0], targetsPaddingElements[1], targetsPaddingElements[2], targetsPaddingElements[3]});
						break;

					case 5u:
						CV::Advanced::AdvancedFrameChannels::separateTo1Channel<TSource>(sourceFrame.constdata<TSource>(), {targetFrames[0].data<TTarget>(), targetFrames[1].data<TTarget>(), targetFrames[2].data<TTarget>(), targetFrames[3].data<TTarget>(), targetFrames[4].data<TTarget>()}, sourceFrame.width(), sourceFrame.height(), sourceFactor, targetFactor, sourcePaddingElements, {targetsPaddingElements[0], targetsPaddingElements[1], targetsPaddingElements[2], targetsPaddingElements[3], targetsPaddingElements[4]});
						break;

					default:
						ocean_assert(false && "This should never happen!");
						allSucceeded = false;
						break;
				}

				performance.stopIf(performanceIteration);
			}

			for (size_t n = 0; n < targetFrames.size(); ++n)
			{
				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrames[n], copyTargetFrames[n]))
				{
					ocean_assert(false && "Invalid padding memory!");
					allSucceeded = false;
					break;
				}
			}

			for (unsigned int c = 0u; c < sourceFrame.channels(); ++c)
			{
				const Frame& targetFrame = targetFrames[c];
				ocean_assert(targetFrame.channels() == 1u);

				for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
				{
					for (unsigned int x = 0u; x < sourceFrame.width(); ++x)
					{
						const TSource sourceValue = sourceFrame.constpixel<TSource>(x, y)[c];
						const TTarget targetValue = targetFrame.constpixel<TTarget>(x, y)[0];

						const TSource intermediateSourceValue = sourceValue * sourceFactor;
						const TTarget value = TTarget(intermediateSourceValue) * targetFactor;

						const TTarget controlValue = TTarget(sourceValue * sourceFactor) * targetFactor;
						ocean_assert_and_suppress_unused(value == controlValue, controlValue);

						if (targetValue != value)
						{
							allSucceeded = false;
						}
					}
				}
			}

			if (performanceIteration)
			{
				// testing a naive (but fair) implementation, in which the target frame is provided as one big memory block

				Frame targetFramesAsBlock(FrameType(targetFrames[0], sourceFrame.width(), sourceFrame.height() * sourceFrame.channels()));
				CV::CVUtilities::randomizeFrame(targetFramesAsBlock, false, &randomGenerator);

				const unsigned int pixels = sourceFrame.width() * sourceFrame.height();

				TTarget* const targetFrame = targetFramesAsBlock.data<TTarget>();

				const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceNaive);

				for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
				{
					for (unsigned int x = 0u; x < sourceFrame.width(); ++x)
					{
						const unsigned int pixelIndex = y * sourceFrame.width() + x;

						const TSource* const sourcePixel = sourceFrame.constpixel<TSource>(x, y);

						for (unsigned int n = 0; n < sourceFrame.channels(); ++n)
						{
							targetFrame[pixels * n + pixelIndex] = TTarget(sourcePixel[n] * sourceFactor) * targetFactor;
						}
					}
				}

#ifdef OCEAN_DEBUG
				for (unsigned int c = 0u; c < sourceFrame.channels(); ++c)
				{
					for (unsigned int y = 0u; y < sourceFrame.height(); ++y)
					{
						for (unsigned int x = 0u; x < sourceFrame.width(); ++x)
						{
							const TSource sourceValue = sourceFrame.constpixel<TSource>(x, y)[c];
							const TSource intermediateSourceValue = sourceValue * sourceFactor;
							const TTarget value = TTarget(intermediateSourceValue) * targetFactor;

							if (targetFramesAsBlock.constpixel<TTarget>(x, y + c * sourceFrame.height())[0] != value)
							{
								allSucceeded = false;
							}
						}
					}
				}
#endif
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true) || performance.measurements() == 0u);

	Log::info() << "Naive: Best: " << String::toAString(performanceNaive.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceNaive.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceNaive.averageMseconds(), 3u) << "ms";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 3u) << "ms, average: " << String::toAString(performance.averageMseconds(), 3u) << "ms";

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

template <typename TSource, typename TTarget>
bool TestAdvancedFrameChannels::testZipChannels(const unsigned int width, const unsigned int height, const unsigned int channels, const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for data type " << TypeNamer::name<TSource>() << " -> " << TypeNamer::name<TTarget>() << ", with " << channels << " channels:";

	bool allSucceeded = true;

	HighPerformanceStatistic performance;
	HighPerformanceStatistic performanceNaive;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			constexpr unsigned int sourceChannels = 1u;
			const unsigned int targetChannels = channels;

			const FrameType::PixelOrigin pixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

			const FrameType::PixelFormat sourcePixelFormat = FrameType::genericPixelFormat<TSource>(sourceChannels);
			const FrameType::PixelFormat targetPixelFormat = FrameType::genericPixelFormat<TTarget>(targetChannels);

			const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 1u, 1280u);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 1u, 720u);

			Frames sourceFrames;

			for (unsigned int n = 0u; n < targetChannels; ++n)
			{
				const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
				Frame sourceFrame(FrameType(testWidth, testHeight, sourcePixelFormat, pixelOrigin), paddingElements);

				if constexpr (std::is_floating_point<TSource>::value)
				{
					// ensuring that we have a value range of [0, 1] for floating point elements

					TSource* sourceData = sourceFrame.data<TSource>();

					for (unsigned int i = 0u; i < sourceFrame.height() * sourceFrame.strideElements(0u); ++i)
					{
						sourceData[i] = RandomT<TSource>::scalar(randomGenerator, TSource(0), TSource(1));
					}
				}
				else
				{
					CV::CVUtilities::randomizeFrame(sourceFrame, false, &randomGenerator);
				}

				sourceFrames.emplace_back(std::move(sourceFrame));
			}

			Frame targetFrame;
			Frame copyTargetFrame;

			const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			targetFrame = Frame(FrameType(testWidth, testHeight, targetPixelFormat, pixelOrigin), paddingElements);

			CV::CVUtilities::randomizeFrame(targetFrame, false, &randomGenerator);

			copyTargetFrame = Frame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			TSource sourceFactor = TSource(0);
			TTarget targetFactor = TTarget(0);

			if constexpr (std::is_floating_point<TSource>::value && !std::is_floating_point<TTarget>::value)
			{
				sourceFactor = TSource(255.0);
				targetFactor = TTarget(1u);
			}
			else if constexpr (!std::is_floating_point<TSource>::value && std::is_floating_point<TTarget>::value)
			{
				sourceFactor = TSource(1u);
				targetFactor = TTarget(1.0 / 255.0);
			}
			else if constexpr (std::is_floating_point<TSource>::value && std::is_floating_point<TTarget>::value)
			{
				sourceFactor = RandomT<TSource>::scalar(randomGenerator, TSource(1.001), TSource(2));
				targetFactor = RandomT<TTarget>::scalar(randomGenerator, TTarget(1.001), TTarget(2));
			}
			else
			{
				ocean_assert(!std::is_floating_point<TSource>::value && !std::is_floating_point<TTarget>::value);

				const Indices32 values = {1u, 2u};
				const unsigned int index = RandomI::random(randomGenerator, 1u);

				sourceFactor = TSource(values[index]);
				targetFactor = TTarget(values[(index + 1u) % 2u]);
			}

			ocean_assert(sourceFactor != TSource(0) && targetFactor != TTarget(0));

			for (const bool useInitializerList : {true, false})
			{
				if (useInitializerList)
				{
					switch (targetChannels)
					{
						case 1u:
						{
							performance.startIf(performanceIteration);
								CV::Advanced::AdvancedFrameChannels::zipChannels<TSource, TTarget>({sourceFrames[0].constdata<TSource>()}, targetFrame.data<TTarget>(), targetFrame.width(), targetFrame.height(), sourceFactor, targetFactor, {sourceFrames[0].paddingElements()}, targetFrame.paddingElements());
							performance.stopIf(performanceIteration);

							break;
						}

						case 2u:
						{
							performance.startIf(performanceIteration);
								CV::Advanced::AdvancedFrameChannels::zipChannels<TSource, TTarget>({sourceFrames[0].constdata<TSource>(), sourceFrames[1].constdata<TSource>()}, targetFrame.data<TTarget>(), targetFrame.width(), targetFrame.height(), sourceFactor, targetFactor, {sourceFrames[0].paddingElements(), sourceFrames[1].paddingElements()}, targetFrame.paddingElements());
							performance.stopIf(performanceIteration);

							break;
						}

						case 3u:
						{
							performance.startIf(performanceIteration);
								CV::Advanced::AdvancedFrameChannels::zipChannels<TSource, TTarget>({sourceFrames[0].constdata<TSource>(), sourceFrames[1].constdata<TSource>(), sourceFrames[2].constdata<TSource>()}, targetFrame.data<TTarget>(), targetFrame.width(), targetFrame.height(), sourceFactor, targetFactor, {sourceFrames[0].paddingElements(), sourceFrames[1].paddingElements(), sourceFrames[2].paddingElements()}, targetFrame.paddingElements());
							performance.stopIf(performanceIteration);

							break;
						}

						case 4u:
						{
							performance.startIf(performanceIteration);
								CV::Advanced::AdvancedFrameChannels::zipChannels<TSource, TTarget>({sourceFrames[0].constdata<TSource>(), sourceFrames[1].constdata<TSource>(), sourceFrames[2].constdata<TSource>(), sourceFrames[3].constdata<TSource>()}, targetFrame.data<TTarget>(), targetFrame.width(), targetFrame.height(), sourceFactor, targetFactor, {sourceFrames[0].paddingElements(), sourceFrames[1].paddingElements(), sourceFrames[2].paddingElements(), sourceFrames[3].paddingElements()}, targetFrame.paddingElements());
							performance.stopIf(performanceIteration);

							break;
						}

						case 5u:
						{
							performance.startIf(performanceIteration);
								CV::Advanced::AdvancedFrameChannels::zipChannels<TSource, TTarget>({sourceFrames[0].constdata<TSource>(), sourceFrames[1].constdata<TSource>(), sourceFrames[2].constdata<TSource>(), sourceFrames[3].constdata<TSource>(), sourceFrames[4].constdata<TSource>()}, targetFrame.data<TTarget>(), targetFrame.width(), targetFrame.height(), sourceFactor, targetFactor, {sourceFrames[0].paddingElements(), sourceFrames[1].paddingElements(), sourceFrames[2].paddingElements(), sourceFrames[3].paddingElements(), sourceFrames[4].paddingElements()}, targetFrame.paddingElements());
							performance.stopIf(performanceIteration);

							break;
						}

						default:
						{
							ocean_assert(false && "This should never happen!");

							allSucceeded = false;
							break;
						}
					}
				}
				else
				{
					std::vector<const TSource*> sourcePointers;
					sourcePointers.reserve(targetChannels);

					for (const Frame& sourceFrame : sourceFrames)
					{
						sourcePointers.emplace_back(sourceFrame.constdata<TSource>());
					}

					Indices32 sourcePaddingElements;
					sourcePaddingElements.reserve(targetChannels);

					for (const Frame& sourceFrame : sourceFrames)
					{
						sourcePaddingElements.emplace_back(sourceFrame.paddingElements());
					}

					CV::Advanced::AdvancedFrameChannels::zipChannels<TSource, TTarget>(sourcePointers.data(), targetFrame.data<TTarget>(), targetFrame.width(), targetFrame.height(), targetChannels, sourceFactor, targetFactor, sourcePaddingElements.data(), targetFrame.paddingElements());
				}

				if (copyTargetFrame.isValid())
				{
					if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
					{
						ocean_assert(false && "Invalid padding memory!");
						return false;
					}
				}

				for (unsigned int c = 0u; c < targetFrame.channels(); ++c)
				{
					const Frame& sourceFrame = sourceFrames[c];
					ocean_assert(sourceFrame.channels() == 1u);

					for (unsigned int y = 0u; y < targetFrame.height(); ++y)
					{
						for (unsigned int x = 0u; x < targetFrame.width(); ++x)
						{
							const TSource sourceValue = sourceFrame.constpixel<TSource>(x, y)[0];
							const TTarget targetValue = targetFrame.constpixel<TTarget>(x, y)[c];

							const TSource intermediateSourceValue = sourceValue * sourceFactor;
							const TTarget value = TTarget(intermediateSourceValue) * targetFactor;

							const TTarget controlValue = TTarget(sourceValue * sourceFactor) * targetFactor;
							ocean_assert_and_suppress_unused(value == controlValue, controlValue);

							if (targetValue != value)
							{
								allSucceeded = false;
							}
						}
					}
				}
			}

			if (performanceIteration)
			{
				// testing a naive (but fair) implementation, in which the source frame is provided as one big memory block

				Frame sourceFramesAsBlock(FrameType(sourceFrames[0], targetFrame.width(), targetFrame.height() * targetFrame.channels()));

				if constexpr (std::is_floating_point<TSource>::value)
				{
					// ensuring that we have a value range of [0, 1] for floating point elements

					TSource* sourceData = sourceFramesAsBlock.data<TSource>();

					for (unsigned int i = 0u; i < sourceFramesAsBlock.height() * sourceFramesAsBlock.strideElements(0u); ++i)
					{
						sourceData[i] = RandomT<TSource>::scalar(randomGenerator, TSource(0), TSource(1));
					}
				}
				else
				{
					CV::CVUtilities::randomizeFrame(sourceFramesAsBlock, false, &randomGenerator);
				}

				const unsigned int pixels = targetFrame.width() * targetFrame.height();

				const TSource* sourceFrame = sourceFramesAsBlock.constdata<TSource>();

				const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performanceNaive);

				for (unsigned int y = 0u; y < targetFrame.height(); ++y)
				{
					for (unsigned int x = 0u; x < targetFrame.width(); ++x)
					{
						const unsigned int pixelIndex = y * targetFrame.width() + x;

						TTarget* const targetPixel = targetFrame.pixel<TTarget>(x, y);

						for (unsigned int n = 0; n < targetFrame.channels(); ++n)
						{
							targetPixel[n] = TTarget(sourceFrame[pixels * n + pixelIndex] * sourceFactor) * targetFactor;
						}
					}
				}

#ifdef OCEAN_DEBUG
				for (unsigned int c = 0u; c < targetFrame.channels(); ++c)
				{
					for (unsigned int y = 0u; y < targetFrame.height(); ++y)
					{
						for (unsigned int x = 0u; x < targetFrame.width(); ++x)
						{
							const TSource sourceValue = sourceFramesAsBlock.constpixel<TSource>(x, y + c * targetFrame.height())[0];
							const TSource intermediateSourceValue = sourceValue * sourceFactor;
							const TTarget value = TTarget(intermediateSourceValue) * targetFactor;

							if (targetFrame.constpixel<TTarget>(x, y)[c] != value)
							{
								allSucceeded = false;
							}
						}
					}
				}
#endif
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true) || performance.measurements() == 0u);

	Log::info() << "Naive: Best: " << String::toAString(performanceNaive.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceNaive.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceNaive.averageMseconds(), 3u) << "ms";
	Log::info() << "Performance: Best: " << String::toAString(performance.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performance.worstMseconds(), 3u) << "ms, average: " << String::toAString(performance.averageMseconds(), 3u) << "ms";

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

}

}

}

}
