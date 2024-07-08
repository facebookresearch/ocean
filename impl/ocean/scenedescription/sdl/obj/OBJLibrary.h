/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_OBJ_LIBRARY_H
#define META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_OBJ_LIBRARY_H

#include "ocean/scenedescription/sdl/obj/OBJ.h"

#include "ocean/scenedescription/Library.h"
#include "ocean/scenedescription/Manager.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDL
{

namespace OBJ
{

/**
 * This class implements the scene library for all obj file types.
 * @ingroup scenedescriptionsdlobj
 */
class OCEAN_SCENEDESCRIPTION_SDL_OBJ_EXPORT OBJLibrary : public Library
{
	friend class SceneDescription::Manager;

	public:

		/**
		 * Registers this library at the global scene description manager.
		 * With each register call, the reference counter for a specific library will be incremented.
		 * Each call to registerLibrary() needs to be balanced with a corresponding call of unregisterLibrary() before shutting down.
		 * @return True, if the library has not been registered before
		 * @see Manager, unregisterLibrary()
		 */
		static bool registerLibrary();

		/**
		 * Unregisters this library at the global scene description manager.
		 * With each unregister call, the reference counter for a specific library will be decremented and removed from the system if the counter reaches zero.
		 * Each call to registerLibrary() needs to be balanced with a corresponding call of unregisterLibrary() before shutting down.
		 * @return True, if the library was actually removed from the system (as the reference counter reached zero); False, if the library is still used by someone else
		 * @see registerLibrary().
		 */
		static bool unregisterLibrary();

	protected:

		/**
		 * Creates a new obj library.
		 */
		OBJLibrary();

		/**
		 * Destructs a obj library.
		 */
		~OBJLibrary() override;

		/**
		 * Creates this library and returns it an object reference.
		 * @return The new library object
		 */
		static LibraryRef create();

		/**
		 * Loads a new scene by a given filename and returns a transient scene hierarchy which can be converted to a rendering scene graph afterwards.
		 * @see Library::loadTransient().
		 */
		SceneRef loadTransient(const std::string& filename, const std::string& fileExtension, float* progress, bool* cancel) override;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDL_OBJ_OBJ_LIBRARY_H
