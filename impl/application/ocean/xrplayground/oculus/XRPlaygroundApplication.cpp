// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/oculus/XRPlaygroundApplication.h"

#include "application/ocean/xrplayground/common/ContentManager.h"
#include "application/ocean/xrplayground/common/PlatformSpecific.h"
#include "application/ocean/xrplayground/common/PrototypeDevices.h"

#include "application/ocean/xrplayground/common/experiences/ExperiencesManager.h"

#include "ocean/devices/mapbuilding/MapBuilding.h"
#include "ocean/devices/quest/Quest.h"
#include "ocean/devices/quest/vrapi/VrApi.h"
#include "ocean/devices/quest/vrapi/VrApiFactory.h"
#include "ocean/devices/pattern/Pattern.h"

#include "ocean/interaction/Manager.h"
#include "ocean/interaction/javascript/JavaScript.h"

#include "ocean/io/Directory.h"
#include "ocean/io/FileResolver.h"

#include "ocean/media/android/Android.h"
#include "ocean/media/openimagelibraries/OpenImageLibraries.h"
#include "ocean/media/quest/Quest.h"

#include "ocean/network/tigon/TigonClient.h"

#include "metaonly/ocean/network/verts/Manager.h"

#include "ocean/platform/android/Resource.h"
#include "ocean/platform/android/Utilities.h"

#include "ocean/platform/meta/avatars/quest/InputQuest.h"

#include "ocean/platform/meta/quest/vrapi/HeadsetPoses.h"

#include "ocean/platform/meta/quest/platformsdk/Application.h"
#include "ocean/platform/meta/quest/platformsdk/Manager.h"
#include "ocean/platform/meta/quest/platformsdk/Room.h"

#include "ocean/rendering/Utilities.h"

#include "ocean/scenedescription/Manager.h"
#include "ocean/scenedescription/sdl/assimp/Assimp.h"
#include "ocean/scenedescription/sdl/obj/OBJ.h"
#include "ocean/scenedescription/sdx/x3d/X3D.h"

namespace Ocean
{

namespace XRPlayground
{

namespace Oculus
{

using namespace Platform::Meta;
using namespace Platform::Meta::Quest;
using namespace Platform::Meta::Quest::VrApi;
using namespace Platform::Meta::Quest::VrApi::Application;

bool XRPlaygroundApplication::LaunchExperienceHolder::setExperience(std::string&& experience)
{
	const ScopedLock scopedLock(lock_);

	if (experience.empty())
	{
		return false;
	}

	Log::info() << "Setting launch experience '" << experience << "'";

	experience_ = std::move(experience);

	return true;
}

std::string XRPlaygroundApplication::LaunchExperienceHolder::experience() const
{
	const ScopedLock scopedLock(lock_);

	return experience_;
}

XRPlaygroundApplication::XRPlaygroundApplication(struct android_app* androidApp) :
	VRNativeApplication(androidApp)
{
	Media::Android::registerAndroidLibrary();
	Media::OpenImageLibraries::registerOpenImageLibrariesLibrary();
	Media::Quest::registerQuestLibrary();

	Devices::MapBuilding::registerMapBuildingLibrary();
	Devices::Quest::registerQuestLibrary();
	Devices::Quest::VrApi::registerQuestVrApiLibrary();
	Devices::Pattern::registerPatternLibrary();

	SceneDescription::SDX::X3D::registerX3DLibrary();
	SceneDescription::SDL::OBJ::registerOBJLibrary();
	SceneDescription::SDL::Assimp::registerAssimpLibrary();

	Interaction::JavaScript::registerJavaScriptLibrary();

	ExperiencesManager::registerAllCodeBasedExperiences();

	PrototypeDevices::registerPrototypeDevices();
}

XRPlaygroundApplication::~XRPlaygroundApplication()
{
	// nothing to do here
}

bool XRPlaygroundApplication::setLaunchExperience(std::string&& experience)
{
	return LaunchExperienceHolder::get().setExperience(std::move(experience));
}

XRPlaygroundApplication::AndroidPermissionsSet XRPlaygroundApplication::androidPermissionsToRequest()
{
	AndroidPermissionsSet permissions = VRNativeApplication::androidPermissionsToRequest();

	permissions.emplace("com.oculus.permission.ACCESS_MR_SENSOR_DATA");
	permissions.emplace("android.permission.RECORD_AUDIO");
	permissions.emplace("android.permission.ACCESS_FINE_LOCATION");
	permissions.emplace("android.permission.INTERNET");
	permissions.emplace("android.permission.ACCESS_NETWORK_STATE");

	return permissions;
}

void XRPlaygroundApplication::onReadyToUse(const bool validInitialization)
{
	std::string facebookAccountUserName = "unknown";
	std::string errorMessage;
	if (!Network::Tigon::TigonClient::get().determineUserName(facebookAccountUserName, Network::Tigon::TigonClient::defaultGraphQLURL, &errorMessage))
	{
		Log::error() << "Failed to determine user name of facebook account, error: " << errorMessage;
	}

	// Display the current version code of this app
	int manifestVersionCode = -1;
	if (!Platform::Android::Utilities::manifestVersionCode(androidApp_->activity->vm, androidApp_->activity->clazz, manifestVersionCode))
	{
		Log::error() << "Failed to determine version code";
	}

	ocean_assert(manifestVersionCode >= 0);
	std::string versionString = " Version: " + (manifestVersionCode >= 0 ? String::toAString(manifestVersionCode) : " unknown") + " ";
	OCEAN_APPLY_IF_DEBUG(versionString += "\n (debug) ");
	versionString += "\n Facebook account: " + facebookAccountUserName + " ";

	if (!validInitialization)
	{
		versionString += "\n\n Parts of the initialization failed ";

		if (!hasAssetsAccess_)
		{
			versionString += "\n No access to assets ";
		}

		if (!avatarsInitialized_)
		{
			versionString += "\n No access to avatars ";
		}
	}

	const Rendering::TransformRef text = Rendering::Utilities::createText(*engine_, versionString, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.25), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
	text->setTransformation(HomogenousMatrix4(Vector3(0, 3, -9.95)));
	scene_->addChild(text);

	const std::string launchExperience = LaunchExperienceHolder::get().experience();

	if (launchExperience.empty())
	{
		ContentManager::get().loadContent("instructions_oculus.ox3dv", ContentManager::LM_LOAD_REPLACE);
	}
	else
	{
		ContentManager::get().loadContent(launchExperience, ContentManager::LM_LOAD_REPLACE);
		scene_->clear(); // removing version text
	}
}

void XRPlaygroundApplication::onFramebufferInitialized()
{
	VRNativeApplication::onFramebufferInitialized();

	experiencesMenu_ = ExperiencesMenu(engine_, framebuffer_);

	scene_ = engine_->factory().createScene();
	framebuffer_->addScene(scene_);

	ocean_assert(engine_ && framebuffer_);

	ocean_assert(Platform::Android::ResourceManager::get().isValid());
	const IO::Directory externalDirectory(Platform::Android::ResourceManager::get().externalFilesDirectory());

	// controller assets
	const IO::Directory temporaryControllerTargetDirectory = externalDirectory + IO::Directory("meta_quest_application/controller");
	if (Platform::Android::ResourceManager::get().copyAssets(temporaryControllerTargetDirectory(), /* createDirectory */ true, "ocean_meta_quest_application/controller"))
	{
		vrControllerVisualizer_ = VRControllerVisualizer(engine_, framebuffer_, deviceType(), temporaryControllerTargetDirectory());

		PlatformSpecific::get().setVRControllerVisualizer(vrControllerVisualizer_);
	}

	if (!vrControllerVisualizer_.isValid())
	{
		Log::error() << "Failed to load controller models from directory '" << temporaryControllerTargetDirectory() << "'";
	}

	// experience assets
	const IO::Directory temporaryAssetTargetDirectory = externalDirectory + IO::Directory("xrplayground/assets");
	if (Platform::Android::ResourceManager::get().copyAssets(temporaryAssetTargetDirectory(), /* createDirectory */ true))
	{
		if (temporaryAssetTargetDirectory.exists())
		{
			hasAssetsAccess_ = true;

			IO::FileResolver::get().addReferencePath(temporaryAssetTargetDirectory);
		}
		else
		{
			Log::error() << "Failed to create the asset directory";
		}
	}
	else
	{
		Log::error() << "Failed to copy scene asset files";
	}
}

void XRPlaygroundApplication::onFramebufferReleasing()
{
	Log::debug() << "XRPlaygroundApplication::onFramebufferReleasing()";

	mrPassthroughVisualizer_ = MRPassthroughVisualizer();

	{
		ContentManager::get().unloadContent();

		const Interaction::UserInterface userInterface;
		const Timestamp anyTimestamp(true);

		ContentManager::get().processContent(userInterface, engine_, *framebuffer_, anyTimestamp);
	}

	Avatars::Manager::get().release();

	vrControllerVisualizer_ = VRControllerVisualizer();
	vrHandVisualizer_ = VRHandVisualizer();
	experiencesMenu_.release();

	framebuffer_->removeScene(scene_);
	scene_.release();

	VRNativeApplication::onFramebufferReleasing();
}

void XRPlaygroundApplication::onVrModeEntered()
{
	VRNativeApplication::onVrModeEntered();

	PlatformSpecific::get().setOVR(ovrMobile_);

	vrHandVisualizer_ = VRHandVisualizer(engine_, framebuffer_, ovrMobile_);
	PlatformSpecific::get().setVRHandVisualizer(vrHandVisualizer_);

	handPoses_ = HandPoses(ovrMobile_);

	PlatformSpecific::get().setHandPoses(handPoses_);

	PlatformSpecific::get().setTrackedRemoteDevice(trackedRemoteDevice());

	if (!mrPassthroughVisualizer_.isValid())
	{
		mrPassthroughVisualizer_ = MRPassthroughVisualizer(engine_, framebuffer_, ovrMobile_, &java());

		mrPassthroughVisualizer_.enterVrMode();
		mrPassthroughVisualizer_.pausePassthrough();
	}
	else
	{
		mrPassthroughVisualizer_.enterVrMode();
	}

	PlatformSpecific::get().setMRPassthroughVisualizer(mrPassthroughVisualizer_);
}

void XRPlaygroundApplication::onVrModeLeft()
{
	mrPassthroughVisualizer_.leaveVrMode();

	handPoses_ = HandPoses();

	vrHandVisualizer_ = VRHandVisualizer();

	VRNativeApplication::onVrModeLeft();
}

void XRPlaygroundApplication::onPreRender(const Timestamp& renderTimestamp)
{
	ocean_assert(renderTimestamp.isValid());

	if (initializationTimestamp_.isInvalid())
	{
		initializationTimestamp_ = renderTimestamp;
	}

	Network::Verts::Manager::get().update(renderTimestamp);

	vrControllerVisualizer_.visualizeControllersInWorld(trackedRemoteDevice(), renderTimestamp);

	handPoses_.update(double(renderTimestamp));
	vrHandVisualizer_.visualizeHandsInWorld(handPoses_);

	Devices::Quest::VrApi::VrApiFactory::update(ovrMobile_, deviceType(), trackedRemoteDevice(), renderTimestamp);

	PlatformSDK::Manager::get().update(double(renderTimestamp));

	if (userId_ == uint64_t(0) || accessToken_.empty())
	{
		PlatformSDK::Room::get().ensureInitialization();

		userId_ = PlatformSDK::Manager::get().userId(&accessToken_);

		if (userId_ != uint64_t(0) && !accessToken_.empty())
		{
			Network::Verts::Manager::get().initialize("XRPlayground", accessToken_);

			const ovrAvatar2Platform ovrPlatform = Avatars::Quest::InputQuest::translateDeviceType(deviceType());

			if (Avatars::Manager::get().initialize(ovrPlatform, Avatars::Avatar::UT_OCULUS, userId_, accessToken_))
			{
				Log::info() << "Avatar2 initialized for local user " << userId_;

				for (const Platform::Meta::Login::LoginType loginType : Platform::Meta::Login::get().loginTypes())
				{
					if (loginType != Platform::Meta::Login::LT_OCULUS)
					{
						const std::string userToken = Platform::Meta::Login::get().userToken(loginType);

						if (!userToken.empty())
						{
							Avatars::Manager::get().setAccessToken(translateLoginType(loginType), userToken);
						}
					}
				}

				// let's check whether the app has been launched with a given deep link, we use the link to start a specific experience

				std::string deepLink;
				if (PlatformSDK::Application::get().hasLaunchDeepLink(deepLink))
				{
					const std::string deepLinkPrefix = "XRPlayground://";

					if (deepLink.find(deepLinkPrefix) == 0 && deepLink.size() > deepLinkPrefix.size())
					{
						LaunchExperienceHolder::get().setExperience(deepLink.substr(deepLinkPrefix.size()));
					}
					else
					{
						Log::warning() << "Invalid deep link '" << deepLink << "'";
					}
				}

				avatarsInitialized_ = true;
			}
		}
	}

	if (!readyToUse_)
	{
		if (hasAssetsAccess_ && avatarsInitialized_)
		{
			onReadyToUse(true);
			readyToUse_ = true;
		}
		else
		{
			if (renderTimestamp > initializationTimestamp_ + 6.0)
			{
				Log::error() << "Parts of the initialization failed:";

				if (!hasAssetsAccess_)
				{
					Log::error() << "Failed to access the assets";
				}

				if (!avatarsInitialized_)
				{
					Log::error() << "Failed to initialize avatars";
				}

				onReadyToUse(false);
				readyToUse_ = true;
			}
		}
	}
	else
	{
		// let's check whether a new/changed deep link has arrived to (re-)launch a new experience

		std::string changedDeepLink;
		if (PlatformSDK::Application::get().hasChangedLaunchDeepLink(changedDeepLink))
		{
			const std::string deepLinkPrefix = "XRPlayground://";

			if (changedDeepLink.find(deepLinkPrefix) == 0 && changedDeepLink.size() > deepLinkPrefix.size())
			{
				ContentManager::get().loadContent(changedDeepLink.substr(deepLinkPrefix.size()), ContentManager::LM_LOAD_REPLACE);
				scene_->clear(); // removing version text
			}
			else
			{
				Log::warning() << "Invalid deep link '" << changedDeepLink << "'";
			}
		}
	}

	Avatars::Manager::get().updateLocalBodyTrackingData(Avatars::Quest::InputQuest::createBodyTrackingData(ovrMobile_, deviceType(), trackedRemoteDevice(), renderTimestamp));

	if (trackedRemoteDevice().buttonsState() & ovrButton_B)
	{
		if (!experiencesMenu_.isShown() && timestampPressedButtonB_.isValid() && renderTimestamp > timestampPressedButtonB_ + 1.5) // long press
		{
			experiencesMenu_.show(HomogenousMatrix4(Vector3(0, -0.2, -1)), HeadsetPoses::world_T_device(ovrMobile_, renderTimestamp));
		}
	}

	if (trackedRemoteDevice().buttonsState() & ovrButton_Y)
	{
		if (timestampPressedButtonY_.isValid() && renderTimestamp > timestampPressedButtonY_ + 1.5) // long press
		{
			if (mrPassthroughVisualizer_.isPassthroughRunning())
			{
				mrPassthroughVisualizer_.pausePassthrough();
			}
			else
			{
				mrPassthroughVisualizer_.resumePassthrough();
			}

			timestampPressedButtonY_.toInvalid();
		}
	}

	if (experiencesMenu_.isShown())
	{
		vrControllerVisualizer_.setControllerRayLength(Scalar(0.2));

		std::string experienceUrl;
		if (experiencesMenu_.onPreRender(trackedRemoteDevice(), renderTimestamp, experienceUrl))
		{
			experiencesMenu_.hide();
			vrControllerVisualizer_.setControllerRayLength(0);

			if (!experienceUrl.empty())
			{
				ocean_assert(Platform::Android::ResourceManager::get().isValid());
				const IO::File file(Platform::Android::ResourceManager::get().externalFilesDirectory() + "/xrplayground/assets/" + experienceUrl);

				if (file.exists())
				{
					experienceUrl = file();
				}
			}

			ContentManager::get().loadContent(experienceUrl, ContentManager::LM_LOAD_REPLACE); // if filename is empty, unload all scenes

			scene_->clear(); // removing version text
		}
	}
	else
	{
		const Interaction::UserInterface userInterface;

		ContentManager::get().processContent(userInterface, engine_, *framebuffer_, renderTimestamp);

		const Rendering::PerspectiveViewRef perspectiveView = framebuffer_->view();

		if (perspectiveView.isNull())
		{
			return;
		}

		Timestamp updateTimestamp = SceneDescription::Manager::get().preUpdate(perspectiveView, renderTimestamp);

		updateTimestamp = Interaction::Manager::get().preUpdate(userInterface, engine_, perspectiveView, updateTimestamp);

		SceneDescription::Manager::get().update(perspectiveView, updateTimestamp);

		VRNativeApplication::onPreRender(renderTimestamp);

		Interaction::Manager::get().postUpdate(userInterface, engine_, framebuffer_->view(), renderTimestamp);
	}

	Avatars::Manager::get().update(engine_, double(renderTimestamp));

	if (mrPassthroughVisualizer_.isPassthroughRunning())
	{
		mrPassthroughVisualizer_.update();

		CustomLayer customLayer = mrPassthroughVisualizer_.render(renderTimestamp);

		if (customLayer.isValid())
		{
			addCustomLayer(std::move(customLayer), /* postLayer */ false);
		}
	}
}

void XRPlaygroundApplication::onButtonPressed(const uint32_t buttons, const uint32_t buttonsLeft, const uint32_t buttonsRight, const Timestamp& timestamp)
{
	ocean_assert(buttons != 0u);

	if (!experiencesMenu_.isShown())
	{
		std::vector<std::string> readableButtons;
		readableButtons.reserve(4);

		TrackedRemoteDevice::translateButtons(buttonsLeft, buttonsRight, readableButtons);

		const Interaction::UserInterface userInterface;

		for (const std::string& readableButton : readableButtons)
		{
			Interaction::Manager::get().onKeyPress(userInterface, engine_, readableButton, timestamp);
		}
	}

	if (buttons & ovrButton_B)
	{
		if (experiencesMenu_.isShown())
		{
			experiencesMenu_.hide();
			vrControllerVisualizer_.setControllerRayLength(0);
		}

		timestampPressedButtonB_ = timestamp;
	}

	if (buttons & ovrButton_Y)
	{
		timestampPressedButtonY_ = timestamp;
	}
}

void XRPlaygroundApplication::onButtonReleased(const uint32_t buttons, const uint32_t buttonsLeft, const uint32_t buttonsRight, const Timestamp& timestamp)
{
	if (!experiencesMenu_.isShown())
	{
		std::vector<std::string> readableButtons;
		readableButtons.reserve(4);

		TrackedRemoteDevice::translateButtons(buttonsLeft, buttonsRight, readableButtons);

		const Interaction::UserInterface userInterface;

		for (const std::string& readableButton : readableButtons)
		{
			Interaction::Manager::get().onKeyRelease(userInterface, engine_, readableButton, timestamp);
		}
	}

	if (buttons & ovrButton_B)
	{
		timestampPressedButtonB_.toInvalid();
	}

	if (buttons & ovrButton_Y)
	{
		timestampPressedButtonY_.toInvalid();
	}
}

void XRPlaygroundApplication::onActivityStart()
{
	VRNativeApplication::onActivityStart();

	PlatformSDK::Manager::get().initialize(androidApp_->activity->clazz, ovrJava_.Env, "4591725520915836");
}

void XRPlaygroundApplication::onReleaseResources()
{
	Log::debug() << "XRPlaygroundApplication::onReleaseResources()";

	Interaction::JavaScript::unregisterJavaScriptLibrary();

	SceneDescription::SDL::Assimp::unregisterAssimpLibrary();
	SceneDescription::SDL::OBJ::unregisterOBJLibrary();
	SceneDescription::SDX::X3D::unregisterX3DLibrary();

	Devices::Pattern::unregisterPatternLibrary();
	Devices::Quest::VrApi::unregisterQuestVrApiLibrary();
	Devices::Quest::unregisterQuestLibrary();
	Devices::MapBuilding::unregisterMapBuildingLibrary();

	Media::Quest::unregisterQuestLibrary();
	Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
	Media::Android::unregisterAndroidLibrary();

	VRNativeApplication::onReleaseResources();

	ContentManager::get().release();

	// finally ensuring that any left over interaction or scene is released

	Interaction::Manager::get().release();
	SceneDescription::Manager::get().release();
}

Platform::Meta::Avatars::Manager::UserType XRPlaygroundApplication::translateLoginType(const Platform::Meta::Login::LoginType loginType)
{
	switch (loginType)
	{
		case Platform::Meta::Login::LT_UNKNOWN:
			return Platform::Meta::Avatars::Avatar::UT_UNKNOWN;

		case Platform::Meta::Login::LT_META:
			return Platform::Meta::Avatars::Avatar::UT_META;

		case Platform::Meta::Login::LT_FACEBOOK:
			return Platform::Meta::Avatars::Avatar::UT_FACEBOOK;

		case Platform::Meta::Login::LT_INSTAGRAM:
			return Platform::Meta::Avatars::Avatar::UT_INSTAGRAM;

		case Platform::Meta::Login::LT_OCULUS:
			return Platform::Meta::Avatars::Avatar::UT_OCULUS;
	}

	ocean_assert(false && "Invalid login type!");
	return Platform::Meta::Avatars::Avatar::UT_UNKNOWN;
}

}

}

}
