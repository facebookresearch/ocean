/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_OPERATIONS_H
#define META_OCEAN_CV_FRAME_OPERATIONS_H

#include "ocean/base/Worker.h"
#include "ocean/base/Frame.h"

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameChannels.h"

namespace Ocean
{

namespace CV
{

/**
 * Class for commonly used (arithmetic) operators on and between frames.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameOperations
{
	public:

		/**
		 * Computes the difference between two frames and stores the difference
		 *
		 * For the pixel-wise subtraction of
		 *
		 *   A and B, C_i = A_i - B_i,
		 *
		 * where i is the index of a pixel.
		 *
		 * @note Refrain from using this function, if binary size is important, e.g. for development on mobile platform. Instead use the pointer-based function @c subtract()
		 *
		 * @param source0 First source frame, must be valid
		 * @param source1 Second source frame, must be valid and have the same type as @c source0
		 * @param target Resulting frame, must be valid and have the same type as @c source0
		 * @param worker Optional worker instance for parallelization
		 * @return True on success, otherwise false (e.g., for unsupported pixel formats, or mismatch of pixel formats)
		 */
		static bool subtract(const Frame& source0, const Frame& source1, Frame& target, Worker* worker = nullptr);

		/**
		 * Computes the difference between two frames
		 *  The pixel-wise subtraction of two frames is defined as `C(y,x) = A(y,x) - B(y,x)`
		 * @param source0 Pointer to the first source, must not be null
		 * @param source1 Pointer to the second source, must not be null
		 * @param target Pointer to the result location, must not be null
		 * @param width The width of the sources and the target (must all be equal)
		 * @param height The height of the sources and the target (must all be equal)
		 * @param source0PaddingElements The number of padding elements at the end of each row of the first source, in elements, with range [0, infinity)
		 * @param source1PaddingElements The number of padding elements at the end of each row of the second source, in elements, with range [0, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param conversionFlag Determines the type of conversion
		 * @param worker Optional worker instance for parallelization
		 * @tparam TSource0 Data type of the first source
		 * @tparam TSource1 Data type of the second source
		 * @tparam TTarget Data type of the output
		 * @tparam TIntermediate Data type for the computation of intermediate results (before the conversion to @c TTarget)
		 * @tparam tChannels Number of channels of the sources and the target
		 */
		template <typename TSource0, typename TSource1, typename TTarget, typename TIntermediate, unsigned int tChannels>
		static void subtract(const TSource0* source0, const TSource1* source1, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int source0PaddingElements, const unsigned int source1PaddingElements, const unsigned int targetPaddingElements, const FrameChannels::ConversionFlag conversionFlag = FrameChannels::CONVERT_NORMAL, Worker* worker = nullptr);

	protected:

		/**
		 * Collections of pixel operations
		 * @tparam tChannels Number of channels of the data to process
		 */
		template <unsigned int tChannels>
		struct Operations
		{
			/**
			 * Subtraction of frames
			 *
			 * For the pixel-wise subtraction of two frames, C_i = A_i - B_i, where
			 * i is the index of a pixel.
			 *
			 * @param source0 Pointer to the first source, must not be null
			 * @param source1 Pointer to the second source, must not be null
			 * @param target Pointer to the result location, must not be null
			 * @tparam TSource0 Type of the first source
			 * @tparam TSource1 Type of the second source
			 * @tparam TTarget Type of the target
			 */
			template <typename TSource0, typename TSource1, typename TTarget>
			static inline void subtractPixel(const TSource0* source0, const TSource1* source1, TTarget* target);
		};
};

template <typename TSource0, typename TSource1, typename TTarget, typename TIntermediate, unsigned int tChannels>
void FrameOperations::subtract(const TSource0* source0, const TSource1* source1, TTarget* target, const unsigned int width, const unsigned int height, const unsigned int source0PaddingElements, const unsigned int source1PaddingElements, const unsigned int targetPaddingElements, const FrameChannels::ConversionFlag conversionFlag, Worker* worker)
{
	static_assert(tChannels > 0u, "Number of channels must be larger then zero.");

	ocean_assert(source0 && source1 && target);
	ocean_assert(width > 0u && height > 0u);

	FrameChannels::applyBivariateOperator<TSource0, TSource1, TTarget, TIntermediate, tChannels, tChannels, Operations<tChannels>::template subtractPixel<TSource0, TSource1, TTarget>>(source0, source1, target, width, height, source0PaddingElements, source1PaddingElements, targetPaddingElements, conversionFlag, worker);
}

template <unsigned int tChannels>
template <typename TSource0, typename TSource1, typename TTarget>
inline void FrameOperations::Operations<tChannels>::subtractPixel(const TSource0* source0, const TSource1* source1, TTarget* target)
{
	static_assert(tChannels > 0u, "Number of channels must be larger then zero.");

	ocean_assert(source0 && source1 && target);

	for (unsigned int channel = 0u; channel < tChannels; ++channel)
	{
		target[channel] = static_cast<TTarget>(source0[channel] - source1[channel]);
	}
}

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_FRAME_OPERATIONS_H
