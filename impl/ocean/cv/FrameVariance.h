/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_VARIANCE_H
#define META_OCEAN_CV_FRAME_VARIANCE_H

#include "ocean/cv/CV.h"

#include "ocean/math/Numeric.h"

namespace Ocean
{

namespace CV
{


/**
 * This class implements functions to determine the frame variance.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameVariance
{
	public:

		/**
		 * This functions determines the frame deviation of a 1 channel 8 bit frame.
		 * @param frame Original frame to determine the variance for
		 * @param integral Bordered integral frame of the original frame with border size equal have of the specified window size, the border must be mirrored
		 * @param deviation Resulting deviation frame
		 * @param width The width of the original frame
		 * @param height The height of the original frame
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param deviationPaddingElements The number of padding elements at the end of each deviation row, in elements, with range [0, infinity)
		 * @param window Size of the window must be odd, with range [1, infinity)
		 * @return True, if succeeded
		 */
		template <typename T, typename TIntegral>
		static bool deviation1Channel8Bit(const T* frame, const TIntegral* integral, uint8_t* deviation, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int deviationPaddingElements, const unsigned int window);

		/**
		 * This functions determines the deviation within a 1-channel 8 bit frame.<br>
		 * The function uses an integral image for the calculation of the deviation.
		 * @param frame The frame for which the deviation will be determined, must be valid
		 * @param deviation Resulting deviation frame
		 * @param width The width of the given frame, in pixel, with range [1, infinity)
		 * @param height The height of the given frame, in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param deviationPaddingElements The number of padding elements at the end of each deviation row, in elements, with range [0, infinity)
		 * @param window Size of the window must be odd, with range [1, min(width, height) * 2]
		 * @return True, if succeeded
		 * @tparam T The data type of the frame elements, either 'int8_t' or 'uint8_t'
		 */
		template <typename T>
		static bool deviation1Channel8Bit(const T* frame, uint8_t* deviation, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int deviationPaddingElements, const unsigned int window);

		/**
		 * Compute the per-channel mean, variance, and standard deviation over an image
		 *
		 * The basic outline of the computation is this (this example assumes no multi-channel and no stride but the actual implementation supports both, of course):
		 * @code
		 * T0 imagePixels[pixelCount] = { ... };
		 * T1 sum = 0;
		 * T1 squareSum = 0;
		 * for (i = 0; i < pixelCount; ++i)
		 * {
		 *     sum[i] += T1(imagePixels[i]);
		 *     squareSum[i] += T1(T2(imagePixel[i]) * T2(imagePixels[i]))
		 * }
		 * double mean = (double)sum / (double)pixelCount
		 * double variance = ((double)squareSum / pixelCount) - (mean * mean);
		 * double stddev = std::sqrt(variance)
		 *
		 * // Type `T0` is the type of the pixel elements, `T1` is used for the summations and type `T2` is used for multiplications.
		 * // This allows for a optimal control over how to compute and with what precision.
		 * // Example configurations are:
		 * // - `T0`=float,         `T1`=float,              `T2`=double
		 * // - `T0`=unsigned char, `T1`=unsigned long long, `T2`=unsigned short
		 * @endcode
		 * @note The most related OpenCV function is cv::meanStdDev()
		 * @param frame The input frame; mustn't be empty and must be a 1-channel image
		 * @param width The width of the input image, range: [1, infinity)
		 * @param height The height of the input image, range: [1, infinity)
		 * @param framePaddingElements Optional number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param mean Array for the resulting per-channel mean values, will be ignored if set to `nullptr` otherwise it must have `channels` elements
		 * @param variance Array for the resulting per-channel variance, will be ignored if set to `nullptr` otherwise it must have `channels` elements
		 * @param standardDeviation Array for the resulting per-channel standard deviations, will be ignored if set to `nullptr` otherwise it must have `channels` elements
		 * @tparam TElementType Type of the elements of the pixels of the input image
		 * @tparam TSummationType Type used for the internal computation of the pixel sums (should be at least as large as `TElementType`), cf. function description for details
		 * @tparam TMultiplicationType Type used to compute the square values of pixels (allows control over integer vs. floating point multiplication), cf. function description for details
		 * @tparam tChannels Number of channels of the input image, range: [1, infinity)
		 */
		template <typename TElementType, typename TSummationType, typename TMultiplicationType, unsigned int tChannels>
		static void imageStatistics(const TElementType* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, double* mean = nullptr, double* variance = nullptr, double* standardDeviation = nullptr);
};

template <typename TElementType, typename TSummationType, typename TMultiplicationType, unsigned int tChannels>
void FrameVariance::imageStatistics(const TElementType* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, double* mean, double* variance, double* standardDeviation)
{
	static_assert(tChannels != 0u, "Number of channels must be in the range [1, infinity)");
	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int pixelCount = width * height;
	const unsigned int elementsCount = (width * tChannels + framePaddingElements) * height;

	const TElementType* const frameEnd = frame + elementsCount;

	TSummationType sum[tChannels];
	TSummationType squareSum[tChannels];
	memset(sum, 0, tChannels * sizeof(TSummationType));
	memset(squareSum, 0, tChannels * sizeof(TSummationType));

	if (framePaddingElements == 0u)
	{
		for (unsigned int i = 0u; i < pixelCount; ++i)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				ocean_assert_and_suppress_unused(frame < frameEnd, frameEnd);

				sum[c] += TSummationType(*frame);

				ocean_assert(std::is_integral<TSummationType>::value == false || (squareSum[c] <= NumericT<TSummationType>::maxValue() - TSummationType(TMultiplicationType(*frame) * TMultiplicationType(*frame)) && "Integer overflow; TSummationType must be a wider type, cf. NextLargerTyper<TSummationType>::Type"));

				squareSum[c] += TSummationType(TMultiplicationType(*frame) * TMultiplicationType(*frame));

				frame++;
			}
		}
	}
	else
	{
		for (unsigned int y = 0u; y < height; ++y)
		{
			for (unsigned int x = 0u; x < width; ++x)
			{
				for (unsigned int c = 0u; c < tChannels; ++c)
				{
					ocean_assert(frame < frameEnd);

					sum[c] += TSummationType(*frame);

					ocean_assert(std::is_integral<TSummationType>::value == false || (squareSum[c] <= NumericT<TSummationType>::maxValue() - TSummationType(TMultiplicationType(*frame) * TMultiplicationType(*frame)) && "Integer overflow; TSummationType must be a wider type, cf. NextLargerTyper<TSummationType>::Type"));

					squareSum[c] += TSummationType(TMultiplicationType(*frame) * TMultiplicationType(*frame));

					frame++;
				}
			}

			frame += framePaddingElements;
		}
	}

	double localMean[tChannels];
	double localVariance[tChannels];

	ocean_assert(pixelCount != 0u);
	const double normalizer = 1.0 / double(pixelCount);

	for (unsigned int c = 0u; c < tChannels; ++c)
	{
		localMean[c] = double(sum[c]) * normalizer;

		// var = sum((I_i - mean)^2) / N, i = 1...N
		//     = (sum(I_i^2)) / N) - (2 * sum(I_i) * mean / N) + mean^2
		//     = (sum(I_i^2)) / N) - (2 * mean * mean)         + mean^2
		//     = (sum(I_i^2)) / N) - mean^2
		localVariance[c] = std::max(0.0, (double(squareSum[c]) * normalizer) - (localMean[c] * localMean[c]));
	}

	if (mean)
	{
		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			mean[c] = localMean[c];
		}
	}

	if (variance)
	{
		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			variance[c] = localVariance[c];
		}
	}

	if (standardDeviation)
	{
		for (unsigned int c = 0u; c < tChannels; ++c)
		{
			ocean_assert(localVariance[c] >= 0.0);
			standardDeviation[c] = NumericD::sqrt(localVariance[c]);
		}
	}
}

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_FRAME_VARIANCE_H
