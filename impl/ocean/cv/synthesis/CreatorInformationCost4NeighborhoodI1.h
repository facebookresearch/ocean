/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_CREATOR_INFORMATION_COST_4_NEIGHBORHOOD_I_1_H
#define META_OCEAN_CV_SYNTHESIS_CREATOR_INFORMATION_COST_4_NEIGHBORHOOD_I_1_H

#include "ocean/cv/synthesis/Synthesis.h"
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
 * This class implements a creator that determines the mapping cost for a four neighborhood for mappings with integer accuracy.
 * @tparam tWeightFactor Spatial weight impact, with range [0, infinity)
 * @tparam tBorderFactor Weight factor of border pixels, with range [1, infinity)
 * @ingroup cvsynthesis
 */
template <unsigned int tWeightFactor, unsigned int tBorderFactor>
class CreatorInformationCost4NeighborhoodI1 :
	virtual public CreatorI,
	virtual public CreatorSubset,
	virtual public Creator1
{
	protected:

		/**
		 * Definition of a vector holding costs.
		 */
		typedef std::vector<uint64_t> Costs;

	public:

		/**
		 * Creates a new creator object.
		 * @param layer Synthesis layer that is used for to create the information
		 * @param cost Resulting mapping cost
		 * @param maxSpatialCost Maximal spatial cost
		 */
		inline CreatorInformationCost4NeighborhoodI1(const LayerI1& layer, uint64_t& cost, const unsigned int maxSpatialCost = (unsigned int)(-1));

		/**
		 * Invokes the creator.
		 * @see CreatorSubset::invoke().
		 */
		bool invoke(Worker* worker = nullptr) const override;

	protected:

		/**
		 * Creates a subset of the information.
		 * @see CreatorSubsxet::createSubset().
		 */
		void createSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const override;

		/**
		 * Specialization of the default function that creates a subset of the information.
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 * @tparam tChannels Number of channels of the target frame
		 */
		template <unsigned int tChannels>
		void createSubsetChannels(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const;

	protected:

		/// Specialized layer reference.
		const LayerI1& layerI1_;

		/// Resulting creator cost.
		uint64_t& cost_;

		/// Maximal spatial cost.
		const unsigned int maxSpatialCost_;

		/// Intermediate row costs.
		mutable Costs rowCost_;
};

template <unsigned int tWeightFactor, unsigned int tBorderFactor>
inline CreatorInformationCost4NeighborhoodI1<tWeightFactor, tBorderFactor>::CreatorInformationCost4NeighborhoodI1(const LayerI1& layer, uint64_t& cost, const unsigned int maxSpatialCost) :
	Creator(layer),
	CreatorI(layer),
	CreatorSubset(layer),
	Creator1(layer),
	layerI1_(layer),
	cost_(cost),
	maxSpatialCost_(maxSpatialCost)
{
	// nothing to do here
}

template <unsigned int tWeightFactor, unsigned int tBorderFactor>
bool CreatorInformationCost4NeighborhoodI1<tWeightFactor, tBorderFactor>::invoke(Worker* worker) const
{
	rowCost_ = Costs(layerI1_.height(), 0u);
	cost_ = 0ull;

	if (!CreatorSubset::invoke(worker))
	{
		return false;
	}

	const size_t firstRow = layerI1_.boundingBox() ? layerI1_.boundingBox().top() : 0u;
	const size_t endRow = layerI1_.boundingBox() ? layerI1_.boundingBox().bottomEnd() : layerI1_.height();

#ifdef OCEAN_DEBUG

	for (size_t n = 0; n < firstRow; ++n)
	{
		ocean_assert(rowCost_[n] == 0);
	}
	for (size_t n = endRow; n < layerI1_.height(); ++n)
	{
		ocean_assert(rowCost_[n] == 0);
	}

#endif // OCEAN_DEBUG

	for (size_t n = firstRow; n < endRow; ++n)
	{
		cost_ += rowCost_[n];
	}

	return true;
}

template <unsigned int tWeightFactor, unsigned int tBorderFactor>
void CreatorInformationCost4NeighborhoodI1<tWeightFactor, tBorderFactor>::createSubset(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	ocean_assert(layerI1_.frame().numberPlanes() == 1u);

	switch (layerI1_.frame().channels())
	{
		case 1u:
			createSubsetChannels<1u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		case 2u:
			createSubsetChannels<2u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		case 3u:
			createSubsetChannels<3u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		case 4u:
			createSubsetChannels<4u>(firstColumn, numberColumns, firstRow, numberRows);
			break;

		default:
			ocean_assert(false && "Invalid frame type.");
	}
}

template <unsigned int tWeightFactor, unsigned int tBorderFactor>
template <unsigned int tChannels>
void CreatorInformationCost4NeighborhoodI1<tWeightFactor, tBorderFactor>::createSubsetChannels(const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows) const
{
	const uint8_t* layerFrame = layerI1_.frame().template constdata<uint8_t>();
	const uint8_t* layerMask = layerI1_.mask().template constdata<uint8_t>();

	const unsigned int layerFramePaddingElements = layerI1_.frame().paddingElements();
	const unsigned int layerMaskPaddingElements = layerI1_.mask().paddingElements();

	const MappingI1& layerMapping = layerI1_.mapping();

#ifdef OCEAN_DEBUG
	const PixelBoundingBox& debugLayerBoundingBox = layerI1_.boundingBox();
	ocean_assert(!debugLayerBoundingBox || firstRow >= debugLayerBoundingBox.top());
	ocean_assert(!debugLayerBoundingBox || firstRow + numberRows <= debugLayerBoundingBox.bottomEnd());
#endif // OCEAN_DEBUG

	uint64_t rowCost;

	for (unsigned int y = firstRow; y < firstRow + numberRows; ++y)
	{
		rowCost = 0ull;

		const uint8_t* maskRow = layerI1_.mask().template constrow<uint8_t>(y);
		const PixelPosition* mappingRow = layerI_.mapping().row(y);

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (maskRow[x] != 0xFFu)
			{
				const PixelPosition& mapping = mappingRow[x];

				const unsigned int spatialCost = layerMapping.spatialCost4Neighborhood<tChannels>(x, y, mapping.x(), mapping.y(), layerMask, layerMaskPaddingElements, maxSpatialCost_);
				const unsigned int appearanceCost = layerMapping.appearanceCost5x5<tChannels, tBorderFactor>(x, y, mapping.x(), mapping.y(), layerFrame, layerMask, layerFramePaddingElements, layerMaskPaddingElements);

				rowCost += uint64_t(tWeightFactor) * uint64_t(spatialCost) + uint64_t(appearanceCost);
			}
		}

		ocean_assert(y < rowCost_.size());
		rowCost_[y] = rowCost;
	}
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_CREATOR_INFORMATION_COST_4_NEIGHBORHOOD_I_1_H
