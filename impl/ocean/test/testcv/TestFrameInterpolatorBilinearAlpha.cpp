/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameInterpolatorBilinearAlpha.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameInterpolatorBilinearAlpha.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace Test
{

namespace TestCV
{

bool TestFrameInterpolatorBilinearAlpha::test(const unsigned int width, const unsigned int height, const double testDuration, Worker& /*worker*/)
{
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   Bilinear alpha-interpolation test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testInterpolatePixel8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testInterpolateInfiniteBorder8BitPerChannel(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Bilinear alpha-interpolation test succeeded.";
	}
	else
	{
		Log::info() << "Bilinear alpha-interpolation test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameInterpolatorBilinearAlpha, InterpolatePixel8BitPerChannel_float_TopLeft)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinearAlpha::testInterpolatePixel8BitPerChannel<float>(CV::PC_TOP_LEFT, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinearAlpha, InterpolatePixel8BitPerChannel_float_Center)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinearAlpha::testInterpolatePixel8BitPerChannel<float>(CV::PC_CENTER, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinearAlpha, InterpolatePixel8BitPerChannel_double_TopLeft)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinearAlpha::testInterpolatePixel8BitPerChannel<double>(CV::PC_TOP_LEFT, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinearAlpha, InterpolatePixel8BitPerChannel_double_Center)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinearAlpha::testInterpolatePixel8BitPerChannel<double>(CV::PC_CENTER, GTEST_TEST_DURATION)));
}


TEST(TestFrameInterpolatorBilinearAlpha, InterpolateInfiniteBorder8BitPerChannel_1920x1080_Channel1)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinearAlpha::testInterpolateInfiniteBorder8BitPerChannel<1u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinearAlpha, InterpolateInfiniteBorder8BitPerChannel_1920x1080_Channel2)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinearAlpha::testInterpolateInfiniteBorder8BitPerChannel<2u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinearAlpha, InterpolateInfiniteBorder8BitPerChannel_1920x1080_Channel3)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinearAlpha::testInterpolateInfiniteBorder8BitPerChannel<3u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

TEST(TestFrameInterpolatorBilinearAlpha, InterpolateInfiniteBorder8BitPerChannel_1920x1080_Channel4)
{
	EXPECT_TRUE((TestFrameInterpolatorBilinearAlpha::testInterpolateInfiniteBorder8BitPerChannel<4u>(1920u, 1080u, GTEST_TEST_DURATION)));
}

#endif // OCEAN_USE_GTEST

bool TestFrameInterpolatorBilinearAlpha::testInterpolatePixel8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Pixel interpolation test with 7bit precision:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testInterpolatePixel8BitPerChannel<float>(CV::PC_TOP_LEFT, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInterpolatePixel8BitPerChannel<float>(CV::PC_CENTER, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInterpolatePixel8BitPerChannel<double>(CV::PC_TOP_LEFT, testDuration) && allSucceeded;
	Log::info() << " ";
	allSucceeded = testInterpolatePixel8BitPerChannel<double>(CV::PC_CENTER, testDuration) && allSucceeded;
	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Pixel interpolation test succeeded.";
	}
	else
	{
		Log::info() << "Pixel interpolation test FAILED!";
	}

	return allSucceeded;
}

template <typename TScalar>
bool TestFrameInterpolatorBilinearAlpha::testInterpolatePixel8BitPerChannel(const CV::PixelCenter pixelCenter, const double testDuration)
{
	ocean_assert(pixelCenter == CV::PC_TOP_LEFT || pixelCenter == CV::PC_CENTER);

	ocean_assert(testDuration > 0.0);

	if (pixelCenter == CV::PC_TOP_LEFT)
	{
		Log::info() << "... with '" << TypeNamer::name<TScalar>() << "' and with pixel center at (0.0, 0.0):";
	}
	else
	{
		Log::info() << "... with '" << TypeNamer::name<TScalar>() << "' and with pixel center at (0.5, 0.5):";
	}

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1080u);
		const unsigned int channels = RandomI::random(randomGenerator, 1u, 8u);

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const TScalar xMax = pixelCenter == CV::PC_TOP_LEFT ? TScalar(width - 1u) : TScalar(width);
		const TScalar yMax = pixelCenter == CV::PC_TOP_LEFT ? TScalar(height - 1u) : TScalar(height);

		for (unsigned int n = 0u; n < 100u; ++n)
		{
			const VectorT2<TScalar> position(RandomT<TScalar>::scalar(randomGenerator, TScalar(0), xMax), RandomT<TScalar>::scalar(randomGenerator, TScalar(0), yMax));

			{
				constexpr bool tAlphaAtFront = true;
				constexpr bool tTransparentIs0xFF = true;

				std::vector<uint8_t> resultPixel(channels + 1u);
				std::vector<uint8_t> testPixel(channels);

				for (uint8_t& value : resultPixel)
				{
					value = uint8_t(RandomI::random(randomGenerator, 255u));
				}

				const uint8_t lastValue = resultPixel.back();

				if (!CV::FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::Comfort::interpolatePixel8BitPerChannel(frame.constdata<uint8_t>(), frame.channels(),frame.width(), frame.height(), frame.paddingElements(), pixelCenter, position, resultPixel.data()))
				{
					ocean_assert(false && "This should never happenn!");
					allSucceeded = false;
				}

				if (lastValue != resultPixel.back())
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (interpolatePixel8BitPerChannel(frame, tAlphaAtFront, tTransparentIs0xFF, position, pixelCenter, testPixel.data()))
				{
					if (memcmp(resultPixel.data(), testPixel.data(), channels * sizeof(uint8_t)) != 0)
					{
						allSucceeded = false;
					}
				}
				else
				{
					allSucceeded = false;
				}
			}

			{
				constexpr bool tAlphaAtFront = true;
				constexpr bool tTransparentIs0xFF = false;

				std::vector<uint8_t> resultPixel(channels + 1u);
				std::vector<uint8_t> testPixel(channels);

				for (uint8_t& value : resultPixel)
				{
					value = uint8_t(RandomI::random(randomGenerator, 255u));
				}

				const uint8_t lastValue = resultPixel.back();

				if (!CV::FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::Comfort::interpolatePixel8BitPerChannel(frame.constdata<uint8_t>(), frame.channels(),frame.width(), frame.height(), frame.paddingElements(), pixelCenter, position, resultPixel.data()))
				{
					ocean_assert(false && "This should never happenn!");
					allSucceeded = false;
				}

				if (lastValue != resultPixel.back())
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (interpolatePixel8BitPerChannel(frame, tAlphaAtFront, tTransparentIs0xFF, position, pixelCenter, testPixel.data()))
				{
					if (memcmp(resultPixel.data(), testPixel.data(), channels * sizeof(uint8_t)) != 0)
					{
						allSucceeded = false;
					}
				}
				else
				{
					allSucceeded = false;
				}
			}

			{
				constexpr bool tAlphaAtFront = false;
				constexpr bool tTransparentIs0xFF = true;

				std::vector<uint8_t> resultPixel(channels + 1u);
				std::vector<uint8_t> testPixel(channels);

				for (uint8_t& value : resultPixel)
				{
					value = uint8_t(RandomI::random(randomGenerator, 255u));
				}

				const uint8_t lastValue = resultPixel.back();

				if (!CV::FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::Comfort::interpolatePixel8BitPerChannel(frame.constdata<uint8_t>(), frame.channels(),frame.width(), frame.height(), frame.paddingElements(), pixelCenter, position, resultPixel.data()))
				{
					ocean_assert(false && "This should never happenn!");
					allSucceeded = false;
				}

				if (lastValue != resultPixel.back())
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (interpolatePixel8BitPerChannel(frame, tAlphaAtFront, tTransparentIs0xFF, position, pixelCenter, testPixel.data()))
				{
					if (memcmp(resultPixel.data(), testPixel.data(), channels * sizeof(uint8_t)) != 0)
					{
						allSucceeded = false;
					}
				}
				else
				{
					allSucceeded = false;
				}
			}

			{
				constexpr bool tAlphaAtFront = false;
				constexpr bool tTransparentIs0xFF = false;

				std::vector<uint8_t> resultPixel(channels + 1u);
				std::vector<uint8_t> testPixel(channels);

				for (uint8_t& value : resultPixel)
				{
					value = uint8_t(RandomI::random(randomGenerator, 255u));
				}

				const uint8_t lastValue = resultPixel.back();

				if (!CV::FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::Comfort::interpolatePixel8BitPerChannel(frame.constdata<uint8_t>(), frame.channels(),frame.width(), frame.height(), frame.paddingElements(), pixelCenter, position, resultPixel.data()))
				{
					ocean_assert(false && "This should never happenn!");
					allSucceeded = false;
				}

				if (lastValue != resultPixel.back())
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				if (interpolatePixel8BitPerChannel(frame, tAlphaAtFront, tTransparentIs0xFF, position, pixelCenter, testPixel.data()))
				{
					if (memcmp(resultPixel.data(), testPixel.data(), channels * sizeof(uint8_t)) != 0)
					{
						allSucceeded = false;
					}
				}
				else
				{
					allSucceeded = false;
				}
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

bool TestFrameInterpolatorBilinearAlpha::testInterpolateInfiniteBorder8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	Log::info() << "Infinite border interpolation test:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testInterpolateInfiniteBorder8BitPerChannel<1u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateInfiniteBorder8BitPerChannel<2u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateInfiniteBorder8BitPerChannel<3u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateInfiniteBorder8BitPerChannel<4u>(width, height, testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testInterpolateInfiniteBorder8BitPerChannel<5u>(width, height, testDuration) && allSucceeded;

	return allSucceeded;
}

template <unsigned int tChannels>
bool TestFrameInterpolatorBilinearAlpha::testInterpolateInfiniteBorder8BitPerChannel(const unsigned int width, const unsigned int height, const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	Log::info() << "... with " << tChannels << " channels:";

	RandomGenerator randomGenerator;

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		for (const bool performanceIteration : {true, false})
		{
			const unsigned int testWidth = performanceIteration ? width : RandomI::random(randomGenerator, 2u, width);
			const unsigned int testHeight = performanceIteration ? height : RandomI::random(randomGenerator, 2u, height);

			const unsigned int paddingElements = RandomI::random(randomGenerator, 1u, 100u) * RandomI::random(randomGenerator, 1u);

			Frame frame(FrameType(testWidth, testHeight, FrameType::genericPixelFormat<uint8_t, tChannels>(), FrameType::ORIGIN_UPPER_LEFT), paddingElements);
			CV::CVUtilities::randomizeFrame(frame, false, &randomGenerator);

			for (unsigned int iteration = 0u; iteration < 100u; ++iteration)
			{
				const Vector2 position(Random::scalar(randomGenerator, -20, Scalar(frame.width() + 20u)), Random::scalar(randomGenerator, -20, Scalar(frame.height() + 20u)));

				{
					constexpr bool tAlphaAtFront = true;
					constexpr bool tTransparentIs0xFF = true;

					std::vector<uint8_t> resultPixel(tChannels);
					std::vector<uint8_t> testPixel(tChannels);

					CV::FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolateInfiniteBorder8BitPerChannel<tChannels>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), position, resultPixel.data());
					infiniteBorderInterpolation8BitPerChannel(frame, tAlphaAtFront, tTransparentIs0xFF, position, testPixel.data());

					if (memcmp(resultPixel.data(), testPixel.data(), tChannels * sizeof(uint8_t)) != 0)
					{
						allSucceeded = false;
					}
				}

				{
					constexpr bool tAlphaAtFront = true;
					constexpr bool tTransparentIs0xFF = false;

					std::vector<uint8_t> resultPixel(tChannels);
					std::vector<uint8_t> testPixel(tChannels);

					CV::FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolateInfiniteBorder8BitPerChannel<tChannels>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), position, resultPixel.data());
					infiniteBorderInterpolation8BitPerChannel(frame, tAlphaAtFront, tTransparentIs0xFF, position, testPixel.data());

					if (memcmp(resultPixel.data(), testPixel.data(), tChannels * sizeof(uint8_t)) != 0)
					{
						allSucceeded = false;
					}
				}

				{
					constexpr bool tAlphaAtFront = false;
					constexpr bool tTransparentIs0xFF = true;

					std::vector<uint8_t> resultPixel(tChannels);
					std::vector<uint8_t> testPixel(tChannels);

					CV::FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolateInfiniteBorder8BitPerChannel<tChannels>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), position, resultPixel.data());
					infiniteBorderInterpolation8BitPerChannel(frame, tAlphaAtFront, tTransparentIs0xFF, position, testPixel.data());

					if (memcmp(resultPixel.data(), testPixel.data(), tChannels * sizeof(uint8_t)) != 0)
					{
						allSucceeded = false;
					}
				}

				{
					constexpr bool tAlphaAtFront = false;
					constexpr bool tTransparentIs0xFF = false;

					std::vector<uint8_t> resultPixel(tChannels);
					std::vector<uint8_t> testPixel(tChannels);

					CV::FrameInterpolatorBilinearAlpha<tAlphaAtFront, tTransparentIs0xFF>::interpolateInfiniteBorder8BitPerChannel<tChannels>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), position, resultPixel.data());
					infiniteBorderInterpolation8BitPerChannel(frame, tAlphaAtFront, tTransparentIs0xFF, position, testPixel.data());

					if (memcmp(resultPixel.data(), testPixel.data(), tChannels * sizeof(uint8_t)) != 0)
					{
						allSucceeded = false;
					}
				}
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

template <typename TScalar>
bool TestFrameInterpolatorBilinearAlpha::interpolatePixel8BitPerChannel(const Frame& frame, const bool alphaAtFront, const bool transparentIs0xFF, const VectorT2<TScalar>& position, const CV::PixelCenter pixelCenter, uint8_t* result)
{
	ocean_assert(frame.isValid() && result != nullptr);
	ocean_assert(frame.channels() >= 1u && frame.numberPlanes() == 1u);

	unsigned int leftPixel = (unsigned int)(-1);
	unsigned int topPixel = (unsigned int)(-1);

	TScalar sFactorRight = NumericT<TScalar>::minValue();
	TScalar sFactorBottom = NumericT<TScalar>::minValue();

	if (pixelCenter == CV::PC_TOP_LEFT)
	{
		ocean_assert(position.x() >= TScalar(0) && position.x() <= TScalar(frame.width() - 1u));
		ocean_assert(position.y() >= TScalar(0) && position.y() <= TScalar(frame.height() - 1u));

		if (position.x() < TScalar(0) || position.x() > TScalar(frame.width() - 1u))
		{
			return false;
		}

		if (position.y() < TScalar(0) || position.y() > TScalar(frame.height() - 1u))
		{
			return false;
		}

		leftPixel = (unsigned int)(position.x());
		topPixel = (unsigned int)(position.y());

		sFactorRight = position.x() - TScalar(leftPixel);
		sFactorBottom = position.y() - TScalar(topPixel);
	}
	else
	{
		ocean_assert(pixelCenter == CV::PC_CENTER);

		ocean_assert(position.x() >= TScalar(0) && position.x() <= TScalar(frame.width()));
		ocean_assert(position.y() >= TScalar(0) && position.y() <= TScalar(frame.height()));

		if (position.x() < TScalar(0) || position.x() > TScalar(frame.width()))
		{
			return false;
		}

		if (position.y() < TScalar(0) || position.y() > TScalar(frame.height()))
		{
			return false;
		}

		const VectorT2<TScalar> shiftedPosition = position - VectorT2<TScalar>(TScalar(0.5), TScalar(0.5));
		const VectorT2<TScalar> clampedShiftedPosition(std::max(TScalar(0), shiftedPosition.x()), std::max(TScalar(0), shiftedPosition.y()));

		leftPixel = (unsigned int)(clampedShiftedPosition.x());
		topPixel = (unsigned int)(clampedShiftedPosition.y());

		sFactorRight = clampedShiftedPosition.x() - TScalar(leftPixel);
		sFactorBottom = clampedShiftedPosition.y() - TScalar(topPixel);
	}

	ocean_assert(leftPixel < frame.width());
	ocean_assert(topPixel < frame.height());

	const unsigned int rightPixel = min(leftPixel + 1u, frame.width() - 1u);
	const unsigned int bottomPixel = min(topPixel + 1u, frame.height() - 1u);

	ocean_assert(sFactorRight >= TScalar(0) && sFactorRight <= TScalar(1));
	ocean_assert(sFactorBottom >= TScalar(0) && sFactorBottom <= TScalar(1));

	const unsigned int factorRight = (unsigned int)(sFactorRight * TScalar(128) + TScalar(0.5));
	const unsigned int factorBottom = (unsigned int)(sFactorBottom * TScalar(128) + TScalar(0.5));

	const unsigned int factorLeft = 128u - factorRight;
	const unsigned int factorTop = 128u - factorBottom;

	const uint8_t* pixelTopLeft = frame.constpixel<uint8_t>(leftPixel, topPixel);
	const uint8_t* pixelTopRight = frame.constpixel<uint8_t>(rightPixel, topPixel);
	const uint8_t* pixelBottomLeft = frame.constpixel<uint8_t>(leftPixel, bottomPixel);
	const uint8_t* pixelBottomRight = frame.constpixel<uint8_t>(rightPixel, bottomPixel);

	const unsigned int alphaOffset = alphaAtFront ? 0u : (frame.channels() - 1u);
	const unsigned int colorOffset = alphaAtFront ? 1u : 0u;

	const uint8_t alphaTopLeft = pixelTopLeft[alphaOffset];
	const uint8_t alphaTopRight = pixelTopRight[alphaOffset];
	const uint8_t alphaBottomLeft = pixelBottomLeft[alphaOffset];
	const uint8_t alphaBottomRight = pixelBottomRight[alphaOffset];

	const unsigned int factorTopLeft = factorTop * factorLeft;
	const unsigned int factorTopRight = factorTop * factorRight;
	const unsigned int factorBottomLeft = factorBottom * factorLeft;
	const unsigned int factorBottomRight = factorBottom * factorRight;

	ocean_assert(factorTopLeft + factorTopRight + factorBottomLeft + factorBottomRight == 16384u);

	const unsigned int factorTopLeftAlpha = factorTopLeft * (transparentIs0xFF ? 0xFFu - alphaTopLeft : alphaTopLeft);
	const unsigned int factorTopRightAlpha = factorTopRight * (transparentIs0xFF ? 0xFF - alphaTopRight : alphaTopRight);
	const unsigned int factorBottomLeftAlpha = factorBottomLeft * (transparentIs0xFF ? 0xFF - alphaBottomLeft : alphaBottomLeft);
	const unsigned int factorBottomRightAlpha = factorBottomRight * (transparentIs0xFF ? 0xFF - alphaBottomRight : alphaBottomRight);

	const unsigned int sumFactorsAlpha = factorTopLeftAlpha + factorTopRightAlpha + factorBottomLeftAlpha + factorBottomRightAlpha;

	if (sumFactorsAlpha == 0u)
	{
		// all pixels are fully transparent

		for (unsigned int n = 0; n < frame.channels(); ++n)
		{
			result[n] = uint8_t((pixelTopLeft[n] * factorTopLeft + pixelTopRight[n] * factorTopRight + pixelBottomLeft[n] * factorBottomLeft + pixelBottomRight[n] * factorBottomRight + 8192u) / 16384u);
		}
	}
	else
	{
		const unsigned int sumFactorsAlpha_2 = sumFactorsAlpha / 2u;

		for (unsigned int n = 0u; n < frame.channels() - 1u; ++n)
		{
			result[colorOffset + n] = uint8_t((pixelTopLeft[colorOffset + n] * factorTopLeftAlpha + pixelTopRight[colorOffset + n] * factorTopRightAlpha + pixelBottomLeft[colorOffset + n] * factorBottomLeftAlpha + pixelBottomRight[colorOffset + n] * factorBottomRightAlpha + sumFactorsAlpha_2) / sumFactorsAlpha);
		}

		result[alphaOffset] = uint8_t((pixelTopLeft[alphaOffset] * factorTopLeft + pixelTopRight[alphaOffset] * factorTopRight + pixelBottomLeft[alphaOffset] * factorBottomLeft + pixelBottomRight[alphaOffset] * factorBottomRight + 8192u) / 16384u);
	}

	return true;
}

bool TestFrameInterpolatorBilinearAlpha::infiniteBorderInterpolation8BitPerChannel(const Frame& frame, const bool alphaAtFront, const bool transparentIs0xFF, const Vector2& position, uint8_t* result)
{
	ocean_assert(frame.isValid() && result != nullptr);

	const unsigned int channels = frame.channels();

	// if the position is outside the frame area
	if (position.x() <= Scalar(-0.5) || position.y() <= Scalar(-0.5) || position.x() >= Scalar(frame.width()) + Scalar(0.5) || position.y() >= Scalar(frame.height()) + Scalar(0.5))
	{
		for (unsigned int n = 0u; n < channels; ++n)
		{
			result[n] = 0x00u;
		}

		if (alphaAtFront)
		{
			result[0] = transparentIs0xFF ? 0xFFu : 0x00u;
		}
		else
		{
			result[channels - 1u] = transparentIs0xFF ? 0xFFu : 0x00u;
		}

		return false;
	}

	// if the position is inside the frame core
	if (position.x() >= Scalar(0.5) && position.y() >= Scalar(0.5) && position.x() + Scalar(0.5) < Scalar(frame.width()) && position.y() + Scalar(0.5) < Scalar(frame.height()))
	{
		return interpolatePixel8BitPerChannel(frame, alphaAtFront, transparentIs0xFF, position, CV::PC_CENTER, result);
	}

	// if the position is inside the frame area but not in the core area
	ocean_assert(position.x() < Scalar(0.5) || position.y() < Scalar(0.5) || position.x() >= Scalar(frame.width()) - Scalar(0.5) || position.y() >= Scalar(frame.height()) - Scalar(0.5));

	const int left = int(Numeric::floor(position.x() - Scalar(0.5)));
	const int top = int(Numeric::floor(position.y() - Scalar(0.5)));
	const int right = left + 1;
	const int bottom = top + 1;

	const unsigned int colorLeft = (unsigned int)(max(0, left));
	const unsigned int colorTop = (unsigned int)(max(0, top));
	const unsigned int colorRight = (unsigned int)(min(right, int(frame.width()) - 1));
	const unsigned int colorBottom = (unsigned int)(min(bottom, int(frame.height()) - 1));

	const Scalar tx = (position.x() - Scalar(0.5)) - Scalar(left);
	const Scalar ty = (position.y() - Scalar(0.5)) - Scalar(top);

	ocean_assert(tx >= 0 && tx <= 1);
	ocean_assert(ty >= 0 && ty <= 1);

	const unsigned int txi = (unsigned int)(tx * Scalar(128) + Scalar(0.5));
	const unsigned int tyi = (unsigned int)(ty * Scalar(128) + Scalar(0.5));

	ocean_assert(txi >= 0u && txi <= 128u);
	ocean_assert(tyi >= 0u && tyi <= 128u);

	const unsigned int txi_ = 128u - txi;
	const unsigned int tyi_ = 128u - tyi;

	const uint8_t* colorTopLeft = frame.constpixel<uint8_t>(colorLeft, colorTop);
	const uint8_t* colorTopRight = frame.constpixel<uint8_t>(colorRight, colorTop);
	const uint8_t* colorBottomLeft = frame.constpixel<uint8_t>(colorLeft, colorBottom);
	const uint8_t* colorBottomRight = frame.constpixel<uint8_t>(colorRight, colorBottom);

	const uint8_t transparent = transparentIs0xFF ? 0xFF : 0x00;
	const unsigned int alphaOffset = alphaAtFront ? 0u : (channels - 1u);
	const unsigned int dataOffset = alphaAtFront ? 1u : 0u;

	const uint8_t* alphaTopLeft = (left == int(colorLeft) && top == int(colorTop)) ? colorTopLeft + alphaOffset : &transparent;
	const uint8_t* alphaTopRight = (right == int(colorRight) && top == int(colorTop)) ? colorTopRight + alphaOffset : &transparent;
	const uint8_t* alphaBottomLeft = (left == int(colorLeft) && bottom == int(colorBottom)) ? colorBottomLeft + alphaOffset : &transparent;
	const uint8_t* alphaBottomRight = (right == int(colorRight) && bottom == int(colorBottom)) ? colorBottomRight + alphaOffset : &transparent;

	const unsigned int denominator = (transparentIs0xFF ? 0xFF - *alphaTopLeft : *alphaTopLeft) * txi_ * tyi_
										+ (transparentIs0xFF ? 0xFF - *alphaTopRight : *alphaTopRight) * txi * tyi_
										+ (transparentIs0xFF ? 0xFF - *alphaBottomLeft : *alphaBottomLeft) * txi_ * tyi
										+ (transparentIs0xFF ? 0xFF - *alphaBottomRight : *alphaBottomRight) * txi * tyi;

	if (denominator != 0u)
	{
		const unsigned int denominator_2 = denominator / 2u;

		// color channels (and alpha channel)
		for (unsigned int n = dataOffset; n < channels + dataOffset - 1u; ++n)
		{
			result[n] = uint8_t((colorTopLeft[n] * txi_ * tyi_ * (transparentIs0xFF ? 0xFF - *alphaTopLeft : *alphaTopLeft)
							+ colorTopRight[n] * txi * tyi_ * (transparentIs0xFF ? 0xFF - *alphaTopRight : *alphaTopRight)
							+ colorBottomLeft[n] * txi_ * tyi * (transparentIs0xFF ? 0xFF - *alphaBottomLeft : *alphaBottomLeft)
							+ colorBottomRight[n] * txi * tyi * (transparentIs0xFF ? 0xFF - *alphaBottomRight : *alphaBottomRight) + denominator_2) / denominator);
		}

		// now overwrite the alpha channel
		result[alphaOffset] = uint8_t((*alphaTopLeft * txi_ * tyi_ + *alphaTopRight * txi * tyi_ + *alphaBottomLeft * txi_ * tyi + *alphaBottomRight * txi * tyi + 8192u) / 16384u);
	}
	else
	{
		// color channels (and alpha channel)
		for (unsigned int n = dataOffset; n < channels + dataOffset - 1u; ++n)
		{
			result[n] = uint8_t((colorTopLeft[n] * txi_ * tyi_ + colorTopRight[n] * txi * tyi_ + colorBottomLeft[n] * txi_ * tyi + colorBottomRight[n] * txi * tyi + 8192u) / 16384u);
		}

		// now overwrite the alpha channel
		result[alphaOffset] = uint8_t((*alphaTopLeft * txi_ * tyi_ + *alphaTopRight * txi * tyi_ + *alphaBottomLeft * txi_ * tyi + *alphaBottomRight * txi * tyi + 8192u) / 16384u);
	}

	return true;
}

}

}

}
