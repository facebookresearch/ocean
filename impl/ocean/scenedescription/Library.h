/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_LIBRARY_H
#define META_OCEAN_SCENEDESCRIPTION_LIBRARY_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/NodeRefManager.h"
#include "ocean/scenedescription/SDLScene.h"
#include "ocean/scenedescription/SDLNode.h"

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration.
class Library;

/**
 * Definition of a object reference holding a library object.
 * @see ObjectRef, Library.
 * @ingroup media
 */
typedef ObjectRef<Library> LibraryRef;

/**
 * This class is the base class for all scene description libraries.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT Library
{
	friend class Manager;
	friend class ObjectRef<Library>;

	public:

		/**
		 * Definition of a map mapping file extensions to file type descriptions.
		 */
		typedef std::map<std::string, std::string> FileExtensions;

	public:

		/**
		 * Loads a new scene by a given filename.
		 * @param filename Entire filename of the scene to load
		 * @param fileExtension File extention of the scene to load
		 * @param engine Rendering engine to be connected with the scene description, must be defined for permanent scene description objects only
		 * @param timestamp The current timestamp, must be valid
		 * @param descriptionType Scene description type
		 * @param progress Progress state receiving recurrently information about the load state with range [0, 1]
		 * @param cancel Cancel state allows the cancelation of a load process while loading process hasn't finished, if the cancel state is used the load process stops if the value is set to True
		 * @return Resulting scene
		 * @see Manager::load().
		 */
		SceneRef load(const std::string& filename, const std::string& fileExtension, const Rendering::EngineRef& engine, const Timestamp& timestamp, const DescriptionType descriptionType, float* progress = nullptr, bool* cancel = nullptr);

		/**
		 * Returns the name of the library.
		 * @return Library name
		 */
		inline const std::string& name() const;

		/**
		 * Returns the description type supported by the library.
		 * @return Scene description type
		 */
		inline DescriptionType descriptionType() const;

		/**
		 * Returns the node reference of the first available scene description node with a specified name.
		 * @param name The name of the node to return
		 * @return Node reference of the node, the reference will be empty if the node does not exist
		 */
		virtual NodeRef node(const std::string& name) const;

		/**
		 * Returns all node references of all available scene description nodes with a specified name.
		 * @param name The name of the nodes to return
		 * @return All node references found
		 */
		virtual NodeRefs nodes(const std::string& name) const;

		/**
		 * Returns the node reference manager of this library.
		 * @return Node reference manager
		 */
		inline NodeRefManager& nodeManager() const;

		/**
		 * Releases the library.
		 */
		virtual void release();

	protected:

		/**
		 * Disabled copy constructor.
		 * @param library Object which would be copied
		 */
		Library(const Library& library) = delete;

		/**
		 * Creates a new library object.
		 * @param name The name of the library
		 * @param descriptionType Supported scene description type
		 * @param priority The priority of this library, the higher the value, the higher the priority, with range [0, infinity)
		 */
		Library(const std::string& name, const DescriptionType descriptionType, const unsigned int priority);

		/**
		 * Destructs a library object.
		 */
		virtual ~Library();

		/**
		 * Returns the priority of this library.
		 * In case two libraries support the same scene description format, the library with higher priority will be used first to load the scene.
		 * @return The library's priority, with range [0, infinity)
		 */
		inline unsigned int priority() const;

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
		 * Loads a new scene by a given filename and returns a scene hierarchy which can be used permanently.
		 * @param filename Entire filename of the scene to load
		 * @param fileExtension File extention of the scene to load
		 * @param engine Rendering engine to be connected with the scene description
		 * @param timestamp The current timestamp, must be valid
		 * @param progress Progress state receiving recurrently information about the load state with range [0, 1]
		 * @param cancel Cancel state allows the cancelation of a load process while loading process hasn't finished, if the cancel state is used the load process stops if the value is set to True
		 * @return Resulting scene
		 */
		virtual SceneRef loadPermanent(const std::string& filename, const std::string& fileExtension, const Rendering::EngineRef& engine, const Timestamp& timestamp, float* progress, bool* cancel);

		/**
		 * Loads a new scene by a given filename and returns a transient scene hierarchy which can be converted to a rendering scene graph afterwards.
		 * @param filename Entire filename of the scene to load
		 * @param fileExtension File extention of the scene to load
		 * @param progress Progress state receiving recurrently information about the load state with range [0, 1]
		 * @param cancel Cancel state allows the cancelation of a load process while loading process hasn't finished, if the cancel state is used the load process stops if the value is set to True
		 * @return Resulting scene
		 */
		virtual SceneRef loadTransient(const std::string& filename, const std::string& fileExtension, float* progress, bool* cancel);

		/**
		 * Disabled copy operator.
		 * @param library Object which would be copied
		 * @return Reference to this object
		 */
		Library& operator=(const Library& library) = delete;

		/**
		 * Unregisters a library at the manager.
		 * @param library Name of the library to unregister
		 * @return True, if succeeded
		 */
		static bool unregisterLibrary(const std::string& library);

	private:

		/// The name of the library.
		std::string name_;

		/// The priority of this library, the higher the value, the higher the priority.
		unsigned int priority_ = 0u;

		/// Map holding accepted file extensions supported by this library.
		FileExtensions fileExtensions_;

		/// Supported scene description type.
		DescriptionType descriptionType_;

		/// Node reference manager for this library only.
		mutable NodeRefManager nodeRefManager_;
};

inline const std::string& Library::name() const
{
	return name_;
}

inline Library::FileExtensions Library::registeredFileExtensions() const
{
	return fileExtensions_;
}

inline DescriptionType Library::descriptionType() const
{
	return descriptionType_;
}

inline NodeRefManager& Library::nodeManager() const
{
	return nodeRefManager_;
}

inline unsigned int Library::priority() const
{
	return priority_;
}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_LIBRARY_H
