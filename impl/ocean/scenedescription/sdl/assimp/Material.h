/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_MATERIAL_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_MATERIAL_H

#include "ocean/scenedescription/sdl/assimp/Assimp.h"

#include "ocean/rendering/AttributeSet.h"
#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Textures.h"

#include <assimp/material.h>
#include <assimp/scene.h>

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace Assimp
{

/**
 * This class implements a converters for Assimp materials.
 * @ingroup scenedescriptionsdlassimp
 */
class OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_EXPORT Material
{
	public:

		/**
		 * Parses an Assimp material and creates a corresponding an Ocean AttributeSet object (which can hold e.g., a material and textures).
		 * @param engine The rendering engine for which the material object will be created
		 * @param sceneFilename The filename of the scene, if known
		 * @param assimpScene The Assimp scene from which the material objects will be created
		 * @param assimpMaterial The Assimp material to parse
		 * @return The resulting Ocean AttributeSet object
		 */
		static Rendering::AttributeSetRef parseMaterial(const Rendering::Engine& engine, const std::string& sceneFilename, const aiScene& assimpScene, const aiMaterial& assimpMaterial);

		/**
		 * Parses all Assimp material objects from an Assimp scene.
		 * @param engine The rendering engine for which the material objects will be created
		 * @param sceneFilename The filename of the scene, if known
		 * @param assimpScene The Assimp scene from which the material objects will be created
		 * @return The Ocean AttributeSet objects, one for each Assimp material (with preserved order)
		 */
		static std::vector<Rendering::AttributeSetRef> parseMaterials(const Rendering::Engine& engine, const std::string& sceneFilename, const aiScene& assimpScene);

		/**
		 * Parses all textures from an Assimp material.
		 * @param engine The rendering engine for which the material object will be created
		 * @param sceneFilename The filename of the scene, if known
		 * @param assimpScene The Assimp scene from which the material objects will be created
		 * @param assimpMaterial The Assimp material to parse
		 * @param textureIsTransparent Resulting statement whether the texture is transparent or opaque
		 * @return The resulting textures
		 */
		static Rendering::TexturesRef parseTextures(const Rendering::Engine& engine, const std::string& sceneFilename, const aiScene& assimpScene, const aiMaterial& assimpMaterial, bool& textureIsTransparent);
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_MATERIAL_H
