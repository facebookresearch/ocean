/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

#include "ocean/base/String.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESVertexSet::GLESVertexSet() :
	GLESObject(),
	VertexSet()
{
	// nothing to do here
}

GLESVertexSet::~GLESVertexSet()
{
	release();
}

Normals GLESVertexSet::normals() const
{
	throw NotSupportedException("OpenGL ES does not support reading of normals.");
}

TextureCoordinates GLESVertexSet::textureCoordinates(const unsigned int /*layerIndex*/) const
{
	throw NotSupportedException("OpenGL ES does not support reading of texture coordinates.");
}

std::string GLESVertexSet::phantomTextureCoordinateSystem() const
{
	const ScopedLock scopedLock(objectLock);

	return phantomTextureReferenceCoordinateSystem_;
}

Vertices GLESVertexSet::vertices() const
{
	const ScopedLock scopedLock(objectLock);

	return vertices_;
}

RGBAColors GLESVertexSet::colors() const
{
	throw NotSupportedException("OpenGL ES does not support reading of colors.");
}

unsigned int GLESVertexSet::numberNormals() const
{
	const ScopedLock scopedLock(objectLock);

	return bufferNormals_.numberElements();
}

unsigned int GLESVertexSet::numberTextureCoordinates(const unsigned int layerIndex) const
{
	const ScopedLock scopedLock(objectLock);

	if (layerIndex != 0)
	{
		throw NotSupportedException("Currently only one texture layer is supported.");
	}

	return bufferTextureCoordinates2D_.numberElements();
}

unsigned int GLESVertexSet::numberVertices() const
{
	const ScopedLock scopedLock(objectLock);

	return (unsigned int)(vertices_.size());
}

unsigned int GLESVertexSet::numberColors() const
{
	const ScopedLock scopedLock(objectLock);

	return bufferColors_.numberElements();
}

void GLESVertexSet::setNormals(const Normals& normals)
{
	const ScopedLock scopedLock(objectLock);

	setNormals(normals.data(), normals.size());
}

void GLESVertexSet::setNormals(const Vector3* normals, const size_t size)
{
	const ScopedLock scopedLock(objectLock);

	if (size == 0)
	{
		bufferNormals_.release();
	}
	else
	{
		bufferNormals_.setData(normals, size);
	}
}

void GLESVertexSet::setTextureCoordinates(const TextureCoordinates& textureCoordinates, const unsigned int layerIndex)
{
	const ScopedLock scopedLock(objectLock);

	if (layerIndex != 0)
	{
		throw NotSupportedException("Currently only one texture layer is supported.");
	}

	if (textureCoordinates.empty())
	{
		bufferTextureCoordinates2D_.release();
	}
	else
	{
		bufferTextureCoordinates2D_.setData(textureCoordinates.data(), textureCoordinates.size());
	}
}

void GLESVertexSet::setPhantomTextureCoordinates(const Vertices& textureCoordinates, const unsigned int layerIndex)
{
	const ScopedLock scopedLock(objectLock);

	if (layerIndex != 0)
	{
		throw NotSupportedException("Currently only one texture layer is supported.");
	}

	if (textureCoordinates.empty())
	{
		bufferTextureCoordinates3D_.release();
	}
	else
	{
		bufferTextureCoordinates3D_.setData(textureCoordinates.data(), textureCoordinates.size());
	}
}

void GLESVertexSet::setPhantomTextureCoordinateSystem(const std::string& reference)
{
	const ScopedLock scopedLock(objectLock);

	phantomTextureReferenceCoordinateSystem_ = reference;
}

void GLESVertexSet::setVertices(const Vertices& vertices)
{
	setVertices(vertices.data(), vertices.size());
}

void GLESVertexSet::setVertices(const Vector3* vertices, const size_t size)
{
	const ScopedLock scopedLock(objectLock);

	vertices_ = Vectors3(vertices, vertices + size);

	if (vertices_.empty())
	{
		bufferVertices_.release();
	}
	else
	{
		bufferVertices_.setData(vertices, size);
	}
}

void GLESVertexSet::setColors(const RGBAColors& colors)
{
	if (colors.empty())
	{
		bufferColors_.release();
	}
	else
	{
		bufferColors_.setData(colors.data(), colors.size());
	}
}

void GLESVertexSet::set(const Vertices& vertices, const Normals& normals, const TextureCoordinates& textureCoordinates, const RGBAColors& colors)
{
	setVertices(vertices);
	setNormals(normals);
	setTextureCoordinates(textureCoordinates, 0);
	setColors(colors);
}

void GLESVertexSet::bindVertexSet(const GLuint programId)
{
	ocean_assert(programId != 0u);
	ocean_assert(glIsProgram(programId));

	ocean_assert(GL_NO_ERROR == glGetError());

	if (vertexArray_ == 0u)
	{
		glGenVertexArrays(1, &vertexArray_);
		ocean_assert(GL_NO_ERROR == glGetError());
	}

	ocean_assert(vertexArray_ != 0u);
	glBindVertexArray(vertexArray_);
	ocean_assert(GL_NO_ERROR == glGetError());

	bufferVertices_.bind(programId, nullptr);
	bufferNormals_.bind(programId, nullptr);
	bufferTextureCoordinates2D_.bind(programId, nullptr);
	bufferTextureCoordinates3D_.bind(programId, nullptr);
	bufferColors_.bind(programId, nullptr);

	for (VertexBufferObjectMap::const_iterator iObject = customVertexBufferObjectMap_.cbegin(); iObject != customVertexBufferObjectMap_.cend(); ++iObject)
	{
		iObject->second->bind(programId);
	}

	const GLint locationUsePerVertexColors = glGetUniformLocation(programId, "usePerVertexColors");
	if (locationUsePerVertexColors != -1)
	{
		const GLint usePerVertexColorsValue = bufferColors_.numberElements() != 0u ? 1 : 0;

		setUniform(locationUsePerVertexColors, usePerVertexColorsValue);
	}

	ocean_assert(GL_NO_ERROR == glGetError());
}

BoundingBox GLESVertexSet::boundingBox(const VertexIndices& vertexIndices) const
{
	if (vertexIndices.empty())
	{
		return BoundingBox();
	}

	const ScopedLock scopedLock(objectLock);

	if (vertices_.empty())
	{
		return BoundingBox();
	}

	BoundingBox result;

	for (const VertexIndex& vertexIndex : vertexIndices)
	{
		ocean_assert(vertexIndex < vertices_.size());
		result += vertices_[vertexIndex];
	}

	return result;
}

BoundingBox GLESVertexSet::boundingBox(const TriangleFaces& triangleFaces) const
{
	if (triangleFaces.empty())
	{
		return BoundingBox();
	}

	const ScopedLock scopedLock(objectLock);

	if (vertices_.empty())
	{
		return BoundingBox();
	}

	BoundingBox result;

	for (const TriangleFace& triangleFace : triangleFaces)
	{
		for (unsigned int n = 0u; n < 3u; ++n)
		{
			ocean_assert(triangleFace[n] < vertices_.size());
			result += vertices_[triangleFace[n]];
		}
	}

	return result;
}

BoundingBox GLESVertexSet::boundingBox(const VertexIndexGroups& strips) const
{
	if (strips.empty())
	{
		return BoundingBox();
	}

	const ScopedLock scopedLock(objectLock);

	if (vertices_.empty())
	{
		return BoundingBox();
	}

	BoundingBox result;

	for (const Indices32& strip : strips)
	{
		for (const Index32& index : strip)
		{
			ocean_assert(index < vertices_.size());
			result += vertices_[index];
		}
	}

	return result;
}

BoundingBox GLESVertexSet::boundingBox(const unsigned int numberVertices) const
{
	if (numberVertices == 0u)
	{
		return BoundingBox();
	}

	const ScopedLock scopedLock(objectLock);

	if (vertices_.empty())
	{
		return BoundingBox();
	}

	ocean_assert(size_t(numberVertices) <= vertices_.size());

	BoundingBox result;

	for (size_t n = 0; n < std::min(vertices_.size(), size_t(numberVertices)); ++n)
	{
		result += vertices_[n];
	}

	return result;
}

void GLESVertexSet::release()
{
	ocean_assert(GL_NO_ERROR == glGetError());

	bufferColors_.release();
	bufferNormals_.release();
	bufferVertices_.release();
	bufferTextureCoordinates2D_.release();
	bufferTextureCoordinates3D_.release();

	customVertexBufferObjectMap_.clear();

	ocean_assert(GL_NO_ERROR == glGetError());

	if (vertexArray_ != 0u)
	{
		glDeleteVertexArrays(1, &vertexArray_);
		ocean_assert(GL_NO_ERROR == glGetError());

		vertexArray_ = 0u;
	}

	vertices_.clear();
}

}

}

}
