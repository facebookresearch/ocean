/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterSobelMagnitude.h"
#include "ocean/cv/FrameFilterSobel.h"

namespace Ocean
{

namespace CV
{

bool FrameFilterSobelMagnitude::Comfort::filterHorizontalVerticalAs1Channel(const Frame& source, Frame& target, Worker* worker)
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
						FrameFilterSobelMagnitude::filterHorizontalVerticalAs1Channel8Bit<int8_t, 1u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalAs1Channel8Bit<int8_t, 2u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalAs1Channel8Bit<int8_t, 3u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalAs1Channel8Bit<int8_t, 4u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
			else if (target.dataType() == FrameType::DT_SIGNED_INTEGER_16)
			{
				switch (source.channels())
				{
					case 1u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalAs1Channel8Bit<int16_t, 1u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalAs1Channel8Bit<int16_t, 2u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalAs1Channel8Bit<int16_t, 3u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalAs1Channel8Bit<int16_t, 4u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameFilterSobelMagnitude::Comfort::filterDiagonalAs1Channel(const Frame& source, Frame& target, Worker* worker)
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
						FrameFilterSobelMagnitude::filterDiagonalAs1Channel8Bit<int8_t, 1u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterSobelMagnitude::filterDiagonalAs1Channel8Bit<int8_t, 2u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterSobelMagnitude::filterDiagonalAs1Channel8Bit<int8_t, 3u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterSobelMagnitude::filterDiagonalAs1Channel8Bit<int8_t, 4u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
			else if (target.dataType() == FrameType::DT_SIGNED_INTEGER_16)
			{
				switch (source.channels())
				{
					case 1u:
						FrameFilterSobelMagnitude::filterDiagonalAs1Channel8Bit<int16_t, 1u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterSobelMagnitude::filterDiagonalAs1Channel8Bit<int16_t, 2u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterSobelMagnitude::filterDiagonalAs1Channel8Bit<int16_t, 3u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterSobelMagnitude::filterDiagonalAs1Channel8Bit<int16_t, 4u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameFilterSobelMagnitude::Comfort::filterAs1Channel(const Frame& source, Frame& target, Worker* worker)
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
						FrameFilterSobelMagnitude::filterAs1Channel8Bit<int8_t, 1u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterSobelMagnitude::filterAs1Channel8Bit<int8_t, 2u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterSobelMagnitude::filterAs1Channel8Bit<int8_t, 3u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterSobelMagnitude::filterAs1Channel8Bit<int8_t, 4u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
			else if (target.dataType() == FrameType::DT_SIGNED_INTEGER_16)
			{
				switch (source.channels())
				{
					case 1u:
						FrameFilterSobelMagnitude::filterAs1Channel8Bit<int16_t, 1u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterSobelMagnitude::filterAs1Channel8Bit<int16_t, 2u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterSobelMagnitude::filterAs1Channel8Bit<int16_t, 3u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterSobelMagnitude::filterAs1Channel8Bit<int16_t, 4u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameFilterSobelMagnitude::Comfort::filterHorizontalVerticalTo1Response(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.numberPlanes() == 1u);
	ocean_assert(source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(source.width() == target.width() && source.height() == target.height());
	ocean_assert(target.channels() == 1u);
	ocean_assert(source.pixelOrigin() == target.pixelOrigin());
	ocean_assert(target.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 || target.dataType() == FrameType::DT_UNSIGNED_INTEGER_16);

	if (source.isValid() && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (source.width() == target.width() && source.height() == target.height() && source.pixelOrigin() == target.pixelOrigin() && target.channels() == 1u)
		{
			if (target.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
			{
				switch (source.channels())
				{
					case 1u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalTo1Response8Bit<uint8_t, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalTo1Response8Bit<uint8_t, 2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalTo1Response8Bit<uint8_t, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalTo1Response8Bit<uint8_t, 4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
			else if (target.dataType() == FrameType::DT_UNSIGNED_INTEGER_16)
			{
				switch (source.channels())
				{
					case 1u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalTo1Response8Bit<uint16_t, 1u>(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalTo1Response8Bit<uint16_t, 2u>(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalTo1Response8Bit<uint16_t, 3u>(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						FrameFilterSobelMagnitude::filterHorizontalVerticalTo1Response8Bit<uint16_t, 4u>(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
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
