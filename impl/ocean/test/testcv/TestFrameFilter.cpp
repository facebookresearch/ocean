// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

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
	//ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Frame filter test:   ---";
	Log::info() << " ";

	allSucceeded = testMagnitude(testDuration, worker) && allSucceeded;

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
	EXPECT_TRUE((TestFrameFilter::testMagnitude(GTEST_TEST_DURATION, worker)));
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

	const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, framePixelFormat, FrameType::ORIGIN_UPPER_LEFT), false, &randomGenerator, true /*limitedValueRange*/);

	Frame magnitude = CV::CVUtilities::randomizedFrame(FrameType(frame, magnitudePixelFormat), false, &randomGenerator);

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

bool TestFrameFilter::isBorderZero(const Frame& frame)
{
	if (frame.isValid() && frame.numberPlanes() == 1u)
	{
		switch (frame.dataType())
		{
			case FrameType::DT_UNSIGNED_INTEGER_8:
				return isBorderZero<uint8_t>(frame);

			case FrameType::DT_SIGNED_INTEGER_8:
				return isBorderZero<int8_t>(frame);

			case FrameType::DT_UNSIGNED_INTEGER_16:
				return isBorderZero<uint16_t>(frame);

			case FrameType::DT_SIGNED_INTEGER_16:
				return isBorderZero<int16_t>(frame);

			case FrameType::DT_UNSIGNED_INTEGER_32:
				return isBorderZero<uint32_t>(frame);

			case FrameType::DT_SIGNED_INTEGER_32:
				return isBorderZero<int32_t>(frame);

			case FrameType::DT_UNSIGNED_INTEGER_64:
				return isBorderZero<uint64_t>(frame);

			case FrameType::DT_SIGNED_INTEGER_64:
				return isBorderZero<int64_t>(frame);

			case FrameType::DT_SIGNED_FLOAT_32:
				return isBorderZero<float>(frame);

			case FrameType::DT_SIGNED_FLOAT_64:
				return isBorderZero<double>(frame);

			default:
				break;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

template <typename T>
bool TestFrameFilter::isBorderZero(const Frame& frame)
{
	const unsigned int width_1 = frame.width() - 1u;
	const unsigned int height_1 = frame.height() - 1u;

	// upper and lower border
	for (unsigned int x = 0u; x < frame.width(); ++x)
	{
		for (unsigned int n = 0u; n < frame.channels(); ++n)
		{
			if (frame.constpixel<T>(x, 0u)[n] != T(0))
			{
				return false;
			}

			if (frame.constpixel<T>(x, height_1)[n] != T(0))
			{
				return false;
			}
		}
	}

	// left and right border
	for (unsigned int y = 0u; y < frame.height(); ++y)
	{
		for (unsigned int n = 0u; n < frame.channels(); ++n)
		{
			if (frame.constpixel<T>(0u, y)[n] != T(0))
			{
				return false;
			}

			if (frame.constpixel<T>(width_1, y)[n] != T(0))
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
