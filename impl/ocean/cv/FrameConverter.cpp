/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameChannels.h"
#include "ocean/cv/FrameConverterABGR32.h"
#include "ocean/cv/FrameConverterARGB32.h"
#include "ocean/cv/FrameConverterBGR24.h"
#include "ocean/cv/FrameConverterBGR32.h"
#include "ocean/cv/FrameConverterBGR565.h"
#include "ocean/cv/FrameConverterBGRA32.h"
#include "ocean/cv/FrameConverterRGB24.h"
#include "ocean/cv/FrameConverterRGB32.h"
#include "ocean/cv/FrameConverterRGB565.h"
#include "ocean/cv/FrameConverterRGBA32.h"
#include "ocean/cv/FrameConverterRGBA64.h"
#include "ocean/cv/FrameConverterRGGB10_Packed.h"
#include "ocean/cv/FrameConverterUYVY16.h"
#include "ocean/cv/FrameConverterY8.h"
#include "ocean/cv/FrameConverterY10_Packed.h"
#include "ocean/cv/FrameConverterY16.h"
#include "ocean/cv/FrameConverterY32.h"
#include "ocean/cv/FrameConverterYA16.h"
#include "ocean/cv/FrameConverterY_U_V12.h"
#include "ocean/cv/FrameConverterY_U_V24.h"
#include "ocean/cv/FrameConverterY_V_U12.h"
#include "ocean/cv/FrameConverterYUV24.h"
#include "ocean/cv/FrameConverterYUVA32.h"
#include "ocean/cv/FrameConverterYUYV16.h"
#include "ocean/cv/FrameConverterY_UV12.h"
#include "ocean/cv/FrameConverterY_VU12.h"
#include "ocean/cv/FrameConverterYVU24.h"
#include "ocean/cv/SSE.h"

#include "ocean/base/DataType.h"

namespace Ocean
{

namespace CV
{

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_1_UINT8)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const OneSourceGammaOneTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_GAMMA_TO_1_UINT8)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetAlphaConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_1_UINT8_ALPHA)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetBlackLevelWhiteBalanceGammaConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_1_UINT8_BLACKLEVEL_WHITEBALANCE_GAMMA)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint8_t, uint16_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_1_UINT16)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint16_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT16_TO_1_UINT8)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint16_t, uint16_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT16_TO_1_UINT16)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint32_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT32_TO_1_UINT8)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const OneSourceOneTargetConversionFunction<uint32_t, uint16_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT32_TO_1_UINT16)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const OneSourceTwoTargetsConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_2_UINT8)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const OneSourceThreeTargetsConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_1_UINT8_TO_3_UINT8)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const TwoSourcesOneTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_2_UINT8_TO_1_UINT8)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const TwoSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_2_UINT8_TO_1_UINT8_ALPHA)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const TwoSourcesThreeTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_2_UINT8_TO_3_UINT8)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const ThreeSourcesOneTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_3_UINT8_TO_1_UINT8)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const ThreeSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_3_UINT8_TO_1_UINT8_ALPHA)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::FunctionWrapper::FunctionWrapper(const ThreeSourcesThreeTargetConversionFunction<uint8_t, uint8_t> function) :
	function_((const void*)(function)),
	functionType_(FT_3_UINT8_TO_3_UINT8)
{
	// nothing to do here
}

FrameConverter::ConversionFunctionMap::ConversionFunctionMap()
{
	// FORMAT_ABGR32
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_ABGR32, FrameType::FORMAT_BGR24), FrameConverterABGR32::convertABGR32ToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_ABGR32, FrameType::FORMAT_BGRA32), FrameConverterABGR32::convertABGR32ToBGRA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_ABGR32, FrameType::FORMAT_RGBA32), FrameConverterABGR32::convertABGR32ToRGBA32);

	// FORMAT_ABGR32
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_ABGR32, FrameType::FORMAT_BGR24), FrameConverterABGR32::convertABGR32ToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_ABGR32, FrameType::FORMAT_BGRA32), FrameConverterABGR32::convertABGR32ToBGRA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_ABGR32, FrameType::FORMAT_RGBA32), FrameConverterABGR32::convertABGR32ToRGBA32);

	// FORMAT_ARGB32
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_ARGB32, FrameType::FORMAT_BGRA32), FrameConverterARGB32::convertARGB32ToBGRA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_ARGB32, FrameType::FORMAT_RGB24), FrameConverterARGB32::convertARGB32ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_ARGB32, FrameType::FORMAT_RGBA32), FrameConverterARGB32::convertARGB32ToRGBA32);

	// FORMAT_BGR24
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR24, FrameType::FORMAT_BGRA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterBGR24::convertBGR24ToBGRA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR24, FrameType::FORMAT_RGB24), FrameConverterBGR24::convertBGR24ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR24, FrameType::FORMAT_RGBA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterBGR24::convertBGR24ToRGBA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR24, FrameType::FORMAT_Y8), FrameConverterBGR24::convertBGR24ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR24, FrameType::FORMAT_YUV24), FrameConverterBGR24::convertBGR24ToYUV24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR24, FrameType::FORMAT_Y_U_V12_FULL_RANGE), FrameConverterBGR24::convertBGR24FullRangeToY_U_V12FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR24, FrameType::FORMAT_Y_V_U12_FULL_RANGE), FrameConverterBGR24::convertBGR24FullRangeToY_V_U12FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR24, FrameType::FORMAT_Y_U_V12_LIMITED_RANGE), FrameConverterBGR24::convertBGR24FullRangeToY_U_V12LimitedRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR24, FrameType::FORMAT_Y_V_U12_LIMITED_RANGE), FrameConverterBGR24::convertBGR24FullRangeToY_V_U12LimitedRange);

	// FORMAT_BGR32
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR32, FrameType::FORMAT_RGB24), FrameConverterBGR32::convertBGR32ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR32, FrameType::FORMAT_RGBA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterBGR32::convertBGR32ToRGBA32);

	// FORMAT_BGR565
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR565, FrameType::FORMAT_BGR24), FrameConverterBGR565::convertBGR565ToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR565, FrameType::FORMAT_RGB24), FrameConverterBGR565::convertBGR565ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGR565, FrameType::FORMAT_Y8), FrameConverterBGR565::convertBGR565ToY8);

	// FORMAT_BGRA32
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGRA32, FrameType::FORMAT_ARGB32), FrameConverterBGRA32::convertBGRA32ToARGB32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGRA32, FrameType::FORMAT_BGR24), FrameConverterBGRA32::convertBGRA32ToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGRA32, FrameType::FORMAT_BGRA32), FrameConverterBGRA32::convertBGRA32ToBGRA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGRA32, FrameType::FORMAT_RGB24), FrameConverterBGRA32::convertBGRA32ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGRA32, FrameType::FORMAT_RGBA32), FrameConverterBGRA32::convertBGRA32ToRGBA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGRA32, FrameType::FORMAT_Y8), FrameConverterBGRA32::convertBGRA32ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGRA32, FrameType::FORMAT_YA16), FrameConverterBGRA32::convertBGRA32ToYA16);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_BGRA32, FrameType::FORMAT_YUV24), FrameConverterBGRA32::convertBGRA32ToYUV24);

	// FORMAT_RGB24
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_ARGB32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterRGB24::convertRGB24ToARGB32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_BGR24), FrameConverterRGB24::convertRGB24ToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_BGR32), FrameConverterRGB24::convertRGB24ToBGR32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_BGRA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterRGB24::convertRGB24ToBGRA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_RGB32), FrameConverterRGB24::convertRGB24ToRGB32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_RGBA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterRGB24::convertRGB24ToRGBA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y8), FrameConverterRGB24::convertRGB24ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_YUV24), FrameConverterRGB24::convertRGB24ToYUV24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_UV12_LIMITED_RANGE), FrameConverterRGB24::convertRGB24FullRangeToY_UV12LimitedRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_VU12_LIMITED_RANGE), FrameConverterRGB24::convertRGB24FullRangeToY_VU12LimitedRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_UV12_FULL_RANGE), FrameConverterRGB24::convertRGB24FullRangeToY_UV12FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_VU12_FULL_RANGE), FrameConverterRGB24::convertRGB24FullRangeToY_VU12FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_U_V12_LIMITED_RANGE), FrameConverterRGB24::convertRGB24FullRangeToY_U_V12LimitedRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_V_U12_LIMITED_RANGE), FrameConverterRGB24::convertRGB24FullRangeToY_V_U12LimitedRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_U_V12_FULL_RANGE), FrameConverterRGB24::convertRGB24FullRangeToY_U_V12FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB24, FrameType::FORMAT_Y_V_U12_FULL_RANGE), FrameConverterRGB24::convertRGB24FullRangeToY_V_U12FullRange);

	// FORMAT_RGB32
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB32, FrameType::FORMAT_RGB24), FrameConverterRGB32::convertRGB32ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB32, FrameType::FORMAT_RGBA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterRGB32::convertRGB32ToRGBA32);

	// FORMAT_RGB565
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB565, FrameType::FORMAT_RGB24), FrameConverterRGB565::convertRGB565ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGB565, FrameType::FORMAT_Y8), FrameConverterRGB565::convertRGB565ToY8);

	// FORMAT_RGBA32
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGBA32, FrameType::FORMAT_ABGR32), FrameConverterRGBA32::convertRGBA32ToABGR32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGBA32, FrameType::FORMAT_ARGB32), FrameConverterRGBA32::convertRGBA32ToARGB32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGBA32, FrameType::FORMAT_BGR24), FrameConverterRGBA32::convertRGBA32ToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGBA32, FrameType::FORMAT_BGR32), FrameConverterRGBA32::convertRGBA32ToBGR32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGBA32, FrameType::FORMAT_BGRA32), FrameConverterRGBA32::convertRGBA32ToBGRA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGBA32, FrameType::FORMAT_RGB24), FrameConverterRGBA32::convertRGBA32ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGBA32, FrameType::FORMAT_RGB32), FrameConverterRGBA32::convertRGBA32ToRGB32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGBA32, FrameType::FORMAT_Y8), FrameConverterRGBA32::convertRGBA32ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGBA32, FrameType::FORMAT_YA16), FrameConverterRGBA32::convertRGBA32ToYA16);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGBA32, FrameType::FORMAT_YUV24), FrameConverterRGBA32::convertRGBA32ToYUV24);

	// FORMAT_RGBA64
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGBA64, FrameType::FORMAT_RGB24), FrameConverterRGBA64::convertRGBA64ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGBA64, FrameType::FORMAT_RGBA32), FrameConverterRGBA64::convertRGBA64ToRGBA32);

	// FORMAT_RGGB10_Packed
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGGB10_PACKED, FrameType::FORMAT_BGR24), FrameConverterRGGB10_Packed::convertRGGB10_PackedToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGGB10_PACKED, FrameType::FORMAT_RGB24), FrameConverterRGGB10_Packed::convertRGGB10_PackedToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_RGGB10_PACKED, FrameType::FORMAT_RGB24, Options::OT_BLACKLEVEL_WHITEBALANCE_GAMMA), FrameConverterRGGB10_Packed::convertRGGB10_PackedToRGB24BlacklevelWhiteBalanceGammaLUT);

	// FORMAT_UYVY16
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_UYVY16, FrameType::FORMAT_BGR24), FrameConverterUYVY16::convertUYVY16ToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_UYVY16, FrameType::FORMAT_RGB24), FrameConverterUYVY16::convertUYVY16ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_UYVY16, FrameType::FORMAT_YUV24), FrameConverterUYVY16::convertUYVY16ToYUV24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_UYVY16, FrameType::FORMAT_YVU24), FrameConverterUYVY16::convertUYVY16ToYVU24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_UYVY16, FrameType::FORMAT_Y8), FrameConverterUYVY16::convertUYVY16ToY8);

	// FORMAT_Y8
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y8, FrameType::FORMAT_BGR24), FrameConverterY8::convertY8ToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y8, FrameType::FORMAT_RGB24), FrameConverterY8::convertY8ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y8, FrameType::FORMAT_RGBA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterY8::convertY8ToRGBA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y8, FrameType::FORMAT_Y8, Options::OT_GAMMA_CORRECTION), FrameConverterY8::convertY8ToY8GammaLUT);

	// FORMAT_Y10_Packed
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y10_PACKED, FrameType::FORMAT_Y8), FrameConverterY10_Packed::convertY10_PackedToY8Linear);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y10_PACKED, FrameType::FORMAT_Y8, Options::OT_GAMMA_CORRECTION), FrameConverterY10_Packed::convertY10_PackedToY8GammaLUT);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y10_PACKED, FrameType::FORMAT_Y10), FrameConverterY10_Packed::convertY10_PackedToY10);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y10_PACKED, FrameType::FORMAT_BGR24), FrameConverterY10_Packed::convertY10_PackedToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y10_PACKED, FrameType::FORMAT_RGB24), FrameConverterY10_Packed::convertY10_PackedToRGB24);

	// FORMAT_Y16
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y16, FrameType::FORMAT_Y8), FrameConverterY16::convertY16ToY8);

	// FORMAT_Y32
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y32, FrameType::FORMAT_Y8), FrameConverterY32::convertY32ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y32, FrameType::FORMAT_Y16), FrameConverterY32::convertY32ToY16);

	// FORMAT_YA16
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YA16, FrameType::FORMAT_BGRA32), FrameConverterYA16::convertYA16ToBGRA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YA16, FrameType::FORMAT_RGBA32), FrameConverterYA16::convertYA16ToRGBA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YA16, FrameType::FORMAT_Y8), FrameConverterYA16::convertYA16ToY8);

	// FORMAT_Y_U_V12_LIMITED_RANGE (alias is FORMAT_Y_U_V12)
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_Y_U_V12_LIMITED_RANGE), FrameConverterY_U_V12::convertY_U_V12ToY_U_V12);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_Y8), FrameConverterY_U_V12::convertY_U_V12ToY8); // needed for backward compatibility, correct would be an own conversion function
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_Y8_LIMITED_RANGE), FrameConverterY_U_V12::convertY_U_V12ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_BGR24), FrameConverterY_U_V12::convertY_U_V12LimitedRangeToBGR24FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_BGRA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterY_U_V12::convertY_U_V12LimitedRangeToBGRA32FullRangePrecision6Bit);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_RGB24), FrameConverterY_U_V12::convertY_U_V12LimitedRangeToRGB24FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_RGB24, Options::OT_APPROXIMATED), FrameConverterY_U_V12::convertY_U_V12LimitedRangeToRGB24FullRangePrecision6Bit);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_RGBA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterY_U_V12::convertY_U_V12LimitedRangeToRGBA32FullRangePrecision6Bit);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_YUV24), FrameConverterY_U_V12::convertY_U_V12ToYUV24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_LIMITED_RANGE, FrameType::FORMAT_YVU24), FrameConverterY_U_V12::convertY_U_V12ToYVU24);

	// FORMAT_Y_U_V12_FULL_RANGE
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_FULL_RANGE, FrameType::FORMAT_Y8_FULL_RANGE), FrameConverterY_U_V12::convertY_U_V12ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_FULL_RANGE, FrameType::FORMAT_BGR24), FrameConverterY_U_V12::convertY_U_V12FullRangeToBGR24FullRangePrecision6Bit);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V12_FULL_RANGE, FrameType::FORMAT_RGB24), FrameConverterY_U_V12::convertY_U_V12FullRangeToRGB24FullRangePrecision6Bit);

	// FORMAT_Y_U_V24
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V24_LIMITED_RANGE, FrameType::FORMAT_BGR24), FrameConverterY_U_V24::convertY_U_V24LimitedRangeToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V24_LIMITED_RANGE, FrameType::FORMAT_BGRA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterY_U_V24::convertY_U_V24LimitedRangeToBGRA32);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V24_LIMITED_RANGE, FrameType::FORMAT_RGB24), FrameConverterY_U_V24::convertY_U_V24LimitedRangeToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_U_V24_LIMITED_RANGE, FrameType::FORMAT_RGBA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterY_U_V24::convertY_U_V24LimitedRangeToRGBA32);

	// FORMAT_YUV24
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUV24, FrameType::FORMAT_BGR24), FrameConverterYUV24::convertYUV24ToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUV24, FrameType::FORMAT_RGB24), FrameConverterYUV24::convertYUV24ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUV24, FrameType::FORMAT_Y8), FrameConverterYUV24::convertYUV24ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUV24, FrameType::FORMAT_YVU24), FrameConverterYUV24::convertYUV24ToYVU24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUV24, FrameType::FORMAT_Y_U_V12), FrameConverterYUV24::convertYUV24ToY_U_V12);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUV24, FrameType::FORMAT_BGRA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterYUV24::convertYUV24ToBGRA32Precision6Bit);

	// FORMAT_YUVA32
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUVA32, FrameType::FORMAT_YUV24), FrameChannels::removeLastChannel<uint8_t, 4u>);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUVA32, FrameType::FORMAT_Y8), FrameConverterYUVA32::convertYUVA32ToY8);

	// FORMAT_YUVT32
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUVT32, FrameType::FORMAT_YUV24), FrameChannels::removeLastChannel<uint8_t, 4u>);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUVT32, FrameType::FORMAT_Y8), FrameConverterYUVA32::convertYUVA32ToY8);

	// FORMAT_YVU24
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YVU24, FrameType::FORMAT_BGR24), FrameConverterYVU24::convertYVU24ToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YVU24, FrameType::FORMAT_RGB24), FrameConverterYVU24::convertYVU24ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YVU24, FrameType::FORMAT_Y8), FrameConverterYVU24::convertYVU24ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YVU24, FrameType::FORMAT_YUV24), FrameConverterYVU24::convertYVU24ToYUV24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YVU24, FrameType::FORMAT_Y_V_U12), FrameConverterYVU24::convertYVU24ToY_V_U12);

	// FORMAT_YUYV16
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUYV16, FrameType::FORMAT_BGR24), FrameConverterYUYV16::convertYUYV16ToBGR24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUYV16, FrameType::FORMAT_RGB24), FrameConverterYUYV16::convertYUYV16ToRGB24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUYV16, FrameType::FORMAT_YUV24), FrameConverterYUYV16::convertYUYV16ToYUV24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUYV16, FrameType::FORMAT_YVU24), FrameConverterYUYV16::convertYUYV16ToYVU24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_YUYV16, FrameType::FORMAT_Y8), FrameConverterYUYV16::convertYUYV16ToY8);

	// FORMAT_Y_V_U12
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_V_U12, FrameType::FORMAT_Y8), FrameConverterY_V_U12::convertY_V_U12ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_V_U12_LIMITED_RANGE, FrameType::FORMAT_BGR24), FrameConverterY_V_U12::convertY_V_U12LimitedRangeToBGR24FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_V_U12_LIMITED_RANGE, FrameType::FORMAT_RGB24), FrameConverterY_V_U12::convertY_V_U12LimitedRangeToRGB24FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_V_U12, FrameType::FORMAT_YVU24), FrameConverterY_V_U12::convertY_V_U12ToYVU24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_V_U12, FrameType::FORMAT_YUV24), FrameConverterY_V_U12::convertY_V_U12ToYUV24);

	// FORMAT_Y_V_U12_FULL_RANGE
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_V_U12_FULL_RANGE, FrameType::FORMAT_BGR24), FrameConverterY_V_U12::convertY_V_U12FullRangeToBGR24FullRangePrecision6Bit);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_V_U12_FULL_RANGE, FrameType::FORMAT_RGB24), FrameConverterY_V_U12::convertY_V_U12FullRangeToRGB24FullRangePrecision6Bit);

	// FORMAT_Y_UV12
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_UV12, FrameType::FORMAT_Y8), FrameConverterY_UV12::convertY_UV12ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_UV12_FULL_RANGE, FrameType::FORMAT_Y8), FrameConverterY_UV12::convertY_UV12ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_UV12_LIMITED_RANGE, FrameType::FORMAT_BGR24), FrameConverterY_UV12::convertY_UV12LimitedRangeToBGR24FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_UV12_LIMITED_RANGE, FrameType::FORMAT_RGB24), FrameConverterY_UV12::convertY_UV12LimitedRangeToRGB24FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_UV12_FULL_RANGE, FrameType::FORMAT_BGR24), FrameConverterY_UV12::convertY_UV12FullRangeToBGR24FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_UV12_FULL_RANGE, FrameType::FORMAT_RGB24), FrameConverterY_UV12::convertY_UV12FullRangeToRGB24FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_UV12, FrameType::FORMAT_YUV24), FrameConverterY_UV12::convertY_UV12ToYUV24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_UV12, FrameType::FORMAT_YVU24), FrameConverterY_UV12::convertY_UV12ToYVU24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_UV12, FrameType::FORMAT_Y_U_V12), FrameConverterY_UV12::convertY_UV12ToY_U_V12);

	// FORMAT_Y_VU12_LIMITED_RANGE (alias is FORMAT_Y_VU12)
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_Y8), FrameConverterY_VU12::convertY_VU12ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_BGR24), FrameConverterY_VU12::convertY_VU12LimitedRangeToBGR24FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_BGRA32, Options::OT_ALPHA_CHANNEL_TARGET_VALUE), FrameConverterY_VU12::convertY_VU12FullRangeToBGRA32FullRangeAndroid);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_RGB24), FrameConverterY_VU12::convertY_VU12LimitedRangeToRGB24FullRange);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_YUV24), FrameConverterY_VU12::convertY_VU12ToYUV24);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_VU12_LIMITED_RANGE, FrameType::FORMAT_YVU24), FrameConverterY_VU12::convertY_VU12ToYVU24);

	// FORMAT_Y_VU12_FULL_RANGE
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_VU12_FULL_RANGE, FrameType::FORMAT_Y8_FULL_RANGE), FrameConverterY_VU12::convertY_VU12ToY8);
	formatPair2FunctionWrapperMap_.emplace(ConversionTriple(FrameType::FORMAT_Y_VU12_FULL_RANGE, FrameType::FORMAT_RGB24), FrameConverterY_VU12::convertY_VU12FullRangeToRGB24FullRangePrecision6Bit);
}

const void* FrameConverter::ConversionFunctionMap::function(const FrameType::PixelFormat& sourcePixelFormat, const FrameType::PixelFormat& targetPixelFormat, FunctionType& functionType, const Options& options) const
{
	FormatPair2FunctionWrapperMap::const_iterator i = formatPair2FunctionWrapperMap_.find(ConversionTriple(sourcePixelFormat, targetPixelFormat, options.optionsType()));

	if (i != formatPair2FunctionWrapperMap_.cend())
	{
		functionType = i->second.functionType_;
		return i->second.function_;
	}

	if ((options.optionsType() & Options::OT_APPROXIMATED) == Options::OT_APPROXIMATED && options.optionsType() != Options::OT_APPROXIMATED)
	{
		// let's see whether we have a perfect matching not-approximating conversion for the source and target pixel format

		const Options::OptionsType notApproximatedOptions = Options::OptionsType(options.optionsType() & ~Options::OT_APPROXIMATED);

		i = formatPair2FunctionWrapperMap_.find(ConversionTriple(sourcePixelFormat, targetPixelFormat, notApproximatedOptions));

		if (i != formatPair2FunctionWrapperMap_.cend())
		{
			functionType = i->second.functionType_;
			return i->second.function_;
		}
	}

	if (options.optionsType() != Options::OT_DEFAULT)
	{
		// let's see whether we have a default conversion for the source and target pixel format

		i = formatPair2FunctionWrapperMap_.find(ConversionTriple(sourcePixelFormat, targetPixelFormat, Options::OT_DEFAULT));

		if (i != formatPair2FunctionWrapperMap_.cend())
		{
			functionType = i->second.functionType_;
			return i->second.function_;
		}
	}

	return nullptr;
}

bool FrameConverter::Comfort::isSupported(const FrameType& sourceType, const FrameType::PixelFormat targetPixelFormat, const Options& options)
{
	if (!sourceType.isValid() || targetPixelFormat == FrameType::FORMAT_UNDEFINED)
	{
		return false;
	}

	if (sourceType.width() % FrameType::widthMultiple(targetPixelFormat) != 0u || sourceType.height() % FrameType::heightMultiple(targetPixelFormat) != 0u)
	{
		return false;
	}

	if (sourceType.numberPlanes() == 1u && ((sourceType.pixelFormat() == targetPixelFormat && FrameType::formatIsGeneric(sourceType.pixelFormat()))
		 || (FrameType::formatIsPureGeneric(sourceType.pixelFormat()) && sourceType.pixelFormat() == FrameType::makeGenericPixelFormat(targetPixelFormat))))
	{
		ocean_assert(sourceType.channels() >= 1u);

		return sourceType.channels() <= 4u;
	}

	ConversionFunctionMap::FunctionType functionType = ConversionFunctionMap::FT_INVALID;
	const void* function = ConversionFunctionMap::get().function(sourceType.pixelFormat(), targetPixelFormat, functionType, options);

	return function != nullptr;
}

bool FrameConverter::Comfort::convert(const Frame& source, const FrameType::PixelFormat targetPixelFormat, const FrameType::PixelOrigin targetPixelOrigin, Frame& target, const bool forceCopy, Worker* worker, const Options& options)
{
	ocean_assert(source.isValid());
	ocean_assert(targetPixelFormat != FrameType::FORMAT_UNDEFINED);
	ocean_assert(targetPixelOrigin != FrameType::ORIGIN_INVALID);

	if (&source == &target)
	{
		ocean_assert(false && "Source and target must not be the same object!");
		return false;
	}

	if (!source.isValid())
	{
		ocean_assert(false && "Invalid source frame.");
		return false;
	}

	if (targetPixelFormat == FrameType::FORMAT_UNDEFINED || targetPixelOrigin == FrameType::ORIGIN_INVALID)
	{
		ocean_assert(false && "Invalid target pixel format or pixel origin.");
		return false;
	}

	const FrameType targetType(source.frameType(), targetPixelFormat, targetPixelOrigin);

	if (!targetType.isValid())
	{
		ocean_assert(false && "Invalid target frame type");
		return false;
	}

	if (targetType.width() % FrameType::widthMultiple(targetType.pixelFormat()) != 0u || targetType.height() % FrameType::heightMultiple(targetType.pixelFormat()) != 0u)
	{
		ocean_assert(false && "Invalid target frame resolution.");
		return false;
	}

	const ConversionFlag flag = source.pixelOrigin() == targetType.pixelOrigin() ? CONVERT_NORMAL : CONVERT_FLIPPED;

	const bool perfectMatchAndGeneric = source.pixelFormat() == targetType.pixelFormat() && FrameType::formatIsGeneric(source.pixelFormat());
	const bool compatibleAndAtLeastOneIsPureGeneric = (FrameType::formatIsPureGeneric(source.pixelFormat()) || FrameType::formatIsPureGeneric(targetType.pixelFormat())) && FrameType::arePixelFormatsCompatible(source.pixelFormat(), targetType.pixelFormat());

	ConversionFunctionMap::FunctionType functionType = ConversionFunctionMap::FT_INVALID;
	const void* function = ConversionFunctionMap::get().function(source.pixelFormat(), targetType.pixelFormat(), functionType, options);

	if (function != nullptr)
	{
		constexpr bool forceOwner = false;
		constexpr bool forceWritable = true;

		if (!target.set(targetType, forceOwner, forceWritable))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		switch (functionType)
		{
			case ConversionFunctionMap::FT_1_UINT8_TO_1_UINT8:
			{
				typedef ConversionFunctionMap::OneSourceOneTargetConversionFunction<uint8_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
				break;
			}

			case ConversionFunctionMap::FT_1_UINT8_GAMMA_TO_1_UINT8:
			{
				typedef ConversionFunctionMap::OneSourceGammaOneTargetConversionFunction<uint8_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), flag, options.gamma(), source.paddingElements(), target.paddingElements(), worker);
				break;
			}

			case ConversionFunctionMap::FT_1_UINT8_TO_1_UINT8_ALPHA:
			{
				typedef ConversionFunctionMap::OneSourceOneTargetAlphaConversionFunction<uint8_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), options.alphaChannelTargetValue(), worker);
				break;
			}

			case ConversionFunctionMap::FT_1_UINT8_TO_1_UINT8_BLACKLEVEL_WHITEBALANCE_GAMMA:
			{
				typedef ConversionFunctionMap::OneSourceOneTargetBlackLevelWhiteBalanceGammaConversionFunction<uint8_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), flag, options.blackLevel(), options.whiteBalance(), options.gamma(), source.paddingElements(), target.paddingElements(), worker);
				break;
			}

			case ConversionFunctionMap::FT_1_UINT8_TO_1_UINT16:
			{
				typedef ConversionFunctionMap::OneSourceOneTargetConversionFunction<uint8_t, uint16_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
				break;
			}

			case ConversionFunctionMap::FT_1_UINT16_TO_1_UINT8:
			{
				typedef ConversionFunctionMap::OneSourceOneTargetConversionFunction<uint16_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint16_t>(), target.data<uint8_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
				break;
			}

			case ConversionFunctionMap::FT_1_UINT16_TO_1_UINT16:
			{
				typedef ConversionFunctionMap::OneSourceOneTargetConversionFunction<uint16_t, uint16_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
				break;
			}

			case ConversionFunctionMap::FT_1_UINT32_TO_1_UINT8:
			{
				typedef ConversionFunctionMap::OneSourceOneTargetConversionFunction<uint32_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint32_t>(), target.data<uint8_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
				break;
			}

			case ConversionFunctionMap::FT_1_UINT32_TO_1_UINT16:
			{
				typedef ConversionFunctionMap::OneSourceOneTargetConversionFunction<uint32_t, uint16_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint32_t>(), target.data<uint16_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
				break;
			}

			case ConversionFunctionMap::FT_1_UINT8_TO_2_UINT8:
			{
				typedef ConversionFunctionMap::OneSourceTwoTargetsConversionFunction<uint8_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(), target.data<uint8_t>(0u), target.data<uint8_t>(1u), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(0u), target.paddingElements(1u), worker);
				break;
			}

			case ConversionFunctionMap::FT_1_UINT8_TO_3_UINT8:
			{
				typedef ConversionFunctionMap::OneSourceThreeTargetsConversionFunction<uint8_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(), target.data<uint8_t>(0u), target.data<uint8_t>(1u), target.data<uint8_t>(2u), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(0u), target.paddingElements(1u), target.paddingElements(2u), worker);
				break;
			}

			case ConversionFunctionMap::FT_2_UINT8_TO_1_UINT8:
			{
				typedef ConversionFunctionMap::TwoSourcesOneTargetConversionFunction<uint8_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), target.data<uint8_t>(0u), source.width(), source.height(), flag, source.paddingElements(0u), source.paddingElements(1u), target.paddingElements(0u), worker);
				break;
			}

			case ConversionFunctionMap::FT_2_UINT8_TO_1_UINT8_ALPHA:
			{
				typedef ConversionFunctionMap::TwoSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), target.data<uint8_t>(0u), source.width(), source.height(), flag, source.paddingElements(0u), source.paddingElements(1u), target.paddingElements(0u), options.alphaChannelTargetValue(), worker);
				break;
			}

			case ConversionFunctionMap::FT_2_UINT8_TO_3_UINT8:
			{
				typedef ConversionFunctionMap::TwoSourcesThreeTargetConversionFunction<uint8_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), target.data<uint8_t>(0u), target.data<uint8_t>(1u), target.data<uint8_t>(2u), source.width(), source.height(), flag, source.paddingElements(0u), source.paddingElements(1u), target.paddingElements(0u), target.paddingElements(1u), target.paddingElements(2u), worker);
				break;
			}

			case ConversionFunctionMap::FT_3_UINT8_TO_1_UINT8:
			{
				typedef ConversionFunctionMap::ThreeSourcesOneTargetConversionFunction<uint8_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), source.constdata<uint8_t>(2u), target.data<uint8_t>(0u), source.width(), source.height(), flag, source.paddingElements(0u), source.paddingElements(1u), source.paddingElements(2u), target.paddingElements(0u), worker);
				break;
			}

			case ConversionFunctionMap::FT_3_UINT8_TO_1_UINT8_ALPHA:
			{
				typedef ConversionFunctionMap::ThreeSourcesOneTargetAlphaConversionFunction<uint8_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), source.constdata<uint8_t>(2u), target.data<uint8_t>(0u), source.width(), source.height(), flag, source.paddingElements(0u), source.paddingElements(1u), source.paddingElements(2u), target.paddingElements(0u), options.alphaChannelTargetValue(), worker);
				break;
			}

			case ConversionFunctionMap::FT_3_UINT8_TO_3_UINT8:
			{
				typedef ConversionFunctionMap::ThreeSourcesThreeTargetConversionFunction<uint8_t, uint8_t> SpecializedFunction;
				const SpecializedFunction specializedFunction = (const SpecializedFunction)(function);

				specializedFunction(source.constdata<uint8_t>(0u), source.constdata<uint8_t>(1u), source.constdata<uint8_t>(2u), target.data<uint8_t>(0u), target.data<uint8_t>(1u), target.data<uint8_t>(2u), source.width(), source.height(), flag, source.paddingElements(0u), source.paddingElements(1u), source.paddingElements(2u), target.paddingElements(0u), target.paddingElements(1u), target.paddingElements(2u), worker);
				break;
			}

			default:
				ocean_assert(false && "Invalid function type!");
				return false;
		}
	}
	else if (source.numberPlanes() == 1u && (perfectMatchAndGeneric || compatibleAndAtLeastOneIsPureGeneric))
	{
		if (source.pixelOrigin() == targetType.pixelOrigin() && forceCopy == false)
		{
			target = Frame(source, Frame::ACM_USE_KEEP_LAYOUT);
		}
		else
		{
			constexpr bool forceOwner = false;
			constexpr bool forceWritable = true;

			if (!target.set(targetType, forceOwner, forceWritable))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			ocean_assert(source.numberPlanes() == 1u && target.numberPlanes() == 1u);

			switch (source.dataType())
			{
				case FrameType::DT_UNSIGNED_INTEGER_8:
				case FrameType::DT_SIGNED_INTEGER_8:
				{
					switch (source.channels())
					{
						case 1u:
							FrameChannels::transformGeneric<uint8_t, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						case 2u:
							FrameChannels::transformGeneric<uint8_t, 2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						case 3u:
							FrameChannels::transformGeneric<uint8_t, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						case 4u:
							FrameChannels::transformGeneric<uint8_t, 4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						default:
							ocean_assert(false && "Invalid function type!");
							return false;
					}

					break;
				}

				case FrameType::DT_UNSIGNED_INTEGER_16:
				case FrameType::DT_SIGNED_INTEGER_16:
				case FrameType::DT_SIGNED_FLOAT_16:
				{
					switch (source.channels())
					{
						case 1u:
							FrameChannels::transformGeneric<uint16_t, 1u>(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						case 2u:
							FrameChannels::transformGeneric<uint16_t, 2u>(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						case 3u:
							FrameChannels::transformGeneric<uint16_t, 3u>(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						case 4u:
							FrameChannels::transformGeneric<uint16_t, 4u>(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						default:
							ocean_assert(false && "Invalid function type!");
							return false;
					}

					break;
				}

				case FrameType::DT_UNSIGNED_INTEGER_32:
				case FrameType::DT_SIGNED_INTEGER_32:
				case FrameType::DT_SIGNED_FLOAT_32:
				{
					switch (source.channels())
					{
						case 1u:
							FrameChannels::transformGeneric<uint32_t, 1u>(source.constdata<uint32_t>(), target.data<uint32_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						case 2u:
							FrameChannels::transformGeneric<uint32_t, 2u>(source.constdata<uint32_t>(), target.data<uint32_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						case 3u:
							FrameChannels::transformGeneric<uint32_t, 3u>(source.constdata<uint32_t>(), target.data<uint32_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						case 4u:
							FrameChannels::transformGeneric<uint32_t, 4u>(source.constdata<uint32_t>(), target.data<uint32_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						default:
							ocean_assert(false && "Invalid function type!");
							return false;
					}

					break;
				}

				case FrameType::DT_UNSIGNED_INTEGER_64:
				case FrameType::DT_SIGNED_INTEGER_64:
				case FrameType::DT_SIGNED_FLOAT_64:
				{
					switch (source.channels())
					{
						case 1u:
							FrameChannels::transformGeneric<uint64_t, 1u>(source.constdata<uint64_t>(), target.data<uint64_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						case 2u:
							FrameChannels::transformGeneric<uint64_t, 2u>(source.constdata<uint64_t>(), target.data<uint64_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						case 3u:
							FrameChannels::transformGeneric<uint64_t, 3u>(source.constdata<uint64_t>(), target.data<uint64_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						case 4u:
							FrameChannels::transformGeneric<uint64_t, 4u>(source.constdata<uint64_t>(), target.data<uint64_t>(), source.width(), source.height(), flag, source.paddingElements(), target.paddingElements(), worker);
							break;

						default:
							ocean_assert(false && "Invalid function type!");
							return false;
					}

					break;
				}

				case FrameType::DT_UNDEFINED:
				case FrameType::DT_END:
					ocean_assert(false && "Invalid data type!");
					return false;
			}
		}
	}
	else
	{
		ocean_assert(false && "Invalid frame types.");
		return false;
	}

	target.setTimestamp(source.timestamp());
	target.setRelativeTimestamp(source.relativeTimestamp());

	return true;
}

bool FrameConverter::Comfort::convertAndCopy(const Frame& source, Frame& target, Worker* worker, const Options& options)
{
	if (!source.isValid() || !target.isValid())
	{
		ocean_assert(false && "Source and target frame must be defined!");
		return false;
	}

	if (source.haveIntersectingMemory(target))
	{
		ocean_assert(false && "Source and target frame must not share the same memory!");
		return false;
	}

	if (source.width() != target.width() || source.height() != target.height())
	{
		ocean_assert(false && "Source and target frame must have the same dimension!");
		return false;
	}

	if (target.isReadOnly())
	{
		ocean_assert(false && "The target frame must contain writable memory!");
		return false;
	}

	return convert(source, target.pixelFormat(), target.pixelOrigin(), target, true, worker, options);
}

MatrixD FrameConverter::transformationMatrix_FullRangeRGB24_To_FullRangeYUV24_BT601()
{
	// BT.601, analog RGB to (analog) YPbPr

	// Color space with full range:
	// RGB input value range:  [0, 255]x[0, 255]x[0, 255]
	// YUV output value range: [0, 255]x[0, 255]x[0, 255]

	// | Y |   |  0.299       0.587       0.114        0 |   | R |
	// | U | = | -0.168736   -0.331264    0.5        128 | * | G |
	// | V |   |  0.5        -0.418688   -0.081312   128 |   | B |
	//                                                       | 1 |

	// Approximation with 7 bit precision:
	//       | Y |     |  38     75    15     0    128 |   | R |
	// 128 * | U |  =  | -22    -42    64    128 * 128 | * | G |
	//       | V |     |  64    -54   -10    128 * 128 |   | B |
	//                                                     | 1 |

	MatrixD transformationMatrix(3, 4, false);

	transformationMatrix(0, 0) = 0.299;
	transformationMatrix(1, 0) = -0.168736;
	transformationMatrix(2, 0) = 0.5;

	transformationMatrix(0, 1) = 0.587;
	transformationMatrix(1, 1) = -0.331264;
	transformationMatrix(2, 1) = -0.418688;

	transformationMatrix(0, 2) = 0.114;
	transformationMatrix(1, 2) = 0.5;
	transformationMatrix(2, 2) = -0.081312;

	transformationMatrix(0, 3) = 0.0;
	transformationMatrix(1, 3) = 128.0;
	transformationMatrix(2, 3) = 128.0;

	return transformationMatrix;
}

MatrixD FrameConverter::transformationMatrix_FullRangeRGB24_To_FullRangeYVU24_BT601()
{
	// | Y |   | 1       |   | Y |
	// | V | = |       1 | * | U |
	// | U |   |   1     |   | V |

	MatrixD yvu_T_yuv(3, 3, false);

	yvu_T_yuv(0, 0) = 1.0;
	yvu_T_yuv(1, 2) = 1.0;
	yvu_T_yuv(2, 1) = 1.0;

	return yvu_T_yuv * transformationMatrix_FullRangeRGB24_To_FullRangeYUV24_BT601();
}

MatrixD FrameConverter::transformationMatrix_FullRangeRGB24_To_LimitedRangeYUV24_BT601()
{
	// BT.601, analog RGB to (digital) YCbCr

	// Color space with limited range:
	// RGB input value range:  [0, 255]x[0, 255]x[0, 255]
	// YUV output value range: [16, 235]x[16, 240]x[16, 240]

	// | Y |   |  0.2578125   0.5039063   0.09765625  16.0  |   | R |
	// | U | = | -0.1484375  -0.2890625   0.4375      128.0 | * | G |
	// | V |   |  0.4375     -0.3671875  -0.0703125   128.0 |   | B |
	//                                                          | 1 |

	// Approximation with 7 bit precision:
	//       | Y |     |  33     64    13     16 * 128 |   | R |
	// 128 * | U |  =  | -19    -37    56    128 * 128 | * | G |
	//       | V |     |  56    -47   -9     128 * 128 |   | B |
	//                                                     | 1 |

	MatrixD transformationMatrix(3, 4, false);

	transformationMatrix(0, 0) = 0.2578125;
	transformationMatrix(1, 0) = -0.1484375;
	transformationMatrix(2, 0) = 0.4375;

	transformationMatrix(0, 1) = 0.5039063;
	transformationMatrix(1, 1) = -0.2890625;
	transformationMatrix(2, 1) = -0.3671875;

	transformationMatrix(0, 2) = 0.09765625;
	transformationMatrix(1, 2) = 0.4375;
	transformationMatrix(2, 2) = -0.0703125;

	transformationMatrix(0, 3) = 16.0;
	transformationMatrix(1, 3) = 128.0;
	transformationMatrix(2, 3) = 128.0;

	return transformationMatrix;
}

MatrixD FrameConverter::transformationMatrix_FullRangeRGB24_To_LimitedRangeYVU24_BT601()
{
	// | Y |   | 1       |   | Y |
	// | V | = |       1 | * | U |
	// | U |   |   1     |   | V |

	MatrixD yvu_T_yuv(3, 3, false);

	yvu_T_yuv(0, 0) = 1.0;
	yvu_T_yuv(1, 2) = 1.0;
	yvu_T_yuv(2, 1) = 1.0;

	return yvu_T_yuv * transformationMatrix_FullRangeRGB24_To_LimitedRangeYUV24_BT601();
}

MatrixD FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeBGR24_BT601()
{
	// | B |     | 0  0  1 |   | R |
	// | G |  =  | 0  1  0 | * | G |
	// | R |     | 1  0  0 |   | B |

	MatrixD rgbToBgr(3, 3, false);
	rgbToBgr(0, 2) = 1.0;
	rgbToBgr(1, 1) = 1.0;
	rgbToBgr(2, 0) = 1.0;

	return rgbToBgr * transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_BT601();
}

MatrixD FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_BT601()
{
	// BT.601, (analog) YPbPr to analog RGB

	// Color space with limited range:
	// YUV input value range:  [0, 255]x[0, 255]x[0, 255]
	// RGB output value range: [0, 255]x[0, 255]x[0, 255]

	// | R |     | 1.0     0.0        1.402     -179.456   |   | Y |
	// | G |  =  | 1.0    -0.34414   -0.71414    135.45984 | * | U |
	// | B |     | 1.0     1.772      0.0       -226.816   |   | V |
	//                                                         | 1 |

	// Approximation with 6 bit precision:
	//      | R |     | 64    0     90 |   |    Y    |
	// 64 * | G |  =  | 64   -22   -46 | * | U - 128 |
	//      | B |     | 64   113     0 |   | V - 128 |

	MatrixD transformationMatrix(3, 4, false);

	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 0) = 1.0;
	transformationMatrix(2, 0) = 1.0;

	transformationMatrix(0, 1) = 0.0;
	transformationMatrix(1, 1) = -0.34414;
	transformationMatrix(2, 1) = 1.772;

	transformationMatrix(0, 2) = 1.402;
	transformationMatrix(1, 2) = -0.71414;
	transformationMatrix(2, 2) = 0.0;

	transformationMatrix(0, 3) = -179.456;
	transformationMatrix(1, 3) = 135.45984;
	transformationMatrix(2, 3) = -226.816;

	return transformationMatrix;
}

MatrixD FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeBGR24_Android()
{
	// | B |     | 0  0  1 |   | R |
	// | G |  =  | 0  1  0 | * | G |
	// | R |     | 1  0  0 |   | B |

	MatrixD bgr_T_rgb(3, 3, false);
	bgr_T_rgb(0, 2) = 1.0;
	bgr_T_rgb(1, 1) = 1.0;
	bgr_T_rgb(2, 0) = 1.0;

	return bgr_T_rgb * transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_Android();
}

MatrixD FrameConverter::transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_Android()
{
	// Android-specific (analog) YPbPr to analog RGB, however quite close to BT.601

	// Color space with limited range:
	// YUV input value range:  [0, 255]x[0, 255]x[0, 255]
	// RGB output value range: [0, 255]x[0, 255]x[0, 255]

	// | R |     | 1.0     0.0         1.370705  |   |    Y    |     | 1.0     0.0         1.370705   -175.45024  |   | Y |
	// | G |  =  | 1.0    -0.337633   -0.698001  | * | U - 128 |  =  | 1.0    -0.337633   -0.698001    132.561152 | * | U |
	// | B |     | 1.0     1.732446    0.0       |   | V - 128 |     | 1.0     1.732446    0.0        -221.753088 |   | V |
	//                                                                                                                | 1 |

	MatrixD transformationMatrix(3, 4, false);

	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 0) = 1.0;
	transformationMatrix(2, 0) = 1.0;

	transformationMatrix(0, 1) = 0.0;
	transformationMatrix(1, 1) = -0.337633;
	transformationMatrix(2, 1) = 1.732446;

	transformationMatrix(0, 2) = 1.370705;
	transformationMatrix(1, 2) = -0.698001;
	transformationMatrix(2, 2) = 0.0;

	transformationMatrix(0, 3) = -175.45024;
	transformationMatrix(1, 3) = 132.561152;
	transformationMatrix(2, 3) = -221.753088;

	return transformationMatrix;
}

MatrixD FrameConverter::transformationMatrix_FullRangeYVU24_To_FullRangeBGR24_Android()
{
	// | B |     | 0  0  1 |   | R |
	// | G |  =  | 0  1  0 | * | G |
	// | R |     | 1  0  0 |   | B |

	MatrixD bgr_T_rgb(3, 3, false);
	bgr_T_rgb(0, 2) = 1.0;
	bgr_T_rgb(1, 1) = 1.0;
	bgr_T_rgb(2, 0) = 1.0;

	return bgr_T_rgb * transformationMatrix_FullRangeYVU24_To_FullRangeRGB24_Android();
}

MatrixD FrameConverter::transformationMatrix_FullRangeYVU24_To_FullRangeRGB24_Android()
{
	// Android-specific (analog) YPbPr to analog RGB, however quite close to BT.601

	// Color space with limited range:
	// YUV input value range:  [0, 255]x[0, 255]x[0, 255]
	// RGB output value range: [0, 255]x[0, 255]x[0, 255]

	// | R |     | 1.0     1.370705    0.0      |   |    Y    |     | 1.0    1.370705    0.0        -175.45024  |   | Y |
	// | G |  =  | 1.0    -0.698001   -0.337633 | * | V - 128 |  =  | 1.0   -0.698001   -0.337633    132.561152 | * | V |
	// | B |     | 1.0     0.0         1.732446 |   | U - 128 |     | 1.0    0.0         1.732446   -221.753088 |   | U |
	//                                                                                                              | 1 |

	MatrixD transformationMatrix(3, 4, false);

	transformationMatrix(0, 0) = 1.0;
	transformationMatrix(1, 0) = 1.0;
	transformationMatrix(2, 0) = 1.0;

	transformationMatrix(0, 1) = 1.370705;
	transformationMatrix(1, 1) = -0.698001;
	transformationMatrix(2, 1) = 0.0;

	transformationMatrix(0, 2) = 0.0;
	transformationMatrix(1, 2) = -0.337633;
	transformationMatrix(2, 2) = 1.732446;

	transformationMatrix(0, 3) = -175.45024;
	transformationMatrix(1, 3) = 132.561152;
	transformationMatrix(2, 3) = -221.753088;

	return transformationMatrix;
}

MatrixD FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601()
{
	// BT.601, (digital) YCbCr to analog RGB

	// Color space with limited range:
	// YUV input value range:  [16, 235]x[16, 240]x[16, 240]
	// RGB output value range: [0, 255]x[0, 255]x[0, 255]

	// | R |     | 1.1639404296875   0.0              1.595947265625  -222.904296875 |   | Y |
	// | G |  =  | 1.1639404296875  -0.3909912109375 -0.81298828125    135.486328125 | * | U |
	// | B |     | 1.1639404296875   2.0179443359375  0.0             -276.919921875 |   | V |
	//                                                                                   | 1 |

	// Approximation with 13 bit precision:
	//        | R |     | 9535     0         13074 |   | Y -  16 |
	// 8192 * | G |  =  | 9535    -3203     -6660  | * | U - 128 |
	//        | B |     | 9535     16531     0     |   | V - 128 |

	// Approximation with 10 bit precision:
	//        | R |     | 1192     0        1634 |   | Y -  16 |       | 1192     0        1634     -223 * 1024 |   | Y |
	// 1024 * | G |  =  | 1192    -400     -833  | * | U - 128 |   =   | 1192    -400     -833       135 * 1024 | * | U |
	//        | B |     | 1192     2066     0    |   | V - 128 |       | 1192     2066     0        -277 * 1024 |   | V |
	//                                                                                                              | 1 |

	// Approximation with 8 bit precision:
	//       | R |     | 298     0       409  |   | Y -  16 |
	// 256 * | G |  =  | 298    -409    -208  | * | U - 128 |
	//       | B |     | 298     516     0    |   | V - 128 |

	// Approximation with 6 bit precision:
	//      | R |     | 75    0     102 |   | Y -  16 |
	// 64 * | G |  =  | 75   -25   -52  | * | U - 128 |
	//      | B |     | 75   128     0  |   | V - 128 |

	MatrixD transformationMatrix(3, 4, false);

	transformationMatrix(0, 0) = 1.1639404296875;
	transformationMatrix(1, 0) = 1.1639404296875;
	transformationMatrix(2, 0) = 1.1639404296875;

	transformationMatrix(0, 1) = 0.0;
	transformationMatrix(1, 1) = -0.3909912109375;
	transformationMatrix(2, 1) = 2.0179443359375;

	transformationMatrix(0, 2) = 1.595947265625;
	transformationMatrix(1, 2) = -0.81298828125;
	transformationMatrix(2, 2) = 0.0;

	transformationMatrix(0, 3) = -222.904296875;
	transformationMatrix(1, 3) = 135.486328125;
	transformationMatrix(2, 3) = -276.919921875;

	return transformationMatrix;
}

MatrixD FrameConverter::transformationMatrix_FullRangeBGR24_To_LimitedRangeYUV24_BT601()
{
	// | R |     | 0  0  1  0 |   | B |
	// | G |  =  | 0  1  0  0 | * | G |
	// | B |     | 1  0  0  0 |   | R |
	// | 1 |     | 0  0  0  1 |   | 1 |

	MatrixD rgb_T_bgr(4, 4, false);
	rgb_T_bgr(0, 2) = 1.0;
	rgb_T_bgr(1, 1) = 1.0;
	rgb_T_bgr(2, 0) = 1.0;
	rgb_T_bgr(3, 3) = 1.0;

	return transformationMatrix_FullRangeRGB24_To_LimitedRangeYUV24_BT601() * rgb_T_bgr;
}

MatrixD FrameConverter::transformationMatrix_FullRangeBGR24_To_FullRangeYUV24_BT601()
{
	// | R |     | 0  0  1  0 |   | B |
	// | G |  =  | 0  1  0  0 | * | G |
	// | B |     | 1  0  0  0 |   | R |
	// | 1 |     | 0  0  0  1 |   | 1 |

	MatrixD rgb_T_bgr(4, 4, false);
	rgb_T_bgr(0, 2) = 1.0;
	rgb_T_bgr(1, 1) = 1.0;
	rgb_T_bgr(2, 0) = 1.0;
	rgb_T_bgr(3, 3) = 1.0;

	return transformationMatrix_FullRangeRGB24_To_FullRangeYUV24_BT601() * rgb_T_bgr;
}

MatrixD FrameConverter::transformationMatrix_FullRangeBGR24_To_FullRangeYVU24_BT601()
{
	// | Y |   | 1       |   | Y |
	// | V | = |       1 | * | U |
	// | U |   |   1     |   | V |

	MatrixD yvu_T_yuv(3, 3, false);

	yvu_T_yuv(0, 0) = 1.0;
	yvu_T_yuv(1, 2) = 1.0;
	yvu_T_yuv(2, 1) = 1.0;

	return yvu_T_yuv * transformationMatrix_FullRangeBGR24_To_FullRangeYUV24_BT601();
}

MatrixD FrameConverter::transformationMatrix_LimitedRangeYUV24_To_FullRangeBGR24_BT601()
{
	// | B |     | 0  0  1 |   | R |
	// | G |  =  | 0  1  0 | * | G |
	// | R |     | 1  0  0 |   | B |

	MatrixD rgbToBgr(3, 3, false);
	rgbToBgr(0, 2) = 1.0;
	rgbToBgr(1, 1) = 1.0;
	rgbToBgr(2, 0) = 1.0;

	return rgbToBgr * transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601();
}

MatrixD FrameConverter::transformationMatrix_LimitedRangeYVU24_To_FullRangeBGR24_BT601()
{
	// | Y |     | 1  0  0  0 |   | Y |
	// | U |  =  | 0  0  1  0 | * | V |
	// | V |     | 0  1  0  0 |   | U |
	// | 1 |     | 0  0  0  1 |   | 1 |

	MatrixD yvuToYuv(4, 4, false);
	yvuToYuv(0, 0) = 1.0;
	yvuToYuv(1, 2) = 1.0;
	yvuToYuv(2, 1) = 1.0;
	yvuToYuv(3, 3) = 1.0;

	// | B |     | 0  0  1 |   | R |
	// | G |  =  | 0  1  0 | * | G |
	// | R |     | 1  0  0 |   | B |

	MatrixD rgbToBgr(3, 3, false);
	rgbToBgr(0, 2) = 1.0;
	rgbToBgr(1, 1) = 1.0;
	rgbToBgr(2, 0) = 1.0;

	return rgbToBgr * transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601() * yvuToYuv;
}

MatrixD FrameConverter::transformationMatrix_LimitedRangeYVU24_To_FullRangeRGB24_BT601()
{
	// | Y |     | 1  0  0  0 |   | Y |
	// | U |  =  | 0  0  1  0 | * | V |
	// | V |     | 0  1  0  0 |   | U |
	// | 1 |     | 0  0  0  1 |   | 1 |

	MatrixD yvuToYuv(4, 4, false);
	yvuToYuv(0, 0) = 1.0;
	yvuToYuv(1, 2) = 1.0;
	yvuToYuv(2, 1) = 1.0;
	yvuToYuv(3, 3) = 1.0;

	return transformationMatrix_LimitedRangeYUV24_To_FullRangeRGB24_BT601() * yvuToYuv;
}

MatrixD FrameConverter::transformationMatrix_FullRangeYVU24_To_FullRangeRGB24_BT601()
{
	// | Y |     | 1  0  0  0 |   | Y |
	// | U |  =  | 0  0  1  0 | * | V |
	// | V |     | 0  1  0  0 |   | U |
	// | 1 |     | 0  0  0  1 |   | 1 |

	MatrixD yvuToYuv(4, 4, false);
	yvuToYuv(0, 0) = 1.0;
	yvuToYuv(1, 2) = 1.0;
	yvuToYuv(2, 1) = 1.0;
	yvuToYuv(3, 3) = 1.0;

	return transformationMatrix_FullRangeYUV24_To_FullRangeRGB24_BT601() * yvuToYuv;
}

MatrixD FrameConverter::transformationMatrix_FullRangeYVU24_To_FullRangeBGR24_BT601()
{
	// | B |     | 0  0  1 |   | R |
	// | G |  =  | 0  1  0 | * | G |
	// | R |     | 1  0  0 |   | B |

	MatrixD rgbToBgr(3, 3, false);
	rgbToBgr(0, 2) = 1.0;
	rgbToBgr(1, 1) = 1.0;
	rgbToBgr(2, 0) = 1.0;

	return rgbToBgr * transformationMatrix_FullRangeYVU24_To_FullRangeRGB24_BT601();
}

const FrameConverter::ConversionFlags& FrameConverter::conversionFlags()
{
	static const ConversionFlags flags =
	{
		CV::FrameConverter::CONVERT_NORMAL,
		CV::FrameConverter::CONVERT_FLIPPED,
		CV::FrameConverter::CONVERT_MIRRORED,
		CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED
	};

	return flags;
}

std::string FrameConverter::translateConversionFlag(const ConversionFlag conversionFlag)
{
	switch (conversionFlag)
	{
		case CV::FrameConverter::CONVERT_NORMAL:
			return std::string("normal");

		case CV::FrameConverter::CONVERT_FLIPPED:
			return std::string("flipped");

		case CV::FrameConverter::CONVERT_MIRRORED:
			return std::string("mirrored");

		case CV::FrameConverter::CONVERT_FLIPPED_AND_MIRRORED:
			return std::string("flipped and mirrored");

		default:
			break;
	}

	ocean_assert(false && "Invalid conversion flag!");
	return std::string("INVALID");
}

void FrameConverter::convertGenericPixelFormatSubset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourceStrideBytes, const unsigned int targetStrideBytes, const ConversionFlag flag, const RowConversionFunction<uint8_t, uint8_t> rowConversionFunction, const RowReversePixelOrderInPlaceFunction<uint8_t> targetReversePixelOrderInPlaceFunction, const bool areContinuous, const void* options, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(sourceStrideBytes >= width && targetStrideBytes >= width);
	ocean_assert(rowConversionFunction != nullptr);
	ocean_assert(flag == CONVERT_NORMAL || flag == CONVERT_FLIPPED || targetReversePixelOrderInPlaceFunction != nullptr);

	ocean_assert(numberRows > 0u);
	ocean_assert(firstRow + numberRows <= height);

	switch (flag)
	{
		case CONVERT_NORMAL:
		case CONVERT_FLIPPED:
		{
			if (areContinuous && flag == CONVERT_NORMAL)
			{
				// special case, we can treat the rows within the image subset as one large row

				source += sourceStrideBytes * firstRow;
				target += targetStrideBytes * firstRow;

				rowConversionFunction(source, target, width * numberRows, options);
				return;
			}

			const int signedTargetStrideBytes = flag == CONVERT_NORMAL ? int(targetStrideBytes) : -int(targetStrideBytes);

			if (flag == CONVERT_FLIPPED)
			{
				target += (height - 1u) * targetStrideBytes;
			}

			source += sourceStrideBytes * firstRow;
			target += signedTargetStrideBytes * int(firstRow);

			for (unsigned int r = 0u; r < numberRows; ++r)
			{
				rowConversionFunction(source, target, width, options);

				source += sourceStrideBytes;
				target += signedTargetStrideBytes;
			}

			break;
		}

		case CONVERT_MIRRORED:
		case CONVERT_FLIPPED_AND_MIRRORED:
		{
			const int signedTargetStrideBytes = flag == CONVERT_MIRRORED ? int(targetStrideBytes) : -int(targetStrideBytes);

			if (flag == CONVERT_FLIPPED_AND_MIRRORED)
			{
				target += (height - 1u) * targetStrideBytes;
			}

			source += sourceStrideBytes * firstRow;
			target += signedTargetStrideBytes * int(firstRow);

			for (unsigned int r = 0u; r < numberRows; ++r)
			{
				rowConversionFunction(source, target, width, options);
				targetReversePixelOrderInPlaceFunction(target, width);

				source += sourceStrideBytes;
				target += signedTargetStrideBytes;
			}

			break;
		}

		default:
			ocean_assert(false && "Not supported flag!");
	}
}

void FrameConverter::convertArbitraryPixelFormatSubset(const void** sources, void** targets, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int multipleRowsPerIteration, const MultipleRowsConversionFunction multipleRowsConversionFunction, const void* options, const unsigned int firstMultipleRow, const unsigned int numberMultipleRows)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert_and_suppress_unused(multipleRowsPerIteration >= 1u, multipleRowsPerIteration);
	ocean_assert(multipleRowsConversionFunction != nullptr);

	ocean_assert((firstMultipleRow + numberMultipleRows) * multipleRowsPerIteration <= height);

	for (unsigned int r = firstMultipleRow; r < firstMultipleRow + numberMultipleRows; ++r)
	{
		multipleRowsConversionFunction(sources, targets, r, width, height, flag, options);
	}
}

void FrameConverter::convertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);
	ocean_assert(multipleRowIndex < height);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: targetPlanePaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
	// t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
	// t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)

	const unsigned int sourcePlane0PaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int sourcePlane1PaddingElements = (unsigned int)(intOptions[1]);
	const unsigned int targetPlanePaddingElements = (unsigned int)(intOptions[2]);

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);

	uint8_t* targetPlane = (uint8_t*)(targets[0]);

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width + sourcePlane1PaddingElements; // 2x2 downsampling but 2 channels
	const unsigned int targetPlaneStrideElements = width * 3u + targetPlanePaddingElements;

	const int factorChannel00_64 = intOptions[3];
	const int factorChannel10_64 = intOptions[4];
	const int factorChannel20_64 = intOptions[5];

	const int factorChannel01_64 = intOptions[6];
	const int factorChannel11_64 = intOptions[7];
	const int factorChannel21_64 = intOptions[8];

	const int factorChannel02_64 = intOptions[9];
	const int factorChannel12_64 = intOptions[10];
	const int factorChannel22_64 = intOptions[11];

	ocean_assert(std::abs(factorChannel00_64 + factorChannel01_64 + factorChannel02_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel10_64 + factorChannel11_64 + factorChannel12_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel20_64 + factorChannel21_64 + factorChannel22_64) < 64 * 4);

	const int bias0 = intOptions[12];
	const int bias1 = intOptions[13];
	const int bias2 = intOptions[14];

	ocean_assert(bias0 >= 0 && bias0 <= 128);
	ocean_assert(bias1 >= 0 && bias1 <= 128);
	ocean_assert(bias2 >= 0 && bias2 <= 128);

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	sourcePlane0 += multipleRowIndex * sourcePlane0StrideElements;
	sourcePlane1 += (multipleRowIndex / 2u) * sourcePlane1StrideElements;

	targetPlane = flipTarget ? (targetPlane + (height - multipleRowIndex - 1u) * targetPlaneStrideElements) : targetPlane + multipleRowIndex * targetPlaneStrideElements;

	const uint8_t* const sourcePlane0End = sourcePlane0 + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	if (blocks >= 1u)
	{
		const int16x8_t factorChannel00_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel00_64));
		const int16x8_t factorChannel10_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel10_64));
		const int16x8_t factorChannel20_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel20_64));

		const int16x8_t factorChannel01_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel01_64));
		const int16x8_t factorChannel11_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel11_64));
		const int16x8_t factorChannel21_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel21_64));

		const int16x8_t factorChannel02_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel02_64));
		const int16x8_t factorChannel12_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel12_64));
		const int16x8_t factorChannel22_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel22_64));

		const uint8x8_t biasChannel0_u_8x8 = vdup_n_u8((uint8_t)(bias0));
		const uint8x8_t biasChannel1_u_8x8 = vdup_n_u8((uint8_t)(bias1));
		const uint8x8_t biasChannel2_u_8x8 = vdup_n_u8((uint8_t)(bias2));

		for (size_t n = 0; n < blocks; ++n)
		{
			const uint8x16_t sourcePlane0_u_8x16 = vld1q_u8(sourcePlane0);
			const uint8x8x2_t sourcePlane1_u_8x8x2 = vld2_u8(sourcePlane1);

			// Y' = Y - bias0, U' = U - bias1, V' = V - bias2
			const int16x8_t source0_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(sourcePlane0_u_8x16), biasChannel0_u_8x8));
			const int16x8_t source0_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(sourcePlane0_u_8x16), biasChannel0_u_8x8));

			const int16x8_t source1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(sourcePlane1_u_8x8x2.val[0], biasChannel1_u_8x8));
			const int16x8_t source2_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(sourcePlane1_u_8x8x2.val[1], biasChannel2_u_8x8));

			// first we apply the 3x3 matrix multiplication for the second and third channel

			int16x8_t intermediateResults0_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel01_64_s_16x8);
			int16x8_t intermediateResults1_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel11_64_s_16x8);
			int16x8_t intermediateResults2_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel21_64_s_16x8);

			intermediateResults0_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8, vmulq_s16(source2_s_16x8, factorChannel02_64_s_16x8)); // intermediateResults0 = saturated(intermediateResults0 + source2_low * factorChannel02)
			intermediateResults1_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8, vmulq_s16(source2_s_16x8, factorChannel12_64_s_16x8));
			intermediateResults2_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8, vmulq_s16(source2_s_16x8, factorChannel22_64_s_16x8));

			// we up-sample the results for channel 2 and 3

			const int16x8x2_t intermediateResults0_s_16x8x2 = vzipq_s16(intermediateResults0_s_16x8, intermediateResults0_s_16x8);
			const int16x8x2_t intermediateResults1_s_16x8x2 = vzipq_s16(intermediateResults1_s_16x8, intermediateResults1_s_16x8);
			const int16x8x2_t intermediateResults2_s_16x8x2 = vzipq_s16(intermediateResults2_s_16x8, intermediateResults2_s_16x8);

			// now we multiply apply the 3x3 matrix multiplication

			const int16x8_t intermediateResults0_low_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8x2.val[0], vmulq_s16(source0_low_s_16x8, factorChannel00_64_s_16x8)); // intermediateResults0_low = saturated(intermediateResults0_low + source0_low * factorChannel00)
			const int16x8_t intermediateResults1_low_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8x2.val[0], vmulq_s16(source0_low_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2_low_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8x2.val[0], vmulq_s16(source0_low_s_16x8, factorChannel20_64_s_16x8));

			const int16x8_t intermediateResults0_high_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8x2.val[1], vmulq_s16(source0_high_s_16x8, factorChannel00_64_s_16x8)); // intermediateResults0_low = saturated(intermediateResults0_low + source0_low * factorChannel00)
			const int16x8_t intermediateResults1_high_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8x2.val[1], vmulq_s16(source0_high_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2_high_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8x2.val[1], vmulq_s16(source0_high_s_16x8, factorChannel20_64_s_16x8));

			uint8x16x3_t results_u_8x16x3;

			// saturated narrow signed to unsigned, normalized by 2^6
			results_u_8x16x3.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults0_high_s_16x8, 6));
			results_u_8x16x3.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults1_high_s_16x8, 6));
			results_u_8x16x3.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults2_high_s_16x8, 6));

			// and we can store the result
			vst3q_u8(targetPlane, results_u_8x16x3);

			sourcePlane0 += blockSize;
			sourcePlane1 += blockSize; // 2x2 downsampled, but two channels

			targetPlane += blockSize * 3u;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sourcePlane0 != sourcePlane0End)
	{
		ocean_assert(sourcePlane0 < sourcePlane0End);

		const int16_t source1 = int16_t(sourcePlane1[0] - int16_t(bias1));
		const int16_t source2 = int16_t(sourcePlane1[1] - int16_t(bias2));

		const int16_t intermediate0 = source1 * int16_t(factorChannel01_64) + source2 * int16_t(factorChannel02_64);
		const int16_t intermediate1 = source1 * int16_t(factorChannel11_64) + source2 * int16_t(factorChannel12_64);
		const int16_t intermediate2 = source1 * int16_t(factorChannel21_64) + source2 * int16_t(factorChannel22_64);

		targetPlane[0] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0[0] - int16_t(bias0)) * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlane[1] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0[0] - int16_t(bias0)) * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlane[2] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0[0] - int16_t(bias0)) * int16_t(factorChannel20_64) + intermediate2) / 64, 255));

		targetPlane[3] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0[1] - int16_t(bias0)) * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlane[4] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0[1] - int16_t(bias0)) * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlane[5] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0[1] - int16_t(bias0)) * int16_t(factorChannel20_64) + intermediate2) / 64, 255));

		sourcePlane0 += 2;
		sourcePlane1 += 2; // 2x2 downsampled, but two channels

		targetPlane += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetPlane - width * 3u, width);
	}
}

void FrameConverter::convertOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);
	ocean_assert(multipleRowIndex < height);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: targetPlanePaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

	const unsigned int sourcePlane0PaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int sourcePlane1PaddingElements = (unsigned int)(intOptions[1]);
	const unsigned int targetPlanePaddingElements = (unsigned int)(intOptions[2]);

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);

	uint8_t* targetPlane = (uint8_t*)(targets[0]);

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width + sourcePlane1PaddingElements; // 2x2 downsampling but 2 channels
	const unsigned int targetPlanetrideElements = width * 3u + targetPlanePaddingElements;

	const int factorChannel00_1024 = intOptions[3];
	const int factorChannel10_1024 = intOptions[4];
	const int factorChannel20_1024 = intOptions[5];

	const int factorChannel01_1024 = intOptions[6];
	const int factorChannel11_1024 = intOptions[7];
	const int factorChannel21_1024 = intOptions[8];

	const int factorChannel02_1024 = intOptions[9];
	const int factorChannel12_1024 = intOptions[10];
	const int factorChannel22_1024 = intOptions[11];

	const int bias0 = intOptions[12];
	const int bias1 = intOptions[13];
	const int bias2 = intOptions[14];

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	sourcePlane0 += multipleRowIndex * sourcePlane0StrideElements;
	sourcePlane1 += (multipleRowIndex / 2u) * sourcePlane1StrideElements;

	targetPlane = flipTarget ? (targetPlane + (height - multipleRowIndex - 1u) * targetPlanetrideElements) : targetPlane + multipleRowIndex * targetPlanetrideElements;

	const uint8_t* const sourcePlane0End = sourcePlane0 + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const int16x4_t factorChannel00_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel00_1024));
	const int16x4_t factorChannel10_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel10_1024));
	const int16x4_t factorChannel20_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel20_1024));

	const int16x4_t factorChannel01_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel01_1024));
	const int16x4_t factorChannel11_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel11_1024));
	const int16x4_t factorChannel21_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel21_1024));

	const int16x4_t factorChannel02_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel02_1024));
	const int16x4_t factorChannel12_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel12_1024));
	const int16x4_t factorChannel22_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel22_1024));

	const int32x4_t bias0_1024_s_32x4 = vdupq_n_s32(bias0 * 1024);
	const int32x4_t bias1_1024_s_32x4 = vdupq_n_s32(bias1 * 1024);
	const int32x4_t bias2_1024_s_32x4 = vdupq_n_s32(bias2 * 1024);

	const uint8x8_t mask_low_u_8x8 = vreinterpret_u8_u16(vdup_n_u16(0x00FFu));

	constexpr unsigned int blockSize = 8u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const int16x8_t sourcePlane0_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(sourcePlane0)));

		const uint8x8_t sourcePlane1_u_8x8 = vld1_u8(sourcePlane1);

		const int16x4_t sourcePlane0_low_s_16x4 = vget_low_s16(sourcePlane0_s_16x8);
		const int16x4_t sourcePlane0_high_s_16x4 = vget_high_s16(sourcePlane0_s_16x8);

		const uint8x8_t sourcePlane1_A_u_8x8 = vand_u8(sourcePlane1_u_8x8, mask_low_u_8x8);
		const uint8x8_t sourcePlane1_B_u_8x8 = vreinterpret_u8_u16(vshr_n_u16(vreinterpret_u16_u8(sourcePlane1_u_8x8), 8));
		const int16x4_t sourcePlane1_A_s_16x4 = vreinterpret_s16_u8(sourcePlane1_A_u_8x8);
		const int16x4_t sourcePlane1_B_s_16x4 = vreinterpret_s16_u8(sourcePlane1_B_u_8x8);


		// first, handling zipped part

		int32x4_t zippedIntermediateResults0_s_32x4 = vmlal_s16(bias0_1024_s_32x4, sourcePlane1_A_s_16x4, factorChannel01_1024_s_16x4); // f01 * zipped0 + b0
		int32x4_t zippedIntermediateResults1_s_32x4 = vmlal_s16(bias1_1024_s_32x4, sourcePlane1_A_s_16x4, factorChannel11_1024_s_16x4); // f11 * zipped0 + b1
		int32x4_t zippedIntermediateResults2_s_32x4 = vmlal_s16(bias2_1024_s_32x4, sourcePlane1_A_s_16x4, factorChannel21_1024_s_16x4); // f21 * zipped0 + b2

		zippedIntermediateResults0_s_32x4 = vmlal_s16(zippedIntermediateResults0_s_32x4, sourcePlane1_B_s_16x4, factorChannel02_1024_s_16x4); // += f02 * zipped1
		zippedIntermediateResults1_s_32x4 = vmlal_s16(zippedIntermediateResults1_s_32x4, sourcePlane1_B_s_16x4, factorChannel12_1024_s_16x4); // += f12 * zipped1
		zippedIntermediateResults2_s_32x4 = vmlal_s16(zippedIntermediateResults2_s_32x4, sourcePlane1_B_s_16x4, factorChannel22_1024_s_16x4); // += f22 * zipped1


		// now, handling plane part

		const int32x4_t sourcePlane0_Multiplied0_low_s_32x4 = vmull_s16(sourcePlane0_low_s_16x4, factorChannel00_1024_s_16x4); // f00 * plane
		const int32x4_t sourcePlane0_Multiplied0_high_s_32x4 = vmull_s16(sourcePlane0_high_s_16x4, factorChannel00_1024_s_16x4);

		const int32x4_t sourcePlane0_Multiplied1_low_s_32x4 = vmull_s16(sourcePlane0_low_s_16x4, factorChannel10_1024_s_16x4); // f10 * plane
		const int32x4_t sourcePlane0_Multiplied1_high_s_32x4 = vmull_s16(sourcePlane0_high_s_16x4, factorChannel10_1024_s_16x4);

		const int32x4_t sourcePlane0_Multiplied2_low_s_32x4 = vmull_s16(sourcePlane0_low_s_16x4, factorChannel20_1024_s_16x4); // f20 * plane
		const int32x4_t sourcePlane0_Multiplied2_high_s_32x4 = vmull_s16(sourcePlane0_high_s_16x4, factorChannel20_1024_s_16x4);


		// now, we can align zipped results with plane results

		const int32x4x2_t zippedIntermediateResults0_s_32x4x2 = vzipq_s32(zippedIntermediateResults0_s_32x4, zippedIntermediateResults0_s_32x4);
		const int32x4x2_t zippedIntermediateResults1_s_32x4x2 = vzipq_s32(zippedIntermediateResults1_s_32x4, zippedIntermediateResults1_s_32x4);
		const int32x4x2_t zippedIntermediateResults2_s_32x4x2 = vzipq_s32(zippedIntermediateResults2_s_32x4, zippedIntermediateResults2_s_32x4);


		// now, we can determine the upper results

		const int32x4_t resultUpper0_low_s_32x4 = vaddq_s32(zippedIntermediateResults0_s_32x4x2.val[0], sourcePlane0_Multiplied0_low_s_32x4);
		const int32x4_t resultUpper0_high_s_32x4 = vaddq_s32(zippedIntermediateResults0_s_32x4x2.val[1], sourcePlane0_Multiplied0_high_s_32x4);
		const int32x4_t resultUpper1_low_s_32x4 = vaddq_s32(zippedIntermediateResults1_s_32x4x2.val[0], sourcePlane0_Multiplied1_low_s_32x4);
		const int32x4_t resultUpper1_high_s_32x4 = vaddq_s32(zippedIntermediateResults1_s_32x4x2.val[1], sourcePlane0_Multiplied1_high_s_32x4);
		const int32x4_t resultUpper2_low_s_32x4 = vaddq_s32(zippedIntermediateResults2_s_32x4x2.val[0], sourcePlane0_Multiplied2_low_s_32x4);
		const int32x4_t resultUpper2_high_s_32x4 = vaddq_s32(zippedIntermediateResults2_s_32x4x2.val[1], sourcePlane0_Multiplied2_high_s_32x4);

		uint8x8x3_t resultsUpper_u_8x8x3;

		// saturated narrow signed to unsigned
		resultsUpper_u_8x8x3.val[0] = vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper0_low_s_32x4, 10), vqrshrun_n_s32(resultUpper0_high_s_32x4, 10)));
		resultsUpper_u_8x8x3.val[1] = vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper1_low_s_32x4, 10), vqrshrun_n_s32(resultUpper1_high_s_32x4, 10)));
		resultsUpper_u_8x8x3.val[2] = vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper2_low_s_32x4, 10), vqrshrun_n_s32(resultUpper2_high_s_32x4, 10)));

		// and we can store the result
		vst3_u8(targetPlane, resultsUpper_u_8x8x3);

		sourcePlane0 += blockSize;
		sourcePlane1 += blockSize; // 2x2 downsampled, but two channels

		targetPlane += blockSize * 3u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sourcePlane0 != sourcePlane0End)
	{
		ocean_assert(sourcePlane0 < sourcePlane0End);

		const int zipped0 = sourcePlane1[0] * int16_t(factorChannel01_1024) + sourcePlane1[1] * int16_t(factorChannel02_1024);
		const int zipped1 = sourcePlane1[0] * int16_t(factorChannel11_1024) + sourcePlane1[1] * int16_t(factorChannel12_1024);
		const int zipped2 = sourcePlane1[0] * int16_t(factorChannel21_1024) + sourcePlane1[1] * int16_t(factorChannel22_1024);

		// first and second upper pixel

		targetPlane[0] = (uint8_t)(minmax<int>(0, (sourcePlane0[0] * int16_t(factorChannel00_1024) + zipped0) / 1024 + int16_t(bias0), 255));
		targetPlane[1] = (uint8_t)(minmax<int>(0, (sourcePlane0[0] * int16_t(factorChannel10_1024) + zipped1) / 1024 + int16_t(bias1), 255));
		targetPlane[2] = (uint8_t)(minmax<int>(0, (sourcePlane0[0] * int16_t(factorChannel20_1024) + zipped2) / 1024 + int16_t(bias2), 255));

		targetPlane[3] = (uint8_t)(minmax<int>(0, (sourcePlane0[1] * int16_t(factorChannel00_1024) + zipped0) / 1024 + int16_t(bias0), 255));
		targetPlane[4] = (uint8_t)(minmax<int>(0, (sourcePlane0[1] * int16_t(factorChannel10_1024) + zipped1) / 1024 + int16_t(bias1), 255));
		targetPlane[5] = (uint8_t)(minmax<int>(0, (sourcePlane0[1] * int16_t(factorChannel20_1024) + zipped2) / 1024 + int16_t(bias2), 255));

		sourcePlane0 += 2;
		sourcePlane1 += 2; // 2x2 downsampled, but two channels

		targetPlane += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetPlane - width * 3u, width);
	}
}

void FrameConverter::convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);
	ocean_assert(multipleRowIndex < height);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: targetPlanePaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = clamp(0, f00 * (s0 - b0) + f01 * (s1 - b1) + f02 * (s2 - b2), 255)
	// t1 = clamp(0, f10 * (s0 - b0) + f11 * (s1 - b1) + f12 * (s2 - b2), 255)
	// t2 = clamp(0, f20 * (s0 - b0) + f21 * (s1 - b1) + f22 * (s2 - b2), 255)

	const unsigned int sourcePlane0PaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int sourcePlane1PaddingElements = (unsigned int)(intOptions[1]);
	const unsigned int targetPlanePaddingElements = (unsigned int)(intOptions[2]);

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);

	uint8_t* targetPlane = (uint8_t*)(targets[0]);

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width + sourcePlane1PaddingElements; // 2x2 downsampling but 2 channels
	const unsigned int targetPlaneStrideElements = width * 3u + targetPlanePaddingElements;

	const int factorChannel00_64 = intOptions[3];
	const int factorChannel10_64 = intOptions[4];
	const int factorChannel20_64 = intOptions[5];

	const int factorChannel01_64 = intOptions[6];
	const int factorChannel11_64 = intOptions[7];
	const int factorChannel21_64 = intOptions[8];

	const int factorChannel02_64 = intOptions[9];
	const int factorChannel12_64 = intOptions[10];
	const int factorChannel22_64 = intOptions[11];

	ocean_assert(std::abs(factorChannel00_64 + factorChannel01_64 + factorChannel02_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel10_64 + factorChannel11_64 + factorChannel12_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel20_64 + factorChannel21_64 + factorChannel22_64) < 64 * 4);

	const int bias0 = intOptions[12];
	const int bias1 = intOptions[13];
	const int bias2 = intOptions[14];

	ocean_assert(bias0 >= 0 && bias0 <= 128);
	ocean_assert(bias1 >= 0 && bias1 <= 128);
	ocean_assert(bias2 >= 0 && bias2 <= 128);

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* sourcePlane0Upper = sourcePlane0 + multipleRowIndex * 2u * sourcePlane0StrideElements;
	sourcePlane1 += multipleRowIndex * sourcePlane1StrideElements;

	uint8_t* targetPlaneUpper = flipTarget ? (targetPlane + (height - multipleRowIndex * 2u - 1u) * targetPlaneStrideElements) : targetPlane + multipleRowIndex * 2u * targetPlaneStrideElements;
	uint8_t* targetPlaneLower = flipTarget ? targetPlaneUpper - targetPlaneStrideElements : targetPlaneUpper + targetPlaneStrideElements;

	const uint8_t* const sPlaneUpperEnd = sourcePlane0Upper + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	if (blocks >= 1u)
	{
		const int16x8_t factorChannel00_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel00_64));
		const int16x8_t factorChannel10_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel10_64));
		const int16x8_t factorChannel20_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel20_64));

		const int16x8_t factorChannel01_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel01_64));
		const int16x8_t factorChannel11_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel11_64));
		const int16x8_t factorChannel21_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel21_64));

		const int16x8_t factorChannel02_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel02_64));
		const int16x8_t factorChannel12_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel12_64));
		const int16x8_t factorChannel22_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel22_64));

		const uint8x8_t biasChannel0_u_8x8 = vdup_n_u8((uint8_t)(bias0));
		const uint8x8_t biasChannel1_u_8x8 = vdup_n_u8((uint8_t)(bias1));
		const uint8x8_t biasChannel2_u_8x8 = vdup_n_u8((uint8_t)(bias2));

		for (size_t n = 0; n < blocks; ++n)
		{
			const uint8x16_t sourcePlane0_Upper_s_8x16 = vld1q_u8(sourcePlane0Upper);
			const uint8x16_t sourcePlane0_Lower_s_8x16 = vld1q_u8(sourcePlane0Upper + sourcePlane0StrideElements);

			const uint8x8x2_t sourcePlane1_u_8x8x2 = vld2_u8(sourcePlane1);


			// Y' = Y - bias0, U' = U - bias1, V' = V - bias2
			const int16x8_t source0_Upper_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(sourcePlane0_Upper_s_8x16), biasChannel0_u_8x8));
			const int16x8_t source0_Upper_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(sourcePlane0_Upper_s_8x16), biasChannel0_u_8x8));

			const int16x8_t source0_Lower_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(sourcePlane0_Lower_s_8x16), biasChannel0_u_8x8));
			const int16x8_t source0_Lower_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(sourcePlane0_Lower_s_8x16), biasChannel0_u_8x8));

			const int16x8_t source1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(sourcePlane1_u_8x8x2.val[0], biasChannel1_u_8x8));
			const int16x8_t source2_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(sourcePlane1_u_8x8x2.val[1], biasChannel2_u_8x8));


			// first we apply the 3x3 matrix multiplication for the second and third channel

			int16x8_t intermediateResults0_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel01_64_s_16x8);
			int16x8_t intermediateResults1_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel11_64_s_16x8);
			int16x8_t intermediateResults2_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel21_64_s_16x8);

			intermediateResults0_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8, vmulq_s16(source2_s_16x8, factorChannel02_64_s_16x8)); // intermediateResults0 = saturated(intermediateResults0 + source2_low * factorChannel02)
			intermediateResults1_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8, vmulq_s16(source2_s_16x8, factorChannel12_64_s_16x8));
			intermediateResults2_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8, vmulq_s16(source2_s_16x8, factorChannel22_64_s_16x8));


			// we up-sample the results for channel 2 and 3

			const int16x8x2_t intermediateResults0_s_16x8x2 = vzipq_s16(intermediateResults0_s_16x8, intermediateResults0_s_16x8);
			const int16x8x2_t intermediateResults1_s_16x8x2 = vzipq_s16(intermediateResults1_s_16x8, intermediateResults1_s_16x8);
			const int16x8x2_t intermediateResults2_s_16x8x2 = vzipq_s16(intermediateResults2_s_16x8, intermediateResults2_s_16x8);


			// now we multiply apply the 3x3 matrix multiplication

			const int16x8_t intermediateResults0_Upper_low_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8x2.val[0], vmulq_s16(source0_Upper_low_s_16x8, factorChannel00_64_s_16x8)); // intermediateResults0_low = saturated(intermediateResults0_low + source0_low * factorChannel00)
			const int16x8_t intermediateResults1_Upper_low_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8x2.val[0], vmulq_s16(source0_Upper_low_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2_Upper_low_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8x2.val[0], vmulq_s16(source0_Upper_low_s_16x8, factorChannel20_64_s_16x8));

			const int16x8_t intermediateResults0_Upper_high_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8x2.val[1], vmulq_s16(source0_Upper_high_s_16x8, factorChannel00_64_s_16x8)); // intermediateResults0_low = saturated(intermediateResults0_low + source0_low * factorChannel00)
			const int16x8_t intermediateResults1_Upper_high_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8x2.val[1], vmulq_s16(source0_Upper_high_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2_Upper_high_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8x2.val[1], vmulq_s16(source0_Upper_high_s_16x8, factorChannel20_64_s_16x8));


			const int16x8_t intermediateResults0_Lower_low_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8x2.val[0], vmulq_s16(source0_Lower_low_s_16x8, factorChannel00_64_s_16x8)); // intermediateResults0_low = saturated(intermediateResults0_low + source0_low * factorChannel00)
			const int16x8_t intermediateResults1_Lower_low_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8x2.val[0], vmulq_s16(source0_Lower_low_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2_Lower_low_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8x2.val[0], vmulq_s16(source0_Lower_low_s_16x8, factorChannel20_64_s_16x8));

			const int16x8_t intermediateResults0_Lower_high_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8x2.val[1], vmulq_s16(source0_Lower_high_s_16x8, factorChannel00_64_s_16x8)); // intermediateResults0_low = saturated(intermediateResults0_low + source0_low * factorChannel00)
			const int16x8_t intermediateResults1_Lower_high_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8x2.val[1], vmulq_s16(source0_Lower_high_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2_Lower_high_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8x2.val[1], vmulq_s16(source0_Lower_high_s_16x8, factorChannel20_64_s_16x8));


			// saturated narrow signed to unsigned, normalized by 2^6

			uint8x16x3_t results_Upper_u_8x16x3;
			results_Upper_u_8x16x3.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0_Upper_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults0_Upper_high_s_16x8, 6));
			results_Upper_u_8x16x3.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1_Upper_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults1_Upper_high_s_16x8, 6));
			results_Upper_u_8x16x3.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2_Upper_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults2_Upper_high_s_16x8, 6));

			uint8x16x3_t results_Lower_u_8x16x3;
			results_Lower_u_8x16x3.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0_Lower_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults0_Lower_high_s_16x8, 6));
			results_Lower_u_8x16x3.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1_Lower_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults1_Lower_high_s_16x8, 6));
			results_Lower_u_8x16x3.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2_Lower_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults2_Lower_high_s_16x8, 6));


			// and we can store the result
			vst3q_u8(targetPlaneUpper, results_Upper_u_8x16x3);
			vst3q_u8(targetPlaneLower, results_Lower_u_8x16x3);

			sourcePlane0Upper += blockSize;
			sourcePlane1 += blockSize; // 2x2 downsampled, but two channels

			targetPlaneUpper += blockSize * 3u;
			targetPlaneLower += blockSize * 3u;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sourcePlane0Upper != sPlaneUpperEnd)
	{
		ocean_assert(sourcePlane0Upper < sPlaneUpperEnd);

		const int16_t source1 = int16_t(sourcePlane1[0] - int16_t(bias1));
		const int16_t source2 = int16_t(sourcePlane1[1] - int16_t(bias2));

		const int16_t intermediate0 = source1 * int16_t(factorChannel01_64) + source2 * int16_t(factorChannel02_64);
		const int16_t intermediate1 = source1 * int16_t(factorChannel11_64) + source2 * int16_t(factorChannel12_64);
		const int16_t intermediate2 = source1 * int16_t(factorChannel21_64) + source2 * int16_t(factorChannel22_64);

		// first and second upper pixel

		targetPlaneUpper[0] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0Upper[0] - int16_t(bias0)) * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlaneUpper[1] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0Upper[0] - int16_t(bias0)) * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlaneUpper[2] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0Upper[0] - int16_t(bias0)) * int16_t(factorChannel20_64) + intermediate2) / 64, 255));

		targetPlaneUpper[3] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0Upper[1] - int16_t(bias0)) * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlaneUpper[4] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0Upper[1] - int16_t(bias0)) * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlaneUpper[5] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0Upper[1] - int16_t(bias0)) * int16_t(factorChannel20_64) + intermediate2) / 64, 255));


		// first and second lower pixel

		targetPlaneLower[0] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0Upper[sourcePlane0StrideElements + 0] - int16_t(bias0)) * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlaneLower[1] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0Upper[sourcePlane0StrideElements + 0] - int16_t(bias0)) * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlaneLower[2] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0Upper[sourcePlane0StrideElements + 0] - int16_t(bias0)) * int16_t(factorChannel20_64) + intermediate2) / 64, 255));

		targetPlaneLower[3] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0Upper[sourcePlane0StrideElements + 1] - int16_t(bias0)) * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlaneLower[4] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0Upper[sourcePlane0StrideElements + 1] - int16_t(bias0)) * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlaneLower[5] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane0Upper[sourcePlane0StrideElements + 1] - int16_t(bias0)) * int16_t(factorChannel20_64) + intermediate2) / 64, 255));

		sourcePlane0Upper += 2;
		sourcePlane1 += 2; // 2x2 downsampled, but two channels

		targetPlaneUpper += 2u * 3u;
		targetPlaneLower += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetPlaneUpper - width * 3u, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetPlaneLower - width * 3u, width);
	}
}

void FrameConverter::convertTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: targetPlanePaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

	const unsigned int sourcePlane0PaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int sourcePlane1PaddingElements = (unsigned int)(intOptions[1]);
	const unsigned int targetPlanePaddingElements = (unsigned int)(intOptions[2]);

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);

	uint8_t* targetPlane = (uint8_t*)(targets[0]);

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width + sourcePlane1PaddingElements; // 2x2 downsampling but 2 channels
	const unsigned int targetPlaneStrideElements = width * 3u + targetPlanePaddingElements;

	const int factorChannel00_1024 = intOptions[3];
	const int factorChannel10_1024 = intOptions[4];
	const int factorChannel20_1024 = intOptions[5];

	const int factorChannel01_1024 = intOptions[6];
	const int factorChannel11_1024 = intOptions[7];
	const int factorChannel21_1024 = intOptions[8];

	const int factorChannel02_1024 = intOptions[9];
	const int factorChannel12_1024 = intOptions[10];
	const int factorChannel22_1024 = intOptions[11];

	const int bias0 = intOptions[12];
	const int bias1 = intOptions[13];
	const int bias2 = intOptions[14];

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* sourcePlane0Upper = sourcePlane0 + multipleRowIndex * 2u * sourcePlane0StrideElements;
	sourcePlane1 += multipleRowIndex * sourcePlane1StrideElements;

	uint8_t* targetPlaneUpper = flipTarget ? (targetPlane + (height - multipleRowIndex * 2u - 1u) * targetPlaneStrideElements) : targetPlane + multipleRowIndex * 2u * targetPlaneStrideElements;
	uint8_t* targetPlaneLower = flipTarget ? targetPlaneUpper - targetPlaneStrideElements : targetPlaneUpper + targetPlaneStrideElements;

	const uint8_t* const sourcePlane0UpperEnd = sourcePlane0Upper + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const int16x4_t factorChannel00_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel00_1024));
	const int16x4_t factorChannel10_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel10_1024));
	const int16x4_t factorChannel20_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel20_1024));

	const int16x4_t factorChannel01_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel01_1024));
	const int16x4_t factorChannel11_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel11_1024));
	const int16x4_t factorChannel21_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel21_1024));

	const int16x4_t factorChannel02_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel02_1024));
	const int16x4_t factorChannel12_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel12_1024));
	const int16x4_t factorChannel22_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel22_1024));

	const int32x4_t bias0_1024_s_32x4 = vdupq_n_s32(bias0 * 1024);
	const int32x4_t bias1_1024_s_32x4 = vdupq_n_s32(bias1 * 1024);
	const int32x4_t bias2_1024_s_32x4 = vdupq_n_s32(bias2 * 1024);

	const uint8x8_t mask_low_u_8x8 = vreinterpret_u8_u16(vdup_n_u16(0x00FFu));

	constexpr unsigned int blockSize = 8u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const int16x8_t sourcePlane0_Upper_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(sourcePlane0Upper)));
		const int16x8_t sourcePlane0_Lower_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(sourcePlane0Upper + sourcePlane0StrideElements)));

		const uint8x8_t sourcePlane1_u_8x8 = vld1_u8(sourcePlane1);

		const int16x4_t sourcePlane0_Upper_low_s_16x4 = vget_low_s16(sourcePlane0_Upper_s_16x8);
		const int16x4_t sourcePlane0_Upper_high_s_16x4 = vget_high_s16(sourcePlane0_Upper_s_16x8);
		const int16x4_t sourcePlane0_Lower_low_s_16x4 = vget_low_s16(sourcePlane0_Lower_s_16x8);
		const int16x4_t sourcePlane0_Lower_high_s_16x4 = vget_high_s16(sourcePlane0_Lower_s_16x8);

		const uint8x8_t sourcePlane1_A_u_8x8 = vand_u8(sourcePlane1_u_8x8, mask_low_u_8x8);
		const uint8x8_t sourcePlane1_B_u_8x8 = vreinterpret_u8_u16(vshr_n_u16(vreinterpret_u16_u8(sourcePlane1_u_8x8), 8));
		const int16x4_t sourcePlane1_A_s_16x4 = vreinterpret_s16_u8(sourcePlane1_A_u_8x8);
		const int16x4_t sourcePlane1_B_s_16x4 = vreinterpret_s16_u8(sourcePlane1_B_u_8x8);


		// first, handling 2-channel part of the second plane

		int32x4_t plane1IntermediateResults0_s_32x4 = vmlal_s16(bias0_1024_s_32x4, sourcePlane1_A_s_16x4, factorChannel01_1024_s_16x4); // f01 * plane1_A + b0
		int32x4_t plane1IntermediateResults1_s_32x4 = vmlal_s16(bias1_1024_s_32x4, sourcePlane1_A_s_16x4, factorChannel11_1024_s_16x4); // f11 * plane1_A + b1
		int32x4_t plane1IntermediateResults2_s_32x4 = vmlal_s16(bias2_1024_s_32x4, sourcePlane1_A_s_16x4, factorChannel21_1024_s_16x4); // f21 * plane1_A + b2

		plane1IntermediateResults0_s_32x4 = vmlal_s16(plane1IntermediateResults0_s_32x4, sourcePlane1_B_s_16x4, factorChannel02_1024_s_16x4); // += f02 * plane1_B
		plane1IntermediateResults1_s_32x4 = vmlal_s16(plane1IntermediateResults1_s_32x4, sourcePlane1_B_s_16x4, factorChannel12_1024_s_16x4); // += f12 * plane1_B
		plane1IntermediateResults2_s_32x4 = vmlal_s16(plane1IntermediateResults2_s_32x4, sourcePlane1_B_s_16x4, factorChannel22_1024_s_16x4); // += f22 * plane1_B


		// now, handling the 1-channel part of the first plane

		const int32x4_t plane0UpperMultiplied0_low_s_32x4 = vmull_s16(sourcePlane0_Upper_low_s_16x4, factorChannel00_1024_s_16x4); // f00 * plane
		const int32x4_t plane0UpperMultiplied0_high_s_32x4 = vmull_s16(sourcePlane0_Upper_high_s_16x4, factorChannel00_1024_s_16x4);
		const int32x4_t plane0LowerMultiplied0_low_s_32x4 = vmull_s16(sourcePlane0_Lower_low_s_16x4, factorChannel00_1024_s_16x4);
		const int32x4_t plane0LowerMultiplied0_high_s_32x4 = vmull_s16(sourcePlane0_Lower_high_s_16x4, factorChannel00_1024_s_16x4);

		const int32x4_t plane0UpperMultiplied1_low_s_32x4 = vmull_s16(sourcePlane0_Upper_low_s_16x4, factorChannel10_1024_s_16x4); // f10 * plane
		const int32x4_t plane0UpperMultiplied1_high_s_32x4 = vmull_s16(sourcePlane0_Upper_high_s_16x4, factorChannel10_1024_s_16x4);
		const int32x4_t plane0LowerMultiplied1_low_s_32x4 = vmull_s16(sourcePlane0_Lower_low_s_16x4, factorChannel10_1024_s_16x4);
		const int32x4_t plane0LowerMultiplied1_high_s_32x4 = vmull_s16(sourcePlane0_Lower_high_s_16x4, factorChannel10_1024_s_16x4);

		const int32x4_t plane0UpperMultiplied2_low_s_32x4 = vmull_s16(sourcePlane0_Upper_low_s_16x4, factorChannel20_1024_s_16x4); // f20 * plane
		const int32x4_t plane0UpperMultiplied2_high_s_32x4 = vmull_s16(sourcePlane0_Upper_high_s_16x4, factorChannel20_1024_s_16x4);
		const int32x4_t plane0LowerMultiplied2_low_s_32x4 = vmull_s16(sourcePlane0_Lower_low_s_16x4, factorChannel20_1024_s_16x4);
		const int32x4_t plane0LowerMultiplied2_high_s_32x4 = vmull_s16(sourcePlane0_Lower_high_s_16x4, factorChannel20_1024_s_16x4);


		// now, we can align the 2-channel results of the second plane with the 1-channel results of the first plane

		const int32x4x2_t plane1IntermediateResults0_s_32x4x2 = vzipq_s32(plane1IntermediateResults0_s_32x4, plane1IntermediateResults0_s_32x4);
		const int32x4x2_t plane1IntermediateResults1_s_32x4x2 = vzipq_s32(plane1IntermediateResults1_s_32x4, plane1IntermediateResults1_s_32x4);
		const int32x4x2_t plane1IntermediateResults2_s_32x4x2 = vzipq_s32(plane1IntermediateResults2_s_32x4, plane1IntermediateResults2_s_32x4);


		// now, we can determine the upper results

		const int32x4_t resultUpper0_low_s_32x4 = vaddq_s32(plane1IntermediateResults0_s_32x4x2.val[0], plane0UpperMultiplied0_low_s_32x4);
		const int32x4_t resultUpper0_high_s_32x4 = vaddq_s32(plane1IntermediateResults0_s_32x4x2.val[1], plane0UpperMultiplied0_high_s_32x4);
		const int32x4_t resultUpper1_low_s_32x4 = vaddq_s32(plane1IntermediateResults1_s_32x4x2.val[0], plane0UpperMultiplied1_low_s_32x4);
		const int32x4_t resultUpper1_high_s_32x4 = vaddq_s32(plane1IntermediateResults1_s_32x4x2.val[1], plane0UpperMultiplied1_high_s_32x4);
		const int32x4_t resultUpper2_low_s_32x4 = vaddq_s32(plane1IntermediateResults2_s_32x4x2.val[0], plane0UpperMultiplied2_low_s_32x4);
		const int32x4_t resultUpper2_high_s_32x4 = vaddq_s32(plane1IntermediateResults2_s_32x4x2.val[1], plane0UpperMultiplied2_high_s_32x4);

		uint8x8x3_t resultsUpper_u_8x8x3;

		// saturated narrow signed to unsigned
		resultsUpper_u_8x8x3.val[0] = vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper0_low_s_32x4, 10), vqrshrun_n_s32(resultUpper0_high_s_32x4, 10)));
		resultsUpper_u_8x8x3.val[1] = vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper1_low_s_32x4, 10), vqrshrun_n_s32(resultUpper1_high_s_32x4, 10)));
		resultsUpper_u_8x8x3.val[2] = vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper2_low_s_32x4, 10), vqrshrun_n_s32(resultUpper2_high_s_32x4, 10)));

		// and we can store the result
		vst3_u8(targetPlaneUpper, resultsUpper_u_8x8x3);


		// now, we can determine the lower results

		const int32x4_t resultLower0_low_s_32x4 = vaddq_s32(plane1IntermediateResults0_s_32x4x2.val[0], plane0LowerMultiplied0_low_s_32x4);
		const int32x4_t resultLower0_high_s_32x4 = vaddq_s32(plane1IntermediateResults0_s_32x4x2.val[1], plane0LowerMultiplied0_high_s_32x4);
		const int32x4_t resultLower1_low_s_32x4 = vaddq_s32(plane1IntermediateResults1_s_32x4x2.val[0], plane0LowerMultiplied1_low_s_32x4);
		const int32x4_t resultLower1_high_s_32x4 = vaddq_s32(plane1IntermediateResults1_s_32x4x2.val[1], plane0LowerMultiplied1_high_s_32x4);
		const int32x4_t resultLower2_low_s_32x4 = vaddq_s32(plane1IntermediateResults2_s_32x4x2.val[0], plane0LowerMultiplied2_low_s_32x4);
		const int32x4_t resultLower2_high_s_32x4 = vaddq_s32(plane1IntermediateResults2_s_32x4x2.val[1], plane0LowerMultiplied2_high_s_32x4);

		uint8x8x3_t resultsLower_u_8x8x3;

		// saturated narrow signed to unsigned
		resultsLower_u_8x8x3.val[0] = vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultLower0_low_s_32x4, 10), vqrshrun_n_s32(resultLower0_high_s_32x4, 10)));
		resultsLower_u_8x8x3.val[1] = vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultLower1_low_s_32x4, 10), vqrshrun_n_s32(resultLower1_high_s_32x4, 10)));
		resultsLower_u_8x8x3.val[2] = vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultLower2_low_s_32x4, 10), vqrshrun_n_s32(resultLower2_high_s_32x4, 10)));

		// and we can store the result
		vst3_u8(targetPlaneLower, resultsLower_u_8x8x3);

		sourcePlane0Upper += blockSize;
		sourcePlane1 += blockSize; // 2x2 downsampled, but two channels

		targetPlaneUpper += blockSize * 3u;
		targetPlaneLower += blockSize * 3u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sourcePlane0Upper != sourcePlane0UpperEnd)
	{
		ocean_assert(sourcePlane0Upper < sourcePlane0UpperEnd);

		const int intermediate0 = sourcePlane1[0] * int16_t(factorChannel01_1024) + sourcePlane1[1] * int16_t(factorChannel02_1024);
		const int intermediate1 = sourcePlane1[0] * int16_t(factorChannel11_1024) + sourcePlane1[1] * int16_t(factorChannel12_1024);
		const int intermediate2 = sourcePlane1[0] * int16_t(factorChannel21_1024) + sourcePlane1[1] * int16_t(factorChannel22_1024);

		// first and second upper pixel

		targetPlaneUpper[0] = (uint8_t)(minmax<int>(0, (sourcePlane0Upper[0] * int16_t(factorChannel00_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		targetPlaneUpper[1] = (uint8_t)(minmax<int>(0, (sourcePlane0Upper[0] * int16_t(factorChannel10_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		targetPlaneUpper[2] = (uint8_t)(minmax<int>(0, (sourcePlane0Upper[0] * int16_t(factorChannel20_1024) + intermediate2) / 1024 + int16_t(bias2), 255));

		targetPlaneUpper[3] = (uint8_t)(minmax<int>(0, (sourcePlane0Upper[1] * int16_t(factorChannel00_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		targetPlaneUpper[4] = (uint8_t)(minmax<int>(0, (sourcePlane0Upper[1] * int16_t(factorChannel10_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		targetPlaneUpper[5] = (uint8_t)(minmax<int>(0, (sourcePlane0Upper[1] * int16_t(factorChannel20_1024) + intermediate2) / 1024 + int16_t(bias2), 255));


		// first and second lower pixel

		targetPlaneLower[0] = (uint8_t)(minmax<int>(0, (sourcePlane0Upper[sourcePlane0StrideElements + 0] * int16_t(factorChannel00_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		targetPlaneLower[1] = (uint8_t)(minmax<int>(0, (sourcePlane0Upper[sourcePlane0StrideElements + 0] * int16_t(factorChannel10_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		targetPlaneLower[2] = (uint8_t)(minmax<int>(0, (sourcePlane0Upper[sourcePlane0StrideElements + 0] * int16_t(factorChannel20_1024) + intermediate2) / 1024 + int16_t(bias2), 255));

		targetPlaneLower[3] = (uint8_t)(minmax<int>(0, (sourcePlane0Upper[sourcePlane0StrideElements + 1] * int16_t(factorChannel00_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		targetPlaneLower[4] = (uint8_t)(minmax<int>(0, (sourcePlane0Upper[sourcePlane0StrideElements + 1] * int16_t(factorChannel10_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		targetPlaneLower[5] = (uint8_t)(minmax<int>(0, (sourcePlane0Upper[sourcePlane0StrideElements + 1] * int16_t(factorChannel20_1024) + intermediate2) / 1024 + int16_t(bias2), 255));

		sourcePlane0Upper += 2;
		sourcePlane1 += 2; // 2x2 downsampled, but two channels

		targetPlaneUpper += 2u * 3u;
		targetPlaneLower += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetPlaneUpper - width * 3u, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetPlaneLower - width * 3u, width);
	}
}

void FrameConverter::convertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlanePaddingElements
	// uint32_t: targetPlane0PaddingElements
	// uint32_t: targetPlane1PaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = clamp(0, f00 * s0 + f01 * s1 + f02 * s2 + b0, 255)
	// t1 = clamp(0, f10 * s0 + f11 * s1 + f12 * s2 + b1, 255)
	// t2 = clamp(0, f20 * s0 + f21 * s1 + f22 * s2 + b2, 255)

	const unsigned int sourcePlanePaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int targetPlane0PaddingElements = (unsigned int)(intOptions[1]);
	const unsigned int targetPlane1PaddingElements = (unsigned int)(intOptions[2]);

	const uint8_t* sourcePlane = (const uint8_t*)(sources[0]);

	uint8_t* targetPlane0 = (uint8_t*)(targets[0]);
	uint8_t* targetPlane1 = (uint8_t*)(targets[1]);

	const unsigned int sourcePlaneStrideElements = width * 3u + sourcePlanePaddingElements;
	const unsigned int targetPlane0StrideElements = width + targetPlane0PaddingElements;
	const unsigned int targetPlane1StrideElements = width + targetPlane1PaddingElements; // 2x2 downsampling but 2 channels

	const int16_t factorChannel00_128 = int16_t(intOptions[3]);
	const int16_t factorChannel10_128 = int16_t(intOptions[4]);
	const int16_t factorChannel20_128 = int16_t(intOptions[5]);

	const int16_t factorChannel01_128 = int16_t(intOptions[6]);
	const int16_t factorChannel11_128 = int16_t(intOptions[7]);
	const int16_t factorChannel21_128 = int16_t(intOptions[8]);

	const int16_t factorChannel02_128 = int16_t(intOptions[9]);
	const int16_t factorChannel12_128 = int16_t(intOptions[10]);
	const int16_t factorChannel22_128 = int16_t(intOptions[11]);

	ocean_assert(std::abs(factorChannel00_128 + factorChannel01_128 + factorChannel02_128) <= 128);
	ocean_assert(std::abs(factorChannel10_128 + factorChannel11_128 + factorChannel12_128) <= 128);
	ocean_assert(std::abs(factorChannel20_128 + factorChannel21_128 + factorChannel22_128) <= 128);

	const int16_t bias0 = int16_t(intOptions[12]);
	const int16_t bias1 = int16_t(intOptions[13]);
	const int16_t bias2 = int16_t(intOptions[14]);

	ocean_assert(bias0 >= -128 && bias0 <= 128);
	ocean_assert(bias1 >= -128 && bias1 <= 128);
	ocean_assert(bias2 >= -128 && bias2 <= 128);

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	sourcePlane += multipleRowIndex * 2u * sourcePlaneStrideElements; // upper row

	uint8_t* targetPlane0Upper = flipTarget ? (targetPlane0 + (height - multipleRowIndex * 2u - 1u) * targetPlane0StrideElements) : targetPlane0 + multipleRowIndex * 2u * targetPlane0StrideElements;
	uint8_t* targetPlane0Lower = flipTarget ? targetPlane0Upper - targetPlane0StrideElements : targetPlane0Upper + targetPlane0StrideElements;
	targetPlane1 = flipTarget ? (targetPlane1 + (height / 2u - multipleRowIndex - 1u) * targetPlane1StrideElements) : targetPlane1 + multipleRowIndex * targetPlane1StrideElements;

	const uint8_t* const sourcePlaneEnd = sourcePlane + width * 3u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	if (blocks >= 1u)
	{
		const int16x8_t biasChannel0_s_16x8 = vdupq_n_s16(bias0);
		const int16x8_t biasChannel1_s_16x8 = vdupq_n_s16(bias1);
		const int16x8_t biasChannel2_s_16x8 = vdupq_n_s16(bias2);

		for (size_t n = 0; n < blocks; ++n)
		{
			// let's handle the upper row

			const uint8x16x3_t sourcePlane_Upper_u_8x16x3 = vld3q_u8(sourcePlane);

			const uint16x8_t sourcePlaneAverage_0_Upper_u_16x8 = vpaddlq_u8(sourcePlane_Upper_u_8x16x3.val[0]);
			const uint16x8_t sourcePlaneAverage_1_Upper_u_16x8 = vpaddlq_u8(sourcePlane_Upper_u_8x16x3.val[1]);
			const uint16x8_t sourcePlaneAverage_2_Upper_u_16x8 = vpaddlq_u8(sourcePlane_Upper_u_8x16x3.val[2]);

			const int16x8_t sourcePlane_0A_Upper_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sourcePlane_Upper_u_8x16x3.val[0])));
			const int16x8_t sourcePlane_0B_Upper_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sourcePlane_Upper_u_8x16x3.val[0])));

			const int16x8_t sourcePlane_1A_Upper_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sourcePlane_Upper_u_8x16x3.val[1])));
			const int16x8_t sourcePlane_1B_Upper_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sourcePlane_Upper_u_8x16x3.val[1])));

			const int16x8_t sourcePlane_2A_Upper_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sourcePlane_Upper_u_8x16x3.val[2])));
			const int16x8_t sourcePlane_2B_Upper_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sourcePlane_Upper_u_8x16x3.val[2])));

			int16x8_t intermediate_0A_Upper_s_16x8 = vmlaq_n_s16(vmlaq_n_s16(vmulq_n_s16(sourcePlane_0A_Upper_s_8x16, factorChannel00_128), sourcePlane_1A_Upper_s_8x16, factorChannel01_128), sourcePlane_2A_Upper_s_8x16, factorChannel02_128); // = channel0 * factor0 + channel1 * factor1 + channel2 * factor2
			int16x8_t intermediate_0B_Upper_s_16x8 = vmlaq_n_s16(vmlaq_n_s16(vmulq_n_s16(sourcePlane_0B_Upper_s_8x16, factorChannel00_128), sourcePlane_1B_Upper_s_8x16, factorChannel01_128), sourcePlane_2B_Upper_s_8x16, factorChannel02_128);


			intermediate_0A_Upper_s_16x8 = vrshrq_n_s16(intermediate_0A_Upper_s_16x8, 7); // /= 128
			intermediate_0B_Upper_s_16x8 = vrshrq_n_s16(intermediate_0B_Upper_s_16x8, 7);

			intermediate_0A_Upper_s_16x8 = vaddq_s16(intermediate_0A_Upper_s_16x8, biasChannel0_s_16x8); // += bias
			intermediate_0B_Upper_s_16x8 = vaddq_s16(intermediate_0B_Upper_s_16x8, biasChannel0_s_16x8);

			const uint8x8_t result_0A_Upper_u_8x8 = vqmovun_s16(intermediate_0A_Upper_s_16x8);
			const uint8x8_t result_0B_Upper_u_8x8 = vqmovun_s16(intermediate_0B_Upper_s_16x8);

			const uint8x16_t result_0_Upper_u_8x16 = vcombine_u8(result_0A_Upper_u_8x8, result_0B_Upper_u_8x8);

			vst1q_u8(targetPlane0Upper, result_0_Upper_u_8x16);


			// let's handle the lower row

			const uint8x16x3_t sourcePlane_Lower_u_8x16x3 = vld3q_u8(sourcePlane + sourcePlaneStrideElements);

			const uint16x8_t sourcePlaneAverage_0_Lower_u_16x8 = vpaddlq_u8(sourcePlane_Lower_u_8x16x3.val[0]);
			const uint16x8_t sourcePlaneAverage_1_Lower_u_16x8 = vpaddlq_u8(sourcePlane_Lower_u_8x16x3.val[1]);
			const uint16x8_t sourcePlaneAverage_2_Lower_u_16x8 = vpaddlq_u8(sourcePlane_Lower_u_8x16x3.val[2]);

			const int16x8_t sourcePlane_0A_Lower_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sourcePlane_Lower_u_8x16x3.val[0])));
			const int16x8_t sourcePlane_0B_Lower_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sourcePlane_Lower_u_8x16x3.val[0])));

			const int16x8_t sourcePlane_1A_Lower_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sourcePlane_Lower_u_8x16x3.val[1])));
			const int16x8_t sourcePlane_1B_Lower_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sourcePlane_Lower_u_8x16x3.val[1])));

			const int16x8_t sourcePlane_2A_Lower_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sourcePlane_Lower_u_8x16x3.val[2])));
			const int16x8_t sourcePlane_2B_Lower_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sourcePlane_Lower_u_8x16x3.val[2])));

			int16x8_t intermediate_0A_Lower_s_16x8 = vmlaq_n_s16(vmlaq_n_s16(vmulq_n_s16(sourcePlane_0A_Lower_s_8x16, factorChannel00_128), sourcePlane_1A_Lower_s_8x16, factorChannel01_128), sourcePlane_2A_Lower_s_8x16, factorChannel02_128); // = channel0 * factor0 + channel1 * factor1 + channel2 * factor2
			int16x8_t intermediate_0B_Lower_s_16x8 = vmlaq_n_s16(vmlaq_n_s16(vmulq_n_s16(sourcePlane_0B_Lower_s_8x16, factorChannel00_128), sourcePlane_1B_Lower_s_8x16, factorChannel01_128), sourcePlane_2B_Lower_s_8x16, factorChannel02_128);


			intermediate_0A_Lower_s_16x8 = vrshrq_n_s16(intermediate_0A_Lower_s_16x8, 7); // /= 128
			intermediate_0B_Lower_s_16x8 = vrshrq_n_s16(intermediate_0B_Lower_s_16x8, 7);

			intermediate_0A_Lower_s_16x8 = vaddq_s16(intermediate_0A_Lower_s_16x8, biasChannel0_s_16x8); // += bias
			intermediate_0B_Lower_s_16x8 = vaddq_s16(intermediate_0B_Lower_s_16x8, biasChannel0_s_16x8);

			const uint8x8_t result_0A_Lower_u_8x8 = vqmovun_s16(intermediate_0A_Lower_s_16x8);
			const uint8x8_t result_0B_Lower_u_8x8 = vqmovun_s16(intermediate_0B_Lower_s_16x8);

			const uint8x16_t result_0_Lower_u_8x16 = vcombine_u8(result_0A_Lower_u_8x8, result_0B_Lower_u_8x8);

			vst1q_u8(targetPlane0Lower, result_0_Lower_u_8x16);


			// let's handle the last two channels

			const int16x8_t sourcePlaneAverage_0_s_16x8 = vreinterpretq_s16_u16(vrshrq_n_s16(vrhaddq_u16(sourcePlaneAverage_0_Upper_u_16x8, sourcePlaneAverage_0_Lower_u_16x8), 1));
			const int16x8_t sourcePlaneAverage_1_s_16x8 = vreinterpretq_s16_u16(vrshrq_n_s16(vrhaddq_u16(sourcePlaneAverage_1_Upper_u_16x8, sourcePlaneAverage_1_Lower_u_16x8), 1));
			const int16x8_t sourcePlaneAverage_2_s_16x8 = vreinterpretq_s16_u16(vrshrq_n_s16(vrhaddq_u16(sourcePlaneAverage_2_Upper_u_16x8, sourcePlaneAverage_2_Lower_u_16x8), 1));

			int16x8_t intermediate_1_s_16x8 = vmlaq_n_s16(vmlaq_n_s16(vmulq_n_s16(sourcePlaneAverage_0_s_16x8, factorChannel10_128), sourcePlaneAverage_1_s_16x8, factorChannel11_128), sourcePlaneAverage_2_s_16x8, factorChannel12_128); // = channel0 * factor0 + channel1 * factor1 + channel2 * factor2
			int16x8_t intermediate_2_s_16x8 = vmlaq_n_s16(vmlaq_n_s16(vmulq_n_s16(sourcePlaneAverage_0_s_16x8, factorChannel20_128), sourcePlaneAverage_1_s_16x8, factorChannel21_128), sourcePlaneAverage_2_s_16x8, factorChannel22_128);

			intermediate_1_s_16x8 = vrshrq_n_s16(intermediate_1_s_16x8, 7); // /= 128
			intermediate_2_s_16x8 = vrshrq_n_s16(intermediate_2_s_16x8, 7);

			intermediate_1_s_16x8 = vaddq_s16(intermediate_1_s_16x8, biasChannel1_s_16x8); // += bias
			intermediate_2_s_16x8 = vaddq_s16(intermediate_2_s_16x8, biasChannel2_s_16x8);

			uint8x8x2_t result_12_u_8x8x2;
			result_12_u_8x8x2.val[0] = vqmovun_s16(intermediate_1_s_16x8);
			result_12_u_8x8x2.val[1] = vqmovun_s16(intermediate_2_s_16x8);

			vst2_u8(targetPlane1, result_12_u_8x8x2);

			sourcePlane += blockSize * 3u;

			targetPlane0Upper += blockSize;
			targetPlane0Lower += blockSize;
			targetPlane1 += blockSize; // 2x2 downsampled, but two channels
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sourcePlane != sourcePlaneEnd)
	{
		ocean_assert(sourcePlane < sourcePlaneEnd);

		const uint8_t* sourcePlaneLower = sourcePlane + sourcePlaneStrideElements;

		// upper target row
		targetPlane0Upper[0] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane[0]) * factorChannel00_128 + int16_t(sourcePlane[1]) * factorChannel01_128 + int16_t(sourcePlane[2]) * factorChannel02_128) / 128 + bias0, 255));
		targetPlane0Upper[1] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane[3]) * factorChannel00_128 + int16_t(sourcePlane[4]) * factorChannel01_128 + int16_t(sourcePlane[5]) * factorChannel02_128) / 128 + bias0, 255));

		// lower target row
		targetPlane0Lower[0u] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlaneLower[0]) * factorChannel00_128 + int16_t(sourcePlaneLower[1]) * factorChannel01_128 + int16_t(sourcePlaneLower[2]) * factorChannel02_128) / 128 + bias0, 255));
		targetPlane0Lower[1u] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlaneLower[3]) * factorChannel00_128 + int16_t(sourcePlaneLower[4]) * factorChannel01_128 + int16_t(sourcePlaneLower[5]) * factorChannel02_128) / 128 + bias0, 255));

		const int16_t averageSource0 = (sourcePlane[0] + sourcePlane[3] + sourcePlaneLower[0] + sourcePlaneLower[3] + 2u) / 4u;
		const int16_t averageSource1 = (sourcePlane[1] + sourcePlane[4] + sourcePlaneLower[1] + sourcePlaneLower[4] + 2u) / 4u;
		const int16_t averageSource2 = (sourcePlane[2] + sourcePlane[5] + sourcePlaneLower[2] + sourcePlaneLower[5] + 2u) / 4u;

		targetPlane1[0] = (uint8_t)(minmax<int16_t>(0, (averageSource0 * factorChannel10_128 + averageSource1 * factorChannel11_128 + averageSource2 * factorChannel12_128) / 128 + bias1, 255));
		targetPlane1[1] = (uint8_t)(minmax<int16_t>(0, (averageSource0 * factorChannel20_128 + averageSource1 * factorChannel21_128 + averageSource2 * factorChannel22_128) / 128 + bias2, 255));

		sourcePlane += 2u * 3u;

		targetPlane0Upper += 2;
		targetPlane0Lower += 2;
		targetPlane1 += 2; // 2x2 downsampled, but two channels
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>(targetPlane0Upper - width, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>(targetPlane0Lower - width, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 2u>(targetPlane1 - width, width / 2u);
	}
}

void FrameConverter::convertTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelsDownsampled2x2_8BitPerChannel_Precision7Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	const unsigned int width_2 = width / 2u;

	// options layout:
	// uint32_t: sourcePlanePaddingElements
	// uint32_t: targetPlane0PaddingElements
	// uint32_t: targetPlane1PaddingElements
	// uint32_t: targetPlane2PaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = clamp(0, f00 * s0 + f01 * s1 + f02 * s2 + b0, 255)
	// t1 = clamp(0, f10 * s0 + f11 * s1 + f12 * s2 + b1, 255)
	// t2 = clamp(0, f20 * s0 + f21 * s1 + f22 * s2 + b2, 255)

	const unsigned int sourcePlanePaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int targetPlane0PaddingElements = (unsigned int)(intOptions[1]);
	const unsigned int targetPlane1PaddingElements = (unsigned int)(intOptions[2]);
	const unsigned int targetPlane2PaddingElements = (unsigned int)(intOptions[3]);

	const uint8_t* sourcePlane = (const uint8_t*)(sources[0]);

	uint8_t* targetPlane0 = (uint8_t*)(targets[0]);
	uint8_t* targetPlane1 = (uint8_t*)(targets[1]);
	uint8_t* targetPlane2 = (uint8_t*)(targets[2]);

	const unsigned int sourcePlaneStrideElements = width * 3u + sourcePlanePaddingElements;
	const unsigned int targetPlane0StrideElements = width + targetPlane0PaddingElements;
	const unsigned int targetPlane1StrideElements = width_2 + targetPlane1PaddingElements;
	const unsigned int targetPlane2StrideElements = width_2 + targetPlane2PaddingElements;

	const int16_t factorChannel00_128 = int16_t(intOptions[4]);
	const int16_t factorChannel10_128 = int16_t(intOptions[5]);
	const int16_t factorChannel20_128 = int16_t(intOptions[6]);

	const int16_t factorChannel01_128 = int16_t(intOptions[7]);
	const int16_t factorChannel11_128 = int16_t(intOptions[8]);
	const int16_t factorChannel21_128 = int16_t(intOptions[9]);

	const int16_t factorChannel02_128 = int16_t(intOptions[10]);
	const int16_t factorChannel12_128 = int16_t(intOptions[11]);
	const int16_t factorChannel22_128 = int16_t(intOptions[12]);

	ocean_assert(std::abs(factorChannel00_128 + factorChannel01_128 + factorChannel02_128) <= 128);
	ocean_assert(std::abs(factorChannel10_128 + factorChannel11_128 + factorChannel12_128) <= 128);
	ocean_assert(std::abs(factorChannel20_128 + factorChannel21_128 + factorChannel22_128) <= 128);

	const int16_t bias0 = int16_t(intOptions[13]);
	const int16_t bias1 = int16_t(intOptions[14]);
	const int16_t bias2 = int16_t(intOptions[15]);

	ocean_assert(bias0 >= -128 && bias0 <= 128);
	ocean_assert(bias1 >= -128 && bias1 <= 128);
	ocean_assert(bias2 >= -128 && bias2 <= 128);

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	sourcePlane += multipleRowIndex * 2u * sourcePlaneStrideElements; // upper row

	uint8_t* targetPlane0Upper = flipTarget ? (targetPlane0 + (height - multipleRowIndex * 2u - 1u) * targetPlane0StrideElements) : targetPlane0 + multipleRowIndex * 2u * targetPlane0StrideElements;
	uint8_t* targetPlane0Lower = flipTarget ? targetPlane0Upper - targetPlane0StrideElements : targetPlane0Upper + targetPlane0StrideElements;
	targetPlane1 = flipTarget ? (targetPlane1 + (height / 2u - multipleRowIndex - 1u) * targetPlane1StrideElements) : targetPlane1 + multipleRowIndex * targetPlane1StrideElements;
	targetPlane2 = flipTarget ? (targetPlane2 + (height / 2u - multipleRowIndex - 1u) * targetPlane2StrideElements) : targetPlane2 + multipleRowIndex * targetPlane2StrideElements;

	const uint8_t* const sourcePlaneEnd = sourcePlane + width * 3u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	if (blocks >= 1u)
	{
		const int16x8_t biasChannel0_s_16x8 = vdupq_n_s16(bias0);
		const int16x8_t biasChannel1_s_16x8 = vdupq_n_s16(bias1);
		const int16x8_t biasChannel2_s_16x8 = vdupq_n_s16(bias2);

		for (size_t n = 0; n < blocks; ++n)
		{
			// let's handle the upper row

			const uint8x16x3_t sourcePlane_Upper_u_8x16x3 = vld3q_u8(sourcePlane);

			const uint16x8_t sourcePlaneAverage_0_Upper_u_16x8 = vpaddlq_u8(sourcePlane_Upper_u_8x16x3.val[0]);
			const uint16x8_t sourcePlaneAverage_1_Upper_u_16x8 = vpaddlq_u8(sourcePlane_Upper_u_8x16x3.val[1]);
			const uint16x8_t sourcePlaneAverage_2_Upper_u_16x8 = vpaddlq_u8(sourcePlane_Upper_u_8x16x3.val[2]);

			const int16x8_t sourcePlane_0A_Upper_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sourcePlane_Upper_u_8x16x3.val[0])));
			const int16x8_t sourcePlane_0B_Upper_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sourcePlane_Upper_u_8x16x3.val[0])));

			const int16x8_t sourcePlane_1A_Upper_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sourcePlane_Upper_u_8x16x3.val[1])));
			const int16x8_t sourcePlane_1B_Upper_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sourcePlane_Upper_u_8x16x3.val[1])));

			const int16x8_t sourcePlane_2A_Upper_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sourcePlane_Upper_u_8x16x3.val[2])));
			const int16x8_t sourcePlane_2B_Upper_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sourcePlane_Upper_u_8x16x3.val[2])));

			int16x8_t intermediate_0A_Upper_s_16x8 = vmlaq_n_s16(vmlaq_n_s16(vmulq_n_s16(sourcePlane_0A_Upper_s_8x16, factorChannel00_128), sourcePlane_1A_Upper_s_8x16, factorChannel01_128), sourcePlane_2A_Upper_s_8x16, factorChannel02_128); // = channel0 * factor0 + channel1 * factor1 + channel2 * factor2
			int16x8_t intermediate_0B_Upper_s_16x8 = vmlaq_n_s16(vmlaq_n_s16(vmulq_n_s16(sourcePlane_0B_Upper_s_8x16, factorChannel00_128), sourcePlane_1B_Upper_s_8x16, factorChannel01_128), sourcePlane_2B_Upper_s_8x16, factorChannel02_128);


			intermediate_0A_Upper_s_16x8 = vrshrq_n_s16(intermediate_0A_Upper_s_16x8, 7); // /= 128
			intermediate_0B_Upper_s_16x8 = vrshrq_n_s16(intermediate_0B_Upper_s_16x8, 7);

			intermediate_0A_Upper_s_16x8 = vaddq_s16(intermediate_0A_Upper_s_16x8, biasChannel0_s_16x8); // += bias
			intermediate_0B_Upper_s_16x8 = vaddq_s16(intermediate_0B_Upper_s_16x8, biasChannel0_s_16x8);

			const uint8x8_t result_0A_Upper_u_8x8 = vqmovun_s16(intermediate_0A_Upper_s_16x8);
			const uint8x8_t result_0B_Upper_u_8x8 = vqmovun_s16(intermediate_0B_Upper_s_16x8);

			const uint8x16_t result_0_Upper_u_8x16 = vcombine_u8(result_0A_Upper_u_8x8, result_0B_Upper_u_8x8);

			vst1q_u8(targetPlane0Upper, result_0_Upper_u_8x16);


			// let's handle the lower row

			const uint8x16x3_t sourcePlane_Lower_u_8x16x3 = vld3q_u8(sourcePlane + sourcePlaneStrideElements);

			const uint16x8_t sourcePlaneAverage_0_Lower_u_16x8 = vpaddlq_u8(sourcePlane_Lower_u_8x16x3.val[0]);
			const uint16x8_t sourcePlaneAverage_1_Lower_u_16x8 = vpaddlq_u8(sourcePlane_Lower_u_8x16x3.val[1]);
			const uint16x8_t sourcePlaneAverage_2_Lower_u_16x8 = vpaddlq_u8(sourcePlane_Lower_u_8x16x3.val[2]);

			const int16x8_t sourcePlane_0A_Lower_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sourcePlane_Lower_u_8x16x3.val[0])));
			const int16x8_t sourcePlane_0B_Lower_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sourcePlane_Lower_u_8x16x3.val[0])));

			const int16x8_t sourcePlane_1A_Lower_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sourcePlane_Lower_u_8x16x3.val[1])));
			const int16x8_t sourcePlane_1B_Lower_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sourcePlane_Lower_u_8x16x3.val[1])));

			const int16x8_t sourcePlane_2A_Lower_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sourcePlane_Lower_u_8x16x3.val[2])));
			const int16x8_t sourcePlane_2B_Lower_s_8x16 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sourcePlane_Lower_u_8x16x3.val[2])));

			int16x8_t intermediate_0A_Lower_s_16x8 = vmlaq_n_s16(vmlaq_n_s16(vmulq_n_s16(sourcePlane_0A_Lower_s_8x16, factorChannel00_128), sourcePlane_1A_Lower_s_8x16, factorChannel01_128), sourcePlane_2A_Lower_s_8x16, factorChannel02_128); // = channel0 * factor0 + channel1 * factor1 + channel2 * factor2
			int16x8_t intermediate_0B_Lower_s_16x8 = vmlaq_n_s16(vmlaq_n_s16(vmulq_n_s16(sourcePlane_0B_Lower_s_8x16, factorChannel00_128), sourcePlane_1B_Lower_s_8x16, factorChannel01_128), sourcePlane_2B_Lower_s_8x16, factorChannel02_128);


			intermediate_0A_Lower_s_16x8 = vrshrq_n_s16(intermediate_0A_Lower_s_16x8, 7); // /= 128
			intermediate_0B_Lower_s_16x8 = vrshrq_n_s16(intermediate_0B_Lower_s_16x8, 7);

			intermediate_0A_Lower_s_16x8 = vaddq_s16(intermediate_0A_Lower_s_16x8, biasChannel0_s_16x8); // += bias
			intermediate_0B_Lower_s_16x8 = vaddq_s16(intermediate_0B_Lower_s_16x8, biasChannel0_s_16x8);

			const uint8x8_t result_0A_Lower_u_8x8 = vqmovun_s16(intermediate_0A_Lower_s_16x8);
			const uint8x8_t result_0B_Lower_u_8x8 = vqmovun_s16(intermediate_0B_Lower_s_16x8);

			const uint8x16_t result_0_Lower_u_8x16 = vcombine_u8(result_0A_Lower_u_8x8, result_0B_Lower_u_8x8);

			vst1q_u8(targetPlane0Lower, result_0_Lower_u_8x16);


			// let's handle the last two channels

			const int16x8_t sourcePlaneAverage_0_s_16x8 = vreinterpretq_s16_u16(vrshrq_n_s16(vrhaddq_u16(sourcePlaneAverage_0_Upper_u_16x8, sourcePlaneAverage_0_Lower_u_16x8), 1));
			const int16x8_t sourcePlaneAverage_1_s_16x8 = vreinterpretq_s16_u16(vrshrq_n_s16(vrhaddq_u16(sourcePlaneAverage_1_Upper_u_16x8, sourcePlaneAverage_1_Lower_u_16x8), 1));
			const int16x8_t sourcePlaneAverage_2_s_16x8 = vreinterpretq_s16_u16(vrshrq_n_s16(vrhaddq_u16(sourcePlaneAverage_2_Upper_u_16x8, sourcePlaneAverage_2_Lower_u_16x8), 1));

			int16x8_t intermediate_1_s_16x8 = vmlaq_n_s16(vmlaq_n_s16(vmulq_n_s16(sourcePlaneAverage_0_s_16x8, factorChannel10_128), sourcePlaneAverage_1_s_16x8, factorChannel11_128), sourcePlaneAverage_2_s_16x8, factorChannel12_128); // = channel0 * factor0 + channel1 * factor1 + channel2 * factor2
			int16x8_t intermediate_2_s_16x8 = vmlaq_n_s16(vmlaq_n_s16(vmulq_n_s16(sourcePlaneAverage_0_s_16x8, factorChannel20_128), sourcePlaneAverage_1_s_16x8, factorChannel21_128), sourcePlaneAverage_2_s_16x8, factorChannel22_128);

			intermediate_1_s_16x8 = vrshrq_n_s16(intermediate_1_s_16x8, 7); // /= 128
			intermediate_2_s_16x8 = vrshrq_n_s16(intermediate_2_s_16x8, 7);

			intermediate_1_s_16x8 = vaddq_s16(intermediate_1_s_16x8, biasChannel1_s_16x8); // += bias
			intermediate_2_s_16x8 = vaddq_s16(intermediate_2_s_16x8, biasChannel2_s_16x8);

			const uint8x8_t result_1_u_8x8 = vqmovun_s16(intermediate_1_s_16x8);
			const uint8x8_t result_2_u_8x8 = vqmovun_s16(intermediate_2_s_16x8);

			vst1_u8(targetPlane1, result_1_u_8x8);
			vst1_u8(targetPlane2, result_2_u_8x8);

			sourcePlane += blockSize * 3u;

			targetPlane0Upper += blockSize;
			targetPlane0Lower += blockSize;
			targetPlane1 += blockSize / 2u;
			targetPlane2 += blockSize / 2u;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sourcePlane != sourcePlaneEnd)
	{
		ocean_assert(sourcePlane < sourcePlaneEnd);

		const uint8_t* sourcePlaneLower = sourcePlane + sourcePlaneStrideElements;

		// upper target row
		targetPlane0Upper[0] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane[0]) * factorChannel00_128 + int16_t(sourcePlane[1]) * factorChannel01_128 + int16_t(sourcePlane[2]) * factorChannel02_128) / 128 + bias0, 255));
		targetPlane0Upper[1] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlane[3]) * factorChannel00_128 + int16_t(sourcePlane[4]) * factorChannel01_128 + int16_t(sourcePlane[5]) * factorChannel02_128) / 128 + bias0, 255));

		// lower target row
		targetPlane0Lower[0u] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlaneLower[0]) * factorChannel00_128 + int16_t(sourcePlaneLower[1]) * factorChannel01_128 + int16_t(sourcePlaneLower[2]) * factorChannel02_128) / 128 + bias0, 255));
		targetPlane0Lower[1u] = (uint8_t)(minmax<int16_t>(0, (int16_t(sourcePlaneLower[3]) * factorChannel00_128 + int16_t(sourcePlaneLower[4]) * factorChannel01_128 + int16_t(sourcePlaneLower[5]) * factorChannel02_128) / 128 + bias0, 255));

		const int16_t averageSource0 = (sourcePlane[0] + sourcePlane[3] + sourcePlaneLower[0] + sourcePlaneLower[3] + 2u) / 4u;
		const int16_t averageSource1 = (sourcePlane[1] + sourcePlane[4] + sourcePlaneLower[1] + sourcePlaneLower[4] + 2u) / 4u;
		const int16_t averageSource2 = (sourcePlane[2] + sourcePlane[5] + sourcePlaneLower[2] + sourcePlaneLower[5] + 2u) / 4u;

		targetPlane1[0] = (uint8_t)(minmax<int16_t>(0, (averageSource0 * factorChannel10_128 + averageSource1 * factorChannel11_128 + averageSource2 * factorChannel12_128) / 128 + bias1, 255));
		targetPlane2[0] = (uint8_t)(minmax<int16_t>(0, (averageSource0 * factorChannel20_128 + averageSource1 * factorChannel21_128 + averageSource2 * factorChannel22_128) / 128 + bias2, 255));

		sourcePlane += 2u * 3u;

		targetPlane0Upper += 2;
		targetPlane0Lower += 2;
		++targetPlane1;
		++targetPlane2;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>(targetPlane0Upper - width, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>(targetPlane0Lower - width, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>(targetPlane1 - width_2, width_2);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>(targetPlane2 - width_2, width_2);
	}
}

template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
void FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	static_assert(tSourceChannelIndex0 < 3u && tSourceChannelIndex1 < 3u && tSourceChannelIndex2 < 3u, "Invalid source channels!");

	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 1u);
	ocean_assert(height >= 1u);
	ocean_assert(multipleRowIndex < height);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: sourcePlane2PaddingElements
	// uint32_t: targetPlanePaddingElements

	const unsigned int sourcePlane0PaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int sourcePlane1PaddingElements = (unsigned int)(intOptions[1]);
	const unsigned int sourcePlane2PaddingElements = (unsigned int)(intOptions[2]);
	const unsigned int targetPlanePaddingElements = (unsigned int)(intOptions[3]);

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);
	const uint8_t* sourcePlane2 = (const uint8_t*)(sources[2]);

	uint8_t* targetPlane = (uint8_t*)(targets[0]);

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width + sourcePlane1PaddingElements;
	const unsigned int sourcePlane2StrideElements = width + sourcePlane2PaddingElements;
	const unsigned int targetPlaneStrideElements = width * 3u + targetPlanePaddingElements;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	sourcePlane0 += multipleRowIndex * sourcePlane0StrideElements;
	sourcePlane1 += multipleRowIndex * sourcePlane1StrideElements;
	sourcePlane2 += multipleRowIndex * sourcePlane2StrideElements;

	targetPlane = flipTarget ? (targetPlane + (height - multipleRowIndex - 1u) * targetPlaneStrideElements) : targetPlane + multipleRowIndex * targetPlaneStrideElements;

	const uint8_t* const sourcePlane0End = sourcePlane0 + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16_t sourcePlane0_u_8x16 = vld1q_u8(sourcePlane0);
		const uint8x16_t sourcePlane1_u_8x16 = vld1q_u8(sourcePlane1);
		const uint8x16_t sourcePlane2_u_8x16 = vld1q_u8(sourcePlane2);

		uint8x16x3_t results_u_8x16x3;


		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			results_u_8x16x3.val[0] = sourcePlane0_u_8x16;
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			results_u_8x16x3.val[0] = sourcePlane1_u_8x16;
		}
		else
		{
			 ocean_assert(tSourceChannelIndex0 == 2u);

			results_u_8x16x3.val[0] = sourcePlane2_u_8x16;
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			results_u_8x16x3.val[1] = sourcePlane0_u_8x16;
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			results_u_8x16x3.val[1] = sourcePlane1_u_8x16;
		}
		else
		{
			 ocean_assert(tSourceChannelIndex1 == 2u);

			results_u_8x16x3.val[1] = sourcePlane2_u_8x16;
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			results_u_8x16x3.val[2] = sourcePlane0_u_8x16;
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			results_u_8x16x3.val[2] = sourcePlane1_u_8x16;
		}
		else
		{
			 ocean_assert(tSourceChannelIndex2 == 2u);

			results_u_8x16x3.val[2] = sourcePlane2_u_8x16;
		}


		// and we can store the result
		vst3q_u8(targetPlane, results_u_8x16x3);

		sourcePlane0 += blockSize;
		sourcePlane1 += blockSize;
		sourcePlane2 += blockSize;

		targetPlane += blockSize * 3u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sourcePlane0 != sourcePlane0End)
	{
		ocean_assert(sourcePlane0 < sourcePlane0End);

		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			targetPlane[0] = sourcePlane0[0];
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			targetPlane[0] = sourcePlane1[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex0 == 2u);

			targetPlane[0] = sourcePlane2[0];
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			targetPlane[1] = sourcePlane0[0];
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			targetPlane[1] = sourcePlane1[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex1 == 2u);

			targetPlane[1] = sourcePlane2[0];
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			targetPlane[2] = sourcePlane0[0];
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			targetPlane[2] = sourcePlane1[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex2 == 2u);

			targetPlane[2] = sourcePlane2[0];
		}

		++sourcePlane0;
		++sourcePlane1;
		++sourcePlane2;

		targetPlane += 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetPlane - width * 3u, width);
	}
}

template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<2u, 0u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_3Plane1Channel_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);

template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
void FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	static_assert(tSourceChannelIndex0 < 3u && tSourceChannelIndex1 < 3u && tSourceChannelIndex2 < 3u, "Invalid source channels!");

	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);
	ocean_assert(multipleRowIndex < height);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: targetZippedPaddingElements

	const unsigned int sourcePlane0PaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int sourcePlane1PaddingElements = (unsigned int)(intOptions[1]);
	const unsigned int targetPlanePaddingElements = (unsigned int)(intOptions[2]);

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);

	uint8_t* targetPlane = (uint8_t*)(targets[0]);

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width + sourcePlane1PaddingElements; // 2x2 downsampling but 2 channels
	const unsigned int targetPlaneStrideElements = width * 3u + targetPlanePaddingElements;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	sourcePlane0 += multipleRowIndex * sourcePlane0StrideElements;
	sourcePlane1 += (multipleRowIndex / 2u) * sourcePlane1StrideElements;

	targetPlane = flipTarget ? (targetPlane + (height - multipleRowIndex - 1u) * targetPlaneStrideElements) : targetPlane + multipleRowIndex * targetPlaneStrideElements;

	const uint8_t* const sourcePlane0End = sourcePlane0 + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16_t sourcePlane0_u_8x16 = vld1q_u8(sourcePlane0);

		const uint8x8x2_t sourcePlane1_u_8x8x2 = vld2_u8(sourcePlane1);
		const uint8x16_t sourcePlane1_u_8x16 = vcombine_u8(sourcePlane1_u_8x8x2.val[0], sourcePlane1_u_8x8x2.val[1]);

		const uint8x16x2_t sourcePlane1_u_8x16x2 = vzipq_u8(sourcePlane1_u_8x16, sourcePlane1_u_8x16);

		uint8x16x3_t results_u_8x16x3;


		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			results_u_8x16x3.val[0] = sourcePlane0_u_8x16;
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			results_u_8x16x3.val[0] = sourcePlane1_u_8x16x2.val[0];
		}
		else
		{
			 ocean_assert(tSourceChannelIndex0 == 2u);

			results_u_8x16x3.val[0] = sourcePlane1_u_8x16x2.val[1];
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			results_u_8x16x3.val[1] = sourcePlane0_u_8x16;
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			results_u_8x16x3.val[1] = sourcePlane1_u_8x16x2.val[0];
		}
		else
		{
			 ocean_assert(tSourceChannelIndex1 == 2u);

			results_u_8x16x3.val[1] = sourcePlane1_u_8x16x2.val[1];
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			results_u_8x16x3.val[2] = sourcePlane0_u_8x16;
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			results_u_8x16x3.val[2] = sourcePlane1_u_8x16x2.val[0];
		}
		else
		{
			 ocean_assert(tSourceChannelIndex2 == 2u);

			results_u_8x16x3.val[2] = sourcePlane1_u_8x16x2.val[1];
		}


		// and we can store the result
		vst3q_u8(targetPlane, results_u_8x16x3);

		sourcePlane0 += blockSize;
		sourcePlane1 += blockSize; // 2x2 downsampled, but two channels

		targetPlane += blockSize * 3u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sourcePlane0 != sourcePlane0End)
	{
		ocean_assert(sourcePlane0 < sourcePlane0End);

		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			targetPlane[0] = sourcePlane0[0];
			targetPlane[3] = sourcePlane0[1];
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			targetPlane[0] = sourcePlane1[0];
			targetPlane[3] = sourcePlane1[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex0 == 2u);

			targetPlane[0] = sourcePlane1[1];
			targetPlane[3] = sourcePlane1[1];
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			targetPlane[1] = sourcePlane0[0];
			targetPlane[4] = sourcePlane0[1];
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			targetPlane[1] = sourcePlane1[0];
			targetPlane[4] = sourcePlane1[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex1 == 2u);

			targetPlane[1] = sourcePlane1[1];
			targetPlane[4] = sourcePlane1[1];
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			targetPlane[2] = sourcePlane0[0];
			targetPlane[5] = sourcePlane0[1];
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			targetPlane[2] = sourcePlane1[0];
			targetPlane[5] = sourcePlane1[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex2 == 2u);

			targetPlane[2] = sourcePlane1[1];
			targetPlane[5] = sourcePlane1[1];
		}

		sourcePlane0 += 2;
		sourcePlane1 += 2; // 2x2 downsampled, but two channels

		targetPlane += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetPlane - width * 3u, width);
	}
}

template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 0u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);

template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
void FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	static_assert(tSourceChannelIndex0 < 3u && tSourceChannelIndex1 < 3u && tSourceChannelIndex2 < 3u, "Invalid source channels!");

	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlanePaddingElements
	// uint32_t: sourceZippedPaddingElements
	// uint32_t: targetZippedPaddingElements

	const unsigned int sourcePlanePaddingElements = (unsigned int)intOptions[0];
	const unsigned int sourceZippedPaddingElements = (unsigned int)intOptions[1];
	const unsigned int targetZippedPaddingElements = (unsigned int)intOptions[2];

	const uint8_t* sourcePlane = (const uint8_t*)(sources[0]);
	const uint8_t* sourceZipped = (const uint8_t*)(sources[1]);

	uint8_t* targetZipped = (uint8_t*)(targets[0]);

	const unsigned int sourcePlaneStrideElements = width + sourcePlanePaddingElements;
	const unsigned int sourceZippedStrideElements = width + sourceZippedPaddingElements; // 2x2 downsampling but 2 channels
	const unsigned int targetZippedStrideElements = width * 3u + targetZippedPaddingElements;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* sPlaneUpper = sourcePlane + multipleRowIndex * 2u * sourcePlaneStrideElements;
	const uint8_t* sZipped = sourceZipped + multipleRowIndex * sourceZippedStrideElements;

	uint8_t* tUpper = flipTarget ? (targetZipped + (height - multipleRowIndex * 2u - 1u) * targetZippedStrideElements) : targetZipped + multipleRowIndex * 2u * targetZippedStrideElements;
	uint8_t* tLower = flipTarget ? tUpper - targetZippedStrideElements : tUpper + targetZippedStrideElements;

	const uint8_t* const sPlaneUpperEnd = sPlaneUpper + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16_t sPlaneUpper_u_8x16 = vld1q_u8(sPlaneUpper);
		const uint8x16_t sPlaneLower_u_8x16 = vld1q_u8(sPlaneUpper + sourcePlaneStrideElements);

		const uint8x8x2_t sZipped_u_8x8x2 = vld2_u8(sZipped);
		const uint8x16_t sZipped_u_8x16 = vcombine_u8(sZipped_u_8x8x2.val[0], sZipped_u_8x8x2.val[1]);

		const uint8x16x2_t sZipped_u_8x16x2 = vzipq_u8(sZipped_u_8x16, sZipped_u_8x16);

		uint8x16x3_t resultsUpper_u_8x16x3;
		uint8x16x3_t resultsLower_u_8x16x3;


		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			resultsUpper_u_8x16x3.val[0] = sPlaneUpper_u_8x16;
			resultsLower_u_8x16x3.val[0] = sPlaneLower_u_8x16;
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			resultsUpper_u_8x16x3.val[0] = sZipped_u_8x16x2.val[0];
			resultsLower_u_8x16x3.val[0] = sZipped_u_8x16x2.val[0];
		}
		else
		{
			 ocean_assert(tSourceChannelIndex0 == 2u);

			resultsUpper_u_8x16x3.val[0] = sZipped_u_8x16x2.val[1];
			resultsLower_u_8x16x3.val[0] = sZipped_u_8x16x2.val[1];
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			resultsUpper_u_8x16x3.val[1] = sPlaneUpper_u_8x16;
			resultsLower_u_8x16x3.val[1] = sPlaneLower_u_8x16;
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			resultsUpper_u_8x16x3.val[1] = sZipped_u_8x16x2.val[0];
			resultsLower_u_8x16x3.val[1] = sZipped_u_8x16x2.val[0];
		}
		else
		{
			 ocean_assert(tSourceChannelIndex1 == 2u);

			resultsUpper_u_8x16x3.val[1] = sZipped_u_8x16x2.val[1];
			resultsLower_u_8x16x3.val[1] = sZipped_u_8x16x2.val[1];
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			resultsUpper_u_8x16x3.val[2] = sPlaneUpper_u_8x16;
			resultsLower_u_8x16x3.val[2] = sPlaneLower_u_8x16;
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			resultsUpper_u_8x16x3.val[2] = sZipped_u_8x16x2.val[0];
			resultsLower_u_8x16x3.val[2] = sZipped_u_8x16x2.val[0];
		}
		else
		{
			 ocean_assert(tSourceChannelIndex2 == 2u);

			resultsUpper_u_8x16x3.val[2] = sZipped_u_8x16x2.val[1];
			resultsLower_u_8x16x3.val[2] = sZipped_u_8x16x2.val[1];
		}


		// and we can store the result
		vst3q_u8(tUpper, resultsUpper_u_8x16x3);
		vst3q_u8(tLower, resultsLower_u_8x16x3);

		sPlaneUpper += blockSize;
		sZipped += blockSize; // 2x2 downsampled, but two channels
		tUpper += blockSize * 3u;
		tLower += blockSize * 3u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sPlaneUpper != sPlaneUpperEnd)
	{
		ocean_assert(sPlaneUpper < sPlaneUpperEnd);

		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			tUpper[0] = sPlaneUpper[0];
			tUpper[3] = sPlaneUpper[1];

			tLower[0] = sPlaneUpper[sourcePlaneStrideElements + 0u];
			tLower[3] = sPlaneUpper[sourcePlaneStrideElements + 1u];
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			tUpper[0] = sZipped[0];
			tUpper[3] = sZipped[0];

			tLower[0] = sZipped[0];
			tLower[3] = sZipped[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex0 == 2u);

			tUpper[0] = sZipped[1];
			tUpper[3] = sZipped[1];

			tLower[0] = sZipped[1];
			tLower[3] = sZipped[1];
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			tUpper[1] = sPlaneUpper[0];
			tUpper[4] = sPlaneUpper[1];

			tLower[1] = sPlaneUpper[sourcePlaneStrideElements + 0u];
			tLower[4] = sPlaneUpper[sourcePlaneStrideElements + 1u];
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			tUpper[1] = sZipped[0];
			tUpper[4] = sZipped[0];

			tLower[1] = sZipped[0];
			tLower[4] = sZipped[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex1 == 2u);

			tUpper[1] = sZipped[1];
			tUpper[4] = sZipped[1];

			tLower[1] = sZipped[1];
			tLower[4] = sZipped[1];
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			tUpper[2] = sPlaneUpper[0];
			tUpper[5] = sPlaneUpper[1];

			tLower[2] = sPlaneUpper[sourcePlaneStrideElements + 0u];
			tLower[5] = sPlaneUpper[sourcePlaneStrideElements + 1u];
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			tUpper[2] = sZipped[0];
			tUpper[5] = sZipped[0];

			tLower[2] = sZipped[0];
			tLower[5] = sZipped[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex2 == 2u);

			tUpper[2] = sZipped[1];
			tUpper[5] = sZipped[1];

			tLower[2] = sZipped[1];
			tLower[5] = sZipped[1];
		}

		sPlaneUpper += 2;
		sZipped += 2; // 2x2 downsampled, but two channels
		tUpper += 2u * 3u;
		tLower += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(tUpper - width * 3u, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(tLower - width * 3u, width);
	}
}

template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 0u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane1ChannelAnd1Plane2ChannelsDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);

void FrameConverter::convertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);
	ocean_assert(multipleRowIndex < height);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: sourcePlane2PaddingElements
	// uint32_t: targetZippedPaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

	const unsigned int sourcePlane0PaddingElements = (unsigned int)intOptions[0];
	const unsigned int sourcePlane1PaddingElements = (unsigned int)intOptions[1];
	const unsigned int sourcePlane2PaddingElements = (unsigned int)intOptions[2];
	const unsigned int targetZippedPaddingElements = (unsigned int)intOptions[3];

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);
	const uint8_t* sourcePlane2 = (const uint8_t*)(sources[2]);

	uint8_t* targetZipped = (uint8_t*)(targets[0]);

	const unsigned int width_2 = width / 2u;

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width_2 + sourcePlane1PaddingElements;
	const unsigned int sourcePlane2StrideElements = width_2 + sourcePlane2PaddingElements;
	const unsigned int targetZippedStrideElements = width * 3u + targetZippedPaddingElements;

	const int factorChannel00_1024 = intOptions[4];
	const int factorChannel10_1024 = intOptions[5];
	const int factorChannel20_1024 = intOptions[6];

	const int factorChannel01_1024 = intOptions[7];
	const int factorChannel11_1024 = intOptions[8];
	const int factorChannel21_1024 = intOptions[9];

	const int factorChannel02_1024 = intOptions[10];
	const int factorChannel12_1024 = intOptions[11];
	const int factorChannel22_1024 = intOptions[12];

	const int bias0 = intOptions[13];
	const int bias1 = intOptions[14];
	const int bias2 = intOptions[15];

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* sPlane0Upper = sourcePlane0 + multipleRowIndex * sourcePlane0StrideElements;
	const uint8_t* sPlane1 = sourcePlane1 + (multipleRowIndex / 2u) * sourcePlane1StrideElements;
	const uint8_t* sPlane2 = sourcePlane2 + (multipleRowIndex / 2u) * sourcePlane2StrideElements;

	uint8_t* target = flipTarget ? (targetZipped + (height - multipleRowIndex - 1u) * targetZippedStrideElements) : targetZipped + multipleRowIndex * targetZippedStrideElements;

	const uint8_t* const sPlane0UpperEnd = sPlane0Upper + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const int16x4_t factorChannel00_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel00_1024));
	const int16x4_t factorChannel10_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel10_1024));
	const int16x4_t factorChannel20_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel20_1024));

	const int16x4_t factorChannel01_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel01_1024));
	const int16x4_t factorChannel11_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel11_1024));
	const int16x4_t factorChannel21_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel21_1024));

	const int16x4_t factorChannel02_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel02_1024));
	const int16x4_t factorChannel12_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel12_1024));
	const int16x4_t factorChannel22_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel22_1024));

	const int32x4_t bias0_1024_s_32x4 = vdupq_n_s32(bias0 * 1024);
	const int32x4_t bias1_1024_s_32x4 = vdupq_n_s32(bias1 * 1024);
	const int32x4_t bias2_1024_s_32x4 = vdupq_n_s32(bias2 * 1024);

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const int16x8_t sPlane1_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(sPlane1)));
		const int16x8_t sPlane2_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(sPlane2)));

		const int16x4_t sPlane1A_s_16x4 = vget_low_s16(sPlane1_s_16x8);
		const int16x4_t sPlane1B_s_16x4 = vget_high_s16(sPlane1_s_16x8);
		const int16x4_t sPlane2A_s_16x4 = vget_low_s16(sPlane2_s_16x8);
		const int16x4_t sPlane2B_s_16x4 = vget_high_s16(sPlane2_s_16x8);


		// first, handling sub-sampled planes 1 and 2 part

		int32x4_t plane12IntermediateResults0A_s_32x4 = vmlal_s16(bias0_1024_s_32x4, sPlane1A_s_16x4, factorChannel01_1024_s_16x4); // f01 * plane1 + b0
		int32x4_t plane12IntermediateResults1A_s_32x4 = vmlal_s16(bias1_1024_s_32x4, sPlane1A_s_16x4, factorChannel11_1024_s_16x4); // f11 * plane1 + b1
		int32x4_t plane12IntermediateResults2A_s_32x4 = vmlal_s16(bias2_1024_s_32x4, sPlane1A_s_16x4, factorChannel21_1024_s_16x4); // f21 * plane1 + b2

		int32x4_t plane12IntermediateResults0B_s_32x4 = vmlal_s16(bias0_1024_s_32x4, sPlane1B_s_16x4, factorChannel01_1024_s_16x4);
		int32x4_t plane12IntermediateResults1B_s_32x4 = vmlal_s16(bias1_1024_s_32x4, sPlane1B_s_16x4, factorChannel11_1024_s_16x4);
		int32x4_t plane12IntermediateResults2B_s_32x4 = vmlal_s16(bias2_1024_s_32x4, sPlane1B_s_16x4, factorChannel21_1024_s_16x4);

		plane12IntermediateResults0A_s_32x4 = vmlal_s16(plane12IntermediateResults0A_s_32x4, sPlane2A_s_16x4, factorChannel02_1024_s_16x4); // += f02 * plane2
		plane12IntermediateResults1A_s_32x4 = vmlal_s16(plane12IntermediateResults1A_s_32x4, sPlane2A_s_16x4, factorChannel12_1024_s_16x4); // += f12 * plane2
		plane12IntermediateResults2A_s_32x4 = vmlal_s16(plane12IntermediateResults2A_s_32x4, sPlane2A_s_16x4, factorChannel22_1024_s_16x4); // += f22 * plane2

		plane12IntermediateResults0B_s_32x4 = vmlal_s16(plane12IntermediateResults0B_s_32x4, sPlane2B_s_16x4, factorChannel02_1024_s_16x4);
		plane12IntermediateResults1B_s_32x4 = vmlal_s16(plane12IntermediateResults1B_s_32x4, sPlane2B_s_16x4, factorChannel12_1024_s_16x4);
		plane12IntermediateResults2B_s_32x4 = vmlal_s16(plane12IntermediateResults2B_s_32x4, sPlane2B_s_16x4, factorChannel22_1024_s_16x4);


		// now, handling plane 0

		const uint8x16_t sPlane0Upper_u_8x16 = vld1q_u8(sPlane0Upper); // upper row
		const int16x8_t sPlane0UpperA_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sPlane0Upper_u_8x16)));
		const int16x8_t sPlane0UpperB_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sPlane0Upper_u_8x16)));

		const int16x4_t sPlane0UpperA_s_16x4 = vget_low_s16(sPlane0UpperA_s_16x8);
		const int16x4_t sPlane0UpperB_s_16x4 = vget_high_s16(sPlane0UpperA_s_16x8);
		const int16x4_t sPlane0UpperC_s_16x4 = vget_low_s16(sPlane0UpperB_s_16x8);
		const int16x4_t sPlane0UpperD_s_16x4 = vget_high_s16(sPlane0UpperB_s_16x8);

		const int32x4_t sPlaneUpperMultiplied0A_s_32x4 = vmull_s16(sPlane0UpperA_s_16x4, factorChannel00_1024_s_16x4); // f00 * plane (upper)
		const int32x4_t sPlaneUpperMultiplied0B_s_32x4 = vmull_s16(sPlane0UpperB_s_16x4, factorChannel00_1024_s_16x4);
		const int32x4_t sPlaneUpperMultiplied0C_s_32x4 = vmull_s16(sPlane0UpperC_s_16x4, factorChannel00_1024_s_16x4);
		const int32x4_t sPlaneUpperMultiplied0D_s_32x4 = vmull_s16(sPlane0UpperD_s_16x4, factorChannel00_1024_s_16x4);

		const int32x4_t sPlaneUpperMultiplied1A_s_32x4 = vmull_s16(sPlane0UpperA_s_16x4, factorChannel10_1024_s_16x4); // f10 * plane (upper)
		const int32x4_t sPlaneUpperMultiplied1B_s_32x4 = vmull_s16(sPlane0UpperB_s_16x4, factorChannel10_1024_s_16x4);
		const int32x4_t sPlaneUpperMultiplied1C_s_32x4 = vmull_s16(sPlane0UpperC_s_16x4, factorChannel10_1024_s_16x4);
		const int32x4_t sPlaneUpperMultiplied1D_s_32x4 = vmull_s16(sPlane0UpperD_s_16x4, factorChannel10_1024_s_16x4);

		const int32x4_t sPlaneUpperMultiplied2A_s_32x4 = vmull_s16(sPlane0UpperA_s_16x4, factorChannel20_1024_s_16x4); // f20 * plane (upper)
		const int32x4_t sPlaneUpperMultiplied2B_s_32x4 = vmull_s16(sPlane0UpperB_s_16x4, factorChannel20_1024_s_16x4);
		const int32x4_t sPlaneUpperMultiplied2C_s_32x4 = vmull_s16(sPlane0UpperC_s_16x4, factorChannel20_1024_s_16x4);
		const int32x4_t sPlaneUpperMultiplied2D_s_32x4 = vmull_s16(sPlane0UpperD_s_16x4, factorChannel20_1024_s_16x4);


		// now, we can align plane 0 with planes 1 and 2

		const int32x4x2_t plane12IntermediateResults0A_s_32x4x2 = vzipq_s32(plane12IntermediateResults0A_s_32x4, plane12IntermediateResults0A_s_32x4);
		const int32x4x2_t plane12IntermediateResults1A_s_32x4x2 = vzipq_s32(plane12IntermediateResults1A_s_32x4, plane12IntermediateResults1A_s_32x4);
		const int32x4x2_t plane12IntermediateResults2A_s_32x4x2 = vzipq_s32(plane12IntermediateResults2A_s_32x4, plane12IntermediateResults2A_s_32x4);

		const int32x4x2_t plane12IntermediateResults0B_s_32x4x2 = vzipq_s32(plane12IntermediateResults0B_s_32x4, plane12IntermediateResults0B_s_32x4);
		const int32x4x2_t plane12IntermediateResults1B_s_32x4x2 = vzipq_s32(plane12IntermediateResults1B_s_32x4, plane12IntermediateResults1B_s_32x4);
		const int32x4x2_t plane12IntermediateResults2B_s_32x4x2 = vzipq_s32(plane12IntermediateResults2B_s_32x4, plane12IntermediateResults2B_s_32x4);


		// now, we can determine the upper results

		const int32x4_t resultUpper0A_s_32x4 = vaddq_s32(plane12IntermediateResults0A_s_32x4x2.val[0], sPlaneUpperMultiplied0A_s_32x4);
		const int32x4_t resultUpper0B_s_32x4 = vaddq_s32(plane12IntermediateResults0A_s_32x4x2.val[1], sPlaneUpperMultiplied0B_s_32x4);
		const int32x4_t resultUpper0C_s_32x4 = vaddq_s32(plane12IntermediateResults0B_s_32x4x2.val[0], sPlaneUpperMultiplied0C_s_32x4);
		const int32x4_t resultUpper0D_s_32x4 = vaddq_s32(plane12IntermediateResults0B_s_32x4x2.val[1], sPlaneUpperMultiplied0D_s_32x4);

		const int32x4_t resultUpper1A_s_32x4 = vaddq_s32(plane12IntermediateResults1A_s_32x4x2.val[0], sPlaneUpperMultiplied1A_s_32x4);
		const int32x4_t resultUpper1B_s_32x4 = vaddq_s32(plane12IntermediateResults1A_s_32x4x2.val[1], sPlaneUpperMultiplied1B_s_32x4);
		const int32x4_t resultUpper1C_s_32x4 = vaddq_s32(plane12IntermediateResults1B_s_32x4x2.val[0], sPlaneUpperMultiplied1C_s_32x4);
		const int32x4_t resultUpper1D_s_32x4 = vaddq_s32(plane12IntermediateResults1B_s_32x4x2.val[1], sPlaneUpperMultiplied1D_s_32x4);

		const int32x4_t resultUpper2A_s_32x4 = vaddq_s32(plane12IntermediateResults2A_s_32x4x2.val[0], sPlaneUpperMultiplied2A_s_32x4);
		const int32x4_t resultUpper2B_s_32x4 = vaddq_s32(plane12IntermediateResults2A_s_32x4x2.val[1], sPlaneUpperMultiplied2B_s_32x4);
		const int32x4_t resultUpper2C_s_32x4 = vaddq_s32(plane12IntermediateResults2B_s_32x4x2.val[0], sPlaneUpperMultiplied2C_s_32x4);
		const int32x4_t resultUpper2D_s_32x4 = vaddq_s32(plane12IntermediateResults2B_s_32x4x2.val[1], sPlaneUpperMultiplied2D_s_32x4);


		uint8x16x3_t resultsUpper_u_8x16x3;

		// saturated narrow signed to unsigned
		resultsUpper_u_8x16x3.val[0] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper0A_s_32x4, 10), vqrshrun_n_s32(resultUpper0B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper0C_s_32x4, 10), vqrshrun_n_s32(resultUpper0D_s_32x4, 10))));
		resultsUpper_u_8x16x3.val[1] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper1A_s_32x4, 10), vqrshrun_n_s32(resultUpper1B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper1C_s_32x4, 10), vqrshrun_n_s32(resultUpper1D_s_32x4, 10))));
		resultsUpper_u_8x16x3.val[2] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper2A_s_32x4, 10), vqrshrun_n_s32(resultUpper2B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper2C_s_32x4, 10), vqrshrun_n_s32(resultUpper2D_s_32x4, 10))));

		// and we can store the result
		vst3q_u8(target, resultsUpper_u_8x16x3);


		sPlane0Upper += blockSize;
		sPlane1 += blockSize / 2u;
		sPlane2 += blockSize / 2u;

		target += blockSize * 3u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sPlane0Upper != sPlane0UpperEnd)
	{
		ocean_assert(sPlane0Upper < sPlane0UpperEnd);

		const int intermediate0 = sPlane1[0] * int16_t(factorChannel01_1024) + sPlane2[0] * int16_t(factorChannel02_1024);
		const int intermediate1 = sPlane1[0] * int16_t(factorChannel11_1024) + sPlane2[0] * int16_t(factorChannel12_1024);
		const int intermediate2 = sPlane1[0] * int16_t(factorChannel21_1024) + sPlane2[0] * int16_t(factorChannel22_1024);

		// first and second upper pixel

		target[0] = (uint8_t)(minmax<int>(0, (sPlane0Upper[0] * int16_t(factorChannel00_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		target[1] = (uint8_t)(minmax<int>(0, (sPlane0Upper[0] * int16_t(factorChannel10_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		target[2] = (uint8_t)(minmax<int>(0, (sPlane0Upper[0] * int16_t(factorChannel20_1024) + intermediate2) / 1024 + int16_t(bias2), 255));

		target[3] = (uint8_t)(minmax<int>(0, (sPlane0Upper[1] * int16_t(factorChannel00_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		target[4] = (uint8_t)(minmax<int>(0, (sPlane0Upper[1] * int16_t(factorChannel10_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		target[5] = (uint8_t)(minmax<int>(0, (sPlane0Upper[1] * int16_t(factorChannel20_1024) + intermediate2) / 1024 + int16_t(bias2), 255));


		sPlane0Upper += 2;
		sPlane1++;
		sPlane2++;

		target += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(target - width * 3u, width);
	}
}

void FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: sourcePlane2PaddingElements
	// uint32_t: targetZippedPaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

	const unsigned int sourcePlane0PaddingElements = (unsigned int)intOptions[0];
	const unsigned int sourcePlane1PaddingElements = (unsigned int)intOptions[1];
	const unsigned int sourcePlane2PaddingElements = (unsigned int)intOptions[2];
	const unsigned int targetZippedPaddingElements = (unsigned int)intOptions[3];

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);
	const uint8_t* sourcePlane2 = (const uint8_t*)(sources[2]);

	uint8_t* targetZipped = (uint8_t*)(targets[0]);

	const unsigned int width_2 = width / 2u;

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width_2 + sourcePlane1PaddingElements;
	const unsigned int sourcePlane2StrideElements = width_2 + sourcePlane2PaddingElements;
	const unsigned int targetZippedStrideElements = width * 3u + targetZippedPaddingElements;

	const int factorChannel00_1024 = intOptions[4];
	const int factorChannel10_1024 = intOptions[5];
	const int factorChannel20_1024 = intOptions[6];

	const int factorChannel01_1024 = intOptions[7];
	const int factorChannel11_1024 = intOptions[8];
	const int factorChannel21_1024 = intOptions[9];

	const int factorChannel02_1024 = intOptions[10];
	const int factorChannel12_1024 = intOptions[11];
	const int factorChannel22_1024 = intOptions[12];

	const int bias0 = intOptions[13];
	const int bias1 = intOptions[14];
	const int bias2 = intOptions[15];

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* sPlane0Upper = sourcePlane0 + multipleRowIndex * 2u * sourcePlane0StrideElements;
	const uint8_t* sPlane1 = sourcePlane1 + multipleRowIndex * sourcePlane1StrideElements;
	const uint8_t* sPlane2 = sourcePlane2 + multipleRowIndex * sourcePlane2StrideElements;

	uint8_t* tUpper = flipTarget ? (targetZipped + (height - multipleRowIndex * 2u - 1u) * targetZippedStrideElements) : targetZipped + multipleRowIndex * 2u * targetZippedStrideElements;
	uint8_t* tLower = flipTarget ? tUpper - targetZippedStrideElements : tUpper + targetZippedStrideElements;

	const uint8_t* const sPlane0UpperEnd = sPlane0Upper + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const int16x4_t factorChannel00_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel00_1024));
	const int16x4_t factorChannel10_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel10_1024));
	const int16x4_t factorChannel20_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel20_1024));

	const int16x4_t factorChannel01_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel01_1024));
	const int16x4_t factorChannel11_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel11_1024));
	const int16x4_t factorChannel21_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel21_1024));

	const int16x4_t factorChannel02_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel02_1024));
	const int16x4_t factorChannel12_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel12_1024));
	const int16x4_t factorChannel22_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel22_1024));

	const int32x4_t bias0_1024_s_32x4 = vdupq_n_s32(bias0 * 1024);
	const int32x4_t bias1_1024_s_32x4 = vdupq_n_s32(bias1 * 1024);
	const int32x4_t bias2_1024_s_32x4 = vdupq_n_s32(bias2 * 1024);

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const int16x8_t sPlane1_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(sPlane1)));
		const int16x8_t sPlane2_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vld1_u8(sPlane2)));

		const int16x4_t sPlane1A_s_16x4 = vget_low_s16(sPlane1_s_16x8);
		const int16x4_t sPlane1B_s_16x4 = vget_high_s16(sPlane1_s_16x8);
		const int16x4_t sPlane2A_s_16x4 = vget_low_s16(sPlane2_s_16x8);
		const int16x4_t sPlane2B_s_16x4 = vget_high_s16(sPlane2_s_16x8);


		// first, handling sub-sampled planes 1 and 2 part

		int32x4_t plane12IntermediateResults0A_s_32x4 = vmlal_s16(bias0_1024_s_32x4, sPlane1A_s_16x4, factorChannel01_1024_s_16x4); // f01 * plane1 + b0
		int32x4_t plane12IntermediateResults1A_s_32x4 = vmlal_s16(bias1_1024_s_32x4, sPlane1A_s_16x4, factorChannel11_1024_s_16x4); // f11 * plane1 + b1
		int32x4_t plane12IntermediateResults2A_s_32x4 = vmlal_s16(bias2_1024_s_32x4, sPlane1A_s_16x4, factorChannel21_1024_s_16x4); // f21 * plane1 + b2

		int32x4_t plane12IntermediateResults0B_s_32x4 = vmlal_s16(bias0_1024_s_32x4, sPlane1B_s_16x4, factorChannel01_1024_s_16x4);
		int32x4_t plane12IntermediateResults1B_s_32x4 = vmlal_s16(bias1_1024_s_32x4, sPlane1B_s_16x4, factorChannel11_1024_s_16x4);
		int32x4_t plane12IntermediateResults2B_s_32x4 = vmlal_s16(bias2_1024_s_32x4, sPlane1B_s_16x4, factorChannel21_1024_s_16x4);

		plane12IntermediateResults0A_s_32x4 = vmlal_s16(plane12IntermediateResults0A_s_32x4, sPlane2A_s_16x4, factorChannel02_1024_s_16x4); // += f02 * plane2
		plane12IntermediateResults1A_s_32x4 = vmlal_s16(plane12IntermediateResults1A_s_32x4, sPlane2A_s_16x4, factorChannel12_1024_s_16x4); // += f12 * plane2
		plane12IntermediateResults2A_s_32x4 = vmlal_s16(plane12IntermediateResults2A_s_32x4, sPlane2A_s_16x4, factorChannel22_1024_s_16x4); // += f22 * plane2

		plane12IntermediateResults0B_s_32x4 = vmlal_s16(plane12IntermediateResults0B_s_32x4, sPlane2B_s_16x4, factorChannel02_1024_s_16x4);
		plane12IntermediateResults1B_s_32x4 = vmlal_s16(plane12IntermediateResults1B_s_32x4, sPlane2B_s_16x4, factorChannel12_1024_s_16x4);
		plane12IntermediateResults2B_s_32x4 = vmlal_s16(plane12IntermediateResults2B_s_32x4, sPlane2B_s_16x4, factorChannel22_1024_s_16x4);


		// now, handling plane 0

		const uint8x16_t sPlane0Upper_u_8x16 = vld1q_u8(sPlane0Upper); // upper row
		const int16x8_t sPlane0UpperA_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sPlane0Upper_u_8x16)));
		const int16x8_t sPlane0UpperB_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sPlane0Upper_u_8x16)));

		const int16x4_t sPlane0UpperA_s_16x4 = vget_low_s16(sPlane0UpperA_s_16x8);
		const int16x4_t sPlane0UpperB_s_16x4 = vget_high_s16(sPlane0UpperA_s_16x8);
		const int16x4_t sPlane0UpperC_s_16x4 = vget_low_s16(sPlane0UpperB_s_16x8);
		const int16x4_t sPlane0UpperD_s_16x4 = vget_high_s16(sPlane0UpperB_s_16x8);

		const int32x4_t sPlaneUpperMultiplied0A_s_32x4 = vmull_s16(sPlane0UpperA_s_16x4, factorChannel00_1024_s_16x4); // f00 * plane (upper)
		const int32x4_t sPlaneUpperMultiplied0B_s_32x4 = vmull_s16(sPlane0UpperB_s_16x4, factorChannel00_1024_s_16x4);
		const int32x4_t sPlaneUpperMultiplied0C_s_32x4 = vmull_s16(sPlane0UpperC_s_16x4, factorChannel00_1024_s_16x4);
		const int32x4_t sPlaneUpperMultiplied0D_s_32x4 = vmull_s16(sPlane0UpperD_s_16x4, factorChannel00_1024_s_16x4);

		const int32x4_t sPlaneUpperMultiplied1A_s_32x4 = vmull_s16(sPlane0UpperA_s_16x4, factorChannel10_1024_s_16x4); // f10 * plane (upper)
		const int32x4_t sPlaneUpperMultiplied1B_s_32x4 = vmull_s16(sPlane0UpperB_s_16x4, factorChannel10_1024_s_16x4);
		const int32x4_t sPlaneUpperMultiplied1C_s_32x4 = vmull_s16(sPlane0UpperC_s_16x4, factorChannel10_1024_s_16x4);
		const int32x4_t sPlaneUpperMultiplied1D_s_32x4 = vmull_s16(sPlane0UpperD_s_16x4, factorChannel10_1024_s_16x4);

		const int32x4_t sPlaneUpperMultiplied2A_s_32x4 = vmull_s16(sPlane0UpperA_s_16x4, factorChannel20_1024_s_16x4); // f20 * plane (upper)
		const int32x4_t sPlaneUpperMultiplied2B_s_32x4 = vmull_s16(sPlane0UpperB_s_16x4, factorChannel20_1024_s_16x4);
		const int32x4_t sPlaneUpperMultiplied2C_s_32x4 = vmull_s16(sPlane0UpperC_s_16x4, factorChannel20_1024_s_16x4);
		const int32x4_t sPlaneUpperMultiplied2D_s_32x4 = vmull_s16(sPlane0UpperD_s_16x4, factorChannel20_1024_s_16x4);


		const uint8x16_t sPlane0Lower_u_8x16 = vld1q_u8(sPlane0Upper + sourcePlane0StrideElements); // lower row
		const int16x8_t sPlane0LowerA_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(sPlane0Lower_u_8x16)));
		const int16x8_t sPlane0LowerB_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(sPlane0Lower_u_8x16)));

		const int16x4_t sPlane0LowerA_s_16x4 = vget_low_s16(sPlane0LowerA_s_16x8);
		const int16x4_t sPlane0LowerB_s_16x4 = vget_high_s16(sPlane0LowerA_s_16x8);
		const int16x4_t sPlane0LowerC_s_16x4 = vget_low_s16(sPlane0LowerB_s_16x8);
		const int16x4_t sPlane0LowerD_s_16x4 = vget_high_s16(sPlane0LowerB_s_16x8);

		const int32x4_t sPlaneLowerMultiplied0A_s_32x4 = vmull_s16(sPlane0LowerA_s_16x4, factorChannel00_1024_s_16x4); // f00 * plane (lower)
		const int32x4_t sPlaneLowerMultiplied0B_s_32x4 = vmull_s16(sPlane0LowerB_s_16x4, factorChannel00_1024_s_16x4);
		const int32x4_t sPlaneLowerMultiplied0C_s_32x4 = vmull_s16(sPlane0LowerC_s_16x4, factorChannel00_1024_s_16x4);
		const int32x4_t sPlaneLowerMultiplied0D_s_32x4 = vmull_s16(sPlane0LowerD_s_16x4, factorChannel00_1024_s_16x4);

		const int32x4_t sPlaneLowerMultiplied1A_s_32x4 = vmull_s16(sPlane0LowerA_s_16x4, factorChannel10_1024_s_16x4); // f10 * plane (lower)
		const int32x4_t sPlaneLowerMultiplied1B_s_32x4 = vmull_s16(sPlane0LowerB_s_16x4, factorChannel10_1024_s_16x4);
		const int32x4_t sPlaneLowerMultiplied1C_s_32x4 = vmull_s16(sPlane0LowerC_s_16x4, factorChannel10_1024_s_16x4);
		const int32x4_t sPlaneLowerMultiplied1D_s_32x4 = vmull_s16(sPlane0LowerD_s_16x4, factorChannel10_1024_s_16x4);

		const int32x4_t sPlaneLowerMultiplied2A_s_32x4 = vmull_s16(sPlane0LowerA_s_16x4, factorChannel20_1024_s_16x4); // f20 * plane (lower)
		const int32x4_t sPlaneLowerMultiplied2B_s_32x4 = vmull_s16(sPlane0LowerB_s_16x4, factorChannel20_1024_s_16x4);
		const int32x4_t sPlaneLowerMultiplied2C_s_32x4 = vmull_s16(sPlane0LowerC_s_16x4, factorChannel20_1024_s_16x4);
		const int32x4_t sPlaneLowerMultiplied2D_s_32x4 = vmull_s16(sPlane0LowerD_s_16x4, factorChannel20_1024_s_16x4);


		// now, we can align plane 0 with planes 1 and 2

		const int32x4x2_t plane12IntermediateResults0A_s_32x4x2 = vzipq_s32(plane12IntermediateResults0A_s_32x4, plane12IntermediateResults0A_s_32x4);
		const int32x4x2_t plane12IntermediateResults1A_s_32x4x2 = vzipq_s32(plane12IntermediateResults1A_s_32x4, plane12IntermediateResults1A_s_32x4);
		const int32x4x2_t plane12IntermediateResults2A_s_32x4x2 = vzipq_s32(plane12IntermediateResults2A_s_32x4, plane12IntermediateResults2A_s_32x4);

		const int32x4x2_t plane12IntermediateResults0B_s_32x4x2 = vzipq_s32(plane12IntermediateResults0B_s_32x4, plane12IntermediateResults0B_s_32x4);
		const int32x4x2_t plane12IntermediateResults1B_s_32x4x2 = vzipq_s32(plane12IntermediateResults1B_s_32x4, plane12IntermediateResults1B_s_32x4);
		const int32x4x2_t plane12IntermediateResults2B_s_32x4x2 = vzipq_s32(plane12IntermediateResults2B_s_32x4, plane12IntermediateResults2B_s_32x4);


		// now, we can determine the upper results

		const int32x4_t resultUpper0A_s_32x4 = vaddq_s32(plane12IntermediateResults0A_s_32x4x2.val[0], sPlaneUpperMultiplied0A_s_32x4);
		const int32x4_t resultUpper0B_s_32x4 = vaddq_s32(plane12IntermediateResults0A_s_32x4x2.val[1], sPlaneUpperMultiplied0B_s_32x4);
		const int32x4_t resultUpper0C_s_32x4 = vaddq_s32(plane12IntermediateResults0B_s_32x4x2.val[0], sPlaneUpperMultiplied0C_s_32x4);
		const int32x4_t resultUpper0D_s_32x4 = vaddq_s32(plane12IntermediateResults0B_s_32x4x2.val[1], sPlaneUpperMultiplied0D_s_32x4);

		const int32x4_t resultUpper1A_s_32x4 = vaddq_s32(plane12IntermediateResults1A_s_32x4x2.val[0], sPlaneUpperMultiplied1A_s_32x4);
		const int32x4_t resultUpper1B_s_32x4 = vaddq_s32(plane12IntermediateResults1A_s_32x4x2.val[1], sPlaneUpperMultiplied1B_s_32x4);
		const int32x4_t resultUpper1C_s_32x4 = vaddq_s32(plane12IntermediateResults1B_s_32x4x2.val[0], sPlaneUpperMultiplied1C_s_32x4);
		const int32x4_t resultUpper1D_s_32x4 = vaddq_s32(plane12IntermediateResults1B_s_32x4x2.val[1], sPlaneUpperMultiplied1D_s_32x4);

		const int32x4_t resultUpper2A_s_32x4 = vaddq_s32(plane12IntermediateResults2A_s_32x4x2.val[0], sPlaneUpperMultiplied2A_s_32x4);
		const int32x4_t resultUpper2B_s_32x4 = vaddq_s32(plane12IntermediateResults2A_s_32x4x2.val[1], sPlaneUpperMultiplied2B_s_32x4);
		const int32x4_t resultUpper2C_s_32x4 = vaddq_s32(plane12IntermediateResults2B_s_32x4x2.val[0], sPlaneUpperMultiplied2C_s_32x4);
		const int32x4_t resultUpper2D_s_32x4 = vaddq_s32(plane12IntermediateResults2B_s_32x4x2.val[1], sPlaneUpperMultiplied2D_s_32x4);


		uint8x16x3_t resultsUpper_u_8x16x3;

		// saturated narrow signed to unsigned
		resultsUpper_u_8x16x3.val[0] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper0A_s_32x4, 10), vqrshrun_n_s32(resultUpper0B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper0C_s_32x4, 10), vqrshrun_n_s32(resultUpper0D_s_32x4, 10))));
		resultsUpper_u_8x16x3.val[1] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper1A_s_32x4, 10), vqrshrun_n_s32(resultUpper1B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper1C_s_32x4, 10), vqrshrun_n_s32(resultUpper1D_s_32x4, 10))));
		resultsUpper_u_8x16x3.val[2] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper2A_s_32x4, 10), vqrshrun_n_s32(resultUpper2B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultUpper2C_s_32x4, 10), vqrshrun_n_s32(resultUpper2D_s_32x4, 10))));

		// and we can store the result
		vst3q_u8(tUpper, resultsUpper_u_8x16x3);


		// now, we can determine the lower results

		const int32x4_t resultLower0A_s_32x4 = vaddq_s32(plane12IntermediateResults0A_s_32x4x2.val[0], sPlaneLowerMultiplied0A_s_32x4);
		const int32x4_t resultLower0B_s_32x4 = vaddq_s32(plane12IntermediateResults0A_s_32x4x2.val[1], sPlaneLowerMultiplied0B_s_32x4);
		const int32x4_t resultLower0C_s_32x4 = vaddq_s32(plane12IntermediateResults0B_s_32x4x2.val[0], sPlaneLowerMultiplied0C_s_32x4);
		const int32x4_t resultLower0D_s_32x4 = vaddq_s32(plane12IntermediateResults0B_s_32x4x2.val[1], sPlaneLowerMultiplied0D_s_32x4);

		const int32x4_t resultLower1A_s_32x4 = vaddq_s32(plane12IntermediateResults1A_s_32x4x2.val[0], sPlaneLowerMultiplied1A_s_32x4);
		const int32x4_t resultLower1B_s_32x4 = vaddq_s32(plane12IntermediateResults1A_s_32x4x2.val[1], sPlaneLowerMultiplied1B_s_32x4);
		const int32x4_t resultLower1C_s_32x4 = vaddq_s32(plane12IntermediateResults1B_s_32x4x2.val[0], sPlaneLowerMultiplied1C_s_32x4);
		const int32x4_t resultLower1D_s_32x4 = vaddq_s32(plane12IntermediateResults1B_s_32x4x2.val[1], sPlaneLowerMultiplied1D_s_32x4);

		const int32x4_t resultLower2A_s_32x4 = vaddq_s32(plane12IntermediateResults2A_s_32x4x2.val[0], sPlaneLowerMultiplied2A_s_32x4);
		const int32x4_t resultLower2B_s_32x4 = vaddq_s32(plane12IntermediateResults2A_s_32x4x2.val[1], sPlaneLowerMultiplied2B_s_32x4);
		const int32x4_t resultLower2C_s_32x4 = vaddq_s32(plane12IntermediateResults2B_s_32x4x2.val[0], sPlaneLowerMultiplied2C_s_32x4);
		const int32x4_t resultLower2D_s_32x4 = vaddq_s32(plane12IntermediateResults2B_s_32x4x2.val[1], sPlaneLowerMultiplied2D_s_32x4);

		uint8x16x3_t resultsLower_u_8x16x3;

		// saturated narrow signed to unsigned
		resultsLower_u_8x16x3.val[0] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultLower0A_s_32x4, 10), vqrshrun_n_s32(resultLower0B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultLower0C_s_32x4, 10), vqrshrun_n_s32(resultLower0D_s_32x4, 10))));
		resultsLower_u_8x16x3.val[1] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultLower1A_s_32x4, 10), vqrshrun_n_s32(resultLower1B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultLower1C_s_32x4, 10), vqrshrun_n_s32(resultLower1D_s_32x4, 10))));
		resultsLower_u_8x16x3.val[2] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultLower2A_s_32x4, 10), vqrshrun_n_s32(resultLower2B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(resultLower2C_s_32x4, 10), vqrshrun_n_s32(resultLower2D_s_32x4, 10))));

		// and we can store the result
		vst3q_u8(tLower, resultsLower_u_8x16x3);

		sPlane0Upper += blockSize;
		sPlane1 += blockSize / 2u;
		sPlane2 += blockSize / 2u;

		tUpper += blockSize * 3u;
		tLower += blockSize * 3u;
	}

#elif defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	if (blocks != 0u)
	{
		const __m128i factorChannel00_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel00_1024));
		const __m128i factorChannel10_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel10_1024));
		const __m128i factorChannel20_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel20_1024));

		const __m128i factorChannel01_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel01_1024));
		const __m128i factorChannel11_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel11_1024));
		const __m128i factorChannel21_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel21_1024));

		const __m128i factorChannel02_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel02_1024));
		const __m128i factorChannel12_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel12_1024));
		const __m128i factorChannel22_1024_s_16x8 = _mm_set1_epi16(int16_t(factorChannel22_1024));

		const __m128i bias0_1024_s_32x4 = _mm_set1_epi32(bias0 * 1024);
		const __m128i bias1_1024_s_32x4 = _mm_set1_epi32(bias1 * 1024);
		const __m128i bias2_1024_s_32x4 = _mm_set1_epi32(bias2 * 1024);

		const __m128i constant_255_s_16x8 = _mm_set1_epi16(int16_t(255));

		const __m128i shuffleMask_lower = SSE::set128i(0xFFFFFFFFFFFFFFFFull, 0x0E0C0A0806040200ull);
		const __m128i shuffleMask_upper = SSE::set128i(0x0E0C0A0806040200ull, 0xFFFFFFFFFFFFFFFFull);

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			const __m128i sPlane0Upper_u_8x16 = SSE::load128i(sPlane0Upper); // upper row
			const __m128i sPlane0Lower_u_8x16 = SSE::load128i(sPlane0Upper + sourcePlane0StrideElements); // lower row
			const __m128i source1_u_16x8 = _mm_unpacklo_epi8(SSE::load128iLower64(sPlane1), _mm_setzero_si128());
			const __m128i source2_u_16x8 = _mm_unpacklo_epi8(SSE::load128iLower64(sPlane2), _mm_setzero_si128());

			const __m128i source0Upper_A_s_16x8 = _mm_unpacklo_epi8(sPlane0Upper_u_8x16, _mm_setzero_si128());
			const __m128i source0Upper_B_s_16x8 = _mm_unpackhi_epi8(sPlane0Upper_u_8x16, _mm_setzero_si128());

			const __m128i source0Lower_A_s_16x8 = _mm_unpacklo_epi8(sPlane0Lower_u_8x16, _mm_setzero_si128());
			const __m128i source0Lower_B_s_16x8 = _mm_unpackhi_epi8(sPlane0Lower_u_8x16, _mm_setzero_si128());


			// now we apply the 3x3 matrix multiplication

			__m128i intermediateResultsLow_0_s_16x8 = _mm_mullo_epi16(source1_u_16x8, factorChannel01_1024_s_16x8); // channel 1
			__m128i intermediateResultsHigh_0_s_16x8 = _mm_mulhi_epi16(source1_u_16x8, factorChannel01_1024_s_16x8);
			__m128i intermediateResultsLow_1_s_16x8 = _mm_mullo_epi16(source1_u_16x8, factorChannel11_1024_s_16x8);
			__m128i intermediateResultsHigh_1_s_16x8 = _mm_mulhi_epi16(source1_u_16x8, factorChannel11_1024_s_16x8);
			__m128i intermediateResultsLow_2_s_16x8 = _mm_mullo_epi16(source1_u_16x8, factorChannel21_1024_s_16x8);
			__m128i intermediateResultsHigh_2_s_16x8 = _mm_mulhi_epi16(source1_u_16x8, factorChannel21_1024_s_16x8);

			__m128i intermediateResults0_A_s_32x4 = _mm_unpacklo_epi16(intermediateResultsLow_0_s_16x8, intermediateResultsHigh_0_s_16x8);
			__m128i intermediateResults0_B_s_32x4 = _mm_unpackhi_epi16(intermediateResultsLow_0_s_16x8, intermediateResultsHigh_0_s_16x8);
			__m128i intermediateResults1_A_s_32x4 = _mm_unpacklo_epi16(intermediateResultsLow_1_s_16x8, intermediateResultsHigh_1_s_16x8);
			__m128i intermediateResults1_B_s_32x4 = _mm_unpackhi_epi16(intermediateResultsLow_1_s_16x8, intermediateResultsHigh_1_s_16x8);
			__m128i intermediateResults2_A_s_32x4 = _mm_unpacklo_epi16(intermediateResultsLow_2_s_16x8, intermediateResultsHigh_2_s_16x8);
			__m128i intermediateResults2_B_s_32x4 = _mm_unpackhi_epi16(intermediateResultsLow_2_s_16x8, intermediateResultsHigh_2_s_16x8);

			intermediateResultsLow_0_s_16x8 = _mm_mullo_epi16(source2_u_16x8, factorChannel02_1024_s_16x8); // channel 2
			intermediateResultsHigh_0_s_16x8 = _mm_mulhi_epi16(source2_u_16x8, factorChannel02_1024_s_16x8);
			intermediateResultsLow_1_s_16x8 = _mm_mullo_epi16(source2_u_16x8, factorChannel12_1024_s_16x8);
			intermediateResultsHigh_1_s_16x8 = _mm_mulhi_epi16(source2_u_16x8, factorChannel12_1024_s_16x8);
			intermediateResultsLow_2_s_16x8 = _mm_mullo_epi16(source2_u_16x8, factorChannel22_1024_s_16x8);
			intermediateResultsHigh_2_s_16x8 = _mm_mulhi_epi16(source2_u_16x8, factorChannel22_1024_s_16x8);

			intermediateResults0_A_s_32x4 = _mm_add_epi32(intermediateResults0_A_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_0_s_16x8, intermediateResultsHigh_0_s_16x8)); // channel 2
			intermediateResults0_B_s_32x4 = _mm_add_epi32(intermediateResults0_B_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_0_s_16x8, intermediateResultsHigh_0_s_16x8));
			intermediateResults1_A_s_32x4 = _mm_add_epi32(intermediateResults1_A_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_1_s_16x8, intermediateResultsHigh_1_s_16x8));
			intermediateResults1_B_s_32x4 = _mm_add_epi32(intermediateResults1_B_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_1_s_16x8, intermediateResultsHigh_1_s_16x8));
			intermediateResults2_A_s_32x4 = _mm_add_epi32(intermediateResults2_A_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_2_s_16x8, intermediateResultsHigh_2_s_16x8));
			intermediateResults2_B_s_32x4 = _mm_add_epi32(intermediateResults2_B_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_2_s_16x8, intermediateResultsHigh_2_s_16x8));

			intermediateResults0_A_s_32x4 = _mm_add_epi32(intermediateResults0_A_s_32x4, bias0_1024_s_32x4); // adding bias
			intermediateResults0_B_s_32x4 = _mm_add_epi32(intermediateResults0_B_s_32x4, bias0_1024_s_32x4);
			intermediateResults1_A_s_32x4 = _mm_add_epi32(intermediateResults1_A_s_32x4, bias1_1024_s_32x4);
			intermediateResults1_B_s_32x4 = _mm_add_epi32(intermediateResults1_B_s_32x4, bias1_1024_s_32x4);
			intermediateResults2_A_s_32x4 = _mm_add_epi32(intermediateResults2_A_s_32x4, bias2_1024_s_32x4);
			intermediateResults2_B_s_32x4 = _mm_add_epi32(intermediateResults2_B_s_32x4, bias2_1024_s_32x4);

			__m128i intermediateResults0_Aa_s_32x4 = _mm_unpacklo_epi32(intermediateResults0_A_s_32x4, intermediateResults0_A_s_32x4); // upsampling the results
			__m128i intermediateResults0_Ab_s_32x4 = _mm_unpackhi_epi32(intermediateResults0_A_s_32x4, intermediateResults0_A_s_32x4);
			__m128i intermediateResults0_Ba_s_32x4 = _mm_unpacklo_epi32(intermediateResults0_B_s_32x4, intermediateResults0_B_s_32x4);
			__m128i intermediateResults0_Bb_s_32x4 = _mm_unpackhi_epi32(intermediateResults0_B_s_32x4, intermediateResults0_B_s_32x4);
			__m128i intermediateResults1_Aa_s_32x4 = _mm_unpacklo_epi32(intermediateResults1_A_s_32x4, intermediateResults1_A_s_32x4);
			__m128i intermediateResults1_Ab_s_32x4 = _mm_unpackhi_epi32(intermediateResults1_A_s_32x4, intermediateResults1_A_s_32x4);
			__m128i intermediateResults1_Ba_s_32x4 = _mm_unpacklo_epi32(intermediateResults1_B_s_32x4, intermediateResults1_B_s_32x4);
			__m128i intermediateResults1_Bb_s_32x4 = _mm_unpackhi_epi32(intermediateResults1_B_s_32x4, intermediateResults1_B_s_32x4);
			__m128i intermediateResults2_Aa_s_32x4 = _mm_unpacklo_epi32(intermediateResults2_A_s_32x4, intermediateResults2_A_s_32x4);
			__m128i intermediateResults2_Ab_s_32x4 = _mm_unpackhi_epi32(intermediateResults2_A_s_32x4, intermediateResults2_A_s_32x4);
			__m128i intermediateResults2_Ba_s_32x4 = _mm_unpacklo_epi32(intermediateResults2_B_s_32x4, intermediateResults2_B_s_32x4);
			__m128i intermediateResults2_Bb_s_32x4 = _mm_unpackhi_epi32(intermediateResults2_B_s_32x4, intermediateResults2_B_s_32x4);

			intermediateResultsLow_0_s_16x8 = _mm_mullo_epi16(source0Upper_A_s_16x8, factorChannel00_1024_s_16x8); // channel 0, upper row
			intermediateResultsHigh_0_s_16x8 = _mm_mulhi_epi16(source0Upper_A_s_16x8, factorChannel00_1024_s_16x8);
			intermediateResultsLow_1_s_16x8 = _mm_mullo_epi16(source0Upper_A_s_16x8, factorChannel10_1024_s_16x8);
			intermediateResultsHigh_1_s_16x8 = _mm_mulhi_epi16(source0Upper_A_s_16x8, factorChannel10_1024_s_16x8);
			intermediateResultsLow_2_s_16x8 = _mm_mullo_epi16(source0Upper_A_s_16x8, factorChannel20_1024_s_16x8);
			intermediateResultsHigh_2_s_16x8 = _mm_mulhi_epi16(source0Upper_A_s_16x8, factorChannel20_1024_s_16x8);

			const __m128i intermediateResults0Upper_Aa_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults0_Aa_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_0_s_16x8, intermediateResultsHigh_0_s_16x8)));
			const __m128i intermediateResults0Upper_Ab_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults0_Ab_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_0_s_16x8, intermediateResultsHigh_0_s_16x8)));
			const __m128i intermediateResults1Upper_Aa_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults1_Aa_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_1_s_16x8, intermediateResultsHigh_1_s_16x8)));
			const __m128i intermediateResults1Upper_Ab_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults1_Ab_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_1_s_16x8, intermediateResultsHigh_1_s_16x8)));
			const __m128i intermediateResults2Upper_Aa_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults2_Aa_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_2_s_16x8, intermediateResultsHigh_2_s_16x8)));
			const __m128i intermediateResults2Upper_Ab_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults2_Ab_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_2_s_16x8, intermediateResultsHigh_2_s_16x8)));

			intermediateResultsLow_0_s_16x8 = _mm_mullo_epi16(source0Upper_B_s_16x8, factorChannel00_1024_s_16x8);
			intermediateResultsHigh_0_s_16x8 = _mm_mulhi_epi16(source0Upper_B_s_16x8, factorChannel00_1024_s_16x8);
			intermediateResultsLow_1_s_16x8 = _mm_mullo_epi16(source0Upper_B_s_16x8, factorChannel10_1024_s_16x8);
			intermediateResultsHigh_1_s_16x8 = _mm_mulhi_epi16(source0Upper_B_s_16x8, factorChannel10_1024_s_16x8);
			intermediateResultsLow_2_s_16x8 = _mm_mullo_epi16(source0Upper_B_s_16x8, factorChannel20_1024_s_16x8);
			intermediateResultsHigh_2_s_16x8 = _mm_mulhi_epi16(source0Upper_B_s_16x8, factorChannel20_1024_s_16x8);

			const __m128i intermediateResults0Upper_Ba_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults0_Ba_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_0_s_16x8, intermediateResultsHigh_0_s_16x8)));
			const __m128i intermediateResults0Upper_Bb_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults0_Bb_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_0_s_16x8, intermediateResultsHigh_0_s_16x8)));
			const __m128i intermediateResults1Upper_Ba_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults1_Ba_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_1_s_16x8, intermediateResultsHigh_1_s_16x8)));
			const __m128i intermediateResults1Upper_Bb_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults1_Bb_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_1_s_16x8, intermediateResultsHigh_1_s_16x8)));
			const __m128i intermediateResults2Upper_Ba_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults2_Ba_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_2_s_16x8, intermediateResultsHigh_2_s_16x8)));
			const __m128i intermediateResults2Upper_Bb_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults2_Bb_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_2_s_16x8, intermediateResultsHigh_2_s_16x8)));


			intermediateResultsLow_0_s_16x8 = _mm_mullo_epi16(source0Lower_A_s_16x8, factorChannel00_1024_s_16x8); // channel 0, lower row
			intermediateResultsHigh_0_s_16x8 = _mm_mulhi_epi16(source0Lower_A_s_16x8, factorChannel00_1024_s_16x8);
			intermediateResultsLow_1_s_16x8 = _mm_mullo_epi16(source0Lower_A_s_16x8, factorChannel10_1024_s_16x8);
			intermediateResultsHigh_1_s_16x8 = _mm_mulhi_epi16(source0Lower_A_s_16x8, factorChannel10_1024_s_16x8);
			intermediateResultsLow_2_s_16x8 = _mm_mullo_epi16(source0Lower_A_s_16x8, factorChannel20_1024_s_16x8);
			intermediateResultsHigh_2_s_16x8 = _mm_mulhi_epi16(source0Lower_A_s_16x8, factorChannel20_1024_s_16x8);

			const __m128i intermediateResults0Lower_Aa_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults0_Aa_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_0_s_16x8, intermediateResultsHigh_0_s_16x8)));
			const __m128i intermediateResults0Lower_Ab_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults0_Ab_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_0_s_16x8, intermediateResultsHigh_0_s_16x8)));
			const __m128i intermediateResults1Lower_Aa_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults1_Aa_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_1_s_16x8, intermediateResultsHigh_1_s_16x8)));
			const __m128i intermediateResults1Lower_Ab_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults1_Ab_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_1_s_16x8, intermediateResultsHigh_1_s_16x8)));
			const __m128i intermediateResults2Lower_Aa_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults2_Aa_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_2_s_16x8, intermediateResultsHigh_2_s_16x8)));
			const __m128i intermediateResults2Lower_Ab_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults2_Ab_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_2_s_16x8, intermediateResultsHigh_2_s_16x8)));

			intermediateResultsLow_0_s_16x8 = _mm_mullo_epi16(source0Lower_B_s_16x8, factorChannel00_1024_s_16x8);
			intermediateResultsHigh_0_s_16x8 = _mm_mulhi_epi16(source0Lower_B_s_16x8, factorChannel00_1024_s_16x8);
			intermediateResultsLow_1_s_16x8 = _mm_mullo_epi16(source0Lower_B_s_16x8, factorChannel10_1024_s_16x8);
			intermediateResultsHigh_1_s_16x8 = _mm_mulhi_epi16(source0Lower_B_s_16x8, factorChannel10_1024_s_16x8);
			intermediateResultsLow_2_s_16x8 = _mm_mullo_epi16(source0Lower_B_s_16x8, factorChannel20_1024_s_16x8);
			intermediateResultsHigh_2_s_16x8 = _mm_mulhi_epi16(source0Lower_B_s_16x8, factorChannel20_1024_s_16x8);

			const __m128i intermediateResults0Lower_Ba_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults0_Ba_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_0_s_16x8, intermediateResultsHigh_0_s_16x8)));
			const __m128i intermediateResults0Lower_Bb_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults0_Bb_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_0_s_16x8, intermediateResultsHigh_0_s_16x8)));
			const __m128i intermediateResults1Lower_Ba_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults1_Ba_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_1_s_16x8, intermediateResultsHigh_1_s_16x8)));
			const __m128i intermediateResults1Lower_Bb_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults1_Bb_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_1_s_16x8, intermediateResultsHigh_1_s_16x8)));
			const __m128i intermediateResults2Lower_Ba_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults2_Ba_s_32x4, _mm_unpacklo_epi16(intermediateResultsLow_2_s_16x8, intermediateResultsHigh_2_s_16x8)));
			const __m128i intermediateResults2Lower_Bb_s_32x4 = _mm_max_epi32(_mm_setzero_si128(), _mm_add_epi32(intermediateResults2_Bb_s_32x4, _mm_unpackhi_epi16(intermediateResultsLow_2_s_16x8, intermediateResultsHigh_2_s_16x8)));


			// shifting by 10 bits, and combining neighboring blocks

			const __m128i intermediateResults0Upper_A_s_16x8 = _mm_min_epi16(_mm_packus_epi32(_mm_srli_epi32(intermediateResults0Upper_Aa_s_32x4, 10), _mm_srli_epi32(intermediateResults0Upper_Ab_s_32x4, 10)), constant_255_s_16x8);
			const __m128i intermediateResults1Upper_A_s_16x8 = _mm_min_epi16(_mm_packus_epi32(_mm_srli_epi32(intermediateResults1Upper_Aa_s_32x4, 10), _mm_srli_epi32(intermediateResults1Upper_Ab_s_32x4, 10)), constant_255_s_16x8);
			const __m128i intermediateResults2Upper_A_s_16x8 = _mm_min_epi16(_mm_packus_epi32(_mm_srli_epi32(intermediateResults2Upper_Aa_s_32x4, 10), _mm_srli_epi32(intermediateResults2Upper_Ab_s_32x4, 10)), constant_255_s_16x8);

			const __m128i intermediateResults0Upper_B_s_16x8 = _mm_min_epi16(_mm_packus_epi32(_mm_srli_epi32(intermediateResults0Upper_Ba_s_32x4, 10), _mm_srli_epi32(intermediateResults0Upper_Bb_s_32x4, 10)), constant_255_s_16x8);
			const __m128i intermediateResults1Upper_B_s_16x8 = _mm_min_epi16(_mm_packus_epi32(_mm_srli_epi32(intermediateResults1Upper_Ba_s_32x4, 10), _mm_srli_epi32(intermediateResults1Upper_Bb_s_32x4, 10)), constant_255_s_16x8);
			const __m128i intermediateResults2Upper_B_s_16x8 = _mm_min_epi16(_mm_packus_epi32(_mm_srli_epi32(intermediateResults2Upper_Ba_s_32x4, 10), _mm_srli_epi32(intermediateResults2Upper_Bb_s_32x4, 10)), constant_255_s_16x8);

			const __m128i intermediateResults0Lower_A_s_16x8 = _mm_min_epi16(_mm_packus_epi32(_mm_srli_epi32(intermediateResults0Lower_Aa_s_32x4, 10), _mm_srli_epi32(intermediateResults0Lower_Ab_s_32x4, 10)), constant_255_s_16x8);
			const __m128i intermediateResults1Lower_A_s_16x8 = _mm_min_epi16(_mm_packus_epi32(_mm_srli_epi32(intermediateResults1Lower_Aa_s_32x4, 10), _mm_srli_epi32(intermediateResults1Lower_Ab_s_32x4, 10)), constant_255_s_16x8);
			const __m128i intermediateResults2Lower_A_s_16x8 = _mm_min_epi16(_mm_packus_epi32(_mm_srli_epi32(intermediateResults2Lower_Aa_s_32x4, 10), _mm_srli_epi32(intermediateResults2Lower_Ab_s_32x4, 10)), constant_255_s_16x8);

			const __m128i intermediateResults0Lower_B_s_16x8 = _mm_min_epi16(_mm_packus_epi32(_mm_srli_epi32(intermediateResults0Lower_Ba_s_32x4, 10), _mm_srli_epi32(intermediateResults0Lower_Bb_s_32x4, 10)), constant_255_s_16x8);
			const __m128i intermediateResults1Lower_B_s_16x8 = _mm_min_epi16(_mm_packus_epi32(_mm_srli_epi32(intermediateResults1Lower_Ba_s_32x4, 10), _mm_srli_epi32(intermediateResults1Lower_Bb_s_32x4, 10)), constant_255_s_16x8);
			const __m128i intermediateResults2Lower_B_s_16x8 = _mm_min_epi16(_mm_packus_epi32(_mm_srli_epi32(intermediateResults2Lower_Ba_s_32x4, 10), _mm_srli_epi32(intermediateResults2Lower_Bb_s_32x4, 10)), constant_255_s_16x8);


			// writing upper results

			const __m128i result0Upper_u_8x16 = _mm_or_si128(_mm_shuffle_epi8(intermediateResults0Upper_A_s_16x8, shuffleMask_lower), _mm_shuffle_epi8(intermediateResults0Upper_B_s_16x8, shuffleMask_upper));
			const __m128i result1Upper_u_8x16 = _mm_or_si128(_mm_shuffle_epi8(intermediateResults1Upper_A_s_16x8, shuffleMask_lower), _mm_shuffle_epi8(intermediateResults1Upper_B_s_16x8, shuffleMask_upper));
			const __m128i result2Upper_u_8x16 = _mm_or_si128(_mm_shuffle_epi8(intermediateResults2Upper_A_s_16x8, shuffleMask_lower), _mm_shuffle_epi8(intermediateResults2Upper_B_s_16x8, shuffleMask_upper));

			__m128i interleavedA_128, interleavedB_128, interleavedC_128;
			SSE::interleave3Channel8Bit48Elements(result0Upper_u_8x16, result1Upper_u_8x16, result2Upper_u_8x16, interleavedA_128, interleavedB_128, interleavedC_128);

			SSE::store128i(interleavedA_128, tUpper + 0);
			SSE::store128i(interleavedB_128, tUpper + 16);
			SSE::store128i(interleavedC_128, tUpper + 32);

			const __m128i result0Lower_u_8x16 = _mm_or_si128(_mm_shuffle_epi8(intermediateResults0Lower_A_s_16x8, shuffleMask_lower), _mm_shuffle_epi8(intermediateResults0Lower_B_s_16x8, shuffleMask_upper));
			const __m128i result1Lower_u_8x16 = _mm_or_si128(_mm_shuffle_epi8(intermediateResults1Lower_A_s_16x8, shuffleMask_lower), _mm_shuffle_epi8(intermediateResults1Lower_B_s_16x8, shuffleMask_upper));
			const __m128i result2Lower_u_8x16 = _mm_or_si128(_mm_shuffle_epi8(intermediateResults2Lower_A_s_16x8, shuffleMask_lower), _mm_shuffle_epi8(intermediateResults2Lower_B_s_16x8, shuffleMask_upper));

			SSE::interleave3Channel8Bit48Elements(result0Lower_u_8x16, result1Lower_u_8x16, result2Lower_u_8x16, interleavedA_128, interleavedB_128, interleavedC_128);

			SSE::store128i(interleavedA_128, tLower + 0);
			SSE::store128i(interleavedB_128, tLower + 16);
			SSE::store128i(interleavedC_128, tLower + 32);

			sPlane0Upper += blockSize;
			sPlane1 += blockSize / 2u;
			sPlane2 += blockSize / 2u;

			tUpper += blockSize * 3u;
			tLower += blockSize * 3u;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sPlane0Upper != sPlane0UpperEnd)
	{
		ocean_assert(sPlane0Upper < sPlane0UpperEnd);

		const int intermediate0 = sPlane1[0] * int16_t(factorChannel01_1024) + sPlane2[0] * int16_t(factorChannel02_1024);
		const int intermediate1 = sPlane1[0] * int16_t(factorChannel11_1024) + sPlane2[0] * int16_t(factorChannel12_1024);
		const int intermediate2 = sPlane1[0] * int16_t(factorChannel21_1024) + sPlane2[0] * int16_t(factorChannel22_1024);

		// first and second upper pixel

		tUpper[0] = (uint8_t)(minmax<int>(0, (sPlane0Upper[0] * int16_t(factorChannel00_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		tUpper[1] = (uint8_t)(minmax<int>(0, (sPlane0Upper[0] * int16_t(factorChannel10_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		tUpper[2] = (uint8_t)(minmax<int>(0, (sPlane0Upper[0] * int16_t(factorChannel20_1024) + intermediate2) / 1024 + int16_t(bias2), 255));

		tUpper[3] = (uint8_t)(minmax<int>(0, (sPlane0Upper[1] * int16_t(factorChannel00_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		tUpper[4] = (uint8_t)(minmax<int>(0, (sPlane0Upper[1] * int16_t(factorChannel10_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		tUpper[5] = (uint8_t)(minmax<int>(0, (sPlane0Upper[1] * int16_t(factorChannel20_1024) + intermediate2) / 1024 + int16_t(bias2), 255));


		// first and second lower pixel

		tLower[0] = (uint8_t)(minmax<int>(0, (sPlane0Upper[sourcePlane0StrideElements + 0] * int16_t(factorChannel00_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		tLower[1] = (uint8_t)(minmax<int>(0, (sPlane0Upper[sourcePlane0StrideElements + 0] * int16_t(factorChannel10_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		tLower[2] = (uint8_t)(minmax<int>(0, (sPlane0Upper[sourcePlane0StrideElements + 0] * int16_t(factorChannel20_1024) + intermediate2) / 1024 + int16_t(bias2), 255));

		tLower[3] = (uint8_t)(minmax<int>(0, (sPlane0Upper[sourcePlane0StrideElements + 1] * int16_t(factorChannel00_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		tLower[4] = (uint8_t)(minmax<int>(0, (sPlane0Upper[sourcePlane0StrideElements + 1] * int16_t(factorChannel10_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		tLower[5] = (uint8_t)(minmax<int>(0, (sPlane0Upper[sourcePlane0StrideElements + 1] * int16_t(factorChannel20_1024) + intermediate2) / 1024 + int16_t(bias2), 255));

		sPlane0Upper += 2;
		sPlane1++;
		sPlane2++;

		tUpper += 2u * 3u;
		tLower += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(tUpper - width * 3u, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(tLower - width * 3u, width);
	}
}

template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
void FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	static_assert(tSourceChannelIndex0 < 3u && tSourceChannelIndex1 < 3u && tSourceChannelIndex2 < 3u, "Invalid source channels!");

	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);
	ocean_assert(multipleRowIndex < height);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: sourcePlane2PaddingElements
	// uint32_t: targetZippedPaddingElements

	const unsigned int sourcePlane0PaddingElements = (unsigned int)intOptions[0];
	const unsigned int sourcePlane1PaddingElements = (unsigned int)intOptions[1];
	const unsigned int sourcePlane2PaddingElements = (unsigned int)intOptions[2];
	const unsigned int targetZippedPaddingElements = (unsigned int)intOptions[3];

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);
	const uint8_t* sourcePlane2 = (const uint8_t*)(sources[2]);

	uint8_t* targetZipped = (uint8_t*)(targets[0]);

	const unsigned int width_2 = width / 2u;

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width_2 + sourcePlane1PaddingElements;
	const unsigned int sourcePlane2StrideElements = width_2 + sourcePlane2PaddingElements;
	const unsigned int targetZippedStrideElements = width * 3u + targetZippedPaddingElements;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* sPlane0Upper = sourcePlane0 + multipleRowIndex * sourcePlane0StrideElements;
	const uint8_t* sPlane1 = sourcePlane1 + (multipleRowIndex / 2u) * sourcePlane1StrideElements;
	const uint8_t* sPlane2 = sourcePlane2 + (multipleRowIndex / 2u) * sourcePlane2StrideElements;

	uint8_t* target = flipTarget ? (targetZipped + (height - multipleRowIndex - 1u) * targetZippedStrideElements) : targetZipped + multipleRowIndex * targetZippedStrideElements;

	const uint8_t* const sPlane0UpperEnd = sPlane0Upper + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16_t sPlane0Upper_u_8x16 = vld1q_u8(sPlane0Upper); // upper row

		const uint8x8_t sPlane1_u_8x8 = vld1_u8(sPlane1);
		const uint8x8_t sPlane2_u_8x8 = vld1_u8(sPlane2);
		const uint8x16_t sPlane12_u_8x16 = vcombine_u8(sPlane1_u_8x8, sPlane2_u_8x8);

		const uint8x16x2_t sPlane12_u_8x16x2 = vzipq_u8(sPlane12_u_8x16, sPlane12_u_8x16);

		uint8x16x3_t resultsUpper_u_8x16x3;

		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			resultsUpper_u_8x16x3.val[0] = sPlane0Upper_u_8x16;
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			resultsUpper_u_8x16x3.val[0] = sPlane12_u_8x16x2.val[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex0 == 2u);

			resultsUpper_u_8x16x3.val[0] = sPlane12_u_8x16x2.val[1];
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			resultsUpper_u_8x16x3.val[1] = sPlane0Upper_u_8x16;
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			resultsUpper_u_8x16x3.val[1] = sPlane12_u_8x16x2.val[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex1 == 2u);

			resultsUpper_u_8x16x3.val[1] = sPlane12_u_8x16x2.val[1];
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			resultsUpper_u_8x16x3.val[2] = sPlane0Upper_u_8x16;
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			resultsUpper_u_8x16x3.val[2] = sPlane12_u_8x16x2.val[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex2 == 2u);

			resultsUpper_u_8x16x3.val[2] = sPlane12_u_8x16x2.val[1];
		}


		// and we can store the result
		vst3q_u8(target, resultsUpper_u_8x16x3);

		sPlane0Upper += blockSize;
		sPlane1 += blockSize / 2u;
		sPlane2 += blockSize / 2u;

		target += blockSize * 3u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sPlane0Upper != sPlane0UpperEnd)
	{
		ocean_assert(sPlane0Upper < sPlane0UpperEnd);

		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			target[0] = sPlane0Upper[0];
			target[3] = sPlane0Upper[1];
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			target[0] = sPlane1[0];
			target[3] = sPlane1[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex0 == 2u);

			target[0] = sPlane2[0];
			target[3] = sPlane2[0];
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			target[1] = sPlane0Upper[0];
			target[4] = sPlane0Upper[1];
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			target[1] = sPlane1[0];
			target[4] = sPlane1[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex1 == 2u);

			target[1] = sPlane2[0];
			target[4] = sPlane2[0];
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			target[2] = sPlane0Upper[0];
			target[5] = sPlane0Upper[1];
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			target[2] = sPlane1[0];
			target[5] = sPlane1[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex2 == 2u);

			target[2] = sPlane2[0];
			target[5] = sPlane2[0];
		}

		sPlane0Upper += 2;
		sPlane1++;
		sPlane2++;

		target += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(target - width * 3u, width);
	}
}

template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 0u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);

template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
void FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	static_assert(tSourceChannelIndex0 < 3u && tSourceChannelIndex1 < 3u && tSourceChannelIndex2 < 3u, "Invalid source channels!");

	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: sourcePlane2PaddingElements
	// uint32_t: targetZippedPaddingElements

	const unsigned int sourcePlane0PaddingElements = (unsigned int)intOptions[0];
	const unsigned int sourcePlane1PaddingElements = (unsigned int)intOptions[1];
	const unsigned int sourcePlane2PaddingElements = (unsigned int)intOptions[2];
	const unsigned int targetZippedPaddingElements = (unsigned int)intOptions[3];

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);
	const uint8_t* sourcePlane2 = (const uint8_t*)(sources[2]);

	uint8_t* targetZipped = (uint8_t*)(targets[0]);

	const unsigned int width_2 = width / 2u;

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width_2 + sourcePlane1PaddingElements;
	const unsigned int sourcePlane2StrideElements = width_2 + sourcePlane2PaddingElements;
	const unsigned int targetZippedStrideElements = width * 3u + targetZippedPaddingElements;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* sPlane0Upper = sourcePlane0 + multipleRowIndex * 2u * sourcePlane0StrideElements;
	const uint8_t* sPlane1 = sourcePlane1 + multipleRowIndex * sourcePlane1StrideElements;
	const uint8_t* sPlane2 = sourcePlane2 + multipleRowIndex * sourcePlane2StrideElements;

	uint8_t* tUpper = flipTarget ? (targetZipped + (height - multipleRowIndex * 2u - 1u) * targetZippedStrideElements) : targetZipped + multipleRowIndex * 2u * targetZippedStrideElements;
	uint8_t* tLower = flipTarget ? tUpper - targetZippedStrideElements : tUpper + targetZippedStrideElements;

	const uint8_t* const sPlane0UpperEnd = sPlane0Upper + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16_t sPlane0Upper_u_8x16 = vld1q_u8(sPlane0Upper); // upper row
		const uint8x16_t sPlane0Lower_u_8x16 = vld1q_u8(sPlane0Upper + sourcePlane0StrideElements); // lower row

		const uint8x8_t sPlane1_u_8x8 = vld1_u8(sPlane1);
		const uint8x8_t sPlane2_u_8x8 = vld1_u8(sPlane2);
		const uint8x16_t sPlane12_u_8x16 = vcombine_u8(sPlane1_u_8x8, sPlane2_u_8x8);

		const uint8x16x2_t sPlane12_u_8x16x2 = vzipq_u8(sPlane12_u_8x16, sPlane12_u_8x16);

		uint8x16x3_t resultsUpper_u_8x16x3;
		uint8x16x3_t resultsLower_u_8x16x3;

		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			resultsUpper_u_8x16x3.val[0] = sPlane0Upper_u_8x16;
			resultsLower_u_8x16x3.val[0] = sPlane0Lower_u_8x16;
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			resultsUpper_u_8x16x3.val[0] = sPlane12_u_8x16x2.val[0];
			resultsLower_u_8x16x3.val[0] = sPlane12_u_8x16x2.val[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex0 == 2u);

			resultsUpper_u_8x16x3.val[0] = sPlane12_u_8x16x2.val[1];
			resultsLower_u_8x16x3.val[0] = sPlane12_u_8x16x2.val[1];
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			resultsUpper_u_8x16x3.val[1] = sPlane0Upper_u_8x16;
			resultsLower_u_8x16x3.val[1] = sPlane0Lower_u_8x16;
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			resultsUpper_u_8x16x3.val[1] = sPlane12_u_8x16x2.val[0];
			resultsLower_u_8x16x3.val[1] = sPlane12_u_8x16x2.val[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex1 == 2u);

			resultsUpper_u_8x16x3.val[1] = sPlane12_u_8x16x2.val[1];
			resultsLower_u_8x16x3.val[1] = sPlane12_u_8x16x2.val[1];
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			resultsUpper_u_8x16x3.val[2] = sPlane0Upper_u_8x16;
			resultsLower_u_8x16x3.val[2] = sPlane0Lower_u_8x16;
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			resultsUpper_u_8x16x3.val[2] = sPlane12_u_8x16x2.val[0];
			resultsLower_u_8x16x3.val[2] = sPlane12_u_8x16x2.val[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex2 == 2u);

			resultsUpper_u_8x16x3.val[2] = sPlane12_u_8x16x2.val[1];
			resultsLower_u_8x16x3.val[2] = sPlane12_u_8x16x2.val[1];
		}


		// and we can store the result
		vst3q_u8(tUpper, resultsUpper_u_8x16x3);
		vst3q_u8(tLower, resultsLower_u_8x16x3);

		sPlane0Upper += blockSize;
		sPlane1 += blockSize / 2u;
		sPlane2 += blockSize / 2u;

		tUpper += blockSize * 3u;
		tLower += blockSize * 3u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sPlane0Upper != sPlane0UpperEnd)
	{
		ocean_assert(sPlane0Upper < sPlane0UpperEnd);

		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			tUpper[0] = sPlane0Upper[0];
			tUpper[3] = sPlane0Upper[1];

			tLower[0] = sPlane0Upper[sourcePlane0StrideElements + 0u];
			tLower[3] = sPlane0Upper[sourcePlane0StrideElements + 1u];
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			tUpper[0] = sPlane1[0];
			tUpper[3] = sPlane1[0];

			tLower[0] = sPlane1[0];
			tLower[3] = sPlane1[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex0 == 2u);

			tUpper[0] = sPlane2[0];
			tUpper[3] = sPlane2[0];

			tLower[0] = sPlane2[0];
			tLower[3] = sPlane2[0];
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			tUpper[1] = sPlane0Upper[0];
			tUpper[4] = sPlane0Upper[1];

			tLower[1] = sPlane0Upper[sourcePlane0StrideElements + 0u];
			tLower[4] = sPlane0Upper[sourcePlane0StrideElements + 1u];
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			tUpper[1] = sPlane1[0];
			tUpper[4] = sPlane1[0];

			tLower[1] = sPlane1[0];
			tLower[4] = sPlane1[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex1 == 2u);

			tUpper[1] = sPlane2[0];
			tUpper[4] = sPlane2[0];

			tLower[1] = sPlane2[0];
			tLower[4] = sPlane2[0];
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			tUpper[2] = sPlane0Upper[0];
			tUpper[5] = sPlane0Upper[1];

			tLower[2] = sPlane0Upper[sourcePlane0StrideElements + 0u];
			tLower[5] = sPlane0Upper[sourcePlane0StrideElements + 1u];
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			tUpper[2] = sPlane1[0];
			tUpper[5] = sPlane1[0];

			tLower[2] = sPlane1[0];
			tLower[5] = sPlane1[0];
		}
		else
		{
			ocean_assert(tSourceChannelIndex2 == 2u);

			tUpper[2] = sPlane2[0];
			tUpper[5] = sPlane2[0];

			tLower[2] = sPlane2[0];
			tLower[5] = sPlane2[0];
		}

		sPlane0Upper += 2;
		sPlane1++;
		sPlane2++;

		tUpper += 2u * 3u;
		tLower += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(tUpper - width * 3u, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(tLower - width * 3u, width);
	}
}

template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 0u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);

template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
void FrameConverter::mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	static_assert(tSourceChannelIndex0 < 3u && tSourceChannelIndex1 < 3u && tSourceChannelIndex2 < 3u, "Invalid source channels!");

	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 2u && height % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourceZippedPaddingElements
	// uint32_t: targetPlane0PaddingElements
	// uint32_t: targetPlane1PaddingElements
	// uint32_t: targetPlane2PaddingElements

	const unsigned int sourceZippedPaddingElements = (unsigned int)intOptions[0];
	const unsigned int targetPlane0PaddingElements = (unsigned int)intOptions[1];
	const unsigned int targetPlane1PaddingElements = (unsigned int)intOptions[2];
	const unsigned int targetPlane2PaddingElements = (unsigned int)intOptions[3];

	const uint8_t* sourceZipped = (const uint8_t*)(sources[0]);

	uint8_t* targetPlane0 = (uint8_t*)(targets[0]);
	uint8_t* targetPlane1 = (uint8_t*)(targets[1]);
	uint8_t* targetPlane2 = (uint8_t*)(targets[2]);

	const unsigned int width_2 = width / 2u;
	const unsigned int height_2 = height / 2u;

	const unsigned int sourceZippedStrideElements = width * 3u + sourceZippedPaddingElements;
	const unsigned int targetPlane0StrideElements = width + targetPlane0PaddingElements;
	const unsigned int targetPlane1StrideElements = width_2 + targetPlane1PaddingElements;
	const unsigned int targetPlane2StrideElements = width_2 + targetPlane2PaddingElements;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* sZippedUpper = sourceZipped + multipleRowIndex * 2u * sourceZippedStrideElements;

	uint8_t* tPlane0Upper = flipTarget ? (targetPlane0 + (height - multipleRowIndex * 2u - 1u) * targetPlane0StrideElements) : targetPlane0 + multipleRowIndex * 2u * targetPlane0StrideElements;
	uint8_t* tPlane0Lower = flipTarget ? tPlane0Upper - targetPlane0StrideElements : tPlane0Upper + targetPlane0StrideElements;

	uint8_t* tPlane1 = flipTarget ? (targetPlane1 + (height_2 - multipleRowIndex - 1u) * targetPlane1StrideElements) : targetPlane1 + multipleRowIndex * targetPlane1StrideElements;
	uint8_t* tPlane2 = flipTarget ? (targetPlane2 + (height_2 - multipleRowIndex - 1u) * targetPlane2StrideElements) : targetPlane2 + multipleRowIndex * targetPlane2StrideElements;

	const uint8_t* const sZippedUpperEnd = sZippedUpper + width * 3u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16x3_t sZippedUpper_u_8x16x3 = vld3q_u8(sZippedUpper); // upper row
		const uint8x16x3_t sZippedLower_u_8x16x3 = vld3q_u8(sZippedUpper + sourceZippedStrideElements); // upper row


		// target channel 0:

		uint8x16_t tPlane0Upper_u_8x16;
		uint8x16_t tPlane0Lower_u_8x16;

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			tPlane0Upper_u_8x16 = sZippedUpper_u_8x16x3.val[0];
			tPlane0Lower_u_8x16 = sZippedLower_u_8x16x3.val[0];
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			tPlane0Upper_u_8x16 = sZippedUpper_u_8x16x3.val[1];
			tPlane0Lower_u_8x16 = sZippedLower_u_8x16x3.val[1];
		}
		else
		{
			ocean_assert(tSourceChannelIndex0 == 2u);
			tPlane0Upper_u_8x16 = sZippedUpper_u_8x16x3.val[2];
			tPlane0Lower_u_8x16 = sZippedLower_u_8x16x3.val[2];
		}

		vst1q_u8(tPlane0Upper, tPlane0Upper_u_8x16);
		vst1q_u8(tPlane0Lower, tPlane0Lower_u_8x16);


		// target channel 1:

		uint8x8_t tPlane1_u_8x8;

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			tPlane1_u_8x8 = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(sZippedUpper_u_8x16x3.val[0], sZippedLower_u_8x16x3.val[0])), 1); // averaging in 2x2 neighborhood
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			tPlane1_u_8x8 = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(sZippedUpper_u_8x16x3.val[1], sZippedLower_u_8x16x3.val[1])), 1);
		}
		else
		{
			ocean_assert(tSourceChannelIndex1 == 2u);
			tPlane1_u_8x8 = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(sZippedUpper_u_8x16x3.val[2], sZippedLower_u_8x16x3.val[2])), 1);
		}

		vst1_u8(tPlane1, tPlane1_u_8x8);


		// target channel 2:

		uint8x8_t tPlane2_u_8x8;

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			tPlane2_u_8x8 = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(sZippedUpper_u_8x16x3.val[0], sZippedLower_u_8x16x3.val[0])), 1); // averaging in 2x2 neighborhood
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			tPlane2_u_8x8 = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(sZippedUpper_u_8x16x3.val[1], sZippedLower_u_8x16x3.val[1])), 1);
		}
		else
		{
			ocean_assert(tSourceChannelIndex2 == 2u);
			tPlane2_u_8x8 = vrshrn_n_u16(vpaddlq_u8(vrhaddq_u8(sZippedUpper_u_8x16x3.val[2], sZippedLower_u_8x16x3.val[2])), 1);
		}

		vst1_u8(tPlane2, tPlane2_u_8x8);

		sZippedUpper += blockSize * 3u;

		tPlane0Upper += blockSize;
		tPlane0Lower += blockSize;
		tPlane1 += blockSize / 2u;
		tPlane2 += blockSize / 2u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sZippedUpper != sZippedUpperEnd)
	{
		ocean_assert(sZippedUpper < sZippedUpperEnd);

		// target channel 0, upper 2 pixels:
		tPlane0Upper[0u] = sZippedUpper[tSourceChannelIndex0 + 0u];
		tPlane0Upper[1u] = sZippedUpper[tSourceChannelIndex0 + 3u];
		tPlane0Lower[0u] = sZippedUpper[tSourceChannelIndex0 + 0u + sourceZippedStrideElements];
		tPlane0Lower[1u] = sZippedUpper[tSourceChannelIndex0 + 3u + sourceZippedStrideElements];

		// target channel 1, averaging 2x2 pixels:
		tPlane1[0u] = (sZippedUpper[tSourceChannelIndex1 + 0u] + sZippedUpper[tSourceChannelIndex1 + 3u] + sZippedUpper[tSourceChannelIndex1 + 0u + sourceZippedStrideElements] + sZippedUpper[tSourceChannelIndex1 + 3u + sourceZippedStrideElements] + 2u) / 4u;

		// target channel 2, averaging 2x2 pixels
		tPlane2[0u] = (sZippedUpper[tSourceChannelIndex2 + 0u] + sZippedUpper[tSourceChannelIndex2 + 3u] + sZippedUpper[tSourceChannelIndex2 + 0u + sourceZippedStrideElements] + sZippedUpper[tSourceChannelIndex2 + 3u + sourceZippedStrideElements] + 2u) / 4u;

		sZippedUpper += 2u * 3u;

		tPlane0Upper += 2;
		tPlane0Lower += 2;

		tPlane1++;
		tPlane2++;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>(tPlane0Upper - width, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>(tPlane0Lower - width, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>(tPlane1 - width_2, width_2);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>(tPlane2 - width_2, width_2);
	}
}

template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel<0u, 1u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel<0u, 2u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel<1u, 0u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel<1u, 2u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel<2u, 0u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapTwoRows_1Plane3Channels_To_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_8BitPerChannel<2u, 1u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);

void FrameConverter::convertOneRow_3Planes1Channel_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: sourcePlane2PaddingElements
	// uint32_t: targetZippedPaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

	const unsigned int sourcePlane0PaddingElements = (unsigned int)intOptions[0];
	const unsigned int sourcePlane1PaddingElements = (unsigned int)intOptions[1];
	const unsigned int sourcePlane2PaddingElements = (unsigned int)intOptions[2];
	const unsigned int targetZippedPaddingElements = (unsigned int)intOptions[3];

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);
	const uint8_t* sourcePlane2 = (const uint8_t*)(sources[2]);

	uint8_t* targetZipped = (uint8_t*)(targets[0]);

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width + sourcePlane1PaddingElements;
	const unsigned int sourcePlane2StrideElements = width + sourcePlane2PaddingElements;
	const unsigned int targetZippedStrideElements = width * 3u + targetZippedPaddingElements;

	const int factorChannel00_64 = intOptions[4];
	const int factorChannel10_64 = intOptions[5];
	const int factorChannel20_64 = intOptions[6];

	const int factorChannel01_64 = intOptions[7];
	const int factorChannel11_64 = intOptions[8];
	const int factorChannel21_64 = intOptions[9];

	const int factorChannel02_64 = intOptions[10];
	const int factorChannel12_64 = intOptions[11];
	const int factorChannel22_64 = intOptions[12];

	ocean_assert(std::abs(factorChannel00_64 + factorChannel01_64 + factorChannel02_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel10_64 + factorChannel11_64 + factorChannel12_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel20_64 + factorChannel21_64 + factorChannel22_64) < 64 * 4);

	const int bias0 = intOptions[13];
	const int bias1 = intOptions[14];
	const int bias2 = intOptions[15];

	ocean_assert(bias0 >= 0 && bias0 <= 128);
	ocean_assert(bias1 >= 0 && bias1 <= 128);
	ocean_assert(bias2 >= 0 && bias2 <= 128);

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* sPlane0 = sourcePlane0 + multipleRowIndex * sourcePlane0StrideElements;
	const uint8_t* sPlane1 = sourcePlane1 + multipleRowIndex * sourcePlane1StrideElements;
	const uint8_t* sPlane2 = sourcePlane2 + multipleRowIndex * sourcePlane2StrideElements;

	uint8_t* target = flipTarget ? (targetZipped + (height - multipleRowIndex - 1u) * targetZippedStrideElements) : targetZipped + multipleRowIndex * targetZippedStrideElements;

	const uint8_t* const sPlane0End = sPlane0 + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const int16x8_t factorChannel00_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel00_64));
	const int16x8_t factorChannel10_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel10_64));
	const int16x8_t factorChannel20_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel20_64));

	const int16x8_t factorChannel01_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel01_64));
	const int16x8_t factorChannel11_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel11_64));
	const int16x8_t factorChannel21_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel21_64));

	const int16x8_t factorChannel02_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel02_64));
	const int16x8_t factorChannel12_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel12_64));
	const int16x8_t factorChannel22_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel22_64));

	const uint8x8_t bias0_u_8x8 = vdup_n_u8((uint8_t)(bias0));
	const uint8x8_t bias1_u_8x8 = vdup_n_u8((uint8_t)(bias1));
	const uint8x8_t bias2_u_8x8 = vdup_n_u8((uint8_t)(bias2));

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16_t source0_u_8x16 = vld1q_u8(sPlane0);
		const uint8x16_t source1_u_8x16 = vld1q_u8(sPlane1);
		const uint8x16_t source2_u_8x16 = vld1q_u8(sPlane2);

		// Y' = Y - bias0, U' = U - bias1, V' = V - bias2
		const int16x8_t source0_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source0_u_8x16), bias0_u_8x8));
		const int16x8_t source1_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source1_u_8x16), bias1_u_8x8));
		const int16x8_t source2_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source2_u_8x16), bias2_u_8x8));

		const int16x8_t source0_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source0_u_8x16), bias0_u_8x8));
		const int16x8_t source1_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source1_u_8x16), bias1_u_8x8));
		const int16x8_t source2_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source2_u_8x16), bias2_u_8x8));

		// now we apply the 3x3 matrix multiplication

		int16x8_t intermediateResults0_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel00_64_s_16x8);
		int16x8_t intermediateResults1_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel10_64_s_16x8);
		int16x8_t intermediateResults2_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel20_64_s_16x8);

		int16x8_t intermediateResults0_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel00_64_s_16x8);
		int16x8_t intermediateResults1_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel10_64_s_16x8);
		int16x8_t intermediateResults2_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel20_64_s_16x8);

		intermediateResults0_low_s_16x8 = vqaddq_s16(intermediateResults0_low_s_16x8, vmulq_s16(source1_low_s_16x8, factorChannel01_64_s_16x8)); // intermediateResults0 = saturated(intermediateResults0 + source10_low * factorChannel01)
		intermediateResults1_low_s_16x8 = vqaddq_s16(intermediateResults1_low_s_16x8, vmulq_s16(source1_low_s_16x8, factorChannel11_64_s_16x8));
		intermediateResults2_low_s_16x8 = vqaddq_s16(intermediateResults2_low_s_16x8, vmulq_s16(source1_low_s_16x8, factorChannel21_64_s_16x8));

		intermediateResults0_high_s_16x8 = vqaddq_s16(intermediateResults0_high_s_16x8, vmulq_s16(source1_high_s_16x8, factorChannel01_64_s_16x8));
		intermediateResults1_high_s_16x8 = vqaddq_s16(intermediateResults1_high_s_16x8, vmulq_s16(source1_high_s_16x8, factorChannel11_64_s_16x8));
		intermediateResults2_high_s_16x8 = vqaddq_s16(intermediateResults2_high_s_16x8, vmulq_s16(source1_high_s_16x8, factorChannel21_64_s_16x8));

		intermediateResults0_low_s_16x8 = vqaddq_s16(intermediateResults0_low_s_16x8, vmulq_s16(source2_low_s_16x8, factorChannel02_64_s_16x8));
		intermediateResults1_low_s_16x8 = vqaddq_s16(intermediateResults1_low_s_16x8, vmulq_s16(source2_low_s_16x8, factorChannel12_64_s_16x8));
		intermediateResults2_low_s_16x8 = vqaddq_s16(intermediateResults2_low_s_16x8, vmulq_s16(source2_low_s_16x8, factorChannel22_64_s_16x8));

		intermediateResults0_high_s_16x8 = vqaddq_s16(intermediateResults0_high_s_16x8, vmulq_s16(source2_high_s_16x8, factorChannel02_64_s_16x8));
		intermediateResults1_high_s_16x8 = vqaddq_s16(intermediateResults1_high_s_16x8, vmulq_s16(source2_high_s_16x8, factorChannel12_64_s_16x8));
		intermediateResults2_high_s_16x8 = vqaddq_s16(intermediateResults2_high_s_16x8, vmulq_s16(source2_high_s_16x8, factorChannel22_64_s_16x8));

		uint8x16x3_t results_u_8x16x3;

		// saturated narrow signed to unsigned, normalized by 2^6
		results_u_8x16x3.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults0_high_s_16x8, 6));
		results_u_8x16x3.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults1_high_s_16x8, 6));
		results_u_8x16x3.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults2_high_s_16x8, 6));

		// and we can store the result
		vst3q_u8(target, results_u_8x16x3);

		sPlane0 += blockSize;
		sPlane1 += blockSize;
		sPlane2 += blockSize;

		target += blockSize * 3u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sPlane0 != sPlane0End)
	{
		ocean_assert(sPlane0 < sPlane0End);

		const int16_t source0 = int16_t(*sPlane0) - int16_t(bias0);
		const int16_t source1 = int16_t(*sPlane1) - int16_t(bias1);
		const int16_t source2 = int16_t(*sPlane2) - int16_t(bias2);

		target[0] = (uint8_t)(minmax<int>(0, (source0 * int16_t(factorChannel00_64) + source1 * int16_t(factorChannel01_64) + source2 * int16_t(factorChannel02_64)) / 64, 255));
		target[1] = (uint8_t)(minmax<int>(0, (source0 * int16_t(factorChannel10_64) + source1 * int16_t(factorChannel11_64) + source2 * int16_t(factorChannel12_64)) / 64, 255));
		target[2] = (uint8_t)(minmax<int>(0, (source0 * int16_t(factorChannel20_64) + source1 * int16_t(factorChannel21_64) + source2 * int16_t(factorChannel22_64)) / 64, 255));

		sPlane0++;
		sPlane1++;
		sPlane2++;

		target += 3;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(target - width * 3u, width);
	}
}

void FrameConverter::convertOneRow_3Planes1Channel_To_1Plane4Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: sourcePlane2PaddingElements
	// uint32_t: targetZippedPaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2
	// uint32_t: channelValue3

	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2
	// t3 = channelValue3

	const unsigned int sourcePlane0PaddingElements = (unsigned int)intOptions[0];
	const unsigned int sourcePlane1PaddingElements = (unsigned int)intOptions[1];
	const unsigned int sourcePlane2PaddingElements = (unsigned int)intOptions[2];
	const unsigned int targetZippedPaddingElements = (unsigned int)intOptions[3];

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);
	const uint8_t* sourcePlane2 = (const uint8_t*)(sources[2]);

	uint8_t* targetZipped = (uint8_t*)(targets[0]);

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width + sourcePlane1PaddingElements;
	const unsigned int sourcePlane2StrideElements = width + sourcePlane2PaddingElements;
	const unsigned int targetZippedStrideElements = width * 4u + targetZippedPaddingElements;

	const int factorChannel00_64 = intOptions[4];
	const int factorChannel10_64 = intOptions[5];
	const int factorChannel20_64 = intOptions[6];

	const int factorChannel01_64 = intOptions[7];
	const int factorChannel11_64 = intOptions[8];
	const int factorChannel21_64 = intOptions[9];

	const int factorChannel02_64 = intOptions[10];
	const int factorChannel12_64 = intOptions[11];
	const int factorChannel22_64 = intOptions[12];

	ocean_assert(std::abs(factorChannel00_64 + factorChannel01_64 + factorChannel02_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel10_64 + factorChannel11_64 + factorChannel12_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel20_64 + factorChannel21_64 + factorChannel22_64) < 64 * 4);

	const int bias0 = intOptions[13];
	const int bias1 = intOptions[14];
	const int bias2 = intOptions[15];

	ocean_assert(bias0 >= 0 && bias0 <= 128);
	ocean_assert(bias1 >= 0 && bias1 <= 128);
	ocean_assert(bias2 >= 0 && bias2 <= 128);

	const int valueChannel3 = intOptions[16];
	ocean_assert(valueChannel3 >= 0 && valueChannel3 <= 255);

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* sPlane0 = sourcePlane0 + multipleRowIndex * sourcePlane0StrideElements;
	const uint8_t* sPlane1 = sourcePlane1 + multipleRowIndex * sourcePlane1StrideElements;
	const uint8_t* sPlane2 = sourcePlane2 + multipleRowIndex * sourcePlane2StrideElements;

	uint8_t* target = flipTarget ? (targetZipped + (height - multipleRowIndex - 1u) * targetZippedStrideElements) : targetZipped + multipleRowIndex * targetZippedStrideElements;

	const uint8_t* const sPlane0End = sPlane0 + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	const int16x8_t factorChannel00_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel00_64));
	const int16x8_t factorChannel10_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel10_64));
	const int16x8_t factorChannel20_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel20_64));

	const int16x8_t factorChannel01_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel01_64));
	const int16x8_t factorChannel11_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel11_64));
	const int16x8_t factorChannel21_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel21_64));

	const int16x8_t factorChannel02_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel02_64));
	const int16x8_t factorChannel12_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel12_64));
	const int16x8_t factorChannel22_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel22_64));

	const uint8x8_t bias0_u_8x8 = vdup_n_u8((uint8_t)(bias0));
	const uint8x8_t bias1_u_8x8 = vdup_n_u8((uint8_t)(bias1));
	const uint8x8_t bias2_u_8x8 = vdup_n_u8((uint8_t)(bias2));

	const uint8x16_t valueChannel3_u_8x16 = vdupq_n_u8((uint8_t)(valueChannel3));

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16_t source0_u_8x16 = vld1q_u8(sPlane0);
		const uint8x16_t source1_u_8x16 = vld1q_u8(sPlane1);
		const uint8x16_t source2_u_8x16 = vld1q_u8(sPlane2);

		// Y' = Y - bias0, U' = U - bias1, V' = V - bias2
		const int16x8_t source0_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source0_u_8x16), bias0_u_8x8));
		const int16x8_t source1_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source1_u_8x16), bias1_u_8x8));
		const int16x8_t source2_low_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source2_u_8x16), bias2_u_8x8));

		const int16x8_t source0_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source0_u_8x16), bias0_u_8x8));
		const int16x8_t source1_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source1_u_8x16), bias1_u_8x8));
		const int16x8_t source2_high_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source2_u_8x16), bias2_u_8x8));

		// now we apply the 3x3 matrix multiplication

		int16x8_t intermediateResults0_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel00_64_s_16x8);
		int16x8_t intermediateResults1_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel10_64_s_16x8);
		int16x8_t intermediateResults2_low_s_16x8 = vmulq_s16(source0_low_s_16x8, factorChannel20_64_s_16x8);

		int16x8_t intermediateResults0_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel00_64_s_16x8);
		int16x8_t intermediateResults1_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel10_64_s_16x8);
		int16x8_t intermediateResults2_high_s_16x8 = vmulq_s16(source0_high_s_16x8, factorChannel20_64_s_16x8);

		intermediateResults0_low_s_16x8 = vqaddq_s16(intermediateResults0_low_s_16x8, vmulq_s16(source1_low_s_16x8, factorChannel01_64_s_16x8)); // intermediateResults0 = saturated(intermediateResults0 + source10_low * factorChannel01)
		intermediateResults1_low_s_16x8 = vqaddq_s16(intermediateResults1_low_s_16x8, vmulq_s16(source1_low_s_16x8, factorChannel11_64_s_16x8));
		intermediateResults2_low_s_16x8 = vqaddq_s16(intermediateResults2_low_s_16x8, vmulq_s16(source1_low_s_16x8, factorChannel21_64_s_16x8));

		intermediateResults0_high_s_16x8 = vqaddq_s16(intermediateResults0_high_s_16x8, vmulq_s16(source1_high_s_16x8, factorChannel01_64_s_16x8));
		intermediateResults1_high_s_16x8 = vqaddq_s16(intermediateResults1_high_s_16x8, vmulq_s16(source1_high_s_16x8, factorChannel11_64_s_16x8));
		intermediateResults2_high_s_16x8 = vqaddq_s16(intermediateResults2_high_s_16x8, vmulq_s16(source1_high_s_16x8, factorChannel21_64_s_16x8));

		intermediateResults0_low_s_16x8 = vqaddq_s16(intermediateResults0_low_s_16x8, vmulq_s16(source2_low_s_16x8, factorChannel02_64_s_16x8));
		intermediateResults1_low_s_16x8 = vqaddq_s16(intermediateResults1_low_s_16x8, vmulq_s16(source2_low_s_16x8, factorChannel12_64_s_16x8));
		intermediateResults2_low_s_16x8 = vqaddq_s16(intermediateResults2_low_s_16x8, vmulq_s16(source2_low_s_16x8, factorChannel22_64_s_16x8));

		intermediateResults0_high_s_16x8 = vqaddq_s16(intermediateResults0_high_s_16x8, vmulq_s16(source2_high_s_16x8, factorChannel02_64_s_16x8));
		intermediateResults1_high_s_16x8 = vqaddq_s16(intermediateResults1_high_s_16x8, vmulq_s16(source2_high_s_16x8, factorChannel12_64_s_16x8));
		intermediateResults2_high_s_16x8 = vqaddq_s16(intermediateResults2_high_s_16x8, vmulq_s16(source2_high_s_16x8, factorChannel22_64_s_16x8));

		uint8x16x4_t results_u_8x16x4;

		// saturated narrow signed to unsigned, normalized by 2^6
		results_u_8x16x4.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults0_high_s_16x8, 6));
		results_u_8x16x4.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults1_high_s_16x8, 6));
		results_u_8x16x4.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2_low_s_16x8, 6), vqrshrun_n_s16(intermediateResults2_high_s_16x8, 6));
		results_u_8x16x4.val[3] = valueChannel3_u_8x16;

		// and we can store the result
		vst4q_u8(target, results_u_8x16x4);

		sPlane0 += blockSize;
		sPlane1 += blockSize;
		sPlane2 += blockSize;

		target += blockSize * 4u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sPlane0 != sPlane0End)
	{
		ocean_assert(sPlane0 < sPlane0End);

		const int16_t source0 = int16_t(*sPlane0) - int16_t(bias0);
		const int16_t source1 = int16_t(*sPlane1) - int16_t(bias1);
		const int16_t source2 = int16_t(*sPlane2) - int16_t(bias2);

		target[0] = (uint8_t)(minmax<int>(0, (source0 * int16_t(factorChannel00_64) + source1 * int16_t(factorChannel01_64) + source2 * int16_t(factorChannel02_64)) / 64, 255));
		target[1] = (uint8_t)(minmax<int>(0, (source0 * int16_t(factorChannel10_64) + source1 * int16_t(factorChannel11_64) + source2 * int16_t(factorChannel12_64)) / 64, 255));
		target[2] = (uint8_t)(minmax<int>(0, (source0 * int16_t(factorChannel20_64) + source1 * int16_t(factorChannel21_64) + source2 * int16_t(factorChannel22_64)) / 64, 255));
		target[3] = (uint8_t)valueChannel3;

		sPlane0++;
		sPlane1++;
		sPlane2++;

		target += 4;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 4u>(target - width * 4u, width);
	}
}

void FrameConverter::convertOneRow_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: sourcePlane2PaddingElements
	// uint32_t: targetZippedPaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

	const unsigned int sourcePlane0PaddingElements = (unsigned int)intOptions[0];
	const unsigned int sourcePlane1PaddingElements = (unsigned int)intOptions[1];
	const unsigned int sourcePlane2PaddingElements = (unsigned int)intOptions[2];
	const unsigned int targetZippedPaddingElements = (unsigned int)intOptions[3];

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);
	const uint8_t* sourcePlane2 = (const uint8_t*)(sources[2]);

	uint8_t* targetZipped = (uint8_t*)(targets[0]);

	const unsigned int width_2 = width / 2u;

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width_2 + sourcePlane1PaddingElements;
	const unsigned int sourcePlane2StrideElements = width_2 + sourcePlane2PaddingElements;
	const unsigned int targetZippedStrideElements = width * 4u + targetZippedPaddingElements;

	const int factorChannel00_64 = intOptions[4];
	const int factorChannel10_64 = intOptions[5];
	const int factorChannel20_64 = intOptions[6];

	const int factorChannel01_64 = intOptions[7];
	const int factorChannel11_64 = intOptions[8];
	const int factorChannel21_64 = intOptions[9];

	const int factorChannel02_64 = intOptions[10];
	const int factorChannel12_64 = intOptions[11];
	const int factorChannel22_64 = intOptions[12];

	ocean_assert(std::abs(factorChannel00_64 + factorChannel01_64 + factorChannel02_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel10_64 + factorChannel11_64 + factorChannel12_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel20_64 + factorChannel21_64 + factorChannel22_64) < 64 * 4);

	const int bias0 = intOptions[13];
	const int bias1 = intOptions[14];
	const int bias2 = intOptions[15];

	ocean_assert(bias0 >= 0 && bias0 <= 128);
	ocean_assert(bias1 >= 0 && bias1 <= 128);
	ocean_assert(bias2 >= 0 && bias2 <= 128);

	const int valueChannel3 = intOptions[16];
	ocean_assert(valueChannel3 >= 0 && valueChannel3 <= 255);

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* sPlane0 = sourcePlane0 + multipleRowIndex * sourcePlane0StrideElements;
	const uint8_t* sPlane1 = sourcePlane1 + (multipleRowIndex / 2u) * sourcePlane1StrideElements;
	const uint8_t* sPlane2 = sourcePlane2 + (multipleRowIndex / 2u) * sourcePlane2StrideElements;

	uint8_t* target = flipTarget ? (targetZipped + (height - multipleRowIndex - 1u) * targetZippedStrideElements) : targetZipped + multipleRowIndex * targetZippedStrideElements;

	const uint8_t* const sPlane0End = sPlane0 + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	if (blocks != 0u)
	{
		const int16x8_t factorChannel00_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel00_64));
		const int16x8_t factorChannel10_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel10_64));
		const int16x8_t factorChannel20_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel20_64));

		const int16x8_t factorChannel01_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel01_64));
		const int16x8_t factorChannel11_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel11_64));
		const int16x8_t factorChannel21_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel21_64));

		const int16x8_t factorChannel02_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel02_64));
		const int16x8_t factorChannel12_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel12_64));
		const int16x8_t factorChannel22_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel22_64));

		const uint8x8_t bias0_u_8x8 = vdup_n_u8((uint8_t)(bias0));
		const uint8x8_t bias1_u_8x8 = vdup_n_u8((uint8_t)(bias1));
		const uint8x8_t bias2_u_8x8 = vdup_n_u8((uint8_t)(bias2));

		const uint8x16_t valueChannel3_u_8x16 = vdupq_n_u8((uint8_t)(valueChannel3));

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			const uint8x16_t sPlane0_u_8x16 = vld1q_u8(sPlane0); // upper row
			const uint8x8_t source1_u_8x8 = vld1_u8(sPlane1);
			const uint8x8_t source2_u_8x8 = vld1_u8(sPlane2);

			// Y' = Y - bias0, U' = U - bias1, V' = V - bias2
			const int16x8_t source0_A_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(sPlane0_u_8x16), bias0_u_8x8));
			const int16x8_t source0_B_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(sPlane0_u_8x16), bias0_u_8x8));

			const int16x8_t source1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(source1_u_8x8, bias1_u_8x8));
			const int16x8_t source2_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(source2_u_8x8, bias2_u_8x8));


			// now we apply the 3x3 matrix multiplication

			int16x8_t intermediateResults0_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel01_64_s_16x8); // downsampled channel 1
			int16x8_t intermediateResults1_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel11_64_s_16x8);
			int16x8_t intermediateResults2_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel21_64_s_16x8);

			intermediateResults0_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8, vmulq_s16(source2_s_16x8, factorChannel02_64_s_16x8)); // downsampled channel 2
			intermediateResults1_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8, vmulq_s16(source2_s_16x8, factorChannel12_64_s_16x8));
			intermediateResults2_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8, vmulq_s16(source2_s_16x8, factorChannel22_64_s_16x8));

			const int16x8x2_t intermediateResults0_AB_s_16x8x2 = vzipq_s16(intermediateResults0_s_16x8, intermediateResults0_s_16x8); // upsampling channel the results
			const int16x8x2_t intermediateResults1_AB_s_16x8x2 = vzipq_s16(intermediateResults1_s_16x8, intermediateResults1_s_16x8);
			const int16x8x2_t intermediateResults2_AB_s_16x8x2 = vzipq_s16(intermediateResults2_s_16x8, intermediateResults2_s_16x8);


			const int16x8_t intermediateResults0_A_s_16x8 = vqaddq_s16(intermediateResults0_AB_s_16x8x2.val[0], vmulq_s16(source0_A_s_16x8, factorChannel00_64_s_16x8)); // channel 0, upper row
			const int16x8_t intermediateResults1_A_s_16x8 = vqaddq_s16(intermediateResults1_AB_s_16x8x2.val[0], vmulq_s16(source0_A_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2_A_s_16x8 = vqaddq_s16(intermediateResults2_AB_s_16x8x2.val[0], vmulq_s16(source0_A_s_16x8, factorChannel20_64_s_16x8));

			const int16x8_t intermediateResults0_B_s_16x8 = vqaddq_s16(intermediateResults0_AB_s_16x8x2.val[1], vmulq_s16(source0_B_s_16x8, factorChannel00_64_s_16x8));
			const int16x8_t intermediateResults1_B_s_16x8 = vqaddq_s16(intermediateResults1_AB_s_16x8x2.val[1], vmulq_s16(source0_B_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2_B_s_16x8 = vqaddq_s16(intermediateResults2_AB_s_16x8x2.val[1], vmulq_s16(source0_B_s_16x8, factorChannel20_64_s_16x8));


			// writing results

			uint8x16x4_t results_u_8x16x4;

			// saturated narrow signed to unsigned, normalized by 2^6
			results_u_8x16x4.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults0_B_s_16x8, 6));
			results_u_8x16x4.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults1_B_s_16x8, 6));
			results_u_8x16x4.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults2_B_s_16x8, 6));
			results_u_8x16x4.val[3] = valueChannel3_u_8x16;

			// and we can store the result
			vst4q_u8(target, results_u_8x16x4);

			sPlane0 += blockSize;
			sPlane1 += blockSize / 2u;
			sPlane2 += blockSize / 2u;

			target += blockSize * 4u;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sPlane0 != sPlane0End)
	{
		ocean_assert(sPlane0 < sPlane0End);

		const int16_t source1 = int16_t(sPlane1[0]) - int16_t(bias1);
		const int16_t source2 = int16_t(sPlane2[0]) - int16_t(bias2);

		const int intermediate0 = source1 * int16_t(factorChannel01_64) + source2 * int16_t(factorChannel02_64);
		const int intermediate1 = source1 * int16_t(factorChannel11_64) + source2 * int16_t(factorChannel12_64);
		const int intermediate2 = source1 * int16_t(factorChannel21_64) + source2 * int16_t(factorChannel22_64);

		// first and second upper pixel

		const int16_t source0Left = int16_t(sPlane0[0]) - int16_t(bias0);
		const int16_t source0Right = int16_t(sPlane0[1]) - int16_t(bias0);

		target[0] = uint8_t(minmax<int>(0, (source0Left * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		target[1] = uint8_t(minmax<int>(0, (source0Left * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		target[2] = uint8_t(minmax<int>(0, (source0Left * int16_t(factorChannel20_64) + intermediate2) / 64, 255));
		target[3] = uint8_t(valueChannel3);

		target[4] = uint8_t(minmax<int>(0, (source0Right * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		target[5] = uint8_t(minmax<int>(0, (source0Right * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		target[6] = uint8_t(minmax<int>(0, (source0Right * int16_t(factorChannel20_64) + intermediate2) / 64, 255));
		target[7] = uint8_t(valueChannel3);

		sPlane0 += 2;
		sPlane1++;
		sPlane2++;

		target += 2u * 4u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 4u>(target - width * 4u, width);
	}
}

void FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane3Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: sourcePlane2PaddingElements
	// uint32_t: targetPlanePaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

	const unsigned int sourcePlane0PaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int sourcePlane1PaddingElements = (unsigned int)(intOptions[1]);
	const unsigned int sourcePlane2PaddingElements = (unsigned int)(intOptions[2]);
	const unsigned int targetPlanePaddingElements = (unsigned int)(intOptions[3]);

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);
	const uint8_t* sourcePlane2 = (const uint8_t*)(sources[2]);

	uint8_t* targetZipped = (uint8_t*)(targets[0]);

	const unsigned int width_2 = width / 2u;

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width_2 + sourcePlane1PaddingElements;
	const unsigned int sourcePlane2StrideElements = width_2 + sourcePlane2PaddingElements;
	const unsigned int targetPlaneStrideElements = width * 3u + targetPlanePaddingElements;

	const int factorChannel00_64 = intOptions[4];
	const int factorChannel10_64 = intOptions[5];
	const int factorChannel20_64 = intOptions[6];

	const int factorChannel01_64 = intOptions[7];
	const int factorChannel11_64 = intOptions[8];
	const int factorChannel21_64 = intOptions[9];

	const int factorChannel02_64 = intOptions[10];
	const int factorChannel12_64 = intOptions[11];
	const int factorChannel22_64 = intOptions[12];

	ocean_assert(std::abs(factorChannel00_64 + factorChannel01_64 + factorChannel02_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel10_64 + factorChannel11_64 + factorChannel12_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel20_64 + factorChannel21_64 + factorChannel22_64) < 64 * 4);

	const int bias0 = intOptions[13];
	const int bias1 = intOptions[14];
	const int bias2 = intOptions[15];

	ocean_assert(bias0 >= 0 && bias0 <= 128);
	ocean_assert(bias1 >= 0 && bias1 <= 128);
	ocean_assert(bias2 >= 0 && bias2 <= 128);

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* source0Upper = sourcePlane0 + multipleRowIndex * 2u * sourcePlane0StrideElements;
	const uint8_t* source1 = sourcePlane1 + multipleRowIndex * sourcePlane1StrideElements;
	const uint8_t* source2 = sourcePlane2 + multipleRowIndex * sourcePlane2StrideElements;

	uint8_t* targetPlaneUpper = flipTarget ? (targetZipped + (height - multipleRowIndex * 2u - 1u) * targetPlaneStrideElements) : targetZipped + multipleRowIndex * 2u * targetPlaneStrideElements;
	uint8_t* targetPlaneLower = flipTarget ? targetPlaneUpper - targetPlaneStrideElements : targetPlaneUpper + targetPlaneStrideElements;

	const uint8_t* const source0UpperEnd = source0Upper + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	if (blocks != 0u)
	{
		const int16x8_t factorChannel00_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel00_64));
		const int16x8_t factorChannel10_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel10_64));
		const int16x8_t factorChannel20_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel20_64));

		const int16x8_t factorChannel01_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel01_64));
		const int16x8_t factorChannel11_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel11_64));
		const int16x8_t factorChannel21_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel21_64));

		const int16x8_t factorChannel02_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel02_64));
		const int16x8_t factorChannel12_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel12_64));
		const int16x8_t factorChannel22_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel22_64));

		const uint8x8_t bias0_u_8x8 = vdup_n_u8((uint8_t)(bias0));
		const uint8x8_t bias1_u_8x8 = vdup_n_u8((uint8_t)(bias1));
		const uint8x8_t bias2_u_8x8 = vdup_n_u8((uint8_t)(bias2));

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			const uint8x16_t source0Upper_u_8x16 = vld1q_u8(source0Upper); // upper row
			const uint8x16_t source0Lower_u_8x16 = vld1q_u8(source0Upper + sourcePlane0StrideElements); // lower row
			const uint8x8_t source1_u_8x8 = vld1_u8(source1);
			const uint8x8_t source2_u_8x8 = vld1_u8(source2);

			// Y' = Y - bias0, U' = U - bias1, V' = V - bias2
			const int16x8_t source0Upper_A_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source0Upper_u_8x16), bias0_u_8x8));
			const int16x8_t source0Lower_A_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(source0Lower_u_8x16), bias0_u_8x8));
			const int16x8_t source0Upper_B_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source0Upper_u_8x16), bias0_u_8x8));
			const int16x8_t source0Lower_B_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(source0Lower_u_8x16), bias0_u_8x8));

			const int16x8_t source1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(source1_u_8x8, bias1_u_8x8));
			const int16x8_t source2_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(source2_u_8x8, bias2_u_8x8));


			// now we apply the 3x3 matrix multiplication

			int16x8_t intermediateResults0_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel01_64_s_16x8); // downsampled channel 1
			int16x8_t intermediateResults1_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel11_64_s_16x8);
			int16x8_t intermediateResults2_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel21_64_s_16x8);

			intermediateResults0_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8, vmulq_s16(source2_s_16x8, factorChannel02_64_s_16x8)); // downsampled channel 2
			intermediateResults1_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8, vmulq_s16(source2_s_16x8, factorChannel12_64_s_16x8));
			intermediateResults2_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8, vmulq_s16(source2_s_16x8, factorChannel22_64_s_16x8));

			const int16x8x2_t intermediateResults0_AB_s_16x8x2 = vzipq_s16(intermediateResults0_s_16x8, intermediateResults0_s_16x8); // upsampling the results
			const int16x8x2_t intermediateResults1_AB_s_16x8x2 = vzipq_s16(intermediateResults1_s_16x8, intermediateResults1_s_16x8);
			const int16x8x2_t intermediateResults2_AB_s_16x8x2 = vzipq_s16(intermediateResults2_s_16x8, intermediateResults2_s_16x8);


			const int16x8_t intermediateResults0Upper_A_s_16x8 = vqaddq_s16(intermediateResults0_AB_s_16x8x2.val[0], vmulq_s16(source0Upper_A_s_16x8, factorChannel00_64_s_16x8)); // channel 0, upper row
			const int16x8_t intermediateResults1Upper_A_s_16x8 = vqaddq_s16(intermediateResults1_AB_s_16x8x2.val[0], vmulq_s16(source0Upper_A_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2Upper_A_s_16x8 = vqaddq_s16(intermediateResults2_AB_s_16x8x2.val[0], vmulq_s16(source0Upper_A_s_16x8, factorChannel20_64_s_16x8));

			const int16x8_t intermediateResults0Upper_B_s_16x8 = vqaddq_s16(intermediateResults0_AB_s_16x8x2.val[1], vmulq_s16(source0Upper_B_s_16x8, factorChannel00_64_s_16x8));
			const int16x8_t intermediateResults1Upper_B_s_16x8 = vqaddq_s16(intermediateResults1_AB_s_16x8x2.val[1], vmulq_s16(source0Upper_B_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2Upper_B_s_16x8 = vqaddq_s16(intermediateResults2_AB_s_16x8x2.val[1], vmulq_s16(source0Upper_B_s_16x8, factorChannel20_64_s_16x8));


			const int16x8_t intermediateResults0Lower_A_s_16x8 = vqaddq_s16(intermediateResults0_AB_s_16x8x2.val[0], vmulq_s16(source0Lower_A_s_16x8, factorChannel00_64_s_16x8)); // channel 0, lower row
			const int16x8_t intermediateResults1Lower_A_s_16x8 = vqaddq_s16(intermediateResults1_AB_s_16x8x2.val[0], vmulq_s16(source0Lower_A_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2Lower_A_s_16x8 = vqaddq_s16(intermediateResults2_AB_s_16x8x2.val[0], vmulq_s16(source0Lower_A_s_16x8, factorChannel20_64_s_16x8));

			const int16x8_t intermediateResults0Lower_B_s_16x8 = vqaddq_s16(intermediateResults0_AB_s_16x8x2.val[1], vmulq_s16(source0Lower_B_s_16x8, factorChannel00_64_s_16x8));
			const int16x8_t intermediateResults1Lower_B_s_16x8 = vqaddq_s16(intermediateResults1_AB_s_16x8x2.val[1], vmulq_s16(source0Lower_B_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2Lower_B_s_16x8 = vqaddq_s16(intermediateResults2_AB_s_16x8x2.val[1], vmulq_s16(source0Lower_B_s_16x8, factorChannel20_64_s_16x8));


			// writing upper results

			uint8x16x3_t resultsUpper_u_8x16x3;

			// saturated narrow signed to unsigned, normalized by 2^6
			resultsUpper_u_8x16x3.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0Upper_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults0Upper_B_s_16x8, 6));
			resultsUpper_u_8x16x3.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1Upper_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults1Upper_B_s_16x8, 6));
			resultsUpper_u_8x16x3.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2Upper_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults2Upper_B_s_16x8, 6));

			// and we can store the result
			vst3q_u8(targetPlaneUpper, resultsUpper_u_8x16x3);


			// writing lower results

			uint8x16x3_t resultsLower_u_8x16x3;

			// saturated narrow signed to unsigned, normalized by 2^6
			resultsLower_u_8x16x3.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0Lower_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults0Lower_B_s_16x8, 6));
			resultsLower_u_8x16x3.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1Lower_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults1Lower_B_s_16x8, 6));
			resultsLower_u_8x16x3.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2Lower_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults2Lower_B_s_16x8, 6));

			// and we can store the result
			vst3q_u8(targetPlaneLower, resultsLower_u_8x16x3);

			source0Upper += blockSize;
			source1 += blockSize / 2u;
			source2 += blockSize / 2u;

			targetPlaneUpper += blockSize * 3u;
			targetPlaneLower += blockSize * 3u;
		}
	}

#elif defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	if (blocks != 0u)
	{
		const __m128i factorChannel00_64_s_16x8 = _mm_set1_epi16(int16_t(factorChannel00_64));
		const __m128i factorChannel10_64_s_16x8 = _mm_set1_epi16(int16_t(factorChannel10_64));
		const __m128i factorChannel20_64_s_16x8 = _mm_set1_epi16(int16_t(factorChannel20_64));

		const __m128i factorChannel01_64_s_16x8 = _mm_set1_epi16(int16_t(factorChannel01_64));
		const __m128i factorChannel11_64_s_16x8 = _mm_set1_epi16(int16_t(factorChannel11_64));
		const __m128i factorChannel21_64_s_16x8 = _mm_set1_epi16(int16_t(factorChannel21_64));

		const __m128i factorChannel02_64_s_16x8 = _mm_set1_epi16(int16_t(factorChannel02_64));
		const __m128i factorChannel12_64_s_16x8 = _mm_set1_epi16(int16_t(factorChannel12_64));
		const __m128i factorChannel22_64_s_16x8 = _mm_set1_epi16(int16_t(factorChannel22_64));

		const __m128i bias0_u_16x8 = _mm_set1_epi16(int16_t(bias0));
		const __m128i bias1_u_16x8 = _mm_set1_epi16(int16_t(bias1));
		const __m128i bias2_u_16x8 = _mm_set1_epi16(int16_t(bias2));

		const __m128i constant_255_s_16x8 = _mm_set1_epi16(int16_t(255));

		const __m128i shuffleMask_lower = SSE::set128i(0xFFFFFFFFFFFFFFFFull, 0x0E0C0A0806040200ull);
		const __m128i shuffleMask_upper = SSE::set128i(0x0E0C0A0806040200ull, 0xFFFFFFFFFFFFFFFFull);

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			const __m128i source0Upper_u_8x16 = SSE::load128i(source0Upper); // upper row
			const __m128i source0Lower_u_8x16 = SSE::load128i(source0Upper + sourcePlane0StrideElements); // lower row
			const __m128i source1_u_16x8 = _mm_unpacklo_epi8(SSE::load128iLower64(source1), _mm_setzero_si128());
			const __m128i source2_u_16x8 = _mm_unpacklo_epi8(SSE::load128iLower64(source2), _mm_setzero_si128());

			// Y' = Y - bias0, U' = U - bias1, V' = V - bias2
			const __m128i source0Upper_A_s_16x8 = _mm_subs_epi16(_mm_unpacklo_epi8(source0Upper_u_8x16, _mm_setzero_si128()), bias0_u_16x8);
			const __m128i source0Lower_A_s_16x8 = _mm_subs_epi16(_mm_unpacklo_epi8(source0Lower_u_8x16, _mm_setzero_si128()), bias0_u_16x8);
			const __m128i source0Upper_B_s_16x8 = _mm_subs_epi16(_mm_unpackhi_epi8(source0Upper_u_8x16, _mm_setzero_si128()), bias0_u_16x8);
			const __m128i source0Lower_B_s_16x8 = _mm_subs_epi16(_mm_unpackhi_epi8(source0Lower_u_8x16, _mm_setzero_si128()), bias0_u_16x8);

			const __m128i source1_s_16x8 = _mm_subs_epi16(source1_u_16x8, bias1_u_16x8);
			const __m128i source2_s_16x8 = _mm_subs_epi16(source2_u_16x8, bias2_u_16x8);


			// now we apply the 3x3 matrix multiplication

			__m128i intermediateResults0_s_16x8 = _mm_mullo_epi16(source1_s_16x8, factorChannel01_64_s_16x8); // downsampled channel 1
			__m128i intermediateResults1_s_16x8 = _mm_mullo_epi16(source1_s_16x8, factorChannel11_64_s_16x8);
			__m128i intermediateResults2_s_16x8 = _mm_mullo_epi16(source1_s_16x8, factorChannel21_64_s_16x8);

			intermediateResults0_s_16x8 = _mm_add_epi16(intermediateResults0_s_16x8, _mm_mullo_epi16(source2_s_16x8, factorChannel02_64_s_16x8)); // downsampled channel 2
			intermediateResults1_s_16x8 = _mm_add_epi16(intermediateResults1_s_16x8, _mm_mullo_epi16(source2_s_16x8, factorChannel12_64_s_16x8));
			intermediateResults2_s_16x8 = _mm_add_epi16(intermediateResults2_s_16x8, _mm_mullo_epi16(source2_s_16x8, factorChannel22_64_s_16x8));

			__m128i intermediateResults0_A_s_16x8 = _mm_unpacklo_epi16(intermediateResults0_s_16x8, intermediateResults0_s_16x8); // upsampling the results
			__m128i intermediateResults0_B_s_16x8 = _mm_unpackhi_epi16(intermediateResults0_s_16x8, intermediateResults0_s_16x8);
			__m128i intermediateResults1_A_s_16x8 = _mm_unpacklo_epi16(intermediateResults1_s_16x8, intermediateResults1_s_16x8);
			__m128i intermediateResults1_B_s_16x8 = _mm_unpackhi_epi16(intermediateResults1_s_16x8, intermediateResults1_s_16x8);
			__m128i intermediateResults2_A_s_16x8 = _mm_unpacklo_epi16(intermediateResults2_s_16x8, intermediateResults2_s_16x8);
			__m128i intermediateResults2_B_s_16x8 = _mm_unpackhi_epi16(intermediateResults2_s_16x8, intermediateResults2_s_16x8);

			__m128i intermediateResults0Upper_A_s_16x8 = _mm_adds_epi16(intermediateResults0_A_s_16x8, _mm_mullo_epi16(source0Upper_A_s_16x8, factorChannel00_64_s_16x8)); // channel 0, upper row
			__m128i intermediateResults1Upper_A_s_16x8 = _mm_adds_epi16(intermediateResults1_A_s_16x8, _mm_mullo_epi16(source0Upper_A_s_16x8, factorChannel10_64_s_16x8));
			__m128i intermediateResults2Upper_A_s_16x8 = _mm_adds_epi16(intermediateResults2_A_s_16x8, _mm_mullo_epi16(source0Upper_A_s_16x8, factorChannel20_64_s_16x8));

			__m128i intermediateResults0Upper_B_s_16x8 = _mm_adds_epi16(intermediateResults0_B_s_16x8, _mm_mullo_epi16(source0Upper_B_s_16x8, factorChannel00_64_s_16x8));
			__m128i intermediateResults1Upper_B_s_16x8 = _mm_adds_epi16(intermediateResults1_B_s_16x8, _mm_mullo_epi16(source0Upper_B_s_16x8, factorChannel10_64_s_16x8));
			__m128i intermediateResults2Upper_B_s_16x8 = _mm_adds_epi16(intermediateResults2_B_s_16x8, _mm_mullo_epi16(source0Upper_B_s_16x8, factorChannel20_64_s_16x8));


			__m128i intermediateResults0Lower_A_s_16x8 = _mm_adds_epi16(intermediateResults0_A_s_16x8, _mm_mullo_epi16(source0Lower_A_s_16x8, factorChannel00_64_s_16x8)); // channel 0, lower row
			__m128i intermediateResults1Lower_A_s_16x8 = _mm_adds_epi16(intermediateResults1_A_s_16x8, _mm_mullo_epi16(source0Lower_A_s_16x8, factorChannel10_64_s_16x8));
			__m128i intermediateResults2Lower_A_s_16x8 = _mm_adds_epi16(intermediateResults2_A_s_16x8, _mm_mullo_epi16(source0Lower_A_s_16x8, factorChannel20_64_s_16x8));

			__m128i intermediateResults0Lower_B_s_16x8 = _mm_adds_epi16(intermediateResults0_B_s_16x8, _mm_mullo_epi16(source0Lower_B_s_16x8, factorChannel00_64_s_16x8));
			__m128i intermediateResults1Lower_B_s_16x8 = _mm_adds_epi16(intermediateResults1_B_s_16x8, _mm_mullo_epi16(source0Lower_B_s_16x8, factorChannel10_64_s_16x8));
			__m128i intermediateResults2Lower_B_s_16x8 = _mm_adds_epi16(intermediateResults2_B_s_16x8, _mm_mullo_epi16(source0Lower_B_s_16x8, factorChannel20_64_s_16x8));


			// clamping the results and shifting by 6 bits

			intermediateResults0Upper_A_s_16x8 = _mm_min_epi16(_mm_srli_epi16(_mm_max_epi16(_mm_setzero_si128(), intermediateResults0Upper_A_s_16x8), 6), constant_255_s_16x8);
			intermediateResults1Upper_A_s_16x8 = _mm_min_epi16(_mm_srli_epi16(_mm_max_epi16(_mm_setzero_si128(), intermediateResults1Upper_A_s_16x8), 6), constant_255_s_16x8);
			intermediateResults2Upper_A_s_16x8 = _mm_min_epi16(_mm_srli_epi16(_mm_max_epi16(_mm_setzero_si128(), intermediateResults2Upper_A_s_16x8), 6), constant_255_s_16x8);

			intermediateResults0Upper_B_s_16x8 = _mm_min_epi16(_mm_srli_epi16(_mm_max_epi16(_mm_setzero_si128(), intermediateResults0Upper_B_s_16x8), 6), constant_255_s_16x8);
			intermediateResults1Upper_B_s_16x8 = _mm_min_epi16(_mm_srli_epi16(_mm_max_epi16(_mm_setzero_si128(), intermediateResults1Upper_B_s_16x8), 6), constant_255_s_16x8);
			intermediateResults2Upper_B_s_16x8 = _mm_min_epi16(_mm_srli_epi16(_mm_max_epi16(_mm_setzero_si128(), intermediateResults2Upper_B_s_16x8), 6), constant_255_s_16x8);

			intermediateResults0Lower_A_s_16x8 = _mm_min_epi16(_mm_srli_epi16(_mm_max_epi16(_mm_setzero_si128(), intermediateResults0Lower_A_s_16x8), 6), constant_255_s_16x8);
			intermediateResults1Lower_A_s_16x8 = _mm_min_epi16(_mm_srli_epi16(_mm_max_epi16(_mm_setzero_si128(), intermediateResults1Lower_A_s_16x8), 6), constant_255_s_16x8);
			intermediateResults2Lower_A_s_16x8 = _mm_min_epi16(_mm_srli_epi16(_mm_max_epi16(_mm_setzero_si128(), intermediateResults2Lower_A_s_16x8), 6), constant_255_s_16x8);

			intermediateResults0Lower_B_s_16x8 = _mm_min_epi16(_mm_srli_epi16(_mm_max_epi16(_mm_setzero_si128(), intermediateResults0Lower_B_s_16x8), 6), constant_255_s_16x8);
			intermediateResults1Lower_B_s_16x8 = _mm_min_epi16(_mm_srli_epi16(_mm_max_epi16(_mm_setzero_si128(), intermediateResults1Lower_B_s_16x8), 6), constant_255_s_16x8);
			intermediateResults2Lower_B_s_16x8 = _mm_min_epi16(_mm_srli_epi16(_mm_max_epi16(_mm_setzero_si128(), intermediateResults2Lower_B_s_16x8), 6), constant_255_s_16x8);


			// writing upper results

			const __m128i result0Upper_u_8x16 = _mm_or_si128(_mm_shuffle_epi8(intermediateResults0Upper_A_s_16x8, shuffleMask_lower), _mm_shuffle_epi8(intermediateResults0Upper_B_s_16x8, shuffleMask_upper));
			const __m128i result1Upper_u_8x16 = _mm_or_si128(_mm_shuffle_epi8(intermediateResults1Upper_A_s_16x8, shuffleMask_lower), _mm_shuffle_epi8(intermediateResults1Upper_B_s_16x8, shuffleMask_upper));
			const __m128i result2Upper_u_8x16 = _mm_or_si128(_mm_shuffle_epi8(intermediateResults2Upper_A_s_16x8, shuffleMask_lower), _mm_shuffle_epi8(intermediateResults2Upper_B_s_16x8, shuffleMask_upper));

			__m128i interleavedA_128, interleavedB_128, interleavedC_128;
			SSE::interleave3Channel8Bit48Elements(result0Upper_u_8x16, result1Upper_u_8x16, result2Upper_u_8x16, interleavedA_128, interleavedB_128, interleavedC_128);

			SSE::store128i(interleavedA_128, targetPlaneUpper + 0);
			SSE::store128i(interleavedB_128, targetPlaneUpper + 16);
			SSE::store128i(interleavedC_128, targetPlaneUpper + 32);

			const __m128i result0Lower_u_8x16 = _mm_or_si128(_mm_shuffle_epi8(intermediateResults0Lower_A_s_16x8, shuffleMask_lower), _mm_shuffle_epi8(intermediateResults0Lower_B_s_16x8, shuffleMask_upper));
			const __m128i result1Lower_u_8x16 = _mm_or_si128(_mm_shuffle_epi8(intermediateResults1Lower_A_s_16x8, shuffleMask_lower), _mm_shuffle_epi8(intermediateResults1Lower_B_s_16x8, shuffleMask_upper));
			const __m128i result2Lower_u_8x16 = _mm_or_si128(_mm_shuffle_epi8(intermediateResults2Lower_A_s_16x8, shuffleMask_lower), _mm_shuffle_epi8(intermediateResults2Lower_B_s_16x8, shuffleMask_upper));

			SSE::interleave3Channel8Bit48Elements(result0Lower_u_8x16, result1Lower_u_8x16, result2Lower_u_8x16, interleavedA_128, interleavedB_128, interleavedC_128);

			SSE::store128i(interleavedA_128, targetPlaneLower + 0);
			SSE::store128i(interleavedB_128, targetPlaneLower + 16);
			SSE::store128i(interleavedC_128, targetPlaneLower + 32);

			source0Upper += blockSize;
			source1 += blockSize / 2u;
			source2 += blockSize / 2u;

			targetPlaneUpper += blockSize * 3u;
			targetPlaneLower += blockSize * 3u;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (source0Upper != source0UpperEnd)
	{
		ocean_assert(source0Upper < source0UpperEnd);

		const int16_t biasedSource1 = int16_t(source1[0]) - int16_t(bias1);
		const int16_t biasedSource2 = int16_t(source2[0]) - int16_t(bias2);

		const int intermediate0 = biasedSource1 * int16_t(factorChannel01_64) + biasedSource2 * int16_t(factorChannel02_64);
		const int intermediate1 = biasedSource1 * int16_t(factorChannel11_64) + biasedSource2 * int16_t(factorChannel12_64);
		const int intermediate2 = biasedSource1 * int16_t(factorChannel21_64) + biasedSource2 * int16_t(factorChannel22_64);

		// first and second upper pixel

		const int16_t source0UpperLeft = int16_t(source0Upper[0]) - int16_t(bias0);
		const int16_t source0UpperRight = int16_t(source0Upper[1]) - int16_t(bias0);

		targetPlaneUpper[0] = (uint8_t)(minmax<int>(0, (source0UpperLeft * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlaneUpper[1] = (uint8_t)(minmax<int>(0, (source0UpperLeft * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlaneUpper[2] = (uint8_t)(minmax<int>(0, (source0UpperLeft * int16_t(factorChannel20_64) + intermediate2) / 64, 255));

		targetPlaneUpper[3] = (uint8_t)(minmax<int>(0, (source0UpperRight * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlaneUpper[4] = (uint8_t)(minmax<int>(0, (source0UpperRight * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlaneUpper[5] = (uint8_t)(minmax<int>(0, (source0UpperRight * int16_t(factorChannel20_64) + intermediate2) / 64, 255));


		// first and second lower pixel

		const int16_t source0LowerLeft = int16_t(source0Upper[sourcePlane0StrideElements + 0u]) - int16_t(bias0);
		const int16_t source0LowerRight = int16_t(source0Upper[sourcePlane0StrideElements + 1u]) - int16_t(bias0);

		targetPlaneLower[0] = (uint8_t)(minmax<int>(0, (source0LowerLeft * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlaneLower[1] = (uint8_t)(minmax<int>(0, (source0LowerLeft * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlaneLower[2] = (uint8_t)(minmax<int>(0, (source0LowerLeft * int16_t(factorChannel20_64) + intermediate2) / 64, 255));

		targetPlaneLower[3] = (uint8_t)(minmax<int>(0, (source0LowerRight * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlaneLower[4] = (uint8_t)(minmax<int>(0, (source0LowerRight * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlaneLower[5] = (uint8_t)(minmax<int>(0, (source0LowerRight * int16_t(factorChannel20_64) + intermediate2) / 64, 255));

		source0Upper += 2;
		source1++;
		source2++;

		targetPlaneUpper += 2u * 3u;
		targetPlaneLower += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetPlaneUpper - width * 3u, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetPlaneLower - width * 3u, width);
	}
}

void FrameConverter::convertTwoRows_1Plane1ChannelAnd2Planes1ChannelDownsampled2x2_To_1Plane4Channels_8BitPerChannel_Precision6Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePlane0PaddingElements
	// uint32_t: sourcePlane1PaddingElements
	// uint32_t: sourcePlane2PaddingElements
	// uint32_t: targetPlanePaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2
	// uint32_t: alphaChannel

	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

	const unsigned int sourcePlane0PaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int sourcePlane1PaddingElements = (unsigned int)(intOptions[1]);
	const unsigned int sourcePlane2PaddingElements = (unsigned int)(intOptions[2]);
	const unsigned int targetPlanePaddingElements = (unsigned int)(intOptions[3]);

	const uint8_t* sourcePlane0 = (const uint8_t*)(sources[0]);
	const uint8_t* sourcePlane1 = (const uint8_t*)(sources[1]);
	const uint8_t* sourcePlane2 = (const uint8_t*)(sources[2]);

	uint8_t* targetPlane = (uint8_t*)(targets[0]);

	const unsigned int width_2 = width / 2u;

	const unsigned int sourcePlane0StrideElements = width + sourcePlane0PaddingElements;
	const unsigned int sourcePlane1StrideElements = width_2 + sourcePlane1PaddingElements;
	const unsigned int sourcePlane2StrideElements = width_2 + sourcePlane2PaddingElements;
	const unsigned int targetPlaneStrideElements = width * 4u + targetPlanePaddingElements;

	const int factorChannel00_64 = intOptions[4];
	const int factorChannel10_64 = intOptions[5];
	const int factorChannel20_64 = intOptions[6];

	const int factorChannel01_64 = intOptions[7];
	const int factorChannel11_64 = intOptions[8];
	const int factorChannel21_64 = intOptions[9];

	const int factorChannel02_64 = intOptions[10];
	const int factorChannel12_64 = intOptions[11];
	const int factorChannel22_64 = intOptions[12];

	ocean_assert(std::abs(factorChannel00_64 + factorChannel01_64 + factorChannel02_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel10_64 + factorChannel11_64 + factorChannel12_64) < 64 * 4);
	ocean_assert(std::abs(factorChannel20_64 + factorChannel21_64 + factorChannel22_64) < 64 * 4);

	const int bias0 = intOptions[13];
	const int bias1 = intOptions[14];
	const int bias2 = intOptions[15];

	ocean_assert(bias0 >= 0 && bias0 <= 128);
	ocean_assert(bias1 >= 0 && bias1 <= 128);
	ocean_assert(bias2 >= 0 && bias2 <= 128);

	const int valueChannel3 = intOptions[16];
	ocean_assert(valueChannel3 >= 0 && valueChannel3 <= 255);

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	const uint8_t* sourcePlane0Upper = sourcePlane0 + multipleRowIndex * 2u * sourcePlane0StrideElements;
	sourcePlane1 += multipleRowIndex * sourcePlane1StrideElements;
	sourcePlane2 += multipleRowIndex * sourcePlane2StrideElements;

	uint8_t* targetPlaneUpper = flipTarget ? (targetPlane + (height - multipleRowIndex * 2u - 1u) * targetPlaneStrideElements) : targetPlane + multipleRowIndex * 2u * targetPlaneStrideElements;
	uint8_t* targetPlaneLower = flipTarget ? targetPlaneUpper - targetPlaneStrideElements : targetPlaneUpper + targetPlaneStrideElements;

	const uint8_t* const sourcePlane0UpperEnd = sourcePlane0Upper + width;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	if (blocks != 0u)
	{
		const int16x8_t factorChannel00_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel00_64));
		const int16x8_t factorChannel10_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel10_64));
		const int16x8_t factorChannel20_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel20_64));

		const int16x8_t factorChannel01_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel01_64));
		const int16x8_t factorChannel11_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel11_64));
		const int16x8_t factorChannel21_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel21_64));

		const int16x8_t factorChannel02_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel02_64));
		const int16x8_t factorChannel12_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel12_64));
		const int16x8_t factorChannel22_64_s_16x8 = vdupq_n_s16(int16_t(factorChannel22_64));

		const uint8x8_t bias0_u_8x8 = vdup_n_u8((uint8_t)(bias0));
		const uint8x8_t bias1_u_8x8 = vdup_n_u8((uint8_t)(bias1));
		const uint8x8_t bias2_u_8x8 = vdup_n_u8((uint8_t)(bias2));

		const uint8x16_t valueChannel3_u_8x16 = vdupq_n_u8((uint8_t)(valueChannel3));

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			const uint8x16_t sourcePlane0Upper_u_8x16 = vld1q_u8(sourcePlane0Upper); // upper row
			const uint8x16_t sPlane0Lower_u_8x16 = vld1q_u8(sourcePlane0Upper + sourcePlane0StrideElements); // lower row
			const uint8x8_t source1_u_8x8 = vld1_u8(sourcePlane1);
			const uint8x8_t source2_u_8x8 = vld1_u8(sourcePlane2);

			// Y' = Y - bias0, U' = U - bias1, V' = V - bias2
			const int16x8_t source0Upper_A_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(sourcePlane0Upper_u_8x16), bias0_u_8x8));
			const int16x8_t source0Lower_A_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_low_u8(sPlane0Lower_u_8x16), bias0_u_8x8));
			const int16x8_t source0Upper_B_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(sourcePlane0Upper_u_8x16), bias0_u_8x8));
			const int16x8_t source0Lower_B_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(vget_high_u8(sPlane0Lower_u_8x16), bias0_u_8x8));

			const int16x8_t source1_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(source1_u_8x8, bias1_u_8x8));
			const int16x8_t source2_s_16x8 = vreinterpretq_s16_u16(vsubl_u8(source2_u_8x8, bias2_u_8x8));


			// now we apply the 3x3 matrix multiplication

			int16x8_t intermediateResults0_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel01_64_s_16x8); // downsampled channel 1
			int16x8_t intermediateResults1_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel11_64_s_16x8);
			int16x8_t intermediateResults2_s_16x8 = vmulq_s16(source1_s_16x8, factorChannel21_64_s_16x8);

			intermediateResults0_s_16x8 = vqaddq_s16(intermediateResults0_s_16x8, vmulq_s16(source2_s_16x8, factorChannel02_64_s_16x8)); // downsampled channel 2
			intermediateResults1_s_16x8 = vqaddq_s16(intermediateResults1_s_16x8, vmulq_s16(source2_s_16x8, factorChannel12_64_s_16x8));
			intermediateResults2_s_16x8 = vqaddq_s16(intermediateResults2_s_16x8, vmulq_s16(source2_s_16x8, factorChannel22_64_s_16x8));

			const int16x8x2_t intermediateResults0_AB_s_16x8x2 = vzipq_s16(intermediateResults0_s_16x8, intermediateResults0_s_16x8); // upsampling channel the results
			const int16x8x2_t intermediateResults1_AB_s_16x8x2 = vzipq_s16(intermediateResults1_s_16x8, intermediateResults1_s_16x8);
			const int16x8x2_t intermediateResults2_AB_s_16x8x2 = vzipq_s16(intermediateResults2_s_16x8, intermediateResults2_s_16x8);


			const int16x8_t intermediateResults0Upper_A_s_16x8 = vqaddq_s16(intermediateResults0_AB_s_16x8x2.val[0], vmulq_s16(source0Upper_A_s_16x8, factorChannel00_64_s_16x8)); // channel 0, upper row
			const int16x8_t intermediateResults1Upper_A_s_16x8 = vqaddq_s16(intermediateResults1_AB_s_16x8x2.val[0], vmulq_s16(source0Upper_A_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2Upper_A_s_16x8 = vqaddq_s16(intermediateResults2_AB_s_16x8x2.val[0], vmulq_s16(source0Upper_A_s_16x8, factorChannel20_64_s_16x8));

			const int16x8_t intermediateResults0Upper_B_s_16x8 = vqaddq_s16(intermediateResults0_AB_s_16x8x2.val[1], vmulq_s16(source0Upper_B_s_16x8, factorChannel00_64_s_16x8));
			const int16x8_t intermediateResults1Upper_B_s_16x8 = vqaddq_s16(intermediateResults1_AB_s_16x8x2.val[1], vmulq_s16(source0Upper_B_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2Upper_B_s_16x8 = vqaddq_s16(intermediateResults2_AB_s_16x8x2.val[1], vmulq_s16(source0Upper_B_s_16x8, factorChannel20_64_s_16x8));


			const int16x8_t intermediateResults0Lower_A_s_16x8 = vqaddq_s16(intermediateResults0_AB_s_16x8x2.val[0], vmulq_s16(source0Lower_A_s_16x8, factorChannel00_64_s_16x8)); // channel 0, lower row
			const int16x8_t intermediateResults1Lower_A_s_16x8 = vqaddq_s16(intermediateResults1_AB_s_16x8x2.val[0], vmulq_s16(source0Lower_A_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2Lower_A_s_16x8 = vqaddq_s16(intermediateResults2_AB_s_16x8x2.val[0], vmulq_s16(source0Lower_A_s_16x8, factorChannel20_64_s_16x8));

			const int16x8_t intermediateResults0Lower_B_s_16x8 = vqaddq_s16(intermediateResults0_AB_s_16x8x2.val[1], vmulq_s16(source0Lower_B_s_16x8, factorChannel00_64_s_16x8));
			const int16x8_t intermediateResults1Lower_B_s_16x8 = vqaddq_s16(intermediateResults1_AB_s_16x8x2.val[1], vmulq_s16(source0Lower_B_s_16x8, factorChannel10_64_s_16x8));
			const int16x8_t intermediateResults2Lower_B_s_16x8 = vqaddq_s16(intermediateResults2_AB_s_16x8x2.val[1], vmulq_s16(source0Lower_B_s_16x8, factorChannel20_64_s_16x8));


			// writing upper results

			uint8x16x4_t resultsUpper_u_8x16x4;

			// saturated narrow signed to unsigned, normalized by 2^6
			resultsUpper_u_8x16x4.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0Upper_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults0Upper_B_s_16x8, 6));
			resultsUpper_u_8x16x4.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1Upper_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults1Upper_B_s_16x8, 6));
			resultsUpper_u_8x16x4.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2Upper_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults2Upper_B_s_16x8, 6));
			resultsUpper_u_8x16x4.val[3] = valueChannel3_u_8x16;

			// and we can store the result
			vst4q_u8(targetPlaneUpper, resultsUpper_u_8x16x4);


			// writing lower results

			uint8x16x4_t resultsLower_u_8x16x4;

			// saturated narrow signed to unsigned, normalized by 2^6
			resultsLower_u_8x16x4.val[0] = vcombine_u8(vqrshrun_n_s16(intermediateResults0Lower_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults0Lower_B_s_16x8, 6));
			resultsLower_u_8x16x4.val[1] = vcombine_u8(vqrshrun_n_s16(intermediateResults1Lower_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults1Lower_B_s_16x8, 6));
			resultsLower_u_8x16x4.val[2] = vcombine_u8(vqrshrun_n_s16(intermediateResults2Lower_A_s_16x8, 6), vqrshrun_n_s16(intermediateResults2Lower_B_s_16x8, 6));
			resultsLower_u_8x16x4.val[3] = valueChannel3_u_8x16;

			// and we can store the result
			vst4q_u8(targetPlaneLower, resultsLower_u_8x16x4);

			sourcePlane0Upper += blockSize;
			sourcePlane1 += blockSize / 2u;
			sourcePlane2 += blockSize / 2u;

			targetPlaneUpper += blockSize * 4u;
			targetPlaneLower += blockSize * 4u;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (sourcePlane0Upper != sourcePlane0UpperEnd)
	{
		ocean_assert(sourcePlane0Upper < sourcePlane0UpperEnd);

		const int16_t source1 = int16_t(sourcePlane1[0]) - int16_t(bias1);
		const int16_t source2 = int16_t(sourcePlane2[0]) - int16_t(bias2);

		const int intermediate0 = source1 * int16_t(factorChannel01_64) + source2 * int16_t(factorChannel02_64);
		const int intermediate1 = source1 * int16_t(factorChannel11_64) + source2 * int16_t(factorChannel12_64);
		const int intermediate2 = source1 * int16_t(factorChannel21_64) + source2 * int16_t(factorChannel22_64);

		// first and second upper pixel

		const int16_t source0UpperLeft = int16_t(sourcePlane0Upper[0]) - int16_t(bias0);
		const int16_t source0UpperRight = int16_t(sourcePlane0Upper[1]) - int16_t(bias0);

		targetPlaneUpper[0] = (uint8_t)(minmax<int>(0, (source0UpperLeft * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlaneUpper[1] = (uint8_t)(minmax<int>(0, (source0UpperLeft * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlaneUpper[2] = (uint8_t)(minmax<int>(0, (source0UpperLeft * int16_t(factorChannel20_64) + intermediate2) / 64, 255));
		targetPlaneUpper[3] = (uint8_t)valueChannel3;

		targetPlaneUpper[4] = (uint8_t)(minmax<int>(0, (source0UpperRight * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlaneUpper[5] = (uint8_t)(minmax<int>(0, (source0UpperRight * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlaneUpper[6] = (uint8_t)(minmax<int>(0, (source0UpperRight * int16_t(factorChannel20_64) + intermediate2) / 64, 255));
		targetPlaneUpper[7] = (uint8_t)valueChannel3;


		// first and second lower pixel

		const int16_t source0LowerLeft = int16_t(sourcePlane0Upper[sourcePlane0StrideElements + 0u]) - int16_t(bias0);
		const int16_t source0LowerRight = int16_t(sourcePlane0Upper[sourcePlane0StrideElements + 1u]) - int16_t(bias0);

		targetPlaneLower[0] = (uint8_t)(minmax<int>(0, (source0LowerLeft * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlaneLower[1] = (uint8_t)(minmax<int>(0, (source0LowerLeft * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlaneLower[2] = (uint8_t)(minmax<int>(0, (source0LowerLeft * int16_t(factorChannel20_64) + intermediate2) / 64, 255));
		targetPlaneLower[3] = (uint8_t)valueChannel3;

		targetPlaneLower[4] = (uint8_t)(minmax<int>(0, (source0LowerRight * int16_t(factorChannel00_64) + intermediate0) / 64, 255));
		targetPlaneLower[5] = (uint8_t)(minmax<int>(0, (source0LowerRight * int16_t(factorChannel10_64) + intermediate1) / 64, 255));
		targetPlaneLower[6] = (uint8_t)(minmax<int>(0, (source0LowerRight * int16_t(factorChannel20_64) + intermediate2) / 64, 255));
		targetPlaneLower[7] = (uint8_t)valueChannel3;

		sourcePlane0Upper += 2;
		sourcePlane1++;
		sourcePlane2++;

		targetPlaneUpper += 2u * 4u;
		targetPlaneLower += 2u * 4u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 4u>(targetPlaneUpper - width * 4u, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 4u>(targetPlaneLower - width * 4u, width);
	}
}

void FrameConverter::convertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 1u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePaddingElements
	// uint32_t: targetPaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

	const unsigned int sourcePaddingElements = (unsigned int)intOptions[0];
	const unsigned int targetPaddingElements = (unsigned int)intOptions[1];

	const uint8_t* source = (const uint8_t*)(sources[0]);
	uint8_t* target = (uint8_t*)(targets[0]);

	const unsigned int sourceStrideElements = width * 2u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	const int factorChannel00_1024 = intOptions[2];
	const int factorChannel10_1024 = intOptions[3];
	const int factorChannel20_1024 = intOptions[4];

	const int factorChannel01_1024 = intOptions[5];
	const int factorChannel11_1024 = intOptions[6];
	const int factorChannel21_1024 = intOptions[7];

	const int factorChannel02_1024 = intOptions[8];
	const int factorChannel12_1024 = intOptions[9];
	const int factorChannel22_1024 = intOptions[10];

	const int bias0 = intOptions[11];
	const int bias1 = intOptions[12];
	const int bias2 = intOptions[13];

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	source += multipleRowIndex * sourceStrideElements;

	target = flipTarget ? (target + (height - multipleRowIndex - 1u) * targetStrideElements) : target + multipleRowIndex * targetStrideElements;

	const uint8_t* const sourceEnd = source + width * 2u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	if (blocks > 0u)
	{
		const int16x4_t factorChannel00_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel00_1024));
		const int16x4_t factorChannel10_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel10_1024));
		const int16x4_t factorChannel20_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel20_1024));

		const int16x4_t factorChannel01_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel01_1024));
		const int16x4_t factorChannel11_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel11_1024));
		const int16x4_t factorChannel21_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel21_1024));

		const int16x4_t factorChannel02_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel02_1024));
		const int16x4_t factorChannel12_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel12_1024));
		const int16x4_t factorChannel22_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel22_1024));

		const int32x4_t bias0_1024_s_32x4 = vdupq_n_s32(bias0 * 1024);
		const int32x4_t bias1_1024_s_32x4 = vdupq_n_s32(bias1 * 1024);
		const int32x4_t bias2_1024_s_32x4 = vdupq_n_s32(bias2 * 1024);

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			const uint8x16x2_t source_u_8x16x2 = vld2q_u8(source);

			const uint8x16_t& source0_u_8x16 = source_u_8x16x2.val[0];

			const uint8x8x2_t source12_u_8x8x2 = vuzp_u8(vget_low_u8(source_u_8x16x2.val[1]), vget_high_u8(source_u_8x16x2.val[1]));

			const int16x8_t source1_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(source12_u_8x8x2.val[0]));
			const int16x8_t source2_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(source12_u_8x8x2.val[1]));

			const int16x4_t source1A_s_16x4 = vget_low_s16(source1_s_16x8);
			const int16x4_t source1B_s_16x4 = vget_high_s16(source1_s_16x8);
			const int16x4_t source2A_s_16x4 = vget_low_s16(source2_s_16x8);
			const int16x4_t source2B_s_16x4 = vget_high_s16(source2_s_16x8);


			// first, handling sub-sampled channel 1 and 2

			int32x4_t source12IntermediateResults0A_s_32x4 = vmlal_s16(bias0_1024_s_32x4, source1A_s_16x4, factorChannel01_1024_s_16x4); // f01 * channel1 + b0
			int32x4_t source12IntermediateResults1A_s_32x4 = vmlal_s16(bias1_1024_s_32x4, source1A_s_16x4, factorChannel11_1024_s_16x4); // f11 * channel1 + b1
			int32x4_t source12IntermediateResults2A_s_32x4 = vmlal_s16(bias2_1024_s_32x4, source1A_s_16x4, factorChannel21_1024_s_16x4); // f21 * channel1 + b2

			int32x4_t source12IntermediateResults0B_s_32x4 = vmlal_s16(bias0_1024_s_32x4, source1B_s_16x4, factorChannel01_1024_s_16x4);
			int32x4_t source12IntermediateResults1B_s_32x4 = vmlal_s16(bias1_1024_s_32x4, source1B_s_16x4, factorChannel11_1024_s_16x4);
			int32x4_t source12IntermediateResults2B_s_32x4 = vmlal_s16(bias2_1024_s_32x4, source1B_s_16x4, factorChannel21_1024_s_16x4);

			source12IntermediateResults0A_s_32x4 = vmlal_s16(source12IntermediateResults0A_s_32x4, source2A_s_16x4, factorChannel02_1024_s_16x4); // += f02 * channel2
			source12IntermediateResults1A_s_32x4 = vmlal_s16(source12IntermediateResults1A_s_32x4, source2A_s_16x4, factorChannel12_1024_s_16x4); // += f12 * channel2
			source12IntermediateResults2A_s_32x4 = vmlal_s16(source12IntermediateResults2A_s_32x4, source2A_s_16x4, factorChannel22_1024_s_16x4); // += f22 * channel2

			source12IntermediateResults0B_s_32x4 = vmlal_s16(source12IntermediateResults0B_s_32x4, source2B_s_16x4, factorChannel02_1024_s_16x4);
			source12IntermediateResults1B_s_32x4 = vmlal_s16(source12IntermediateResults1B_s_32x4, source2B_s_16x4, factorChannel12_1024_s_16x4);
			source12IntermediateResults2B_s_32x4 = vmlal_s16(source12IntermediateResults2B_s_32x4, source2B_s_16x4, factorChannel22_1024_s_16x4);


			// now, handling cannel 0

			const int16x8_t source0A_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(source0_u_8x16)));
			const int16x8_t source0B_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(source0_u_8x16)));

			const int16x4_t source0A_s_16x4 = vget_low_s16(source0A_s_16x8);
			const int16x4_t source0B_s_16x4 = vget_high_s16(source0A_s_16x8);
			const int16x4_t source0C_s_16x4 = vget_low_s16(source0B_s_16x8);
			const int16x4_t source0D_s_16x4 = vget_high_s16(source0B_s_16x8);

			const int32x4_t sourceMultiplied0A_s_32x4 = vmull_s16(source0A_s_16x4, factorChannel00_1024_s_16x4); // f00 * channel0
			const int32x4_t sourceMultiplied0B_s_32x4 = vmull_s16(source0B_s_16x4, factorChannel00_1024_s_16x4);
			const int32x4_t sourceMultiplied0C_s_32x4 = vmull_s16(source0C_s_16x4, factorChannel00_1024_s_16x4);
			const int32x4_t sourceMultiplied0D_s_32x4 = vmull_s16(source0D_s_16x4, factorChannel00_1024_s_16x4);

			const int32x4_t sourceMultiplied1A_s_32x4 = vmull_s16(source0A_s_16x4, factorChannel10_1024_s_16x4); // f10 * channel0
			const int32x4_t sourceMultiplied1B_s_32x4 = vmull_s16(source0B_s_16x4, factorChannel10_1024_s_16x4);
			const int32x4_t sourceMultiplied1C_s_32x4 = vmull_s16(source0C_s_16x4, factorChannel10_1024_s_16x4);
			const int32x4_t sourceMultiplied1D_s_32x4 = vmull_s16(source0D_s_16x4, factorChannel10_1024_s_16x4);

			const int32x4_t sourceMultiplied2A_s_32x4 = vmull_s16(source0A_s_16x4, factorChannel20_1024_s_16x4); // f20 * channel0
			const int32x4_t sourceMultiplied2B_s_32x4 = vmull_s16(source0B_s_16x4, factorChannel20_1024_s_16x4);
			const int32x4_t sourceMultiplied2C_s_32x4 = vmull_s16(source0C_s_16x4, factorChannel20_1024_s_16x4);
			const int32x4_t sourceMultiplied2D_s_32x4 = vmull_s16(source0D_s_16x4, factorChannel20_1024_s_16x4);


			// now, we can align channel 0 with channel 1 and 2

			const int32x4x2_t source12IntermediateResults0A_s_32x4x2 = vzipq_s32(source12IntermediateResults0A_s_32x4, source12IntermediateResults0A_s_32x4);
			const int32x4x2_t source12IntermediateResults1A_s_32x4x2 = vzipq_s32(source12IntermediateResults1A_s_32x4, source12IntermediateResults1A_s_32x4);
			const int32x4x2_t source12IntermediateResults2A_s_32x4x2 = vzipq_s32(source12IntermediateResults2A_s_32x4, source12IntermediateResults2A_s_32x4);

			const int32x4x2_t source12IntermediateResults0B_s_32x4x2 = vzipq_s32(source12IntermediateResults0B_s_32x4, source12IntermediateResults0B_s_32x4);
			const int32x4x2_t source12IntermediateResults1B_s_32x4x2 = vzipq_s32(source12IntermediateResults1B_s_32x4, source12IntermediateResults1B_s_32x4);
			const int32x4x2_t source12IntermediateResults2B_s_32x4x2 = vzipq_s32(source12IntermediateResults2B_s_32x4, source12IntermediateResults2B_s_32x4);


			// now, we can determine the results

			const int32x4_t result0A_s_32x4 = vaddq_s32(source12IntermediateResults0A_s_32x4x2.val[0], sourceMultiplied0A_s_32x4);
			const int32x4_t result0B_s_32x4 = vaddq_s32(source12IntermediateResults0A_s_32x4x2.val[1], sourceMultiplied0B_s_32x4);
			const int32x4_t result0C_s_32x4 = vaddq_s32(source12IntermediateResults0B_s_32x4x2.val[0], sourceMultiplied0C_s_32x4);
			const int32x4_t result0D_s_32x4 = vaddq_s32(source12IntermediateResults0B_s_32x4x2.val[1], sourceMultiplied0D_s_32x4);

			const int32x4_t result1A_s_32x4 = vaddq_s32(source12IntermediateResults1A_s_32x4x2.val[0], sourceMultiplied1A_s_32x4);
			const int32x4_t result1B_s_32x4 = vaddq_s32(source12IntermediateResults1A_s_32x4x2.val[1], sourceMultiplied1B_s_32x4);
			const int32x4_t result1C_s_32x4 = vaddq_s32(source12IntermediateResults1B_s_32x4x2.val[0], sourceMultiplied1C_s_32x4);
			const int32x4_t result1D_s_32x4 = vaddq_s32(source12IntermediateResults1B_s_32x4x2.val[1], sourceMultiplied1D_s_32x4);

			const int32x4_t result2A_s_32x4 = vaddq_s32(source12IntermediateResults2A_s_32x4x2.val[0], sourceMultiplied2A_s_32x4);
			const int32x4_t result2B_s_32x4 = vaddq_s32(source12IntermediateResults2A_s_32x4x2.val[1], sourceMultiplied2B_s_32x4);
			const int32x4_t result2C_s_32x4 = vaddq_s32(source12IntermediateResults2B_s_32x4x2.val[0], sourceMultiplied2C_s_32x4);
			const int32x4_t result2D_s_32x4 = vaddq_s32(source12IntermediateResults2B_s_32x4x2.val[1], sourceMultiplied2D_s_32x4);


			uint8x16x3_t results_u_8x16x3;

			// saturated narrow signed to unsigned
			results_u_8x16x3.val[0] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(result0A_s_32x4, 10), vqrshrun_n_s32(result0B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(result0C_s_32x4, 10), vqrshrun_n_s32(result0D_s_32x4, 10))));
			results_u_8x16x3.val[1] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(result1A_s_32x4, 10), vqrshrun_n_s32(result1B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(result1C_s_32x4, 10), vqrshrun_n_s32(result1D_s_32x4, 10))));
			results_u_8x16x3.val[2] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(result2A_s_32x4, 10), vqrshrun_n_s32(result2B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(result2C_s_32x4, 10), vqrshrun_n_s32(result2D_s_32x4, 10))));

			// and we can store the result
			vst3q_u8(target, results_u_8x16x3);

			source += blockSize * 2u;
			target += blockSize * 3u;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		const int intermediate0 = source[1] * int16_t(factorChannel01_1024) + source[3] * int16_t(factorChannel02_1024);
		const int intermediate1 = source[1] * int16_t(factorChannel11_1024) + source[3] * int16_t(factorChannel12_1024);
		const int intermediate2 = source[1] * int16_t(factorChannel21_1024) + source[3] * int16_t(factorChannel22_1024);

		// first and second pixel

		target[0] = (uint8_t)(minmax<int>(0, (source[0] * int16_t(factorChannel00_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		target[1] = (uint8_t)(minmax<int>(0, (source[0] * int16_t(factorChannel10_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		target[2] = (uint8_t)(minmax<int>(0, (source[0] * int16_t(factorChannel20_1024) + intermediate2) / 1024 + int16_t(bias2), 255));

		target[3] = (uint8_t)(minmax<int>(0, (source[2] * int16_t(factorChannel00_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		target[4] = (uint8_t)(minmax<int>(0, (source[2] * int16_t(factorChannel10_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		target[5] = (uint8_t)(minmax<int>(0, (source[2] * int16_t(factorChannel20_1024) + intermediate2) / 1024 + int16_t(bias2), 255));

		source += 2u * 2u;
		target += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(target - width * 3u, width);
	}
}

void FrameConverter::convertOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel_Precision10Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 1u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePaddingElements
	// uint32_t: targetPaddingElements
	//  int32_t: f00
	//  int32_t: f10
	//  int32_t: f20
	//  int32_t: f01
	//  ...
	//  int32_t: f22
	//  int32_t: b0
	//  int32_t: b1
	//  int32_t: b2

	// with transformation:
	// t0 = f00 * s0 + f01 * s1 + f02 * s2 + b0
	// t1 = f10 * s0 + f11 * s1 + f12 * s2 + b1
	// t2 = f20 * s0 + f21 * s1 + f22 * s2 + b2

	const unsigned int sourcePaddingElements = (unsigned int)intOptions[0];
	const unsigned int targetPaddingElements = (unsigned int)intOptions[1];

	const uint8_t* source = (const uint8_t*)(sources[0]);
	uint8_t* target = (uint8_t*)(targets[0]);

	const unsigned int sourceStrideElements = width * 2u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	const int factorChannel00_1024 = intOptions[2];
	const int factorChannel10_1024 = intOptions[3];
	const int factorChannel20_1024 = intOptions[4];

	const int factorChannel01_1024 = intOptions[5];
	const int factorChannel11_1024 = intOptions[6];
	const int factorChannel21_1024 = intOptions[7];

	const int factorChannel02_1024 = intOptions[8];
	const int factorChannel12_1024 = intOptions[9];
	const int factorChannel22_1024 = intOptions[10];

	const int bias0 = intOptions[11];
	const int bias1 = intOptions[12];
	const int bias2 = intOptions[13];

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	source += multipleRowIndex * sourceStrideElements;

	target = flipTarget ? (target + (height - multipleRowIndex - 1u) * targetStrideElements) : target + multipleRowIndex * targetStrideElements;

	const uint8_t* const sourceEnd = source + width * 2u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	if (blocks > 0u)
	{
		const int16x4_t factorChannel00_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel00_1024));
		const int16x4_t factorChannel10_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel10_1024));
		const int16x4_t factorChannel20_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel20_1024));

		const int16x4_t factorChannel01_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel01_1024));
		const int16x4_t factorChannel11_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel11_1024));
		const int16x4_t factorChannel21_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel21_1024));

		const int16x4_t factorChannel02_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel02_1024));
		const int16x4_t factorChannel12_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel12_1024));
		const int16x4_t factorChannel22_1024_s_16x4 = vdup_n_s16(int16_t(factorChannel22_1024));

		const int32x4_t bias0_1024_s_32x4 = vdupq_n_s32(bias0 * 1024);
		const int32x4_t bias1_1024_s_32x4 = vdupq_n_s32(bias1 * 1024);
		const int32x4_t bias2_1024_s_32x4 = vdupq_n_s32(bias2 * 1024);

		for (unsigned int n = 0u; n < blocks; ++n)
		{
			const uint8x16x2_t source_u_8x16x2 = vld2q_u8(source);

			const uint8x16_t& source1_u_8x16 = source_u_8x16x2.val[1];

			const uint8x8x2_t source02_u_8x8x2 = vuzp_u8(vget_low_u8(source_u_8x16x2.val[0]), vget_high_u8(source_u_8x16x2.val[0]));

			const int16x8_t source0_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(source02_u_8x8x2.val[0]));
			const int16x8_t source2_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(source02_u_8x8x2.val[1]));

			const int16x4_t source0A_s_16x4 = vget_low_s16(source0_s_16x8);
			const int16x4_t source0B_s_16x4 = vget_high_s16(source0_s_16x8);
			const int16x4_t source2A_s_16x4 = vget_low_s16(source2_s_16x8);
			const int16x4_t source2B_s_16x4 = vget_high_s16(source2_s_16x8);


			// first, handling sub-sampled channel 1 and 2

			int32x4_t source02IntermediateResults0A_s_32x4 = vmlal_s16(bias0_1024_s_32x4, source0A_s_16x4, factorChannel00_1024_s_16x4); // f00 * channel0 + b0
			int32x4_t source02IntermediateResults1A_s_32x4 = vmlal_s16(bias1_1024_s_32x4, source0A_s_16x4, factorChannel10_1024_s_16x4); // f10 * channel0 + b1
			int32x4_t source02IntermediateResults2A_s_32x4 = vmlal_s16(bias2_1024_s_32x4, source0A_s_16x4, factorChannel20_1024_s_16x4); // f20 * channel0 + b2

			int32x4_t source02IntermediateResults0B_s_32x4 = vmlal_s16(bias0_1024_s_32x4, source0B_s_16x4, factorChannel00_1024_s_16x4);
			int32x4_t source02IntermediateResults1B_s_32x4 = vmlal_s16(bias1_1024_s_32x4, source0B_s_16x4, factorChannel10_1024_s_16x4);
			int32x4_t source02IntermediateResults2B_s_32x4 = vmlal_s16(bias2_1024_s_32x4, source0B_s_16x4, factorChannel20_1024_s_16x4);

			source02IntermediateResults0A_s_32x4 = vmlal_s16(source02IntermediateResults0A_s_32x4, source2A_s_16x4, factorChannel02_1024_s_16x4); // += f02 * channel2
			source02IntermediateResults1A_s_32x4 = vmlal_s16(source02IntermediateResults1A_s_32x4, source2A_s_16x4, factorChannel12_1024_s_16x4); // += f12 * channel2
			source02IntermediateResults2A_s_32x4 = vmlal_s16(source02IntermediateResults2A_s_32x4, source2A_s_16x4, factorChannel22_1024_s_16x4); // += f22 * channel2

			source02IntermediateResults0B_s_32x4 = vmlal_s16(source02IntermediateResults0B_s_32x4, source2B_s_16x4, factorChannel02_1024_s_16x4);
			source02IntermediateResults1B_s_32x4 = vmlal_s16(source02IntermediateResults1B_s_32x4, source2B_s_16x4, factorChannel12_1024_s_16x4);
			source02IntermediateResults2B_s_32x4 = vmlal_s16(source02IntermediateResults2B_s_32x4, source2B_s_16x4, factorChannel22_1024_s_16x4);


			// now, handling cannel 0

			const int16x8_t source1A_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_low_u8(source1_u_8x16)));
			const int16x8_t source1B_s_16x8 = vreinterpretq_s16_u16(vmovl_u8(vget_high_u8(source1_u_8x16)));

			const int16x4_t source1A_s_16x4 = vget_low_s16(source1A_s_16x8);
			const int16x4_t source1B_s_16x4 = vget_high_s16(source1A_s_16x8);
			const int16x4_t source1C_s_16x4 = vget_low_s16(source1B_s_16x8);
			const int16x4_t source1D_s_16x4 = vget_high_s16(source1B_s_16x8);

			const int32x4_t sourceMultiplied0A_s_32x4 = vmull_s16(source1A_s_16x4, factorChannel01_1024_s_16x4); // f01 * channel1
			const int32x4_t sourceMultiplied0B_s_32x4 = vmull_s16(source1B_s_16x4, factorChannel01_1024_s_16x4);
			const int32x4_t sourceMultiplied0C_s_32x4 = vmull_s16(source1C_s_16x4, factorChannel01_1024_s_16x4);
			const int32x4_t sourceMultiplied0D_s_32x4 = vmull_s16(source1D_s_16x4, factorChannel01_1024_s_16x4);

			const int32x4_t sourceMultiplied1A_s_32x4 = vmull_s16(source1A_s_16x4, factorChannel11_1024_s_16x4); // f11 * channel1
			const int32x4_t sourceMultiplied1B_s_32x4 = vmull_s16(source1B_s_16x4, factorChannel11_1024_s_16x4);
			const int32x4_t sourceMultiplied1C_s_32x4 = vmull_s16(source1C_s_16x4, factorChannel11_1024_s_16x4);
			const int32x4_t sourceMultiplied1D_s_32x4 = vmull_s16(source1D_s_16x4, factorChannel11_1024_s_16x4);

			const int32x4_t sourceMultiplied2A_s_32x4 = vmull_s16(source1A_s_16x4, factorChannel21_1024_s_16x4); // f21 * channel1
			const int32x4_t sourceMultiplied2B_s_32x4 = vmull_s16(source1B_s_16x4, factorChannel21_1024_s_16x4);
			const int32x4_t sourceMultiplied2C_s_32x4 = vmull_s16(source1C_s_16x4, factorChannel21_1024_s_16x4);
			const int32x4_t sourceMultiplied2D_s_32x4 = vmull_s16(source1D_s_16x4, factorChannel21_1024_s_16x4);


			// now, we can align channel 1 with channel 0 and 2

			const int32x4x2_t source02IntermediateResults0A_s_32x4x2 = vzipq_s32(source02IntermediateResults0A_s_32x4, source02IntermediateResults0A_s_32x4);
			const int32x4x2_t source02IntermediateResults1A_s_32x4x2 = vzipq_s32(source02IntermediateResults1A_s_32x4, source02IntermediateResults1A_s_32x4);
			const int32x4x2_t source02IntermediateResults2A_s_32x4x2 = vzipq_s32(source02IntermediateResults2A_s_32x4, source02IntermediateResults2A_s_32x4);

			const int32x4x2_t source02IntermediateResults0B_s_32x4x2 = vzipq_s32(source02IntermediateResults0B_s_32x4, source02IntermediateResults0B_s_32x4);
			const int32x4x2_t source02IntermediateResults1B_s_32x4x2 = vzipq_s32(source02IntermediateResults1B_s_32x4, source02IntermediateResults1B_s_32x4);
			const int32x4x2_t source02IntermediateResults2B_s_32x4x2 = vzipq_s32(source02IntermediateResults2B_s_32x4, source02IntermediateResults2B_s_32x4);


			// now, we can determine the results

			const int32x4_t result0A_s_32x4 = vaddq_s32(source02IntermediateResults0A_s_32x4x2.val[0], sourceMultiplied0A_s_32x4);
			const int32x4_t result0B_s_32x4 = vaddq_s32(source02IntermediateResults0A_s_32x4x2.val[1], sourceMultiplied0B_s_32x4);
			const int32x4_t result0C_s_32x4 = vaddq_s32(source02IntermediateResults0B_s_32x4x2.val[0], sourceMultiplied0C_s_32x4);
			const int32x4_t result0D_s_32x4 = vaddq_s32(source02IntermediateResults0B_s_32x4x2.val[1], sourceMultiplied0D_s_32x4);

			const int32x4_t result1A_s_32x4 = vaddq_s32(source02IntermediateResults1A_s_32x4x2.val[0], sourceMultiplied1A_s_32x4);
			const int32x4_t result1B_s_32x4 = vaddq_s32(source02IntermediateResults1A_s_32x4x2.val[1], sourceMultiplied1B_s_32x4);
			const int32x4_t result1C_s_32x4 = vaddq_s32(source02IntermediateResults1B_s_32x4x2.val[0], sourceMultiplied1C_s_32x4);
			const int32x4_t result1D_s_32x4 = vaddq_s32(source02IntermediateResults1B_s_32x4x2.val[1], sourceMultiplied1D_s_32x4);

			const int32x4_t result2A_s_32x4 = vaddq_s32(source02IntermediateResults2A_s_32x4x2.val[0], sourceMultiplied2A_s_32x4);
			const int32x4_t result2B_s_32x4 = vaddq_s32(source02IntermediateResults2A_s_32x4x2.val[1], sourceMultiplied2B_s_32x4);
			const int32x4_t result2C_s_32x4 = vaddq_s32(source02IntermediateResults2B_s_32x4x2.val[0], sourceMultiplied2C_s_32x4);
			const int32x4_t result2D_s_32x4 = vaddq_s32(source02IntermediateResults2B_s_32x4x2.val[1], sourceMultiplied2D_s_32x4);


			uint8x16x3_t results_u_8x16x3;

			// saturated narrow signed to unsigned
			results_u_8x16x3.val[0] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(result0A_s_32x4, 10), vqrshrun_n_s32(result0B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(result0C_s_32x4, 10), vqrshrun_n_s32(result0D_s_32x4, 10))));
			results_u_8x16x3.val[1] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(result1A_s_32x4, 10), vqrshrun_n_s32(result1B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(result1C_s_32x4, 10), vqrshrun_n_s32(result1D_s_32x4, 10))));
			results_u_8x16x3.val[2] = vcombine_u8(vqmovn_u16(vcombine_u16(vqrshrun_n_s32(result2A_s_32x4, 10), vqrshrun_n_s32(result2B_s_32x4, 10))), vqmovn_u16(vcombine_u16(vqrshrun_n_s32(result2C_s_32x4, 10), vqrshrun_n_s32(result2D_s_32x4, 10))));

			// and we can store the result
			vst3q_u8(target, results_u_8x16x3);

			source += blockSize * 2u;
			target += blockSize * 3u;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		const int intermediate0 = source[0] * int16_t(factorChannel00_1024) + source[2] * int16_t(factorChannel02_1024);
		const int intermediate1 = source[0] * int16_t(factorChannel10_1024) + source[2] * int16_t(factorChannel12_1024);
		const int intermediate2 = source[0] * int16_t(factorChannel20_1024) + source[2] * int16_t(factorChannel22_1024);

		// first and second pixel

		target[0] = (uint8_t)(minmax<int>(0, (source[1] * int16_t(factorChannel01_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		target[1] = (uint8_t)(minmax<int>(0, (source[1] * int16_t(factorChannel11_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		target[2] = (uint8_t)(minmax<int>(0, (source[1] * int16_t(factorChannel21_1024) + intermediate2) / 1024 + int16_t(bias2), 255));

		target[3] = (uint8_t)(minmax<int>(0, (source[3] * int16_t(factorChannel01_1024) + intermediate0) / 1024 + int16_t(bias0), 255));
		target[4] = (uint8_t)(minmax<int>(0, (source[3] * int16_t(factorChannel11_1024) + intermediate1) / 1024 + int16_t(bias1), 255));
		target[5] = (uint8_t)(minmax<int>(0, (source[3] * int16_t(factorChannel21_1024) + intermediate2) / 1024 + int16_t(bias2), 255));

		source += 2u * 2u;
		target += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(target - width * 3u, width);
	}
}

template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
void FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	static_assert(tSourceChannelIndex0 < 3u && tSourceChannelIndex1 < 3u && tSourceChannelIndex2 < 3u, "Invalid source channels!");

	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 1u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePaddingElements
	// uint32_t: targetPaddingElements

	const unsigned int sourcePaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int targetPaddingElements = (unsigned int)(intOptions[1]);

	const uint8_t* source = (const uint8_t*)sources[0];
	uint8_t* target = (uint8_t*)targets[0];

	const unsigned int sourceStrideElements = width * 2u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	source += multipleRowIndex * sourceStrideElements;

	target = flipTarget ? (target + (height - multipleRowIndex - 1u) * targetStrideElements) : target + multipleRowIndex * targetStrideElements;

	const uint8_t* const sourceEnd = source + width * 2u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16x2_t source_u_8x16x2 = vld2q_u8(source);

		const uint8x16_t& source0_u_8x16 = source_u_8x16x2.val[0];

		const uint8x16x2_t source12_u_8x16x2 = vtrnq_u8(source_u_8x16x2.val[1], source_u_8x16x2.val[1]);
		const uint8x16_t& source1_u_8x16 = source12_u_8x16x2.val[0];
		const uint8x16_t& source2_u_8x16 = source12_u_8x16x2.val[1];

		uint8x16x3_t results_u_8x16x3;

		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			results_u_8x16x3.val[0] = source0_u_8x16;
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			results_u_8x16x3.val[0] = source1_u_8x16;
		}
		else
		{
			ocean_assert(tSourceChannelIndex0 == 2u);
			results_u_8x16x3.val[0] = source2_u_8x16;
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			results_u_8x16x3.val[1] = source0_u_8x16;
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			results_u_8x16x3.val[1] = source1_u_8x16;
		}
		else
		{
			ocean_assert(tSourceChannelIndex1 == 2u);
			results_u_8x16x3.val[1] = source2_u_8x16;
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			results_u_8x16x3.val[2] = source0_u_8x16;
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			results_u_8x16x3.val[2] = source1_u_8x16;
		}
		else
		{
			ocean_assert(tSourceChannelIndex2 == 2u);
			results_u_8x16x3.val[2] = source2_u_8x16;
		}


		// and we can store the result
		vst3q_u8(target, results_u_8x16x3);

		source += blockSize * 2u;
		target += blockSize * 3u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			target[0] = source[0]; // YUYV -> Y   Y
			target[3] = source[2]; // 0123    012 345
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			target[0] = source[1]; // YUYV -> U   U
			target[3] = source[1]; // 0123    012 345
		}
		else
		{
			ocean_assert(tSourceChannelIndex0 == 2u);

			target[0] = source[3]; // YUYV -> V   V
			target[3] = source[3]; // 0123    012 345
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			target[1] = source[0];
			target[4] = source[2];
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			target[1] = source[1]; // YUYV -> YUV, YUV
			target[4] = source[1]; // 0123    012  345
		}
		else
		{
			ocean_assert(tSourceChannelIndex1 == 2u);

			target[1] = source[3];
			target[4] = source[3];
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			target[2] = source[0];
			target[5] = source[2];
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			target[2] = source[1];
			target[5] = source[1];
		}
		else
		{
			ocean_assert(tSourceChannelIndex2 == 2u);

			target[2] = source[3]; // YUYV -> YUV, YUV
			target[5] = source[3]; // 0123    012  345
		}

		source += 2u * 2u;
		target += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(target - width * 3u, width);
	}
}

template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel<2u, 0u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1BackIsDownsampled_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);

template <unsigned int tSourceChannelIndex0, unsigned int tSourceChannelIndex1, unsigned int tSourceChannelIndex2>
void FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	static_assert(tSourceChannelIndex0 < 3u && tSourceChannelIndex1 < 3u && tSourceChannelIndex2 < 3u, "Invalid source channels!");

	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 2u && width % 2u == 0u);
	ocean_assert(height >= 1u);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePaddingElements
	// uint32_t: targetPaddingElements

	const unsigned int sourcePaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int targetPaddingElements = (unsigned int)(intOptions[1]);

	const uint8_t* source = (const uint8_t*)sources[0];
	uint8_t* target = (uint8_t*)targets[0];

	const unsigned int sourceStrideElements = width * 2u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	source += multipleRowIndex * sourceStrideElements;

	target = flipTarget ? (target + (height - multipleRowIndex - 1u) * targetStrideElements) : target + multipleRowIndex * targetStrideElements;

	const uint8_t* const sourceEnd = source + width * 2u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16x2_t source_u_8x16x2 = vld2q_u8(source);

		const uint8x16_t& source1_u_8x16 = source_u_8x16x2.val[1];

		const uint8x16x2_t source02_u_8x16x2 = vtrnq_u8(source_u_8x16x2.val[0], source_u_8x16x2.val[0]);
		const uint8x16_t& source0_u_8x16 = source02_u_8x16x2.val[0];
		const uint8x16_t& source2_u_8x16 = source02_u_8x16x2.val[1];

		uint8x16x3_t results_u_8x16x3;

		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			results_u_8x16x3.val[0] = source0_u_8x16;
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			results_u_8x16x3.val[0] = source1_u_8x16;
		}
		else
		{
			ocean_assert(tSourceChannelIndex0 == 2u);
			results_u_8x16x3.val[0] = source2_u_8x16;
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			results_u_8x16x3.val[1] = source0_u_8x16;
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			results_u_8x16x3.val[1] = source1_u_8x16;
		}
		else
		{
			ocean_assert(tSourceChannelIndex1 == 2u);
			results_u_8x16x3.val[1] = source2_u_8x16;
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			results_u_8x16x3.val[2] = source0_u_8x16;
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			results_u_8x16x3.val[2] = source1_u_8x16;
		}
		else
		{
			ocean_assert(tSourceChannelIndex2 == 2u);
			results_u_8x16x3.val[2] = source2_u_8x16;
		}


		// and we can store the result
		vst3q_u8(target, results_u_8x16x3);

		source += blockSize * 2u;
		target += blockSize * 3u;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		// target channel 0:

		if constexpr (tSourceChannelIndex0 == 0u)
		{
			target[0] = source[0]; // UYVY -> U   U
			target[3] = source[0]; // 0123    012 345
		}
		else if constexpr (tSourceChannelIndex0 == 1u)
		{
			target[0] = source[1]; // UYVY -> Y   Y
			target[3] = source[3]; // 0123    012 345
		}
		else
		{
			ocean_assert(tSourceChannelIndex0 == 2u);

			target[0] = source[2]; // UYVY -> V   V
			target[3] = source[2]; // 0123    012 345
		}


		// target channel 1:

		if constexpr (tSourceChannelIndex1 == 0u)
		{
			target[1] = source[0];
			target[4] = source[0];
		}
		else if constexpr (tSourceChannelIndex1 == 1u)
		{
			target[1] = source[1];
			target[4] = source[3];
		}
		else
		{
			ocean_assert(tSourceChannelIndex1 == 2u);

			target[1] = source[2];
			target[4] = source[2];
		}


		// target channel 2:

		if constexpr (tSourceChannelIndex2 == 0u)
		{
			target[2] = source[0];
			target[5] = source[0];
		}
		else if constexpr (tSourceChannelIndex2 == 1u)
		{
			target[2] = source[1];
			target[5] = source[3];
		}
		else
		{
			ocean_assert(tSourceChannelIndex2 == 2u);

			target[2] = source[2];
			target[5] = source[2];
		}

		source += 2u * 2u;
		target += 2u * 3u;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(target - width * 3u, width);
	}
}

template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel<0u, 1u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel<0u, 2u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel<1u, 0u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel<1u, 2u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel<2u, 0u, 1u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::mapOneRow_1Plane3ChannelsWith2ChannelsDownsampled2x1FrontIsDownsampled_To_1Plane3Channels_8BitPerChannel<2u, 1u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);

template <unsigned int tIndexRed, unsigned int tIndexGreen, unsigned int tIndexBlue>
void FrameConverter::convertTwoRows_1PlaneMosaicPacked10Bit_To_1PlaneUnpacked3Channels8Bit(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	static_assert(tIndexRed < 3u && tIndexGreen < 3u && tIndexBlue < 3u, "Invalid channel indices!");
	static_assert(tIndexRed != tIndexGreen && tIndexRed != tIndexBlue && tIndexGreen != tIndexBlue, "Invalid channel indices!");

	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 4u && width % 4u == 0u && height % 2u == 0u);
	ocean_assert(height >= 2u);

	ocean_assert(multipleRowIndex < height / 2u);

	const unsigned int rowIndex = multipleRowIndex * 2u;

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePaddingElements
	// uint32_t: targetPaddingElements

	const unsigned int sourcePaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int targetPaddingElements = (unsigned int)(intOptions[1]);

	const uint8_t* source = (const uint8_t*)(sources[0]);
	uint8_t* target = (uint8_t*)(targets[0]);

	const unsigned int sourceStrideElements = (width * 5u) / 4u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	// example RGGB10_PACKED

	// R G R G X   R G R G X   R G . .
	// G B G B X   G B G B X   G B . .
	// R G R G X   R G R G X   R G . .
	// G B G B X   G B G B X   G B . .
	// R G R G X   R G R G X   R G . .

	// second pixel in third row:
	// red:         green:       blue:
	// G B G B      G B G B      G b G B
	// r G r G      R g R G      R G R G
	// G B G B      G B G B      G b G B
	// (sampling pixels marked with lower case characters)

	// third pixel in third row:
	// red:         green:       blue:
	// G B G B      G B g B      G b G b
	// R G r G      R g R g      R G R G
	// G B G B      G B g B      G b G b
	// (sampling pixels marked with lower case characters)

	// in case we are handling the two top rows or two bottom rows, mirror the missing rows to the correct row (1, or height - 2)

	const unsigned int topRowIndex = rowIndex > 0u ? (rowIndex - 1u) : 1u;
	const unsigned int bottomRowIndex = rowIndex + 2u < height ? rowIndex + 2u : (height - 2u);

	const uint8_t* sourceRowA = source + topRowIndex * sourceStrideElements;
	const uint8_t* sourceRowB = source + rowIndex * sourceStrideElements;
	const uint8_t* sourceRowC = source + (rowIndex + 1u) * sourceStrideElements;
	const uint8_t* sourceRowD = source + bottomRowIndex * sourceStrideElements;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	uint8_t* targetRow0 = nullptr;
	uint8_t* targetRow1 = nullptr;

	if (flipTarget)
	{
		targetRow0 = target + (height - rowIndex - 1u) * targetStrideElements;
		targetRow1 = target + (height - rowIndex - 2u) * targetStrideElements;
	}
	else
	{
		targetRow0 = target + rowIndex * targetStrideElements;
		targetRow1 = target + (rowIndex + 1u) * targetStrideElements;
	}

	ocean_assert(targetRow0 != nullptr);
	ocean_assert(targetRow1 != nullptr);

	// first pixel in rows

	targetRow0[0u + tIndexRed] = sourceRowB[0];
	targetRow0[0u + tIndexGreen] = (sourceRowA[0] + sourceRowC[0] + 1u) / 2u;
	targetRow0[0u + tIndexBlue] = (sourceRowA[1] + sourceRowC[1] + 1u) / 2u;

	targetRow1[0u + tIndexRed] = (sourceRowB[0] + sourceRowD[0] + 1u) / 2u;
	targetRow1[0u + tIndexGreen] = sourceRowC[0];
	targetRow1[0u + tIndexBlue] = sourceRowC[1];

	targetRow0 += 3;
	targetRow1 += 3;

	for (unsigned int x = 1u; x < width - 3u; x += 4u)
	{
		// first row, first core pixel
		targetRow0[0u + tIndexRed] = (sourceRowB[0] + sourceRowB[2] + 1u) / 2u;
		targetRow0[0u + tIndexGreen] = sourceRowB[1];
		targetRow0[0u + tIndexBlue] = (sourceRowA[1] + sourceRowC[1] + 1u) / 2u;

		// second pixel
		targetRow0[3u + tIndexRed] = sourceRowB[2];
		targetRow0[3u + tIndexGreen] = (sourceRowA[2] + sourceRowB[1] + sourceRowB[3] + sourceRowC[2] + 2u) / 4u;
		targetRow0[3u + tIndexBlue] = (sourceRowA[1] + sourceRowA[3] + sourceRowC[1] + sourceRowC[3] + 2u) / 4u;

		// third pixel
		targetRow0[6u + tIndexRed] = (sourceRowB[2] + sourceRowB[5] + 1u) / 2u; // sourceRowB[5]: we skip one element between mosaic pixel 3 and 5 (as this contains the 2 bits for each of the first four pixels)
		targetRow0[6u + tIndexGreen] = sourceRowB[3];
		targetRow0[6u + tIndexBlue] = (sourceRowA[3] + sourceRowC[3] + 1u) / 2u;

		// fourth pixel
		targetRow0[9u + tIndexRed] = sourceRowB[5];
		targetRow0[9u + tIndexGreen] = (sourceRowA[5] + sourceRowB[3] + sourceRowB[6] + sourceRowC[5] + 2u) / 4u;
		targetRow0[9u + tIndexBlue] = (sourceRowA[3] + sourceRowA[6] + sourceRowC[3] + sourceRowC[6] + 2u) / 4u;


		// second row, first core pixel
		targetRow1[0u + tIndexRed] = (sourceRowB[0] + sourceRowB[2] + sourceRowD[0] + sourceRowD[2] + 2u) / 4u;
		targetRow1[0u + tIndexGreen] = (sourceRowB[1] + sourceRowC[0] + sourceRowC[2] + sourceRowD[1] + 2u) / 4u;
		targetRow1[0u + tIndexBlue] = sourceRowC[1];

		// second pixel
		targetRow1[3u + tIndexRed] = (sourceRowB[2] + sourceRowD[2] + 1u) / 2u;
		targetRow1[3u + tIndexGreen] = sourceRowC[2];
		targetRow1[3u + tIndexBlue] = (sourceRowC[1] + sourceRowC[3] + 1u) / 2u;

		// third pixel
		targetRow1[6u + tIndexRed] = (sourceRowB[2] + sourceRowB[5] + sourceRowD[2] + sourceRowD[5] + 2u) / 4u;
		targetRow1[6u + tIndexGreen] = (sourceRowB[3] + sourceRowC[2] + sourceRowC[5] + sourceRowD[3] + 2u) / 4u;
		targetRow1[6u + tIndexBlue] = sourceRowC[3];

		// fourth pixel
		targetRow1[9u + tIndexRed] = (sourceRowB[5] + sourceRowD[5] + 1u) / 2u;
		targetRow1[9u + tIndexGreen] = sourceRowC[5];
		targetRow1[9u + tIndexBlue] = (sourceRowC[3] + sourceRowC[6] + 1u) / 2u;


		targetRow0 += 12;
		targetRow1 += 12;

		sourceRowA += 5; // 4 + 1 for the packed 2 bit per pixel
		sourceRowB += 5;
		sourceRowC += 5;
		sourceRowD += 5;
	}

	// last three pixels

	// first row, first pixel
	targetRow0[0u + tIndexRed] = (sourceRowB[0] + sourceRowB[2] + 1u) / 2u;
	targetRow0[0u + tIndexGreen] = sourceRowB[1];
	targetRow0[0u + tIndexBlue] = (sourceRowA[1] + sourceRowC[1] + 1u) / 2u;

	// second pixel
	targetRow0[3u + tIndexRed] = sourceRowB[2];
	targetRow0[3u + tIndexGreen] = (sourceRowA[2] + sourceRowC[2] + sourceRowB[1] + sourceRowB[3] + 2u) / 4u;
	targetRow0[3u + tIndexBlue] = (sourceRowA[1] + sourceRowA[3] + sourceRowC[1] + sourceRowC[3] + 2u) / 4u;

	// third pixel
	targetRow0[6u + tIndexRed] = sourceRowB[2];
	targetRow0[6u + tIndexGreen] = sourceRowB[3];
	targetRow0[6u + tIndexBlue] = (sourceRowA[3] + sourceRowC[3] + 1u) / 2u;


	// second row, first pixel
	targetRow1[0u + tIndexRed] = (sourceRowB[0] + sourceRowB[2] + sourceRowD[0] + sourceRowD[2] + 2u) / 4u;
	targetRow1[0u + tIndexGreen] = (sourceRowB[1] + sourceRowC[0] + sourceRowC[2] + sourceRowD[1] + 2u) / 4u;
	targetRow1[0u + tIndexBlue] = sourceRowC[1];

	// second pixel
	targetRow1[3u + tIndexRed] = (sourceRowB[2] + sourceRowD[2] + 1u) / 2u;
	targetRow1[3u + tIndexGreen] = sourceRowC[2];
	targetRow1[3u + tIndexBlue] = (sourceRowC[1] + sourceRowC[3] + 1u) / 2u;

	// third pixel
	targetRow1[6u + tIndexRed] = (sourceRowB[2] + sourceRowD[2] + 1u) / 2u;
	targetRow1[6u + tIndexGreen] = (sourceRowB[3] + sourceRowD[3] + 1u) / 2u;
	targetRow1[6u + tIndexBlue] = sourceRowC[3];

	if (mirrorTarget)
	{
		// note that the target pointers are currently 3 pixels from the end of the row
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetRow0 - (width - 3u) * 3u, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetRow1 - (width - 3u) * 3u, width);
	}
}

template void OCEAN_CV_EXPORT FrameConverter::convertTwoRows_1PlaneMosaicPacked10Bit_To_1PlaneUnpacked3Channels8Bit<0u, 1u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::convertTwoRows_1PlaneMosaicPacked10Bit_To_1PlaneUnpacked3Channels8Bit<2u, 1u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);

template <unsigned int tIndexRed, unsigned int tIndexGreen, unsigned int tIndexBlue>
void FrameConverter::convertTwoRows_1PlaneMosaicPacked10Bit_To_1PlaneUnpacked3Channels8BitAdvanced(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	static_assert(tIndexRed < 3u && tIndexGreen < 3u && tIndexBlue < 3u, "Invalid channel indices!");
	static_assert(tIndexRed != tIndexGreen && tIndexRed != tIndexBlue && tIndexGreen != tIndexBlue, "Invalid channel indices!");

	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 4u && width % 4u == 0u && height % 2u == 0u);
	ocean_assert(height >= 2u);

	ocean_assert(multipleRowIndex < height / 2u);

	const unsigned int rowIndex = multipleRowIndex * 2u;

	ocean_assert(options != nullptr);
	const RGGB10ToRGB24AdvancedOptions* rggb10ToRgb24AdvancedOptions = reinterpret_cast<const RGGB10ToRGB24AdvancedOptions*>(options);

	const uint16_t blackLevel = rggb10ToRgb24AdvancedOptions->blackLevel;
	ocean_assert(blackLevel < 1024u);
	const uint32_t* const whiteBalance7 = rggb10ToRgb24AdvancedOptions->whiteBalance7;
	const uint8_t* const gammaLookupValues = rggb10ToRgb24AdvancedOptions->gammaLookupValues;

	const unsigned int sourcePaddingElements = rggb10ToRgb24AdvancedOptions->sourcePaddingElements;
	const unsigned int targetPaddingElements = rggb10ToRgb24AdvancedOptions->targetPaddingElements;

	const uint8_t* source = (const uint8_t*)(sources[0]);
	uint8_t* target = (uint8_t*)(targets[0]);

	const unsigned int sourceStrideElements = (width * 5u) / 4u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	// example RGGB10_PACKED

	// R G R G X   R G R G X   R G . .
	// G B G B X   G B G B X   G B . .
	// R G R G X   R G R G X   R G . .
	// G B G B X   G B G B X   G B . .
	// R G R G X   R G R G X   R G . .

	// second pixel in third row:
	// red:         green:       blue:
	// G B G B      G B G B      G b G B
	// r G r G      R g R G      R G R G
	// G B G B      G B G B      G b G B
	// (sampling pixels marked with lower case characters)

	// third pixel in third row:
	// red:         green:       blue:
	// G B G B      G B g B      G b G b
	// R G r G      R g R g      R G R G
	// G B G B      G B g B      G b G b
	// (sampling pixels marked with lower case characters)

	// in case we are handling the two top rows or two bottom rows, mirror the missing rows to the correct row (1, or height - 2)

	const unsigned int topRowIndex = rowIndex > 0u ? (rowIndex - 1u) : 1u;
	const unsigned int bottomRowIndex = rowIndex + 2u < height ? rowIndex + 2u : (height - 2u);

	const uint8_t* sourceRowA = source + topRowIndex * sourceStrideElements;
	const uint8_t* sourceRowB = source + rowIndex * sourceStrideElements;
	const uint8_t* sourceRowC = source + (rowIndex + 1u) * sourceStrideElements;
	const uint8_t* sourceRowD = source + bottomRowIndex * sourceStrideElements;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	uint8_t* targetRow0 = nullptr;
	uint8_t* targetRow1 = nullptr;

	if (flipTarget)
	{
		targetRow0 = target + (height - rowIndex - 1u) * targetStrideElements;
		targetRow1 = target + (height - rowIndex - 2u) * targetStrideElements;
	}
	else
	{
		targetRow0 = target + rowIndex * targetStrideElements;
		targetRow1 = target + (rowIndex + 1u) * targetStrideElements;
	}

	ocean_assert(targetRow0 != nullptr);
	ocean_assert(targetRow1 != nullptr);

	// Memory for the unpacked pixel values: 4 values of the current block and 4 of the next block
	uint16_t sourceBlockA[8];
	uint16_t sourceBlockB[8];
	uint16_t sourceBlockC[8];
	uint16_t sourceBlockD[8];

	// Unpack the current block and then subtract blacklevel and apply white balance
	unpack5ElementsBayerMosaicPacked10Bit(sourceRowA, sourceBlockA);
	unpack5ElementsBayerMosaicPacked10Bit(sourceRowB, sourceBlockB);
	unpack5ElementsBayerMosaicPacked10Bit(sourceRowC, sourceBlockC);
	unpack5ElementsBayerMosaicPacked10Bit(sourceRowD, sourceBlockD);

	sourceBlockA[0] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockA[0]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);
	sourceBlockA[1] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockA[1]) - int32_t(blackLevel)) * whiteBalance7[tIndexBlue], 1023 * 128) + 64u) >> 7u);
	sourceBlockA[2] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockA[2]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);
	sourceBlockA[3] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockA[3]) - int32_t(blackLevel)) * whiteBalance7[tIndexBlue], 1023 * 128) + 64u) >> 7u);

	sourceBlockB[0] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockB[0]) - int32_t(blackLevel)) * whiteBalance7[tIndexRed], 1023 * 128) + 64u) >> 7u);
	sourceBlockB[1] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockB[1]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);
	sourceBlockB[2] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockB[2]) - int32_t(blackLevel)) * whiteBalance7[tIndexRed], 1023 * 128) + 64u) >> 7u);
	sourceBlockB[3] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockB[3]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);

	sourceBlockC[0] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockC[0]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);
	sourceBlockC[1] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockC[1]) - int32_t(blackLevel)) * whiteBalance7[tIndexBlue], 1023 * 128) + 64u) >> 7u);
	sourceBlockC[2] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockC[2]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);
	sourceBlockC[3] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockC[3]) - int32_t(blackLevel)) * whiteBalance7[tIndexBlue], 1023 * 128) + 64u) >> 7u);

	sourceBlockD[0] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockD[0]) - int32_t(blackLevel)) * whiteBalance7[tIndexRed], 1023 * 128) + 64u) >> 7u);
	sourceBlockD[1] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockD[1]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);
	sourceBlockD[2] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockD[2]) - int32_t(blackLevel)) * whiteBalance7[tIndexRed], 1023 * 128) + 64u) >> 7u);
	sourceBlockD[3] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockD[3]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);

	// first pixel in rows
	targetRow0[0u + tIndexRed] = gammaLookupValues[sourceBlockB[0]];
	targetRow0[0u + tIndexGreen] = gammaLookupValues[(sourceBlockA[0] + sourceBlockC[0] + 1u) / 2u];
	targetRow0[0u + tIndexBlue] = gammaLookupValues[(sourceBlockA[1] + sourceBlockC[1] + 1u) / 2u];

	targetRow1[0u + tIndexRed] = gammaLookupValues[(sourceBlockB[0] + sourceBlockD[0] + 1u) / 2u];
	targetRow1[0u + tIndexGreen] = gammaLookupValues[sourceBlockC[0]];
	targetRow1[0u + tIndexBlue] = gammaLookupValues[sourceBlockC[1]];

	targetRow0 += 3;
	targetRow1 += 3;

	for (unsigned int x = 1u; x < width - 5u; x += 4u)
	{
		// Unpack the next block and then subtract blacklevel and apply white balance
		unpack5ElementsBayerMosaicPacked10Bit(sourceRowA + 5, sourceBlockA + 4);
		unpack5ElementsBayerMosaicPacked10Bit(sourceRowB + 5, sourceBlockB + 4);
		unpack5ElementsBayerMosaicPacked10Bit(sourceRowC + 5, sourceBlockC + 4);
		unpack5ElementsBayerMosaicPacked10Bit(sourceRowD + 5, sourceBlockD + 4);

		sourceBlockA[4] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockA[4]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);
		sourceBlockA[5] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockA[5]) - int32_t(blackLevel)) * whiteBalance7[tIndexBlue], 1023 * 128) + 64u) >> 7u);
		sourceBlockA[6] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockA[6]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);
		sourceBlockA[7] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockA[7]) - int32_t(blackLevel)) * whiteBalance7[tIndexBlue], 1023 * 128) + 64u) >> 7u);

		sourceBlockB[4] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockB[4]) - int32_t(blackLevel)) * whiteBalance7[tIndexRed], 1023 * 128) + 64u) >> 7u);
		sourceBlockB[5] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockB[5]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);
		sourceBlockB[6] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockB[6]) - int32_t(blackLevel)) * whiteBalance7[tIndexRed], 1023 * 128) + 64u) >> 7u);
		sourceBlockB[7] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockB[7]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);

		sourceBlockC[4] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockC[4]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);
		sourceBlockC[5] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockC[5]) - int32_t(blackLevel)) * whiteBalance7[tIndexBlue], 1023 * 128) + 64u) >> 7u);
		sourceBlockC[6] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockC[6]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);
		sourceBlockC[7] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockC[7]) - int32_t(blackLevel)) * whiteBalance7[tIndexBlue], 1023 * 128) + 64u) >> 7u);

		sourceBlockD[4] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockD[4]) - int32_t(blackLevel)) * whiteBalance7[tIndexRed], 1023 * 128) + 64u) >> 7u);
		sourceBlockD[5] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockD[5]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);
		sourceBlockD[6] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockD[6]) - int32_t(blackLevel)) * whiteBalance7[tIndexRed], 1023 * 128) + 64u) >> 7u);
		sourceBlockD[7] =  uint16_t((minmax<int32_t>(0, (int32_t(sourceBlockD[7]) - int32_t(blackLevel)) * whiteBalance7[tIndexGreen], 1023 * 128) + 64u) >> 7u);

		// first row, first core pixel
		targetRow0[0u + tIndexRed] = gammaLookupValues[(sourceBlockB[0] + sourceBlockB[2] + 1u) / 2u];
		targetRow0[0u + tIndexGreen] = gammaLookupValues[sourceBlockB[1]];
		targetRow0[0u + tIndexBlue] = gammaLookupValues[(sourceBlockA[1] + sourceBlockC[1] + 1u) / 2u];

		// second pixel
		targetRow0[3u + tIndexRed] = gammaLookupValues[sourceBlockB[2]];
		targetRow0[3u + tIndexGreen] = gammaLookupValues[(sourceBlockA[2] + sourceBlockB[1] + sourceBlockB[3] + sourceBlockC[2] + 2u) / 4u];
		targetRow0[3u + tIndexBlue] = gammaLookupValues[(sourceBlockA[1] + sourceBlockA[3] + sourceBlockC[1] + sourceBlockC[3] + 2u) / 4u];

		// third pixel
		targetRow0[6u + tIndexRed] = gammaLookupValues[(sourceBlockB[2] + sourceBlockB[4] + 1u) / 2u];
		targetRow0[6u + tIndexGreen] = gammaLookupValues[sourceBlockB[3]];
		targetRow0[6u + tIndexBlue] = gammaLookupValues[(sourceBlockA[3] + sourceBlockC[3] + 1u) / 2u];

		// fourth pixel
		targetRow0[9u + tIndexRed] = gammaLookupValues[sourceBlockB[4]];
		targetRow0[9u + tIndexGreen] = gammaLookupValues[(sourceBlockA[4] + sourceBlockB[3] + sourceBlockB[5] + sourceBlockC[4] + 2u) / 4u];
		targetRow0[9u + tIndexBlue] = gammaLookupValues[(sourceBlockA[3] + sourceBlockA[5] + sourceBlockC[3] + sourceBlockC[5] + 2u) / 4u];


		// second row, first core pixel
		targetRow1[0u + tIndexRed] = gammaLookupValues[(sourceBlockB[0] + sourceBlockB[2] + sourceBlockD[0] + sourceBlockD[2] + 2u) / 4u];
		targetRow1[0u + tIndexGreen] = gammaLookupValues[(sourceBlockB[1] + sourceBlockC[0] + sourceBlockC[2] + sourceBlockD[1] + 2u) / 4u];
		targetRow1[0u + tIndexBlue] = gammaLookupValues[sourceBlockC[1]];

		// second pixel
		targetRow1[3u + tIndexRed] = gammaLookupValues[(sourceBlockB[2] + sourceBlockD[2] + 1u) / 2u];
		targetRow1[3u + tIndexGreen] = gammaLookupValues[sourceBlockC[2]];
		targetRow1[3u + tIndexBlue] = gammaLookupValues[(sourceBlockC[1] + sourceBlockC[3] + 1u) / 2u];

		// third pixel
		targetRow1[6u + tIndexRed] = gammaLookupValues[(sourceBlockB[2] + sourceBlockB[4] + sourceBlockD[2] + sourceBlockD[4] + 2u) / 4u];
		targetRow1[6u + tIndexGreen] = gammaLookupValues[(sourceBlockB[3] + sourceBlockC[2] + sourceBlockC[4] + sourceBlockD[3] + 2u) / 4u];
		targetRow1[6u + tIndexBlue] = gammaLookupValues[sourceBlockC[3]];

		// fourth pixel
		targetRow1[9u + tIndexRed] = gammaLookupValues[(sourceBlockB[4] + sourceBlockD[4] + 1u) / 2u];
		targetRow1[9u + tIndexGreen] = gammaLookupValues[sourceBlockC[4]];
		targetRow1[9u + tIndexBlue] = gammaLookupValues[(sourceBlockC[3] + sourceBlockC[5] + 1u) / 2u];


		memcpy(sourceBlockA, sourceBlockA + 4, 4 * sizeof(uint16_t)); // previous block = current block
		memcpy(sourceBlockB, sourceBlockB + 4, 4 * sizeof(uint16_t));
		memcpy(sourceBlockC, sourceBlockC + 4, 4 * sizeof(uint16_t));
		memcpy(sourceBlockD, sourceBlockD + 4, 4 * sizeof(uint16_t));

		targetRow0 += 12;
		targetRow1 += 12;

		sourceRowA += 5; // 4 + 1 for the packed 2 bit per pixel
		sourceRowB += 5;
		sourceRowC += 5;
		sourceRowD += 5;
	}

	// last three pixels

	// first row, first pixel
	targetRow0[0u + tIndexRed] = gammaLookupValues[(sourceBlockB[0] + sourceBlockB[2] + 1u) / 2u];
	targetRow0[0u + tIndexGreen] = gammaLookupValues[sourceBlockB[1]];
	targetRow0[0u + tIndexBlue] = gammaLookupValues[(sourceBlockA[1] + sourceBlockC[1] + 1u) / 2u];

	// second pixel
	targetRow0[3u + tIndexRed] = gammaLookupValues[sourceBlockB[2]];
	targetRow0[3u + tIndexGreen] = gammaLookupValues[(sourceBlockA[2] + sourceBlockC[2] + sourceBlockB[1] + sourceBlockB[3] + 2u) / 4u];
	targetRow0[3u + tIndexBlue] = gammaLookupValues[(sourceBlockA[1] + sourceBlockA[3] + sourceBlockC[1] + sourceBlockC[3] + 2u) / 4u];

	// third pixel
	targetRow0[6u + tIndexRed] = gammaLookupValues[sourceBlockB[2]];
	targetRow0[6u + tIndexGreen] = gammaLookupValues[sourceBlockB[3]];
	targetRow0[6u + tIndexBlue] = gammaLookupValues[(sourceBlockA[3] + sourceBlockC[3] + 1u) / 2u];


	// second row, first pixel
	targetRow1[0u + tIndexRed] = gammaLookupValues[(sourceBlockB[0] + sourceBlockB[2] + sourceBlockD[0] + sourceBlockD[2] + 2u) / 4u];
	targetRow1[0u + tIndexGreen] = gammaLookupValues[(sourceBlockB[1] + sourceBlockC[0] + sourceBlockC[2] + sourceBlockD[1] + 2u) / 4u];
	targetRow1[0u + tIndexBlue] = gammaLookupValues[sourceBlockC[1]];

	// second pixel
	targetRow1[3u + tIndexRed] = gammaLookupValues[(sourceBlockB[2] + sourceBlockD[2] + 1u) / 2u];
	targetRow1[3u + tIndexGreen] = gammaLookupValues[sourceBlockC[2]];
	targetRow1[3u + tIndexBlue] = gammaLookupValues[(sourceBlockC[1] + sourceBlockC[3] + 1u) / 2u];

	// third pixel
	targetRow1[6u + tIndexRed] = gammaLookupValues[(sourceBlockB[2] + sourceBlockD[2] + 1u) / 2u];
	targetRow1[6u + tIndexGreen] = gammaLookupValues[(sourceBlockB[3] + sourceBlockD[3] + 1u) / 2u];
	targetRow1[6u + tIndexBlue] = gammaLookupValues[sourceBlockC[3]];

	if (mirrorTarget)
	{
		// note that the target pointers are currently 3 pixels from the end of the row
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetRow0 - (width - 3u) * 3u, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>(targetRow1 - (width - 3u) * 3u, width);
	}
}

template void OCEAN_CV_EXPORT FrameConverter::convertTwoRows_1PlaneMosaicPacked10Bit_To_1PlaneUnpacked3Channels8BitAdvanced<0u, 1u, 2u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);
template void OCEAN_CV_EXPORT FrameConverter::convertTwoRows_1PlaneMosaicPacked10Bit_To_1PlaneUnpacked3Channels8BitAdvanced<2u, 1u, 0u>(const void**, void**, const unsigned int, const unsigned int, const unsigned int, const ConversionFlag, const void*);

void FrameConverter::mapOneRow_1Plane2Channels_To_2Planes1Channel_8BitPerChannel(const void** sources, void** targets, const unsigned int multipleRowIndex, const unsigned int width, const unsigned int height, const ConversionFlag conversionFlag, const void* options)
{
	ocean_assert(sources != nullptr && targets != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert(multipleRowIndex < height);

	ocean_assert(options != nullptr);
	const int* intOptions = reinterpret_cast<const int*>(options);
	ocean_assert(intOptions != nullptr);

	// options layout:
	// uint32_t: sourcePaddingElements
	// uint32_t: targetPaddingElements0
	// uint32_t: targetPaddingElements1

	const unsigned int sourcePaddingElements = (unsigned int)(intOptions[0]);
	const unsigned int targetPaddingElements0 = (unsigned int)(intOptions[1]);
	const unsigned int targetPaddingElements1 = (unsigned int)(intOptions[2]);

	const uint8_t* source = (const uint8_t*)(sources[0]);
	uint8_t* target0 = (uint8_t*)(targets[0]);
	uint8_t* target1 = (uint8_t*)(targets[1]);

	const unsigned int sourceStrideElements = width * 2u + sourcePaddingElements;
	const unsigned int targetStrideElements0 = width + targetPaddingElements0;
	const unsigned int targetStrideElements1 = width + targetPaddingElements1;

	const bool flipTarget = conversionFlag == CONVERT_FLIPPED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;
	const bool mirrorTarget = conversionFlag == CONVERT_MIRRORED || conversionFlag == CONVERT_FLIPPED_AND_MIRRORED;

	source += multipleRowIndex * sourceStrideElements;

	target0 = flipTarget ? (target0 + (height - multipleRowIndex - 1u) * targetStrideElements0) : target0 + multipleRowIndex * targetStrideElements0;
	target1 = flipTarget ? (target1 + (height - multipleRowIndex - 1u) * targetStrideElements1) : target1 + multipleRowIndex * targetStrideElements1;

	const uint8_t* const sourceEnd = source + width * 2u;

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	constexpr unsigned int blockSize = 16u;
	const unsigned int blocks = width / blockSize;

	for (unsigned int n = 0u; n < blocks; ++n)
	{
		const uint8x16x2_t source_u_8x16x2 = vld2q_u8(source);

		vst1q_u8(target0, source_u_8x16x2.val[0]);
		vst1q_u8(target1, source_u_8x16x2.val[1]);

		source += 32;
		target0 += 16;
		target1 += 16;
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	while (source != sourceEnd)
	{
		ocean_assert(source < sourceEnd);

		target0[0] = source[0];
		target1[0] = source[1];

		source += 2u;
		++target0;
		++target1;
	}

	if (mirrorTarget)
	{
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>(target0 - width, width);
		CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 1u>(target1 - width, width);
	}
}

}

}
