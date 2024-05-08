/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_SCENE_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_SCENE_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Scene.h"
#include "ocean/scenedescription/SDXNode.h"

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration.
class SDXScene;

/**
 * Definition of a smart object reference for scene desciption X scenes.
 * @see SDXScene, Node.
 * @ingroup scenedescription
 */
typedef SmartObjectRef<SDXScene, Node> SDXSceneRef;

/**
 * This class implements the base class for all sdx scene object providing access to all elements of a scene.
 * A new scene object can be created by the scene description Manager object.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT SDXScene :
	virtual public Scene,
	virtual public SDXNode
{
	public:

		/**
		 * Returns the name of the file defining this node.
		 * Beware: A node can be defined outside a file context and thus does not have a file!
		 * @return Filename
		 */
		inline const std::string& filename() const;

		/**
		 * Returns the associated scene rendering object.
		 * @return Associated scene rendering object
		 */
		inline Rendering::SceneRef renderingScene() const;

		/**
		 * Returns the environment object of this scene.
		 * @return Environment object
		 */
		inline const SDXEnvironment* environment() const;

		/**
		 * Registers a global light source to this scene.
		 * Licht sources in local transformations with global state should be registered only.<br>
		 * The transformation, position or orientation of the light source is unchanged.
		 * @param lightSource Rendering light source object to be registered
		 * @see unregisterGlobalLight().
		 */
		virtual void registerGlobalLight(const Rendering::LightSourceRef& lightSource);

		/**
		 * Unregisters a global light source from this scene.
		 * @param lightSource Renring light source object to be unregistered
		 * @see registerGlobalLight().
		 */
		virtual void unregisterGlobalLight(const Rendering::LightSourceRef& lightSource);

		/**
		 * Event function to inform the scene that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @param timestamp Initialization timestamp
		 */
		virtual void initialize(const Timestamp timestamp);

	protected:

		/**
		 * Creates a new scene object.
		 * @param filename Name of the scene file, if any
		 * @param library Library providing this scene node
		 * @param engine Rendering engine corresponding with this scene and all child nodes
		 */
		SDXScene(const std::string& filename, const Library& library, const Rendering::EngineRef& engine);

		/**
		 * Event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * The scene node has an own initialize() function, use this instead
		 * @see SDXNode::initialize().
		 */
		void initialize(const Rendering::SceneRef& scene, const Timestamp timestamp, const bool reinitialize = false) override;

		/**
		 * Internal event function to inform the scene that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * @param timestamp Initialization timestamp
		 */
		virtual void onInitialize(const Timestamp timestamp);

		/**
		 * Internal event function to inform the node that it has been initialized and can apply all internal values to corresponding rendering objects.
		 * The scene node has an own onInitialize() function, use this instead
		 * @see SDXNode::onInitialize().
		 */
		void onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp) override;

	protected:

		/// Scene environment object.
		SDXEnvironment sceneEnvironment_;
};

inline const std::string& SDXScene::filename() const
{
	return SDXNode::filename();
}

inline Rendering::SceneRef SDXScene::renderingScene() const
{
	return Rendering::SceneRef(renderingObject());
}

inline const SDXScene::SDXEnvironment* SDXScene::environment() const
{
	return environment_;
}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_SCENE_H
