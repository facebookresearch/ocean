/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterScharrMagnitude.h"

namespace Ocean
{

namespace CV
{

bool FrameFilterScharrMagnitude::Comfort::filterHorizontalVerticalAs1Channel(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.numberPlanes() == 1u);
	ocean_assert(source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(source.width() == target.width() && source.height() == target.height());
	ocean_assert(target.channels() == 2u);
	ocean_assert(source.pixelOrigin() == target.pixelOrigin());
	ocean_assert(target.dataType() == FrameType::DT_SIGNED_INTEGER_8 || target.dataType() == FrameType::DT_SIGNED_INTEGER_16);

	if (source.isValid() && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (source.width() == target.width() && source.height() == target.height() && source.pixelOrigin() == target.pixelOrigin() && target.channels() == 2u)
		{
			if (target.dataType() == FrameType::DT_SIGNED_INTEGER_8)
			{
				switch (source.channels())
				{
					case 1u:
						FrameFilterScharrMagnitude::filterHorizontalVerticalAs1Channel8Bit<int8_t, 1u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterScharrMagnitude::filterHorizontalVerticalAs1Channel8Bit<int8_t, 2u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterScharrMagnitude::filterHorizontalVerticalAs1Channel8Bit<int8_t, 3u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterScharrMagnitude::filterHorizontalVerticalAs1Channel8Bit<int8_t, 4u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
			else if (target.dataType() == FrameType::DT_SIGNED_INTEGER_16)
			{
				switch (source.channels())
				{
					case 1u:
						FrameFilterScharrMagnitude::filterHorizontalVerticalAs1Channel8Bit<int16_t, 1u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterScharrMagnitude::filterHorizontalVerticalAs1Channel8Bit<int16_t, 2u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterScharrMagnitude::filterHorizontalVerticalAs1Channel8Bit<int16_t, 3u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterScharrMagnitude::filterHorizontalVerticalAs1Channel8Bit<int16_t, 4u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameFilterScharrMagnitude::Comfort::filterDiagonalAs1Channel(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.numberPlanes() == 1u);
	ocean_assert(source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(source.width() == target.width() && source.height() == target.height());
	ocean_assert(target.channels() == 2u);
	ocean_assert(source.pixelOrigin() == target.pixelOrigin());
	ocean_assert(target.dataType() == FrameType::DT_SIGNED_INTEGER_8 || target.dataType() == FrameType::DT_SIGNED_INTEGER_16);

	if (source.isValid() && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (source.width() == target.width() && source.height() == target.height() && source.pixelOrigin() == target.pixelOrigin() && target.channels() == 2u)
		{
			if (target.dataType() == FrameType::DT_SIGNED_INTEGER_8)
			{
				switch (source.channels())
				{
					case 1u:
						FrameFilterScharrMagnitude::filterDiagonalAs1Channel8Bit<int8_t, 1u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterScharrMagnitude::filterDiagonalAs1Channel8Bit<int8_t, 2u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterScharrMagnitude::filterDiagonalAs1Channel8Bit<int8_t, 3u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterScharrMagnitude::filterDiagonalAs1Channel8Bit<int8_t, 4u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
			else if (target.dataType() == FrameType::DT_SIGNED_INTEGER_16)
			{
				switch (source.channels())
				{
					case 1u:
						FrameFilterScharrMagnitude::filterDiagonalAs1Channel8Bit<int16_t, 1u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterScharrMagnitude::filterDiagonalAs1Channel8Bit<int16_t, 2u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterScharrMagnitude::filterDiagonalAs1Channel8Bit<int16_t, 3u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterScharrMagnitude::filterDiagonalAs1Channel8Bit<int16_t, 4u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameFilterScharrMagnitude::Comfort::filterAs1Channel(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.numberPlanes() == 1u);
	ocean_assert(source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(source.width() == target.width() && source.height() == target.height());
	ocean_assert(target.channels() == 4u);
	ocean_assert(source.pixelOrigin() == target.pixelOrigin());
	ocean_assert(target.dataType() == FrameType::DT_SIGNED_INTEGER_8 || target.dataType() == FrameType::DT_SIGNED_INTEGER_16);

	if (source.isValid() && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (source.width() == target.width() && source.height() == target.height() && source.pixelOrigin() == target.pixelOrigin() && target.channels() == 4u)
		{
			if (target.dataType() == FrameType::DT_SIGNED_INTEGER_8)
			{
				switch (source.channels())
				{
					case 1u:
						FrameFilterScharrMagnitude::filterAs1Channel8Bit<int8_t, 1u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterScharrMagnitude::filterAs1Channel8Bit<int8_t, 2u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterScharrMagnitude::filterAs1Channel8Bit<int8_t, 3u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterScharrMagnitude::filterAs1Channel8Bit<int8_t, 4u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
			else if (target.dataType() == FrameType::DT_SIGNED_INTEGER_16)
			{
				switch (source.channels())
				{
					case 1u:
						FrameFilterScharrMagnitude::filterAs1Channel8Bit<int16_t, 1u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterScharrMagnitude::filterAs1Channel8Bit<int16_t, 2u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterScharrMagnitude::filterAs1Channel8Bit<int16_t, 3u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterScharrMagnitude::filterAs1Channel8Bit<int16_t, 4u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

}

}
