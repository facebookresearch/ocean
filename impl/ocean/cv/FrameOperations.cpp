/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameOperations.h"

namespace Ocean
{

namespace CV
{

bool FrameOperations::subtract(const Frame& source0, const Frame& source1, Frame& target, Worker* worker)
{
	if (source0.frameType() != source1.frameType() || source0.frameType() != target.frameType())
	{
		ocean_assert(false && "Invalid input");

		return false;
	}

	const unsigned int width = source0.width();
	const unsigned int height = source0.height();

	const FrameChannels::ConversionFlag conversionFlag = FrameChannels::CONVERT_NORMAL;

	switch ((std::underlying_type<FrameType::PixelFormat>::type)(FrameType::makeGenericPixelFormat(source0.pixelFormat())))
	{
		// uint8_t, 1 channel
		case FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, 1u):
			subtract<uint8_t, uint8_t, uint8_t, uint8_t, 1u>(source0.constdata<uint8_t>(), source1.constdata<uint8_t>(), target.data<uint8_t>(), width, height, source0.paddingElements(), source1.paddingElements(), target.paddingElements(), conversionFlag, worker);
			break;

		// uint8_t, 2 channel
		case FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, 2u):
			subtract<uint8_t, uint8_t, uint8_t, uint8_t, 2u>(source0.constdata<uint8_t>(), source1.constdata<uint8_t>(), target.data<uint8_t>(), width, height, source0.paddingElements(), source1.paddingElements(), target.paddingElements(), conversionFlag, worker);
			break;

		// uint8_t, 3 channels
		case FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, 3u):
			subtract<uint8_t, uint8_t, uint8_t, uint8_t, 3u>(source0.constdata<uint8_t>(), source1.constdata<uint8_t>(), target.data<uint8_t>(), width, height, source0.paddingElements(), source1.paddingElements(), target.paddingElements(), conversionFlag, worker);
			break;

		// uint8_t, 4 channels
		case FrameType::genericPixelFormat(FrameType::DT_UNSIGNED_INTEGER_8, 4u):
			subtract<uint8_t, uint8_t, uint8_t, uint8_t, 4u>(source0.constdata<uint8_t>(), source1.constdata<uint8_t>(), target.data<uint8_t>(), width, height, source0.paddingElements(), source1.paddingElements(), target.paddingElements(), conversionFlag, worker);
			break;

		// float, 1 channel
		case FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, 1u):
			subtract<float, float, float, float, 1u>(source0.constdata<float>(), source1.constdata<float>(), target.data<float>(), width, height, source0.paddingElements(), source1.paddingElements(), target.paddingElements(), conversionFlag, worker);
			break;

		// float, 1 channel
		case FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, 2u):
			subtract<float, float, float, float, 2u>(source0.constdata<float>(), source1.constdata<float>(), target.data<float>(), width, height, source0.paddingElements(), source1.paddingElements(), target.paddingElements(), conversionFlag, worker);
			break;

		// float, 3 channels
		case FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, 3u):
			subtract<float, float, float, float, 3u>(source0.constdata<float>(), source1.constdata<float>(), target.data<float>(), width, height, source0.paddingElements(), source1.paddingElements(), target.paddingElements(), conversionFlag, worker);
			break;

		// float, 4 channels
		case FrameType::genericPixelFormat(FrameType::DT_SIGNED_FLOAT_32, 4u):
			subtract<float, float, float, float, 4u>(source0.constdata<float>(), source1.constdata<float>(), target.data<float>(), width, height, source0.paddingElements(), source1.paddingElements(), target.paddingElements(), conversionFlag, worker);
			break;

		default:
			ocean_assert(false && "Unsupported pixel format. Consider adding it or using the template-based function subtract() directly");
			return false;
	}

	return true;
}

} // namespace CV

} // namespace Ocean
