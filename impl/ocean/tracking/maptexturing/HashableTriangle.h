/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_HASHABLE_TRIANGLE_H
#define META_OCEAN_TRACKING_MAPTEXTURING_HASHABLE_TRIANGLE_H

#include "ocean/tracking/maptexturing/MapTexturing.h"

#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

// Forward declaration.
template <typename T> class HashableTriangleT;

/**
 * Definition of a hash-able triangle with Scalar precision.
 * @see HashableTriangleT
 * @ingroup trackingmaptexturing
 */
typedef HashableTriangleT<Scalar> HashableTriangle;

/**
 * This class implements a 3D triangle which is hash-able.
 * @tparam T the scalar data type of the vertex, e.g., 'float' or 'double'
 * @ingroup trackingmaptexturing
 */
template <typename T>
class HashableTriangleT
{
	public:

		/**
		 * Default constructor for an invalid triangle.
		 */
		HashableTriangleT() = default;

		/**
		 * Creates a new triangle.
		 * @param vertices The three vertices of the triangle, must be valid
		 */
		explicit inline HashableTriangleT(const VectorT3<T>* vertices);

		/**
		 * Creates a new triangle.
		 * @param vertex0 The first vertex of the triangle
		 * @param vertex1 The first vertex of the triangle
		 * @param vertex2 The first vertex of the triangle
		 */
		inline HashableTriangleT(const VectorT3<T>& vertex0, const VectorT3<T>& vertex1, const VectorT3<T>& vertex2);

		/**
		 * Returns the first vertex of this triangle.
		 * @return The triangle's first vertex
		 */
		inline const VectorT3<T>& vertex0() const;

		/**
		 * Returns the second vertex of this triangle.
		 * @return The triangle's second vertex
		 */
		inline const VectorT3<T>& vertex1() const;

		/**
		 * Returns the third vertex of this triangle.
		 * @return The triangle's third vertex
		 */
		inline const VectorT3<T>& vertex2() const;

		/**
		 * Returns the three vertices of this triangle.
		 * @return The triangle's three vertices
		 */
		inline const VectorT3<T>* vertices() const;

		/**
		 * Returns whether two triangles contain exactly the same vertices.
		 * @param triangle The second triangle
		 * @return True, if so
		 */
		inline bool operator==(const HashableTriangleT<T>& triangle) const;

		/**
		 * Returns whether two triangles do not contain exactly the same vertices.
		 * @param triangle The second triangle
		 * @return True, if so
		 */
		inline bool operator!=(const HashableTriangleT<T>& triangle) const;

		/**
		 * Hash function.
		 * @param triangle The triangle for which the hash value will be determined
		 * @return The resulting hash value
		 */
		inline size_t operator()(const HashableTriangleT<T>& triangle) const;

	protected:

		/**
		 * Determines the hash value for three vertices.
		 * @param vertex0 The first vertex
		 * @param vertex1 The second vertex
		 * @param vertex2 The third vertex
		 * @return The resulting hash value
		 */
		static inline size_t hashValue(const VectorT3<T>& vertex0, const VectorT3<T>& vertex1, const VectorT3<T>& vertex2);

	protected:

		/// The three vertices of this triangle.
		VectorT3<T> vertices_[3];
};

template <typename T>
inline HashableTriangleT<T>::HashableTriangleT(const VectorT3<T>* vertices)
{
	ocean_assert(vertices != nullptr);

	vertices_[0] = vertices[0];
	vertices_[1] = vertices[1];
	vertices_[2] = vertices[2];
}

template <typename T>
inline HashableTriangleT<T>::HashableTriangleT(const VectorT3<T>& vertex0, const VectorT3<T>& vertex1, const VectorT3<T>& vertex2)
{
	vertices_[0] = vertex0;
	vertices_[1] = vertex1;
	vertices_[2] = vertex2;
}

template <typename T>
inline const VectorT3<T>& HashableTriangleT<T>::vertex0() const
{
	return vertices_[0];
}

template <typename T>
inline const VectorT3<T>& HashableTriangleT<T>::vertex1() const
{
	return vertices_[1];
}

template <typename T>
inline const VectorT3<T>& HashableTriangleT<T>::vertex2() const
{
	return vertices_[2];
}

template <typename T>
inline const VectorT3<T>* HashableTriangleT<T>::vertices() const
{
	return vertices_;
}

template <typename T>
inline bool HashableTriangleT<T>::operator==(const HashableTriangleT<T>& triangle) const
{
	return vertices_[0].x() == triangle.vertices_[0].x() && vertices_[0].y() == triangle.vertices_[0].y() && vertices_[0].z() == triangle.vertices_[0].z()
				&& vertices_[1].x() == triangle.vertices_[1].x() && vertices_[1].y() == triangle.vertices_[1].y() && vertices_[1].z() == triangle.vertices_[1].z()
				&& vertices_[2].x() == triangle.vertices_[2].x() && vertices_[2].y() == triangle.vertices_[2].y() && vertices_[2].z() == triangle.vertices_[2].z();
}

template <typename T>
inline bool HashableTriangleT<T>::operator!=(const HashableTriangleT<T>& triangle) const
{
	return !(*this == triangle);
}

template <typename T>
inline size_t HashableTriangleT<T>::operator()(const HashableTriangleT& triangle) const
{
	return hashValue(triangle.vertex0(), triangle.vertex1(), triangle.vertex2());
}

template <typename T>
inline size_t HashableTriangleT<T>::hashValue(const VectorT3<T>& vertex0, const VectorT3<T>& vertex1, const VectorT3<T>& vertex2)
{
	size_t seed = std::hash<T>{}(vertex0.x());
	seed ^= std::hash<T>{}(vertex0.y()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(vertex0.z()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	seed ^= std::hash<T>{}(vertex1.x()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(vertex1.y()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(vertex1.z()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	seed ^= std::hash<T>{}(vertex2.x()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(vertex2.y()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	seed ^= std::hash<T>{}(vertex2.z()) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

	return seed;
}

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_HASHABLE_TRIANGLE_H
