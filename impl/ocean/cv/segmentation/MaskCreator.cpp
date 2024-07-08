/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/segmentation/MaskCreator.h"
#include "ocean/cv/segmentation/ContourAnalyzer.h"
#include "ocean/cv/segmentation/MaskAnalyzer.h"
#include "ocean/cv/segmentation/Triangulation.h"

#include "ocean/base/Frame.h"

#include "ocean/math/Vector2.h"

#include <algorithm>

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

void MaskCreator::smoothMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const unsigned int iterations, const unsigned int incrementValue)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);
	ocean_assert(iterations != 0u && incrementValue != 0u);
	ocean_assert(iterations * incrementValue <= 255);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	uint8_t value = uint8_t(incrementValue);

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		PixelPositions outlinePixels4;
		MaskAnalyzer::findOutline4(mask, width, height, maskPaddingElements, outlinePixels4);

		for (PixelPositions::const_iterator i = outlinePixels4.begin(); i != outlinePixels4.end(); ++i)
		{
			if (i->x() < width && i->y() < height)
			{
				mask[i->y() * maskStrideElements + i->x()] = value;
			}
		}

		ocean_assert(NumericT<uint8_t>::isInsideValueRange((unsigned int)(value) + incrementValue));

		value += uint8_t(incrementValue);
	}
}

bool MaskCreator::contour2inclusiveMaskByTriangulation(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelContour& simplifiedContour, const unsigned int maskValue, Worker* worker, bool* triangulationForced)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);

	if (simplifiedContour.isEmpty())
	{
		return true;
	}

	if (simplifiedContour.boundingBox().right() >= width || simplifiedContour.boundingBox().bottom() >= height)
	{
		return false;
	}

	ocean_assert(simplifiedContour.isSimplified() && "The contour should be simplified to improve the performance");
	const Advanced::PixelTriangles triangles(Triangulation::triangulation2triangles(simplifiedContour.pixels(), Triangulation::triangulate(simplifiedContour, true, triangulationForced)));

	if (!triangles.empty())
	{
		ocean_assert(maskValue <= 255u);
		triangles2inclusiveMask(mask, width, height, maskPaddingElements, triangles.data(), triangles.size(), uint8_t(maskValue), worker);
	}

	return true;
}

void MaskCreator::denseContour2inclusiveMaskHotizontallyConvex(uint8_t* mask, const unsigned int width, const unsigned int height, const PixelPositions& densePixelPositions, const uint8_t maskValue)
{
	ocean_assert(mask && width != 0u && height != 0u);

	ocean_assert(PixelContour(densePixelPositions).isDense());

	IndexPairs32 indexPairs(height, std::make_pair(0xFFFFFFFFu, 0u));

	for (PixelPositions::const_iterator i = densePixelPositions.begin(); i != densePixelPositions.end(); ++i)
	{
		if (i->y() < height)
		{
			IndexPair32& indexPair = indexPairs[i->y()];

			if (i->x() < indexPair.first)
			{
				indexPair.first = i->x();
			}
			if (i->x() > indexPair.second)
			{
				indexPair.second = i->x();
			}
		}
	}

	const IndexPairs32& constIndexPairs = indexPairs;

	for (unsigned int y = 0u; y < constIndexPairs.size(); ++y)
	{
		const IndexPair32& indexPair = constIndexPairs[y];

		if (indexPair.first <= indexPair.second)
		{
			ocean_assert(y < height);

			const unsigned int length = (indexPair.second >= width) ? (width - indexPair.first) : (indexPair.second - indexPair.first + 1u);

			ocean_assert(indexPair.first + length <= width);
			memset(mask + y * width + indexPair.first, maskValue, length);
		}
	}
}

void MaskCreator::denseContour2inclusiveMaskOffsets(const PixelContour& denseDistinctContour, IndexGroups32& offsetGroups)
{
	ocean_assert(!denseDistinctContour.isEmpty());

	if (denseDistinctContour.pixels().size() == 1)
	{
		ocean_assert(denseDistinctContour.pixels().front().y() < offsetGroups.size());

		offsetGroups[denseDistinctContour.pixels().front().y()].push_back(denseDistinctContour.pixels().front().x());
		offsetGroups[denseDistinctContour.pixels().front().y()].push_back(denseDistinctContour.pixels().front().x());
		return;
	}

	ocean_assert(denseDistinctContour);
	ocean_assert(denseDistinctContour.isDistinct());
	ocean_assert(denseDistinctContour.isDense());

	const size_t firstIndex = denseDistinctContour.indexLeftPosition();
	const bool counterClockwise = denseDistinctContour.isCounterClockwise();

	const PixelPositions& pixels = denseDistinctContour.pixels();

	for (size_t n = firstIndex; n < firstIndex + pixels.size(); ++n)
	{
		const PixelPosition& previous = pixels[modulo(int(n) - 1, int(pixels.size()))];
		const PixelPosition& current = pixels[modulo(int(n), int(pixels.size()))];
		const PixelPosition& next = pixels[modulo(int(n) + 1, int(pixels.size()))];

		const PixelPosition::RoughPixelDirection directionMinus = PixelPosition::roughDirection(previous, current);
		const PixelPosition::RoughPixelDirection directionPlus = PixelPosition::roughDirection(current, next);

		if (previous.y() != next.y() && previous.y() != current.y() && next.y() != current.y())
		{
			ocean_assert(current.y() < offsetGroups.size());

			offsetGroups[current.y()].push_back(current.x());
			continue;
		}

		if (previous.y() != next.y() && directionMinus == PixelPosition::RPD_VERTICAL && directionPlus == PixelPosition::RPD_VERTICAL)
		{
			ocean_assert(current.y() < offsetGroups.size());

			offsetGroups[current.y()].push_back(current.x());
			continue;
		}

		if (previous.x() != next.x() && directionMinus == PixelPosition::RPD_HORIZONTAL && directionPlus == PixelPosition::RPD_HORIZONTAL)
		{
			continue;
		}

		const VectorI2 plus(next.x() - current.x(), next.y() - current.y());
		const VectorI2 minus(previous.x() - current.x(), previous.y() - current.y());

		const int product = plus.cross(minus);

		if (product == 0 || (product < 0 && counterClockwise) || (product > 0 && !counterClockwise))
		{
			ocean_assert(current.y() < offsetGroups.size());

			offsetGroups[current.y()].push_back(current.x());

			if (previous.y() == next.y() && previous.y() != current.y())
			{
				offsetGroups[current.y()].push_back(current.x());
			}

			continue;
		}
	}
}

void MaskCreator::denseContour2exclusiveMaskOffsets(const PixelContour& denseDistinctContour, IndexGroups32& offsetGroups)
{
	if (denseDistinctContour.size() <= 1)
	{
		return;
	}

	ocean_assert(denseDistinctContour);
	ocean_assert(denseDistinctContour.isDistinct());
	ocean_assert(denseDistinctContour.isDense());

	const size_t firstIndex = denseDistinctContour.indexLeftPosition();
	const bool counterClockwise = denseDistinctContour.isCounterClockwise();

	const PixelPositions& pixels = denseDistinctContour.pixels();

	for (size_t n = firstIndex; n < pixels.size() + firstIndex; ++n)
	{
		const PixelPosition& previous = pixels[modulo(int(n) - 1, int(pixels.size()))];
		const PixelPosition& current = pixels[modulo(int(n), int(pixels.size()))];
		const PixelPosition& next = pixels[modulo(int(n) + 1, int(pixels.size()))];

		const PixelPosition::RoughPixelDirection directionMinus = PixelPosition::roughDirection(previous, current);
		const PixelPosition::RoughPixelDirection directionPlus = PixelPosition::roughDirection(current, next);

		if (previous.y() != next.y() && previous.y() != current.y() && next.y() != current.y())
		{
			ocean_assert(current.y() < offsetGroups.size());

			offsetGroups[current.y()].push_back(current.x());
			continue;
		}

		if (previous.y() != next.y() && directionMinus == PixelPosition::RPD_VERTICAL && directionPlus == PixelPosition::RPD_VERTICAL)
		{
			ocean_assert(current.y() < offsetGroups.size());

			offsetGroups[current.y()].push_back(current.x());
			continue;
		}

		if (previous.x() != next.x() && directionMinus == PixelPosition::RPD_HORIZONTAL && directionPlus == PixelPosition::RPD_HORIZONTAL)
		{
			continue;
		}

		const VectorI2 plus(next.x() - current.x(), next.y() - current.y());
		const VectorI2 minus(previous.x() - current.x(), previous.y() - current.y());

		const int product = plus.cross(minus);

		if (product == 0 || (product < 0 && !counterClockwise) || (product > 0 && counterClockwise))
		{
			ocean_assert(current.y() < offsetGroups.size());

			offsetGroups[current.y()].push_back(current.x());

			if (previous.y() == next.y() && previous.y() != current.y())
			{
				offsetGroups[current.y()].push_back(current.x());
			}

			continue;
		}
	}
}

void MaskCreator::inclusiveMaskOffsets2inclusiveMask(IndexGroups32& offsetGroups, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t maskValue)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);
	ocean_assert_and_suppress_unused(offsetGroups.size() <= height, height);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	// now we fill the pixels between the individual offsets for each row
	for (unsigned int n = 0u; n < offsetGroups.size(); ++n)
	{
		if (!offsetGroups[n].empty())
		{
			Indices32& row = offsetGroups[n];

			ocean_assert(row.size() % 2 == 0);

			// the offsets need to be ordered
			if (row.size() == 2)
			{
				if (row[0] > row[1])
					std::swap(row[0], row[1]);
			}
			else
			{
				std::sort(row.begin(), row.end());
			}

			unsigned int i = 0u;
			while (i < row.size())
			{
				const unsigned int start = row[i++];
				ocean_assert(i < row.size());

				while (i + 1 < row.size() && row[i + 1] - row[i] <= 1u)
				{
					i += 2;
					ocean_assert(i < row.size());
				}

				ocean_assert(i < row.size());
				const unsigned int columns = row[i] - start + 1u;

				ocean_assert(start + columns <= width);

				memset(mask + n * maskStrideElements + start, maskValue, columns);
				++i;
			}
		}
	}
}

void MaskCreator::exclusiveMaskOffsets2exclusiveMask(IndexGroups32& offsetGroups, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t maskValue)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);
	ocean_assert_and_suppress_unused(offsetGroups.size() <= height, height);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	// now we fill the pixels between the individual offsets for each row
	for (unsigned int n = 0u; n < offsetGroups.size(); ++n)
	{
		if (!offsetGroups[n].empty())
		{
			Indices32& row = offsetGroups[n];

			ocean_assert(row.size() % 2 == 0);

			// the offsets need to be ordered
			if (row.size() == 2)
			{
				if (row[0] > row[1])
				{
					std::swap(row[0], row[1]);
				}
			}
			else
			{
				std::sort(row.begin(), row.end());
			}

			unsigned int i = 0u;
			while (i < row.size())
			{
				const int start = int(row[i++]) + 1;

				ocean_assert(i < row.size());
				const int stop = int(row[i++]) - 1;

				if (stop >= start)
				{
					ocean_assert(stop <= int(width));

					memset(mask + n * maskStrideElements + start, maskValue, stop - start + 1);
				}
			}
		}
	}
}

void MaskCreator::inclusiveMaskOffsets2inclusiveMaskXOR(IndexGroups32& offsetGroups, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t xorReference)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);
	ocean_assert_and_suppress_unused(offsetGroups.size() <= height, height);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	// now we fill the pixels between the individual offsets for each row
	for (unsigned int n = 0u; n < offsetGroups.size(); ++n)
	{
		if (!offsetGroups[n].empty())
		{
			Indices32& row = offsetGroups[n];

			ocean_assert(row.size() % 2 == 0);

			// the offsets need to be ordered
			if (row.size() == 2)
			{
				if (row[0] > row[1])
				{
					std::swap(row[0], row[1]);
				}
			}
			else
			{
				std::sort(row.begin(), row.end());
			}

			unsigned int i = 0u;
			while (i < row.size())
			{
				const unsigned int start = row[i++];
				ocean_assert(i < row.size());

				while (i + 1 < row.size() && row[i + 1] - row[i] <= 1u)
				{
					i += 2;
					ocean_assert(i < row.size());
				}

				ocean_assert(i < row.size());
				const unsigned int columns = row[i++] - start + 1u;

				ocean_assert(start + columns <= width);

				uint8_t* maskPointer = mask + n * maskStrideElements + start;
				uint8_t* const maskPointerEnd = maskPointer + columns;

				while (maskPointer != maskPointerEnd)
				{
					*maskPointer++ ^= xorReference;
				}
			}
		}
	}
}

void MaskCreator::exclusiveMaskOffsets2exclusiveMaskXOR(IndexGroups32& offsetGroups, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t xorReference)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);
	ocean_assert_and_suppress_unused(offsetGroups.size() <= height, height);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	// now we fill the pixels between the individual offsets for each row
	for (unsigned int n = 0u; n < offsetGroups.size(); ++n)
	{
		if (!offsetGroups[n].empty())
		{
			Indices32& row = offsetGroups[n];

			ocean_assert(row.size() % 2 == 0);

			// the offsets need to be ordered
			if (row.size() == 2)
			{
				if (row[0] > row[1])
				{
					std::swap(row[0], row[1]);
				}
			}
			else
			{
				std::sort(row.begin(), row.end());
			}

			unsigned int i = 0u;
			while (i < row.size())
			{
				const int start = int(row[i++]) + 1;

				ocean_assert(i < row.size());
				const int stop = int(row[i++]) - 1;

				if (stop >= start)
				{
					ocean_assert(stop <= int(width));

					uint8_t* maskPointer = mask + n * maskStrideElements + start;
					uint8_t* const maskPointerEnd = maskPointer + stop - start + 1;

					while (maskPointer != maskPointerEnd)
					{
						*maskPointer++ ^= xorReference;
					}
				}
			}
		}
	}
}

void MaskCreator::separation2mask(const uint32_t* separation, const unsigned int width, const unsigned int height, const unsigned int separationPaddingElements, const uint32_t id, uint8_t* mask, const unsigned int maskPaddingElements, const uint8_t maskValue)
{
	ocean_assert(separation != nullptr && mask != nullptr );
	ocean_assert(width != 0u && height != 0u);

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			if (*separation == id)
			{
				*mask = maskValue;
			}

			++separation;
			++mask;
		}

		separation += separationPaddingElements;
		mask += maskPaddingElements;
	}
}

void MaskCreator::separations2mask(const uint32_t* separation, const unsigned int width, const unsigned int height, const unsigned int separationPaddingElements, const uint8_t* ids, const size_t numberIds, const uint8_t maskValue, uint8_t* mask, const unsigned int maskPaddingElements)
{
	ocean_assert(separation != nullptr && mask != nullptr && ids != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert_and_suppress_unused(numberIds != 0, numberIds);

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			if (ids[*separation])
			{
				*mask = maskValue;
			}

			++separation;
			++mask;
		}

		separation += separationPaddingElements;
		mask += maskPaddingElements;
	}
}

void MaskCreator::joinMasksSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(mask != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	mask += firstRow * maskStrideElements;
	target += firstRow * targetStrideElements;

	if (maskPaddingElements == 0u && targetPaddingElements == 0u)
	{
		for (unsigned int n = 0u; n < numberRows * width; ++n)
		{
			if (mask[n] == maskValue)
			{
				target[n] = maskValue;
			}
		}
	}
	else
	{
		for (unsigned int n = 0u; n < numberRows; ++n)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				if (mask[x] == maskValue)
				{
					target[x] = maskValue;
				}
			}

			mask += maskStrideElements;
			target += targetStrideElements;
		}
	}
}

}

}

}
