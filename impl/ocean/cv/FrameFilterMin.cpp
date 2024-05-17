/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterMin.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace CV
{

bool FrameFilterMin::Comfort::filter(const Frame& source, Frame& target, const unsigned int filterSize, Worker* worker)
{
	ocean_assert(source.isValid());
	ocean_assert(!target.isValid() || source.isFrameTypeCompatible(target, false));

	if (!source.isValid() || (target.isValid() && !source.isFrameTypeCompatible(target, false)))
	{
		return false;
	}

	if (!target.isValid())
	{
		if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}
	}

	for (unsigned int planeIndex = 0u; planeIndex < source.numberPlanes(); ++planeIndex)
	{
		if (source.planeWidth(planeIndex) != source.width() || source.planeHeight(planeIndex) != source.height())
		{
			ocean_assert(false && "All planes must have the same resolution");
			return false;
		}

		switch (target.dataType())
		{
			case FrameType::DT_UNSIGNED_INTEGER_8:
			{
				switch (target.planeChannels(planeIndex))
				{
					case 1u:
						FrameFilterMin::filter<uint8_t, 1u>(source.constdata<uint8_t>(planeIndex), target.data<uint8_t>(planeIndex), source.width(), source.height(), source.paddingElements(planeIndex), target.paddingElements(planeIndex), filterSize, worker);
						break;

					case 2u:
						FrameFilterMin::filter<uint8_t, 2u>(source.constdata<uint8_t>(planeIndex), target.data<uint8_t>(planeIndex), source.width(), source.height(), source.paddingElements(planeIndex), target.paddingElements(planeIndex), filterSize, worker);
						break;

					case 3u:
						FrameFilterMin::filter<uint8_t, 3u>(source.constdata<uint8_t>(planeIndex), target.data<uint8_t>(planeIndex), source.width(), source.height(), source.paddingElements(planeIndex), target.paddingElements(planeIndex), filterSize, worker);
						break;

					case 4u:
						FrameFilterMin::filter<uint8_t, 4u>(source.constdata<uint8_t>(planeIndex), target.data<uint8_t>(planeIndex), source.width(), source.height(), source.paddingElements(planeIndex), target.paddingElements(planeIndex), filterSize, worker);
						break;

					default:
						ocean_assert(false && "Invalid channel number!");
						return false;
				}

				break;
			}

			case FrameType::DT_SIGNED_FLOAT_32:
			{
				switch (target.planeChannels(planeIndex))
				{
					case 1u:
						FrameFilterMin::filter<float, 1u>(source.constdata<float>(planeIndex), target.data<float>(planeIndex), source.width(), source.height(), source.paddingElements(planeIndex), target.paddingElements(planeIndex), filterSize, worker);
						break;

					case 2u:
						FrameFilterMin::filter<float, 2u>(source.constdata<float>(planeIndex), target.data<float>(planeIndex), source.width(), source.height(), source.paddingElements(planeIndex), target.paddingElements(planeIndex), filterSize, worker);
						break;

					case 3u:
						FrameFilterMin::filter<float, 3u>(source.constdata<float>(planeIndex), target.data<float>(planeIndex), source.width(), source.height(), source.paddingElements(planeIndex), target.paddingElements(planeIndex), filterSize, worker);
						break;

					case 4u:
						FrameFilterMin::filter<float, 4u>(source.constdata<float>(planeIndex), target.data<float>(planeIndex), source.width(), source.height(), source.paddingElements(planeIndex), target.paddingElements(planeIndex), filterSize, worker);
						break;

					default:
						ocean_assert(false && "Invalid channel number!");
						return false;
				}

				break;
			}

			default:
				ocean_assert(false && "Data type not supported!");
				return false;
		}
	}

	return true;
}

bool FrameFilterMin::Comfort::filter(Frame& frame, const unsigned int filterSize, Worker* worker)
{
	Frame tmpFrame;
	if (!filter(frame, tmpFrame, filterSize, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);

	return true;
}

}

}
