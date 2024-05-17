/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/advanced/FrameRectification.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

bool FrameRectification::Comfort::planarRectangleObject(const Frame& cameraFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector3& rectangleOrigin, const Vector3& rectangleHorizontal, const Vector3& rectangleVertical, Frame& rectifiedFrame, Worker* worker, const uint8_t* outsideFrameColor, const unsigned int approximationBinSize)
{
	ocean_assert(cameraFrame.isValid() && rectifiedFrame.isValid() && camera.isValid());
	ocean_assert(cameraFrame.pixelFormat() == rectifiedFrame.pixelFormat());
	ocean_assert(cameraFrame.pixelOrigin() == rectifiedFrame.pixelOrigin());

	ocean_assert(rectangleHorizontal.length() > 0 && rectangleVertical.length() > 0);
	ocean_assert(Numeric::isEqualEps(rectangleHorizontal * rectangleVertical));

	ocean_assert(rectifiedFrame.width() > 0u && rectifiedFrame.height() > 0u);

	ocean_assert(cameraFrame.width() == camera.width());
	ocean_assert(cameraFrame.height() == camera.height());

	ocean_assert(cameraFrame.numberPlanes() == 1u && cameraFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	if (!cameraFrame.isValid() || !rectifiedFrame.isValid() || !camera.isValid() || cameraFrame.width() != camera.width() || cameraFrame.height() != camera.height())
	{
		return false;
	}

	if (cameraFrame.pixelFormat() != rectifiedFrame.pixelFormat() || cameraFrame.pixelOrigin() != rectifiedFrame.pixelOrigin())
	{
		return false;
	}

	if (cameraFrame.numberPlanes() != 1u || cameraFrame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	const HomogenousMatrix4 flippedCamera_T_world(AnyCamera::standard2InvertedFlipped(world_T_camera));

	switch (cameraFrame.channels())
	{
		case 1u:
			planarRectangleObjectIF8BitPerChannel<1u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangleOrigin, rectangleHorizontal, rectangleVertical, rectifiedFrame.data<uint8_t>(), rectifiedFrame.width(), rectifiedFrame.height(), rectifiedFrame.paddingElements(), worker, outsideFrameColor, approximationBinSize);
			break;

		case 2u:
			planarRectangleObjectIF8BitPerChannel<2u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangleOrigin, rectangleHorizontal, rectangleVertical, rectifiedFrame.data<uint8_t>(), rectifiedFrame.width(), rectifiedFrame.height(), rectifiedFrame.paddingElements(), worker, outsideFrameColor, approximationBinSize);
			break;

		case 3u:
			planarRectangleObjectIF8BitPerChannel<3u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangleOrigin, rectangleHorizontal, rectangleVertical, rectifiedFrame.data<uint8_t>(), rectifiedFrame.width(), rectifiedFrame.height(), rectifiedFrame.paddingElements(), worker, outsideFrameColor, approximationBinSize);
			break;

		case 4u:
			planarRectangleObjectIF8BitPerChannel<4u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangleOrigin, rectangleHorizontal, rectangleVertical, rectifiedFrame.data<uint8_t>(), rectifiedFrame.width(), rectifiedFrame.height(), rectifiedFrame.paddingElements(), worker, outsideFrameColor, approximationBinSize);
			break;

		default:
			ocean_assert(false && "Invalid pixel format!");
			return false;
	}

	return true;
}

bool FrameRectification::Comfort::arbitraryRectangleObject(const Frame& cameraFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector3& rectangle0, const Vector3& rectangle1, const Vector3& rectangle2, const Vector3& rectangle3, Frame& rectifiedFrame, Worker* worker, const uint8_t* outsideFrameColor, const unsigned int approximationBinSize)
{
	ocean_assert(cameraFrame.isValid() && rectifiedFrame.isValid() && camera.isValid());
	ocean_assert(cameraFrame.pixelFormat() == rectifiedFrame.pixelFormat());
	ocean_assert(cameraFrame.pixelOrigin() == rectifiedFrame.pixelOrigin());

	ocean_assert(rectifiedFrame.width() > 0u && rectifiedFrame.height() > 0u);

	ocean_assert(cameraFrame.width() == camera.width());
	ocean_assert(cameraFrame.height() == camera.height());

	ocean_assert(cameraFrame.numberPlanes() == 1u && cameraFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	if (!cameraFrame.isValid() || !rectifiedFrame.isValid() || !camera.isValid() || cameraFrame.width() != camera.width() || cameraFrame.height() != camera.height())
	{
		return false;
	}

	if (cameraFrame.pixelFormat() != rectifiedFrame.pixelFormat() || cameraFrame.pixelOrigin() != rectifiedFrame.pixelOrigin())
	{
		return false;
	}

	if (cameraFrame.numberPlanes() != 1u || cameraFrame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	const HomogenousMatrix4 flippedCamera_T_world(AnyCamera::standard2InvertedFlipped(world_T_camera));

	switch (cameraFrame.channels())
	{
		case 1u:
			arbitraryRectangleObjectIF8BitPerChannel<1u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangle0, rectangle1, rectangle2, rectangle3, rectifiedFrame.data<uint8_t>(), rectifiedFrame.width(), rectifiedFrame.height(), rectifiedFrame.paddingElements(), worker, outsideFrameColor, approximationBinSize);
			break;

		case 2u:
			arbitraryRectangleObjectIF8BitPerChannel<2u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangle0, rectangle1, rectangle2, rectangle3, rectifiedFrame.data<uint8_t>(), rectifiedFrame.width(), rectifiedFrame.height(), rectifiedFrame.paddingElements(), worker, outsideFrameColor, approximationBinSize);
			break;

		case 3u:
			arbitraryRectangleObjectIF8BitPerChannel<3u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangle0, rectangle1, rectangle2, rectangle3, rectifiedFrame.data<uint8_t>(), rectifiedFrame.width(), rectifiedFrame.height(), rectifiedFrame.paddingElements(), worker, outsideFrameColor, approximationBinSize);
			break;

		case 4u:
			arbitraryRectangleObjectIF8BitPerChannel<4u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangle0, rectangle1, rectangle2, rectangle3, rectifiedFrame.data<uint8_t>(), rectifiedFrame.width(), rectifiedFrame.height(), rectifiedFrame.paddingElements(), worker, outsideFrameColor, approximationBinSize);
			break;

		default:
			ocean_assert(false && "Invalid pixel format!");
			return false;
	}

	return true;
}

bool FrameRectification::Comfort::triangleObject(const Frame& cameraFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Triangle2& triangle2, const Triangle3& triangle3, Frame& targetFrame, Worker* worker, const uint8_t* outsideFrameColor, const unsigned int approximationBinSize)
{
	ocean_assert(cameraFrame.isValid() && targetFrame.isValid() && camera.isValid());
	ocean_assert(cameraFrame.pixelFormat() == targetFrame.pixelFormat());
	ocean_assert(cameraFrame.pixelOrigin() == targetFrame.pixelOrigin());

	ocean_assert(triangle2.isValid() && triangle3.isValid());

	ocean_assert(targetFrame.width() > 0u && targetFrame.height() > 0u);

	ocean_assert(cameraFrame.width() == camera.width());
	ocean_assert(cameraFrame.height() == camera.height());

	ocean_assert(cameraFrame.numberPlanes() == 1u && cameraFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	if (!cameraFrame.isValid() || !targetFrame.isValid() || !camera.isValid())
	{
		return false;
	}

	if (cameraFrame.pixelFormat() != targetFrame.pixelFormat() || cameraFrame.pixelOrigin() != targetFrame.pixelOrigin())
	{
		return false;
	}

	if (cameraFrame.numberPlanes() != 1u || cameraFrame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	const HomogenousMatrix4 flippedCamera_T_world(AnyCamera::standard2InvertedFlipped(world_T_camera));

	switch (cameraFrame.channels())
	{
		case 1u:
			triangleObjectIF8BitPerChannel<1u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, triangle2, triangle3, targetFrame.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), worker, outsideFrameColor, approximationBinSize);
			break;

		case 2u:
			triangleObjectIF8BitPerChannel<2u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, triangle2, triangle3, targetFrame.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), worker, outsideFrameColor, approximationBinSize);
			break;

		case 3u:
			triangleObjectIF8BitPerChannel<3u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, triangle2, triangle3, targetFrame.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), worker, outsideFrameColor, approximationBinSize);
			break;

		case 4u:
			triangleObjectIF8BitPerChannel<4u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, triangle2, triangle3, targetFrame.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), worker, outsideFrameColor, approximationBinSize);
			break;

		default:
			ocean_assert(false && "Invalid pixel format!");
			return false;
	}

	return true;
}

bool FrameRectification::Comfort::planarRectangleObjectMask(const Frame& cameraFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector3& rectangleOrigin, const Vector3& rectangleHorizontal, const Vector3& rectangleVertical, Frame& targetFrame, Frame& targetMask, Worker* worker, const uint8_t maskValue, const unsigned int approximationBinSize)
{
	ocean_assert(cameraFrame.isValid() && targetFrame.isValid() && camera.isValid());
	ocean_assert(cameraFrame.pixelFormat() == targetFrame.pixelFormat());
	ocean_assert(cameraFrame.pixelOrigin() == targetFrame.pixelOrigin());

	ocean_assert(rectangleHorizontal.length() > 0 && rectangleVertical.length() > 0);
	ocean_assert(Numeric::isEqualEps(rectangleHorizontal * rectangleVertical));

	ocean_assert(cameraFrame.width() == camera.width());
	ocean_assert(cameraFrame.height() == camera.height());

	ocean_assert(cameraFrame.numberPlanes() == 1u && cameraFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	if (!cameraFrame.isValid() || !targetFrame.isValid() || !camera.isValid() || cameraFrame.width() != camera.width() || cameraFrame.height() != camera.height())
	{
		return false;
	}

	if (cameraFrame.pixelFormat() != targetFrame.pixelFormat() || cameraFrame.pixelOrigin() != targetFrame.pixelOrigin())
	{
		return false;
	}

	if (cameraFrame.numberPlanes() != 1u || cameraFrame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	const HomogenousMatrix4 flippedCamera_T_world(AnyCamera::standard2InvertedFlipped(world_T_camera));

	if (!targetMask.set(FrameType(targetFrame, FrameType::FORMAT_Y8), false /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	switch (cameraFrame.channels())
	{
		case 1u:
			planarRectangleObjectMaskIF8BitPerChannel<1u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangleOrigin, rectangleHorizontal, rectangleVertical, targetFrame.data<uint8_t>(), targetMask.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), targetMask.paddingElements(), worker, maskValue, approximationBinSize);
			break;

		case 2u:
			planarRectangleObjectMaskIF8BitPerChannel<2u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangleOrigin, rectangleHorizontal, rectangleVertical, targetFrame.data<uint8_t>(), targetMask.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), targetMask.paddingElements(), worker, maskValue, approximationBinSize);
			break;

		case 3u:
			planarRectangleObjectMaskIF8BitPerChannel<3u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangleOrigin, rectangleHorizontal, rectangleVertical, targetFrame.data<uint8_t>(), targetMask.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), targetMask.paddingElements(), worker, maskValue, approximationBinSize);
			break;

		case 4u:
			planarRectangleObjectMaskIF8BitPerChannel<4u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangleOrigin, rectangleHorizontal, rectangleVertical, targetFrame.data<uint8_t>(), targetMask.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), targetMask.paddingElements(), worker, maskValue, approximationBinSize);
			break;

		default:
			ocean_assert(false && "Invalid pixel format!");
			return false;
	}

	return true;
}

bool FrameRectification::Comfort::arbitraryRectangleObjectMask(const Frame& cameraFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector3& rectangle0, const Vector3& rectangle1, const Vector3& rectangle2, const Vector3& rectangle3, Frame& targetFrame, Frame& targetMask, Worker* worker, const uint8_t maskValue, const unsigned int approximationBinSize)
{
	ocean_assert(cameraFrame.isValid() && targetFrame.isValid() && camera.isValid());
	ocean_assert(cameraFrame.pixelFormat() == targetFrame.pixelFormat());
	ocean_assert(cameraFrame.pixelOrigin() == targetFrame.pixelOrigin());

	ocean_assert(cameraFrame.width() == camera.width());
	ocean_assert(cameraFrame.height() == camera.height());

	ocean_assert(cameraFrame.numberPlanes() == 1u && cameraFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	if (!cameraFrame.isValid() || !targetFrame.isValid() || !camera.isValid() || cameraFrame.width() != camera.width() || cameraFrame.height() != camera.height())
	{
		return false;
	}

	if (cameraFrame.pixelFormat() != targetFrame.pixelFormat() || cameraFrame.pixelOrigin() != targetFrame.pixelOrigin())
	{
		return false;
	}

	if (cameraFrame.numberPlanes() != 1u || cameraFrame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	const HomogenousMatrix4 flippedCamera_T_world(AnyCamera::standard2InvertedFlipped(world_T_camera));

	if (!targetMask.set(FrameType(targetFrame, FrameType::FORMAT_Y8), false /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	switch (cameraFrame.channels())
	{
		case 1u:
			arbitraryRectangleObjectMaskIF8BitPerChannel<1u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangle0, rectangle1, rectangle2, rectangle3, targetFrame.data<uint8_t>(), targetMask.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), targetMask.paddingElements(), worker, maskValue, approximationBinSize);
			break;

		case 2u:
			arbitraryRectangleObjectMaskIF8BitPerChannel<2u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangle0, rectangle1, rectangle2, rectangle3, targetFrame.data<uint8_t>(), targetMask.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), targetMask.paddingElements(), worker, maskValue, approximationBinSize);
			break;

		case 3u:
			arbitraryRectangleObjectMaskIF8BitPerChannel<3u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangle0, rectangle1, rectangle2, rectangle3, targetFrame.data<uint8_t>(), targetMask.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), targetMask.paddingElements(), worker, maskValue, approximationBinSize);
			break;

		case 4u:
			arbitraryRectangleObjectMaskIF8BitPerChannel<4u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, rectangle0, rectangle1, rectangle2, rectangle3, targetFrame.data<uint8_t>(), targetMask.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), targetMask.paddingElements(), worker, maskValue, approximationBinSize);
			break;

		default:
			ocean_assert(false && "Invalid pixel format!");
			return false;
	}

	return true;
}

bool FrameRectification::Comfort::triangleObjectMask(const Frame& cameraFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Triangle2& triangle2, const Triangle3& triangle3, Frame& targetFrame, Frame& targetMask, Worker* worker, const uint8_t maskValue, const unsigned int approximationBinSize)
{
	ocean_assert(cameraFrame.isValid() && targetFrame.isValid() && camera.isValid());
	ocean_assert(cameraFrame.pixelFormat() == targetFrame.pixelFormat());
	ocean_assert(cameraFrame.pixelOrigin() == targetFrame.pixelOrigin());

	ocean_assert(triangle2.isValid() && triangle3.isValid());

	ocean_assert(cameraFrame.width() == camera.width());
	ocean_assert(cameraFrame.height() == camera.height());

	ocean_assert(cameraFrame.numberPlanes() == 1u && cameraFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	if (!cameraFrame.isValid() || !targetFrame.isValid() || !camera.isValid() || cameraFrame.width() != camera.width() || cameraFrame.height() != camera.height())
	{
		return false;
	}

	if (cameraFrame.pixelFormat() != targetFrame.pixelFormat() || cameraFrame.pixelOrigin() != targetFrame.pixelOrigin())
	{
		return false;
	}

	if (cameraFrame.numberPlanes() != 1u || cameraFrame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		return false;
	}

	const HomogenousMatrix4 flippedCamera_T_world(AnyCamera::standard2InvertedFlipped(world_T_camera));

	if (!targetMask.set(FrameType(targetFrame, FrameType::FORMAT_Y8), false /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	switch (cameraFrame.channels())
	{
		case 1u:
			triangleObjectMaskIF8BitPerChannel<1u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, triangle2, triangle3, targetFrame.data<uint8_t>(), targetMask.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), targetMask.paddingElements(), worker, maskValue, approximationBinSize);
			break;

		case 2u:
			triangleObjectMaskIF8BitPerChannel<2u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, triangle2, triangle3, targetFrame.data<uint8_t>(), targetMask.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), targetMask.paddingElements(), worker, maskValue, approximationBinSize);
			break;

		case 3u:
			triangleObjectMaskIF8BitPerChannel<3u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, triangle2, triangle3, targetFrame.data<uint8_t>(), targetMask.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), targetMask.paddingElements(), worker, maskValue, approximationBinSize);
			break;

		case 4u:
			triangleObjectMaskIF8BitPerChannel<4u>(cameraFrame.constdata<uint8_t>(), cameraFrame.paddingElements(), cameraFrame.pixelOrigin(), camera, flippedCamera_T_world, triangle2, triangle3, targetFrame.data<uint8_t>(), targetMask.data<uint8_t>(), targetFrame.width(), targetFrame.height(), targetFrame.paddingElements(), targetMask.paddingElements(), worker, maskValue, approximationBinSize);
			break;

		default:
			ocean_assert(false && "Invalid pixel format!");
			return false;
	}

	return true;
}

}

}

}
