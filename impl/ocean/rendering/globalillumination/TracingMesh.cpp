/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/TracingMesh.h"
#include "ocean/rendering/globalillumination/Lighting.h"

#include "ocean/rendering/Material.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

TracingMesh::Octree::Octree(const VertexIndices& indices, const TracingMesh& mesh)
{
	ocean_assert(!indices.empty());

	for (VertexIndices::const_iterator i = indices.begin(); i != indices.end(); ++i)
	{
		const Triangle3& triangle = mesh.tracingTriangles[*i];
		octreeBoundingBox += triangle.point0();
		octreeBoundingBox += triangle.point1();
		octreeBoundingBox += triangle.point2();
	}

	for (unsigned int n = 0u; n < 8u; ++n)
	{
		octreeChildren[n] = nullptr;
	}

	if (indices.size() > 20)
	{
		const Vector3 center(octreeBoundingBox.center());

		VertexIndices newIndices;
		newIndices.reserve(indices.size());

		size_t debugTotal = 0u;

		unsigned int usedSubsets = 0u;
		for (unsigned int xArea = 0u; xArea <= 1u; ++xArea)
		{
			for (unsigned int yArea = 0u; yArea <= 1u; ++yArea)
			{
				for (unsigned int zArea = 0u; zArea <= 1u; ++zArea)
				{
					newIndices.clear();

					for (VertexIndices::const_iterator i = indices.begin(); i != indices.end(); ++i)
					{
						const Triangle3& triangle = mesh.tracingTriangles[*i];

						if ((xArea == 0u && triangle.point0().x() <= center.x() && triangle.point1().x() <= center.x() && triangle.point2().x() <= center.x())
									|| (xArea == 1u && (triangle.point0().x() > center.x() || triangle.point1().x() > center.x() || triangle.point2().x() > center.x())))
						{
							if ((yArea == 0u && triangle.point0().y() <= center.y() && triangle.point1().y() <= center.y() && triangle.point2().y() <= center.y())
										|| (yArea == 1u && (triangle.point0().y() > center.y() || triangle.point1().y() > center.y() || triangle.point2().y() > center.y())))
							{
								if ((zArea == 0u && triangle.point0().z() <= center.z() && triangle.point1().z() <= center.z() && triangle.point2().z() <= center.z())
											|| (zArea == 1u && (triangle.point0().z() > center.z() || triangle.point1().z() > center.z() || triangle.point2().z() > center.z())))
								{
									newIndices.push_back(*i);
								}
							}
						}
					}

					if (newIndices.size() == indices.size())
					{
						ocean_assert_and_suppress_unused(debugTotal == 0, debugTotal);
						octreeIndices = indices;
						return;
					}

					debugTotal += newIndices.size();

					if (!newIndices.empty())
					{
						ocean_assert(usedSubsets < 8);

						octreeChildren[usedSubsets] = new Octree(newIndices, mesh);
						usedSubsets++;
					}
				}
			}
		}

		ocean_assert(debugTotal == indices.size());
	}
	else
	{
		octreeIndices = indices;
	}
}

void TracingMesh::Octree::findNearestIntersection(const Line3& ray, const HomogenousMatrix4& objectTransformation, const HomogenousMatrix4& invertedObjectTransformation, const TracingMesh& mesh, RayIntersection& intersection, const bool frontFace, const Scalar eps, const TracingObject* excludedObject) const
{
	ocean_assert(ray.isValid());

	if (!octreeBoundingBox.hasIntersection(ray))
	{
		return;
	}

	if (octreeIndices.empty())
	{
		for (unsigned int n = 0u; n < 8u && octreeChildren[n]; ++n)
		{
			octreeChildren[n]->findNearestIntersection(ray, objectTransformation, invertedObjectTransformation, mesh, intersection, frontFace, eps, excludedObject);
		}
	}
	else
	{
		Vector3 intersectionPoint;
		Vector3 intersectionBarycentric;
		Scalar intersectionDistance;

		for (VertexIndices::const_iterator i = octreeIndices.begin(); i != octreeIndices.end(); ++i)
		{
			ocean_assert(*i < mesh.tracingTriangles.size());
			const Triangle3& triangle = mesh.tracingTriangles[*i];

			if (triangle.intersection(ray, intersectionPoint, intersectionBarycentric, intersectionDistance) && intersectionDistance > eps && intersectionDistance < intersection.distance())
			{
				const Normal& normal0 = mesh.tracingNormals[*i * 3 + 0];
				const Normal& normal1 = mesh.tracingNormals[*i * 3 + 1];
				const Normal& normal2 = mesh.tracingNormals[*i * 3 + 2];

				const Normal normal((normal0 * intersectionBarycentric[0] + normal1 * intersectionBarycentric[1] + normal2 * intersectionBarycentric[2]).normalizedOrZero());
				ocean_assert(Numeric::isEqual(normal.length(), 1));

				const bool isFrontFace = normal * ray.direction() < 0;

				if (isFrontFace == frontFace)
				{
					TextureCoordinate textureCoordinate(0, 0);

					if (!mesh.tracingTextureCoordinates.empty())
					{
						const TextureCoordinate& coordinate0 = mesh.tracingTextureCoordinates[*i * 3 + 0];
						const TextureCoordinate& coordinate1 = mesh.tracingTextureCoordinates[*i * 3 + 1];
						const TextureCoordinate& coordinate2 = mesh.tracingTextureCoordinates[*i * 3 + 2];

						textureCoordinate = coordinate0 * intersectionBarycentric[0] + coordinate1 * intersectionBarycentric[1] + coordinate2 * intersectionBarycentric[2];
					}

					if (mesh.attributes_)
					{
						intersection = RayIntersection(intersectionPoint, ray.direction(), normal, textureCoordinate, intersectionDistance, &mesh, mesh.lightSources_);
					}
					else
					{
						intersection = RayIntersection(intersectionPoint, ray.direction(), normal, textureCoordinate, intersectionDistance, &mesh, mesh.lightSources_);
					}
				}
			}
		}
	}
}

bool TracingMesh::Octree::hasIntersection(const Line3& ray, const HomogenousMatrix4& objectTransformation, const HomogenousMatrix4& invertedObjectTransformation, const TracingMesh& mesh, const Scalar maximalDistance, const TracingObject* excludedObject) const
{
	ocean_assert(ray.isValid());

	if (octreeBoundingBox.hasIntersection(ray, invertedObjectTransformation))
	{
		Vector3 intersectionPoint;
		Scalar intersectionDistance;

		if (octreeIndices.empty())
		{
			for (unsigned int n = 0u; n < 8u && octreeChildren[n]; ++n)
			{
				if (octreeChildren[n]->hasIntersection(ray, objectTransformation, invertedObjectTransformation, mesh, maximalDistance, excludedObject))
				{
					return true;
				}
			}
		}
		else
		{
			for (VertexIndices::const_iterator i = octreeIndices.begin(); i != octreeIndices.end(); ++i)
			{
				ocean_assert(*i < mesh.tracingTriangles.size());
				const Triangle3& triangle = mesh.tracingTriangles[*i];

				if (triangle.intersection(ray, intersectionPoint, intersectionDistance) && intersectionDistance < maximalDistance)
				{
					return true;
				}
			}
		}
	}

	return false;
}

TracingMesh::Octree::~Octree()
{
	for (unsigned int n = 0u; n < 8u; ++n)
		delete octreeChildren[n];
}

TracingMesh::TracingMesh() :
	tracingOctree(nullptr)
{
	// nothing to do here
}

TracingMesh::~TracingMesh()
{
	delete tracingOctree;
}

void TracingMesh::setTriangles(const Vertices& vertices, const Normals& normals, const TextureCoordinates& textureCoordinates, const TriangleFaces& faces, const HomogenousMatrix4& objectTransformation, const BoundingBox& localBoundingBox)
{
	ocean_assert(tracingTriangles.empty());
	ocean_assert(tracingNormals.empty());

	objectTransformation_ = objectTransformation;
	invertedObjectTransformation_ = objectTransformation.inverted();

	tracingTriangles.reserve(faces.size());
	tracingNormals.reserve(faces.size() * 3);

	if (!textureCoordinates.empty())
		tracingTextureCoordinates.reserve(tracingNormals.size());

	if (normals.empty())
	{
		for (TriangleFaces::const_iterator i = faces.begin(); i != faces.end(); ++i)
		{
			const unsigned int index0 = i->index(0);
			const unsigned int index1 = i->index(1);
			const unsigned int index2 = i->index(2);

			ocean_assert(index0 < vertices.size());
			ocean_assert(index1 < vertices.size());
			ocean_assert(index2 < vertices.size());

			const Vertex& localVertex0 = vertices[index0];
			const Vertex& localVertex1 = vertices[index1];
			const Vertex& localVertex2 = vertices[index2];

			const Vertex vertex0(objectTransformation * localVertex0);
			const Vertex vertex1(objectTransformation * localVertex1);
			const Vertex vertex2(objectTransformation * localVertex2);
			tracingTriangles.push_back(Triangle3(vertex0, vertex1, vertex2));

			const Normal normal((vertex1 - vertex0).cross(vertex2 - vertex0).normalizedOrZero());
			tracingNormals.push_back(normal);
			tracingNormals.push_back(normal);
			tracingNormals.push_back(normal);

			if (!textureCoordinates.empty())
			{
				ocean_assert(index0 < textureCoordinates.size());
				ocean_assert(index1 < textureCoordinates.size());
				ocean_assert(index2 < textureCoordinates.size());

				tracingTextureCoordinates.push_back(textureCoordinates[index0]);
				tracingTextureCoordinates.push_back(textureCoordinates[index1]);
				tracingTextureCoordinates.push_back(textureCoordinates[index2]);
			}
		}
	}
	else
	{
		const SquareMatrix3 invertedTransposed(objectTransformation.rotationMatrix().inverted().transposed());

		for (TriangleFaces::const_iterator i = faces.begin(); i != faces.end(); ++i)
		{
			const unsigned int index0 = i->index(0);
			const unsigned int index1 = i->index(1);
			const unsigned int index2 = i->index(2);

			ocean_assert(index0 < vertices.size());
			ocean_assert(index1 < vertices.size());
			ocean_assert(index2 < vertices.size());

			const Vertex& localVertex0 = vertices[index0];
			const Vertex& localVertex1 = vertices[index1];
			const Vertex& localVertex2 = vertices[index2];

			const Vertex vertex0(objectTransformation * localVertex0);
			const Vertex vertex1(objectTransformation * localVertex1);
			const Vertex vertex2(objectTransformation * localVertex2);

			tracingTriangles.push_back(Triangle3(vertex0, vertex1, vertex2));

			ocean_assert(i->index(0) < normals.size());
			ocean_assert(i->index(1) < normals.size());
			ocean_assert(i->index(2) < normals.size());

			const Normal normal0((invertedTransposed * normals[index0]).normalizedOrZero());
			const Normal normal1((invertedTransposed * normals[index1]).normalizedOrZero());
			const Normal normal2((invertedTransposed * normals[index2]).normalizedOrZero());

			tracingNormals.push_back(normal0);
			tracingNormals.push_back(normal1);
			tracingNormals.push_back(normal2);

			if (!textureCoordinates.empty())
			{
				ocean_assert(index0 < textureCoordinates.size());
				ocean_assert(index1 < textureCoordinates.size());
				ocean_assert(index2 < textureCoordinates.size());

				tracingTextureCoordinates.push_back(textureCoordinates[index0]);
				tracingTextureCoordinates.push_back(textureCoordinates[index1]);
				tracingTextureCoordinates.push_back(textureCoordinates[index2]);
			}
		}
	}

	tracingLocalBoundingBox = localBoundingBox;
	tracingLocalBoundingSphere = BoundingSphere(localBoundingBox);

	ocean_assert(tracingNormals.size() == tracingTriangles.size() * 3);

	VertexIndices indices;
	indices.reserve(tracingTriangles.size());
	for (unsigned int n = 0u; n < tracingTriangles.size(); ++n)
	{
		indices.push_back(n);
	}

	delete tracingOctree;
	tracingOctree = new Octree(indices, *this);
}

void TracingMesh::setTriangleStrips(const Vertices& vertices, const Normals& normals, const TextureCoordinates& textureCoordinates, const VertexIndexGroups& indicesSet, const HomogenousMatrix4& objectTransformation, const BoundingBox& localBoundingBox)
{
	ocean_assert(tracingTriangles.empty());
	ocean_assert(tracingNormals.empty());

	setObjectTransformation(objectTransformation);

	if (normals.empty())
	{
		for (VertexIndexGroups::const_iterator i = indicesSet.begin(); i != indicesSet.end(); ++i)
		{
			const VertexIndices& indices = *i;
			ocean_assert(indices.size() >= 3);

			for (size_t n = 2; n < i->size(); n += 2)
			{
				size_t take0 = n - 2;
				size_t take1 = n - 1;
				size_t take2 = n + 0;

				ocean_assert(indices[take0] < vertices.size());
				ocean_assert(indices[take1] < vertices.size());
				ocean_assert(indices[take2] < vertices.size());

				size_t index0 = indices[take0];
				size_t index1 = indices[take1];
				size_t index2 = indices[take2];

				const Vertex& aLocalVertex0 = vertices[index0];
				const Vertex& aLocalVertex1 = vertices[index1];
				const Vertex& aLocalVertex2 = vertices[index2];

				const Vertex aVertex0(objectTransformation * aLocalVertex0);
				const Vertex aVertex1(objectTransformation * aLocalVertex1);
				const Vertex aVertex2(objectTransformation * aLocalVertex2);
				tracingTriangles.push_back(Triangle3(aVertex0, aVertex1, aVertex2));

				const Normal aNormal((aVertex1 - aVertex0).cross(aVertex2 - aVertex0).normalizedOrZero());
				tracingNormals.push_back(aNormal);
				tracingNormals.push_back(aNormal);
				tracingNormals.push_back(aNormal);

				if (!textureCoordinates.empty())
				{
					ocean_assert(index0 < textureCoordinates.size());
					ocean_assert(index1 < textureCoordinates.size());
					ocean_assert(index2 < textureCoordinates.size());

					tracingTextureCoordinates.push_back(textureCoordinates[index0]);
					tracingTextureCoordinates.push_back(textureCoordinates[index1]);
					tracingTextureCoordinates.push_back(textureCoordinates[index2]);
				}

				if (n + 1 >= i->size())
				{
					break;
				}

				take0 = n + 1;
				take1 = n + 0;
				take2 = n - 1;

				ocean_assert(indices[take0] < vertices.size());
				ocean_assert(indices[take1] < vertices.size());
				ocean_assert(indices[take2] < vertices.size());

				index0 = indices[take0];
				index1 = indices[take1];
				index2 = indices[take2];

				const Vertex& bLocalVertex0 = vertices[index0];
				const Vertex& bLocalVertex1 = vertices[index1];
				const Vertex& bLocalVertex2 = vertices[index2];

				const Vertex bVertex0(objectTransformation * bLocalVertex0);
				const Vertex bVertex1(objectTransformation * bLocalVertex1);
				const Vertex bVertex2(objectTransformation * bLocalVertex2);
				tracingTriangles.push_back(Triangle3(bVertex0, bVertex1, bVertex2));

				const Normal bNormal((bVertex1 - bVertex0).cross(bVertex2 - bVertex0).normalizedOrZero());
				tracingNormals.push_back(bNormal);
				tracingNormals.push_back(bNormal);
				tracingNormals.push_back(bNormal);

				if (!textureCoordinates.empty())
				{
					ocean_assert(index0 < textureCoordinates.size());
					ocean_assert(index1 < textureCoordinates.size());
					ocean_assert(index2 < textureCoordinates.size());

					tracingTextureCoordinates.push_back(textureCoordinates[index0]);
					tracingTextureCoordinates.push_back(textureCoordinates[index1]);
					tracingTextureCoordinates.push_back(textureCoordinates[index2]);
				}
			}
		}
	}
	else
	{
		const SquareMatrix3 invertedTransposed(objectTransformation.rotationMatrix().inverted().transposed());

		for (VertexIndexGroups::const_iterator i = indicesSet.begin(); i != indicesSet.end(); ++i)
		{
			const VertexIndices& indices = *i;
			ocean_assert(indices.size() >= 3);

			for (size_t n = 2; n < i->size(); n += 2)
			{
				size_t take0 = n - 2;
				size_t take1 = n - 1;
				size_t take2 = n + 0;

				ocean_assert(indices[take0] < vertices.size());
				ocean_assert(indices[take1] < vertices.size());
				ocean_assert(indices[take2] < vertices.size());

				size_t index0 = indices[take0];
				size_t index1 = indices[take1];
				size_t index2 = indices[take2];

				const Vertex& aLocalVertex0 = vertices[index0];
				const Vertex& aLocalVertex1 = vertices[index1];
				const Vertex& aLocalVertex2 = vertices[index2];

				const Vertex aVertex0(objectTransformation * aLocalVertex0);
				const Vertex aVertex1(objectTransformation * aLocalVertex1);
				const Vertex aVertex2(objectTransformation * aLocalVertex2);
				tracingTriangles.push_back(Triangle3(aVertex0, aVertex1, aVertex2));

				ocean_assert(indices[n - 2] < normals.size());
				ocean_assert(indices[n - 1] < normals.size());
				ocean_assert(indices[n + 0] < normals.size());

				const Normal aNormal0((invertedTransposed * normals[index0]).normalizedOrZero());
				const Normal aNormal1((invertedTransposed * normals[index1]).normalizedOrZero());
				const Normal aNormal2((invertedTransposed * normals[index2]).normalizedOrZero());

				tracingNormals.push_back(aNormal0);
				tracingNormals.push_back(aNormal1);
				tracingNormals.push_back(aNormal2);

				if (!textureCoordinates.empty())
				{
					ocean_assert(index0 < textureCoordinates.size());
					ocean_assert(index1 < textureCoordinates.size());
					ocean_assert(index2 < textureCoordinates.size());

					tracingTextureCoordinates.push_back(textureCoordinates[index0]);
					tracingTextureCoordinates.push_back(textureCoordinates[index1]);
					tracingTextureCoordinates.push_back(textureCoordinates[index2]);
				}

				if (n + 1 >= i->size())
					break;

				take0 = n + 1;
				take1 = n + 0;
				take2 = n - 1;

				index0 = indices[take0];
				index1 = indices[take1];
				index2 = indices[take2];

				ocean_assert(index0 < vertices.size());
				ocean_assert(index1 < vertices.size());
				ocean_assert(index2 < vertices.size());

				const Vertex& bLocalVertex0 = vertices[index0];
				const Vertex& bLocalVertex1 = vertices[index1];
				const Vertex& bLocalVertex2 = vertices[index2];

				const Vertex bVertex0(objectTransformation * bLocalVertex0);
				const Vertex bVertex1(objectTransformation * bLocalVertex1);
				const Vertex bVertex2(objectTransformation * bLocalVertex2);
				tracingTriangles.push_back(Triangle3(bVertex0, bVertex1, bVertex2));

				ocean_assert(index0 < normals.size());
				ocean_assert(index1 < normals.size());
				ocean_assert(index2 < normals.size());

				const Normal bNormal0((invertedTransposed * normals[index0]).normalizedOrZero());
				const Normal bNormal1((invertedTransposed * normals[index1]).normalizedOrZero());
				const Normal bNormal2((invertedTransposed * normals[index2]).normalizedOrZero());

				tracingNormals.push_back(bNormal0);
				tracingNormals.push_back(bNormal1);
				tracingNormals.push_back(bNormal2);

				if (!textureCoordinates.empty())
				{
					ocean_assert(index0 < textureCoordinates.size());
					ocean_assert(index1 < textureCoordinates.size());
					ocean_assert(index2 < textureCoordinates.size());

					tracingTextureCoordinates.push_back(textureCoordinates[index0]);
					tracingTextureCoordinates.push_back(textureCoordinates[index1]);
					tracingTextureCoordinates.push_back(textureCoordinates[index2]);
				}
			}
		}
	}

	tracingLocalBoundingBox = localBoundingBox;
	tracingLocalBoundingSphere = BoundingSphere(localBoundingBox);

	ocean_assert(tracingNormals.size() == tracingTriangles.size() * 3);

	VertexIndices indices;
	indices.reserve(tracingTriangles.size());
	for (unsigned int n = 0u; n < tracingTriangles.size(); ++n)
	{
		indices.push_back(n);
	}

	delete tracingOctree;
	tracingOctree = new Octree(indices, *this);
}

void TracingMesh::findNearestIntersection(const Line3& ray, RayIntersection& intersection, const bool frontFace, const Scalar eps, const TracingObject* excludedObject) const
{
	if (this == excludedObject)
	{
		return;
	}

	const Line3 localRay(invertedObjectTransformation_ * ray.point(), invertedObjectTransformation_.rotationMatrix(ray.direction()));
	if (!tracingLocalBoundingSphere.hasIntersection(localRay) || !tracingLocalBoundingBox.hasIntersection(localRay))
	{
		return;
	}

	ocean_assert(tracingOctree);
	tracingOctree->findNearestIntersection(ray, objectTransformation_, invertedObjectTransformation_, *this, intersection, frontFace, eps, excludedObject);
}

bool TracingMesh::hasIntersection(const Line3& ray, const Scalar maximalDistance, const TracingObject* excludedObject) const
{
	if (this == excludedObject)
	{
		return false;
	}

	const Line3 localRay(invertedObjectTransformation_ * ray.point(), invertedObjectTransformation_.rotationMatrix(ray.direction()));
	if (!tracingLocalBoundingSphere.hasIntersection(localRay) || !tracingLocalBoundingBox.hasIntersection(localRay))
	{
		return false;
	}

	ocean_assert(tracingOctree);
	return tracingOctree->hasIntersection(ray, objectTransformation_, invertedObjectTransformation_, *this, maximalDistance, excludedObject);
}

bool TracingMesh::determineDampingColor(const Line3& ray, RGBAColor& color, const Scalar maximalDistance) const
{
	ocean_assert(ray.isValid());

	const Line3 localRay(invertedObjectTransformation_ * ray.point(), invertedObjectTransformation_.rotationMatrix(ray.direction()));
	if (!tracingLocalBoundingSphere.hasIntersection(localRay) || !tracingLocalBoundingBox.hasIntersection(localRay))
	{
		return true;
	}

	RayIntersection intersection;

	ocean_assert(tracingOctree);
	tracingOctree->findNearestIntersection(ray, objectTransformation_, invertedObjectTransformation_, *this, intersection, true, Numeric::eps());
	if (!intersection || intersection.distance() >= maximalDistance)
	{
		return true;
	}

	if (!material_)
	{
		return false;
	}

	if (material_->transparencyInline() == 0)
	{
		return false;
	}

	const RGBAColor transmittedColor = color.damped(material_->transparencyInline());
	const RGBAColor opaqueColor = color * material_->diffuseColorInline().combined(material_->ambientColorInline()).damped(1.0f - material_->transparencyInline());

	color = transmittedColor.combined(opaqueColor).damped(material_->transparencyInline());

	return true;
}

bool TracingMesh::determineColor(const Vector3& viewPosition, const Vector3& viewObjectDirection, const RayIntersection& intersection, const TracingGroup& group, const unsigned int bounces, const TracingObject* /*excludedObject*/, const Lighting::LightingModes lightingModes, RGBAColor& color) const
{
	return Lighting::dampedLight(viewPosition, viewObjectDirection, intersection.position(), intersection.normal(), intersection.textureCoordinate(), material_, textures_, intersection.lightSources(), *this, group, bounces, lightingModes, color);
}

}

}

}
