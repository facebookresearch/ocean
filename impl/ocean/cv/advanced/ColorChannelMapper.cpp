/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/ColorChannelMapper.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

bool ColorChannelMapper::createLookup8BitsPerChannel(FrameType::PixelFormat pixelFormat, Frame& lookupFrame, const ColorChannelMapFunction& function)
{
	ocean_assert(FrameType::dataType(pixelFormat) == FrameType::DT_UNSIGNED_INTEGER_8);
	ocean_assert(FrameType::numberPlanes(pixelFormat) == 1u);

	const unsigned int channels = FrameType::channels(pixelFormat);

	if (!lookupFrame.set(FrameType(256u, 1u, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), false, true))
	{
		return false;
	}

	uint8_t* data = lookupFrame.data<uint8_t>();

	for (unsigned int i = 0u; i < 256u; i++)
	{
		for (unsigned int c = 0u; c < channels; c++)
		{
			*data++ = function(c, uint8_t(i));
		}
	}

	return true;
}

void ColorChannelMapper::applyLookup8BitsPerChannel(Frame& frame, const Frame& lookupFrame, Worker* worker)
{
	ocean_assert(frame.isValid() && lookupFrame.isValid());
	ocean_assert(frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frame.numberPlanes() == 1u);
	ocean_assert(lookupFrame.height() == 1);

	switch (frame.channels())
	{
		case 1u:
		{
			if (worker)
			{
				worker->executeFunction(Worker::Function::createStatic(&applyLookup8BitsPerChannelSubset<1u>, frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), lookupFrame.constdata<uint8_t>(), 0u, 0u), 0u, frame.height());
			}
			else
			{
				applyLookup8BitsPerChannelSubset<1u>(frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), lookupFrame.constdata<uint8_t>(), 0u, frame.height());
			}

			break;
		}

		case 3u:
		{
			if (worker)
			{
				worker->executeFunction(Worker::Function::createStatic(&applyLookup8BitsPerChannelSubset<3u>, frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), lookupFrame.constdata<uint8_t>(), 0u, 0u), 0u, frame.height());
			}
			else
			{
				applyLookup8BitsPerChannelSubset<3u>(frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), lookupFrame.constdata<uint8_t>(), 0u, frame.height());
			}

			break;
		}

		case 4u:
		{
			if (worker)
			{
				worker->executeFunction(Worker::Function::createStatic(&applyLookup8BitsPerChannelSubset<4u>, frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), lookupFrame.constdata<uint8_t>(), 0u, 0u), 0u, frame.height());
			}
			else
			{
				applyLookup8BitsPerChannelSubset<4u>(frame.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), lookupFrame.constdata<uint8_t>(), 0u, frame.height());
			}

			break;
		}

		default:
			ocean_assert(false && "Invalid channel number!");
	}
}

void ColorChannelMapper::createLookupScaleOffset(const FrameType::PixelFormat& pixelFormat, Frame& lookupFrame, const VectorF3& scale, const VectorF3& offset, const VectorF3& gamma)
{
	ocean_assert(FrameType::dataType(pixelFormat) == FrameType::DT_UNSIGNED_INTEGER_8);

	const unsigned int channels = FrameType::channels(pixelFormat);
	ocean_assert(channels == 3u || channels == 4u); // 3 or 4 color channels required

	bool alphaIsLast = true;
	const bool hasAlpha = FrameType::formatHasAlphaChannel(pixelFormat, &alphaIsLast);
	const int alphaIndex = hasAlpha ? (alphaIsLast ? channels - 1 : 0) : -1;
	const int colorIndex = alphaIsLast ? 0u : 1u;

	const bool mirrorRGB = pixelFormat == FrameType::FORMAT_ABGR32 || pixelFormat == FrameType::FORMAT_BGR24 || pixelFormat == FrameType::FORMAT_BGR32 || pixelFormat == FrameType::FORMAT_BGRA32;

	float mapScale[4], mapOffset[4], mapGamma[4];

	for (unsigned int c = 0; c < 3u; c++)
	{
		const int index = mirrorRGB ? 2u - c : c + colorIndex;
		mapScale[index] = scale.data()[c];
		mapOffset[index] = offset.data()[c];
		mapGamma[index] = gamma.data()[c];
	}

	if (hasAlpha)
	{
		mapScale[alphaIndex] = 1.0f;
		mapOffset[alphaIndex] = 0.0f;
		mapGamma[alphaIndex] = 1.0f;
	}

	if (channels == 3u)
	{
		ScaleOffsetMapping<3u> mapping(mapScale, mapOffset, mapGamma);
		createLookup8BitsPerChannel(pixelFormat, lookupFrame, ColorChannelMapFunction(mapping, &ScaleOffsetMapping<3u>::map));
	}
	else
	{
		ScaleOffsetMapping<4u> mapping(mapScale, mapOffset, mapGamma);
		createLookup8BitsPerChannel(pixelFormat, lookupFrame, ColorChannelMapFunction(mapping, &ScaleOffsetMapping<4u>::map));
	}
}

void ColorChannelMapper::mapScaleOffset(Frame& frame, const VectorF3& scale, const VectorF3& offset, const VectorF3& gamma, Worker* worker)
{
	Frame lookup;
	createLookupScaleOffset(frame.pixelFormat(), lookup, scale, offset, gamma);
	applyLookup8BitsPerChannel(frame, lookup, worker);
}

}

}

}
