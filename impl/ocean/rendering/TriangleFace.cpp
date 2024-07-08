/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/rendering/TriangleFace.h"

namespace Ocean
{

namespace Rendering
{

Vectors3 TriangleFace::calculatePerFaceNormals(const TriangleFaces& faces, const Vertices& vertices, const bool counterClockWise)
{
	Vectors3 perFaceNormals;
	perFaceNormals.reserve(faces.size());

	for (const TriangleFace& face : faces)
	{
		ocean_assert(face.index(0u) < vertices.size());
		ocean_assert(face.index(1u) < vertices.size());
		ocean_assert(face.index(2u) < vertices.size());

		const Vector3& vertex0 = vertices[face.index(0u)];
		const Vector3& vertex1 = vertices[face.index(1u)];
		const Vector3& vertex2 = vertices[face.index(2u)];

		ocean_assert(vertex0 != vertex1);
		ocean_assert(vertex0 != vertex2);
		ocean_assert(vertex1 != vertex2);

		const Vector3 normal = (vertex1 - vertex0).cross(vertex2 - vertex0);

		if (counterClockWise)
		{
			perFaceNormals.emplace_back(normal.normalizedOrZero());
		}
		else
		{
			perFaceNormals.emplace_back(-normal.normalizedOrZero());
		}
	}

	return perFaceNormals;
}

Vectors3 TriangleFace::calculateSmoothedPerVertexNormals(const TriangleFaces& faces, const Vertices& vertices, const Vectors3& perFaceNormals)
{
	// create a map mapping vertices to their face indices

	VertexMap vertexMap;

	for (size_t nFace = 0; nFace < faces.size(); ++nFace)
	{
		const TriangleFace& face = faces[nFace];

		for (unsigned int n = 0; n < 3u; ++n)
		{
			const Vector3& faceVertex = vertices[face.index(n)];

			ocean_assert(face.index(n) < vertices.size());
			VertexMap::iterator iVertex = vertexMap.find(faceVertex);

			if (iVertex == vertexMap.cend())
			{
				vertexMap.emplace(faceVertex, Indices32(1, Index32(nFace)));
			}
			else
			{
				iVertex->second.emplace_back(Index32(nFace));
			}
		}
	}

	// now, we know which faces have intersecting vertices

	Vectors3 perVertexNormals;
	perVertexNormals.reserve(vertices.size());

	for (const Vector3& vertex : vertices)
	{
		const VertexMap::const_iterator iVertex = vertexMap.find(vertex);

		if (iVertex == vertexMap.cend())
		{
			// this vertex is not part of any given face
			perVertexNormals.emplace_back(Scalar(0), Scalar(0), Scalar(0));

			continue;
		}

		Vector3 combinedNormal(0, 0, 0);

		for (const Index32& faceIndex : iVertex->second)
		{
			const Vector3& faceNormal = perFaceNormals[faceIndex];
			ocean_assert(faceNormal.isUnit());

			combinedNormal += faceNormal;
		}

		perVertexNormals.emplace_back(combinedNormal.normalizedOrZero());
	}

	ocean_assert(vertices.size() == perVertexNormals.size());

	return perVertexNormals;
}

bool TriangleFace::calculateSmoothedPerVertexNormals(const TriangleFaces& faces, const Vectors3& vertices, Vectors3& normals, const Scalar creaseAngle)
{
	if (creaseAngle < 0 || creaseAngle > Numeric::pi_2())
	{
		return false;
	}

	// create a map mapping vertices to vertex indices with identical vertex

	VertexMap vertexMap;

	for (const TriangleFace& face : faces)
	{
		for (unsigned int n = 0; n < 3u; ++n)
		{
			ocean_assert(face.index(n) < vertices.size());
			VertexMap::iterator iVertex = vertexMap.find(vertices[face.index(n)]);

			if (iVertex == vertexMap.cend())
			{
				iVertex = vertexMap.emplace(vertices[face.index(n)], VertexIndices()).first;
			}

			ocean_assert(iVertex != vertexMap.cend());
			iVertex->second.emplace_back(face.index(n));
		}
	}

	// now, we know which faces have intersecting vertices

	const Scalar creaseAngleCos = Numeric::cos(creaseAngle);

	Vectors3 perVertexNormals;
	perVertexNormals.reserve(normals.size());

	for (const TriangleFace& face : faces)
	{
		for (unsigned int n = 0; n < 3u; ++n)
		{
			ocean_assert(normals.size() > face.index(n));
			const Normal& referenceNormal = normals[face.index(n)];

			if (referenceNormal.isNull())
			{
				perVertexNormals.emplace_back(Scalar(0), Scalar(0), Scalar(0));
				continue;
			}

			ocean_assert(referenceNormal.isUnit());

			const VertexMap::const_iterator iVertex = vertexMap.find(vertices[face.index(n)]);
			ocean_assert(iVertex != vertexMap.cend());

			Normal combinedNormal = referenceNormal;

			for (const VertexIndex& vertexIndex : iVertex->second)
			{
				const Normal& testNormal = normals[vertexIndex];
				ocean_assert(testNormal.isUnit());

				if (testNormal != referenceNormal && testNormal.isNull() == false && referenceNormal * testNormal > creaseAngleCos)
				{
					combinedNormal += normals[vertexIndex];
				}
			}

			if (combinedNormal.normalize())
			{
				perVertexNormals.push_back(combinedNormal);
			}
			else
			{
				perVertexNormals.push_back(referenceNormal);
			}
		}
	}

	ocean_assert(normals.size() == perVertexNormals.size());
	normals = perVertexNormals;

	return true;
}

Vectors3 TriangleFace::calculateSmoothedPerVertexNormals(const TriangleFaces& faces, const Vectors3& vertices)
{
	Vectors3 normals(vertices.size());

	for (const TriangleFace& face : faces)
	{
		const Vector3& vertex0 = vertices[face[0u]];
		const Vector3& vertex1 = vertices[face[1u]];
		const Vector3& vertex2 = vertices[face[2u]];

		const Vector3 normal = (vertex1 - vertex0).cross(vertex2 - vertex0).normalizedOrZero();

		normals[face[0u]] += normal;
		normals[face[1u]] += normal;
		normals[face[2u]] += normal;
	}

	for (Vector3& normal : normals)
	{
		normal.normalizedOrZero();
	}

	return normals;
}

}

}
