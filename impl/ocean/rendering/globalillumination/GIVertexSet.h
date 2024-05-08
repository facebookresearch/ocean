/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_VERTEX_SET_H
#define META_OCEAN_RENDERING_GI_VERTEX_SET_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIObject.h"

#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

#define maximalTextureLayers 8

/**
 * This class implements a vertex set.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIVertexSet :
	virtual public GIObject,
	virtual public VertexSet
{
	friend class GIFactory;

	public:

		/**
		 * Returns the normals of this set.
		 * @see VertexSet::normals().
		 */
		Normals normals() const override;

		/**
		 * Returns the texture coordinates of this set.
		 * @see VertexSet::textureCoordinates().
		 */
		TextureCoordinates textureCoordinates(const unsigned int layerIndex) const override;

		/**
		 * Returns the vertices of this set.
		 * @see VertexSet::vertices().
		 */
		Vertices vertices() const override;

		/**
		 * Returns the colors of this set.
		 * @see VertexSet::colors().
		 */
		RGBAColors colors() const override;

		/**
		 * Returns the number of normals of this set.
		 * @see VertexSet::numberNormals().
		 */
		unsigned int numberNormals() const override;

		/**
		 * Returns the number of texture coordinats of this set.
		 * @see VertexSet::numberTextureCoordinates().
		 */
		unsigned int numberTextureCoordinates(const unsigned int layerIndex) const override;

		/**
		 * Returns the number of vertices of this set.
		 * @see VertexSet::numberVertices().
		 */
		unsigned int numberVertices() const override;

		/**
		 * Returns the number of colors of this set.
		 * @see VertexSet::numberColors().
		 */
		unsigned int numberColors() const override;

		/**
		 * Sets the normals for this set.
		 * @see VertexSet::setNormals().
		 */
		void setNormals(const Normals& normals) override;

		/**
		 * Sets the texels for this set.
		 * @see VertexSet::setTextureCoordinates().
		 */
		void setTextureCoordinates(const TextureCoordinates& textureCoordinates, const unsigned int layerIndex) override;

		/**
		 * Sets the vertices for this set.
		 * @see VertexSet::setVertices().
		 */
		void setVertices(const Vertices& vertices) override;

		/**
		 * Sets the colors for this set.
		 * @see VertexSet::setColors().
		 */
		void setColors(const RGBAColors& colors) override;

		/**
		 * Sets several attributes concurrently.
		 * @see VertexSet::set().
		 */
		void set(const Vertices& vertices, const Normals& normals, const TextureCoordinates& textureCoordinates, const RGBAColors& colors) override;

	protected:

		/**
		 * Creates a new vertex set object.
		 */
		GIVertexSet();

		/**
		 * Destructs a vertex set object.
		 */
		~GIVertexSet() override;

	protected:

		/// Vertex set normals.
		Normals vertexSetNormals;

		/// Vertex set texture coordinates.
		TextureCoordinates vertexSetTextureCoordinates[maximalTextureLayers];

		/// Vertex set vertices.
		Vertices vertexSetVertices;

		/// Vertex set colors.
		RGBAColors vertexSetColors;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_VERTEX_SET_H
