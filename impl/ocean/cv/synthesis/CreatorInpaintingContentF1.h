/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_CREATOR_INPAINTING_CONTENT_F_1_H
#define META_OCEAN_CV_SYNTHESIS_CREATOR_INPAINTING_CONTENT_F_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/CreatorFrame.h"
#include "ocean/cv/synthesis/CreatorF.h"
#include "ocean/cv/synthesis/CreatorSubset.h"
#include "ocean/cv/synthesis/Creator1.h"
#include "ocean/cv/synthesis/LayerF1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements a creator object that creates the final synthesis image for a mapping with float accuracy and a mapping within the same frame.
 * @see CreatorInpaintingContentI1
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT CreatorInpaintingContentF1 :
	virtual public CreatorFrame,
	virtual public CreatorF,
	virtual public CreatorSubset,
	virtual public Creator1
{
	public:

		/**
		 * Creates a new creator object.
		 * @param layer The layer that is used to create the information
		 * @param target The target frame that will receive the creator output
		 */
		inline CreatorInpaintingContentF1(const LayerF1& layer, Frame& target);

	protected:

		/**
		 * Creates a subset of the information.
		 * @see CreatorSubset::createSubset().
		 */
		void createSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const override;

		/**
		 * Specialization of the default function that creates a subset of the information.
		 * The template parameter specifies the number of channels of the target frame.<br>
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannels Number of data channels of the frame
		 */
		template <unsigned int tChannels>
		void createSubsetChannels(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const;
};

inline CreatorInpaintingContentF1::CreatorInpaintingContentF1(const LayerF1& layer, Frame& target) :
	Creator(layer),
	CreatorFrame(layer, target),
	CreatorF(layer),
	CreatorSubset(layer),
	Creator1(layer)
{
	// nothing to do here
}

template <unsigned int tChannels>
void CreatorInpaintingContentF1::createSubsetChannels(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	const unsigned int layerWidth = layerF_.width();
	const unsigned int layerHeight = layerF_.height();

	ocean_assert(layerF_.mask().width() == layerWidth);
	ocean_assert(layerF_.mask().height() == layerHeight);

	uint8_t* const frameData = target_.data<uint8_t>();

	const unsigned int framePaddingElements = target_.paddingElements();

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		uint8_t* framePixel = target_.pixel<uint8_t>(firstColumn, y);
		const uint8_t* maskPixel = layerF_.mask().constpixel<uint8_t>(firstColumn, y);
		const Vector2* mappingPixel = layerF_.mapping()() + y * layerWidth + firstColumn;

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (*maskPixel != 0xFF)
			{
				const Vector2& position = *mappingPixel;

				ocean_assert(position.x() >= 0 && position.y() >= 0 && position.x() <= Scalar(layerWidth - 1u) && position.y() <= Scalar(layerHeight - 1u));
				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(frameData, layerWidth, layerHeight, framePaddingElements, position, framePixel);
			}

			framePixel += tChannels;
			++maskPixel;
			++mappingPixel;
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_CREATOR_INPAINTING_CONTENT_F_1_H
