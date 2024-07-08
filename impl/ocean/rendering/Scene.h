/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_SCENE_H
#define META_OCEAN_RENDERING_SCENE_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/Transform.h"
#include "ocean/rendering/ObjectRef.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class Scene;

/**
 * Definition of a smart object reference holding a scene node.
 * @see SmartObjectRef, Scene.
 * @ingroup rendering
 */
typedef SmartObjectRef<Scene> SceneRef;

/**
 * Definition of a vector holding scenes.
 * @ingroup rendering
 */
typedef std::vector<SceneRef> Scenes;

/**
 * This is the base class for all rendering scenes.
 * A once created scene can be added to a framebuffer to render the defined scene with all child nodes.
 * @see Framebuffer
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT Scene : virtual public Transform
{
	public:

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

		/**
		 * Returns whether this scene is lit with the view's headlight in case the view's headlight is active.
		 * @return True, if so; True, by default
		 */
		virtual bool useHeadlight() const;

		/**
		 * Sets whether this scene is lit with the view's headlight in case the view's headlight is active.
		 * @param state True, to light this scene with the view's headlight in case the view's headlight is active; False; to not use the view's headlight in any case
		 */
		virtual void setUseHeadlight(const bool state);

	protected:

		/**
		 * Creates a new scene object.
		 */
		Scene();

		/**
		 * Destructs a scene object.
		 */
		~Scene() override;

	protected:

		/// True, to light this scene with the view's headlight in case the view's headlight is active; False; to not use the view's headlight in any case
		bool useViewHeadlight_ = true;
};

}

}

#endif // META_OCEAN_RENDERING_SCENE_H
