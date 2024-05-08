/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_CREATOR_INFORMATION_SPATIAL_COST_I_1_H
#define META_OCEAN_CV_SYNTHESIS_CREATOR_INFORMATION_SPATIAL_COST_I_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/CreatorFrame.h"
#include "ocean/cv/synthesis/CreatorI.h"
#include "ocean/cv/synthesis/CreatorSubset.h"
#include "ocean/cv/synthesis/Creator1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements a creator object that creates a visual representation of the spatial mapping cost.
 * The information output is a frame visualizing target pixels with zero and non-zero spatial mapping cost.<br>
 * The template parameter defines the size of the neighborhood that is used for the output.
 * @tparam tNeighborhood Number of neighbors that must have perfect mapping, with range [1, 4]
 * @tparam tOnlyCenterPixels True, if only center pixels will be considered
 * @ingroup cvsynthesis
 */
template <unsigned int tNeighborhood, bool tOnlyCenterPixels>
class CreatorInformationSpatialCostI1 :
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
		inline CreatorInformationSpatialCostI1(const LayerI1& layer, Frame& target);

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

template <unsigned int tNeighborhood, bool tOnlyCenterPixels>
inline CreatorInformationSpatialCostI1<tNeighborhood, tOnlyCenterPixels>::CreatorInformationSpatialCostI1(const LayerI1& layer, Frame& target) :
	Creator(layer),
	CreatorFrame(layer, target),
	CreatorI(layer),
	CreatorSubset(layer),
	Creator1(layer)
{
	// nothing to do here
}

template <unsigned int tNeighborhood, bool tOnlyCenterPixels>
void CreatorInformationSpatialCostI1<tNeighborhood, tOnlyCenterPixels>::createSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	ocean_assert(target_.numberPlanes() == 1u);

	switch (target_.channels())
	{
		case 1u:
			createSubsetChannels<1u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		default:
			ocean_assert(false && "Invalid frame type.");
	}
}

template <unsigned int tNeighborhood, bool tOnlyCenterPixels>
template <unsigned int tChannels>
void CreatorInformationSpatialCostI1<tNeighborhood, tOnlyCenterPixels>::createSubsetChannels(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	static_assert(tNeighborhood >= 1u && tNeighborhood <= 4u, "Invalid number of neighbors!");
	static_assert(tChannels == 1u, "Invalid channel number!");

	ocean_assert(target_.isValid());

	const unsigned int layerWidth = layerI_.width();
	const unsigned int layerHeight = layerI_.height();

	const MappingI& layerMapping = layerI_.mapping();

	const Frame& layerMask = layerI_.mask();

	const unsigned int maskStrideElements = layerMask.strideElements();

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		uint8_t* targetRow = target_.row<uint8_t>(y) + firstColumn;
		const uint8_t* maskPixel = layerI_.mask().template constrow<uint8_t>(y) + firstColumn;

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (*maskPixel != 0xFFu)
			{
				unsigned int counter = 0u;

				const PixelPosition& position = layerMapping.position(x, y);
				ocean_assert(position);

				// top
				if (y > 0u && *(maskPixel - maskStrideElements) != 0xFFu)
				{
					if (layerMapping.position(x, y - 1u) == position.north())
					{
						++counter;
					}
				}
				else if (!tOnlyCenterPixels)
				{
					++counter;
				}

				// left
				if (x > 0u && *(maskPixel - 1) != 0xFFu)
				{
					if (layerMapping.position(x - 1u, y) == position.west())
					{
						++counter;
					}
				}
				else if (!tOnlyCenterPixels)
				{
					++counter;
				}

				// bottom
				if (y < layerHeight - 1u && *(maskPixel + maskStrideElements) != 0xFFu)
				{
					if (layerMapping.position(x, y + 1u) == position.south())
					{
						++counter;
					}
				}
				else if (!tOnlyCenterPixels)
				{
					++counter;
				}

				// right
				if (x < layerWidth - 1u && *(maskPixel + 1) != 0xFFu)
				{
					if (layerMapping.position(x + 1u, y) == position.east())
					{
						++counter;
					}
				}
				else if (!tOnlyCenterPixels)
				{
					++counter;
				}

				if (counter >= tNeighborhood)
				{
					*targetRow = 0x80u;
				}
				else
				{
					*targetRow = 0x00u;
				}
			}

			++targetRow;
			++maskPixel;
		}
	}
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_CREATOR_INFORMATION_SPATIAL_COST_I_1_H
