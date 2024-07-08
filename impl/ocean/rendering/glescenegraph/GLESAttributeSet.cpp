/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"
#include "ocean/rendering/glescenegraph/GLESAttribute.h"
#include "ocean/rendering/glescenegraph/GLESFramebuffer.h"
#include "ocean/rendering/glescenegraph/GLESShaderProgram.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESAttributeSet::GLESAttributeSet() :
	GLESObject(),
	AttributeSet()
{
	// nothing to do here
}

GLESAttributeSet::~GLESAttributeSet()
{
	// nothing to do here
}

void GLESAttributeSet::addAttribute(const AttributeRef& attribute)
{
	if (attribute.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	AttributeSet::addAttribute(attribute);
	shaderProgramTypeChanged_ = true;

	attributeCounterMap_[attribute->type()]++;
}

void GLESAttributeSet::removeAttribute(const AttributeRef& attribute)
{
	if (attribute.isNull())
	{
		return;
	}

	const ScopedLock scopedLock(objectLock);

	AttributeSet::removeAttribute(attribute);
	shaderProgramTypeChanged_ = true;

	AttributeCounterMap::iterator i = attributeCounterMap_.find(attribute->type());

	ocean_assert(i != attributeCounterMap_.cend());
	ocean_assert(i->second >= 1u);

	if (--i->second == 0u)
	{
		attributeCounterMap_.erase(i);
	}
}

void GLESAttributeSet::bindAttributes(const GLESFramebuffer& framebuffer, const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_model, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, const Lights& lights, const GLESAttribute::ProgramType additionalProgramTypes, GLESAttribute* additionalAttribute)
{
	const ScopedLock scopedLock(objectLock);

	if (shaderProgramTypeIsBasedOnLight_ != !lights.empty())
	{
		shaderProgramTypeChanged_ = true;
	}

	if (shaderProgramTypeChanged_ || ((shaderProgramType_ & additionalProgramTypes) != additionalProgramTypes))
	{
		const GLESAttribute::ProgramType newShaderType(determineShaderType(lights, additionalProgramTypes, additionalAttribute));

		if ((newShaderType & GLESAttribute::PT_CUSTOM) == GLESAttribute::PT_CUSTOM)
		{
			shaderProgram_ = attribute(Object::TYPE_SHADER_PROGRAM);
			ocean_assert(shaderProgram_);
		}
		else
		{
			if ((newShaderType & GLESAttribute::PT_PENDING) == GLESAttribute::PT_PENDING)
			{
				if (shaderProgram_)
				{
					shaderProgram_.release();
				}

#ifdef OCEAN_DEBUG
				static unsigned int debugCounter = 0u;
				static unsigned int debugModulo = 1u;
				if ((++debugCounter % debugModulo) == 0u)
				{
					Log::warning() << "<debug> PENDING shader: " << GLESAttribute::translateProgramType(newShaderType);
					debugModulo *= 2u; // ensuring that we see the message less and less often
				}
#endif
				return;
			}

			if (newShaderType != shaderProgramType_)
			{
				shaderProgramType_ = newShaderType;
				shaderProgram_ = GLESProgramManager::get().program(engine(), shaderProgramType_);

				ocean_assert(shaderProgram_ && shaderProgram_->isCompiled());
			}
		}

		shaderProgramTypeChanged_ = false;
	}

	shaderProgramTypeIsBasedOnLight_ = !lights.empty();

	if (shaderProgram_ && shaderProgram_->isCompiled())
	{
		shaderProgram_->bind(framebuffer, projection, camera_T_model, camera_T_world, normalMatrix);

		for (Attributes::const_iterator i = setAttributes.begin(); i != setAttributes.end(); ++i)
		{
			const SmartObjectRef<GLESAttribute> glesAttribute(*i);
			ocean_assert(glesAttribute);

			glesAttribute->bindAttribute(framebuffer, *shaderProgram_);
		}

		if (additionalAttribute)
		{
			additionalAttribute->bindAttribute(framebuffer, *shaderProgram_);
		}

		if (setAttributes.empty() && additionalAttribute == nullptr)
		{
			// we have the PT_STATIC_COLOR shader

			const GLint locationColor = glGetUniformLocation(shaderProgram_->id(), "color");
			ocean_assert(locationColor != -1);

			setUniform(locationColor, RGBAColor(1.0f, 1.0f, 1.0f));
		}

		constexpr unsigned int maximalNumberLights = 8u;

		const unsigned int numberLights = std::min((unsigned int)(lights.size()), maximalNumberLights);

		const GLint locationNumberLights = glGetUniformLocation(shaderProgram_->id(), "numberLights");

		if (locationNumberLights != -1)
		{
			setUniform(locationNumberLights, GLint(numberLights));

			for (unsigned int lightIndex = 0u; lightIndex < numberLights; ++lightIndex)
			{
				ocean_assert(lightIndex < lights.size());
				ocean_assert(lights[lightIndex].first);

				lights[lightIndex].first->bindLight(*shaderProgram_, lights[lightIndex].second, camera_T_world, normalMatrix, lightIndex);
			}
		}

		// apply non-default (overwrite) framebuffer lighting model
		if (framebuffer.lightingMode() != PrimitiveAttribute::LM_DEFAULT)
		{
			const GLint locationLightingTwoSided = glGetUniformLocation(shaderProgram_->id(), "lightingTwoSided");

			if (locationLightingTwoSided != -1)
			{
				setUniform(locationLightingTwoSided, framebuffer.lightingMode() == PrimitiveAttribute::LM_TWO_SIDED_LIGHTING ? 1 : 0);
			}
		}
	}
}

void GLESAttributeSet::unbindAttributes()
{
	for (Attributes::const_reverse_iterator i = setAttributes.rbegin(); i != setAttributes.rend(); ++i)
	{
		const SmartObjectRef<GLESAttribute> glesAttribute(*i);
		ocean_assert(glesAttribute);

		glesAttribute->unbindAttribute();
	}
}

GLESAttribute::ProgramType GLESAttributeSet::determineShaderType(const Lights& lights, const GLESAttribute::ProgramType additionalProgramTypes, const GLESAttribute* additionalAttribute) const
{
	if (setAttributes.empty() && additionalAttribute == nullptr)
	{
		return GLESAttribute::PT_STATIC_COLOR;
	}

	GLESAttribute::ProgramType result = GLESAttribute::PT_UNKNOWN;

	for (Attributes::const_iterator i = setAttributes.begin(); i != setAttributes.end(); ++i)
	{
		const SmartObjectRef<GLESAttribute> glesAttribute(*i);
		ocean_assert(glesAttribute);

		result = GLESAttribute::ProgramType(result | glesAttribute->necessaryShader());
	}

	if (additionalAttribute)
	{
		result = GLESAttribute::ProgramType(result | additionalAttribute->necessaryShader());
	}

	result = GLESAttribute::ProgramType(result | additionalProgramTypes);

	if (!lights.empty())
	{
		// in case we do have a light source and if we do have a material-based shader, then we need a light-based shader as well

		if (result & GLESAttribute::PT_MATERIAL)
		{
			result = GLESAttribute::ProgramType(result | GLESAttribute::PT_LIGHT);
		}
	}

	return result;
}

}

}

}
