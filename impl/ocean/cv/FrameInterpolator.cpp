/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameInterpolator.h"

namespace Ocean
{

namespace CV
{

bool FrameInterpolator::resize(Frame& frame, const unsigned int width, const unsigned int height, const ResizeMethod resizeMethod, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(!FrameType::formatIsPacked(frame.pixelFormat()));
	ocean_assert(width >= 1u && height >= 1u);

	if (!frame.isValid() || width == 0u || height == 0u || FrameType::formatIsPacked(frame.pixelFormat()))
	{
		return false;
	}

	if (frame.width() == width && frame.height() == height)
	{
		return true;
	}

	if (frame.width() % FrameType::widthMultiple(frame.pixelFormat()) != 0u || frame.height() % FrameType::heightMultiple(frame.pixelFormat()) != 0u)
	{
		ocean_assert(false && "Invalid image resolution");
		return false;
	}

	Frame tmpFrame(FrameType(frame, width, height));

	if (!resize(frame, tmpFrame, resizeMethod, worker))
	{
		return false;
	}

	frame = std::move(tmpFrame);

	return true;
}

bool FrameInterpolator::resize(const Frame& source, Frame& target, const ResizeMethod resizeMethod, Worker* worker)
{
	ocean_assert(!source.haveIntersectingMemory(target));

	if (!source.isValid() || !target.isValid() || source.pixelFormat() != target.pixelFormat() || source.pixelOrigin() != target.pixelOrigin() || source.dataType() != FrameType::DT_UNSIGNED_INTEGER_8 || FrameType::formatIsPacked(source.pixelFormat()))
	{
		ocean_assert(false && "Invalid parameters!");
		return false;
	}

	for (unsigned int planeIndex = 0u; planeIndex < source.numberPlanes(); ++planeIndex)
	{
		switch (source.planeChannels(planeIndex))
		{
			case 1u:
			{
				if (!resize<uint8_t, 1u>(source.constdata<uint8_t>(planeIndex), target.data<uint8_t>(planeIndex), source.planeWidth(planeIndex), source.planeHeight(planeIndex), target.planeWidth(planeIndex), target.planeHeight(planeIndex), source.paddingElements(planeIndex), target.paddingElements(planeIndex), resizeMethod, worker))
				{
					return false;
				}

				break;
			}

			case 2u:
			{
				if (!resize<uint8_t, 2u>(source.constdata<uint8_t>(planeIndex), target.data<uint8_t>(planeIndex), source.planeWidth(planeIndex), source.planeHeight(planeIndex), target.planeWidth(planeIndex), target.planeHeight(planeIndex), source.paddingElements(planeIndex), target.paddingElements(planeIndex), resizeMethod, worker))
				{
					return false;
				}

				break;
			}

			case 3u:
			{
				if (!resize<uint8_t, 3u>(source.constdata<uint8_t>(planeIndex), target.data<uint8_t>(planeIndex), source.planeWidth(planeIndex), source.planeHeight(planeIndex), target.planeWidth(planeIndex), target.planeHeight(planeIndex), source.paddingElements(planeIndex), target.paddingElements(planeIndex), resizeMethod, worker))
				{
					return false;
				}

				break;
			}

			case 4u:
			{
				if (!resize<uint8_t, 4u>(source.constdata<uint8_t>(planeIndex), target.data<uint8_t>(planeIndex), source.planeWidth(planeIndex), source.planeHeight(planeIndex), target.planeWidth(planeIndex), target.planeHeight(planeIndex), source.paddingElements(planeIndex), target.paddingElements(planeIndex), resizeMethod, worker))
				{
					return false;
				}

				break;
			}

			default:
				ocean_assert(false && "Invalid channel number!");
				return false;
		}
	}

	return true;
}

bool FrameInterpolator::affine(const Frame& input, Frame& output, const SquareMatrix3& input_A_output, const InterpolationMethod interpolationMethod, const uint8_t* borderColor, Worker* worker, const PixelPositionI& outputOrigin)
{
	switch (interpolationMethod)
	{
		case IM_NEAREST_PIXEL:
			return FrameInterpolatorNearestPixel::Comfort::affine(input, output, input_A_output, borderColor, worker, outputOrigin);

		default:
			ocean_assert(interpolationMethod == IM_BILINEAR);
			return FrameInterpolatorBilinear::Comfort::affine(input, output, input_A_output, borderColor, worker, outputOrigin);
	}
}

bool FrameInterpolator::homography(const Frame& input, Frame& output, const SquareMatrix3& input_H_output, const InterpolationMethod interpolationMethod, const uint8_t* borderColor, Worker* worker, const PixelPositionI& outputOrigin)
{
	switch (interpolationMethod)
	{
		case IM_NEAREST_PIXEL:
			return FrameInterpolatorNearestPixel::Comfort::homography(input, output, input_H_output, borderColor, worker, outputOrigin);

		default:
			ocean_assert(interpolationMethod == IM_BILINEAR);
			return FrameInterpolatorBilinear::Comfort::homography(input, output, input_H_output, borderColor, worker, outputOrigin);
	}
}

template <typename T, unsigned int tChannels>
bool FrameInterpolator::resize(const T* source, T* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const ResizeMethod resizeMethod, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	switch (resizeMethod)
	{
		case RM_INVALID:
			break;

		case RM_NEAREST_PIXEL:
			return resize<T, tChannels, RM_NEAREST_PIXEL>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, worker);

		case RM_BILINEAR:
			return resize<T, tChannels, RM_BILINEAR>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, worker);

		case RM_NEAREST_PYRAMID_LAYER_11_BILINEAR:
			return resize<T, tChannels, RM_NEAREST_PYRAMID_LAYER_11_BILINEAR>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, worker);

		case RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR:
			return resize<T, tChannels, RM_NEAREST_PYRAMID_LAYER_14641_BILINEAR>(source, target, sourceWidth, sourceHeight, targetWidth, targetHeight, sourcePaddingElements, targetPaddingElements, worker);
	}

	ocean_assert(false && "This should never happen!");
	return false;
}

}

}
