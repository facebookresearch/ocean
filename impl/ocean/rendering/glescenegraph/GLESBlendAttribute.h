/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_BLEND_ATTRIBUTE_H
#define META_OCEAN_RENDERING_GLES_BLEND_ATTRIBUTE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESAttribute.h"

#include "ocean/rendering/BlendAttribute.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph blend attribute.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESBlendAttribute :
	virtual public GLESAttribute,
	virtual public BlendAttribute
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the blending function for the source object.
		 * @see BlendAttribute::sourceFunction().
		 */
		BlendingFunction sourceFunction() const override;

		/**
		 * Returns the blending function for the destination object.
		 * @see BlendAttribute::destinationFunction().
		 */
		BlendingFunction destinationFunction() const override;

		/**
		 * Sets the blending function for the source object.
		 * @see BlendAttribute::setSourceFunction().
		 */
		void setSourceFunction(const BlendingFunction function) override;

		/**
		 * Sets the blending function for the destination object.
		 * @see BlendAttribute::setDestinationFunction().
		 */
		void setDestinationFunction(const BlendingFunction function) override;

		/**
		 * Binds this attribute.
		 */
		void bindAttribute(const GLESFramebuffer& framebuffer, GLESShaderProgram& shaderProgram) override;

		/**
		 * Unbindes this attribute.
		 */
		void unbindAttribute() override;

	protected:

		/**
		 * Creates a new GLESceneGraph blend attribute.
		 */
		GLESBlendAttribute();

		/**
		 * Destructs a GLESceneGraph blend attribute.
		 */
		~GLESBlendAttribute() override;

		/**
		 * Translates a source blending function to the corresponding OpenGL ES enum value.
		 * @param function Source blending function to translate
		 * @return Corresponding OpenGL ES enum value
		 */
		static GLenum translateSourceBlendingFunction(const BlendingFunction function);

		/**
		 * Translates a destination blending function to the corresponding OpenGL ES enum value.
		 * @param function Destination blending function to translate
		 * @return Corresponding OpenGL ES enum value
		 */
		static GLenum translateDestinationBlendingFunction(const BlendingFunction function);

	protected:

		/// Selected source blending function of this blend attribute.
		BlendingFunction attributeSourceBlendingFunction;

		/// Selected destination blending function of this blend attribute.
		BlendingFunction attributeDestinationBlendingFunction;

		/// OpenGL ES source blending value.
		GLenum attributeGLESSourceBlendingFunction;

		/// OpenGL ES destination blending value.
		GLenum attributeGLESDestinationBlendingFunction;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_BLEND_ATTRIBUTE_H
