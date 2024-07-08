/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/InitializerContourMappingI1.h"

#include "ocean/cv/FrameFilterErosion.h"

#include "ocean/cv/segmentation/ContourMapping.h"
#include "ocean/cv/segmentation/MaskAnalyzer.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

bool InitializerContourMappingI1::invoke(Worker* worker) const
{
	const Frame& mask = layerI_.mask();
	Frame& frame = layerI_.frame();

	if (mask.pixelFormat() != FrameType::FORMAT_Y8 || frame.pixelOrigin() != mask.pixelOrigin())
	{
		return false;
	}

	const PixelBoundingBox& boundingBox = layerI_.boundingBox();

	PixelPositions outlinePixels4;
	Segmentation::MaskAnalyzer::findOutline4(mask.constdata<uint8_t>(), mask.width(), mask.height(), mask.paddingElements(), outlinePixels4, boundingBox);

	PixelPositions contour;

	if (Segmentation::MaskAnalyzer::pixels2contour(outlinePixels4, mask.width(), mask.height(), contour))
	{
		const PixelBoundingBox countourBoundingBox(contour);

		if (frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
		{
			Frame copyMask(mask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

			switch (frame.channels())
			{
				case 1u:
				{
					const Indices32 mapping = Segmentation::ContourMapping::contour2mapping8BitPerChannel<1u>(frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), contour);
					Segmentation::ContourMapping::mappingInterpolation8BitPerChannel<1u>(frame.data<uint8_t>(), copyMask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), copyMask.paddingElements(), contour, countourBoundingBox, mapping);
					break;
				}

				case 3u:
				{
					const Indices32 mapping = Segmentation::ContourMapping::contour2mapping8BitPerChannel<3u>(frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), contour);
					Segmentation::ContourMapping::mappingInterpolation8BitPerChannel<3u>(frame.data<uint8_t>(), copyMask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), copyMask.paddingElements(), contour, countourBoundingBox, mapping);
					break;
				}

				case 4u:
				{
					const Indices32 mapping = Segmentation::ContourMapping::contour2mapping8BitPerChannel<4u>(frame.constdata<uint8_t>(), mask.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), mask.paddingElements(), contour);
					Segmentation::ContourMapping::mappingInterpolation8BitPerChannel<4u>(frame.data<uint8_t>(), copyMask.data<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), copyMask.paddingElements(), contour, countourBoundingBox, mapping);
					break;
				}

				default:
					ocean_assert(false && "Missing implementation!");
					break;
			}

			FrameFilterErosion::Comfort::shrinkMaskRandom(frame, copyMask, CV::FrameFilterErosion::MF_SQUARE_3);
			return appearanceInitializer_.invoke(worker);
		}
	}

	return false;
}

}

}

}
