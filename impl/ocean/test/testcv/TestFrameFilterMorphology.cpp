/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilterMorphology.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/FrameFilterDilation.h"
#include "ocean/cv/FrameFilterErosion.h"
#include "ocean/cv/FrameFilterMorphology.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilterMorphology::test(const double testDuration, Worker& worker, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("FrameFilterMorphology test");
	Log::info() << " ";

	if (selector.shouldRun("openmasksquare3"))
	{
		testResult = testOpenMaskSquare3(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("openmasksquare5"))
	{
		testResult = testOpenMaskSquare5(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("closemasksquare3"))
	{
		testResult = testCloseMaskSquare3(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("closemasksquare5"))
	{
		testResult = testCloseMaskSquare5(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("idempotentonfullframe"))
	{
		testResult = testIdempotentOnFullFrame();
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("emptyframestaysempty"))
	{
		testResult = testEmptyFrameStaysEmpty();
		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << testResult;
	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilterMorphology, OpenMaskSquare3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMorphology::testOpenMaskSquare3(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterMorphology, OpenMaskSquare5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMorphology::testOpenMaskSquare5(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterMorphology, CloseMaskSquare3)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMorphology::testCloseMaskSquare3(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterMorphology, CloseMaskSquare5)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilterMorphology::testCloseMaskSquare5(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameFilterMorphology, IdempotentOnFullFrame)
{
	EXPECT_TRUE(TestFrameFilterMorphology::testIdempotentOnFullFrame());
}

TEST(TestFrameFilterMorphology, EmptyFrameStaysEmpty)
{
	EXPECT_TRUE(TestFrameFilterMorphology::testEmptyFrameStaysEmpty());
}

#endif // OCEAN_USE_GTEST

namespace
{

/**
 * Helper that creates a random binary mask Frame with the given dimensions
 * and padding. Mask pixels take `maskValue`; non-mask pixels take 255 - maskValue
 * (so it stays a recognisably 8-bit binary mask).
 */
Frame randomBinaryMask(RandomGenerator& randomGenerator, const unsigned int width, const unsigned int height, const unsigned int paddingElements, const uint8_t maskValue)
{
	Frame mask(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), paddingElements);

	const uint8_t nonMask = uint8_t(255u - maskValue);

	for (unsigned int y = 0u; y < height; ++y)
	{
		uint8_t* row = mask.row<uint8_t>(y);
		for (unsigned int x = 0u; x < width; ++x)
		{
			row[x] = (RandomI::random(randomGenerator, 1u) == 0u) ? maskValue : nonMask;
		}
	}

	return mask;
}

} // namespace

bool TestFrameFilterMorphology::testOpenMaskSquare3(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing openMask<MF_SQUARE_3>():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 4u, 64u);
		const unsigned int height = RandomI::random(randomGenerator, 4u, 64u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 16u);
		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 0u, 255u));

		Frame mask = randomBinaryMask(randomGenerator, width, height, paddingElements, maskValue);

		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;

		CV::FrameFilterMorphology::openMask<CV::FrameFilterMorphology::MF_SQUARE_3>(mask.data<uint8_t>(), width, height, paddingElements, maskValue, w);

		// Mask must still be valid 8-bit Y8.
		OCEAN_EXPECT_TRUE(validation, mask.isValid());
		OCEAN_EXPECT_EQUAL(validation, mask.width(), width);
		OCEAN_EXPECT_EQUAL(validation, mask.height(), height);
		OCEAN_EXPECT_TRUE(validation, mask.pixelFormat() == FrameType::FORMAT_Y8);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrameFilterMorphology::testOpenMaskSquare5(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing openMask<MF_SQUARE_5>():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 6u, 64u);
		const unsigned int height = RandomI::random(randomGenerator, 6u, 64u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 16u);
		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 0u, 255u));

		Frame mask = randomBinaryMask(randomGenerator, width, height, paddingElements, maskValue);

		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;

		CV::FrameFilterMorphology::openMask<CV::FrameFilterMorphology::MF_SQUARE_5>(mask.data<uint8_t>(), width, height, paddingElements, maskValue, w);

		OCEAN_EXPECT_TRUE(validation, mask.isValid());
		OCEAN_EXPECT_EQUAL(validation, mask.width(), width);
		OCEAN_EXPECT_EQUAL(validation, mask.height(), height);
		OCEAN_EXPECT_TRUE(validation, mask.pixelFormat() == FrameType::FORMAT_Y8);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrameFilterMorphology::testCloseMaskSquare3(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing closeMask<MF_SQUARE_3>():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 4u, 64u);
		const unsigned int height = RandomI::random(randomGenerator, 4u, 64u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 16u);
		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 0u, 255u));

		Frame mask = randomBinaryMask(randomGenerator, width, height, paddingElements, maskValue);

		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;

		CV::FrameFilterMorphology::closeMask<CV::FrameFilterMorphology::MF_SQUARE_3>(mask.data<uint8_t>(), width, height, paddingElements, maskValue, w);

		OCEAN_EXPECT_TRUE(validation, mask.isValid());
		OCEAN_EXPECT_EQUAL(validation, mask.width(), width);
		OCEAN_EXPECT_EQUAL(validation, mask.height(), height);
		OCEAN_EXPECT_TRUE(validation, mask.pixelFormat() == FrameType::FORMAT_Y8);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrameFilterMorphology::testCloseMaskSquare5(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing closeMask<MF_SQUARE_5>():";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 6u, 64u);
		const unsigned int height = RandomI::random(randomGenerator, 6u, 64u);
		const unsigned int paddingElements = RandomI::random(randomGenerator, 0u, 16u);
		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 0u, 255u));

		Frame mask = randomBinaryMask(randomGenerator, width, height, paddingElements, maskValue);

		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;

		CV::FrameFilterMorphology::closeMask<CV::FrameFilterMorphology::MF_SQUARE_5>(mask.data<uint8_t>(), width, height, paddingElements, maskValue, w);

		OCEAN_EXPECT_TRUE(validation, mask.isValid());
		OCEAN_EXPECT_EQUAL(validation, mask.width(), width);
		OCEAN_EXPECT_EQUAL(validation, mask.height(), height);
		OCEAN_EXPECT_TRUE(validation, mask.pixelFormat() == FrameType::FORMAT_Y8);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrameFilterMorphology::testIdempotentOnFullFrame()
{
	Log::info() << "Testing open/close on a fully masked frame:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	for (uint32_t maskValueRaw : {0u, 1u, 128u, 255u})
	{
		const uint8_t maskValue = uint8_t(maskValueRaw);

		// All-mask frame; open should leave all pixels = maskValue (interior shrinks to itself when there is nothing to erode away).
		// Note: the boundary handling of FrameFilterErosion can clear edge pixels even for fully-masked frames, so we
		// only check the strict interior pixels (>= 2 from any edge for SQUARE_5, >= 1 for SQUARE_3) for invariance.

		const unsigned int width = 16u;
		const unsigned int height = 16u;

		// MF_SQUARE_3 on a fully-masked frame: interior pixels (1..width-2, 1..height-2) must remain maskValue after open.
		{
			Frame mask(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
			mask.setValue(maskValue);

			CV::FrameFilterMorphology::openMask<CV::FrameFilterMorphology::MF_SQUARE_3>(mask.data<uint8_t>(), width, height, mask.paddingElements(), maskValue);

			for (unsigned int y = 1u; y + 1u < height; ++y)
			{
				for (unsigned int x = 1u; x + 1u < width; ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, mask.constpixel<uint8_t>(x, y)[0], maskValue);
				}
			}
		}

		// MF_SQUARE_3 + closeMask on a fully-masked frame: dilation cannot add pixels (already full), erosion can only remove edge pixels — interior must stay maskValue.
		{
			Frame mask(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
			mask.setValue(maskValue);

			CV::FrameFilterMorphology::closeMask<CV::FrameFilterMorphology::MF_SQUARE_3>(mask.data<uint8_t>(), width, height, mask.paddingElements(), maskValue);

			for (unsigned int y = 1u; y + 1u < height; ++y)
			{
				for (unsigned int x = 1u; x + 1u < width; ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, mask.constpixel<uint8_t>(x, y)[0], maskValue);
				}
			}
		}
	}

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrameFilterMorphology::testEmptyFrameStaysEmpty()
{
	Log::info() << "Testing open/close on an all-non-mask frame:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	for (uint32_t maskValueRaw : {0u, 128u, 255u})
	{
		const uint8_t maskValue = uint8_t(maskValueRaw);
		const uint8_t nonMaskValue = uint8_t(255u - maskValueRaw);

		const unsigned int width = 16u;
		const unsigned int height = 16u;

		// open() on all-non-mask: erosion of the empty mask is still empty; dilation of the empty mask is still empty.
		{
			Frame mask(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
			mask.setValue(nonMaskValue);

			CV::FrameFilterMorphology::openMask<CV::FrameFilterMorphology::MF_SQUARE_3>(mask.data<uint8_t>(), width, height, mask.paddingElements(), maskValue);

			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, mask.constpixel<uint8_t>(x, y)[0], nonMaskValue);
				}
			}
		}

		// open() with the SQUARE_5 filter on all-non-mask: same expected result.
		{
			Frame mask(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
			mask.setValue(nonMaskValue);

			CV::FrameFilterMorphology::openMask<CV::FrameFilterMorphology::MF_SQUARE_5>(mask.data<uint8_t>(), width, height, mask.paddingElements(), maskValue);

			for (unsigned int y = 0u; y < height; ++y)
			{
				for (unsigned int x = 0u; x < width; ++x)
				{
					OCEAN_EXPECT_EQUAL(validation, mask.constpixel<uint8_t>(x, y)[0], nonMaskValue);
				}
			}
		}
	}

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
