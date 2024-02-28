// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_CV_FRAME_FILTER_GRADIENT_SSE_H
#define META_OCEAN_CV_FRAME_FILTER_GRADIENT_SSE_H

#include "ocean/cv/CV.h"

#include "ocean/base/Worker.h"

#if defined(OCEAN_HARDWARE_SSE_VERSION) && OCEAN_HARDWARE_SSE_VERSION >= 41

namespace Ocean
{

namespace CV
{

/**
 * This class implements a gradient frame filter using SSE cpu instructions.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT FrameFilterGradientSSE
{
	public:

		/**
		 * Filter function determining the squared horizontal, vertical and the product between horizontal and vertical gradient filter responses for 1 channel 8 bit frames.
		 * @see FrameFilterGradient::filterHorizontalVertical3Squared1Channel8Bit().
		 */
		static inline void filterHorizontalVertical3Products1Channel8Bit(const uint8_t* source, int16_t* target, const unsigned int width, const unsigned int height, Worker* worker = nullptr);

	protected:

		/**
		 * Filter function determining the squared horizontal, vertical and the product between horizontal and vertical gradient filter responses for a subset of a 1 channel 8 bit frames.
		 * @see FrameFilterGradient::filterHorizontalVertical3Products1Channel8Bit().
		 */
		static void filterHorizontalVertical3Products1Channel8BitSubset(const uint8_t* source, int16_t* target, const unsigned int width, const unsigned int height, const unsigned int firstRow, const unsigned int numberRows);
};

inline void FrameFilterGradientSSE::filterHorizontalVertical3Products1Channel8Bit(const uint8_t* source, int16_t* target, const unsigned int width, const unsigned int height, Worker* worker)
{
	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(FrameFilterGradientSSE::filterHorizontalVertical3Products1Channel8BitSubset, source, target, width, height, 0u, 0u), 0u, height, 4u, 5u, 20u);
	}
	else
	{
		filterHorizontalVertical3Products1Channel8BitSubset(source, target, width, height, 0u, height);
	}
}

} // namespace CV

} // namespace Ocean

#endif // OCEAN_HARDWARE_SSE_VERSION >= 41

#endif // META_OCEAN_CV_FRAME_FILTER_GRADIENT_SSE_H
