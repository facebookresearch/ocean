// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/vrapi/application/VRNativeApplicationAdvanced.h"

#include "ocean/devices/quest/Quest.h"
#include "ocean/devices/quest/vrapi/VrApi.h"
#include "ocean/devices/quest/vrapi/VrApiFactory.h"

#include "ocean/io/Directory.h"

#include "ocean/media/android/Android.h"

#include "ocean/media/openimagelibraries/OpenImageLibraries.h"

#include "ocean/platform/android/Resource.h"

#include "ocean/scenedescription/Manager.h"
#include "ocean/scenedescription/sdl/assimp/Assimp.h"
#include "ocean/scenedescription/sdl/obj/OBJ.h"
#include "ocean/scenedescription/sdx/x3d/X3D.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

namespace Application
{

VRNativeApplicationAdvanced::VRNativeApplicationAdvanced(struct android_app* androidApp) :
	VRNativeApplication(androidApp)
{
	Media::Android::registerAndroidLibrary();
	Media::OpenImageLibraries::registerOpenImageLibrariesLibrary();

	Devices::Quest::registerQuestLibrary();
	Devices::Quest::VrApi::registerQuestVrApiLibrary();

	SceneDescription::SDX::X3D::registerX3DLibrary();
	SceneDescription::SDL::OBJ::registerOBJLibrary();
	SceneDescription::SDL::Assimp::registerAssimpLibrary();
}

VRNativeApplicationAdvanced::~VRNativeApplicationAdvanced()
{
	// nothing to do here
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

void VRNativeApplicationAdvanced::onReleaseResources()
{
	Log::debug() << "VRNativeApplicationAdvanced::onReleaseResources()";

	SceneDescription::SDL::Assimp::unregisterAssimpLibrary();
	SceneDescription::SDL::OBJ::unregisterOBJLibrary();
	SceneDescription::SDX::X3D::unregisterX3DLibrary();

	Devices::Quest::VrApi::unregisterQuestVrApiLibrary();
	Devices::Quest::unregisterQuestLibrary();

	Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
	Media::Android::unregisterAndroidLibrary();

	VRNativeApplication::onReleaseResources();
}

void VRNativeApplicationAdvanced::onFramebufferReleasing()
{
	Log::debug() << "VRNativeApplicationAdvanced::onFramebufferReleasing()";

	vrControllerVisualizer_ = VRControllerVisualizer();
	vrHandVisualizer_ = VRHandVisualizer();

	VRNativeApplication::onFramebufferReleasing();
}

void VRNativeApplicationAdvanced::onVrModeEntered()
{
	Log::debug() << "VRNativeApplicationAdvanced::onVrModeEntered()";

	VRNativeApplication::onVrModeEntered();

	vrHandVisualizer_ = VRHandVisualizer(engine_, framebuffer_, ovrMobile_);

	// Load the render models from disk and initialize the visualizers
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

	handPoses_ = HandPoses(ovrMobile_);
}

void VRNativeApplicationAdvanced::onVrModeLeft()
{
	Log::debug() << "VRNativeApplicationAdvanced::onVrModeLeft()";

	handPoses_ = HandPoses();

	vrHandVisualizer_ = VRHandVisualizer();

	VRNativeApplication::onVrModeLeft();
}

void VRNativeApplicationAdvanced::onPreRender(const Timestamp& renderTimestamp)
{
	VRNativeApplication::onPreRender(renderTimestamp);

	vrControllerVisualizer_.visualizeControllersInWorld(trackedRemoteDevice(), renderTimestamp);

	handPoses_.update(double(renderTimestamp));
	vrHandVisualizer_.visualizeHandsInWorld(handPoses_);

	Devices::Quest::VrApi::VrApiFactory::update(ovrMobile_, deviceType(), trackedRemoteDevice(), renderTimestamp);

	SceneDescription::Manager::get().preUpdate(framebuffer_->view(), renderTimestamp);
	SceneDescription::Manager::get().update(framebuffer_->view(), renderTimestamp);

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

		if (queueIsEmpty)
		{
			break;
		}
	}

	{
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

				const SceneDescription::SceneRef scene = SceneDescription::Manager::get().load(modelFilename, engine_, renderTimestamp, SceneDescription::TYPE_PERMANENT);

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
