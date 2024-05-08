/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_SYNTHESIS_PYRAMID_I_1_H
#define META_OCEAN_CV_SYNTHESIS_SYNTHESIS_PYRAMID_I_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Constraint.h"
#include "ocean/cv/synthesis/LayerI1.h"
#include "ocean/cv/synthesis/SynthesisPyramid.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the main pixel synthesis object holding several synthesis pyramid layers for one-frame synthesis.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT SynthesisPyramidI1 : public SynthesisPyramid
{
	public:

		// We want to use all implementations in this class.
		using SynthesisPyramid::applyInpainting;

	public:

		/**
		 * Creates a new synthesis pyramid object.
		 * Each synthesis pyramid must be arranged before an inpainting strategy can be invoked.
		 * @see arrange().
		 */
		inline SynthesisPyramidI1();

		/**
		 * Returns the finest synthesis layer of this pyramid object.
		 * Beware: Ensure that this pyramid holds at least one layer before calling this function!
		 * @return The pyramid's finest synthesis layer
		 */
		inline const LayerI1& finestLayer() const;

		/**
		 * Returns the finest synthesis layer of this pyramid object.
		 * Beware: Ensure that this pyramid holds at least one layer before calling this function!
		 * @return The pyramid's finest synthesis layer
		 */
		inline LayerI1& finestLayer();

		/**
		 * Returns the number of layers of this pyramid.
		 * @return The pyramid's number of layers, with range [0, infinity)
		 */
		inline size_t layers() const;

		/**
		 * Applies the inpainting on an initialized synthesis pyramid while a specific initialization technique is used on the coarsest pyramid layer.
		 * @see SynthesisPyramid::applyInpainting().
		 */
		bool applyInpainting(const InitializationTechnique initializationTechnique, RandomGenerator& randomGenerator, const unsigned int weightFactor = 5u, const unsigned int borderFactor = 26u, const unsigned int maxSpatialCost = (unsigned int)(-1), const unsigned int optimizationIterations = 2u, const unsigned int forced1ChannelLayers = 0u, const unsigned int skippingLayers = 0u, Worker* worker = nullptr) override;

		/**
		 * Applies the inpainting on an initialized synthesis pyramid while using a set of constraints to guide the inpainting result and quality.
		 * @param constraints The constraints to be used to guide the inpainting, at least one
		 * @param randomGenerator The random number generator to be used
		 * @param weightFactor Spatial weight impact, with range [0, infinity)
		 * @param borderFactor Weight factor of border pixels, with range [1, infinity)
		 * @param maxSpatialCost Maximal spatial cost, with range [0, 0xFFFFFFFF]
		 * @param optimizationIterations The number of optimization iterations on each pyramid layer (but on the coarsest layer), with range [1, infinity)
		 * @param skippingConstraintLayers The number of (finest) layers on which the constraints will not be applied anymore, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool applyInpainting(const Constraints& constraints, RandomGenerator& randomGenerator, const unsigned int weightFactor = 5u, const unsigned int borderFactor = 26u, const unsigned int maxSpatialCost = (unsigned int)(-1), const unsigned int optimizationIterations = 4u, const unsigned int skippingConstraintLayers = 2u, Worker* worker = nullptr);

		/**
		 * Creates the final inpainting result for the finest pyramid layer.
		 * @see SynthesisPyramid::createInpaintingResult().
		 */
		bool createInpaintingResult(Frame& frame, Worker* worker = nullptr) const override;

		/**
		 * Returns whether this pyramid object holds at least one layer.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	private:

		/// The individual synthesis layers for individual frame resolutions with reversed layer order.
		LayersI1 layersReversedOrder_;
};

inline SynthesisPyramidI1::SynthesisPyramidI1() :
	SynthesisPyramid()
{
	// nothing to do here
}

inline const LayerI1& SynthesisPyramidI1::finestLayer() const
{
	ocean_assert(!layersReversedOrder_.empty());
	return layersReversedOrder_.back();
}

inline LayerI1& SynthesisPyramidI1::finestLayer()
{
	ocean_assert(!layersReversedOrder_.empty());
	return layersReversedOrder_.back();
}

inline size_t SynthesisPyramidI1::layers() const
{
	return layersReversedOrder_.size();
}

inline SynthesisPyramidI1::operator bool() const
{
	return !layersReversedOrder_.empty();
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_SYNTHESIS_PYRAMID_I_1_H
