/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_MEDIA_TEXTURE_2D_H
#define META_OCEAN_RENDERING_GI_MEDIA_TEXTURE_2D_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GITexture2D.h"

#include "ocean/rendering/MediaTexture2D.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class wraps a Global Illumination media texture object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIMediaTexture2D final :
	virtual public GITexture2D,
	virtual public MediaTexture2D
{
	friend class GIFactory;

	public:

		/**
		 * Returns the frame of this texture.
		 * @return Texture frame
		 */
		inline const Frame& frame() const;

		/**
		 * Returns the texture color for a given texture coordinate.
		 * @see GITexture2D::textureColor().
		 */
		bool textureColor(const TextureCoordinate& textureCoordinate, RGBAColor& color) const override;

		/**
		 * Returns whether the texture internally holds valid data.
		 * @see Texture::isValid().
		 */
		bool isValid() const override;

	protected:

		/**
		 * Creates a new Global Illumination texture 2D object.
		 */
		GIMediaTexture2D();

		/**
		 * Destructs a Global Illumination texture 2D object.
		 */
		~GIMediaTexture2D() override;

		/**
		 * Update function called by the framebuffer.
		 * @see DynamicObject::onDynamicUpdate().
		 */
		void onDynamicUpdate(const ViewRef& view, const Timestamp timestamp) override;

	protected:

		/// Texture frame.
		Frame textureFrame_;
};

inline const Frame& GIMediaTexture2D::frame() const
{
	return textureFrame_;
}

}

}

}

#endif // META_OCEAN_RENDERING_GI_MEDIA_TEXTURE_2D_H
