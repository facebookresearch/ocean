/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_LAYER_I_H
#define META_OCEAN_CV_SYNTHESIS_LAYER_I_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Layer.h"
#include "ocean/cv/synthesis/MappingI.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the base class for all synthesis layers with integer accuracy.
 * @ingroup cvsynthesis
 */
class LayerI : public Layer
{
	public:

		/**
		 * Returns the mapping of this synthesis layer.
		 * @return Synthesis mapping
		 */
		virtual const MappingI& mapping() const = 0;

		/**
		 * Returns the mapping of this synthesis layer.
		 * @return Synthesis mapping
		 */
		virtual MappingI& mapping() = 0;

	protected:

		/**
		 * Creates an empty layer object.
		 */
		inline LayerI();

		/**
		 * Copy constructor.
		 * @param layer The layer to be copied
		 */
		inline LayerI(const LayerI& layer);

		/**
		 * Move constructor.
		 * @param layer The layer to be moved
		 */
		inline LayerI(LayerI&& layer) noexcept;

		/**
		 * Creates a new inpainting layer by a given frame and corresponding mask.
		 * @param frame Inpainting frame for this layer
		 * @param mask Inpainting mask of this layer
		 * @param boundingBox Optional inpainting bounding box covering all mask pixels, if defined
		 */
		inline LayerI(Frame& frame, const Frame& mask, const PixelBoundingBox& boundingBox = PixelBoundingBox());

		/**
		 * Assign operator.
		 * @param layer The layer to be copied
		 * @return The reference to this layer
		 */
		inline LayerI& operator=(const LayerI& layer);

		/**
		 * Move operator.
		 * @param layer The layer to be moved
		 * @return The reference to this layer
		 */
		inline LayerI& operator=(LayerI&& layer) noexcept;
};

inline LayerI::LayerI() :
	Layer()
{
	// nothing to do here
}

inline LayerI::LayerI(const LayerI& layer) :
	Layer(std::move(layer))
{
	// nothing to do here
}

inline LayerI::LayerI(LayerI&& layer) noexcept :
	Layer(std::move(layer))
{
	// nothing to do here
}

inline LayerI::LayerI(Frame& frame, const Frame& mask, const PixelBoundingBox& boundingBox) :
	Layer(frame, mask, boundingBox)
{
	// nothing to do here
}

inline LayerI& LayerI::operator=(const LayerI& layer)
{
	Layer::operator=(layer);
	return *this;
}

inline LayerI& LayerI::operator=(LayerI&& layer) noexcept
{
	if (this != &layer)
	{
		Layer::operator=(std::move(layer));
	}

	return *this;
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_LAYER_I_H
