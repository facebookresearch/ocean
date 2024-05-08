/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace Rendering
{

VertexSet::VertexSet() :
	Object()
{
	// nothing to do here
}

VertexSet::~VertexSet()
{
	// nothing to do here
}

Normals VertexSet::normals() const
{
	throw NotSupportedException("VertexSet::normals() is not supported.");
}

TextureCoordinates VertexSet::textureCoordinates(const unsigned int /*layerIndex*/) const
{
	throw NotSupportedException("VertexSet::textureCoordinates() is not supported.");
}

std::string VertexSet::phantomTextureCoordinateSystem() const
{
	throw NotSupportedException("VertexSet::phantomTextureCoordinateSystem() is not supported.");
}

Vertices VertexSet::vertices() const
{
	throw NotSupportedException("VertexSet::vertices() is not supported.");
}

RGBAColors VertexSet::colors() const
{
	throw NotSupportedException("VertexSet::colors() is not supported.");
}

unsigned int VertexSet::numberNormals() const
{
	throw NotSupportedException("VertexSet::numberNormals() is not supported.");
}

unsigned int VertexSet::numberTextureCoordinates(const unsigned int /*layerIndex*/) const
{
	throw NotSupportedException("VertexSet::numberTexutreCoordinates() is not supported.");
}

unsigned int VertexSet::numberVertices() const
{
	throw NotSupportedException("VertexSet::numberVertices() is not supported.");
}

unsigned int VertexSet::numberColors() const
{
	throw NotSupportedException("VertexSet::numberColors() is not supported.");
}

void VertexSet::setNormals(const Normals& /*normals*/)
{
	throw NotSupportedException("VertexSet::setNormals() is not supported.");
}

void VertexSet::setNormals(const Vector3* /*normals*/, const size_t /*size*/)
{
	throw NotSupportedException("VertexSet::setNormals() is not supported.");
}

void VertexSet::setTextureCoordinates(const TextureCoordinates& /*textureCoordinates*/, const unsigned int /*layerIndex*/)
{
	throw NotSupportedException("VertexSet::setTextureCoordinates() is not supported.");
}

void VertexSet::setPhantomTextureCoordinates(const Vertices& /*textureCoordinates*/, const unsigned int /*layerIndex*/)
{
	throw NotSupportedException("VertexSet::setPhantomTextureCoordinates() is not supported.");
}

void VertexSet::setPhantomTextureCoordinateSystem(const std::string& /*reference*/)
{
	throw NotSupportedException("VertexSet::setPhantomTextureCoordinateSystem() is not supported.");
}

void VertexSet::setVertices(const Vertices& /*vertices*/)
{
	throw NotSupportedException("VertexSet::setVertices() is not supported.");
}

void VertexSet::setVertices(const Vector3* /*vertices*/, const size_t /*size*/)
{
	throw NotSupportedException("VertexSet::setVertices() is not supported.");
}

void VertexSet::setColors(const RGBAColors& /*colors*/)
{
	throw NotSupportedException("VertexSet::setColors() is not supported.");
}

void VertexSet::set(const Vertices& /*vertices*/, const Normals& /*normals*/, const TextureCoordinates& /*textureCoordinates*/, const RGBAColors& /*colors*/)
{
	throw NotSupportedException("VertexSet::set() is not supported.");
}

VertexSet::ObjectType VertexSet::type() const
{
	return TYPE_VERTEX_SET;
}


}

}
