// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/advanced/FrameColorAdjustment.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

bool FrameColorAdjustment::adjustFrameBilinear(const Frame& reference, const Frame& referenceMask, Frame& frame, const Frame& frameMask, const unsigned int horizontalBins, const unsigned int verticalBins, const Scalar threshold, const unsigned char maskValue, Worker* worker)
{
	ocean_assert(reference.isValid() && frame.isValid());

	ocean_assert(referenceMask.isNull() || (referenceMask.width() == reference.width() && referenceMask.height() == reference.height() && referenceMask.pixelOrigin() == reference.pixelOrigin()));
	ocean_assert(frameMask.isNull() || (frameMask.width() == frame.width() && frameMask.height() == frame.height() && frameMask.pixelOrigin() == frame.pixelOrigin()));

	if (reference.isValid() && frame.isValid() && reference.frameType() == frame.frameType() && frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		const uint8_t* referenceMaskData = referenceMask.isNull() ? nullptr : referenceMask.constdata<uint8_t>();
		const uint8_t* frameMaskData = frameMask.isNull() ? nullptr : frameMask.constdata<uint8_t>();

		const unsigned int referenceMaskPaddingElements = referenceMask.isNull() ? 0u : referenceMask.paddingElements();
		const unsigned int frameMaskPaddingElements = frameMask.isNull() ? 0u : frameMask.paddingElements();

		switch (frame.channels())
		{
			case 1u:
				adjustFrameBilinear8BitPerChannel<1u>(reference.constdata<uint8_t>(), referenceMaskData, frame.data<uint8_t>(), frameMaskData, reference.width(), reference.height(), reference.paddingElements(), referenceMaskPaddingElements, frame.paddingElements(), frameMaskPaddingElements, horizontalBins, verticalBins, threshold, maskValue, worker);
				return true;

			case 2u:
				adjustFrameBilinear8BitPerChannel<2u>(reference.constdata<uint8_t>(), referenceMaskData, frame.data<uint8_t>(), frameMaskData, reference.width(), reference.height(), reference.paddingElements(), referenceMaskPaddingElements, frame.paddingElements(), frameMaskPaddingElements, horizontalBins, verticalBins, threshold, maskValue, worker);
				return true;

			case 3u:
				adjustFrameBilinear8BitPerChannel<3u>(reference.constdata<uint8_t>(), referenceMaskData, frame.data<uint8_t>(), frameMaskData, reference.width(), reference.height(), reference.paddingElements(), referenceMaskPaddingElements, frame.paddingElements(), frameMaskPaddingElements, horizontalBins, verticalBins, threshold, maskValue, worker);
				return true;

			case 4u:
				adjustFrameBilinear8BitPerChannel<4u>(reference.constdata<uint8_t>(), referenceMaskData, frame.data<uint8_t>(), frameMaskData, reference.width(), reference.height(), reference.paddingElements(), referenceMaskPaddingElements, frame.paddingElements(), frameMaskPaddingElements, horizontalBins, verticalBins, threshold, maskValue, worker);
				return true;
		}
	}

	ocean_assert(false && "Invalid frame!");
	return false;
}

}

}

}
