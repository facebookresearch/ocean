/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_GAUSSIAN_H
#define META_OCEAN_CV_FRAME_FILTER_GAUSSIAN_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameFilterSeparable.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"
#include "ocean/base/ScopedValue.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements Gaussian image blur filters.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterGaussian
{
	public:

		/**
		 * This class holds re-usable memory for the filtering process.
		 */
		class ReusableMemory
		{
			friend class FrameFilterGaussian;

			public:

				/**
				 * Default constructor.
				 */
				ReusableMemory() = default;

			protected:

				/// The reusable memory object for the separable filter.
				FrameFilterSeparable::ReusableMemory separableReusableMemory_;

				/// The reusable memory for horizontal filter factors.
				Memory horizontalFilterMemory_;

				/// The reusable memory for vertical filter factors.
				Memory verticalFilterMemory_;

				/// The reusable memory for several response rows.
				Memory responseRowsMemory_;
		};

	public:

		/**
		 * Calculates the ideal size of a box filter for a specified sigma defining the shape of the Gauss distribution.
		 * @param sigma The sigma defining the shape of the Gauss distribution in pixel, with range (0, infinity)
		 * @return The ideal size of the box filter in pixel, with range [1, infinity], will be odd
		 * @tparam T The data type of sigma, should be 'float' or 'double'
		 */
		template <typename T>
		static inline unsigned int sigma2filterSize(const T sigma);

		/**
		 * Calculates the sigma corresponding to a specified box filter so that the Gauss distribution using the sigma represents the box filter.
		 * @param filterSize The size of the filter in pixel, with range [1, infinity), must be odd
		 * @return The resulting sigma in pixel, with range (0, infinity)
		 * @tparam T The data type of sigma, should be 'float' or 'double'
		 */
		template <typename T>
		static inline T filterSize2sigma(const unsigned int filterSize);

		/**
		 * Determines 1D Gaussian blur filter factors for a given filter size.
		 * The resulting filter will be normalized for filter values with floating point precision and will not be normalized for filter values with integer precision.<br>
		 * This function will determine the sigma based on the specified size of the filter by using 'filterSize2sigma'.
		 * @param filterSize The size of the filter in pixel, with range [1, infinity), must be odd
		 * @param filter The buffer receiving the resulting filter values, must be valid
		 * @param denominator Optional resulting denominator if the resulting filter values are not normalized
		 * @tparam T The data type of the filter elements, e.g., 'unsigned int', or 'float'
		 * @see filterSize2sigma(), determineFilterFactorsWithExplicitSigma().
		 */
		template <typename T>
		static void determineFilterFactors(const unsigned int filterSize, T* filter, T* denominator = nullptr);

		/**
		 * Determines 1D Gaussian blur filter factors for a given filter size.
		 * The resulting filter will be normalized for filter values with floating point precision and will not be normalized for filter values with integer precision.<br>
		 * Information: This function is the equivalent to OpenCV's cv::getGaussianKerne().
		 * @param filterSize The size of the filter in pixel, with range [1, infinity), must be odd
		 * @param sigma The explicit sigma which will be used to determine the filter values, with range (0, infinity)
		 * @param filter The buffer receiving the resulting filter values, must be valid
		 * @param denominator Optional resulting denominator if the resulting filter values are not normalized
		 * @tparam T The data type of the filter elements, must be  'unsigned int', or 'float', or 'double'
		 * @see determineFilterFactors(), filterSize2sigma().
		 */
		template <typename T>
		static void determineFilterFactorsWithExplicitSigma(const unsigned int filterSize, const float sigma, T* filter, T* denominator = nullptr);

		/**
		 * Applies a Gaussian blur filter to a given source image and copies the resulting filter results to a given output frame.
		 * If the target frame type does not match the source frame type the target frame type will be adjusted.
		 * Information: This function is the equivalent to OpenCV's cv::GaussianBlur().
		 * @param source The source frame to which the blur filter will be applied, must be valid
		 * @param target The target frame receiving the blurred image content, will be set to the correct frame type if invalid or not matching
		 * @param filterSize The size of the filter to be applied, with range [1, min(source.width(), source.height())], must be odd
		 * @param worker Optional worker object to distribute the computational load
		 * @param reusableMemory An optional object holding reusable memory which can be used during filtering, nullptr otherwise
		 * @return True, if succeeded
		 */
		static bool filter(const Frame& source, Frame& target, const unsigned int filterSize, Worker* worker = nullptr, ReusableMemory* reusableMemory = nullptr);

		/**
		 * Applies a Gaussian blur filter to a given frame.
		 * In case the given frame is a read-only frame, the frame will be replaced with a new frame owning the memory.<br>
		 * In case the given frame is a writable frame, the filter will be applied in place.
		 * @param frame The frame to which the blur filter will be applied, must be valid
		 * @param filterSize The size of the filter to be applied, with range [1, min(source.width(), source.height())], must be odd
		 * @param worker Optional worker object to distribute the computational load
		 * @param reusableMemory An optional object holding reusable memory which can be used during filtering, nullptr otherwise
		 * @return True, if succeeded
		 */
		static bool filter(Frame& frame, const unsigned int filterSize, Worker* worker = nullptr, ReusableMemory* reusableMemory = nullptr);

		/**
		 * Applies a Gaussian blur filter to a given frame.
		 * @param source The source frame to be filtered, must be valid
		 * @param target The target frame receiving the filtered results, can be the same memory pointer as 'source', must be valid
		 * @param width The width of the source (and target) frame in pixel, with range [tFilterSize, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [tFilterSize, infinity)
		 * @param channels The number of channels the source frame (and target frame) has, with range [1, infinity)
		 * @param sourcePaddingElements The number of padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param horizontalFilterSize The number of elements the horizontal filter has, with range [1, width], must be odd
		 * @param verticalFilterSize The number of elements the vertical filter has, with range [1, height], must be odd
		 * @param sigma The Optional sigma that is applied explicitly, with range (0, infinity), -1 to calculate the sigma automatically based on the filter sizes
		 * @param worker Optional worker object to distribute the computation
		 * @param reusableMemory An optional object holding reusable memory which can be used during filtering, nullptr otherwise
		 * @param processorInstructions The set of available instructions, may be any combination of instructions
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel of the source frame (and target frame) e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of each filter elements e.g., 'unsigned int', or 'float'
		 */
		template <typename T, typename TFilter>
		static bool filter(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const float sigma = -1.0f, Worker* worker = nullptr, ReusableMemory* reusableMemory = nullptr, const ProcessorInstructions processorInstructions = Processor::get().instructions());

		/**
		 * Applies a Gaussian blur filter to a given frame.
		 * @param frame The frame to be filtered, must be valid
		 * @param width The width of the frame in pixel, with range [tFilterSize, infinity)
		 * @param height The height of the frame in pixel, with range [tFilterSize, infinity)
		 * @param channels The number of channels the source frame (and target frame) has, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param horizontalFilterSize The number of elements the horizontal filter has, with range [1, width], must be odd
		 * @param verticalFilterSize The number of elements the vertical filter has, with range [1, height], must be odd
		 * @param sigma The Optional sigma that is applied explicitly, with range (0, infinity), -1 to calculate the sigma automatically based on the filter sizes
		 * @param worker Optional worker object to distribute the computation
		 * @param reusableMemory An optional object holding reusable memory which can be used during filtering, nullptr otherwise
		 * @param processorInstructions The set of available instructions, may be any combination of instructions
		 * @return True, if succeeded
		 * @tparam T The data type of each pixel channel of the source frame (and target frame) e.g., 'uint8_t', or 'float'
		 * @tparam TFilter The data type of each filter elements e.g., 'unsigned int', or 'float'
		 */
		template <typename T, typename TFilter>
		static inline bool filter(T* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int framePaddingElements, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const float sigma = -1.0f, Worker* worker = nullptr, ReusableMemory* reusableMemory = nullptr, const ProcessorInstructions processorInstructions = Processor::get().instructions());

	protected:

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

		/**
		 * Applies a horizontal and vertical filtering with a Gaussian kernel with size 3, applying a horizontal and vertical 121 filter kernel.
		 * The frame must be a 1 channel 8 bit per pixel image.<br>
		 * Instead of applying a separated horizontal and vertical filter, the function applies the 2D filter directly to speed up the process significantly.<br>
		 * This function applies NEON instructions and can handle frames with width >= 18 pixels only.
		 * @param source The source frame to be filtered, must be valid
		 * @param target The target frame receiving the filtered results, must be valid
		 * @param width The width of the source (and target) frame in pixel, with range [18, infinity)
		 * @param height The height of the source (and target) frame in pixel, with range [1, infinity)
		 * @param sourcePaddingElements Optional padding elements at the end of each source row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param reusableMemory An optional object holding reusable memory which can be used during filtering, nullptr otherwise
		 */
		static inline void filter1Channel8Bit121NEON(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, ReusableMemory* reusableMemory);

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10
};

template <typename T>
inline unsigned int FrameFilterGaussian::sigma2filterSize(const T sigma)
{
	ocean_assert(sigma > NumericT<T>::eps());

	const unsigned int size = (unsigned int)NumericT<T>::ceil((sigma - T(0.8)) * T(6.666666666) + T(2.999)) | 0x01u; // bitwise or to create an odd size

	ocean_assert(size >= 1u);
	ocean_assert(size % 2u == 1u);

	return size;
}

template <typename T>
inline T FrameFilterGaussian::filterSize2sigma(const unsigned int filterSize)
{
	ocean_assert(filterSize >= 1u && (filterSize % 2u) == 1u);

	return T(0.3) * (T(filterSize / 2u) - T(1)) + T(0.8);
}

template <>
inline void FrameFilterGaussian::determineFilterFactorsWithExplicitSigma<unsigned int>(const unsigned int filterSize, const float sigma, unsigned int* filter, unsigned int* denominator)
{
	ocean_assert(filterSize % 2u == 1u);
	ocean_assert(filter != nullptr);

	std::vector<float> floatFilter(filterSize);
	determineFilterFactorsWithExplicitSigma<float>(filterSize, sigma, floatFilter.data());

	const float factor = 1.0f / floatFilter[0];

	unsigned int filterSum = 0u;

	for (unsigned int n = 0u; n < filterSize; ++n)
	{
		filter[n] = (unsigned int)(floatFilter[n] * factor + 0.5f);
		filterSum += filter[n];
	}

	if (denominator)
	{
		*denominator = filterSum;
	}
}

template <typename T>
void FrameFilterGaussian::determineFilterFactorsWithExplicitSigma(const unsigned int filterSize, const float sigma, T* filter, T* denominator)
{
	static_assert(std::is_same<float, T>::value || std::is_same<double, T>::value, "Invalid data type for a filter!");

	ocean_assert(filterSize % 2u == 1u);
	ocean_assert(sigma > NumericF::eps());

	ocean_assert(filter != nullptr);

	// we calculate e ^ -(x^2 / 2 * sigma^2)
	// while x = i - (filterSize / 2)

	const unsigned int filterSize_2 = filterSize / 2u;

	const T scaleFactor = T(-0.5f / (sigma * sigma));

	T filterSum = T(0);

	for (unsigned int n = 0u; n < filterSize; ++n)
	{
		const int i = int(n - filterSize_2);

		filter[n] = NumericT<T>::exp(scaleFactor * T(i) * T(i));

		filterSum += filter[n];
	}

	const T invFilterSum = T(1) / filterSum;

	for (unsigned int n = 0u; n < filterSize; ++n)
	{
		filter[n] *= invFilterSum;
	}

#ifdef OCEAN_DEBUG
	{
		T debugFilterSum = T(0);
		for (unsigned int n = 0u; n < filterSize; ++n)
		{
			debugFilterSum += filter[n];
		}
		ocean_assert(NumericT<T>::isEqual(debugFilterSum, T(1)));
	}
#endif

	if (denominator)
	{
		*denominator = T(1);
	}
}

template <>
inline void FrameFilterGaussian::determineFilterFactors<unsigned int>(const unsigned int filterSize, unsigned int* filter, unsigned int* denominator)
{
	ocean_assert(filterSize % 2u == 1u);
	ocean_assert(filter != nullptr);

	if (filterSize <= 7u)
	{
		static constexpr std::array<unsigned int, 16> predefinedFilters =
		{
			1u,
			1u, 2u, 1u,
			1u, 4u, 6u, 4u, 1u,
			1u, 4u, 7u, 9u, 7u, 4u, 1u
		};

		static constexpr std::array<unsigned int, 4> predefinedDenominators =
		{
			1u,
			4u,
			16u,
			33u
		};

		static constexpr std::array<unsigned int, 4> offsets =
		{
			0u,
			1u,
			4u,
			9u
		};

		ocean_assert(filterSize / 2u < offsets.size());
		const unsigned int filterOffset = offsets[filterSize / 2u];

		for (unsigned int n = 0u; n < filterSize; ++n)
		{
			ocean_assert(filterOffset + n < predefinedFilters.size());
			filter[n] = predefinedFilters[filterOffset + n];
		}


		if (denominator != nullptr)
		{
			ocean_assert(filterSize / 2u < predefinedDenominators.size());
			*denominator = predefinedDenominators[filterSize / 2u];
		}

		return;
	}

	const float sigma = filterSize2sigma<float>(filterSize);

	determineFilterFactorsWithExplicitSigma<unsigned int>(filterSize, sigma, filter, denominator);
}

template <typename T>
void FrameFilterGaussian::determineFilterFactors(const unsigned int filterSize, T* filter, T* denominator)
{
	ocean_assert(filterSize % 2u == 1u);
	ocean_assert(filter != nullptr);

	if (filterSize <= 7u)
	{
		static constexpr std::array<float, 16> predefinedFilters =
		{
			1.0f,
			0.25f, 0.5f, 0.25f,
			0.0625f, 0.25f, 0.375f, 0.25f, 0.0625f,
			0.03125f, 0.109375f, 0.21875f, 0.28125f, 0.21875f, 0.109375f, 0.03125f,

		};

		static constexpr std::array<unsigned int, 4> offsets =
		{
			0u,
			1u,
			4u,
			9u
		};

		ocean_assert(filterSize / 2u < offsets.size());
		const unsigned int filterOffset = offsets[filterSize / 2u];

		for (unsigned int n = 0u; n < filterSize; ++n)
		{
			ocean_assert(filterOffset + n < predefinedFilters.size());
			filter[n] = T(predefinedFilters[filterOffset + n]);
		}

		if (denominator != nullptr)
		{
			*denominator = T(1);
		}

		return;
	}

	const float sigma = filterSize2sigma<float>(filterSize);

	determineFilterFactorsWithExplicitSigma<T>(filterSize, sigma, filter, denominator);
}

template <typename T, typename TFilter>
bool FrameFilterGaussian::filter(const T* source, T* target, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const float sigma, Worker* worker, ReusableMemory* reusableMemory, const ProcessorInstructions processorInstructions)
{
	ocean_assert(source != nullptr && target != nullptr);
	ocean_assert(width >= horizontalFilterSize && height >= verticalFilterSize);

	ocean_assert(horizontalFilterSize >= 1u && horizontalFilterSize % 2u == 1u);
	ocean_assert(verticalFilterSize >= 1u && verticalFilterSize % 2u == 1u);
	if (horizontalFilterSize == 0u || horizontalFilterSize % 2u != 1u || verticalFilterSize == 0u || verticalFilterSize % 2u != 1u)
	{
		return false;
	}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

	// we have a special implementation for small filter kernels

	if (std::is_same<T, uint8_t>::value && std::is_same<TFilter, unsigned int>::value)
	{
		if (width >= 18u && channels == 1u && horizontalFilterSize == 3u && verticalFilterSize == 3u && sigma <= 0.0f)
		{
			filter1Channel8Bit121NEON((const uint8_t*)(source), (uint8_t*)(target), width, height, sourcePaddingElements, targetPaddingElements, reusableMemory);
			return true;
		}
	}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

	FrameFilterSeparable::ReusableMemory* separableReusableMemory = reusableMemory != nullptr ? &reusableMemory->separableReusableMemory_ : nullptr;

	std::vector<TFilter> localHorizontalFilter;
	TFilter* horizontalFilter = nullptr;

	if (reusableMemory != nullptr)
	{
		if (reusableMemory->horizontalFilterMemory_.size() != horizontalFilterSize * sizeof(TFilter))
		{
			reusableMemory->horizontalFilterMemory_ = Memory::create<TFilter>(horizontalFilterSize);
		}

		horizontalFilter = reusableMemory->horizontalFilterMemory_.data<TFilter>();
	}
	else
	{
		localHorizontalFilter.resize(horizontalFilterSize);
		horizontalFilter = localHorizontalFilter.data();
	}

	if (sigma <= 0.0f)
	{
		determineFilterFactors(horizontalFilterSize, horizontalFilter);
	}
	else
	{
		determineFilterFactorsWithExplicitSigma(horizontalFilterSize, sigma, horizontalFilter);
	}

	if (horizontalFilterSize == verticalFilterSize)
	{
		return FrameFilterSeparable::filter<T, TFilter>(source, target, width, height, channels, sourcePaddingElements, targetPaddingElements, horizontalFilter, horizontalFilterSize, horizontalFilter, horizontalFilterSize, worker, separableReusableMemory, processorInstructions);
	}
	else
	{
		std::vector<TFilter> localVerticalFilter;
		TFilter* verticalFilter = nullptr;

		if (reusableMemory != nullptr)
		{
			if (reusableMemory->verticalFilterMemory_.size() != verticalFilterSize * sizeof(TFilter))
			{
				reusableMemory->verticalFilterMemory_ = Memory::create<TFilter>(verticalFilterSize);
			}

			verticalFilter = reusableMemory->verticalFilterMemory_.data<TFilter>();
		}
		else
		{
			localVerticalFilter.resize(verticalFilterSize);
			verticalFilter = localVerticalFilter.data();
		}

		if (sigma <= 0.0f)
		{
			determineFilterFactors(verticalFilterSize, verticalFilter);
		}
		else
		{
			determineFilterFactorsWithExplicitSigma(verticalFilterSize, sigma, verticalFilter);
		}

		return FrameFilterSeparable::filter<T, TFilter>(source, target, width, height, channels, sourcePaddingElements, targetPaddingElements, horizontalFilter, horizontalFilterSize, verticalFilter, verticalFilterSize, worker, separableReusableMemory, processorInstructions);
	}
}

template <typename T, typename TFilter>
inline bool FrameFilterGaussian::filter(T* frame, const unsigned int width, const unsigned int height, const unsigned int channels, const unsigned int framePaddingElements, const unsigned int horizontalFilterSize, const unsigned int verticalFilterSize, const float sigma, Worker* worker, ReusableMemory* reusableMemory, const ProcessorInstructions processorInstructions)
{
	return filter<T, TFilter>(frame, frame, width, height, channels, framePaddingElements, framePaddingElements, horizontalFilterSize, verticalFilterSize, sigma, worker, reusableMemory, processorInstructions);
}

#if defined(OCEAN_HARDWARE_NEON_VERSION) && OCEAN_HARDWARE_NEON_VERSION >= 10

inline void FrameFilterGaussian::filter1Channel8Bit121NEON(const uint8_t* source, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int sourcePaddingElements, const unsigned int targetPaddingElements, ReusableMemory* reusableMemory)
{
	ocean_assert(source != nullptr);
	ocean_assert(target != nullptr);
	ocean_assert(width >= 18u);
	ocean_assert(height >= 1u);

	// [2, 2, 2, 2, 2, 2, 2, 2]
	const uint8x8_t constant_2_u_8x8 = vdup_n_u8(2u);
	const uint16x8_t constant_2_u_16x8 = vdupq_n_u16(2u);

	const unsigned int sourceStrideElements = width * 1u + sourcePaddingElements;
	const unsigned int targetStrideElements = width * 1u + targetPaddingElements;

	const unsigned int innerPixels = width - 2u;

	Memory memoryResponseRows; // memory for three response rows, each row contains 'innerPixels' uint16_t elements
	uint16_t* responseRows = nullptr;

	const unsigned int reusableMemoryNecessaryElements = width * 4u;

	if (reusableMemory != nullptr)
	{
		if (reusableMemory->responseRowsMemory_.size() != reusableMemoryNecessaryElements * sizeof(uint16_t))
		{
			reusableMemory->responseRowsMemory_ = Memory::create<uint16_t>(reusableMemoryNecessaryElements);
		}

		responseRows = reusableMemory->responseRowsMemory_.data<uint16_t>();
	}
	else
	{
		memoryResponseRows = Memory::create<uint16_t>(reusableMemoryNecessaryElements);
		responseRows = memoryResponseRows.data<uint16_t>();
	}

	ocean_assert(responseRows != nullptr);

	uint16_t* responseTopRow = responseRows + width * 0u;

	// first, we determine the horizontal filter response for the 1D filter [1 2 1]

	responseTopRow[0] = source[0] * 3u + source[1]; // special handling for first pixel response

	for (unsigned int n = 0u; n < innerPixels; n += 16u)
	{
		if (n + 16u > innerPixels)
		{
			ocean_assert(n >= 16u && innerPixels > 16u);
			const unsigned int newN = innerPixels - 16u;

			const unsigned int offset = n - newN;
			ocean_assert_and_suppress_unused(offset < innerPixels, offset);

			ocean_assert(n > newN);

			n = newN;

			// the for loop will stop after this iteration
			ocean_assert(n + 16u == innerPixels);
			ocean_assert(!(n + 16u < innerPixels));
		}

		const uint8x16_t source_0_u_8x16 = vld1q_u8(source + n + 0u);
		const uint8x16_t source_1_u_8x16 = vld1q_u8(source + n + 1u);
		const uint8x16_t source_2_u_8x16 = vld1q_u8(source + n + 2u);

		// result = source0 + source2
		uint16x8_t low_u_16x8 = vaddl_u8(vget_low_u8(source_0_u_8x16), vget_low_u8(source_2_u_8x16));
		uint16x8_t high_u_16x8 = vaddl_u8(vget_high_u8(source_0_u_8x16), vget_high_u8(source_2_u_8x16));

		// result += 2 * source1
		low_u_16x8 = vmlal_u8(low_u_16x8, vget_low_u8(source_1_u_8x16), constant_2_u_8x8);
		high_u_16x8 = vmlal_u8(high_u_16x8, vget_high_u8(source_1_u_8x16), constant_2_u_8x8);

		vst1q_u16(responseTopRow + 1u + n + 0u, low_u_16x8);
		vst1q_u16(responseTopRow + 1u + n + 8u, high_u_16x8);
	}

	responseTopRow[width - 1u] = source[width - 2u] + source[width - 1u] * 3u; // special handling for last pixel response

	// due to border mirroring, our top and center row is identical for the first iteration
	uint16_t* responseCenterRow = responseTopRow;
	uint16_t* responseBottomRow = responseRows + width * 2u;
	uint8_t* const sourceExtraCopy = (uint8_t*)(responseRows + width * 3u);

	source += sourceStrideElements;

	for (unsigned int y = 0u; y < height; ++y)
	{
		if (y == height - 2u)
		{
			// we need to make a copy of the last source row for in-place filtering
			memcpy(sourceExtraCopy, source, width * sizeof(uint8_t));
		}

		// for each iteration, we have a pre-calculated (horizontal) response for the top and center row already

		responseBottomRow[0u] = source[0] * 3u + source[1];

		// handle left pixel:                       (outside) (inside)
		// |  3  1                                         1 |  2  1
		// | [6] 2                                         2 | [4] 2
		// |  3  1     the filter factors are based on:    1 |  2  1

		// using scoped value for intermediate storage as source and target can be identical e.g., for in-place filtering
		const ScopedValueT<uint8_t> firstPixelValue(*target, uint8_t((responseTopRow[0] + responseCenterRow[0] * 2u + responseBottomRow[0] + 8u) / 16u));

		for (unsigned int n = 0u; n < innerPixels; n += 16u)
		{
			if (n + 16u > innerPixels)
			{
				ocean_assert(n >= 16u && innerPixels > 16u);
				const unsigned int newN = innerPixels - 16u;

				const unsigned int offset = n - newN;
				ocean_assert_and_suppress_unused(offset < innerPixels, offset);

				ocean_assert(n > newN);

				n = newN;

				// the for loop will stop after this iteration
				ocean_assert(n + 16u == innerPixels);
				ocean_assert(!(n + 16u < innerPixels));
			}

			const uint8x16_t sourceBottom_0_u_8x16 = vld1q_u8(source + n + 0u);
			const uint8x16_t sourceBottom_1_u_8x16 = vld1q_u8(source + n + 1u);
			const uint8x16_t sourceBottom_2_u_8x16 = vld1q_u8(source + n + 2u);

			// bottomResult = bottomSource0 + bottomSource2
			uint16x8_t bottomLow_u_16x8 = vaddl_u8(vget_low_u8(sourceBottom_0_u_8x16), vget_low_u8(sourceBottom_2_u_8x16));
			uint16x8_t bottomHigh_u_16x8 = vaddl_u8(vget_high_u8(sourceBottom_0_u_8x16), vget_high_u8(sourceBottom_2_u_8x16));

			// bottomResult += 2 * bottomSource1
			bottomLow_u_16x8 = vmlal_u8(bottomLow_u_16x8, vget_low_u8(sourceBottom_1_u_8x16), constant_2_u_8x8);
			bottomHigh_u_16x8 = vmlal_u8(bottomHigh_u_16x8, vget_high_u8(sourceBottom_1_u_8x16), constant_2_u_8x8);


			// load the pre-calculated values for top
			const uint16x8_t topLow_u_16x8 = vld1q_u16(responseTopRow + 1u + n + 0u);
			const uint16x8_t topHigh_u_16x8 = vld1q_u16(responseTopRow + 1u + n + 8u);

			// load the pre-calculated values for bottom
			const uint16x8_t centerLow_u_16x8 = vld1q_u16(responseCenterRow + 1u + n + 0u);
			const uint16x8_t centerHigh_u_16x8 = vld1q_u16(responseCenterRow + 1u + n + 8u);

			// result = top + bottom
			uint16x8_t resultLow_u_16x8 = vaddq_u16(topLow_u_16x8, bottomLow_u_16x8);
			uint16x8_t resultHigh_u_16x8 = vaddq_u16(topHigh_u_16x8, bottomHigh_u_16x8);

			// result += 2 * center
			resultLow_u_16x8 = vmlaq_u16(resultLow_u_16x8, centerLow_u_16x8, constant_2_u_16x8);
			resultHigh_u_16x8 = vmlaq_u16(resultHigh_u_16x8, centerHigh_u_16x8, constant_2_u_16x8);

			// write the results for the bottom row so that we can use them as new pre-calculated values in the next iteration
			// as we may re-calculate the last 16 pixels once again in the very last iteration, we cannot simply write the results to the center row
			vst1q_u16(responseBottomRow + 1u + n + 0u, bottomLow_u_16x8);
			vst1q_u16(responseBottomRow + 1u + n + 8u, bottomHigh_u_16x8);

			// result = (result + 8) / 16
			const uint8x16_t result_u_8x16 = vcombine_u8(vrshrn_n_u16(resultLow_u_16x8, 4), vrshrn_n_u16(resultHigh_u_16x8, 4));

			vst1q_u8(target + 1u + n, result_u_8x16);
		}

		responseBottomRow[width - 1u] = source[width - 2u] + source[width - 1u] * 3u;

		// handle right pixel:                      (inside) (outside)
		// 1  3  |                                    1  2  | 1
		// 2 [6] |                                    2 [4] | 2
		// 1  3  |                                    1  2  | 1

		target[width - 1u] = uint8_t((responseTopRow[width - 1u] + responseCenterRow[width - 1u] * 2u + responseBottomRow[width - 1u] + 8u) / 16u);

		source += sourceStrideElements;
		target += targetStrideElements;

		std::swap(responseTopRow, responseCenterRow);

		if (y == 0u)
		{
			// the next row will not have any border mirroring anymore

			responseCenterRow = responseRows + width * 1u;
		}
		else if (y == height - 2u)
		{
			// the next iteration will handle the last row in the frame
			// the bottom row will be mirrored which is actually the last row again

			source = sourceExtraCopy;
		}

		std::swap(responseCenterRow, responseBottomRow);
	}
}

#endif // OCEAN_HARDWARE_NEON_VERSION >= 10

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_GAUSSIAN_H
