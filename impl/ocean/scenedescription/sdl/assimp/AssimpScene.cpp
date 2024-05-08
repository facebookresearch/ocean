/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/assimp/AssimpScene.h"
#include "ocean/scenedescription/sdl/assimp/Material.h"
#include "ocean/scenedescription/sdl/assimp/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <assimp/material.h>
#include <assimp/mesh.h>
#include <assimp/scene.h>

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace Assimp
{

AssimpScene::AssimpScene(const std::string& filename) :
	Scene(filename),
	SDLScene(filename)
{
	// nothing to do here
}

AssimpScene::~AssimpScene()
{
	// nothing to do here
}

Rendering::SceneRef AssimpScene::internalApply(const Rendering::EngineRef& engine)
{
	ocean_assert(engine);

	::Assimp::Importer assimpImporter;

	unsigned int importerFlags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices;

	const aiScene* assimpScene = assimpImporter.ReadFile(filename(), importerFlags);

	if (assimpScene == nullptr)
	{
		Log::warning() << "ScenedescriptionAssimp: Failed to open `" << filename() << "`";
		Log::warning() << "Reason: " << assimpImporter.GetErrorString();

		return Rendering::SceneRef();
	}

	const std::vector<Rendering::AttributeSetRef> attributeSets = Material::parseMaterials(*engine, filename(), *assimpScene);
	const std::vector<Rendering::GeometryRef> geometries = Mesh::parseMeshes(*engine, attributeSets, *assimpScene);

	ocean_assert(assimpScene->mRootNode != nullptr);

	typedef std::pair<aiNode*, Rendering::TransformRef> NodePair;

	const Rendering::SceneRef scene(engine->factory().createScene());
	ocean_assert(scene);

	std::vector<NodePair> nodeStack(1, NodePair(assimpScene->mRootNode, scene));

	while (!nodeStack.empty())
	{
		const aiNode* assimpNode = nodeStack.back().first;
		Rendering::TransformRef parentTransform = nodeStack.back().second;

		ocean_assert(assimpNode != nullptr);
		ocean_assert(parentTransform);

		nodeStack.pop_back();

		Rendering::TransformRef transform = engine->factory().createTransform();
		ocean_assert(transform);

		if (assimpNode->mName.length != 0u)
		{
			transform->setName(assimpNode->mName.data);
		}

		transform->setTransformation(HomogenousMatrix4((const ai_real*)&assimpNode->mTransformation, /*row major*/ true));

		parentTransform->addChild(transform);

		for (unsigned int n = 0u; n < assimpNode->mNumMeshes; ++n)
		{
			const unsigned int meshIndex = assimpNode->mMeshes[n];

			if (meshIndex < (unsigned int)(geometries.size()))
			{
				transform->addChild(geometries[meshIndex]);
			}
		}

		for (unsigned int n = 0u; n < assimpNode->mNumChildren; ++n)
		{
			aiNode* childNode = assimpNode->mChildren[n];
			ocean_assert(childNode != nullptr);

			nodeStack.emplace_back(childNode, transform);
		}
	}

	return scene;
}

}

}

}

}
