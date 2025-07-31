/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/globalillumination/GIBox.h"
#include "ocean/rendering/globalillumination/GIVertexSet.h"
#include "ocean/rendering/globalillumination/TracingBox.h"

#include "ocean/rendering/Engine.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

GIBox::GIBox() :
	GIShape(),
	GITriangles(),
	Box(),
	boxSize(1, 1, 1),
	boxDiagonal(boxSize.length())
{
	rebuildPrimitives();
}

GIBox::~GIBox()
{
	// nothing to do here
}

Vector3 GIBox::size() const
{
	return boxSize;
}

bool GIBox::setSize(const Vector3& size)
{
	if (size.x() < 0 || size.y() < 0 || size.z() < 0)
		return false;

	if (size == boxSize)
		return true;

	boxSize = size;
	boxDiagonal = boxSize.length();

	rebuildPrimitives();
	return true;
}

BoundingBox GIBox::boundingBox() const
{
	return BoundingBox(-boxSize * Scalar(0.5), boxSize * Scalar(0.5));
}

BoundingSphere GIBox::boundingSphere() const
{
	return BoundingSphere(Vector3(0, 0, 0), boxDiagonal * Scalar(0.5));
}

void GIBox::rebuildPrimitives()
{
	Vertex pos(boxSize * Scalar(0.5));

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

	Vertices vertices;
	Normals normals;
	TextureCoordinates textureCoordinates;
	TriangleFaces faces;

	vertices.reserve(36);
	normals.reserve(36);
	textureCoordinates.reserve(36);
	faces.reserve(12);

	// front
	vertices.push_back(v0);
	vertices.push_back(v2);
	vertices.push_back(v1);
	normals.insert(normals.end(), 3, front);
	textureCoordinates.push_back(tc01);
	textureCoordinates.push_back(tc10);
	textureCoordinates.push_back(tc00);
	vertices.push_back(v0);
	vertices.push_back(v3);
	vertices.push_back(v2);
	normals.insert(normals.end(), 3, front);
	textureCoordinates.push_back(tc01);
	textureCoordinates.push_back(tc11);
	textureCoordinates.push_back(tc10);

	// right
	vertices.push_back(v3);
	vertices.push_back(v6);
	vertices.push_back(v2);
	normals.insert(normals.end(), 3, right);
	textureCoordinates.push_back(tc01);
	textureCoordinates.push_back(tc10);
	textureCoordinates.push_back(tc00);
	vertices.push_back(v3);
	vertices.push_back(v7);
	vertices.push_back(v6);
	normals.insert(normals.end(), 3, right);
	textureCoordinates.push_back(tc01);
	textureCoordinates.push_back(tc11);
	textureCoordinates.push_back(tc10);

	// back
	vertices.push_back(v7);
	vertices.push_back(v5);
	vertices.push_back(v6);
	normals.insert(normals.end(), 3, back);
	textureCoordinates.push_back(tc01);
	textureCoordinates.push_back(tc10);
	textureCoordinates.push_back(tc00);
	vertices.push_back(v7);
	vertices.push_back(v4);
	vertices.push_back(v5);
	normals.insert(normals.end(), 3, back);
	textureCoordinates.push_back(tc01);
	textureCoordinates.push_back(tc11);
	textureCoordinates.push_back(tc10);

	// left
	vertices.push_back(v4);
	vertices.push_back(v1);
	vertices.push_back(v5);
	normals.insert(normals.end(), 3, left);
	textureCoordinates.push_back(tc01);
	textureCoordinates.push_back(tc10);
	textureCoordinates.push_back(tc00);
	vertices.push_back(v4);
	vertices.push_back(v0);
	vertices.push_back(v1);
	normals.insert(normals.end(), 3, left);
	textureCoordinates.push_back(tc01);
	textureCoordinates.push_back(tc11);
	textureCoordinates.push_back(tc10);

	// top
	vertices.push_back(v1);
	vertices.push_back(v6);
	vertices.push_back(v5);
	normals.insert(normals.end(), 3, top);
	textureCoordinates.push_back(tc01);
	textureCoordinates.push_back(tc10);
	textureCoordinates.push_back(tc00);
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v6);
	normals.insert(normals.end(), 3, top);
	textureCoordinates.push_back(tc01);
	textureCoordinates.push_back(tc11);
	textureCoordinates.push_back(tc10);

	// bottom
	vertices.push_back(v4);
	vertices.push_back(v3);
	vertices.push_back(v0);
	normals.insert(normals.end(), 3, bottom);
	textureCoordinates.push_back(tc01);
	textureCoordinates.push_back(tc10);
	textureCoordinates.push_back(tc00);
	vertices.push_back(v4);
	vertices.push_back(v7);
	vertices.push_back(v3);
	normals.insert(normals.end(), 3, bottom);
	textureCoordinates.push_back(tc01);
	textureCoordinates.push_back(tc11);
	textureCoordinates.push_back(tc10);

	TriangleFaces triangleFaces;
	triangleFaces.reserve(12);
	unsigned int index = 0;

	for (unsigned int n = 0; n < 12; n++)
	{
		triangleFaces.push_back(TriangleFace(index));
		index += 3;
	}

	const VertexSetRef vertexSet = engine().factory().createVertexSet();
	vertexSet->setVertices(vertices);
	vertexSet->setNormals(normals);
	vertexSet->setTextureCoordinates(textureCoordinates, 0);
	setVertexSet(vertexSet);

	setFaces(triangleFaces);
}

GIBox::ObjectType GIBox::type() const
{
	return TYPE_BOX;
}

void GIBox::buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const AttributeSetRef& attributes, const LightSources& lightSources) const
{
	TracingBox* box = new TracingBox();

	box->setBox(modelTransform, boundingBox());
	box->setLightSources(lightSources);
	box->setAttributes(attributes);

	group.addObject(box);
}

}

}

}
