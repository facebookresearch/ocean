/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTriangles.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESTriangles::GLESTriangles() :
	GLESIndependentPrimitive(),
	Triangles()
{
	// nothing to do here
}

GLESTriangles::~GLESTriangles()
{
	release();
}

TriangleFaces GLESTriangles::faces() const
{
	throw NotSupportedException("OpenGL ES does not support reading of vertex buffer objects.");
}

unsigned int GLESTriangles::numberFaces() const
{
	ocean_assert(numberImplicitTriangleFaces_ == 0u || explicitTriangleFaces_.empty());

	if (numberImplicitTriangleFaces_ == 0u)
	{
		return (unsigned int)(explicitTriangleFaces_.size());
	}
	else
	{
		return numberImplicitTriangleFaces_;
	}
}

void GLESTriangles::setFaces(const TriangleFaces& faces)
{
	if (faces.empty())
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

	static_assert(sizeof(TriangleFace) == sizeof(unsigned int) * 3, "Invalid data type!");

	explicitTriangleFaces_ = faces;
	numberImplicitTriangleFaces_ = 0u;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * GLsizeiptr(explicitTriangleFaces_.size() * 3), faces.data(), GL_STATIC_DRAW);

	const GLenum error = glGetError();

	if (error == GL_OUT_OF_MEMORY)
	{
		release();

		Log::warning() << "Not enough memory on the graphic chip to create " << explicitTriangleFaces_.size() * 3 << " triangle indices.";
	}
	else
	{
		ocean_assert(GL_NO_ERROR == error);
	}

	updateBoundingBox();
}

void GLESTriangles::setFaces(const unsigned int numberImplicitTriangleFaces)
{
	release();

	numberImplicitTriangleFaces_ = numberImplicitTriangleFaces;

	updateBoundingBox();
}

void GLESTriangles::render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights)
{
	if (explicitTriangleFaces_.empty() && numberImplicitTriangleFaces_ == 0u)
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

		drawTriangles();
	}

	attributeSet.unbindAttributes();
}

void GLESTriangles::render(const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESShaderProgram& shaderProgram)
{
	ocean_assert(shaderProgram.isCompiled());

	if (explicitTriangleFaces_.empty() && numberImplicitTriangleFaces_ == 0u)
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

	drawTriangles();
}

void GLESTriangles::drawTriangles()
{
	if (numberImplicitTriangleFaces_ == 0u)
	{
		ocean_assert(vboIndices_ != 0u);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());

		glDrawElements(GL_TRIANGLES, GLsizei(explicitTriangleFaces_.size() * 3), GL_UNSIGNED_INT, nullptr);
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0u, numberImplicitTriangleFaces_ * 3u);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
}

void GLESTriangles::drawTriangles(const unsigned int firstTriangle, const unsigned int numberTriangles)
{
	if (numberTriangles == 0u)
	{
		return;
	}

	if (numberImplicitTriangleFaces_ == 0u)
	{
		ocean_assert((firstTriangle + numberTriangles) * 3u <= (unsigned int)(explicitTriangleFaces_.size()) * 3u);
		ocean_assert(vboIndices_ != 0u);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());

		glDrawElements(GL_TRIANGLES, numberTriangles * 3u, GL_UNSIGNED_INT, (void*)(firstTriangle * 3u * sizeof(unsigned int)));
		ocean_assert(GL_NO_ERROR == glGetError());

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0u);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
	else
	{
		ocean_assert(firstTriangle + numberTriangles <= numberImplicitTriangleFaces_);

		glDrawArrays(GL_TRIANGLES, firstTriangle * 3u, numberTriangles * 3u);
		ocean_assert(GL_NO_ERROR == glGetError());
	}
}

void GLESTriangles::release()
{
	if (vboIndices_ != 0)
	{
		glDeleteBuffers(1, &vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());

		vboIndices_ = 0u;
	}

	explicitTriangleFaces_.clear();
	numberImplicitTriangleFaces_ = 0u;

	boundingBox_ = BoundingBox();
}

void GLESTriangles::updateBoundingBox()
{
	boundingBox_ = BoundingBox();

	if (primitiveVertexSet.isNull() || explicitTriangleFaces_.empty())
	{
		return;
	}

	const SmartObjectRef<GLESVertexSet> glesVertexSet(primitiveVertexSet);
	ocean_assert(glesVertexSet);

	if (explicitTriangleFaces_.empty())
	{
		boundingBox_ = glesVertexSet->boundingBox(numberImplicitTriangleFaces_ * 3u);
	}
	else
	{
		boundingBox_ = glesVertexSet->boundingBox(explicitTriangleFaces_);
	}
}

}

}

}
