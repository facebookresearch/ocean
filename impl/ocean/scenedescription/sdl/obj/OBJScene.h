/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_OBJ_SCENE_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_OBJ_SCENE_H

#include "ocean/scenedescription/sdl/obj/OBJ.h"
#include "ocean/scenedescription/sdl/obj/Material.h"

#include "ocean/scenedescription/SDLScene.h"

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/QuadFace.h"
#include "ocean/rendering/TriangleFace.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

/**
 * This class holds the entire scene contained in one obj file.
 * @ingroup scenedescriptionsdlobj
 */
class OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT OBJScene : public SDLScene
{
	public:

		/**
		 * Definition of different face types.
		 */
		enum FaceType
		{
			/// Face holding vertices only.
			TYPE_V = 0,
			/// Face holding vertices and normals.
			TYPE_VN = 1,
			/// Face holding vertices and texture coordinates.
			TYPE_VT = 2,
			/// Face holding vertices, texture coordinates and normals.
			TYPE_VNT = TYPE_VN | TYPE_VT
		};

		/**
		 * This class holds vertex, normal and texture indices for a triangle.
		 */
		class Face
		{
			public:

				/**
				 * Creates a new face object.
				 * @param vertexIndices Vertex indices of the face
				 * @param normalIndices Normal indices of the face
				 * @param textureIndices Texture coordinate indices of the face
				 */
				inline Face(const Rendering::VertexIndices& vertexIndices, const Rendering::VertexIndices& normalIndices, const Rendering::VertexIndices& textureIndices);

				/**
				 * Returns the face type of this object.
				 * @return Face type
				 */
				inline FaceType type() const;

				/**
				 * Returns the vertex indices of the face.
				 * @return Face vertex indices
				 */
				inline const Rendering::VertexIndices& vertexIndices() const;

				/**
				 * Returns the normal indices of the face.
				 * @return Face normal indices
				 */
				inline const Rendering::VertexIndices& normalIndices() const;

				/**
				 * Returns the texture coordinate indices of the face.
				 * @return Face texture coordinate indices
				 */
				inline const Rendering::VertexIndices& textureIndices() const;

			protected:

				/// Vertex indices.
				Rendering::VertexIndices vertexIndices_;

				/// Noraml indices.
				Rendering::VertexIndices normalIndices_;

				/// Texture coordinate indices.
				Rendering::VertexIndices textureIndices_;
		};

		/**
		 * Definition of a vector holding materials.
		 */
		typedef std::vector<Material> Materials;

		/**
		 * Definition of a material index.
		 */
		typedef unsigned int MaterialIndex;

		/**
		 * Definition of an invalid material index.
		 */
		static constexpr MaterialIndex invalidMaterialIndex_ = OBJScene::MaterialIndex(-1);

		/**
		 * Definition of a map mapping material names to material indices.
		 */
		typedef std::unordered_map<std::string, unsigned int> MaterialIndexMap;

		/**
		 * Definition of a vector holding faces.
		 */
		typedef std::vector<Face> Faces;

		/**
		 * Definition of a pair of face type and material index.
		 */
		typedef std::pair<FaceType, MaterialIndex> FacePair;

		/**
		 * Definition of a map mapping face pairs to faces.
		 */
		typedef std::map<FacePair, Faces> FacesMap;

	public:

		/**
		 * Creates a new OBJ scene object.
		 * @param filename Scene filename
		 */
		OBJScene(const std::string& filename);

		/**
		 * Destructs an OBJ scene object.
		 */
		~OBJScene() override;

		/**
		 * Returns the vertices of the entire obj scene.
		 * @return Scene vertices
		 */
		inline const Rendering::Vertices& vertices() const;

		/**
		 * Returns the normals of the entire obj scene.
		 * @return Scene normals
		 */
		inline const Rendering::Normals& normals() const;

		/**
		 * Returns the texture coordinates of the entire obj scene.
		 * @return Scene texture coordinates
		 */
		inline const Rendering::TextureCoordinates& textureCoordinates() const;

		/**
		 * Returns all faces of the entire obj scene.
		 * @return The scene's faces
		 */
		inline const FacesMap& facesMap() const;

		/**
		 * Sets the materials of the obj scene.
		 * @param materials The materials for the scene
		 */
		void setMaterials(Materials&& materials);

		/**
		 * Adds a new vertex to the scene.
		 * @param vertex Vertex to add
		 */
		inline void addVertex(const Rendering::Vertex& vertex);

		/**
		 * Adds a new normal to the scene.
		 * @param normal Normal to add
		 */
		inline void addNormal(const Rendering::Normal& normal);

		/**
		 * Adds a new texture coordinate to the scene.
		 * @param textureCoordinate Texture coordinate to add
		 */
		inline void addTextureCoordinate(const Rendering::TextureCoordinate& textureCoordinate);

		/**
		 * Adds a new face to the scene.
		 * @param face New face to add
		 */
		void addFace(const Face& face);

		/**
		 * Sets the current selected material.
		 * @param material Current material name
		 */
		void setCurrentMaterial(const std::string& material);

	protected:

		/**
		 * Applies the entire scene to the rendering engine.
		 * @see Scene::internalApply().
		 */
		Rendering::SceneRef internalApply(const Rendering::EngineRef& engine) override;

		/**
		 * Creates a new triangle mesh object.
		 * @param engine Rendering engine to use
		 * @return Group holding triangle meshes
		 */
		Rendering::NodeRef createTriangles(const Rendering::EngineRef& engine);

	protected:

		/// Vector holding all vertices of the obj scene.
		Rendering::Vertices vertices_;

		/// Vector holding all normals of the obj scene.
		Rendering::Normals normals_;

		/// Vector holding all texture coordinate of the obj scene.
		Rendering::TextureCoordinates textureCoordinates_;

		/// Map holding all faces of the obj scene.
		FacesMap facesMap_;

		/// All material objects of this scene.
		Materials materials_;

		/// Map mapping material names to material indices.
		MaterialIndexMap materialIndexMap_;

		/// Current selected material index.
		unsigned int selectedMaterialIndex_ = invalidMaterialIndex_;
};

inline OBJScene::Face::Face(const Rendering::VertexIndices& vertexIndices, const Rendering::VertexIndices& normalIndices, const Rendering::VertexIndices& textureIndices) :
	vertexIndices_(vertexIndices),
	normalIndices_(normalIndices),
	textureIndices_(textureIndices)
{
	ocean_assert(vertexIndices.size() > 2);
	ocean_assert(normalIndices_.empty() || normalIndices_.size() == vertexIndices.size());
	ocean_assert(textureIndices_.empty() || textureIndices_.size() == vertexIndices.size());
}

inline OBJScene::FaceType OBJScene::Face::type() const
{
	return FaceType((unsigned int)(!normalIndices_.empty()) | ((unsigned int)(!textureIndices_.empty()) << 1));
}

inline const Rendering::VertexIndices& OBJScene::Face::vertexIndices() const
{
	return vertexIndices_;
}

inline const Rendering::VertexIndices& OBJScene::Face::normalIndices() const
{
	return normalIndices_;
}

inline const Rendering::VertexIndices& OBJScene::Face::textureIndices() const
{
	return textureIndices_;
}

inline const Rendering::Vertices& OBJScene::vertices() const
{
	return vertices_;
}

inline const Rendering::Normals& OBJScene::normals() const
{
	return normals_;
}

inline const Rendering::TextureCoordinates& OBJScene::textureCoordinates() const
{
	return textureCoordinates_;
}

inline const OBJScene::FacesMap& OBJScene::facesMap() const
{
	return facesMap_;
}

inline void OBJScene::addVertex(const Rendering::Vertex& vertex)
{
	vertices_.emplace_back(vertex);
}

inline void OBJScene::addNormal(const Rendering::Normal& normal)
{
	normals_.emplace_back(normal);
}

inline void OBJScene::addTextureCoordinate(const Rendering::TextureCoordinate& textureCoordinate)
{
	textureCoordinates_.emplace_back(textureCoordinate);
}

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_OBJ_SCENE_H
