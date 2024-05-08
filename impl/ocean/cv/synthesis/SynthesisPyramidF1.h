/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_SYNTHESIS_PYRAMID_F_1_H
#define META_OCEAN_CV_SYNTHESIS_SYNTHESIS_PYRAMID_F_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/LayerF1.h"
#include "ocean/cv/synthesis/SynthesisPyramid.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the main pixel synthesis object holding several synthesis pyramid layers.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT SynthesisPyramidF1 : public SynthesisPyramid
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
		inline SynthesisPyramidF1();

		/**
		 * Returns the finest synthesis layer of this pyramid object.
		 * Beware: Ensure that this pyramid holds at least one layer before calling this function!
		 * @return The pyramid's finest synthesis layer
		 */
		inline const LayerF1& finestLayer() const;

		/**
		 * Returns the finest synthesis layer of this pyramid object.
		 * Beware: Ensure that this pyramid holds at least one layer before calling this function!
		 * @return The pyramid's finest synthesis layer
		 */
		inline LayerF1& finestLayer();

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
		LayersF1 layersReversedOrder_;
};

inline SynthesisPyramidF1::SynthesisPyramidF1() :
	SynthesisPyramid()
{
	// nothing to do here
}

inline const LayerF1& SynthesisPyramidF1::finestLayer() const
{
	ocean_assert(!layersReversedOrder_.empty());
	return layersReversedOrder_.back();
}

inline LayerF1& SynthesisPyramidF1::finestLayer()
{
	ocean_assert(!layersReversedOrder_.empty());
	return layersReversedOrder_.back();
}

inline size_t SynthesisPyramidF1::layers() const
{
	return layersReversedOrder_.size();
}

inline SynthesisPyramidF1::operator bool() const
{
	return !layersReversedOrder_.empty();
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_SYNTHESIS_PYRAMID_F_1_H
