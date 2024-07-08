/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/platform/meta/quest/openxr/solarsystem/quest/SolarSystem.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/platform/android/ResourceManager.h"

SolarSystem::SolarSystem(struct android_app* androidApp) :
	VRNativeApplicationAdvanced(androidApp)
{
	// nothing to do here
}

XrSpace SolarSystem::baseSpace() const
{
	// we want to render something always in relation to the user's local coordinate system

	return xrSpaceLocal_.object();
}

void SolarSystem::onFramebufferInitialized()
{
	VRNativeApplicationAdvanced::onFramebufferInitialized();

	// Copy the assets to disk
	ocean_assert(Platform::Android::ResourceManager::get().isValid());
	const IO::Directory temporaryTargetDirectory = IO::Directory(Platform::Android::ResourceManager::get().externalFilesDirectory()) + IO::Directory("scene");

	if (!Platform::Android::ResourceManager::get().copyAssets(temporaryTargetDirectory(), /* createDirectory */ true, "scene"))
	{
		Log::error() << "Failed to copy the application assets to disk";
		return;
	}

	if (!temporaryTargetDirectory.exists())
	{
		Log::error() << "The directory containing the application assets does not exist";
		return;
	}

	const IO::File file = temporaryTargetDirectory + IO::File("solarsystem.x3dv");

	if (!file.exists())
	{
		Log::error() << "The file '" << file.base() << "' does not exist, full path '" << file() << "'";
		return;
	}

	const HomogenousMatrix4 world_T_model(Vector3(0, 0, Scalar(-2.5))); // moving the model 2.5 meters in front of the user

	loadModel(file(), world_T_model);
}

void SolarSystem::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	VRNativeApplicationAdvanced::onPreRender(xrPredictedDisplayTime, predictedDisplayTime);
}
