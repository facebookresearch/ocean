/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameInterpolatorTrilinear.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/FrameInterpolatorTrilinear.h"
#include "ocean/cv/FramePyramid.h"

#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameInterpolatorTrilinear::test(const double testDuration, Worker& worker, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("FrameInterpolatorTrilinear test");
	Log::info() << " ";

	if (selector.shouldRun("resizeframe"))
	{
		testResult = testResizeFrame(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("resizeidentitysize"))
	{
		testResult = testResizeIdentitySize(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("resizeconstantframe"))
	{
		testResult = testResizeConstantFrame(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("interpolateposition"))
	{
		testResult = testInterpolatePosition(testDuration);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("workerequivalencestress"))
	{
		testResult = testWorkerEquivalenceStress(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("extremescalestress"))
	{
		testResult = testExtremeScaleStress(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("homographystress"))
	{
		testResult = testHomographyStress(testDuration, worker);
		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("randomparametersstress"))
	{
		testResult = testRandomParametersStress(testDuration, worker);
		Log::info() << " ";
	}

	Log::info() << " ";
	Log::info() << testResult;
	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameInterpolatorTrilinear, ResizeFrame)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorTrilinear::testResizeFrame(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorTrilinear, ResizeIdentitySize)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorTrilinear::testResizeIdentitySize(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorTrilinear, ResizeConstantFrame)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorTrilinear::testResizeConstantFrame(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorTrilinear, InterpolatePosition)
{
	EXPECT_TRUE(TestFrameInterpolatorTrilinear::testInterpolatePosition(GTEST_TEST_DURATION));
}

TEST(TestFrameInterpolatorTrilinear, WorkerEquivalenceStress)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorTrilinear::testWorkerEquivalenceStress(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorTrilinear, ExtremeScaleStress)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorTrilinear::testExtremeScaleStress(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorTrilinear, HomographyStress)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorTrilinear::testHomographyStress(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameInterpolatorTrilinear, RandomParametersStress)
{
	Worker worker;
	EXPECT_TRUE(TestFrameInterpolatorTrilinear::testRandomParametersStress(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

namespace
{

/**
 * Helper that fills a frame's elements with random uint8_t values.
 */
void fillRandomFrame(RandomGenerator& randomGenerator, Frame& frame)
{
	const unsigned int channels = frame.channels();
	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		uint8_t* row = frame.row<uint8_t>(y);
		for (unsigned int x = 0u; x < frame.width() * channels; ++x)
		{
			row[x] = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}
	}
}

} // namespace

bool TestFrameInterpolatorTrilinear::testResizeFrame(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing resize() across pixel formats and shrink/enlarge factors:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const FrameType::PixelFormat pixelFormats[] = {
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_RGBA32,
	};

	const Timestamp startTimestamp(true);
	do
	{
		const FrameType::PixelFormat pixelFormat = pixelFormats[RandomI::random(randomGenerator, 0u, 2u)];

		const unsigned int sourceWidth = RandomI::random(randomGenerator, 16u, 96u);
		const unsigned int sourceHeight = RandomI::random(randomGenerator, 16u, 96u);
		const unsigned int targetWidth = RandomI::random(randomGenerator, 4u, 128u);
		const unsigned int targetHeight = RandomI::random(randomGenerator, 4u, 128u);

		Frame source(FrameType(sourceWidth, sourceHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		fillRandomFrame(randomGenerator, source);

		Frame target(FrameType(targetWidth, targetHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));

		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;

		const bool ok = CV::FrameInterpolatorTrilinear::resize(source, target, w);

		OCEAN_EXPECT_TRUE(validation, ok);
		OCEAN_EXPECT_TRUE(validation, target.isValid());
		OCEAN_EXPECT_EQUAL(validation, target.width(), targetWidth);
		OCEAN_EXPECT_EQUAL(validation, target.height(), targetHeight);
		OCEAN_EXPECT_TRUE(validation, target.pixelFormat() == pixelFormat);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrameInterpolatorTrilinear::testResizeIdentitySize(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing resize() to the same size produces a valid output frame:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 16u, 64u);
		const unsigned int height = RandomI::random(randomGenerator, 16u, 64u);

		Frame source(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		fillRandomFrame(randomGenerator, source);

		Frame target(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		target.setValue(0u);

		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;
		const bool ok = CV::FrameInterpolatorTrilinear::resize(source, target, w);

		OCEAN_EXPECT_TRUE(validation, ok);
		OCEAN_EXPECT_TRUE(validation, target.isValid());
		OCEAN_EXPECT_EQUAL(validation, target.width(), width);
		OCEAN_EXPECT_EQUAL(validation, target.height(), height);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrameInterpolatorTrilinear::testResizeConstantFrame(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing resize() of a constant-color frame stays close to that color:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int sourceWidth = RandomI::random(randomGenerator, 32u, 96u);
		const unsigned int sourceHeight = RandomI::random(randomGenerator, 32u, 96u);
		const unsigned int targetWidth = RandomI::random(randomGenerator, 8u, 64u);
		const unsigned int targetHeight = RandomI::random(randomGenerator, 8u, 64u);

		const uint8_t value = uint8_t(RandomI::random(randomGenerator, 16u, 240u));

		Frame source(FrameType(sourceWidth, sourceHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		source.setValue(value);

		Frame target(FrameType(targetWidth, targetHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;
		const bool ok = CV::FrameInterpolatorTrilinear::resize(source, target, w);
		OCEAN_EXPECT_TRUE(validation, ok);

		// Allow some tolerance because tri-linear interpolation samples pyramid layers, which were
		// computed by averaging neighbouring pixels and may slightly deviate from the constant value.
		const int tolerance = 4;
		for (unsigned int y = 0u; y < targetHeight; ++y)
		{
			const uint8_t* row = target.constrow<uint8_t>(y);
			for (unsigned int x = 0u; x < targetWidth; ++x)
			{
				const int diff = int(row[x]) - int(value);
				OCEAN_EXPECT_TRUE(validation, diff >= -tolerance && diff <= tolerance);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrameInterpolatorTrilinear::testInterpolatePosition(const double testDuration)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Testing interpolatePosition() returns in-range coordinates:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int layerCount = RandomI::random(randomGenerator, 1u, 8u);
		const Scalar cx = Scalar(RandomI::random(randomGenerator, 1u, 256u));
		const Scalar cy = Scalar(RandomI::random(randomGenerator, 1u, 256u));

		const Vector2 center(cx, cy);
		const Vector2 c1 = center + Vector2(Scalar(-0.5), Scalar(-0.5));
		const Vector2 c2 = center + Vector2(Scalar(0.5), Scalar(-0.5));
		const Vector2 c3 = center + Vector2(Scalar(0.5), Scalar(0.5));
		const Vector2 c4 = center + Vector2(Scalar(-0.5), Scalar(0.5));

		const Vector3 result = CV::FrameInterpolatorTrilinear::interpolatePosition(layerCount, center, c1, c2, c3, c4);

		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(result.x(), cx));
		OCEAN_EXPECT_TRUE(validation, Numeric::isEqual(result.y(), cy));
		OCEAN_EXPECT_TRUE(validation, result.z() >= Scalar(0));
		OCEAN_EXPECT_TRUE(validation, result.z() <= Scalar(layerCount - 1u));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrameInterpolatorTrilinear::testWorkerEquivalenceStress(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Stress: worker vs single-thread output equivalence:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const FrameType::PixelFormat pixelFormats[] = {
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_RGBA32,
	};

	const Timestamp startTimestamp(true);
	do
	{
		const FrameType::PixelFormat pixelFormat = pixelFormats[RandomI::random(randomGenerator, 0u, 2u)];
		const unsigned int sourceWidth = RandomI::random(randomGenerator, 16u, 128u);
		const unsigned int sourceHeight = RandomI::random(randomGenerator, 16u, 128u);
		const unsigned int targetWidth = RandomI::random(randomGenerator, 4u, 192u);
		const unsigned int targetHeight = RandomI::random(randomGenerator, 4u, 192u);

		Frame source(FrameType(sourceWidth, sourceHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		fillRandomFrame(randomGenerator, source);

		Frame targetSingle(FrameType(targetWidth, targetHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		Frame targetWorker(FrameType(targetWidth, targetHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		targetSingle.setValue(0xABu);
		targetWorker.setValue(0xCDu);

		const bool ok1 = CV::FrameInterpolatorTrilinear::resize(source, targetSingle, nullptr);
		const bool ok2 = CV::FrameInterpolatorTrilinear::resize(source, targetWorker, &worker);

		OCEAN_EXPECT_TRUE(validation, ok1);
		OCEAN_EXPECT_TRUE(validation, ok2);

		const unsigned int channels = targetSingle.channels();
		for (unsigned int y = 0u; y < targetHeight; ++y)
		{
			const uint8_t* rowS = targetSingle.constrow<uint8_t>(y);
			const uint8_t* rowW = targetWorker.constrow<uint8_t>(y);
			for (unsigned int x = 0u; x < targetWidth * channels; ++x)
			{
				OCEAN_EXPECT_EQUAL(validation, rowS[x], rowW[x]);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrameInterpolatorTrilinear::testExtremeScaleStress(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Stress: extreme shrink and enlarge factors:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const bool shrink = RandomI::random(randomGenerator, 1u) == 0u;

		const unsigned int sourceWidth = shrink ? RandomI::random(randomGenerator, 64u, 256u) : RandomI::random(randomGenerator, 4u, 16u);
		const unsigned int sourceHeight = shrink ? RandomI::random(randomGenerator, 64u, 256u) : RandomI::random(randomGenerator, 4u, 16u);
		const unsigned int targetWidth = shrink ? RandomI::random(randomGenerator, 2u, 8u) : RandomI::random(randomGenerator, 64u, 256u);
		const unsigned int targetHeight = shrink ? RandomI::random(randomGenerator, 2u, 8u) : RandomI::random(randomGenerator, 64u, 256u);

		Frame source(FrameType(sourceWidth, sourceHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		fillRandomFrame(randomGenerator, source);

		Frame target(FrameType(targetWidth, targetHeight, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;
		const bool ok = CV::FrameInterpolatorTrilinear::resize(source, target, w);

		OCEAN_EXPECT_TRUE(validation, ok);
		OCEAN_EXPECT_TRUE(validation, target.isValid());
		OCEAN_EXPECT_EQUAL(validation, target.width(), targetWidth);
		OCEAN_EXPECT_EQUAL(validation, target.height(), targetHeight);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrameInterpolatorTrilinear::testHomographyStress(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Stress: homography on random source frames:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);
	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 16u, 96u);
		const unsigned int height = RandomI::random(randomGenerator, 16u, 96u);

		Frame source(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		fillRandomFrame(randomGenerator, source);

		const CV::FramePyramid sourcePyramid(source, CV::FramePyramid::AS_MANY_LAYERS_AS_POSSIBLE, false /*copyFirstLayer*/, &worker);

		// Identity homography -- output should equal source.
		const SquareMatrix3 homography(true);

		Frame target(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
		target.setValue(0xEEu);

		const uint8_t borderColor = 0u;
		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;
		CV::FrameInterpolatorTrilinear::homography8BitPerChannel<1u>(sourcePyramid, width, height, homography, &borderColor, target.data<uint8_t>(), w);

		OCEAN_EXPECT_TRUE(validation, target.isValid());
		OCEAN_EXPECT_EQUAL(validation, target.width(), width);
		OCEAN_EXPECT_EQUAL(validation, target.height(), height);

		// Sanity check: identity transform should give output that's reasonably close to the source for interior pixels.
		// We only check that not all bytes were left at 0xEE (i.e., the function actually wrote something).
		bool wrote = false;
		for (unsigned int y = 0u; y < height && !wrote; ++y)
		{
			const uint8_t* row = target.constrow<uint8_t>(y);
			for (unsigned int x = 0u; x < width; ++x)
			{
				if (row[x] != uint8_t(0xEEu))
				{
					wrote = true;
					break;
				}
			}
		}
		OCEAN_EXPECT_TRUE(validation, wrote);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

bool TestFrameInterpolatorTrilinear::testRandomParametersStress(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);
	Log::info() << "Stress: random parameter combinations are stable:";

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const FrameType::PixelFormat pixelFormats[] = {
		FrameType::FORMAT_Y8,
		FrameType::FORMAT_RGB24,
		FrameType::FORMAT_RGBA32,
	};

	const Timestamp startTimestamp(true);
	do
	{
		const FrameType::PixelFormat pixelFormat = pixelFormats[RandomI::random(randomGenerator, 0u, 2u)];
		const unsigned int sourceWidth = RandomI::random(randomGenerator, 4u, 96u);
		const unsigned int sourceHeight = RandomI::random(randomGenerator, 4u, 96u);
		const unsigned int targetWidth = RandomI::random(randomGenerator, 2u, 96u);
		const unsigned int targetHeight = RandomI::random(randomGenerator, 2u, 96u);

		Frame source(FrameType(sourceWidth, sourceHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));
		fillRandomFrame(randomGenerator, source);

		Frame target(FrameType(targetWidth, targetHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT));

		Worker* w = (RandomI::random(randomGenerator, 1u) == 0u) ? nullptr : &worker;
		const bool ok = CV::FrameInterpolatorTrilinear::resize(source, target, w);

		OCEAN_EXPECT_TRUE(validation, ok);
		OCEAN_EXPECT_TRUE(validation, target.isValid());
		OCEAN_EXPECT_EQUAL(validation, target.width(), targetWidth);
		OCEAN_EXPECT_EQUAL(validation, target.height(), targetHeight);
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	Log::info() << "Validation: " << validation;
	return validation.succeeded();
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
