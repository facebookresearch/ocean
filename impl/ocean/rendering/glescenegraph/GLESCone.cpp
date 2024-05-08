/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESCone.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESCone::GLESCone() :
	GLESShape(),
	GLESTriangles(),
	Cone()
{
	rebuildPrimitives();
}

GLESCone::~GLESCone()
{
	// nothing to do here
}

Scalar GLESCone::radius() const
{
	return radius_;
}

Scalar GLESCone::height() const
{
	return height_;
}

bool GLESCone::setRadius(const Scalar radius)
{
	if (radius <= 0)
	{
		return false;
	}

	if (radius == radius_)
	{
		return true;
	}

	radius_ = radius;
	rebuildPrimitives();

	return true;
}

bool GLESCone::setHeight(const Scalar height)
{
	if (height <= 0)
	{
		return false;
	}

	if (height == height_)
	{
		return true;
	}

	height_ = height;
	rebuildPrimitives();

	return true;
}

void GLESCone::rebuildPrimitives()
{
#ifdef OCEAN_PLATFORM_BUILD_MOBILE
	constexpr unsigned int steps = 50u;
#else
	constexpr unsigned int steps = 100u;
#endif

	Vectors3 vertices;
	Vectors3 normals;
	Vectors2 textureCoordinates;

	// Although the first and the last vertices are identical they have different texture coordinates
	// Therefore, they have to be defined explicitly resulting in two additional elements

	const unsigned int side = steps * 2u + 1u;
	const unsigned int bottom = steps;
	const unsigned int elements = side + bottom;

	vertices.reserve(elements);
	normals.reserve(elements);
	textureCoordinates.reserve(elements);

	const Scalar angleFactor = Numeric::pi2() / Scalar(steps);
	const Scalar textureFactor = Scalar(1) / Scalar(steps);
	const Scalar height_2 = height_ * Scalar(0.5);

	const Scalar normalY = Numeric::sin(Numeric::atan2(radius_, height_));

	// vertices for the side
	for (unsigned int n = 0u; n < steps; n++)
	{
		const Scalar angle = Scalar(n) * angleFactor;
		const Scalar angleHalf = angle + angleFactor * Scalar(0.5);

		const Scalar valueX = Numeric::sin(angle);
		const Scalar valueZ = Numeric::cos(angle);

		vertices.emplace_back(Scalar(0), Scalar(height_2), Scalar(0));
		vertices.emplace_back(valueX * radius_, -height_2, valueZ * radius_);

		Normal normal(Numeric::sin(angleHalf), normalY, Numeric::cos(angleHalf));
		normals.emplace_back(normal.normalized());

		normal = Normal(Normal(valueX, normalY, valueZ));
		normals.emplace_back(normal.normalized());

		textureCoordinates.emplace_back(textureFactor * (Scalar(n) + Scalar(0.5)), Scalar(1));
		textureCoordinates.emplace_back(textureFactor * Scalar(n), Scalar(0));
	}
	vertices.emplace_back(vertices[1]);
	normals.emplace_back(normals[1]);
	textureCoordinates.emplace_back(Scalar(1), Scalar(0));

	// vertices for the bottom
	for (unsigned int n = 0u; n < steps; n++)
	{
		const Scalar angle = Scalar(n) * angleFactor;

		vertices.emplace_back(vertices[n * 2u + 1u]);
		normals.emplace_back(Scalar(0), Scalar(-1), Scalar(0));

		textureCoordinates.emplace_back(Scalar(0.5) + Numeric::sin(angle) * Scalar(0.5), Scalar(0.5) + Numeric::cos(angle) * Scalar(0.5));
	}

	ocean_assert(vertices.size() == elements);
	ocean_assert(normals.size() == elements);
	ocean_assert(textureCoordinates.size() == elements);

	const unsigned int numberFaces = steps * 2u - 2u;

	TriangleFaces triangleFaces;
	triangleFaces.reserve(numberFaces);

	// triangle faces for the side
	unsigned int index = 0u;
	for (unsigned int n = 0u; n < steps - 1u; n++)
	{
		triangleFaces.emplace_back(index, index + 1u, index + 3u);
		index += 2u;
	}
	triangleFaces.emplace_back(index, index + 1u, index + 2u);

	// triangle faces for the bottom
	for (unsigned int n = side + 1u; n < side + bottom - 1u; n++)
	{
		triangleFaces.emplace_back(side, n + 1u, n);
	}

	ocean_assert(numberFaces == triangleFaces.size());

	shapeVertexSet->setVertices(vertices);
	shapeVertexSet->setNormals(normals);
	shapeVertexSet->setTextureCoordinates(textureCoordinates, 0);

	setFaces(triangleFaces);
	setVertexSet(shapeVertexSet);
}

void GLESCone::updateBoundingBox()
{
	const Scalar diamenter = radius_ * Scalar(2);
	boundingBox_ = BoundingBox(Box3(Vector3(0, 0, 0), diamenter, height_, diamenter));
}

GLESCone::ObjectType GLESCone::type() const
{
	return TYPE_CONE;
}

}

}

}
