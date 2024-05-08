/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_MESH_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_MESH_H

#include "ocean/scenedescription/sdl/assimp/Assimp.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Material.h"
#include "ocean/rendering/Geometry.h"

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

/**
 * This class implements converters for Assimp meshes.
 * @ingroup scenedescriptionsdlassimp
 */
class OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_EXPORT Mesh
{
	public:

		/**
		 * Parses an Assimp mesh and converts it to an Ocean geometry object.
		 * @param engine The rendering engine for which the geometry object will be created
		 * @param attributeSets All AttributeSet objects which have been parsed/extracted from the Assimp scene in which the Assimp mesh is defined
		 * @param assimpMesh The Assimp mesh which will be converted to an Ocean geometry object
		 * @return The resulting Ocean geometry object
		 */
		static Rendering::GeometryRef parseMesh(const Rendering::Engine& engine, const std::vector<Rendering::AttributeSetRef>& attributeSets, const aiMesh& assimpMesh);

		/**
		 * Parses all Assimp meshes and converts them to Ocean geometry objects.
		 * @param engine The rendering engine for which the geometry object will be created
		 * @param attributeSets All AttributeSet objects which have been parsed/extracted from the Assimp scene in which the Assimp mesh is defined
		 * @param assimpScene The Assimp scene from which the geometry objects will be created
		 * @return The resulting Ocean geometry objects (with preserved order)
		 */
		static std::vector<Rendering::GeometryRef> parseMeshes(const Rendering::Engine& engine, const std::vector<Rendering::AttributeSetRef>& attributeSets, const aiScene& assimpScene);
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_MESH_H
