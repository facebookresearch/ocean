/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameShrinkerAlpha.h"

namespace Ocean
{

namespace CV
{

template <bool tTransparentIs0xFF>
bool FrameShrinkerAlpha::Comfort::divideByTwo(const Frame& source, Frame& target, Worker* worker)
{
	bool alphaIsLastChannel = false;

	if (source.isValid() && source.width() > 1u && source.height() > 1u && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && FrameType::formatHasAlphaChannel(source.pixelFormat(), &alphaIsLastChannel))
	{
		const unsigned int width_2 = source.width() / 2u;
		const unsigned int height_2 = source.height() / 2u;

		if (!target.set(FrameType(source, width_2, height_2), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		switch (source.channels())
		{
			case 1u:
			{
				if (alphaIsLastChannel)
					divideByTwo8BitPerChannel<1u, false, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				else
					divideByTwo8BitPerChannel<1u, true, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case 2u:
			{
				if (alphaIsLastChannel)
					divideByTwo8BitPerChannel<2u, false, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				else
					divideByTwo8BitPerChannel<2u, true, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case 3u:
			{
				if (alphaIsLastChannel)
					divideByTwo8BitPerChannel<3u, false, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				else
					divideByTwo8BitPerChannel<3u, true, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}

			case 4u:
			{
				if (alphaIsLastChannel)
					divideByTwo8BitPerChannel<4u, false, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
				else
					divideByTwo8BitPerChannel<4u, true, tTransparentIs0xFF>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);

				return true;
			}
		}

		ocean_assert(false && "FrameShrinkerAlpha: Not supported frame type!");
		return false;
	}

	ocean_assert(false && "FrameShrinkerAlpha: Invalid frame!");
	return false;
}

// Explicit instantiations
template bool FrameShrinkerAlpha::Comfort::divideByTwo<true>(const Frame&, Frame&, Worker*);
template bool FrameShrinkerAlpha::Comfort::divideByTwo<false>(const Frame&, Frame&, Worker*);

template <bool tTransparentIs0xFF>
bool FrameShrinkerAlpha::Comfort::divideByTwo(Frame& frame, Worker* worker)
{
	Frame tmpFrame;
	if (!divideByTwo<tTransparentIs0xFF>(frame, tmpFrame, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);
	return true;
}

// Explicit instantiations
template bool FrameShrinkerAlpha::Comfort::divideByTwo<true>(Frame&, Worker*);
template bool FrameShrinkerAlpha::Comfort::divideByTwo<false>(Frame&, Worker*);

}

}
