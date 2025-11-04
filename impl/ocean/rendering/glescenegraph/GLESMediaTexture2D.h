/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_MEDIA_TEXTURE_2D_H
#define META_OCEAN_RENDERING_GLES_MEDIA_TEXTURE_2D_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESTexture2D.h"

#include "ocean/rendering/MediaTexture2D.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class wraps a GLESceneGraph media texture object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESMediaTexture2D final :
	virtual public GLESTexture2D,
	virtual public MediaTexture2D
{
	friend class GLESFactory;

	public:

		/**
		 * Binds this texture.
		 * @see GLESTexture::bindTexture().
		 */
		unsigned int bindTexture(GLESShaderProgram& shaderProgram, const unsigned int id) override;

		/**
		 * Returns whether the texture internally holds valid data.
		 * @see Texture::isValid().
		 */
		bool isValid() const override;

		/**
		 * Returns descriptive information about the object as string.
		 * @see Object::descriptiveInformation().
		 */
		std::string descriptiveInformation() const override;

	protected:

		/**
		 * Creates a new GLESceneGraph texture 2D object.
		 */
		GLESMediaTexture2D();

		/**
		 * Destructs a GLESceneGraph texture 2D object.
		 */
		~GLESMediaTexture2D() override;

		/**
		 * Update function called by the framebuffer.
		 * @see DynamicObject::onDynamicUpdate().
		 */
		void onDynamicUpdate(const ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Updates the texture based on a given frame.
		 * @param frame The frame to be used to update the texture, must be valid
		 * @return True, if succeeded
		 */
		bool updateTexture(const Frame& frame);

	protected:

		/// The recent timestamp of the rendering engine.
		Timestamp renderTimestamp_;

		/// Optional temp conversion frame.
		Frame conversionFrame_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_MEDIA_TEXTURE_2D_H
