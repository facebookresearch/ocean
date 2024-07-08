/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTraverser.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

void GLESTraverser::render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_world)
{
	// first, we render all objects which apply a special depth handling

	for (const TraverserObject& traverserObject : depthTraverserObjects_)
	{
		traverserObject.render(framebuffer, projection, camera_T_world);
	}

	// now, we render all default objects

	for (const TraverserObject& traverserObject : defaultTraverserObjects_)
	{
		traverserObject.render(framebuffer, projection, camera_T_world);
	}

	// finally, we render all transparent objects
	// sorting objets based on their distance to camera, renderables with largest distance first

	std::sort(blendTraverserObjects_.rbegin(), blendTraverserObjects_.rend(), TraverserObject::compareDistance);

	for (const TraverserObject& traverserObject : blendTraverserObjects_)
	{
		traverserObject.render(framebuffer, projection, camera_T_world);
	}
}

void GLESTraverser::renderColorIds(const Engine& engine, const SquareMatrix4& projection, const HomogenousMatrix4& camera_T_world)
{
	if (shaderProgramColorId_.isNull())
	{
		shaderProgramColorId_ = GLESProgramManager::get().program(engine, GLESAttribute::PT_COLOR_ID);

		if (shaderProgramColorId_.isNull())
		{
			ocean_assert(false && "This should never happen!");
			return;
		}
	}

	glUseProgram(shaderProgramColorId_->id());
	ocean_assert(GL_NO_ERROR == glGetError());

	const GLint colorIdLocation = glGetUniformLocation(shaderProgramColorId_->id(), "colorId");
	ocean_assert(colorIdLocation != -1);

	uint32_t colorId = 0u;

	// first, we render all objects which apply a special depth handling

	for (const TraverserObject& traverserObject : depthTraverserObjects_)
	{
		GLESObject::setUniform(colorIdLocation, ++colorId);

		traverserObject.render(projection, camera_T_world, *shaderProgramColorId_);
	}

	// now, we render all default objects

	for (const TraverserObject& traverserObject : defaultTraverserObjects_)
	{
		GLESObject::setUniform(colorIdLocation, ++colorId);

		traverserObject.render(projection, camera_T_world, *shaderProgramColorId_);
	}

	// finally, we render all transparent objects
	// sorting objects based on their distance to camera, renderables with largest distance first

	std::sort(blendTraverserObjects_.rbegin(), blendTraverserObjects_.rend(), TraverserObject::compareDistance);

	for (const TraverserObject& traverserObject : blendTraverserObjects_)
	{
		GLESObject::setUniform(colorIdLocation, ++colorId);

		traverserObject.render(projection, camera_T_world, *shaderProgramColorId_);
	}
}

void GLESTraverser::addRenderable(const RenderableRef& renderable, const AttributeSetRef& attributeSet, const HomogenousMatrix4& camera_T_object, const SquareMatrix3& normalMatrix, const Lights& lights)
{
	ocean_assert(camera_T_object.isValid());
	ocean_assert(!normalMatrix.isSingular());

	if (renderable.isNull() || attributeSet.isNull())
	{
		ocean_assert(false && "Invalid input!");
		return;
	}

	if (attributeSet)
	{
		 const GLESAttributeSet& glesAttributeSet = attributeSet.force<GLESAttributeSet>();

		 if (glesAttributeSet.containsAttribute(Object::TYPE_BLEND_ATTRIBUTE))
		 {
			blendTraverserObjects_.emplace_back(renderable, attributeSet, camera_T_object, normalMatrix, lights);
			return;
		 }
		 else if (glesAttributeSet.containsAttribute(Object::TYPE_DEPTH_ATTRIBUTE))
		 {
			depthTraverserObjects_.emplace_back(renderable, attributeSet, camera_T_object, normalMatrix, lights);
			return;
		 }
	}

	defaultTraverserObjects_.emplace_back(renderable, attributeSet, camera_T_object, normalMatrix, lights);
}

RenderableRef GLESTraverser::renderableFromColorId(const uint32_t colorId) const
{
	ocean_assert(shaderProgramColorId_);

	if (colorId == 0u)
	{
		return RenderableRef();
	}

	uint32_t adjustedColorId = colorId - 1u;

	if (size_t(adjustedColorId) < depthTraverserObjects_.size())
	{
		return depthTraverserObjects_[adjustedColorId].renderable();
	}

	adjustedColorId -= uint32_t(depthTraverserObjects_.size());

	if (adjustedColorId < defaultTraverserObjects_.size())
	{
		return defaultTraverserObjects_[adjustedColorId].renderable();
	}

	adjustedColorId -= uint32_t(defaultTraverserObjects_.size());

	if (size_t(adjustedColorId) >= blendTraverserObjects_.size())
	{
		ocean_assert(false && "This must never happen, renderables out of sync!");

		return RenderableRef();
	}

	return blendTraverserObjects_[adjustedColorId].renderable();
}

void GLESTraverser::clear()
{
	depthTraverserObjects_.clear();
	defaultTraverserObjects_.clear();
	blendTraverserObjects_.clear();
}

}

}

}
