/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_MANAGER_H
#define META_OCEAN_INTERACTION_MANAGER_H

#include "ocean/interaction/Interaction.h"
#include "ocean/interaction/Library.h"

#include "ocean/base/Singleton.h"
#include "ocean/base/Timestamp.h"

#include "ocean/io/FileManager.h"

namespace Ocean
{

namespace Interaction
{

/**
 * This class implements the manager for all interaction libraries / plugins.
 * @ingroup interaction
 */
class OCEAN_INTERACTION_EXPORT Manager :
	public IO::FileManager,
	public Singleton<Manager>
{
	friend class Singleton<Manager>;
	friend class Library;

	protected:

		/**
		 * Definition of a set holding registered interaction libraries.
		 */
		typedef std::unordered_set<Library*> LibrarySet;

		/**
		 * Definition of a set holding names of interaction libraries.
		 */
		typedef std::unordered_set<std::string> NameSet;

		/**
		 * Definition of a vector holding names.
		 */
		typedef std::vector<std::string> Names;

	public:

		/**
		 * Loads a new interaction file.
		 * @param userInterface The application's UI elements
		 * @param engine Current engine
		 * @param timestamp Recent timestmap
		 * @param filename Filename of the interaction to load
		 * @return True, if succeeded
		 */
		bool load(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename);

		/**
		 * Unloads one specific loaded interaction files.
		 * @param userInterface The application's UI elements
		 * @param engine Current engine
		 * @param timestamp Recent timestmap
		 * @param filename Filename of the interaction to be unloaded
		 * @return True, if succeeded
		 */
		bool unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename);

		/**
		 * Unloads all loaded interaction files.
		 * @param userInterface The application's UI elements
		 * @param engine Current engine
		 * @param timestamp Recent timestmap
		 */
		void unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp);

		/**
		 * Returns whether currently at least one interaction library handles mouse events.
		 * Because mouse events can produce high computational overhead this test can safe computational time.
		 * @return True, if so
		 */
		inline bool handlesMouseEvents() const;

		/**
		 * Pre file load interaction function.
		 * @param userInterface The application's UI elements
		 * @param filename Filename of the file to be loaded
		 */
		void preFileLoad(const UserInterface& userInterface, const std::string& filename);

		/**
		 * Post file load interaction function.
		 * @param userInterface The application's UI elements
		 * @param filename Filename of the loaded filename
		 * @param succeeded State determining whether the file has been loaded successfully
		 */
		void postFileLoad(const UserInterface& userInterface, const std::string& filename, const bool succeeded);

		/**
		 * Pre update interaction function.
		 * This function is invoked before each update process, the application interaction module has the possibility to return a different preferred update timestamp.<br>
		 * If several interaction modules are used and each is changing the timestamp each changed timestamp is given to the next module as preferred timestamp.<br>
		 * Therefore the last registered interaction module has the possiblity to define the used timestamp.<br>
		 * @param userInterface The application's UI elements
		 * @param engine Engine to be updated, must be valid
		 * @param view The view which will be used to render, must be valid
		 * @param timestamp Recent update timestmap
		 * @return Timestamp which should be used for the update process, if the timestamp is null any timestamp can be used
		 */
		Timestamp preUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp);

		/**
		 * Post update interaction function.
		 * @param userInterface The application's UI elements
		 * @param engine Engine which has been updated, must be valid
		 * @param view The view which will be used to render, must be valid
		 * @param timestamp Recent update timestamp
		 */
		void postUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp);

		/**
		 * Mouse press event function.
		 * @param userInterface The application's UI elements
		 * @param engine Rendering engine currently used
		 * @param button Pressed mouse button
		 * @param screenPosition 2D screen position of the mouse cursor
		 * @param ray 3D picking ray
		 * @param pickedObject Name of the possible picked object
		 * @param pickedPosition Possible intersection point between mouse-pick-ray and 3D object
		 * @param timestamp Event timestamp
		 */
		void onMousePress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp);

		/**
		 * Mouse move event function.
		 * @param userInterface The application's UI elements
		 * @param engine Rendering engine currently used
		 * @param button Pressed mouse button
		 * @param screenPosition 2D screen position of the mouse cursor
		 * @param ray 3D picking ray
		 * @param pickedObject Name of the possible picked object
		 * @param pickedPosition Possible intersection point between mouse-pick-ray and 3D object
		 * @param timestamp Event timestamp
		 */
		void onMouseMove(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp);

		/**
		 * Mouse release event function.
		 * @param userInterface The application's UI elements
		 * @param engine Rendering engine currently used
		 * @param button Released mouse button
		 * @param screenPosition 2D screen position of the mouse cursor
		 * @param ray 3D picking ray
		 * @param pickedObject Name of the possible picked object
		 * @param pickedPosition Possible intersection point between mouse-pick-ray and 3D object
		 * @param timestamp Event timestamp
		 */
		void onMouseRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp);

		/**
		 * Key press function.
		 * @param userInterface The application's UI elements
		 * @param engine Rendering engine currently used
		 * @param key Key which has been pressed
		 * @param timestamp Event timestamp
		 */
		void onKeyPress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp);

		/**
		 * Key release function.
		 * @param userInterface The application's UI elements
		 * @param engine Rendering engine currently used
		 * @param key Key which has been pressed
		 * @param timestamp Event timestamp
		 */
		void onKeyRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp);

		/**
		 * Returns all currently supported file extensions.
		 * @see FileManager::supportedExtensions().
		 */
		virtual FileExtensions supportedExtensions();

		/**
		 * Returns the names of all currently registered libraries.
		 * @return Library names
		 */
		Names libraries() const;


		/**
		 * Releases all interaction libraries.
		 */
		void release();

		/**
		 * Registers an interaction library handling mouse events.
		 * @param name The name of the library interested in mouse events
		 */
		void registerMouseEventLibrary(const std::string& name);

		/**
		 * Unregisters an interaction library handling mouse events.
		 * @param name The name of the library not interested in mouse events anymore
		 * @return True, if succeeded
		 */
		bool unregisterMouseEventLibrary(const std::string& name);

		/**
		 * Retruns the lock object of this engine.
		 * Beware: Use this lock with caution.
		 * @return Engine lock object
		 */
		inline Lock& lock() const;

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
		 * Registers a new interaction library able to read a specific interaction format.
		 * @param library Library to register
		 */
		void registerLibrary(Library& library);

		/**
		 * Unregisters a library.
		 * @param name The name of the library to unregister
		 * @return True, if succeeded
		 */
		bool unregisterLibrary(const std::string& name);

	protected:

		/// Set holding all registered interaction libraries.
		LibrarySet librarySet_;

		/**
		 * Set holding all names of libraries interested in mouse events.
		 * This set is used to decided whether mouse events should be processed for this interaction libraries.
		 */
		NameSet mouseEventLibraries_;

		/// Manager lock.
		mutable Lock lock_;
};

inline bool Manager::handlesMouseEvents() const
{
	return mouseEventLibraries_.empty() == false;
}

inline Lock& Manager::lock() const
{
	return lock_;
}

}

}

#endif // META_OCEAN_INTERACTION_MANAGER_H
