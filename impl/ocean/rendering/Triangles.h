/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_TRIANGLES_H
#define META_OCEAN_RENDERING_TRIANGLES_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/IndependentPrimitive.h"
#include "ocean/rendering/TriangleFace.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Triangles;

/**
 * Definition of a smart object reference holding a triangles node.
 * @see SmartObjectRef, Triangles.
 * @ingroup rendering
 */
typedef SmartObjectRef<Triangles> TrianglesRef;

/**
 * This class is the base for all triangles objects.
 * A triangles object defines a mesh using a vertex set and triangle faces.<br>
 * The mesh is composed by several independent triangles.<br>
 * The vertices of all triangles are given by the vertex set, the mesh topology is given by triangle faces.<br>
 * Each triangle can hold a normal, a texture coordinate or/and a color value for ech triangle vertex.<br>
 * Different triangles objects can use the same vertex set to share common vertices and to reduce memory usage.<br>
 * @see VertexSet, TriangleFaces
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Triangles : virtual public IndependentPrimitive
{
	public:

		/**
		 * Returns the faces of this triangles object.
		 * @return Triangle faces
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual TriangleFaces faces() const;

		/**
		 * Returns the number of triangle faces defined in this primitive.
		 * @return Number of triangle faces
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual unsigned int numberFaces() const;

		/**
		 * Sets the faces of this triangles object.
		 * @param faces The vertex indices of the individual triangles to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setFaces(const TriangleFaces& faces);

		/**
		 * Sets the faces of this triangles object.
		 * @param numberImplicitTriangleFaces The number of triangle faces based on vertices with consecutive indices, with range [0, infinity)
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setFaces(const unsigned int numberImplicitTriangleFaces);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

		/**
		 * Creates a new set of triangles with face normals out of a given set of vertices and corresponding triangle faces.
		 * @param vertices The existing set of vertices out of which the new mesh will be created
		 * @param faces The set of existing triangles for which the new mesh will be created, corresponding to the set of existing vertices
		 * @param triangleVertices The resulting new triangle vertices
		 * @param triangleNormals The resulting (new) triangle normals, one normal for each resulting vertex
		 * @param triangleFaces The resulting triangle faces, corresponding to the resulting vertices
		 * @param skipIrregularTriangles True, to skip an irregular triangle and to proceed with the next one; False, to stop the creating immediately if an irregular triangle occurs
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		static bool createTriangleMesh(const Vertices& vertices, const TriangleFaces& faces, Vertices& triangleVertices, Normals& triangleNormals, TriangleFaces& triangleFaces, const bool skipIrregularTriangles = true);

	protected:

		/**
		 * Creates a new triangles object.
		 */
		Triangles();

		/**
		 * Destructs a triangles object.
		 */
		~Triangles() override;
};

}

}

#endif // META_OCEAN_RENDERING_TRIANGLES_H
