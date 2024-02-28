// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/cv/synthesis/SynthesisPyramidI1.h"
#include "ocean/cv/synthesis/CreatorInformationCost4NeighborhoodI1.h"
#include "ocean/cv/synthesis/CreatorInformationSpatialCostI1.h"
#include "ocean/cv/synthesis/CreatorInpaintingContentI1.h"
#include "ocean/cv/synthesis/InitializerAppearanceMappingI1.h"
#include "ocean/cv/synthesis/InitializerAppearanceMappingAreaConstrainedI1.h"
#include "ocean/cv/synthesis/InitializerCoarserMappingAdaptionAreaConstrainedI1.h"
#include "ocean/cv/synthesis/InitializerCoarserMappingAdaptionSpatialCostMaskI1.h"
#include "ocean/cv/synthesis/InitializerCoarserMappingAdaptionI1.h"
#include "ocean/cv/synthesis/InitializerContourMappingI1.h"
#include "ocean/cv/synthesis/InitializerRandomMappingAreaConstrainedI1.h"
#include "ocean/cv/synthesis/InitializerRandomMappingI1.h"
#include "ocean/cv/synthesis/InitializerShrinkingErosionI1.h"
#include "ocean/cv/synthesis/InitializerShrinkingErosionRandomizedI1.h"
#include "ocean/cv/synthesis/InitializerShrinkingPatchMatchingI1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodAreaConstrainedI1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodHighPerformanceI1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodHighPerformanceSkippingI1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodHighPerformanceSkippingByCostMaskI1.h"
#include "ocean/cv/synthesis/Optimizer4NeighborhoodStructuralConstrainedI1.h"

// #define WINDOWS_DEBUG_DESKTOP_OUTPUT

#if defined(WINDOWS_DEBUG_DESKTOP_OUTPUT) && defined(_WINDOWS)
	#include "ocean/platform/win/Utilities.h"
#endif

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

bool SynthesisPyramidI1::applyInpainting(const InitializationTechnique initializationTechnique, RandomGenerator& randomGenerator, const unsigned int weightFactor, const unsigned int borderFactor, const unsigned int maxSpatialCost, const unsigned int optimizationIterations, const unsigned int forced1ChannelLayers, const unsigned int skippingLayers, Worker* worker)
{
#ifdef OCEAN_DEBUG
	ocean_assert(synthesisHasBeenArranged_);
#endif

	ocean_assert(synthesisFramePyramid_.validLayers() == synthesisMaskPyramid_.validLayers());
	ocean_assert(synthesisFilterPyramid_.isNull() || synthesisFilterPyramid_.validLayers() == synthesisFramePyramid_.validLayers());
	ocean_assert(synthesisBoundingBoxes_.size() >= synthesisFramePyramid_.validLayers());

	layersReversedOrder_.clear(); // **TODO** replace - not clear
	layersReversedOrder_.reserve(synthesisFramePyramid_.validLayers());

	ocean_assert(optimizationIterations >= 1u);

	ocean_assert(weightFactor == 5u && borderFactor == 25u && "Currently we do not support other parameters as we need those parameters as template parameters, a solution can be a template and non-template implementation");
	OCEAN_SUPPRESS_UNUSED_WARNING(weightFactor);
	OCEAN_SUPPRESS_UNUSED_WARNING(borderFactor);

	const unsigned int layers = (unsigned int)synthesisFramePyramid_.validLayers();
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
			Frame frame(synthesisFramePyramid_[layerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);
			const LegacyFrame& mask = synthesisMaskPyramid_[layerIndex];
			const LegacyFrame* filter = synthesisFilterPyramid_.isValid() ? &(synthesisFilterPyramid_[layerIndex]) : nullptr;

			ocean_assert(frame.isValid() && mask.isValid() && FrameType(frame, mask.pixelFormat()) == mask.frameType());
			ocean_assert(filter == nullptr || filter->frameType() == mask.frameType());

			const PixelBoundingBox& boundingBox = synthesisBoundingBoxes_[layerIndex];

			layersReversedOrder_.emplace_back(frame, mask, boundingBox);
			LayerI1& layer = layersReversedOrder_.back();

			switch (initializationTechnique)
			{
				case IT_APPEARANCE:
				{
					if (filter != nullptr)
					{
						InitializerAppearanceMappingAreaConstrainedI1<1u, 100u>(layer, randomGenerator, Frame(*filter, Frame::temporary_ACM_USE_KEEP_LAYOUT)).invoke(worker);
					}
					else
					{
						InitializerAppearanceMappingI1<1u, 100u>(layer, randomGenerator).invoke(worker);
					}

					break;
				}

				case IT_RANDOM:
				{
					if (filter != nullptr)
					{
						InitializerRandomMappingAreaConstrainedI1(layer, randomGenerator, Frame(*filter, Frame::temporary_ACM_USE_KEEP_LAYOUT)).invoke(worker);
					}
					else
					{
						InitializerRandomMappingI1(layer, randomGenerator).invoke(worker);
					}

					break;
				}

				case IT_EROSION:
				{
					if (filter != nullptr)
					{
						InitializerShrinkingErosionI1(layer, randomGenerator, InitializerAppearanceMappingAreaConstrainedI1<1u, 75u>(layer, randomGenerator, Frame(*filter, Frame::temporary_ACM_USE_KEEP_LAYOUT))).invoke(worker);
					}
					else
					{
						InitializerShrinkingErosionI1(layer, randomGenerator, InitializerAppearanceMappingI1<1u, 75u>(layer, randomGenerator)).invoke(worker);
					}

					break;
				}

				case IT_RANDOM_EROSION:
				{
					if (filter != nullptr)
					{
						InitializerShrinkingErosionRandomizedI1(layer, randomGenerator, InitializerAppearanceMappingAreaConstrainedI1<1u, 75u>(layer, randomGenerator, Frame(*filter, Frame::temporary_ACM_USE_KEEP_LAYOUT))).invoke(worker);
					}
					else
					{
						InitializerShrinkingErosionRandomizedI1(layer, randomGenerator, InitializerAppearanceMappingI1<1u, 75u>(layer, randomGenerator)).invoke(worker);
					}

					break;
				}

				case IT_CONTOUR_MAPPING:
				{
					if (filter != nullptr)
					{
						InitializerContourMappingI1(layer, randomGenerator, InitializerAppearanceMappingAreaConstrainedI1<1u, 75u>(layer, randomGenerator, Frame(*filter, Frame::temporary_ACM_USE_KEEP_LAYOUT))).invoke(worker);
					}
					else
					{
						InitializerContourMappingI1(layer, randomGenerator, InitializerAppearanceMappingI1<1u, 75u>(layer, randomGenerator)).invoke(worker);
					}

					break;
				}

				case IT_PATCH_FULL_AREA_1:
					ocean_assert(filter == nullptr && "Not yet implemented!");
					InitializerShrinkingPatchMatchingI1(layer, randomGenerator, 1u, false, (unsigned int)(-1)).invoke(worker);
					break;

				case IT_PATCH_FULL_AREA_2:
					ocean_assert(filter == nullptr && "Not yet implemented!");
					InitializerShrinkingPatchMatchingI1(layer, randomGenerator, 2u, false, (unsigned int)(-1)).invoke(worker);
					break;

				case IT_PATCH_SUB_REGION_1:
					ocean_assert(filter == nullptr && "Not yet implemented!");
					InitializerShrinkingPatchMatchingI1(layer, randomGenerator, 1u, false, 30u).invoke(worker);
					break;

				case IT_PATCH_SUB_REGION_2:
					ocean_assert(filter == nullptr && "Not yet implemented!");
					InitializerShrinkingPatchMatchingI1(layer, randomGenerator, 2u, false, 30u).invoke(worker);
					break;

				case IT_PATCH_FULL_AREA_HEURISTIC_1:
					ocean_assert(filter == nullptr && "Not yet implemented!");
					InitializerShrinkingPatchMatchingI1(layer, randomGenerator, 1u, true, (unsigned int)(-1)).invoke(worker);
					break;

				case IT_PATCH_FULL_AREA_HEURISTIC_2:
					ocean_assert(filter == nullptr && "Not yet implemented!");
					InitializerShrinkingPatchMatchingI1(layer, randomGenerator, 2u, true, (unsigned int)(-1)).invoke(worker);
					break;
			}

			if (filter)
			{
				Optimizer4NeighborhoodAreaConstrainedI1<5u, 25u, true>(layer, randomGenerator, *filter).invoke(5u, 4u, maxSpatialCostLayer, worker, true);
			}
			else
			{
				Optimizer4NeighborhoodHighPerformanceI1<5u, 25u, true>(layer, randomGenerator).invoke(5u, 4u, maxSpatialCostLayer, worker, true);
			}

#ifdef WINDOWS_DEBUG_DESKTOP_OUTPUT
			{
				LegacyFrame frameCopy(layer.frame(), true);
				Platform::Win::Utilities::desktopFrameOutput(0, 0, frameCopy.width() * 4, frameCopy.height() * 4, frameCopy);
				Sleep(200u);

				CreatorInpaintingContentI1(layer, frameCopy).invoke(worker);
				Platform::Win::Utilities::desktopFrameOutput(0, 0, frameCopy.width() * 4, frameCopy.height() * 4, frameCopy);
				Sleep(200u);
			}
#endif // WINDOWS_DEBUG_DESKTOP_OUTPUT

		}
		else
		{
			// now as we have a synthesis information from the previous (coarser layer) we can propagate the information to this current layer

			Frame frame(synthesisFramePyramid_[layerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);
			const LegacyFrame& mask = synthesisMaskPyramid_[layerIndex];
			const LegacyFrame* filter = synthesisFilterPyramid_.isValid() ? &(synthesisFilterPyramid_[layerIndex]) : nullptr;

			ocean_assert(frame.isValid() && mask.isValid() && FrameType(frame, mask.pixelFormat()) == mask.frameType());
			ocean_assert(filter == nullptr || filter->frameType() == mask.frameType());

			const PixelBoundingBox& boundingBox = synthesisBoundingBoxes_[layerIndex];

			Frame frameToUse(frame, Frame::ACM_USE_KEEP_LAYOUT);

			// the finer synthesis layer can be converted to 1 channel 8 bit frames to speed up the computation (while some synthesis quality may get lost)
			if (layerIndex < forced1ChannelLayers)
			{
				FrameConverter::Comfort::convert(frame, FrameType(frame, FrameType::FORMAT_Y8), frameToUse, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, worker);
			}

			layersReversedOrder_.emplace_back(frameToUse, mask, boundingBox);

			if (filter != nullptr)
			{
				InitializerCoarserMappingAdaptionAreaConstrainedI1<2u>(layersReversedOrder_.back(), randomGenerator, layersReversedOrder_[layersReversedOrder_.size() - 2], Frame(*filter, Frame::temporary_ACM_USE_KEEP_LAYOUT)).invoke(worker);
			}
			else
			{
				InitializerCoarserMappingAdaptionI1<2u>(layersReversedOrder_.back(), randomGenerator, layersReversedOrder_[layersReversedOrder_.size() - 2]).invoke(worker);
			}

#if 1

			if (filter != nullptr)
			{
				Optimizer4NeighborhoodAreaConstrainedI1<5u, 25u, true>(layersReversedOrder_.back(), randomGenerator, *filter).invoke(5u, optimizationIterations, maxSpatialCostLayer, worker, true);
			}
			else
			{
				if (layerIndex < skippingLayers)
				{
					Optimizer4NeighborhoodHighPerformanceSkippingI1<5u, 25u, true>(layersReversedOrder_.back(), randomGenerator).invoke(5u, optimizationIterations, maxSpatialCostLayer, worker, true);
				}
				else
				{
					Optimizer4NeighborhoodHighPerformanceI1<5u, 25u, true>(layersReversedOrder_.back(), randomGenerator).invoke(5u, optimizationIterations, maxSpatialCostLayer, worker, true);
				}
			}

#else

			LegacyFrame costMask;
			if (layerIndex == 0)
			{
				InitializerCoarserMappingAdaptionSpatialCostMaskI1<2u, 1u>(synthesisLayersReversedOrder.back(), randomGenerator, synthesisLayersReversedOrder[synthesisLayersReversedOrder.size() - 2], costMask).invoke(worker);
			}
			else
			{
				InitializerCoarserMappingAdaptionSpatialCostMaskI1<2u, 9u>(synthesisLayersReversedOrder.back(), randomGenerator, synthesisLayersReversedOrder[synthesisLayersReversedOrder.size() - 2], costMask).invoke(worker);
			}

			Optimizer4NeighborhoodHighPerformanceSkippingByCostMaskI1<5u, 25u, true>(synthesisLayersReversedOrder.back(), randomGenerator, costMask).invoke(5u, optimizationIterations, maxSpatialCostLayer, worker, true);

#endif


#ifdef WINDOWS_DEBUG_DESKTOP_OUTPUT

			{
				uint64_t layerCost = 0ull;
				CreatorInformationCost4NeighborhoodI1<5u, 25u>(synthesisLayersReversedOrder.back(), layerCost).invoke(worker);

				LegacyFrame frameCopy(synthesisLayersReversedOrder.back().frame(), true);
				CreatorInpaintingContentI1(synthesisLayersReversedOrder.back(), frameCopy).invoke(worker);
				Platform::Win::Utilities::desktopFrameOutput(0, 0, frameCopy.width() * 2, frameCopy.height() * 2, frameCopy);
				Sleep(200);
			}

			{
				LegacyFrame frameCopy(FrameType(synthesisLayersReversedOrder.back().frame(), FrameType::FORMAT_Y8));
				memset(frameCopy.data<uint8_t>(), 0xFF, frameCopy.size());

				CreatorInformationSpatialCostI1<4u, false>(synthesisLayersReversedOrder.back(), frameCopy).invoke(worker);
				Platform::Win::Utilities::desktopFrameOutput(0, 0, frameCopy);
				Sleep(200);
			}

#endif // WINDOWS_DEBUG_DESKTOP_OUTPUT

		}
	}

	return true;
}

bool SynthesisPyramidI1::applyInpainting(const Constraints& constraints, RandomGenerator& randomGenerator, const unsigned int weightFactor, const unsigned int borderFactor, const unsigned int maxSpatialCost, const unsigned int optimizationIterations, const unsigned int skippingConstraintLayers, Worker* worker)
{
#ifdef OCEAN_DEBUG
	ocean_assert(synthesisHasBeenArranged_);
#endif

	ocean_assert(synthesisFramePyramid_.validLayers() == synthesisMaskPyramid_.validLayers());
	ocean_assert(synthesisBoundingBoxes_.size() >= synthesisFramePyramid_.validLayers());

	layersReversedOrder_.clear(); // **TODO** replace - not clear
	layersReversedOrder_.reserve(synthesisFramePyramid_.validLayers());

	ocean_assert(optimizationIterations >= 1u);

	ocean_assert(weightFactor == 5u && borderFactor == 25u && "Currently we do not support other parameters as we need those parameters as template parameters, a solution can be a template and non-template implementation");
	OCEAN_SUPPRESS_UNUSED_WARNING(weightFactor);
	OCEAN_SUPPRESS_UNUSED_WARNING(borderFactor);

	const unsigned int layers = (unsigned int)synthesisFramePyramid_.validLayers();
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
			Frame frame(synthesisFramePyramid_[layerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);
			const LegacyFrame& mask = synthesisMaskPyramid_[layerIndex];
			ocean_assert(frame.isValid() && mask.isValid());

			const PixelBoundingBox& boundingBox = synthesisBoundingBoxes_[layerIndex];

			layersReversedOrder_.emplace_back(frame, mask, boundingBox);
			LayerI1& layer = layersReversedOrder_.back();

			InitializerShrinkingPatchMatchingI1(layer, randomGenerator, 2u, false, 35u).invoke(worker);

			const Constraints scaledConstraints(constraints, Numeric::pow(Scalar(0.5), Scalar(layerIndex)));
			Optimizer4NeighborhoodStructuralConstrainedI1<5u, 25u, true>(layer, randomGenerator, scaledConstraints).invoke(5u, 4u, maxSpatialCostLayer, worker, true);

#ifdef WINDOWS_DEBUG_DESKTOP_OUTPUT
			{
				LegacyFrame frameCopy(layer.frame(), true);
				CreatorInpaintingContentI1(layer, frameCopy).invoke(worker);
				Platform::Win::Utilities::desktopFrameOutput(0, 0, frameCopy.width() * 4, frameCopy.height() * 4, frameCopy);
			}
#endif // WINDOWS_DEBUG_DESKTOP_OUTPUT
		}
		else
		{
			// now as we have a synthesis information from the previous (coarser layer) we can propagate the information to this current layer

			Frame frame(synthesisFramePyramid_[layerIndex], Frame::temporary_ACM_USE_KEEP_LAYOUT);
			const LegacyFrame& mask = synthesisMaskPyramid_[layerIndex];
			ocean_assert(frame.isValid() && mask.isValid());

			const PixelBoundingBox& boundingBox = synthesisBoundingBoxes_[layerIndex];

			layersReversedOrder_.emplace_back(frame, mask, boundingBox);
			InitializerCoarserMappingAdaptionI1<2u>(layersReversedOrder_.back(), randomGenerator, layersReversedOrder_[layersReversedOrder_.size() - 2]).invoke(worker);

			if (layerIndex < skippingConstraintLayers)
			{
				Optimizer4NeighborhoodHighPerformanceI1<5u, 25u, true>(layersReversedOrder_.back(), randomGenerator).invoke(5u, optimizationIterations, maxSpatialCostLayer, worker, true);
			}
			else
			{
				const Constraints scaledConstraints(constraints, Numeric::pow(Scalar(0.5), Scalar(layerIndex)));
				Optimizer4NeighborhoodStructuralConstrainedI1<5u, 25u, true>(layersReversedOrder_.back(), randomGenerator, scaledConstraints).invoke(5u, optimizationIterations, maxSpatialCostLayer, worker, true);
			}

#ifdef WINDOWS_DEBUG_DESKTOP_OUTPUT
			{
				LegacyFrame frameCopy(synthesisLayersReversedOrder.back().frame(), true);
				CreatorInpaintingContentI1(synthesisLayersReversedOrder.back(), frameCopy).invoke(worker);
				Platform::Win::Utilities::desktopFrameOutput(0, 0, frameCopy.width() * 2, frameCopy.height() * 2, frameCopy);
			}
#endif // WINDOWS_DEBUG_DESKTOP_OUTPUT
		}
	}

	return true;
}

bool SynthesisPyramidI1::createInpaintingResult(Frame& frame, Worker* worker) const
{
	ocean_assert(!layersReversedOrder_.empty());

	ocean_assert(frame.isValid());
	ocean_assert(FrameType(layersReversedOrder_.back().frame().frameType(), frame.pixelFormat()) == frame.frameType());

	if (layersReversedOrder_.empty() || frame.isNull() || FrameType(layersReversedOrder_.back().frame().frameType(), frame.pixelFormat()) != frame.frameType())
	{
		return false;
	}

	if (CreatorInpaintingContentI1(layersReversedOrder_.back(), frame).invoke(worker))
	{
		return true;
	}

	return false;

}

}

}

}
