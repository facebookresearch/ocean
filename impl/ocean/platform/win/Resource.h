/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_RESOURCE_H
#define META_OCEAN_PLATFORM_WIN_RESOURCE_H

#include "ocean/platform/win/Win.h"
#include "ocean/platform/win/System.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Singleton.h"

#include <vector>

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class implements function allowing to operate with resource data.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT Resource
{
	public:

		/**
		 * Definition of a vector holding 8 bit values.
		 */
		typedef std::vector<unsigned char> Buffer;

	public:

		/**
		 * Extracts the specific resource information frosm a given module specified by the id and type of the resource.
		 * @param module The handle of the module from which the resource is requested, nullptr to extract the resource from the module which created the current process
		 * @param resourceId The id of the resource
		 * @param type The type of the resource
		 * @param buffer The resulting resource information
		 * @return True, if succeeded
		 */
		static bool extractResource(const HMODULE module, const unsigned int resourceId, const std::wstring& type, Buffer& buffer);
};

/**
 * This class implements a manager for resource data.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT ResourceManager : public Singleton<ResourceManager>
{
	friend class Singleton<ResourceManager>;

	protected:

		/**
		 * Definition of a map mapping resource ids to frames.
		 */
		typedef std::map<unsigned int, FrameRef> FrameMap;

	public:

		/**
		 * Returns a specific resource frame.
		 * @param resourceId The id of the resource
		 * @param type The type of the frame, e.g., PNG, BMP, etc.
		 * @param module The handle of the module from which the resource is requested, nullptr to extract the resource from the module which created the current process
		 * @return The resource frame, if any
		 */
		FrameRef frame(const unsigned int resourceId, const std::wstring& type, const HMODULE module = System::currentLibraryModule());

		/**
		 * Releases the entire resources of this manager.
		 */
		void release();

	protected:

		/**
		 * Creates a new manager object.
		 */
		inline ResourceManager();

	protected:

		/// The lock of the manager.
		Lock lock_;

		/// The frames of the manager.
		FrameMap frameMap_;
};

inline ResourceManager::ResourceManager()
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_RESOURCE_H
