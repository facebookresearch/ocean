/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_STEREO_ATTRIBUTE_H
#define META_OCEAN_RENDERING_GLES_STEREO_ATTRIBUTE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESAttribute.h"

#include "ocean/rendering/StereoAttribute.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph stereo attribute.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESStereoAttribute :
	virtual public GLESAttribute,
	virtual public StereoAttribute
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the stereo visibility property.
		 * @see StereoAttribute::stereoVisibility().
		 */
		StereoVisibility stereoVisibility() const override;

		/**
		 * Sets the stereo visibility property.
		 * @see StereoAttribute::setStereoVisibility().
		 */
		void setStereoVisibility(const StereoVisibility stereoVisibility) override;

		/**
		 * Binds this attribute.
		 * @see GLESAttribute::bindAttribute().
		 */
		void bindAttribute(const GLESFramebuffer& framebuffer, GLESShaderProgram& shaderProgram) override;

		/**
		 * Unbindes this attribute.
		 * @see GLESAttribute::unbindAttribute().
		 */
		void unbindAttribute() override;

	protected:

		/**
		 * Creates a new GLESceneGraph stereo attribute.
		 */
		GLESStereoAttribute();

		/**
		 * Destructs a GLESceneGraph stereo attribute.
		 */
		~GLESStereoAttribute() override;

	protected:

		/// The stereo visibility property of this attribute.
		StereoVisibility stereoVisibility_;

		/// The previous cull face value, will be used to restore the previous state when unbinding.
		GLboolean previousCullFaceValue_;

		/// The previous cull face mode, will be used to restore the previous state when unbinding.
		GLint previousCullFaceMode_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_STEREO_ATTRIBUTE_H
