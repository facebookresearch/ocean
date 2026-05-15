/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestCVUtilities.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestCVUtilities::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("CVUtilities test");
	Log::info() << " ";

	if (selector.shouldRun("definedpixelformats"))
	{
		testResult = testDefinedPixelFormats();
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("mirrorindex"))
	{
		testResult = testMirrorIndex();
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("isborderzero"))
	{
		testResult = testIsBorderZero(testDuration);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("ispaddingmemoryidentical"))
	{
		testResult = testIsPaddingMemoryIdentical(testDuration);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("checkerboard"))
	{
		testResult = testCheckerboardImage(testDuration);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("randomizedframe"))
	{
		testResult = testRandomizedFrame(testDuration);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("randomizedbinarymask"))
	{
		testResult = testRandomizedBinaryMask(testDuration);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("copypixel"))
	{
		testResult = testCopyPixel(testDuration);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("mirrorindexstress"))
	{
		testResult = testMirrorIndexStress(testDuration);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("randomizedframestress"))
	{
		testResult = testRandomizedFrameStress(testDuration);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("copypixelstress"))
	{
		testResult = testCopyPixelStress(testDuration);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("checkerboardedge"))
	{
		testResult = testCheckerboardImageEdge();
		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << testResult;
	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestCVUtilities, DefinedPixelFormats)
{
	EXPECT_TRUE(TestCVUtilities::testDefinedPixelFormats());
}

TEST(TestCVUtilities, MirrorIndex)
{
	EXPECT_TRUE(TestCVUtilities::testMirrorIndex());
}

TEST(TestCVUtilities, IsBorderZero)
{
	EXPECT_TRUE(TestCVUtilities::testIsBorderZero(GTEST_TEST_DURATION));
}

TEST(TestCVUtilities, IsPaddingMemoryIdentical)
{
	EXPECT_TRUE(TestCVUtilities::testIsPaddingMemoryIdentical(GTEST_TEST_DURATION));
}

TEST(TestCVUtilities, CheckerboardImage)
{
	EXPECT_TRUE(TestCVUtilities::testCheckerboardImage(GTEST_TEST_DURATION));
}

TEST(TestCVUtilities, RandomizedFrame)
{
	EXPECT_TRUE(TestCVUtilities::testRandomizedFrame(GTEST_TEST_DURATION));
}

TEST(TestCVUtilities, RandomizedBinaryMask)
{
	EXPECT_TRUE(TestCVUtilities::testRandomizedBinaryMask(GTEST_TEST_DURATION));
}

TEST(TestCVUtilities, CopyPixel)
{
	EXPECT_TRUE(TestCVUtilities::testCopyPixel(GTEST_TEST_DURATION));
}

TEST(TestCVUtilities, MirrorIndexStress)
{
	EXPECT_TRUE(TestCVUtilities::testMirrorIndexStress(GTEST_TEST_DURATION));
}

TEST(TestCVUtilities, RandomizedFrameStress)
{
	EXPECT_TRUE(TestCVUtilities::testRandomizedFrameStress(GTEST_TEST_DURATION));
}

TEST(TestCVUtilities, CopyPixelStress)
{
	EXPECT_TRUE(TestCVUtilities::testCopyPixelStress(GTEST_TEST_DURATION));
}

TEST(TestCVUtilities, CheckerboardImageEdge)
{
	EXPECT_TRUE(TestCVUtilities::testCheckerboardImageEdge());
}

#endif // OCEAN_USE_GTEST

bool TestCVUtilities::testDefinedPixelFormats()
{
	Log::info() << "Testing definedPixelFormats():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const FrameType::PixelFormats baseFormats = CV::CVUtilities::definedPixelFormats();

	OCEAN_EXPECT_FALSE(validation, baseFormats.empty());

	// Each defined format must be valid
	for (const FrameType::PixelFormat& pixelFormat : baseFormats)
	{
		OCEAN_EXPECT_TRUE(validation, pixelFormat != FrameType::FORMAT_UNDEFINED);
	}

	// Adding generic formats should grow the list by exactly that many entries.
	FrameType::PixelFormats extra;
	extra.emplace_back(FrameType::genericPixelFormat<uint8_t, 1u>());
	extra.emplace_back(FrameType::genericPixelFormat<float, 3u>());

	const FrameType::PixelFormats combined = CV::CVUtilities::definedPixelFormats(extra);
	OCEAN_EXPECT_EQUAL(validation, combined.size(), baseFormats.size() + extra.size());

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestCVUtilities::testMirrorIndex()
{
	Log::info() << "Testing mirrorIndex() / mirrorOffset():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	// Inside the valid range, mirrorIndex returns the index unchanged.
	for (unsigned int elements : {1u, 2u, 3u, 7u, 16u, 100u})
	{
		for (unsigned int x = 0u; x < elements; ++x)
		{
			OCEAN_EXPECT_EQUAL(validation, CV::CVUtilities::mirrorIndex(int(x), elements), x);
			OCEAN_EXPECT_EQUAL(validation, CV::CVUtilities::mirrorOffset(int(x), elements), 0);
		}
	}

	// Negative indices reflect: -1 -> 0, -2 -> 1, -3 -> 2.
	for (unsigned int elements : {3u, 5u, 8u, 32u})
	{
		const int half = int(elements) / 2;
		for (int x = -half; x < 0; ++x)
		{
			const unsigned int expected = (unsigned int)(-x) - 1u;
			OCEAN_EXPECT_EQUAL(validation, CV::CVUtilities::mirrorIndex(x, elements), expected);
			OCEAN_EXPECT_EQUAL(validation, CV::CVUtilities::mirrorOffset(x, elements), int(expected) - x);
		}

		// Indices >= elements reflect: elements -> elements-1, elements+1 -> elements-2.
		for (int x = int(elements); x < int(elements) + half; ++x)
		{
			const unsigned int expected = elements * 2u - (unsigned int)(x) - 1u;
			OCEAN_EXPECT_EQUAL(validation, CV::CVUtilities::mirrorIndex(x, elements), expected);
			OCEAN_EXPECT_EQUAL(validation, CV::CVUtilities::mirrorOffset(x, elements), int(expected) - x);
		}
	}

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestCVUtilities::testIsBorderZero(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing isBorderZero():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 2u, 64u);
		const unsigned int height = RandomI::random(randomGenerator, 2u, 64u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 32u);

		Frame frame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), paddingElements);
		frame.setValue(0x00);

		// Initially all zero -> border must be zero.
		OCEAN_EXPECT_TRUE(validation, CV::CVUtilities::isBorderZero(frame));

		// Touch one border pixel -> must report false.
		const unsigned int side = RandomI::random(randomGenerator, 0u, 3u);
		unsigned int xb = 0u;
		unsigned int yb = 0u;
		if (side == 0u) { xb = RandomI::random(randomGenerator, width - 1u); yb = 0u; }
		else if (side == 1u) { xb = RandomI::random(randomGenerator, width - 1u); yb = height - 1u; }
		else if (side == 2u) { xb = 0u; yb = RandomI::random(randomGenerator, height - 1u); }
		else { xb = width - 1u; yb = RandomI::random(randomGenerator, height - 1u); }

		frame.pixel<uint8_t>(xb, yb)[0] = 0xFFu;
		OCEAN_EXPECT_FALSE(validation, CV::CVUtilities::isBorderZero(frame));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestCVUtilities::testIsPaddingMemoryIdentical(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing isPaddingMemoryIdentical():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 64u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 64u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 16u);

		Frame frameA(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);
		Frame frameB(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);

		// Fill image area with same content; padding is whatever raw memory had,
		// but we explicitly initialize padding to zero so we have a deterministic state.
		for (unsigned int y = 0u; y < height; ++y)
		{
			memset(frameA.row<uint8_t>(y), 0, frameA.strideElements());
			memset(frameB.row<uint8_t>(y), 0, frameB.strideElements());
		}

		OCEAN_EXPECT_TRUE(validation, CV::CVUtilities::isPaddingMemoryIdentical(frameA, frameB));

		if (paddingElements > 0u)
		{
			// Modify a padding byte in B -> mismatch
			const unsigned int yMod = RandomI::random(randomGenerator, height - 1u);
			frameB.row<uint8_t>(yMod)[width] = 0xAAu;

			OCEAN_EXPECT_FALSE(validation, CV::CVUtilities::isPaddingMemoryIdentical(frameA, frameB));
		}

		// Different padding -> not identical.
		Frame frameC(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements + 1u);
		OCEAN_EXPECT_FALSE(validation, CV::CVUtilities::isPaddingMemoryIdentical(frameA, frameC));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestCVUtilities::testCheckerboardImage(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing createCheckerboardImage():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int hElements = RandomI::random(randomGenerator, 1u, 8u);
		const unsigned int vElements = RandomI::random(randomGenerator, 1u, 8u);
		const unsigned int colMul = RandomI::random(randomGenerator, 1u, 8u);
		const unsigned int rowMul = RandomI::random(randomGenerator, 1u, 8u);
		const unsigned int width = hElements * colMul;
		const unsigned int height = vElements * rowMul;
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 16u);

		const uint8_t bright = uint8_t(RandomI::random(randomGenerator, 128u, 255u));
		const uint8_t dark = uint8_t(RandomI::random(randomGenerator, 0u, 64u));

		const Frame board = CV::CVUtilities::createCheckerboardImage(width, height, hElements, vElements, paddingElements, bright, dark);

		OCEAN_EXPECT_TRUE(validation, board.isValid());
		OCEAN_EXPECT_EQUAL(validation, board.width(), width);
		OCEAN_EXPECT_EQUAL(validation, board.height(), height);
		OCEAN_EXPECT_EQUAL(validation, board.paddingElements(), paddingElements);

		// Upper-left element must be dark.
		OCEAN_EXPECT_EQUAL(validation, board.constpixel<uint8_t>(0u, 0u)[0], dark);

		// Each element block must be uniformly dark or bright per the parity.
		const unsigned int colsPerEl = width / hElements;
		const unsigned int rowsPerEl = height / vElements;
		for (unsigned int v = 0u; v < vElements; ++v)
		{
			for (unsigned int h = 0u; h < hElements; ++h)
			{
				const uint8_t expected = ((v % 2u) == (h % 2u)) ? dark : bright;
				const unsigned int xc = h * colsPerEl + colsPerEl / 2u;
				const unsigned int yc = v * rowsPerEl + rowsPerEl / 2u;
				OCEAN_EXPECT_EQUAL(validation, board.constpixel<uint8_t>(xc, yc)[0], expected);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestCVUtilities::testRandomizedFrame(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing randomizedFrame():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 64u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 64u);

		const FrameType frameType(width, height, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT);

		const Frame frame = CV::CVUtilities::randomizedFrame(frameType, &randomGenerator);
		OCEAN_EXPECT_TRUE(validation, frame.isValid());
		OCEAN_EXPECT_EQUAL(validation, frame.width(), width);
		OCEAN_EXPECT_EQUAL(validation, frame.height(), height);
		OCEAN_EXPECT_EQUAL(validation, frame.pixelFormat(), frameType.pixelFormat());
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	// Invalid frame type returns invalid frame.
	{
		const Frame invalidFrame = CV::CVUtilities::randomizedFrame(FrameType());
		OCEAN_EXPECT_FALSE(validation, invalidFrame.isValid());
	}

	// Test typed randomizeFrame with explicit min/max.
	{
		Frame frame(FrameType(20u, 20u, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT));
		CV::CVUtilities::randomizeFrame<uint8_t>(frame, uint8_t(50u), uint8_t(60u), true, &randomGenerator);
		for (unsigned int y = 0u; y < frame.height(); ++y)
		{
			for (unsigned int x = 0u; x < frame.width(); ++x)
			{
				const uint8_t v = frame.constpixel<uint8_t>(x, y)[0];
				OCEAN_EXPECT_TRUE(validation, v >= 50u && v <= 60u);
			}
		}
	}

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestCVUtilities::testRandomizedBinaryMask(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing randomizedBinaryMask():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 32u, 256u);
		const unsigned int height = RandomI::random(randomGenerator, 32u, 256u);
		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		const uint8_t nonMaskValue = uint8_t(0xFFu - maskValue);

		const Frame mask = CV::CVUtilities::randomizedBinaryMask(width, height, maskValue, &randomGenerator);

		OCEAN_EXPECT_TRUE(validation, mask.isValid());
		OCEAN_EXPECT_EQUAL(validation, mask.width(), width);
		OCEAN_EXPECT_EQUAL(validation, mask.height(), height);
		OCEAN_EXPECT_EQUAL(validation, mask.pixelFormat(), FrameType::FORMAT_Y8);

		// Every pixel must be either maskValue or nonMaskValue.
		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				const uint8_t v = mask.constpixel<uint8_t>(x, y)[0];
				OCEAN_EXPECT_TRUE(validation, v == maskValue || v == nonMaskValue);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestCVUtilities::testCopyPixel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing copyPixel():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		// Test single-pixel copy
		{
			constexpr unsigned int channels = 4u;
			uint8_t source[channels];
			uint8_t target[channels];

			for (unsigned int n = 0u; n < channels; ++n)
			{
				source[n] = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
				target[n] = 0u;
			}
			CV::CVUtilities::copyPixel<uint8_t, channels>(target, source);

			for (unsigned int n = 0u; n < channels; ++n)
			{
				OCEAN_EXPECT_EQUAL(validation, target[n], source[n]);
			}
		}

		// Test indexed copy in a small buffer
		{
			constexpr unsigned int channels = 3u;
			constexpr unsigned int pixels = 8u;
			uint8_t source[pixels * channels];
			uint8_t target[pixels * channels];

			for (unsigned int n = 0u; n < pixels * channels; ++n)
			{
				source[n] = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
				target[n] = 0u;
			}
			const unsigned int srcIdx = RandomI::random(randomGenerator, pixels - 1u);
			const unsigned int tgtIdx = RandomI::random(randomGenerator, pixels - 1u);

			CV::CVUtilities::copyPixel<uint8_t, channels>(target, source, tgtIdx, srcIdx);
			for (unsigned int c = 0u; c < channels; ++c)
			{
				OCEAN_EXPECT_EQUAL(validation, target[tgtIdx * channels + c], source[srcIdx * channels + c]);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestCVUtilities::testMirrorIndexStress(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Stress testing mirrorIndex():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	uint64_t calls = 0u;

	do
	{
		const unsigned int elements = RandomI::random(randomGenerator, 1u, 4096u);

		const int half = int(elements) / 2;
		const int low = -half;
		const int high = int(elements) + half;

		for (int n = 0; n < 64; ++n)
		{
			const int index = RandomI::random(randomGenerator, low, high);

			const unsigned int mirrored = CV::CVUtilities::mirrorIndex(index, elements);

			// The mirrored index must always be a valid in-range index.
			OCEAN_EXPECT_TRUE(validation, mirrored < elements);

			// And the offset must be self-consistent: mirroredIndex == index + offset.
			const int offset = CV::CVUtilities::mirrorOffset(index, elements);
			OCEAN_EXPECT_EQUAL(validation, int(mirrored), index + offset);

			++calls;
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Calls: " << calls;
	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestCVUtilities::testRandomizedFrameStress(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Stress testing randomizedFrame() over many pixel formats:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const FrameType::PixelFormats pixelFormats = CV::CVUtilities::definedPixelFormats();
	OCEAN_EXPECT_FALSE(validation, pixelFormats.empty());

	const Timestamp startTimestamp(true);

	do
	{
		const FrameType::PixelFormat pixelFormat = pixelFormats[RandomI::random(randomGenerator, (unsigned int)(pixelFormats.size() - 1u))];

		// Some non-generic formats need width/height multiples; pick a generous
		// multiple to avoid invalid frame types being created.
		const unsigned int wMul = std::max(2u, FrameType::widthMultiple(pixelFormat));
		const unsigned int hMul = std::max(2u, FrameType::heightMultiple(pixelFormat));

		const unsigned int width = wMul * RandomI::random(randomGenerator, 1u, 64u);
		const unsigned int height = hMul * RandomI::random(randomGenerator, 1u, 64u);

		const FrameType frameType(width, height, pixelFormat, FrameType::ORIGIN_UPPER_LEFT);
		if (!frameType.isValid())
		{
			continue;
		}

		const Frame frame = CV::CVUtilities::randomizedFrame(frameType, &randomGenerator);
		OCEAN_EXPECT_TRUE(validation, frame.isValid());
		OCEAN_EXPECT_EQUAL(validation, frame.frameType(), frameType);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	// Skip-padding behaviour: padding bytes must be left untouched.
	{
		const unsigned int width = 32u;
		const unsigned int height = 32u;
		const unsigned int padding = 16u;

		Frame frame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), padding);

		// Set whole memory to a sentinel.
		for (unsigned int y = 0u; y < height; ++y)
		{
			memset(frame.row<uint8_t>(y), 0xCDu, frame.strideElements());
		}

		// Randomize but preserve padding.
		CV::CVUtilities::randomizeFrame(frame, true /* skipPaddingArea */, &randomGenerator);

		for (unsigned int y = 0u; y < height; ++y)
		{
			const uint8_t* row = frame.constrow<uint8_t>(y);
			for (unsigned int p = 0u; p < padding; ++p)
			{
				OCEAN_EXPECT_EQUAL(validation, row[width + p], uint8_t(0xCDu));
			}
		}
	}

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestCVUtilities::testCopyPixelStress(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Stress testing copyPixel() patch overload:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		constexpr unsigned int channels = 3u;

		const unsigned int sourceWidth = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int sourceHeight = RandomI::random(randomGenerator, 1u, 1080u);
		const unsigned int targetWidth = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int targetHeight = RandomI::random(randomGenerator, 1u, 1080u);

		const unsigned int sourcePadding = RandomI::random(randomGenerator, 0u, 32u);
		const unsigned int targetPadding = RandomI::random(randomGenerator, 0u, 32u);

		Frame source(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat<uint8_t, channels>(), FrameType::ORIGIN_UPPER_LEFT), sourcePadding);
		Frame target(FrameType(targetWidth, targetHeight, FrameType::genericPixelFormat<uint8_t, channels>(), FrameType::ORIGIN_UPPER_LEFT), targetPadding);

		CV::CVUtilities::randomizeFrame(source, true, &randomGenerator);
		CV::CVUtilities::randomizeFrame(target, true, &randomGenerator);

		const unsigned int xs = RandomI::random(randomGenerator, sourceWidth - 1u);
		const unsigned int ys = RandomI::random(randomGenerator, sourceHeight - 1u);
		const unsigned int xt = RandomI::random(randomGenerator, targetWidth - 1u);
		const unsigned int yt = RandomI::random(randomGenerator, targetHeight - 1u);

		uint8_t expected[channels];
		for (unsigned int c = 0u; c < channels; ++c)
		{
			expected[c] = source.constpixel<uint8_t>(xs, ys)[c];
		}

		CV::CVUtilities::copyPixel<uint8_t, channels>(target.data<uint8_t>(), source.constdata<uint8_t>(), xt, yt, xs, ys, targetWidth, sourceWidth, targetPadding, sourcePadding);

		for (unsigned int c = 0u; c < channels; ++c)
		{
			OCEAN_EXPECT_EQUAL(validation, target.constpixel<uint8_t>(xt, yt)[c], expected[c]);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestCVUtilities::testCheckerboardImageEdge()
{
	Log::info() << "Edge case test for createCheckerboardImage():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	// Smallest possible checkerboard: 1x1 with 1 element each direction. Single dark pixel.
	{
		const Frame board = CV::CVUtilities::createCheckerboardImage(1u, 1u, 1u, 1u, 0u);
		OCEAN_EXPECT_TRUE(validation, board.isValid());
		OCEAN_EXPECT_EQUAL(validation, board.constpixel<uint8_t>(0u, 0u)[0], uint8_t(0x00u));
	}

	// 2x2 board with 2 horizontal/vertical elements: alternates dark and bright.
	{
		const Frame board = CV::CVUtilities::createCheckerboardImage(2u, 2u, 2u, 2u, 0u, 0xFFu, 0x00u);
		OCEAN_EXPECT_EQUAL(validation, board.constpixel<uint8_t>(0u, 0u)[0], uint8_t(0x00u));
		OCEAN_EXPECT_EQUAL(validation, board.constpixel<uint8_t>(1u, 0u)[0], uint8_t(0xFFu));
		OCEAN_EXPECT_EQUAL(validation, board.constpixel<uint8_t>(0u, 1u)[0], uint8_t(0xFFu));
		OCEAN_EXPECT_EQUAL(validation, board.constpixel<uint8_t>(1u, 1u)[0], uint8_t(0x00u));
	}

	// Single-element board with large dimensions and padding.
	{
		const Frame board = CV::CVUtilities::createCheckerboardImage(64u, 64u, 1u, 1u, 7u, 0xFFu, 0x40u);
		OCEAN_EXPECT_TRUE(validation, board.isValid());
		OCEAN_EXPECT_EQUAL(validation, board.paddingElements(), 7u);

		// All pixels should be dark since there is just one element which is dark.
		for (unsigned int y = 0u; y < board.height(); ++y)
		{
			for (unsigned int x = 0u; x < board.width(); ++x)
			{
				OCEAN_EXPECT_EQUAL(validation, board.constpixel<uint8_t>(x, y)[0], uint8_t(0x40u));
			}
		}
	}

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

}

}

}
