/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/Histogram.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/base/Memory.h"

namespace Ocean
{

namespace CV
{

bool Histogram::equalization(Frame& frame, const Scalar factor, Worker* worker)
{
	ocean_assert(frame.isValid());
	ocean_assert(factor >= 0 && factor <= 1);

	if (frame.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		switch (frame.channels())
		{
			case 1u:
				return equalization<1u>(frame.data<uint8_t>(), frame.width(), frame.height(), factor, frame.paddingElements(), worker);

			case 2u:
				return equalization<2u>(frame.data<uint8_t>(), frame.width(), frame.height(), factor, frame.paddingElements(), worker);

			case 3u:
				return equalization<3u>(frame.data<uint8_t>(), frame.width(), frame.height(), factor, frame.paddingElements(), worker);

			case 4u:
				return equalization<4u>(frame.data<uint8_t>(), frame.width(), frame.height(), factor, frame.paddingElements(), worker);
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool Histogram::equalization(const Frame& source, Frame& target, const Scalar factor, Worker* worker)
{
	ocean_assert(source.isValid());
	ocean_assert(factor >= 0 && factor <= 1);

	if (source.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		switch (target.channels())
		{
			case 1u:
			{
				if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				return equalization<1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), factor, source.paddingElements(), target.paddingElements(), worker);
			}

			case 2u:
			{
				if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				return equalization<2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), factor, source.paddingElements(), target.paddingElements(), worker);
			}

			case 3u:
			{
				if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				return equalization<3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), factor, source.paddingElements(), target.paddingElements(), worker);
			}

			case 4u:
			{
				if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				return equalization<4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), factor, source.paddingElements(), target.paddingElements(), worker);
			}
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool Histogram::adjustColorToReference(Frame& frame, const Frame& reference, Worker* worker)
{
	ocean_assert(frame.isValid() && reference.isValid());
	ocean_assert(frame.pixelFormat() == reference.pixelFormat());
	ocean_assert(frame.pixelOrigin() == reference.pixelOrigin());

	if (!frame.isValid() || !reference.isValid() || frame.pixelFormat() != reference.pixelFormat() || frame.pixelOrigin() != reference.pixelOrigin())
	{
		return false;
	}

	if (frame.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		switch (frame.channels())
		{
			case 1u:
				return adjustColorToReference<1u>(frame.data<uint8_t>(), frame.width(), frame.height(), reference.constdata<uint8_t>(), reference.width(), reference.height(), frame.paddingElements(), reference.paddingElements(), worker);

			case 2u:
				return adjustColorToReference<2u>(frame.data<uint8_t>(), frame.width(), frame.height(), reference.constdata<uint8_t>(), reference.width(), reference.height(), frame.paddingElements(), reference.paddingElements(), worker);

			case 3u:
				return adjustColorToReference<3u>(frame.data<uint8_t>(), frame.width(), frame.height(), reference.constdata<uint8_t>(), reference.width(), reference.height(), frame.paddingElements(), reference.paddingElements(), worker);

			case 4u:
				return adjustColorToReference<4u>(frame.data<uint8_t>(), frame.width(), frame.height(), reference.constdata<uint8_t>(), reference.width(), reference.height(), frame.paddingElements(), reference.paddingElements(), worker);
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool Histogram::adjustColorToReference(Frame& frame, const Frame& reference, const unsigned int horizontalBins, const unsigned int verticalBins, Worker* worker)
{
	ocean_assert(frame.isValid() && reference.isValid());
	ocean_assert(frame.pixelFormat() == reference.pixelFormat());
	ocean_assert(frame.pixelOrigin() == reference.pixelOrigin());

	if (!frame.isValid() || !reference.isValid() || frame.pixelFormat() != reference.pixelFormat() || frame.pixelOrigin() != reference.pixelOrigin())
	{
		return false;
	}

	if (frame.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		switch (frame.channels())
		{
			case 1u:
				return adjustColorToReference<1u>(frame.data<uint8_t>(), frame.width(), frame.height(), reference.constdata<uint8_t>(), reference.width(), reference.height(), horizontalBins, verticalBins, frame.paddingElements(), reference.paddingElements(), worker);

			case 2u:
				return adjustColorToReference<2u>(frame.data<uint8_t>(), frame.width(), frame.height(), reference.constdata<uint8_t>(), reference.width(), reference.height(), horizontalBins, verticalBins, frame.paddingElements(), reference.paddingElements(),worker);

			case 3u:
				return adjustColorToReference<3u>(frame.data<uint8_t>(), frame.width(), frame.height(), reference.constdata<uint8_t>(), reference.width(), reference.height(), horizontalBins, verticalBins, frame.paddingElements(), reference.paddingElements(),worker);

			case 4u:
				return adjustColorToReference<4u>(frame.data<uint8_t>(), frame.width(), frame.height(), reference.constdata<uint8_t>(), reference.width(), reference.height(), horizontalBins, verticalBins, frame.paddingElements(), reference.paddingElements(),worker);
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

bool Histogram::adjustColorToReference(const Frame& source, Frame& target, const Frame& reference, Worker* worker)
{
	ocean_assert(source.isValid() && reference.isValid());
	ocean_assert(source.pixelFormat() == reference.pixelFormat());
	ocean_assert(source.pixelOrigin() == reference.pixelOrigin());

	if (!source.isValid() || !reference.isValid() || source.pixelFormat() != reference.pixelFormat() || source.pixelOrigin() != reference.pixelOrigin())
	{
		return false;
	}

	if (source.isPixelFormatCompatible(FrameType::FORMAT_Y8))
	{
		switch (source.channels())
		{
			case 1u:
			{
				if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				return adjustColorToReference<1u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), reference.constdata<uint8_t>(), reference.width(), reference.height(), source.paddingElements(), target.paddingElements(), reference.paddingElements(), worker);
			}

			case 2u:
			{
				if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				return adjustColorToReference<2u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), reference.constdata<uint8_t>(), reference.width(), reference.height(), source.paddingElements(), target.paddingElements(), reference.paddingElements(), worker);
			}

			case 3u:
			{
				if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				return adjustColorToReference<3u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), reference.constdata<uint8_t>(), reference.width(), reference.height(), source.paddingElements(), target.paddingElements(), reference.paddingElements(), worker);
			}

			case 4u:
			{
				if (!target.set(source.frameType(), false /*forceOwner*/, true /*forceWritable*/))
				{
					ocean_assert(false && "This should never happen!");
					return false;
				}

				return adjustColorToReference<4u>(source.constdata<uint8_t>(), target.data<uint8_t>(), source.width(), source.height(), reference.constdata<uint8_t>(), reference.width(), reference.height(), source.paddingElements(), target.paddingElements(), reference.paddingElements(), worker);
			}
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

void ContrastLimitedAdaptiveHistogram::equalization8BitPerChannel(const uint8_t* const source, const unsigned int width, const unsigned height, uint8_t* const target, const Scalar clipLimit, const unsigned int horizontalTiles, const unsigned int verticalTiles, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(clipLimit > Scalar(0));
	ocean_assert(horizontalTiles >= 2u && verticalTiles >= 2u);
	ocean_assert(horizontalTiles <= width && verticalTiles <= height);

	const TileLookupCenter2 tileLookupCenter2(width, height, horizontalTiles, verticalTiles);

	std::vector<uint8_t> tileLookupTables;
	computeTileLookupTables(source, tileLookupCenter2, tileLookupTables, clipLimit, sourcePaddingElements, worker);
	bilinearInterpolation(source, tileLookupCenter2, target, tileLookupTables, sourcePaddingElements, targetPaddingElements, worker);
}

void ContrastLimitedAdaptiveHistogram::computeTileLookupTables(const uint8_t* const source, const TileLookupCenter2& lookupCenter2, std::vector<uint8_t>& tileLookupTables, const Scalar clipLimit, const unsigned int sourcePaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr);
	ocean_assert(lookupCenter2.sizeX() != 0u && lookupCenter2.sizeY() != 0u);
	ocean_assert(lookupCenter2.binsX() != 0u && lookupCenter2.binsY() != 0u);
	ocean_assert(clipLimit > Scalar(0));

	const unsigned int tilesCount = (unsigned int)(lookupCenter2.binsX() * lookupCenter2.binsY());
	tileLookupTables.resize(tilesCount * histogramSize);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&computeTileLookupTablesSubset, source, &lookupCenter2, tileLookupTables.data(), clipLimit, sourcePaddingElements, 0u, 0u), 0u, tilesCount);
	}
	else
	{
		computeTileLookupTablesSubset(source, &lookupCenter2, tileLookupTables.data(), clipLimit, sourcePaddingElements, 0u, tilesCount);
	}
}

void ContrastLimitedAdaptiveHistogram::computeLowBilinearInterpolationFactors7BitPrecision(const TileLookupCenter2& lookupCenter2, const bool isHorizontal, Index32* lowBins, uint8_t* lowFactors_fixed7)
{
	ocean_assert(lowBins && lowFactors_fixed7);

	const unsigned int tilesCount = (unsigned int)(isHorizontal ? lookupCenter2.binsX() : lookupCenter2.binsY());
	ocean_assert(tilesCount >= 2u);
	const unsigned int firstTileCenter = (unsigned int)((isHorizontal ? lookupCenter2.binCenterPositionX(0) : lookupCenter2.binCenterPositionY(0)) + Scalar(0.5));

	const unsigned int imageEdgeLength = (unsigned int)(isHorizontal ? lookupCenter2.sizeX() : lookupCenter2.sizeY());
	ocean_assert(imageEdgeLength >= tilesCount);

	// Left: columns left of first tile center
	unsigned int pixelIndex = 0u;
	unsigned int lowBin = 0u;

	while (pixelIndex < firstTileCenter)
	{
		lowBins[pixelIndex] = lowBin;
		lowFactors_fixed7[pixelIndex] = 128u;

		++pixelIndex;
	}

	// Center: column from the first tile center just before the last tile center
	ocean_assert(pixelIndex == firstTileCenter);

	const unsigned int secondLastTile = (unsigned int)std::max(0, (int)tilesCount - 2);
	const unsigned int lastTile = secondLastTile + 1u;
	const unsigned int lastTileCenter = (unsigned int)((isHorizontal ? lookupCenter2.binCenterPositionX(lastTile) : lookupCenter2.binCenterPositionY(lastTile)) + Scalar(0.5));

	float lowCenterF = (float)(isHorizontal ? lookupCenter2.binCenterPositionX(lowBin) : lookupCenter2.binCenterPositionY(lowBin));
	unsigned int lowCenterI = (unsigned int)(lowCenterF + 0.5f);

	while (lowBin < lastTile)
	{
		const unsigned int highBin = lowBin + 1u;
		ocean_assert(highBin < tilesCount);

		const float highCenterF = (float)(isHorizontal ? lookupCenter2.binCenterPositionX(highBin) : lookupCenter2.binCenterPositionY(highBin));
		const unsigned int highCenterI = (unsigned int)(highCenterF + 0.5f);
		ocean_assert_and_suppress_unused(highCenterI <= lastTileCenter, lastTileCenter);

		const float centerDistance = highCenterF - lowCenterF;
		ocean_assert(NumericF::isNotEqualEps(centerDistance));
		const float centerDistanceInv = 1.0f / centerDistance;

		ocean_assert_and_suppress_unused(pixelIndex == lowCenterI, lowCenterI);

		while (pixelIndex < highCenterI)
		{
			lowBins[pixelIndex] = lowBin;

			ocean_assert(highCenterF >= (float)pixelIndex);
			const float xFactor = (highCenterF - (float)pixelIndex) * centerDistanceInv;
			ocean_assert(xFactor >= 0.0f && xFactor <= 1.0f);

			lowFactors_fixed7[pixelIndex] = (uint8_t)(128.0f * xFactor + 0.5f);

			++pixelIndex;
		}

		lowBin = highBin;
		lowCenterF = highCenterF;
		lowCenterI = highCenterI;
	}

	// Right: columns from last tile center to the end of the image width
	ocean_assert(pixelIndex == lastTileCenter);

	while (pixelIndex < imageEdgeLength)
	{
		lowBins[pixelIndex] = secondLastTile;
		lowFactors_fixed7[pixelIndex] = 0u;

		++pixelIndex;
	}

	ocean_assert(pixelIndex == imageEdgeLength);
}

void ContrastLimitedAdaptiveHistogram::bilinearInterpolation(const uint8_t* const source, const TileLookupCenter2& lookupCenter2, uint8_t* const target, const std::vector<uint8_t>& tileLookupTables, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(lookupCenter2.sizeX() != 0u && lookupCenter2.sizeY() != 0u);
	ocean_assert(lookupCenter2.binsX() != 0u && lookupCenter2.binsY() != 0u);
	ocean_assert(tileLookupTables.size() == lookupCenter2.binsX() * lookupCenter2.binsY() * histogramSize);

	const unsigned int width = (unsigned int)lookupCenter2.sizeX();
	const unsigned int height = (unsigned int)lookupCenter2.sizeY();

	Memory leftBins = Memory::create<Index32>(width);
	Memory leftFactors_fixed7 = Memory::create<uint8_t>(width);
	computeLowBilinearInterpolationFactors7BitPrecision(lookupCenter2, /* isHorizontal */ true, leftBins.data<Index32>(), leftFactors_fixed7.data<uint8_t>());

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
	Memory topBins = Memory::create<Index32>(height);
	Memory topFactors_fixed7 = Memory::create<uint8_t>(height);
	computeLowBilinearInterpolationFactors7BitPrecision(lookupCenter2, /* isHorizontal */ false, topBins.data<Index32>(), topFactors_fixed7.data<uint8_t>());

	const unsigned int horizontalBins = (unsigned int)lookupCenter2.binsX();
	const unsigned int verticalBins = (unsigned int)lookupCenter2.binsY();
	const unsigned int tilesCount = horizontalBins * verticalBins;

	const bool useNeon = width / horizontalBins >= 8u;
#endif

	if (worker)
	{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
		if (useNeon)
		{
			worker->executeFunction(Worker::Function::createStatic(&bilinearInterpolationNEON7BitPrecisionSubset, source, &lookupCenter2, target, tileLookupTables.data(), (const Index32* const)leftBins.data<Index32>(), (const uint8_t* const)leftFactors_fixed7.data<uint8_t>(), (const Index32* const)topBins.data<Index32>(), (const uint8_t* const)topFactors_fixed7.data<uint8_t>(), sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, tilesCount);
			return;
		}
#endif

		worker->executeFunction(Worker::Function::createStatic(&bilinearInterpolation7BitPrecisionSubset, source, &lookupCenter2, target, tileLookupTables.data(), (const Index32* const)leftBins.data(), (const uint8_t* const)leftFactors_fixed7.data(), sourcePaddingElements, targetPaddingElements, 0u, 0u), 0u, height);
	}
	else
	{
#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10
		if (useNeon)
		{
			bilinearInterpolationNEON7BitPrecisionSubset(source, &lookupCenter2, target, tileLookupTables.data(), leftBins.data<Index32>(), leftFactors_fixed7.data<uint8_t>(), topBins.data<Index32>(), topFactors_fixed7.data<uint8_t>(), sourcePaddingElements, targetPaddingElements, 0u, tilesCount);
			return;
		}
#endif

		bilinearInterpolation7BitPrecisionSubset(source, &lookupCenter2, target, tileLookupTables.data(), leftBins.data<Index32>(), leftFactors_fixed7.data<uint8_t>(), sourcePaddingElements, targetPaddingElements, 0u, height);
	}
}

void ContrastLimitedAdaptiveHistogram::computeTileLookupTablesSubset(const uint8_t* const source, const TileLookupCenter2* lookupCenter2, uint8_t* const tileLookupTables, const Scalar clipLimit, const unsigned int sourcePaddingElements, const unsigned int firstTile, const unsigned int tileCount)
{
	ocean_assert(source != nullptr);
	ocean_assert(lookupCenter2 != nullptr);
	ocean_assert(lookupCenter2->sizeX() != 0u && lookupCenter2->sizeY() != 0u);
	ocean_assert(lookupCenter2->binsX() != 0u && lookupCenter2->binsY() != 0u);
	ocean_assert(tileLookupTables != nullptr);
	ocean_assert(clipLimit > Scalar(0));

	const unsigned int sourceWidth = (unsigned int)lookupCenter2->sizeX();
	const unsigned int sourceStrideElements = (unsigned int)lookupCenter2->sizeX() + sourcePaddingElements;
	const unsigned int binEnd = firstTile + tileCount;
	const unsigned int horizontalTiles = (unsigned int)lookupCenter2->binsX();

	for (unsigned int tileIndex = firstTile; tileIndex < binEnd; ++tileIndex)
	{
		const unsigned int binY = tileIndex / horizontalTiles;
		const unsigned int binX = tileIndex % horizontalTiles;
		ocean_assert(binX < lookupCenter2->binsX() && binY < lookupCenter2->binsY());

		const unsigned int tileYStart = (unsigned int)lookupCenter2->binTopLeftY(binY);
		const unsigned int tileYEnd = (unsigned int)lookupCenter2->binBottomRightY(binY);
		const unsigned int tileHeight = tileYEnd - tileYStart + 1u;

		const unsigned int tileXStart = (unsigned int)lookupCenter2->binTopLeftX(binX);
		const unsigned int tileXEnd = (unsigned int)lookupCenter2->binBottomRightX(binX);
		const unsigned int tileWidth = tileXEnd - tileXStart + 1u;
		ocean_assert(tileWidth <= sourceWidth);
		const unsigned int tilePaddingElements = (sourceWidth - tileWidth) + sourcePaddingElements;

		const uint8_t* tile = source + tileYStart * sourceStrideElements + tileXStart;
		uint8_t* tileLookupTable = tileLookupTables + tileIndex * histogramSize;

		computeLookupTable(tile, tileWidth, tileHeight, tileLookupTable, clipLimit, tilePaddingElements);
	}
}

void ContrastLimitedAdaptiveHistogram::bilinearInterpolation7BitPrecisionSubset(const uint8_t* const source, const TileLookupCenter2* lookupCenter2, uint8_t* const target, const uint8_t* const tileLookupTables, const Index32* const leftBins, const uint8_t* const leftFactors_fixed7, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int rowStart, const unsigned int rowCount)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(lookupCenter2 != nullptr);
	ocean_assert(lookupCenter2->sizeX() != 0u && lookupCenter2->sizeY() != 0u);
	ocean_assert(lookupCenter2->binsX() != 0u && lookupCenter2->binsY() != 0u);
	ocean_assert(tileLookupTables != nullptr);
	ocean_assert(leftBins != nullptr && leftFactors_fixed7 != nullptr);

	const unsigned int width = (unsigned int)lookupCenter2->sizeX();
	const unsigned int horizontalTiles = (unsigned int)lookupCenter2->binsX();

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;
	const unsigned int rowEnd = rowStart + rowCount;
	ocean_assert(rowEnd <= lookupCenter2->sizeY());

	for (unsigned int y = rowStart; y < rowEnd; ++y)
	{
		const size_t yBin = lookupCenter2->binY(Scalar(y));
		const float yBinCenter = (float)(lookupCenter2->binCenterPositionY(yBin));

		const size_t topBin = (float(y) >= yBinCenter) ? yBin : max(0, int(yBin) - 1);
		const size_t bottomBin = (float(y) < yBinCenter) ? yBin : min(topBin + 1, lookupCenter2->binsY() - 1);
		ocean_assert(((topBin == 0 || topBin == lookupCenter2->binsY() - 1) && bottomBin == topBin) || topBin + 1 == bottomBin);

		const float topCenter = (float)lookupCenter2->binCenterPositionY(topBin);
		const float bottomCenter = (float)lookupCenter2->binCenterPositionY(bottomBin);
		ocean_assert(topCenter <= bottomCenter);
		ocean_assert(topBin == bottomBin || (topCenter <= float(y) && Numeric::isNotEqual(topCenter, bottomCenter)));

		const float topFactor = topBin != bottomBin ? (bottomCenter - (float)y) / (bottomCenter - topCenter) : 1.0f;
		ocean_assert(topFactor >= 0.0f && topFactor <= 1.0f);

		const uint8_t topFactor_fixed7 = (uint8_t)(128.0f * topFactor + 0.5f);
		const uint8_t bottomFactor_fixed7 = 128u - topFactor_fixed7;

		const uint8_t* const sourceRow = source + y * sourceStrideElements;
		uint8_t* const targetRow = target + y * targetStrideElements;

		for (unsigned int x = 0u; x < width; ++x)
		{
			const Index32 leftBin = leftBins[x];

			const uint8_t* const topLeftLUT = tileLookupTables + (topBin * horizontalTiles + leftBin) * histogramSize;
			const uint8_t* const topRightLUT = topLeftLUT + histogramSize;
			const uint8_t* const bottomLeftLUT = tileLookupTables + (bottomBin * horizontalTiles + leftBin) * histogramSize;
			const uint8_t* const bottomRightLUT = bottomLeftLUT + histogramSize;

			const uint8_t leftFactor_fixed7 = leftFactors_fixed7[x];
			const uint8_t rightFactor_fixed7 = 128u - leftFactor_fixed7;

			const unsigned int topLeftFactor_fixed7 = leftFactor_fixed7 * topFactor_fixed7;
			const unsigned int topRightFactor_fixed7 = rightFactor_fixed7 * topFactor_fixed7;
			const unsigned int bottomLeftFactor_fixed7 = leftFactor_fixed7 * bottomFactor_fixed7;
			const unsigned int bottomRightFactor_fixed7 = rightFactor_fixed7 * bottomFactor_fixed7;
			ocean_assert(topLeftFactor_fixed7 + topRightFactor_fixed7 + bottomLeftFactor_fixed7 + bottomRightFactor_fixed7 == 128u * 128u);

			const uint8_t sourceValue = sourceRow[x];
			const unsigned int targetValue_fixed7 = topLeftLUT[sourceValue] * topLeftFactor_fixed7 + topRightLUT[sourceValue] * topRightFactor_fixed7 + bottomLeftLUT[sourceValue] * bottomLeftFactor_fixed7 + bottomRightLUT[sourceValue] * bottomRightFactor_fixed7;

			targetRow[x] = (uint8_t)((targetValue_fixed7 + 8192u) >> 14u);
		}
	}
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

void ContrastLimitedAdaptiveHistogram::bilinearInterpolationNEON7BitPrecisionSubset(const uint8_t* const source, const TileLookupCenter2* lookupCenter2, uint8_t* const target, const uint8_t* const tileLookupTables, const Index32* const leftBins, const uint8_t* const leftFactors_fixed7, const Index32* const topBins, const uint8_t* const topFactors_fixed7, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int tileStart, const unsigned int tileCount)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(lookupCenter2 != nullptr);
	ocean_assert(tileLookupTables != nullptr);
	ocean_assert(leftBins != nullptr && leftFactors_fixed7 != nullptr);
	ocean_assert(topBins != nullptr && topFactors_fixed7 != nullptr);

	const unsigned int width = (unsigned int)lookupCenter2->sizeX();
	const unsigned int height = (unsigned int)lookupCenter2->sizeY();
	ocean_assert(width >= 8u && height != 0u);

	const unsigned int horizontalTiles = (unsigned int)lookupCenter2->binsX();
	const unsigned int verticalTiles = (unsigned int)lookupCenter2->binsY();
	ocean_assert(horizontalTiles >= 2u && verticalTiles >= 2u);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	const unsigned int tileEnd = tileStart + tileCount;
	ocean_assert(tileEnd <= horizontalTiles * verticalTiles);
	const unsigned int lastHorizontalTile = horizontalTiles - 1u;
	ocean_assert(lastHorizontalTile < horizontalTiles);
	const unsigned int lastVerticalTile = verticalTiles - 1u;
	ocean_assert(lastVerticalTile < verticalTiles);

	const unsigned int secondLastHorizontalTile = (horizontalTiles >= 2u ? horizontalTiles - 2u : 0u);
	const unsigned int secondLastVerticalTile = (verticalTiles >= 2u ? verticalTiles - 2u : 0u);

	// Memory aligned storage for the lookup tables of four corners that will be used for the interpolation
	uint8_t cornerLookupTables0[4 * (size_t)histogramSize + 3];
	ocean_assert((uintptr_t)cornerLookupTables0 % 4 <= 3);
	uint8_t* cornerLookupTables = cornerLookupTables0 + (4 - ((uintptr_t)cornerLookupTables0) % 4) % 4;
	ocean_assert((uintptr_t)cornerLookupTables % 4 == 0);

	for (unsigned int tileIndex = tileStart; tileIndex < tileEnd; ++tileIndex)
	{
		//      Image partitioned into tiles:                                 Processing partition used for the interpolation:
		//      (pixel value = tile index)                                    (right-most and bottom-most tiles are handled by
		//                                                                    left/upper neighboring tiles, tiles 2, 5, 8, 11)
		//
		//       0   1   2   3   4   5   6   7   8   9  10  11  12  -->       0   1   2   3   4   5   6   7   8   9  10  11  12
		//     ---------------------------------------------------- -->     ----------------------------------------------------
		//  0:   0   0   0   0   0|  1   1   1   1|  2   2   2   2| -->  0:   0   0   0   0   0|  0   0   1   1|  1   1   1   1|
		//  1:   0   0   0   0   0|  1   1   1   1|  2   2   2   2| -->  1:   0   0   0   0   0|  0   0   1   1|  1   1   1   1|
		//  2:   0   0   0   0   0|  1   1   1   1|  2   2   2   2| -->  2:   0   0   0   0   0|  0   0   1   1|  1   1   1   1|
		//  3:   0   0   0   0   0|  1   1   1   1|  2   2   2   2| -->  3:   0   0   0   0   0|  0   0   1   1|  1   1   1   1|
		//     ---------------------------------------------------- -->     ----------------------------------------------------
		//  4:   3   3   3   3   3|  4   4   4   4|  5   5   5   5| -->  4:   0   0   0   0   0|  0   0   1   1|  1   1   1   1|
		//  5:   3   3   3   3   3|  4   4   4   4|  5   5   5   5| -->  5:   0   0   0   0   0|  0   0   1   1|  1   1   1   1|
		//  6:   3   3   3   3   3|  4   4   4   4|  5   5   5   5| -->  6:   3   3   3   3   3|  3   3   4   4|  4   4   4   4|
		//  7:   3   3   3   3   3|  4   4   4   4|  5   5   5   5| -->  7:   3   3   3   3   3|  3   3   4   4|  4   4   4   4|
		//     ---------------------------------------------------- -->     ----------------------------------------------------
		//  8:   6   6   6   6   6|  7   7   7   7|  8   8   8   8| -->  8:   3   3   3   3   3|  3   3   4   4|  4   4   4   4|
		//  9:   6   6   6   6   6|  7   7   7   7|  8   8   8   8| -->  9:   3   3   3   3   3|  3   3   4   4|  4   4   4   4|
		// 10:   6   6   6   6   6|  7   7   7   7|  8   8   8   8| --> 10:   6   6   6   6   6|  6   6   7   7|  7   7   7   7|
		// 11:   6   6   6   6   6|  7   7   7   7|  8   8   8   8| --> 11:   6   6   6   6   6|  6   6   7   7|  7   7   7   7|
		//     ---------------------------------------------------- -->     ----------------------------------------------------
		// 12:   9   9   9   9   9| 10  10  10  10| 11  11  11  11| --> 12:   6   6   6   6   6|  6   6   7   7|  7   7   7   7|
		// 13:   9   9   9   9   9| 10  10  10  10| 11  11  11  11| --> 13:   6   6   6   6   6|  6   6   7   7|  7   7   7   7|
		// 14:   9   9   9   9   9| 10  10  10  10| 11  11  11  11| --> 14:   6   6   6   6   6|  6   6   7   7|  7   7   7   7|
		//     ---------------------------------------------------- -->     ----------------------------------------------------

		const unsigned int tileY = tileIndex / horizontalTiles;
		const unsigned int tileX = tileIndex % horizontalTiles;
		ocean_assert(tileX < horizontalTiles && tileY < verticalTiles);

		// The areas of the right-most and bottom-most tiles are handled by their left/upper neighboring tile
		if (tileX == lastHorizontalTile || tileY == lastVerticalTile)
		{
			// TODO Make sure to iterate only over (horizontalTiles - 1u) * (verticalTiles - 1u) tiles to improve performance of multi-core implementations
			continue;
		}

		ocean_assert(tileY < lastVerticalTile);
		const unsigned int tileStartY = (tileY == 0u ? 0u : (unsigned int)(lookupCenter2->binCenterPositionY(tileY) + Scalar(0.5)));
		const unsigned int tileEndY = (tileY == secondLastVerticalTile ? height : (unsigned int)(lookupCenter2->binCenterPositionY(tileY + 1u) + Scalar(0.5)));

		ocean_assert(tileX < lastHorizontalTile);
		const unsigned int tileStartX = (tileX == 0u ? 0u : (unsigned int)(lookupCenter2->binCenterPositionX(tileX) + Scalar(0.5)));
		const unsigned int tileEndX = (tileX == secondLastHorizontalTile ? width : (unsigned int)(lookupCenter2->binCenterPositionX(tileX + 1u) + Scalar(0.5)));

		ocean_assert(tileStartX + 8u <= tileEndX && "Tiles too small to be processed with NEON");
		const unsigned tileWidth = tileEndX - tileStartX;

		// Extract the LUTs of the four corners that are used for the interpolation for the current tile
		const unsigned int tileIndexTL = tileIndex;
		const unsigned int tileIndexTR = tileIndex + 1u;
		const unsigned int tileIndexBL = tileIndex + horizontalTiles;
		const unsigned int tileIndexBR = tileIndexBL + 1u;

		const uint8_t* const tileLookupTableTL = tileLookupTables + tileIndexTL * histogramSize;
		const uint8_t* const tileLookupTableTR = tileLookupTables + tileIndexTR * histogramSize;
		const uint8_t* const tileLookupTableBL = tileLookupTables + tileIndexBL * histogramSize;
		const uint8_t* const tileLookupTableBR = tileLookupTables + tileIndexBR * histogramSize;

		static_assert(histogramSize == 256u, "Implementation requires histogramSize to be 256");
		for (unsigned int i = 0u; i < histogramSize / 16u; ++i)
		{
			uint8x16x4_t tileLookupTables_TL_TR_BL_BR;
			tileLookupTables_TL_TR_BL_BR.val[0] = vld1q_u8(tileLookupTableTL + i * 16u);
			tileLookupTables_TL_TR_BL_BR.val[1] = vld1q_u8(tileLookupTableTR + i * 16u);
			tileLookupTables_TL_TR_BL_BR.val[2] = vld1q_u8(tileLookupTableBL + i * 16u);
			tileLookupTables_TL_TR_BL_BR.val[3] = vld1q_u8(tileLookupTableBR + i * 16u);

			vst4q_u8(cornerLookupTables + i * 64u, tileLookupTables_TL_TR_BL_BR);
		}

		// Interpolate
		const uint8x8_t m64_constant_128 = vdup_n_u8(128u);

		for (unsigned int y = tileStartY; y < tileEndY; ++y)
		{
			const uint8_t* sourceRow = source + y * sourceStrideElements;
			uint8_t* targetRow = target + y * targetStrideElements;

			// m128_u_topFactors_fixed7 = [v, v, v, v, v, v, v, v], v = topFactors_fixed7[y]
			const uint16x8_t m128_u_topFactors_fixed7 = vdupq_n_u16(topFactors_fixed7[y]);

			// m128_u_bottomFactors_fixed7 = [v, v, v, v, v, v, v, v], v = 128 - topFactors_fixed7[y]
			const uint16x8_t m128_u_bottomFactors_fixed7 = vdupq_n_u16(128u - topFactors_fixed7[y]);

			uint8x8x4_t corners_TL_TR_BL_BR;
			corners_TL_TR_BL_BR.val[0] = vdup_n_u8(0u);
			corners_TL_TR_BL_BR.val[1] = vdup_n_u8(0u);
			corners_TL_TR_BL_BR.val[2] = vdup_n_u8(0u);
			corners_TL_TR_BL_BR.val[3] = vdup_n_u8(0u);

			for (unsigned int x = tileStartX; x < tileEndX; x += 8u)
			{
				if (x + 8u > tileEndX)
				{
					// The last iteration will not fit. We shift x left by some pixels so that we have enough pixels to process
					// (shift at most 7, which means we will calculate some pixels again)
					ocean_assert(x >= 8u && tileWidth > 8u);
					const unsigned int newX = tileStartX + tileWidth - 8u;

					ocean_assert(x > newX);
					x = newX;

					// the for loop will stop after this iteration
					ocean_assert((x + 8u < tileWidth) == false);
				}

				// m128_u_leftFactors_fixed7 = [v0, v1, v2, v3, v4, v5, v6, v7], vi = leftFactors_fixed7[x + i], i = 0...7
				const uint8x8_t m64_u_leftFactors_fixed7 = vld1_u8(leftFactors_fixed7 + x);

				// m64_u_rightFactors_fixed7 = [v0, v1, v2, v3, v4, v5, v6, v7], vi = 128 - leftFactors_fixed7[x + i], i = 0...7
				const uint8x8_t m64_u_rightFactors_fixed7 = vsub_u8(m64_constant_128, m64_u_leftFactors_fixed7);

				corners_TL_TR_BL_BR = vld4_lane_u8(cornerLookupTables + 4 * sourceRow[x + 0], corners_TL_TR_BL_BR, 0);
				corners_TL_TR_BL_BR = vld4_lane_u8(cornerLookupTables + 4 * sourceRow[x + 1], corners_TL_TR_BL_BR, 1);
				corners_TL_TR_BL_BR = vld4_lane_u8(cornerLookupTables + 4 * sourceRow[x + 2], corners_TL_TR_BL_BR, 2);
				corners_TL_TR_BL_BR = vld4_lane_u8(cornerLookupTables + 4 * sourceRow[x + 3], corners_TL_TR_BL_BR, 3);
				corners_TL_TR_BL_BR = vld4_lane_u8(cornerLookupTables + 4 * sourceRow[x + 4], corners_TL_TR_BL_BR, 4);
				corners_TL_TR_BL_BR = vld4_lane_u8(cornerLookupTables + 4 * sourceRow[x + 5], corners_TL_TR_BL_BR, 5);
				corners_TL_TR_BL_BR = vld4_lane_u8(cornerLookupTables + 4 * sourceRow[x + 6], corners_TL_TR_BL_BR, 6);
				corners_TL_TR_BL_BR = vld4_lane_u8(cornerLookupTables + 4 * sourceRow[x + 7], corners_TL_TR_BL_BR, 7);

				// weightedTopLeftValues = [v0, v1, v2, v3, v4, v5, v6, v7], vi = topLeftValues[i] * leftFactors[i], i=0...7
				const uint16x8_t m128_u_weightedTopLeftValues = vmull_u8(corners_TL_TR_BL_BR.val[0], m64_u_leftFactors_fixed7);

				// weightedTopRightValues = [v0, v1, v2, v3, v4, v5, v6, v7], vi = topRightValues[i] * rightFactors[i], i=0...7
				const uint16x8_t m128_u_weightedTopRightValues = vmull_u8(corners_TL_TR_BL_BR.val[1], m64_u_rightFactors_fixed7);

				// weightedBottomLeftValues = [v0, v1, v2, v3, v4, v5, v6, v7], vi = bottomLeftValues[i] * leftFactors[i], i=0...7
				const uint16x8_t m128_u_weightedBottomLeftValues = vmull_u8(corners_TL_TR_BL_BR.val[2], m64_u_leftFactors_fixed7);

				// weightedTopRightValues = [v0, v1, v2, v3, v4, v5, v6, v7], vi = topRightValues[i] * rightFactors[i], i=0...7
				const uint16x8_t m128_u_weightedBottomRightValues = vmull_u8(corners_TL_TR_BL_BR.val[3], m64_u_rightFactors_fixed7);

				// weighedTopValues = [v0, v1, v2, v3, v4, v5, v6, v7], vi = (((weightedTopLeftValues[i] + weightedTopRightValues[i]) + 0.5) >> 7) * topFactors[i], i=0...7
				const uint16x8_t m128_u_weighedTopValues = vmulq_u16(vrshrq_n_u16(vaddq_u16(m128_u_weightedTopLeftValues, m128_u_weightedTopRightValues), 7), m128_u_topFactors_fixed7);

				// weightedBottomValues = [v0, v1, v2, v3, v4, v5, v6, v7], vi = (((weightedBottomLeftValues[i] + weightedBottomRightValues[i]) + 0.5) >> 7) * bottomFactors[i], i=0...7
				const uint16x8_t m128_u_weightedBottomValues = vmulq_u16(vrshrq_n_u16(vaddq_u16(m128_u_weightedBottomLeftValues, m128_u_weightedBottomRightValues), 7), m128_u_bottomFactors_fixed7);

				// interpolatedValues = [v0, v1, v2, v3, v4, v5, v6, v7], vi = (uint8_t)((weighedTopValues[i] + weightedBottomValues[i]) + 0.5) >> 7)
				const uint8x8_t m64_u_interpolatedValues = vmovn_u16(vrshrq_n_u16(vaddq_u16(m128_u_weighedTopValues, m128_u_weightedBottomValues), 7));

				// targetRow[x + i] = m64_u_interpolatedValues[i], i = 0...7
				vst1_u8(targetRow + x, m64_u_interpolatedValues);
			}
		}
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION

} // namespace CV

} // namespace Ocean
