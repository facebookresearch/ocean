/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameVariance.h"
#include "ocean/cv/IntegralImage.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"

#include "ocean/math/Approximation.h"
#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{

template <typename T, typename TIntegral>
bool FrameVariance::deviation1Channel8Bit(const T* frame, const TIntegral* integral, uint8_t* deviation, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int deviationPaddingElements, const unsigned int window)
{
	ocean_assert(frame != nullptr && integral != nullptr && deviation != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(window > 0u && window % 2u == 1u);

	if (window % 2u != 1u)
	{
		return false;
	}

	const unsigned int border = window / 2u;

	ocean_assert(border <= std::min(width, height));
	if (border > std::min(width, height))
	{
		return false;
	}

	const unsigned int integralWidth = width + window;
	const unsigned int integralHeight = height + window;

	Frame integralSquared(FrameType(integralWidth, integralHeight, FrameType::FORMAT_Y64, FrameType::ORIGIN_UPPER_LEFT));
	IntegralImage::createBorderedImageSquaredMirror<T, uint64_t, 1u>(frame, integralSquared.data<uint64_t>(), width, height, border, framePaddingElements, integralSquared.paddingElements());

	ocean_assert(integralSquared.isContinuous());

	const TIntegral* integral0 = integral;
	const TIntegral* integral1 = integral0 + integralWidth * window;

	const uint64_t* integralSquared0 = integralSquared.constdata<uint64_t>();
	const uint64_t* integralSquared1 = integralSquared0 + integralWidth * window;
	const uint64_t area = window * window;

	const Scalar normalization = Scalar(1) / Scalar(area);

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			const uint64_t sqrValues = *integralSquared0 - *(integralSquared0 + window) - *integralSquared1 + *(integralSquared1 + window);
			const TIntegral values = *integral0 - *(integral0 + window) - *integral1 + *(integral1 + window);

			const Scalar meanSqrValues = Scalar(sqrValues) * normalization;
			const Scalar sqrMeanValues = Numeric::sqr(Scalar(values) * normalization);

			ocean_assert(meanSqrValues >= sqrMeanValues);

			const uint16_t variance = uint16_t(meanSqrValues - sqrMeanValues + Scalar(0.5));
			ocean_assert(variance <= 128u * 128u);

			*deviation = Approximation::sqrt(variance);

			++deviation;
			++integral0;
			++integral1;
			++integralSquared0;
			++integralSquared1;
		}

		deviation += deviationPaddingElements;

		integral0 += window;
		integral1 += window;
		integralSquared0 += window;
		integralSquared1 += window;
	}

	return true;
}

template bool OCEAN_CV_EXPORT FrameVariance::deviation1Channel8Bit(const int8_t*, const int32_t*, uint8_t*, const unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int);
template bool OCEAN_CV_EXPORT FrameVariance::deviation1Channel8Bit(const uint8_t*, const uint32_t*, uint8_t*, const unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int);

template <typename T>
bool FrameVariance::deviation1Channel8Bit(const T* frame, uint8_t* deviation, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int deviationPaddingElements, const unsigned int window)
{
	ocean_assert(frame != nullptr);
	ocean_assert(deviation != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	if (window % 2u != 1u)
	{
		return false;
	}

	const unsigned int border = window / 2u;

	ocean_assert(border <= std::min(width, height));
	if (border > std::min(width, height))
	{
		return false;
	}

	using TIntegral = typename NextLargerTyper<T>::TypePerformance;
	static_assert(std::is_signed<T>::value == std::is_signed<TIntegral>::value, "Invalid data type!");

	Frame integralFrame(FrameType(width + window, height + window, FrameType::FORMAT_Y32, FrameType::ORIGIN_UPPER_LEFT));
	CV::IntegralImage::createBorderedImageMirror<T, TIntegral, 1u>(frame, integralFrame.data<TIntegral>(), width, height, border, framePaddingElements, integralFrame.paddingElements());

	ocean_assert(integralFrame.isContinuous());
	return deviation1Channel8Bit(frame, integralFrame.constdata<TIntegral>(), deviation, width, height, framePaddingElements, deviationPaddingElements, window);
}

template bool OCEAN_CV_EXPORT FrameVariance::deviation1Channel8Bit(const int8_t*, uint8_t*, const unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int);
template bool OCEAN_CV_EXPORT FrameVariance::deviation1Channel8Bit(const uint8_t*, uint8_t*, const unsigned int, const unsigned int, const unsigned int, const unsigned int, const unsigned int);

}

}
