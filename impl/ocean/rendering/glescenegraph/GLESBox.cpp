/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/glescenegraph/GLESBox.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

GLESBox::GLESBox() :
	GLESShape(),
	GLESTriangles(),
	Box()
{
	rebuildPrimitives();
}

GLESBox::~GLESBox()
{
	// nothing to do here
}

Vector3 GLESBox::size() const
{
	return size_;
}

bool GLESBox::setSize(const Vector3& size)
{
	if (size.x() < 0 || size.y() < 0 || size.z() < 0)
	{
		return false;
	}

	if (size == size_)
	{
		return true;
	}

	size_ = size;
	rebuildPrimitives();

	return true;
}

void GLESBox::rebuildPrimitives()
{
	Vertex pos(size_ * Scalar(0.5));

	const Vertex v0(-pos.x(), -pos.y(), pos.z());
	const Vertex v1(-pos.x(), pos.y(), pos.z());
	const Vertex v2(pos.x(), pos.y(), pos.z());
	const Vertex v3(pos.x(), -pos.y(), pos.z());
	const Vertex v4(-pos.x(), -pos.y(), -pos.z());
	const Vertex v5(-pos.x(), pos.y(), -pos.z());
	const Vertex v6(pos.x(), pos.y(), -pos.z());
	const Vertex v7(pos.x(), -pos.y(), -pos.z());

	const Normal right(1, 0, 0);
	const Normal left(-1, 0, 0);
	const Normal top(0, 1, 0);
	const Normal bottom(0, -1, 0);
	const Normal front(0, 0, 1);
	const Normal back(0, 0, -1);

	const TextureCoordinate tc00(0, 1);
	const TextureCoordinate tc01(0, 0);
	const TextureCoordinate tc10(1, 1);
	const TextureCoordinate tc11(1, 0);

	const Vectors3 vertices =
	{
		v0, v2, v1, // front
		v0, v3, v2,
		v3, v6, v2, // right
		v3, v7, v6,
		v7, v5, v6, // back
		v7, v4, v5,
		v4, v1, v5, // left
		v4, v0, v1,
		v1, v6, v5, // top
		v1, v2, v6,
		v4, v3, v0, // bottom
		v4, v7, v3
	};

	const Vectors3 normals =
	{
		front, front, front,
		front, front, front,
		right, right, right,
		right, right, right,
		back, back, back,
		back, back, back,
		left, left, left,
		left, left, left,
		top, top, top,
		top, top, top,
		bottom, bottom, bottom,
		bottom, bottom, bottom
	};

	const Vectors2 textureCoordinates =
	{
		tc01, tc10, tc00, // front
		tc01, tc11, tc10,

		tc01, tc10, tc00, // right
		tc01, tc11, tc10,

		tc01, tc10, tc00, // back
		tc01, tc11, tc10,

		tc01, tc10, tc00, // left
		tc01, tc11, tc10,

		tc01, tc10, tc00, // top
		tc01, tc11, tc10,

		tc01, tc10, tc00, // bottom
		tc01, tc11, tc10
	};

	unsigned int index = 0u;

	TriangleFaces triangleFaces;
	triangleFaces.reserve(12);

	for (unsigned int n = 0u; n < 12u; n++)
	{
		triangleFaces.emplace_back(index);
		index += 3u;
	}

	shapeVertexSet->setVertices(vertices);
	shapeVertexSet->setNormals(normals);
	shapeVertexSet->setTextureCoordinates(textureCoordinates, 0u);

	setFaces(triangleFaces);
	setVertexSet(shapeVertexSet);
}

void GLESBox::updateBoundingBox()
{
	boundingBox_ = BoundingBox(Box3(Vector3(0, 0, 0), size_.x(), size_.y(), size_.z()));
}

GLESBox::ObjectType GLESBox::type() const
{
	return TYPE_BOX;
}

}

}

}
