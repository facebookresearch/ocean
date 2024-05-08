/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Utilities.h"

#include <algorithm>

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

void MaskAnalyzer::analyzeMaskSeparation8Bit(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, uint32_t* separation, const unsigned int separationPaddingElements, MaskBlocks& blocks)
{
	ocean_assert(mask != nullptr && separation != nullptr);
	ocean_assert(width != 0u && height != 0u);

	ocean_assert(blocks.empty());

	if (separationPaddingElements == 0u)
	{
		memset(separation, 0x00, sizeof(uint32_t) * width * height);
	}
	else
	{
		uint32_t* separationRow = separation;

		for (unsigned int y = 0u; y < height; ++y)
		{
			memset(separationRow, 0x00, sizeof(uint32_t) * width);
			separationRow += separationPaddingElements;
		}
	}

	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int separationStrideElements = width + separationPaddingElements;

	Frame stateFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	stateFrame.setValue(0x00);

	ocean_assert(stateFrame.isContinuous());
	uint8_t* state = stateFrame.data<uint8_t>();

	uint8_t* const stateBegin = state;
	const uint8_t* const maskBegin = mask;

	PixelPositions candidates;
	candidates.reserve(width * height / 4u);

	uint32_t id = 1u;

	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			// if this pixel is a mask pixel and has not been assigned to a block
			if (*mask != 0xFF && *state == 0x00)
			{
				uint32_t* ids = separation + y * separationStrideElements + x;

				*state = 0xFF;
				*ids = id;
				unsigned int size = 1u;
				bool intersectBorder = false;

				ocean_assert(candidates.empty());

				// bottom
				if (y + 1u < height && *(state + width) == 0x00)
				{
					*(state + width) = 0x80;
					candidates.emplace_back(x, y + 1u);
				}

				// right
				if (x + 1u < width && *(state + 1) == 0x00)
				{
					*(state + 1) = 0x80;
					candidates.emplace_back(x + 1u, y);
				}

				// left
				if (x > 0u && *(state - 1) == 0x00)
				{
					*(state - 1) = 0x80;
					candidates.emplace_back(x - 1u, y);
				}

				// top
				if (y > 0u && *(state - width) == 0x00)
				{
					*(state - width) = 0x80;
					candidates.emplace_back(x, y - 1u);
				}

				while (!candidates.empty())
				{
					const PixelPosition candidate = candidates.back();
					candidates.pop_back();

					const unsigned int stateIndex = candidate.y() * width + candidate.x();

					if (maskBegin[candidate.y() * maskStrideElements + candidate.x()] != 0xFF && stateBegin[stateIndex] != 0xFF)
					{
						stateBegin[stateIndex] = 0xFF;
						separation[candidate.y() * separationStrideElements + candidate.x()] = id;
						++size;

						intersectBorder = intersectBorder || candidate.x() == 0u || candidate.y() == 0 || candidate.x() == width - 1u || candidate.y() == height - 1u;

						// bottom
						if (candidate.y() + 1u < height && stateBegin[stateIndex + width] == 0x00)
						{
							stateBegin[stateIndex + width] = 0x80;
							candidates.emplace_back(candidate.x(), candidate.y() + 1u);
						}

						// right
						if (candidate.x() + 1u < width && stateBegin[stateIndex + 1u] == 0x00)
						{
							stateBegin[stateIndex + 1u] = 0x80;
							candidates.emplace_back(candidate.x() + 1u, candidate.y());
						}

						// left
						if (candidate.x() > 0u && stateBegin[stateIndex - 1u] == 0x00)
						{
							stateBegin[stateIndex - 1u] = 0x80;
							candidates.emplace_back(candidate.x() - 1u, candidate.y());
						}

						// top
						if (candidate.y() > 0u && stateBegin[stateIndex - width] == 0x00)
						{
							stateBegin[stateIndex - width] = 0x80;
							candidates.emplace_back(candidate.x(), candidate.y() - 1u);
						}
					}
				}

				blocks.emplace_back(PixelPosition(x, y), id, size, intersectBorder);
				++id;
			}

			++mask;
			++state;
		}

		mask += maskPaddingElements;
		ocean_assert(stateFrame.isContinuous());
	}
}

void MaskAnalyzer::analyzeNonMaskSeparation8Bit(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, uint32_t* separation, const unsigned int separationPaddingElements, MaskBlocks& blocks)
{
	ocean_assert(mask != nullptr && separation != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(blocks.empty());

	if (separationPaddingElements == 0u)
	{
		memset(separation, 0x00, sizeof(uint32_t) * width * height);
	}
	else
	{
		uint32_t* separationRow = separation;

		for (unsigned int y = 0u; y < height; ++y)
		{
			memset(separationRow, 0x00, sizeof(uint32_t) * width);
			separationRow += separationPaddingElements;
		}
	}

	const unsigned int maskStrideElements = width + maskPaddingElements;
	const unsigned int separationStrideElements = width + separationPaddingElements;

	Frame stateFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	stateFrame.setValue(0x00);

	ocean_assert(stateFrame.isContinuous());

	uint8_t* state = stateFrame.data<uint8_t>();

	uint8_t* const stateBegin = state;
	const uint8_t* const maskBegin = mask;

	PixelPositions candidates;
	candidates.reserve(width * height);

	uint32_t id = 1u;

	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			// if this pixel is a mask pixel and has not been assigned to a block
			if (*mask == 0xFF && *state != 0xFF)
			{
				uint32_t* ids = separation + y * separationStrideElements + x;

				*state = 0xFF;
				*ids = id;
				unsigned int size = 1u;
				bool intersectBorder = false;

				ocean_assert(candidates.empty());

				// bottom
				if (y + 1u < height && *(state + width) == 0x00)
				{
					*(state + width) = 0x80;
					candidates.push_back(PixelPosition(x, y + 1u));
				}

				// right
				if (x + 1u < width && *(state + 1) == 0x00)
				{
					*(state + 1) = 0x80;
					candidates.push_back(PixelPosition(x + 1u, y));
				}

				// left
				if (x > 0u && *(state - 1) == 0x00)
				{
					*(state - 1) = 0x80;
					candidates.push_back(PixelPosition(x - 1u, y));
				}

				// top
				if (y > 0u && *(state - width) == 0x00)
				{
					*(state - width) = 0x80;
					candidates.push_back(PixelPosition(x, y - 1u));
				}

				while (!candidates.empty())
				{
					const PixelPosition candidate = candidates.back();
					candidates.pop_back();

					const unsigned int stateIndex = candidate.y() * width + candidate.x();

					if (maskBegin[candidate.y() * maskStrideElements + candidate.x()] == 0xFF && stateBegin[stateIndex] != 0xFF)
					{
						stateBegin[stateIndex] = 0xFF;
						separation[candidate.y() * separationStrideElements + candidate.x()] = id;
						++size;

						intersectBorder = intersectBorder || candidate.x() == 0u || candidate.y() == 0 || candidate.x() == width - 1u || candidate.y() == height - 1u;

						// bottom
						if (candidate.y() + 1u < height && stateBegin[stateIndex + width] == 0x00)
						{
							stateBegin[stateIndex + width] = 0x80;
							candidates.push_back(PixelPosition(candidate.x(), candidate.y() + 1u));
						}

						// right
						if (candidate.x() + 1u < width && stateBegin[stateIndex + 1u] == 0x00)
						{
							stateBegin[stateIndex + 1u] = 0x80;
							candidates.push_back(PixelPosition(candidate.x() + 1u, candidate.y()));
						}

						// left
						if (candidate.x() > 0u && stateBegin[stateIndex - 1u] == 0x00)
						{
							stateBegin[stateIndex - 1u] = 0x80;
							candidates.push_back(PixelPosition(candidate.x() - 1u, candidate.y()));
						}

						// top
						if (candidate.y() > 0u && stateBegin[stateIndex - width] == 0x00)
						{
							stateBegin[stateIndex - width] = 0x80;
							candidates.push_back(PixelPosition(candidate.x(), candidate.y() - 1u));
						}
					}
				}

				blocks.emplace_back(PixelPosition(x, y), id, size, intersectBorder);
				++id;
			}

			++mask;
			++state;
		}

		mask += maskPaddingElements;
		ocean_assert(stateFrame.isContinuous());
	}
}

void MaskAnalyzer::findOutline4(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions& outlinePixels4, const PixelBoundingBox& boundingBox, const uint8_t nonMaskValue)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);

	ocean_assert(outlinePixels4.empty());

	const unsigned int maskStrideElements = width + maskPaddingElements;

	if (boundingBox.isValid() && !CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), width, height).isInside(boundingBox))
	{
		ocean_assert(false && "Invalid bounding box!");
		return;
	}

	const unsigned int width_1 = width - 1u;
	const unsigned int height_1 = height - 1u;

	unsigned int firstColumn = 0u;
	unsigned int firstRow = 0u;

	unsigned int endColumn = width;
	unsigned int endRow = height;

	if (boundingBox.isValid())
	{
		firstColumn = boundingBox.left();
		firstRow = boundingBox.top();

		endColumn = boundingBox.rightEnd();
		endRow = boundingBox.bottomEnd();
	}

	// first row

	if (firstRow == 0u)
	{
		const uint8_t* maskRow = mask;
		const uint8_t* maskRowBottom = mask + maskStrideElements;

		// top left corner

		if (firstColumn == 0u)
		{
			if (maskRow[0] != nonMaskValue)
			{
				// we have outline pixels outside of the frame

				outlinePixels4.emplace_back(0u, (unsigned int)(-1));
				outlinePixels4.emplace_back((unsigned int)(-1), 0u);
			}
			else
			{
				// the pixel may be an outline pixel

				if (maskRow[1] != nonMaskValue || maskRowBottom[0] != nonMaskValue)
				{
					outlinePixels4.emplace_back(0u, 0u);
				}
			}
		}

		// top center pixels

		for (unsigned int x = std::max(1u, firstColumn); x < std::min(endColumn, width_1); ++x)
		{
			if (maskRow[x] != nonMaskValue)
			{
				// we have outline pixels outside of the frame

				outlinePixels4.emplace_back(x, (unsigned int)(-1));
			}
			else
			{
				// the pixel may be an outline pixel

				if (maskRow[x - 1u] != nonMaskValue || maskRow[x + 1u] != nonMaskValue || maskRowBottom[x] != nonMaskValue)
				{
					outlinePixels4.emplace_back(x, 0u);
				}
			}
		}

		// top right corner

		if (endColumn == width)
		{
			if (maskRow[width_1] != nonMaskValue)
			{
				// we have outline pixels outside of the frame

				outlinePixels4.emplace_back(width_1, (unsigned int)(-1));
				outlinePixels4.emplace_back(width, 0u);
			}
			else
			{
				// the pixel may be an outline pixel

				if (maskRow[width_1 - 1u] != nonMaskValue || maskRowBottom[width_1] != nonMaskValue)
				{
					outlinePixels4.emplace_back(width_1, 0u);
				}
			}
		}
	}

	// center rows

	const uint8_t* maskRow = mask + std::max(1u, firstRow) * maskStrideElements;
	const uint8_t* maskRowTop = maskRow - maskStrideElements;
	const uint8_t* maskRowBottom = maskRow + maskStrideElements;

	for (unsigned int y = std::max(1u, firstRow); y < std::min(endRow, height_1); ++y)
	{
		ocean_assert(maskRowTop >= mask);

		// top left corner

		if (firstColumn == 0u)
		{
			if (maskRow[0] != nonMaskValue)
			{
				// we have outline pixels outside of the frame

				outlinePixels4.emplace_back((unsigned int)(-1), y);
			}
			else
			{
				// the pixel may be an outline pixel

				if (maskRow[1] != nonMaskValue || maskRowTop[0] != nonMaskValue || maskRowBottom[0] != nonMaskValue)
				{
					outlinePixels4.emplace_back(0u, y);
				}
			}
		}

		// top center pixels

		for (unsigned int x = std::max(1u, firstColumn); x < std::min(endColumn, width_1); ++x)
		{
			if (maskRow[x - 1u] != nonMaskValue || maskRow[x + 1u] != nonMaskValue || maskRowTop[x] != nonMaskValue || maskRowBottom[x] != nonMaskValue)
			{
				outlinePixels4.emplace_back(x, y);
			}
		}

		// top right corner

		if (endColumn == width)
		{
			if (maskRow[width_1] != nonMaskValue)
			{
				// we have outline pixels outside of the frame

				outlinePixels4.emplace_back(width, y);
			}
			else
			{
				// the pixel may be an outline pixel

				if (maskRow[width_1 - 1u] != nonMaskValue || maskRowTop[width_1] != nonMaskValue || maskRowBottom[width_1] != nonMaskValue)
				{
					outlinePixels4.emplace_back(width_1, y);
				}
			}
		}

		maskRowTop = maskRow;
		maskRow = maskRowBottom;
		maskRowBottom += maskStrideElements;
	}

	// last row

	if (endRow == height)
	{
		maskRow = mask + height_1 * maskStrideElements;
		maskRowTop = maskRow - maskStrideElements;

		// bottom left corner

		if (firstColumn == 0u)
		{
			if (maskRow[0] != nonMaskValue)
			{
				// we have outline pixels outside of the frame

				outlinePixels4.emplace_back(0u, height);
				outlinePixels4.emplace_back((unsigned int)(-1), height_1);
			}
			else
			{
				// the pixel may be an outline pixel

				if (maskRow[1] != nonMaskValue || maskRowTop[0] != nonMaskValue)
				{
					outlinePixels4.emplace_back(0u, height_1);
				}
			}
		}

		// bottom center pixels

		for (unsigned int x = std::max(1u, firstColumn); x < std::min(endColumn, width_1); ++x)
		{
			if (maskRow[x] != nonMaskValue)
			{
				// we have outline pixels outside of the frame

				outlinePixels4.emplace_back(x, height);
			}
			else
			{
				// the pixel may be an outline pixel

				if (maskRow[x - 1u] != nonMaskValue || maskRow[x + 1u] != nonMaskValue || maskRowTop[x] != nonMaskValue)
				{
					outlinePixels4.emplace_back(x, height_1);
				}
			}
		}

		// bottom right corner

		if (endColumn == width)
		{
			if (maskRow[width_1] != nonMaskValue)
			{
				// we have outline pixels outside of the frame

				outlinePixels4.emplace_back(width_1, height);
				outlinePixels4.emplace_back(width, height_1);
			}
			else
			{
				// the pixel may be an outline pixel

				if (maskRow[width_1 - 1u] != nonMaskValue || maskRowTop[width_1] != nonMaskValue)
				{
					outlinePixels4.emplace_back(width_1, height_1);
				}
			}
		}
	}
}

void MaskAnalyzer::findBorderPixels4(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions& borderPixels, const PixelBoundingBox& boundingBox, Worker* worker, const uint8_t nonMaskValue)
{
	ocean_assert(mask && width != 0u && height != 0u);
	ocean_assert(borderPixels.empty());

	const unsigned int firstColumn = boundingBox ? boundingBox.left() : 0u;
	const unsigned int numberColumns = boundingBox ? boundingBox.width() : width;

	const unsigned int firstRow = boundingBox ? boundingBox.top() : 0u;
	const unsigned int numberRows = boundingBox ? boundingBox.height() : height;

	if (worker)
	{
		std::vector<PixelPositions> pixelPositionsArray(worker->threads());

		worker->executeFunction(Worker::Function::createStatic(&MaskAnalyzer::findBorderPixels4Subset, mask, width, height, maskPaddingElements, pixelPositionsArray.data(), nonMaskValue, firstColumn, numberColumns, 0u, 0u, 0u), firstRow, numberRows, 8u, 9u, 20u, 10u);

		unsigned int total = 0u;
		for (unsigned int n = 0u; n < pixelPositionsArray.size(); ++n)
		{
			total += (unsigned int)(pixelPositionsArray[n].size());
		}

		borderPixels.reserve(total);

		for (unsigned int n = 0u; n < pixelPositionsArray.size(); ++n)
		{
			borderPixels.insert(borderPixels.end(), pixelPositionsArray[n].begin(), pixelPositionsArray[n].end());
		}
	}
	else
	{
		findBorderPixels4Subset(mask, width, height, maskPaddingElements, &borderPixels, nonMaskValue, firstColumn, numberColumns, firstRow, numberRows, 0u);
	}
}

void MaskAnalyzer::findBorderPixels8(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions& borderPixels, const PixelBoundingBox& boundingBox, Worker* worker, const uint8_t nonMaskValue)
{
	ocean_assert(mask && width != 0u && height != 0u);
	ocean_assert(borderPixels.empty());

	const unsigned int firstColumn = boundingBox ? boundingBox.left() : 0u;
	const unsigned int numberColumns = boundingBox ? boundingBox.width() : width;

	const unsigned int firstRow = boundingBox ? boundingBox.top() : 0u;
	const unsigned int numberRows = boundingBox ? boundingBox.height() : height;

	if (worker)
	{
		std::vector<PixelPositions> pixelPositionsArray(worker->threads());

		worker->executeFunction(Worker::Function::createStatic(&MaskAnalyzer::findBorderPixels8Subset, mask, width, height, maskPaddingElements, pixelPositionsArray.data(), nonMaskValue, firstColumn, numberColumns, 0u, 0u, 0u), firstRow, numberRows, 8u, 9u, 20u, 10u);

		unsigned int total = 0u;
		for (unsigned int n = 0u; n < pixelPositionsArray.size(); ++n)
		{
			total += (unsigned int)(pixelPositionsArray[n].size());
		}

		borderPixels.reserve(total);

		for (unsigned int n = 0u; n < pixelPositionsArray.size(); ++n)
		{
			borderPixels.insert(borderPixels.end(), pixelPositionsArray[n].begin(), pixelPositionsArray[n].end());
		}
	}
	else
	{
		findBorderPixels8Subset(mask, width, height, maskPaddingElements, &borderPixels, nonMaskValue, firstColumn, numberColumns, firstRow, numberRows, 0u);
	}
}

void MaskAnalyzer::findNonUniquePixels4(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions& nonUniquePixels, const PixelBoundingBox& boundingBox)
{
	ocean_assert(mask != nullptr);
	ocean_assert(width >= 2u && height >= 2u);

	ocean_assert(nonUniquePixels.empty());

	const unsigned int maskStrideElements = width + maskPaddingElements;

	if (boundingBox.isValid() && !CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), width, height).isInside(boundingBox))
	{
		ocean_assert(false && "Invalid bounding box!");
		return;
	}

	const unsigned int width_1 = width - 1u;
	const unsigned int height_1 = height - 1u;

	unsigned int firstColumn = 0u;
	unsigned int firstRow = 0u;

	unsigned int endColumn = width;
	unsigned int endRow = height;

	if (boundingBox.isValid())
	{
		firstColumn = boundingBox.left();
		firstRow = boundingBox.top();

		endColumn = boundingBox.rightEnd();
		endRow = boundingBox.bottomEnd();
	}

	// handling first row

	if (firstRow == 0u)
	{
		const uint8_t* const maskRow = mask;
		const uint8_t* const maskRowBottom = mask + maskStrideElements;

		// left pixel

		if (firstColumn == 0u)
		{
			const uint8_t value = mask[0];

			if (value != maskRow[1] || value != maskRowBottom[0])
			{
				nonUniquePixels.emplace_back(0u, 0u);
			}
		}

		// center pixels

		for (unsigned int x = std::max(1u, firstColumn); x < std::min(endColumn,  width_1); ++x)
		{
			const uint8_t value = maskRow[x];

			if (value != maskRow[x - 1u] || value != maskRow[x + 1u] || value != maskRowBottom[x])
			{
				nonUniquePixels.emplace_back(x, 0u);
			}
		}

		// right pixel

		if (endColumn == width)
		{
			const uint8_t value = maskRow[width_1];

			if (value != maskRow[width_1 - 1u] || value != maskRowBottom[width_1])
			{
				nonUniquePixels.emplace_back(width_1, 0u);
			}
		}
	}

	// center rows

	const uint8_t* maskRow = mask + std::max(1u, firstRow) * maskStrideElements;
	const uint8_t* maskRowTop = maskRow - maskStrideElements;
	const uint8_t* maskRowBottom = maskRow + maskStrideElements;

	for (unsigned int y = std::max(1u, firstRow); y < std::min(endRow, height_1); ++y)
	{
		if (firstColumn == 0u)
		{
			const uint8_t value = maskRow[0];

			if (value != maskRow[1] || value != maskRowTop[0] || value != maskRowBottom[0])
			{
				nonUniquePixels.emplace_back(0u, y);
			}
		}

		for (unsigned int x = std::max(1u, firstColumn); x < std::min(endColumn,  width_1); ++x)
		{
			const uint8_t value = maskRow[x];

			if (value != maskRow[x - 1u] || value != maskRow[x + 1u] || value != maskRowTop[x] || value != maskRowBottom[x])
			{
				nonUniquePixels.emplace_back(x, y);
			}
		}

		if (endColumn == width)
		{
			const uint8_t value = maskRow[width_1];

			if (value != maskRow[width_1 - 1u] || value != maskRowTop[width_1] || value != maskRowBottom[width_1])
			{
				nonUniquePixels.emplace_back(width_1, y);
			}
		}

		maskRowTop = maskRow;
		maskRow = maskRowBottom;
		maskRowBottom += maskStrideElements;
	}

	// handling last row

	if (endRow == height)
	{
		maskRow = mask + height_1 * maskStrideElements;
		maskRowTop = maskRow - maskStrideElements;

		// left pixel

		if (firstColumn == 0u)
		{
			const uint8_t value = maskRow[0];

			if (value != maskRow[1] || value != maskRowTop[0])
			{
				nonUniquePixels.emplace_back(0u, height_1);
			}
		}

		// center pixels

		for (unsigned int x = std::max(1u, firstColumn); x < std::min(endColumn,  width_1); ++x)
		{
			const uint8_t value = maskRow[x];

			if (value != maskRow[x - 1u] || value != maskRow[x + 1u] || value != maskRowTop[x])
			{
				nonUniquePixels.emplace_back(x, height_1);
			}
		}

		// right pixel

		if (endColumn == width)
		{
			const uint8_t value = maskRow[width_1];

			if (value != maskRow[width_1 - 1u] || value != maskRowTop[width_1])
			{
				nonUniquePixels.emplace_back(width_1, height_1);
			}
		}
	}
}

void MaskAnalyzer::findNonUniquePixels8(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions& nonUniquePixels, const PixelBoundingBox& boundingBox)
{
	ocean_assert(mask != nullptr);
	ocean_assert(width >= 2u && height >= 2u);

	ocean_assert(nonUniquePixels.empty());

	const unsigned int maskStrideElements = width + maskPaddingElements;

	if (boundingBox.isValid() && !CV::PixelBoundingBox(CV::PixelPosition(0u, 0u), width, height).isInside(boundingBox))
	{
		ocean_assert(false && "Invalid bounding box!");
		return;
	}

	const unsigned int width_1 = width - 1u;
	const unsigned int height_1 = height - 1u;

	unsigned int firstColumn = 0u;
	unsigned int firstRow = 0u;

	unsigned int endColumn = width;
	unsigned int endRow = height;

	if (boundingBox.isValid())
	{
		firstColumn = boundingBox.left();
		firstRow = boundingBox.top();

		endColumn = boundingBox.rightEnd();
		endRow = boundingBox.bottomEnd();
	}

	// handling first row

	if (firstRow == 0u)
	{
		const uint8_t* const maskRow = mask;
		const uint8_t* const maskRowBottom = mask + maskStrideElements;

		// left pixel

		if (firstColumn == 0u)
		{
			const uint8_t value = maskRow[0];

			if (value != maskRow[1] || value != maskRowBottom[0] || value != maskRowBottom[1])
			{
				nonUniquePixels.emplace_back(0u, 0u);
			}
		}

		// center pixels

		for (unsigned int x = std::max(1u, firstColumn); x < std::min(endColumn,  width_1); ++x)
		{
			const uint8_t value = mask[x];

			if (value != maskRow[x - 1u] || value != maskRow[x + 1u] || value != maskRowBottom[x - 1u] || value != maskRowBottom[x] || value != maskRowBottom[x + 1u])
			{
				nonUniquePixels.emplace_back(x, 0u);
			}
		}

		// right pixel

		if (endColumn == width)
		{
			const uint8_t value = maskRow[width_1];

			if (value != maskRow[width_1 - 1u] || value != maskRowBottom[width_1] || value != maskRowBottom[width_1 - 1u])
			{
				nonUniquePixels.emplace_back(width_1, 0u);
			}
		}
	}

	// center rows

	const uint8_t* maskRow = mask + std::max(1u, firstRow) * maskStrideElements;
	const uint8_t* maskRowTop = maskRow - maskStrideElements;
	const uint8_t* maskRowBottom = maskRow + maskStrideElements;

	for (unsigned int y = std::max(1u, firstRow); y < std::min(endRow, height_1); ++y)
	{
		if (firstColumn == 0u)
		{
			const uint8_t value = maskRow[0];

			if (value != maskRow[1] || value != maskRowTop[0] || value != maskRowTop[1] || value != maskRowBottom[0] || value != maskRowBottom[1])
			{
				nonUniquePixels.emplace_back(0u, y);
			}
		}

		for (unsigned int x = std::max(1u, firstColumn); x < std::min(endColumn,  width_1); ++x)
		{
			const uint8_t value = maskRow[x];

			if (value != maskRow[x - 1u] || value != maskRow[x + 1u] || value != maskRowTop[x - 1u] || value != maskRowTop[x] || value != maskRowTop[x + 1u] || value != maskRowBottom[x - 1u] || value != maskRowBottom[x + 1u])
			{
				nonUniquePixels.emplace_back(x, y);
			}
		}

		if (endColumn == width)
		{
			const uint8_t value = maskRow[width_1];

			if (value != maskRow[width_1 - 1u] || value != maskRowTop[width_1 - 1u] || value != maskRowTop[width_1] || value != maskRowBottom[width_1] || value != maskRowBottom[width_1 - 1u])
			{
				nonUniquePixels.emplace_back(width_1, y);
			}
		}

		maskRowTop = maskRow;
		maskRow = maskRowBottom;
		maskRowBottom += maskStrideElements;
	}

	// handling last row

	if (endRow == height)
	{
		maskRow = mask + height_1 * maskStrideElements;
		maskRowTop = maskRow - maskStrideElements;

		// left pixel

		if (firstColumn == 0u)
		{
			const uint8_t value = maskRow[0];

			if (value != maskRow[1] || value != maskRowTop[0] || value != maskRowTop[1])
			{
				nonUniquePixels.emplace_back(0u, height_1);
			}
		}

		// center pixels

		for (unsigned int x = std::max(1u, firstColumn); x < std::min(endColumn,  width_1); ++x)
		{
			const uint8_t value = maskRow[x];

			if (value != maskRow[x - 1u] || value != maskRow[x + 1u] || value != maskRowTop[x - 1u] || value != maskRowTop[x] || value != maskRowTop[x + 1u])
			{
				nonUniquePixels.emplace_back(x, height_1);
			}
		}

		// right pixel

		if (endColumn == width)
		{
			const uint8_t value = maskRow[width_1];

			if (value != maskRow[width_1 - 1u] || value != maskRowTop[width_1 - 1u] || value != maskRowTop[width_1])
			{
				nonUniquePixels.emplace_back(width_1, height_1);
			}
		}
	}
}

bool MaskAnalyzer::pixels2contour(const PixelPositions& pixels, const unsigned int width, const unsigned int height, PixelPositions& contour, PixelPositions* remainingPixels)
{
	ocean_assert(width != 0u && height != 0u);

	ocean_assert(!pixels.empty());
	if (pixels.empty())
		return false;

	// we store the provided pixels in a set ensuring that the locations are sorted, unique and that they can be sought in O(log n)

	const PixelPositionSet outlineSet(pixels.begin(), pixels.end());
	ocean_assert(outlineSet.size() == pixels.size());

	if (outlineSet.size() == 1)
	{
		contour = pixels;
		return true;
	}

	// first we determined the most left pixel (with largest y value)
	unsigned int left = (unsigned int)(-1);
	unsigned int bottom = 0u;

	PixelPosition startPoint;
	for (PixelPositions::const_iterator i = pixels.begin(); i != pixels.end(); ++i)
		if (i->x() < left || (i->x() == left && i->y() > bottom))
		{
			left = i->x();
			bottom = i->y();
			startPoint = *i;
		}

	PixelPositions tmpContour;
	tmpContour.reserve(pixels.size());

	tmpContour.push_back(startPoint);

	// we start at the most left pixel and begin processing in south direction to determine the next following pixel and so on
	ProcessDirection direction = PD_S;

	bool failed = false;

	while (!failed)
	{
		const PixelPosition& last = tmpContour.back();

		// we check whether we have reached the start pixel so that the contour has been closed so that we can stop

		if (last == startPoint && tmpContour.size() > 1)
		{
			tmpContour.pop_back();
			break;
		}

		if (tmpContour.size() > 3 * outlineSet.size())
		{
			ocean_assert(false && "Invalid outline");
			contour = tmpContour;
			return false;
		}

		// the contour is not closed yet so we follow the contour

		switch (direction)
		{
			case PD_W:
			{
				/**
				 * new:
				 * | <----
				 * | 2 1 0
				 * | 3 X 7
				 * | 4 5 6
				 */

				if (outlineSet.find(last.northEast()) != outlineSet.end()) // NE
				{
					tmpContour.push_back(last.northEast());
					direction = PD_NE;
				}
				else if (outlineSet.find(last.north()) != outlineSet.end()) // N
				{
					tmpContour.push_back(last.north());
					direction = PD_N;
				}
				else if (outlineSet.find(last.northWest()) != outlineSet.end()) // NW
				{
					tmpContour.push_back(last.northWest());
					direction = PD_NW;
				}
				else if (outlineSet.find(last.west()) != outlineSet.end()) // W
				{
					tmpContour.push_back(last.west());
					direction = PD_W;
				}
				else if (outlineSet.find(last.southWest()) != outlineSet.end()) // SW
				{
					tmpContour.push_back(last.southWest());
					direction = PD_SW;
				}
				else if (outlineSet.find(last.south()) != outlineSet.end()) // S
				{
					tmpContour.push_back(last.south());
					direction = PD_S;
				}
				else if (outlineSet.find(last.southEast()) != outlineSet.end()) // SE
				{
					tmpContour.push_back(last.southEast());
					direction = PD_SE;
				}
				else if (outlineSet.find(last.east()) != outlineSet.end()) // E
				{
					tmpContour.push_back(last.east());
					direction = PD_E;
				}
				else
				{
					failed = true;
				}

				break;
			};

			case PD_SW:
			{
				/**
				 * new:
				 * |     |
				 * |   |
				 * | V
				 * | 1 0 7
				 * | 2 X 6
				 * | 3 4 5
				 */

				if (outlineSet.find(last.north()) != outlineSet.end()) // N
				{
					tmpContour.push_back(last.north());
					direction = PD_N;
				}
				else if (outlineSet.find(last.northWest()) != outlineSet.end()) // NW
				{
					tmpContour.push_back(last.northWest());
					direction = PD_NW;
				}
				else if (outlineSet.find(last.west()) != outlineSet.end()) // W
				{
					tmpContour.push_back(last.west());
					direction = PD_W;
				}
				else if (outlineSet.find(last.southWest()) != outlineSet.end()) // SW
				{
					tmpContour.push_back(last.southWest());
					direction = PD_SW;
				}
				else if (outlineSet.find(last.south()) != outlineSet.end()) // S
				{
					tmpContour.push_back(last.south());
					direction = PD_S;
				}
				else if (outlineSet.find(last.southEast()) != outlineSet.end()) // SE
				{
					tmpContour.push_back(last.southEast());
					direction = PD_SE;
				}
				else if (outlineSet.find(last.east()) != outlineSet.end()) // E
				{
					tmpContour.push_back(last.east());
					direction = PD_E;
				}
				else if (outlineSet.find(last.northEast()) != outlineSet.end()) // NE
				{
					tmpContour.push_back(last.northEast());
					direction = PD_NE;
				}
				else
				{
					failed = true;
				}

				break;
			}

			case PD_S:
			{
				/**
				 * new:
				 * |   |
				 * |   |
				 * |   V
				 * | 0 7 6
				 * | 1 X 5
				 * | 2 3 4
				 */

				if (outlineSet.find(last.northWest()) != outlineSet.end()) // NW
				{
					tmpContour.push_back(last.northWest());
					direction = PD_NW;
				}
				else if (outlineSet.find(last.west()) != outlineSet.end()) // W
				{
					tmpContour.push_back(last.west());
					direction = PD_W;
				}
				else if (outlineSet.find(last.southWest()) != outlineSet.end()) // SW
				{
					tmpContour.push_back(last.southWest());
					direction = PD_SW;
				}
				else if (outlineSet.find(last.south()) != outlineSet.end()) // S
				{
					tmpContour.push_back(last.south());
					direction = PD_S;
				}
				else if (outlineSet.find(last.southEast()) != outlineSet.end()) // SE
				{
					tmpContour.push_back(last.southEast());
					direction = PD_SE;
				}
				else if (outlineSet.find(last.east()) != outlineSet.end()) // E
				{
					tmpContour.push_back(last.east());
					direction = PD_E;
				}
				else if (outlineSet.find(last.northEast()) != outlineSet.end()) // NE
				{
					tmpContour.push_back(last.northEast());
					direction = PD_NE;
				}
				else if (outlineSet.find(last.north()) != outlineSet.end()) // N
				{
					tmpContour.push_back(last.north());
					direction = PD_N;
				}
				else
				{
					failed = true;
				}

				break;
			}

			case PD_SE:
			{
				/**
				 * new:
				 * | |
				 * |   |
				 * |     V
				 * | 7 6 5
				 * | 0 X 4
				 * | 1 2 3
				 */

				if (outlineSet.find(last.west()) != outlineSet.end()) // W
				{
					tmpContour.push_back(last.west());
					direction = PD_W;
				}
				else if (outlineSet.find(last.southWest()) != outlineSet.end()) // SW
				{
					tmpContour.push_back(last.southWest());
					direction = PD_SW;
				}
				else if (outlineSet.find(last.south()) != outlineSet.end()) // S
				{
					tmpContour.push_back(last.south());
					direction = PD_S;
				}
				else if (outlineSet.find(last.southEast()) != outlineSet.end()) // SE
				{
					tmpContour.push_back(last.southEast());
					direction = PD_SE;
				}
				else if (outlineSet.find(last.east()) != outlineSet.end()) // E
				{
					tmpContour.push_back(last.east());
					direction = PD_E;
				}
				else if (outlineSet.find(last.northEast()) != outlineSet.end()) // NE
				{
					tmpContour.push_back(last.northEast());
					direction = PD_NE;
				}
				else if (outlineSet.find(last.north()) != outlineSet.end()) // N
				{
					tmpContour.push_back(last.north());
					direction = PD_N;
				}
				else if (outlineSet.find(last.northWest()) != outlineSet.end()) // NW
				{
					tmpContour.push_back(last.northWest());
					direction = PD_NW;
				}
				else
				{
					failed = true;
				}

				break;
			}

			case PD_E:
			{
				/**
				 * new:
				 * |
				 * | ---->
				 * |
				 * | 6 5 4
				 * | 7 X 3
				 * | 0 1 2
				 */

				if (outlineSet.find(last.southWest()) != outlineSet.end()) // SW
				{
					tmpContour.push_back(last.southWest());
					direction = PD_SW;
				}
				else if (outlineSet.find(last.south()) != outlineSet.end()) // S
				{
					tmpContour.push_back(last.south());
					direction = PD_S;
				}
				else if (outlineSet.find(last.southEast()) != outlineSet.end()) // SE
				{
					tmpContour.push_back(last.southEast());
					direction = PD_SE;
				}
				else if (outlineSet.find(last.east()) != outlineSet.end()) // E
				{
					tmpContour.push_back(last.east());
					direction = PD_E;
				}
				else if (outlineSet.find(last.northEast()) != outlineSet.end()) // NE
				{
					tmpContour.push_back(last.northEast());
					direction = PD_NE;
				}
				else if (outlineSet.find(last.north()) != outlineSet.end()) // N
				{
					tmpContour.push_back(last.north());
					direction = PD_N;
				}
				else if (outlineSet.find(last.northWest()) != outlineSet.end()) // NW
				{
					tmpContour.push_back(last.northWest());
					direction = PD_NW;
				}
				else if (outlineSet.find(last.west()) != outlineSet.end()) // W
				{
					tmpContour.push_back(last.west());
					direction = PD_W;
				}
				else
				{
					failed = true;
				}

				break;
			}

			case PD_NE:
			{
				/**
				 * new:
				 * |     ^
				 * |   |
				 * | |
				 * | 5 4 3
				 * | 6 X 2
				 * | 7 0 1
				 */

				if (outlineSet.find(last.south()) != outlineSet.end()) // S
				{
					tmpContour.push_back(last.south());
					direction = PD_S;
				}
				else if (outlineSet.find(last.southEast()) != outlineSet.end()) // SE
				{
					tmpContour.push_back(last.southEast());
					direction = PD_SE;
				}
				else if (outlineSet.find(last.east()) != outlineSet.end()) // E
				{
					tmpContour.push_back(last.east());
					direction = PD_E;
				}
				else if (outlineSet.find(last.northEast()) != outlineSet.end()) // NE
				{
					tmpContour.push_back(last.northEast());
					direction = PD_NE;
				}
				else if (outlineSet.find(last.north()) != outlineSet.end()) // N
				{
					tmpContour.push_back(last.north());
					direction = PD_N;
				}
				else if (outlineSet.find(last.northWest()) != outlineSet.end()) // NW
				{
					tmpContour.push_back(last.northWest());
					direction = PD_NW;
				}
				else if (outlineSet.find(last.west()) != outlineSet.end()) // W
				{
					tmpContour.push_back(last.west());
					direction = PD_W;
				}
				else if (outlineSet.find(last.southWest()) != outlineSet.end()) // SW
				{
					tmpContour.push_back(last.southWest());
					direction = PD_SW;
				}
				else
				{
					failed = true;
				}

				break;
			}

			case PD_N:
			{
				/**
				 * new:
				 * |   ^
				 * |   |
				 * |   |
				 * | 4 3 2
				 * | 5 X 1
				 * | 6 7 0
				 */

				if (outlineSet.find(last.southEast()) != outlineSet.end()) // SE
				{
					tmpContour.push_back(last.southEast());
					direction = PD_SE;
				}
				else if (outlineSet.find(last.east()) != outlineSet.end()) // E
				{
					tmpContour.push_back(last.east());
					direction = PD_E;
				}
				else if (outlineSet.find(last.northEast()) != outlineSet.end()) // NE
				{
					tmpContour.push_back(last.northEast());
					direction = PD_NE;
				}
				else if (outlineSet.find(last.north()) != outlineSet.end()) // N
				{
					tmpContour.push_back(last.north());
					direction = PD_N;
				}
				else if (outlineSet.find(last.northWest()) != outlineSet.end()) // NW
				{
					tmpContour.push_back(last.northWest());
					direction = PD_NW;
				}
				else if (outlineSet.find(last.west()) != outlineSet.end()) // W
				{
					tmpContour.push_back(last.west());
					direction = PD_W;
				}
				else if (outlineSet.find(last.southWest()) != outlineSet.end()) // SW
				{
					tmpContour.push_back(last.southWest());
					direction = PD_SW;
				}
				else if (outlineSet.find(last.south()) != outlineSet.end()) // S
				{
					tmpContour.push_back(last.south());
					direction = PD_S;
				}
				else
				{
					failed = true;
				}

				break;
			}

			case PD_NW:
			{
				/**
				 * new:
				 * | ^
				 * |   |
				 * |     |
				 * | 3 2 1
				 * | 4 X 0
				 * | 5 6 7
				 */

				if (outlineSet.find(last.east()) != outlineSet.end()) // E
				{
					tmpContour.push_back(last.east());
					direction = PD_E;
				}
				else if (outlineSet.find(last.northEast()) != outlineSet.end()) // NE
				{
					tmpContour.push_back(last.northEast());
					direction = PD_NE;
				}
				else if (outlineSet.find(last.north()) != outlineSet.end()) // N
				{
					tmpContour.push_back(last.north());
					direction = PD_N;
				}
				else if (outlineSet.find(last.northWest()) != outlineSet.end()) // NW
				{
					tmpContour.push_back(last.northWest());
					direction = PD_NW;
				}
				else if (outlineSet.find(last.west()) != outlineSet.end()) // W
				{
					tmpContour.push_back(last.west());
					direction = PD_W;
				}
				else if (outlineSet.find(last.southWest()) != outlineSet.end()) // SW
				{
					tmpContour.push_back(last.southWest());
					direction = PD_SW;
				}
				else if (outlineSet.find(last.south()) != outlineSet.end()) // S
				{
					tmpContour.push_back(last.south());
					direction = PD_S;
				}
				else if (outlineSet.find(last.southEast()) != outlineSet.end()) // SE
				{
					tmpContour.push_back(last.southEast());
					direction = PD_SE;
				}
				else
				{
					failed = true;
				}

				break;
			}

			default:
				ocean_assert(false && "Invalid direction!");
		}
	}

	contour.clear();

	if (failed)
	{
		// although the contour could not be determined we will provided the remaining pixels if requested

		if (remainingPixels)
		{
			PixelPositionSet remainingOutlineSet(outlineSet);

			for (PixelPositions::const_iterator i = tmpContour.begin(); i != tmpContour.end(); ++i)
				if (i->x() < width && i->y() < height)
					remainingOutlineSet.erase(*i);

			*remainingPixels = PixelPositions(remainingOutlineSet.begin(), remainingOutlineSet.end());
			ocean_assert(remainingPixels->size() < pixels.size());
		}
	}
	else
	{
		// we do not simply return the determined contour pixels, we do not use pixels outside the frame dimension

		contour.reserve(tmpContour.size());

		if (remainingPixels)
		{
			// in addition to the contour pixels, we preserve all pixels not part of the contour (as requested)

			PixelPositionSet remainingOutlineSet(outlineSet);

			for (PixelPositions::const_iterator i = tmpContour.begin(); i != tmpContour.end(); ++i)
				if (i->x() < width && i->y() < height)
				{
					contour.push_back(*i);

					remainingOutlineSet.erase(*i);
				}

			*remainingPixels = PixelPositions(remainingOutlineSet.begin(), remainingOutlineSet.end());
			ocean_assert(remainingPixels->size() < pixels.size());
		}
		else
		{
			for (PixelPositions::const_iterator i = tmpContour.begin(); i != tmpContour.end(); ++i)
				if (i->x() < width && i->y() < height)
					contour.push_back(*i);
		}
	}

	return !failed;
}

bool MaskAnalyzer::pixels2contours(const uint8_t* mask, const unsigned int width, const unsigned int height, const PixelPositions& pixels, PixelContours& outerContours, PixelContours& innerContours, const uint8_t maskValue)
{
	ocean_assert(mask && width != 0u && height != 0u);

	PixelPositions intermediatePixels(pixels);

	while (!intermediatePixels.empty())
	{
		CV::PixelPositions contourPositions;
		CV::PixelPositions remainingPixels;

		pixels2contour(intermediatePixels, width, height, contourPositions, &remainingPixels);

		if (!contourPositions.empty())
		{
			PixelContour pixelContour(std::move(contourPositions));

			if (isOuterContour(mask, width, pixelContour, maskValue))
			{
				outerContours.push_back(std::move(pixelContour));
			}
			else
			{
				innerContours.push_back(std::move(pixelContour));
			}
		}

		std::swap(intermediatePixels, remainingPixels);
	}

	return true;
}

unsigned int MaskAnalyzer::countMaskPixels(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelBoundingBox& boundingBox, const uint8_t nonMaskValue)
{
	ocean_assert(mask != 0u && width != 0u && height != 0u);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	unsigned int xStart = 0u;
	unsigned int xEnd = width;

	unsigned int yStart = 0u;
	unsigned int yEnd = height;

	if (boundingBox)
	{
		ocean_assert(boundingBox.left() + boundingBox.width() <= width);
		ocean_assert(boundingBox.top() + boundingBox.height() <= height);

		xStart = boundingBox.left();
		xEnd = boundingBox.rightEnd();

		yStart = boundingBox.top();
		yEnd = boundingBox.bottomEnd();
	}

	unsigned int number = 0u;

	for (unsigned int y = yStart; y < yEnd; ++y)
	{
		const uint8_t* maskRow = mask + y * maskStrideElements;

		for (unsigned int x = xStart; x < xEnd; ++x)
		{
			if (maskRow[x] != nonMaskValue)
			{
				++number;
			}
		}
	}

	ocean_assert(number <= width * height);
	return number;
}

void MaskAnalyzer::determineDistancesToBorder8Bit(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const unsigned int iterations, const bool assignFinal, const PixelBoundingBox& boundingBox, Worker* worker)
{
	ocean_assert(mask != nullptr);
	ocean_assert(width >= 3u && height >= 3u);
	ocean_assert(iterations <= 254u);

	ocean_assert(!boundingBox || (boundingBox.rightEnd() <= width && boundingBox.bottomEnd() <= height));

#ifdef OCEAN_DEBUG
	for (unsigned int y = 0u; y < height; ++y)
	{
		const uint8_t* maskRow = mask + y * (width + maskPaddingElements);

		for (unsigned int x = 0u; x < width; ++x)
		{
			ocean_assert(maskRow[x] == 0x00 || maskRow[x] == 0xFF);
		}
	}
#endif // OCEAN_DEBUG

	unsigned int firstColumn = boundingBox ? boundingBox.left() : 0u;
	unsigned int numberColumns = boundingBox ? boundingBox.width() : width;

	unsigned int firstRow = boundingBox ? boundingBox.top() : 0u;
	unsigned int numberRows = boundingBox ? boundingBox.height() : height;

	for (unsigned int n = 0u; n < iterations; ++n)
	{
		const uint8_t searchValue = (n == 0) ? 0xFF : uint8_t(n);
		const uint8_t resultValue = uint8_t(n + 1u);

		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(&MaskAnalyzer::determineDistancesToBorder8BitSubset, mask, width, height, maskPaddingElements, searchValue, resultValue, firstColumn, numberColumns, 0u, 0u), firstRow, numberRows, 8u, 9u, 20u);
		}
		else
		{
			determineDistancesToBorder8BitSubset(mask, width, height, maskPaddingElements, searchValue, resultValue, firstColumn, numberColumns, firstRow, numberRows);
		}

		if (n > 0u)
		{
			if (numberColumns <= 2u || numberRows <= 2u)
			{
				break;
			}

			++firstColumn;
			++firstRow;

			numberColumns -= 2u;
			numberRows -= 2u;
		}
	}

	if (assignFinal)
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				if (*mask == 0x00)
				{
					*mask = uint8_t(iterations);
				}

				++mask;
			}

			mask += maskPaddingElements;
		}
	}
}

void MaskAnalyzer::findBorderPixels4Subset(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions* borderPixelsArray, const uint8_t nonMaskValue, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, const unsigned int threadId)
{
	ocean_assert(mask && borderPixelsArray);

	ocean_assert(firstColumn + numberColumns <= width);
	ocean_assert(firstRow + numberRows <= height);

	PixelPositions& borderPixels = borderPixelsArray[threadId];
	borderPixels.reserve(128u);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	// frame-border pixels are mask-border pixels

	if (firstRow == 0u)
	{
		// each mask pixel in the top row is a border pixel

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (mask[x] != nonMaskValue)
			{
				borderPixels.emplace_back(x, 0u);
			}
		}
	}

	const unsigned int coreFirstRow = max(1u, firstRow);
	const unsigned int coreFirstColumn = max(1u, firstColumn);

	const unsigned int width_1 = width - 1u;
	const unsigned int height_1 = height - 1u;

	const unsigned int coreEndRow = min(firstRow + numberRows, height_1);
	const unsigned int coreEndColumn = min(firstColumn + numberColumns, width_1);

	for (unsigned int y = coreFirstRow; y < coreEndRow; ++y)
	{
		const uint8_t* row0 = mask + (y - 1u) * maskStrideElements;
		const uint8_t* row1 = mask + y * maskStrideElements;
		const uint8_t* row2 = mask + (y + 1u) * maskStrideElements;

		if (firstColumn == 0u && row1[0] != nonMaskValue)
		{
			// each mask pixel in the left column is border pixel
			borderPixels.emplace_back(0, y);
		}

		for (unsigned int x = coreFirstColumn; x < coreEndColumn; ++x)
		{
			if (row1[x] != nonMaskValue && (row1[x - 1u] == nonMaskValue || row1[x + 1u] == nonMaskValue || row0[x] == nonMaskValue || row2[x] == nonMaskValue))
			{
				borderPixels.emplace_back(x, y);
			}
		}

		if (firstColumn + numberColumns == width && row1[width_1] != nonMaskValue)
		{
			// each mask pixel in the right column is border pixel
			borderPixels.emplace_back(width_1, y);
		}
	}

	if (firstRow + numberRows == height)
	{
		// each mask pixel in the bottom row is a border pixel

		mask += height_1 * maskStrideElements;

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (mask[x] != nonMaskValue)
			{
				borderPixels.emplace_back(x, height_1);
			}
		}
	}
}

void MaskAnalyzer::findBorderPixels8Subset(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions* borderPixelsArray, const uint8_t nonMaskValue, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, const unsigned int threadId)
{
	ocean_assert(mask && borderPixelsArray);

	ocean_assert(firstColumn + numberColumns <= width);
	ocean_assert(firstRow + numberRows <= height);

	PixelPositions& borderPixels = borderPixelsArray[threadId];
	borderPixels.reserve(128u);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	// frame-border pixels are mask-border pixels

	if (firstRow == 0u)
	{
		// each mask pixel in the top row is a border pixel

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (mask[x] != nonMaskValue)
			{
				borderPixels.emplace_back(x, 0u);
			}
		}
	}

	const unsigned int coreFirstRow = max(1u, firstRow);
	const unsigned int coreFirstColumn = max(1u, firstColumn);

	const unsigned int width_1 = width - 1u;
	const unsigned int height_1 = height - 1u;

	const unsigned int coreEndRow = min(firstRow + numberRows, height_1);
	const unsigned int coreEndColumn = min(firstColumn + numberColumns, width_1);

	for (unsigned int y = coreFirstRow; y < coreEndRow; ++y)
	{
		const uint8_t* row0 = mask + (y - 1u) * maskStrideElements;
		const uint8_t* row1 = mask + y * maskStrideElements;
		const uint8_t* row2 = mask + (y + 1u) * maskStrideElements;

		if (firstColumn == 0u && row1[0] != nonMaskValue)
		{
			// each mask pixel in the left column is border pixel
			borderPixels.emplace_back(0, y);
		}

		for (unsigned int x = coreFirstColumn; x < coreEndColumn; ++x)
		{
			if (row1[x] != nonMaskValue && (row1[x - 1u] == nonMaskValue || row1[x + 1u] == nonMaskValue || row0[x - 1u] == nonMaskValue || row0[x] == nonMaskValue || row0[x + 1u] == nonMaskValue || row2[x - 1u] == nonMaskValue || row2[x] == nonMaskValue || row2[x + 1u] == nonMaskValue))
			{
				borderPixels.emplace_back(x, y);
			}
		}

		if (firstColumn + numberColumns == width && row1[width_1] != nonMaskValue)
		{
			// each mask pixel in the right column is border pixel
			borderPixels.emplace_back(width_1, y);
		}
	}

	if (firstRow + numberRows == height)
	{
		// each mask pixel in the bottom row is a border pixel

		mask += height_1 * maskStrideElements;

		for (unsigned int x = firstColumn; x < firstColumn + numberColumns; ++x)
		{
			if (mask[x] != nonMaskValue)
			{
				borderPixels.emplace_back(x, height_1);
			}
		}
	}
}

void MaskAnalyzer::determineDistancesToBorder8BitSubset(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t searchPixel, const uint8_t resultValue, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(mask != nullptr);
	ocean_assert(firstColumn + numberColumns <= width);
	ocean_assert(firstRow + numberRows <= height);
	ocean_assert(searchPixel != resultValue);

	ocean_assert(resultValue <= 254u);

	/**
	 * 8-neighborhood template:
	 * O O O
	 * O X O
	 * O O O
	 */

	const unsigned int xBegin = max(1u, firstColumn);
	const unsigned int xEnd = min(firstColumn + numberColumns, width - 1u); // exclusive position

	const unsigned int yBegin = max(1u, firstRow);
	const unsigned int yEnd = min(firstRow + numberRows, height - 1u); // exclusive position

	const unsigned int maskStrideElements = width + maskPaddingElements;

	// top border row

	if (firstRow == 0u)
	{
		uint8_t* maskPointer = mask + firstColumn;

		for (unsigned int n = 0u; n < numberColumns; ++n)
		{
			if (*maskPointer == 0u)
			{
				*maskPointer = 1u;
			}

			++maskPointer;
		}
	}

	for (unsigned int y = yBegin; y < yEnd; ++y)
	{
		uint8_t* const maskRow = mask + y * maskStrideElements;

		// first pixel in row
		if (firstColumn == 0u && *maskRow == 0u)
		{
			*maskRow = 1u;
		}

		uint8_t* maskMiddle = maskRow + xBegin;
		const uint8_t* maskUpper = maskMiddle - maskStrideElements;
		const uint8_t* maskLower = maskMiddle + maskStrideElements;

		// remaining pixels in the row

		for (unsigned int x = xBegin; x < xEnd; ++x)
		{
			if (*maskMiddle == 0u &&
					(*(maskMiddle - 1) == searchPixel || *(maskMiddle + 1) == searchPixel
						|| *(maskUpper - 1) == searchPixel || *(maskUpper) == searchPixel || *(maskUpper + 1) == searchPixel
						|| *(maskLower - 1) == searchPixel || *(maskLower) == searchPixel || *(maskLower + 1) == searchPixel))
			{

				*maskMiddle = resultValue;
			}

			++maskUpper;
			++maskMiddle;
			++maskLower;
		}

		// last pixel row
		if (firstColumn + numberColumns == width && maskRow[width - 1u] == 0u)
		{
			maskRow[width - 1u] = 1u;
		}
	}

	// bottom border row

	if (firstRow + numberRows == height)
	{
		uint8_t* maskPointer = mask + (height - 1u) * maskStrideElements + firstColumn;

		for (unsigned int n = 0u; n < numberColumns; ++n)
		{
			if (*maskPointer == 0u)
			{
				*maskPointer = 1u;
			}

			++maskPointer;
		}
	}
}

PixelBoundingBoxes MaskAnalyzer::detectBoundingBoxes(const uint8_t* const mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t maskValue, const bool useNeighborhood4)
{
	ocean_assert(mask != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	SweepMaskIslands sweepMaskIslands;
	sweepMaskIslands.reserve(16);

	for (unsigned int y = 0u; y < height; ++y)
	{
		const uint8_t* const row = mask + y * maskStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			if (row[x] == maskValue)
			{
				// we have the start of a new mask block

				unsigned int blockStart = x;
				unsigned int blockEnd = width; // exclusive

				while (++x < width)
				{
					if (row[x] != maskValue)
					{
						blockEnd = x;
						break;
					}
				}

				// the mask block has ended

				size_t firstMatchingSweepMaskIndex = size_t(-1);

				for (size_t sweepMaskIndex = 0; sweepMaskIndex < sweepMaskIslands.size(); /*noop*/)
				{
					if (sweepMaskIslands[sweepMaskIndex].hasIntersection(blockStart, blockEnd, useNeighborhood4))
					{
						// we have an intersection with an existing mask

						if (firstMatchingSweepMaskIndex == size_t(-1))
						{
							firstMatchingSweepMaskIndex = sweepMaskIndex;

							sweepMaskIslands[sweepMaskIndex].addSegment(y, blockStart, blockEnd);
						}
						else
						{
							// we have an intersection with another existing mask, we can join both masks

							sweepMaskIslands[firstMatchingSweepMaskIndex].join(sweepMaskIslands[sweepMaskIndex]);
							sweepMaskIslands[sweepMaskIndex] = std::move(sweepMaskIslands.back());
							sweepMaskIslands.pop_back();

							continue;
						}
					}

					++sweepMaskIndex;
				}

				if (firstMatchingSweepMaskIndex == size_t(-1))
				{
					// we did not have any intersection, so that we start a new mask

					sweepMaskIslands.emplace_back(y, blockStart, blockEnd);
				}
			}
		}

		for (SweepMaskIsland& sweepMaskIsland : sweepMaskIslands)
		{
			sweepMaskIsland.nextRow();
		}
	}

	PixelBoundingBoxes result;
	result.reserve(sweepMaskIslands.size());

	for (const SweepMaskIsland& sweepMaskIsland : sweepMaskIslands)
	{
		result.emplace_back(sweepMaskIsland.boundingBox());
	}

	return result;
}

} // namespace Segmentation

} // namespace CV

} // namespace Ocean
