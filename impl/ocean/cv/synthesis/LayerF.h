/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_LAYER_F_H
#define META_OCEAN_CV_SYNTHESIS_LAYER_F_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Layer.h"
#include "ocean/cv/synthesis/MappingF.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the base class for all synthesis layers with float accuracy.
 * @ingroup cvsynthesis
 */
class LayerF : public Layer
{
	public:

		/**
		 * Returns the mapping of this synthesis layer.
		 * @return Synthesis mapping
		 */
		virtual const MappingF& mapping() const = 0;

		/**
		 * Returns the mapping of this synthesis layer.
		 * @return Synthesis mapping
		 */
		virtual MappingF& mapping() = 0;

	protected:

		/**
		 * Creates an empty layer object.
		 */
		inline LayerF();

		/**
		 * Copy constructor.
		 * @param layer The layer to be copied
		 */
		inline LayerF(const LayerF& layer);

		/**
		 * Move constructor.
		 * @param layer The layer to be moved
		 */
		inline LayerF(LayerF&& layer) noexcept;

		/**
		 * Creates a new inpainting layer by a given frame and corresponding mask.
		 * @param frame Inpainting frame for this layer
		 * @param mask Inpainting mask of this layer
		 * @param boundingBox Optional inpainting bounding box covering all mask pixels, if defined
		 */
		inline LayerF(Frame& frame, const Frame& mask, const PixelBoundingBox& boundingBox = PixelBoundingBox());

		/**
		 * Assign operator.
		 * @param layer The layer to be copied
		 * @return The reference to this layer
		 */
		inline LayerF& operator=(const LayerF& layer);

		/**
		 * Move operator.
		 * @param layer The layer to be moved
		 * @return The reference to this layer
		 */
		inline LayerF& operator=(LayerF&& layer) noexcept;
};

inline LayerF::LayerF() :
	Layer()
{
	// nothing to do here
}

inline LayerF::LayerF(const LayerF& layer) :
	Layer(layer)
{
	// nothing to do here
}

inline LayerF::LayerF(LayerF&& layer) noexcept :
	Layer(std::move(layer))
{
	// nothing to do here
}

inline LayerF::LayerF(Frame& frame, const Frame& mask, const PixelBoundingBox& boundingBox) :
	Layer(frame, mask, boundingBox)
{
	// nothing to do here
}

inline LayerF& LayerF::operator=(const LayerF& layer)
{
	Layer::operator=(layer);
	return *this;
}

inline LayerF& LayerF::operator=(LayerF&& layer) noexcept
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
