/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_QUAD_FACE_H
#define META_OCEAN_RENDERING_QUAD_FACE_H

#include "ocean/rendering/Rendering.h"

#include <vector>

namespace Ocean
{

namespace Rendering
{

/// Foward declaration.
class QuadFace;

/**
 * Definition of a vector holding quad faces.
 * @ingroup rendering
 */
typedef std::vector<QuadFace> QuadFaces;

/**
 * Definition of a quad face with four vertex indices.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT QuadFace
{
	public:

		/**
		 * Creates a new quad face object with undefined indices.
		 */
		inline QuadFace();

		/**
		 * Creates a new quad face object with successive indices.<br>
		 * The first index is given, the following indices will be set to successive values.
		 * @param startIndex Index of the first vertex
		 */
		inline explicit QuadFace(const VertexIndex startIndex);

		/**
		 * Creates a new quad face object with four given indices.
		 * @param first First vertex index
		 * @param second Second vertex index
		 * @param third Third vertex index
		 * @param fourth Fourth vertex index
		 */
		inline QuadFace(const VertexIndex first, const VertexIndex second, const VertexIndex third, const VertexIndex fourth);

		/**
		 * Creates a new quad face object by an array of at least four indices.
		 * @param arrayValue Array with at least four vertex indices
		 */
		inline explicit QuadFace(const VertexIndex* arrayValue);

		/**
		 * Returns a specific vertex index specified by it's index inside the face.
		 * Beware: No range check will be done!
		 * @param index Index of the vertex index with range [0, 3]
		 * @return Specified vertex index
		 */
		inline VertexIndex index(const unsigned int index) const;

		/**
		 * Returns a specific vertex index specified by it's index inside the face.
		 * Beware: No range check will be done!
		 * @param index Index of the vertex index with range [0, 3]
		 * @return Specified vertex index
		 */
		inline VertexIndex& index(const unsigned int index);

		/**
		 * Returns a specific vertex index specified by it's index inside the face.
		 * Beware: No range check will be done!
		 * @param index Index of the vertex index with range [0, 3]
		 * @return Specified vertex index
		 */
		inline VertexIndex operator[](const unsigned int index) const;

		/**
		 * Returns a specific vertex index specified by it's index inside the face.
		 * Beware: No range check will be done!
		 * @param index Index of the vertex index with range [0, 3]
		 * @return Specified vertex index
		 */
		inline VertexIndex& operator[](const unsigned int index);

		/**
		 * Returns the pointer to the first element of the vertex indices.
		 * @return Array with vertex indices
		 */
		inline const VertexIndex* operator()() const;

		/**
		 * Returns the pointer to the first element of the vertex indices.
		 * @return Array with vertex indices
		 */
		inline VertexIndex* operator()();

	protected:

		/// The four vertex indices.
		VertexIndex faceIndices[4];
};

inline QuadFace::QuadFace()
{
	// nothing to do here
}

inline QuadFace::QuadFace(const VertexIndex startIndex)
{
	faceIndices[0] = startIndex;
	faceIndices[1] = startIndex + 1;
	faceIndices[2] = startIndex + 2;
	faceIndices[3] = startIndex + 3;
}

inline QuadFace::QuadFace(const VertexIndex first, const VertexIndex second, const VertexIndex third, const VertexIndex fourth)
{
	faceIndices[0] = first;
	faceIndices[1] = second;
	faceIndices[2] = third;
	faceIndices[3] = fourth;
}

inline QuadFace::QuadFace(const VertexIndex* arrayValue)
{
	memcpy(faceIndices, arrayValue, sizeof(VertexIndex) * 4);
}

inline VertexIndex QuadFace::index(const unsigned int index) const
{
	ocean_assert(index < 4);
	return faceIndices[index];
}

inline VertexIndex& QuadFace::index(const unsigned int index)
{
	ocean_assert(index < 4);
	return faceIndices[index];
}

inline VertexIndex QuadFace::operator[](const unsigned int index) const
{
	ocean_assert(index < 4);
	return faceIndices[index];
}

inline VertexIndex& QuadFace::operator[](const unsigned int index)
{
	ocean_assert(index < 4);
	return faceIndices[index];
}

inline const VertexIndex* QuadFace::operator()() const
{
	return faceIndices;
}

inline VertexIndex* QuadFace::operator()()
{
	return faceIndices;
}

}

}

#endif // META_OCEAN_RENDERING_QUAD_FACE_H
