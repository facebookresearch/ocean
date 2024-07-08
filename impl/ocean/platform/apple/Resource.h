/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_RESOURCE_H
#define META_OCEAN_PLATFORM_APPLE_RESOURCE_H

#include "ocean/platform/apple/Apple.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace Platform
{

namespace Apple
{

/**
 * This class implements functions allowing to operate with resource data.
 * @ingroup platformapple
 */
class Resource
{
	public:

		/**
		 * Definition of a vector holding 8 bit values.
		 */
		typedef std::vector<uint8_t> Buffer;

	public:

		/**
		 * Extracts specific resource information specified by the name and type of the resource.
		 * @param bundle The name of the bundle from which the resource will be extracted, an empty string to use the main bundle
		 * @param name The name of the resource
		 * @param type The type of the resource
		 * @param buffer The resulting resource information
		 * @return True, if succeeded
		 */
		static bool extractResource(const std::wstring& bundle, const std::wstring& name, const std::wstring& type, Buffer& buffer);

		/**
		 * Determines the path of a specified resource file.
		 * @param name The name of the resource
		 * @param type The type of the resource
		 * @param bundle The name of the bundle from which the resource will be extracted, an empty string to use the main bundle
		 * @return The path of the resource file within the bundle
		 */
		static std::wstring resourcePath(const std::wstring& name, const std::wstring& type, const std::wstring& bundle = std::wstring());

		/**
		 * Determines the paths of all resource files with specified type.
		 * @param type The type of the resource
		 * @param directory Optional directory in which the resource files are located
		 * @param bundle The name of the bundle from which the resource will be extracted, an empty string to use the main bundle
		 * @return The path of the resource file within the bundle
		 */
		static std::vector<std::wstring> resourcePaths(const std::wstring& type, const std::wstring& directory = std::wstring(), const std::wstring& bundle = std::wstring());
};

/**
 * This class implements a manager for resource data.
 * @ingroup platformapple
 */
class ResourceManager : public Singleton<ResourceManager>
{
	friend class Singleton<ResourceManager>;

	protected:

		/**
		 * Definition of a pair combining a resource name and a resource type.
		 */
		typedef std::pair<std::wstring, std::wstring> ResourcePair;

		/**
		 * Definition of a map mapping resources pair to frames.
		 */
		typedef std::map<ResourcePair, FrameRef> FrameMap;

	public:

		/**
		 * Returns a specific resource frame.
		 * @param name The name of the resource
		 * @param type The type of the resource frame, the file extension: e.g., png, bmp, etc.
		 * @param bundle The name of the bundle from which the resource will be extracted, an empty string to use the main bundle
		 * @return The resource frame, if any
		 */
		FrameRef frame(const std::wstring& name, const std::wstring& type, const std::wstring& bundle = std::wstring());

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
		FrameMap frames_;
};

inline ResourceManager::ResourceManager()
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_RESOURCE_H
