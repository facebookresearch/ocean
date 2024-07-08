/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GITriangles.h"
#include "ocean/rendering/globalillumination/GIVertexSet.h"
#include "ocean/rendering/globalillumination/TracingMesh.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GITriangles::GITriangles()
{
	// nothing to do here
}

GITriangles::~GITriangles()
{
	// nothing to do here
}

TriangleFaces GITriangles::faces() const
{
	return trianglesFaces;
}

unsigned int GITriangles::numberFaces() const
{
	return (unsigned int)trianglesFaces.size();
}

BoundingBox GITriangles::boundingBox() const
{
	const ScopedLock scopedLock(objectLock);

	if (trianglesFaces.empty())
		return BoundingBox();

	if (primitiveVertexSet.isNull())
		return BoundingBox();

	BoundingBox boundingBox;
	const Vertices vertices(primitiveVertexSet->vertices());

	for (TriangleFaces::const_iterator i = trianglesFaces.begin(); i != trianglesFaces.end(); ++i)
	{
		ocean_assert(i->index(0) < vertices.size());
		ocean_assert(i->index(1) < vertices.size());
		ocean_assert(i->index(2) < vertices.size());

		boundingBox += vertices[i->index(0)];
		boundingBox += vertices[i->index(1)];
		boundingBox += vertices[i->index(2)];
	}

	return boundingBox;
}

BoundingSphere GITriangles::boundingSphere() const
{
	return BoundingSphere(boundingBox());
}

void GITriangles::setFaces(const TriangleFaces& faces)
{
	const ScopedLock scopedLock(objectLock);
	trianglesFaces = faces;
}

void GITriangles::buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const AttributeSetRef& attributes, const LightSources& lightSources) const
{
	TracingMesh* mesh = new TracingMesh();

	const SmartObjectRef<GIVertexSet> vertexSet(primitiveVertexSet);
	ocean_assert(vertexSet);

	mesh->setTriangles(vertexSet->vertices(), vertexSet->normals(), vertexSet->textureCoordinates(0), trianglesFaces, modelTransform, boundingBox());
	mesh->setLightSources(lightSources);
	mesh->setAttributes(attributes);

	group.addObject(mesh);
}

}

}

}
