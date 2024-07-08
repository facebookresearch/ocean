/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_TEXTURES_H
#define META_OCEAN_RENDERING_GLES_TEXTURES_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESAttribute.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"

#include "ocean/rendering/Textures.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph texture attributes object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESTextures :
	virtual public GLESAttribute,
	virtual public Textures
{
	friend class GLESFactory;

	public:

		/**
		 * Sets the texture and attribute for a specified texture layer.
		 * @see Textures::setTexture().
		 */
		void setTexture(const TextureRef& texture, const unsigned int layerIndex) override;

		/**
		 * Binds this texture.
		 * @see GLESAttribute::bindAttribute().
		 */
		void bindAttribute(const GLESFramebuffer& framebuffer, GLESShaderProgram& shaderProgram) override;

		/**
		 * Unbindes this attribute.
		 * @see GLESAttribute::unbindAttribute().
		 */
		void unbindAttribute() override;

		/**
		 * Returns the shader type necessary to render an object with this attribute.
		 * @see GLESAttribute::necessaryShader().
		 */
		ProgramType necessaryShader() const override;

	protected:

		/**
		 * Creates a new GLESceneGraph textures object.
		 */
		GLESTextures();

		/**
		 * Destructs a GLESceneGraph textures objets.
		 */
		~GLESTextures() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_TEXTURES_H
