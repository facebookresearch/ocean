/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterScharr.h"

namespace Ocean
{

namespace CV
{

bool FrameFilterScharr::Comfort::filterHorizontalVertical(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.numberPlanes() == 1u);
	ocean_assert(source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(source.width() == target.width() && source.height() == target.height());
	ocean_assert(source.channels() * 2u == target.channels());
	ocean_assert(source.pixelOrigin() == target.pixelOrigin());
	ocean_assert(target.dataType() == FrameType::DT_SIGNED_INTEGER_8 || target.dataType() == FrameType::DT_SIGNED_INTEGER_16);

	if (source.isValid() && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (source.width() == target.width() && source.height() == target.height() && source.pixelOrigin() == target.pixelOrigin() && source.channels() * 2u == target.channels())
		{
			if (target.dataType() == FrameType::DT_SIGNED_INTEGER_8)
			{
				switch (source.channels())
				{
					case 1u:
						filterHorizontalVertical8BitPerChannel<int8_t, 1u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						filterHorizontalVertical8BitPerChannel<int8_t, 2u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						filterHorizontalVertical8BitPerChannel<int8_t, 3u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						filterHorizontalVertical8BitPerChannel<int8_t, 4u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
			else if (target.dataType() == FrameType::DT_SIGNED_INTEGER_16)
			{
				switch (source.channels())
				{
					case 1u:
						filterHorizontalVertical8BitPerChannel<int16_t, 1u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						filterHorizontalVertical8BitPerChannel<int16_t, 2u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						filterHorizontalVertical8BitPerChannel<int16_t, 3u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						filterHorizontalVertical8BitPerChannel<int16_t, 4u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameFilterScharr::Comfort::filterDiagonal(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.numberPlanes() == 1u);
	ocean_assert(source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(source.width() == target.width() && source.height() == target.height());
	ocean_assert(source.channels() * 2u == target.channels());
	ocean_assert(source.pixelOrigin() == target.pixelOrigin());
	ocean_assert(target.dataType() == FrameType::DT_SIGNED_INTEGER_8 || target.dataType() == FrameType::DT_SIGNED_INTEGER_16);

	if (source.isValid() && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (source.width() == target.width() && source.height() == target.height() && source.pixelOrigin() == target.pixelOrigin() && source.channels() * 2u == target.channels())
		{
			if (target.dataType() == FrameType::DT_SIGNED_INTEGER_8)
			{
				switch (source.channels())
				{
					case 1u:
						filterDiagonal8BitPerChannel<int8_t, 1u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						filterDiagonal8BitPerChannel<int8_t, 2u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						filterDiagonal8BitPerChannel<int8_t, 3u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						filterDiagonal8BitPerChannel<int8_t, 4u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
			else if (target.dataType() == FrameType::DT_SIGNED_INTEGER_16)
			{
				switch (source.channels())
				{
					case 1u:
						filterDiagonal8BitPerChannel<int16_t, 1u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						filterDiagonal8BitPerChannel<int16_t, 2u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						filterDiagonal8BitPerChannel<int16_t, 3u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						filterDiagonal8BitPerChannel<int16_t, 4u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameFilterScharr::Comfort::filter(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.numberPlanes() == 1u);
	ocean_assert(source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(source.width() == target.width() && source.height() == target.height());
	ocean_assert(source.channels() * 4u == target.channels());
	ocean_assert(source.pixelOrigin() == target.pixelOrigin());
	ocean_assert(target.dataType() == FrameType::DT_SIGNED_INTEGER_8 || target.dataType() == FrameType::DT_SIGNED_INTEGER_16);

	if (source.isValid() && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (source.width() == target.width() && source.height() == target.height() && source.pixelOrigin() == target.pixelOrigin() && source.channels() * 4u == target.channels())
		{
			if (target.dataType() == FrameType::DT_SIGNED_INTEGER_8)
			{
				switch (source.channels())
				{
					case 1u:
						filter8BitPerChannel<int8_t, 1u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						filter8BitPerChannel<int8_t, 2u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						filter8BitPerChannel<int8_t, 3u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						filter8BitPerChannel<int8_t, 4u>(source.constdata<uint8_t>(), target.data<int8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
			else if (target.dataType() == FrameType::DT_SIGNED_INTEGER_16)
			{
				switch (source.channels())
				{
					case 1u:
						filter8BitPerChannel<int16_t, 1u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						filter8BitPerChannel<int16_t, 2u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						filter8BitPerChannel<int16_t, 3u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						filter8BitPerChannel<int16_t, 4u>(source.constdata<uint8_t>(), target.data<int16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameFilterScharr::Comfort::filterHorizontalVerticalMaximumAbsolute(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.numberPlanes() == 1u);
	ocean_assert(source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(source.width() == target.width() && source.height() == target.height());
	ocean_assert(source.channels() == target.channels());
	ocean_assert(source.pixelOrigin() == target.pixelOrigin());
	ocean_assert(target.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 || target.dataType() == FrameType::DT_UNSIGNED_INTEGER_16);

	if (source.isValid() && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (source.width() == target.width() && source.height() == target.height() && source.pixelOrigin() == target.pixelOrigin() && source.channels() == target.channels())
		{
			if (target.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
			{
				switch (source.channels())
				{
					case 1u:
						filterHorizontalVerticalMaximumAbsolute8BitPerChannel<uint8_t, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						filterHorizontalVerticalMaximumAbsolute8BitPerChannel<uint8_t, 2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						filterHorizontalVerticalMaximumAbsolute8BitPerChannel<uint8_t, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						filterHorizontalVerticalMaximumAbsolute8BitPerChannel<uint8_t, 4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
			else if (target.dataType() == FrameType::DT_UNSIGNED_INTEGER_16)
			{
				switch (source.channels())
				{
					case 1u:
						filterHorizontalVerticalMaximumAbsolute8BitPerChannel<uint16_t, 1u>(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						filterHorizontalVerticalMaximumAbsolute8BitPerChannel<uint16_t, 2u>(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						filterHorizontalVerticalMaximumAbsolute8BitPerChannel<uint16_t, 3u>(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						filterHorizontalVerticalMaximumAbsolute8BitPerChannel<uint16_t, 4u>(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameFilterScharr::Comfort::filterMaximumAbsolute(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());
	ocean_assert(source.numberPlanes() == 1u);
	ocean_assert(source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	ocean_assert(source.width() == target.width() && source.height() == target.height());
	ocean_assert(source.channels() == target.channels());
	ocean_assert(source.pixelOrigin() == target.pixelOrigin());
	ocean_assert(target.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 || target.dataType() == FrameType::DT_UNSIGNED_INTEGER_16);

	if (source.isValid() && source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (source.width() == target.width() && source.height() == target.height() && source.pixelOrigin() == target.pixelOrigin() && source.channels() == target.channels())
		{
			if (target.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
			{
				switch (source.channels())
				{
					case 1u:
						filterMaximumAbsolute8BitPerChannel<uint8_t, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						filterMaximumAbsolute8BitPerChannel<uint8_t, 2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						filterMaximumAbsolute8BitPerChannel<uint8_t, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						filterMaximumAbsolute8BitPerChannel<uint8_t, 4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;
				}
			}
			else if (target.dataType() == FrameType::DT_UNSIGNED_INTEGER_16)
			{
				switch (source.channels())
				{
					case 1u:
						filterMaximumAbsolute8BitPerChannel<uint16_t, 1u>(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 2u:
						filterMaximumAbsolute8BitPerChannel<uint16_t, 2u>(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 3u:
						filterMaximumAbsolute8BitPerChannel<uint16_t, 3u>(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
						return true;

					case 4u:
						filterMaximumAbsolute8BitPerChannel<uint16_t, 4u>(source.constdata<uint8_t>(), target.data<uint16_t>(), source.width(), source.height(), source.paddingElements(), target.paddingElements(), worker);
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
