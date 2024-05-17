/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/cv/IntegralImage.h"

#include "ocean/math/Numeric.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace CV
{

bool FrameInterpolatorBilinear::Comfort::resize(const Frame& source, Frame& target, Worker* worker)
{
	ocean_assert(source.isValid() && target.isValid());

	if (source.pixelFormat() != target.pixelFormat() || source.pixelOrigin() != target.pixelOrigin())
	{
		return false;
	}

	if (source.numberPlanes() == 1u)
	{
		const FrameType::DataType dataType = source.dataType();

		if (dataType == FrameType::DT_UNSIGNED_INTEGER_8)
		{
			switch (source.channels())
			{
				case 1u:
					FrameInterpolatorBilinear::resize<uint8_t, 1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 2u:
					FrameInterpolatorBilinear::resize<uint8_t, 2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 3u:
					FrameInterpolatorBilinear::resize<uint8_t, 3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 4u:
					FrameInterpolatorBilinear::resize<uint8_t, 4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;
			}
		}
		else if (dataType == FrameType::DT_UNSIGNED_INTEGER_16)
		{
			switch (source.channels())
			{
				case 1u:
					FrameInterpolatorBilinear::resize<uint16_t, 1u>(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 2u:
					FrameInterpolatorBilinear::resize<uint16_t, 2u>(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 3u:
					FrameInterpolatorBilinear::resize<uint16_t, 3u>(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 4u:
					FrameInterpolatorBilinear::resize<uint16_t, 4u>(source.constdata<uint16_t>(), target.data<uint16_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;
			}
		}
		else if (dataType == FrameType::DT_UNSIGNED_INTEGER_32)
		{
			switch (source.channels())
			{
				case 1u:
					FrameInterpolatorBilinear::resize<uint32_t, 1u>(source.constdata<uint32_t>(), target.data<uint32_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 2u:
					FrameInterpolatorBilinear::resize<uint32_t, 2u>(source.constdata<uint32_t>(), target.data<uint32_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 3u:
					FrameInterpolatorBilinear::resize<uint32_t, 3u>(source.constdata<uint32_t>(), target.data<uint32_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 4u:
					FrameInterpolatorBilinear::resize<uint32_t, 4u>(source.constdata<uint32_t>(), target.data<uint32_t>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;
			}
		}
		else if (dataType == FrameType::DT_SIGNED_FLOAT_32)
		{
			switch (source.channels())
			{
				case 1u:
					FrameInterpolatorBilinear::resize<float, 1u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 2u:
					FrameInterpolatorBilinear::resize<float, 2u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 3u:
					FrameInterpolatorBilinear::resize<float, 3u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;

				case 4u:
					FrameInterpolatorBilinear::resize<float, 4u>(source.constdata<float>(), target.data<float>(), source.width(), source.height(), target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
					return true;
			}
		}
	}

	ocean_assert(false && "Not supported pixel format!");
	return false;
}

bool FrameInterpolatorBilinear::Comfort::zoom(const Frame& source, Frame& target, const Scalar zoomFactor, Worker* worker)
{
	ocean_assert(source.isValid());
	ocean_assert(zoomFactor > 0);

	if (!source.isValid() || zoomFactor <= 0)
	{
		return false;
	}

	if (source.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		// we need a generic pixel format with uint8_t data type e.g., FORMAT_Y8, FORAT_RGB24, etc.
		ocean_assert(false && "Invalid data type!");
		return false;
	}

	if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	ocean_assert(Numeric::isNotEqualEps(zoomFactor));
	const Scalar invZoomFactor = Scalar(1) / zoomFactor;

	const SquareMatrix3 translation(Vector3(1, 0, 0), Vector3(0, 1, 0), Vector3(Scalar(source.width()) * Scalar(0.5), Scalar(source.height()) * Scalar(0.5), 1));
	const SquareMatrix3 scale(Vector3(invZoomFactor, 0, 0), Vector3(0, invZoomFactor, 0), Vector3(0, 0, 1));

	const SquareMatrix3 affine(translation * scale * translation.inverted());

	return homography(source, target, affine, nullptr, worker);
}

bool FrameInterpolatorBilinear::Comfort::affine(const Frame& source, Frame& target, const SquareMatrix3& source_A_target, const uint8_t* borderColor, Worker* worker, const PixelPositionI& targetOrigin)
{
	ocean_assert(source && target);

	if (source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && target.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && source.numberPlanes() == 1u && target.isValid() && target.pixelFormat() == source.pixelFormat() && target.pixelOrigin() == source.pixelOrigin())
	{
		switch (source.channels())
		{
			case 1u:
				affine8BitPerChannel<1u>(source.constdata<uint8_t>(), source.width(), source.height(), source_A_target, borderColor, target.data<uint8_t>(), targetOrigin, target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 2u:
				affine8BitPerChannel<2u>(source.constdata<uint8_t>(), source.width(), source.height(), source_A_target, borderColor, target.data<uint8_t>(), targetOrigin, target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 3u:
				affine8BitPerChannel<3u>(source.constdata<uint8_t>(), source.width(), source.height(), source_A_target, borderColor, target.data<uint8_t>(), targetOrigin, target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			case 4u:
				affine8BitPerChannel<4u>(source.constdata<uint8_t>(), source.width(), source.height(), source_A_target, borderColor, target.data<uint8_t>(), targetOrigin, target.width(), target.height(), source.paddingElements(), target.paddingElements(), worker);
				return true;

			default:
				// Nothing else to do - will fail below
				break;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorBilinear::Comfort::homography(const Frame& input, Frame& output, const SquareMatrix3& input_H_output, const void* borderColor, Worker* worker, const PixelPositionI& outputOrigin)
{
	ocean_assert(input.isValid() && output.isValid());

	if (FrameType::arePixelFormatsCompatible(input.pixelFormat(), output.pixelFormat()) && input.numberPlanes() == 1u && input.pixelOrigin() == output.pixelOrigin())
	{
		if (input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
		{
			const uint8_t* const borderColor_u8 = (const uint8_t*)(borderColor);

			switch (input.channels())
			{
				case 1u:
					FrameInterpolatorBilinear::homography<uint8_t, 1u>(input.constdata<uint8_t>(), input.width(), input.height(), input_H_output, borderColor_u8, output.data<uint8_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 2u:
					FrameInterpolatorBilinear::homography<uint8_t, 2u>(input.constdata<uint8_t>(), input.width(), input.height(), input_H_output, borderColor_u8, output.data<uint8_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 3u:
					FrameInterpolatorBilinear::homography<uint8_t, 3u>(input.constdata<uint8_t>(), input.width(), input.height(), input_H_output, borderColor_u8, output.data<uint8_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 4u:
					FrameInterpolatorBilinear::homography<uint8_t, 4u>(input.constdata<uint8_t>(), input.width(), input.height(), input_H_output, borderColor_u8, output.data<uint8_t>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;
			}
		}
		else if (input.dataType() == FrameType::DT_SIGNED_FLOAT_32)
		{
			ocean_assert(size_t(borderColor) % sizeof(float) == 0); // the border color is most likely not provided as float values

			const float* const borderColor_float = (const float*)(borderColor);

			switch (input.channels())
			{
				case 1u:
					FrameInterpolatorBilinear::homography<float, 1u>(input.constdata<float>(), input.width(), input.height(), input_H_output, borderColor_float, output.data<float>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 2u:
					FrameInterpolatorBilinear::homography<float, 2u>(input.constdata<float>(), input.width(), input.height(), input_H_output, borderColor_float, output.data<float>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 3u:
					FrameInterpolatorBilinear::homography<float, 3u>(input.constdata<float>(), input.width(), input.height(), input_H_output, borderColor_float, output.data<float>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 4u:
					FrameInterpolatorBilinear::homography<float, 4u>(input.constdata<float>(), input.width(), input.height(), input_H_output, borderColor_float, output.data<float>(), outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
					return true;
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorBilinear::Comfort::homographies(const Frame& input, Frame& output, const SquareMatrix3 homography[4], const Vector2& outputQuadrantCenter, const uint8_t* borderColor, Worker* worker, const PixelPositionI& outputOrigin)
{
	ocean_assert(input && output);

	ocean_assert(outputQuadrantCenter.x() >= 0 && outputQuadrantCenter.x() < Scalar(output.width()));
	ocean_assert(outputQuadrantCenter.y() >= 0 && outputQuadrantCenter.y() < Scalar(output.height()));

	if (input.dataType() == output.dataType() && input.channels() == output.channels()
		&& input.numberPlanes() == 1u
		&& input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8
		&& input.pixelOrigin() == output.pixelOrigin())
	{
		switch (input.channels())
		{
			case 1u:
				homographies8BitPerChannel<1u>(input.constdata<uint8_t>(), input.width(), input.height(), homography, borderColor, output.data<uint8_t>(), outputQuadrantCenter, outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			case 2u:
				homographies8BitPerChannel<2u>(input.constdata<uint8_t>(), input.width(), input.height(), homography, borderColor, output.data<uint8_t>(), outputQuadrantCenter, outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			case 3u:
				homographies8BitPerChannel<3u>(input.constdata<uint8_t>(), input.width(), input.height(), homography, borderColor, output.data<uint8_t>(), outputQuadrantCenter, outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			case 4u:
				homographies8BitPerChannel<4u>(input.constdata<uint8_t>(), input.width(), input.height(), homography, borderColor, output.data<uint8_t>(), outputQuadrantCenter, outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), worker);
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorBilinear::Comfort::homographiesMask(const Frame& input, Frame& output, Frame& outputMask, const SquareMatrix3* homographies, const Vector2& outputQuadrantCenter, Worker* worker, const uint8_t maskValue, const PixelPositionI& outputOrigin)
{
	ocean_assert(input && output && outputMask);
	ocean_assert(homographies);

	ocean_assert(outputQuadrantCenter.x() >= 0 && outputQuadrantCenter.x() < Scalar(output.width()));
	ocean_assert(outputQuadrantCenter.y() >= 0 && outputQuadrantCenter.y() < Scalar(output.height()));

	if (input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && output.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && input.numberPlanes() == 1u && output.isValid() && output.pixelFormat() == input.pixelFormat() && output.pixelOrigin() == input.pixelOrigin())
	{
		ocean_assert(FrameType(output, FrameType::FORMAT_Y8) == outputMask.frameType());

		switch (input.channels())
		{
			case 1u:
				homographiesMask8BitPerChannel<1u>(input.constdata<uint8_t>(), input.width(), input.height(), homographies, output.data<uint8_t>(), outputMask.data<uint8_t>(), outputQuadrantCenter, outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), outputMask.paddingElements(), worker, maskValue);
				return true;

			case 2u:
				homographiesMask8BitPerChannel<2u>(input.constdata<uint8_t>(), input.width(), input.height(), homographies, output.data<uint8_t>(), outputMask.data<uint8_t>(), outputQuadrantCenter, outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), outputMask.paddingElements(), worker, maskValue);
				return true;

			case 3u:
				homographiesMask8BitPerChannel<3u>(input.constdata<uint8_t>(), input.width(), input.height(), homographies, output.data<uint8_t>(), outputMask.data<uint8_t>(), outputQuadrantCenter, outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), outputMask.paddingElements(), worker, maskValue);
				return true;

			case 4u:
				homographiesMask8BitPerChannel<4u>(input.constdata<uint8_t>(), input.width(), input.height(), homographies, output.data<uint8_t>(), outputMask.data<uint8_t>(), outputQuadrantCenter, outputOrigin, output.width(), output.height(), input.paddingElements(), output.paddingElements(), outputMask.paddingElements(), worker, maskValue);
				return true;

			default:
				// Nothing else to do - will fail below
				break;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorBilinear::Comfort::homographyMask(const Frame& input, Frame& output, Frame& outputMask, const SquareMatrix3& input_H_output, Worker* worker, const uint8_t maskValue, const PixelPositionI& outputOrigin)
{
	ocean_assert(input && output && outputMask);

	if (input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && output.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && input.numberPlanes() == 1u && output.isValid() && output.pixelFormat() == input.pixelFormat() && output.pixelOrigin() == input.pixelOrigin())
	{
		ocean_assert(outputMask.isFrameTypeCompatible(FrameType(output, FrameType::FORMAT_Y8), false /*allowDifferentPixelOrigins*/));

		switch (input.channels())
		{
			case 1u:
				homographyMask8BitPerChannel<1u>(input.constdata<uint8_t>(), input.width(), input.height(), input_H_output, output.data<uint8_t>(), outputMask.data<uint8_t>(), outputOrigin, output.width(), output.height(), maskValue, input.paddingElements(), output.paddingElements(), outputMask.paddingElements(), worker);
				return true;

			case 2u:
				homographyMask8BitPerChannel<2u>(input.constdata<uint8_t>(), input.width(), input.height(), input_H_output, output.data<uint8_t>(), outputMask.data<uint8_t>(), outputOrigin, output.width(), output.height(), maskValue, input.paddingElements(), output.paddingElements(), outputMask.paddingElements(), worker);
				return true;

			case 3u:
				homographyMask8BitPerChannel<3u>(input.constdata<uint8_t>(), input.width(), input.height(), input_H_output, output.data<uint8_t>(), outputMask.data<uint8_t>(), outputOrigin, output.width(), output.height(), maskValue, input.paddingElements(), output.paddingElements(), outputMask.paddingElements(), worker);
				return true;

			case 4u:
				homographyMask8BitPerChannel<4u>(input.constdata<uint8_t>(), input.width(), input.height(), input_H_output, output.data<uint8_t>(), outputMask.data<uint8_t>(), outputOrigin, output.width(), output.height(), maskValue, input.paddingElements(), output.paddingElements(), outputMask.paddingElements(), worker);
				return true;

			default:
				// Nothing else to do - will fail below
				break;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorBilinear::Comfort::homographyWithCamera(const PinholeCamera& inputCamera, const PinholeCamera& outputCamera, const Frame& input, Frame& output, const SquareMatrix3& homography, const bool useDistortionParameters, const uint8_t* borderColor, Worker* worker)
{
	ocean_assert(inputCamera.isValid() && outputCamera.isValid() && input && output);

	ocean_assert(inputCamera.width() == input.width());
	ocean_assert(inputCamera.height() == input.height());

	if (input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && input.numberPlanes() == 1u)
	{
		if (!output.set(FrameType(input.frameType(), outputCamera.width(), outputCamera.height()), true, true))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		switch (input.channels())
		{
			case 1u:
				homographyWithCamera8BitPerChannel<1u>(inputCamera, outputCamera, input.constdata<uint8_t>(), homography, useDistortionParameters, borderColor, output.data<uint8_t>(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			case 2u:
				homographyWithCamera8BitPerChannel<2u>(inputCamera, outputCamera, input.constdata<uint8_t>(), homography, useDistortionParameters, borderColor, output.data<uint8_t>(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			case 3u:
				homographyWithCamera8BitPerChannel<3u>(inputCamera, outputCamera, input.constdata<uint8_t>(), homography, useDistortionParameters, borderColor, output.data<uint8_t>(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			case 4u:
				homographyWithCamera8BitPerChannel<4u>(inputCamera, outputCamera, input.constdata<uint8_t>(), homography, useDistortionParameters, borderColor, output.data<uint8_t>(), input.paddingElements(), output.paddingElements(), worker);
				return true;

			default:
				// Nothing else to do - will fail below
				break;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorBilinear::Comfort::homographyWithCameraMask(const AnyCamera& inputCamera, const AnyCamera& outputCamera, const Frame& input, Frame& output, Frame& outputMask, const SquareMatrix3& homography, Worker* worker, const uint8_t maskValue)
{
	ocean_assert(inputCamera.isValid() && outputCamera.isValid() && input && output);

	ocean_assert(inputCamera.width() == input.width());
	ocean_assert(inputCamera.height() == input.height());

	if (input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && input.numberPlanes() == 1u)
	{
		if (!output.set(FrameType(input.frameType(), outputCamera.width(), outputCamera.height()), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (!outputMask.set(FrameType(output, FrameType::FORMAT_Y8), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (inputCamera.name() == AnyCameraPinhole::WrappedCamera::name() && outputCamera.name() == AnyCameraPinhole::WrappedCamera::name())
		{
			const AnyCameraPinhole& inputPinholeCamera = (const AnyCameraPinhole&)(inputCamera);
			const AnyCameraPinhole& outputPinholeCamera = (const AnyCameraPinhole&)(outputCamera);

			switch (input.channels())
			{
				case 1u:
					homographyWithCameraMask8BitPerChannel<1u>(inputPinholeCamera.actualCamera(), outputPinholeCamera.actualCamera(), input.constdata<uint8_t>(), input.paddingElements(), homography, output.data<uint8_t>(), outputMask.data<uint8_t>(), output.paddingElements(), outputMask.paddingElements(), worker, maskValue);
					return true;

				case 2u:
					homographyWithCameraMask8BitPerChannel<2u>(inputPinholeCamera.actualCamera(), outputPinholeCamera.actualCamera(), input.constdata<uint8_t>(), input.paddingElements(), homography, output.data<uint8_t>(), outputMask.data<uint8_t>(), output.paddingElements(), outputMask.paddingElements(), worker, maskValue);
					return true;

				case 3u:
					homographyWithCameraMask8BitPerChannel<3u>(inputPinholeCamera.actualCamera(), outputPinholeCamera.actualCamera(), input.constdata<uint8_t>(), input.paddingElements(), homography, output.data<uint8_t>(), outputMask.data<uint8_t>(), output.paddingElements(), outputMask.paddingElements(), worker, maskValue);
					return true;

				case 4u:
					homographyWithCameraMask8BitPerChannel<4u>(inputPinholeCamera.actualCamera(), outputPinholeCamera.actualCamera(), input.constdata<uint8_t>(), input.paddingElements(), homography, output.data<uint8_t>(), outputMask.data<uint8_t>(), output.paddingElements(), outputMask.paddingElements(), worker, maskValue);
					return true;

				default:
					// Nothing else to do - will fail below
					break;
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorBilinear::Comfort::lookup(const Frame& input, Frame& output, const LookupTable& lookupTable, const bool offset, const void* borderColor, Worker* worker)
{
	ocean_assert(!lookupTable.isEmpty() && input.isValid());

	if (input.isValid() && input.numberPlanes() == 1u)
	{
		if (input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
		{
			if (!output.set(FrameType(input, (unsigned int)(lookupTable.sizeX()), (unsigned int)(lookupTable.sizeY())), false /*forceOwner*/, true /*foreWritable*/))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			switch (input.channels())
			{
				case 1u:
					FrameInterpolatorBilinear::lookup<uint8_t, 1u>(input.constdata<uint8_t>(), input.width(), input.height(), lookupTable, offset, (const uint8_t*)(borderColor), output.data<uint8_t>(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 2u:
					FrameInterpolatorBilinear::lookup<uint8_t, 2u>(input.constdata<uint8_t>(), input.width(), input.height(), lookupTable, offset, (const uint8_t*)(borderColor), output.data<uint8_t>(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 3u:
					FrameInterpolatorBilinear::lookup<uint8_t, 3u>(input.constdata<uint8_t>(), input.width(), input.height(), lookupTable, offset, (const uint8_t*)(borderColor), output.data<uint8_t>(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 4u:
					FrameInterpolatorBilinear::lookup<uint8_t, 4u>(input.constdata<uint8_t>(), input.width(), input.height(), lookupTable, offset, (const uint8_t*)(borderColor), output.data<uint8_t>(), input.paddingElements(), output.paddingElements(), worker);
					return true;
			}
		}
		else if (input.dataType() == FrameType::DT_SIGNED_FLOAT_32)
		{
			if (!output.set(FrameType(input, (unsigned int)(lookupTable.sizeX()), (unsigned int)(lookupTable.sizeY())), false /*forceOwner*/, true /*foreWritable*/))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			ocean_assert(borderColor == nullptr || size_t(borderColor) % sizeof(float) == 0);

			switch (input.channels())
			{
				case 1u:
					FrameInterpolatorBilinear::lookup<float, 1u>(input.constdata<float>(), input.width(), input.height(), lookupTable, offset, (const float*)(borderColor), output.data<float>(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 2u:
					FrameInterpolatorBilinear::lookup<float, 2u>(input.constdata<float>(), input.width(), input.height(), lookupTable, offset, (const float*)(borderColor), output.data<float>(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 3u:
					FrameInterpolatorBilinear::lookup<float, 3u>(input.constdata<float>(), input.width(), input.height(), lookupTable, offset, (const float*)(borderColor), output.data<float>(), input.paddingElements(), output.paddingElements(), worker);
					return true;

				case 4u:
					FrameInterpolatorBilinear::lookup<float, 4u>(input.constdata<float>(), input.width(), input.height(), lookupTable, offset, (const float*)(borderColor), output.data<float>(), input.paddingElements(), output.paddingElements(), worker);
					return true;
			}
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorBilinear::Comfort::lookupMask(const Frame& input, Frame& output, Frame& outputMask, const LookupTable& lookupTable, const bool offset, Worker* worker, const uint8_t maskValue)
{
	ocean_assert(!lookupTable.isEmpty() && input);

	if (input.isValid() && input.numberPlanes() == 1u && input.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (!output.set(FrameType(input, (unsigned int)(lookupTable.sizeX()), (unsigned int)(lookupTable.sizeY())), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (!outputMask.set(FrameType(output, FrameType::FORMAT_Y8), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		switch (input.channels())
		{
			case 1u:
				lookupMask8BitPerChannel<1u>(input.constdata<uint8_t>(), input.width(), input.height(), lookupTable, offset, output.data<uint8_t>(), outputMask.data<uint8_t>(), input.paddingElements(), output.paddingElements(), outputMask.paddingElements(), worker, maskValue);
				return true;

			case 2u:
				lookupMask8BitPerChannel<2u>(input.constdata<uint8_t>(), input.width(), input.height(), lookupTable, offset, output.data<uint8_t>(), outputMask.data<uint8_t>(), input.paddingElements(), output.paddingElements(), outputMask.paddingElements(), worker, maskValue);
				return true;

			case 3u:
				lookupMask8BitPerChannel<3u>(input.constdata<uint8_t>(), input.width(), input.height(), lookupTable, offset, output.data<uint8_t>(), outputMask.data<uint8_t>(), input.paddingElements(), output.paddingElements(), outputMask.paddingElements(), worker, maskValue);
				return true;

			case 4u:
				lookupMask8BitPerChannel<4u>(input.constdata<uint8_t>(), input.width(), input.height(), lookupTable, offset, output.data<uint8_t>(), outputMask.data<uint8_t>(), input.paddingElements(), output.paddingElements(), outputMask.paddingElements(), worker, maskValue);
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorBilinear::Comfort::rotate(const Frame& source, Frame& target, const Scalar horizontalAnchorPosition, const Scalar verticalAnchorPosition, const Scalar angle, Worker* worker, const uint8_t* borderColor)
{
	ocean_assert(source);
	ocean_assert(!target.isValid() || source.frameType() == target.frameType());

	if (source.numberPlanes() == 1u && source.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		switch (source.channels())
		{
			case 1u:
				rotate8BitPerChannel<1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), horizontalAnchorPosition, verticalAnchorPosition, angle, source.paddingElements(), target.paddingElements(), worker, borderColor);
				return true;

			case 2u:
				rotate8BitPerChannel<2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), horizontalAnchorPosition, verticalAnchorPosition, angle, source.paddingElements(), target.paddingElements(), worker, borderColor);
				return true;

			case 3u:
				rotate8BitPerChannel<3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), horizontalAnchorPosition, verticalAnchorPosition, angle, source.paddingElements(), target.paddingElements(), worker, borderColor);
				return true;

			case 4u:
				rotate8BitPerChannel<4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), horizontalAnchorPosition, verticalAnchorPosition, angle, source.paddingElements(), target.paddingElements(), worker, borderColor);
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

bool FrameInterpolatorBilinear::Comfort::resampleCameraImage(const Frame& sourceFrame, const AnyCamera& sourceCamera, const SquareMatrix3& source_R_target, const AnyCamera& targetCamera, Frame& targetFrame, LookupCorner2<Vector2>* source_OLT_target, Worker* worker, const unsigned int binSizeInPixel, const void* borderColor)
{
	ocean_assert(sourceFrame.isValid());
	ocean_assert(sourceCamera.isValid());
	ocean_assert(sourceFrame.width() == sourceCamera.width() && sourceFrame.height() == sourceCamera.height());
	ocean_assert(source_R_target.isOrthonormal());
	ocean_assert(targetCamera.isValid());

	if (sourceFrame.pixelOrigin() != FrameType::ORIGIN_UPPER_LEFT)
	{
		ocean_assert(false && "Pixel origin must be top left");
		return false;
	}

	if (sourceFrame.numberPlanes() == 1u)
	{
		if (sourceFrame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
		{
			ocean_assert(borderColor == nullptr || size_t(borderColor) % sizeof(uint8_t) == 0);

			if (!targetFrame.set(FrameType(sourceFrame.frameType(), targetCamera.width(), targetCamera.height()), false /*forceOwner*/, true /*forceWritable*/))
			{
				return false;
			}

			switch (sourceFrame.channels())
			{
				case 1u:
					FrameInterpolatorBilinear::resampleCameraImage<uint8_t, 1u>(sourceFrame.constdata<uint8_t>(), sourceCamera, source_R_target, targetCamera, targetFrame.data<uint8_t>(), sourceFrame.paddingElements(), targetFrame.paddingElements(), source_OLT_target, worker, binSizeInPixel, (const uint8_t*)(borderColor));
					return true;

				case 2u:
					FrameInterpolatorBilinear::resampleCameraImage<uint8_t, 2u>(sourceFrame.constdata<uint8_t>(), sourceCamera, source_R_target, targetCamera, targetFrame.data<uint8_t>(), sourceFrame.paddingElements(), targetFrame.paddingElements(), source_OLT_target, worker, binSizeInPixel, (const uint8_t*)(borderColor));
					return true;

				case 3u:
					FrameInterpolatorBilinear::resampleCameraImage<uint8_t, 3u>(sourceFrame.constdata<uint8_t>(), sourceCamera, source_R_target, targetCamera, targetFrame.data<uint8_t>(), sourceFrame.paddingElements(), targetFrame.paddingElements(), source_OLT_target, worker, binSizeInPixel, (const uint8_t*)(borderColor));
					return true;

				case 4u:
					FrameInterpolatorBilinear::resampleCameraImage<uint8_t, 4u>(sourceFrame.constdata<uint8_t>(), sourceCamera, source_R_target, targetCamera, targetFrame.data<uint8_t>(), sourceFrame.paddingElements(), targetFrame.paddingElements(), source_OLT_target, worker, binSizeInPixel, (const uint8_t*)(borderColor));
					return true;
			}
		}
		else if (sourceFrame.dataType() == FrameType::DT_SIGNED_FLOAT_32)
		{
			ocean_assert(borderColor == nullptr || size_t(borderColor) % sizeof(float) == 0);

			if (!targetFrame.set(FrameType(sourceFrame.frameType(), targetCamera.width(), targetCamera.height()), false /*forceOwner*/, true /*forceWritable*/))
			{
				return false;
			}

			switch (sourceFrame.channels())
			{
				case 1u:
					FrameInterpolatorBilinear::resampleCameraImage<float, 1u>(sourceFrame.constdata<float>(), sourceCamera, source_R_target, targetCamera, targetFrame.data<float>(), sourceFrame.paddingElements(), targetFrame.paddingElements(), source_OLT_target, worker, binSizeInPixel, (const float*)(borderColor));
					return true;

				case 2u:
					FrameInterpolatorBilinear::resampleCameraImage<float, 2u>(sourceFrame.constdata<float>(), sourceCamera, source_R_target, targetCamera, targetFrame.data<float>(), sourceFrame.paddingElements(), targetFrame.paddingElements(), source_OLT_target, worker, binSizeInPixel, (const float*)(borderColor));
					return true;

				case 3u:
					FrameInterpolatorBilinear::resampleCameraImage<float, 3u>(sourceFrame.constdata<float>(), sourceCamera, source_R_target, targetCamera, targetFrame.data<float>(), sourceFrame.paddingElements(), targetFrame.paddingElements(), source_OLT_target, worker, binSizeInPixel, (const float*)(borderColor));
					return true;

				case 4u:
					FrameInterpolatorBilinear::resampleCameraImage<float, 4u>(sourceFrame.constdata<float>(), sourceCamera, source_R_target, targetCamera, targetFrame.data<float>(), sourceFrame.paddingElements(), targetFrame.paddingElements(), source_OLT_target, worker, binSizeInPixel, (const float*)(borderColor));
					return true;
			}
		}
	}

	ocean_assert(false && "Not supported pixel format!");
	return false;
}

void FrameInterpolatorBilinear::SpecialCases::resize400x400To224x224_8BitPerChannel(const uint8_t* const source, uint8_t* const target, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10 && defined(__aarch64__)

	// every 14 elements, the lookup locations and the interpolation factors repeat; 25 pixels will be resized to 14 pixels

	/*
	 * 0   1   2   3   4   5   6   7   8   9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24
	 * L       L   L       L       L       L        L    L         L         L         L         L    L         L
	 *     R       R   R       R       R       R         R    R         R         R         R         R    R         R
	 *
	 * ^                                   ^
	 * |                                   |
	 * sourceA                             sourceB
	 *                                     0   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
	 *
	 * identical lookup locations for top/bottom row
	 */

	constexpr uint8_t topRowOffsets[14] = {0u, 2u, 3u, 5u, 7u, 9u, 11u, 12u, 14u, 16u, 18u, 20u, 21u, 23u};

	constexpr uint8x16_t shuffleA_u_8x16 = {0u, 1u, 2u, 3u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 12u, 13u}; // [ 0L 0R 1L 1R ...
	constexpr uint8x16_t shuffleB_u_8x16 = {5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 12u, 13u, 14u, 15u, 255u, 255u, 255u, 255u}; // [ 8L 8R 9L 9R ... 13L 13R X X X X ]

	/*
	 *                  0    1    2    3    4    5    6    7    8    9    10    11    12    13
	 * factors left:   78  105    5   32   59   87  114   14   41   69    96   123    23    50
	 * factors right:  50   23  123   96   69   41   14  114   87   59    32     5   105    78   (128 - factors left), can also be mirrored
	 *
	 * identical factors for top/bottom
	 */

	constexpr uint8_t factorsTop[14] = {78u, 105u, 5u, 32u, 59u, 87u, 114u, 14u, 41u, 69u, 96u, 123u, 23u, 50u};

	constexpr uint8x8_t factorsLeftRightA_u_8x8 = {78u, 50u, 105u, 23u, 5u, 123u, 32u, 96u};
	constexpr uint8x8_t factorsLeftRightB_u_8x8 = {59u, 69u, 87u, 41u, 114u, 14u, 14u, 114u};
	constexpr uint8x8_t factorsLeftRightC_u_8x8 = {41u, 87u, 69u, 59u, 96u, 32u, 123u, 5u};
	constexpr uint8x8_t factorsLeftRightD_u_8x8 = {23u, 105u, 50u, 78u, 0u, 0u, 0u, 0u};


	const unsigned int sourceStrideElements = 400u + sourcePaddingElements;
	const unsigned int targetStrideElements = 224u + targetPaddingElements;

	// intermediate source row
	Memory intermediateSource(400);

	for (unsigned int yBlock = 0u; yBlock < 16u; ++yBlock)
	{
		for (unsigned int n = 0u; n < 14u; ++n)
		{
			const unsigned int ySourceTop = yBlock * 25u + topRowOffsets[n];
			const unsigned int yTarget = yBlock * 14u + n;

			ocean_assert(ySourceTop + 1u < 400u);
			ocean_assert(yTarget < 224u);

			const uint8_t* sourceRowTop = source + ySourceTop * sourceStrideElements;
			const uint8_t* sourceRowBottom = source + (ySourceTop + 1u) * sourceStrideElements;

			uint8_t* targetRow = target + yTarget * targetStrideElements;

			const unsigned int factorTop = factorsTop[n];

			const uint8x8_t factorsTop_u_8x8 = vdup_n_u8(uint8_t(factorTop));
			const uint8x8_t factorsBottom_u_8x8 = vdup_n_u8(uint8_t(128u - factorTop));

			// first, we interpolate the top and bottom row and store the result in an intermediate buffer

			uint8_t* intermediateRow = intermediateSource.data<uint8_t>();

			for (unsigned int xBlock = 0u; xBlock < 25u; ++xBlock)
			{
				const uint8x16_t sourceTop_u_8x16 = vld1q_u8(sourceRowTop);
				const uint8x16_t sourceBottom_u_8x16 = vld1q_u8(sourceRowBottom);

				uint16x8_t intermediateA_16x8 = vmull_u8(vget_low_u8(sourceTop_u_8x16), factorsTop_u_8x8);
				uint16x8_t intermediateB_16x8 = vmull_u8(vget_high_u8(sourceTop_u_8x16), factorsTop_u_8x8);

				intermediateA_16x8 = vmlal_u8(intermediateA_16x8, vget_low_u8(sourceBottom_u_8x16), factorsBottom_u_8x8);
				intermediateB_16x8 = vmlal_u8(intermediateB_16x8, vget_high_u8(sourceBottom_u_8x16), factorsBottom_u_8x8);

				// result = (intermediate_16x8 + 2^6) / 2^7
				const uint8x8_t resultA_u_8x8 = vqrshrn_n_u16(intermediateA_16x8, 7);
				const uint8x8_t resultB_u_8x8 = vqrshrn_n_u16(intermediateB_16x8, 7);

				const uint8x16_t result_u_8x16 = vcombine_u8(resultA_u_8x8, resultB_u_8x8);

				vst1q_u8(intermediateRow, result_u_8x16);

				sourceRowTop += 16;
				sourceRowBottom += 16;
				intermediateRow += 16;
			}

			// now, we interpolate the intermediate row horizontally

			intermediateRow = intermediateSource.data<uint8_t>();

			for (unsigned int xBlock = 0u; xBlock < 16u; ++xBlock)
			{
				const uint8x16_t sourceA_u_8x16 = vld1q_u8(intermediateRow + 0u);
				const uint8x16_t sourceB_u_8x16 = vld1q_u8(intermediateRow + 9u);

				const uint8x16_t sourceLeftRightA_u8x16 = vqtbl1q_u8(sourceA_u_8x16, shuffleA_u_8x16); // [L R L R L R L R L R L R L R L R]
				const uint8x16_t sourceLeftRightB_u8x16 = vqtbl1q_u8(sourceB_u_8x16, shuffleB_u_8x16);

				const uint16x8_t intermediateA_16x8 = vmull_u8(vget_low_u8(sourceLeftRightA_u8x16), factorsLeftRightA_u_8x8);
				const uint16x8_t intermediateB_16x8 = vmull_u8(vget_high_u8(sourceLeftRightA_u8x16), factorsLeftRightB_u_8x8);

				const uint16x8_t intermediateC_16x8 = vmull_u8(vget_low_u8(sourceLeftRightB_u8x16), factorsLeftRightC_u_8x8);
				const uint16x8_t intermediateD_16x8 = vmull_u8(vget_high_u8(sourceLeftRightB_u8x16), factorsLeftRightD_u_8x8);

				// adding adjacent entries, and round shifting by 7
				const uint8x8_t resultA_u_8x8 = vrshrn_n_u16(vpaddq_u16(intermediateA_16x8, intermediateB_16x8), 7);
				const uint8x8_t resultB_u_8x8 = vrshrn_n_u16(vpaddq_u16(intermediateC_16x8, intermediateD_16x8), 7);

				uint8x16_t result_u_8x16 = vcombine_u8(resultA_u_8x8, resultB_u_8x8);

				if (xBlock != 15u)
				{
					vst1q_u8(targetRow, result_u_8x16); // overwriting 16 elements, although 14 are valid
				}
				else
				{
					const uint8_t intermediate0 = *(targetRow - 2); // storing two bytes which we will overwrite with invalid data
					const uint8_t intermediate1 = *(targetRow - 1);

					result_u_8x16 = vextq_u8(result_u_8x16, result_u_8x16, 14); // shifting result_u_8x16 bytes

					vst1q_u8(targetRow - 2, result_u_8x16); // overwriting all valid 16 elements

					*(targetRow - 2) = intermediate0; // writing the valid data back again
					*(targetRow - 1) = intermediate1;
				}

				intermediateRow += 25;
				targetRow += 14;
			}
		}
	}

#else // OCEAN_HARDWARE_NEON_VERSION

	resize<uint8_t, 1u>(source, target, 400u, 400u, 224u, 224u, sourcePaddingElements, targetPaddingElements);

#endif // OCEAN_HARDWARE_NEON_VERSION

}

void FrameInterpolatorBilinear::SpecialCases::resize400x400To256x256_8BitPerChannel(const uint8_t* const source, uint8_t* const target, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10 && defined(__aarch64__)

	// every 16 elements, the lookup locations and the interpolation factors repeat; 25 pixels will be resized to 16 pixels

	/*
	 * 0   1   2   3   4   5   6   7   8   9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24
	 * L   L       L   L       L       L   L        L    L         L    L         L         L    L         L    L
	 *     R   R       R   R       R       R   R         R    R         R    R         R         R    R         R    R
	 *
	 * ^                                   ^
	 * |                                   |
	 * sourceA                             sourceB
	 *                                     0   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
	 *
	 * identical lookup locations for top/bottom row
	 */

	constexpr uint8_t topRowOffsets[16] = {0u, 1u, 3u, 4u, 6u, 8u, 9u, 11u, 12u, 14u, 15u, 17u, 19u, 20u, 22u, 23u};

	constexpr uint8x16_t shuffleA_u_8x16 = {0u, 1u, 1u, 2u, 3u, 4u, 4u, 5u, 6u, 7u, 8u, 9u, 9u, 10u, 11u, 12u}; // [ 0L 0R 1L 1R ...
	constexpr uint8x16_t shuffleB_u_8x16 = {3u, 4u, 5u, 6u, 6u, 7u, 8u, 9u, 10u, 11u, 11u, 12u, 13u, 14u, 14u, 15u}; // [ 8L 8R 9L 9R ...

	/*
	 *                  0    1    2    3    4    5    6    7    8    9    10    11    12    13    14    15
	 * factors left:   92   20   76    4   60  116   44  100   28   84    12    68   124    52   108    36
	 * factors right:  36  108   52  124   68   12   84   28  100   44   116    60     4    76    20    92  (128 - factors left), can also be mirrored
	 *
	 * identical factors for top/bottom
	 */

	constexpr uint8_t factorsTop[16] = {92u, 20u, 76u, 4u, 60u, 116u, 44u, 100u, 28u, 84u, 12u, 68u, 124u, 52u, 108u, 36u};

	constexpr uint8x8_t factorsLeftRightA_u_8x8 = {92u, 36u, 20u, 108u, 76u, 52u, 4u, 124u};
	constexpr uint8x8_t factorsLeftRightB_u_8x8 = {60u, 68u, 116u, 12u, 44u, 84u, 100u, 28u};
	constexpr uint8x8_t factorsLeftRightC_u_8x8 = {28u, 100u, 84u, 44u, 12u, 116u, 68u, 60u};
	constexpr uint8x8_t factorsLeftRightD_u_8x8 = {124u, 4u, 52u, 76u, 108u, 20u, 36u, 92u};


	const unsigned int sourceStrideElements = 400u + sourcePaddingElements;
	const unsigned int targetStrideElements = 256u + targetPaddingElements;

	// intermediate source row
	Memory intermediateSource(400);

	for (unsigned int yBlock = 0u; yBlock < 16u; ++yBlock)
	{
		for (unsigned int n = 0u; n < 16u; ++n)
		{
			const unsigned int ySourceTop = yBlock * 25u + topRowOffsets[n];
			const unsigned int yTarget = yBlock * 16u + n;

			ocean_assert(ySourceTop + 1u < 400u);
			ocean_assert(yTarget < 256u);

			const uint8_t* sourceRowTop = source + ySourceTop * sourceStrideElements;
			const uint8_t* sourceRowBottom = source + (ySourceTop + 1u) * sourceStrideElements;

			uint8_t* targetRow = target + yTarget * targetStrideElements;

			const unsigned int factorTop = factorsTop[n];

			const uint8x8_t factorsTop_u_8x8 = vdup_n_u8(uint8_t(factorTop));
			const uint8x8_t factorsBottom_u_8x8 = vdup_n_u8(uint8_t(128u - factorTop));

			// first, we interpolate the top and bottom row and store the result in an intermediate buffer

			uint8_t* intermediateRow = intermediateSource.data<uint8_t>();

			for (unsigned int xBlock = 0u; xBlock < 25u; ++xBlock)
			{
				const uint8x16_t sourceTop_u_8x16 = vld1q_u8(sourceRowTop);
				const uint8x16_t sourceBottom_u_8x16 = vld1q_u8(sourceRowBottom);

				uint16x8_t intermediateA_u_16x8 = vmull_u8(vget_low_u8(sourceTop_u_8x16), factorsTop_u_8x8);
				uint16x8_t intermediateB_u_16x8 = vmull_u8(vget_high_u8(sourceTop_u_8x16), factorsTop_u_8x8);

				intermediateA_u_16x8 = vmlal_u8(intermediateA_u_16x8, vget_low_u8(sourceBottom_u_8x16), factorsBottom_u_8x8);
				intermediateB_u_16x8 = vmlal_u8(intermediateB_u_16x8, vget_high_u8(sourceBottom_u_8x16), factorsBottom_u_8x8);

				// result = (intermediate_16x8 + 2^6) / 2^7
				const uint8x8_t resultA_u_8x8 = vqrshrn_n_u16(intermediateA_u_16x8, 7);
				const uint8x8_t resultB_u_8x8 = vqrshrn_n_u16(intermediateB_u_16x8, 7);

				const uint8x16_t result_u_8x16 = vcombine_u8(resultA_u_8x8, resultB_u_8x8);

				vst1q_u8(intermediateRow, result_u_8x16);

				sourceRowTop += 16;
				sourceRowBottom += 16;
				intermediateRow += 16;
			}

			// now, we interpolate the intermediate row horizontally

			intermediateRow = intermediateSource.data<uint8_t>();

			for (unsigned int xBlock = 0u; xBlock < 16u; ++xBlock)
			{
				const uint8x16_t sourceA_u_8x16 = vld1q_u8(intermediateRow + 0u);
				const uint8x16_t sourceB_u_8x16 = vld1q_u8(intermediateRow + 9u);

				const uint8x16_t sourceLeftRightA_u8x16 = vqtbl1q_u8(sourceA_u_8x16, shuffleA_u_8x16); // [L R L R L R L R L R L R L R L R]
				const uint8x16_t sourceLeftRightB_u8x16 = vqtbl1q_u8(sourceB_u_8x16, shuffleB_u_8x16);

				const uint16x8_t intermediateA_u_16x8 = vmull_u8(vget_low_u8(sourceLeftRightA_u8x16), factorsLeftRightA_u_8x8);
				const uint16x8_t intermediateB_u_16x8 = vmull_u8(vget_high_u8(sourceLeftRightA_u8x16), factorsLeftRightB_u_8x8);

				const uint16x8_t intermediateC_u_16x8 = vmull_u8(vget_low_u8(sourceLeftRightB_u8x16), factorsLeftRightC_u_8x8);
				const uint16x8_t intermediateD_u_16x8 = vmull_u8(vget_high_u8(sourceLeftRightB_u8x16), factorsLeftRightD_u_8x8);

				// adding adjacent entries, and round shifting by 7
				const uint8x8_t resultA_u_8x8 = vrshrn_n_u16(vpaddq_u16(intermediateA_u_16x8, intermediateB_u_16x8), 7);
				const uint8x8_t resultB_u_8x8 = vrshrn_n_u16(vpaddq_u16(intermediateC_u_16x8, intermediateD_u_16x8), 7);

				const uint8x16_t result_u_8x16 = vcombine_u8(resultA_u_8x8, resultB_u_8x8);

				vst1q_u8(targetRow, result_u_8x16);

				intermediateRow += 25;
				targetRow += 16;
			}
		}
	}

#elif defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

	constexpr uint8_t topRowOffsets[16] = {0u, 1u, 3u, 4u, 6u, 8u, 9u, 11u, 12u, 14u, 15u, 17u, 19u, 20u, 22u, 23u};

	const __m128i shuffleA_u_8x16 = _mm_setr_epi8(0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9, 9, 10, 11, 12); // [ 0L 0R 1L 1R ...
	const __m128i shuffleB_u_8x16 = _mm_setr_epi8(3, 4, 5, 6, 6, 7, 8, 9, 10, 11, 11, 12, 13, 14, 14, 15); // [ 8L 8R 9L 9R ...

	constexpr uint8_t factorsTop[16] = {92u, 20u, 76u, 4u, 60u, 116u, 44u, 100u, 28u, 84u, 12u, 68u, 124u, 52u, 108u, 36u};

	const __m128i  factorsLeftRightA_s_8x16 = _mm_setr_epi8(92, 36, 20, 108, 76, 52, 4, 124, 60, 68, 116, 12, 44, 84, 100, 28);
	const __m128i  factorsLeftRightB_s_8x16 = _mm_setr_epi8(28, 100, 84, 44, 12, 116, 68, 60, 124, 4, 52, 76, 108, 20, 36, 92);

	const __m128i constant_64_u_16x8 = _mm_set1_epi16(64);

	const unsigned int sourceStrideElements = 400u + sourcePaddingElements;
	const unsigned int targetStrideElements = 256u + targetPaddingElements;

	// intermediate source row
	Memory intermediateSource(400);

	for (unsigned int yBlock = 0u; yBlock < 16u; ++yBlock)
	{
		for (unsigned int n = 0u; n < 16u; ++n)
		{
			const unsigned int ySourceTop = yBlock * 25u + topRowOffsets[n];
			const unsigned int yTarget = yBlock * 16u + n;

			ocean_assert(ySourceTop + 1u < 400u);
			ocean_assert(yTarget < 256u);

			const uint8_t* sourceRowTop = source + ySourceTop * sourceStrideElements;
			const uint8_t* sourceRowBottom = source + (ySourceTop + 1u) * sourceStrideElements;

			uint8_t* targetRow = target + yTarget * targetStrideElements;

			const unsigned int factorTop = factorsTop[n];

			const __m128i factorsTop_s_16x8 = _mm_set1_epi16(int16_t(factorTop));
			const __m128i factorsBottom_s_16x8 = _mm_set1_epi16(int16_t(128u - factorTop));

			// first, we interpolate the top and bottom row and store the result in an intermediate buffer

			uint8_t* intermediateRow = intermediateSource.data<uint8_t>();

			for (unsigned int xBlock = 0u; xBlock < 25u; ++xBlock)
			{
				const __m128i sourceTop_u_8x16 = _mm_lddqu_si128((const __m128i*)(sourceRowTop));
				const __m128i sourceBottom_u_8x16 = _mm_lddqu_si128((const __m128i*)(sourceRowBottom));

				__m128i intermediateA_s_16x8 = _mm_mullo_epi16(_mm_unpacklo_epi8(sourceTop_u_8x16, _mm_setzero_si128()), factorsTop_s_16x8);
				__m128i intermediateB_s_16x8 = _mm_mullo_epi16(_mm_unpackhi_epi8(sourceTop_u_8x16, _mm_setzero_si128()), factorsTop_s_16x8);

				intermediateA_s_16x8 = _mm_add_epi16(intermediateA_s_16x8, _mm_mullo_epi16(_mm_unpacklo_epi8(sourceBottom_u_8x16, _mm_setzero_si128()), factorsBottom_s_16x8));
				intermediateB_s_16x8 = _mm_add_epi16(intermediateB_s_16x8, _mm_mullo_epi16(_mm_unpackhi_epi8(sourceBottom_u_8x16, _mm_setzero_si128()), factorsBottom_s_16x8));

				// result = (intermediate_16x8 + 2^6) / 2^7
				const __m128i resultA_u_16x8 = _mm_srli_epi16(_mm_add_epi16(intermediateA_s_16x8, constant_64_u_16x8), 7);
				const __m128i resultB_u_16x8 = _mm_srli_epi16(_mm_add_epi16(intermediateB_s_16x8, constant_64_u_16x8), 7);

				const __m128i result_u_8x16 = _mm_packus_epi16(resultA_u_16x8, resultB_u_16x8);

				_mm_storeu_si128((__m128i*)(intermediateRow), result_u_8x16);

				sourceRowTop += 16;
				sourceRowBottom += 16;
				intermediateRow += 16;
			}

			// now, we interpolate the intermediate row horizontally

			intermediateRow = intermediateSource.data<uint8_t>();

			for (unsigned int xBlock = 0u; xBlock < 16u; ++xBlock)
			{
				const __m128i sourceA_u_8x16 = _mm_lddqu_si128((const __m128i*)(intermediateRow + 0u));
				const __m128i sourceB_u_8x16 = _mm_lddqu_si128((const __m128i*)(intermediateRow + 9u));

				const __m128i sourceLeftRightA_u_8x16 = _mm_shuffle_epi8(sourceA_u_8x16, shuffleA_u_8x16); // [L R L R L R L R L R L R L R L R]
				const __m128i sourceLeftRightB_u_8x16 = _mm_shuffle_epi8(sourceB_u_8x16, shuffleB_u_8x16);

				const __m128i intermediateA_s_16x8 = _mm_maddubs_epi16(sourceLeftRightA_u_8x16, factorsLeftRightA_s_8x16); // [sourceLeft0 * factorLeft0 + sourceRight0 * factorRight0, sourceLeft1 * factorLeft1 + sourceRight1 * factorRight1, ... ]
				const __m128i intermediateB_s_16x8 = _mm_maddubs_epi16(sourceLeftRightB_u_8x16, factorsLeftRightB_s_8x16);

				// result = (intermediate_16x8 + 2^6) / 2^7
				const __m128i resultA_u_16x8 = _mm_srli_epi16(_mm_add_epi16(intermediateA_s_16x8, constant_64_u_16x8), 7);
				const __m128i resultB_u_16x8 = _mm_srli_epi16(_mm_add_epi16(intermediateB_s_16x8, constant_64_u_16x8), 7);

				const __m128i result_u_8x16 = _mm_packus_epi16(resultA_u_16x8, resultB_u_16x8);

				_mm_storeu_si128((__m128i*)(targetRow), result_u_8x16);

				intermediateRow += 25;
				targetRow += 16;
			}
		}
	}

#else // OCEAN_HARDWARE_NEON_VERSION

	resize<uint8_t, 1u>(source, target, 400u, 400u, 256u, 256u, sourcePaddingElements, targetPaddingElements);

#endif // OCEAN_HARDWARE_NEON_VERSION

}

#ifdef OCEAN_WE_KEEP_THIS_IMPLEMENTATION_AS_WE_NEED_THIS_TO_FOR_A_NEW_NEON_IMPLEMENTATION

void FrameInterpolatorBilinear::SpecialCases::resize400x400To256x256_8BitPerChannel(const uint8_t* const source, uint8_t* const target, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10 && defined(__aarch64__)

	// every 16 elements, the lookup locations and the interpolation factors repeat; 25 pixels will be resized to 16 pixels

	/*
	 * 0   1   2   3   4   5   6   7   8   9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24
	 * L   L       L   L       L       L   L        L    L         L    L         L         L    L         L    L
	 *     R   R       R   R       R       R   R         R    R         R    R         R         R    R         R    R
	 *
	 * ^                                   ^
	 * |                                   |
	 * sourceA                             sourceB
	 *                                     0   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
	 *
	 * identical lookup locations for top/bottom row
	 */

	constexpr uint8_t topRowOffsets[16] = {0u, 1u, 3u, 4u, 6u, 8u, 9u, 11u, 12u, 14u, 15u, 17u, 19u, 20u, 22u, 23u};

	constexpr uint8x16_t shuffleLeftA_u_8x16 = {16u, 16u, 16u, 16u, 16u, 0u, 1u, 3u, 4u, 6u, 8u, 9u, 11u, 12u, 14u, 15u};
	constexpr uint8x16_t shuffleLeftB_u_8x16 = {8u, 10u, 11u, 13u, 14u};

	constexpr uint8x16_t shuffleRightA_u_8x16 = {16u, 16u, 16u, 16u, 16u, 16u, 1u, 2u, 4u, 5u, 7u, 9u, 10u, 12u, 13u, 15u};
	constexpr uint8x16_t shuffleRightB_u_8x16 = {7u, 9u, 11u, 12u, 14u, 15u};

	/*
	 *                  0    1    2    3    4    5    6    7    8    9    10    11    12    13    14    15
	 * factors left:   92   20   76    4   60  116   44  100   28   84    12    68   124    52   108    36
	 * factors right:  36  108   52  124   68   12   84   28  100   44   116    60     4    76    20    92  (128 - factors left), can also be mirrored
	 *
	 * identical factors for top/bottom
	 */

	constexpr uint8_t factorsTop[16] = {92u, 20u, 76u, 4u, 60u, 116u, 44u, 100u, 28u, 84u, 12u, 68u, 124u, 52u, 108u, 36u};

	constexpr uint8x8_t factorsLeftA_u_8x8 = {92u, 20u, 76u, 4u, 60u, 116u, 44u, 100};
	constexpr uint8x8_t factorsLeftB_u_8x8 = {28u, 84u, 12u, 68u, 124u, 52u, 108u, 36u};

	constexpr uint8x8_t factorsRightA_u_8x8 = {36u, 108u, 52u, 124u, 68u, 12u, 84u, 28u};
	constexpr uint8x8_t factorsRightB_u_8x8 = {100u, 44u, 116u, 60u, 4u, 76u, 20u, 92u};


	const unsigned int sourceStrideElements = 400u + sourcePaddingElements;
	const unsigned int targetStrideElements = 256u + targetPaddingElements;

	for (unsigned int yBlock = 0u; yBlock < 16u; ++yBlock)
	{
		for (unsigned int n = 0u; n < 16u; ++n)
		{
			const unsigned int ySourceTop = yBlock * 25u + topRowOffsets[n];
			const unsigned int yTarget = yBlock * 16u + n;

			const uint8_t* sourceRowTop = source + ySourceTop * sourceStrideElements;
			const uint8_t* sourceRowBottom = source + (ySourceTop + 1u) * sourceStrideElements;

			uint8_t* targetRow = target + yTarget * targetStrideElements;

			const unsigned int factorTop = factorsTop[n];

			for (unsigned int xBlock = 0u; xBlock < 16u; ++xBlock)
			{
				const uint8x16_t sourceTopA_u_8x16 = vld1q_u8(sourceRowTop + 0u);
				const uint8x16_t sourceTopB_u_8x16 = vld1q_u8(sourceRowTop + 9u);

				const uint8x16_t sourceTopLeftA_u8x16 = vqtbl1q_u8(sourceTopA_u_8x16, shuffleLeftA_u_8x16); // [x x x x x L L L L L L L L L L L]
				const uint8x16_t sourceTopLeftB_u8x16 = vqtbl1q_u8(sourceTopB_u_8x16, shuffleLeftB_u_8x16); // [L L L L L x x x x x x x x x x x]

				const uint8x16_t sourceTopLeft_u_8x16 = vextq_u8(sourceTopLeftA_u8x16, sourceTopLeftB_u8x16, 5); // [L L L L L L L L L L L L L L L L]


				const uint8x16_t sourceTopRightA_u8x16 = vqtbl1q_u8(sourceTopA_u_8x16, shuffleRightA_u_8x16); // [x x x x x x R R R R R R R R R R]
				const uint8x16_t sourceTopRightB_u8x16 = vqtbl1q_u8(sourceTopB_u_8x16, shuffleRightB_u_8x16); // [R R R R R R x x x x x x x x x x]

				const uint8x16_t sourceTopRight_u_8x16 = vextq_u8(sourceTopRightA_u8x16, sourceTopRightB_u8x16, 6); // [R R R R R R R R R R R R R R R R]


				uint16x8_t intermediateTopA_16x8 = vmull_u8(vget_low_u8(sourceTopLeft_u_8x16), factorsLeftA_u_8x8);
				uint16x8_t intermediateTopB_16x8 = vmull_u8(vget_high_u8(sourceTopLeft_u_8x16), factorsLeftB_u_8x8);

				intermediateTopA_16x8 = vmlal_u8(intermediateTopA_16x8, vget_low_u8(sourceTopRight_u_8x16), factorsRightA_u_8x8);
				intermediateTopB_16x8 = vmlal_u8(intermediateTopB_16x8, vget_high_u8(sourceTopRight_u_8x16), factorsRightB_u_8x8);

				// result = (intermediate_16x8 + 2^6) / 2^7
				const uint8x8_t resultTopA_u_8x8 = vrshrn_n_u16(intermediateTopA_16x8, 7);
				const uint8x8_t resultTopB_u_8x8 = vrshrn_n_u16(intermediateTopB_16x8, 7);



				// identical calculation for bottom row

				const uint8x16_t sourceBottomA_u_8x16 = vld1q_u8(sourceRowBottom + 0u);
				const uint8x16_t sourceBottomB_u_8x16 = vld1q_u8(sourceRowBottom + 9u);

				const uint8x16_t sourceBottomLeftA_u8x16 = vqtbl1q_u8(sourceBottomA_u_8x16, shuffleLeftA_u_8x16); // [x x x x x L L L L L L L L L L L]
				const uint8x16_t sourceBottomLeftB_u8x16 = vqtbl1q_u8(sourceBottomB_u_8x16, shuffleLeftB_u_8x16); // [L L L L L x x x x x x x x x x x]

				const uint8x16_t sourceBottomLeft_u_8x16 = vextq_u8(sourceBottomLeftA_u8x16, sourceBottomLeftB_u8x16, 5); // [L L L L L L L L L L L L L L L L]


				const uint8x16_t sourceBottomRightA_u8x16 = vqtbl1q_u8(sourceBottomA_u_8x16, shuffleRightA_u_8x16); // [x x x x x x R R R R R R R R R R]
				const uint8x16_t sourceBottomRightB_u8x16 = vqtbl1q_u8(sourceBottomB_u_8x16, shuffleRightB_u_8x16); // [R R R R R R x x x x x x x x x x]

				const uint8x16_t sourceBottomRight_u_8x16 = vextq_u8(sourceBottomRightA_u8x16, sourceBottomRightB_u8x16, 6); // [R R R R R R R R R R R R R R R R]


				uint16x8_t intermediateBottomA_16x8 = vmull_u8(vget_low_u8(sourceBottomLeft_u_8x16), factorsLeftA_u_8x8);
				uint16x8_t intermediateBottomB_16x8 = vmull_u8(vget_high_u8(sourceBottomLeft_u_8x16), factorsLeftB_u_8x8);

				intermediateBottomA_16x8 = vmlal_u8(intermediateBottomA_16x8, vget_low_u8(sourceBottomRight_u_8x16), factorsRightA_u_8x8);
				intermediateBottomB_16x8 = vmlal_u8(intermediateBottomB_16x8, vget_high_u8(sourceBottomRight_u_8x16), factorsRightB_u_8x8);

				// result = (intermediate_16x8 + 2^6) / 2^7
				const uint8x8_t resultBottomA_u_8x8 = vrshrn_n_u16(intermediateBottomA_16x8, 7);
				const uint8x8_t resultBottomB_u_8x8 = vrshrn_n_u16(intermediateBottomB_16x8, 7);



				// now, we interpolate intermediate top and bottom rows

				const uint8x8_t factorsTop_u_8x8 = vdup_n_u8(uint8_t(factorTop));
				const uint8x8_t factorsBottom_u_8x8 = vdup_n_u8(uint8_t(128u - factorTop));

				uint16x8_t intermediateA_16x8 = vmull_u8(resultTopA_u_8x8, factorsTop_u_8x8);
				uint16x8_t intermediateB_16x8 = vmull_u8(resultTopB_u_8x8, factorsTop_u_8x8);

				intermediateA_16x8 = vmlal_u8(intermediateA_16x8, resultBottomA_u_8x8, factorsBottom_u_8x8);
				intermediateB_16x8 = vmlal_u8(intermediateB_16x8, resultBottomB_u_8x8, factorsBottom_u_8x8);

				// result = (intermediate_16x8 + 2^6) / 2^7
				const uint8x8_t resultA_u_8x8 = vqrshrn_n_u16(intermediateA_16x8, 7);
				const uint8x8_t resultB_u_8x8 = vqrshrn_n_u16(intermediateB_16x8, 7);

				const uint8x16_t result_u_8x16 = vcombine_u8(resultA_u_8x8, resultB_u_8x8);

				vst1q_u8(targetRow, result_u_8x16);

				sourceRowTop += 25;
				sourceRowBottom += 25;

				targetRow += 16;
			}
		}
	}

#else // OCEAN_HARDWARE_NEON_VERSION

	resize<uint8_t, 1u>(source, target, 400u, 400u, 256u, 256u, sourcePaddingElements, targetPaddingElements);

#endif // OCEAN_HARDWARE_NEON_VERSION

}

void FrameInterpolatorBilinear::SpecialCases::resize400x400To256x256_8BitPerChannel(const uint8_t* const source, uint8_t* const target, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements)
{
	ocean_assert(source != nullptr && target != nullptr);

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10 && defined(__aarch64__)

	// every 16 elements, the lookup locations and the interpolation factors repeat; 25 pixels will be resized to 16 pixels

	/*
	 * 0   1   2   3   4   5   6   7   8   9   10   11   12   13   14   15   16   17   18   19   20   21   22   23   24
	 * L   L       L   L       L       L   L        L    L         L    L         L         L    L         L    L
	 *     R   R       R   R       R       R   R         R    R         R    R         R         R    R         R    R
	 *
	 * ^                                   ^
	 * |                                   |
	 * sourceA                             sourceB
	 *                                     0   1    2    3    4    5    6    7    8    9    10   11   12   13   14   15
	 *
	 * identical lookup locations for top/bottom row
	 */

	constexpr uint8_t topRowOffsets[16] = {0u, 1u, 3u, 4u, 6u, 8u, 9u, 11u, 12u, 14u, 15u, 17u, 19u, 20u, 22u, 23u};

	constexpr uint8x16_t shuffleA_u_8x16 = {0u, 1u, 1u, 2u, 3u, 4u, 4u, 5u, 6u, 7u, 8u, 9u, 9u, 10u, 11u, 12u}; // [ 0L 0R 1L 1R ...
	constexpr uint8x16_t shuffleB_u_8x16 = {3u, 4u, 5u, 6u, 6u, 7u, 8u, 9u, 10u, 11u, 11u, 12u, 13u, 14u, 14u, 15u}; // [ 8L 8R 9L 9R ...

	/*
	 *                  0    1    2    3    4    5    6    7    8    9    10    11    12    13    14    15
	 * factors left:   92   20   76    4   60  116   44  100   28   84    12    68   124    52   108    36
	 * factors right:  36  108   52  124   68   12   84   28  100   44   116    60     4    76    20    92  (128 - factors left), can also be mirrored
	 *
	 * identical factors for top/bottom
	 */

	constexpr uint8_t factorsTop[16] = {92u, 20u, 76u, 4u, 60u, 116u, 44u, 100u, 28u, 84u, 12u, 68u, 124u, 52u, 108u, 36u};

	constexpr uint8x8_t factorsLeftRightA_u_8x8 = {92u, 36u, 20u, 108u, 76u, 52u, 4u, 124u};
	constexpr uint8x8_t factorsLeftRightB_u_8x8 = {60u, 68u, 116u, 12u, 44u, 84u, 100u, 28u};
	constexpr uint8x8_t factorsLeftRightC_u_8x8 = {28u, 100u, 84u, 44u, 12u, 116u, 68u, 60u};
	constexpr uint8x8_t factorsLeftRightD_u_8x8 = {124u, 4u, 52u, 76u, 108u, 20u, 36u, 92u};


	const unsigned int sourceStrideElements = 400u + sourcePaddingElements;
	const unsigned int targetStrideElements = 256u + targetPaddingElements;

	for (unsigned int yBlock = 0u; yBlock < 16u; ++yBlock)
	{
		for (unsigned int n = 0u; n < 16u; ++n)
		{
			const unsigned int ySourceTop = yBlock * 25u + topRowOffsets[n];
			const unsigned int yTarget = yBlock * 16u + n;

			const uint8_t* sourceRowTop = source + ySourceTop * sourceStrideElements;
			const uint8_t* sourceRowBottom = source + (ySourceTop + 1u) * sourceStrideElements;

			uint8_t* targetRow = target + yTarget * targetStrideElements;

			const unsigned int factorTop = factorsTop[n];

			const uint8x8_t factorsTop_u_8x8 = vdup_n_u8(uint8_t(factorTop));
			const uint8x8_t factorsBottom_u_8x8 = vdup_n_u8(uint8_t(128u - factorTop));

			for (unsigned int xBlock = 0u; xBlock < 16u; ++xBlock)
			{
				// top row

				const uint8x16_t sourceTopA_u_8x16 = vld1q_u8(sourceRowTop + 0u);
				const uint8x16_t sourceTopB_u_8x16 = vld1q_u8(sourceRowTop + 9u);

				const uint8x16_t sourceTopLeftRightA_u8x16 = vqtbl1q_u8(sourceTopA_u_8x16, shuffleA_u_8x16); // [L R L R L R L R L R L R L R L R]
				const uint8x16_t sourceTopLeftRightB_u8x16 = vqtbl1q_u8(sourceTopB_u_8x16, shuffleB_u_8x16);

				const uint16x8_t intermediateTopA_16x8 = vmull_u8(vget_low_u8(sourceTopLeftRightA_u8x16), factorsLeftRightA_u_8x8);
				const uint16x8_t intermediateTopB_16x8 = vmull_u8(vget_high_u8(sourceTopLeftRightA_u8x16), factorsLeftRightB_u_8x8);

				const uint16x8_t intermediateTopC_16x8 = vmull_u8(vget_low_u8(sourceTopLeftRightB_u8x16), factorsLeftRightC_u_8x8);
				const uint16x8_t intermediateTopD_16x8 = vmull_u8(vget_high_u8(sourceTopLeftRightB_u8x16), factorsLeftRightD_u_8x8);

				// adding adjacent entries, and round shifting by 7
				const uint8x8_t resultTopA_u_8x8 = vrshrn_n_u16(vpaddq_u16(intermediateTopA_16x8, intermediateTopB_16x8), 7);
				const uint8x8_t resultTopB_u_8x8 = vrshrn_n_u16(vpaddq_u16(intermediateTopC_16x8, intermediateTopD_16x8), 7);


				// bottom row

				const uint8x16_t sourceBottomA_u_8x16 = vld1q_u8(sourceRowBottom + 0u);
				const uint8x16_t sourceBottomB_u_8x16 = vld1q_u8(sourceRowBottom + 9u);

				const uint8x16_t sourceBottomLeftRightA_u8x16 = vqtbl1q_u8(sourceBottomA_u_8x16, shuffleA_u_8x16); // [L R L R L R L R L R L R L R L R]
				const uint8x16_t sourceBottomLeftRightB_u8x16 = vqtbl1q_u8(sourceBottomB_u_8x16, shuffleB_u_8x16);

				const uint16x8_t intermediateBottomA_16x8 = vmull_u8(vget_low_u8(sourceBottomLeftRightA_u8x16), factorsLeftRightA_u_8x8);
				const uint16x8_t intermediateBottomB_16x8 = vmull_u8(vget_high_u8(sourceBottomLeftRightA_u8x16), factorsLeftRightB_u_8x8);

				const uint16x8_t intermediateBottomC_16x8 = vmull_u8(vget_low_u8(sourceBottomLeftRightB_u8x16), factorsLeftRightC_u_8x8);
				const uint16x8_t intermediateBottomD_16x8 = vmull_u8(vget_high_u8(sourceBottomLeftRightB_u8x16), factorsLeftRightD_u_8x8);

				// adding adjacent entries, and round shifting by 7
				const uint8x8_t resultBottomA_u_8x8 = vrshrn_n_u16(vpaddq_u16(intermediateBottomA_16x8, intermediateBottomB_16x8), 7);
				const uint8x8_t resultBottomB_u_8x8 = vrshrn_n_u16(vpaddq_u16(intermediateBottomC_16x8, intermediateBottomD_16x8), 7);



				// now, we interpolate intermediate top and bottom rows

				uint16x8_t intermediateA_16x8 = vmull_u8(resultTopA_u_8x8, factorsTop_u_8x8);
				uint16x8_t intermediateB_16x8 = vmull_u8(resultTopB_u_8x8, factorsTop_u_8x8);

				intermediateA_16x8 = vmlal_u8(intermediateA_16x8, resultBottomA_u_8x8, factorsBottom_u_8x8);
				intermediateB_16x8 = vmlal_u8(intermediateB_16x8, resultBottomB_u_8x8, factorsBottom_u_8x8);

				// result = (intermediate_16x8 + 2^6) / 2^7
				const uint8x8_t resultA_u_8x8 = vqrshrn_n_u16(intermediateA_16x8, 7);
				const uint8x8_t resultB_u_8x8 = vqrshrn_n_u16(intermediateB_16x8, 7);

				const uint8x16_t result_u_8x16 = vcombine_u8(resultA_u_8x8, resultB_u_8x8);

				vst1q_u8(targetRow, result_u_8x16);

				sourceRowTop += 25;
				sourceRowBottom += 25;

				targetRow += 16;
			}
		}
	}

#else // OCEAN_HARDWARE_NEON_VERSION

	resize<uint8_t, 1u>(source, target, 400u, 400u, 256u, 256u, sourcePaddingElements, targetPaddingElements);

#endif // OCEAN_HARDWARE_NEON_VERSION

}

#endif // OCEAN_WE_KEEP_THIS_IMPLEMENTATION_AS_WE_NEED_THIS_TO_FOR_A_NEW_NEON_IMPLEMENTATION

Scalar FrameInterpolatorBilinear::patchIntensitySum1Channel(const uint32_t* linedIntegralFrame, const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int lineIntegralFramePaddingElements, const Vector2& center, const PixelCenter pixelCenter, const unsigned int patchWidth, const unsigned int patchHeight)
{
	ocean_assert(linedIntegralFrame != nullptr);

	ocean_assert(frameWidth > 0u && frameHeight > 0u);
	ocean_assert(patchWidth > 0u && patchHeight > 0u);

	const unsigned int lineIntegralFrameStrideElements = (frameWidth + 1u) + lineIntegralFramePaddingElements;

	const Scalar pixelCenterOffset = pixelCenter == PC_TOP_LEFT ? Scalar(0.5) : Scalar(0.0);

	const Scalar patchLeft = center.x() + pixelCenterOffset - Scalar(patchWidth) * Scalar(0.5);
	const Scalar patchTop = center.y() + pixelCenterOffset - Scalar(patchHeight) * Scalar(0.5);
	ocean_assert(patchLeft >= Scalar(0) && patchLeft + Scalar(patchWidth) < Scalar(frameWidth));
	ocean_assert_and_suppress_unused(patchTop >= Scalar(0) && patchTop + Scalar(patchHeight) < Scalar(frameHeight), frameHeight);

	const unsigned int pixelPatchLeft = (unsigned int)(patchLeft);
	const unsigned int pixelPatchTop = (unsigned int)(patchTop);

	const Scalar factorRight = patchLeft - Scalar(pixelPatchLeft);
	const Scalar factorBottom = patchTop - Scalar(pixelPatchTop);

	ocean_assert(factorRight >= Scalar(0.0) && factorRight <= Scalar(1.0));
	ocean_assert(factorBottom >= Scalar(0.0) && factorBottom <= Scalar(1.0));

	const Scalar factorLeft = Scalar(1) - factorRight;
	const Scalar factorTop = Scalar(1) - factorBottom;

	const Scalar factorTopLeft = factorTop * factorLeft;
	ocean_assert(factorTopLeft >= Scalar(0.0) && factorTopLeft <= Scalar(1.0));

	const uint32_t intensityTopLeft = CV::IntegralImage::linedIntegralSum<uint32_t>(linedIntegralFrame, lineIntegralFrameStrideElements, pixelPatchLeft, pixelPatchTop, patchWidth, patchHeight);

	if (Numeric::isEqual(factorTopLeft, Scalar(1)))
	{
		return Scalar(intensityTopLeft);
	}

	const Scalar factorTopRight = factorTop * factorRight;
	const Scalar factorBottomLeft = factorBottom * factorLeft;
	const Scalar factorBottomRight = factorBottom * factorRight;

	const uint32_t intensityTopRight = Numeric::isEqual(factorTopRight, Scalar(0)) ? 0u : CV::IntegralImage::linedIntegralSum<uint32_t>(linedIntegralFrame, lineIntegralFrameStrideElements, pixelPatchLeft + 1u, pixelPatchTop, patchWidth, patchHeight);
	const uint32_t intensityBottomLeft = Numeric::isEqual(factorBottomLeft, Scalar(0)) ? 0u : CV::IntegralImage::linedIntegralSum<uint32_t>(linedIntegralFrame, lineIntegralFrameStrideElements, pixelPatchLeft, pixelPatchTop + 1u, patchWidth, patchHeight);
	const uint32_t intensityBottomRight = Numeric::isEqual(factorBottomRight, Scalar(0)) ? 0u : CV::IntegralImage::linedIntegralSum<uint32_t>(linedIntegralFrame, lineIntegralFrameStrideElements, pixelPatchLeft + 1u, pixelPatchTop + 1u, patchWidth, patchHeight);

	return factorTopLeft * Scalar(intensityTopLeft) + factorTopRight * Scalar(intensityTopRight) + factorBottomLeft * Scalar(intensityBottomLeft) + factorBottomRight * Scalar(intensityBottomRight);
}

bool FrameInterpolatorBilinear::coversHomographyInputFrame(const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int outputWidth, const unsigned int outputHeight, const SquareMatrix3& input_H_output, const int outputOriginX, const int outputOriginY)
{
	ocean_assert(inputWidth >= 1u && inputHeight >= 1u);
	ocean_assert(outputWidth >= 1u && outputHeight >= 1u);
	ocean_assert(input_H_output.isHomography());

	const Scalar scalarInputWidth_1 = Scalar(inputWidth - 1u);
	const Scalar scalarInputHeight_1 = Scalar(inputHeight - 1u);

	const Vector2 inputPosition0(input_H_output * Vector2(Scalar(outputOriginX), Scalar(outputOriginY)));
	const Vector2 inputPosition1(input_H_output * Vector2(Scalar(outputOriginX + int(outputWidth) - 1), Scalar(outputOriginY)));
	const Vector2 inputPosition2(input_H_output * Vector2(Scalar(outputOriginX), Scalar(outputOriginY + int(outputHeight) - 1)));
	const Vector2 inputPosition3(input_H_output * Vector2(Scalar(outputOriginX + int(outputWidth) - 1), Scalar(outputOriginY + int(outputHeight) - 1)));

	return inputPosition0.x() >= Scalar(0) && inputPosition0.x() <= scalarInputWidth_1 && inputPosition0.y() >= Scalar(0) && inputPosition0.y() <= scalarInputHeight_1
				&& inputPosition1.x() >= Scalar(0) && inputPosition1.x() <= scalarInputWidth_1 && inputPosition1.y() >= Scalar(0) && inputPosition1.y() <= scalarInputHeight_1
				&& inputPosition2.x() >= Scalar(0) && inputPosition2.x() <= scalarInputWidth_1 && inputPosition2.y() >= Scalar(0) && inputPosition2.y() <= scalarInputHeight_1
				&& inputPosition3.x() >= Scalar(0) && inputPosition3.x() <= scalarInputWidth_1 && inputPosition3.y() >= Scalar(0) && inputPosition3.y() <= scalarInputHeight_1;
}

void FrameInterpolatorBilinear::interpolateRowHorizontal8BitPerChannel7BitPrecision(const uint8_t* extendedSourceRow, uint8_t* targetRow, const unsigned int targetWidth, const unsigned int channels, const unsigned int* interpolationLocations, const uint8_t* interpolationFactors)
{
	ocean_assert(extendedSourceRow != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(targetWidth >= 8u);
	ocean_assert(interpolationLocations != nullptr);
	ocean_assert(interpolationFactors != nullptr);

	for (unsigned int x = 0u; x < targetWidth; ++x)
	{
		// interpolationLocations are defined in elements, not in pixels (pixel position * channels)
		const unsigned int& leftLocation = *interpolationLocations;
		const unsigned int rightLocation = leftLocation + channels;

		const uint8_t& leftFactor = interpolationFactors[0];
		const uint8_t& rightFactor = interpolationFactors[1];

		const uint8_t* const leftSource = extendedSourceRow + leftLocation;
		const uint8_t* const rightSource = extendedSourceRow + rightLocation;

		for (unsigned int n = 0u; n < channels; ++n)
		{
			targetRow[n] = (uint8_t)((leftSource[n] * leftFactor + rightSource[n] * rightFactor + 64u) / 128u);
		}

		targetRow += channels;
		interpolationLocations++;
		interpolationFactors += 2u;
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

void FrameInterpolatorBilinear::interpolateRowVertical8BitPerChannel7BitPrecisionNEON(const uint8_t* sourceRowTop, const uint8_t* sourceRowBottom, uint8_t* targetRow, const unsigned int elements, const unsigned int factorBottom)
{
	ocean_assert(sourceRowTop != nullptr);
	ocean_assert(sourceRowBottom != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(elements >= 16u);

	ocean_assert(factorBottom <= 128u);

	// [128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u]
	const uint8x8_t constant_128_u_8x8 = vdup_n_u8(128u);

	const uint8x8_t factorsBottom_u_8x8 = vdup_n_u8(uint8_t(factorBottom));
	const uint8x8_t factorsTop_u_8x8 = vsub_u8(constant_128_u_8x8, factorsBottom_u_8x8); // factorTop = 128 - factorBottom

	for (unsigned int n = 0u; n < elements; n += 16u)
	{
		if (n + 16u > elements)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some elements (at most 15) and we will calculate some elements again

			ocean_assert(n >= 16u && elements > 16u);
			const unsigned int offset = n - (elements - 16u);
			ocean_assert(offset < 16u);

			sourceRowTop -= offset;
			sourceRowBottom -= offset;
			targetRow -= offset;

			// the for loop will stop after this iteration
			ocean_assert(!(n + 16u < elements));
		}

		// loading the next eight 8 bit values from the top and bottom row
		const uint8x16_t top_8x16 = vld1q_u8(sourceRowTop);
		const uint8x16_t bottom_8x16 = vld1q_u8(sourceRowBottom);

		// interpolatedRow_16x8 = top_8x8 * factorsTop + bottom_8x8 * factorsBottom
		uint16x8_t interpolatedRow_0_16x8 = vmull_u8(vget_low_u8(top_8x16), factorsTop_u_8x8);
		uint16x8_t interpolatedRow_1_16x8 = vmull_u8(vget_high_u8(top_8x16), factorsTop_u_8x8);

		interpolatedRow_0_16x8 = vmlal_u8(interpolatedRow_0_16x8, vget_low_u8(bottom_8x16), factorsBottom_u_8x8);
		interpolatedRow_1_16x8 = vmlal_u8(interpolatedRow_1_16x8, vget_high_u8(bottom_8x16), factorsBottom_u_8x8);

		// interpolatedRow_8x8 = (interpolatedRow_16x8 + 2^6) / 2^7
		const uint8x8_t interpolatedRow_0_8x8 = vqrshrn_n_u16(interpolatedRow_0_16x8, 7);
		const uint8x8_t interpolatedRow_1_8x8 = vqrshrn_n_u16(interpolatedRow_1_16x8, 7);

		const uint8x16_t interpolatedRow_8x16 = vcombine_u8(interpolatedRow_0_8x8, interpolatedRow_1_8x8);

		// writing back the eight interpolated 8 bit results
		vst1q_u8(targetRow, interpolatedRow_8x16);

		sourceRowTop += 16;
		sourceRowBottom += 16;
		targetRow += 16;
	}
}

template <>
inline void FrameInterpolatorBilinear::interpolateRowHorizontal8BitPerChannel7BitPrecisionNEON<1u>(const uint8_t* extendedSourceRow, uint8_t* targetRow, const unsigned int targetWidth, const unsigned int channels, const unsigned int* interpolationLocations, const uint8_t* interpolationFactors)
{
	ocean_assert(extendedSourceRow != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(targetWidth >= 8u);
	ocean_assert(interpolationLocations != nullptr);
	ocean_assert(interpolationFactors != nullptr);

	ocean_assert(channels == 1u);

	struct LeftRightPixel
	{
		uint8_t left;
		uint8_t right;
	};

	static_assert(sizeof(LeftRightPixel) == 2, "Invalid data type!");

	uint8_t pixels[16];

	for (unsigned int x = 0; x < targetWidth; x += 8u)
	{
		if (x + 8u > targetWidth)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && targetWidth > 8u);
			const unsigned int newX = targetWidth - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			targetRow -= offset;
			interpolationLocations -= offset;
			interpolationFactors -= offset * 2u;

			x = newX;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < targetWidth));
		}

		// we load the left and the right pixels into an intermediate buffer
		// with following pattern (with left L, and right R):
		//  7 6 5 4 3 2 1 0     7 6 5 4 3 2 1 0
		// [R L R L R L R L],  [R L R L R L R L]

		for (unsigned int n = 0u; n < 4u; ++n)
		{
			((LeftRightPixel*)pixels)[0u + n] = *(LeftRightPixel*)(extendedSourceRow + interpolationLocations[0u + n]);
			((LeftRightPixel*)pixels)[4u + n] = *(LeftRightPixel*)(extendedSourceRow + interpolationLocations[4u + n]);
		}

		const uint8x16_t pixels_u_8x16 = vld1q_u8(pixels);
		const uint8x8_t pixels_0_u_8x8 = vget_low_u8(pixels_u_8x16);
		const uint8x8_t pixels_1_u_8x8 = vget_high_u8(pixels_u_8x16);

#if 0   // different strategy to load the data, with slightly different layout, but seems to be slower on most devices...
		pixels_8x8x2 = vld2_lane_u8(extendedSourceRow + interpolationLocations[0], pixels_8x8x2, 0);
		pixels_8x8x2 = vld2_lane_u8(extendedSourceRow + interpolationLocations[1], pixels_8x8x2, 1);

		pixels_8x8x2 = vld2_lane_u8(extendedSourceRow + interpolationLocations[2], pixels_8x8x2, 2);
		pixels_8x8x2 = vld2_lane_u8(extendedSourceRow + interpolationLocations[3], pixels_8x8x2, 3);

		pixels_8x8x2 = vld2_lane_u8(extendedSourceRow + interpolationLocations[4], pixels_8x8x2, 4);
		pixels_8x8x2 = vld2_lane_u8(extendedSourceRow + interpolationLocations[5], pixels_8x8x2, 5);

		pixels_8x8x2 = vld2_lane_u8(extendedSourceRow + interpolationLocations[6], pixels_8x8x2, 6);
		pixels_8x8x2 = vld2_lane_u8(extendedSourceRow + interpolationLocations[7], pixels_8x8x2, 7);
#endif


		// we load the pre-computed interpolation vectors for left and right pixels (accuracy 7 bits)
		const uint8x16_t factors_u_16x8 = vld1q_u8(interpolationFactors);


		// we multiply each pixel with each factor and sum adjacent pixels
		const uint16x8_t multiplication_0_u_16x8 = vmull_u8(pixels_0_u_8x8, vget_low_u8(factors_u_16x8));
		const uint16x8_t multiplication_1_u_16x8 = vmull_u8(pixels_1_u_8x8, vget_high_u8(factors_u_16x8));

		const uint16x4_t multiplication_0_u_16x4 = vpadd_u16(vget_low_u16(multiplication_0_u_16x8), vget_high_u16(multiplication_0_u_16x8));
		const uint16x4_t multiplication_1_u_16x4 = vpadd_u16(vget_low_u16(multiplication_1_u_16x8), vget_high_u16(multiplication_1_u_16x8));

		// we normalize the interpolation results by 128 (rounded right shift with 7 bits)
		const uint8x8_t result_u_8x8 = vrshrn_n_u16(vcombine_u16(multiplication_0_u_16x4, multiplication_1_u_16x4), 7);


		// we write back the results
		vst1_u8(targetRow, result_u_8x8);


		targetRow += 8;
		interpolationLocations += 8;
		interpolationFactors += 16;
	}
}

template <>
inline void FrameInterpolatorBilinear::interpolateRowHorizontal8BitPerChannel7BitPrecisionNEON<4u>(const uint8_t* extendedSourceRow, uint8_t* targetRow, const unsigned int targetWidth, const unsigned int channels, const unsigned int* interpolationLocations, const uint8_t* interpolationFactors)
{
	ocean_assert(extendedSourceRow != nullptr);
	ocean_assert(targetRow != nullptr);
	ocean_assert(targetWidth >= 8u);
	ocean_assert(interpolationLocations != nullptr);
	ocean_assert(interpolationFactors != nullptr);

	ocean_assert(channels == 4u);

	const uint8x8_t mask_02_8x8 = {0, 0, 0, 0, 2, 2, 2, 2};
	const uint8x8_t mask_13_8x8 = {1, 1, 1, 1, 3, 3, 3, 3};
	const uint8x8_t mask_46_8x8 = {4, 4, 4, 4, 6, 6, 6, 6};
	const uint8x8_t mask_57_8x8 = {5, 5, 5, 5, 7, 7, 7, 7};

	for (unsigned int x = 0; x < targetWidth; x += 8u)
	{
		if (x + 8u > targetWidth)
		{
			// the last iteration will not fit into the output frame,
			// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

			ocean_assert(x >= 8u && targetWidth > 8u);
			const unsigned int newX = targetWidth - 8u;

			ocean_assert(x > newX);
			const unsigned int offset = x - newX;

			targetRow -= offset * 4u;
			interpolationLocations -= offset;
			interpolationFactors -= offset * 2u;

			x = newX;

			// the for loop will stop after this iteration
			ocean_assert(!(x + 8u < targetWidth));
		}

		// we load the pre-computed interpolation vectors for left and right pixels (accuracy 7 bits)
		const uint8x16_t factors_u_8x16 = vld1q_u8(interpolationFactors);
		uint8x8_t factors_u_8x8 = vget_low_u8(factors_u_8x16);


		// we handle the first 4 pixels

		const uint8x8_t pixel_0_8x8 = vld1_u8(extendedSourceRow + interpolationLocations[0]); // left, right
		const uint8x8_t pixel_1_8x8 = vld1_u8(extendedSourceRow + interpolationLocations[1]); // left, right
		const uint8x8_t pixel_2_8x8 = vld1_u8(extendedSourceRow + interpolationLocations[2]); // left, right
		const uint8x8_t pixel_3_8x8 = vld1_u8(extendedSourceRow + interpolationLocations[3]); // left, right

		const uint32x2x2_t pixel_01_32x2x2 = vtrn_u32(vreinterpret_u32_u8(pixel_0_8x8), vreinterpret_u32_u8(pixel_1_8x8)); // [left, left], [right, right]
		const uint32x2x2_t pixel_23_32x2x2 = vtrn_u32(vreinterpret_u32_u8(pixel_2_8x8), vreinterpret_u32_u8(pixel_3_8x8)); // [left, left], [right, right]

		const uint8x8_t factors_01_left_u_8x8 = vtbl1_u8(factors_u_8x8, mask_02_8x8);
		const uint8x8_t factors_01_right_u_8x8 = vtbl1_u8(factors_u_8x8, mask_13_8x8);
		const uint8x8_t factors_23_left_u_8x8 = vtbl1_u8(factors_u_8x8, mask_46_8x8);
		const uint8x8_t factors_23_right_u_8x8 = vtbl1_u8(factors_u_8x8, mask_57_8x8);

		uint16x8_t multiplication_01_u_16x8 = vmull_u8(vreinterpret_u8_u32(pixel_01_32x2x2.val[0]), factors_01_left_u_8x8);
		uint16x8_t multiplication_23_u_16x8 = vmull_u8(vreinterpret_u8_u32(pixel_23_32x2x2.val[0]), factors_23_left_u_8x8);

		multiplication_01_u_16x8 = vmlal_u8(multiplication_01_u_16x8, vreinterpret_u8_u32(pixel_01_32x2x2.val[1]), factors_01_right_u_8x8);
		multiplication_23_u_16x8 = vmlal_u8(multiplication_23_u_16x8, vreinterpret_u8_u32(pixel_23_32x2x2.val[1]), factors_23_right_u_8x8);

		vst1q_u8(targetRow, vcombine_u8(vrshrn_n_u16(multiplication_01_u_16x8, 7), vrshrn_n_u16(multiplication_23_u_16x8, 7)));
		targetRow += 16;



		// we handle the second 4 pixels

		factors_u_8x8 = vget_high_u8(factors_u_8x16);

		const uint8x8_t pixel_4_8x8 = vld1_u8(extendedSourceRow + interpolationLocations[4]); // left, right
		const uint8x8_t pixel_5_8x8 = vld1_u8(extendedSourceRow + interpolationLocations[5]); // left, right
		const uint8x8_t pixel_6_8x8 = vld1_u8(extendedSourceRow + interpolationLocations[6]); // left, right
		const uint8x8_t pixel_7_8x8 = vld1_u8(extendedSourceRow + interpolationLocations[7]); // left, right

		const uint32x2x2_t pixel_45_32x2x2 = vtrn_u32(vreinterpret_u32_u8(pixel_4_8x8), vreinterpret_u32_u8(pixel_5_8x8)); // [left, left], [right, right]
		const uint32x2x2_t pixel_67_32x2x2 = vtrn_u32(vreinterpret_u32_u8(pixel_6_8x8), vreinterpret_u32_u8(pixel_7_8x8)); // [left, left], [right, right]

		const uint8x8_t factors_45_left_u_8x8 = vtbl1_u8(factors_u_8x8, mask_02_8x8);
		const uint8x8_t factors_45_right_u_8x8 = vtbl1_u8(factors_u_8x8, mask_13_8x8);
		const uint8x8_t factors_67_left_u_8x8 = vtbl1_u8(factors_u_8x8, mask_46_8x8);
		const uint8x8_t factors_67_right_u_8x8 = vtbl1_u8(factors_u_8x8, mask_57_8x8);

		uint16x8_t multiplication_45_u_16x8 = vmull_u8(vreinterpret_u8_u32(pixel_45_32x2x2.val[0]), factors_45_left_u_8x8);
		uint16x8_t multiplication_67_u_16x8 = vmull_u8(vreinterpret_u8_u32(pixel_67_32x2x2.val[0]), factors_67_left_u_8x8);

		multiplication_45_u_16x8 = vmlal_u8(multiplication_45_u_16x8, vreinterpret_u8_u32(pixel_45_32x2x2.val[1]), factors_45_right_u_8x8);
		multiplication_67_u_16x8 = vmlal_u8(multiplication_67_u_16x8, vreinterpret_u8_u32(pixel_67_32x2x2.val[1]), factors_67_right_u_8x8);

		vst1q_u8(targetRow, vcombine_u8(vrshrn_n_u16(multiplication_45_u_16x8, 7), vrshrn_n_u16(multiplication_67_u_16x8, 7)));
		targetRow += 16;


		interpolationLocations += 8;
		interpolationFactors += 16;
	}
}

void FrameInterpolatorBilinear::scale8BitPerChannelSubset7BitPrecisionNEON(const uint8_t* source, uint8_t* target, const unsigned int sourceWidth, const unsigned int sourceHeight, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int channels, const double xTargetToSource, const double yTargetToSource, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(sourceWidth * channels >= 16u && sourceWidth <= 65535u);
	ocean_assert(sourceHeight >= 1u && sourceHeight <= 65535u);
	ocean_assert(targetWidth >= 8u && targetWidth <= 65535u);
	ocean_assert(targetHeight >= 1u && targetHeight <= 65535u);
	ocean_assert(xTargetToSource > 0.0 && yTargetToSource > 0.0);

	ocean_assert(sourceWidth != targetWidth || sourceHeight != targetHeight);

	const unsigned int sourceStrideElements = sourceWidth * channels + sourcePaddingElements;
	const unsigned int targetStrideElements = targetWidth * channels + targetPaddingElements;

	target += targetStrideElements * firstTargetRow;

	// this function uses fixed point numbers with 16 bit for the calculation of the interpolation positions and factors:
	// fixedPointLocation = floatLocation * 2^16
	//
	// [FEDCBA98, 76543210]
	// [pixel   , subpixel]
	//
	// fixedPointLocation = pixel + subpixel / 2^16
	//
	// Thus, the upper 16 bit represent the location of e.g., the left pixel (for the linear interpolation)
	// while the lower 16 bit represent one of both interpolation factors (and 2^16 - subpixel represents the second interpolation factor)
	//
	// first, we apply a vertical interpolation for two rows and create an intermediate row
	// second, we apply a horizontal interpolation based on the intermediate row
	//
	// in general, the horizontal interpolation is more complicated as the vertical interpolation
	// we can optimize the performance by pre-calculating the interpolation coordinates and interpolation factors

	typedef void (*RowInterpolationHorizontalFunction)(const uint8_t*, uint8_t*, const unsigned int, const unsigned int, const unsigned int*, const uint8_t*);

	RowInterpolationHorizontalFunction rowInterpolationHorizontalFunction = nullptr;

	if (sourceWidth != targetWidth)
	{
		switch (channels)
		{
			case 1u:
				rowInterpolationHorizontalFunction = interpolateRowHorizontal8BitPerChannel7BitPrecisionNEON<1u>;
				break;

			// case 2u: // **TODO** will be added soon
			// case 3u:

			case 4u:
				rowInterpolationHorizontalFunction = interpolateRowHorizontal8BitPerChannel7BitPrecisionNEON<4u>;
				break;

			default:
				rowInterpolationHorizontalFunction = interpolateRowHorizontal8BitPerChannel7BitPrecision;
				break;
		}
	}

	Memory memoryIntermediateExtendedRow;
	Memory memoryHorizontalInterpolationLocations;
	Memory memoryHorizontalInterpolationFactors;

	if (sourceWidth != targetWidth)
	{
		// in case we are scaling the width of the frame, we use an intermediate buffer and pre-calculated interpolation locations and factors

		memoryIntermediateExtendedRow = Memory::create<uint8_t>((sourceWidth + 1u) * channels); // one additional pixel

		memoryHorizontalInterpolationLocations = Memory::create<unsigned int>(targetWidth); // one offset for each target pixel

		memoryHorizontalInterpolationFactors = Memory::create<uint8_t>(targetWidth * 2u); // two factors (left and right) for each target pixel
	}

	if (memoryHorizontalInterpolationLocations)
	{
		ocean_assert(memoryHorizontalInterpolationFactors);

		// we pre-calculate the interpolation factors and pixel locations in horizontal direction

		const unsigned int targetToSourceX_fixed16 = (unsigned int)(double(0x10000u) * xTargetToSource + 0.5);

		// we store 4 integers: [targetToSourceX_fixed16, targetToSourceX_fixed16, targetToSourceX_fixed16, targetToSourceX_fixed16]
		const uint32x4_t targetToSourceX_fixed16_u_32x4 = vdupq_n_u32(targetToSourceX_fixed16);

		int targetOffsetX_fixed16 = (int)(double(0x10000u) * ((xTargetToSource * 0.5) - 0.5) + 0.5);

		// we store 4 integers: [targetToSourceX_fixed16, targetToSourceX_fixed16, targetToSourceX_fixed16, targetToSourceX_fixed16]
		int32x4_t targetOffsetX_fixed16_s_32x4 = vdupq_n_s32(targetOffsetX_fixed16);

		// we store 4 integers: [sourceWidth - 1, sourceWidth - 1, sourceWidth - 1, sourceWidth - 1]
		const uint32x4_t sourceWidth_1_u_32x4 = vdupq_n_u32(sourceWidth - 1u);

		// we store 4 integers: [0, 0, 0, 0]
		const int32x4_t constant_0_s_32x4 = vdupq_n_s32(0);

		// we store 4 integers: [64, 64, 64, 64]
		const uint32x4_t constant_64_u_32x4 = vdupq_n_u32(64);

		// we store 4 integers: [channels, channels, channels, channels]
		const uint32x4_t constant_channels_u_32x4 = vdupq_n_u32(channels);

		// [128u, 128u, 128u, 128u, 128u, 128u, 128u, 128u]
		const uint8x8_t constant_128_u_8x8 = vdup_n_u8(128u);

		// [8u, 8u, 8u, 8u]
		const uint32x4_t constant_8_u_32x4 = vdupq_n_u32(8u);

		// [0u, 1u, 2u, 3u]
		const unsigned int u_0123[4] = {0u, 1u, 2u, 3u};
		uint32x4_t x_0123_u_32x4 = vld1q_u32(u_0123);

		// [4u, 5u, 6u, 7u]
		const unsigned int u_4567[4] = {4u, 5u, 6u, 7u};
		uint32x4_t x_4567_u_32x4 = vld1q_u32(u_4567);

		// as we apply integer multiplication for calculation of the lookup position,
		// we will loose precision with growing x values
		// thus, we simply reset 'x' every 64th pixels
		unsigned int xNextRefinementStep = 64u;

		for (unsigned int x = 0u; x < targetWidth; x += 8u)
		{
			if (x + 8u > targetWidth)
			{
				// the last iteration will not fit into the output frame,
				// so we simply shift x left by some pixels (at most 7) and we will calculate some pixels again

				ocean_assert(x >= 8u && targetWidth > 8u);
				const unsigned int newX = targetWidth - 8u;

				ocean_assert(x > newX);
				const unsigned int offset = x - newX;

				x = newX;

				x_0123_u_32x4 = vsubq_u32(x_0123_u_32x4, vdupq_n_u32(offset));
				x_4567_u_32x4 = vsubq_u32(x_4567_u_32x4, vdupq_n_u32(offset));

				// the for loop will stop after this iteration
				ocean_assert(!(x + 8u < targetWidth));
			}
			else
			{
				if (x >= xNextRefinementStep)
				{
					// we simply reset x every 64th pixels by adding a delta to 'targetOffsetX_fixed16':
					//
					// xSource = (xTarget + 0.5) * sourceToTarget - 0.5
					//         = (xTarget0 + xTarget^ + 0.5) * sourceToTarget - 0.5
					//         = (xTarget0 * sourceToTarget) + (xTarget^ * sourceToTarget) + (0.5 * sourceToTarget - 0.5)
					//         = newTargetOffsetX + (xTarget^ * sourceToTarget)
					//
					// with 'xTarget0' a constant x for each block, the result of xTarget0 * sourceToTarget will be calculated with floating point precision
					//      'xTarget^' the increasing x

					ocean_assert(x == xNextRefinementStep);

					xNextRefinementStep = x + 64u;

					targetOffsetX_fixed16 = (int)(double(0x10000u) * (xTargetToSource * double(x) + (xTargetToSource * 0.5) - 0.5) + 0.5);

					// we store 4 integers: [targetToSourceX_fixed16, targetToSourceX_fixed16, targetToSourceX_fixed16, targetToSourceX_fixed16]
					targetOffsetX_fixed16_s_32x4 = vdupq_n_s32(targetOffsetX_fixed16);

					// actually we will reset x to [0, 1, 2, 3] and [4, 5, 6, 7]
					x_0123_u_32x4 = vsubq_u32(x_0123_u_32x4, constant_64_u_32x4);
					x_4567_u_32x4 = vsubq_u32(x_4567_u_32x4, constant_64_u_32x4);
				}
			}

			// we calculate the four source locations for our four target locations
			const int32x4_t sourceX_0123_fixed16_s_32x4 = vmaxq_s32(constant_0_s_32x4, vaddq_s32(targetOffsetX_fixed16_s_32x4, vreinterpretq_s32_u32(vmulq_u32(targetToSourceX_fixed16_u_32x4, x_0123_u_32x4))));
			const uint32x4_t sourceX_0123_fixed16_u_32x4 = vreinterpretq_u32_s32(sourceX_0123_fixed16_s_32x4);

			const int32x4_t sourceX_4567_fixed16_s_32x4 = vmaxq_s32(constant_0_s_32x4, vaddq_s32(targetOffsetX_fixed16_s_32x4, vreinterpretq_s32_u32(vmulq_u32(targetToSourceX_fixed16_u_32x4, x_4567_u_32x4))));
			const uint32x4_t sourceX_4567_fixed16_u_32x4 = vreinterpretq_u32_s32(sourceX_4567_fixed16_s_32x4);

			// now we determine the pixel/integer accurate source locations
			// m128_u_left = min(floor(m128_f_sourceX), sourceWidth - 1)
			uint32x4_t left_0123_u_32x4 = vminq_u32(vshrq_n_u32(sourceX_0123_fixed16_u_32x4, 16), sourceWidth_1_u_32x4); // not vrshrq_n_u32 as we must not round here
			uint32x4_t left_4567_u_32x4 = vminq_u32(vshrq_n_u32(sourceX_4567_fixed16_u_32x4, 16), sourceWidth_1_u_32x4);

			// we store the interpolation locations in relations to elements and not to pixels (to avoid a multiplication by 'channels' later)
			left_0123_u_32x4 = vmulq_u32(left_0123_u_32x4, constant_channels_u_32x4);
			left_4567_u_32x4 = vmulq_u32(left_4567_u_32x4, constant_channels_u_32x4);

			// we store the offsets we have calculated
			vst1q_u32(memoryHorizontalInterpolationLocations.data<unsigned int>() + x + 0, left_0123_u_32x4);
			vst1q_u32(memoryHorizontalInterpolationLocations.data<unsigned int>() + x + 4, left_4567_u_32x4);

			const uint8x8_t factorsRight_u_8x8 = vqmovn_u16(vrshrq_n_u16(vcombine_u16(vmovn_u32(sourceX_0123_fixed16_u_32x4), vmovn_u32(sourceX_4567_fixed16_u_32x4)), 9));
			const uint8x8_t factorsLeft_u_8x8 = vsub_u8(constant_128_u_8x8, factorsRight_u_8x8);

			const uint8x8x2_t factorsLeftRight_u_8x8x2 = vzip_u8(factorsLeft_u_8x8, factorsRight_u_8x8);

			vst1q_u8(memoryHorizontalInterpolationFactors.data<uint8_t>() + x * 2u, vcombine_u8(factorsLeftRight_u_8x8x2.val[0], factorsLeftRight_u_8x8x2.val[1]));

			// [x + 0, x + 1, x + 2, x + 3] + [8, 8, 8, 8]
			x_0123_u_32x4 = vaddq_u32(x_0123_u_32x4, constant_8_u_32x4);
			x_4567_u_32x4 = vaddq_u32(x_4567_u_32x4, constant_8_u_32x4);
		}
	}

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		const unsigned int sourceY_fixed16 = minmax<int>(0, (unsigned int)(float(0x10000u) * ((float(y) + 0.5f) * float(yTargetToSource) - 0.5f)), (sourceHeight - 1u) << 16u);

		const unsigned int sourceRowTop = sourceY_fixed16 >> 16u; // we must not round here
		const unsigned int factorBottom_fixed16 = sourceY_fixed16 & 0x0000FFFFu;
		const unsigned int factorBottom = (factorBottom_fixed16 + 256u) >> 9u;

		const unsigned int sourceRowBottom = min(sourceRowTop + 1u, sourceHeight - 1u);

		const uint8_t* const sourceTopRow = source + sourceStrideElements * sourceRowTop;
		const uint8_t* const sourceBottomRow = source + sourceStrideElements * sourceRowBottom;

		uint8_t* targetRow = nullptr;

		if (sourceHeight == targetHeight || (memoryIntermediateExtendedRow && factorBottom == 0u)) // factorBottom == 0u: we can simply use the top row
		{
			ocean_assert(sourceWidth != targetWidth);
			ocean_assert(memoryIntermediateExtendedRow);

			ocean_assert(rowInterpolationHorizontalFunction != nullptr);

			// we do not need to interpolate two lines, thus we simply need to copy the row (as we need an additional pixel at the end)
			memcpy(memoryIntermediateExtendedRow.data<uint8_t>(), sourceTopRow, sourceWidth * channels);
		}
		else if (factorBottom == 128u && memoryIntermediateExtendedRow)
		{
			// we can simply use the bottom row
			memcpy(memoryIntermediateExtendedRow.data<uint8_t>(), sourceBottomRow, sourceWidth * channels);
		}
		else
		{
			// in case we do not scale the width of the frame, we can write the result to the target frame directly
			targetRow = rowInterpolationHorizontalFunction == nullptr ? target : memoryIntermediateExtendedRow.data<uint8_t>();

			ocean_assert(targetRow != nullptr);
			interpolateRowVertical8BitPerChannel7BitPrecisionNEON(sourceTopRow, sourceBottomRow, targetRow, sourceWidth * channels, factorBottom);
		}

		if (rowInterpolationHorizontalFunction != nullptr)
		{
			// we use an extended row (with one additional pixel at the end - equal to the last pixel)
			// so we have to copy the last pixel

			uint8_t* const lastExtendedRowPixel = memoryIntermediateExtendedRow.data<uint8_t>() + sourceWidth * channels;
			const uint8_t* const lastValidExtendedRowPixel = lastExtendedRowPixel - channels;
			memcpy(lastExtendedRowPixel, lastValidExtendedRowPixel, channels);

			rowInterpolationHorizontalFunction(memoryIntermediateExtendedRow.data<uint8_t>(), target, targetWidth, channels, memoryHorizontalInterpolationLocations.data<unsigned int>(), memoryHorizontalInterpolationFactors.data<uint8_t>());
		}

		target += targetStrideElements;
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

}
