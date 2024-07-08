/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_MANAGER_H
#define META_OCEAN_SCENEDESCRIPTION_MANAGER_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Library.h"
#include "ocean/scenedescription/Node.h"
#include "ocean/scenedescription/SDLScene.h"
#include "ocean/scenedescription/SDXScene.h"

#include "ocean/base/Singleton.h"

#include "ocean/io/FileManager.h"

#include "ocean/rendering/View.h"

#include <map>
#include <vector>

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration.
class EventNode;

// Forward declaration.
class UpdateNode;

// Forward declaration.
class SDXEventNode;

// Forward declaration.
class SDXUpdateNode;

/**
 * This class implements the manager for all scene descriptions.
 * The manager encapsulates all registered scene description libraries.<br>
 * Use the load() function to load a new scene description object.<br>
 *
 * Example demonstrating the manager usage:
 * @code
 * // This example shows how to use the load() function and how to determine which type of scene description has been returned
 *
 * // Get the rendering engine from e.g. the application
 * Rendering::EngineRef renderingEngine = application.getRenderingEngine();
 * ocean_assert(renderingEngine);
 *
 * // Get the rendering framebuffer from e.g. the application
 * Rendering::FramebufferRef renderingFramebuffer = application.getRenderingFramebuffer();
 * ocean_assert(renderingFramebuffer);
 *
 * // Load a scene description file - a permanent scene description is preferred
 * SceneRef scene = Manager::get().load("scenefile.x3dv", renderingEngine, SceneDescription::TYPE_PERMANENT);
 *
 * if (scene)
 * {
 *    if (scene->descriptionType() == SceneDescription::TYPE_PERMANENT)
 *    {
 *       SDXSceneRef sdxScene(scene);
 *       ocean_assert(sdxScene);
 *
 *       // The rendering scene object of the permanent scene description object can be added to the used rendering framebuffer directly
 *       renderingFramebuffer->addScene(sdxScene->renderingScene());
 *    }
 *    else
 *    {
 *       ocean_assert(scene->descriptionType() == SceneDescription::TYPE_TRANSIENT);
 *
 *       SDLSceneRef sdlScene(scene);
 *       ocean_assert(sdlScene);
 *
 *       // We have a transient scene description object, thus we have to apply the description to the rendering engine explicitly
 *       Rendering::SceneRef renderingScene = sdlScene->apply(renderingEngine);
 *
 *       // If the created rendering scene object is valid is can be added to the used rendering framebuffer
 *       if (renderingScene)
 *          renderingFramebuffer->addScene(renderingScene);
 *    }
 * }
 * else
 * {
 *    // The scene could not be loaded, e.g. the file did not exist, or an error occurred.
 *    Log::error() << "The scene description file could not be loaded.";
 * }
 *
 * @endcode
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT Manager :
	public IO::FileManager,
	public Singleton<Manager>
{
	friend class Singleton<Manager>;
	friend class Library;
	friend class SDXEventNode;
	friend class SDXUpdateNode;

	public:

		/**
		 * Definition of a vector holding library names.
		 */
		typedef std::vector<std::string> LibraryNames;

	protected:

		/**
		 * Definition of a pair combining a library with a reference counter.
		 */
		typedef std::pair<LibraryRef, unsigned int> LibraryCounterPair;

		/**
		 * Definition of a vector holding library pairs.
		 */
		typedef std::vector<LibraryCounterPair> Libraries;

		/**
		 * Definition of a set holding library names.
		 */
		typedef std::unordered_set<std::string> NameSet;

		/**
		 * Definition of a map mapping scene description node ids to event nodes.
		 */
		typedef std::unordered_map<NodeId, SDXEventNode*> EventNodes;

		/**
		 * Definition of a map mapping scene description node ids to update nodes.
		 */
		typedef std::unordered_map<NodeId, SDXUpdateNode*> UpdateNodes;

		/**
		 * Definition of a set holding permanent scene description objects.
		 */
		typedef std::map<SceneId, SDXSceneRef> PermanentSceneMap;

	public:

		/**
		 * Loads a new scene and creates a scene description hierarchy.
		 * Two different scene description types are available.<br>
		 * First: A transient scene description creates a scene and object hierarchy and must be applied to a rendering engine afterwards explicitly.<br>
		 * After that all scene description objects are not needed anymore and can be disposed.<br>
		 *
		 * Second: a permanent scene description creates a scene and object hierarchy and directly creates corresponding rendering engine objects.<br>
		 * Therefore, all rendering objects exist as long as the corresponding scene description object exist.<br>
		 * The scene description object allows for easy scene manipulation without knowledge of the underling rendering structure.<br>
		 * This manager will hold a copy of all permanent scene description objects as long as they are unloaded explicitly.<br>
		 *
		 * Beware: Check the type of returned scene description object.<br>
		 * Depending on the available scene description libraries the description type may vary.<br>
		 * To not hold a reference of the returned object longer than necessary!<br>
		 * @param filename The filename of the scene to load
		 * @param engine Rendering engine to be connected with the scene description, must be defined for permanent scene description objects only
		 * @param timestamp The current timestamp, must be valid
		 * @param preferredDescriptionType Preferred description type of the scene to load
		 * @param progress The progress state receiving recurrently information about the load state with range [0, 1]
		 * @param cancel The cancel state allows the cancellation of a load process while loading process hasn't finished, if the cancel state is used the load process stops if the value is set to True
		 * @return Resulting scene
		 * @see unload().
		 */
		SceneRef load(const std::string& filename, const Rendering::EngineRef& engine, const Timestamp& timestamp, const DescriptionType preferredDescriptionType = TYPE_PERMANENT, float* progress = nullptr, bool* cancel = nullptr);

		/**
		 * Unloads a given permanent scene description objects.
		 * Transient description objects must not be unloaded because the manager does not hold a copy of transient scene objects.<br>
		 * Beware: At the moment the scene is unloaded there should not be any other resource holding a reference to this object!
		 * @param sceneId Scene id of the permanent scene description object to be unloaded
		 * @return True, if succeeded
		 * @see load().
		 */
		bool unload(const SceneId sceneId);

		/**
		 * Unloads all permanent scene description objects.
		 */
		void unloadScenes();

		/**
		 * Returns a list of all registered libraries.
		 * @return Registered libraries
		 */
		LibraryNames libraries();

		/**
		 * Returns all currently supported file extensions.
		 * @see FileManager::supportedFormats().
		 */
		FileExtensions supportedExtensions();

		/**
		 * Returns whether currently at least one scene description node handles mouse events.
		 * The state change with every new frame.<br>
		 * Because mouse events can produce high computational overhead this test can safe computational time.
		 * @return True, if so
		 */
		inline bool handlesMouseEvents() const;

		/**
		 * Sends a mouse event to be handled by the scene description.
		 * @param button The button type
		 * @param buttonEvent Type of the event
		 * @param screenPosition Screen device position
		 * @param objectPosition Object position
		 * @param objectId Id of a rendering object associated with the event
		 * @param timestamp Event timestamp
		 */
		void mouseEvent(const ButtonType button, const ButtonEvent buttonEvent, const Vector2& screenPosition, const Vector3& objectPosition, const Rendering::ObjectId objectId, const Timestamp timestamp);

		/**
		 * Sends a key event to be handled by the scene description.
		 * @param key The key type
		 * @param buttonEvent Type of the event
		 * @param objectId Id of a rendering object associated with the event
		 * @param timestamp Event timestamp
		 */
		void keyEvent(const int key, const ButtonEvent buttonEvent, const Rendering::ObjectId objectId, const Timestamp timestamp);

		/**
		 * Pre-updates all scene description objects needing regularly pre updates.
		 * @param view Rendering view to be used for rendering
		 * @param timestamp Preferred update timestamp
		 * @return Actually used update timestamp
		 */
		Timestamp preUpdate(const Rendering::ViewRef& view, const Timestamp timestamp);

		/**
		 * Pre-updates all scene description objects of a specified library only.
		 * @param library Name of the library
		 * @param view Rendering view to be used for rendering
		 * @param timestamp Preferred Update timestamp
		 * @return Actually used update timestamp
		 */
		Timestamp preUpdate(const std::string& library, const Rendering::ViewRef& view, const Timestamp timestamp);

		/**
		 * Updates all scene description objects needing regularly updates.
		 * @param view Rendering view to be used for rendering
		 * @param timestamp Update timestamp
		 */
		void update(const Rendering::ViewRef& view, const Timestamp timestamp);

		/**
		 * Updates all scene description objects of a specified library only.
		 * @param library Name of the library
		 * @param view Rendering view to be used for rendering
		 * @param timestamp Update timestamp
		 */
		void update(const std::string& library, const Rendering::ViewRef& view, const Timestamp timestamp);

		/**
		 * Returns the node reference of the first available scene description node with a specified name.
		 * This function searches in all libraries and all description types.
		 * @param name The name of the node to return
		 * @return Node reference of the node, the reference will be empty if the node does not exist
		 */
		NodeRef node(const std::string& name) const;

		/**
		 * Returns the node reference of the first available scene description node with a specified name defined inside a specific library.
		 * @param library The library to be searched
		 * @param name The name of the node to return
		 * @return Node reference of the node, the reference will be empty if the node does not exist
		 */
		NodeRef node(const std::string& library, const std::string& name) const;

		/**
		 * Returns all node references of all available scene description nodes with a specified name.
		 * This function searches in all libraries and all description types.
		 * @param name The name of the nodes to return
		 * @return All node references found
		 */
		NodeRefs nodes(const std::string& name) const;

		/**
		 * Returns all node references of all available scene description nodes with a specified name defined inside a specific library.
		 * @param library The library to be searched
		 * @param name The name of the nodes to return
		 * @return All node references found
		 */
		NodeRefs nodes(const std::string& library, const std::string& name) const;

		/**
		 * Releases all scene library.
		 */
		void release();

		/**
		 * Registers a new library.
		 * With each register call, the reference counter for a specific library will be incremented.
		 * Each call to registerLibrary() needs to be balanced with a corresponding call of unregisterLibrary() before shutting down.
		 * @param name The name of the library to register, must be valid
		 * @return True, if the library has not been registered before
		 * @tparam T The data type of the library to register
		 * @see unregisterLibrary().
		 */
		template <typename T>
		bool registerLibrary(const std::string& name);

		/**
		 * Unregisters a library.
		 * With each unregister call, the reference counter for a specific library will be decremented and removed from the system if the counter reaches zero.
		 * Each call to registerLibrary() needs to be balanced with a corresponding call of unregisterLibrary() before shutting down.
		 * @param name The name of the library to unregister, must be valid
		 * @return True, if the library was actually removed from the system (as the reference counter reached zero); False, if the library is still used by someone else
		 * @see registerLibrary().
		 */
		bool unregisterLibrary(const std::string& name);

	protected:

		/**
		 * Creates a new manager.
		 */
		Manager();

		/**
		 * Destructs a manager.
		 */
		virtual ~Manager();

		/**
		 * Registers a new event node.
		 * @param node Event node to register
		 */
		void registerEventNode(SDXEventNode& node);

		/**
		 * Unregisters an event node.
		 * @param node Event node to unregister
		 */
		void unregisterEventNode(SDXEventNode& node);

		/**
		 * Registers a new update node.
		 * @param node Update node to register
		 */
		void registerUpdateNode(SDXUpdateNode& node);

		/**
		 * Unregisters an update node.
		 * @param node Update node to unregister
		 */
		void unregisterUpdateNode(SDXUpdateNode& node);

	protected:

		/// The vector holding all registered scene description libraries.
		Libraries libraries_;

		/// Map holding all permanent scene description objects.
		PermanentSceneMap permanentSceneMap_;

		/// Map holding all event nodes.
		EventNodes eventNodes_;

		/// Map holding all update nodes.
		UpdateNodes updateNodes_;

		/// Manager lock.
		mutable Lock managerLock_;

		/// Library lock.
		Lock libraryLock_;
};

inline bool Manager::handlesMouseEvents() const
{
	const ScopedLock scopedLock(managerLock_);

	return eventNodes_.empty() == false;
}

template <typename T>
bool Manager::registerLibrary(const std::string& name)
{
	const ScopedLock mLock(managerLock_);
	const ScopedLock lLock(libraryLock_);

	// first we check whether the library has been registered already

	for (Libraries::iterator i = libraries_.begin(); i != libraries_.end(); ++i)
	{
		ocean_assert(i->first);

		if (i->first->name() == name)
		{
			++i->second;
			return false;
		}
	}

	// the library has not been registered before, so we insert the library based on the priority

	LibraryRef newLibrary = T::create();

	for (Libraries::iterator i = libraries_.begin(); i != libraries_.end(); ++i)
	{
		ocean_assert(i->first);

		if (i->first->priority() < newLibrary->priority())
		{
			libraries_.insert(i, std::make_pair(std::move(newLibrary), 1u));
			return true;
		}
	}

	// the library goes to the end

	libraries_.emplace_back(std::make_pair(std::move(newLibrary), 1u));
	return true;
}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_MANAGER_H
