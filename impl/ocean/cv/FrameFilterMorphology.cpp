/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/FrameFilterMorphology.h"
#include "ocean/cv/FrameFilterErosion.h"
#include "ocean/cv/FrameFilterDilation.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace CV
{

template <FrameFilterMorphology::MorphologyFilter tFilter>
void FrameFilterMorphology::openMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(mask != nullptr && width >= 4u && height >= 4u);

	Frame intermediateFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	if constexpr (tFilter == MF_SQUARE_5)
	{
		CV::FrameFilterErosion::filter1Channel8Bit24Neighbor(mask, intermediateFrame.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateFrame.paddingElements(), worker);
		CV::FrameFilterDilation::filter1Channel8Bit24Neighbor(intermediateFrame.constdata<uint8_t>(), mask, width, height, maskValue, intermediateFrame.paddingElements(), maskPaddingElements, worker);
	}
	else
	{
		ocean_assert(tFilter == MF_SQUARE_3);

		CV::FrameFilterErosion::filter1Channel8Bit8Neighbor(mask, intermediateFrame.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateFrame.paddingElements(), worker);
		CV::FrameFilterDilation::filter1Channel8Bit8Neighbor(intermediateFrame.constdata<uint8_t>(), mask, width, height, maskValue, intermediateFrame.paddingElements(), maskPaddingElements, worker);
	}
}

// We force the compilation of the following template-based functions to ensure that they exist when needed/linked
template OCEAN_CV_EXPORT void FrameFilterMorphology::openMask<FrameFilterMorphology::MF_SQUARE_3>(uint8_t*, const unsigned int, const unsigned int, const unsigned int maskPaddingElements, const uint8_t maskValue, Worker* worker);
template OCEAN_CV_EXPORT void FrameFilterMorphology::openMask<FrameFilterMorphology::MF_SQUARE_5>(uint8_t*, const unsigned int, const unsigned int, const unsigned int maskPaddingElements, const uint8_t maskValue, Worker* worker);

template <FrameFilterMorphology::MorphologyFilter tFilter>
void FrameFilterMorphology::closeMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(mask != nullptr && width >= 4u && height >= 4u);

	Frame intermediateFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));

	if constexpr (tFilter == MF_SQUARE_5)
	{
		CV::FrameFilterDilation::filter1Channel8Bit24Neighbor(mask, intermediateFrame.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateFrame.paddingElements(), worker);
		CV::FrameFilterErosion::filter1Channel8Bit24Neighbor(intermediateFrame.constdata<uint8_t>(), mask, width, height, maskValue, intermediateFrame.paddingElements(), maskPaddingElements, worker);
	}
	else
	{
		ocean_assert(tFilter == MF_SQUARE_3);

		CV::FrameFilterDilation::filter1Channel8Bit8Neighbor(mask, intermediateFrame.data<uint8_t>(), width, height, maskValue, maskPaddingElements, intermediateFrame.paddingElements(), worker);
		CV::FrameFilterErosion::filter1Channel8Bit8Neighbor(intermediateFrame.constdata<uint8_t>(), mask, width, height, maskValue, intermediateFrame.paddingElements(), maskPaddingElements, worker);
	}
}

// We force the compilation of the following template-based functions to ensure that they exist when needed/linked
template OCEAN_CV_EXPORT void FrameFilterMorphology::closeMask<FrameFilterMorphology::MF_SQUARE_3>(uint8_t*, const unsigned int, const unsigned int, const unsigned int maskPaddingElements, const uint8_t maskValue, Worker* worker);
template OCEAN_CV_EXPORT void FrameFilterMorphology::closeMask<FrameFilterMorphology::MF_SQUARE_5>(uint8_t*, const unsigned int, const unsigned int, const unsigned int maskPaddingElements, const uint8_t maskValue, Worker* worker);

}

}
