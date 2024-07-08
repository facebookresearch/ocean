/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testcv/TestFrameConverter.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/math/Random.h"

#include <array>

namespace Ocean
{

namespace Test
{

namespace TestCV
{

TestFrameConverter::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_1_UINT8)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint8_t, uint16_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_1_UINT16)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const OneSourceGammaOneTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_GAMMA_TO_1_UINT8)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetAlphaConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_1_UINT8_ALPHA)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetBlackLevelWhiteBalanceGammaConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_1_UINT8_BLACKLEVEL_WHITEBALANCE_GAMMA)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint16_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT16_TO_1_UINT8)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint16_t, uint16_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT16_TO_1_UINT16)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint32_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT32_TO_1_UINT8)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint32_t, uint16_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT32_TO_1_UINT16)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const OneSourceTwoTargetsConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_2_UINT8)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const OneSourceThreeTargetsConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_3_UINT8)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const TwoSourcesOneTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_2_UINT8_TO_1_UINT8)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const TwoSourcesThreeTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_2_UINT8_TO_3_UINT8)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const TwoSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_2_UINT8_TO_1_UINT8_ALPHA)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const ThreeSourcesOneTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_3_UINT8_TO_1_UINT8)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const ThreeSourcesThreeTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_3_UINT8_TO_3_UINT8)
{
	// nothing to do here
}

TestFrameConverter::FunctionWrapper::FunctionWrapper(const ThreeSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_3_UINT8_TO_1_UINT8_ALPHA)
{
	// nothing to do here
}

bool TestFrameConverter::FunctionWrapper::invoke(const Frame& source, Frame& target, const FrameConverter::ConversionFlag conversionFlag, const void* options, Worker* worker) const
{
	if (function_ == nullptr || source.width() != target.width() || source.height() != target.height())
	{
		ocean_assert(false && "This must never happen!");
		return false;
	}

	switch (functionType_)
	{
		case FT_1_UINT8_TO_1_UINT8:
			((OneSourceOneTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT8_TO_1_UINT16:
			((OneSourceOneTargetConversionFunction<uint8_t, uint16_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint16_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT8_GAMMA_TO_1_UINT8:
			((OneSourceGammaOneTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, ValueProvider::get().gammaValue(), source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT8_TO_1_UINT8_ALPHA:
			((OneSourceOneTargetAlphaConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), ValueProvider::get().alphaValue(), worker);
			return true;

		case FT_1_UINT8_TO_1_UINT8_BLACKLEVEL_WHITEBALANCE_GAMMA:
		{
			// floatOptions[0] - black level, uint16_t, range: [0, 1024)
			// floatOptions[1] - white balance red, float, range: [0, infinity)
			// floatOptions[2] - white balance green, float, range: [0, infinity)
			// floatOptions[3] - white balance blue, float, range: [0, infinity)
			// floatOptions[4] - gamma, float, range: (0, 2)
			const float* floatOptions = reinterpret_cast<const float*>(options);

			const uint16_t blackLevel = floatOptions ? uint16_t(floatOptions[0] + 0.5f) : uint16_t(0u);

			ocean_assert(floatOptions[1] >= 0.0f && floatOptions[2] >= 0.0f && floatOptions[3] >= 0.0f);
			const float whiteBalance[3] =
			{
				floatOptions[1],
				floatOptions[2],
				floatOptions[3],
			};

			const float gamma = floatOptions ? floatOptions[4] : 1.0f;

			((OneSourceOneTargetBlackLevelWhiteBalanceGammaConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, blackLevel, whiteBalance, gamma, source.paddingElements(0u), target.paddingElements(0u), worker);

			return true;
		}

		case FT_1_UINT16_TO_1_UINT8:
			((OneSourceOneTargetConversionFunction<uint16_t, uint8_t>)(function_))(source.constdata<uint16_t>(0u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT16_TO_1_UINT16:
			((OneSourceOneTargetConversionFunction<uint16_t, uint16_t>)(function_))(source.constdata<uint16_t>(0u), target.data<uint16_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT32_TO_1_UINT8:
			((OneSourceOneTargetConversionFunction<uint32_t, uint8_t>)(function_))(source.constdata<uint32_t>(0u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT32_TO_1_UINT16:
			((OneSourceOneTargetConversionFunction<uint32_t, uint16_t>)(function_))(source.constdata<uint32_t>(0u), target.data<uint16_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), worker);
			return true;

		case FT_1_UINT8_TO_2_UINT8:
			((OneSourceTwoTargetsConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint8_t>(0u), target.data<uint8_t>(1u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), target.paddingElements(1u), worker);
			return true;

		case FT_1_UINT8_TO_3_UINT8:
			((OneSourceThreeTargetsConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), target.data<uint8_t>(0u), target.data<uint8_t>(1u), target.data<uint8_t>(2u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), target.paddingElements(0u), target.paddingElements(1u), target.paddingElements(2u), worker);
			return true;

		case FT_2_UINT8_TO_1_UINT8:
			((TwoSourcesOneTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), target.paddingElements(0u), worker);
			return true;

		case FT_2_UINT8_TO_3_UINT8:
			((TwoSourcesThreeTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), target.data<uint8_t>(0u), target.data<uint8_t>(1u), target.data<uint8_t>(2u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), target.paddingElements(0u), target.paddingElements(1u), target.paddingElements(2u), worker);
			return true;

		case FT_2_UINT8_TO_1_UINT8_ALPHA:
			((TwoSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), target.paddingElements(0u), ValueProvider::get().alphaValue(), worker);
			return true;

		case FT_3_UINT8_TO_1_UINT8:
			((ThreeSourcesOneTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), source.constdata<uint8_t>(2u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), source.paddingElements(2u), target.paddingElements(0u), worker);
			return true;

		case FT_3_UINT8_TO_3_UINT8:
			((ThreeSourcesThreeTargetConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), source.constdata<uint8_t>(2u), target.data<uint8_t>(0u), target.data<uint8_t>(1u), target.data<uint8_t>(2u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), source.paddingElements(2u), target.paddingElements(0u), target.paddingElements(1u), target.paddingElements(2u), worker);
			return true;

		case FT_3_UINT8_TO_1_UINT8_ALPHA:
			((ThreeSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t>)(function_))(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), source.constdata<uint8_t>(2u), target.data<uint8_t>(0u), source.width(), source.height(), conversionFlag, source.paddingElements(0u), source.paddingElements(1u), source.paddingElements(2u), target.paddingElements(0u), ValueProvider::get().alphaValue(), worker);
			return true;

		default:
			break;
	}

	ocean_assert(false && "Invalid function type!");
	return false;
}

bool TestFrameConverter::test(const double testDuration, Worker& /*worker*/)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	Log::info() << "---   Frame converter test:   ---";
	Log::info() << " ";

	allSucceeded = testComfortConvert(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testComfortConvertAndCopy(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";


	allSucceeded = testComfortChange(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCast(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testNormalizedCast(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSubFrame(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testSubFrameMask(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPatchFrame(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testPatchFrameMirroredBorder(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConversionMatrices(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testMapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertOneRow_3Planes1Channel_To_1Plane3Channels_8BitPerChannel_Precision6Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(testDuration) && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testConvertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Frame converter test succeeded.";
	}
	else
	{
		Log::info() << "Frame converter test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestFrameConverter, ComfortConvert)
{
	EXPECT_TRUE(TestFrameConverter::testComfortConvert(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ComfortConvertAndCopy)
{
	EXPECT_TRUE(TestFrameConverter::testComfortConvertAndCopy(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ComfortChange)
{
	EXPECT_TRUE(TestFrameConverter::testComfortChange(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, Cast)
{
	EXPECT_TRUE(TestFrameConverter::testCast(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, NormalizedCast)
{
	EXPECT_TRUE(TestFrameConverter::testNormalizedCast(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, SubFrame)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testSubFrame(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, SubFrameMask)
{
	EXPECT_TRUE(TestFrameConverter::testSubFrameMask(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, PatchFrame)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testPatchFrame(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, PatchFrameMirroredBorder)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testPatchFrameMirroredBorder(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConversionMatrices)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConversionMatrices(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, MapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testMapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, MapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testMapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, MapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testMapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, MapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testMapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, MapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testMapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, MapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testMapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, MapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testMapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(GTEST_TEST_DURATION));
}

TEST(TestFrameConverter, ConvertOneRow_3Planes1Channel_To_1Plane3Channels_8BitPerChannel_Precision6Bit)
{
	Worker worker;
	EXPECT_TRUE(TestFrameConverter::testConvertOneRow_3Planes1Channel_To_1Plane3Channels_8BitPerChannel_Precision6Bit(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestFrameConverter::testComfortConvert(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test comfort convert function:";

	bool allSucceeded = true;

	const FrameType::PixelFormats pixelFormats = CV::CVUtilities::definedPixelFormats();

	// ensuring that we have covered all pixel formats

	if (pixelFormats.size() != size_t(FrameType::FORMAT_END) - 1)
	{
		ocean_assert(false && "Missing pixel format!");
		allSucceeded = false;
	}

	const std::vector<FrameType::DataType> dataTypes =
	{
		FrameType::DataType::DT_UNSIGNED_INTEGER_8,
		FrameType::DataType::DT_SIGNED_INTEGER_8,
		FrameType::DataType::DT_UNSIGNED_INTEGER_16,
		FrameType::DataType::DT_SIGNED_INTEGER_16,
		FrameType::DataType::DT_UNSIGNED_INTEGER_32,
		FrameType::DataType::DT_SIGNED_INTEGER_32,
		FrameType::DataType::DT_UNSIGNED_INTEGER_64,
		FrameType::DataType::DT_SIGNED_INTEGER_64,
		// FrameType::DataType::DT_SIGNED_FLOAT_16, not supported
		FrameType::DataType::DT_SIGNED_FLOAT_32,
		FrameType::DataType::DT_SIGNED_FLOAT_64
	};

	if (dataTypes.size() != size_t(FrameType::DT_END) - 2) // -2 due to missing DT_SIGNED_FLOAT_16
	{
		ocean_assert(false && "Missing data type!");
		allSucceeded = false;
	}

	const std::vector<Options> allOptions =
	{
		Options(),
		Options(uint8_t(255)),
		Options(0.7f),
		Options(64u, 2.0f, 1.0f, 2.0f, 1.5f),
	};

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		FrameType::PixelFormat sourcePixelFormat = RandomI::random(randomGenerator, pixelFormats);
		FrameType::PixelFormat targetPixelFormat = RandomI::random(randomGenerator, pixelFormats);

		Options options = RandomI::random(randomGenerator, allOptions);

		{
			ConversionFunctionMap::FunctionType functionType;
			if (FrameConverter::ConversionFunctionMap::get().function(sourcePixelFormat, targetPixelFormat, functionType, options) == nullptr)
			{
				// the combination is not supported
				continue;
			}
		}

		if (RandomI::random(randomGenerator, 3u) == 3u)
		{
			// using a generic pixel format every 4th iteration

			const FrameType::DataType dataType = RandomI::random(randomGenerator, dataTypes);
			const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

			sourcePixelFormat = FrameType::genericPixelFormat(dataType, channels);
			targetPixelFormat = sourcePixelFormat;

			options = Options();
		}

		const unsigned int widthMultiple = FrameType::widthMultiple(sourcePixelFormat) * FrameType::widthMultiple(targetPixelFormat);
		const unsigned int heightMultiple = FrameType::heightMultiple(sourcePixelFormat) * FrameType::heightMultiple(targetPixelFormat);

		const unsigned int width = RandomI::random(randomGenerator, 1u, 400u) * widthMultiple;
		const unsigned int height = RandomI::random(randomGenerator, 1u, 400u) * heightMultiple;

		const FrameType::PixelOrigin sourcePixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});
		const FrameType::PixelOrigin targetPixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const FrameType sourceFrameType(width, height, sourcePixelFormat, sourcePixelOrigin);

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(sourceFrameType, &randomGenerator);

		const bool forceCopy = RandomI::boolean(randomGenerator);

		Frame targetFrame;

		bool localResult = false;

		constexpr Index32 ID_PIXELFORMAT_AND_PIXELORIGIN = 0u;
		constexpr Index32 ID_PIXELFORMAT = 1u;
		constexpr Index32 ID_PIXELORIGIN = 2u;

		Indices32 possibleFunctionIds(1u, ID_PIXELFORMAT_AND_PIXELORIGIN);

		if (sourcePixelOrigin == targetPixelOrigin)
		{
			possibleFunctionIds.emplace_back(ID_PIXELFORMAT);
		}

		if (sourcePixelFormat == targetPixelFormat)
		{
			possibleFunctionIds.emplace_back(ID_PIXELORIGIN);
		}

		switch (RandomI::random(randomGenerator, possibleFunctionIds))
		{
			case ID_PIXELFORMAT_AND_PIXELORIGIN:
				// testing pixel format and pixel origin function
				localResult = CV::FrameConverter::Comfort::convert(sourceFrame, targetPixelFormat, targetPixelOrigin, targetFrame, forceCopy, nullptr, options);
				break;

			case ID_PIXELFORMAT:
				// testing pixel format-only function
				localResult = CV::FrameConverter::Comfort::convert(sourceFrame, targetPixelFormat, targetFrame, forceCopy, nullptr, options);
				break;

			case ID_PIXELORIGIN:
				// testing pixel origin-only function
				localResult = CV::FrameConverter::Comfort::convert(sourceFrame, targetPixelOrigin, targetFrame, forceCopy, nullptr, options);
				break;

			default:
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
				break;
		}

		if (localResult)
		{
			const FrameType targetFrameType(sourceFrame.frameType(), targetPixelFormat, targetPixelOrigin);

			if (targetFrame.frameType() != targetFrameType)
			{
				allSucceeded = false;
			}

			if (forceCopy && !targetFrame.isOwner())
			{
				allSucceeded = false;
			}

			if (targetFrame.timestamp() != sourceFrame.timestamp())
			{
				allSucceeded = false;
			}

			if (sourcePixelFormat == targetPixelFormat && options.optionsType() == Options::OT_DEFAULT)
			{
				// identical source and pixel formats with default options can be verified by comparing the memory

				Frame convetedTargetFrame;
				if (CV::FrameConverter::Comfort::convert(targetFrame, sourceFrame.pixelFormat(), sourceFrame.pixelOrigin(), convetedTargetFrame, forceCopy))
				{
					for (unsigned int planeIndex = 0u; planeIndex < convetedTargetFrame.numberPlanes(); ++planeIndex)
					{
						const Frame::Plane& sourcePlane = sourceFrame.planes()[planeIndex];
						const Frame::Plane& convertedTargetPlane = convetedTargetFrame.planes()[planeIndex];

						ocean_assert(sourcePlane.widthBytes() == convertedTargetPlane.widthBytes());

						for (unsigned int y = 0u; y < sourcePlane.height(); ++y)
						{
							if (memcmp(reinterpret_cast<const void*>(sourcePlane.constdata<uint8_t>() + y * sourcePlane.strideBytes()), reinterpret_cast<const void*>(convertedTargetPlane.constdata<uint8_t>() + y * convertedTargetPlane.strideBytes()), sourcePlane.widthBytes()) != 0)
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

bool TestFrameConverter::testComfortConvertAndCopy(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test comfort convert & copy function:";

	bool allSucceeded = true;

	const FrameType::PixelFormats pixelFormats = CV::CVUtilities::definedPixelFormats();

	// ensuring that we have covered all pixel formats

	if (pixelFormats.size() != size_t(FrameType::FORMAT_END) - 1)
	{
		ocean_assert(false && "Missing pixel format!");
		allSucceeded = false;
	}

	const std::vector<FrameType::DataType> dataTypes =
	{
		FrameType::DataType::DT_UNSIGNED_INTEGER_8,
		FrameType::DataType::DT_SIGNED_INTEGER_8,
		FrameType::DataType::DT_UNSIGNED_INTEGER_16,
		FrameType::DataType::DT_SIGNED_INTEGER_16,
		FrameType::DataType::DT_UNSIGNED_INTEGER_32,
		FrameType::DataType::DT_SIGNED_INTEGER_32,
		FrameType::DataType::DT_UNSIGNED_INTEGER_64,
		FrameType::DataType::DT_SIGNED_INTEGER_64,
		// FrameType::DataType::DT_SIGNED_FLOAT_16, not supported
		FrameType::DataType::DT_SIGNED_FLOAT_32,
		FrameType::DataType::DT_SIGNED_FLOAT_64
	};

	if (dataTypes.size() != size_t(FrameType::DT_END) - 2) // -2 due to missing DT_SIGNED_FLOAT_16
	{
		ocean_assert(false && "Missing data type!");
		allSucceeded = false;
	}

	Memory externalMemory;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		FrameType::PixelFormat sourcePixelFormat = RandomI::random(randomGenerator, pixelFormats);
		FrameType::PixelFormat targetPixelFormat = RandomI::random(randomGenerator, pixelFormats);

		if (RandomI::random(randomGenerator, 3u) == 3u)
		{
			// using a generic pixel format every 4th iteration

			const FrameType::DataType dataType = RandomI::random(randomGenerator, dataTypes);
			const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

			sourcePixelFormat = FrameType::genericPixelFormat(dataType, channels);
			targetPixelFormat = sourcePixelFormat;
		}

		const unsigned int widthMultiple = FrameType::widthMultiple(sourcePixelFormat) * FrameType::widthMultiple(targetPixelFormat);
		const unsigned int heightMultiple = FrameType::heightMultiple(sourcePixelFormat) * FrameType::heightMultiple(targetPixelFormat);

		const unsigned int width = RandomI::random(randomGenerator, 1u, 400u) * widthMultiple;
		const unsigned int height = RandomI::random(randomGenerator, 1u, 400u) * heightMultiple;

		const FrameType::PixelOrigin sourcePixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});
		const FrameType::PixelOrigin targetPixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const FrameType sourceFrameType(width, height, sourcePixelFormat, sourcePixelOrigin);
		const FrameType targetFrameType(width, height, targetPixelFormat, targetPixelOrigin);

		bool expectSuccess = true;

		if (!FrameConverter::Comfort::isSupported(sourceFrameType, targetPixelFormat))
		{
			// the combination is not supported
			expectSuccess = false;
		}

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(sourceFrameType, &randomGenerator);

		Frame targetFrame;

		const unsigned int selection = RandomI::random(randomGenerator, 4u);

		switch (selection)
		{
			case 0u:
			{
				// target frame is invalid
				expectSuccess = false;
				break;
			}

			case 1u:
			{
				// target frame is owner of the memory
				targetFrame = Frame(targetFrameType);
				break;
			}

			case 2u:
			{
				// target frame is not owner of the writable memory
				externalMemory = Memory(targetFrameType.frameTypeSize() * targetFrameType.numberPlanes());

				Frame::PlaneInitializers<void> planeInitialiers;

				for (unsigned int planeIndex = 0u; planeIndex < targetFrameType.numberPlanes(); ++planeIndex)
				{
					planeInitialiers.emplace_back((void*)(externalMemory.data<uint8_t>() + targetFrameType.frameTypeSize() * planeIndex), Frame::CM_USE_KEEP_LAYOUT);
				}

				targetFrame = Frame(targetFrameType, planeInitialiers);
				break;
			}

			case 3u:
			{
				// target frame is not owning the read-only memory


				// target frame is not owner of the writable memory
				externalMemory = Memory(targetFrameType.frameTypeSize() * targetFrameType.numberPlanes());

				Frame::PlaneInitializers<void> planeInitialiers;

				for (unsigned int planeIndex = 0u; planeIndex < targetFrameType.numberPlanes(); ++planeIndex)
				{
					planeInitialiers.emplace_back((const void*)(externalMemory.data<uint8_t>() + targetFrameType.frameTypeSize() * planeIndex), Frame::CM_USE_KEEP_LAYOUT);
				}

				targetFrame = Frame(targetFrameType, planeInitialiers);

				expectSuccess = false;
				break;
			}

			case 4u:
			{
				// target frame has wrong dimensions
				targetFrame = Frame(FrameType(targetFrameType, targetFrameType.width() + FrameType::widthMultiple(targetPixelFormat), targetFrameType.height() + FrameType::heightMultiple(targetPixelFormat)));

				expectSuccess = false;
				break;
			}
		}

		if (!expectSuccess)
		{
#ifdef OCEAN_DEBUG
			// as the convertAndCopy function has asserts catching invalid input, we must not run the conversion in debug builds
			continue;
#endif
		}

		if (CV::FrameConverter::Comfort::convertAndCopy(sourceFrame, targetFrame) != expectSuccess)
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

bool TestFrameConverter::testComfortChange(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Test comfort change function:";

	bool allSucceeded = true;

	const FrameType::PixelFormats pixelFormats = CV::CVUtilities::definedPixelFormats();

	// ensuring that we have covered all pixel formats

	if (pixelFormats.size() != size_t(FrameType::FORMAT_END) - 1)
	{
		ocean_assert(false && "Missing pixel format!");
		allSucceeded = false;
	}

	const std::vector<FrameType::DataType> dataTypes =
	{
		FrameType::DataType::DT_UNSIGNED_INTEGER_8,
		FrameType::DataType::DT_SIGNED_INTEGER_8,
		FrameType::DataType::DT_UNSIGNED_INTEGER_16,
		FrameType::DataType::DT_SIGNED_INTEGER_16,
		FrameType::DataType::DT_UNSIGNED_INTEGER_32,
		FrameType::DataType::DT_SIGNED_INTEGER_32,
		FrameType::DataType::DT_UNSIGNED_INTEGER_64,
		FrameType::DataType::DT_SIGNED_INTEGER_64,
		// FrameType::DataType::DT_SIGNED_FLOAT_16, not supported
		FrameType::DataType::DT_SIGNED_FLOAT_32,
		FrameType::DataType::DT_SIGNED_FLOAT_64
	};

	if (dataTypes.size() != size_t(FrameType::DT_END) - 2) // -2 due to missing DT_SIGNED_FLOAT_16
	{
		ocean_assert(false && "Missing data type!");
		allSucceeded = false;
	}

	const std::vector<Options> allOptions =
	{
		Options(),
		Options(uint8_t(255)),
		Options(0.7f),
		Options(64u, 2.0f, 1.0f, 2.0f, 1.5f),
	};

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		FrameType::PixelFormat sourcePixelFormat = RandomI::random(randomGenerator, pixelFormats);
		FrameType::PixelFormat targetPixelFormat = RandomI::random(randomGenerator, pixelFormats);

		Options options = RandomI::random(randomGenerator, allOptions);

		{
			ConversionFunctionMap::FunctionType functionType;
			if (FrameConverter::ConversionFunctionMap::get().function(sourcePixelFormat, targetPixelFormat, functionType, options) == nullptr)
			{
				// the combination is not supported
				continue;
			}
		}

		if (RandomI::random(randomGenerator, 3u) == 3u)
		{
			// using a generic pixel format every 4th iteration

			const FrameType::DataType dataType = RandomI::random(randomGenerator, dataTypes);
			const unsigned int channels = RandomI::random(randomGenerator, 1u, 4u);

			sourcePixelFormat = FrameType::genericPixelFormat(dataType, channels);
			targetPixelFormat = sourcePixelFormat;

			options = Options();
		}

		const unsigned int widthMultiple = FrameType::widthMultiple(sourcePixelFormat) * FrameType::widthMultiple(targetPixelFormat);
		const unsigned int heightMultiple = FrameType::heightMultiple(sourcePixelFormat) * FrameType::heightMultiple(targetPixelFormat);

		const unsigned int width = RandomI::random(randomGenerator, 1u, 400u) * widthMultiple;
		const unsigned int height = RandomI::random(randomGenerator, 1u, 400u) * heightMultiple;

		const FrameType::PixelOrigin sourcePixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});
		const FrameType::PixelOrigin targetPixelOrigin = RandomI::random(randomGenerator, {FrameType::ORIGIN_UPPER_LEFT, FrameType::ORIGIN_LOWER_LEFT});

		const FrameType sourceFrameType(width, height, sourcePixelFormat, sourcePixelOrigin);

		Frame frame = CV::CVUtilities::randomizedFrame(sourceFrameType, &randomGenerator);

		const Frame copyFrame(frame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const bool forceCopy = RandomI::boolean(randomGenerator);

		bool localResult = false;

		constexpr Index32 ID_PIXELFORMAT_AND_PIXELORIGIN = 0u;
		constexpr Index32 ID_PIXELFORMAT = 1u;
		constexpr Index32 ID_PIXELORIGIN = 2u;

		Indices32 possibleFunctionIds(1u, ID_PIXELFORMAT_AND_PIXELORIGIN);

		if (sourcePixelOrigin == targetPixelOrigin)
		{
			possibleFunctionIds.emplace_back(ID_PIXELFORMAT);
		}

		if (sourcePixelFormat == targetPixelFormat)
		{
			possibleFunctionIds.emplace_back(ID_PIXELORIGIN);
		}

		switch (RandomI::random(randomGenerator, possibleFunctionIds))
		{
			case ID_PIXELFORMAT_AND_PIXELORIGIN:
				// testing pixel format and pixel origin function
				localResult = CV::FrameConverter::Comfort::change(frame, targetPixelFormat, targetPixelOrigin, forceCopy, nullptr, options);
				break;

			case ID_PIXELFORMAT:
				// testing pixel format-only function
				localResult = CV::FrameConverter::Comfort::change(frame, targetPixelFormat, forceCopy, nullptr, options);
				break;

			case ID_PIXELORIGIN:
				// testing pixel origin-only function
				localResult = CV::FrameConverter::Comfort::change(frame, targetPixelOrigin, forceCopy, nullptr, options);
				break;

			default:
				ocean_assert(false && "This should never happen!");
				allSucceeded = false;
				break;
		}

		if (localResult)
		{
			const FrameType targetFrameType(copyFrame.frameType(), targetPixelFormat, targetPixelOrigin);

			if (frame.frameType() != targetFrameType)
			{
				allSucceeded = false;
			}

			if (forceCopy && !frame.isOwner())
			{
				allSucceeded = false;
			}

			if (frame.timestamp() != copyFrame.timestamp())
			{
				allSucceeded = false;
			}

			if (sourcePixelFormat == targetPixelFormat && options.optionsType() == Options::OT_DEFAULT)
			{
				// identical source and pixel formats with default options can be verified by comparing the memory

				Frame convetedTargetFrame;
				if (CV::FrameConverter::Comfort::convert(frame, copyFrame.pixelFormat(), copyFrame.pixelOrigin(), convetedTargetFrame, forceCopy))
				{
					for (unsigned int planeIndex = 0u; planeIndex < convetedTargetFrame.numberPlanes(); ++planeIndex)
					{
						const Frame::Plane& sourcePlane = copyFrame.planes()[planeIndex];
						const Frame::Plane& convertedTargetPlane = convetedTargetFrame.planes()[planeIndex];

						ocean_assert(sourcePlane.widthBytes() == convertedTargetPlane.widthBytes());

						for (unsigned int y = 0u; y < sourcePlane.height(); ++y)
						{
							if (memcmp(reinterpret_cast<const void*>(sourcePlane.constdata<uint8_t>() + y * sourcePlane.strideBytes()), reinterpret_cast<const void*>(convertedTargetPlane.constdata<uint8_t>() + y * convertedTargetPlane.strideBytes()), sourcePlane.widthBytes()) != 0)
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

bool TestFrameConverter::testCast(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Frame cast test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 100u);
		const unsigned int height = RandomI::random(1u, 100u);

		const unsigned int channels = RandomI::random(1u, 31u);

		allSucceeded = testCast<uint8_t>(width, height, channels) && allSucceeded;

		allSucceeded = testCast<float>(width, height, channels) && allSucceeded;
		allSucceeded = testCast<double>(width, height, channels) && allSucceeded;

		allSucceeded = testCast<int16_t>(width, height, channels) && allSucceeded;
		allSucceeded = testCast<uint16_t>(width, height, channels) && allSucceeded;

		allSucceeded = testCast<int32_t>(width, height, channels) && allSucceeded;
		allSucceeded = testCast<uint32_t>(width, height, channels) && allSucceeded;

		allSucceeded = testCast<int64_t>(width, height, channels) && allSucceeded;
		allSucceeded = testCast<uint64_t>(width, height, channels) && allSucceeded;
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

bool TestFrameConverter::testNormalizedCast(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Frame normalized cast test:";

	bool allSucceeded = true;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 100u);
		const unsigned int height = RandomI::random(1u, 100u);

		const unsigned int channels = RandomI::random(1u, 31u);

		allSucceeded = testNormalizedCast<float>(width, height, channels, RandomF::scalar(-1.0f, 1.0f), RandomF::scalar(-100.0f, 100.0f)) && allSucceeded;
		allSucceeded = testNormalizedCast<double>(width, height, channels, RandomD::scalar(-1.0, 1.0), RandomF::scalar(-100.0, 100.0)) && allSucceeded;

		allSucceeded = testNormalizedCast<int16_t>(width, height, channels, int16_t(RandomI::random(-3, 3)), int16_t(RandomI::random(-100, 100))) && allSucceeded;
		allSucceeded = testNormalizedCast<uint16_t>(width, height, channels, uint16_t(RandomI::random(0u, 3u)), uint16_t(RandomI::random(0u, 100u))) && allSucceeded;

		allSucceeded = testNormalizedCast<int32_t>(width, height, channels, RandomI::random(-10, 10), RandomI::random(-100, 100)) && allSucceeded;
		allSucceeded = testNormalizedCast<uint32_t>(width, height, channels, RandomI::random(0u, 10u), RandomI::random(0, 100)) && allSucceeded;

		allSucceeded = testNormalizedCast<int64_t>(width, height, channels, RandomI::random(-100, 100), RandomI::random(-1000, 1000)) && allSucceeded;
		allSucceeded = testNormalizedCast<uint64_t>(width, height, channels, RandomI::random(0u, 100u), RandomI::random(0, 1000)) && allSucceeded;
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

bool TestFrameConverter::testSubFrame(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Sub-frame test:";

	bool allSucceeded = true;

	allSucceeded = testSubFrame<int8_t>(testDuration) && allSucceeded;
	allSucceeded = testSubFrame<uint8_t>(testDuration) && allSucceeded;
	allSucceeded = testSubFrame<int16_t>(testDuration) && allSucceeded;
	allSucceeded = testSubFrame<float>(testDuration) && allSucceeded;
	allSucceeded = testSubFrame<uint64_t>(testDuration) && allSucceeded;

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

bool TestFrameConverter::testSubFrameMask(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Sub-frame with mask test:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;
	HighPerformanceStatistic performance;

	Timestamp startTimestamp(true);

	do
	{
		const bool measurePerformance = RandomI::boolean(randomGenerator);
		const bool useFrameInterface = RandomI::boolean(randomGenerator);

		const unsigned int sourceWidth = measurePerformance ? 1920u : RandomI::random(randomGenerator, 200u, 1000u);
		const unsigned int sourceHeight = measurePerformance ? 1920u : RandomI::random(randomGenerator, 200u, 1000u);

		const unsigned int targetWidth = measurePerformance ? sourceWidth : RandomI::random(randomGenerator, 200u, 1000u);
		const unsigned int targetHeight = measurePerformance ? sourceHeight : RandomI::random(randomGenerator, 200u, 1000u);

		const CV::PixelPosition sourceTopLeft(measurePerformance ? 0u : RandomI::random(randomGenerator, 0u, sourceWidth / 2u), measurePerformance ? 0u : RandomI::random(randomGenerator, 0u, sourceHeight / 2u));
		const CV::PixelPosition targetTopLeft(measurePerformance ? 0u : RandomI::random(randomGenerator, 0u, targetWidth / 2u), measurePerformance ? 0u : RandomI::random(randomGenerator, 0u, targetHeight / 2u));

		const unsigned int widthRemaining = std::min(sourceWidth - sourceTopLeft.x(), targetWidth - targetTopLeft.x());
		const unsigned int heightRemaining = std::min(sourceHeight - sourceTopLeft.y(), targetHeight - targetTopLeft.y());

		ocean_assert(widthRemaining >= 1u && widthRemaining <= std::min(sourceWidth, targetWidth) && heightRemaining >= 1u && heightRemaining <= std::min(sourceHeight, targetHeight));

		const unsigned int subFrameWidth = measurePerformance ? sourceWidth : RandomI::random(randomGenerator, 1u, widthRemaining);
		const unsigned int subFrameHeight = measurePerformance ? sourceHeight : RandomI::random(randomGenerator, 1u, heightRemaining);

		FrameType::DataType dataType;
		if (measurePerformance)
		{
			dataType = FrameType::DT_UNSIGNED_INTEGER_8;
		}
		else
		{
			dataType = RandomI::random(randomGenerator, {FrameType::DT_UNSIGNED_INTEGER_8, FrameType::DT_SIGNED_FLOAT_32});
		}

		const unsigned int channels = measurePerformance ? 4u : RandomI::random(randomGenerator, 1u, 5u);

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat(dataType, channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(targetWidth, targetHeight, FrameType::genericPixelFormat(dataType, channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const Frame targetFrameCopy(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const uint8_t maskValue = uint8_t(RandomI::random(randomGenerator, 255u));
		const Frame mask = CV::CVUtilities::randomizedBinaryMask(subFrameWidth, subFrameHeight, maskValue, &randomGenerator);

		performance.startIf(measurePerformance);
		if (dataType == FrameType::DT_UNSIGNED_INTEGER_8)
		{
			if (useFrameInterface)
			{
				CV::FrameConverter::subFrameMask<uint8_t>(sourceFrame, targetFrame, mask, sourceTopLeft.x(), sourceTopLeft.y(), targetTopLeft.x(), targetTopLeft.y(), subFrameWidth, subFrameHeight, maskValue);
			}
			else
			{
				CV::FrameConverter::subFrameMask<uint8_t>(sourceFrame.constdata<uint8_t>(), targetFrame.data<uint8_t>(), mask.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.channels(), sourceTopLeft.x(), sourceTopLeft.y(), targetTopLeft.x(), targetTopLeft.y(), subFrameWidth, subFrameHeight, sourceFrame.paddingElements(), targetFrame.paddingElements(), mask.paddingElements(), maskValue);
			}

		}
		else
		{
			ocean_assert(dataType == FrameType::DT_SIGNED_FLOAT_32);
			if (useFrameInterface)
			{
				CV::FrameConverter::subFrameMask<float>(sourceFrame, targetFrame, mask, sourceTopLeft.x(), sourceTopLeft.y(), targetTopLeft.x(), targetTopLeft.y(), subFrameWidth, subFrameHeight, maskValue);
			}
			else
			{
				CV::FrameConverter::subFrameMask<float>(sourceFrame.constdata<float>(), targetFrame.data<float>(), mask.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrame.width(), targetFrame.height(), sourceFrame.channels(), sourceTopLeft.x(), sourceTopLeft.y(), targetTopLeft.x(), targetTopLeft.y(), subFrameWidth, subFrameHeight, sourceFrame.paddingElements(), targetFrame.paddingElements(), mask.paddingElements(), maskValue);
			}
		}
		performance.stopIf(measurePerformance);

		if (dataType == FrameType::DT_UNSIGNED_INTEGER_8)
		{
			if (!validateSubFrameMask<uint8_t>(sourceFrame.channels(), sourceFrame.constdata<uint8_t>(), sourceFrame.width(), sourceFrame.height(), targetFrameCopy.constdata<uint8_t>(), targetFrame.constdata<uint8_t>(), targetFrame.width(), targetFrame.height(), mask.constdata<uint8_t>(), sourceTopLeft.x(), sourceTopLeft.y(), targetTopLeft.x(), targetTopLeft.y(), subFrameWidth, subFrameHeight, sourceFrame.paddingElements(), targetFrame.paddingElements(), mask.paddingElements(), maskValue))
			{
				allSucceeded = false;
			}
		}
		else
		{
			ocean_assert(dataType == FrameType::DT_SIGNED_FLOAT_32);
			if (!validateSubFrameMask<float>(sourceFrame.channels(), sourceFrame.constdata<float>(), sourceFrame.width(), sourceFrame.height(), targetFrameCopy.constdata<float>(), targetFrame.constdata<float>(), targetFrame.width(), targetFrame.height(), mask.constdata<uint8_t>(), sourceTopLeft.x(), sourceTopLeft.y(), targetTopLeft.x(), targetTopLeft.y(), subFrameWidth, subFrameHeight, sourceFrame.paddingElements(), targetFrame.paddingElements(), mask.paddingElements(), maskValue))
			{
				allSucceeded = false;
			}
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, targetFrameCopy))
		{
			// This is serious - abort immediately!
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}
	while (performance.measurements() == 0u && startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Performance first: " << performance.firstMseconds() << "ms";
	Log::info() << "Performance average: " << performance.averageMseconds() << "ms";

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

bool TestFrameConverter::testPatchFrame(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing patch creator:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testPatchFrame<uint8_t>(testDuration) && allSucceeded;
	allSucceeded = testPatchFrame<int16_t>(testDuration) && allSucceeded;
	allSucceeded = testPatchFrame<float>(testDuration) && allSucceeded;
	allSucceeded = testPatchFrame<uint64_t>(testDuration) && allSucceeded;

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

bool TestFrameConverter::testPatchFrameMirroredBorder(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing patch creator with mirrored border:";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testPatchFrameMirroredBorder<uint8_t, 1u>(testDuration) && allSucceeded;
	allSucceeded = testPatchFrameMirroredBorder<uint8_t, 2u>(testDuration) && allSucceeded;
	allSucceeded = testPatchFrameMirroredBorder<uint8_t, 3u>(testDuration) && allSucceeded;
	allSucceeded = testPatchFrameMirroredBorder<uint8_t, 4u>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchFrameMirroredBorder<int16_t, 1u>(testDuration) && allSucceeded;
	allSucceeded = testPatchFrameMirroredBorder<int16_t, 2u>(testDuration) && allSucceeded;
	allSucceeded = testPatchFrameMirroredBorder<int16_t, 3u>(testDuration) && allSucceeded;
	allSucceeded = testPatchFrameMirroredBorder<int16_t, 4u>(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testPatchFrameMirroredBorder<float, 1u>(testDuration) && allSucceeded;
	allSucceeded = testPatchFrameMirroredBorder<float, 2u>(testDuration) && allSucceeded;
	allSucceeded = testPatchFrameMirroredBorder<float, 3u>(testDuration) && allSucceeded;
	allSucceeded = testPatchFrameMirroredBorder<float, 4u>(testDuration) && allSucceeded;

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

bool TestFrameConverter::testConvertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 1-row converter of 3 channels, 2 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels (6 bit precision):";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = (width * 2u) / 2u + source1PaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePlane0UpperAndLowerPixels(source0StrideElements * 2u);
		Elements sourcePlane1Pixels(source1StrideElements);

		Elements targetUpperAndLowerPixels(targetStrideElements * 2u);

		for (uint8_t& element : sourcePlane0UpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetUpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetUpperAndLowerPixels(targetUpperAndLowerPixels);

		int factors[15];

		// padding parameters

		factors[0] = int(source0PaddingElements);
		factors[1] = int(source1PaddingElements);
		factors[2] = int(targetPaddingElements);

		// bias value / translation value

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			factors[12u + n] = RandomI::random(randomGenerator, 0, 128);
		}

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			const int factor0 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);
			const int factor1 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);
			const int factor2 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);

			if (std::abs((255 - factors[12]) * factor0) < 10922 // 10922 = (2^15 - 1) / 3
				&& std::abs((255 - factors[13]) * factor1) < 10922
				&& std::abs((255 - factors[14]) * factor2) < 10922)
			{
				factors[n + 3u] = factor0;
				factors[n + 6u] = factor1;
				factors[n + 9u] = factor2;
			}
			else
			{
				--n;
			}
		}

		const void* sources[2] = {sourcePlane0UpperAndLowerPixels.data(), sourcePlane1Pixels.data()};
		void* targets[1] = {targetUpperAndLowerPixels.data()};

		for (unsigned int row = 0u; row < 2u; ++row)
		{
			CV::FrameConverter::convertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, factors);

			// verify that the conversion is correct
			for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
			{
				// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
				// with transformation:
				// t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
				// t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
				// t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)

				const uint8_t* const source0Upper = sourcePlane0UpperAndLowerPixels.data() + 0;
				const uint8_t* const source0Lower = sourcePlane0UpperAndLowerPixels.data() + source0StrideElements;
				const uint8_t* const source0PixelUpperLeft = source0Upper + x_2 * 2u;
				const uint8_t* const source0PixelLowerLeft = source0Lower + x_2 * 2u;

				const uint8_t* const source12 = sourcePlane1Pixels.data();
				const uint8_t* const source1Pixel = source12 + x_2 * 2u + 0u;
				const uint8_t* const source2Pixel = source12 + x_2 * 2u + 1u;

				if (row == 0u)
				{
					const uint8_t* const target = targetUpperAndLowerPixels.data();
					const uint8_t* const copyTarget = copyTargetUpperAndLowerPixels.data();

					const uint8_t* const targetPixelLeft = target + x_2 * 2u * 3u;

					{
						// upper left pixel

						const int targetUpperLeft0 = minmax<int>(0, (int(source0PixelUpperLeft[0] - factors[12]) * factors[3] + int(source1Pixel[0] - factors[13]) * factors[6] + int(source2Pixel[0] - factors[14]) * factors[9]) / 64, 255);
						const int targetUpperLeft1 = minmax<int>(0, (int(source0PixelUpperLeft[0] - factors[12]) * factors[4] + int(source1Pixel[0] - factors[13]) * factors[7] + int(source2Pixel[0] - factors[14]) * factors[10]) / 64, 255);
						const int targetUpperLeft2 = minmax<int>(0, (int(source0PixelUpperLeft[0] - factors[12]) * factors[5] + int(source1Pixel[0] - factors[13]) * factors[8] + int(source2Pixel[0] - factors[14]) * factors[11]) / 64, 255);

						if (abs(targetUpperLeft0 - int(targetPixelLeft[0u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetUpperLeft1 - int(targetPixelLeft[1u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetUpperLeft2 - int(targetPixelLeft[2u])) > 1)
						{
							allSucceeded = false;
						}
					}

					{
						// upper right pixel

						const int targetUpperRight0 = minmax<int>(0, (int(source0PixelUpperLeft[1] - factors[12]) * factors[3] + int(source1Pixel[0] - factors[13]) * factors[6] + int(source2Pixel[0] - factors[14]) * factors[9]) / 64, 255);
						const int targetUpperRight1 = minmax<int>(0, (int(source0PixelUpperLeft[1] - factors[12]) * factors[4] + int(source1Pixel[0] - factors[13]) * factors[7] + int(source2Pixel[0] - factors[14]) * factors[10]) / 64, 255);
						const int targetUpperRight2 = minmax<int>(0, (int(source0PixelUpperLeft[1] - factors[12]) * factors[5] + int(source1Pixel[0] - factors[13]) * factors[8] + int(source2Pixel[0] - factors[14]) * factors[11]) / 64, 255);

						if (abs(targetUpperRight0 - int(targetPixelLeft[3u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetUpperRight1 - int(targetPixelLeft[4u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetUpperRight2 - int(targetPixelLeft[5u])) > 1)
						{
							allSucceeded = false;
						}
					}

					// verify that the padding elements are untouched
					if (targetPaddingElements > 0u)
					{
						if (memcmp(target + width * 3u, copyTarget + width * 3u, targetPaddingElements) != 0)
						{
							allSucceeded = false;
						}
					}
				}
				else
				{
					ocean_assert(row == 1u);

					const uint8_t* const target = targetUpperAndLowerPixels.data() + targetStrideElements * row;
					const uint8_t* const copyTarget = copyTargetUpperAndLowerPixels.data() + targetStrideElements * row;

					const uint8_t* const targetPixelLeft = target + x_2 * 2u * 3u;

					{
						// lower left pixel

						const int targetLowerLeft0 = minmax<int>(0, (int(source0PixelLowerLeft[0] - factors[12]) * factors[3] + int(source1Pixel[0] - factors[13]) * factors[6] + int(source2Pixel[0] - factors[14]) * factors[9]) / 64, 255);
						const int targetLowerLeft1 = minmax<int>(0, (int(source0PixelLowerLeft[0] - factors[12]) * factors[4] + int(source1Pixel[0] - factors[13]) * factors[7] + int(source2Pixel[0] - factors[14]) * factors[10]) / 64, 255);
						const int targetLowerLeft2 = minmax<int>(0, (int(source0PixelLowerLeft[0] - factors[12]) * factors[5] + int(source1Pixel[0] - factors[13]) * factors[8] + int(source2Pixel[0] - factors[14]) * factors[11]) / 64, 255);

						if (abs(targetLowerLeft0 - int(targetPixelLeft[0u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetLowerLeft1 - int(targetPixelLeft[1u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetLowerLeft2 - int(targetPixelLeft[2u])) > 1)
						{
							allSucceeded = false;
						}
					}

					{
						// lower right pixel

						const int targetLowerRight0 = minmax<int>(0, (int(source0PixelLowerLeft[1] - factors[12]) * factors[3] + int(source1Pixel[0] - factors[13]) * factors[6] + int(source2Pixel[0] - factors[14]) * factors[9]) / 64, 255);
						const int targetLowerRight1 = minmax<int>(0, (int(source0PixelLowerLeft[1] - factors[12]) * factors[4] + int(source1Pixel[0] - factors[13]) * factors[7] + int(source2Pixel[0] - factors[14]) * factors[10]) / 64, 255);
						const int targetLowerRight2 = minmax<int>(0, (int(source0PixelLowerLeft[1] - factors[12]) * factors[5] + int(source1Pixel[0] - factors[13]) * factors[8] + int(source2Pixel[0] - factors[14]) * factors[11]) / 64, 255);

						if (abs(targetLowerRight0 - int(targetPixelLeft[3u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetLowerRight1 - int(targetPixelLeft[4u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetLowerRight2 - int(targetPixelLeft[5u])) > 1)
						{
							allSucceeded = false;
						}
					}

					// verify that the padding elements are untouched
					if (targetPaddingElements > 0u)
					{
						if (memcmp(target + width * 3u, copyTarget + width * 3u, targetPaddingElements) != 0)
						{
							allSucceeded = false;
						}
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

bool TestFrameConverter::testConvertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 1-row converter of 3 channels, 2 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels (10 bit precision):";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = (width * 2u) / 2u + source1PaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePlane0UpperAndLowerPixels(source0StrideElements * 2u);
		Elements sourcePlane1Pixels(source1StrideElements);

		Elements targetUpperAndLowerPixels(targetStrideElements * 2u);

		for (uint8_t& element : sourcePlane0UpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetUpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetUpperAndLowerPixels(targetUpperAndLowerPixels);

		int factors[15];

		factors[0] = int(source0PaddingElements);
		factors[1] = int(source1PaddingElements);
		factors[2] = int(targetPaddingElements);

		// multiplication factors

		for (unsigned int n = 3u; n < 9u; ++n)
		{
			factors[n] = RandomI::random(randomGenerator, -1024, 1024);
		}

		factors[9] = 1024 - factors[3] - factors[6];
		factors[10] = 1024 - factors[4] - factors[7];
		factors[11] = 1024 - factors[5] - factors[8];

		// bias values
		factors[12] = RandomI::random(randomGenerator, -127, 127);
		factors[13] = RandomI::random(randomGenerator, -127, 127);
		factors[14] = RandomI::random(randomGenerator, -127, 127);

		const void* sources[2] = {sourcePlane0UpperAndLowerPixels.data(), sourcePlane1Pixels.data()};
		void* targets[1] = {targetUpperAndLowerPixels.data()};

		for (unsigned int row = 0u; row < 2u; ++row)
		{
			CV::FrameConverter::convertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, factors);

			// verify that the conversion is correct
			for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
			{
				// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
				// with transformation:
				// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
				// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
				// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

				const uint8_t* const source0Upper = sourcePlane0UpperAndLowerPixels.data() + 0;
				const uint8_t* const source0Lower = sourcePlane0UpperAndLowerPixels.data() + source0StrideElements;
				const uint8_t* const source0PixelUpperLeft = source0Upper + x_2 * 2u;
				const uint8_t* const source0PixelLowerLeft = source0Lower + x_2 * 2u;

				const uint8_t* const source12 = sourcePlane1Pixels.data();
				const uint8_t* const source1Pixel = source12 + x_2 * 2u + 0u;
				const uint8_t* const source2Pixel = source12 + x_2 * 2u + 1u;

				if (row == 0u)
				{
					const uint8_t* const target = targetUpperAndLowerPixels.data();
					const uint8_t* const copyTarget = copyTargetUpperAndLowerPixels.data();

					const uint8_t* const targetPixelLeft = target + x_2 * 2u * 3u;

					{
						// upper left pixel

						const int targetUpperLeft0 = minmax<int>(0, (int(source0PixelUpperLeft[0]) * factors[3] + int(source1Pixel[0]) * factors[6] + int(source2Pixel[0]) * factors[9]) / 1024 + factors[12], 255);
						const int targetUpperLeft1 = minmax<int>(0, (int(source0PixelUpperLeft[0]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
						const int targetUpperLeft2 = minmax<int>(0, (int(source0PixelUpperLeft[0]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);

						if (abs(targetUpperLeft0 - int(targetPixelLeft[0u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetUpperLeft1 - int(targetPixelLeft[1u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetUpperLeft2 - int(targetPixelLeft[2u])) > 1)
						{
							allSucceeded = false;
						}
					}

					{
						// upper right pixel

						const int targetUpperRight0 = minmax<int>(0, (int(source0PixelUpperLeft[1]) * factors[3] + int(source1Pixel[0]) * factors[6] + int(source2Pixel[0]) * factors[9]) / 1024 + factors[12], 255);
						const int targetUpperRight1 = minmax<int>(0, (int(source0PixelUpperLeft[1]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
						const int targetUpperRight2 = minmax<int>(0, (int(source0PixelUpperLeft[1]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);

						if (abs(targetUpperRight0 - int(targetPixelLeft[3u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetUpperRight1 - int(targetPixelLeft[4u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetUpperRight2 - int(targetPixelLeft[5u])) > 1)
						{
							allSucceeded = false;
						}
					}

					// verify that the padding elements are untouched
					if (targetPaddingElements > 0u)
					{
						if (memcmp(target + width * 3u, copyTarget + width * 3u, targetPaddingElements) != 0)
						{
							allSucceeded = false;
						}
					}
				}
				else
				{
					ocean_assert(row == 1u);

					const uint8_t* const target = targetUpperAndLowerPixels.data() + targetStrideElements * row;
					const uint8_t* const copyTarget = copyTargetUpperAndLowerPixels.data() + targetStrideElements * row;

					const uint8_t* const targetPixelLeft = target + x_2 * 2u * 3u;

					{
						// lower left pixel

						const int targetLowerLeft0 = minmax<int>(0, (int(source0PixelLowerLeft[0]) * factors[3] + int(source1Pixel[0]) * factors[6] + int(source2Pixel[0]) * factors[9]) / 1024 + factors[12], 255);
						const int targetLowerLeft1 = minmax<int>(0, (int(source0PixelLowerLeft[0]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
						const int targetLowerLeft2 = minmax<int>(0, (int(source0PixelLowerLeft[0]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);

						if (abs(targetLowerLeft0 - int(targetPixelLeft[0u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetLowerLeft1 - int(targetPixelLeft[1u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetLowerLeft2 - int(targetPixelLeft[2u])) > 1)
						{
							allSucceeded = false;
						}
					}

					{
						// lower right pixel

						const int targetLowerRight0 = minmax<int>(0, (int(source0PixelLowerLeft[1]) * factors[3] + int(source1Pixel[0]) * factors[6] + int(source2Pixel[0]) * factors[9]) / 1024 + factors[12], 255);
						const int targetLowerRight1 = minmax<int>(0, (int(source0PixelLowerLeft[1]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
						const int targetLowerRight2 = minmax<int>(0, (int(source0PixelLowerLeft[1]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);

						if (abs(targetLowerRight0 - int(targetPixelLeft[3u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetLowerRight1 - int(targetPixelLeft[4u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetLowerRight2 - int(targetPixelLeft[5u])) > 1)
						{
							allSucceeded = false;
						}
					}

					// verify that the padding elements are untouched
					if (targetPaddingElements > 0u)
					{
						if (memcmp(target + width * 3u, copyTarget + width * 3u, targetPaddingElements) != 0)
						{
							allSucceeded = false;
						}
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

bool TestFrameConverter::testConvertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 2-row converter of 3 channels, 2 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels (6 bit precision):";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = (width * 2u) / 2u + source1PaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePlane0UpperAndLowerPixels(source0StrideElements * 2u);
		Elements sourcePlane1Pixels(source1StrideElements);

		Elements targetUpperAndLowerPixels(targetStrideElements * 2u);

		for (uint8_t& element : sourcePlane0UpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetUpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetUpperAndLowerPixels(targetUpperAndLowerPixels);

		int factors[15];

		factors[0] = int(source0PaddingElements);
		factors[1] = int(source1PaddingElements);
		factors[2] = int(targetPaddingElements);

		// bias values
		factors[12] = RandomI::random(randomGenerator, 0, 128);
		factors[13] = RandomI::random(randomGenerator, 0, 128);
		factors[14] = RandomI::random(randomGenerator, 0, 128);

		// multiplication factors

		for (unsigned int n = 0u; n < 3u; /*noop*/)
		{
			const int factor0 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);
			const int factor1 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);
			const int factor2 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);

			if (std::abs((255 - factors[12]) * factor0) < 10922 // 10922 = (2^15 - 1) / 3
				&& std::abs((255 - factors[13]) * factor1) < 10922
				&& std::abs((255 - factors[14]) * factor2) < 10922)
			{
				factors[n + 3u] = factor0;
				factors[n + 6u] = factor1;
				factors[n + 9u] = factor2;

				++n;
			}
		}

		const void* sources[2] = {sourcePlane0UpperAndLowerPixels.data(), sourcePlane1Pixels.data()};
		void* targets[1] = {targetUpperAndLowerPixels.data()};

		CV::FrameConverter::convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, factors);

		// verify that the conversion is correct
		for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
			// t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
			// t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)

			const uint8_t* const source0Upper = sourcePlane0UpperAndLowerPixels.data() + 0;
			const uint8_t* const source0Lower = sourcePlane0UpperAndLowerPixels.data() + source0StrideElements;
			const uint8_t* const source0PixelUpperLeft = source0Upper + x_2 * 2u;
			const uint8_t* const source0PixelLowerLeft = source0Lower + x_2 * 2u;

			const uint8_t* const source12 = sourcePlane1Pixels.data();
			const uint8_t* const source1Pixel = source12 + x_2 * 2u + 0u;
			const uint8_t* const source2Pixel = source12 + x_2 * 2u + 1u;

			const uint8_t* const targetUpper = targetUpperAndLowerPixels.data() + 0;
			const uint8_t* const targetLower = targetUpperAndLowerPixels.data() + targetStrideElements;

			const uint8_t* const targetPixelUpperLeft = targetUpper + x_2 * 2u * 3u;
			const uint8_t* const targetPixelLowerLeft = targetLower + x_2 * 2u * 3u;

			{
				// upper left pixel

				const int targetUpperLeft0 = minmax<int>(0, (int(source0PixelUpperLeft[0] - factors[12]) * factors[3] + int(source1Pixel[0] - factors[13]) * factors[6] + int(source2Pixel[0] - factors[14]) * factors[ 9]) / 64, 255);
				const int targetUpperLeft1 = minmax<int>(0, (int(source0PixelUpperLeft[0] - factors[12]) * factors[4] + int(source1Pixel[0] - factors[13]) * factors[7] + int(source2Pixel[0] - factors[14]) * factors[10]) / 64, 255);
				const int targetUpperLeft2 = minmax<int>(0, (int(source0PixelUpperLeft[0] - factors[12]) * factors[5] + int(source1Pixel[0] - factors[13]) * factors[8] + int(source2Pixel[0] - factors[14]) * factors[11]) / 64, 255);


				if (abs(targetUpperLeft0 - int(targetPixelUpperLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperLeft1 - int(targetPixelUpperLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperLeft2 - int(targetPixelUpperLeft[2u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// upper right pixel

				const int targetUpperRight0 = minmax<int>(0, (int(source0PixelUpperLeft[1] - factors[12]) * factors[3] + int(source1Pixel[0] - factors[13]) * factors[6] + int(source2Pixel[0] - factors[14]) * factors[ 9]) / 64, 255);
				const int targetUpperRight1 = minmax<int>(0, (int(source0PixelUpperLeft[1] - factors[12]) * factors[4] + int(source1Pixel[0] - factors[13]) * factors[7] + int(source2Pixel[0] - factors[14]) * factors[10]) / 64, 255);
				const int targetUpperRight2 = minmax<int>(0, (int(source0PixelUpperLeft[1] - factors[12]) * factors[5] + int(source1Pixel[0] - factors[13]) * factors[8] + int(source2Pixel[0] - factors[14]) * factors[11]) / 64, 255);

				if (abs(targetUpperRight0 - int(targetPixelUpperLeft[3u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperRight1 - int(targetPixelUpperLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperRight2 - int(targetPixelUpperLeft[5u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// lower left pixel

				const int targetLowerLeft0 = minmax<int>(0, (int(source0PixelLowerLeft[0] - factors[12]) * factors[3] + int(source1Pixel[0] - factors[13]) * factors[6] + int(source2Pixel[0] - factors[14]) * factors[ 9]) / 64, 255);
				const int targetLowerLeft1 = minmax<int>(0, (int(source0PixelLowerLeft[0] - factors[12]) * factors[4] + int(source1Pixel[0] - factors[13]) * factors[7] + int(source2Pixel[0] - factors[14]) * factors[10]) / 64, 255);
				const int targetLowerLeft2 = minmax<int>(0, (int(source0PixelLowerLeft[0] - factors[12]) * factors[5] + int(source1Pixel[0] - factors[13]) * factors[8] + int(source2Pixel[0] - factors[14]) * factors[11]) / 64, 255);

				if (abs(targetLowerLeft0 - int(targetPixelLowerLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerLeft1 - int(targetPixelLowerLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerLeft2 - int(targetPixelLowerLeft[2u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// lower right pixel

				const int targetLowerRight0 = minmax<int>(0, (int(source0PixelLowerLeft[1] - factors[12]) * factors[3] + int(source1Pixel[0] - factors[13]) * factors[6] + int(source2Pixel[0] - factors[14]) * factors[ 9]) / 64, 255);
				const int targetLowerRight1 = minmax<int>(0, (int(source0PixelLowerLeft[1] - factors[12]) * factors[4] + int(source1Pixel[0] - factors[13]) * factors[7] + int(source2Pixel[0] - factors[14]) * factors[10]) / 64, 255);
				const int targetLowerRight2 = minmax<int>(0, (int(source0PixelLowerLeft[1] - factors[12]) * factors[5] + int(source1Pixel[0] - factors[13]) * factors[8] + int(source2Pixel[0] - factors[14]) * factors[11]) / 64, 255);

				if (abs(targetLowerRight0 - int(targetPixelLowerLeft[3u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerRight1 - int(targetPixelLowerLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerRight2 - int(targetPixelLowerLeft[5u])) > 1)
				{
					allSucceeded = false;
				}
			}
		}

		// verify that the padding elements are untouched
		if (targetPaddingElements > 0u)
		{
			// upper row
			if (memcmp(targetUpperAndLowerPixels.data() + width * 3u, copyTargetUpperAndLowerPixels.data() + width * 3u, targetPaddingElements) != 0)
			{
				allSucceeded = false;
			}

			// lower row
			if (memcmp(targetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, copyTargetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testConvertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 2-row converter of 3 channels, 2 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels (10 bit precision):";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = (width * 2u) / 2u + source1PaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePlane0UpperAndLowerPixels(source0StrideElements * 2u);
		Elements sourcePlane1Pixels(source1StrideElements);

		Elements targetUpperAndLowerPixels(targetStrideElements * 2u);

		for (uint8_t& element : sourcePlane0UpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetUpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetUpperAndLowerPixels(targetUpperAndLowerPixels);

		int factors[15];

		factors[0] = int(source0PaddingElements);
		factors[1] = int(source1PaddingElements);
		factors[2] = int(targetPaddingElements);

		// multiplication factors

		for (unsigned int n = 3u; n < 9u; ++n)
		{
			factors[n] = RandomI::random(randomGenerator, -1024, 1024);
		}

		factors[9] = 1024 - factors[3] - factors[6];
		factors[10] = 1024 - factors[4] - factors[7];
		factors[11] = 1024 - factors[5] - factors[8];

		// bias values
		factors[12] = RandomI::random(randomGenerator, -127, 127);
		factors[13] = RandomI::random(randomGenerator, -127, 127);
		factors[14] = RandomI::random(randomGenerator, -127, 127);

		const void* sources[2] = {sourcePlane0UpperAndLowerPixels.data(), sourcePlane1Pixels.data()};
		void* targets[1] = {targetUpperAndLowerPixels.data()};

		CV::FrameConverter::convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, factors);

		// verify that the conversion is correct
		for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
			// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
			// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

			const uint8_t* const source0Upper = sourcePlane0UpperAndLowerPixels.data() + 0;
			const uint8_t* const source0Lower = sourcePlane0UpperAndLowerPixels.data() + source0StrideElements;
			const uint8_t* const source0PixelUpperLeft = source0Upper + x_2 * 2u;
			const uint8_t* const source0PixelLowerLeft = source0Lower + x_2 * 2u;

			const uint8_t* const source12 = sourcePlane1Pixels.data();
			const uint8_t* const source1Pixel = source12 + x_2 * 2u + 0u;
			const uint8_t* const source2Pixel = source12 + x_2 * 2u + 1u;

			const uint8_t* const targetUpper = targetUpperAndLowerPixels.data() + 0;
			const uint8_t* const targetLower = targetUpperAndLowerPixels.data() + targetStrideElements;

			const uint8_t* const targetPixelUpperLeft = targetUpper + x_2 * 2u * 3u;
			const uint8_t* const targetPixelLowerLeft = targetLower + x_2 * 2u * 3u;

			{
				// upper left pixel

				const int targetUpperLeft0 = minmax<int>(0, (int(source0PixelUpperLeft[0]) * factors[3] + int(source1Pixel[0]) * factors[6] + int(source2Pixel[0]) * factors[9]) / 1024 + factors[12], 255);
				const int targetUpperLeft1 = minmax<int>(0, (int(source0PixelUpperLeft[0]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
				const int targetUpperLeft2 = minmax<int>(0, (int(source0PixelUpperLeft[0]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);

				if (abs(targetUpperLeft0 - int(targetPixelUpperLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperLeft1 - int(targetPixelUpperLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperLeft2 - int(targetPixelUpperLeft[2u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// upper right pixel

				const int targetUpperRight0 = minmax<int>(0, (int(source0PixelUpperLeft[1]) * factors[3] + int(source1Pixel[0]) * factors[6] + int(source2Pixel[0]) * factors[9]) / 1024 + factors[12], 255);
				const int targetUpperRight1 = minmax<int>(0, (int(source0PixelUpperLeft[1]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
				const int targetUpperRight2 = minmax<int>(0, (int(source0PixelUpperLeft[1]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);

				if (abs(targetUpperRight0 - int(targetPixelUpperLeft[3u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperRight1 - int(targetPixelUpperLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperRight2 - int(targetPixelUpperLeft[5u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// lower left pixel

				const int targetLowerLeft0 = minmax<int>(0, (int(source0PixelLowerLeft[0]) * factors[3] + int(source1Pixel[0]) * factors[6] + int(source2Pixel[0]) * factors[9]) / 1024 + factors[12], 255);
				const int targetLowerLeft1 = minmax<int>(0, (int(source0PixelLowerLeft[0]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
				const int targetLowerLeft2 = minmax<int>(0, (int(source0PixelLowerLeft[0]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);

				if (abs(targetLowerLeft0 - int(targetPixelLowerLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerLeft1 - int(targetPixelLowerLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerLeft2 - int(targetPixelLowerLeft[2u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// lower right pixel

				const int targetLowerRight0 = minmax<int>(0, (int(source0PixelLowerLeft[1]) * factors[3] + int(source1Pixel[0]) * factors[6] + int(source2Pixel[0]) * factors[9]) / 1024 + factors[12], 255);
				const int targetLowerRight1 = minmax<int>(0, (int(source0PixelLowerLeft[1]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
				const int targetLowerRight2 = minmax<int>(0, (int(source0PixelLowerLeft[1]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);

				if (abs(targetLowerRight0 - int(targetPixelLowerLeft[3u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerRight1 - int(targetPixelLowerLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerRight2 - int(targetPixelLowerLeft[5u])) > 1)
				{
					allSucceeded = false;
				}
			}
		}

		// verify that the padding elements are untouched
		if (targetPaddingElements > 0u)
		{
			// upper row
			if (memcmp(targetUpperAndLowerPixels.data() + width * 3u, copyTargetUpperAndLowerPixels.data() + width * 3u, targetPaddingElements) != 0)
			{
				allSucceeded = false;
			}

			// lower row
			if (memcmp(targetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, copyTargetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testConvertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 2-row converter of 3 channels, 2 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels (7 bit precision):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		constexpr unsigned int height = 2u;

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y_UV12, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		int factors[15];

		factors[0] = int(sourceFrame.paddingElements());
		factors[1] = int(targetFrame.paddingElements(0u));
		factors[2] = int(targetFrame.paddingElements(1u));

		// multiplication factors
		for (unsigned int n = 3u; n < 12u; /*noop*/)
		{
			factors[n] = RandomI::random(randomGenerator, -64, 64);

			if (n == 9u)
			{
				if (abs(factors[3] + factors[6] + factors[9]) > 128)
				{
					continue;
				}
			}
			else if (n == 10u)
			{
				if (abs(factors[4] + factors[7] + factors[10]) > 128)
				{
					continue;
				}
			}
			if (n == 11u)
			{
				if (abs(factors[5] + factors[8] + factors[11]) > 128)
				{
					continue;
				}
			}

			++n;
		}

		// bias values
		for (unsigned int n = 12u; n < 15u; ++n)
		{
			factors[n] = RandomI::random(randomGenerator, -128, 128);
		}

		const void* sources[1] = {sourceFrame.constdata<uint8_t>()};
		void* targets[2] = {targetFrame.data<uint8_t>(0u), targetFrame.data<uint8_t>(1u)};

		CV::FrameConverter::convertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, factors);

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
		{
			ocean_assert(false && "This must never happen!");
			return false;
		}

		// verify that the conversion is correct
		for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = clamp(0, f00 * s0 + f01 * s1 + f02 * s2 + b0, 255)
			// t1 = clamp(0, f10 * s0 + f11 * s1 + f12 * s2 + b1, 255)
			// t2 = clamp(0, f20 * s0 + f21 * s1 + f22 * s2 + b2, 255)

			const uint8_t* const sourcePixelUpperLeft = sourceFrame.constpixel<uint8_t>(x_2 * 2u + 0u, 0u);
			const uint8_t* const sourcePixelLowerLeft = sourceFrame.constpixel<uint8_t>(x_2 * 2u + 0u, 1u);

			const uint8_t* const target0PixelUpperLeft = targetFrame.constpixel<uint8_t>(x_2 * 2u + 0u, 0u);
			const uint8_t* const target0PixelLowerLeft = targetFrame.constpixel<uint8_t>(x_2 * 2u + 0u, 1u);
			const uint8_t* const target1Pixel = targetFrame.constpixel<uint8_t>(x_2, 0u, 1u);

			const int target0ValueUpperLeft = minmax<int>(0, (sourcePixelUpperLeft[0u] * factors[3] + sourcePixelUpperLeft[1u] * factors[6] + sourcePixelUpperLeft[2u] * factors[9]) / 128 + factors[12], 255);
			const int target0ValueUpperRight = minmax<int>(0, (sourcePixelUpperLeft[3u] * factors[3] + sourcePixelUpperLeft[4u] * factors[6] + sourcePixelUpperLeft[5u] * factors[9]) / 128 + factors[12], 255);
			const int target0ValueLowerLeft = minmax<int>(0, (sourcePixelLowerLeft[0u] * factors[3] + sourcePixelLowerLeft[1u] * factors[6] + sourcePixelLowerLeft[2u] * factors[9]) / 128 + factors[12], 255);
			const int target0ValueLowerRight = minmax<int>(0, (sourcePixelLowerLeft[3u] * factors[3] + sourcePixelLowerLeft[4u] * factors[6] + sourcePixelLowerLeft[5u] * factors[9]) / 128 + factors[12], 255);

			if (abs(target0ValueUpperLeft - int(target0PixelUpperLeft[0])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(target0ValueUpperRight - int(target0PixelUpperLeft[1])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(target0ValueLowerLeft - int(target0PixelLowerLeft[0])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(target0ValueLowerRight - int(target0PixelLowerLeft[1])) > 1)
			{
				allSucceeded = false;
			}

			const int averageSourcePixel0 = int((sourcePixelUpperLeft[0] + sourcePixelUpperLeft[3] + sourcePixelLowerLeft[0] + sourcePixelLowerLeft[3] + 2u) / 4u);
			const int averageSourcePixel1 = int((sourcePixelUpperLeft[1] + sourcePixelUpperLeft[4] + sourcePixelLowerLeft[1] + sourcePixelLowerLeft[4] + 2u) / 4u);
			const int averageSourcePixel2 = int((sourcePixelUpperLeft[2] + sourcePixelUpperLeft[5] + sourcePixelLowerLeft[2] + sourcePixelLowerLeft[5] + 2u) / 4u);

			const int target1Value = minmax<int>(0, (averageSourcePixel0 * factors[4] + averageSourcePixel1 * factors[7] + averageSourcePixel2 * factors[10]) / 128 + factors[13], 255);
			const int target2Value = minmax<int>(0, (averageSourcePixel0 * factors[5] + averageSourcePixel1 * factors[8] + averageSourcePixel2 * factors[11]) / 128 + factors[14], 255);

			if (abs(target1Value - int(target1Pixel[0])) > 2)
			{
				allSucceeded = false;
			}

			if (abs(target2Value - int(target1Pixel[1])) > 2)
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

bool TestFrameConverter::testConvertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 2-row converter of 3 channels, 3 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels (7 bit precision):";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		constexpr unsigned int height = 2u;

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::FORMAT_Y_U_V12, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		int factors[16];

		factors[0] = int(sourceFrame.paddingElements());
		factors[1] = int(targetFrame.paddingElements(0u));
		factors[2] = int(targetFrame.paddingElements(1u));
		factors[3] = int(targetFrame.paddingElements(2u));

		// multiplication factors
		for (unsigned int n = 4u; n < 13u; /*noop*/)
		{
			factors[n] = RandomI::random(randomGenerator, -64, 64);

			if (n == 10u)
			{
				if (abs(factors[4] + factors[7] + factors[10]) > 128)
				{
					continue;
				}
			}
			else if (n == 11u)
			{
				if (abs(factors[5] + factors[8] + factors[11]) > 128)
				{
					continue;
				}
			}
			if (n == 12u)
			{
				if (abs(factors[6] + factors[9] + factors[12]) > 128)
				{
					continue;
				}
			}

			++n;
		}

		// bias values
		for (unsigned int n = 13u; n < 16u; ++n)
		{
			factors[n] = RandomI::random(randomGenerator, -128, 128);
		}

		const void* sources[1] = {sourceFrame.constdata<uint8_t>()};
		void* targets[3] = {targetFrame.data<uint8_t>(0u), targetFrame.data<uint8_t>(1u), targetFrame.data<uint8_t>(2u)};

		CV::FrameConverter::convertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, factors);

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
		{
			ocean_assert(false && "This must never happen!");
			return false;
		}

		// verify that the conversion is correct
		for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = clamp(0, f00 * s0 + f01 * s1 + f02 * s2 + b0, 255)
			// t1 = clamp(0, f10 * s0 + f11 * s1 + f12 * s2 + b1, 255)
			// t2 = clamp(0, f20 * s0 + f21 * s1 + f22 * s2 + b2, 255)

			const uint8_t* const sourcePixelUpperLeft = sourceFrame.constpixel<uint8_t>(x_2 * 2u + 0u, 0u);
			const uint8_t* const sourcePixelLowerLeft = sourceFrame.constpixel<uint8_t>(x_2 * 2u + 0u, 1u);

			const uint8_t* const target0PixelUpperLeft = targetFrame.constpixel<uint8_t>(x_2 * 2u + 0u, 0u);
			const uint8_t* const target0PixelLowerLeft = targetFrame.constpixel<uint8_t>(x_2 * 2u + 0u, 1u);
			const uint8_t* const target1Pixel = targetFrame.constpixel<uint8_t>(x_2, 0u, 1u);
			const uint8_t* const target2Pixel = targetFrame.constpixel<uint8_t>(x_2, 0u, 2u);

			const int target0ValueUpperLeft = minmax<int>(0, (sourcePixelUpperLeft[0u] * factors[4] + sourcePixelUpperLeft[1u] * factors[7] + sourcePixelUpperLeft[2u] * factors[10]) / 128 + factors[13], 255);
			const int target0ValueUpperRight = minmax<int>(0, (sourcePixelUpperLeft[3u] * factors[4] + sourcePixelUpperLeft[4u] * factors[7] + sourcePixelUpperLeft[5u] * factors[10]) / 128 + factors[13], 255);
			const int target0ValueLowerLeft = minmax<int>(0, (sourcePixelLowerLeft[0u] * factors[4] + sourcePixelLowerLeft[1u] * factors[7] + sourcePixelLowerLeft[2u] * factors[10]) / 128 + factors[13], 255);
			const int target0ValueLowerRight = minmax<int>(0, (sourcePixelLowerLeft[3u] * factors[4] + sourcePixelLowerLeft[4u] * factors[7] + sourcePixelLowerLeft[5u] * factors[10]) / 128 + factors[13], 255);

			if (abs(target0ValueUpperLeft - int(target0PixelUpperLeft[0])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(target0ValueUpperRight - int(target0PixelUpperLeft[1])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(target0ValueLowerLeft - int(target0PixelLowerLeft[0])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(target0ValueLowerRight - int(target0PixelLowerLeft[1])) > 1)
			{
				allSucceeded = false;
			}

			const int averageSourcePixel0 = int((sourcePixelUpperLeft[0] + sourcePixelUpperLeft[3] + sourcePixelLowerLeft[0] + sourcePixelLowerLeft[3] + 2u) / 4u);
			const int averageSourcePixel1 = int((sourcePixelUpperLeft[1] + sourcePixelUpperLeft[4] + sourcePixelLowerLeft[1] + sourcePixelLowerLeft[4] + 2u) / 4u);
			const int averageSourcePixel2 = int((sourcePixelUpperLeft[2] + sourcePixelUpperLeft[5] + sourcePixelLowerLeft[2] + sourcePixelLowerLeft[5] + 2u) / 4u);

			const int target1Value = minmax<int>(0, (averageSourcePixel0 * factors[5] + averageSourcePixel1 * factors[8] + averageSourcePixel2 * factors[11]) / 128 + factors[14], 255);
			const int target2Value = minmax<int>(0, (averageSourcePixel0 * factors[6] + averageSourcePixel1 * factors[9] + averageSourcePixel2 * factors[12]) / 128 + factors[15], 255);

			if (abs(target1Value - int(target1Pixel[0])) > 2)
			{
				allSucceeded = false;
			}

			if (abs(target2Value - int(target2Pixel[0])) > 2)
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

bool TestFrameConverter::testMapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 1-row map of 3 planes 1 channel to 1 plane 3 channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		constexpr unsigned int height = 1u;

		const Frame sourceFrame0 = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		const Frame sourceFrame1 = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		const Frame sourceFrame2 = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 1u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 3u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const void* sources[3] = {sourceFrame0.constdata<uint8_t>(), sourceFrame1.constdata<uint8_t>(), sourceFrame2.constdata<uint8_t>()};
		void* targets[1] = {targetFrame.data<uint8_t>()};

		for (const unsigned int iteration : {0u, 1u})
		{
			Indices32 lookup;

			const unsigned int options[4] = {sourceFrame0.paddingElements(), sourceFrame1.paddingElements(), sourceFrame2.paddingElements(), targetFrame.paddingElements()};

			switch (iteration)
			{
				case 0u:
					// Y_U_V24 -> YUV24
					lookup = {0u, 1u, 2u};
					CV::FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(sources, targets, 0u, width, 1u, CV::FrameConverter::CONVERT_NORMAL, &options);
					break;

				case 1u:
					// Y_U_V24 -> YVU24
					lookup = {0u, 2u, 1u};
					CV::FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(sources, targets, 0u, width, 1u, CV::FrameConverter::CONVERT_NORMAL, &options);
					break;

				default:
					ocean_assert(false && "This should never happen!");
					allSucceeded = false;
					break;
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "This must never happen!");
				return false;
			}

			for (unsigned int x = 0u; x < width; ++x)
			{
				const uint8_t sourcePixel[3] = {sourceFrame0.constpixel<uint8_t>(x, 0u)[0], sourceFrame1.constpixel<uint8_t>(x, 0u)[0], sourceFrame2.constpixel<uint8_t>(x, 0u)[0]};
				const uint8_t* targetPixel = targetFrame.constpixel<uint8_t>(x, 0u);

				for (unsigned int n = 0u; n < 3u; ++n)
				{
					if (targetPixel[n] != sourcePixel[lookup[n]])
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

bool TestFrameConverter::testMapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 1-row map of 3 channels, 1 plane, with 2x1 downsampling of channel 2 and 3 to zipped 3 channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u) * 2u;
		constexpr unsigned int height = 1u;

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 2u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 3u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const void* sources[1] = {sourceFrame.constdata<uint8_t>()};
		void* targets[1] = {targetFrame.data<uint8_t>()};

		for (const unsigned int iteration : {0u, 1u})
		{
			Indices32 lookup;

			const unsigned int options[2] = {sourceFrame.paddingElements(), targetFrame.paddingElements()};

			switch (iteration)
			{
				case 0u:
					// YUYV16 -> YUV24, YUV24
					lookup = {0u, 1u, 3u, 2u, 1u, 3u};
					CV::FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(sources, targets, 0u, width, 1u, CV::FrameConverter::CONVERT_NORMAL, &options);
					break;

				case 1u:
					// YUYV16 -> YVU24, YVU24
					lookup = {0u, 3u, 1u, 2u, 3u, 1u};
					CV::FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(sources, targets, 0u, width, 1u, CV::FrameConverter::CONVERT_NORMAL, &options);
					break;

				default:
					ocean_assert(false && "This should never happen!");
					allSucceeded = false;
					break;
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "This must never happen!");
				return false;
			}

			for (unsigned int x = 0u; x < width; x += 2u)
			{
				ocean_assert(x + 1u < width);

				const uint8_t* sourcePixel = sourceFrame.constpixel<uint8_t>(x, 0u);
				const uint8_t* targetPixel = targetFrame.constpixel<uint8_t>(x, 0u);

				for (unsigned int n = 0u; n < 6u; ++n)
				{
					if (targetPixel[n] != sourcePixel[lookup[n]])
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

bool TestFrameConverter::testMapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 1-row map of 3 channels, 1 plane, with 2x1 downsampling of channel 1 and 3 to zipped 3 channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		constexpr unsigned int height = 1u;

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 2u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t, 3u>(), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		const void* sources[1] = {sourceFrame.constdata<uint8_t>()};
		void* targets[1] = {targetFrame.data<uint8_t>()};

		for (const unsigned int iteration : {0u, 1u})
		{
			Indices32 lookup;

			const unsigned int options[2] = {sourceFrame.paddingElements(), targetFrame.paddingElements()};

			switch (iteration)
			{
				case 0u:
					// UYVY16 -> YUV24, YUV24
					lookup = {1u, 0u, 2u, 3u, 0u, 2u};
					CV::FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>(sources, targets, 0u, width, 1u, CV::FrameConverter::CONVERT_NORMAL, &options);
					break;

				case 1u:
					// UYVY16 -> YVU24, YVU24
					lookup = {1u, 2u, 0u, 3u, 2u, 0u};
					CV::FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>(sources, targets, 0u, width, 1u, CV::FrameConverter::CONVERT_NORMAL, &options);
					break;

				default:
					ocean_assert(false && "This should never happen!");
					allSucceeded = false;
					break;
			}

			if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
			{
				ocean_assert(false && "This must never happen!");
				return false;
			}

			for (unsigned int x = 0u; x < width; x += 2u)
			{
				ocean_assert(x + 1u < width);

				const uint8_t* sourcePixel = sourceFrame.constpixel<uint8_t>(x, 0u);
				const uint8_t* targetPixel = targetFrame.constpixel<uint8_t>(x, 0u);

				for (unsigned int n = 0u; n < 6u; ++n)
				{
					if (targetPixel[n] != sourcePixel[lookup[n]])
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

bool TestFrameConverter::testMapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 1-row mapping of 3 channels, 2 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels:";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = (width * 2u) / 2u + source1PaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePlane0UpperAndLowerPixels(source0StrideElements * 2u);
		Elements sourcePlane1Pixels(source1StrideElements);

		Elements targetUpperAndLowerPixels(targetStrideElements * 2u);

		for (uint8_t& element : sourcePlane0UpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetUpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetUpperAndLowerPixels(targetUpperAndLowerPixels);

		const unsigned int options[3] = {source0PaddingElements, source1PaddingElements, targetPaddingElements};

		const void* sources[2] = {sourcePlane0UpperAndLowerPixels.data(), sourcePlane1Pixels.data()};
		void* targets[1] = {targetUpperAndLowerPixels.data()};

		unsigned int sourceChannelIndex0 = (unsigned int)(-1);
		unsigned int sourceChannelIndex1 = (unsigned int)(-1);
		unsigned int sourceChannelIndex2 = (unsigned int)(-1);
		RandomI::random(randomGenerator, 2u, sourceChannelIndex0, sourceChannelIndex1, sourceChannelIndex2);

		for (unsigned int row = 0u; row < 2u; ++row)
		{
			switch ((sourceChannelIndex0 << 0u) | (sourceChannelIndex1 << 4u) | (sourceChannelIndex2 << 8u))
			{
				case 0x210u:
					CV::FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
					break;

				case 0x120u:
					CV::FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
					break;

				case 0x201u:
					CV::FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
					break;

				case 0x021u:
					CV::FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
					break;

				case 0x102u:
					CV::FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 0u, 1u>(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
					break;

				case 0x012u:
					CV::FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
					break;

				default:
					ocean_assert(false && "Must never happen!");
					allSucceeded = false;
					break;
			}

			// verify that the conversion is correct
			for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
			{
				// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
				// with transformation:
				// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
				// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
				// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

				const uint8_t* const source0Upper = sourcePlane0UpperAndLowerPixels.data() + 0;
				const uint8_t* const source0PixelUpperLeft = source0Upper + x_2 * 2u;

				const uint8_t* const source12 = sourcePlane1Pixels.data();
				const uint8_t* const source1Pixel = source12 + x_2 * 2u + 0u;
				const uint8_t* const source2Pixel = source12 + x_2 * 2u + 1u;

				const uint8_t* const targetUpper = targetUpperAndLowerPixels.data() + 0;

				const uint8_t* const targetPixelUpperLeft = targetUpper + x_2 * 2u * 3u;

				if (row == 0u)
				{
					{
						// upper left pixel

						const int sourceValues[3] = {source0PixelUpperLeft[0], source1Pixel[0], source2Pixel[0]};

						if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelUpperLeft[0u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelUpperLeft[1u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelUpperLeft[2u])) > 1)
						{
							allSucceeded = false;
						}
					}

					{
						// upper right pixel

						const int sourceValues[3] = {source0PixelUpperLeft[1], source1Pixel[0], source2Pixel[0]};

						if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelUpperLeft[3u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelUpperLeft[4u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelUpperLeft[5u])) > 1)
						{
							allSucceeded = false;
						}
					}
				}
				else
				{
					ocean_assert(row == 1u);

					const uint8_t* const source0Lower = sourcePlane0UpperAndLowerPixels.data() + source0StrideElements;
					const uint8_t* const source0PixelLowerLeft = source0Lower + x_2 * 2u;

					const uint8_t* const targetLower = targetUpperAndLowerPixels.data() + targetStrideElements;

					const uint8_t* const targetPixelLowerLeft = targetLower + x_2 * 2u * 3u;

					{
						// lower left pixel

						const int sourceValues[3] = {source0PixelLowerLeft[0], source1Pixel[0], source2Pixel[0]};

						if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelLowerLeft[0u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelLowerLeft[1u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelLowerLeft[2u])) > 1)
						{
							allSucceeded = false;
						}
					}

					{
						// lower right pixel

						const int sourceValues[3] = {source0PixelLowerLeft[1], source1Pixel[0], source2Pixel[0]};

						if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelLowerLeft[3u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelLowerLeft[4u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelLowerLeft[5u])) > 1)
						{
							allSucceeded = false;
						}
					}
				}
			}

			// verify that the padding elements are untouched
			if (targetPaddingElements > 0u)
			{
				// upper row
				if (memcmp(targetUpperAndLowerPixels.data() + width * 3u, copyTargetUpperAndLowerPixels.data() + width * 3u, targetPaddingElements) != 0)
				{
					allSucceeded = false;
				}

				// lower row
				if (memcmp(targetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, copyTargetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testMapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 2-row mapping of 3 channels, 2 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels:";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = (width * 2u) / 2u + source1PaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePlane0UpperAndLowerPixels(source0StrideElements * 2u);
		Elements sourcePlane1Pixels(source1StrideElements);

		Elements targetUpperAndLowerPixels(targetStrideElements * 2u);

		for (uint8_t& element : sourcePlane0UpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetUpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetUpperAndLowerPixels(targetUpperAndLowerPixels);

		const unsigned int options[3] = {source0PaddingElements, source1PaddingElements, targetPaddingElements};

		const void* sources[2] = {sourcePlane0UpperAndLowerPixels.data(), sourcePlane1Pixels.data()};
		void* targets[1] = {targetUpperAndLowerPixels.data()};

		unsigned int sourceChannelIndex0 = (unsigned int)(-1);
		unsigned int sourceChannelIndex1 = (unsigned int)(-1);
		unsigned int sourceChannelIndex2 = (unsigned int)(-1);
		RandomI::random(randomGenerator, 2u, sourceChannelIndex0, sourceChannelIndex1, sourceChannelIndex2);

		switch ((sourceChannelIndex0 << 0u) | (sourceChannelIndex1 << 4u) | (sourceChannelIndex2 << 8u))
		{
			case 0x210u:
				CV::FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
				break;

			case 0x120u:
				CV::FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
				break;

			case 0x201u:
				CV::FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
				break;

			case 0x021u:
				CV::FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
				break;

			case 0x102u:
				CV::FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 0u, 1u>(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
				break;

			case 0x012u:
				CV::FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
				break;

			default:
				ocean_assert(false && "Must never happen!");
				allSucceeded = false;
				break;
		}

		// verify that the conversion is correct
		for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
			// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
			// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

			const uint8_t* const source0Upper = sourcePlane0UpperAndLowerPixels.data() + 0;
			const uint8_t* const source0Lower = sourcePlane0UpperAndLowerPixels.data() + source0StrideElements;
			const uint8_t* const source0PixelUpperLeft = source0Upper + x_2 * 2u;
			const uint8_t* const source0PixelLowerLeft = source0Lower + x_2 * 2u;

			const uint8_t* const source12 = sourcePlane1Pixels.data();
			const uint8_t* const source1Pixel = source12 + x_2 * 2u + 0u;
			const uint8_t* const source2Pixel = source12 + x_2 * 2u + 1u;

			const uint8_t* const targetUpper = targetUpperAndLowerPixels.data() + 0;
			const uint8_t* const targetLower = targetUpperAndLowerPixels.data() + targetStrideElements;

			const uint8_t* const targetPixelUpperLeft = targetUpper + x_2 * 2u * 3u;
			const uint8_t* const targetPixelLowerLeft = targetLower + x_2 * 2u * 3u;

			{
				// upper left pixel

				const int sourceValues[3] = {source0PixelUpperLeft[0], source1Pixel[0], source2Pixel[0]};

				if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelUpperLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelUpperLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelUpperLeft[2u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// upper right pixel

				const int sourceValues[3] = {source0PixelUpperLeft[1], source1Pixel[0], source2Pixel[0]};

				if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelUpperLeft[3u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelUpperLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelUpperLeft[5u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// lower left pixel

				const int sourceValues[3] = {source0PixelLowerLeft[0], source1Pixel[0], source2Pixel[0]};

				if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelLowerLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelLowerLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelLowerLeft[2u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// lower right pixel

				const int sourceValues[3] = {source0PixelLowerLeft[1], source1Pixel[0], source2Pixel[0]};

				if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelLowerLeft[3u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelLowerLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelLowerLeft[5u])) > 1)
				{
					allSucceeded = false;
				}
			}
		}

		// verify that the padding elements are untouched
		if (targetPaddingElements > 0u)
		{
			// upper row
			if (memcmp(targetUpperAndLowerPixels.data() + width * 3u, copyTargetUpperAndLowerPixels.data() + width * 3u, targetPaddingElements) != 0)
			{
				allSucceeded = false;
			}

			// lower row
			if (memcmp(targetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, copyTargetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testConvertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 1-row converter of 3 channels, 3 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels (10 bit precision):";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source2PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = width / 2u + source1PaddingElements;
		const unsigned int source2StrideElements = width / 2u + source2PaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePlane0UpperAndLowerPixels(source0StrideElements * 2u);
		Elements sourcePlane1Pixels(source1StrideElements);
		Elements sourcePlane2Pixels(source2StrideElements);

		Elements targetUpperAndLowerPixels(targetStrideElements * 2u);

		for (uint8_t& element : sourcePlane0UpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane2Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetUpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetUpperAndLowerPixels(targetUpperAndLowerPixels);

		int factors[16];

		factors[0] = int(source0PaddingElements);
		factors[1] = int(source1PaddingElements);
		factors[2] = int(source2PaddingElements);
		factors[3] = int(targetPaddingElements);

		// multiplication factors

		for (unsigned int n = 4u; n < 10u; ++n)
		{
			factors[n] = RandomI::random(randomGenerator, -1024, 1024);
		}

		factors[10] = 1024 - factors[4] - factors[7];
		factors[11] = 1024 - factors[5] - factors[8];
		factors[12] = 1024 - factors[6] - factors[9];

		// bias values
		factors[13] = RandomI::random(randomGenerator, -127, 127);
		factors[14] = RandomI::random(randomGenerator, -127, 127);
		factors[15] = RandomI::random(randomGenerator, -127, 127);

		const void* sources[3] = {sourcePlane0UpperAndLowerPixels.data(), sourcePlane1Pixels.data(), sourcePlane2Pixels.data()};
		void* targets[1] = {targetUpperAndLowerPixels.data()};

		for (unsigned int row = 0u; row < 2u; ++row)
		{
			CV::FrameConverter::convertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, factors);

			// verify that the conversion is correct
			for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
			{
				// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
				// with transformation:
				// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
				// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
				// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

				const uint8_t* const source0Upper = sourcePlane0UpperAndLowerPixels.data() + 0;
				const uint8_t* const source0PixelUpperLeft = source0Upper + x_2 * 2u;

				const uint8_t* const source1 = sourcePlane1Pixels.data();
				const uint8_t* const source2 = sourcePlane2Pixels.data();
				const uint8_t* const source1Pixel = source1 + x_2;
				const uint8_t* const source2Pixel = source2 + x_2;

				const uint8_t* const targetUpper = targetUpperAndLowerPixels.data() + 0;

				const uint8_t* const targetPixelUpperLeft = targetUpper + x_2 * 2u * 3u;

				if (row == 0u)
				{
					{
						// upper left pixel

						const int targetUpperLeft0 = minmax<int>(0, (int(source0PixelUpperLeft[0]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
						const int targetUpperLeft1 = minmax<int>(0, (int(source0PixelUpperLeft[0]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);
						const int targetUpperLeft2 = minmax<int>(0, (int(source0PixelUpperLeft[0]) * factors[6] + int(source1Pixel[0]) * factors[9] + int(source2Pixel[0]) * factors[12]) / 1024 + factors[15], 255);

						if (abs(targetUpperLeft0 - int(targetPixelUpperLeft[0u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetUpperLeft1 - int(targetPixelUpperLeft[1u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetUpperLeft2 - int(targetPixelUpperLeft[2u])) > 1)
						{
							allSucceeded = false;
						}
					}

					{
						// upper right pixel

						const int targetUpperRight0 = minmax<int>(0, (int(source0PixelUpperLeft[1]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
						const int targetUpperRight1 = minmax<int>(0, (int(source0PixelUpperLeft[1]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);
						const int targetUpperRight2 = minmax<int>(0, (int(source0PixelUpperLeft[1]) * factors[6] + int(source1Pixel[0]) * factors[9] + int(source2Pixel[0]) * factors[12]) / 1024 + factors[15], 255);

						if (abs(targetUpperRight0 - int(targetPixelUpperLeft[3u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetUpperRight1 - int(targetPixelUpperLeft[4u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetUpperRight2 - int(targetPixelUpperLeft[5u])) > 1)
						{
							allSucceeded = false;
						}
					}
				}
				else
				{
					ocean_assert(row == 1u);

					const uint8_t* const source0Lower = sourcePlane0UpperAndLowerPixels.data() + source0StrideElements;
					const uint8_t* const source0PixelLowerLeft = source0Lower + x_2 * 2u;

					const uint8_t* const targetLower = targetUpperAndLowerPixels.data() + targetStrideElements;
					const uint8_t* const targetPixelLowerLeft = targetLower + x_2 * 2u * 3u;

					{
						// lower left pixel

						const int targetLowerLeft0 = minmax<int>(0, (int(source0PixelLowerLeft[0]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
						const int targetLowerLeft1 = minmax<int>(0, (int(source0PixelLowerLeft[0]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);
						const int targetLowerLeft2 = minmax<int>(0, (int(source0PixelLowerLeft[0]) * factors[6] + int(source1Pixel[0]) * factors[9] + int(source2Pixel[0]) * factors[12]) / 1024 + factors[15], 255);

						if (abs(targetLowerLeft0 - int(targetPixelLowerLeft[0u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetLowerLeft1 - int(targetPixelLowerLeft[1u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetLowerLeft2 - int(targetPixelLowerLeft[2u])) > 1)
						{
							allSucceeded = false;
						}
					}

					{
						// lower right pixel

						const int targetLowerRight0 = minmax<int>(0, (int(source0PixelLowerLeft[1]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
						const int targetLowerRight1 = minmax<int>(0, (int(source0PixelLowerLeft[1]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);
						const int targetLowerRight2 = minmax<int>(0, (int(source0PixelLowerLeft[1]) * factors[6] + int(source1Pixel[0]) * factors[9] + int(source2Pixel[0]) * factors[12]) / 1024 + factors[15], 255);

						if (abs(targetLowerRight0 - int(targetPixelLowerLeft[3u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetLowerRight1 - int(targetPixelLowerLeft[4u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(targetLowerRight2 - int(targetPixelLowerLeft[5u])) > 1)
						{
							allSucceeded = false;
						}
					}
				}
			}
		}

		// verify that the padding elements are untouched
		if (targetPaddingElements > 0u)
		{
			// upper row
			if (memcmp(targetUpperAndLowerPixels.data() + width * 3u, copyTargetUpperAndLowerPixels.data() + width * 3u, targetPaddingElements) != 0)
			{
				allSucceeded = false;
			}

			// lower row
			if (memcmp(targetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, copyTargetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 2-row converter of 3 channels, 3 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels (6 bit precision):";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source2PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = width / 2u + source1PaddingElements;
		const unsigned int source2StrideElements = width / 2u + source2PaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePlane0UpperAndLowerPixels(source0StrideElements * 2u);
		Elements sourcePlane1Pixels(source1StrideElements);
		Elements sourcePlane2Pixels(source2StrideElements);

		Elements targetUpperAndLowerPixels(targetStrideElements * 2u);

		for (uint8_t& element : sourcePlane0UpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane2Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetUpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetUpperAndLowerPixels(targetUpperAndLowerPixels);

		int factors[16];

		factors[0] = int(source0PaddingElements);
		factors[1] = int(source1PaddingElements);
		factors[2] = int(source2PaddingElements);
		factors[3] = int(targetPaddingElements);

		// bias value / translation value

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			factors[13u + n] = RandomI::random(randomGenerator, 0, 128);
		}

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			const int factor0 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);
			const int factor1 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);
			const int factor2 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);

			if (std::abs((255 - factors[12]) * factor0) < 10922 // 10922 = (2^15 - 1) / 3
				&& std::abs((255 - factors[13]) * factor1) < 10922
				&& std::abs((255 - factors[14]) * factor2) < 10922)
			{
				factors[n + 4u] = factor0;
				factors[n + 7u] = factor1;
				factors[n + 10u] = factor2;
			}
			else
			{
				--n;
			}
		}

		const void* sources[3] = {sourcePlane0UpperAndLowerPixels.data(), sourcePlane1Pixels.data(), sourcePlane2Pixels.data()};
		void* targets[1] = {targetUpperAndLowerPixels.data()};

		CV::FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, factors);

		// verify that the conversion is correct
		for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
			// t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
			// t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)

			const uint8_t* const source0Upper = sourcePlane0UpperAndLowerPixels.data() + 0;
			const uint8_t* const source0Lower = sourcePlane0UpperAndLowerPixels.data() + source0StrideElements;
			const uint8_t* const source0PixelUpperLeft = source0Upper + x_2 * 2u;
			const uint8_t* const source0PixelLowerLeft = source0Lower + x_2 * 2u;

			const uint8_t* const source1 = sourcePlane1Pixels.data();
			const uint8_t* const source2 = sourcePlane2Pixels.data();
			const uint8_t* const source1Pixel = source1 + x_2;
			const uint8_t* const source2Pixel = source2 + x_2;

			const uint8_t* const targetUpper = targetUpperAndLowerPixels.data() + 0;
			const uint8_t* const targetLower = targetUpperAndLowerPixels.data() + targetStrideElements;

			const uint8_t* const targetPixelUpperLeft = targetUpper + x_2 * 2u * 3u;
			const uint8_t* const targetPixelLowerLeft = targetLower + x_2 * 2u * 3u;

			{
				// upper left pixel

				const int targetUpperLeft0 = minmax<int>(0, (int(source0PixelUpperLeft[0] - factors[13]) * factors[4] + int(source1Pixel[0] - factors[14]) * factors[7] + int(source2Pixel[0] - factors[15]) * factors[10]) / 64, 255);
				const int targetUpperLeft1 = minmax<int>(0, (int(source0PixelUpperLeft[0] - factors[13]) * factors[5] + int(source1Pixel[0] - factors[14]) * factors[8] + int(source2Pixel[0] - factors[15]) * factors[11]) / 64, 255);
				const int targetUpperLeft2 = minmax<int>(0, (int(source0PixelUpperLeft[0] - factors[13]) * factors[6] + int(source1Pixel[0] - factors[14]) * factors[9] + int(source2Pixel[0] - factors[15]) * factors[12]) / 64, 255);


				if (abs(targetUpperLeft0 - int(targetPixelUpperLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperLeft1 - int(targetPixelUpperLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperLeft2 - int(targetPixelUpperLeft[2u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// upper right pixel

				const int targetUpperRight0 = minmax<int>(0, (int(source0PixelUpperLeft[1] - factors[13]) * factors[4] + int(source1Pixel[0] - factors[14]) * factors[7] + int(source2Pixel[0] - factors[15]) * factors[10]) / 64, 255);
				const int targetUpperRight1 = minmax<int>(0, (int(source0PixelUpperLeft[1] - factors[13]) * factors[5] + int(source1Pixel[0] - factors[14]) * factors[8] + int(source2Pixel[0] - factors[15]) * factors[11]) / 64, 255);
				const int targetUpperRight2 = minmax<int>(0, (int(source0PixelUpperLeft[1] - factors[13]) * factors[6] + int(source1Pixel[0] - factors[14]) * factors[9] + int(source2Pixel[0] - factors[15]) * factors[12]) / 64, 255);

				if (abs(targetUpperRight0 - int(targetPixelUpperLeft[3u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperRight1 - int(targetPixelUpperLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperRight2 - int(targetPixelUpperLeft[5u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// lower left pixel

				const int targetLowerLeft0 = minmax<int>(0, (int(source0PixelLowerLeft[0] - factors[13]) * factors[4] + int(source1Pixel[0] - factors[14]) * factors[7] + int(source2Pixel[0] - factors[15]) * factors[10]) / 64, 255);
				const int targetLowerLeft1 = minmax<int>(0, (int(source0PixelLowerLeft[0] - factors[13]) * factors[5] + int(source1Pixel[0] - factors[14]) * factors[8] + int(source2Pixel[0] - factors[15]) * factors[11]) / 64, 255);
				const int targetLowerLeft2 = minmax<int>(0, (int(source0PixelLowerLeft[0] - factors[13]) * factors[6] + int(source1Pixel[0] - factors[14]) * factors[9] + int(source2Pixel[0] - factors[15]) * factors[12]) / 64, 255);

				if (abs(targetLowerLeft0 - int(targetPixelLowerLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerLeft1 - int(targetPixelLowerLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerLeft2 - int(targetPixelLowerLeft[2u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// lower right pixel

				const int targetLowerRight0 = minmax<int>(0, (int(source0PixelLowerLeft[1] - factors[13]) * factors[4] + int(source1Pixel[0] - factors[14]) * factors[7] + int(source2Pixel[0] - factors[15]) * factors[10]) / 64, 255);
				const int targetLowerRight1 = minmax<int>(0, (int(source0PixelLowerLeft[1] - factors[13]) * factors[5] + int(source1Pixel[0] - factors[14]) * factors[8] + int(source2Pixel[0] - factors[15]) * factors[11]) / 64, 255);
				const int targetLowerRight2 = minmax<int>(0, (int(source0PixelLowerLeft[1] - factors[13]) * factors[6] + int(source1Pixel[0] - factors[14]) * factors[9] + int(source2Pixel[0] - factors[15]) * factors[12]) / 64, 255);

				if (abs(targetLowerRight0 - int(targetPixelLowerLeft[3u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerRight1 - int(targetPixelLowerLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerRight2 - int(targetPixelLowerLeft[5u])) > 1)
				{
					allSucceeded = false;
				}
			}
		}

		// verify that the padding elements are untouched
		if (targetPaddingElements > 0u)
		{
			// upper row
			if (memcmp(targetUpperAndLowerPixels.data() + width * 3u, copyTargetUpperAndLowerPixels.data() + width * 3u, targetPaddingElements) != 0)
			{
				allSucceeded = false;
			}

			// lower row
			if (memcmp(targetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, copyTargetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 2-row converter of 3 channels, 3 planes, with 2x2 downsampling of channel 2 and 3 to a 1-plane 4 channels (6 bit precision):";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source2PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = width / 2u + source1PaddingElements;
		const unsigned int source2StrideElements = width / 2u + source2PaddingElements;
		const unsigned int targetStrideElements = 4u * width + targetPaddingElements;

		Elements sourcePlane0UpperAndLowerPixels(source0StrideElements * 2u);
		Elements sourcePlane1Pixels(source1StrideElements);
		Elements sourcePlane2Pixels(source2StrideElements);

		Elements targetUpperAndLowerPixels(targetStrideElements * 2u);

		for (uint8_t& element : sourcePlane0UpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane2Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetUpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetUpperAndLowerPixels(targetUpperAndLowerPixels);

		int factors[17];

		factors[0] = int(source0PaddingElements);
		factors[1] = int(source1PaddingElements);
		factors[2] = int(source2PaddingElements);
		factors[3] = int(targetPaddingElements);

		// bias value / translation value

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			factors[13u + n] = RandomI::random(randomGenerator, 0, 128);
		}

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			const int factor0 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);
			const int factor1 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);
			const int factor2 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);

			if (std::abs((255 - factors[12]) * factor0) < 10922 // 10922 = (2^15 - 1) / 3
				&& std::abs((255 - factors[13]) * factor1) < 10922
				&& std::abs((255 - factors[14]) * factor2) < 10922)
			{
				factors[n + 4u] = factor0;
				factors[n + 7u] = factor1;
				factors[n + 10u] = factor2;
			}
			else
			{
				--n;
			}
		}

		const uint8_t alphaValue = uint8_t(RandomI::random(randomGenerator, 255u));

		factors[16] = int(alphaValue);

		const void* sources[3] = {sourcePlane0UpperAndLowerPixels.data(), sourcePlane1Pixels.data(), sourcePlane2Pixels.data()};
		void* targets[1] = {targetUpperAndLowerPixels.data()};

		CV::FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, factors);

		// verify that the conversion is correct
		for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
			// t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
			// t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)

			const uint8_t* const source0Upper = sourcePlane0UpperAndLowerPixels.data() + 0;
			const uint8_t* const source0Lower = sourcePlane0UpperAndLowerPixels.data() + source0StrideElements;
			const uint8_t* const source0PixelUpperLeft = source0Upper + x_2 * 2u;
			const uint8_t* const source0PixelLowerLeft = source0Lower + x_2 * 2u;

			const uint8_t* const source1 = sourcePlane1Pixels.data();
			const uint8_t* const source2 = sourcePlane2Pixels.data();
			const uint8_t* const source1Pixel = source1 + x_2;
			const uint8_t* const source2Pixel = source2 + x_2;

			const uint8_t* const targetUpper = targetUpperAndLowerPixels.data() + 0;
			const uint8_t* const targetLower = targetUpperAndLowerPixels.data() + targetStrideElements;

			const uint8_t* const targetPixelUpperLeft = targetUpper + x_2 * 2u * 4u;
			const uint8_t* const targetPixelLowerLeft = targetLower + x_2 * 2u * 4u;

			{
				// upper left pixel

				const int targetUpperLeft0 = minmax<int>(0, (int(source0PixelUpperLeft[0] - factors[13]) * factors[4] + int(source1Pixel[0] - factors[14]) * factors[7] + int(source2Pixel[0] - factors[15]) * factors[10]) / 64, 255);
				const int targetUpperLeft1 = minmax<int>(0, (int(source0PixelUpperLeft[0] - factors[13]) * factors[5] + int(source1Pixel[0] - factors[14]) * factors[8] + int(source2Pixel[0] - factors[15]) * factors[11]) / 64, 255);
				const int targetUpperLeft2 = minmax<int>(0, (int(source0PixelUpperLeft[0] - factors[13]) * factors[6] + int(source1Pixel[0] - factors[14]) * factors[9] + int(source2Pixel[0] - factors[15]) * factors[12]) / 64, 255);


				if (abs(targetUpperLeft0 - int(targetPixelUpperLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperLeft1 - int(targetPixelUpperLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperLeft2 - int(targetPixelUpperLeft[2u])) > 1)
				{
					allSucceeded = false;
				}

				if (targetPixelUpperLeft[3u] != alphaValue)
				{
					allSucceeded = false;
				}
			}

			{
				// upper right pixel

				const int targetUpperRight0 = minmax<int>(0, (int(source0PixelUpperLeft[1] - factors[13]) * factors[4] + int(source1Pixel[0] - factors[14]) * factors[7] + int(source2Pixel[0] - factors[15]) * factors[10]) / 64, 255);
				const int targetUpperRight1 = minmax<int>(0, (int(source0PixelUpperLeft[1] - factors[13]) * factors[5] + int(source1Pixel[0] - factors[14]) * factors[8] + int(source2Pixel[0] - factors[15]) * factors[11]) / 64, 255);
				const int targetUpperRight2 = minmax<int>(0, (int(source0PixelUpperLeft[1] - factors[13]) * factors[6] + int(source1Pixel[0] - factors[14]) * factors[9] + int(source2Pixel[0] - factors[15]) * factors[12]) / 64, 255);

				if (abs(targetUpperRight0 - int(targetPixelUpperLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperRight1 - int(targetPixelUpperLeft[5u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperRight2 - int(targetPixelUpperLeft[6u])) > 1)
				{
					allSucceeded = false;
				}

				if (targetPixelUpperLeft[7u] != alphaValue)
				{
					allSucceeded = false;
				}
			}

			{
				// lower left pixel

				const int targetLowerLeft0 = minmax<int>(0, (int(source0PixelLowerLeft[0] - factors[13]) * factors[4] + int(source1Pixel[0] - factors[14]) * factors[7] + int(source2Pixel[0] - factors[15]) * factors[10]) / 64, 255);
				const int targetLowerLeft1 = minmax<int>(0, (int(source0PixelLowerLeft[0] - factors[13]) * factors[5] + int(source1Pixel[0] - factors[14]) * factors[8] + int(source2Pixel[0] - factors[15]) * factors[11]) / 64, 255);
				const int targetLowerLeft2 = minmax<int>(0, (int(source0PixelLowerLeft[0] - factors[13]) * factors[6] + int(source1Pixel[0] - factors[14]) * factors[9] + int(source2Pixel[0] - factors[15]) * factors[12]) / 64, 255);

				if (abs(targetLowerLeft0 - int(targetPixelLowerLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerLeft1 - int(targetPixelLowerLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerLeft2 - int(targetPixelLowerLeft[2u])) > 1)
				{
					allSucceeded = false;
				}

				if (targetPixelLowerLeft[3u] != alphaValue)
				{
					allSucceeded = false;
				}
			}

			{
				// lower right pixel

				const int targetLowerRight0 = minmax<int>(0, (int(source0PixelLowerLeft[1] - factors[13]) * factors[4] + int(source1Pixel[0] - factors[14]) * factors[7] + int(source2Pixel[0] - factors[15]) * factors[10]) / 64, 255);
				const int targetLowerRight1 = minmax<int>(0, (int(source0PixelLowerLeft[1] - factors[13]) * factors[5] + int(source1Pixel[0] - factors[14]) * factors[8] + int(source2Pixel[0] - factors[15]) * factors[11]) / 64, 255);
				const int targetLowerRight2 = minmax<int>(0, (int(source0PixelLowerLeft[1] - factors[13]) * factors[6] + int(source1Pixel[0] - factors[14]) * factors[9] + int(source2Pixel[0] - factors[15]) * factors[12]) / 64, 255);

				if (abs(targetLowerRight0 - int(targetPixelLowerLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerRight1 - int(targetPixelLowerLeft[5u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerRight2 - int(targetPixelLowerLeft[6u])) > 1)
				{
					allSucceeded = false;
				}

				if (targetPixelLowerLeft[7u] != alphaValue)
				{
					allSucceeded = false;
				}
			}
		}

		// verify that the padding elements are untouched
		if (targetPaddingElements > 0u)
		{
			// upper row
			if (memcmp(targetUpperAndLowerPixels.data() + width * 4u, copyTargetUpperAndLowerPixels.data() + width * 4u, targetPaddingElements) != 0)
			{
				allSucceeded = false;
			}

			// lower row
			if (memcmp(targetUpperAndLowerPixels.data() + targetStrideElements + width * 4u, copyTargetUpperAndLowerPixels.data() + targetStrideElements + width * 4u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testConvertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 2-row converter of 3 channels, 3 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels (10 bit precision):";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source2PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = width / 2u + source1PaddingElements;
		const unsigned int source2StrideElements = width / 2u + source2PaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePlane0UpperAndLowerPixels(source0StrideElements * 2u);
		Elements sourcePlane1Pixels(source1StrideElements);
		Elements sourcePlane2Pixels(source2StrideElements);

		Elements targetUpperAndLowerPixels(targetStrideElements * 2u);

		for (uint8_t& element : sourcePlane0UpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane2Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetUpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetUpperAndLowerPixels(targetUpperAndLowerPixels);

		int factors[16];

		factors[0] = int(source0PaddingElements);
		factors[1] = int(source1PaddingElements);
		factors[2] = int(source2PaddingElements);
		factors[3] = int(targetPaddingElements);

		// multiplication factors

		for (unsigned int n = 4u; n < 10u; ++n)
		{
			factors[n] = RandomI::random(randomGenerator, -1024, 1024);
		}

		factors[10] = 1024 - factors[4] - factors[7];
		factors[11] = 1024 - factors[5] - factors[8];
		factors[12] = 1024 - factors[6] - factors[9];

		// bias values
		factors[13] = RandomI::random(randomGenerator, -127, 127);
		factors[14] = RandomI::random(randomGenerator, -127, 127);
		factors[15] = RandomI::random(randomGenerator, -127, 127);

		const void* sources[3] = {sourcePlane0UpperAndLowerPixels.data(), sourcePlane1Pixels.data(), sourcePlane2Pixels.data()};
		void* targets[1] = {targetUpperAndLowerPixels.data()};

		CV::FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, factors);

		// verify that the conversion is correct
		for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
			// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
			// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

			const uint8_t* const source0Upper = sourcePlane0UpperAndLowerPixels.data() + 0;
			const uint8_t* const source0Lower = sourcePlane0UpperAndLowerPixels.data() + source0StrideElements;
			const uint8_t* const source0PixelUpperLeft = source0Upper + x_2 * 2u;
			const uint8_t* const source0PixelLowerLeft = source0Lower + x_2 * 2u;

			const uint8_t* const source1 = sourcePlane1Pixels.data();
			const uint8_t* const source2 = sourcePlane2Pixels.data();
			const uint8_t* const source1Pixel = source1 + x_2;
			const uint8_t* const source2Pixel = source2 + x_2;

			const uint8_t* const targetUpper = targetUpperAndLowerPixels.data() + 0;
			const uint8_t* const targetLower = targetUpperAndLowerPixels.data() + targetStrideElements;

			const uint8_t* const targetPixelUpperLeft = targetUpper + x_2 * 2u * 3u;
			const uint8_t* const targetPixelLowerLeft = targetLower + x_2 * 2u * 3u;

			{
				// upper left pixel

				const int targetUpperLeft0 = minmax<int>(0, (int(source0PixelUpperLeft[0]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
				const int targetUpperLeft1 = minmax<int>(0, (int(source0PixelUpperLeft[0]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);
				const int targetUpperLeft2 = minmax<int>(0, (int(source0PixelUpperLeft[0]) * factors[6] + int(source1Pixel[0]) * factors[9] + int(source2Pixel[0]) * factors[12]) / 1024 + factors[15], 255);

				if (abs(targetUpperLeft0 - int(targetPixelUpperLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperLeft1 - int(targetPixelUpperLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperLeft2 - int(targetPixelUpperLeft[2u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// upper right pixel

				const int targetUpperRight0 = minmax<int>(0, (int(source0PixelUpperLeft[1]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
				const int targetUpperRight1 = minmax<int>(0, (int(source0PixelUpperLeft[1]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);
				const int targetUpperRight2 = minmax<int>(0, (int(source0PixelUpperLeft[1]) * factors[6] + int(source1Pixel[0]) * factors[9] + int(source2Pixel[0]) * factors[12]) / 1024 + factors[15], 255);

				if (abs(targetUpperRight0 - int(targetPixelUpperLeft[3u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperRight1 - int(targetPixelUpperLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetUpperRight2 - int(targetPixelUpperLeft[5u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// lower left pixel

				const int targetLowerLeft0 = minmax<int>(0, (int(source0PixelLowerLeft[0]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
				const int targetLowerLeft1 = minmax<int>(0, (int(source0PixelLowerLeft[0]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);
				const int targetLowerLeft2 = minmax<int>(0, (int(source0PixelLowerLeft[0]) * factors[6] + int(source1Pixel[0]) * factors[9] + int(source2Pixel[0]) * factors[12]) / 1024 + factors[15], 255);

				if (abs(targetLowerLeft0 - int(targetPixelLowerLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerLeft1 - int(targetPixelLowerLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerLeft2 - int(targetPixelLowerLeft[2u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// lower right pixel

				const int targetLowerRight0 = minmax<int>(0, (int(source0PixelLowerLeft[1]) * factors[4] + int(source1Pixel[0]) * factors[7] + int(source2Pixel[0]) * factors[10]) / 1024 + factors[13], 255);
				const int targetLowerRight1 = minmax<int>(0, (int(source0PixelLowerLeft[1]) * factors[5] + int(source1Pixel[0]) * factors[8] + int(source2Pixel[0]) * factors[11]) / 1024 + factors[14], 255);
				const int targetLowerRight2 = minmax<int>(0, (int(source0PixelLowerLeft[1]) * factors[6] + int(source1Pixel[0]) * factors[9] + int(source2Pixel[0]) * factors[12]) / 1024 + factors[15], 255);

				if (abs(targetLowerRight0 - int(targetPixelLowerLeft[3u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerRight1 - int(targetPixelLowerLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(targetLowerRight2 - int(targetPixelLowerLeft[5u])) > 1)
				{
					allSucceeded = false;
				}
			}
		}

		// verify that the padding elements are untouched
		if (targetPaddingElements > 0u)
		{
			// upper row
			if (memcmp(targetUpperAndLowerPixels.data() + width * 3u, copyTargetUpperAndLowerPixels.data() + width * 3u, targetPaddingElements) != 0)
			{
				allSucceeded = false;
			}

			// lower row
			if (memcmp(targetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, copyTargetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testMapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 1-row mapping of 3 channels, 3 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels:";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source2PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = width / 2u + source1PaddingElements;
		const unsigned int source2StrideElements = width / 2u + source2PaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePlane0UpperAndLowerPixels(source0StrideElements * 2u);
		Elements sourcePlane1Pixels(source1StrideElements);
		Elements sourcePlane2Pixels(source2StrideElements);

		Elements targetUpperAndLowerPixels(targetStrideElements * 2u);

		for (uint8_t& element : sourcePlane0UpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane2Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetUpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetUpperAndLowerPixels(targetUpperAndLowerPixels);

		const unsigned int options[4] = {source0PaddingElements, source1PaddingElements, source2PaddingElements, targetPaddingElements};

		const void* sources[3] = {sourcePlane0UpperAndLowerPixels.data(), sourcePlane1Pixels.data(), sourcePlane2Pixels.data()};
		void* targets[1] = {targetUpperAndLowerPixels.data()};

		unsigned int sourceChannelIndex0 = (unsigned int)(-1);
		unsigned int sourceChannelIndex1 = (unsigned int)(-1);
		unsigned int sourceChannelIndex2 = (unsigned int)(-1);
		RandomI::random(randomGenerator, 2u, sourceChannelIndex0, sourceChannelIndex1, sourceChannelIndex2);

		for (unsigned int row = 0u; row < 2u; ++row)
		{
			switch ((sourceChannelIndex0 << 0u) | (sourceChannelIndex1 << 4u) | (sourceChannelIndex2 << 8u))
			{
				case 0x210u:
					CV::FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
					break;

				case 0x120u:
					CV::FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
					break;

				case 0x201u:
					CV::FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
					break;

				case 0x021u:
					CV::FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
					break;

				case 0x102u:
					CV::FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 0u, 1u>(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
					break;

				case 0x012u:
					CV::FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>(sources, targets, row, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
					break;

				default:
					ocean_assert(false && "Must never happen!");
					allSucceeded = false;
					break;
			}

			// verify that the conversion is correct
			for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
			{
				// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
				// with transformation:
				// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
				// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
				// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

				const uint8_t* const source0Upper = sourcePlane0UpperAndLowerPixels.data() + 0;
				const uint8_t* const source0Lower = sourcePlane0UpperAndLowerPixels.data() + source0StrideElements;
				const uint8_t* const source0PixelUpperLeft = source0Upper + x_2 * 2u;
				const uint8_t* const source0PixelLowerLeft = source0Lower + x_2 * 2u;

				const uint8_t* const source1 = sourcePlane1Pixels.data();
				const uint8_t* const source1Pixel = source1 + x_2;

				const uint8_t* const source2 = sourcePlane2Pixels.data();
				const uint8_t* const source2Pixel = source2 + x_2;

				if (row == 0u)
				{
					const uint8_t* const targetUpper = targetUpperAndLowerPixels.data() + 0;

					const uint8_t* const targetPixelUpperLeft = targetUpper + x_2 * 2u * 3u;

					{
						// upper left pixel

						const int sourceValues[3] = {source0PixelUpperLeft[0], source1Pixel[0], source2Pixel[0]};

						if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelUpperLeft[0u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelUpperLeft[1u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelUpperLeft[2u])) > 1)
						{
							allSucceeded = false;
						}
					}

					{
						// upper right pixel

						const int sourceValues[3] = {source0PixelUpperLeft[1], source1Pixel[0], source2Pixel[0]};

						if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelUpperLeft[3u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelUpperLeft[4u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelUpperLeft[5u])) > 1)
						{
							allSucceeded = false;
						}
					}
				}
				else
				{
					ocean_assert(row == 1u);

					const uint8_t* const targetLower = targetUpperAndLowerPixels.data() + targetStrideElements;
					const uint8_t* const targetPixelLowerLeft = targetLower + x_2 * 2u * 3u;

					{
						// lower left pixel

						const int sourceValues[3] = {source0PixelLowerLeft[0], source1Pixel[0], source2Pixel[0]};

						if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelLowerLeft[0u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelLowerLeft[1u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelLowerLeft[2u])) > 1)
						{
							allSucceeded = false;
						}
					}

					{
						// lower right pixel

						const int sourceValues[3] = {source0PixelLowerLeft[1], source1Pixel[0], source2Pixel[0]};

						if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelLowerLeft[3u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelLowerLeft[4u])) > 1)
						{
							allSucceeded = false;
						}

						if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelLowerLeft[5u])) > 1)
						{
							allSucceeded = false;
						}
					}
				}
			}

			// verify that the padding elements are untouched
			if (targetPaddingElements > 0u)
			{
				// upper row
				if (memcmp(targetUpperAndLowerPixels.data() + width * 3u, copyTargetUpperAndLowerPixels.data() + width * 3u, targetPaddingElements) != 0)
				{
					allSucceeded = false;
				}

				// lower row
				if (memcmp(targetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, copyTargetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testMapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 2-row mapping of 3 channels, 3 planes, with 2x2 downsampling of channel 2 and 3 to zipped 3 channels:";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int source2PaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = width / 2u + source1PaddingElements;
		const unsigned int source2StrideElements = width / 2u + source2PaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePlane0UpperAndLowerPixels(source0StrideElements * 2u);
		Elements sourcePlane1Pixels(source1StrideElements);
		Elements sourcePlane2Pixels(source2StrideElements);

		Elements targetUpperAndLowerPixels(targetStrideElements * 2u);

		for (uint8_t& element : sourcePlane0UpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane2Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetUpperAndLowerPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetUpperAndLowerPixels(targetUpperAndLowerPixels);

		const unsigned int options[4] = {source0PaddingElements, source1PaddingElements, source2PaddingElements, targetPaddingElements};

		const void* sources[3] = {sourcePlane0UpperAndLowerPixels.data(), sourcePlane1Pixels.data(), sourcePlane2Pixels.data()};
		void* targets[1] = {targetUpperAndLowerPixels.data()};

		unsigned int sourceChannelIndex0 = (unsigned int)(-1);
		unsigned int sourceChannelIndex1 = (unsigned int)(-1);
		unsigned int sourceChannelIndex2 = (unsigned int)(-1);
		RandomI::random(randomGenerator, 2u, sourceChannelIndex0, sourceChannelIndex1, sourceChannelIndex2);

		switch ((sourceChannelIndex0 << 0u) | (sourceChannelIndex1 << 4u) | (sourceChannelIndex2 << 8u))
		{
			case 0x210u:
				CV::FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
				break;

			case 0x120u:
				CV::FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
				break;

			case 0x201u:
				CV::FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
				break;

			case 0x021u:
				CV::FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
				break;

			case 0x102u:
				CV::FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 0u, 1u>(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
				break;

			case 0x012u:
				CV::FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>(sources, targets, 0u, width, 2u, CV::FrameConverter::CONVERT_NORMAL, options);
				break;

			default:
				ocean_assert(false && "Must never happen!");
				allSucceeded = false;
				break;
		}

		// verify that the conversion is correct
		for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
			// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
			// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

			const uint8_t* const source0Upper = sourcePlane0UpperAndLowerPixels.data() + 0;
			const uint8_t* const source0Lower = sourcePlane0UpperAndLowerPixels.data() + source0StrideElements;
			const uint8_t* const source0PixelUpperLeft = source0Upper + x_2 * 2u;
			const uint8_t* const source0PixelLowerLeft = source0Lower + x_2 * 2u;

			const uint8_t* const source1 = sourcePlane1Pixels.data();
			const uint8_t* const source1Pixel = source1 + x_2;

			const uint8_t* const source2 = sourcePlane2Pixels.data();
			const uint8_t* const source2Pixel = source2 + x_2;

			const uint8_t* const targetUpper = targetUpperAndLowerPixels.data() + 0;
			const uint8_t* const targetLower = targetUpperAndLowerPixels.data() + targetStrideElements;

			const uint8_t* const targetPixelUpperLeft = targetUpper + x_2 * 2u * 3u;
			const uint8_t* const targetPixelLowerLeft = targetLower + x_2 * 2u * 3u;

			{
				// upper left pixel

				const int sourceValues[3] = {source0PixelUpperLeft[0], source1Pixel[0], source2Pixel[0]};

				if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelUpperLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelUpperLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelUpperLeft[2u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// upper right pixel

				const int sourceValues[3] = {source0PixelUpperLeft[1], source1Pixel[0], source2Pixel[0]};

				if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelUpperLeft[3u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelUpperLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelUpperLeft[5u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// lower left pixel

				const int sourceValues[3] = {source0PixelLowerLeft[0], source1Pixel[0], source2Pixel[0]};

				if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelLowerLeft[0u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelLowerLeft[1u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelLowerLeft[2u])) > 1)
				{
					allSucceeded = false;
				}
			}

			{
				// lower right pixel

				const int sourceValues[3] = {source0PixelLowerLeft[1], source1Pixel[0], source2Pixel[0]};

				if (abs(sourceValues[sourceChannelIndex0] - int(targetPixelLowerLeft[3u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex1] - int(targetPixelLowerLeft[4u])) > 1)
				{
					allSucceeded = false;
				}

				if (abs(sourceValues[sourceChannelIndex2] - int(targetPixelLowerLeft[5u])) > 1)
				{
					allSucceeded = false;
				}
			}
		}

		// verify that the padding elements are untouched
		if (targetPaddingElements > 0u)
		{
			// upper row
			if (memcmp(targetUpperAndLowerPixels.data() + width * 3u, copyTargetUpperAndLowerPixels.data() + width * 3u, targetPaddingElements) != 0)
			{
				allSucceeded = false;
			}

			// lower row
			if (memcmp(targetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, copyTargetUpperAndLowerPixels.data() + targetStrideElements + width * 3u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testConvertOneRow_3Planes1Channel_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing 1-row converter of 3 planes to zipped 3 channels (6 bit precision):";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u);
		const unsigned int source0PaddingElements = RandomI::random(randomGenerator, 0u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int source1PaddingElements = RandomI::random(randomGenerator, 0u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int source2PaddingElements = RandomI::random(randomGenerator, 0u, 100u) * RandomI::random(randomGenerator, 1u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u) * RandomI::random(randomGenerator, 1u);

		const unsigned int source0StrideElements = width + source0PaddingElements;
		const unsigned int source1StrideElements = width + source1PaddingElements;
		const unsigned int source2StrideElements = width + source2PaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePlane0Pixels(source0StrideElements);
		Elements sourcePlane1Pixels(source1StrideElements);
		Elements sourcePlane2Pixels(source2StrideElements);

		Elements targetPixels(targetStrideElements);

		for (uint8_t& element : sourcePlane0Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane1Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : sourcePlane2Pixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetPixels(targetPixels);

		int factors[16];

		// padding parameters

		factors[0] = int(source0PaddingElements);
		factors[1] = int(source1PaddingElements);
		factors[2] = int(source2PaddingElements);
		factors[3] = int(targetPaddingElements);

		// bias value / translation value

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			factors[13u + n] = RandomI::random(randomGenerator, 0, 128);
		}

		for (unsigned int n = 0u; n < 3u; ++n)
		{
			const int factor0 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);
			const int factor1 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);
			const int factor2 = RandomI::random(randomGenerator, -64 * 2, 64 * 2);

			if (std::abs((255 - factors[13]) * factor0) < 10922 // 10922 = (2^15 - 1) / 3
				&& std::abs((255 - factors[14]) * factor1) < 10922
				&& std::abs((255 - factors[15]) * factor2) < 10922)
			{
				factors[n +  4u] = factor0;
				factors[n +  7u] = factor1;
				factors[n + 10u] = factor2;
			}
			else
			{
				--n;
			}
		}

		const void* sources[3] = {sourcePlane0Pixels.data(), sourcePlane1Pixels.data(), sourcePlane2Pixels.data()};
		void* targets[1] = {targetPixels.data()};

		CV::FrameConverter::convertOneRow_3Planes1Channel_To_1Plane3Channels_8BitPerChannel_Precision6Bit(sources, targets, 0u, width, 1u, CV::FrameConverter::CONVERT_NORMAL, factors);

		// verify that the conversion is correct
		for (unsigned int x = 0u; x < width; ++x)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
			// t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
			// t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)

			const uint8_t sourcePixel0 = sourcePlane0Pixels.data()[x];
			const uint8_t sourcePixel1 = sourcePlane1Pixels.data()[x];
			const uint8_t sourcePixel2 = sourcePlane2Pixels.data()[x];

			const uint8_t* const targetPixel = targetPixels.data() + x * 3u;

			const int target0 = minmax<int>(0, (int(sourcePixel0 - factors[13]) * factors[4] + int(sourcePixel1 - factors[14]) * factors[7] + int(sourcePixel2 - factors[15]) * factors[10]) / 64, 255);
			const int target1 = minmax<int>(0, (int(sourcePixel0 - factors[13]) * factors[5] + int(sourcePixel1 - factors[14]) * factors[8] + int(sourcePixel2 - factors[15]) * factors[11]) / 64, 255);
			const int target2 = minmax<int>(0, (int(sourcePixel0 - factors[13]) * factors[6] + int(sourcePixel1 - factors[14]) * factors[9] + int(sourcePixel2 - factors[15]) * factors[12]) / 64, 255);

			if (abs(target0 - int(targetPixel[0u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(target1 - int(targetPixel[1u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(target2 - int(targetPixel[2u])) > 1)
			{
				allSucceeded = false;
			}

			// verify that the padding elements are untouched
			if (targetPaddingElements > 0u)
			{
				if (memcmp(targetPixels.data() + width * 3u, targetPixels.data() + width * 3u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testConvertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing convert zipped 3 channels (with 2x1 down-sampled 2 channels) to zipped 3 channels, for e.g., YUYV16 (10 bit precision):";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int sourceStrideElements = 2u * width + sourcePaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePixels(sourceStrideElements);
		Elements targetPixels(targetStrideElements);

		for (uint8_t& element : sourcePixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetPixels(targetPixels);

		int factors[14];

		factors[0] = int(sourcePaddingElements);
		factors[1] = int(targetPaddingElements);

		// multiplication factors

		for (unsigned int n = 2u; n < 8u; ++n)
		{
			factors[n] = RandomI::random(randomGenerator, -1024, 1024);
		}

		factors[8] = 1024 - factors[2] - factors[5];
		factors[9] = 1024 - factors[3] - factors[6];
		factors[10] = 1024 - factors[4] - factors[7];

		// bias values
		factors[11] = RandomI::random(randomGenerator, -127, 127);
		factors[12] = RandomI::random(randomGenerator, -127, 127);
		factors[13] = RandomI::random(randomGenerator, -127, 127);

		const uint8_t* source = sourcePixels.data();
		uint8_t* target = targetPixels.data();

		CV::FrameConverter::convertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit((const void**)(&source), (void**)(&target), 0u, width, 1u, CV::FrameConverter::CONVERT_NORMAL, factors);

		// verify that the conversion is correct
		for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
			// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
			// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

			const uint8_t ySource0 = sourcePixels[x_2 * 4u + 0u];
			const uint8_t uSource = sourcePixels[x_2 * 4u + 1u];
			const uint8_t ySource1 = sourcePixels[x_2 * 4u + 2u];
			const uint8_t vSource = sourcePixels[x_2 * 4u + 3u];

			const int rTarget0 = minmax<int>(0, (int(ySource0) * factors[2] + int(uSource) * factors[5] + int(vSource) * factors[8]) / 1024 + factors[11], 255);
			const int gTarget0 = minmax<int>(0, (int(ySource0) * factors[3] + int(uSource) * factors[6] + int(vSource) * factors[9]) / 1024 + factors[12], 255);
			const int bTarget0 = minmax<int>(0, (int(ySource0) * factors[4] + int(uSource) * factors[7] + int(vSource) * factors[10]) / 1024 + factors[13], 255);

			if (abs(rTarget0 - int(targetPixels[x_2 * 6u + 0u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(gTarget0 - int(targetPixels[x_2 * 6u + 1u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(bTarget0 - int(targetPixels[x_2 * 6u + 2u])) > 1)
			{
				allSucceeded = false;
			}

			const int rTarget1 = minmax<int>(0, (int(ySource1) * factors[2] + int(uSource) * factors[5] + int(vSource) * factors[8]) / 1024 + factors[11], 255);
			const int gTarget1 = minmax<int>(0, (int(ySource1) * factors[3] + int(uSource) * factors[6] + int(vSource) * factors[9]) / 1024 + factors[12], 255);
			const int bTarget1 = minmax<int>(0, (int(ySource1) * factors[4] + int(uSource) * factors[7] + int(vSource) * factors[10]) / 1024 + factors[13], 255);

			if (abs(rTarget1 - int(targetPixels[x_2 * 6u + 3u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(gTarget1 - int(targetPixels[x_2 * 6u + 4u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(bTarget1 - int(targetPixels[x_2 * 6u + 5u])) > 1)
			{
				allSucceeded = false;
			}
		}

		// verify that the padding elements are untouched
		if (targetPaddingElements > 0u)
		{
			if (memcmp(targetPixels.data() + width * 3u, copyTargetPixels.data() + width * 3u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testConvertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing convert zipped 3 channels (with 2x1 down-sampled 2 channels) to zipped 3 channels for e.g., UYVY16  (10 bit precision):";

	bool allSucceeded = true;

	typedef std::vector<uint8_t> Elements;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(1u, 1920u) * 2u;
		const unsigned int sourcePaddingElements = RandomI::random(randomGenerator, 0u, 100u);
		const unsigned int targetPaddingElements = RandomI::random(randomGenerator, 0u, 100u);

		const unsigned int sourceStrideElements = 2u * width + sourcePaddingElements;
		const unsigned int targetStrideElements = 3u * width + targetPaddingElements;

		Elements sourcePixels(sourceStrideElements);
		Elements targetPixels(targetStrideElements);

		for (uint8_t& element : sourcePixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		for (uint8_t& element : targetPixels)
		{
			element = uint8_t(RandomI::random(randomGenerator, 0u, 255u));
		}

		const Elements copyTargetPixels(targetPixels);

		int factors[14];

		factors[0] = int(sourcePaddingElements);
		factors[1] = int(targetPaddingElements);

		// multiplication factors

		for (unsigned int n = 2u; n < 8u; ++n)
		{
			factors[n] = RandomI::random(randomGenerator, -1024, 1024);
		}

		factors[8] = 1024 - factors[2] - factors[5];
		factors[9] = 1024 - factors[3] - factors[6];
		factors[10] = 1024 - factors[4] - factors[7];

		// bias values
		factors[11] = RandomI::random(randomGenerator, -127, 127);
		factors[12] = RandomI::random(randomGenerator, -127, 127);
		factors[13] = RandomI::random(randomGenerator, -127, 127);

		const uint8_t* source = sourcePixels.data();
		uint8_t* target = targetPixels.data();

		CV::FrameConverter::convertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit((const void**)(&source), (void**)(&target), 0u, width, 1u, CV::FrameConverter::CONVERT_NORMAL, factors);

		// verify that the conversion is correct
		for (unsigned int x_2 = 0u; x_2 < width / 2u; ++x_2)
		{
			// layout of parameters f00, f10, f20, f01, f02, ..., f22, b0, b1, b2
			// with transformation:
			// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
			// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
			// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

			const uint8_t uSource = sourcePixels[x_2 * 4u + 0u];
			const uint8_t ySource0 = sourcePixels[x_2 * 4u + 1u];
			const uint8_t vSource = sourcePixels[x_2 * 4u + 2u];
			const uint8_t ySource1 = sourcePixels[x_2 * 4u + 3u];

			const int rTarget0 = minmax<int>(0, (int(uSource) * factors[2] + int(ySource0) * factors[5] + int(vSource) * factors[8]) / 1024 + factors[11], 255);
			const int gTarget0 = minmax<int>(0, (int(uSource) * factors[3] + int(ySource0) * factors[6] + int(vSource) * factors[9]) / 1024 + factors[12], 255);
			const int bTarget0 = minmax<int>(0, (int(uSource) * factors[4] + int(ySource0) * factors[7] + int(vSource) * factors[10]) / 1024 + factors[13], 255);

			if (abs(rTarget0 - int(targetPixels[x_2 * 6u + 0u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(gTarget0 - int(targetPixels[x_2 * 6u + 1u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(bTarget0 - int(targetPixels[x_2 * 6u + 2u])) > 1)
			{
				allSucceeded = false;
			}

			const int rTarget1 = minmax<int>(0, (int(uSource) * factors[2] + int(ySource1) * factors[5] + int(vSource) * factors[8]) / 1024 + factors[11], 255);
			const int gTarget1 = minmax<int>(0, (int(uSource) * factors[3] + int(ySource1) * factors[6] + int(vSource) * factors[9]) / 1024 + factors[12], 255);
			const int bTarget1 = minmax<int>(0, (int(uSource) * factors[4] + int(ySource1) * factors[7] + int(vSource) * factors[10]) / 1024 + factors[13], 255);

			if (abs(rTarget1 - int(targetPixels[x_2 * 6u + 3u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(gTarget1 - int(targetPixels[x_2 * 6u + 4u])) > 1)
			{
				allSucceeded = false;
			}

			if (abs(bTarget1 - int(targetPixels[x_2 * 6u + 5u])) > 1)
			{
				allSucceeded = false;
			}
		}

		// verify that the padding elements are untouched
		if (targetPaddingElements > 0u)
		{
			if (memcmp(targetPixels.data() + width * 3u, copyTargetPixels.data() + width * 3u, targetPaddingElements) != 0)
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

bool TestFrameConverter::testFrameConversion(const FrameType::PixelFormat& sourcePixelFormat, const FrameType::PixelFormat& targetPixelFormat, const unsigned int width, const unsigned int height, const FunctionWrapper& functionWrapper, const CV::FrameConverter::ConversionFlag conversionFlag, const FunctionPixelValue functionSourcePixelValue, const FunctionPixelValue functionTargetPixelValue, const MatrixD& transformationMatrix, const double minimalGroundTruthValue, const double maximalGroundTruthValue, const double testDuration, Worker& worker, const unsigned int thresholdMaximalErrorToInteger, const void* options)
{
	ocean_assert(functionSourcePixelValue != nullptr && functionTargetPixelValue != nullptr);
	ocean_assert(testDuration > 0.0);
	ocean_assert(thresholdMaximalErrorToInteger < 20u);

	bool allSucceeded = true;

	Log::info() << "... " << translateConversionFlag(conversionFlag) << ":";

	const unsigned int widthMultiple = std::max(FrameType::widthMultiple(sourcePixelFormat), FrameType::widthMultiple(targetPixelFormat));
	const unsigned int heightMultiple = std::max(FrameType::heightMultiple(sourcePixelFormat), FrameType::heightMultiple(targetPixelFormat));

	if (widthMultiple % FrameType::widthMultiple(sourcePixelFormat) != 0u || widthMultiple % FrameType::widthMultiple(targetPixelFormat) != 0u
			|| heightMultiple % FrameType::heightMultiple(sourcePixelFormat) != 0u || heightMultiple % FrameType::heightMultiple(targetPixelFormat) != 0u)
	{
		ocean_assert(false && "We need to determine the least common multiple!");
		return false;
	}

	RandomGenerator randomGenerator;

	double averageErrorToFloat = 0.0;
	double averageErrorToInteger = 0.0;
	double maximalErrorToFloat = 0.0;
	unsigned int maximalErrorToInteger = 0u;

	unsigned int measurements = 0u;

	HighPerformanceStatistic performanceSinglecore;
	HighPerformanceStatistic performanceMulticore;

	const unsigned int maxWorkerIterations = worker ? 2u : 1u;

	for (unsigned int workerIteration = 0u; workerIteration < maxWorkerIterations; ++workerIteration)
	{
		Worker* useWorker = (workerIteration == 0u) ? nullptr : &worker;
		HighPerformanceStatistic& performance = useWorker ? performanceMulticore : performanceSinglecore;

		Timestamp startTimestamp(true);

		do
		{
			for (const bool& benchmarkIteration : {true, false})
			{

#ifdef OCEAN_USE_GTEST
				const unsigned int testWidth = benchmarkIteration ? width : RandomI::random(randomGenerator, 1u, 1001u) * widthMultiple;
				const unsigned int testHeight = benchmarkIteration ? height : RandomI::random(randomGenerator, 1u, 41u) * heightMultiple;
#else
				const unsigned int testWidth = benchmarkIteration ? width : RandomI::random(randomGenerator, 1u, 1000u) * widthMultiple;
				const unsigned int testHeight = benchmarkIteration ? height : RandomI::random(randomGenerator, 1u, 1000u) * heightMultiple;
#endif

				const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(testWidth, testHeight, sourcePixelFormat, FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
				Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, targetPixelFormat), &randomGenerator);

				const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

				performance.startIf(benchmarkIteration);

				if (!functionWrapper.invoke(sourceFrame, targetFrame, conversionFlag, options, useWorker))
				{
					allSucceeded = false;
				}

				performance.stopIf(benchmarkIteration);

				if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
				{
					ocean_assert(false && "Invalid padding memory!");
					return false;
				}

				double localAverageErrorToFloat;
				double localAverageErrorToInteger;
				double localMaximalErrorToFloat;
				unsigned int localMaximalErrorToInteger;
				if (!validateConversion(sourceFrame, targetFrame, functionSourcePixelValue, functionTargetPixelValue, transformationMatrix, conversionFlag, &localAverageErrorToFloat, &localAverageErrorToInteger, &localMaximalErrorToFloat, &localMaximalErrorToInteger, minimalGroundTruthValue, maximalGroundTruthValue))
				{
					allSucceeded = false;
				}

				averageErrorToFloat += localAverageErrorToFloat;
				averageErrorToInteger += localAverageErrorToInteger;
				maximalErrorToFloat = std::max(maximalErrorToFloat, localMaximalErrorToFloat);
				maximalErrorToInteger = std::max(maximalErrorToInteger, localMaximalErrorToInteger);

				++measurements;
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));
	}

	Log::info() << "Single-core performance: Best: " << String::toAString(performanceSinglecore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceSinglecore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceSinglecore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceSinglecore.medianMseconds(), 3u) << "ms";

	if (performanceMulticore.measurements() != 0u)
	{
		Log::info() << "Multi-core performance: Best: " << String::toAString(performanceMulticore.bestMseconds(), 3u) << "ms, worst: " << String::toAString(performanceMulticore.worstMseconds(), 3u) << "ms, average: " << String::toAString(performanceMulticore.averageMseconds(), 3u) << "ms, median: " << String::toAString(performanceMulticore.medianMseconds(), 3u) << "ms";
		Log::info() << "Multi-core boost factor: Best: " << String::toAString(performanceSinglecore.best() / performanceMulticore.best(), 1u) << "x, worst: " << String::toAString(performanceSinglecore.worst() / performanceMulticore.worst(), 1u) << "x, average: " << String::toAString(performanceSinglecore.average() / performanceMulticore.average(), 1u) << "x, average: " << String::toAString(performanceSinglecore.median() / performanceMulticore.median(), 1u) << "x";
	}

	ocean_assert(measurements != 0u);

	OCEAN_SUPPRESS_UNUSED_WARNING(averageErrorToFloat);

	averageErrorToFloat /= double(measurements);
	averageErrorToInteger /= double(measurements);

	if (maximalErrorToInteger > thresholdMaximalErrorToInteger)
	{
		Log::info() << "Validation FAILED: max error: " << maximalErrorToInteger << ", average error: " << String::toAString(averageErrorToInteger, 2u);
		allSucceeded = false;
	}
	else
	{
		Log::info() << "Validation succeeded: max error: " << maximalErrorToInteger << ", average error: " << String::toAString(averageErrorToInteger, 2u);
	}

	return allSucceeded;
}

bool TestFrameConverter::validateConversion(const Frame& sourceFrame, const Frame& targetFrame, const FunctionPixelValue functionSourcePixelValue, const FunctionPixelValue functionTargetPixelValue, const MatrixD& transformationMatrix, const CV::FrameConverter::ConversionFlag conversionFlag, double* averageAbsErrorToFloat, double* averageAbsErrorToInteger, double* maximalAbsErrorToFloat, unsigned int* maximalAbsErrorToInteger, const double minimalGroundTruthValue, const double maximalGroundTruthValue, const bool skipPlausibilityCheck)
{
	ocean_assert(sourceFrame.isValid() && targetFrame.isValid());
	ocean_assert(functionSourcePixelValue != nullptr && functionTargetPixelValue != nullptr);

	double localAverageErrorToFloat = 0.0;
	double localAverageErrorToInteger = 0.0;
	double localMaximalAbsErrorToFloat = 0.0;
	unsigned int localMaximalErrorToInteger = 0u;

	if (sourceFrame.width() != targetFrame.width() || sourceFrame.height() != targetFrame.height())
	{
		ocean_assert(false && "Invalid parameters!");
		return false;
	}

	if (sourceFrame.channels() != (unsigned int)(transformationMatrix.columns()) && sourceFrame.channels() + 1u != (unsigned int)(transformationMatrix.columns()))
	{
		ocean_assert(false && "Invalid parameters!");
		return false;
	}

	if (targetFrame.channels() != (unsigned int)(transformationMatrix.rows()))
	{
		ocean_assert(false && "Invalid parameters!");
		return false;
	}

	unsigned long long measurements = 0ull;

	for (unsigned int y = 0; y < sourceFrame.height(); ++y)
	{
		for (unsigned int x = 0; x < sourceFrame.width(); ++x)
		{
			const MatrixD targetVector = functionTargetPixelValue(targetFrame, x, y, conversionFlag);
			ocean_assert(targetVector.rows() >= 1 && targetVector.columns() == 1);

			MatrixD sourceVector = functionSourcePixelValue(sourceFrame, x, y, CV::FrameConverter::CONVERT_NORMAL);

			if (!targetVector || !sourceVector)
			{
				ocean_assert(false && "Invalid color vector/matrix!");
				return false;
			}

			if (sourceVector.columns() == 1)
			{
				if (sourceVector.rows() + 1 == transformationMatrix.columns())
				{
					// making the vector a homogeneous vector

					sourceVector = MatrixD(transformationMatrix.columns(), 1, sourceVector, 0, 0, 1.0);
				}
			}
			else
			{
				ocean_assert(sourceVector.columns() >= 2);

				if (sourceVector.columns() != targetFrame.channels())
				{
					ocean_assert(false && "Invalid matrix!");
					return false;
				}

				if (sourceVector.rows() + 1 == transformationMatrix.columns())
				{
					// making the matrix a homogeneous matrix

					sourceVector = MatrixD(transformationMatrix.columns(), sourceVector.columns(), sourceVector, 0, 0, 1.0);
				}
			}

			const MatrixD groundTruthTargetVector = transformationMatrix * sourceVector;

			for (unsigned int nChannel = 0u; nChannel < targetFrame.channels(); ++nChannel)
			{
				double groundTruthValueFloat;

				if (groundTruthTargetVector.columns() == 1)
				{
					groundTruthValueFloat = groundTruthTargetVector(nChannel, 0);
				}
				else
				{
					// in case the source vector was actually a source matrix, each column of that matrix is holding a different channel

					ocean_assert(nChannel < groundTruthTargetVector.columns());
					groundTruthValueFloat = groundTruthTargetVector(nChannel, nChannel);
				}

				if (minimalGroundTruthValue < maximalGroundTruthValue)
				{
					groundTruthValueFloat = minmax(minimalGroundTruthValue, groundTruthValueFloat, maximalGroundTruthValue);
				}
				else
				{
					ocean_assert(groundTruthValueFloat >= minimalGroundTruthValue && groundTruthValueFloat < maximalGroundTruthValue + 0.5);
				}

				const double valueFloat =  targetVector(nChannel, 0);

				const double absError = NumericD::abs(groundTruthValueFloat - valueFloat);

				ocean_assert_and_suppress_unused(skipPlausibilityCheck || absError <= 10.0, skipPlausibilityCheck);

				localAverageErrorToFloat += absError;

				localAverageErrorToInteger += std::abs(int(groundTruthValueFloat) - int(valueFloat));
				localMaximalAbsErrorToFloat = max(localMaximalAbsErrorToFloat, NumericD::abs(groundTruthValueFloat - valueFloat));
				localMaximalErrorToInteger = max(localMaximalErrorToInteger, (unsigned int)(std::abs(int(groundTruthValueFloat) - int(valueFloat))));

				measurements++;
			}
		}
	}

	ocean_assert(measurements != 0ull);

	if (averageAbsErrorToFloat)
	{
		*averageAbsErrorToFloat = localAverageErrorToFloat / double(measurements);
	}

	if (averageAbsErrorToInteger)
	{
		*averageAbsErrorToInteger = localAverageErrorToInteger / double(measurements);
	}

	if (maximalAbsErrorToFloat)
	{
		*maximalAbsErrorToFloat = localMaximalAbsErrorToFloat;
	}

	if (maximalAbsErrorToInteger)
	{
		*maximalAbsErrorToInteger = localMaximalErrorToInteger;
	}

	return true;
}

bool TestFrameConverter::testConversionMatrices(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing color space conversion matrices:";
	Log::info() << " ";

	const double testDurationPerMatrix = testDuration / 1.0;

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	{
		Log::info() << "Testing RGB24 (full range) to YUV24 (full range):";

		const MatrixD rgb2yuv = transformationMatrix_FullRangeRGB24_To_FullRangeYUV24_BT601();
		const MatrixD yuv2rgb = transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_BT601();

		double yMin = NumericD::maxValue();
		double yMax = NumericD::minValue();
		double uMin = NumericD::maxValue();
		double uMax = NumericD::minValue();
		double vMin = NumericD::maxValue();
		double vMax = NumericD::minValue();

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int red = RandomI::random(randomGenerator, 0u, 255u);
			const unsigned int green = RandomI::random(randomGenerator, 0u, 255u);
			const unsigned int blue = RandomI::random(randomGenerator, 0u, 255u);

			const double inputVector[4] = {double(red), double(green), double(blue), 1.0};

			const MatrixD outputVector = rgb2yuv * MatrixD(4, 1, inputVector);
			ocean_assert(outputVector.rows() == 3);

			const double y = outputVector(0, 0);
			const double u = outputVector(1, 0);
			const double v = outputVector(2, 0);

			yMin = std::min(yMin, y);
			yMax = std::max(yMax, y);

			uMin = std::min(uMin, u);
			uMax = std::max(uMax, u);

			vMin = std::min(vMin, v);
			vMax = std::max(vMax, v);


			// backward conversion

			const double backwardInputVector[4] = {double(y), double(u), double(v), 1.0};

			const MatrixD backwardOutputVector = yuv2rgb * MatrixD(4, 1, backwardInputVector);

			for (unsigned int n = 0u; n < 3u; ++n)
			{
				if (NumericD::isNotEqual(backwardOutputVector(n, 0), inputVector[n], 1.0))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDuration > Timestamp(true));

		Log::info() << "Y range: [" << yMin << ", " << yMax << "], expected to be [0, 255]";
		Log::info() << "U range: [" << uMin << ", " << uMax << "], expected to be [0, 255]";
		Log::info() << "V range: [" << vMin << ", " << vMax << "], expected to be [0, 255]";

		if (yMin < 0.0 || yMax >= 256)
		{
			allSucceeded = false;
		}

		if (uMin < 0.0 || uMax >= 256)
		{
			allSucceeded = false;
		}

		if (vMin < 0.0 || vMax >= 256)
		{
			allSucceeded = false;
		}
	}

	Log::info() << " ";
	Log::info() << " ";

	{
		Log::info() << "Testing RGB24 (full range) to YUV24 (limited range = YCbCr):";

		const MatrixD rgb2yuv = transformationMatrix_FullRangeRGB24_To_LimitedRangeYUV24_BT601();
		const MatrixD yuv2rgb = transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601();

		double yMin = NumericD::maxValue();
		double yMax = NumericD::minValue();
		double uMin = NumericD::maxValue();
		double uMax = NumericD::minValue();
		double vMin = NumericD::maxValue();
		double vMax = NumericD::minValue();

		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int red = RandomI::random(randomGenerator, 0u, 255u);
			const unsigned int green = RandomI::random(randomGenerator, 0u, 255u);
			const unsigned int blue = RandomI::random(randomGenerator, 0u, 255u);

			const double inputVector[4] = {double(red), double(green), double(blue), 1.0};

			const MatrixD outputVector = rgb2yuv * MatrixD(4, 1, inputVector);
			ocean_assert(outputVector.rows() == 3);

			const double y = outputVector(0, 0);
			const double u = outputVector(1, 0);
			const double v = outputVector(2, 0);

			yMin = std::min(yMin, y);
			yMax = std::max(yMax, y);

			uMin = std::min(uMin, u);
			uMax = std::max(uMax, u);

			vMin = std::min(vMin, v);
			vMax = std::max(vMax, v);


			// backward conversion

			const double backwardInputVector[4] = {double(y), double(u), double(v), 1.0};

			const MatrixD backwardOutputVector = yuv2rgb * MatrixD(4, 1, backwardInputVector);

			for (unsigned int n = 0u; n < 3u; ++n)
			{
				if (NumericD::isNotEqual(backwardOutputVector(n, 0), inputVector[n], 1.0))
				{
					allSucceeded = false;
				}
			}
		}
		while (startTimestamp + testDurationPerMatrix > Timestamp(true));

		Log::info() << "Y range: [" << yMin << ", " << yMax << "], expected to be [16, 235]";
		Log::info() << "U range: [" << uMin << ", " << uMax << "], expected to be [16, 240]";
		Log::info() << "V range: [" << vMin << ", " << vMax << "], expected to be [16, 240]";

		if (yMin < 16.0 || yMax >= 236)
		{
			allSucceeded = false;
		}

		if (uMin < 16.0 || uMax >= 241)
		{
			allSucceeded = false;
		}

		if (vMin < 16.0 || vMax >= 241)
		{
			allSucceeded = false;
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

MatrixD TestFrameConverter::functionGenericPixel(const Frame& frame, const unsigned int x, const unsigned int y, const CV::FrameConverter::ConversionFlag conversionFlag)
{
	ocean_assert(frame.isValid());
	ocean_assert(FrameType::formatIsGeneric(frame.pixelFormat()));

	ocean_assert(x < frame.width() && y < frame.height());

	unsigned int xAdjusted = x;
	unsigned int yAdjusted = y;

	switch (conversionFlag)
	{
		case CV::FrameConverter::CONVERT_NORMAL:
			break;

		case CV::FrameConverter::CONVERT_FLIPPED:
			yAdjusted = frame.height() - y - 1u;
			break;

		case CV::FrameConverter::CONVERT_MIRRORED:
			xAdjusted = frame.width() - x - 1u;
			break;

		case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
			xAdjusted = frame.width() - x - 1u;
			yAdjusted = frame.height() - y - 1u;
			break;

		default:
			ocean_assert(false && "Not supported conversion flag.");
	}

	const unsigned int channels = frame.channels();

	MatrixD pixelColors(channels, 1, 0.0);

	switch (frame.dataType())
	{
		case FrameType::DT_UNSIGNED_INTEGER_8:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<uint8_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_SIGNED_INTEGER_8:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<int8_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_UNSIGNED_INTEGER_16:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<uint16_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_SIGNED_INTEGER_16:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<int16_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_UNSIGNED_INTEGER_32:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<uint32_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_SIGNED_INTEGER_32:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<int32_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_UNSIGNED_INTEGER_64:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<uint64_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		case FrameType::DT_SIGNED_INTEGER_64:
		{
			for (unsigned int c = 0u; c < channels; ++c)
			{
				pixelColors(c, 0) = double(frame.constpixel<int64_t>(xAdjusted, yAdjusted)[c]);
			}

			break;
		}

		default:
			ocean_assert(false && "Invalid data type!");
			break;
	}

	return pixelColors;
}

template <typename T>
bool TestFrameConverter::testSubFrame(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int sourceWidth = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int sourceHeight = RandomI::random(randomGenerator, 1u, 1080u);

		const unsigned int targetWidth = RandomI::random(randomGenerator, 1u, sourceWidth);
		const unsigned int targetHeight = RandomI::random(randomGenerator, 1u, sourceHeight);

		const unsigned int subFrameWidth = RandomI::random(randomGenerator, 1u, min(sourceWidth, targetWidth));
		const unsigned int subFrameHeight = RandomI::random(randomGenerator, 1u, min(sourceHeight, targetHeight));

		const unsigned int sourceLeft = RandomI::random(randomGenerator, 0u, sourceWidth - subFrameWidth);
		const unsigned int sourceTop = RandomI::random(randomGenerator, 0u, sourceHeight - subFrameHeight);

		const unsigned int targetLeft = RandomI::random(randomGenerator, 0u, targetWidth - subFrameWidth);
		const unsigned int targetTop = RandomI::random(randomGenerator, 0u, targetHeight - subFrameHeight);

		const unsigned int channels = RandomI::random(randomGenerator, 1u, 5u);

		const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceWidth, sourceHeight, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);
		Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, targetWidth, targetHeight), &randomGenerator);

		const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

		if (!CV::FrameConverter::subFrame<T>(sourceFrame.constdata<T>(), targetFrame.data<T>(), sourceWidth, sourceHeight, targetWidth, targetHeight, channels, sourceLeft, sourceTop, targetLeft, targetTop, subFrameWidth, subFrameHeight, sourceFrame.paddingElements(), targetFrame.paddingElements()))
		{
			allSucceeded = false;
		}

		if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
		{
			ocean_assert(false && "Invalid padding memory!");
			return false;
		}

		if (!validateSubFrame<T>(channels, sourceFrame.constdata<T>(), sourceWidth, sourceHeight, targetFrame.constdata<T>(), targetWidth, targetHeight, sourceLeft, sourceTop, targetLeft, targetTop, subFrameWidth, subFrameHeight, sourceFrame.paddingElements(), targetFrame.paddingElements()))
		{
			allSucceeded = false;
		}
	}
	while (startTimestamp + testDuration > Timestamp(true));

	return allSucceeded;
}

template <typename T>
bool TestFrameConverter::testCast(const unsigned int width, const unsigned int height, const unsigned int channels)
{
	ocean_assert(width >= 1u && height >= 1u && channels >= 1u);

	const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, channels), FrameType::ORIGIN_UPPER_LEFT));

	Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(sourceFrame, FrameType::genericPixelFormat<T>(channels)));

	const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

	CV::FrameConverter::cast<uint8_t, T>(sourceFrame.constdata<uint8_t>(), targetFrame.data<T>(), width, height, channels, sourceFrame.paddingElements(), targetFrame.paddingElements());

	if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
	{
		ocean_assert(false && "Invalid padding memory!");
		return false;
	}

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const uint8_t* const sourcePixel = sourceFrame.constpixel<uint8_t>(x, y);
			const T* const targetPixel = targetFrame.constpixel<T>(x, y);

			for (unsigned int c = 0u; c < channels; ++c)
			{
				if (NumericT<T>::isNotEqual(T(sourcePixel[c]), targetPixel[c]))
				{
					return false;
				}
			}
		}
	}

	// now we check whether the back-casted result is identical with the input frame

	Frame backFrame = CV::CVUtilities::randomizedFrame(sourceFrame.frameType());

	const Frame copyBackFrame(backFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

	CV::FrameConverter::cast<T, uint8_t>(targetFrame.data<T>(), backFrame.data<uint8_t>(), width, height, channels, targetFrame.paddingElements(), backFrame.paddingElements());

	if (!CV::CVUtilities::isPaddingMemoryIdentical(backFrame, copyBackFrame))
	{
		ocean_assert(false && "Invalid padding memory!");
		return false;
	}

	for (unsigned int y = 0u; y < height; ++y)
	{
		if (memcmp(sourceFrame.constrow<void>(y), backFrame.constrow<void>(y), width * channels) != 0)
		{
			return false;
		}
	}

	return true;
}

template <typename T>
bool TestFrameConverter::testNormalizedCast(const unsigned int width, const unsigned int height, const unsigned int channels, const T normalization, const T offset)
{
	ocean_assert(width >= 1u && height >= 1u);

	const Frame sourceFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<uint8_t>(channels), FrameType::ORIGIN_UPPER_LEFT));

	Frame targetFrame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT));

	const Frame copyTargetFrame(targetFrame, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

	CV::FrameConverter::normalizedCast<uint8_t, T>(sourceFrame.constdata<uint8_t>(), targetFrame.data<T>(), width, height, channels, normalization, offset, sourceFrame.paddingElements(), targetFrame.paddingElements());

	if (!CV::CVUtilities::isPaddingMemoryIdentical(targetFrame, copyTargetFrame))
	{
		ocean_assert(false && "Invalid padding memory!");
		return false;
	}

	for (unsigned int y = 0u; y < height; ++y)
	{
		const uint8_t* sourceRow = sourceFrame.constrow<uint8_t>(y);

		const T* targetRow = targetFrame.constrow<T>(y);

		for (unsigned int x = 0u; x < width * channels; ++x)
		{
			if (NumericT<T>::isNotWeakEqual(T(sourceRow[x]) * normalization + offset, targetRow[x]))
			{
				return false;
			}
		}
	}

	return true;
}

template <typename T>
bool TestFrameConverter::testPatchFrame(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << TypeNamer::name<T>() << ":";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	for (unsigned int channels = 1u; channels <= 5u; ++channels)
	{
		const Timestamp startTimestamp(true);

		do
		{
			const unsigned int patchSize = RandomI::random(randomGenerator, 1u, 20u) | 0x1u; // size must be odd
			ocean_assert(patchSize % 2u == 1u);

			const unsigned int patchSize_2 = patchSize / 2u;

			const unsigned int width = RandomI::random(randomGenerator, patchSize, 1000u);
			const unsigned int height = RandomI::random(randomGenerator, patchSize, 1000u);

			const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<T>(channels), FrameType::ORIGIN_UPPER_LEFT), &randomGenerator);

			Frame patch = CV::CVUtilities::randomizedFrame(FrameType(frame, patchSize, patchSize), &randomGenerator);

			const Frame patchCopy(patch, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			const unsigned int x = RandomI::random(randomGenerator, patchSize_2, frame.width() - patchSize_2 - 1u);
			const unsigned int y = RandomI::random(randomGenerator, patchSize_2, frame.height() - patchSize_2 - 1u);

			CV::FrameConverter::patchFrame<T>(frame.constdata<T>(), patch.data<T>(), frame.width(), channels, x, y, patchSize, frame.paddingElements(), patch.paddingElements());

			if (!CV::CVUtilities::isPaddingMemoryIdentical(patch, patchCopy))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			for (unsigned int rowIndex = 0u; rowIndex < patchSize; ++rowIndex)
			{
				const T* framePixel = frame.constpixel<T>(x - patchSize_2, y - patchSize_2 + rowIndex);
				const T* patchPixel = patch.constpixel<T>(0u, rowIndex);

				if (memcmp(framePixel, patchPixel, patchSize * channels * sizeof(T)) != 0)
				{
					allSucceeded = false;
				}
			}
		}
		while (Timestamp(true) < startTimestamp + testDuration);
	}

	return allSucceeded;
}

template <typename T, unsigned int tChannels>
bool TestFrameConverter::testPatchFrameMirroredBorder(const double testDuration)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(testDuration > 0.0);

	Log::info() << "... for " << TypeNamer::name<T>() << ", with " << tChannels << " channels:";

	bool allSucceeded = true;

	RandomGenerator randomGenerator;

	const Timestamp startTimestamp(true);

	do
	{
		const unsigned int width = RandomI::random(randomGenerator, 1u, 1920u);
		const unsigned int height = RandomI::random(randomGenerator, 1u, 1920u);

		const Frame frame = CV::CVUtilities::randomizedFrame(FrameType(width, height, FrameType::genericPixelFormat<T, tChannels>(), Frame::ORIGIN_UPPER_LEFT), &randomGenerator);

		for (unsigned int iteration = 0u; iteration < 10u; ++iteration)
		{
			const unsigned int maxPatchSize = std::min(frame.width(), frame.height());

			const unsigned int patchSize = RandomI::random(randomGenerator, maxPatchSize / 2u) * 2u + 1u;

			const unsigned int x = RandomI::random(randomGenerator, frame.width() - 1u);
			const unsigned int y = RandomI::random(randomGenerator, 0u, frame.height() - 1u);

			Frame patch = CV::CVUtilities::randomizedFrame(FrameType(frame, patchSize, patchSize), &randomGenerator);

			const Frame copyPatch(patch, Frame::ACM_COPY_KEEP_LAYOUT_COPY_PADDING_DATA);

			CV::FrameConverter::patchFrameMirroredBorder<T, tChannels>(frame.constdata<T>(), patch.data<T>(), frame.width(), frame.height(), x, y, patchSize, frame.paddingElements(), patch.paddingElements());

			if (!CV::CVUtilities::isPaddingMemoryIdentical(patch, copyPatch))
			{
				ocean_assert(false && "Invalid padding memory!");
				return false;
			}

			const unsigned int patchSize_2 = patchSize / 2u;

			for (unsigned int py = 0u; py < patchSize; ++py)
			{
				const int yy = int(y) - int(patchSize_2) + int(py);

				for (unsigned int px = 0u; px < patchSize; ++px)
				{
					const int xx = int(x) - int(patchSize_2) + int(px);

					int vx, vy;

					if (xx < 0)
					{
						vx = -xx - 1;
					}
					else if (xx >= int(width))
					{
						vx = int(width) - (xx - int(width)) - 1;
					}
					else
					{
						vx = xx;
					}

					if (yy < 0)
					{
						vy = -yy - 1;
					}
					else if (yy >= int(height))
					{
						vy = int(height) - (yy - int(height)) - 1;
					}
					else
					{
						vy = yy;
					}

					const T* framePixel = frame.constpixel<T>((unsigned int)(vx), (unsigned int)(vy));
					const T* patchPixel = patch.constpixel<T>(px, py);

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						if (framePixel[n] != patchPixel[n])
						{
							allSucceeded = false;
						}
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

template <typename T>
bool TestFrameConverter::validateSubFrame(const unsigned int channels, const T* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const T* target, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);

	ocean_assert(sourceLeft + width <= sourceWidth);
	ocean_assert_and_suppress_unused(sourceTop + height <= sourceHeight, sourceHeight);

	ocean_assert(targetLeft + width <= targetWidth);
	ocean_assert_and_suppress_unused(targetTop + height <= targetHeight, targetHeight);

	const unsigned int sourceStrideElements = sourceWidth * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * channels + targetPaddingElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const unsigned int sx = x + sourceLeft;
			const unsigned int sy = y + sourceTop;

			const unsigned int tx = x + targetLeft;
			const unsigned int ty = y + targetTop;

			const T* const s = source + sy * sourceStrideElements + sx * channels;
			const T* const t = target + ty * targetStrideElements + tx * channels;

			for (unsigned int c = 0u; c < channels; ++c)
			{
				if (s[c] != t[c])
				{
					return false;
				}
			}
		}
	}

	return true;
}

template <typename T>
bool TestFrameConverter::validateSubFrameMask(const unsigned int channels, const T* source, const unsigned int sourceWidth, const unsigned int sourceHeight, const T* originalTarget, const T* target, const unsigned int targetWidth, const unsigned int targetHeight, const uint8_t* mask, const unsigned int sourceLeft, const unsigned int sourceTop, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int maskPaddingElements, const uint8_t maskValue)
{
	ocean_assert(source != nullptr && mask != nullptr && originalTarget != nullptr && target != nullptr);

	ocean_assert(sourceLeft + subFrameWidth <= sourceWidth);
	ocean_assert_and_suppress_unused(sourceTop + subFrameHeight <= sourceHeight, sourceHeight);

	ocean_assert(targetLeft + subFrameWidth <= targetWidth);
	ocean_assert_and_suppress_unused(targetTop + subFrameHeight <= targetHeight, targetHeight);

	const unsigned int sourceStrideElements = sourceWidth * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * channels + targetPaddingElements;

	const unsigned int maskStrideElements = subFrameWidth + maskPaddingElements;

	for (unsigned int y = 0u; y < subFrameHeight; ++y)
	{
		for (unsigned int x = 0u; x < subFrameWidth; ++x)
		{
			const unsigned int sx = x + sourceLeft;
			const unsigned int sy = y + sourceTop;

			const unsigned int tx = x + targetLeft;
			const unsigned int ty = y + targetTop;

			const uint8_t* maskPtr = mask + y * maskStrideElements + x;

			const T* const sourcePtr = source + sy * sourceStrideElements + sx * channels;
			const T* const targetPtr = target + ty * targetStrideElements + tx * channels;
			const T* const originalTargetPtr = originalTarget + ty * targetStrideElements + tx * channels;

			// The current target pixel should:
			//  1. be identical to the corresponding source pixel, if *mask == maskValue
			//  2. remain unchanged (so compare against the original target pixel), otherwise
			const T* const referencePtr = *maskPtr == maskValue ? sourcePtr : originalTargetPtr;

			for (unsigned int c = 0u; c < channels; ++c)
			{
				if (referencePtr[c] != targetPtr[c])
				{
					return false;
				}
			}
		}
	}

	return true;
}

} // namespace TestCV

} // namespace Test

} // namespace Ocean
