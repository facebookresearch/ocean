// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_TRACKING_UVTEXTUREMAPPING_MESH_H
#define META_OCEAN_TRACKING_UVTEXTUREMAPPING_MESH_H

#include "ocean/tracking/uvtexturemapping/UVTextureMapping.h"

#include "ocean/base/ObjectRef.h"

#include "ocean/geometry/SpatialDistribution.h"

#include "ocean/math/BoundingBox.h"
#include "ocean/math/Triangle2.h"
#include "ocean/math/Triangle3.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include <array>

namespace Ocean
{

namespace Tracking
{

namespace UVTextureMapping
{

// Forward declaration.
class MeshUVTextureMapping;

/**
 * Definition of an object reference covering a mesh UV texture mapping object.
 * @see MeshUVTextureMapping.
 * @ingroup trackinguvtexturemapping
 */
typedef ObjectRef<MeshUVTextureMapping> MeshUVTextureMappingRef;

/**
 * Implementation allowing for the conversion of a 2D coordinate in a UV texture mapping to a 3D coordinate on the surface of the associated mesh.
 * For each mesh triangle, there is no constraint on the vertex winding order -- this class doesn't consider face normals and is simply an interface for 2D->3D mapping.
 * However, it's usually a sane idea to have a counter-clockwise ordering for the texture coordinates of a triangle (i.e., for a homogeneous representation, [ (v[i1] - v[i0]) x (v[i2] - v[i0]) ].z() < 0), and for the associated 3D triangle have its normal point outward from the surface.
 * Note that, for proper behavior of this class, it is necessary that the 2D UV mesh does not contain overlapping triangles.
 * @ingroup trackinguvtexturemapping
 */
class OCEAN_TRACKING_UVTEXTUREMAPPING_EXPORT MeshUVTextureMapping
{
	public:
		typedef unsigned int VertexIndex;
		typedef std::array<VertexIndex, 3u> TriangleFace;
		typedef std::vector<TriangleFace> TriangleFaces;

		typedef unsigned int TriangleIndex;

		/**
		 * Creates an invalid texture mapping.
		 */
		MeshUVTextureMapping();

		/**
		 * Defines a mapping from a meshed surface to a 2D UV texture space.
		 * @param vertices Set of 3D vertices defining the mesh
		 * @param textureCoordinates Set of texture coordinates defining vertices of the UV-mapped mesh, specified in pixel coordinates relative to the top left corner of the texture; each mesh face can choose its own texture coordinates independently of any other faces that might share vertices with it
		 * @param vertexTriangleFaces Triangulation of the 3D mesh having the same number of elements as textureTriangleFaces; each entry defines three unique indices in the vertices array; it's usually assumed that the vertex ordering should be specified such that the face normal points outward from the surface
		 * @param textureTriangleFaces Triangulation of the UV-mapped mesh having the same number of elements as vertexTriangleFaces, with each entry defining three unique indices in the textureCoordinates array; the input triangles must not overlap
		 */
		MeshUVTextureMapping(const Vectors3& vertices, const Vectors2& textureCoordinates, const TriangleFaces& vertexTriangleFaces, const TriangleFaces& textureTriangleFaces);

		/**
		 * Returns whether an instantiated mapping is valid. Internally, we check whether the mesh has any valid triangles.
		 * @return True if the map was created without errors, otherwise false
		 */
		inline bool isValid() const;

		/**
		 * Computes the associated 3D point on a mesh for the given 2D point in the UV texture space by
		 * converting the 2D barycentric coordinate into a 3D coordinate.
		 * @param point2D UV coordinates of the point in the mapping
		 * @param point3D Output 3D location of the associated point in the coordinate frame of the mesh
		 * @param containingTriangleIndex Optional output index of the mesh triangle that contains the specified 2D point, if such a triangle exists
		 * @return True if the provided 2D point lies within a 3D-mapped region of the UV texture, false otherwise
		 */
		bool textureCoordinateTo3DCoordinate(const Vector2& point2D, Vector3& point3D, Index32* containingTriangleIndex = nullptr) const;

		/**
		 * Computes the associated 3D point on a mesh for the given 2D point in the UV texture space by
		 * converting the 2D barycentric coordinate into a 3D coordinate for the triangle specified by
		 * triangleIndex.
		 * @param point2D UV coordinates of the point in the mapping
		 * @param triangleIndex Index of the mesh triangle to use for the transformation, with range [0, triangles3().size())
		 * @param point3D Output 3D location of the associated point in the coordinate frame of the mesh; this point will always be the correct 3D point for the specified triangle (if valid), even if the function returns false
		 * @return True if the provided 2D point lies within the specified 2D triangle, false otherwise
		 */
		bool textureCoordinateTo3DCoordinateForTriangle(const Vector2& point2D, Index32 triangleIndex, Vector3& point3D) const;

		/**
		 * Returns the set of 3D triangles for the mesh.
		 * @return 3D triangles, with equal length to triangles2()
		 */
		inline const Triangles3& triangles3() const;

		/**
		 * Returns the set of 2D UV-mapped triangles for the mesh.
		 * @return 2D triangles, with equal length to triangles3()
		 */
		inline const Triangles2& triangles2() const;

		/**
		 * Returns the bounding box for the mesh vertices.
		 * @return 3D bounding box
		 */
		inline const BoundingBox& boundingBox() const;

	private:
		/// Set of 3D triangles associated with the mesh, empty if the mesh is invalid.
		Triangles3 triangles3_;

		/// Set of 2D UV-space triangles associated with the mesh, empty if the mesh is invalid.
		Triangles2 triangles2_;

		/// 3D bounding box for the mesh vertices.
		BoundingBox boundingBox_;

		/// For efficient triangle querying, we'll grid up the image space and store the triangle set
		/// overlapping with each bin.
		Geometry::SpatialDistribution::DistributionArray distributionArray_;
};

inline bool MeshUVTextureMapping::isValid() const
{
	return triangles3_.size() > 0u;
}

inline const Triangles3& MeshUVTextureMapping::triangles3() const
{
	return triangles3_;
}

inline const Triangles2& MeshUVTextureMapping::triangles2() const
{
	return triangles2_;
}

inline const BoundingBox& MeshUVTextureMapping::boundingBox() const
{
	return boundingBox_;
}

} // namespace UVTextureMapping

} // namespace Tracking

} // namespace Ocean

#endif // META_OCEAN_TRACKING_UVTEXTUREMAPPING_MESH_H
