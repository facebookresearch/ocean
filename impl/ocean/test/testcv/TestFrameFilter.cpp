/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameFilter.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilter.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameFilter::test(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Frame filter test:   ---";
	Log::info() << " ";

	allSucceeded = testMagnitude(testDuration, worker) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNormalizeValue(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame filter test succeeded.";
	}
	else
	{
		Log::info() << "Frame filter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameFilter, Magnitude)
{
	Worker worker;
	EXPECT_TRUE(TestFrameFilter::testMagnitude(GTEST_TEST_DURATION, worker));
}

#ifdef DISABLED_DUE_TO_FLOAT_TEMPLATE_VALUE // activate once float template parameters are supported

TEST(TestFrameFilter, NormalizeValue_float)
{
	EXPECT_TRUE(TestFrameFilter::testNormalizeValue<float>(GTEST_TEST_DURATION));
}

TEST(TestFrameFilter, NormalizeValue_double)
{
	EXPECT_TRUE(TestFrameFilter::testNormalizeValue<double>(GTEST_TEST_DURATION));
}

#endif // DISABLED_DUE_TO_FLOAT_TEMPLATE_VALUE

TEST(TestFrameFilter, NormalizeValue_int8)
{
	EXPECT_TRUE(TestFrameFilter::testNormalizeValue<int8_t>(GTEST_TEST_DURATION));
}

TEST(TestFrameFilter, NormalizeValue_uint8)
{
	EXPECT_TRUE(TestFrameFilter::testNormalizeValue<uint8_t>(GTEST_TEST_DURATION));
}

TEST(TestFrameFilter, NormalizeValue_int16)
{
	EXPECT_TRUE(TestFrameFilter::testNormalizeValue<int16_t>(GTEST_TEST_DURATION));
}

TEST(TestFrameFilter, NormalizeValue_uint16)
{
	EXPECT_TRUE(TestFrameFilter::testNormalizeValue<uint16_t>(GTEST_TEST_DURATION));
}

TEST(TestFrameFilter, NormalizeValue_int32)
{
	EXPECT_TRUE(TestFrameFilter::testNormalizeValue<int32_t>(GTEST_TEST_DURATION));
}

TEST(TestFrameFilter, NormalizeValue_uint32)
{
	EXPECT_TRUE(TestFrameFilter::testNormalizeValue<uint32_t>(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFrameFilter::testMagnitude(const double testDuration, Worker& worker)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing magnitude:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	using DataTypePair = std::pair<FrameType::DataType, FrameType::DataTypes>;
	using DataTypePairs = std::vector<DataTypePair>;

	const DataTypePairs dataTypePairs =
	{
		{FrameType::DT_SIGNED_INTEGER_8, {FrameType::DT_SIGNED_INTEGER_16, FrameType::DT_UNSIGNED_INTEGER_16, FrameType::DT_SIGNED_INTEGER_32, FrameType::DT_UNSIGNED_INTEGER_32, FrameType::DT_SIGNED_FLOAT_32, FrameType::DT_SIGNED_FLOAT_64}},
		{FrameType::DT_UNSIGNED_INTEGER_8, {FrameType::DT_UNSIGNED_INTEGER_16, FrameType::DT_UNSIGNED_INTEGER_32, FrameType::DT_SIGNED_FLOAT_32, FrameType::DT_SIGNED_FLOAT_64}},

		{FrameType::DT_SIGNED_INTEGER_16, {FrameType::DT_SIGNED_INTEGER_32, FrameType::DT_UNSIGNED_INTEGER_32, FrameType::DT_SIGNED_FLOAT_32, FrameType::DT_SIGNED_FLOAT_64}},
		{FrameType::DT_UNSIGNED_INTEGER_16, {FrameType::DT_UNSIGNED_INTEGER_32, FrameType::DT_SIGNED_FLOAT_32, FrameType::DT_SIGNED_FLOAT_64}},

		{FrameType::DT_SIGNED_FLOAT_32, {FrameType::DT_SIGNED_FLOAT_32, FrameType::DT_SIGNED_FLOAT_32}},
		{FrameType::DT_SIGNED_FLOAT_64, {FrameType::DT_SIGNED_FLOAT_64}},
	};

	Timestamp startTimestamp(true);

	do
	{
		const DataTypePair dataTypePair = RandomI::random(randomGenerator, dataTypePairs);

		const FrameType::DataType frameDataType = dataTypePair.first;
		const FrameType::DataType magnitudeDataType = RandomI::random(randomGenerator, dataTypePair.second);

		switch (frameDataType)
		{
			case FrameType::DT_SIGNED_INTEGER_8:
			{
				if (!testMagnitude<int8_t>(magnitudeDataType, randomGenerator, worker))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_UNSIGNED_INTEGER_8:
			{
				if (!testMagnitude<uint8_t>(magnitudeDataType, randomGenerator, worker))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_SIGNED_INTEGER_16:
			{
				if (!testMagnitude<int16_t>(magnitudeDataType, randomGenerator, worker))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_UNSIGNED_INTEGER_16:
			{
				if (!testMagnitude<uint16_t>(magnitudeDataType, randomGenerator, worker))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_SIGNED_FLOAT_32:
			{
				if (!testMagnitude<float>(magnitudeDataType, randomGenerator, worker))
				{
					allSucceeded = false;
				}

				break;
			}

			case FrameType::DT_SIGNED_FLOAT_64:
			{
				if (!testMagnitude<double>(magnitudeDataType, randomGenerator, worker))
				{
					allSucceeded = false;
				}

				break;
			}

			default:
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
				break;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

bool TestFrameFilter::testNormalizeValue(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing normalize value:";
	Log::info() << " ";

	bool allSucceeded = true;

#ifdef DISABLED_DUE_TO_FLOAT_TEMPLATE_VALUE // activate once float template parameters are supported

	allSucceeded = testNormalizeValue<float>(testDuration) && allSucceeded; // available since C++20
	Log::info() << " ";
	allSucceeded = testNormalizeValue<double>(testDuration) && allSucceeded;

	Log::info() << " ";

#endif // DISABLED_DUE_TO_FLOAT_TEMPLATE_VALUE

	allSucceeded = testNormalizeValue<int8_t>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeValue<uint8_t>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeValue<int16_t>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeValue<uint16_t>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testNormalizeValue<int32_t>(testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testNormalizeValue<uint32_t>(testDuration) && allSucceeded;

	return allSucceeded;
}

template <typename T>
bool TestFrameFilter::testNormalizeValue(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... with " << TypeNamer::name<T>() << ":";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	Timestamp startTimestamp(true);

	do
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			const float value = RandomF::scalar(randomGenerator, -1000.0f, 1000.0f);

			if (!verifyValueNormalization<T, T(1.0)>(value))
			{
				allSucceeded = false;
			}

			if (!verifyValueNormalization<T, T(2.0)>(value))
			{
				allSucceeded = false;
			}

			if (!verifyValueNormalization<T, T(4.0)>(value))
			{
				allSucceeded = false;
			}

			if (!verifyValueNormalization<T, T(8.0)>(value))
			{
				allSucceeded = false;
			}

			if (!verifyValueNormalization<T, T(16.0)>(value))
			{
				allSucceeded = false;
			}

			if (!verifyValueNormalization<T, T(7.5)>(value))
			{
				allSucceeded = false;
			}

			if (!verifyValueNormalization<T, T(9.05)>(value))
			{
				allSucceeded = false;
			}
		}
		else
		{
			static_assert(sizeof(T) <= sizeof(uint32_t), "Invalid data type!");

			const T value = T(RandomI::random32(randomGenerator));

			if (!verifyValueNormalization<T, T(1)>(value))
			{
				allSucceeded = false;
			}

			if (!verifyValueNormalization<T, T(2)>(value))
			{
				allSucceeded = false;
			}

			if (!verifyValueNormalization<T, T(3)>(value))
			{
				allSucceeded = false;
			}

			if (!verifyValueNormalization<T, T(4)>(value))
			{
				allSucceeded = false;
			}

			if (!verifyValueNormalization<T, T(8)>(value))
			{
				allSucceeded = false;
			}

			if (!verifyValueNormalization<T, T(16)>(value))
			{
				allSucceeded = false;
			}
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

template <typename T>
bool TestFrameFilter::testMagnitude(const FrameType::DataType magnitudeDataType, RandomGenerator& randomGenerator, Worker& worker)
{
	switch (magnitudeDataType)
	{
		case FrameType::DT_SIGNED_INTEGER_16:
		{
			if constexpr (sizeof(T) <= sizeof(int16_t))
			{
				return testMagnitude<T, int16_t>(randomGenerator, worker);
			}

			ocean_assert(false && "This should never happen!");
			return false;
		}

		case FrameType::DT_UNSIGNED_INTEGER_16:
		{
			if constexpr (sizeof(T) <= sizeof(uint16_t))
			{
				return testMagnitude<T, uint16_t>(randomGenerator, worker);
			}

			ocean_assert(false && "This should never happen!");
			return false;
		}

		case FrameType::DT_SIGNED_INTEGER_32:
		{
			if constexpr (sizeof(T) <= sizeof(int32_t))
			{
				return testMagnitude<T, int32_t>(randomGenerator, worker);
			}

			ocean_assert(false && "This should never happen!");
			return false;
		}

		case FrameType::DT_UNSIGNED_INTEGER_32:
		{
			if constexpr (sizeof(T) <= sizeof(uint32_t))
			{
				return testMagnitude<T, uint32_t>(randomGenerator, worker);
			}

			ocean_assert(false && "This should never happen!");
			return false;
		}

		case FrameType::DT_SIGNED_FLOAT_32:
		{
			if constexpr (sizeof(T) <= sizeof(float))
			{
				return testMagnitude<T, float>(randomGenerator, worker);
			}

			ocean_assert(false && "This should never happen!");
			return false;
		}

		case FrameType::DT_SIGNED_FLOAT_64:
		{
			if constexpr (sizeof(T) <= sizeof(double))
			{
				return testMagnitude<T, double>(randomGenerator, worker);
			}

			ocean_assert(false && "This should never happen!");
			return false;
		}

		default:
			ocean_assert(false && "This should never happen!");
			break;
	}

	return false;
}

template <typename T, typename TMagnitude>
bool TestFrameFilter::testMagnitude(RandomGenerator& randomGenerator, Worker& worker)
{
	const unsigned int width = RandomI::random(randomGenerator, 1u, 1000u);
	const unsigned int height = RandomI::random(randomGenerator, 1u, 1000u);

	const unsigned int maximalChannels = std::is_floating_point<TMagnitude>::value ? 5u : 2u;

	const unsigned int channels = RandomI::random(randomGenerator, 2u, maximalChannels);

	const FrameType::PixelFormat framePixelFormat = FrameType::genericPixelFormat<T>(channels);
	const FrameType::PixelFormat magnitudePixelFormat = FrameType::genericPixelFormat<TMagnitude, 1u>();

	const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, framePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator, true /*limitedValueRange*/);

	Frame magnitude = CV::CVUtilities::randomizedFrame(FrameType(frame, magnitudePixelFormat), &randomGenerator);

	const Frame copyMagnitude(magnitude, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

	if (!CV::CVUtilities::isPaddingMemoryIdentical(magnitude, copyMagnitude))
	{
		ocean_assert(false && "Invalid padding memory!");
		return false;
	}

	Worker* useWorker = RandomI::random(randomGenerator, 1u) == 0u ? &worker : nullptr;

	CV::FrameFilter::template magnitude<T, TMagnitude>(frame.constdata<T>(), magnitude.data<TMagnitude>(), frame.channels(), frame.width(), frame.height(), frame.paddingElements(), magnitude.paddingElements(), useWorker);

	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		for (unsigned int x = 0u; x < frame.width(); ++x)
		{
			double sqrResult = 0.0;

			for (unsigned int n = 0u; n < channels; ++n)
			{
				sqrResult += NumericD::sqr(double(frame.constpixel<T>(x, y)[n]));
			}

			const double result = NumericD::sqrt(sqrResult);

			const double absError = NumericD::abs(result - double(magnitude.constpixel<TMagnitude>(x, y)[0]));

			if (absError > 2.0)
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T, T tNormalizationDenominator>
bool TestFrameFilter::verifyValueNormalization(const T& value)
{
	static_assert(tNormalizationDenominator >= T(1), "Invalid normalization factor!");

	if constexpr (tNormalizationDenominator == T(1))
	{
		if (CV::FrameFilter::normalizeValue<T, tNormalizationDenominator, true>(value) != value)
		{
			return false;
		}

		if (CV::FrameFilter::normalizeValue<T, tNormalizationDenominator, false>(value) != value)
		{
			return false;
		}
	}
	else
	{
		if constexpr (std::is_floating_point<T>::value)
		{
			if (!verifyValueNormalizationFloat<T, tNormalizationDenominator>(value))
			{
				return false;
			}
		}
		else
		{
			if (!verifyValueNormalizationInteger<T, tNormalizationDenominator>(value))
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T, T tNormalizationDenominator>
bool TestFrameFilter::verifyValueNormalizationFloat(const T& value)
{
	static_assert(tNormalizationDenominator > T(1), "Invalid normalization factor!");
	static_assert(std::is_floating_point<T>::value, "Invalid data type!");

	const T normalizedValue = value / tNormalizationDenominator;

	if (NumericT<T>::isNotEqual(CV::FrameFilter::normalizeValue<T, tNormalizationDenominator, true>(value), normalizedValue))
	{
		return false;
	}

	if (NumericT<T>::isNotEqual(CV::FrameFilter::normalizeValue<T, tNormalizationDenominator, false>(value), normalizedValue))
	{
		return false;
	}

	if (NumericT<T>::isNotEqual(CV::FrameFilter::normalizeValueSlow<T, tNormalizationDenominator, true>(value), normalizedValue))
	{
		return false;
	}

	if (NumericT<T>::isNotEqual(CV::FrameFilter::normalizeValueSlow<T, tNormalizationDenominator, false>(value), normalizedValue))
	{
		return false;
	}

	return true;
}

template <typename T, T tNormalizationDenominator>
bool TestFrameFilter::verifyValueNormalizationInteger(const T& value)
{
	static_assert(tNormalizationDenominator > T(1), "Invalid normalization factor!");
	static_assert(!std::is_floating_point<T>::value, "Invalid data type!");

	static_assert(sizeof(T) < sizeof(int64_t), "Invalid data type!");

	if (!verifyValueNormalizationIntegerRounded<T, tNormalizationDenominator>(value))
	{
		return false;
	}

	if (!verifyValueNormalizationIntegerNotRounded<T, tNormalizationDenominator>(value))
	{
		return false;
	}

	return true;
}

template <typename T, T tNormalizationDenominator>
bool TestFrameFilter::verifyValueNormalizationIntegerRounded(const T& value)
{
	static_assert(tNormalizationDenominator > T(1), "Invalid normalization factor!");
	static_assert(!std::is_floating_point<T>::value, "Invalid data type!");

	static_assert(sizeof(T) < sizeof(int64_t), "Invalid data type!");

	constexpr T normalizationDenominator_2 = tNormalizationDenominator / T(2);

	bool isInsideFastValueRange = true;

	if constexpr (std::is_same<T, int32_t>::value)
	{
		if (value <-2147483648 + normalizationDenominator_2 || value > 2147483647 - normalizationDenominator_2)
		{
			isInsideFastValueRange = false;
		}
	}

	if constexpr (std::is_same<T, uint32_t>::value)
	{
		if (value > 4294967295u - normalizationDenominator_2)
		{
			isInsideFastValueRange = false;
		}
	}

	if (isInsideFastValueRange)
	{
		const T rounded = CV::FrameFilter::normalizeValue<T, tNormalizationDenominator, true>(value);

		const int64_t absValue = (std::abs(int64_t(value)) + int64_t(normalizationDenominator_2)) / int64_t(tNormalizationDenominator);

		if (value > 0)
		{
			ocean_assert(NumericT<T>::isInsideValueRange(absValue));

			const T testRounded = T(absValue);

			if (rounded != testRounded)
			{
				return false;
			}
		}
		else
		{
			ocean_assert(NumericT<T>::isInsideValueRange(-absValue));

			const T testRounded = T(-absValue);

			if (rounded != testRounded)
			{
				return false;
			}
		}

		const T slowRounded = CV::FrameFilter::normalizeValueSlow<T, tNormalizationDenominator, true>(value);

		if (rounded != slowRounded)
		{
			return false;
		}
	}
	else
	{
		const T rounded = CV::FrameFilter::normalizeValueSlow<T, tNormalizationDenominator, true>(value);

		const int64_t absValue = (std::abs(int64_t(value)) + int64_t(normalizationDenominator_2)) / int64_t(tNormalizationDenominator);

		if (value > 0)
		{
			ocean_assert(NumericT<T>::isInsideValueRange(absValue));

			const T testRounded = T(absValue);

			if (rounded != testRounded)
			{
				return false;
			}
		}
		else
		{
			ocean_assert(NumericT<T>::isInsideValueRange(-absValue));

			const T testRounded = T(-absValue);

			if (rounded != testRounded)
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T, T tNormalizationDenominator>
bool TestFrameFilter::verifyValueNormalizationIntegerNotRounded(const T& value)
{
	static_assert(tNormalizationDenominator > T(1), "Invalid normalization factor!");
	static_assert(!std::is_floating_point<T>::value, "Invalid data type!");

	static_assert(sizeof(T) < sizeof(int64_t), "Invalid data type!");

	const T notRounded = CV::FrameFilter::normalizeValue<T, tNormalizationDenominator, false>(value);

	const int64_t absValue = std::abs(int64_t(value)) / int64_t(tNormalizationDenominator);

	if (value > 0)
	{
		ocean_assert(NumericT<T>::isInsideValueRange(absValue));

		const T testNotRounded = T(absValue);

		if (notRounded != testNotRounded)
		{
			return false;
		}
	}
	else
	{
		ocean_assert(NumericT<T>::isInsideValueRange(-absValue));

		const T testNotRounded = T(-absValue);

		if (notRounded != testNotRounded)
		{
			return false;
		}
	}

	const T slowNotRounded = CV::FrameFilter::normalizeValueSlow<T, tNormalizationDenominator, false>(value);

	if (notRounded != slowNotRounded)
	{
		return false;
	}

	return true;
}

}

}

}
