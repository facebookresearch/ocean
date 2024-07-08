/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_TRINAGLE_FACE_H
#define META_OCEAN_RENDERING_TRINAGLE_FACE_H

#include "ocean/rendering/Rendering.h"

#include <map>
#include <vector>

namespace Ocean
{

namespace Rendering
{

/// Foward declaration.
class TriangleFace;

/**
 * Definition of a vector holding triangle faces.
 * @see TriangleFace.
 * @ingroup rendering
 */
typedef std::vector<TriangleFace> TriangleFaces;

/**
 * Definition of a triangle face with three vertex indices.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT TriangleFace
{
	protected:

		/**
		 * Definition of a map mapping vertices to their corresponding face indices.
		 */
		typedef std::map<Vertex, VertexIndices> VertexMap;

	public:

		/**
		 * Creates a new triangle face object with undefined indices.
		 */
		inline TriangleFace();

		/**
		 * Creates a new triangle face object with successive indices.<br>
		 * The first index is given, the following indices will be set to successive values.
		 * @param startIndex Index of the first vertex
		 */
		inline explicit TriangleFace(const VertexIndex startIndex);

		/**
		 * Creates a new triangle face object with three given indices.
		 * @param first First vertex index
		 * @param second Second vertex index
		 * @param third Third vertex index
		 */
		inline TriangleFace(const VertexIndex first, const VertexIndex second, const VertexIndex third);

		/**
		 * Creates a new triangle face object by an array of at least three indices.
		 * @param arrayValue Array with at least three vertex indices
		 */
		inline explicit TriangleFace(const VertexIndex* arrayValue);

		/**
		 * Returns a specific vertex index specified by it's index inside the face.
		 * Beware: No range check will be done!
		 * @param index Index of the vertex index with range [0, 2]
		 * @return Specified vertex index
		 */
		[[nodiscard]] inline VertexIndex index(const unsigned int index) const;

		/**
		 * Returns a specific vertex index specified by it's index inside the face.
		 * Beware: No range check will be done!
		 * @param index Index of the vertex index with range [0, 2]
		 * @return Specified vertex index
		 */
		[[nodiscard]] inline VertexIndex& index(const unsigned int index);

		/**
		 * Returns a specific vertex index specified by it's index inside the face.
		 * Beware: No range check will be done!
		 * @param index Index of the vertex index with range [0, 2]
		 * @return Specified vertex index
		 */
		[[nodiscard]] inline VertexIndex operator[](const unsigned int index) const;

		/**
		 * Returns a specific vertex index specified by it's index inside the face.
		 * Beware: No range check will be done!
		 * @param index Index of the vertex index with range [0, 2]
		 * @return Specified vertex index
		 */
		[[nodiscard]] inline VertexIndex& operator[](const unsigned int index);

		/**
		 * Returns the pointer to the first element of the vertex indices.
		 * @return Array with vertex indices
		 */
		[[nodiscard]] inline const VertexIndex* operator()() const;

		/**
		 * Returns the pointer to the first element of the vertex indices.
		 * @return Array with vertex indices
		 */
		[[nodiscard]] inline VertexIndex* operator()();

		/**
		 * Returns whether two triangle face objects are identical.
		 * @param triangleFace The second object to compare
		 * @return True, if so
		 */
		inline bool operator==(const TriangleFace& triangleFace) const;

		/**
		 * Returns whether two triangle face objects are not identical.
		 * @param triangleFace The second object to compare
		 * @return True, if so
		 */
		inline bool operator!=(const TriangleFace& triangleFace) const;

		/**
		 * Hash function for a TriangleFace object.
		 * @param triangleFace The triangle face for which the hash will be determined
		 * @return The resulting hash value
		 */
		inline size_t operator()(const Rendering::TriangleFace& triangleFace) const;

		/**
		 * Calculates per-face normals for a given set of triangles.
		 * @param faces The triangle faces
		 * @param vertices The triangle vertices corresponding to the faces
		 * @param counterClockWise True, if the faces are provides so that a front face is determined in a counter clockwise order
		 * The resulting per-face normals, one for each faces
		 */
		[[nodiscard]] static Vectors3 calculatePerFaceNormals(const TriangleFaces& faces, const Vertices& vertices, const bool counterClockWise = true);

		/**
		 * Calculates smoothed per-vertex normals for a given set of triangles and per-face normals
		 * @param faces The triangle faces
		 * @param vertices The triangle vertices corresponding to the faces
		 * @param perFaceNormals The per-face normals
		 * The resulting per-vertex normals, one for each vertex
		 */
		[[nodiscard]] static Vectors3 calculateSmoothedPerVertexNormals(const TriangleFaces& faces, const Vertices& vertices, const Vectors3& perFaceNormals);

		/**
		 * Recalculates smoothed per-vertex normals for a given set of triangles with per vertex normals.
		 * @param faces The triangle faces
		 * @param vertices The triangle vertices corresponding to the faces
		 * @param normals The per-vertex normals, which are not smoothed yet
		 * @param creaseAngle Crease angle defining the seperation between per face and per vertex normals in radian, with range [0, PI/2]
		 * @return True, if succeeded
		 */
		static bool calculateSmoothedPerVertexNormals(const TriangleFaces& faces, const Vectors3& vertices, Vectors3& normals, const Scalar creaseAngle);

		/**
		 * Calculates smoothed per-vertexnormals for a given set of triangles.
		 * @param faces The triangle faces
		 * @param vertices The triangle vertices corresponding to the faces
		 * @return The resulting smoothed per-vertex normals
		 */
		static Vectors3 calculateSmoothedPerVertexNormals(const TriangleFaces& faces, const Vectors3& vertices);

		/**
		 * Convertes indices of triangles to triangle faces.
		 * @param indices The indices of the triangle, must be valid
		 * @param size The number of indices, with range [3, infinity), must be a multiple of 3
		 * @return The resulting triangle faces
		 */
		static inline TriangleFaces indices2triangleFaces(const Index32* indices, const size_t size);

	protected:

		/// The three vertex indices.
		VertexIndex indices_[3];
};

inline TriangleFace::TriangleFace()
{
	// nothing to do here
}

inline TriangleFace::TriangleFace(const VertexIndex startIndex)
{
	indices_[0] = startIndex;
	indices_[1] = startIndex + 1;
	indices_[2] = startIndex + 2;
}


inline TriangleFace::TriangleFace(const VertexIndex first, const VertexIndex second, const VertexIndex third)
{
	indices_[0] = first;
	indices_[1] = second;
	indices_[2] = third;
}

inline TriangleFace::TriangleFace(const VertexIndex* arrayValue)
{
	memcpy(indices_, arrayValue, sizeof(VertexIndex) * 3);
}

inline VertexIndex TriangleFace::index(const unsigned int index) const
{
	ocean_assert(index < 3);
	return indices_[index];
}

inline VertexIndex& TriangleFace::index(const unsigned int index)
{
	ocean_assert(index < 3);
	return indices_[index];
}

inline VertexIndex TriangleFace::operator[](const unsigned int index) const
{
	ocean_assert(index < 3);
	return indices_[index];
}

inline VertexIndex& TriangleFace::operator[](const unsigned int index)
{
	ocean_assert(index < 3);
	return indices_[index];
}

inline const VertexIndex* TriangleFace::operator()() const
{
	return indices_;
}

inline VertexIndex* TriangleFace::operator()()
{
	return indices_;
}

inline bool TriangleFace::operator==(const TriangleFace& triangleFace) const
{
	return indices_[0] == triangleFace.indices_[0] && indices_[1] == triangleFace.indices_[1] && indices_[2] == triangleFace.indices_[2];
}

inline bool TriangleFace::operator!=(const TriangleFace& triangleFace) const
{
	return !(*this == triangleFace);
}

inline size_t TriangleFace::operator()(const TriangleFace& triangleFace) const
{
	size_t seed = std::hash<Index32>{}(triangleFace[0]);
	seed ^= std::hash<Index32>{}(triangleFace[1]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<Index32>{}(triangleFace[2]) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

inline TriangleFaces TriangleFace::indices2triangleFaces(const Index32* indices, const size_t size)
{
	ocean_assert(indices != nullptr);
	ocean_assert(size >= 3 && size % 3 == 0);

	TriangleFaces triangleFaces;
	triangleFaces.reserve(size / 3);

	for (size_t n = 0; n < size; n += 3)
	{
		triangleFaces.emplace_back(indices[n + 0], indices[n + 1], indices[n + 2]);
	}

	return triangleFaces;
}

}

}

#endif // META_OCEAN_RENDERING_TRINAGLE_FACE_H
