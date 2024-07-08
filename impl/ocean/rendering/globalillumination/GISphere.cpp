/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GISphere.h"
#include "ocean/rendering/globalillumination/TracingSphere.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Manager.h"

#include "ocean/system/Performance.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GISphere::GISphere() :
	GIShape(),
	GITriangleStrips(),
	Sphere(),
	sphereRadius(1)
{
	rebuildPrimitives();
}

GISphere::~GISphere()
{
	// nothing to do here
}

Scalar GISphere::radius() const
{
	return sphereRadius;
}

bool GISphere::setRadius(const Scalar radius)
{
	if (sphereRadius <= 0)
		return false;

	if (sphereRadius == radius)
		return true;

	sphereRadius = radius;
	rebuildPrimitives();

	return true;
}

BoundingBox GISphere::boundingBox() const
{
	return BoundingBox(Vector3(-sphereRadius, -sphereRadius, -sphereRadius), Vector3(sphereRadius, sphereRadius, sphereRadius));
}

BoundingSphere GISphere::boundingSphere() const
{
	return BoundingSphere(Vector3(0, 0, 0), sphereRadius);
}

void GISphere::rebuildPrimitives()
{
	static const unsigned int laSteps = System::Performance::get().performanceLevel() >= System::Performance::LEVEL_VERY_HIGH ? 40 : 20;
	static const unsigned int loSteps = System::Performance::get().performanceLevel() >= System::Performance::LEVEL_VERY_HIGH ? 80 : 40;

	Vertices vertices;
	Normals normals;
	TextureCoordinates textureCoordinates;

	const unsigned int elements = (laSteps + 1) * (loSteps + 1);

	vertices.reserve(elements);
	normals.reserve(elements);
	textureCoordinates.reserve(elements);

	const Scalar laFactor = Numeric::pi() / Scalar(laSteps);
	const Scalar loFactor = Numeric::pi2() / Scalar(loSteps);

	vertices.insert(vertices.end(), loSteps + 1, Vertex(0, sphereRadius, 0));
	normals.insert(normals.end(), loSteps + 1, Normal(0, 1, 0));

	for (unsigned int la = 1u; la < laSteps; la++)
	{
		for (unsigned int lo = 0u; lo < loSteps; lo++)
		{
			const Normal normal = vertex(Numeric::pi_2() - Scalar(la) * laFactor, Scalar(lo) * loFactor);

			vertices.push_back(normal * sphereRadius);
			normals.push_back(normal);
		}

		const Normal normal = vertex(Numeric::pi_2() - Scalar(la) * laFactor, 0);
		vertices.push_back(normal * sphereRadius);
		normals.push_back(normal);
	}

	vertices.insert(vertices.end(), loSteps + 1, Vertex(0, -sphereRadius, 0));
	normals.insert(normals.end(), loSteps + 1, Normal(0, -1, 0));

	for (unsigned int la = 0; la <= laSteps; la++)
	{
		for (unsigned int lo = 0; lo <= loSteps; lo++)
		{
			const TextureCoordinate coordinate(Scalar(lo) / Scalar(loSteps), 1 - Scalar(la) / Scalar(laSteps));
			textureCoordinates.push_back(coordinate);
		}
	}

	ocean_assert(vertices.size() == elements);
	ocean_assert(normals.size() == elements);
	ocean_assert(textureCoordinates.size() == elements);

	VertexIndexGroups strips;
	strips.reserve(laSteps);

	for (unsigned int n = 0; n < loSteps; n++)
	{
		VertexIndices strip;
		strip.reserve(((laSteps - 2) << 1) + 4);

		strip.push_back(n);

		for (unsigned int i = 0; i < laSteps; i++)
		{
			strip.push_back(n + i * (loSteps + 1));
			ocean_assert(strip.back() < elements);
			strip.push_back(strip.back() + 1);
			ocean_assert(strip.back() < elements);
		}

		strip.push_back(n + (loSteps + 1) * laSteps);
		ocean_assert(strip.back() < elements);

		strips.push_back(strip);
	}

	const VertexSetRef vertexSet = engine().factory().createVertexSet();
	vertexSet->setVertices(vertices);
	vertexSet->setNormals(normals);
	vertexSet->setTextureCoordinates(textureCoordinates, 0);

	setStrips(strips);
	setVertexSet(vertexSet);
}

GISphere::ObjectType GISphere::type() const
{
	return TYPE_SPHERE;
}

void GISphere::buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const AttributeSetRef& attributes, const LightSources& lightSources) const
{
	TracingSphere* sphere = new TracingSphere();

	sphere->setSphere(modelTransform, boundingSphere());
	sphere->setLightSources(lightSources);
	sphere->setAttributes(attributes);

	group.addObject(sphere);
}

}

}

}
