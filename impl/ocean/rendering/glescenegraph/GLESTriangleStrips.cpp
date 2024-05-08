/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESTriangleStrips.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESTriangleStrips::GLESTriangleStrips() :
	GLESObject(),
	GLESPrimitive(),
	TriangleStrips()
{
	// nothing to do here
}

GLESTriangleStrips::~GLESTriangleStrips()
{
	release();
}

VertexIndexGroups GLESTriangleStrips::strips() const
{
	const ScopedLock scopedLock(objectLock);

	return strips_;
}

unsigned int GLESTriangleStrips::numberStrips() const
{
	const ScopedLock scopedLock(objectLock);

	return (unsigned int)(strips_.size());
}

void GLESTriangleStrips::setStrips(const VertexIndexGroups& strips)
{
	const ScopedLock scopedLock(objectLock);

#ifdef OCEAN_DEBUG
	for (const Indices32& strip : strips_)
	{
		ocean_assert(!strip.empty());
	}
#endif

	if (strips.empty() || strips.front().empty())
	{
		release();
		return;
	}

	ocean_assert(GL_NO_ERROR == glGetError());

	if (vboIndices_ == 0u)
	{
		glGenBuffers(1, &vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices_);
	ocean_assert(GL_NO_ERROR == glGetError());

	numberIndices_ = 0u;
	strips_ = strips;

	for (const Indices32& strip : strips_)
	{
		numberIndices_ += (unsigned int)(strip.size());
	}

	numberIndices_ += (unsigned int)((strips_.size() * 2) - 2);

	Indices32 indices;
	indices.reserve(numberIndices_);

	for (const Indices32& strip : strips_)
	{
		if (!indices.empty())
		{
			// create degenerated triangles to switch to next strip

			indices.emplace_back(indices.back());
			indices.emplace_back(Index32(strip.front()));
		}

		for (const Index32& index : strip)
		{
			indices.emplace_back(index);
		}
	}

	ocean_assert(numberIndices_ == (unsigned int)(indices.size()));

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberIndices_, indices.data(), GL_STATIC_DRAW);

	const GLenum error = glGetError();

	if (error == GL_OUT_OF_MEMORY)
	{
		release();

		Log::warning() << "Not enough memory on the graphic chip to create " << numberIndices_ << " triangle strip indices.";
	}
	else
	{
		ocean_assert(GL_NO_ERROR == error);
	}

	updateBoundingBox();
}

void GLESTriangleStrips::render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights)
{
	if (vboIndices_ == 0u)
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

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());

		glDrawElements(GL_TRIANGLE_STRIP, numberIndices_, GL_UNSIGNED_INT, nullptr);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	attributeSet.unbindAttributes();
}

void GLESTriangleStrips::render(const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESShaderProgram& programShader)
{
	ocean_assert(programShader.isCompiled());

	if (vboIndices_ == 0u)
	{
		return;
	}

	const SmartObjectRef<GLESVertexSet> glesVertexSet(vertexSet());
	if (glesVertexSet.isNull())
	{
		return;
	}

	programShader.bind(projectionMatrix, camera_T_object, camera_T_world, normalMatrix);

	glesVertexSet->bindVertexSet(programShader.id());

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices_);
	ocean_assert(GL_NO_ERROR == glGetError());

	glDrawElements(GL_TRIANGLE_STRIP, numberIndices_, GL_UNSIGNED_INT, nullptr);
	ocean_assert(GL_NO_ERROR == glGetError());
}

void GLESTriangleStrips::release()
{
	if (vboIndices_ != 0u)
	{
		glDeleteBuffers(1, &vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());

		vboIndices_ = 0u;
	}

	strips_.clear();
	numberIndices_ = 0u;

	boundingBox_ = BoundingBox();
}

void GLESTriangleStrips::updateBoundingBox()
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
