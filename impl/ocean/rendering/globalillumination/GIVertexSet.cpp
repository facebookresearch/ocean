/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIVertexSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIVertexSet::GIVertexSet()
{
	// nothing to do here
}

GIVertexSet::~GIVertexSet()
{
	// nothing to do here
}

Normals GIVertexSet::normals() const
{
	const ScopedLock scopedLock(objectLock);
	return vertexSetNormals;
}

TextureCoordinates GIVertexSet::textureCoordinates(const unsigned int layerIndex) const
{
	ocean_assert(layerIndex < maximalTextureLayers);
	const ScopedLock scopedLock(objectLock);

	return vertexSetTextureCoordinates[layerIndex];
}

Vertices GIVertexSet::vertices() const
{
	const ScopedLock scopedLock(objectLock);
	return vertexSetVertices;
}

RGBAColors GIVertexSet::colors() const
{
	const ScopedLock scopedLock(objectLock);
	return vertexSetColors;
}

unsigned int GIVertexSet::numberNormals() const
{
	const ScopedLock scopedLock(objectLock);
	return (unsigned int)vertexSetNormals.size();
}

unsigned int GIVertexSet::numberTextureCoordinates(const unsigned int layerIndex) const
{
	ocean_assert(layerIndex < maximalTextureLayers);
	const ScopedLock scopedLock(objectLock);

	return (unsigned int)vertexSetTextureCoordinates[layerIndex].size();
}

unsigned int GIVertexSet::numberVertices() const
{
	const ScopedLock scopedLock(objectLock);
	return (unsigned int)vertexSetVertices.size();
}

unsigned int GIVertexSet::numberColors() const
{
	const ScopedLock scopedLock(objectLock);
	return (unsigned int)vertexSetColors.size();
}

void GIVertexSet::setNormals(const Normals& normals)
{
	const ScopedLock scopedLock(objectLock);
	vertexSetNormals = normals;
}

void GIVertexSet::setTextureCoordinates(const TextureCoordinates& textureCoordinates, const unsigned int layerIndex)
{
	ocean_assert(layerIndex < maximalTextureLayers);
	const ScopedLock scopedLock(objectLock);

	vertexSetTextureCoordinates[layerIndex] = textureCoordinates;
}

void GIVertexSet::setVertices(const Vertices& vertices)
{
	const ScopedLock scopedLock(objectLock);
	vertexSetVertices = vertices;
}

void GIVertexSet::setColors(const RGBAColors& colors)
{
	const ScopedLock scopedLock(objectLock);
	vertexSetColors = colors;
}

void GIVertexSet::set(const Vertices& vertices, const Normals& normals, const TextureCoordinates& textureCoordinates, const RGBAColors& colors)
{
	ocean_assert(0 < maximalTextureLayers);
	const ScopedLock scopedLock(objectLock);

	vertexSetNormals = normals;
	vertexSetTextureCoordinates[0] = textureCoordinates;
	vertexSetVertices = vertices;
	vertexSetColors = colors;
}

}

}

}
