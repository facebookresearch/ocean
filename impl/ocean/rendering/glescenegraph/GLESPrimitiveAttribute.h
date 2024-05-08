/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_PRIMITIVE_ATTRIBUTE_H
#define META_OCEAN_RENDERING_GLES_PRIMITIVE_ATTRIBUTE_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESAttribute.h"

#include "ocean/rendering/PrimitiveAttribute.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph primitive attribute object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESPrimitiveAttribute :
	virtual public GLESAttribute,
	virtual public PrimitiveAttribute
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the face mode of this attribute.
		 * @see PrimitiveAttribute::faceMode().
		 */
		FaceMode faceMode() const override;

		/**
		 * Returns the culling mode of this attribute.
		 * @see PrimitiveAttribute::cullingMode().
		 */
		CullingMode cullingMode() const override;

		/**
		 * Returns the lighting mode of this attribute.
		 * @see PrimitiveAttribute::lightingMode().
		 */
		LightingMode lightingMode() const override;

		/**
		 * Sets the face mode for this attribute.
		 * @see PrimitiveAttribute::setFaceMode().
		 */
		void setFaceMode(const FaceMode mode) override;

		/**
		 * Sets the culling mode for this attribute.
		 * @see PrimitiveAttribute::setCullingMode().
		 */
		void setCullingMode(const CullingMode culling) override;

		/**
		 * Sets the lighting mode for this attribute.
		 * @see PrimitiveAttribute::setLightingMode().
		 */
		void setLightingMode(const LightingMode lighting) override;

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
		 * Creates a new GLESceneGraph primitive attribute.
		 */
		GLESPrimitiveAttribute();

		/**
		 * Destructs a GLESceneGraph primitive attribute.
		 */
		~GLESPrimitiveAttribute() override;

	private:

		/// The face mode of this attribute.
		FaceMode faceMode_;

		/// Culling mode of this attribute.
		CullingMode cullingMode_;

		/// OpenGL ES culling mode.
		GLenum glesCullingMode_;

		/// Two sided lighting state.
		LightingMode lightingMode_;

		/// True, if a parameter unbind is necessary.
		bool unbindNecessary_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_PRIMITIVE_ATTRIBUTE_H
