/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESLineStrips.h"
#include "ocean/rendering/glescenegraph/GLESAttributeSet.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESLineStrips::GLESLineStrips() :
	GLESObject(),
	GLESPrimitive(),
	LineStrips()
{
	// nothing to do here
}

GLESLineStrips::~GLESLineStrips()
{
	release();
}

VertexIndexGroups GLESLineStrips::strips() const
{
	const ScopedLock scopedLock(objectLock);

	return strips_;
}

unsigned int GLESLineStrips::numberStrips() const
{
	const ScopedLock scopedLock(objectLock);

	return (unsigned int)(strips_.size());
}

void GLESLineStrips::setStrips(const VertexIndexGroups& strips)
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

	strips_ = strips;

	unsigned int numberIndices = 0u;

	for (const Indices32& strip : strips_)
	{
		numberIndices += (unsigned int)(strip.size());
	}

	Indices32 indices;
	indices.reserve(numberIndices);

	for (const Indices32& strip : strips_)
	{
		indices.insert(indices.cend(), strip.cbegin(), strip.cend());
	}

	ocean_assert(numberIndices == (unsigned int)(indices.size()));

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * numberIndices, indices.data(), GL_STATIC_DRAW);

	const GLenum error = glGetError();

	if (error == GL_OUT_OF_MEMORY)
	{
		release();

		Log::warning() << "Not enough memory on the graphic chip to create " << numberIndices << " triangle strip indices.";
	}
	else
	{
		ocean_assert(GL_NO_ERROR == error);
	}

	updateBoundingBox();
}

void GLESLineStrips::render(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESAttributeSet& attributeSet, const Lights& lights)
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

	attributeSet.bindAttributes(framebuffer, projectionMatrix, camera_T_object, camera_T_world, normalMatrix, lights, GLESAttribute::PT_POINTS);

	if (attributeSet.shaderProgram() && attributeSet.shaderProgram()->isCompiled())
	{
		glesVertexSet->bindVertexSet(attributeSet.shaderProgram()->id());

		const GLint locationColor = glGetUniformLocation(attributeSet.shaderProgram()->id(), "color");
		if (locationColor != -1)
		{
			setUniform(locationColor, RGBAColor(1.0f, 1.0f, 1.0f));
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());

		static_assert(sizeof(unsigned int) == 4, "Invalid data type!");

		size_t offset = 0;

		for (const Indices32& strip : strips_)
		{
			const GLsizei stripSize = GLsizei(strip.size());

			glDrawElements(GL_LINE_STRIP, stripSize, GL_UNSIGNED_INT, (const void*)(offset));
			ocean_assert(GL_NO_ERROR == glGetError());

			offset += strip.size() * sizeof(unsigned int);
		}
	}

	attributeSet.unbindAttributes();
}

void GLESLineStrips::render(const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const HomogenousMatrix4& camera_T_world, const SquareMatrix3& normalMatrix, GLESShaderProgram& programShader)
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

	static_assert(sizeof(unsigned int) == 4, "Invalid data type!");

	size_t offset = 0;

	for (const Indices32& strip : strips_)
	{
		const GLsizei stripSize = GLsizei(strip.size());

		glDrawElements(GL_LINE_STRIP, stripSize, GL_UNSIGNED_INT, (const void*)(offset));
		ocean_assert(GL_NO_ERROR == glGetError());

		offset += strip.size() * sizeof(unsigned int);
	}
}

void GLESLineStrips::release()
{
	if (vboIndices_ != 0u)
	{
		glDeleteBuffers(1, &vboIndices_);
		ocean_assert(GL_NO_ERROR == glGetError());

		vboIndices_ = 0u;
	}

	strips_.clear();

	boundingBox_ = BoundingBox();
}

void GLESLineStrips::updateBoundingBox()
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
