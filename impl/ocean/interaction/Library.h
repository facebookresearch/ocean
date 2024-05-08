/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_LIBRARY_H
#define META_OCEAN_INTERACTION_LIBRARY_H

#include "ocean/interaction/Interaction.h"
#include "ocean/interaction/UserInterface.h"

#include "ocean/base/Timestamp.h"

#include "ocean/rendering/Engine.h"

namespace Ocean
{

namespace Interaction
{

/**
 * This class implements the base class for all interaction libraries.
 * @ingroup interaction
 */
class OCEAN_INTERACTION_EXPORT Library
{
	friend class Manager;

	public:

		/**
		 * Definition of a map mapping file extensions to file type descriptions.
		 */
		typedef std::map<std::string, std::string> FileExtensions;

	public:

		/**
		 * Returns the name of this library.
		 * @return Library name
		 */
		inline const std::string& name() const;

		/**
		 * Returns whether a specified file extension is supported by this interaction plugin.
		 * @param extension File extension to check
		 * @return True, if suo
		 */
		virtual bool isFileExtensionSupported(const std::string& extension) const;

		/**
		 * Loads a new interaction file.
		 * @param userInterface The application's UI elements
		 * @param engine Current engine
		 * @param timestamp Recent timestmap
		 * @param filename Name of the interaction file to load
		 * @return True, if succeeded
		 */
		virtual bool load(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename) = 0;

		/**
		 * Unloads one specific loaded interaction files.
		 * @param userInterface The application's UI elements
		 * @param engine Current engine
		 * @param timestamp Recent timestmap
		 * @param filename Filename of the interaction to be unloaded
		 * @return True, if succeeded
		 * @see Manager::unload().
		 */
		virtual bool unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& filename) = 0;

		/**
		 * Unloads all loaded scripts.
		 * @param userInterface The application's UI elements
		 * @param engine Current engine
		 * @param timestamp Recent timestmap
		 */
		virtual void unload(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) = 0;

		/**
		 * Pre file load interaction function.
		 * @param userInterface The application's UI elements
		 * @param filename Filename of the file to be loaded
		 */
		virtual void preFileLoad(const UserInterface& userInterface, const std::string& filename);

		/**
		 * Post file load interaction function.
		 * @param userInterface The application's UI elements
		 * @param filename Filename of the loaded filename
		 * @param succeeded State determining whether the file has been loaded successfully
		 */
		virtual void postFileLoad(const UserInterface& userInterface, const std::string& filename, const bool succeeded);

		/**
		 * Pre update interaction function.
		 * This function is invoked before each update process, the application interaction module has the possibility to return a different preferred update timestamp.
		 * @param userInterface The application's UI elements
		 * @param engine Engine to be updated, must be valid
		 * @param view The view which will be used to render, must be valid
		 * @param timestamp Recent update timestmap
		 * @return Timestamp which should be used for the update process, if the timestamp is null any timestamp can be used
		 */
		virtual Timestamp preUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp);

		/**
		 * Post update interaction function.
		 * @param userInterface The application's UI elements
		 * @param engine Engine which has been updated, must be valid
		 * @param view The view which will be used to render, must be valid
		 * @param timestamp Recent update timestamp
		 */
		virtual void postUpdate(const UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp);

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
		virtual void onMousePress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp);

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
		virtual void onMouseMove(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp);

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
		virtual void onMouseRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Rendering::ObjectId pickedObject, const Vector3& pickedPosition, const Timestamp timestamp);

		/**
		 * Key press function.
		 * @param userInterface The application's UI elements
		 * @param engine Rendering engine currently used
		 * @param key Key which has been pressed
		 * @param timestamp Event timestamp
		 */
		virtual void onKeyPress(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp);

		/**
		 * Key release function.
		 * @param userInterface The application's UI elements
		 * @param engine Rendering engine currently used
		 * @param key Key which has been pressed
		 * @param timestamp Event timestamp
		 */
		virtual void onKeyRelease(const UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp);

		/**
		 * Releases this library.
		 */
		virtual void release();

	protected:

		/**
		 * Creates a new library object.
		 * @param name The name of the library
		 */
		Library(const std::string& name);

		/**
		 * Destructs a library object.
		 */
		virtual ~Library();

		/**
		 * Returns all registered file extensions.
		 * @return Registered file extensions
		 */
		inline FileExtensions registeredFileExtensions() const;

		/**
		 * Registers an additional file extension.
		 * @param extension New file extension to register
		 * @param description Description of the file type
		 */
		void registerFileExtension(const std::string& extension, const std::string& description);

		/**
		 * Registers a library at the manager.
		 * Each library should be registered at most once.
		 * @param library Library to register
		 * @return True, if the library hasn't been registered before
		 */
		static bool registerFactory(Library& library);

		/**
		 * Unregisters a library at the manager.
		 * @param library Name of the library to unregister
		 * @return True, if succeeded
		 */
		static bool unregisterLibrary(const std::string& library);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param library Object which would be copied
		 */
		Library(const Library& library) = delete;

		/**
		 * Disabled copy operator.
		 * @param library Object which would be copied
		 * @return Reference to this object
		 */
		Library& operator=(const Library& library) = delete;

	protected:

		/// Library name.
		std::string libraryName;

		/// Map holding accepted file extensions supported by this library.
		FileExtensions libraryFileExtensions;
};

inline const std::string& Library::name() const
{
	return libraryName;
}

inline Library::FileExtensions Library::registeredFileExtensions() const
{
	return libraryFileExtensions;
}

}

}

#endif // META_OCEAN_INTERACTION_LIBRARY_H
