// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/synthesis/InitializerShrinkingErosionRandomizedI1.h"

#include "ocean/cv/FrameFilterErosion.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

bool InitializerShrinkingErosionRandomizedI1::invoke(Worker* worker) const
{
	Frame copyMask(layerI_.mask(), Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	constexpr unsigned int randomNoise = 3u;
	FrameFilterErosion::Comfort::shrinkMaskRandom(layerI_.frame(), copyMask, CV::FrameFilterErosion::MF_SQUARE_3, randomNoise, RandomI::random32(randomGenerator_));

	return appearanceInitializer_.invoke(worker);
}

}

}

}
