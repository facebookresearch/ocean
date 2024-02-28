// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/detector/blob/BlobFeatureDetector.h"

#include "ocean/base/Exception.h"
#include "ocean/base/Utilities.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/math/Quaternion.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace Blob
{

const unsigned int BlobFeatureDetector::densitiesSamplingStep[3][10] =
{
	{1u, 1u, 1u, 1u, 2u, 2u,  4u,  4u,  8u,  8u},
	{2u, 2u, 2u, 2u, 4u, 4u,  8u,  8u, 16u, 16u},
	{4u, 4u, 4u, 4u, 8u, 8u, 16u, 16u, 32u, 32u}
};

const unsigned int BlobFeatureDetector::densitiesSamplingOffsets[3][10] =
{
	{0u, 0u, 0u, 0u, 0u, 0u, 0u,  0u,  0u,  0u},
	{1u, 0u, 1u, 0u, 0u, 2u, 2u,  6u,  6u, 14u},
	{3u, 0u, 1u, 2u, 0u, 2u, 6u, 10u, 18u, 26u}
};

BlobFeatureDetector::ResponseMap::ResponseMap(const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int samplingStep, const unsigned int filterIndex, const unsigned int explicitOffset) :
	mapFrameWidth(frameWidth),
	mapFrameHeight(frameHeight),
	mapResponseWidth(0u),
	mapResponseHeight(0u),
	mapSamplingStep(samplingStep),
	mapFilterIndex(filterIndex),
	mapFilterSize(0u),
	mapResponseFirstX(0u),
	mapResponseFirstY(0u),
	mapFilterOffsetX(0u),
	mapFilterOffsetY(0u)
{
	ocean_assert(mapFilterIndex >= 1u && mapFilterIndex <= 32u);
	ocean_assert(samplingStep >= 1u);

	ocean_assert(mapFrameWidth > 0u);
	ocean_assert(mapFrameHeight > 0u);

	// the first filter has size 9, the second 15, the third 21, ...
	mapFilterSize = 9u + (mapFilterIndex - 1u) * 6u;

	ocean_assert(mapFilterSize >= 3u);
	ocean_assert((mapFilterSize % 2u) == 1u);

	if (calculateResponseParameters(mapFrameWidth, mapFrameHeight, 0u, 0u, mapFrameWidth, mapFrameHeight, mapFilterSize, mapSamplingStep, explicitOffset, mapResponseWidth, mapResponseHeight, mapResponseFirstX, mapResponseFirstY, mapFilterOffsetX, mapFilterOffsetY))
	{
		ocean_assert(mapResponseWidth > 0u);
		ocean_assert(mapResponseHeight > 0u);

		mapFilterResponseFrame.set(FrameType(mapResponseWidth, mapResponseHeight, FrameType::genericPixelFormat<Scalar, 1u>(), FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/);
		mapLaplacianSignFrame.set(FrameType(mapResponseWidth, mapResponseHeight, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>(), FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/);

		ocean_assert(mapFilterResponseFrame.isContinuous());
		ocean_assert(mapLaplacianSignFrame.isContinuous());
	}
}

BlobFeatureDetector::ResponseMap::ResponseMap(const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const unsigned int samplingStep, const unsigned int filterIndex, const unsigned int explicitOffset) :
	mapFrameWidth(frameWidth),
	mapFrameHeight(frameHeight),
	mapResponseWidth(0u),
	mapResponseHeight(0u),
	mapSamplingStep(samplingStep),
	mapFilterIndex(filterIndex),
	mapFilterSize(0u),
	mapResponseFirstX(0u),
	mapResponseFirstY(0u),
	mapFilterOffsetX(0u),
	mapFilterOffsetY(0u)
{
	ocean_assert(mapFilterIndex >= 1u && mapFilterIndex <= 32u);
	ocean_assert(samplingStep >= 1u);

	ocean_assert(mapFrameWidth > 0u);
	ocean_assert(mapFrameHeight > 0u);

	// the first filter has size 9, the second 15, the third 21, ...
	mapFilterSize = 9u + (mapFilterIndex - 1u) * 6u;

	ocean_assert(mapFilterSize >= 3u);
	ocean_assert((mapFilterSize % 2u) == 1u);

	const unsigned int sFrameLeft = min(subframeLeft, frameWidth);
	const unsigned int sFrameTop = min(subframeTop, frameHeight);
	const unsigned int sFrameWidth = min(subframeLeft + subframeWidth, frameWidth) - sFrameLeft;
	const unsigned int sFrameHeight = min(subframeTop + subframeHeight, frameHeight) - sFrameTop;

	if (calculateResponseParameters(mapFrameWidth, mapFrameHeight, sFrameLeft, sFrameTop, sFrameWidth, sFrameHeight, mapFilterSize, mapSamplingStep, explicitOffset, mapResponseWidth, mapResponseHeight, mapResponseFirstX, mapResponseFirstY, mapFilterOffsetX, mapFilterOffsetY))
	{
		ocean_assert(mapResponseWidth > 0u);
		ocean_assert(mapResponseHeight > 0u);

		mapFilterResponseFrame.set(FrameType(mapResponseWidth, mapResponseHeight, FrameType::genericPixelFormat<Scalar, 1u>(), FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/);
		mapLaplacianSignFrame.set(FrameType(mapResponseWidth, mapResponseHeight, FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>(), FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/);

		ocean_assert(mapFilterResponseFrame.isContinuous());
		ocean_assert(mapLaplacianSignFrame.isContinuous());
	}
}

bool BlobFeatureDetector::ResponseMap::calculateOriginalPosition(const unsigned int responseX, const unsigned int responseY, unsigned int& frameX, unsigned int& frameY) const
{
	ocean_assert(mapSamplingStep > 0u && mapResponseFirstX > 0u && mapResponseFirstY > 0u);

	if (responseX >= mapResponseWidth || responseY >= mapResponseHeight)
	{
		return false;
	}

	frameX = responseX * mapSamplingStep + mapResponseFirstX;
	frameY = responseY * mapSamplingStep + mapResponseFirstY;

	ocean_assert(frameX < mapFrameWidth);
	ocean_assert(frameY < mapFrameHeight);

	return true;
}

bool BlobFeatureDetector::ResponseMap::calculateOriginalPosition(const Scalar responseX, const Scalar responseY, Scalar& frameX, Scalar& frameY) const
{
	ocean_assert(mapSamplingStep > 0u && mapResponseFirstX > 0u && mapResponseFirstY > 0u);

	if (responseX >= Scalar(mapResponseWidth) || responseY >= Scalar(mapResponseHeight))
	{
		return false;
	}

	frameX = responseX * Scalar(mapSamplingStep) + Scalar(mapResponseFirstX);
	frameY = responseY * Scalar(mapSamplingStep) + Scalar(mapResponseFirstY);

	ocean_assert(frameX < Scalar(mapFrameWidth));
	ocean_assert(frameY < Scalar(mapFrameHeight));

	return true;
}

bool BlobFeatureDetector::ResponseMap::calculateResponsePosition(const unsigned int frameX, const unsigned int frameY, unsigned int& responseX, unsigned int& responseY) const
{
	ocean_assert(mapSamplingStep > 0u && mapResponseFirstX > 0u && mapResponseFirstY > 0u);

	if (mapResponseFirstX > frameX || mapResponseFirstY > frameY)
	{
		return false;
	}

	ocean_assert((frameX - mapResponseFirstX) % mapSamplingStep == 0u);
	const unsigned int x = (frameX - mapResponseFirstX) / mapSamplingStep;

	ocean_assert((frameY - mapResponseFirstY) % mapSamplingStep == 0u);
	const unsigned int y = (frameY - mapResponseFirstY) / mapSamplingStep;

	if (x >= mapResponseWidth || y >= mapResponseHeight)
	{
		return false;
	}

	responseX = x;
	responseY = y;
	return true;
}

void BlobFeatureDetector::ResponseMap::filter(const uint32_t* linedIntegralImage, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::create(*this, &BlobFeatureDetector::ResponseMap::filterSubset, linedIntegralImage, 0u, 0u), 0u, mapResponseHeight, 1u, 2u);
	}
	else
	{
		filterSubset(linedIntegralImage, 0u, mapResponseHeight);
	}
}

void BlobFeatureDetector::ResponseMap::filterSubset(const uint32_t* linedIntegralImage, const unsigned int firstResponseRow, const unsigned int numberResponseRows)
{
	ocean_assert(linedIntegralImage);

	ocean_assert(numberResponseRows > 0);
	ocean_assert(firstResponseRow + numberResponseRows <= mapResponseHeight);

	/**
	 * Computation of Lxx
	 * The Lxx filter has the following scheme, with the filter lobe [B C B]:
	 *  -----------------
	 * |        A        |
	 * |-----------------|
	 * |     |     |     |
	 * |  B  |  C  |  B  |
	 * |     |     |     |
	 * |-----------------|
	 * |        A        |
	 *  -----------------
	 * A: Each pixel is multiplied by zero, therefore A does not need to be handled.
	 *    For a 9x9 filter each A block has a size of 2x9, a filter of 15x15 has an A block size of 3x15, 21x21 has 4x21, ...
	 *    The block size is computed by (1 + filterIndex)x(filterSizeFull).
	 *
	 * B: Each pixel is multiplied by 1.
	 *    For a 9x9 filter each B block has a size of 5x3, a filter of 15x15 has a B block size of 9x5, 21x21 has 13x7, ...
	 *    The block size is computed by (1 + filterIndex * 4)x(filterSizeFull / 3).
	 *
	 * C: Each pixel is multiplied by -2.
	 *    The filter size is identical to the B block.
	 */

	/**
	 * Computation of Lyy
	 * The Lyy filter has the following scheme, with the (transposed) filter lobe [B C B]
	 *  -----------------
	 * |   |    B    |   |
	 * |   |---------|   |
	 * | A |    C    | A |
	 * |   |---------|   |
	 * |   |    B    |   |
	 *  -----------------
	 * A, B and C have the same size as for Lxx.
	 * Lxx and Lyy are identical except a 90 degree rotation.
	 */

	/**
	 * Computation of Lxy
	 * The Lxy filter has the following scheme:
	 *  ---------------------
	 * | A                 A |
	 * |    -------------    |
	 * |   |     | |     |   |
	 * |   |  B  | |  C  |   |
	 * |   |     | |     |   |
	 * |   |-----   -----|   |
	 * |   |      D      |   |
	 * |   |-----   -----|   |
	 * |   |     | |     |   |
	 * |   |  C  | |  B  |   |
	 * |   |     | |     |   |
	 * |    -------------    |
	 * | A                 A |
	 *  ---------------------
	 * A: Each pixel is multiplied by zero, therefore A does not need to be handled.
	 *    A is a border surrounding the entire filter.
	 *    For a 9x9 filter the A border has a width of 1 pixel, a filter of 15x15 has a border of 2 pixels, 21x21 has 3 pixels, ....
	 *    The border width is identical to the filterIndex.
	 *
	 * B: Each pixel is multiplied by 1.
	 *    For a 9x9 filter each B block has a size of 3x3, a 15x15 filter has a B block size of 5x5, 21x21 has 7x7, ...
	 *    The block size is computed by (1 + filterIndex * 2)x(1 + filterIndex * 2).
	 *
	 * C: Each pixel is multiplied by -1.
	 *    The size of each C block is identical to the B blocks.
	 *
	 * D: Each pixel is multiplied by zero, therefore D does not need to be handled.
	 *    Independent of the filter size D has a simple cross shape with one pixel thickness.
	 */

	const unsigned int filterLobeSmall = mapFilterSize / 3u;
	const unsigned int filterLobeLarge = 1u + (mapFilterIndex << 2u); // mapFilterIndex * 4;
	const unsigned int filterZero = 1u + mapFilterIndex;
	ocean_assert(filterLobeSmall * 3u == mapFilterSize);

	const unsigned int linedIntegralImageWidth = mapFrameWidth + 1u;
	const unsigned int lobeSmallVerticalOffset = filterLobeSmall * linedIntegralImageWidth;

	const Scalar normalization = Scalar(1) / Scalar(filterArea() * filterArea());

	ocean_assert(mapFilterResponseFrame.isContinuous());
	Scalar* response = mapFilterResponseFrame.data<Scalar>() + mapResponseWidth * firstResponseRow - 1u;
	const Scalar* const responseEnd = response + mapResponseWidth * numberResponseRows;

	ocean_assert(mapLaplacianSignFrame.isContinuous());
	uint8_t* laplacianSign = mapLaplacianSignFrame.data<uint8_t>() + mapResponseWidth * firstResponseRow - 1u;

	linedIntegralImage += (mapSamplingStep * firstResponseRow) * linedIntegralImageWidth;

	// shifting integral pointer to the (top left) filter position, respecting explicit offset and sub-image offset
	linedIntegralImage += linedIntegralImageWidth * mapFilterOffsetY + mapFilterOffsetX;

	const unsigned int notTouchedEnd = mapFrameWidth - (mapResponseWidth - 1u) * mapSamplingStep - mapFilterSize;
	const unsigned int nextLineOffset = mapFilterSize - mapSamplingStep + notTouchedEnd + 1u;
	const unsigned int integralLineEndOffset = linedIntegralImageWidth * (mapSamplingStep - 1u) + nextLineOffset;

	while (response != responseEnd)
	{
		ocean_assert(*(linedIntegralImage - mapFilterOffsetX) == 0u);

		const Scalar* const responseRowEnd = response + mapResponseWidth;

		while (response != responseRowEnd)
		{

#ifdef OCEAN_DEBUG

			// Lxx
			const uint32_t* integralDebug = linedIntegralImage + linedIntegralImageWidth * filterZero;

			int xxDebug = int(*integralDebug) - 3 * (int(*(integralDebug + filterLobeSmall)) - int(*(integralDebug + (filterLobeSmall * 2)))) - int(*(integralDebug + 3 * filterLobeSmall));
			integralDebug += filterLobeLarge * linedIntegralImageWidth;
			xxDebug -= int(*integralDebug) - 3 * (int(*(integralDebug + filterLobeSmall)) - int(*(integralDebug + (filterLobeSmall * 2)))) - int(*(integralDebug + 3 * filterLobeSmall));


			// Lyy
			integralDebug = linedIntegralImage + filterZero;

			int yyDebug = int(*integralDebug) - int(*(integralDebug + filterLobeLarge));
			integralDebug += lobeSmallVerticalOffset;
			yyDebug -= 3 * (int(*integralDebug) - int(*(integralDebug + filterLobeLarge)) - int(*(integralDebug + lobeSmallVerticalOffset)) + int(*(integralDebug + lobeSmallVerticalOffset + filterLobeLarge)));
			integralDebug += (lobeSmallVerticalOffset * 2);
			yyDebug += int(*(integralDebug + filterLobeLarge)) - int(*integralDebug);


			// Lxy
			integralDebug = linedIntegralImage + linedIntegralImageWidth * (filterZero - 1) + filterZero - 1;

			int xyDebug = int(*integralDebug) - int(*(integralDebug + filterLobeSmall)) - int(*(integralDebug + filterLobeSmall + 1)) + int(*(integralDebug + (filterLobeSmall * 2) + 1));
			integralDebug += lobeSmallVerticalOffset;

			xyDebug -= int(*integralDebug) - int(*(integralDebug + filterLobeSmall)) - int(*(integralDebug + filterLobeSmall + 1)) + int(*(integralDebug + (filterLobeSmall * 2) + 1));
			integralDebug += linedIntegralImageWidth;

			xyDebug -= int(*integralDebug) - int(*(integralDebug + filterLobeSmall)) - int(*(integralDebug + filterLobeSmall + 1)) + int(*(integralDebug + (filterLobeSmall * 2) + 1));
			integralDebug += lobeSmallVerticalOffset;

			xyDebug += int(*integralDebug) - int(*(integralDebug + filterLobeSmall)) - int(*(integralDebug + filterLobeSmall + 1)) + int(*(integralDebug + (filterLobeSmall * 2) + 1));

#endif // OCEAN_DEBUG

			// Lxx
			const uint32_t* integral = linedIntegralImage + linedIntegralImageWidth * filterZero;

			int xx = *integral - 3u * (*(integral + filterLobeSmall) - *(integral + (filterLobeSmall * 2u))) - *(integral + 3u * filterLobeSmall);
			integral += filterLobeLarge * linedIntegralImageWidth;
			xx -= *integral - 3u * (*(integral + filterLobeSmall) - *(integral + (filterLobeSmall * 2u))) - *(integral + 3u * filterLobeSmall);


			// Lyy
			integral = linedIntegralImage + filterZero;

			int yy = *integral - *(integral + filterLobeLarge);
			integral += lobeSmallVerticalOffset;
			yy -= 3 * (*integral - *(integral + filterLobeLarge) - *(integral + lobeSmallVerticalOffset) + *(integral + lobeSmallVerticalOffset + filterLobeLarge));
			integral += (lobeSmallVerticalOffset * 2u);
			yy += *(integral + filterLobeLarge) - *integral;


			// Lxy
			integral = linedIntegralImage + linedIntegralImageWidth * (filterZero - 1u) + filterZero - 1u;

			int xy = *integral - *(integral + filterLobeSmall) - *(integral + filterLobeSmall + 1u) + *(integral + (filterLobeSmall * 2u) + 1u);
			integral += lobeSmallVerticalOffset;

			xy -= *integral - *(integral + filterLobeSmall) - *(integral + filterLobeSmall + 1u) + *(integral + (filterLobeSmall * 2u) + 1u);
			integral += linedIntegralImageWidth;

			xy -= *integral - *(integral + filterLobeSmall) - *(integral + filterLobeSmall + 1u) + *(integral + (filterLobeSmall * 2u) + 1u);
			integral += lobeSmallVerticalOffset;

			xy += *integral - *(integral + filterLobeSmall) - *(integral + filterLobeSmall + 1u) + *(integral + (filterLobeSmall * 2u) + 1u);

#ifdef OCEAN_DEBUG

			ocean_assert(xxDebug == xx);
			ocean_assert(yyDebug == yy);
			ocean_assert(xyDebug == xy);

#endif // OCEAN_DEBUG

			*++response = (Scalar(xx) * Scalar(yy) - Scalar(0.81) * Scalar(xy) * Scalar(xy)) * normalization;
			*++laplacianSign = xx + yy > 0;

			linedIntegralImage += mapSamplingStep;
		}

		linedIntegralImage += integralLineEndOffset;
	}
}

bool BlobFeatureDetector::ResponseMap::calculateResponseParameters(const unsigned int frameWidth, const unsigned int frameHeight, const unsigned int subframeLeft, const unsigned int subframeTop, const unsigned int subframeWidth, const unsigned int subframeHeight, const unsigned int filterSize, const unsigned int samplingStep, const unsigned int explicitOffset, unsigned int& responseWidth, unsigned int& responseHeight, unsigned int& responseFirstX, unsigned int& responseFirstY, unsigned int& filterOffsetX, unsigned int& filterOffsetY)
{
	if (frameWidth == 0u || filterSize > frameWidth || frameHeight == 0u || filterSize > frameHeight)
	{
		return false;
	}

	if (subframeLeft + subframeWidth > frameWidth || subframeTop + subframeHeight > frameHeight)
	{
		return false;
	}

	if (samplingStep == 0u || filterSize == 0u || (filterSize % 2u) != 1u)
	{
		return false;
	}

	const unsigned int filterSize_2 = filterSize / 2u;


	// filterOffsetX >= subframeLeft
	// filterOffsetX = explicitOffset + missingSamplesX * samplingStep, missingSamplesX >= 0
	// missingSamplesX >= (subframeLeft - explicitOffset) / samplingStep
	unsigned int missingSamplesX = max((int(subframeLeft) - int(explicitOffset)) / int(samplingStep), 0);
	if (subframeLeft > explicitOffset && ((subframeLeft - explicitOffset) % samplingStep) != 0)
	{
		++missingSamplesX;
	}

	// missingSamplesY >= (subframeTop - explicitOffset) / samplingStep
	unsigned int missingSamplesY = max((int(subframeTop) - int(explicitOffset)) / int(samplingStep), 0);
	if (subframeTop > explicitOffset && ((subframeTop - explicitOffset) % samplingStep)!= 0)
	{
		++missingSamplesY;
	}

	filterOffsetX = explicitOffset + missingSamplesX * samplingStep;
	ocean_assert(filterOffsetX >= subframeLeft);

	filterOffsetY = explicitOffset + missingSamplesY * samplingStep;
	ocean_assert(filterOffsetY >= subframeTop);

	responseFirstX = filterSize_2 + filterOffsetX;
	responseFirstY = filterSize_2 + filterOffsetY;

	/**
	 * (responseWidth - 1) * samplingStep + 1 + responseFirst + filterSizeHalf <= subframeLeft + subframeWidth
	 * responseWidth <= 1 + (subframeLeft + subframeWidth - responseFirst - filterSizeHalf - 1) / samplingStep
	 */
	responseWidth = max(1 + (int(subframeLeft) + int(subframeWidth) - int(responseFirstX) - int(filterSize_2) - 1) / int(samplingStep), 0);

	// (responseHeight - 1) * samplingStep + 1 + responseFirst + filterSizeHalf <= subframeTop + subframeHeight
	responseHeight = max(1 + (int(subframeTop) + int(subframeHeight) - int(responseFirstY) - int(filterSize_2) - 1) / int(samplingStep), 0);

	return responseWidth >= 3u && responseHeight >= 3u;
}

unsigned int BlobFeatureDetector::detectFeatures(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const unsigned int layer, const SamplingDense samplingDense, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker)
{
	ocean_assert(samplingDense >= SAMPLING_DENSE && samplingDense <= SAMPLING_SPARSE);
	ocean_assert(linedIntegralImage != nullptr);

	ocean_assert(width > 0u && height > 0u);
	ocean_assert(layer >= 1u);

	/**
	 * Filter data:
	 *    Filter   |  Layer  | Dense sampling steps | Normal sampling steps | Sparse sampling steps |
	 *     9x9          1              1                        2                      4
	 *    15x15         2              1                        2                      4
	 *    21x21         3              1                        2                      4
	 *    27x27         4              1                        2                      4
	 *    39x39         6              2                        4                      8
	 *    51x51         8              2                        4                      8
	 *    75x75        12              4                        8                     16
	 *    99x99        16              4                        8                     16
	 *   147x147       24              8                       16                     32
	 *   195x195       32              8                       16                     32
	 *
	 */

	unsigned int samplingStep = 2u * (layer / 4u + (layer % 4u) != 0u);

	if (samplingDense == SAMPLING_DENSE)
	{
		ocean_assert(samplingStep % 2u == 0u);
		samplingStep /= 2u;
	}
	else if (samplingDense == SAMPLING_SPARSE)
	{
		samplingStep *= 2u;
	}

	ResponseMap responseMap(width, height, samplingStep, layer, 0u);

	if (responseMap)
	{
		responseMap.filter(linedIntegralImage, worker);
		findExtremes(responseMap, threshold, frameIsUndistorted, features, worker);
	}

	return (unsigned int)(features.size());
}

unsigned int BlobFeatureDetector::detectFeatures(const uint32_t* linedIntegralImage, const unsigned int width, const unsigned int height, const unsigned int subFrameLeft, const unsigned int subFrameTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const SamplingDense samplingDense, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker)
{

	ocean_assert(samplingDense >= SAMPLING_DENSE && samplingDense <= SAMPLING_SPARSE);
	ocean_assert(linedIntegralImage != nullptr);

	ocean_assert(width > 0u && height > 0u);
	ocean_assert(subFrameLeft + subFrameWidth <= width);
	ocean_assert(subFrameTop + subFrameHeight <= height);

	ocean_assert(features.empty());

	/**
	 * Filter data:
	 *
	 * Normal sampling dense
	 *   Filter   | Sampling step | Normal Start Index | Explicit Offset | Explicit Start Index | Sampling positions           | Common start positions
	 *     9x9            2                 4                    1                  5             5, 7, 9, 11, ...               11
	 *    15x15           2                 7                    0                  7             7, 9, 11, 13, 15, 17, ...      11    13    19
	 *    21x21           2                10                    1                 11             11, 13, 15, 17, 19, 21, ...    11    13
	 *    27x27           2                13                    0                 13             13, 15, 17, 19, 21, 23, ...          13    19    27    39
	 *    39x39           4                19                    0                 19             19, 23, 27, 31, 35, 39 ...                 19    27
	 *    51x51           4                25                    2                 27             27, 31, 35, 39, 43, 47, ...                      27    39    55    79
	 *    75x75           8                37                    2                 39             39, 47, 55, 63, 71, 79, ...                            39    55
	 *    99x99           8                49                    6                 55             55, 63, 71, 79, 87, 95, ...                                  55    79    111
	 *   147x147         16                73                    6                 79             79, 95, 111, 127, 143, ...                                         79    111
	 *   195x195         16                97                    14               111             111, 127, 143, 159, ...                                                  111
	 */


	// Filter: 9x9
	ResponseMap responseMap1 = (width == subFrameWidth && height == subFrameHeight)
								? ResponseMap(width, height, densitiesSamplingStep[samplingDense][0], 1, densitiesSamplingOffsets[samplingDense][0])
								: ResponseMap(width, height, subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, densitiesSamplingStep[samplingDense][0], 1, densitiesSamplingOffsets[samplingDense][0]);
	// Filter: 15x15
	ResponseMap responseMap2 = (width == subFrameWidth && height == subFrameHeight)
								? ResponseMap(width, height, densitiesSamplingStep[samplingDense][1], 2, densitiesSamplingOffsets[samplingDense][1])
								: ResponseMap(width, height, subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, densitiesSamplingStep[samplingDense][1], 2, densitiesSamplingOffsets[samplingDense][1]);
	// Filter: 21x21
	ResponseMap responseMap3 = (width == subFrameWidth && height == subFrameHeight)
								? ResponseMap(width, height, densitiesSamplingStep[samplingDense][2], 3, densitiesSamplingOffsets[samplingDense][2])
								: ResponseMap(width, height, subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, densitiesSamplingStep[samplingDense][2], 3, densitiesSamplingOffsets[samplingDense][2]);
	// Filter: 27x27
	ResponseMap responseMap4 = (width == subFrameWidth && height == subFrameHeight)
								? ResponseMap(width, height, densitiesSamplingStep[samplingDense][3], 4, densitiesSamplingOffsets[samplingDense][3])
								: ResponseMap(width, height, subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, densitiesSamplingStep[samplingDense][3], 4, densitiesSamplingOffsets[samplingDense][3]);


	// Filter: 39x39
	ResponseMap responseMap6 = (width == subFrameWidth && height == subFrameHeight)
								? ResponseMap(width, height, densitiesSamplingStep[samplingDense][4], 6, densitiesSamplingOffsets[samplingDense][4])
								: ResponseMap(width, height, subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, densitiesSamplingStep[samplingDense][4], 6, densitiesSamplingOffsets[samplingDense][4]);
	// Filter: 51x51
	ResponseMap responseMap8 = (width == subFrameWidth && height == subFrameHeight)
								? ResponseMap(width, height, densitiesSamplingStep[samplingDense][5], 8, densitiesSamplingOffsets[samplingDense][5])
								: ResponseMap(width, height, subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, densitiesSamplingStep[samplingDense][5], 8, densitiesSamplingOffsets[samplingDense][5]);


	// Filter: 75x75
	ResponseMap responseMap12 = (width == subFrameWidth && height == subFrameHeight)
								? ResponseMap(width, height, densitiesSamplingStep[samplingDense][6], 12, densitiesSamplingOffsets[samplingDense][6])
								: ResponseMap(width, height, subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, densitiesSamplingStep[samplingDense][6], 12, densitiesSamplingOffsets[samplingDense][6]);
	// Filter: 99x99
	ResponseMap responseMap16 = (width == subFrameWidth && height == subFrameHeight)
								? ResponseMap(width, height, densitiesSamplingStep[samplingDense][7], 16, densitiesSamplingOffsets[samplingDense][7])
								: ResponseMap(width, height, subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, densitiesSamplingStep[samplingDense][7], 16, densitiesSamplingOffsets[samplingDense][7]);


	// Filter: 147x147
	ResponseMap responseMap24 = (width == subFrameWidth && height == subFrameHeight)
								? ResponseMap(width, height, densitiesSamplingStep[samplingDense][8], 24, densitiesSamplingOffsets[samplingDense][8])
								: ResponseMap(width, height, subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, densitiesSamplingStep[samplingDense][8], 24, densitiesSamplingOffsets[samplingDense][8]);
	// Filter: 195x195
	ResponseMap responseMap32 = (width == subFrameWidth && height == subFrameHeight)
								? ResponseMap(width, height, densitiesSamplingStep[samplingDense][9], 32, densitiesSamplingOffsets[samplingDense][9])
								: ResponseMap(width, height, subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, densitiesSamplingStep[samplingDense][9], 32, densitiesSamplingOffsets[samplingDense][9]);

	if (responseMap1)
	{
		responseMap1.filter(linedIntegralImage, worker);
	}

	if (responseMap2)
	{
		responseMap2.filter(linedIntegralImage, worker);
	}

	if (responseMap3)
	{
		responseMap3.filter(linedIntegralImage, worker);
	}

	if (responseMap4)
	{
		responseMap4.filter(linedIntegralImage, worker);
	}

	if (responseMap6)
	{
		responseMap6.filter(linedIntegralImage, worker);
	}

	if (responseMap8)
	{
		responseMap8.filter(linedIntegralImage, worker);
	}

	if (worker)
	{
		// the following response maps are so small that we execute all of them in parallel

		Worker::Functions workerFunctions;
		workerFunctions.reserve(4);

		if (responseMap12)
		{
			workerFunctions.push_back(Worker::Function::create(responseMap12, &ResponseMap::filter, linedIntegralImage, (Worker*)nullptr));
		}

		if (responseMap16)
		{
			workerFunctions.push_back(Worker::Function::create(responseMap16, &ResponseMap::filter, linedIntegralImage, (Worker*)nullptr));
		}

		if (responseMap24)
		{
			workerFunctions.push_back(Worker::Function::create(responseMap24, &ResponseMap::filter, linedIntegralImage, (Worker*)nullptr));
		}

		if (responseMap32)
		{
			workerFunctions.push_back(Worker::Function::create(responseMap32, &ResponseMap::filter, linedIntegralImage, (Worker*)nullptr));
		}

		worker->executeFunctions(workerFunctions);
	}
	else
	{
		if (responseMap12)
		{
			responseMap12.filter(linedIntegralImage);
		}

		if (responseMap16)
		{
			responseMap16.filter(linedIntegralImage);
		}

		if (responseMap24)
		{
			responseMap24.filter(linedIntegralImage);
		}

		if (responseMap32)
		{
			responseMap32.filter(linedIntegralImage);
		}
	}

	features.reserve(1024);

	// Response maps: 9x9, 15x15, 21x21, 27x27
	// 9x9, 15x15, 21x21
	if (responseMap1 && responseMap2 && responseMap3)
	{
		findExtremes(responseMap1, responseMap2, responseMap3, threshold, frameIsUndistorted, features, worker);
	}

	// 15x15, 21x21, 27x27
	if (responseMap2 && responseMap3 && responseMap4)
	{
		findExtremes(responseMap2, responseMap3, responseMap4, threshold, frameIsUndistorted, features, worker);
	}

	// Response maps: 15x15, 27x27, 39x39, 51x51
	// 15x15, 27x27, 39x39
	if (responseMap2 && responseMap4 && responseMap6)
	{
		findExtremes(responseMap2, responseMap4, responseMap6, threshold, frameIsUndistorted, features, worker);
	}

	// 27x27, 39x39, 51x51
	if (responseMap4 && responseMap6 && responseMap8)
	{
		findExtremes(responseMap4, responseMap6, responseMap8, threshold, frameIsUndistorted, features, worker);
	}

	// Response maps: 27x27, 51x51, 75x75, 99x99
	// 27x27, 51x51, 75x75
	if (responseMap4 && responseMap8 && responseMap12)
	{
		findExtremes(responseMap4, responseMap8, responseMap12, threshold, frameIsUndistorted, features, nullptr /* no worker */);
	}

	// 51x51, 75x75, 99x99
	if (responseMap8 && responseMap12 && responseMap16)
	{
		findExtremes(responseMap8, responseMap12, responseMap16, threshold, frameIsUndistorted, features, nullptr /* no worker */);
	}

	// Response maps: 51x51, 99x99, 147x147, 195x195
	// 51x51, 99x99, 147x147
	if (responseMap8 && responseMap16 && responseMap24)
	{
		findExtremes(responseMap8, responseMap16, responseMap24, threshold, frameIsUndistorted, features, nullptr /* no worker */);
	}

	// 99x99, 147x147, 195x195
	if (responseMap16 && responseMap24 && responseMap32)
	{
		findExtremes(responseMap16, responseMap24, responseMap32, threshold, frameIsUndistorted, features, nullptr /* no worker */);
	}

	return (unsigned int)features.size();
}

unsigned int BlobFeatureDetector::detectFeatures(const Frame& frame, const SamplingDense samplingDense, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker)
{
	ocean_assert(frame);

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		ocean_assert(false && "Invalid frame type!");
		return 0u;
	}

	constexpr unsigned int integralFramePaddingElements = 0u;
	Frame integralFrame(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT), integralFramePaddingElements);
	CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), integralFrame.data<uint32_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), integralFrame.paddingElements());

	ocean_assert(integralFrame.isContinuous());
	return detectFeatures(integralFrame.data<uint32_t>(), yFrame.width(), yFrame.height(), samplingDense, threshold, frameIsUndistorted, features, worker);
}

unsigned int BlobFeatureDetector::detectFeatures(const Frame& frame, const unsigned int subFrameLeft, const unsigned int subFrameTop, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const SamplingDense samplingDense, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures& features, Worker* worker)
{
	ocean_assert(frame);

	Frame yFrame;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yFrame, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker))
	{
		ocean_assert(false && "Invalid frame type!");
		return 0u;
	}

	constexpr unsigned int integralFramePaddingElements = 0u;
	Frame integralFrame(FrameType(yFrame.width() + 1u, yFrame.height() + 1u, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT), integralFramePaddingElements);
	CV::IntegralImage::createLinedImage<uint8_t, uint32_t, 1u>(yFrame.constdata<uint8_t>(), integralFrame.data<uint32_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), integralFrame.paddingElements());

	ocean_assert(integralFrame.isContinuous());
	return detectFeatures(integralFrame.data<uint32_t>(), yFrame.width(), yFrame.height(), subFrameLeft, subFrameTop, subFrameWidth, subFrameHeight, samplingDense, threshold, frameIsUndistorted, features, worker);
}

void BlobFeatureDetector::findExtremesSubset(const ResponseMap* low, const ResponseMap* middle, const ResponseMap* high, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures* features, Lock* lock, const unsigned int firstHighRow, const unsigned int numberHighRows)
{
	ocean_assert(low && middle && high);
	ocean_assert(features);

	// if the highest layer holds to less response values to compare any candidates
	if (high->responseWidth() < 3u || high->responseHeight() < 3u)
	{
		return;
	}

	ocean_assert(firstHighRow + numberHighRows <= high->responseHeight() - 2);

	ocean_assert(high->samplingStep() >= low->samplingStep());
	const int samplingLow = high->samplingStep() / low->samplingStep();

	ocean_assert(high->samplingStep() >= middle->samplingStep());
	const int samplingMiddle = high->samplingStep() / middle->samplingStep();

	unsigned int originalLeft = 0u;
	unsigned int originalTop = 0u;
	unsigned int middleResponseLeft = 0u;
	unsigned int middleResponseTop = 0u;
	unsigned int lowResponseLeft = 0u;
	unsigned int lowResponseTop = 0u;

	if (high->calculateOriginalPosition(0, 0, originalLeft, originalTop) == false
		|| middle->calculateResponsePosition(originalLeft, originalTop, middleResponseLeft, middleResponseTop) == false
		|| low->calculateResponsePosition(originalLeft, originalTop, lowResponseLeft, lowResponseTop) == false)
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	BlobFeatures localFeatures;
	localFeatures.reserve(256);

	const Scalar* highResponse = high->filterResponse() + high->responseWidth() * firstHighRow;
	const Scalar* const highResponseEnd = highResponse + high->responseWidth() * numberHighRows;

	unsigned int yHigh = firstHighRow;

	while (highResponse != highResponseEnd)
	{
		const Scalar* const highResponseRowEnd = highResponse + high->responseWidth() - 2;

		const Scalar* middleResponse = middle->filterResponse() + (samplingMiddle * yHigh + middleResponseTop) * middle->responseWidth() + middleResponseLeft;
		const Scalar* lowResponse = low->filterResponse() + (samplingLow * yHigh + lowResponseTop) * low->responseWidth() + lowResponseLeft;

		unsigned int xHigh = 0;

		while (highResponse != highResponseRowEnd)
		{
			const Scalar candidate = *(middleResponse + samplingMiddle * (middle->responseWidth() + 1));

			if (candidate >= threshold
				// high layer
				&& candidate > *highResponse && candidate > *(highResponse + 1) && candidate > *(highResponse + 2)
				&& candidate > *(highResponse + high->responseWidth()) && candidate > *(highResponse + high->responseWidth() + 1) && candidate > *(highResponse + high->responseWidth() + 2)
				&& candidate > *(highResponse + (high->responseWidth() << 1)) && candidate > *(highResponse + (high->responseWidth() << 1) + 1) && candidate > *(highResponse + (high->responseWidth() << 1) + 2)
				// middle layer
				&& candidate > *middleResponse && candidate > *(middleResponse + samplingMiddle) && candidate > *(middleResponse + (samplingMiddle << 1))
				&& candidate > *(middleResponse + middle->responseWidth() * samplingMiddle) && candidate > *(middleResponse + (middle->responseWidth() + 2) * samplingMiddle)
				&& candidate > *(middleResponse + middle->responseWidth() * (samplingMiddle << 1)) && candidate > *(middleResponse + middle->responseWidth() * (samplingMiddle << 1) + samplingMiddle) && candidate > *(middleResponse + (middle->responseWidth() + 1) * (samplingMiddle << 1))
				// low layer
				&& candidate > *lowResponse && candidate > *(lowResponse + samplingLow) && candidate > *(lowResponse + (samplingLow << 1))
				&& candidate > *(lowResponse + low->responseWidth() * samplingLow) && candidate > *(lowResponse + (low->responseWidth() + 1) * samplingLow) && candidate > *(lowResponse + (low->responseWidth() + 2) * samplingLow)
				&& candidate > *(lowResponse + low->responseWidth() * (samplingLow << 1)) && candidate > *(lowResponse + low->responseWidth() * (samplingLow << 1) + samplingLow) && candidate > *(lowResponse + (low->responseWidth() + 1) * (samplingLow << 1)))
			{
				determineExactFeaturePosition(*low, *middle, *high, xHigh, yHigh, 6, frameIsUndistorted ? BlobFeature::DS_UNDISTORTED : BlobFeature::DS_DISTORTED, localFeatures);
			}

			++xHigh;
			++highResponse;
			middleResponse += samplingMiddle;
			lowResponse += samplingLow;
		}

		highResponse += 2;
		++yHigh;
	}

	const OptionalScopedLock optionalScopedLock(lock);

	features->insert(features->end(), localFeatures.begin(), localFeatures.end());
}

void BlobFeatureDetector::findExtremesSubset(const ResponseMap* map, const Scalar threshold, const bool frameIsUndistorted, BlobFeatures* features, Lock* lock, const unsigned int firstResponseRow, const unsigned int numberResponseRows)
{
	ocean_assert(map && features);
	ocean_assert(firstResponseRow + numberResponseRows <= map->responseHeight());
	ocean_assert(numberResponseRows >= 3);

	if (map->responseWidth() < 3u || map->responseHeight() < 3u)
	{
		return;
	}

	BlobFeatures localFeatures;
	localFeatures.reserve(256);

	const unsigned int responseRowStart = max(0, int(firstResponseRow - 1)) + 1; // inclusive position of a response value
	const unsigned int responseRowEnd = min(firstResponseRow + numberResponseRows + 1, map->responseHeight()) - 1; // exclusive position of a response value

	ocean_assert(responseRowStart >= 1);
	ocean_assert(responseRowEnd <= map->responseHeight());

	const Scalar* responseTop = map->filterResponse() + map->responseWidth() * (responseRowStart - 1) + 1;
	const Scalar* responseMiddle = map->filterResponse() + map->responseWidth() * responseRowStart + 1;
	const Scalar* responseBottom = map->filterResponse() + map->responseWidth() * (responseRowStart + 1) + 1;

	const Scalar* const responseMiddleEnd = map->filterResponse() + map->responseWidth() * responseRowEnd + 1;

	unsigned int y = responseRowStart;

	while (responseMiddle != responseMiddleEnd)
	{
		const Scalar* const responseMiddleRowEnd = responseMiddle + map->responseWidth() - 2;

		unsigned int x = 1;

		while (responseMiddle != responseMiddleRowEnd)
		{
			if (*responseMiddle >= threshold && *responseMiddle > *(responseMiddle - 1) && *responseMiddle > *(responseMiddle + 1)
					&& *responseMiddle > *(responseTop - 1) && *responseMiddle > *responseTop && *responseMiddle > *(responseTop + 1)
					&& *responseMiddle > *(responseBottom - 1) && *responseMiddle > *responseBottom && *responseMiddle > *(responseBottom + 1))
			{
				determineExactFeaturePosition(*map, responseTop, responseMiddle, responseBottom, x, y, 6, frameIsUndistorted ? BlobFeature::DS_UNDISTORTED : BlobFeature::DS_DISTORTED, localFeatures);
			}

			++x;

			++responseTop;
			++responseMiddle;
			++responseBottom;
		}

		++y;

		responseTop += 2;
		responseMiddle += 2;
		responseBottom += 2;
	}

	const OptionalScopedLock optionalScopedLock(lock);

	features->insert(features->end(), localFeatures.begin(), localFeatures.end());
}

void BlobFeatureDetector::determineExactFeaturePosition(const ResponseMap& low, const ResponseMap& middle, const ResponseMap& high, const unsigned int xHigh, const unsigned int yHigh, const unsigned int iterations, const BlobFeature::DistortionState distortionState, BlobFeatures& features)
{
	ocean_assert(iterations != 0);

	ocean_assert(high.samplingStep() >= low.samplingStep());
	const int samplingLow = high.samplingStep() / low.samplingStep();

	ocean_assert(high.samplingStep() >= middle.samplingStep());
	const int samplingMiddle = high.samplingStep() / middle.samplingStep();

	unsigned int originalLeft = 0u;
	unsigned int originalTop = 0u;
	unsigned int middleResponseLeft = 0u;
	unsigned int middleResponseTop = 0u;
	unsigned int lowResponseLeft = 0u;
	unsigned int lowResponseTop = 0u;

	if (high.calculateOriginalPosition(xHigh, yHigh, originalLeft, originalTop) == false
			|| middle.calculateResponsePosition(originalLeft, originalTop, middleResponseLeft, middleResponseTop) == false
			|| low.calculateResponsePosition(originalLeft, originalTop, lowResponseLeft, lowResponseTop) == false)
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	const Scalar* highResponse = high.filterResponse() + yHigh * high.responseWidth() + xHigh;
	const Scalar* middleResponse = middle.filterResponse() + middleResponseTop * middle.responseWidth() + middleResponseLeft;
	const Scalar* lowResponse = low.filterResponse() + lowResponseTop * low.responseWidth() + lowResponseLeft;

	const Scalar candidate = *(middleResponse + samplingMiddle * (middle.responseWidth() + 1));

	const Scalar dxLow = *(middleResponse + middle.responseWidth() * samplingMiddle);
	const Scalar dxHigh = *(middleResponse + (middle.responseWidth() + 2) * samplingMiddle);
	const Scalar dx = (dxHigh - dxLow) * Scalar(0.5);
	const Scalar dxx = dxHigh + dxLow - 2 * candidate;

	const Scalar dyHigh = *(middleResponse + middle.responseWidth() * (samplingMiddle << 1) + samplingMiddle);
	const Scalar dyLow = *(middleResponse + samplingMiddle);
	const Scalar dy = (dyHigh - dyLow) * Scalar(0.5);
	const Scalar dyy = dyHigh + dyLow - 2 * candidate;

	const Scalar dsHigh = *(highResponse + high.responseWidth() + 1);
	const Scalar dsLow = *(lowResponse + (low.responseWidth() + 1) * samplingLow);
	const Scalar ds = (dsHigh - dsLow) * Scalar(0.5);
	const Scalar dss = dsHigh + dsLow - 2 * candidate;

	const Scalar dxy = (*middleResponse - *(middleResponse + (samplingMiddle << 1)) - *(middleResponse + middle.responseWidth() * (samplingMiddle << 1)) + *(middleResponse + (middle.responseWidth() + 1) * (samplingMiddle << 1))) * Scalar(0.25);
	const Scalar dxs = (*(highResponse + high.responseWidth() + 2) - *(highResponse + high.responseWidth()) + *(lowResponse + low.responseWidth() * samplingLow) - *(lowResponse + (low.responseWidth() + 2) * samplingLow)) * Scalar(0.25);
	const Scalar dys = (*(highResponse + (high.responseWidth() << 1) + 1) - *(highResponse + 1) + *(lowResponse + samplingLow) - *(lowResponse + low.responseWidth() * (samplingLow << 1) + samplingLow)) * Scalar(0.25);

	const Scalar denominator = 2 * dxs * dxy * dys - dxx * dys * dys - dxs * dxs * dyy + dss * dxx * dyy - dss * dxy * dxy;

	if (Numeric::isNotEqualEps(denominator))
	{
		const Scalar factor = Scalar(1) / denominator;

		const Scalar xOffset = -(((dss * dyy - dys * dys) * dx + (dxs * dys - dss * dxy) * dy + (dxy * dys - dxs * dyy) * ds) * factor);
		const Scalar yOffset = -(((dxs * dys - dss * dxy) * dx + (dss * dxx - dxs * dxs) * dy + (dxs * dxy - dxx * dys) * ds) * factor);
		const Scalar sOffset = -(((dxy * dys - dxs * dyy) * dx + (dxs * dxy - dxx * dys) * dy + (dxx * dyy - dxy * dxy) * ds) * factor);

		Scalar frameX, frameY;

		if (Numeric::abs(xOffset) < Scalar(0.6) && Numeric::abs(yOffset) < Scalar(0.6) && Numeric::abs(sOffset) < Scalar(0.6))
		{
			if (high.calculateOriginalPosition(xOffset + Scalar(xHigh + 1u), yOffset + Scalar(yHigh + 1u), frameX, frameY))
			{
				ocean_assert(high.filterSize() - middle.filterSize() == middle.filterSize() - low.filterSize());

				// a 9x9 filter corresponds to a scale of 1.2; 1.2 / 9 = 0.13333333333333
				const Scalar scale = Scalar(0.1333333333333333) * (Scalar(middle.filterSize()) + Scalar(high.filterSize() - middle.filterSize()) * sOffset);
				ocean_assert(scale > 0);

				ocean_assert(frameX > 0 && frameX < Scalar(high.frameWidth()));
				ocean_assert(frameY > 0 && frameY < Scalar(high.frameHeight()));

				ocean_assert(middleResponse >= middle.filterResponse());

				// offset of the top left 3x3 position added by the offset to receive the middle of the 3x3 area
				const size_t offset = middleResponse - middle.filterResponse() + samplingMiddle * (middle.responseWidth() + 1u);
				ocean_assert(offset < middle.responseWidth() * middle.responseHeight());

				features.emplace_back(Vector2(frameX, frameY), distortionState, scale, candidate, middle.laplacianSign()[offset] != 0u);
			}
			else
				ocean_assert(false && "This should never happen!");
		}
		else if (iterations > 1u)
		{
			unsigned int xHighNew = xHigh;
			unsigned int yHighNew = yHigh;

			if (xOffset > Scalar(0.6) && xHigh + 3u < high.responseWidth())
			{
				++xHighNew;
			}
			else if (xOffset < Scalar(-0.6) && xHigh > 1u)
			{
				--xHighNew;
			}

			if (yOffset > Scalar(0.6) && yHigh + 3 < high.responseHeight())
			{
				++yHighNew;
			}
			else if (yOffset < Scalar(-0.6) && yHigh > 1u)
			{
				--yHighNew;
			}

			if (xHighNew != xHigh || yHighNew != yHigh)
			{
				determineExactFeaturePosition(low, middle, high, xHighNew, yHighNew, iterations - 1u, distortionState, features);
			}
		}
	}
}

void BlobFeatureDetector::determineExactFeaturePosition(const ResponseMap& map, const Scalar* top, const Scalar* middle, const Scalar* bottom, const unsigned int x, const unsigned int y, const unsigned int iterations, const BlobFeature::DistortionState distortionState, BlobFeatures& features)
{
	ocean_assert(top && middle && bottom);

	// [-1 0 1] * 1/2
	const Scalar dx = (*(middle + 1) - *(middle - 1)) * Scalar(0.5);
	const Scalar dy = (*bottom - *top) * Scalar(0.5);

	// [1 -2 1] * 1/1
	const Scalar dxx = *(middle + 1) + *(middle - 1) - *middle * Scalar(2);
	const Scalar dyy = *bottom + *top - *middle * Scalar(2);

	// [ 1  0 -1 ]
	// [ 0  0  0 ] * 1/4
	// [-1  0  1 ]

	const Scalar dxy = (*(bottom + 1) + *(top - 1) - *(bottom - 1) - *(top + 1)) * Scalar(0.25);

	const Scalar denominator = dxx * dyy - dxy * dxy;
	ocean_assert(Numeric::isNotEqualEps(denominator));

	const Scalar factor = 1 / denominator;

	const Scalar offsetX = -(dyy * dx - dxy * dy) * factor;
	const Scalar offsetY = -(dxx * dy - dxy * dx) * factor;

	if (offsetX >= Scalar(-0.6) && offsetX <= Scalar(0.6) && offsetY >= Scalar(-0.6) && offsetY <= Scalar(0.6))
	{
		const Scalar correctedX = Scalar(x) + offsetX;
		const Scalar correctedY = Scalar(y) + offsetY;

		Scalar frameX, frameY;

		if (map.calculateOriginalPosition(correctedX, correctedY, frameX, frameY))
		{
			ocean_assert(frameX >= 0 && frameX < Scalar(map.frameWidth()));
			ocean_assert(frameY >= 0 && frameY < Scalar(map.frameHeight()));

			// a 9x9 filter corresponds to a scale of 1.2; 1.2 / 9 = 0.13333333333333
			const Scalar scale = Scalar(0.1333333333333333) * Scalar(map.filterSize());

			features.emplace_back(Vector2(frameX, frameY), distortionState, scale, *middle, map.laplacianSign()[y * map.responseWidth() + x] != 0u);
		}
	}
	else if (iterations > 1u)
	{
		unsigned int xNew = x;
		unsigned int yNew = y;

		if (offsetX > Scalar(0.6) && x + 2u < map.responseWidth())
		{
			++xNew;
		}
		else if (offsetX < Scalar(-0.6) && x > 1u)
		{
			--xNew;
		}

		if (offsetY > Scalar(0.6) && y + 2u < map.responseHeight())
		{
			++yNew;
		}
		else if (offsetY < Scalar(-0.6) && y > 1u)
		{
			--yNew;
		}

		if (xNew != x || yNew != y)
		{
			ocean_assert(xNew >= 1u && xNew < map.responseWidth() - 1u);
			ocean_assert(yNew >= 1u && yNew < map.responseHeight() - 1u);

			const Scalar* newTop = map.filterResponse() + (yNew - 1u) * map.responseWidth() + xNew + 1u;
			const Scalar* newMiddle = newTop + map.responseWidth();
			const Scalar* newBottom = newMiddle + map.responseWidth();

			determineExactFeaturePosition(map, newTop, newMiddle, newBottom, xNew, yNew, iterations - 1u, distortionState, features);
		}
	}
}

}

}

}

}
