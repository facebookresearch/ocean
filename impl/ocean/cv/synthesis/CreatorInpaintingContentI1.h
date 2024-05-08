/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_CREATOR_INPAINTING_CONTENT_I_1_H
#define META_OCEAN_CV_SYNTHESIS_CREATOR_INPAINTING_CONTENT_I_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/CreatorFrame.h"
#include "ocean/cv/synthesis/CreatorI.h"
#include "ocean/cv/synthesis/CreatorSubset.h"
#include "ocean/cv/synthesis/Creator1.h"
#include "ocean/cv/synthesis/LayerI1.h"

#include "ocean/base/DataType.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements a creator object that creates the final synthesis image for a mapping with float accuracy and a mapping within the same frame.
 * @see CreatorInpaintingContentF1
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT CreatorInpaintingContentI1 :
	virtual public CreatorFrame,
	virtual public CreatorI,
	virtual public CreatorSubset,
	virtual public Creator1
{
	public:

		/**
		 * Creates a new creator object.
		 * @param layer The layer that is used to create the information
		 * @param target The target frame that will receive the creator output
		 */
		inline CreatorInpaintingContentI1(const LayerI1& layer, Frame& target);

	protected:

		/**
		 * Creates a subset of the information.
		 * @see CreatorSubset::createSubset().
		 */
		void createSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const override;

		/**
		 * Specialization of the default function that creates a subset of the information.
		 * The template parameter specifies the number of channels of the target frame.
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannels Number of data channels of the frame
		 */
		template <unsigned int tChannels>
		void createSubsetChannels(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const;
};

inline CreatorInpaintingContentI1::CreatorInpaintingContentI1(const LayerI1& layer, Frame& target) :
	Creator(layer),
	CreatorFrame(layer, target),
	CreatorI(layer),
	CreatorSubset(layer),
	Creator1(layer)
{
	// nothing to do here
}

template <unsigned int tChannels>
void CreatorInpaintingContentI1::createSubsetChannels(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	ocean_assert(firstColumn + numberColumns <= layerI_.width());
	ocean_assert(firstRow + numberRows <= layerI_.height());

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const uint8_t* targetData = target_.data<uint8_t>();

	const unsigned int targetStrideElements = target_.strideElements();

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		PixelType* targetRow = target_.row<PixelType>(y);

		const uint8_t* maskRow = layerI_.mask().constrow<uint8_t>(y);
		const PixelPosition* mappingRow = layerI_.mapping().row(y);

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (maskRow[x] != 0xFFu)
			{
				const PixelPosition& mapping = mappingRow[x];

				ocean_assert(mapping.isValid() && mapping.x() < layerI_.width() && mapping.y() < layerI_.height());
				ocean_assert(layerI_.mask().constpixel<uint8_t>(mapping.x(), mapping.y())[0] == 0xFFu);

				targetRow[x] = *((const PixelType*)(targetData + mapping.y() * targetStrideElements) + mapping.x());
			}
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_CREATOR_INPAINTING_CONTENT_I_1_H
