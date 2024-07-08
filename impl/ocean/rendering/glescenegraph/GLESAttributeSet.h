/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_ATTRIBUTE_SET_H
#define META_OCEAN_RENDERING_GLES_ATTRIBUTE_SET_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESLightSource.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"
#include "ocean/rendering/glescenegraph/GLESTextures.h"
#include "ocean/rendering/glescenegraph/GLESProgramManager.h"

#include "ocean/rendering/AttributeSet.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph attribute set object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESAttributeSet :
	virtual public GLESObject,
	virtual public AttributeSet
{
	friend class GLESFactory;
	friend class GLESPhantomAttribute;

	protected:

		/**
		 * Definition of an unordered map mapping attribute types to counters.
		 */
#if defined(OCEAN_GCC_VERSION) && OCEAN_GCC_VERSION > 0 && OCEAN_GCC_VERSION < 50000
		typedef std::unordered_map<int, unsigned int> AttributeCounterMap; // workaround due to compiler bug
#else
		typedef std::unordered_map<ObjectType, unsigned int> AttributeCounterMap;
#endif

	public:

		/**
		 * Adds a new attribute to this attribute set.
		 * @see AttributeSet::addAttribute().
		 */
		void addAttribute(const AttributeRef& attribute) override;

		/**
		 * Removes a registered attribute from this attribute ses.
		 * @see AttributeSet::removeAttribute().
		 */
		void removeAttribute(const AttributeRef& attribute) override;

		/**
		 * Binds the attributes of this set to the internal OpenGL ES shader program as uniforms.
		 * @param framebuffer Framebuffer initiated the rendering process
		 * @param projection The projection matrix used for this node
		 * @param camera_T_model The transformation between model and camera (aka Modelview matrix), must be valid
		 * @param camera_T_world The transformation between world and camera, must be valid
		 * @param normalMatrix Normal transformation matrix with is the transposed inverse of the upper 3x3 model view matrix
		 * @param lights The lights used to render the scene
		 * @param additionalProgramTypes Optional additional program type(s) which is not reflected in this attribute set automatically
		 * @param additionalAttribute Optional additional attribute which is not part of this attribute set
		 */
		void bindAttributes(const GLESFramebuffer& framebuffer, const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, const Lights& lights, const GLESAttribute::ProgramType additionalProgramTypes = GLESAttribute::PT_UNKNOWN, GLESAttribute* const additionalAttribute = nullptr);

		/**
		 * Unbinds all attributes of this set.
		 */
		void unbindAttributes();

		/**
		 * Returns the shader of this attribute set.
		 * @return Shader
		 */
		inline GLESShaderProgramRef shaderProgram() const;

		/**
		 * Returns whether this attribute set contains at least one attribute of a specific type.
		 * @param attributeType The attribute type to check
		 * @return True, if so
		 */
		inline bool containsAttribute(const ObjectType attributeType) const;

		/**
		 * Resets the shader program e.g., because child element has changed after the shader has been selected.
		 */
		inline void resetShaderProgram();

	protected:

		/**
		 * Creates a new GLESceneGraph attribute set object.
		 */
		GLESAttributeSet();

		/**
		 * Destructs a GLESceneGraph attribute set object.
		 */
		~GLESAttributeSet() override;

		/**
		 * Determines which shader program to use for this entire attribute set
		 * @param lights The lights used to render the scene
		 * @param additionalProgramTypes Optional additional program type(s) which is not reflected in this attribute set automatically
		 * @param additionalAttribute Optional additional attribute which is not part of this attribute set
		 * @return Shader program type
		 */
		GLESAttribute::ProgramType determineShaderType(const Lights& lights, const GLESAttribute::ProgramType additionalProgramTypes = GLESAttribute::PT_UNKNOWN, const GLESAttribute* additionalAttribute = nullptr) const;

	protected:

		/// Shader program specific for this attribute set.
		GLESShaderProgramRef shaderProgram_;

		/// Shader type necessary for this attribute set.
		GLESAttribute::ProgramType shaderProgramType_ = GLESAttribute::PT_UNKNOWN;

		/// True, if the shader has been changed since the last usage.
		bool shaderProgramTypeChanged_ = true;

		/// The map counting the number of attributes;
		AttributeCounterMap attributeCounterMap_;

		/// True, if the current shader program was created for a light source.
		bool shaderProgramTypeIsBasedOnLight_ = false;
};

inline GLESShaderProgramRef GLESAttributeSet::shaderProgram() const
{
	const ScopedLock scopedLock(objectLock);

	return shaderProgram_;
}

inline bool GLESAttributeSet::containsAttribute(const ObjectType attributeType) const
{
	const ScopedLock scopedLock(objectLock);

	return attributeCounterMap_.find(attributeType) != attributeCounterMap_.cend();
}

inline void GLESAttributeSet::resetShaderProgram()
{
	const ScopedLock scopedLock(objectLock);

	shaderProgramTypeChanged_ = true;
}

}

}

}

#endif // META_OCEAN_RENDERING_GLES_ATTRIBUTE_SET_H
