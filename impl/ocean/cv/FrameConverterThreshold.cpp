/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameConverterThreshold.h"
#include "ocean/cv/Histogram.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

void FrameConverterThreshold::convertY8ToB8Subset(uint8_t* buffer, const unsigned int width, const unsigned int height, const unsigned int bufferPaddingElements, const uint8_t threshold, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(buffer != nullptr);
	ocean_assert(width * height > 0);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	ocean_assert(threshold >= 1u);

	const unsigned int bufferStrideElements = width + bufferPaddingElements;

	buffer += firstRow * bufferStrideElements;

	for (unsigned int n = 0u; n < numberRows; ++n)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			buffer[x] = (buffer[x] < threshold) ? 0x00 : 0xFF;
		}

		buffer += bufferStrideElements;
	}
}

void FrameConverterThreshold::convertY8ToB8Subset(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const uint8_t threshold, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source != nullptr);
	ocean_assert(target != nullptr);
	ocean_assert(width * height > 0);
	ocean_assert_and_suppress_unused(firstRow + numberRows <= height, height);

	ocean_assert(threshold >= 1u);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	source += firstRow * sourceStrideElements;
	target += firstRow * targetStrideElements;

	for (unsigned int n = 0u; n < numberRows; ++n)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			target[x] = (source[x] < threshold) ? 0x00 : 0xFF;
		}

		source += sourceStrideElements;
		target += targetStrideElements;
	}
}

void FrameConverterThreshold::convertBorderedY8ToB8Subset(const uint8_t* source, const uint32_t* sourceBorderedIntegral, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int sourceBorderedIntegralPaddingElements, const unsigned int targetPaddingElements, const unsigned int kernelSize, const float threshold, const unsigned int firstRow, const unsigned int numberRows)
{
	ocean_assert(source != nullptr);
	ocean_assert(sourceBorderedIntegral != nullptr);
	ocean_assert(target != nullptr);

	ocean_assert(kernelSize >= 3u && (kernelSize % 2u) == 1u);
	ocean_assert(threshold > 0.0f && threshold < 1.0f);

	ocean_assert_and_suppress_unused(width * height != 0u, height);

	const unsigned int sourceStrideElements = width + sourcePaddingElements;
	const unsigned int targetStrideElements = width + targetPaddingElements;

	const unsigned int kernelSquare = kernelSize * kernelSize;
	const unsigned int borderedWidth = width + kernelSize;

	const unsigned int sourceBorderedIntegralStrideElements = borderedWidth + sourceBorderedIntegralPaddingElements;

	source += firstRow * sourceStrideElements;
	target += firstRow * targetStrideElements;

	const uint32_t* upper = sourceBorderedIntegral + firstRow * sourceBorderedIntegralStrideElements;
	const uint32_t* lower = upper + sourceBorderedIntegralStrideElements * kernelSize;

	const uint32_t factor = uint32_t(float(kernelSquare) / threshold);

	for (unsigned int n = 0u; n < numberRows; ++n)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const uint32_t integralValue = lower[x + kernelSize] - lower[x] - upper[x + kernelSize] + upper[x];

			if (uint32_t(source[x]) * factor < integralValue)
			{
				target[x] = 0x00;
			}
			else
			{
				target[x] = 0xFF;
			}
		}

		source += sourceStrideElements;
		target += targetStrideElements;

		upper += sourceBorderedIntegralStrideElements;
		lower += sourceBorderedIntegralStrideElements;
	}
}

uint8_t FrameConverterThreshold::calculateOtsuThreshold(const uint8_t* source, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, Worker* worker)
{
	ocean_assert(source != nullptr);
	ocean_assert(width * height > 0);

	uint8_t calculatedThreshold(0u);

	const Histogram::Histogram8BitPerChannel<1u> hist(Histogram::determineHistogram8BitPerChannel<1u>(source, width, height, sourcePaddingElements, worker));

	// mean value of the processing region
	Scalar mean(0);
	Scalar histogram[256];


	const Scalar size (Scalar(1) / Scalar(width * height));
	for (unsigned int i = 0u; i < 256u; i++)
	{
		histogram[i] = Scalar(hist.bin<0u>(uint8_t(i))) * size;
		mean += histogram[i] * Scalar(i);
	}

	Scalar max(Numeric::minValue());

	// initial class probabilities
	Scalar class1Probability(0);
	Scalar class2Probability(1);

	// initial class 1 mean value
	Scalar class1MeanInit(0);

	// check all thresholds
	for (unsigned int t = 0u; t < 256u && class2Probability > 0u; t++)
	{
		// calculate class means for the given threshold
		const Scalar class1Mean = class1MeanInit;
		const Scalar class2Mean = (mean - (class1Mean * class1Probability)) / class2Probability;

		// calculate between class variance
		const Scalar squareMean((class1Mean - class2Mean)*(class1Mean - class2Mean));
		const Scalar betweenClassVariance = class1Probability * (Scalar(1) - class1Probability) * squareMean;

		// check if we found new threshold candidate
		if (betweenClassVariance > max)
		{
			max = betweenClassVariance;
			calculatedThreshold = uint8_t(t);
		}

		// update initial probabilities and mean value
		class1MeanInit *= class1Probability;

		class1Probability += histogram[t];
		class2Probability -= histogram[t];

		class1MeanInit += Scalar(t) * histogram[t];

		if (class1Probability != 0)//Numeric::isNotEqualEps(class1Probability))
		{
			class1MeanInit /= class1Probability;
		}
	}
	return calculatedThreshold;
}

}

}
