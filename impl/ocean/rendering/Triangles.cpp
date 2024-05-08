/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/Triangles.h"

namespace Ocean
{

namespace Rendering
{

Triangles::Triangles() :
	Primitive()
{
	// nothing to do here
}

Triangles::~Triangles()
{
	// nothing to do here
}

TriangleFaces Triangles::faces() const
{
	throw NotSupportedException("Triangles::faces() is not supported.");
}

unsigned int Triangles::numberFaces() const
{
	throw NotSupportedException("Triangles::numberFaces() is not supported.");
}

void Triangles::setFaces(const TriangleFaces& /*faces*/)
{
	throw NotSupportedException("Triangles::setFaces() is not supported.");
}

void Triangles::setFaces(const unsigned int /*numberFaces*/)
{
	throw NotSupportedException("Triangles::setFaces() is not supported.");
}

bool Triangles::createTriangleMesh(const Vertices& vertices, const TriangleFaces& faces, Vertices& triangleVertices, Normals& triangleNormals, TriangleFaces& triangleFaces, const bool skipIrregularTriangles)
{
	ocean_assert(triangleVertices.empty() && triangleNormals.empty() && triangleFaces.empty());

	triangleVertices.clear();
	triangleNormals.clear();
	triangleFaces.clear();

	triangleVertices.reserve(faces.size() * 3);
	triangleNormals.reserve(faces.size() * 3);
	triangleFaces.reserve(faces.size());

	for (TriangleFaces::const_iterator i = faces.begin(); i != faces.end(); ++i)
	{
		if (i->index(0) >= vertices.size() || i->index(1) >= vertices.size() || i->index(2) >= vertices.size())
		{
			if (skipIrregularTriangles)
			{
				continue;
			}

			return false;
		}

		const Vertex& vertex0 = vertices[i->index(0)];
		const Vertex& vertex1 = vertices[i->index(1)];
		const Vertex& vertex2 = vertices[i->index(2)];

		Vector3 normal((vertex1 - vertex0).cross(vertex2 - vertex0));

		if (!normal.normalize())
		{
			if (skipIrregularTriangles)
			{
				continue;
			}

			return false;
		}

		triangleFaces.emplace_back((unsigned int)(triangleVertices.size()));

		triangleVertices.push_back(vertex0);
		triangleVertices.push_back(vertex1);
		triangleVertices.push_back(vertex2);

		triangleNormals.push_back(normal);
		triangleNormals.push_back(normal);
		triangleNormals.push_back(normal);
	}

	return true;
}

Triangles::ObjectType Triangles::type() const
{
	return TYPE_TRIANGLES;
}


}

}
