/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/segmentation/BinPacking.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

BinPacking::Packings BinPacking::binPacking(const CV::PixelBoundingBoxes& boxes, const bool allowTransposed, unsigned int* width, unsigned int* height)
{
	ocean_assert(!boxes.empty());
	if (boxes.empty())
	{
		return Packings();
	}

	BoundingBoxIdPairs boundingBoxIdPairs;
	boundingBoxIdPairs.reserve(boxes.size());

	unsigned int maxBoxWidth = 0u;
	unsigned int maxBoxHeight = 0u;
	unsigned int totalSize = 0u;

	size_t largerWidths = 0;

	for (Index32 n = 0u; n < Index32(boxes.size()); ++n)
	{
		const CV::PixelBoundingBox& box = boxes[n];
		ocean_assert(box.isValid());

		totalSize += box.size();

		maxBoxWidth = std::max(maxBoxWidth, box.width());
		maxBoxHeight = std::max(maxBoxHeight, box.height());

		if (box.width() > box.height())
		{
			++largerWidths;
		}

		boundingBoxIdPairs.emplace_back(box, n);
	}

	std::sort(boundingBoxIdPairs.rbegin(), boundingBoxIdPairs.rend(), sortBoundingBoxIdPair);

	unsigned int initialWidth = 0u;
	unsigned int initialHeight = 0u;

	const bool stapleTopDown = largerWidths > boxes.size() / 2;

	if (stapleTopDown)
	{
		// most boxes have a bigger width than a bigger height, let's staple downwards

		initialWidth = std::max(maxBoxWidth, (unsigned int)(Numeric::sqrt(Scalar(totalSize) * Scalar(0.95)) + Scalar(0.5)));
		initialHeight = (unsigned int)(-1);
	}
	else
	{
		// let's staple to the side

		initialWidth = (unsigned int)(-1);
		initialHeight = std::max(maxBoxHeight, (unsigned int)(Numeric::sqrt(Scalar(totalSize) * Scalar(0.95)) + Scalar(0.5)));
	}

	unsigned int currentWidth = 0u;
	unsigned int currentHeight = 0u;

	Packings packings;
	packings.reserve(boxes.size());

	BoxSet emptySpaces;
	emptySpaces.emplace(CV::PixelPosition(0u, 0u), initialWidth, initialHeight);

	for (size_t nBox = 0; nBox < boundingBoxIdPairs.size(); ++nBox)
	{
		const CV::PixelBoundingBox& box = boundingBoxIdPairs[nBox].first;
		const Index32& boxIndex = boundingBoxIdPairs[nBox].second;

		CV::PixelBoundingBox adjustedBox;

		// let's find the smallest empty space

		for (BoxSet::iterator iSpace = emptySpaces.begin(); iSpace != emptySpaces.end(); ++iSpace)
		{
			const CV::PixelBoundingBox space = *iSpace;
			bool transposed = false;

			if (space.width() >= box.width() && space.height() >= box.height())
			{
				adjustedBox = box;
			}
			else if (allowTransposed && space.width() >= box.height() && space.height() >= box.width())
			{
				adjustedBox = CV::PixelBoundingBox(box.topLeft(), box.height(), box.width());
				transposed = true;
			}
			else
			{
				continue;
			}

			packings.emplace_back(boxIndex, space.topLeft(), transposed);

			currentWidth = std::max(currentWidth, space.left() + adjustedBox.width());
			currentHeight = std::max(currentHeight, space.top() + adjustedBox.height());

			emptySpaces.erase(iSpace);

			if (space.width() == adjustedBox.width() && space.height() == adjustedBox.height())
			{
				// the box fits perfectly, nothing to do here
			}
			else if (space.width() == adjustedBox.width())
			{
				ocean_assert(adjustedBox.height() < space.height());

				// the box's width is a perfect fit
				//  -------------------
				// |        box        |
				// |-------------------
				// |  remaining space  |
				//  -------------------

				const CV::PixelBoundingBox adjustedSpace(space.left(), space.top() + adjustedBox.height(), space.right(), space.bottom());

				ocean_assert(adjustedSpace.isValid());
				ocean_assert(adjustedSpace.size() + adjustedBox.size() == space.size());

				ocean_assert(emptySpaces.find(adjustedSpace) == emptySpaces.cend());
				emptySpaces.emplace(adjustedSpace);
			}
			else if (space.height() == adjustedBox.height())
			{
				ocean_assert(adjustedBox.width() < space.width());

				// the box's height is a perfect fit
				//  -----------------------
				// |         |  remaining  |
				// |   box   |             |
				// |         |   space     |
				//  -----------------------

				const CV::PixelBoundingBox adjustedSpace(space.left() + adjustedBox.width(), space.top(), space.right(), space.bottom());

				ocean_assert(adjustedSpace.isValid());
				ocean_assert(adjustedSpace.size() + adjustedBox.size() == space.size());

				ocean_assert(emptySpaces.find(adjustedSpace) == emptySpaces.cend());
				emptySpaces.emplace(adjustedSpace);
			}
			else
			{
				ocean_assert(adjustedBox.width() < space.width());
				ocean_assert(adjustedBox.height() < space.height());

				// the box is smaller than the space

				if (stapleTopDown)
				{
					//  -------------------------
					// |  box  | remaining space |
					// |-------------------------
					// |     remaining space     |
					//  -------------------------

					const CV::PixelBoundingBox adjustedSpaceTop(space.left() + adjustedBox.width(), space.top(), space.right(), space.top() + adjustedBox.height() - 1u);
					const CV::PixelBoundingBox adjustedSpaceBottom(space.left(), space.top() + adjustedBox.height(), space.right(), space.bottom());

					ocean_assert(adjustedSpaceTop.isValid() && adjustedSpaceBottom.isValid());
					ocean_assert(adjustedSpaceTop.size() + adjustedSpaceBottom.size() + adjustedBox.size() == space.size());

					ocean_assert(emptySpaces.find(adjustedSpaceTop) == emptySpaces.cend());
					ocean_assert(emptySpaces.find(adjustedSpaceBottom) == emptySpaces.cend());

					emptySpaces.emplace(adjustedSpaceTop);
					emptySpaces.emplace(adjustedSpaceBottom);
				}
				else
				{
					//  --------------------------------
					// |        box       |  remaining  |
					// |------------------              |
					// |  remaining space |    space    |
					//  --------------------------------

					const CV::PixelBoundingBox adjustedSpaceLeft(space.left(), space.top() + adjustedBox.height(), space.left() + adjustedBox.width() - 1u, space.bottom());
					const CV::PixelBoundingBox adjustedSpaceRight(space.left() + adjustedBox.width(), space.top(), space.right(), space.bottom());

					ocean_assert(adjustedSpaceLeft.isValid() && adjustedSpaceRight.isValid());
					ocean_assert(adjustedSpaceLeft.size() + adjustedSpaceRight.size() + adjustedBox.size() == space.size());

					ocean_assert(emptySpaces.find(adjustedSpaceLeft) == emptySpaces.cend());
					ocean_assert(emptySpaces.find(adjustedSpaceRight) == emptySpaces.cend());

					emptySpaces.emplace(adjustedSpaceLeft);
					emptySpaces.emplace(adjustedSpaceRight);
				}
			}

			break;
		}
	}

	ocean_assert(packings.size() == boxes.size());

	if (width != nullptr)
	{
		*width = currentWidth;
	}

	if (height != nullptr)
	{
		*height = currentHeight;
	}

	return packings;
}

}

}

}
