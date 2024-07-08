/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_LAYER_H
#define META_OCEAN_CV_SYNTHESIS_LAYER_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/Mapping.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/PixelBoundingBox.h"

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements the base class for all inpainting layers.
 * The inpainting layer holds one inpainting frame with a corresponding inpainting mask.<br>
 * Frame and mask must have the same frame dimensions and pixel origins.<br>
 * Optional, each layer can hold a bounding box enclosing the inpainting area to speedup the computation.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT Layer
{
	public:

		/**
		 * Returns the width of this layer.
		 * @return Layer with in pixel, with range [0, infinity)
		 */
		inline unsigned int width() const;

		/**
		 * Returns the height of this layer.
		 * @return Layer height in pixel, with range [0, infinity)
		 */
		inline unsigned int height() const;

		/**
		 * Returns the frame of this layer.
		 * @return Layer frame
		 */
		inline const Frame& frame() const;

		/**
		 * Returns the frame of this layer.
		 * @return Layer frame
		 */
		inline Frame& frame();

		/**
		 * Returns the mask of this layer.
		 * @return Layer mask
		 */
		inline const Frame& mask() const;

		/**
		 * Returns the mask of this layer.
		 * @return Layer mask
		 */
		inline Frame& mask();

		/**
		 * Returns the optional bounding box of this layer.
		 * @return Bounding box covering all mask pixels, if defined
		 */
		inline const PixelBoundingBox& boundingBox() const;

		/**
		 * Returns the mapping of this synthesis layer.
		 * @return Synthesis mapping
		 */
		virtual const Mapping& mapping() const = 0;

		/**
		 * Returns the mapping of this synthesis layer.
		 * @return Synthesis mapping
		 */
		virtual Mapping& mapping() = 0;

		/**
		 * Returns whether this layer holds at least one pixel.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/**
		 * Creates an empty inpainting layer.
		 */
		Layer() = default;

		/**
		 * Copy constructor.
		 * The resulting layer will hold a copy of the underlying image data.
		 * @param layer The layer to be copied
		 */
		Layer(const Layer& layer);

		/**
		 * Move constructor.
		 * @param layer The layer to be moved
		 */
		Layer(Layer&& layer) noexcept;

		/**
		 * Creates a new inpainting layer by a given frame and corresponding mask.
		 * @param frame Inpainting frame for this layer
		 * @param mask Inpainting mask of this layer
		 * @param boundingBox Optional inpainting bounding box covering all mask pixels, if defined
		 */
		Layer(Frame& frame, const Frame& mask, const PixelBoundingBox& boundingBox = PixelBoundingBox());

		/**
		 * Destructs the inpainting layer.
		 */
		virtual ~Layer();

		/**
		 * Assign operator.
		 * Afterwards, this layer will hold a copy of the underlying image data.
		 * @param layer The layer to be copied
		 * @return Reference to this layer
		 */
		Layer& operator=(const Layer& layer);

		/**
		 * Move operator.
		 * @param layer The layer to be moved
		 * @return Reference to this layer
		 */
		Layer& operator=(Layer&& layer) noexcept;

	protected:

		/// Width of this synthesis layer in pixel, with range [0, infinity)
		unsigned int width_ = 0u;

		/// Height of this synthesis layer in pixel, with range [0, infinity)
		unsigned int height_ = 0u;

		/// Frame of the inpainting layer, never owning the memory.
		Frame frame_;

		/// Mask of the inpainting layer, with same dimension and pixel origin as the defined frame, always owning the memory.
		Frame mask_;

		/// Optional bounding box covering all mask pixels, if defined.
		PixelBoundingBox boundingBox_;
};

inline unsigned int Layer::width() const
{
	return width_;
}

inline unsigned int Layer::height() const
{
	return height_;
}

inline const Frame& Layer::frame() const
{
	return frame_;
}

inline Frame& Layer::frame()
{
	return frame_;
}

inline const Frame& Layer::mask() const
{
	return mask_;
}

inline Frame& Layer::mask()
{
	return mask_;
}

inline const PixelBoundingBox& Layer::boundingBox() const
{
	return boundingBox_;
}

inline Layer::operator bool() const
{
	return width_ != 0u && height_ != 0u;
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_LAYER_1_H
