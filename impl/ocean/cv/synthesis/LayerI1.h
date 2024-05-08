/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_LAYER_I_1_H
#define META_OCEAN_CV_SYNTHESIS_LAYER_I_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/LayerI.h"
#include "ocean/cv/synthesis/MappingI1.h"

#include <vector>

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

// Forward declaration.
class LayerI1;

/**
 * Definition of a vector holding one-frame pixel layer objects.
 * @see LayerI1.
 * @ingroup cvsynthesis
 */
typedef std::vector<LayerI1> LayersI1;

/**
 * This class implements a single layer for pixel synthesis within one frame and pixel accuracy.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT LayerI1 : public LayerI
{
	public:

		/**
		 * Creates an empty pixel layer.
		 */
		LayerI1();

		/**
		 * Copy constructor.
		 * @param layer The layer to be copied
		 */
		LayerI1(const LayerI1& layer);

		/**
		 * Move constructor.
		 * @param layer The layer to be moved
		 */
		LayerI1(LayerI1&& layer) noexcept;

		/**
		 * Creates a new pixel layer for a given frame and inpainting mask.
		 * @param frame Inpainting frame
		 * @param mask Inpainting mask with same frame type as the given inpainting frame
		 * @param boundingBox Optional inpainting bounding box to speed up the process
		 */
		LayerI1(Frame& frame, const Frame& mask, const PixelBoundingBox& boundingBox = PixelBoundingBox());

		/**
		 * Returns the mapping of this synthesis layer.
		 * @see Layer::mapping().
		 */
		MappingI1& mapping() override;

		/**
		 * Returns the mapping of this layer.
		 * @see Layer::mapping().
		 */
		const MappingI1& mapping() const override;

		/**
		 * Returns the mapping of this layer.
		 * @return Layer mapping
		 */
		inline const MappingI1& mappingI1() const;

		/**
		 * Returns the mapping of this layer.
		 * @return Layer mapping
		 */
		inline MappingI1& mappingI1();

		/**
		 * Assign operator.
		 * @param layer The layer to be copied
		 * @return The reference to this layer
		 */
		LayerI1& operator=(const LayerI1& layer);

		/**
		 * Move operator.
		 * @param layer The layer to be moved
		 * @return The reference to this layer
		 */
		LayerI1& operator=(LayerI1&& layer) noexcept;

	private:

		/// Pixel mapping associated with this synthesis layer.
		MappingI1 mapping_;
};

inline const MappingI1& LayerI1::mappingI1() const
{
	return mapping_;
}

inline MappingI1& LayerI1::mappingI1()
{
	return mapping_;
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_LAYER_I_1_H
