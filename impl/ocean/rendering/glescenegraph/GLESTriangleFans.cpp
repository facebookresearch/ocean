/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTriangleFans.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESTriangleFans::GLESTriangleFans() :
	GLESStripPrimitive(),
	TriangleFans()
{
	// nothing to do here
}

GLESTriangleFans::~GLESTriangleFans()
{
	release();
}

VertexIndexGroups GLESTriangleFans::strips() const
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(vertexBufferPairs_.size() == strips_.size());

	return strips_;
}

unsigned int GLESTriangleFans::numberStrips() const
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(vertexBufferPairs_.size() == strips_.size());

	return (unsigned int)(strips_.size());
}

void GLESTriangleFans::setStrips(const VertexIndexGroups& strips)
{
	const ScopedLock scopedLock(objectLock);

	if (strips.empty() || strips.front().empty())
	{
		release();
		return;
	}

	ocean_assert(GL_NO_ERROR == glGetError());

	strips_ = strips;
	vertexBufferPairs_.reserve(strips_.size());

	for (const Indices32& strip : strips_)
	{
		GLuint vbo = 0;
		glGenBuffers(1, &vbo);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(sizeof(GLuint) * strip.size()), strip.data(), GL_STATIC_DRAW);
		ocean_assert(GL_NO_ERROR == glGetError());

		vertexBufferPairs_.emplace_back(vbo, (unsigned int)(strip.size()));
	}

	updateBoundingBox();
}

void GLESTriangleFans::render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights)
{
	if (vertexBufferPairs_.empty())
	{
		return;
	}

	const SmartObjectRef<GLESVertexSet> glesVertexSet(vertexSet());
	if (glesVertexSet.isNull())
	{
		return;
	}

	attributeSet.bindAttributes(framebuffer, projectionMatrix, camera_T_object, camera_T_world, normalMatrix, lights);

	if (attributeSet.shaderProgram() && attributeSet.shaderProgram()->isCompiled())
	{
		glesVertexSet->bindVertexSet(attributeSet.shaderProgram()->id());

		for (VertexBufferPairs::const_iterator i = vertexBufferPairs_.cbegin(); i != vertexBufferPairs_.cend(); ++i)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i->first);
			ocean_assert(GL_NO_ERROR == glGetError());

			glDrawElements(GL_TRIANGLE_FAN, i->second, GL_UNSIGNED_INT, nullptr);
			ocean_assert(GL_NO_ERROR == glGetError());
		}
	}

	attributeSet.unbindAttributes();
}

void GLESTriangleFans::render(const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESShaderProgram& shaderProgram)
{
	ocean_assert(shaderProgram.isCompiled());

	if (vertexBufferPairs_.empty())
	{
		return;
	}

	const SmartObjectRef<GLESVertexSet> glesVertexSet(vertexSet());
	if (glesVertexSet.isNull())
	{
		return;
	}

	shaderProgram.bind(projectionMatrix, camera_T_object, camera_T_world, normalMatrix);

	glesVertexSet->bindVertexSet(shaderProgram.id());

	for (VertexBufferPairs::const_iterator i = vertexBufferPairs_.cbegin(); i != vertexBufferPairs_.cend(); ++i)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i->first);
		ocean_assert(GL_NO_ERROR == glGetError());

		glDrawElements(GL_TRIANGLE_FAN, i->second, GL_UNSIGNED_INT, nullptr);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
}

void GLESTriangleFans::release()
{
	for (VertexBufferPairs::const_iterator i = vertexBufferPairs_.begin(); i != vertexBufferPairs_.end(); ++i)
	{
		glDeleteBuffers(1, &i->first);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	strips_.clear();

	boundingBox_ = BoundingBox();
}

void GLESTriangleFans::updateBoundingBox()
{
	boundingBox_ = BoundingBox();

	if (primitiveVertexSet.isNull() || strips_.empty())
	{
		return;
	}

	const SmartObjectRef<GLESVertexSet> glesVertexSet(primitiveVertexSet);
	ocean_assert(glesVertexSet);

	boundingBox_ = glesVertexSet->boundingBox(strips_);
}

}

}

}
