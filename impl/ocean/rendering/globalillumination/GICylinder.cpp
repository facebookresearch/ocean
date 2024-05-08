/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GICylinder.h"

#include "ocean/rendering/Engine.h"

#include "ocean/system/Performance.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GICylinder::GICylinder() :
	GIShape(),
	GITriangleStrips(),
	Cylinder(),
	cylinderHeight(1),
	cylinderRadius(1),
	cylinderTop(true),
	cylinderSide(true),
	cylinderBottom(true)
{
	rebuildPrimitives();
}

GICylinder::~GICylinder()
{
	// nothing to do here
}

Scalar GICylinder::height() const
{
	return cylinderHeight;
}

Scalar GICylinder::radius() const
{
	return cylinderRadius;
}

bool GICylinder::setHeight(const Scalar height)
{
	if (height <= 0)
		return false;

	if (height == cylinderHeight)
		return true;

	cylinderHeight = height;
	rebuildPrimitives();

	return true;
}

bool GICylinder::setRadius(const Scalar radius)
{
	if (radius <= 0)
		return false;

	if (radius == cylinderRadius)
		return true;

	cylinderRadius = radius;
	rebuildPrimitives();

	return true;
}

bool GICylinder::setTop(const bool visible)
{
	if (cylinderTop == visible)
		return true;

	cylinderTop = visible;
	rebuildPrimitives();

	return true;
}

bool GICylinder::setSide(const bool visible)
{
	if (cylinderSide == visible)
		return true;

	cylinderSide = visible;
	rebuildPrimitives();

	return true;
}

bool GICylinder::setBottom(const bool visible)
{
	if (cylinderBottom == visible)
		return true;

	cylinderBottom = visible;
	rebuildPrimitives();

	return true;
}

BoundingBox GICylinder::boundingBox() const
{
	return BoundingBox(Vector3(-cylinderRadius, -cylinderHeight * Scalar(0.5), -cylinderRadius), Vector3(cylinderRadius, cylinderHeight * Scalar(0.5), cylinderRadius));
}

BoundingSphere GICylinder::boundingSphere() const
{
	return BoundingSphere(boundingBox());
}

void GICylinder::rebuildPrimitives()
{
	static const unsigned int steps = System::Performance::get().performanceLevel() >= System::Performance::LEVEL_VERY_HIGH ? 80 : 40;

	Vertices topRingVertices;
	Vertices bottomRingVertices;
	Vertices sideNormals;

	static const Scalar angleFactor = Numeric::pi2() / Scalar(steps);

	const Scalar height2 = cylinderHeight * Scalar(0.5);

	topRingVertices.reserve(steps);
	bottomRingVertices.reserve(steps);
	sideNormals.reserve(steps);

	for (unsigned int n = 0u; n < steps; ++n)
	{
		const Scalar angle = Scalar(n) * angleFactor;
		const Normal normal(sin(angle), 0, cos(angle));

		topRingVertices.push_back(Vertex(normal.x() * cylinderRadius, height2, normal.z() * cylinderRadius));
		bottomRingVertices.push_back(Vertex(normal.x() * cylinderRadius, -height2, normal.z() * cylinderRadius));

		sideNormals.push_back(normal);
	}

	// Although the first and the last vertices are identical they have different texture coordinates
	// Therefore, they have to be defined explicitly resulting in two additional elements

	const unsigned int side = cylinderSide ? (steps << 1u) + 2u : 0u; // (steps + 1) * 2
	const unsigned int top = cylinderTop ? steps : 0u;
	const unsigned int bottom = cylinderBottom ? steps : 0u;
	const unsigned int elements = side + top + bottom;

	unsigned int numberStrips = 0u;

	if (cylinderSide)
		++numberStrips;
	if (cylinderTop)
		++numberStrips;
	if (cylinderBottom)
		++numberStrips;

	Vertices vertices;
	Normals normals;
	TextureCoordinates textureCoordinates;

	vertices.reserve(elements);
	normals.reserve(elements);
	textureCoordinates.reserve(elements);

	VertexIndexGroups strips(numberStrips);
	unsigned int stripIndex = 0u;

	if (cylinderSide)
	{
		static const Scalar textureFactor = Scalar(1) / Scalar(steps);

		// vertices for the side
		for (unsigned int n = 0; n < steps; n++)
		{
			vertices.push_back(topRingVertices[n]);
			vertices.push_back(bottomRingVertices[n]);

			normals.insert(normals.end(), 2, sideNormals[n]);

			const Scalar texX = textureFactor * Scalar(n);
			textureCoordinates.push_back(TextureCoordinate(texX, 1));
			textureCoordinates.push_back(TextureCoordinate(texX, 0));
		}

		vertices.push_back(topRingVertices.front());
		vertices.push_back(bottomRingVertices.front());

		normals.insert(normals.end(), 2, sideNormals.front());

		textureCoordinates.push_back(TextureCoordinate(1, 1));
		textureCoordinates.push_back(TextureCoordinate(1, 0));

		// strip for the side
		strips[stripIndex].reserve(side);
		for (unsigned int n = 0; n < side; n++)
			strips[stripIndex].push_back(n);

		++stripIndex;
	}

	if (cylinderTop)
	{
		// vertices for the top
		for (unsigned int n = 0u; n < steps; n++)
		{
			const Scalar angle = Scalar(n) * angleFactor;

			vertices.push_back(topRingVertices[n]);
			normals.push_back(Normal(0, 1, 0));
			textureCoordinates.push_back(TextureCoordinate(Scalar(0.5) + sin(angle) * Scalar(0.5), Scalar(0.5) - cos(angle) * Scalar(0.5)));
		}

		// strip for top
		strips[stripIndex].reserve(top);
		unsigned int low = side;
		unsigned int high = side + top;

		strips[stripIndex].push_back(low);
		while (true)
		{
			++low;
			if (low == high)
				break;

			strips[stripIndex].push_back(low);

			--high;
			if (low == high)
				break;

			strips[stripIndex].push_back(high);
		}

		++stripIndex;
	}

	if (cylinderBottom)
	{
		// vertices for the bottom
		for (unsigned int n = 0u; n < steps; n++)
		{
			const Scalar angle = Scalar(n) * angleFactor;

			vertices.push_back(bottomRingVertices[n]);
			normals.push_back(Normal(0, -1, 0));

			textureCoordinates.push_back(TextureCoordinate(Scalar(0.5) + sin(angle) * Scalar(0.5), Scalar(0.5) + cos(angle) * Scalar(0.5)));
		}

		// strip for bottom
		strips[stripIndex].reserve(bottom);
		unsigned int low = side + top;
		unsigned int high = side + top + bottom;

		strips[stripIndex].push_back(low);
		while (true)
		{
			--high;
			if (low == high)
				break;

			strips[stripIndex].push_back(high);

			++low;
			if (low == high)
				break;

			strips[stripIndex].push_back(low);
		}

		++stripIndex;
	}

	if (vertices.empty())
	{
		vertices.insert(vertices.end(), 3, Vector3(0, 0, 0));

		ocean_assert(strips.empty());

		VertexIndices indices;
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);

		strips.push_back(indices);

		ocean_assert(normals.empty());
		ocean_assert(textureCoordinates.empty());
	}

	const VertexSetRef vertexSet = engine().factory().createVertexSet();
	vertexSet->setVertices(vertices);
	vertexSet->setNormals(normals);
	vertexSet->setTextureCoordinates(textureCoordinates, 0);

	setStrips(strips);
	setVertexSet(vertexSet);
}

GICylinder::ObjectType GICylinder::type() const
{
	return TYPE_CYLINDER;
}

}

}

}
