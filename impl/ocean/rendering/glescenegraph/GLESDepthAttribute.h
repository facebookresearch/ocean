/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_DEPTH_ATTRIBUTE_H
#define META_OCEAN_RENDERING_GLES_DEPTH_ATTRIBUTE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESAttribute.h"

#include "ocean/rendering/DepthAttribute.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph depth attribute.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESDepthAttribute :
	virtual public GLESAttribute,
	virtual public DepthAttribute
{
	friend class GLESFactory;

	public:

		/**
		 * Returns whether the depth buffer is used for occlusion tests.
		 * @see DepthAttribute::testingEnabled().
		 */
		bool testingEnabled() const override;

		/**
		 * Returns whether the depth buffer writing is enabled.
		 * @see DepthAttribute::writingEnabled().
		 */
		bool writingEnabled() const override;

		/**
		 * Returns the depth function of this attribute.
		 * @see DepthAttribute::depthFunction().
		 */
		DepthFunction depthFunction() const override;

		/**
		 * Sets whether the depth buffer is used for occlusion tests.
		 * @see DepthAttribute::setTestingEnabled().
		 */
		void setTestingEnabled(const bool state) override;

		/**
		 * Sets whether the depth buffer is writable.
		 * @see DepthAttribute::setWritingEnabled().
		 */
		void setWritingEnabled(const bool state) override;

		/**
		 * Sets the depth function of this attribute.
		 * @see DepthAttribute::setDepthFunction().
		 */
		void setDepthFunction(const DepthFunction function) override;

		/**
		 * Binds this attribute.
		 * @see Attribute::bindAttribute().
		 */
		void bindAttribute(const GLESFramebuffer& framebuffer, GLESShaderProgram& shaderProgram) override;

		/**
		 * Unbindes this attribute.
		 * @see Attribute::unbindAttribute().
		 */
		void unbindAttribute() override;

	protected:

		/**
		 * Creates a new GLESceneGraph depth attribute object.
		 */
		GLESDepthAttribute();

		/**
		 * Destructs a GLESceneGraph depth attribute object.
		 */
		~GLESDepthAttribute() override;

		/**
		 * Translates a depth function to the corresponding OpenGL ES enum value.
		 * @param function Depth function to translate
		 * @return Corresponding OpenGL ES enum value
		 */
		static GLenum translateDepthFunction(const DepthFunction function);

	protected:

		/// True, to enable depth buffer testing.
		bool testingEnabled_;

		/// True, to enable depth buffer writing.
		bool writingEnabled_;

		/// Depth function value.
		DepthFunction depthFunction_;

		/// OpenGL ES depth function value.
		GLenum glesDepthFunction;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_DEPTH_ATTRIBUTE_H
