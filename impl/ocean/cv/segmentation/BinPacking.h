/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_BIN_PACKING_H
#define META_OCEAN_CV_SEGMENTATION_BIN_PACKING_H

#include "ocean/cv/segmentation/Segmentation.h"

#include "ocean/cv/PixelBoundingBox.h"
#include "ocean/cv/PixelPosition.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

/**
 * This class implements bin packing algorithms.
 * @ingroup cvsegmentation
 */
class OCEAN_CV_SEGMENTATION_EXPORT BinPacking
{
	public:

		/**
		 * This class provides the relevant packing information for a 2D box with pixel precision.
		 */
		class Packing
		{
			public:

				/**
				 * Default constructor creating an invalid packing information.
				 */
				Packing() = default;

				/**
				 * Creates a new packing information for a 2D box with pixel precision.
				 * @param boxIndex The index of the box
				 * @param topLeft The location of the top left corner of the packed 2D box
				 * @param transposed True, if the box needs to be transposed; False, if the box is packed as given
				 */
				inline Packing(const Index32 boxIndex, const CV::PixelPosition& topLeft, const bool transposed);

				/**
				 * Returns the index of the packed 2D box.
				 * @return The index of the corresponding box
				 */
				inline Index32 boxIndex() const;

				/**
				 * Returns the location of the top left corner of the packed 2D box.
				 * @return The top left corner of the corresponding box
				 */
				inline const CV::PixelPosition topLeft() const;

				/**
				 * Returns whether the packed 2D box needs to be transposed.
				 * @return True, if so
				 */
				inline bool transposed() const;

			protected:

				/// The index of the box.
				const Index32 boxIndex_ = Index32(-1);

				/// The location of the top left corner of the packed 2D box.
				const CV::PixelPosition topLeft_;

				/// True, if the box needs to be transposed; False, if the box is packed as given.
				const bool transposed_ = false;
		};

		/**
		 * Definition of a vector holding Packing objects.
		 */
		typedef std::vector<Packing> Packings;

	protected:

		/**
		 * Definition of a pair combining a 2D box with an id.
		 */
		typedef std::pair<CV::PixelBoundingBox, Index32> BoundingBoxIdPair;

		/**
		 * Definition of a vector holding box id pairs.
		 */
		typedef std::vector<BoundingBoxIdPair> BoundingBoxIdPairs;

		/**
		 * Definition of an ordered set holding 2D boxes with pixel accuracy.
		 */
		typedef std::set<CV::PixelBoundingBox, BinPacking> BoxSet;

	public:

		/**
		 * Invokes a bin packing for given 2D boxes with pixel precision.
		 * @param boxes The 2D boxed to be packed
		 * @param allowTransposed True, if boxes can be transposed during packing; False, to not allow transposing boxes
		 * @param width Optional resulting width the packed area, in pixel, nullptr if not of interest
		 * @param height Optional resulting height the packed area, in pixel, nullptr if not of interest
		 * @return The resulting packing information, one packing for each box
		 */
		static Packings binPacking(const CV::PixelBoundingBoxes& boxes, const bool allowTransposed, unsigned int* width = nullptr, unsigned int* height = nullptr);

		/**
		 * Compares the size of two boxes and returns whether the first box is smaller than the second box.
		 * In case both boxes have the same size, the location of the box is used for comparison as well.
		 * @param boxA The first box to compare
		 * @param boxB The second box to compare
		 * @return True, if the first box is smaller than the second box
		 */
		inline bool operator()(const CV::PixelBoundingBox& boxA, const CV::PixelBoundingBox& boxB) const;

	protected:

		/**
		 * Compares two pairs of bounding boxes with ids and returns whether the left pair as a lower id than the second pair.
		 * @param boundingBoxIdPairA The first box to compare
		 * @param boundingBoxIdPairB The second box to compare
		 * @return True, if so
		 */
		static inline bool sortBoundingBoxIdPair(const BoundingBoxIdPair& boundingBoxIdPairA, const BoundingBoxIdPair& boundingBoxIdPairB);
};

inline BinPacking::Packing::Packing(const Index32 boxIndex, const CV::PixelPosition& topLeft, const bool transposed) :
	boxIndex_(boxIndex),
	topLeft_(topLeft),
	transposed_(transposed)
{
	// nothing to do here
}

inline Index32 BinPacking::Packing::boxIndex() const
{
	return boxIndex_;
}

inline const CV::PixelPosition BinPacking::Packing::topLeft() const
{
	return topLeft_;
}

inline bool BinPacking::Packing::transposed() const
{
	return transposed_;
}

inline bool BinPacking::operator()(const CV::PixelBoundingBox& boxA, const CV::PixelBoundingBox& boxB) const
{
	const uint64_t sizeA = uint64_t(boxA.width()) * uint64_t(boxA.height());
	const uint64_t sizeB = uint64_t(boxB.width()) * uint64_t(boxB.height());

	if (sizeA == sizeB)
	{
		return boxA.left() < boxB.left() || (boxA.left() == boxB.left() && boxA.top() < boxB.top());
	}

	return sizeA < sizeB;
}

inline bool BinPacking::sortBoundingBoxIdPair(const BoundingBoxIdPair& boundingBoxIdPairA, const BoundingBoxIdPair& boundingBoxIdPairB)
{
	return boundingBoxIdPairA.first.size() < boundingBoxIdPairB.first.size();
}

}

}

}

#endif // META_OCEAN_CV_SEGMENTATION_BIN_PACKING_H
