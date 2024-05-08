/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/synthesis/SynthesisPyramidF1.h"
#include "ocean/cv/synthesis/CreatorInpaintingContentF1.h"
#include "ocean/cv/synthesis/InitializerAppearanceMappingF1.h"
#include "ocean/cv/synthesis/InitializerAppearanceMappingI1.h"
#include "ocean/cv/synthesis/InitializerAppearanceMappingAreaConstrainedI1.h"
#include "ocean/cv/synthesis/InitializerCoarserMappingAdaptionAreaConstrainedI1.h"
#include "ocean/cv/synthesis/InitializerCoarserMappingAdaptionF1.h"
#include "ocean/cv/synthesis/InitializerCoarserMappingAdaptionSpatialCostMaskI1.h"
#include "ocean/cv/synthesis/InitializerCoarserMappingAdaptionI1.h"
#include "ocean/cv/synthesis/InitializerContourMappingI1.h"
#include "ocean/cv/synthesis/InitializerConvertMappingF1.h"
#include "ocean/cv/synthesis/InitializerRandomMappingAreaConstrainedI1.h"
#include "ocean/cv/synthesis/InitializerRandomMappingI1.h"
#include "ocean/cv/synthesis/InitializerShrinkingErosionI1.h"
#include "ocean/cv/synthesis/InitializerShrinkingErosionRandomizedI1.h"
#include "ocean/cv/synthesis/InitializerShrinkingPatchMatchingI1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodHighPerformanceF1.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

bool SynthesisPyramidF1::applyInpainting(const InitializationTechnique initializationTechnique, RandomGenerator& randomGenerator, const unsigned int weightFactor, const unsigned int borderFactor, const unsigned int maxSpatialCost, const unsigned int optimizationIterations, const unsigned int forced1ChannelLayers, const unsigned int skippingLayers, Worker* worker)
{
#ifdef OCEAN_DEBUG
	ocean_assert(synthesisHasBeenArranged_);
#endif

	ocean_assert(synthesisFramePyramid_.layers() == synthesisMaskPyramid_.layers());
	ocean_assert(!synthesisFilterPyramid_.isValid() || synthesisFilterPyramid_.layers() == synthesisFramePyramid_.layers());
	ocean_assert(synthesisBoundingBoxes_.size() >= synthesisFramePyramid_.layers());

	layersReversedOrder_.clear(); // **TODO** replace - not clear
	layersReversedOrder_.reserve(synthesisFramePyramid_.layers());

	ocean_assert(optimizationIterations >= 1u);

	ocean_assert(weightFactor == 5u && borderFactor == 25u && "Currently we do not support other parameters as we need those parameters as template parameters, a solution can be a template and non-template implementation");
	OCEAN_SUPPRESS_UNUSED_WARNING(weightFactor);
	OCEAN_SUPPRESS_UNUSED_WARNING(borderFactor);

	const unsigned int layers = (unsigned int)synthesisFramePyramid_.layers();
	ocean_assert(layers >= 1u);

	for (unsigned int layerIndex = layers - 1u; layerIndex != (unsigned int)(-1); --layerIndex)
	{
		const unsigned int iterationIndex = layers - layerIndex - 1u;
		ocean_assert(iterationIndex < layers);

		/**
		 * we adjust the maximal spatial cost wrt. the resolution of the synthesis layer
		 *
		 * finest layer        (layer index 0) has maxSpatialCost
		 * second finest layer (layer index 1) has maxSpatialCost / 4
		 * third  finest layer (layer index 2) has maxSpatialCost / 16
		 * ...
		 */
		const unsigned int maxSpatialCostLayer = maxSpatialCost == (unsigned int)(-1) ? (unsigned int)(-1) : max(1u, maxSpatialCost >> (layerIndex * 2u));

		// the coarsest layer cannot use any synthesis mapping information of a previous layer so that this layer needs a special initialization
		// individual initialization strategies exist with individual visual results
		if (iterationIndex == 0u)
		{
			Frame& frame = synthesisFramePyramid_[layerIndex];
			const Frame& mask = synthesisMaskPyramid_[layerIndex];
			const Frame& filter = synthesisFilterPyramid_.isValid() ? synthesisFilterPyramid_[layerIndex] : Frame();

			ocean_assert(frame.isValid() && mask.isValid() && FrameType(frame, mask.pixelFormat()) == mask.frameType());
			ocean_assert(!filter.isValid() || filter.frameType() == mask.frameType());

			const PixelBoundingBox& boundingBox = synthesisBoundingBoxes_[layerIndex];

			layersReversedOrder_.emplace_back(frame, mask, boundingBox);
			LayerF1& layer = layersReversedOrder_.back();

			switch (initializationTechnique)
			{
				case IT_APPEARANCE:
				{
					ocean_assert(!filter.isValid() && "Not yet implemented!");
					InitializerAppearanceMappingF1<1u, 100u>(layer, randomGenerator).invoke(worker);

					break;
				}

				case IT_RANDOM:
				{
					LayerI1 layerI(frame, mask, boundingBox);

					if (filter.isValid())
					{
						InitializerRandomMappingAreaConstrainedI1(layerI, randomGenerator, filter).invoke(worker);
					}
					else
					{
						InitializerRandomMappingI1(layerI, randomGenerator).invoke(worker);
					}

					InitializerConvertMappingF1(layer, layerI).invoke(worker);

					break;
				}

				case IT_EROSION:
				{
					LayerI1 layerI(frame, mask, boundingBox);

					if (filter.isValid())
					{
						InitializerShrinkingErosionI1(layerI, randomGenerator, InitializerAppearanceMappingAreaConstrainedI1<1u, 75u>(layerI, randomGenerator, filter)).invoke(worker);
					}
					else
					{
						InitializerShrinkingErosionI1(layerI, randomGenerator, InitializerAppearanceMappingI1<1u, 75u>(layerI, randomGenerator)).invoke(worker);
					}

					InitializerConvertMappingF1(layer, layerI).invoke(worker);

					break;
				}

				case IT_RANDOM_EROSION:
				{
					LayerI1 layerI(frame, mask, boundingBox);

					if (filter.isValid())
					{
						InitializerShrinkingErosionRandomizedI1(layerI, randomGenerator, InitializerAppearanceMappingAreaConstrainedI1<1u, 75u>(layerI, randomGenerator, filter)).invoke(worker);
					}
					else
					{
						InitializerShrinkingErosionRandomizedI1(layerI, randomGenerator, InitializerAppearanceMappingI1<1u, 75u>(layerI, randomGenerator)).invoke(worker);
					}

					InitializerConvertMappingF1(layer, layerI).invoke(worker);

					break;
				}

				case IT_CONTOUR_MAPPING:
				{
					LayerI1 layerI(frame, mask, boundingBox);

					if (filter.isValid())
					{
						InitializerContourMappingI1(layerI, randomGenerator, InitializerAppearanceMappingAreaConstrainedI1<1u, 75u>(layerI, randomGenerator, filter)).invoke(worker);
					}
					else
					{
						InitializerContourMappingI1(layerI, randomGenerator, InitializerAppearanceMappingI1<1u, 75u>(layerI, randomGenerator)).invoke(worker);
					}

					InitializerConvertMappingF1(layer, layerI).invoke(worker);

					break;
				}

				case IT_PATCH_FULL_AREA_1:
				{
					LayerI1 layerI(frame, mask, boundingBox);

					ocean_assert(!filter.isValid() && "Not yet implemented!");
					InitializerShrinkingPatchMatchingI1(layerI, randomGenerator, 1u, false, (unsigned int)(-1)).invoke(worker);

					InitializerConvertMappingF1(layer, layerI).invoke(worker);

					break;
				}

				case IT_PATCH_FULL_AREA_2:
				{
					LayerI1 layerI(frame, mask, boundingBox);

					ocean_assert(!filter.isValid() && "Not yet implemented!");
					InitializerShrinkingPatchMatchingI1(layerI, randomGenerator, 2u, false, (unsigned int)(-1)).invoke(worker);

					InitializerConvertMappingF1(layer, layerI).invoke(worker);

					break;
				}

				case IT_PATCH_SUB_REGION_1:
				{
					LayerI1 layerI(frame, mask, boundingBox);

					ocean_assert(!filter.isValid() && "Not yet implemented!");
					InitializerShrinkingPatchMatchingI1(layerI, randomGenerator, 1u, false, 30u).invoke(worker);

					InitializerConvertMappingF1(layer, layerI).invoke(worker);

					break;
				}

				case IT_PATCH_SUB_REGION_2:
				{
					LayerI1 layerI(frame, mask, boundingBox);

					ocean_assert(!filter.isValid() && "Not yet implemented!");
					InitializerShrinkingPatchMatchingI1(layerI, randomGenerator, 2u, false, 30u).invoke(worker);

					InitializerConvertMappingF1(layer, layerI).invoke(worker);

					break;
				}

				case IT_PATCH_FULL_AREA_HEURISTIC_1:
				{
					LayerI1 layerI(frame, mask, boundingBox);

					ocean_assert(!filter.isValid() && "Not yet implemented!");
					InitializerShrinkingPatchMatchingI1(layerI, randomGenerator, 1u, true, (unsigned int)(-1)).invoke(worker);

					InitializerConvertMappingF1(layer, layerI).invoke(worker);

					break;
				}

				case IT_PATCH_FULL_AREA_HEURISTIC_2:
				{
					LayerI1 layerI(frame, mask, boundingBox);

					ocean_assert(!filter.isValid() && "Not yet implemented!");
					InitializerShrinkingPatchMatchingI1(layerI, randomGenerator, 2u, true, (unsigned int)(-1)).invoke(worker);

					InitializerConvertMappingF1(layer, layerI).invoke(worker);

					break;
				}
			}

			ocean_assert(!filter.isValid() && "Not yet implemented!");
			Optimizer4NeighborhoodHighPerformanceF1<5u, 25u, true>(layer, randomGenerator).invoke(5u, 4u, maxSpatialCostLayer, worker, true);
		}
		else
		{
			// now as we have a synthesis information from the previous (coarser layer) we can propagate the information to this current layer

			Frame& frame = synthesisFramePyramid_[layerIndex];
			const Frame& mask = synthesisMaskPyramid_[layerIndex];
			const Frame& filter = synthesisFilterPyramid_.isValid() ? synthesisFilterPyramid_[layerIndex] : Frame();

			ocean_assert(frame.isValid() && mask.isValid() && FrameType(frame, mask.pixelFormat()) == mask.frameType());
			ocean_assert(!filter.isValid() || filter.frameType() == mask.frameType());

			const PixelBoundingBox& boundingBox = synthesisBoundingBoxes_[layerIndex];

			Frame frameToUse(frame, Frame::ACM_USE_KEEP_LAYOUT);

			// the finer synthesis layer can be converted to 1 channel 8 bit frames to speed up the computation (while some synthesis quality may get lost)
			if (layerIndex < forced1ChannelLayers)
			{
				FrameConverter::Comfort::convert(frame, FrameType::FORMAT_Y8, frameToUse, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker);
			}

			layersReversedOrder_.emplace_back(frameToUse, mask, boundingBox);

			ocean_assert_and_suppress_unused(!filter.isValid() && "Not yet implemented!", filter);
			InitializerCoarserMappingAdaptionF1<2u>(layersReversedOrder_.back(), randomGenerator, layersReversedOrder_[layersReversedOrder_.size() - 2]).invoke(worker);

			ocean_assert_and_suppress_unused(skippingLayers == 0u && "Not yet implemented!", skippingLayers);
			Optimizer4NeighborhoodHighPerformanceF1<5u, 25u, true>(layersReversedOrder_.back(), randomGenerator).invoke(5u, optimizationIterations, maxSpatialCostLayer, worker, true);
		}
	}

	return true;
}

bool SynthesisPyramidF1::createInpaintingResult(Frame& frame, Worker* worker) const
{
	ocean_assert(!layersReversedOrder_.empty());

	ocean_assert(frame.isValid());
	ocean_assert(FrameType(layersReversedOrder_.back().frame().frameType(), frame.pixelFormat()) == frame.frameType());

	if (layersReversedOrder_.empty() || !frame.isValid() || FrameType(layersReversedOrder_.back().frame().frameType(), frame.pixelFormat()) != frame.frameType())
	{
		return false;
	}

	if (CreatorInpaintingContentF1(layersReversedOrder_.back(), frame).invoke(worker))
	{
		return true;
	}

	return false;
}

}

}

}
