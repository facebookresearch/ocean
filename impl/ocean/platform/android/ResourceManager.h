/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_RESOURCE_MANAGER_H
#define META_OCEAN_PLATFORM_ANDROID_RESOURCE_MANAGER_H

#include "ocean/platform/android/Android.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

namespace Ocean
{

namespace Platform
{

namespace Android
{

/**
 * This class implements a manager for resource data.
 * @ingroup platformandroid
 */
class ResourceManager : public Singleton<ResourceManager>
{
	friend class Singleton<ResourceManager>;

	public:

		/**
		 * Definition of types of external directories.
		 */
		enum class ExternalDirectoryType : uint32_t
		{
			/// The default type of external directory.
			DEFAULT_TYPE = 0u,
			/// Indicator for external directories that hold music.
			MUSIC,
			/// Indicator for external directories that hold podcasts.
			PODCASTS,
			/// Indicator for external directories that hold ringtones.
			RINGTONES,
			/// Indicator for external directories that hold alarm data.
			ALARMS,
			/// Indicator for external directories that hold notification data.
			NOTIFICATIONS,
			/// Indicator for external directories that hold pictures.
			PICTURES,
			/// Indicator for external directories that hold movies
			MOVIES,
		};

		/**
		 * This class provides access to the memory of an asset file as long as the object exists.
		 */
		class ScopedResource
		{
			friend class ResourceManager;

			public:

				/**
				 * Destructs the object and releases all resources.
				 */
				inline ~ScopedResource();

				/**
				 * Returns the memory pointer to the content of the asset file.
				 * @return The asset file's memory pointer
				 */
				inline const void* data() const;

				/**
				 * Returns the size of the content of the asset file in bytes.
				 * @return The asset file's memory size in bytes
				 */
				inline size_t size() const;

			protected:

				/**
				 * Disabled copy constructor.
				 * @param scopedResource The object which would be copied
				 */
				ScopedResource(const ScopedResource& scopedResource) = delete;

				/**
				 * Creates a new object.
				 * @param asset The asset handle, must be valid
				 * @param data The pointer to the asset's memory, must be valid
				 * @param size the size of the asset in bytes, with range [1, infinity)
				 */
				inline ScopedResource(AAsset* asset, const void* data, const size_t size);

				/**
				 * Disabled assign operator.
				 * @param scopedResource The object which would be assigned
				 * @return The reference to this object
				 */
				ScopedResource& operator=(const ScopedResource& scopedResource) = delete;

			protected:

				/// The asset handle.
				AAsset* const asset_ = nullptr;

				/// The pointer to the asset's memory.
				const void* const data_ = nullptr;

				/// The size of the asset in bytes, with range [1, infinity)
				const size_t size_ = 0;
		};

	public:

		/**
		 * Initializes the singleton using the Java virtual machine and Android activity
		 * @param javaVM The Java virtual machine, must be valid
		 * @param activity The Android main activity, must be valid
		 * @param assetManager The optional Android asset manager to be used; if not specified, it will be retrieved from the main activity
		 * @return True, if succeeded
		 */
		bool initialize(JavaVM* javaVM, jobject activity, AAssetManager* assetManager = nullptr);

		/**
		 * Reads an asset file to memory.
		 * @param assetFilename The filename of the asset to read, must be valid
		 * @param data The resulting memory with the read asset file, may be empty if the asset is empty
		 * @return True, if succeeded
		 */
		bool readAsset(const std::string& assetFilename, std::vector<uint8_t>& data) const;

		/**
		 * Accesses an asset file and returns a resource object providing the memory pointer to the asset.
		 * @param assetFilename The filename of the asset to read, must be valid
		 * @return The resource object, nullptr if invalid
		 */
		std::unique_ptr<ScopedResource> accessAsset(const std::string& assetFilename) const;

		/**
		 * Copies all assets located in a specified asset folder to a specified target location.
		 * @param targetDirectoryName The directory where the assets will be copied to; must be valid and directory must be created before calling this function
		 * @param createDirectory If true it will allow the function to create the destination directory, if it doesn't exist already.
		 * @param assetDirectoryName The name of the asset directory from which all files will be copied, an empty name to use the root asset directory
		 * @return True if succeeded
		 */
		bool copyAssets(const std::string& targetDirectoryName, const bool createDirectory = false, const std::string& assetDirectoryName = std::string()) const;

		/**
		 * Returns whether a specific asset directory exists and whether this directory holds at least one file.
		 * @param assetDirectoryName The name of the asset directory to be checked, can be any sub-directory, must be valid
		 * @return True, if so
		 */
		bool doesAssetDirectoryExist(const std::string& assetDirectoryName) const;

		/**
		 * Returns an app-specific external directory to store data
		 * Make sure to initialize the manager with a valid Java VM and the object of the main activity.
		 * @return The directory; if successful; this will be an empty string without prior initialization
		 * @sa initialize()
		 */
		inline std::string externalFilesDirectory() const;

		/**
		 * Returns true if this object is initialized
		 * @return True, if correctly initialized
		 */
		inline bool isValid() const;

		/**
		 * Returns an app-specific external directory to store data
		 * @param env The JNI environment of this application, must be valid
		 * @param activity The Android main activity, must be valid
		 * @param externalDirectoryName The resulting name of the app-specific external directory, will be terminated with the default separator of the current platform
		 * @param externalDirectoryType The type of the external directory to be returned
		 * @return True, if the directory exists and if it is readable and writable, otherwise false
		 * @sa IO::Path::defaultSeparator()
		 */
		static bool getExternalFilesDirectory(JNIEnv* env, jobject activity, std::string& externalDirectoryName, const ExternalDirectoryType externalDirectoryType = ExternalDirectoryType::DEFAULT_TYPE);

	protected:

		/**
		 * Creates a new manager object.
		 */
		ResourceManager();

	protected:

		// The Android asset manager which is used.
		AAssetManager* assetManager_ = nullptr;

		/// The lock of the manager.
		mutable Lock lock_;

		/// The name of the app-specific external directory.
		std::string externalDirectoryName_;
};

inline ResourceManager::ScopedResource::ScopedResource(AAsset* asset, const void* data, const size_t size) :
	asset_(asset),
	data_(data),
	size_(size)
{
	ocean_assert(asset_ != nullptr && data_ != nullptr && size_ != 0);
}

inline ResourceManager::ScopedResource::~ScopedResource()
{
	AAsset_close(asset_);
}

inline const void* ResourceManager::ScopedResource::data() const
{
	return data_;
}

inline size_t ResourceManager::ScopedResource::size() const
{
	return size_;
}

inline bool ResourceManager::isValid() const
{
	const ScopedLock scopedLock(lock_);
	return assetManager_ != nullptr;
}

std::string ResourceManager::externalFilesDirectory() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(!externalDirectoryName_.empty());
	return externalDirectoryName_;
}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_RESOURCE_MANAGER_H
