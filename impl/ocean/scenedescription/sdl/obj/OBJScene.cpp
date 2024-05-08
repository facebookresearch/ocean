/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdl/obj/OBJScene.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/DirectionalLight.h"
#include "ocean/rendering/Geometry.h"
#include "ocean/rendering/PrimitiveAttribute.h"
#include "ocean/rendering/Quads.h"
#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Transform.h"
#include "ocean/rendering/Triangles.h"
#include "ocean/rendering/VertexSet.h"

#include "ocean/system/Performance.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

OBJScene::OBJScene(const std::string& filename) :
	Scene(filename),
	SDLScene(filename)
{
	// nothing to do here
}

OBJScene::~OBJScene()
{
	// nothing to do here
}

void OBJScene::setMaterials(Materials&& materials)
{
	ocean_assert(materials_.empty());
	ocean_assert(materialIndexMap_.empty());

	materials_ = std::move(materials);

	for (size_t n = 0; n < materials_.size(); ++n)
	{
		materialIndexMap_.emplace(materials_[n].name(), (unsigned int)(n));
	}

	ocean_assert(materials_.size() == materialIndexMap_.size());
}

Rendering::SceneRef OBJScene::internalApply(const Rendering::EngineRef& engine)
{
	ocean_assert(engine);

	const Rendering::SceneRef renderingScene(engine->factory().createScene());
	ocean_assert(renderingScene);

	const Rendering::TransformRef renderingTransform(engine->factory().createTransform());
	ocean_assert(renderingTransform);

	renderingTransform->addChild(createTriangles(engine));
	renderingScene->addChild(renderingTransform);

	return renderingScene;
}

void OBJScene::addFace(const Face& face)
{
	FacePair facePair(face.type(), selectedMaterialIndex_);

	const FacesMap::iterator i = facesMap_.find(facePair);

	if (i == facesMap_.cend())
	{
		facesMap_.emplace(facePair, Faces(1, face));
	}
	else
	{
		i->second.emplace_back(face);
	}
}

void OBJScene::setCurrentMaterial(const std::string& material)
{
	const MaterialIndexMap::const_iterator i = materialIndexMap_.find(material);

	if (i != materialIndexMap_.cend())
	{
		selectedMaterialIndex_ = i->second;
	}
	else
	{
		selectedMaterialIndex_ = invalidMaterialIndex_;
	}
}

Rendering::NodeRef OBJScene::createTriangles(const Rendering::EngineRef& engine)
{
	ocean_assert(engine);

	Rendering::GroupRef group(engine->factory().createGroup());

	unsigned int sceneTriangles = 0u;

	for (FacesMap::iterator i = facesMap_.begin(); i != facesMap_.end(); /*++i*/)
	{
		const FacePair& facePair = i->first;
		const Faces& faces = i->second;

		try
		{
			Rendering::GeometryRef geometry(engine->factory().createGeometry());
			Rendering::VertexSetRef vertexSet(engine->factory().createVertexSet());
			Rendering::TrianglesRef triangles(engine->factory().createTriangles());

			Rendering::Vertices vertices;
			Rendering::Normals normals;
			Rendering::TextureCoordinates textureCoordinates;

			for (const Face& face : faces)
			{
				const Rendering::VertexIndices& vertexIndices = face.vertexIndices();
				const Rendering::VertexIndices& normalIndices = face.normalIndices();
				const Rendering::VertexIndices& textureIndices = face.textureIndices();

#ifdef OCEAN_DEBUG
				for (size_t n = 0; n < vertexIndices.size(); n++)
				{
					ocean_assert(vertexIndices[n] <= vertices_.size());
				}
				for (size_t n = 0; n < normalIndices.size(); n++)
				{
					ocean_assert(normalIndices[n] <= normals_.size());
				}
				for (size_t n = 0; n < textureIndices.size(); n++)
				{
					ocean_assert(textureIndices[n] <= textureCoordinates_.size());
				}
#endif // OCEAN_DEBUG

				ocean_assert(vertexIndices.size() >= 3);

				vertices.emplace_back(vertices_[vertexIndices[0]]);
				vertices.emplace_back(vertices_[vertexIndices[1]]);
				vertices.emplace_back(vertices_[vertexIndices[2]]);

				for (unsigned int n = 3; n < vertexIndices.size(); n++)
				{
					vertices.emplace_back(vertices_[vertexIndices[0]]);
					vertices.emplace_back(vertices_[vertexIndices[n - 1]]);
					vertices.emplace_back(vertices_[vertexIndices[n]]);
				}

				if (face.type() & TYPE_VN)
				{
					ocean_assert(normalIndices.size() >= 3);

					normals.emplace_back(normals_[normalIndices[0]]);
					normals.emplace_back(normals_[normalIndices[1]]);
					normals.emplace_back(normals_[normalIndices[2]]);

					for (unsigned int n = 3; n < normalIndices.size(); n++)
					{
						normals.emplace_back(normals_[normalIndices[0]]);
						normals.emplace_back(normals_[normalIndices[n - 1]]);
						normals.emplace_back(normals_[normalIndices[n]]);
					}
				}
				else
				{
					Rendering::Normal normal((vertices_[vertexIndices[1]] - vertices_[vertexIndices[0]]).cross(vertices_[vertexIndices.back()] - vertices_[vertexIndices[0]]));
					if (normal.normalize())
					{
						normals.insert(normals.end(), 3 * (vertexIndices.size() - 2), normal);
					}
					else
					{
						Log::warning() << "Could not calculate a valid normal.";
						normals.insert(normals.end(), 3 * (vertexIndices.size() - 2), Rendering::Normal(0, 0, 1));
					}
				}

				if (face.type() & TYPE_VT)
				{
					ocean_assert(textureIndices.size() >= 3);

					textureCoordinates.emplace_back(textureCoordinates_[textureIndices[0]]);
					textureCoordinates.emplace_back(textureCoordinates_[textureIndices[1]]);
					textureCoordinates.emplace_back(textureCoordinates_[textureIndices[2]]);

					for (size_t n = 3; n < textureIndices.size(); n++)
					{
						textureCoordinates.emplace_back(textureCoordinates_[textureIndices[0]]);
						textureCoordinates.emplace_back(textureCoordinates_[textureIndices[n - 1]]);
						textureCoordinates.emplace_back(textureCoordinates_[textureIndices[n]]);
					}
				}
			}

			Rendering::TriangleFaces triangleFaces;
			triangleFaces.reserve(faces.size());

			for (size_t n = 0; n < vertices.size(); n += 3)
			{
				triangleFaces.emplace_back(Index32(n));
			}

			if (System::Performance::get().performanceLevel() >= System::Performance::LEVEL_HIGH || triangleFaces.size() <= 10000)
			{
				Rendering::TriangleFace::calculateSmoothedPerVertexNormals(triangleFaces, vertices, normals, Numeric::deg2rad(30));
			}

			vertexSet->setVertices(vertices);
			vertexSet->setNormals(normals);

			if (facePair.first & TYPE_VT)
			{
				vertexSet->setTextureCoordinates(textureCoordinates, 0);
			}

			triangles->setFaces(triangleFaces);
			triangles->setVertexSet(vertexSet);

			Rendering::AttributeSetRef attributeSet;

			if (facePair.second == invalidMaterialIndex_)
			{
				attributeSet = engine->factory().createAttributeSet();

				// we use a default material
				attributeSet->addAttribute(engine->factory().createMaterial());
			}
			else
			{
				attributeSet = materials_[facePair.second].attributeSet(engine, *this);
			}

			geometry->addRenderable(triangles, attributeSet);
			group->addChild(geometry);

			sceneTriangles += (unsigned int)(triangleFaces.size());
		}
		catch (const Exception& exception)
		{
			Log::error() << exception.what();
		}
		catch (...)
		{
			Log::error() << "Unknown exception during triangle creation.";
		}

		facesMap_.erase(i++);
	}

	Log::info() << "\"" << filename() << "\" has " << sceneTriangles << " triangles.";

	return group;
}

}

}

}

}
