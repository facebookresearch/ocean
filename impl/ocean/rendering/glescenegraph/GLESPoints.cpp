/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESPoints.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESPoints::GLESPoints() :
	GLESIndependentPrimitive(),
	Points()
{
	// nothing to do here
}

GLESPoints::~GLESPoints()
{
	release();
}

VertexIndices GLESPoints::indices() const
{
	const ScopedLock scopedLock(objectLock);

	return explicitPointIndices_;
}

unsigned int GLESPoints::numberIndices() const
{
	const ScopedLock scopedLock(objectLock);

	ocean_assert(explicitPointIndices_.empty() || numberImplicitPoints_ == 0u);

	if (explicitPointIndices_.empty())
	{
		return numberImplicitPoints_;
	}

	return (unsigned int)(explicitPointIndices_.size());
}

Scalar GLESPoints::pointSize() const
{
	const ScopedLock scopedLock(objectLock);

	return pointSize_;
}

void GLESPoints::setIndices(const VertexIndices& indices)
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

	explicitPointIndices_ = indices;
	numberImplicitPoints_ = 0u;

	ocean_assert(GL_NO_ERROR == glGetError());
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * explicitPointIndices_.size(), explicitPointIndices_.data(), GL_STATIC_DRAW);

	const GLenum error = glGetError();

	if (error == GL_OUT_OF_MEMORY)
	{
		release();

		Log::warning() << "Not enough memory on the graphic chip to create " << explicitPointIndices_.size() << " point indices.";
	}
	else
	{
		ocean_assert(GL_NO_ERROR == error);
	}

	updateBoundingBox();
}

void GLESPoints::setIndices(const unsigned int numberImplicitPoints)
{
	const ScopedLock scopedLock(objectLock);

	release();

	numberImplicitPoints_ = numberImplicitPoints;

	updateBoundingBox();
}

void GLESPoints::setPointSize(const Scalar pointSize)
{
	ocean_assert(pointSize >= 1);

	const ScopedLock scopedLock(objectLock);

	pointSize_ = pointSize;
}

void GLESPoints::render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights)
{
	if (explicitPointIndices_.empty() && numberImplicitPoints_ == 0u)
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

		const GLint locationPointSize = glGetUniformLocation(attributeSet.shaderProgram()->id(), "pointSize");
		if (locationPointSize != -1)
		{
			setUniform(locationPointSize, pointSize_);
		}

		const GLint locationColor = glGetUniformLocation(attributeSet.shaderProgram()->id(), "color");
		if (locationColor != -1)
		{
			setUniform(locationColor, RGBAColor(1.0f, 1.0f, 1.0f));
		}

		drawPoints();
	}

	attributeSet.unbindAttributes();
}

void GLESPoints::render(const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESShaderProgram& shaderProgram)
{
	ocean_assert(shaderProgram.isCompiled());

	if (explicitPointIndices_.empty() && numberImplicitPoints_ == 0u)
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

	const GLint locationPointSize = glGetUniformLocation(shaderProgram.id(), "pointSize");
	if (locationPointSize != -1)
	{
		setUniform(locationPointSize, pointSize_);
	}

	drawPoints();
}

void GLESPoints::drawPoints()
{
#ifndef OCEAN_RENDERING_GLES_USE_ES
	GLboolean programPointSizeWasEnabled = glIsEnabled(GL_PROGRAM_POINT_SIZE);
	ocean_assert(GL_NO_ERROR == glGetError());

	glEnable(GL_PROGRAM_POINT_SIZE);
	ocean_assert(GL_NO_ERROR == glGetError());
#endif

	if (numberImplicitPoints_ == 0u)
	{
		ocean_assert(vboIndices_ != 0u);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());

		glDrawElements(GL_POINTS, GLsizei(explicitPointIndices_.size()), GL_UNSIGNED_INT, nullptr);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
	else
	{
		glDrawArrays(GL_POINTS, 0u, numberImplicitPoints_);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

#ifndef OCEAN_RENDERING_GLES_USE_ES
	if (programPointSizeWasEnabled == GL_FALSE)
	{
		glDisable(GL_PROGRAM_POINT_SIZE);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
#endif
}

void GLESPoints::release()
{
	if (vboIndices_ != 0)
	{
		ocean_assert(GL_NO_ERROR == glGetError());
		glDeleteBuffers(1, &vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());

		vboIndices_ = 0u;
	}

	explicitPointIndices_.clear();
	numberImplicitPoints_ = 0u;

	boundingBox_ = BoundingBox();
}

void GLESPoints::updateBoundingBox()
{
	boundingBox_ = BoundingBox();

	if (primitiveVertexSet.isNull() || explicitPointIndices_.empty())
	{
		return;
	}

	const SmartObjectRef<GLESVertexSet> glesVertexSet(primitiveVertexSet);
	ocean_assert(glesVertexSet);

	if (explicitPointIndices_.empty())
	{
		boundingBox_ = glesVertexSet->boundingBox(numberImplicitPoints_);
	}
	else
	{
		boundingBox_ = glesVertexSet->boundingBox(explicitPointIndices_);
	}
}

}

}

}
