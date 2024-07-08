/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_SYNTHESIS_PYRAMID_H
#define META_OCEAN_CV_SYNTHESIS_SYNTHESIS_PYRAMID_H

#include "ocean/cv/synthesis/Synthesis.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/PixelBoundingBox.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the base class for all synthesis pyramids.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT SynthesisPyramid
{
	public:

		/**
		 * Definition of individual synthesis qualities.
		 */
		enum SynthesisQuality
		{
			/// Low synthesis image quality, but very fast.
			SQ_LOW,
			/// Moderate synthesis image quality, with good relation between quality and performance.
			SQ_MODERATE,
			/// High synthesis image quality, while the performance will be below normal.
			SQ_HIGH,
			/// Very high synthesis image quality, while the performance will be slow.
			SQ_VERY_HIGH,
		};

		/**
		 * Definition of individual initialization techniques.
		 */
		enum InitializationTechnique
		{
			/// Appearance mapping initialization, for each mask pixel the best matching target pixel is assigned as initial mapping.
			IT_APPEARANCE,
			/// Random mapping initialization, for each mask pixel a random target pixel is assigned as initial mapping.
			IT_RANDOM,
			/// Erosion initialization, shrinking the inpainting mask by an erosion filter in a first step and applying an appearance mapping in a second step.
			IT_EROSION,
			/// Randomized erosion initialization, shrinking the inpainting mask by a randomized erosion filter in a first step and applying an appearance mapping in a second step.
			IT_RANDOM_EROSION,
			/// Contour mapping initialization, for each contour pixel a corresponding second contour pixel is determined an the color values inbetween are interpolated, followed by an appearance mapping.
			IT_CONTOUR_MAPPING,
			/// Patch initialization, a priority patch-based inpainting approach using the entire source frame as visual input information, applying one iteration.
			IT_PATCH_FULL_AREA_1,
			/// Patch initialization, a priority patch-based inpainting approach using the entire source frame as visual input information, applying two iteration.
			IT_PATCH_FULL_AREA_2,
			/// Patch initialization, a priority patch-based inpainting approach using a surrounding sub-region as visual input information, applying one iteration.
			IT_PATCH_SUB_REGION_1,
			/// Patch initialization, a priority patch-based inpainting approach using a surrounding sub-region as visual input information, applying two iteration.
			IT_PATCH_SUB_REGION_2,
			/// Patch initialization, a priority patch-based inpainting approach using the entire source frame as visual input information in combination with an heuristic and propagation appraoch, applying one iteration.
			IT_PATCH_FULL_AREA_HEURISTIC_1,
			/// Patch initialization, a priority patch-based inpainting approach using the entire source frame as visual input information in combination with an heuristic and propagation appraoch, applying two iteration.
			IT_PATCH_FULL_AREA_HEURISTIC_2
		};

	public:

		/**
		 * Arranges this synthesis pyramid object for a specified frame and corresponding inpainting mask.
		 * Each pyramid object must be initialized before the actual inpainting can be invoked.
		 * @param frame The frame containing the undesired object to be removed from the frame, must be valid
		 * @param mask The 8 bit mask identifying undesired pixels in the provided frame, pixel values of 0x00 identify undesired pixels, pixel values of 0xFF identify desired pixels, with same frame dimension and pixel origin as the provided frame
		 * @param worker Optional worker object to distribute the computation
		 * @param binomialFilterOnCoarsestLayers True, to applied a binomial filtering on the coarsest pyramid inpainting layer
		 * @param binomialFilterOnFineLayers True, to applied a binomial filter on every other than the coarsest pyramid inpainting layer,
		 * @param filter Optional filter frame identifying valid and invalid source pixels for the image synthesis, if defined must have the same frame dimension and pixel origin as the provided frame
		 * @return True, if succeeded
		 */
		bool arrange(const Frame& frame, const Frame& mask, Worker* worker = nullptr, const bool binomialFilterOnCoarsestLayers = false, const bool binomialFilterOnFineLayers = false, const Frame& filter = Frame());

		/**
		 * Applies the inpainting on an initialized synthesis pyramid satisfying a specific quality.
		 * @param synthesisQuality The synthesis quality to be reached
		 * @param randomGenerator The random number generator to be used
		 * @param weightFactor Spatial weight impact, with range [0, infinity)
		 * @param borderFactor Weight factor of border pixels, with range [1, infinity)
		 * @param maxSpatialCost Maximal spatial cost, with range [0, 0xFFFFFFFF]
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		virtual bool applyInpainting(const SynthesisQuality synthesisQuality, RandomGenerator& randomGenerator, const unsigned int weightFactor = 5u, const unsigned int borderFactor = 25u, const unsigned int maxSpatialCost = (unsigned int)(-1), Worker* worker = nullptr);

		/**
		 * Applies the inpainting on an initialized synthesis pyramid while a specific initialization technique is used on the coarsest pyramid layer.
		 * @param initializationTechnique The initialization technique to be used on the coarsest pyramid layer
		 * @param randomGenerator The random number generator to be used
		 * @param weightFactor Spatial weight impact, with range [0, infinity)
		 * @param borderFactor Weight factor of border pixels, with range [1, infinity)
		 * @param maxSpatialCost Maximal spatial cost, with range [0, 0xFFFFFFFF]
		 * @param optimizationIterations The number of optimization iterations on each pyramid layer (but on the coarsest layer), with range [1, infinity)
		 * @param forced1ChannelLayers The number of (finest) layers on which the synthesis frame is forced to be (by conversion) a 8 bit grayscale frame, with range [0, infinity)
		 * @param skippingLayers The number of (finest) layers on which a skipped optimization will be applied, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		virtual bool applyInpainting(const InitializationTechnique initializationTechnique, RandomGenerator& randomGenerator, const unsigned int weightFactor = 5u, const unsigned int borderFactor = 25u, const unsigned int maxSpatialCost = (unsigned int)(-1), const unsigned int optimizationIterations = 2u, const unsigned int forced1ChannelLayers = 0u, const unsigned int skippingLayers = 0u, Worker* worker = nullptr) = 0;

		/**
		 * Creates the final inpainting result for the finest pyramid layer.
		 * Make sure the given frame has the same frame dimension as the finest pyramid layer, the pixel format may be arbitrary.
		 * @param frame The frame to be finally inpainted, must be valid
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		virtual bool createInpaintingResult(Frame& frame, Worker* worker = nullptr) const = 0;

	protected:

		/**
		 * Protected constructor creating a new synthesis pyramid object.
		 * Each synthesis pyramid must be initialized before an inpainting strategy can be invoked.
		 * @see initialize().
		 */
		SynthesisPyramid();

		/**
		 * Virtual Destructor
		 */
		virtual ~SynthesisPyramid() {}

		/**
		 * Determines the mask bounding boxes for all pyramid layers.
		 * @param worker Optional worker object to distribute the computation
		 */
		void determineBoundingBoxes(Worker* worker = nullptr);

		/**
		 * Determines the mask bounding boxes for all pyramid layers.
		 * This function is a slow implementation; however, we keep it for debugging purpose.
		 * @param maskPyramid The pyramid of the inpainting masks
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting individual bounding boxes, one for each valid pyramid layer
		 */
		PixelBoundingBoxes slowDetermineBoundingBoxes(const FramePyramid& maskPyramid, Worker* worker = nullptr);

	protected:

		/// The frame pyramid holding the frame to be inpainted for individual frame resolutions.
		FramePyramid synthesisFramePyramid_;

		/// The (frame) pyramid of a mask defining the undesired object for individual frame resolutions.
		FramePyramid synthesisMaskPyramid_;

		/// An optional frame pyramid of an additional filter information identifying valid and invalid source pixels for individual frame resolutions.
		FramePyramid synthesisFilterPyramid_;

		/// The bounding boxes enclosing the undesired mask on each individual layer, with pixel accuracy.
		PixelBoundingBoxes synthesisBoundingBoxes_;

#ifdef OCEAN_DEBUG
		/// True, if this synthesis pyramid object has been arranged.
		bool synthesisHasBeenArranged_;
#endif
};

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_SYNTHESIS_PYRAMID_H
