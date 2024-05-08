/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/assimp/Mesh.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/VertexSet.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace Assimp
{

Rendering::GeometryRef Mesh::parseMesh(const Rendering::Engine& engine, const std::vector<Rendering::AttributeSetRef>& attributeSets, const aiMesh& assimpMesh)
{
	if ((assimpMesh.mPrimitiveTypes & aiPrimitiveType_TRIANGLE) != aiPrimitiveType_TRIANGLE)
	{
		return Rendering::TrianglesRef();
	}

	if (assimpMesh.mNumVertices == 0u)
	{
		return Rendering::TrianglesRef();
	}

	const Rendering::TrianglesRef triangles = engine.factory().createTriangles();
	ocean_assert(triangles);

	if (assimpMesh.mName.length != 0u)
	{
		triangles->setName(assimpMesh.mName.data);
	}

	const Rendering::VertexSetRef vertexSet = engine.factory().createVertexSet();
	ocean_assert(vertexSet);

	if (assimpMesh.mVertices != nullptr)
	{
		Rendering::Vertices vertices;
		vertices.reserve(assimpMesh.mNumVertices);

		for (unsigned int n = 0u; n < assimpMesh.mNumVertices; ++n)
		{
			const aiVector3D& assimpVertex = assimpMesh.mVertices[n];

			vertices.emplace_back(Scalar(assimpVertex.x), Scalar(assimpVertex.y), Scalar(assimpVertex.z));
		}

		vertexSet->setVertices(vertices);
	}

	if (assimpMesh.mNormals != nullptr)
	{
		Rendering::Normals normals;
		normals.reserve(assimpMesh.mNumVertices);

		for (unsigned int n = 0u; n < assimpMesh.mNumVertices; ++n)
		{
			const aiVector3D& assimpNormal = assimpMesh.mNormals[n];

			normals.emplace_back(Scalar(assimpNormal.x), Scalar(assimpNormal.y), Scalar(assimpNormal.z));
		}

		vertexSet->setNormals(normals);
	}

	static_assert(AI_MAX_NUMBER_OF_TEXTURECOORDS >= 1, "Invalid texture coordinates");

	if (assimpMesh.mTextureCoords[0] != nullptr)
	{
		Rendering::TextureCoordinates textureCoordinates;
		textureCoordinates.reserve(assimpMesh.mNumVertices);

		for (unsigned int n = 0u; n < assimpMesh.mNumVertices; ++n)
		{
			const aiVector3D& assimpTextureCoordinate = assimpMesh.mTextureCoords[0][n];

			textureCoordinates.emplace_back(Scalar(assimpTextureCoordinate.x), Scalar(assimpTextureCoordinate.y));
		}

		vertexSet->setTextureCoordinates(textureCoordinates, 0u);
	}

	static_assert(AI_MAX_NUMBER_OF_COLOR_SETS >= 1, "Invalid color sets");

	if (assimpMesh.mColors[0] != nullptr)
	{
		RGBAColors colors;
		colors.reserve(assimpMesh.mNumVertices);

		for (unsigned int n = 0u; n < assimpMesh.mNumVertices; ++n)
		{
			const aiColor4D& assimpColor = assimpMesh.mColors[0][n];

			// Assimp's alpha uses 0 for fully transparent and 1 for fully opaque
			colors.emplace_back(assimpColor.r, assimpColor.g, assimpColor.b, assimpColor.a);
		}

		vertexSet->setColors(colors);
	}

	triangles->setVertexSet(vertexSet);

	Rendering::TriangleFaces triangleFaces;
	triangleFaces.reserve(assimpMesh.mNumFaces);

	for (unsigned int n = 0u; n < assimpMesh.mNumFaces; ++n)
	{
		const aiFace& assimpFace = assimpMesh.mFaces[n];

		assert(assimpFace.mNumIndices == 3u);

		triangleFaces.emplace_back(assimpFace.mIndices);
	}

	triangles->setFaces(triangleFaces);

	const Rendering::GeometryRef geometry = engine.factory().createGeometry();
	ocean_assert(geometry);

	if (assimpMesh.mMaterialIndex < (unsigned int)(attributeSets.size()))
	{
		geometry->addRenderable(triangles, attributeSets[assimpMesh.mMaterialIndex]);
	}
	else
	{
		geometry->addRenderable(triangles, engine.factory().createAttributeSet());
	}

	return geometry;
}

std::vector<Rendering::GeometryRef> Mesh::parseMeshes(const Rendering::Engine& engine, const std::vector<Rendering::AttributeSetRef>& attributeSets, const aiScene& assimpScene)
{
	std::vector<Rendering::GeometryRef> geometries;
	geometries.reserve(assimpScene.mNumMeshes);

	for (unsigned int n = 0u; n < assimpScene.mNumMeshes; ++n)
	{
		const aiMesh* mesh = assimpScene.mMeshes[n];
		ocean_assert(mesh != nullptr);

		geometries.emplace_back(Mesh::parseMesh(engine, attributeSets, *mesh));
	}

	return geometries;
}

}

}

}

}
