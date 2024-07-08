/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_SCENE_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_SCENE_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Scene.h"
#include "ocean/scenedescription/SDLNode.h"

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration.
class SDLScene;

/**
 * Definition of a smart object reference for scene desciption loader scenes.
 * @see SDLScene, Node.
 * @ingroup scenedescription
 */
typedef SmartObjectRef<SDLScene, Node> SDLSceneRef;

/**
 * This class implements the base class for all sdl scene object providing access to all elements of a scene.
 * A new scene object can be created by the scene description Manager object.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT SDLScene :
	virtual public Scene,
	virtual public SDLNode
{
	public:

		/**
		 * Applies the entire scene to the rendering engine.
		 * @param engine Rendering engine to use
		 * @return Resulting rendering scene object
		 */
		Rendering::SceneRef apply(const Rendering::EngineRef& engine);

	protected:

		/**
		 * Creates a new scene object.
		 * @param filename Scene filename
		 */
		SDLScene(const std::string& filename);

		/**
		 * Internal function to apply the entire scene to the rendering engine.
		 * @param engine Rendering engine to use
		 * @return Resulting rendering scene object
		 */
		virtual Rendering::SceneRef internalApply(const Rendering::EngineRef& engine) = 0;

		/**
		 * Applies this node to the rendering engine.
		 * Don't use this function for a scene, use Scene::apply(Rendering::Engine&) instead.
		 * @see Node::apply().
		 */
		Rendering::ObjectRef apply(const Rendering::EngineRef& engine, const SDLScene& scene, SDLNode& parentDescription, const Rendering::ObjectRef& parentRendering) override;
};

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_SCENE_H
