/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameNormalizer.h"

#include "ocean/base/RandomI.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameNormalizer.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameNormalizer::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Frame Normalizer test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testNormalizerToUint8(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNormalizeToFloat(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testValueRangeNormalizerToUint8(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame Normalizer test succeeded.";
	}
	else
	{
		Log::info() << "Frame Normalizer test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameNormalizer, NormalizerToUint8_uint8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizerToUint8<uint8_t>(GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameNormalizer, NormalizerToUint8_int8)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizerToUint8<int8_t>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizerToUint8_uint16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizerToUint8<uint16_t>(GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameNormalizer, NormalizerToUint8_int16)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizerToUint8<int16_t>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizerToUint8_uint32)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizerToUint8<uint32_t>(GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameNormalizer, NormalizerToUint8_int32)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizerToUint8<int32_t>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizerToUint8_float)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizerToUint8<float>(GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameNormalizer, NormalizerToUint8_double)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizerToUint8<double>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizerToUint8_float_extreme)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizerToUint8<float, true>(GTEST_TEST_DURATION, worker)));
}
TEST(TestFrameNormalizer, NormalizerToUint8_double_extreme)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizerToUint8<double, true>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint8_t_float_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint8_t, float, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint8_t_float_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint8_t, float, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint8_t_float_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint8_t, float, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint8_t_float_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint8_t, float, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int8_t_float_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int8_t, float, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int8_t_float_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int8_t, float, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int8_t_float_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int8_t, float, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int8_t_float_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int8_t, float, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint16_t_float_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint16_t, float, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint16_t_float_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint16_t, float, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint16_t_float_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint16_t, float, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint16_t_float_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint16_t, float, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int16_t_float_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int16_t, float, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int16_t_float_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int16_t, float, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int16_t_float_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int16_t, float, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int16_t_float_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int16_t, float, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint32_t_float_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint32_t, float, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint32_t_float_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint32_t, float, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint32_t_float_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint32_t, float, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint32_t_float_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint32_t, float, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int32_t_float_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int32_t, float, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int32_t_float_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int32_t, float, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int32_t_float_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int32_t, float, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int32_t_float_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int32_t, float, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint64_t_float_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint64_t, float, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint64_t_float_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint64_t, float, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint64_t_float_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint64_t, float, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint64_t_float_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint64_t, float, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int64_t_float_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int64_t, float, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int64_t_float_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int64_t, float, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int64_t_float_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int64_t, float, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int64_t_float_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int64_t, float, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_float_float_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<float, float, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_float_float_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<float, float, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_float_float_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<float, float, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_float_float_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<float, float, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_double_float_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<double, float, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_double_float_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<double, float, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_double_float_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<double, float, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_double_float_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<double, float, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint8_t_double_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint8_t, double, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint8_t_double_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint8_t, double, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint8_t_double_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint8_t, double, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint8_t_double_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint8_t, double, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int8_t_double_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int8_t, double, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int8_t_double_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int8_t, double, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int8_t_double_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int8_t, double, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int8_t_double_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int8_t, double, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint16_t_double_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint16_t, double, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint16_t_double_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint16_t, double, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint16_t_double_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint16_t, double, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint16_t_double_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint16_t, double, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int16_t_double_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int16_t, double, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int16_t_double_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int16_t, double, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int16_t_double_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int16_t, double, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int16_t_double_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int16_t, double, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint32_t_double_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint32_t, double, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint32_t_double_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint32_t, double, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint32_t_double_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint32_t, double, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint32_t_double_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint32_t, double, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int32_t_double_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int32_t, double, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int32_t_double_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int32_t, double, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int32_t_double_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int32_t, double, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int32_t_double_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int32_t, double, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint64_t_double_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint64_t, double, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint64_t_double_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint64_t, double, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint64_t_double_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint64_t, double, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_uint64_t_double_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<uint64_t, double, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int64_t_double_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int64_t, double, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int64_t_double_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int64_t, double, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int64_t_double_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int64_t, double, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_int64_t_double_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<int64_t, double, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_float_double_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<float, double, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_float_double_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<float, double, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_float_double_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<float, double, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_float_double_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<float, double, 4u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_double_double_1u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<double, double, 1u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_double_double_2u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<double, double, 2u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_double_double_3u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<double, double, 3u>(GTEST_TEST_DURATION, worker)));
}

TEST(TestFrameNormalizer, NormalizeToFloat_double_double_4u)
{
	Worker worker;
	EXPECT_TRUE((TestFrameNormalizer::testNormalizeToFloat<double, double, 4u>(GTEST_TEST_DURATION, worker)));
}


TEST(TestFrameNormalizer, testValueRangeNormalizerToUint8_float)
{
	Worker worker;
	EXPECT_TRUE(TestFrameNormalizer::testValueRangeNormalizerToUint8<float>(GTEST_TEST_DURATION, worker));
}

TEST(TestFrameNormalizer, testValueRangeNormalizerToUint8_double)
{
	Worker worker;
	EXPECT_TRUE(TestFrameNormalizer::testValueRangeNormalizerToUint8<double>(GTEST_TEST_DURATION, worker));
}

#endif // OCEAN_USE_GTEST

bool TestFrameNormalizer::testNormalizerToUint8(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 1-channel normalizer to 8 bit images:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testNormalizerToUint8<uint8_t>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizerToUint8<int8_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizerToUint8<uint16_t>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizerToUint8<int16_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizerToUint8<uint32_t>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizerToUint8<int32_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizerToUint8<uint64_t>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizerToUint8<int64_t>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizerToUint8<float>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizerToUint8<double>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizerToUint8<float, true>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizerToUint8<double, true>(testDuration, worker) && allSucceeded;

	return allSucceeded;
}

template <typename T, bool tExtremeValueRange>
bool TestFrameNormalizer::testNormalizerToUint8(const double testDuration, Worker& worker)
{
	static_assert(std::is_floating_point<T>::value || tExtremeValueRange == false, "Invalid parameter!");

	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << TypeNamer::name<T>() << ":";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

		const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int normalizedPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame frame(FrameType(width, height, FrameType::genericPixelFormat<T, 1u>(), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
		Frame normalized(FrameType(frame, FrameType::genericPixelFormat<uint8_t, 1u>()), normalizedPaddingElements);

		if constexpr (tExtremeValueRange)
		{
			for (unsigned int y = 0u; y < frame.height(); ++y)
			{
				T* const row = frame.row<T>(y);

				for (unsigned int n = 0u; n < frame.strideElements(); ++n)
				{
					row[n] = T(RandomD::scalar(randomGenerator, -1.0e8, 1.0e8));
				}
			}
		}
		else
		{
			CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);
		}

		CV::CVUtilities::randomizeFrame(normalized, false, &randomGenerator);

		const Frame copyNormalized(normalized, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		CV::FrameNormalizer::normalize1ChannelToUint8<T>(frame.constdata<T>(), normalized.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), normalized.paddingElements(), useWorker);

		if (!CV::CVUtilities::isPaddingMemoryIdentical(normalized, copyNormalized))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		if (!verifyLinearNormalizedUint8<T>(frame.constdata<T>(), normalized.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), normalized.paddingElements()))
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

bool TestFrameNormalizer::testNormalizeToFloat(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing normalization to float images:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testNormalizeToFloat<uint8_t, float, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint8_t, float, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint8_t, float, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint8_t, float, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<int8_t, float, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int8_t, float, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int8_t, float, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int8_t, float, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<uint16_t, float, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint16_t, float, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint16_t, float, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint16_t, float, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<int16_t, float, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int16_t, float, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int16_t, float, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int16_t, float, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<uint32_t, float, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint32_t, float, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint32_t, float, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint32_t, float, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<int32_t, float, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int32_t, float, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int32_t, float, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int32_t, float, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<uint64_t, float, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint64_t, float, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint64_t, float, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint64_t, float, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<int64_t, float, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int64_t, float, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int64_t, float, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int64_t, float, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<float, float, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<float, float, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<float, float, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<float, float, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<double, float, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<double, float, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<double, float, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<double, float, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<uint8_t, double, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint8_t, double, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint8_t, double, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint8_t, double, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<int8_t, double, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int8_t, double, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int8_t, double, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int8_t, double, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<uint16_t, double, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint16_t, double, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint16_t, double, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint16_t, double, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<int16_t, double, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int16_t, double, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int16_t, double, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int16_t, double, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<uint32_t, double, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint32_t, double, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint32_t, double, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint32_t, double, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<int32_t, double, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int32_t, double, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int32_t, double, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int32_t, double, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<uint64_t, double, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint64_t, double, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint64_t, double, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<uint64_t, double, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<int64_t, double, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int64_t, double, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int64_t, double, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<int64_t, double, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<float, double, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<float, double, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<float, double, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<float, double, 4u>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeToFloat<double, double, 1u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<double, double, 2u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<double, double, 3u>(testDuration, worker) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeToFloat<double, double, 4u>(testDuration, worker) && allSucceeded;

	return allSucceeded;
}

template <typename TSource, typename TTarget, unsigned int tChannels>
bool TestFrameNormalizer::testNormalizeToFloat(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << TypeNamer::name<TSource>() << " to " << TypeNamer::name<TTarget>() << " with " << tChannels << " channel(s):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	TTarget bias[tChannels];
	TTarget scale[tChannels];

	double maxAbsoluteError = 0;

	do
	{
		const bool normalizeToRange0To1 = RandomI::random(randomGenerator, 1u) == 0u;
		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		const TTarget* useBias = nullptr;
		const TTarget* useScale = nullptr;

		if (!normalizeToRange0To1)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				bias[c] = RandomT<TTarget>::scalar(randomGenerator, TTarget(-100), TTarget(100));
				scale[c] = TTarget(RandomI::random(randomGenerator, 1u) == 0u ? 1 : -1) * RandomT<TTarget>::scalar(randomGenerator, NumericT<TTarget>::weakEps(), TTarget(5));

				useBias = bias;
				useScale = scale;
			}
		}

		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);

		const unsigned int framePaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int normalizedPaddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

		Frame frame(FrameType(width, height, FrameType::genericPixelFormat<TSource, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), framePaddingElements);
		Frame normalized(FrameType(frame, FrameType::genericPixelFormat<TTarget, tChannels>()), normalizedPaddingElements);

		CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator, /* limitedValueRange */ true);
		CV::CVUtilities::randomizeFrame(normalized, false, &randomGenerator);

		const Frame copyNormalized(normalized, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		CV::FrameNormalizer::normalizeToFloat<TSource, TTarget, tChannels>(frame.constdata<TSource>(), normalized.data<TTarget>(), frame.width(), frame.height(), useBias, useScale, frame.paddingElements(), normalized.paddingElements(), useWorker);

		if (!CV::CVUtilities::isPaddingMemoryIdentical(normalized, copyNormalized))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		double currentMaxAbsoluteError = 0;
		if (!verifyNormalizeToFloat<TSource, TTarget, tChannels>(frame.constdata<TSource>(), normalized.constdata<TTarget>(), frame.width(), frame.height(), useBias, useScale, frame.paddingElements(), normalized.paddingElements(), currentMaxAbsoluteError))
		{
			allSucceeded = false;
		}

		maxAbsoluteError = std::max(maxAbsoluteError, currentMaxAbsoluteError);
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Max. absolute error: " << String::toAString(maxAbsoluteError, 5u);

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

bool TestFrameNormalizer::testValueRangeNormalizerToUint8(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing value range normalize to float:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testValueRangeNormalizerToUint8<float>(testDuration, worker) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testValueRangeNormalizerToUint8<double>(testDuration, worker) && allSucceeded;

	return allSucceeded;
}

template <typename TFloat>
bool TestFrameNormalizer::testValueRangeNormalizerToUint8(const double testDuration, Worker& worker)
{
	static_assert(std::is_floating_point<TFloat>::value, "Invalid source type!");

	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << TypeNamer::name<TFloat>() << ":";

	constexpr unsigned int width = 1000u;
	constexpr unsigned int height = 1000u;

	constexpr unsigned int pixels = width * height;

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

		Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<TFloat, 1u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		TFloat minValue = TFloat(RandomD::scalar(randomGenerator, -1.0e5, 1.0e5));
		TFloat maxValue = TFloat(RandomD::scalar(randomGenerator, -1.0e5, 1.0e5));

		if (minValue > maxValue)
		{
			std::swap(minValue, maxValue);
		}

		ocean_assert(minValue <= maxValue);

		maxValue = std::max(minValue + TFloat(5000), maxValue);

		const TFloat valueRange = maxValue - minValue;
		ocean_assert(valueRange >= TFloat(255));

		const TFloat testMaxValue = minValue + valueRange;
		TFloat floatPrecisionMaxValue = NumericT<TFloat>::abs(testMaxValue - maxValue); // due to floating point precision, we may get a precision gap

		floatPrecisionMaxValue = std::max(floatPrecisionMaxValue, valueRange * TFloat(0.0001));

		unsigned int index = 0u;

		for (unsigned int y = 0u; y < height; ++y)
		{
			TFloat* const row = frame.row<TFloat>(y);

			for (unsigned int x = 0u; x < width; ++x)
			{
				// index == 0: minValue
				// index == pixels - 1: maxValue

				TFloat value = minValue + TFloat(index) * valueRange / TFloat(pixels - 1u);

				ocean_assert(index != 0u || value == minValue);
				ocean_assert(index != pixels - 1u || NumericT<TFloat>::isEqual(value, maxValue, floatPrecisionMaxValue * TFloat(2)));

				ocean_assert(value >= minValue && value <= maxValue + floatPrecisionMaxValue);

				if (index == pixels - 1u)
				{
					value = maxValue;
				}

				if (index >= pixels * 99u / 100u) // % in the last 1% we may see too large values
				{
					value = std::min(value, maxValue);
				}

				ocean_assert(value >= minValue && value <= maxValue);

				row[x] = value;

				++index;
			}
		}

		Frame normalized = CV::CVUtilities::randomizedFrame(FrameType(frame, FrameType::genericPixelFormat<uint8_t, 1u>()), &randomGenerator);

		const Frame copyNormalized(normalized, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		CV::FrameNormalizer::normalize1ChannelToUint8<TFloat>(frame.constdata<TFloat>(), normalized.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), normalized.paddingElements(), useWorker);

		if (!CV::CVUtilities::isPaddingMemoryIdentical(frame, copyFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(normalized, copyNormalized))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		// let's create a histogram to ensure that each bin is equally filled

		Indices32 histogram(256, 0u);

		int previousValue = -1;

		for (unsigned int y = 0u; y < height; ++y)
		{
			const uint8_t* const row = normalized.constrow<uint8_t>(y);

			for (unsigned int x = 0u; x < width; ++x)
			{
				const uint8_t value = row[x];

				if (int(value) < previousValue) // each value must be equal or larger than the previous value
				{
					allSucceeded = false;
				}

				++histogram[value];

				previousValue = int(value);
			}
		}

		constexpr TFloat perfectBinSize = TFloat(pixels) / TFloat(256);

		TFloat maxError = TFloat(0);

		for (const Index32& bin : histogram)
		{
			const TFloat error = NumericT<TFloat>::abs(TFloat(bin) - perfectBinSize);

			if (error > maxError)
			{
				maxError = error;
			}
		}

		const double percent = double(maxError) / double(perfectBinSize);

		if (percent >= 0.005) // 0.5%
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

template <typename T>
bool TestFrameNormalizer::verifyLinearNormalizedUint8(const T* const frame, const uint8_t* const normalized, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int normalizedPaddingElements)
{
	ocean_assert(frame != nullptr);
	ocean_assert(normalized != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int frameStrideElements = width + framePaddingElements;
	const unsigned int normalizedStrideElements = width + normalizedPaddingElements;

	double minValue = NumericD::maxValue();
	double maxValue = NumericD::minValue();

	for (unsigned int y = 0u; y < height; ++y)
	{
		const T* const frameRow = frame + y * frameStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			minValue = std::min(double(frameRow[x]), minValue);
			maxValue = std::max(double(frameRow[x]), maxValue);
		}
	}

	ocean_assert(minValue <= maxValue);

	if (NumericD::isEqual(minValue, maxValue))
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			const uint8_t* const normalizedRow = normalized + y * normalizedStrideElements;

			for (unsigned int x = 0u; x < width; ++x)
			{
				if (normalizedRow[x] != 0u)
				{
					return false;
				}
			}
		}
	}
	else
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			const T* const frameRow = frame + y * frameStrideElements;
			const uint8_t* const normalizedRow = normalized + y * normalizedStrideElements;

			for (unsigned int x = 0u; x < width; ++x)
			{
				const T framePixel = frameRow[x];

				const double relativeValue = (double(framePixel) - minValue) / (maxValue - minValue);
				ocean_assert(relativeValue >= 0.0 && relativeValue <= 1.0);

				const uint8_t normalizedPixel = uint8_t(relativeValue * 255.0 + 0.5);

				if (std::abs(int(normalizedRow[x]) - int(normalizedPixel)) > 1)
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename TSource, typename TTarget, unsigned int tChannels>
bool TestFrameNormalizer::verifyNormalizeToFloat(const TSource* frame, const TTarget* normalized, const unsigned int width, const unsigned height, const TTarget* bias, const TTarget* scale, const unsigned int framePaddingElements, const unsigned int normalizedPaddingElements, double& maxAbsoluteError)
{
	static_assert(std::is_floating_point<TTarget>::value, "Invalid data type!");
	static_assert(tChannels != 0u, "Invalid number of channels");

	ocean_assert(frame != nullptr);
	ocean_assert(normalized != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert((bias != nullptr && scale != nullptr) || (bias == nullptr && scale == nullptr));

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int normalizedStrideElements = width * tChannels + normalizedPaddingElements;

	TTarget localBias[tChannels];
	TTarget localScale[tChannels];

	if (bias == nullptr && scale == nullptr)
	{
		double minValues[tChannels];
		double maxValues[tChannels];

		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			minValues[c] = NumericD::maxValue();
			maxValues[c] = NumericD::minValue();
		}

		for (unsigned int y = 0u; y < height; ++y)
		{
			const TSource* const frameRow = frame + y * frameStrideElements;

			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					const double framePixelValue = double(frameRow[x * tChannels + c]);

					if (!std::is_floating_point<TSource>::value || !NumericD::isInf(framePixelValue)) // +/- inf is ignored, so we preserve the current value
					{
						minValues[c] = std::min(framePixelValue, minValues[c]);
						maxValues[c] = std::max(framePixelValue, maxValues[c]);
					}
				}
			}
		}

		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			ocean_assert(minValues[c] <= maxValues[c]);
			localBias[c] = TTarget(-minValues[c]);
			localScale[c] = TTarget(1.0 / std::max(NumericD::weakEps(), maxValues[c] - minValues[c]));
		}

		bias = localBias;
		scale = localScale;
	}

	ocean_assert(bias != nullptr && scale != nullptr);

	constexpr TTarget maxAllowedAbsoluteError = TTarget(3); // 0.1% of range [-1135, 1775]: 0.001 * 2910 ~ 3
	maxAbsoluteError = 0;

	for (unsigned int y = 0u; y < height; ++y)
	{
		const TSource* const frameRow = frame + y * frameStrideElements;
		const TTarget* const normalizedRow = normalized + y * normalizedStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				const TSource framePixel = frameRow[x * tChannels + c];

				const TTarget expectedValue = (TTarget(framePixel) + bias[c]) * scale[c];
				const TTarget normalizedValue = normalizedRow[x * tChannels + c];

				const TTarget absoluteError = std::abs(expectedValue - normalizedValue);
				maxAbsoluteError = std::max(maxAbsoluteError, double(absoluteError));

				if (maxAbsoluteError > maxAllowedAbsoluteError)
				{
					return false;
				}
			}
		}
	}

	return true;
}

}

}

}
