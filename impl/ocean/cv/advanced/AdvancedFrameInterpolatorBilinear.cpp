/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

bool AdvancedFrameInterpolatorBilinear::Comfort::homographyFilterMask(const Frame& input, const Frame& outputFilterMask, Frame& output, const SquareMatrix3& input_H_output, const PixelBoundingBox& outputBoundingBox, Worker* worker)
{
	ocean_assert(input.isValid() && output.isValid());
	ocean_assert(input.isPixelFormatCompatible(output.pixelFormat()) && input.pixelOrigin() == output.pixelOrigin());

	ocean_assert(outputFilterMask.isValid());
	ocean_assert(outputFilterMask.isPixelFormatCompatible(FrameType::FORMAT_Y8));
	ocean_assert(outputFilterMask.pixelOrigin() == input.pixelOrigin());

	if (!input.isPixelFormatCompatible(output.pixelFormat()) || input.pixelOrigin() != output.pixelOrigin() || input.numberPlanes() != 1u || input.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	switch (input.channels())
	{
		case 1u:
			homographyFilterMask8BitPerChannel<1u>(input.constdata<uint8_t>(), outputFilterMask.constdata<uint8_t>(), output.data<uint8_t>(), input.width(), input.height(), output.width(), output.height(), input.paddingElements(), outputFilterMask.paddingElements(), output.paddingElements(), input_H_output, outputBoundingBox, worker);
			return true;

		case 2u:
			homographyFilterMask8BitPerChannel<2u>(input.constdata<uint8_t>(), outputFilterMask.constdata<uint8_t>(), output.data<uint8_t>(), input.width(), input.height(), output.width(), output.height(), input.paddingElements(), outputFilterMask.paddingElements(), output.paddingElements(), input_H_output, outputBoundingBox, worker);
			return true;

		case 3u:
			homographyFilterMask8BitPerChannel<3u>(input.constdata<uint8_t>(), outputFilterMask.constdata<uint8_t>(), output.data<uint8_t>(), input.width(), input.height(), output.width(), output.height(), input.paddingElements(), outputFilterMask.paddingElements(), output.paddingElements(), input_H_output, outputBoundingBox, worker);
			return true;

		case 4u:
			homographyFilterMask8BitPerChannel<4u>(input.constdata<uint8_t>(), outputFilterMask.constdata<uint8_t>(), output.data<uint8_t>(), input.width(), input.height(), output.width(), output.height(), input.paddingElements(), outputFilterMask.paddingElements(), output.paddingElements(), input_H_output, outputBoundingBox, worker);
			return true;
	}

	ocean_assert(false && "Invalid channel number!");
	return false;
}

bool AdvancedFrameInterpolatorBilinear::interpolateTriangle(const Frame& source, Frame& target, const PixelTriangle& sourceTriangle, const PixelTriangle& targetTriangle, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());

	if (source.pixelFormat() != target.pixelFormat() || source.pixelOrigin() != target.pixelOrigin() || source.numberPlanes() != 1u || source.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	switch (source.channels())
	{
		case 1u:
			interpolateTriangle8BitPerChannel<1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), sourceTriangle, targetTriangle, source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;

		case 2u:
			interpolateTriangle8BitPerChannel<2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), sourceTriangle, targetTriangle, source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;

		case 3u:
			interpolateTriangle8BitPerChannel<3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), sourceTriangle, targetTriangle, source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;

		case 4u:
			interpolateTriangle8BitPerChannel<4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), sourceTriangle, targetTriangle, source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;

		default:
			ocean_assert(false && "Invalid pixel format!");
	}

	return false;
}

bool AdvancedFrameInterpolatorBilinear::interpolateTriangle(const Frame& source, Frame& target, const Triangle2& sourceTriangle, const Triangle2& targetTriangle, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());

	if (source.pixelFormat() != target.pixelFormat() || source.pixelOrigin() != target.pixelOrigin() || source.numberPlanes() != 1u || source.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	switch (source.channels())
	{
		case 1u:
			interpolateTriangle8BitPerChannel<1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), sourceTriangle, targetTriangle, source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;

		case 2u:
			interpolateTriangle8BitPerChannel<2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), sourceTriangle, targetTriangle, source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;

		case 3u:
			interpolateTriangle8BitPerChannel<3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), sourceTriangle, targetTriangle, source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;

		case 4u:
			interpolateTriangle8BitPerChannel<4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), sourceTriangle, targetTriangle, source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
			return true;

		default:
			ocean_assert(false && "Invalid pixel format!");
	}

	return false;
}

}

}

}
