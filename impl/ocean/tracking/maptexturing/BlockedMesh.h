/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_MAPTEXTURING_BLOCKED_MESH_H
#define META_OCEAN_TRACKING_MAPTEXTURING_BLOCKED_MESH_H

#include "ocean/tracking/maptexturing/MapTexturing.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/RGBAColor.h"
#include "ocean/math/Sphere3.h"
#include "ocean/math/Triangle3.h"

#include "ocean/rendering/TriangleFace.h"

namespace Ocean
{

namespace Tracking
{

namespace MapTexturing
{

// Forward declaration.
class BlockedMesh;

/**
 * Definition of a vector holding BlockedMesh objects.
 */
typedef std::vector<BlockedMesh> BlockedMeshes;

/**
 * This class implements a blocked mesh storing only triangles located in a 3D block/box.
 * @ingroup trackingmaptexturing
 */
class OCEAN_TRACKING_MAPTEXTURING_EXPORT BlockedMesh
{
	protected:

		typedef std::unordered_map<Index32, Index32> IndexMap;

	public:

		/**
		 * Default constructor.
		 */
		BlockedMesh() = default;

		BlockedMesh(const VectorI3& block, const Box3& boundingBox, Vectors3&& vertices, Vectors3&& perVertexNormals, RGBAColors&& perVertexColors, Rendering::TriangleFaces&& triangleFaces) :
			block_(block),
			boundingBox_(boundingBox),
			boundingSphere_(boundingBox),
			vertices_(std::move(vertices)),
			perVertexNormals_(std::move(perVertexNormals)),
			perVertexColors_(std::move(perVertexColors)),
			triangleFaces_(std::move(triangleFaces))
		{
			ocean_assert(!triangleFaces_.empty() == boundingBox_.isValid());
		}

		void addTriangle(const Rendering::TriangleFace& triangleFace, const Vector3* vertices);

		void addTriangle(const Rendering::TriangleFace& triangleFace, const Vector3* vertices, const Vector3* perVertexNormals);

		void addTriangle(const Rendering::TriangleFace& triangleFace, const Vector3* vertices, const Vector3* perVertexNormals, const RGBAColor* perVertexColors);

		inline const VectorI3& block() const
		{
			return block_;
		}

		inline const Box3& boundingBox() const
		{
			return boundingBox_;
		}

		inline const Sphere3& boundingSphere() const
		{
			return boundingSphere_;
		}

		inline bool isValid() const
		{
			return !triangleFaces_.empty();
		}

		static BlockedMeshes separateMesh(const Vectors3& vertices, const Rendering::TriangleFaces& triangleFaces, const int blockSize = 1);

		static BlockedMeshes separateMesh(const Vectors3& vertices, const Vectors3& perVertexNormals, const Rendering::TriangleFaces& triangleFaces, const int blockSize = 1);

		static BlockedMeshes separateMesh(const Vectors3& vertices, const Vectors3& perVertexNormals, const RGBAColors& perVertexColors, const Rendering::TriangleFaces& triangleFaces, const int blockSize = 1);

		static VectorT3<int> vertex2block(const Vector3& vertex, const int blockSize);

		static Index64 makeTriangleId(const Index32& meshId, const Index32& triangleIndex)
		{
			return Index64(meshId) | Index64(triangleIndex) << 32u;
		}

		static void separateTriangleId(const Index64 triangleId, Index32& meshId, Index32& triangleIndex)
		{
			meshId = Index32(triangleId & Index64(0xFFFFFFFFu));
			triangleIndex = Index32(triangleId >> 32u);
		}

	public:

		VectorI3 block_;

		Box3 boundingBox_;

		Sphere3 boundingSphere_;

		/// The vertices of this mesh.
		Vectors3 vertices_;

		/// The per-vertex normals of this mesh.
		Vectors3 perVertexNormals_;

		/// The per-vertex colors of this mesh.
		RGBAColors perVertexColors_;

		/// The triangle faces of this mesh.
		Rendering::TriangleFaces triangleFaces_;

		IndexMap indexMap_;
};

}

}

}

#endif // META_OCEAN_TRACKING_MAPTEXTURING_BLOCKED_MESH_H
