/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESCylinder.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESCylinder::GLESCylinder() :
	GLESShape(),
	GLESTriangleStrips(),
	Cylinder()
{
	rebuildPrimitives();
}

GLESCylinder::~GLESCylinder()
{
	// nothing to do here
}

Scalar GLESCylinder::height() const
{
	return height_;
}

Scalar GLESCylinder::radius() const
{
	return radius_;
}

bool GLESCylinder::setHeight(const Scalar height)
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

bool GLESCylinder::setRadius(const Scalar radius)
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

void GLESCylinder::rebuildPrimitives()
{
#ifdef OCEAN_PLATFORM_BUILD_MOBILE
	constexpr unsigned int steps = 40u;
#else
	constexpr unsigned int steps = 80u;
#endif

	Vectors3 vertices;
	Vectors3 normals;
	Vectors2 textureCoordinates;

	// Although the first and the last vertices are identical they have different texture coordinates
	// Therefore, they have to be defined explicitly resulting in two additional elements

	const unsigned int side = steps * 2u + 2u;
	const unsigned int top = steps;
	const unsigned int bottom = top;
	const unsigned int elements = side + top + bottom;

	vertices.reserve(elements);
	normals.reserve(elements);
	textureCoordinates.reserve(elements);

	const Scalar angleFactor = Numeric::pi2() / Scalar(steps);
	const Scalar textureFactor = Scalar(1) / Scalar(steps);
	const Scalar height_2 = height_ * Scalar(0.5);

	// vertices for the side
	for (unsigned int n = 0u; n <= steps; n++)
	{
		const Scalar angle = Scalar(n) * angleFactor;
		const Normal normal(Numeric::sin(angle), 0, Numeric::cos(angle));

		vertices.emplace_back(normal.x() * radius_, height_2, normal.z() * radius_);
		vertices.emplace_back(normal.x() * radius_, -height_2, normal.z() * radius_);

		normals.insert(normals.end(), 2, normal);

		const Scalar texX = textureFactor * Scalar(n);
		textureCoordinates.emplace_back(Scalar(texX), Scalar(1));
		textureCoordinates.emplace_back(Scalar(texX), Scalar(0));
	}

	// vertices for the top
	for (unsigned int n = 0u; n < steps; n++)
	{
		const Scalar angle = Scalar(n) * angleFactor;

		vertices.emplace_back(vertices[2u * n]);
		normals.emplace_back(Scalar(0), Scalar(1), Scalar(0));
		textureCoordinates.emplace_back(Scalar(0.5) + Numeric::sin(angle) * Scalar(0.5), Scalar(0.5) - Numeric::cos(angle) * Scalar(0.5));
	}

	// vertices for the bottom
	for (unsigned int n = 0u; n < steps; n++)
	{
		const Scalar angle = Scalar(n) * angleFactor;

		vertices.emplace_back(vertices[2 * n + 1]);
		normals.emplace_back(Scalar(0), Scalar(-1), Scalar(0));

		textureCoordinates.emplace_back(Scalar(0.5) + Numeric::sin(angle) * Scalar(0.5), Scalar(0.5) + Numeric::cos(angle) * Scalar(0.5));
	}

	shapeVertexSet->setVertices(vertices);
	shapeVertexSet->setNormals(normals);
	shapeVertexSet->setTextureCoordinates(textureCoordinates, 0);

	ocean_assert(vertices.size() == elements);
	ocean_assert(normals.size() == elements);
	ocean_assert(textureCoordinates.size() == elements);

	VertexIndexGroups strips(3);

	// strip for the side
	strips[0].reserve(side);
	for (unsigned int n = 0; n < side; n++)
	{
		strips[0].emplace_back(n);
	}

	// strip for top
	strips[1].reserve(top);
	unsigned int low = side;
	unsigned int high = side + top;

	strips[1].emplace_back(low);
	while (true)
	{
		++low;
		if (low == high)
		{
			break;
		}

		strips[1].emplace_back(low);

		--high;
		if (low == high)
		{
			break;
		}

		strips[1].emplace_back(high);
	}

	// strop for bottom
	strips[2].reserve(bottom);
	low = side + top;
	high = side + top + bottom;

	strips[2].emplace_back(low);
	while (true)
	{
		--high;
		if (low == high)
		{
			break;
		}

		strips[2].emplace_back(high);

		++low;
		if (low == high)
		{
			break;
		}

		strips[2].emplace_back(low);
	}

	setStrips(strips);
	setVertexSet(shapeVertexSet);
}

void GLESCylinder::updateBoundingBox()
{
	const Scalar diamenter = radius_ * Scalar(2);
	boundingBox_ = BoundingBox(Box3(Vector3(0, 0, 0), diamenter, height_, diamenter));
}

GLESCylinder::ObjectType GLESCylinder::type() const
{
	return TYPE_CYLINDER;
}

}

}

}
