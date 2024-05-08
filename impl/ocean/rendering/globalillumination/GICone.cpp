/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GICone.h"

#include "ocean/rendering/Engine.h"

#include "ocean/system/Performance.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GICone::GICone() :
	GIShape(),
	GITriangles(),
	Cone(),
	coneHeight(1),
	coneRadius(1)
{
	rebuildPrimitives();
}

GICone::~GICone()
{
	// nothing to do here
}

Scalar GICone::radius() const
{
	return coneRadius;
}

Scalar GICone::height() const
{
	return coneHeight;
}

bool GICone::setRadius(const Scalar radius)
{
	if (radius <= 0)
		return false;

	if (radius == coneRadius)
		return true;

	coneRadius = radius;
	rebuildPrimitives();
	return true;
}

bool GICone::setHeight(const Scalar height)
{
	if (height <= 0)
		return false;

	if (height == coneHeight)
		return true;

	coneHeight = height;
	rebuildPrimitives();
	return true;
}

BoundingBox GICone::boundingBox() const
{
	return BoundingBox(Vector3(-coneRadius, -coneHeight * Scalar(0.5), -coneRadius), Vector3(coneRadius, coneHeight * Scalar(0.5), coneRadius));
}

BoundingSphere GICone::boundingSphere() const
{
	return BoundingSphere(boundingBox());
}

void GICone::rebuildPrimitives()
{
	static const unsigned int steps = System::Performance::get().performanceLevel() >= System::Performance::LEVEL_VERY_HIGH ? 100 : 50;

	Vertices vertices;
	Normals normals;
	TextureCoordinates textureCoordinates;

	// Although the first and the last vertices are identical they have different texture coordinates
	// Therefore, they have to be defined explicitly resulting in two additional elements

	const unsigned int side = (steps << 1) + 1; // steps * 2 + 1
	const unsigned int bottom = steps;
	const unsigned int elements = side + bottom;

	vertices.reserve(elements);
	normals.reserve(elements);
	textureCoordinates.reserve(elements);

	const Scalar angleFactor = Numeric::pi2() / Scalar(steps);
	const Scalar textureFactor = Scalar(1) / Scalar(steps);
	const Scalar height2 = coneHeight * Scalar(0.5);

	const Scalar normalY = Numeric::sin(Numeric::atan2(coneRadius, coneHeight));

	// vertices for the side
	for (unsigned int n = 0u; n < steps; n++)
	{
		const Scalar angle = Scalar(n) * angleFactor;
		const Scalar angleHalf = angle + angleFactor * Scalar(0.5);

		const Scalar valueX = Numeric::sin(angle);
		const Scalar valueZ = Numeric::cos(angle);

		vertices.push_back(Vertex(0, height2, 0));
		vertices.push_back(Vertex(valueX * coneRadius, -height2, valueZ * coneRadius));

		Normal normal(Numeric::sin(angleHalf), normalY, Numeric::cos(angleHalf));
		normals.push_back(normal.normalized());
		normal = Normal(Normal(valueX, normalY, valueZ));
		normals.push_back(normal.normalized());

		textureCoordinates.push_back(TextureCoordinate(textureFactor * (Scalar(n) + Scalar(0.5)), 1));
		textureCoordinates.push_back(TextureCoordinate(textureFactor * Scalar(n), 0));
	}
	vertices.push_back(vertices[1]);
	normals.push_back(normals[1]);
	textureCoordinates.push_back(TextureCoordinate(1, 0));

	// vertices for the bottom
	for (unsigned int n = 0u; n < steps; n++)
	{
		const Scalar angle = Scalar(n) * angleFactor;

		vertices.push_back(vertices[(n << 1u) + 1u]); // n * 2 + 1
		normals.push_back(Normal(0, -1, 0));

		textureCoordinates.push_back(TextureCoordinate(Scalar(0.5) + Numeric::sin(angle) * Scalar(0.5), Scalar(0.5) + Numeric::cos(angle) * Scalar(0.5)));
	}

	const VertexSetRef vertexSet = engine().factory().createVertexSet();
	vertexSet->setVertices(vertices);
	vertexSet->setNormals(normals);
	vertexSet->setTextureCoordinates(textureCoordinates, 0);

	ocean_assert(vertices.size() == elements);
	ocean_assert(normals.size() == elements);
	ocean_assert(textureCoordinates.size() == elements);

	const unsigned int numberFaces = (steps << 1) - 2; // steps * 2 - 2;

	TriangleFaces faces;
	faces.reserve(numberFaces);

	// triangle faces for the side
	unsigned int index = 0;
	for (unsigned int n = 0; n < steps - 1; n++)
	{
		faces.push_back(TriangleFace(index, index + 1, index + 3));
		index += 2;
	}
	faces.push_back(TriangleFace(index, index + 1, index + 2));

	// triangle faces for the bottom
	for (unsigned int n = side + 1; n < side + bottom - 1; n++)
		faces.push_back(TriangleFace(side, n + 1, n));

	ocean_assert(numberFaces == faces.size());

	setFaces(faces);
	setVertexSet(vertexSet);
}

GICone::ObjectType GICone::type() const
{
	return TYPE_CONE;
}

}

}

}
