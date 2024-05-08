/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_TEXTURE_H
#define META_OCEAN_RENDERING_GLES_TEXTURE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"

#include "ocean/rendering/Texture.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/base/Frame.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class is the base class for all GLESceneGraph texture objects.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESTexture :
	virtual public GLESObject,
	virtual public Texture
{
	public:

		/**
		 * Returns the texture transformation matrix.
		 * @see Texture::transformation().
		 */
		HomogenousMatrix4 transformation() const override;

		/**
		 * Returns the environment mode for this texture.
		 * @see Texture::environmentMode().
		 */
		EnvironmentMode environmentMode() const override;

		/**
		 * Returns the minification filter mode for this texture.
		 * @see Texture::minificationFilterMode().
		 */
		MinFilterMode minificationFilterMode() const override;

		/**
		 * Returns the magnification filter mode for this texture.
		 * @see Texture::magnificationFilterMode().
		 */
		MagFilterMode magnificationFilterMode() const override;

		/**
		 * Returns whether mipmaps are used.
		 * @see Texture::usingMipmaps().
		 */
		bool usingMipmaps() const override;

		/**
		 * Sets the texture transformation matrix.
		 * @see Texture::setTransformation().
		 */
		void setTransformation(const HomogenousMatrix4& transformation) override;

		/**
		 * Sets the environment mode for this texture.
		 * @see Texture::setEnvironmentMode().
		 */
		void setEnvironmentMode(const EnvironmentMode mode) override;

		/**
		 * Sets the minification filter mode for this texture.
		 * @see Texture::setMinificationFilterMode().
		 */
		void setMinificationFilterMode(const MinFilterMode mode) override;

		/**
		 * Sets the magnification filter mode for this texture.
		 * @see Texture::setMagnificationFilterMode().
		 */
		void setMagnificationFilterMode(const MagFilterMode mode) override;

		/**
		 * Specifies whether mipmaps will be used.
		 * @see Texture::setUseMipmaps().
		 */
		void setUseMipmaps(const bool flag) override;

		/**
		 * Binds this texture.
		 * @param shaderProgram Shader programe to bind the texture with
		 * @param id Id of the first free texture
		 * @return Number of bound textures
		 */
		virtual unsigned int bindTexture(GLESShaderProgram& shaderProgram, const unsigned int id) = 0;

		/**
		 * Translates a minification filter mode to the corresponding OpenGL filter mode.
		 * @param minificationFilterMode The minification mode to translate
		 * @return The corresponding OpenGL filter mode
		 */
		static GLenum translateMinificationFilterMode(const MinFilterMode minificationFilterMode);

		/**
		 * Translates a maginification filter mode to the corresponding OpenGL filter mode.
		 * @param magnificationFilterMode The minification mode to translate
		 * @return The corresponding OpenGL filter mode
		 */
		static GLenum translateMagnificationFilterMode(const MagFilterMode magnificationFilterMode);

		/**
		 * Translates a wrap type to the corresponding OpenGL wrap type.
		 * @param wrapType The wrap type to translate
		 * @return The corresponding OpenGL wrap type
		 */
		static GLenum translateWrapType(const WrapType wrapType);

	protected:

		/**
		 * Creates a new GLESceneGraph texture object.
		 */
		GLESTexture();

		/**
		 * Destructs a GLESceneGraph texture object.
		 */
		~GLESTexture() override;

		/**
		 * Creates or updates the mipmap for this texture.
		 */
		virtual void createMipmap() = 0;

	protected:

		/// Homogenous texture transformation matrix.
		HomogenousMatrix4 transformation_;

		/// Texture environment mode.
		EnvironmentMode environmentMode_;

		/// Texture minification filter mode.
		MinFilterMode minificationFilterMode_;

		/// Texture magnification filter mode.
		MagFilterMode magnificationFilterMode_;

		/// True, to create a mipmap for the texture.
		bool useMipmap_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_TEXTURE_H
