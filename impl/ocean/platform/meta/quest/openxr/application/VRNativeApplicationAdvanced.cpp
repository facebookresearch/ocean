/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/openxr/application/VRNativeApplicationAdvanced.h"

#include "ocean/io/Directory.h"

#include "ocean/media/android/Android.h"

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"

#include "ocean/platform/android/ResourceManager.h"

#include "ocean/scenedescription/Manager.h"
#include "ocean/scenedescription/sdl/assimp/Assimp.h"
#include "ocean/scenedescription/sdl/obj/OBJ.h"
#include "ocean/scenedescription/sdx/x3d/X3D.h"

namespace Ocean
{

namespace Platform
{

using namespace OpenXR;

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

namespace Application
{

VRNativeApplicationAdvanced::VRNativeApplicationAdvanced(struct android_app* androidApp) :
	VRNativeApplication(androidApp)
{
	Media::Android::registerAndroidLibrary();
	Media::OpenImageLibraries::registerOpenImageLibrariesLibrary();

	SceneDescription::SDX::X3D::registerX3DLibrary();
	SceneDescription::SDL::OBJ::registerOBJLibrary();
	SceneDescription::SDL::Assimp::registerAssimpLibrary();

#ifdef OCEAN_PLATFORM_QUEST_OPENXR_APPLICATION_USE_EXTERNAL_RESOURCES
	VRNativeApplicationAdvanced_registerExternalResources();
#endif

}

VRNativeApplicationAdvanced::~VRNativeApplicationAdvanced()
{
	// nothing to do here
}

VRNativeApplicationAdvanced::StringSet VRNativeApplicationAdvanced::necessaryOpenXRExtensionNames() const
{
	StringSet extensionNames = VRNativeApplication::necessaryOpenXRExtensionNames();

	extensionNames.insert(HandPoses::necessaryOpenXRExtensionNames().cbegin(), HandPoses::necessaryOpenXRExtensionNames().cend());
	extensionNames.insert(Passthrough::necessaryOpenXRExtensionNames().cbegin(), Passthrough::necessaryOpenXRExtensionNames().cend());

	return extensionNames;
}

void VRNativeApplicationAdvanced::loadModel(std::string modelFilename, const HomogenousMatrix4& world_T_model)
{
	ocean_assert(!modelFilename.empty() && world_T_model.isValid());

	const ScopedLock scopedLock(modelFilenameQueueLock_);

	ocean_assert(sceneFilenameMap_.find(modelFilename) == sceneFilenameMap_.cend());

	modelFilenameLoadQueue_.emplace(std::move(modelFilename), world_T_model);
}

void VRNativeApplicationAdvanced::removeModel(std::string modelFilename)
{
	ocean_assert(!modelFilename.empty());

	const ScopedLock scopedLock(modelFilenameQueueLock_);

	modelFilenameRemoveQueue_.emplace(std::move(modelFilename));
}

void VRNativeApplicationAdvanced::onOpenXRSessionReady()
{
	VRNativeApplication::onOpenXRSessionReady();

	if (!handPoses_.isValid() && !handPoses_.initialize(xrSession_))
	{
		Log::error() << "OpenXR VRNativeApplicationAdvanced: Failed to initialize hand poses";
	}

	bool allPassthroughExtensionsEnabled = true;

	for (const std::string& extensionName : Passthrough::necessaryOpenXRExtensionNames())
	{
		if (xrInstance_.enabledExtensions().find(extensionName) == xrInstance_.enabledExtensions().cend())
		{
			allPassthroughExtensionsEnabled = false;
			break;
		}
	}

	if (allPassthroughExtensionsEnabled)
	{
		if (passthrough_.initialize(xrSession_))
		{
			Log::debug() << "OpenXR VRNativeApplicationAdvanced: Passthrough initialized";
		}
		else
		{
			Log::error() << "OpenXR VRNativeApplicationAdvanced: Failed to initialize passthrough";
		}
	}
}

void VRNativeApplicationAdvanced::onOpenXRSessionStopping()
{
	passthrough_.release();

	VRNativeApplication::onOpenXRSessionStopping();
}

void VRNativeApplicationAdvanced::onReleaseResources()
{
	Log::debug() << "VRNativeApplicationAdvanced::onReleaseResources()";

#ifdef OCEAN_PLATFORM_QUEST_OPENXR_APPLICATION_USE_EXTERNAL_RESOURCES
	VRNativeApplicationAdvanced_unregisterExternalResources();
#endif

	SceneDescription::SDL::Assimp::unregisterAssimpLibrary();
	SceneDescription::SDL::OBJ::unregisterOBJLibrary();
	SceneDescription::SDX::X3D::unregisterX3DLibrary();

	Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
	Media::Android::unregisterAndroidLibrary();

	VRNativeApplication::onReleaseResources();
}

void VRNativeApplicationAdvanced::onFramebufferInitialized()
{
	Log::debug() << "VRNativeApplicationAdvanced::onFramebufferInitialized()";

	VRNativeApplication::onFramebufferInitialized();

	vrHandVisualizer_ = VRHandVisualizer(engine_, framebuffer_);

	// Load the controller models from disk and initialize the visualizers
	ocean_assert(Platform::Android::ResourceManager::get().isValid());
	const IO::Directory temporaryTargetDirectory = IO::Directory(Platform::Android::ResourceManager::get().externalFilesDirectory()) + IO::Directory("meta_quest_application/controller");

	if (Platform::Android::ResourceManager::get().copyAssets(temporaryTargetDirectory(), /* createDirectory */ true, "ocean_meta_quest_application/controller"))
	{
		if (temporaryTargetDirectory.exists())
		{
			vrControllerVisualizer_ = VRControllerVisualizer(engine_, framebuffer_, deviceType(), temporaryTargetDirectory());
		}
	}

	if (!vrControllerVisualizer_.isValid())
	{
		Log::error() << "Failed to load controller models from directory '" << temporaryTargetDirectory() << "'";
	}
}

void VRNativeApplicationAdvanced::onFramebufferReleasing()
{
	Log::debug() << "VRNativeApplicationAdvanced::onFramebufferReleasing()";

	TemporaryScopedLock scopedLock(modelFilenameQueueLock_);
		// let's remove all loaded 3D models

		modelFilenameLoadQueue_ = ModelFilenamePairQueue();

		handleModelRemoveQueue();

		for (const SceneFilenameMap::value_type& scenePair : sceneFilenameMap_)
		{
			invokeRemoveModel(scenePair.second, scenePair.first);
		}

		sceneFilenameMap_.clear();
	scopedLock.release();

	vrControllerVisualizer_.release();
	vrHandVisualizer_.release();

	VRNativeApplication::onFramebufferReleasing();
}

void VRNativeApplicationAdvanced::onAddCompositorBackLayers(XrCompositorLayerUnions& xrCompositorLayerUnions)
{
	if (passthrough_.isValid() && passthrough_.isStarted())
	{
		xrCompositorLayerUnions.emplace_back();

		XrCompositionLayerPassthroughFB& xrCompositionLayerPassthroughFB = xrCompositorLayerUnions.back().xrCompositionLayerPassthroughFB_;

		xrCompositionLayerPassthroughFB = {XR_TYPE_COMPOSITION_LAYER_PASSTHROUGH_FB};
		xrCompositionLayerPassthroughFB.layerHandle = passthrough_.xrPassthroughLayerFB();
		xrCompositionLayerPassthroughFB.flags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
	}
}

void VRNativeApplicationAdvanced::handleModelRemoveQueue()
{
	while (true)
	{
		TemporaryScopedLock temporaryScopedLock(modelFilenameQueueLock_);

		if (modelFilenameRemoveQueue_.empty())
		{
			break;
		}

		SceneFilenameMap::iterator iPair = sceneFilenameMap_.find(modelFilenameRemoveQueue_.front());
		modelFilenameRemoveQueue_.pop();

		const bool queueIsEmpty = modelFilenameRemoveQueue_.empty();

		if (iPair != sceneFilenameMap_.end())
		{
			const std::string filename = std::move(iPair->first);
			const SceneIdPair sceneIdPair = iPair->second;

			sceneFilenameMap_.erase(iPair);

			temporaryScopedLock.release();

			invokeRemoveModel(sceneIdPair, filename);
		}

		if (queueIsEmpty)
		{
			break;
		}
	}
}

void VRNativeApplicationAdvanced::invokeRemoveModel(const SceneIdPair& sceneIdPair, const std::string& filename)
{
	ocean_assert(!filename.empty());

	SceneDescription::Manager::get().unload(sceneIdPair.first);

	if (engine_ && framebuffer_)
	{
		const Rendering::SceneRef scene(engine_->object(sceneIdPair.second));

		if (scene)
		{
			framebuffer_->removeScene(scene);
		}
	}

	onModelRemoved(filename);
}

void VRNativeApplicationAdvanced::handleModelLoadQueue(const Timestamp& predictedDisplayTime)
{
	ocean_assert(predictedDisplayTime.isValid());

	TemporaryScopedLock temporaryScopedLock(modelFilenameQueueLock_);

	if (!modelFilenameLoadQueue_.empty())
	{
		std::string modelFilename = std::move(modelFilenameLoadQueue_.front().first);
		const HomogenousMatrix4 world_T_model(modelFilenameLoadQueue_.front().second);

		modelFilenameLoadQueue_.pop();

		temporaryScopedLock.release();

		Rendering::SceneRef renderingScene;

		try
		{
			// first, we try to load a permanent scene (with support for animations etc.)

			const SceneDescription::SceneRef scene = SceneDescription::Manager::get().load(modelFilename, engine_, predictedDisplayTime, SceneDescription::TYPE_PERMANENT);

			if (scene)
			{
				if (scene->descriptionType() == SceneDescription::TYPE_TRANSIENT)
				{
					const SceneDescription::SDLSceneRef sdlScene(scene);
					ocean_assert(sdlScene);

					renderingScene = sdlScene->apply(engine_);
				}
				else
				{
					ocean_assert(scene->descriptionType() == SceneDescription::TYPE_PERMANENT);

					const SceneDescription::SDXSceneRef sdxScene(scene);
					ocean_assert(sdxScene);

					renderingScene = sdxScene->renderingScene();
				}

				if (renderingScene)
				{
					renderingScene->setTransformation(world_T_model);
					framebuffer_->addScene(renderingScene);

					const ScopedLock scopedLock(modelFilenameQueueLock_);

					ocean_assert(sceneFilenameMap_.find(modelFilename) == sceneFilenameMap_.cend());
					sceneFilenameMap_.emplace(modelFilename, SceneIdPair(scene->id(), renderingScene->id()));
				}
			}
		}
		catch (const std::exception& exceptionObject)
		{
			Log::error() << "Failed to load scene '" << modelFilename << "', reason: " << exceptionObject.what();
		}
		catch (...)
		{
			Log::error() << "Failed to load scene '" << modelFilename << "', unknown reason";
		}

		onModelLoaded(modelFilename, renderingScene);
	}
}

void VRNativeApplicationAdvanced::onPreRender(const XrTime& xrPredictedDisplayTime, const Timestamp& predictedDisplayTime)
{
	VRNativeApplication::onPreRender(xrPredictedDisplayTime, predictedDisplayTime);

	if (vrControllerVisualizer_.isValid())
	{
		vrControllerVisualizer_.visualizeControllersInWorld(trackedController());
	}

	if (!handPoses_.update(baseSpace(), xrPredictedDisplayTime))
	{
		Log::error() << "OpenXR VRNativeApplicationAdvanced: Failed to update hand poses";
	}

	if (vrHandVisualizer_.isValid())
	{
		if (!vrHandVisualizer_.visualizeHands(handPoses_))
		{
			Log::error() << "OpenXR VRNativeApplicationAdvanced: Failed to visualize hands";
		}
	}

	SceneDescription::Manager::get().preUpdate(framebuffer_->view(), predictedDisplayTime);
	SceneDescription::Manager::get().update(framebuffer_->view(), predictedDisplayTime);

	handleModelRemoveQueue();

	handleModelLoadQueue(predictedDisplayTime);
}

void VRNativeApplicationAdvanced::onModelLoaded(const std::string& /*modelFilename*/, const Rendering::SceneRef& /*scene*/)
{
	// can be implemented in derived classes
}

void VRNativeApplicationAdvanced::onModelRemoved(const std::string& /*modelFilename*/)
{
	// can be implemented in derived classes
}

}

}

}

}

}

}
