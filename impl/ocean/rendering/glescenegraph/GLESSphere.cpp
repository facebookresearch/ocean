/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESSphere.h"
#include "ocean/rendering/glescenegraph/GLESFactory.h"
#include "ocean/rendering/glescenegraph/GLESVertexSet.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Manager.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESSphere::GLESSphere() :
	GLESShape(),
	GLESTriangleStrips(),
	Sphere()
{
	rebuildPrimitives();
}

GLESSphere::~GLESSphere()
{
	// nothing to do here
}

Scalar GLESSphere::radius() const
{
	return radius_;
}

bool GLESSphere::setRadius(const Scalar radius)
{
	if (radius_ <= 0)
	{
		return false;
	}

	if (radius_ == radius)
	{
		return true;
	}

	radius_ = radius;
	rebuildPrimitives();

	return true;
}

void GLESSphere::rebuildPrimitives()
{
#ifdef OCEAN_PLATFORM_BUILD_MOBILE
	constexpr unsigned int laSteps = 40u;
	constexpr unsigned int loSteps = 80u;
#else
	constexpr unsigned int laSteps = 20u;
	constexpr unsigned int loSteps = 40u;
#endif

	Vectors3 vertices;
	Vectors3 normals;
	Vectors2 textureCoordinates;

	const unsigned int elements = (laSteps + 1u) * (loSteps + 1u);

	vertices.reserve(elements);
	normals.reserve(elements);
	textureCoordinates.reserve(elements);

	const Scalar laFactor = Numeric::pi() / Scalar(laSteps);
	const Scalar loFactor = Numeric::pi2() / Scalar(loSteps);

	vertices.insert(vertices.end(), loSteps + 1, Vertex(0, radius_, 0));
	normals.insert(normals.end(), loSteps + 1, Normal(0, 1, 0));

	for (unsigned int la = 1u; la < laSteps; la++)
	{
		for (unsigned int lo = 0u; lo < loSteps; lo++)
		{
			const Normal normal = vertex(Numeric::pi_2() - Scalar(la) * laFactor, Scalar(lo) * loFactor);

			vertices.emplace_back(normal * radius_);
			normals.emplace_back(normal);
		}

		const Normal normal = vertex(Numeric::pi_2() - Scalar(la) * laFactor, 0);
		vertices.emplace_back(normal * radius_);
		normals.emplace_back(normal);
	}

	vertices.insert(vertices.end(), loSteps + 1, Vertex(0, -radius_, 0));
	normals.insert(normals.end(), loSteps + 1, Normal(0, -1, 0));

	for (unsigned int la = 0u; la <= laSteps; la++)
	{
		for (unsigned int lo = 0u; lo <= loSteps; lo++)
		{
			textureCoordinates.emplace_back(Scalar(lo) / Scalar(loSteps), Scalar(1) - Scalar(la) / Scalar(laSteps));
		}
	}

	ocean_assert(vertices.size() == elements);
	ocean_assert(normals.size() == elements);
	ocean_assert(textureCoordinates.size() == elements);

	VertexIndexGroups strips;
	strips.reserve(laSteps);

	for (unsigned int n = 0u; n < loSteps; n++)
	{
		VertexIndices strip;
		strip.reserve(((laSteps - 2u) * 2u) + 4u);

		strip.emplace_back(n);

		for (unsigned int i = 0u; i < laSteps; i++)
		{
			strip.emplace_back(n + i * (loSteps + 1u));
			ocean_assert(strip.back() < elements);

			strip.emplace_back(strip.back() + 1u);
			ocean_assert(strip.back() < elements);
		}

		strip.emplace_back(n + (loSteps + 1u) * laSteps);
		ocean_assert(strip.back() < elements);

		strips.emplace_back(strip);
	}

	shapeVertexSet->setVertices(vertices);
	shapeVertexSet->setNormals(normals);
	shapeVertexSet->setTextureCoordinates(textureCoordinates, 0);

	setStrips(strips);
	setVertexSet(shapeVertexSet);
}

void GLESSphere::updateBoundingBox()
{
	const Scalar diamenter = radius_ * Scalar(2);
	boundingBox_ = BoundingBox(Box3(Vector3(0, 0, 0), diamenter, diamenter, diamenter));
}

GLESSphere::ObjectType GLESSphere::type() const
{
	return TYPE_SPHERE;
}

}

}

}
