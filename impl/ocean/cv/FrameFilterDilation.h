/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FRAME_FILTER_DILATION_H
#define META_OCEAN_CV_FRAME_FILTER_DILATION_H

#include "ocean/cv/CV.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameFilterMorphology.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements a frame dilation filter.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterDilation : public FrameFilterMorphology
{
	public:

		/**
		 * Applies several dilation filter iterations for an 8 bit mask image.
		 * The value of a mask pixel (to be dilated) can be defined, every other pixel value is interpreted as a non-mask pixels.
		 * @param mask The mask frame to be filtered, must be valid
		 * @param width The width of the mask frame in pixel, with range [4, infinity)
		 * @param height The height of the mask frame in pixel, with range [4, infinity)
		 * @param iterations Number of iterations to be applied, best performance when number of iterations is even, with range [1, infinity)
		 * @param maskValue The value of a mask pixel to be dilated, pixels with other values will be untouched, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tDilationFilter The type of the dilation filter to be applied
		 * @see filter1Channel8Bit4Neighbor(), filter1Channel8Bit8Neighbor().
		 */
		template <MorphologyFilter tDilationFilter>
		static void filter1Channel8Bit(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int iterations, const uint8_t maskValue = 0x00, const unsigned int maskPaddingElements = 0u, Worker* worker = nullptr);

		/**
		 * Applies one dilation filter iteration for an 8 bit mask image using a 4-neighborhood.
		 * The value of a mask pixel (to be dilated) can be defined, every other pixel value is interpreted as a non-mask pixels.
		 * @param mask The mask frame to be filtered, must be valid
		 * @param target The target frame receiving the filter response, must be valid
		 * @param width The width of the mask frame in pixel, with range [2, infinity)
		 * @param height The height of the mask frame in pixel, with range [2, infinity)
		 * @param maskValue The value of a mask pixel to be dilated, pixels with other values will be untouched, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void filter1Channel8Bit4Neighbor(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue = 0x00, const unsigned int maskPaddingElements = 0u, const unsigned int targetPaddingElements = 0u, Worker* worker = nullptr);

		/**
		 * Applies one dilation filter iteration for an 8 bit mask image using a 8-neighborhood, a 3x3 square kernel.
		 * The value of a mask pixel (to be dilated) can be defined, every other pixel value is interpreted as a non-mask pixels.
		 * @param mask The mask frame to be filtered, must be valid
		 * @param target The target frame receiving the filter response, must be valid
		 * @param width The width of the mask frame in pixel, with range [2, infinity)
		 * @param height The height of the mask frame in pixel, with range [2, infinity)
		 * @param maskValue The value of a mask pixel to be dilated, pixels with other values will be untouched, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void filter1Channel8Bit8Neighbor(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue = 0x00, const unsigned int maskPaddingElements = 0u, const unsigned int targetPaddingElements = 0u, Worker* worker = nullptr);

		/**
		* Applies one dilation filter iteration for an 8 bit mask image using a 24-neighborhood, a 5x5 square kernel.
		* The value of a mask pixel (to be dilated) can be defined, every other pixel value is interpreted as a non-mask pixels.
		* @param mask The mask frame to be filtered, must be valid
		* @param target The target frame receiving the filter response, must be valid
		* @param width The width of the mask frame in pixel, with range [4, infinity)
		* @param height The height of the mask frame in pixel, with range [4, infinity)
		* @param maskValue The value of a mask pixel to be dilated, pixels with other values will be untouched, with range [0, 255]
		* @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		* @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		* @param worker Optional worker object to distribute the computation
		*/
		static inline void filter1Channel8Bit24Neighbor(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue = 0x00, const unsigned int maskPaddingElements = 0u, const unsigned int targetPaddingElements = 0u, Worker* worker = nullptr);

	private:

		/**
		 * Applies one dilation filter iteration in a subset of an 8 bit mask image using a 4-neighborhood.
		 * @param mask The mask frame to be filtered, must be valid
		 * @param target The target frame receiving the filter response, must be valid
		 * @param width The width of the mask frame in pixel, with range [2, infinity)
		 * @param height The height of the mask frame in pixel, with range [2, infinity)
		 * @param maskValue The value of a mask pixel to be eroded, pixels with other values will be untouched, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 */
		static void filter1Channel8Bit4NeighborSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Applies one dilation filter iteration in a subset of an 8 bit mask image using a 8-neighborhood.
		 * @param mask The mask frame to be filtered, must be valid
		 * @param target The target frame receiving the filter response, must be valid
		 * @param width The width of the mask frame in pixel, with range [2, infinity)
		 * @param height The height of the mask frame in pixel, with range [2, infinity)
		 * @param maskValue The value of a mask pixel to be eroded, pixels with other values will be untouched, with range [0, 255]
		 * @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		 */
		static void filter1Channel8Bit8NeighborSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		* Applies one dilation filter iteration in a subset of an 8 bit mask image using a 24-neighborhood.
		* @param mask The mask frame to be filtered, must be valid
		* @param target The target frame receiving the filter response, must be valid
		* @param width The width of the mask frame in pixel, with range [4, infinity)
		* @param height The height of the mask frame in pixel, with range [4, infinity)
		* @param maskValue The value of a mask pixel to be eroded, pixels with other values will be untouched, with range [0, 255]
		* @param maskPaddingElements Optional number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		* @param targetPaddingElements Optional number of padding elements at the end of each target row, in elements, with range [0, infinity)
		* @param firstRow First row to be handled, with range [0, height - 1]
		* @param numberRows Number of rows to be handled, with range [1, height - firstRow]
		*/
		static void filter1Channel8Bit24NeighborSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const unsigned int firstRow, const unsigned int numberRows);

		/**
		* Returns whether each several pixels in a row is not equal to a specified value.
		* @param maskPixels The first pixel within the row to check, must be valid
		* @param maskValue The mask value to check, with range [0, 255]
		* @return True, if so
		* @tparam tSize The number of pixels in a row to check, with range [1, infinity)
		*/
		template <unsigned int tSize>
		static OCEAN_FORCE_INLINE bool eachPixelNotEqual(const uint8_t* const maskPixels, const uint8_t maskValue);
};

template <FrameFilterDilation::MorphologyFilter tDilationFilter>
void FrameFilterDilation::filter1Channel8Bit(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int iterations, const uint8_t maskValue, const unsigned int maskPaddingElements, Worker* worker)
{
	ocean_assert(mask != nullptr);
	ocean_assert(width >= 2u && height >= 2u);
	ocean_assert(iterations >= 1u);

	Frame intermediateTarget(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	switch (tDilationFilter)
	{
		case MF_CROSS_3:
		{
			for (unsigned int n = 0u; n < iterations / 2u; ++n)
			{
				filter1Channel8Bit4Neighbor(mask, intermediateTarget.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateTarget.paddingElements(), worker);
				filter1Channel8Bit4Neighbor(intermediateTarget.constdata<uint8_t>(), mask, width, height, maskValue, intermediateTarget.paddingElements(), maskPaddingElements, worker);
			}

			if (iterations % 2u == 1u)
			{
				filter1Channel8Bit4Neighbor(mask, intermediateTarget.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateTarget.paddingElements(), worker);
			}

			break;
		}

		case MF_SQUARE_3:
		{
			for (unsigned int n = 0u; n < iterations / 2u; ++n)
			{
				filter1Channel8Bit8Neighbor(mask, intermediateTarget.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateTarget.paddingElements(), worker);
				filter1Channel8Bit8Neighbor(intermediateTarget.constdata<uint8_t>(), mask, width, height, maskValue, intermediateTarget.paddingElements(), maskPaddingElements, worker);
			}

			if (iterations % 2u == 1u)
			{
				filter1Channel8Bit8Neighbor(mask, intermediateTarget.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateTarget.paddingElements(), worker);
			}

			break;
		}

		case MF_SQUARE_5:
		{
			for (unsigned int n = 0u; n < iterations / 2u; ++n)
			{
				filter1Channel8Bit24Neighbor(mask, intermediateTarget.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateTarget.paddingElements(), worker);
				filter1Channel8Bit24Neighbor(intermediateTarget.constdata<uint8_t>(), mask, width, height, maskValue, intermediateTarget.paddingElements(), maskPaddingElements, worker);
			}

			if (iterations % 2u == 1u)
			{
				filter1Channel8Bit24Neighbor(mask, intermediateTarget.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateTarget.paddingElements(), worker);
			}

			break;
		}

		default:
			ocean_assert(false && "Invalid dilation filter!");
	}

	if (iterations % 2u == 1u)
	{
		CV::FrameConverter::subFrame<uint8_t>(intermediateTarget.constdata<uint8_t>(), mask, width, height, width, height, 1u, 0u, 0u, 0u, 0u, width, height, intermediateTarget.paddingElements(), maskPaddingElements);
	}
}

inline void FrameFilterDilation::filter1Channel8Bit4Neighbor(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(mask && target);
	ocean_assert(width >= 2u && height >= 2u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameFilterDilation::filter1Channel8Bit4NeighborSubset, mask, target, width, height, maskValue, maskPaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 7u, 8u, 20u);
	}
	else
	{
		filter1Channel8Bit4NeighborSubset(mask, target, width, height, maskValue, maskPaddingElements, targetPaddingElements, 0u, height);
	}
}

inline void FrameFilterDilation::filter1Channel8Bit8Neighbor(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(mask && target);
	ocean_assert(width >= 2u && height >= 2u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameFilterDilation::filter1Channel8Bit8NeighborSubset, mask, target, width, height, maskValue, maskPaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 7u, 8u, 20u);
	}
	else
	{
		filter1Channel8Bit8NeighborSubset(mask, target, width, height, maskValue, maskPaddingElements, targetPaddingElements, 0u, height);
	}
}

inline void FrameFilterDilation::filter1Channel8Bit24Neighbor(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const uint8_t maskValue, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, Worker* worker)
{
	ocean_assert(mask && target);
	ocean_assert(width >= 4u && height >= 4u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&FrameFilterDilation::filter1Channel8Bit24NeighborSubset, mask, target, width, height, maskValue, maskPaddingElements, targetPaddingElements, 0u, 0u), 0u, height, 7u, 8u, 20u);
	}
	else
	{
		filter1Channel8Bit24NeighborSubset(mask, target, width, height, maskValue, maskPaddingElements, targetPaddingElements, 0u, height);
	}
}

template <>
OCEAN_FORCE_INLINE bool FrameFilterDilation::eachPixelNotEqual<3u>(const uint8_t* const maskPixels, const uint8_t maskValue)
{
	ocean_assert(maskPixels != nullptr);

	return maskPixels[0] != maskValue && maskPixels[1] != maskValue && maskPixels[2] != maskValue;
}

template <>
OCEAN_FORCE_INLINE bool FrameFilterDilation::eachPixelNotEqual<4u>(const uint8_t* const maskPixels, const uint8_t maskValue)
{
	ocean_assert(maskPixels != nullptr);

	return maskPixels[0] != maskValue && maskPixels[1] != maskValue && maskPixels[2] != maskValue && maskPixels[3] != maskValue;
}

template <>
OCEAN_FORCE_INLINE bool FrameFilterDilation::eachPixelNotEqual<5u>(const uint8_t* const maskPixels, const uint8_t maskValue)
{
	ocean_assert(maskPixels != nullptr);

	return maskPixels[0] != maskValue && maskPixels[1] != maskValue && maskPixels[2] != maskValue && maskPixels[3] != maskValue && maskPixels[4] != maskValue;
}

}

}

#endif // META_OCEAN_CV_FRAME_FILTER_DILATION_H
