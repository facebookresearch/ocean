/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_FILTER_GAUSSIAN_H
#define META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_FILTER_GAUSSIAN_H

#include "ocean/cv/advanced/Advanced.h"
#include "ocean/cv/advanced/AdvancedFrameFilterSeparable.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameFilterGaussian.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements an advanced Gaussian filter.
 * @ingroup cvadvanced
 */
class AdvancedFrameFilterGaussian
{
	public:

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_ADVANCED_EXPORT Comfort
		{
			public:

				/**
				 * Applies a Gaussian blur filter to a given frame in place.
				 * @param frame The frame to which the blur filter will be applied, with resolution [filterSize/2+1, infinity)x[filterSize/2+1, infinity), must be valid
				 * @param mask The mask frame specifying valid and invalid pixels (before and after filtering), must be valid
				 * @param filterSize The size of the filter to be applied, with range [1, infinity), must be odd
				 * @param maskValue The pixel value for an invalid mask pixel, with range [0, infinity)
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool filter(Frame& frame, Frame& mask, const unsigned int filterSize, const uint8_t maskValue = 0x00u, Worker* worker = nullptr);

				/**
				 * Applies a Gaussian blur filter to a given source image and copies the resulting filter results to a given output frame.
				 * If the target frame type does not match the source frame type the target frame type will be adjusted.
				 * @param source The source frame to which the blur filter will be applied, , with resolution [filterSize/2+1, infinity)x[filterSize/2+1, infinity), must be valid
				 * @param sourceMask The mask frame specifying valid and invalid source pixels, must be valid
				 * @param target The target frame receiving the blurred image content, will be set to the correct frame type if invalid or not matching
				 * @param targetMask The mask frame specifying valid and invalid target pixels, must be valid
				 * @param filterSize The size of the filter to be applied, with range [1, infinity), must be odd
				 * @param maskValue The pixel value for an invalid mask pixel, with range [0, infinity)
				 * @param worker Optional worker object to distribute the computational load
				 * @return True, if succeeded
				 */
				static bool filter(const Frame& source, const Frame& sourceMask, Frame& target, Frame& targetMask, const unsigned int filterSize, const uint8_t maskValue = 0x00u, Worker* worker = nullptr);
		};

		/**
		 * Determines 1D Gaussian blur filter factors for a given filter size.
		 * @param filterSize The size of the filter in pixel, with range [1, infinity), must be odd
		 * @param filter The buffer receiving the resulting filter values, must be valid
		 * @tparam T The data type of the filter elements, e.g., 'unsigned int', or 'float'
		 * @see determineFilterFactorsWithExplicitSigma().
		 */
		template <typename T>
		static void determineFilterFactors(const unsigned int filterSize, T* filter);

		/**
		 * Determines 1D Gaussian blur filter factors for a given filter size.
		 * @param filterSize The size of the filter in pixel, with range [1, infinity), must be odd
		 * @param sigma The explicit sigma which will be used to determine the filter values, with range (0, infinity)
		 * @param filter The buffer receiving the resulting filter values, must be valid
		 * @tparam T The data type of the filter elements, must be  'unsigned int', or 'float', or 'double'
		 * @see determineFilterFactors().
		 */
		template <typename T>
		static void determineFilterFactorsWithExplicitSigma(const unsigned int filterSize, const float sigma, T* filter);

		/**
		 * Applies a Gaussian blur filter to a given frame.
		 * @param frame The source frame to be filtered, must be valid
		 * @param mask The mask frame specifying valid and invalid source pixels (before and after filtering), must be valid
		 * @param width The width of the source (and target) frame in pixel, with range [horizontalFilterSize/2+1, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [verticalFilterSize/2+1, infinity)
		 * @param channels The number of channels the source frame (and target frame) has, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param horizontalFilterSize The number of elements the horizontal filter has, with range [1, width], must be odd
		 * @param verticalFilterSize The number of elements the vertical filter has, with range [1, height], must be odd
		 * @param sigma The Optional sigma that is applied explicitly, with range (0, infinity), -1 to calculate the sigma automatically based on the filter sizes
		 * @param maskValue The pixel value for an invalid mask pixel, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T The data type of each pixel channel of the source frame (and target frame) e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of each filter elements e.g., 'unsigned int', or 'float'
		 */
		template <typename T, typename TFilter>
		static void filter(T* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const float sigma = -1.0f, const uint8_t maskValue = 0x00u, Worker* worker = nullptr);

		/**
		 * Applies a Gaussian blur filter to a given frame.
		 * @param source The source frame to be filtered, must be valid
		 * @param sourceMask The mask frame specifying valid and invalid source pixels, must be valid
		 * @param target The target frame receiving the filtered results, can be the same memory pointer as 'source', must be valid
		 * @param targetMask the mask frame specifying valid and invalid target pixels, must be valid
		 * @param width The width of the source (and target) frame in pixel, with range [horizontalFilterSize/2+1, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [verticalFilterSize/2+1, infinity)
		 * @param channels The number of channels the source frame (and target frame) has, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param sourceMaskPaddingElements The number of padding elements at the end of each source mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param targetMaskPaddingElements The number of padding elements at the end of each target mask row, in elements, with range [0, infinity)
		 * @param horizontalFilterSize The number of elements the horizontal filter has, with range [1, width], must be odd
		 * @param verticalFilterSize The number of elements the vertical filter has, with range [1, height], must be odd
		 * @param sigma The Optional sigma that is applied explicitly, with range (0, infinity), -1 to calculate the sigma automatically based on the filter sizes
		 * @param maskValue The pixel value for an invalid mask pixel, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam T The data type of each pixel channel of the source frame (and target frame) e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of each filter elements e.g., 'unsigned int', or 'float'
		 */
		template <typename T, typename TFilter>
		static void filter(const T* source, const uint8_t* sourceMask, T* target, uint8_t* targetMask, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int sourceMaskPaddingElements, const unsigned int targetPaddingElements, const unsigned int targetMaskPaddingElements, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const float sigma = -1.0f, const uint8_t maskValue = 0x00u, Worker* worker = nullptr);
};

template <typename T>
void AdvancedFrameFilterGaussian::determineFilterFactors(const unsigned int filterSize, T* filter)
{
	ocean_assert(filterSize % 2u == 1u);
	ocean_assert(filter != nullptr);

	if constexpr (std::is_floating_point<T>::value)
	{
		std::vector<unsigned int> integerFilter(filterSize);
		FrameFilterGaussian::determineFilterFactors<unsigned int>(filterSize, integerFilter.data());

		for (unsigned int n = 0u; n < filterSize; ++n)
		{
			filter[n] = T(integerFilter[n]);
		}
	}
	else
	{
		FrameFilterGaussian::determineFilterFactors<T>(filterSize, filter);
	}
}

template <typename T>
void AdvancedFrameFilterGaussian::determineFilterFactorsWithExplicitSigma(const unsigned int filterSize, const float sigma, T* filter)
{
	if constexpr (std::is_floating_point<T>::value)
	{
		std::vector<unsigned int> integerFilter(filterSize);
		FrameFilterGaussian::determineFilterFactorsWithExplicitSigma<unsigned int>(filterSize, sigma, integerFilter.data());

		for (unsigned int n = 0u; n < filterSize; ++n)
		{
			filter[n] = T(integerFilter[n]);
		}
	}
	else
	{
		FrameFilterGaussian::determineFilterFactorsWithExplicitSigma<T>(filterSize, sigma, filter);
	}
}

template <typename T, typename TFilter>
void AdvancedFrameFilterGaussian::filter(T* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const float sigma, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(frame != nullptr && mask != nullptr);

	ocean_assert(horizontalFilterSize >= 1u && horizontalFilterSize % 2u == 1u);
	ocean_assert(verticalFilterSize >= 1u && verticalFilterSize % 2u == 1u);

	const unsigned int horizontalFilterSize_2 = horizontalFilterSize / 2u;
	const unsigned int verticalFilterSize_2 = verticalFilterSize / 2u;
	ocean_assert_and_suppress_unused(width >= horizontalFilterSize_2 + 1u, horizontalFilterSize_2);
	ocean_assert_and_suppress_unused(height >= verticalFilterSize_2 + 1u, verticalFilterSize_2);

	std::vector<TFilter> horizontalFilter(horizontalFilterSize, 0u);

	if (sigma <= 0.0f)
	{
		determineFilterFactors(horizontalFilterSize, horizontalFilter.data());
	}
	else
	{
		determineFilterFactorsWithExplicitSigma(horizontalFilterSize, sigma, horizontalFilter.data());
	}

	if (horizontalFilterSize == verticalFilterSize)
	{
		AdvancedFrameFilterSeparable::filter<T, TFilter>(frame, mask, width, height, channels, framePaddingElements, maskPaddingElements, horizontalFilter.data(), horizontalFilterSize, horizontalFilter.data(), horizontalFilterSize, maskValue, worker);
	}
	else
	{
		std::vector<TFilter> verticalFilter(verticalFilterSize, 0u);

		if (sigma <= 0.0f)
		{
			determineFilterFactors(verticalFilterSize, verticalFilter.data());
		}
		else
		{
			determineFilterFactorsWithExplicitSigma(verticalFilterSize, sigma, verticalFilter.data());
		}

		AdvancedFrameFilterSeparable::filter<T, TFilter>(frame, mask, width, height, channels, framePaddingElements, maskPaddingElements, horizontalFilter.data(), horizontalFilterSize, verticalFilter.data(), verticalFilterSize, maskValue, worker);
	}
}

template <typename T, typename TFilter>
void AdvancedFrameFilterGaussian::filter(const T* source, const uint8_t* sourceMask, T* target, uint8_t* targetMask, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int sourceMaskPaddingElements, const unsigned int targetPaddingElements, const unsigned int targetMaskPaddingElements, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const float sigma, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(source != nullptr && sourceMask != nullptr && target != nullptr && targetMask != nullptr);

	ocean_assert(horizontalFilterSize >= 1u && horizontalFilterSize % 2u == 1u);
	ocean_assert(verticalFilterSize >= 1u && verticalFilterSize % 2u == 1u);

	const unsigned int horizontalFilterSize_2 = horizontalFilterSize / 2u;
	const unsigned int verticalFilterSize_2 = verticalFilterSize / 2u;
	ocean_assert_and_suppress_unused(width >= horizontalFilterSize_2 + 1u, horizontalFilterSize_2);
	ocean_assert_and_suppress_unused(height >= verticalFilterSize_2 + 1u, verticalFilterSize_2);

	std::vector<TFilter> horizontalFilter(horizontalFilterSize, 0u);

	if (sigma <= 0.0f)
	{
		determineFilterFactors(horizontalFilterSize, horizontalFilter.data());
	}
	else
	{
		determineFilterFactorsWithExplicitSigma(horizontalFilterSize, sigma, horizontalFilter.data());
	}

	if (horizontalFilterSize == verticalFilterSize)
	{
		AdvancedFrameFilterSeparable::filter<T, TFilter>(source, sourceMask, target, targetMask, width, height, channels, sourcePaddingElements, sourceMaskPaddingElements, targetPaddingElements, targetMaskPaddingElements, horizontalFilter.data(), horizontalFilterSize, horizontalFilter.data(), horizontalFilterSize, maskValue, worker);
	}
	else
	{
		std::vector<TFilter> verticalFilter(verticalFilterSize, 0u);

		if (sigma <= 0.0f)
		{
			determineFilterFactors(verticalFilterSize, verticalFilter.data());
		}
		else
		{
			determineFilterFactorsWithExplicitSigma(verticalFilterSize, sigma, verticalFilter.data());
		}

		AdvancedFrameFilterSeparable::filter<T, TFilter>(source, sourceMask, target, targetMask, width, height, channels, sourcePaddingElements, sourceMaskPaddingElements, targetPaddingElements, targetMaskPaddingElements, horizontalFilter.data(), horizontalFilterSize, verticalFilter.data(), verticalFilterSize, maskValue, worker);
	}
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_ADVANCED_FRAME_FILTER_GAUSSIAN_H
