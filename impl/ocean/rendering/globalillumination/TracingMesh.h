/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_TRACING_MESH_H
#define META_OCEAN_RENDERING_GI_TRACING_MESH_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GITextures.h"
#include "ocean/rendering/globalillumination/TracingObject.h"

#include "ocean/math/BoundingBox.h"
#include "ocean/math/BoundingSphere.h"
#include "ocean/math/Triangle3.h"

#include "ocean/rendering/Material.h"
#include "ocean/rendering/TriangleFace.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements the tracing object for a mesh.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT TracingMesh : public TracingObject
{
	private:

		/**
		 * This class implements a simple octree-like structure allowing to optimized the intersection performance.
		 * An octree instance is composed of a bounding box, a set of vertices belonging to the octree and optional octree children.
		 */
		class Octree
		{
			public:

				/**
				 * Creates a new octree object by a set of given indices and an corresponding mesh object.
				 * @param indices The indices of all vertices which will be covert by the new octree object
				 * @param mesh The mesh holding the entire mesh already
				 */
				Octree(const VertexIndices& indices, const TracingMesh& mesh);

				/**
				 * Destructs an octree object.
				 */
				~Octree();

				/**
				 * Determines the nearest intersection with between this tracing object and a given 3D ray.
				 * @see TracingObject::findNearestIntersection().
				 */
				void findNearestIntersection(const Line3& ray, const HomogenousMatrix4& objectTransformation, const HomogenousMatrix4& invertedObjectTransformation, const TracingMesh& mesh, RayIntersection& intersection, const bool frontFace, const Scalar eps, const TracingObject* excludedObject = nullptr) const;

				/**
				 * Determines whether this tracing object has an intersection with a provided 3D ray.
				 * @see TracingObject::hasIntersection().
				 */
				bool hasIntersection(const Line3& ray, const HomogenousMatrix4& objectTransformation, const HomogenousMatrix4& invertedObjectTransformation, const TracingMesh& mesh, const Scalar maximalDistance = Numeric::maxValue(), const TracingObject* excludedObject = nullptr) const;

			protected:

				/// The bounding box of this octree object.
				BoundingBox octreeBoundingBox;

				/// This indices of all vertices which are covert by this octree object.
				VertexIndices octreeIndices;

				/// Up to eight octree children object, nullptr if a child does not exist.
				Octree* octreeChildren[8];
		};

	public:

		/**
		 * Creates a new tracing object.
		 */
		TracingMesh();

		/**
		 * Destructs this tracing object.
		 */
		~TracingMesh() override;

		/**
		 * Defines the mesh by setting individual triangles.
		 * @param localVertices The entire set of vertices, defined in the local coordinate system
		 * @param localNormals The entire set of normals, defined in the local coordinate system, one normal for each vertex
		 * @param textureCoordinates The entire set of texture coordinates, one texture coordinate for each vertex
		 * @param faces The topology of the mesh, defining the individual triangles, indices with range [0, localVertices.size())
		 * @param objectTransformation The transformation of the local coordinate system transforming points defined in the local coordinate system to points defined in the world coordinate system
		 * @param localBoundingBox The bounding box of all used vertices, defined in the local coordinate system
		 */
		void setTriangles(const Vertices& localVertices, const Normals& localNormals, const TextureCoordinates& textureCoordinates, const TriangleFaces& faces, const HomogenousMatrix4& objectTransformation, const BoundingBox& localBoundingBox);

		/**
		 * Defines the mesh by setting a triangle strip.
		 * @param localVertices The entire set of vertices, defined in the local coordinate system
		 * @param localNormals The entire set of normals, defined in the local coordinate system, one normal for each vertex
		 * @param textureCoordinates The entire set of texture coordinates, one texture coordinate for each vertex
		 * @param indicesSet The topology of the mesh, defining the individual triangles, indices with range [0, localVertices.size())
		 * @param objectTransformation The transformation of the local coordinate system transforming points defined in the local coordinate system to points defined in the world coordinate system
		 * @param localBoundingBox The bounding box of all used vertices, defined in the local coordinate system
		 */
		void setTriangleStrips(const Vertices& localVertices, const Normals& localNormals, const TextureCoordinates& textureCoordinates, const VertexIndexGroups& indicesSet, const HomogenousMatrix4& objectTransformation, const BoundingBox& localBoundingBox);

		/**
		 * Determines the nearest intersection with between this tracing object and a given 3D ray.
		 * @see TracingObject::findNearestIntersection().
		 */
		void findNearestIntersection(const Line3& ray, RayIntersection& intersection, const bool frontFace, const Scalar eps, const TracingObject* excludedObject = nullptr) const override;

		/**
		 * Determines whether this tracing object has an intersection with a provided 3D ray.
		 * @see TracingObject::hasIntersection().
		 */
		bool hasIntersection(const Line3& ray, const Scalar maximalDistance = Numeric::maxValue(), const TracingObject* excludedObject = nullptr) const override;

		/**
		 * Determines the amount of light that transmits trough this object in the case e.g., this object is transparent.
		 * @see TracingObject::determineDampingColor().
		 */
		bool determineDampingColor(const Line3& ray, RGBAColor& color, const Scalar maximalDistance = Numeric::maxValue()) const override;

		/**
		 * Determines the light (the color) for a specified viewing ray this object does reflect.
		 * @see TracingObject::determineColor().
		 */
		bool determineColor(const Vector3& viewPosition, const Vector3& viewObjectDirection, const RayIntersection& intersection, const TracingGroup& group, const unsigned int bounces, const TracingObject* excludedObject, const Lighting::LightingModes lightingModes, RGBAColor& color) const override;

	private:

		/// The bounding box of this tracing object.
		BoundingBox tracingLocalBoundingBox;

		/// The bounding sphere of this tracing object.
		BoundingSphere tracingLocalBoundingSphere;

		/// The 3D triangles representing the mesh.
		Triangles3 tracingTriangles;

		/// The set of normals connected with the mesh's triangles.
		Normals tracingNormals;

		/// The set of texture coordinates connected with the mesh's triangles.
		TextureCoordinates tracingTextureCoordinates;

		/// The octree used to improved the performance of the intersection determination process.
		Octree* tracingOctree;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_TRACING_MESH_H
