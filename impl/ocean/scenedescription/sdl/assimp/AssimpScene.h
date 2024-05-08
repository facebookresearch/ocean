/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_ASSIMP_SCENE_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_ASSIMP_SCENE_H

#include "ocean/scenedescription/sdl/assimp/Assimp.h"

#include "ocean/scenedescription/SDLScene.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace Assimp
{

/**
 * This class holds the entire scene contained in one assimp-supported file.
 * @ingroup scenedescriptionsdlassimp
 */
class OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_EXPORT AssimpScene : public SDLScene
{
	public:

		/**
		 * Creates a new Assimp scene object.
		 * @param filename Scene filename
		 */
		AssimpScene(const std::string& filename);

		/**
		 * Destructs an Assimp scene object.
		 */
		~AssimpScene() override;

	protected:

		/**
		 * Applies the entire scene to the rendering engine.
		 * @see Scene::internalApply().
		 */
		Rendering::SceneRef internalApply(const Rendering::EngineRef& engine) override;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_ASSIMP_ASSIMP_SCENE_H
