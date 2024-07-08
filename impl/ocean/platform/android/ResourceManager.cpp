/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/ResourceManager.h"
#include "ocean/platform/android/ScopedJNIEnvironment.h"
#include "ocean/platform/android/ScopedJNIObject.h"
#include "ocean/platform/android/Utilities.h"

#include "ocean/io/File.h"
#include "ocean/io/Directory.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

ResourceManager::ResourceManager() :
	assetManager_(nullptr)
{
	// nothing to do here
}

bool ResourceManager::initialize(JavaVM* javaVM, jobject activity, AAssetManager* assetManager)
{
	const ScopedLock scopedLock(lock_);

	if (javaVM == nullptr || activity == nullptr)
	{
		assert(false && "Invalid input!");
		return false;
	}

	if (assetManager_ != nullptr)
	{
		ocean_assert(false && "The ResourceManager is already initialized!");
		return false;
	}

	const ScopedJNIEnvironment scopedJNIEnvironment(javaVM);

	if (!scopedJNIEnvironment)
	{
		return false;
	}

	if (assetManager != nullptr)
	{
		assetManager_ = assetManager;
	}
	else
	{
		const ScopedJClass activityClass(scopedJNIEnvironment, scopedJNIEnvironment.jniEnv()->GetObjectClass(activity));

		if (!activityClass.isValid())
		{
			return false;
		}

		jmethodID getAssetsMethodId = scopedJNIEnvironment.jniEnv()->GetMethodID(activityClass, "getAssets", "()Landroid/content/res/AssetManager;");

		if (getAssetsMethodId == nullptr)
		{
			return false;
		}

		const ScopedJObject jAssetManagerObject(scopedJNIEnvironment, scopedJNIEnvironment.jniEnv()->CallObjectMethod(activity, getAssetsMethodId));

		if (!jAssetManagerObject.isValid())
		{
			return false;
		}

		assetManager_ = AAssetManager_fromJava(scopedJNIEnvironment.jniEnv(), jAssetManagerObject);
	}

	if (assetManager_ == nullptr)
	{
		return false;
	}

	if (!getExternalFilesDirectory(scopedJNIEnvironment.jniEnv(), activity, externalDirectoryName_, ExternalDirectoryType::DEFAULT_TYPE) || externalDirectoryName_.empty())
	{
		return false;
	}

	return true;
}

bool ResourceManager::readAsset(const std::string& assetFilename, std::vector<uint8_t>& data) const
{
	ocean_assert(!assetFilename.empty());

	if (!isValid())
	{
		return false;
	}

	AAsset* asset = AAssetManager_open(assetManager_, assetFilename.c_str(), AASSET_MODE_BUFFER);

	if (asset == nullptr)
	{
		return false;
	}

	bool succeeded = false;

	const off64_t fileSize = AAsset_getLength64(asset);

	if (fileSize == 0)
	{
		data.clear();
		return true;
	}

	if (fileSize != 0)
	{
		data.resize(fileSize);

		succeeded = AAsset_read(asset, data.data(), data.size()) == fileSize;
	}

	AAsset_close(asset);

	return succeeded;
}

std::unique_ptr<ResourceManager::ScopedResource> ResourceManager::accessAsset(const std::string& assetFilename) const
{
	ocean_assert(!assetFilename.empty());

	if (!isValid())
	{
		return nullptr;
	}

	AAsset* asset = AAssetManager_open(assetManager_, assetFilename.c_str(), AASSET_MODE_BUFFER);

	if (asset == nullptr)
	{
		return nullptr;
	}

	const void* buffer = AAsset_getBuffer(asset);
	const size_t size = AAsset_getLength64(asset);

	if (buffer == nullptr || size == 0)
	{
		AAsset_close(asset);
		return nullptr;
	}

	return std::unique_ptr<ScopedResource>(new ScopedResource(asset, buffer, size));
}

bool ResourceManager::copyAssets(const std::string& targetDirectoryName, const bool createDirectory, const std::string& assetDirectoryName) const
{
	ocean_assert(!targetDirectoryName.empty());

	if (!isValid())
	{
		Log::error() << "ResourceManager: Not initialized yet";
		return false;
	}

	const IO::Directory targetDirectory(targetDirectoryName);

	if (createDirectory && !targetDirectory.exists())
	{
		if (!targetDirectory.create())
		{
			Log::error() << "ResourceManager: Failed to create directory '" << targetDirectory() << "'";
			return false;
		}
	}

	if (!targetDirectory.exists())
	{
		ocean_assert(false && "Destination directory does not exist!");
		return false;
	}

	const IO::Directory assetDirectory(assetDirectoryName);

	AAssetDir* aAssetDirectory = AAssetManager_openDir(assetManager_, assetDirectory.base().c_str());

	if (aAssetDirectory == nullptr)
	{
		Log::error() << "The specified asset directory '" << assetDirectoryName << "' does not exist";
		return false;
	}

	bool allSucceeded = true;

	const char* localAssetFilename = AAssetDir_getNextFileName(aAssetDirectory);

	if (localAssetFilename == nullptr)
	{
		Log::warning() << "ResourceManager: No asset to copy at location '" << assetDirectoryName << "'";
	}

	std::vector<uint8_t> buffer;

	while (localAssetFilename != nullptr)
	{
		const IO::File localAssetFile(localAssetFilename);

		buffer.clear();

		if (!readAsset((assetDirectory + localAssetFile)(), buffer))
		{
			allSucceeded = false;

			Log::error() << "ResourceManager: Failed to read asset '" << (assetDirectory + localAssetFile)() << "'";
			break;
		}

		if (!buffer.empty())
		{
			const IO::File targetFile(targetDirectory + localAssetFile);

			std::ofstream destinationFile(targetFile(), std::ofstream::binary);

			if (!destinationFile.is_open())
			{
				ocean_assert(false && "Cannot write to file");
				allSucceeded = false;

				Log::error() << "ResourceManager: Failed to write file '" << targetFile() << "'";
				break;
			}

			destinationFile.write((const char*)(buffer.data()), buffer.size());

			if (destinationFile.good())
			{
				Log::debug() << "ResourceManager: Copied asset file '" << targetFile() << "'";
			}
			else
			{
				Log::error() << "ResourceManager: Failed to write file '" << targetFile() << "'";
			}
		}

		localAssetFilename = AAssetDir_getNextFileName(aAssetDirectory);
	}

	AAssetDir_close(aAssetDirectory);

	return allSucceeded;
}

bool ResourceManager::doesAssetDirectoryExist(const std::string& assetDirectoryName) const
{
	const IO::Directory assetDirectory(assetDirectoryName);

	AAssetDir* aAssetDirectory = AAssetManager_openDir(assetManager_, assetDirectory.base().c_str());

	if (aAssetDirectory == nullptr)
	{
		return false;
	}

	const bool directoryIsNotEmpty = AAssetDir_getNextFileName(aAssetDirectory) != nullptr;

	AAssetDir_close(aAssetDirectory);

	return directoryIsNotEmpty;
}

bool ResourceManager::getExternalFilesDirectory(JNIEnv* env, jobject activity, std::string& externalDirectoryName, const ExternalDirectoryType externalDirectoryType)
{
	ocean_assert(env != nullptr && activity != nullptr);

	const ScopedJClass jRootActivityClass(*env, env->GetObjectClass(activity));

	if (!jRootActivityClass)
	{
		return false;
	}

	jmethodID jGetExternalFilesDirMethod = env->GetMethodID(jRootActivityClass, "getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;");

	if (!jGetExternalFilesDirMethod)
	{
		return false;
	}

	const ScopedJClass jEnvironmentClass(*env, env->FindClass("android/os/Environment"));

	if (!jEnvironmentClass.isValid())
	{
		return false;
	}

	std::string environmentFieldName;
	switch (externalDirectoryType)
	{
		case ExternalDirectoryType::MUSIC:
			environmentFieldName = "DIRECTORY_MUSIC";
			break;

		case ExternalDirectoryType::PODCASTS:
			environmentFieldName = "DIRECTORY_PODCASTS";
			break;

		case ExternalDirectoryType::RINGTONES:
			environmentFieldName = "DIRECTORY_RINGTONES";
			break;

		case ExternalDirectoryType::ALARMS:
			environmentFieldName = "DIRECTORY_ALARMS";
			break;

		case ExternalDirectoryType::NOTIFICATIONS:
			environmentFieldName = "DIRECTORY_NOTIFICATIONS";
			break;

		case ExternalDirectoryType::PICTURES:
			environmentFieldName = "DIRECTORY_PICTURES";
			break;

		case ExternalDirectoryType::MOVIES:
			environmentFieldName = "DIRECTORY_MOVIES";
			break;

		case ExternalDirectoryType::DEFAULT_TYPE:
			environmentFieldName = "";
			break;
	}

	ScopedJString jExternalDirectoryTypeString;
	if (!environmentFieldName.empty())
	{
		jfieldID jEnvironmentFieldId = env->GetStaticFieldID(jEnvironmentClass, environmentFieldName.c_str(), "Ljava/lang/String;");

		if (jEnvironmentFieldId == nullptr)
		{
			return false;
		}

		jExternalDirectoryTypeString = ScopedJString(*env, (jstring)(env->GetStaticObjectField(jEnvironmentClass, jEnvironmentFieldId)));

		if (!jExternalDirectoryTypeString.isValid())
		{
			return false;
		}
	}

	const ScopedJObject jFileObject(*env, env->CallObjectMethod(activity, jGetExternalFilesDirMethod, jExternalDirectoryTypeString.object())); // jExternalDirectoryTypeString may be nullptr

	if (!jFileObject.isValid())
	{
		return false;
	}

	const ScopedJClass jFileClass(*env, env->FindClass("java/io/File"));

	if (!jFileClass.isValid())
	{
		return false;
	}

	jmethodID jFileGetPathMethod = env->GetMethodID(jFileClass, "getPath", "()Ljava/lang/String;");

	if (!jFileGetPathMethod)
	{
		return false;
	}

	const ScopedJString jDirectoryNameString(*env, (jstring)env->CallObjectMethod(jFileObject, jFileGetPathMethod));

	if (!jDirectoryNameString.isValid())
	{
		return false;
	}

	const IO::Directory externalDirectory(Platform::Android::Utilities::toAString(env, jDirectoryNameString));

	if (!externalDirectory.isValid() || !externalDirectory.exists())
	{
		Log::error() << "Invalid or non-existant external directory.";
		return false;
	}

	externalDirectoryName = externalDirectory();
	ocean_assert(!externalDirectoryName.empty() && externalDirectoryName.back() == IO::Path::defaultSeparator());

	return true;
}

}

}

}
