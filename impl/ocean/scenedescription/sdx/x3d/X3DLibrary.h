/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_LIBRARY_H
#define META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_LIBRARY_H

#include "ocean/scenedescription/sdx/x3d/X3D.h"

#include "ocean/scenedescription/Library.h"
#include "ocean/scenedescription/Manager.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

/**
 * This class implements the scene library for all x3d file types.
 * @ingroup scenedescriptionsdxx3d
 */
class OCEAN_SCENEDESCRIPTION_SDX_X3D_EXPORT X3DLibrary : public Library
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

	private:

		/**
		 * Creates a new x3d library.
		 */
		X3DLibrary();

		/**
		 * Destructs a x3d library.
		 */
		~X3DLibrary() override;

		/**
		 * Creates this library and returns it an object reference.
		 * @return The new library object
		 */
		static LibraryRef create();

		/**
		 * Loads a new scene by a given filename and returns a scene hierarchy which can be used permanently.
		 * @see Library::loadPermanent().
		 */
		SceneRef loadPermanent(const std::string& filename, const std::string& fileExtension, const Rendering::EngineRef& engine, const Timestamp& timestamp, float* progress, bool* cancel) override;
};

}

}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_SDX_X3D_X3D_LIBRARY_H
