/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_VERTEX_SET_H
#define META_OCEAN_RENDERING_VERTEX_SET_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Object.h"
#include "ocean/rendering/ObjectRef.h"

#include "ocean/math/RGBAColor.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class VertexSet;

/**
 * Definition of a smart object reference holding a vertex set node.
 * @see SmartObjectRef, VertexSet.
 * @ingroup rendering
 */
typedef SmartObjectRef<VertexSet> VertexSetRef;

/**
 * This class is the base class for all rendering vertex sets.
 * A vertex set holds vertices, normals, texture coordinates and color values.<br>
 * A vertex set must define vertices to be valid, normals, texture coordinates and color values are optional.<br>
 * All elements of a vertex set are one by one mapping elements.<br>
 * Meaning that e.g. the first vertex corresponds to the first normal or the first color value, the second to the second and so on.<br>
 * However, a vertex set does not define the topology of a mesh.<br>
 * Therefore, a vertex set has to be used in combination with primitive objects like e.g. triangles or triangle strips objects.<br>
 * A vertex set can be used in combination with different primitive objects.<br>
 * Beware: Once e.g. normals are defined make sure that the number of defined normals is equal to or larger than the number of vertices.<br>
 * @see Triangles, TriangleFans, TriangleStrips, Quads, QuadStrips, Vertices, Normals, TextureCoordinates, RGBAColors.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT VertexSet : virtual public Object
{
	public:

		/**
		 * Returns the normals of this set.
		 * @return Normals
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Normals normals() const;

		/**
		 * Returns the texture coordinates of this set.
		 * @param layerIndex Index of the texture layer to receive the coordinates for
		 * @return Texture coordinates
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual TextureCoordinates textureCoordinates(const unsigned int layerIndex) const;

		/**
		 * Returns the explicit reference coordinate system of the phantom object, if any.
		 * @return The name of the reference coordinate system
		 */
		virtual std::string phantomTextureCoordinateSystem() const;

		/**
		 * Returns the vertices of this set.
		 * @return Vertices
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Vertices vertices() const;

		/**
		 * Returns the colors of this set.
		 * @return Colors
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual RGBAColors colors() const;

		/**
		 * Returns the number of normals of this set.
		 * @return Number of normals
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual unsigned int numberNormals() const;

		/**
		 * Returns the number of texture coordinates of this set.
		 * @param layerIndex Index of the texture layer to receive the coordinate number for
		 * @return Number of texture coordinates
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual unsigned int numberTextureCoordinates(const unsigned int layerIndex) const;

		/**
		 * Returns the number of vertices of this set.
		 * @return Number of vertices
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual unsigned int numberVertices() const;

		/**
		 * Returns the number of colors of this set.
		 * @return Number of colors
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual unsigned int numberColors() const;

		/**
		 * Sets the normals for this set.
		 * @param normals New normals to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setNormals(const Normals& normals);

		/**
		 * Sets the normals for this set.
		 * @param normals New normals to set, nullptr to reset previously set normals
		 * @param size The number of normals to set, with range [0, infinity)
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setNormals(const Vector3* normals, const size_t size);

		/**
		 * Sets the texels for this set.
		 * @param textureCoordinates New texture coordinates to set
		 * @param layerIndex Index of the texture layer to set the coordinates for
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setTextureCoordinates(const TextureCoordinates& textureCoordinates, const unsigned int layerIndex);

		/**
		 * Sets 3D texels for this set used for phantom objects.
		 * @param textureCoordinates New 3D texture coordinate to set
		 * @param layerIndex Index of the 3D phantom texture layer to set the coordinates for
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setPhantomTextureCoordinates(const Vertices& textureCoordinates, const unsigned int layerIndex);

		/**
		 * Sets the explicit reference coordinate system of the phantom objects.
		 * @param reference The reference coordinate system to be used for the phantom texture coordinates
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setPhantomTextureCoordinateSystem(const std::string& reference);

		/**
		 * Sets the vertices for this set.
		 * @param vertices New vertices to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setVertices(const Vertices& vertices);

		/**
		 * Sets the vertices for this set.
		 * @param vertices New vertices to set, nullptr to reset previously set vertices
		 * @param size The number of vertices to set, with range [0, infinity)
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setVertices(const Vector3* vertices, const size_t size);

		/**
		 * Sets the colors for this set.
		 * @param colors New colors to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setColors(const RGBAColors& colors);

		/**
		 * Sets several attributes concurrently.
		 * @param vertices New vertices to set
		 * @param normals New normals to set
		 * @param textureCoordinates New texture coordinates for the first texture layer to set
		 * @param colors New colors to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void set(const Vertices& vertices, const Normals& normals, const TextureCoordinates& textureCoordinates, const RGBAColors& colors);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new vertex set object.
		 */
		VertexSet();

		/**
		 * Destructs a vertex set object.
		 */
		~VertexSet() override;
};

}

}

#endif // META_OCEAN_RENDERING_VERTEX_SET_H
