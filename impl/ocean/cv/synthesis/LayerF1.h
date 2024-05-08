/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_LAYER_F_1_H
#define META_OCEAN_CV_SYNTHESIS_LAYER_F_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/LayerF.h"
#include "ocean/cv/synthesis/LayerI1.h"
#include "ocean/cv/synthesis/MappingF1.h"

#include "ocean/math/SquareMatrix3.h"

#include <vector>

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

// Forward declaration.
class LayerF1;

/**
 * Definition of a vector holding one-frame sub-pixel layer objects.
 * @see LayerF1.
 * @ingroup cvsynthesis
 */
typedef std::vector<LayerF1> LayersF1;

/**
 * This class implements a single layer for pixel synthesis within one frame and sub-pixel accuracy.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT LayerF1 : public LayerF
{
	public:

		/**
		 * Creates an empty synthesis layer.
		 */
		LayerF1();

		/**
		 * Copy constructor.
		 * @param layer The layer to be copied
		 */
		LayerF1(const LayerF1& layer);

		/**
		 * Move constructor.
		 * @param layer The layer to be moved
		 */
		LayerF1(LayerF1&& layer) noexcept;

		/**
		 * Creates a new synthesis layer.
		 * @param frame The frame of the synthesis layer
		 * @param mask The mask of the synthesis layer, must have the same frame dimension as the frame
		 * @param boundingBox Optional bounding box to focus the computation
		 */
		LayerF1(Frame& frame, const Frame& mask, const PixelBoundingBox& boundingBox = PixelBoundingBox());

		/**
		 * Returns the mapping of this layer.
		 * @see Layer::mapping().
		 */
		const MappingF1& mapping() const override;

		/**
		 * Returns the mapping of this layer.
		 * @see Layer::mapping().
		 */
		MappingF1& mapping() override;

		/**
		 * Returns the mapping of this layer.
		 * @return Layer mapping
		 */
		inline const MappingF1& mappingF1() const;

		/**
		 * Returns the mapping of this layer.
		 * @return Layer mapping
		 */
		inline MappingF1& mappingF1();

		/**
		 * Assign operator.
		 * @param layer The layer to be copied
		 * @return The reference to this layer
		 */
		LayerF1& operator=(const LayerF1& layer);

		/**
		 * Assigns a layer with integer accuracy to this layer and converts the mapping accordingly.
		 * @param layer The layer to be assigned
		 * @return The reference to this layer
		 */
		LayerF1& operator=(const LayerI1& layer);

		/**
		 * Move operator.
		 * @param layer The layer to be moved
		 * @return The reference to this layer
		 */
		LayerF1& operator=(LayerF1&& layer) noexcept;

	private:

		/// Layer synthesis mapping.
		MappingF1 mapping_;
};

inline const MappingF1& LayerF1::mappingF1() const
{
	return mapping_;
}

inline MappingF1& LayerF1::mappingF1()
{
	return mapping_;
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_LAYER_F_1_H
