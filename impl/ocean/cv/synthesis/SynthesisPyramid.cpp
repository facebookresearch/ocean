// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/synthesis/SynthesisPyramid.h"

#include "ocean/cv/FrameShrinker.h"
#include "ocean/cv/MaskAnalyzer.h"

#include "ocean/cv/advanced/AdvancedFrameFilterGaussian.h"
#include "ocean/cv/advanced/AdvancedFrameShrinker.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

SynthesisPyramid::SynthesisPyramid()
{
#ifdef OCEAN_DEBUG
	synthesisHasBeenArranged_ = false;
#endif
}

bool SynthesisPyramid::arrange(const Frame& frame, const Frame& mask, Worker* worker, const bool binomialFilterOnCoarsestLayers, const bool binomialFilterOnFineLayers, const Frame& filter)
{
	ocean_assert(frame && mask);
	ocean_assert(FrameType(frame, mask.pixelFormat()) == mask.frameType());
	ocean_assert(FrameType::formatIsGeneric(mask.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u));

	ocean_assert(CV::MaskAnalyzer::hasValue(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0x00, 0u) && "The mask does not contain any mask pixel, so here is nothing to inpaint!");

	if (!frame || !mask || FrameType(frame, mask.pixelFormat()) != mask.frameType() || !FrameType::formatIsGeneric(mask.pixelFormat(), FrameType::DT_UNSIGNED_INTEGER_8, 1u))
	{
		return false;
	}

	ocean_assert(filter.isNull() || filter.frameType() == mask.frameType());
	ocean_assert((filter.isNull() || CV::MaskAnalyzer::hasValue(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFF, 0u)) && "The filter does not contain any valid pixel, so we will not have any source pixel!");

	if (filter && filter.frameType() != mask.frameType())
	{
		return false;
	}

	// we reserve memory for a (full) frame pyramid
	synthesisFramePyramid_.resize(frame.frameType());
	synthesisMaskPyramid_.resize(mask.frameType());

	// we copy the finest pyramid layer information
	Frame(synthesisFramePyramid_[0], Frame::temporary_ACM_USE_KEEP_LAYOUT).copy(0, 0, frame);
	Frame(synthesisMaskPyramid_[0], Frame::temporary_ACM_USE_KEEP_LAYOUT).copy(0, 0, mask);

	if (filter)
	{
		Frame(synthesisFilterPyramid_[0], Frame::temporary_ACM_USE_KEEP_LAYOUT).copy(0, 0, filter);
	}

	// now we down-sample the frame data as long as no mask pixel exists anymore

	for (unsigned int n = 0u; n < synthesisFramePyramid_.layers() - 1u; ++n)
	{
		bool hasMaskPixel = false;

		Frame nextPyramidLayerFrame(synthesisFramePyramid_[n + 1u], Frame::temporary_ACM_USE_KEEP_LAYOUT);
		Frame nextPyramidLayerMask(synthesisMaskPyramid_[n + 1u], Frame::temporary_ACM_USE_KEEP_LAYOUT);

		if (!Advanced::AdvancedFrameShrinker::divideByTwo(Frame(synthesisFramePyramid_[n], Frame::temporary_ACM_USE_KEEP_LAYOUT), nextPyramidLayerFrame, Frame(synthesisMaskPyramid_[n], Frame::temporary_ACM_USE_KEEP_LAYOUT), nextPyramidLayerMask, false, &hasMaskPixel, worker))
		{
			return false;
		}

		if (filter)
		{
			constexpr unsigned int layerPaddingElements = 0u;

			CV::FrameShrinker::downsampleByTwoBinary1Channel8Bit(synthesisFilterPyramid_[n].constdata<uint8_t>(), synthesisFilterPyramid_[n + 1u].data<uint8_t>(), synthesisFilterPyramid_[n].width(), synthesisFilterPyramid_[n].height(), layerPaddingElements, layerPaddingElements, 766u, worker);
		}

		if (!hasMaskPixel)
		{
			// if the current (downsampled) pyramid layer does not contain any mask pixel we simply can stop here

			if (n == 0u)
			{
				return false;
			}

			synthesisFramePyramid_.setValidLayers(n - 1u);
			synthesisMaskPyramid_.setValidLayers(n - 1u);

			if (filter)
			{
				synthesisFilterPyramid_.setValidLayers(n - 1u);
			}

			break;
		}
	}

	ocean_assert(synthesisFramePyramid_.validLayers() < synthesisFramePyramid_.layers());
	ocean_assert(synthesisFramePyramid_.validLayers() >= 1u);
	ocean_assert(synthesisFramePyramid_.validLayers() == synthesisMaskPyramid_.validLayers());
	ocean_assert(filter.isNull() || synthesisFramePyramid_.validLayers() == synthesisFilterPyramid_.validLayers());

	// determine the mask bounding boxes in the individual layers
	determineBoundingBoxes(worker);

	// apply smoothing on layer 0 to n-2
	if (binomialFilterOnFineLayers)
	{
		for (unsigned int n = 0u; n < synthesisFramePyramid_.validLayers() - 1u; ++n)
		{
			Frame frameLayer(synthesisFramePyramid_[n], Frame::temporary_ACM_USE_KEEP_LAYOUT);
			Frame maskLayer(synthesisMaskPyramid_[n], Frame::ACM_COPY_REMOVE_PADDING_LAYOUT); // we copy the mask layer, otherwise it would be changed during filtering

			Advanced::AdvancedFrameFilterGaussian::Comfort::filter(frameLayer, maskLayer, 3u, 0x00u, worker);
		}
	}

	// apply smoothing on coarsest layer (n-1)
	if (binomialFilterOnCoarsestLayers)
	{
		Frame frameLayer(synthesisFramePyramid_.coarsestLayer(), Frame::temporary_ACM_USE_KEEP_LAYOUT);
		Frame maskLayer(synthesisMaskPyramid_.coarsestLayer(), Frame::ACM_COPY_REMOVE_PADDING_LAYOUT); // we copy the mask layer, otherwise it would be changed during filtering

		Advanced::AdvancedFrameFilterGaussian::Comfort::filter(frameLayer, maskLayer, 3u, 0x00u, worker);
	}

	// now we prepare the masks of the individual pyramid layers for the next coming steps
	// we need to know how far a pixel (inside the mask) is located from the border of the mask, we determine pixels with distances 0, 1, 2, 3, larger distances are not important as we currently use patch sizes with dimension 5x5

	for (unsigned int n = 0u; n < synthesisFramePyramid_.validLayers(); ++n)
	{
		Frame layerMask(synthesisMaskPyramid_[n], Frame::temporary_ACM_USE_KEEP_LAYOUT);
		Segmentation::MaskAnalyzer::determineDistancesToBorder8Bit(layerMask.data<uint8_t>(), layerMask.width(), layerMask.height(), layerMask.paddingElements(), 3u, false, synthesisBoundingBoxes_[n], worker);
	}

#ifdef OCEAN_DEBUG
	synthesisHasBeenArranged_ = true;
#endif

	return true;
}

bool SynthesisPyramid::applyInpainting(const SynthesisQuality synthesisQuality, RandomGenerator& randomGenerator, const unsigned int weightFactor, const unsigned int borderFactor, const unsigned int maxSpatialCost, Worker* worker)
{
	switch (synthesisQuality)
	{
		case SQ_LOW:
			return applyInpainting(IT_RANDOM_EROSION, randomGenerator, weightFactor, borderFactor, maxSpatialCost, 1u, synthesisFramePyramid_.validLayers() - 2u, 0xFFFFFFFF, worker);

		case SQ_MODERATE:
			return applyInpainting(IT_PATCH_FULL_AREA_HEURISTIC_1, randomGenerator, weightFactor, borderFactor, maxSpatialCost, 1u, 2u, 1u, worker);

		case SQ_HIGH:
			return applyInpainting(IT_PATCH_FULL_AREA_2, randomGenerator, weightFactor, borderFactor, maxSpatialCost, 2u, 1u, 0u, worker);

		case SQ_VERY_HIGH:
			return applyInpainting(IT_PATCH_FULL_AREA_2, randomGenerator, weightFactor, borderFactor, maxSpatialCost, 2u, 0u, 0u, worker);
	}

	return false;
}

void SynthesisPyramid::determineBoundingBoxes(Worker* worker)
{
	// determine the mask bounding box
	synthesisBoundingBoxes_.resize(synthesisMaskPyramid_.validLayers());

	for (unsigned int n = synthesisMaskPyramid_.validLayers() - 1u; n != (unsigned int)(-1); --n)
	{
		const Frame mask(synthesisMaskPyramid_[n], Frame::temporary_ACM_USE_KEEP_LAYOUT);

		if (n == synthesisMaskPyramid_.validLayers() - 1u)
		{
			synthesisBoundingBoxes_[n] = MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
		}
		else
		{
			const PixelBoundingBox& previous = synthesisBoundingBoxes_[n + 1];
			const PixelBoundingBox rough(previous.left() * 2u, previous.top() * 2u, previous.right() * 2u, previous.bottom() * 2u);

			synthesisBoundingBoxes_[n] = MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), rough, 4u, 0xFFu, mask.paddingElements());
			ocean_assert(synthesisBoundingBoxes_[n] == MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements()));
		}
	}

	ocean_assert_and_suppress_unused(synthesisBoundingBoxes_ == slowDetermineBoundingBoxes(synthesisMaskPyramid_, worker), worker);
}

PixelBoundingBoxes SynthesisPyramid::slowDetermineBoundingBoxes(const FramePyramid& maskPyramid, Worker* /*worker*/)
{
	PixelBoundingBoxes result;
	result.resize(maskPyramid.validLayers());

	for (unsigned int n = maskPyramid.validLayers() - 1u; n != (unsigned int)(-1); --n)
	{
		const Frame mask(maskPyramid[n], Frame::temporary_ACM_USE_KEEP_LAYOUT);

		result[n] = MaskAnalyzer::detectBoundingBox(mask.constdata<uint8_t>(), mask.width(), mask.height(), 0xFFu, mask.paddingElements());
	}

	return result;
}

}

}

}
