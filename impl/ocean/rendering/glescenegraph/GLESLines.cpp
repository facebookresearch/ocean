/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESLines.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESLines::GLESLines() :
	GLESIndependentPrimitive(),
	Lines()
{
	// nothing to do here
}

GLESLines::~GLESLines()
{
	release();
}

VertexIndices GLESLines::indices() const
{
	const ScopedLock scopedLock(objectLock);

	return explicitLineIndices_;
}

unsigned int GLESLines::numberIndices() const
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(explicitLineIndices_.empty() || numberImplicitLines_ == 0u);

	if (explicitLineIndices_.empty())
	{
		return numberImplicitLines_;
	}

	return (unsigned int)(explicitLineIndices_.size());
}

void GLESLines::setIndices(const VertexIndices& indices)
{
	const ScopedLock scopedLock(objectLock);

	if (indices.empty())
	{
		release();
		return;
	}

	if (vboIndices_ == 0u)
	{
		glGenBuffers(1, &vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices_);
	ocean_assert(GL_NO_ERROR == glGetError());

	explicitLineIndices_ = indices;
	numberImplicitLines_ = 0u;

	ocean_assert(GL_NO_ERROR == glGetError());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * explicitLineIndices_.size(), explicitLineIndices_.data(), GL_STATIC_DRAW);

	const GLenum error = glGetError();

	if (error == GL_OUT_OF_MEMORY)
	{
		release();

		Log::warning() << "Not enough memory on the graphic chip to create " << explicitLineIndices_.size() << " line indices.";
	}
	else
	{
		ocean_assert(GL_NO_ERROR == error);
	}

	updateBoundingBox();
}

void GLESLines::setIndices(const unsigned int numberImplicitLines)
{
	const ScopedLock scopedLock(objectLock);

	release();

	numberImplicitLines_ = numberImplicitLines;

	updateBoundingBox();
}

void GLESLines::render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights)
{
	if (explicitLineIndices_.empty() && numberImplicitLines_ == 0u)
	{
		return;
	}

	const SmartObjectRef<GLESVertexSet> glesVertexSet(vertexSet());
	if (glesVertexSet.isNull())
	{
		return;
	}

	attributeSet.bindAttributes(framebuffer, projectionMatrix, camera_T_object, camera_T_world, normalMatrix, lights, GLESAttribute::PT_POINTS);

	if (attributeSet.shaderProgram() && attributeSet.shaderProgram()->isCompiled())
	{
		glesVertexSet->bindVertexSet(attributeSet.shaderProgram()->id());

		const GLint locationColor = glGetUniformLocation(attributeSet.shaderProgram()->id(), "color");
		if (locationColor != -1)
		{
			setUniform(locationColor, RGBAColor(1.0f, 1.0f, 1.0f));
		}

		drawLines();
	}

	attributeSet.unbindAttributes();
}

void GLESLines::render(const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESShaderProgram& shaderProgram)
{
	ocean_assert(shaderProgram.isCompiled());

	if (explicitLineIndices_.empty() && numberImplicitLines_ == 0u)
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

	drawLines();
}

void GLESLines::drawLines()
{
	if (numberImplicitLines_ == 0u)
	{
		ocean_assert(vboIndices_ != 0u);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());

		glDrawElements(GL_LINES, GLsizei(explicitLineIndices_.size()), GL_UNSIGNED_INT, nullptr);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
	else
	{
		glDrawArrays(GL_LINES, 0u, numberImplicitLines_);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
}

void GLESLines::release()
{
	if (vboIndices_ != 0)
	{
		ocean_assert(GL_NO_ERROR == glGetError());
		glDeleteBuffers(1, &vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());

		vboIndices_ = 0u;
	}

	explicitLineIndices_.clear();
	numberImplicitLines_ = 0u;

	boundingBox_ = BoundingBox();
}

void GLESLines::updateBoundingBox()
{
	boundingBox_ = BoundingBox();

	if (primitiveVertexSet.isNull() || explicitLineIndices_.empty())
	{
		return;
	}

	const SmartObjectRef<GLESVertexSet> glesVertexSet(primitiveVertexSet);
	ocean_assert(glesVertexSet);

	if (explicitLineIndices_.empty())
	{
		boundingBox_ = glesVertexSet->boundingBox(numberImplicitLines_);
	}
	else
	{
		boundingBox_ = glesVertexSet->boundingBox(explicitLineIndices_);
	}
}

}

}

}
