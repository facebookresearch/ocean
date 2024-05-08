/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GITriangleStrips.h"
#include "ocean/rendering/globalillumination/GIVertexSet.h"
#include "ocean/rendering/globalillumination/TracingMesh.h"

#include "ocean/math/Triangle3.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GITriangleStrips::GITriangleStrips() :
	GIPrimitive(),
	TriangleStrips(),
	triangleStripsNumberStrips(0),
	triangleStripsNumberIndices(0)
{
	// nothing to do here
}

GITriangleStrips::~GITriangleStrips()
{
	// nothing to do here
}

unsigned int GITriangleStrips::numberStrips() const
{
	return triangleStripsNumberStrips;
}

BoundingBox GITriangleStrips::boundingBox() const
{
	const ScopedLock scopedLock(objectLock);

	if (primitiveStrips.empty())
		return BoundingBox();

	if (primitiveVertexSet.isNull())
		return BoundingBox();

	BoundingBox boundingBox;
	const Vertices vertices(primitiveVertexSet->vertices());

	for (VertexIndexGroups::const_iterator i = primitiveStrips.begin(); i != primitiveStrips.end(); ++i)
	{
		const VertexIndices& indices = *i;

		for (VertexIndices::const_iterator n = indices.begin(); n != indices.end(); ++n)
		{
			ocean_assert(*n < vertices.size());

			boundingBox += vertices[*n];
		}
	}

	return boundingBox;
}

BoundingSphere GITriangleStrips::boundingSphere() const
{
	return BoundingSphere(boundingBox());
}

void GITriangleStrips::setStrips(const VertexIndexGroups& strips)
{
	const ScopedLock scopedLock(objectLock);
	primitiveStrips = strips;
}

void GITriangleStrips::buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const AttributeSetRef& attributes, const LightSources& lightSources) const
{
	TracingMesh* mesh = new TracingMesh();

	const SmartObjectRef<GIVertexSet> vertexSet(primitiveVertexSet);
	ocean_assert(vertexSet);

	mesh->setTriangleStrips(vertexSet->vertices(), vertexSet->normals(), vertexSet->textureCoordinates(0), primitiveStrips, modelTransform, boundingBox());
	mesh->setLightSources(lightSources);
	mesh->setAttributes(attributes);

	group.addObject(mesh);
}

}

}

}
