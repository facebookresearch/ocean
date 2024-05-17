/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameConverterColorMap.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameConverterY8.h"
#include "ocean/cv/FrameNormalizer.h"

#include "ocean/math/HSVAColor.h"
#include "ocean/math/RGBAColor.h"

namespace Ocean
{

namespace CV
{

bool FrameConverterColorMap::Comfort::convert1ChannelToRGB24(const Frame& source, Frame& target, const ColorMap colorMap, Worker* worker)
{
	if (source.dataType() == FrameType::DT_SIGNED_FLOAT_32 || source.dataType() == FrameType::DT_SIGNED_FLOAT_64)
	{
		return convertFloat1ChannelToRGB24(source, target, colorMap, worker);
	}
	else
	{
		return convertInteger1ChannelToRGB24(source, target, colorMap, worker);
	}
}

bool FrameConverterColorMap::Comfort::convertFloat1ChannelToRGB24(const Frame& source, Frame& target, const ColorMap colorMap, Worker* worker, float minValue, float maxValue)
{
	if (!source.isValid() || colorMap == CM_INVALID)
	{
		return false;
	}

	Frame convertedSourceFrame(source, Frame::AdvancedCopyMode::ACM_USE_KEEP_LAYOUT);

	if (!source.isPixelFormatCompatible(FrameType::FORMAT_F32))
	{
		if (!source.isPixelFormatCompatible(FrameType::FORMAT_F64))
		{
			return false;
		}

		if (!convertedSourceFrame.set(FrameType(source.frameType(), FrameType::FORMAT_F32), true /*forceOwner*/, true /*forceWritable*/))
		{
			return false;
		}

		FrameConverter::cast<double, float>(source.constdata<double>(), convertedSourceFrame.data<float>(), source.width(), source.height(), source.channels(), source.paddingElements(), convertedSourceFrame.paddingElements());
	}

	ocean_assert(convertedSourceFrame.isValid());
	ocean_assert(convertedSourceFrame.isPixelFormatCompatible(FrameType::FORMAT_F32));

	if (!target.isValid() || target.width() != source.width() || target.height() != source.height() || target.pixelFormat() != FrameType::FORMAT_RGB24)
	{
		if (!target.set(FrameType(convertedSourceFrame, FrameType::FORMAT_RGB24), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}

	if (minValue == NumericF::maxValue() || maxValue == NumericF::minValue())
	{
		constexpr bool tIgnoreInfinity = true;
		FrameMinMax::determineMinMaxValues<float, 1u, tIgnoreInfinity>(convertedSourceFrame.constdata<float>(), convertedSourceFrame.width(), convertedSourceFrame.height(), convertedSourceFrame.paddingElements(), minValue == NumericF::maxValue() ? &minValue : nullptr, maxValue == NumericF::minValue() ? &maxValue : nullptr);
	}

	if (minValue > maxValue)
	{
		return false;
	}

	if (NumericF::isEqual(minValue, maxValue))
	{
		target.setValue(0x00u);

		return true;
	}
	else
	{
		switch (colorMap)
		{
			case CM_INVALID:
			{
				ocean_assert(false && "Invalid color map!");
				break;
			}

			case CM_JET:
			{
				convertJetFloat1ChannelToRGB24(convertedSourceFrame.constdata<float>(), target.data<uint8_t>(), convertedSourceFrame.width(), convertedSourceFrame.height(), CONVERT_NORMAL, convertedSourceFrame.paddingElements(), target.paddingElements(), minValue, maxValue, worker);
				return true;
			}

			case CM_TURBO:
			{
				convertTurboFloat1ChannelToRGB24(convertedSourceFrame.constdata<float>(), target.data<uint8_t>(), convertedSourceFrame.width(), convertedSourceFrame.height(), CONVERT_NORMAL, convertedSourceFrame.paddingElements(), target.paddingElements(), minValue, maxValue, worker);
				return true;
			}

			case CM_INFERNO:
			{
				convertInfernoFloat1ChannelToRGB24(convertedSourceFrame.constdata<float>(), target.data<uint8_t>(), convertedSourceFrame.width(), convertedSourceFrame.height(), CONVERT_NORMAL, convertedSourceFrame.paddingElements(), target.paddingElements(), minValue, maxValue, worker);
				return true;
			}

			case CM_SPIRAL_10:
			{
				const float period = (maxValue - minValue) * 0.1f;
				convertSpiralFloat1ChannelToRGB24(convertedSourceFrame.constdata<float>(), target.data<uint8_t>(), convertedSourceFrame.width(), convertedSourceFrame.height(), CONVERT_NORMAL, convertedSourceFrame.paddingElements(), target.paddingElements(), minValue, maxValue, period, worker);
				return true;
			}

			case CM_SPIRAL_20:
			{
				const float period = (maxValue - minValue) * 0.05f;
				convertSpiralFloat1ChannelToRGB24(convertedSourceFrame.constdata<float>(), target.data<uint8_t>(), convertedSourceFrame.width(), convertedSourceFrame.height(), CONVERT_NORMAL, convertedSourceFrame.paddingElements(), target.paddingElements(), minValue, maxValue, period, worker);
				return true;
			}

			case CM_LINEAR:
			{
				Frame yFrame(FrameType(convertedSourceFrame, FrameType::FORMAT_Y8));
				CV::FrameNormalizer::normalize1ChannelToUint8<float>(convertedSourceFrame.constdata<float>(), yFrame.data<uint8_t>(), convertedSourceFrame.width(), convertedSourceFrame.height(), convertedSourceFrame.paddingElements(), yFrame.paddingElements(), worker);

				CV::FrameConverterY8::convertY8ToRGB24(yFrame.constdata<uint8_t>(), target.data<uint8_t>(), yFrame.width(), yFrame.height(), CONVERT_NORMAL, yFrame.paddingElements(), target.paddingElements(), worker);
				return true;
			}
		}
	}

	return false;
}

bool FrameConverterColorMap::Comfort::convertInteger1ChannelToRGB24(const Frame& source, Frame& target, const ColorMap colorMap, Worker* worker)
{
	if (!source.isValid() || colorMap == CM_INVALID)
	{
		return false;
	}

	Frame convertedSourceFrame;

	if (source.isPixelFormatCompatible(Frame::FORMAT_Y8))
	{
		if (!convertedSourceFrame.set(FrameType(source, FrameType::FORMAT_F32), true /*forceOwner*/, true /*forceWritable*/))
		{
			return false;
		}

		FrameConverter::cast<uint8_t, float>(source.constdata<uint8_t>(), convertedSourceFrame.data<float>(), source.width(), source.height(), source.channels(), source.paddingElements(), convertedSourceFrame.paddingElements());
	}
	else if (source.isPixelFormatCompatible(Frame::genericPixelFormat<int8_t, 1u>()))
	{
		if (!convertedSourceFrame.set(FrameType(source, FrameType::FORMAT_F32), true /*forceOwner*/, true /*forceWritable*/))
		{
			return false;
		}

		FrameConverter::cast<int8_t, float>(source.constdata<int8_t>(), convertedSourceFrame.data<float>(), source.width(), source.height(), source.channels(), source.paddingElements(), convertedSourceFrame.paddingElements());
	}
	else if (source.isPixelFormatCompatible(Frame::FORMAT_Y16))
	{
		if (!convertedSourceFrame.set(FrameType(source, FrameType::FORMAT_F32), true /*forceOwner*/, true /*forceWritable*/))
		{
			return false;
		}

		FrameConverter::cast<uint16_t, float>(source.constdata<uint16_t>(), convertedSourceFrame.data<float>(), source.width(), source.height(), source.channels(), source.paddingElements(), convertedSourceFrame.paddingElements());
	}
	else if (source.isPixelFormatCompatible(Frame::genericPixelFormat<int16_t, 1u>()))
	{
		if (!convertedSourceFrame.set(FrameType(source, FrameType::FORMAT_F32), true /*forceOwner*/, true /*forceWritable*/))
		{
			return false;
		}

		FrameConverter::cast<int16_t, float>(source.constdata<int16_t>(), convertedSourceFrame.data<float>(), source.width(), source.height(), source.channels(), source.paddingElements(), convertedSourceFrame.paddingElements());
	}
	else if (source.isPixelFormatCompatible(Frame::FORMAT_Y32))
	{
		if (!convertedSourceFrame.set(FrameType(source, FrameType::FORMAT_F32), true /*forceOwner*/, true /*forceWritable*/))
		{
			return false;
		}

		FrameConverter::cast<uint32_t, float>(source.constdata<uint32_t>(), convertedSourceFrame.data<float>(), source.width(), source.height(), source.channels(), source.paddingElements(), convertedSourceFrame.paddingElements());
	}
	else if (source.isPixelFormatCompatible(Frame::genericPixelFormat<int32_t, 1u>()))
	{
		if (!convertedSourceFrame.set(FrameType(source, FrameType::FORMAT_F32), true /*forceOwner*/, true /*forceWritable*/))
		{
			return false;
		}

		FrameConverter::cast<int32_t, float>(source.constdata<int32_t>(), convertedSourceFrame.data<float>(), source.width(), source.height(), source.channels(), source.paddingElements(), convertedSourceFrame.paddingElements());
	}
	else if (source.isPixelFormatCompatible(Frame::FORMAT_Y64))
	{
		if (!convertedSourceFrame.set(FrameType(source, FrameType::FORMAT_F32), true /*forceOwner*/, true /*forceWritable*/))
		{
			return false;
		}

		FrameConverter::cast<uint64_t, float>(source.constdata<uint64_t>(), convertedSourceFrame.data<float>(), source.width(), source.height(), source.channels(), source.paddingElements(), convertedSourceFrame.paddingElements());
	}
	else if (source.isPixelFormatCompatible(Frame::genericPixelFormat<int64_t, 1u>()))
	{
		if (!convertedSourceFrame.set(FrameType(source, FrameType::FORMAT_F32), true /*forceOwner*/, true /*forceWritable*/))
		{
			return false;
		}

		FrameConverter::cast<int64_t, float>(source.constdata<int64_t>(), convertedSourceFrame.data<float>(), source.width(), source.height(), source.channels(), source.paddingElements(), convertedSourceFrame.paddingElements());
	}

	if (convertedSourceFrame.isValid())
	{
		return convertFloat1ChannelToRGB24(convertedSourceFrame, target, colorMap, worker);
	}

	return false;
}

void FrameConverterColorMap::convertTurboFloat1ChannelToRGB24(const float* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const float minSource, const float maxSource, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(minSource < maxSource);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	// https://ai.googleblog.com/2019/08/turbo-improved-rainbow-colormap-for.html
	static const uint8_t turboSRGB[256][3] = {{48,18,59}, {50,21,67}, {51,24,74}, {52,27,81}, {53,30,88}, {54,33,95}, {55,36,102}, {56,39,109}, {57,42,115}, {58,45,121}, {59,47,128}, {60,50,134}, {61,53,139}, {62,56,145}, {63,59,151}, {63,62,156}, {64,64,162}, {65,67,167}, {65,70,172}, {66,73,177}, {66,75,181}, {67,78,186}, {68,81,191}, {68,84,195}, {68,86,199}, {69,89,203}, {69,92,207}, {69,94,211}, {70,97,214}, {70,100,218}, {70,102,221}, {70,105,224}, {70,107,227}, {71,110,230}, {71,113,233}, {71,115,235}, {71,118,238}, {71,120,240}, {71,123,242}, {70,125,244}, {70,128,246}, {70,130,248}, {70,133,250}, {70,135,251}, {69,138,252}, {69,140,253}, {68,143,254}, {67,145,254}, {66,148,255}, {65,150,255}, {64,153,255}, {62,155,254}, {61,158,254}, {59,160,253}, {58,163,252}, {56,165,251}, {55,168,250}, {53,171,248}, {51,173,247}, {49,175,245}, {47,178,244}, {46,180,242}, {44,183,240}, {42,185,238}, {40,188,235}, {39,190,233}, {37,192,231}, {35,195,228}, {34,197,226}, {32,199,223}, {31,201,221}, {30,203,218}, {28,205,216}, {27,208,213}, {26,210,210}, {26,212,208}, {25,213,205}, {24,215,202}, {24,217,200}, {24,219,197}, {24,221,194}, {24,222,192}, {24,224,189}, {25,226,187}, {25,227,185}, {26,228,182}, {28,230,180}, {29,231,178}, {31,233,175}, {32,234,172}, {34,235,170}, {37,236,167}, {39,238,164}, {42,239,161}, {44,240,158}, {47,241,155}, {50,242,152}, {53,243,148}, {56,244,145}, {60,245,142}, {63,246,138}, {67,247,135}, {70,248,132}, {74,248,128}, {78,249,125}, {82,250,122}, {85,250,118}, {89,251,115}, {93,252,111}, {97,252,108}, {101,253,105}, {105,253,102}, {109,254,98}, {113,254,95}, {117,254,92}, {121,254,89}, {125,255,86}, {128,255,83}, {132,255,81}, {136,255,78}, {139,255,75}, {143,255,73}, {146,255,71}, {150,254,68}, {153,254,66}, {156,254,64}, {159,253,63}, {161,253,61}, {164,252,60}, {167,252,58}, {169,251,57}, {172,251,56}, {175,250,55}, {177,249,54}, {180,248,54}, {183,247,53}, {185,246,53}, {188,245,52}, {190,244,52}, {193,243,52}, {195,241,52}, {198,240,52}, {200,239,52}, {203,237,52}, {205,236,52}, {208,234,52}, {210,233,53}, {212,231,53}, {215,229,53}, {217,228,54}, {219,226,54}, {221,224,55}, {223,223,55}, {225,221,55}, {227,219,56}, {229,217,56}, {231,215,57}, {233,213,57}, {235,211,57}, {236,209,58}, {238,207,58}, {239,205,58}, {241,203,58}, {242,201,58}, {244,199,58}, {245,197,58}, {246,195,58}, {247,193,58}, {248,190,57}, {249,188,57}, {250,186,57}, {251,184,56}, {251,182,55}, {252,179,54}, {252,177,54}, {253,174,53}, {253,172,52}, {254,169,51}, {254,167,50}, {254,164,49}, {254,161,48}, {254,158,47}, {254,155,45}, {254,153,44}, {254,150,43}, {254,147,42}, {254,144,41}, {253,141,39}, {253,138,38}, {252,135,37}, {252,132,35}, {251,129,34}, {251,126,33}, {250,123,31}, {249,120,30}, {249,117,29}, {248,114,28}, {247,111,26}, {246,108,25}, {245,105,24}, {244,102,23}, {243,99,21}, {242,96,20}, {241,93,19}, {240,91,18}, {239,88,17}, {237,85,16}, {236,83,15}, {235,80,14}, {234,78,13}, {232,75,12}, {231,73,12}, {229,71,11}, {228,69,10}, {226,67,10}, {225,65,9}, {223,63,8}, {221,61,8}, {220,59,7}, {218,57,7}, {216,55,6}, {214,53,6}, {212,51,5}, {210,49,5}, {208,47,5}, {206,45,4}, {204,43,4}, {202,42,4}, {200,40,3}, {197,38,3}, {195,37,3}, {193,35,2}, {190,33,2}, {188,32,2}, {185,30,2}, {183,29,2}, {180,27,1}, {178,26,1}, {175,24,1}, {172,23,1}, {169,22,1}, {167,20,1}, {164,19,1}, {161,18,1}, {158,16,1}, {155,15,1}, {152,14,1}, {149,13,1}, {146,11,1}, {142,10,1}, {139,9,2}, {136,8,2}, {133,7,2}, {129,6,2}, {126,5,2}, {122,4,3}};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	const LookupData lookupData(minSource, maxSource, (const uint8_t* const)(turboSRGB));

	FrameConverter::convertGenericPixelFormat<float, uint8_t>(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameConverterColorMap::convertRowLookupFloat1ChannelToRGB24, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, &lookupData, worker);
}

void FrameConverterColorMap::convertInfernoFloat1ChannelToRGB24(const float* const source, uint8_t* const target, const unsigned int width, const unsigned int height, const ConversionFlag flag, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const float minSource, const float maxSource, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(minSource < maxSource);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 3u + targetPaddingElements;

	// https://www.kennethmoreland.com/color-advice/#inferno
	static const uint8_t infernoRGB[256][3] = {{1,0,4}, {1,0,5}, {1,0,6}, {2,1,8}, {2,1,10}, {3,1,12}, {4,2,14}, {4,2,16}, {5,3,18}, {6,3,21}, {7,3,23}, {8,4,25}, {10,4,27}, {11,5,29}, {12,5,32}, {14,6,34}, {15,6,36}, {17,7,38}, {18,7,41}, {19,8,43}, {21,8,45}, {22,8,48}, {24,9,50}, {25,9,53}, {27,9,55}, {28,9,58}, {30,9,60}, {32,9,62}, {33,9,65}, {35,9,67}, {37,9,70}, {39,9,72}, {40,9,74}, {42,8,77}, {44,8,79}, {46,7,81}, {47,7,83}, {49,6,85}, {51,6,87}, {53,5,89}, {55,5,91}, {56,4,93}, {58,4,94}, {60,3,96}, {61,3,97}, {63,3,98}, {65,2,99}, {66,2,100}, {68,2,102}, {70,2,102}, {71,2,103}, {73,2,104}, {74,2,105}, {76,3,106}, {77,3,106}, {79,3,107}, {80,3,107}, {82,4,108}, {83,4,108}, {85,5,109}, {86,6,109}, {88,6,109}, {89,7,110}, {91,7,110}, {92,8,110}, {94,9,110}, {95,10,110}, {97,11,111}, {98,11,111}, {99,12,111}, {101,13,111}, {102,14,111}, {104,15,111}, {105,15,111}, {107,16,111}, {108,17,111}, {110,18,111}, {111,18,111}, {112,19,111}, {114,20,111}, {115,20,111}, {117,21,110}, {118,22,110}, {120,23,110}, {121,23,110}, {123,24,110}, {124,25,110}, {126,25,109}, {127,26,109}, {129,27,109}, {130,27,108}, {131,28,108}, {133,29,108}, {134,29,108}, {136,30,107}, {137,30,107}, {139,31,106}, {140,32,106}, {142,32,106}, {143,33,105}, {145,34,105}, {146,34,104}, {148,35,104}, {149,36,103}, {151,36,103}, {152,37,102}, {154,38,102}, {155,38,101}, {157,39,100}, {158,40,100}, {160,40,99}, {161,41,98}, {162,42,98}, {164,42,97}, {165,43,96}, {167,44,96}, {168,44,95}, {170,45,94}, {171,46,93}, {173,47,93}, {174,47,92}, {176,48,91}, {177,49,90}, {178,50,89}, {180,51,88}, {181,51,88}, {183,52,87}, {184,53,86}, {185,54,85}, {187,55,84}, {188,56,83}, {189,57,82}, {191,57,81}, {192,58,80}, {194,59,79}, {195,60,78}, {196,61,77}, {197,62,76}, {199,63,75}, {200,64,74}, {201,65,73}, {203,66,72}, {204,67,71}, {205,69,70}, {206,70,69}, {207,71,67}, {209,72,66}, {210,73,65}, {211,74,64}, {212,76,63}, {213,77,62}, {214,78,61}, {215,79,59}, {217,81,58}, {218,82,57}, {219,83,56}, {220,85,55}, {221,86,54}, {222,87,52}, {223,89,51}, {224,90,50}, {224,92,49}, {225,93,48}, {226,95,46}, {227,96,45}, {228,98,44}, {229,99,43}, {230,101,41}, {230,102,40}, {231,104,39}, {232,105,37}, {233,107,36}, {233,108,35}, {234,110,34}, {235,112,32}, {235,113,31}, {236,115,29}, {237,117,28}, {237,118,27}, {238,120,25}, {238,122,24}, {239,124,22}, {239,125,21}, {240,127,19}, {240,129,18}, {241,130,16}, {241,132,15}, {241,134,13}, {242,136,12}, {242,138,10}, {242,139,8}, {243,141,7}, {243,143,6}, {243,145,4}, {244,147,3}, {244,149,3}, {244,150,2}, {244,152,1}, {244,154,1}, {244,156,1}, {244,158,1}, {245,160,1}, {245,162,2}, {245,164,2}, {245,166,3}, {245,168,5}, {245,169,6}, {244,171,8}, {244,173,10}, {244,175,12}, {244,177,14}, {244,179,17}, {244,181,19}, {244,183,21}, {243,185,24}, {243,187,26}, {243,189,28}, {243,191,31}, {242,193,33}, {242,195,36}, {242,197,38}, {241,199,41}, {241,201,44}, {240,203,46}, {240,205,49}, {240,207,52}, {239,209,55}, {239,211,58}, {238,213,61}, {238,215,64}, {237,217,67}, {237,219,70}, {236,220,73}, {235,222,77}, {235,224,80}, {234,226,84}, {234,228,87}, {234,230,91}, {233,232,95}, {233,234,99}, {233,235,103}, {232,237,107}, {232,239,111}, {232,240,115}, {233,242,119}, {233,243,124}, {234,245,128}, {234,246,132}, {235,248,136}, {236,249,140}, {237,250,144}, {238,251,148}, {240,252,152}, {241,254,156}, {243,255,160}, {245,255,163}};

	const bool areContinuous = sourcePaddingElements == 0u && targetPaddingElements == 0u;

	const LookupData lookupData(minSource, maxSource, (const uint8_t* const)(infernoRGB));

	FrameConverter::convertGenericPixelFormat<float, uint8_t>(source, target, width, height, sourceStrideElements, targetStrideElements, flag, CV::FrameConverterColorMap::convertRowLookupFloat1ChannelToRGB24, CV::FrameChannels::reverseRowPixelOrderInPlace<uint8_t, 3u>, areContinuous, &lookupData, worker);
}

FrameConverterColorMap::ColorMap FrameConverterColorMap::translateColorMap(const std::string& colorMap)
{
	if (colorMap == "invalid")
	{
		return CM_INVALID;
	}
	else if (colorMap == "jet")
	{
		return CM_JET;
	}
	else if (colorMap == "turbo")
	{
		return CM_TURBO;
	}
	else if (colorMap == "inferno")
	{
		return CM_INFERNO;
	}
	else if (colorMap == "spiral10")
	{
		return CM_SPIRAL_10;
	}
	else if (colorMap == "spiral20")
	{
		return CM_SPIRAL_20;
	}
	else if (colorMap == "linear")
	{
		return CM_LINEAR;
	}

	ocean_assert(false && "Unknown color map name");
	return CM_INVALID;
}

std::string FrameConverterColorMap::translateColorMap(const ColorMap colorMap)
{
	switch (colorMap)
	{
		case CM_INVALID:
			return std::string("invalid");

		case CM_JET:
			return std::string("jet");

		case CM_TURBO:
			return std::string("turbo");

		case CM_INFERNO:
			return std::string("inferno");

		case CM_SPIRAL_10:
			return std::string("spiral10");

		case CM_SPIRAL_20:
			return std::string("spiral20");

		case CM_LINEAR:
			return std::string("linear");
	}

	ocean_assert(false && "Invalid color map!");
	return std::string();
}

const FrameConverterColorMap::ColorMaps& FrameConverterColorMap::definedColorMaps()
{
	static const ColorMaps colorMaps =
	{
		CM_JET,
		CM_TURBO,
		CM_INFERNO,
		CM_SPIRAL_10,
		CM_SPIRAL_20,
		CM_LINEAR
	};

	return colorMaps;
}

void FrameConverterColorMap::convertRowJetFloat1ChannelToRGB24(const float* source, uint8_t* target, const size_t width, const void* options)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(options != nullptr);

	ocean_assert(options != nullptr);
	const float* const minMaxRange = reinterpret_cast<const float*>(options);
	ocean_assert(minMaxRange != nullptr);

	const float minValue = minMaxRange[0];
	const float maxValue = minMaxRange[1];
	ocean_assert(minValue < maxValue);

	const float normalization = 1.0f / (maxValue - minValue);

	constexpr float factor_115 = 1.0f / 1.15f;

	for (unsigned int x = 0u; x < width; ++x)
	{
		if (NumericF::isInf(source[x]) || NumericF::isNan(source[x]))
		{
			target[3u * x + 0u] = uint8_t(0u);
			target[3u * x + 1u] = uint8_t(0u);
			target[3u * x + 2u] = uint8_t(0u);
		}
		else
		{
			float normalizedSourceValue = (source[x] - minValue) * normalization;

			if (normalizedSourceValue < 0.0f)
			{
				normalizedSourceValue = -0.05f;
			}
			else if (normalizedSourceValue > 1.0f)
			{
				normalizedSourceValue = 1.05f;
			}

			const float adjustedSourceValue = normalizedSourceValue * factor_115 + 0.1f; // use slightly asymmetric range to avoid darkest shades of blue

			target[3u * x + 0u] = uint8_t(minmax<int>(0, NumericF::round32((1.5f - 4.0f * NumericF::abs(adjustedSourceValue - 0.75f)) * 255.0f), 255));
			target[3u * x + 1u] = uint8_t(minmax<int>(0, NumericF::round32((1.5f - 4.0f * NumericF::abs(adjustedSourceValue - 0.50f)) * 255.0f), 255));
			target[3u * x + 2u] = uint8_t(minmax<int>(0, NumericF::round32((1.5f - 4.0f * NumericF::abs(adjustedSourceValue - 0.25f)) * 255.0f), 255));
		}
	}
}

void FrameConverterColorMap::convertRowSpiralFloat1ChannelToRGB24(const float* source, uint8_t* target, const size_t width, const void* options)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(options != nullptr);

	ocean_assert(options != nullptr);
	const float* const floatOptions = reinterpret_cast<const float*>(options);
	ocean_assert(floatOptions != nullptr);

	const float minValue = floatOptions[0];
	const float maxValue = floatOptions[1];
	ocean_assert(minValue < maxValue);

	const float period = floatOptions[2];
	ocean_assert(period > 0.0f);

	const float normalization = 1.0f / (maxValue - minValue);
	const float rounds = (maxValue - minValue) / period;

	for (unsigned int x = 0u; x < width; ++x)
	{
		if (NumericF::isInf(source[x]) || NumericF::isNan(source[x]))
		{
			target[3u * x + 0u] = uint8_t(0u);
			target[3u * x + 1u] = uint8_t(0u);
			target[3u * x + 2u] = uint8_t(0u);
		}
		else
		{
			const float normalizedSourceValue = minmax<float>(0.0f, (source[x] - minValue) * normalization, 1.0f); // [0, 1]
			const float normalizedRounds = normalizedSourceValue * rounds; // [0, rounds]

			const float inPeriod = normalizedRounds - float(int(normalizedRounds));
			ocean_assert(inPeriod >= 0.0f && inPeriod <= 1.0f);

			const RGBAColor rgbaColor(HSVAColor(inPeriod * NumericF::pi2(), 1.0f, 1.0f));

			const uint8_t red = uint8_t(rgbaColor.red() * 255.0f);
			const uint8_t green = uint8_t(rgbaColor.green() * 255.0f);
			const uint8_t blue = uint8_t(rgbaColor.blue() * 255.0f);

			const float brightnessMapping = normalizedSourceValue * 1.6f - 0.8f; // [0, 1] -> [-0.8, 0.8]

			if (brightnessMapping >= 0.0f)
			{
				// above 50%, we interpolate to white

				const unsigned int whiteFactor = std::min((unsigned int)(brightnessMapping * 1024.0f + 0.5f), 1024u);

				const unsigned int colorFactor = 1024 - whiteFactor;
				const unsigned int whiteValue = whiteFactor * 255u;

				target[3u * x + 0u] = uint8_t((red * colorFactor + whiteValue) / 1024u);
				target[3u * x + 1u] = uint8_t((green * colorFactor + whiteValue) / 1024u);
				target[3u * x + 2u] = uint8_t((blue * colorFactor + whiteValue) / 1024u);
			}
			else
			{
				// below 50%, we interpolate to black

				const unsigned int blackFactor = std::min((unsigned int)(-brightnessMapping * 1024.0f + 0.5f), 1024u);

				const unsigned int colorFactor = 1024 - blackFactor;

				target[3u * x + 0u] = uint8_t((red * colorFactor) / 1024u);
				target[3u * x + 1u] = uint8_t((green * colorFactor) / 1024u);
				target[3u * x + 2u] = uint8_t((blue * colorFactor) / 1024u);
			}
		}
	}
}

void FrameConverterColorMap::convertRowLookupFloat1ChannelToRGB24(const float* source, uint8_t* target, const size_t width, const void* options)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(options != nullptr);

	ocean_assert(options != nullptr);
	const LookupData* const lookupData = reinterpret_cast<const LookupData*>(options);
	ocean_assert(lookupData != nullptr);

	const float minValue = lookupData->minValue();
	const float maxValue = lookupData->maxValue();
	ocean_assert(minValue < maxValue);

	const float normalization = 255.0f / (maxValue - minValue);

	const uint8_t* const lookupTable = lookupData->lookupTable();

	for (unsigned int x = 0u; x < width; ++x)
	{
		if (NumericF::isInf(source[x]) || NumericF::isNan(source[x]))
		{
			target[3u * x + 0u] = uint8_t(0u);
			target[3u * x + 1u] = uint8_t(0u);
			target[3u * x + 2u] = uint8_t(0u);
		}
		else
		{
			const float floatLookupBin = (source[x] - minValue) * normalization;

			if (floatLookupBin >= 0.0f && floatLookupBin < 256.0f)
			{
				const float adjustedFloatLookupBin = max(0.0f, floatLookupBin - 0.5f);

				const unsigned int leftBin = (unsigned int)(adjustedFloatLookupBin);
				const unsigned int rightBin = min(leftBin + 1u, 255u);

				const unsigned int factorRight = (unsigned int)((adjustedFloatLookupBin - float(leftBin)) * 1024.0f + 0.5f);
				ocean_assert(factorRight <= 1024u);

				const unsigned int factorLeft = 1024u - factorRight;

				const uint8_t* const leftLookupValue = lookupTable + leftBin * 3u;
				const uint8_t* const rightLookupValue = lookupTable + rightBin * 3u;

				target[3u * x + 0u] = uint8_t((leftLookupValue[0] * factorLeft + rightLookupValue[0] * factorRight) / 1024u);
				target[3u * x + 1u] = uint8_t((leftLookupValue[1] * factorLeft + rightLookupValue[1] * factorRight) / 1024u);
				target[3u * x + 2u] = uint8_t((leftLookupValue[2] * factorLeft + rightLookupValue[2] * factorRight) / 1024u);
			}
			else
			{
				target[3u * x + 0u] = uint8_t(0u);
				target[3u * x + 1u] = uint8_t(0u);
				target[3u * x + 2u] = uint8_t(0u);
			}
		}
	}
}

}

}
