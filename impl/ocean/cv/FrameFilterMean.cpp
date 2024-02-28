// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/FrameFilterMean.h"
#include "ocean/cv/IntegralImage.h"

namespace Ocean
{

namespace CV
{

bool FrameFilterMean::filter(const Frame& source, Frame& target, const unsigned int window, Worker* worker)
{
	ocean_assert(source.isValid());
	ocean_assert(window % 2u == 1u);

	if (!source.isValid() || window % 2u != 1u)
	{
		return false;
	}

	if (source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/);

		switch (source.channels())
		{
			case 1u:
				filter8BitPerChannel<1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), window, source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 2u:
				filter8BitPerChannel<2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), window, source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 3u:
				filter8BitPerChannel<3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), window, source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 4u:
				filter8BitPerChannel<4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), window, source.paddingElements(), target.paddingElements(), worker);
				return true;
		}
	}

	ocean_assert(false && "Invalid pixel format");
	return false;
}

bool FrameFilterMean::filter(Frame& frame, const unsigned int window, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(window % 2u == 1u);

	if (!frame.isValid() || window % 2u != 1u)
	{
		return false;
	}

	if (frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		switch (frame.channels())
		{
			case 1u:
				filter8BitPerChannel<1u>(frame.data<uint8_t>(), frame.width(), frame.height(), window, frame.paddingElements(), worker);
				return true;

			case 2u:
				filter8BitPerChannel<2u>(frame.data<uint8_t>(), frame.width(), frame.height(), window, frame.paddingElements(), worker);
				return true;

			case 3u:
				filter8BitPerChannel<3u>(frame.data<uint8_t>(), frame.width(), frame.height(), window, frame.paddingElements(), worker);
				return true;

			case 4u:
				filter8BitPerChannel<4u>(frame.data<uint8_t>(), frame.width(), frame.height(), window, frame.paddingElements(), worker);
				return true;
		}
	}

	ocean_assert(false && "Invalid pixel format");
	return false;
}

}

}
