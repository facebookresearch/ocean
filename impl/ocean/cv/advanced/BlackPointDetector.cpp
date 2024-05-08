/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/BlackPointDetector.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

BlackPointDetector::Color BlackPointDetector::detectBlackPointHistogram(const Frame& frame, Worker* worker)
{
	ocean_assert(frame && frame.numberPlanes() == 1u);

	if (frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frame.numberPlanes() == 1u)
	{
		switch (frame.channels())
		{
			case 1u:
				return detectBlackPointHistogram8BitPerChannel<1u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), worker);

			case 2u:
				return detectBlackPointHistogram8BitPerChannel<2u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), worker);

			case 3u:
				return detectBlackPointHistogram8BitPerChannel<3u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), worker);

			case 4u:
				return detectBlackPointHistogram8BitPerChannel<4u>(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), worker);
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return Color();
}

}

}

}
