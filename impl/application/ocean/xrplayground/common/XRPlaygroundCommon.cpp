// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/XRPlaygroundCommon.h"
#include "application/ocean/xrplayground/common/ContentManager.h"

#include "application/ocean/xrplayground/common/experiences/ExperiencesManager.h"

#include "ocean/base/RandomI.h"

#include "ocean/cv/fonts/FontManager.h"

#include "ocean/devices/GPSTracker.h"
#include "ocean/devices/Manager.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/interaction/Manager.h"

#include "ocean/io/File.h"

#include "ocean/network/verts/Manager.h"

#include "ocean/rendering/Manager.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

#include "ocean/scenedescription/Manager.h"

#include "metaonly/ocean/platform/meta/Login.h"

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	#include "ocean/devices/arcore/ACFactory.h"

	#include "ocean/platform/android/NativeInterfaceManager.h"

	#include "ocean/rendering/glescenegraph/GLESMediaTexture2D.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

XRPlaygroundCommon::XRPlaygroundCommon()
{
	// nothing to do here
}

XRPlaygroundCommon::~XRPlaygroundCommon()
{
	// nothing to do here
}

bool XRPlaygroundCommon::initialize(const std::string& appVersion)
{
	Log::debug() << "XRPlaygroundCommon::initialize()";

	const ScopedLock scopedLock(lock_);

	RandomI::initialize();

	gpsTracker_ = Devices::Manager::get().device(Devices::GPSTracker::deviceTypeGPSTracker());
	if (gpsTracker_)
	{
		gpsTracker_->start();
	}
	else
	{
		Log::warning() << "Failed to access a GPS tracker";
	}

	ExperiencesManager::registerAllCodeBasedExperiences();

	if (CV::Fonts::FontManager::get().registerSystemFonts() != 0)
	{
#ifdef OCEAN_DEBUG
		const std::vector<std::string> familyNames(CV::Fonts::FontManager::get().familyNames());

		for (const std::string& familyName : familyNames)
		{
			Log::debug() << "Registered font '" << familyName << "'";
		}
#endif // OCEAN_DEBUG
	}
	else
	{
		Log::warning() << "Failed to register any system font";
	}

	if (engine_ || framebuffer_)
	{
		ocean_assert(false && "Already initialized!");
		return false;
	}

	engine_ = Rendering::Manager::get().engine("", Rendering::Engine::API_OPENGLES);

	if (engine_.isNull())
	{
		ocean_assert(false && "This should never happen!");

		Log::error() << "Failed to acquire rendering engine";
		return false;
	}

	framebuffer_ = engine_->createFramebuffer(Rendering::Framebuffer::FRAMEBUFFER_WINDOW);

	const Rendering::PerspectiveViewRef view = engine_->factory().createPerspectiveView();

	if (framebuffer_.isNull() || view.isNull())
	{
		ocean_assert(false && "This should never happen!");

		Log::error() << "Failed to create basic rendering objects";
		return false;
	}

	view->setFovX(Numeric::deg2rad(45));
	view->setBackgroundColor(RGBAColor(0, 0, 0));

	framebuffer_->setView(view);

	if (!appVersion.empty())
	{
		sceneText_ = engine_->factory().createScene();

		std::string text = " Version: " + appVersion + " ";

#ifdef OCEAN_DEBUG
		text += "\n (debug) ";
#endif

		const Rendering::TransformRef transformText = Rendering::Utilities::createText(*engine_, text, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.015), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
		transformText->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.14), Scalar(-0.29))));
		sceneText_->addChild(transformText);

		framebuffer_->addScene(sceneText_);
	}

#if defined(OCEAN_PLATFORM_BUILD_ANDROID)
	arCoreTexture_ = engine_->factory().createMediaTexture2D();
#endif

	engine_->update(Timestamp(true));

	initializeNetworking();

	return true;
}

bool XRPlaygroundCommon::loadContent(const std::string& content, const bool replace)
{
	if (sceneText_ && replace)
	{
		// we remove the initial scene holding additional text information

		if (framebuffer_)
		{
			framebuffer_->removeScene(sceneText_);
			sceneText_.release();
		}
	}

	return ContentManager::get().loadContent(content, replace ? ContentManager::LM_LOAD_REPLACE : ContentManager::LM_LOAD_ADD);
}

bool XRPlaygroundCommon::unloadContent(const std::string& content)
{
	return ContentManager::get().unloadContent(content);
}

void XRPlaygroundCommon::unloadContent()
{
	ContentManager::get().unloadContent();
}

bool XRPlaygroundCommon::startVRSRecording(const std::string& filename)
{
	const ScopedLock scopedLock(lock_);

	if (filename.empty())
	{
		ocean_assert(false && "Invalid filename!");
		return false;
	}

	if (deviceRecorder_.isStarted())
	{
		Log::warning() << "VRS recording already active";
		return false;
	}

	ocean_assert(vrsDevices_.empty());
	ocean_assert(vrsFrameMediums_.empty());

	// we add the medium of the video background to the recording

	const Media::FrameMediumRef frameMedium = backgroundFrameMedium();

	if (frameMedium)
	{
		deviceRecorder_.addFrameMedium(frameMedium);

		vrsFrameMediums_.push_back(frameMedium);
	}

	// we ensure that the VRS recording contains a World Tracker and GPS Tracker

#ifdef OCEAN_PLATFORM_BUILD_MOBILE

	if (frameMedium)
	{
#if defined(OCEAN_PLATFORM_BUILD_ANDROID)
		Devices::VisualTrackerRef worldTracker = Devices::Manager::get().device("ARCore 6DOF World Tracker");
#elif defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
		Devices::VisualTrackerRef worldTracker = Devices::Manager::get().device("ARKit 6DOF World Tracker");
#endif

		if (worldTracker)
		{
			worldTracker->setInput(frameMedium);
			worldTracker->start();

			vrsDevices_.push_back(std::move(worldTracker));
		}
	}
#endif // OCEAN_PLATFORM_BUILD_MOBILE

	if (!deviceRecorder_.start(filename))
	{
		Log::error() << "Failed to start the VRS recording!";

		vrsDevices_.clear();

		if (frameMedium)
		{
			deviceRecorder_.removeFrameMedium(frameMedium);
		}

		return false;
	}

	Log::info() << "Starting VRS recording to file '" << filename << "'";
	return true;
}

bool XRPlaygroundCommon::stopVRSRecording()
{
	const ScopedLock scopedLock(lock_);

	if (!deviceRecorder_.isStarted())
	{
		Log::warning() << "VRS recording not active";
		return false;
	}

	if (!deviceRecorder_.stop())
	{
		Log::error() << "Failed to stop the VRS recording!";
		return false;
	}

	for (const Media::FrameMediumRef& frameMedium : vrsFrameMediums_)
	{
		deviceRecorder_.removeFrameMedium(frameMedium);
	}

	vrsFrameMediums_.clear();

	vrsDevices_.clear();

	Log::info() << "Stopped VRS recording";

	return true;
}

bool XRPlaygroundCommon::isVRSRecordingActive()
{
	return deviceRecorder_.isStarted();
}

void XRPlaygroundCommon::resize(const unsigned int width, const unsigned int height, const Scalar aspectRatio)
{
	const Rendering::FramebufferRef framebuffer(framebuffer_);

	if (framebuffer.isNull())
	{
		return;
	}

	const Rendering::PerspectiveViewRef perspectiveView = framebuffer->view();

	if (perspectiveView.isNull())
	{
		return;
	}

	if (width != 0u && height != 0u)
	{
		framebuffer->setViewport(0u, 0u, width, height);
	}

	if (aspectRatio > Numeric::eps())
	{
		perspectiveView->setAspectRatio(aspectRatio);
	}
}

void XRPlaygroundCommon::preRender(const Interaction::UserInterface& userInterface)
{
	const Rendering::EngineRef engine(engine_);
	const Rendering::FramebufferRef framebuffer(framebuffer_);

	if (engine.isNull() || framebuffer.isNull())
	{
		return;
	}

	const Rendering::PerspectiveViewRef perspectiveView = framebuffer->view();

	if (perspectiveView.isNull())
	{
		return;
	}

	const Timestamp currentTimestamp(true);

	ContentManager::get().processContent(userInterface, engine, *framebuffer, currentTimestamp);

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	if (arCoreTexture_)
	{
		// for ARCore, we need to create a dummy texture which we currently do not use for rendering
		Devices::ARCore::ACFactory::update(arCoreTexture_.force<Rendering::GLESceneGraph::GLESMediaTexture2D>().primaryTextureId());
	}
#endif

#ifdef OCEAN_PLATFORM_BUILD_MOBILE
	Platform::Meta::Avatars::Manager::get().update(engine_, double(currentTimestamp));
#endif

	Network::Verts::Manager::get().update(currentTimestamp);

	try
	{
		perspectiveView->updateToIdealFovX();

		// Applying specific pre-updates on all scene descriptions necessary before the interaction plugins are applied
		Timestamp updateTimestamp = SceneDescription::Manager::get().preUpdate(perspectiveView, currentTimestamp);

		updateTimestamp = Interaction::Manager::get().preUpdate(userInterface, engine_, perspectiveView, updateTimestamp);

		// Applying updates on all scene descriptions
		SceneDescription::Manager::get().update(perspectiveView, updateTimestamp);

		// Updates the rendering engine
		engine->update(updateTimestamp);

		Interaction::Manager::get().postUpdate(userInterface, engine_, perspectiveView, updateTimestamp);
	}
	catch (const std::exception& exception)
	{
		Log::error() << exception.what();
	}
	catch (...)
	{
		Log::error() << "Uncaught exception occured during rendering!";
	}
}

void XRPlaygroundCommon::render()
{
	const Rendering::FramebufferRef framebuffer(framebuffer_);

	if (framebuffer.isNull())
	{
		return;
	}

	try
	{
		// Rendering the current frame
		framebuffer->render();
	}
	catch (const std::exception& exception)
	{
		Log::error() << exception.what();
	}
	catch (...)
	{
		Log::error() << "Uncaught exception occured during rendering!";
	}

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	if (arCoreTexture_)
	{
		Devices::ARCore::ACFactory::update(arCoreTexture_.force<Rendering::GLESceneGraph::GLESMediaTexture2D>().primaryTextureId());
	}
#endif
}

bool XRPlaygroundCommon::setDevice_T_display(const HomogenousMatrix4& device_T_display)
{
	const Rendering::FramebufferRef framebuffer(framebuffer_);

	if (framebuffer.isNull())
	{
		return false;
	}

	if (!framebuffer->setDevice_T_display(device_T_display))
	{
		return false;
	}

	const Rendering::PerspectiveViewRef perspectiveView = framebuffer->view();

	if (Rendering::UndistortedBackgroundRef undistortedBackground = perspectiveView->background())
	{
		if (Media::FrameMediumRef frameMedium = undistortedBackground->medium())
		{
			const HomogenousMatrix4 display_T_camera = device_T_display.inverted() * HomogenousMatrix4(frameMedium->device_T_camera());

			undistortedBackground->setOrientation(display_T_camera.rotation());

			perspectiveView->updateToIdealFovX();
		}
	}

	return true;
}

Media::FrameMediumRef XRPlaygroundCommon::backgroundFrameMedium() const
{
	const ScopedLock scopedLock(lock_);

	if (framebuffer_.isNull())
	{
		return Media::FrameMediumRef();
	}

	const Rendering::PerspectiveViewRef perspectiveView = framebuffer_->view();

	if (perspectiveView.isNull())
	{
		return Media::FrameMediumRef();
	}

	const Rendering::UndistortedBackgroundRef undistortedBackground = perspectiveView->background();

	if (undistortedBackground.isNull())
	{
		return Media::FrameMediumRef();
	}

	return undistortedBackground->medium();
}

bool XRPlaygroundCommon::setBackgroundFrameMedium(const Media::FrameMediumRef& frameMedium)
{
	const ScopedLock scopedLock(lock_);

	if (engine_.isNull() || framebuffer_.isNull())
	{
		return !frameMedium;
	}

	Rendering::PerspectiveViewRef perspectiveView = framebuffer_->view();

	if (perspectiveView.isNull())
	{
		if (frameMedium)
		{
			perspectiveView = engine_->factory().createPerspectiveView();
			ocean_assert(perspectiveView);

			if (perspectiveView.isNull())
			{
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	Rendering::UndistortedBackgroundRef undistortedBackground = perspectiveView->background();

	if (undistortedBackground.isNull())
	{
		if (frameMedium)
		{
			undistortedBackground = engine_->factory().createUndistortedBackground();

			ocean_assert(undistortedBackground);

			if (undistortedBackground.isNull())
			{
				return false;
			}

			perspectiveView->addBackground(undistortedBackground);
		}
		else
		{
			return true;
		}
	}

	if (frameMedium.isNull())
	{
		perspectiveView->removeBackground(undistortedBackground);
	}
	else
	{
		undistortedBackground->setMedium(frameMedium);

		const HomogenousMatrix4 device_T_display = framebuffer_->device_T_display();
		ocean_assert(device_T_display.isValid());

		const HomogenousMatrix4 display_T_camera = device_T_display.inverted() * HomogenousMatrix4(frameMedium->device_T_camera());

		undistortedBackground->setOrientation(display_T_camera.rotation());
	}

	return true;
}

void XRPlaygroundCommon::release()
{
	Log::debug() << "XRPlaygroundCommon::release()";

	const ScopedLock scopedLock(lock_);

	gpsTracker_.release();

	deviceRecorder_.release();

	vrsDevices_.clear();
	vrsFrameMediums_.clear();

	ContentManager::get().release();

	Interaction::Manager::get().release();

	SceneDescription::Manager::get().unloadScenes();

	sceneText_.release();

	if (framebuffer_)
	{
		framebuffer_->clearScenes();
	}

#ifdef OCEAN_PLATFORM_BUILD_ANDROID
	arCoreTexture_.release();
#endif

#ifdef OCEAN_PLATFORM_BUILD_MOBILE
	Platform::Meta::Avatars::Manager::get().release();
#endif

	framebuffer_.release();
	engine_.release();
}

void XRPlaygroundCommon::touchEventStarted(const Interaction::UserInterface& userInterface, const Vector2& screenPosition)
{
	Line3 ray;
	Rendering::ObjectId pickedObjectId = Rendering::invalidObjectId;
	Vector3 pickedObjectPosition;

	if (determinePickedObject(screenPosition, ray, pickedObjectId, pickedObjectPosition))
	{
		const Timestamp currentTimestamp(true);

		Interaction::Manager::get().onMousePress(userInterface, engine_, std::string(), screenPosition, ray, pickedObjectId, pickedObjectPosition, currentTimestamp);
	}
}

void XRPlaygroundCommon::touchEventMoved(const Interaction::UserInterface& userInterface, const Vector2& screenPosition)
{
	Line3 ray;
	Rendering::ObjectId pickedObjectId = Rendering::invalidObjectId;
	Vector3 pickedObjectPosition;

	if (determinePickedObject(screenPosition, ray, pickedObjectId, pickedObjectPosition))
	{
		const Timestamp currentTimestamp(true);

		Interaction::Manager::get().onMouseMove(userInterface, engine_, std::string(), screenPosition, ray, pickedObjectId, pickedObjectPosition, currentTimestamp);
	}
}

void XRPlaygroundCommon::touchEventStopped(const Interaction::UserInterface& userInterface, const Vector2& screenPosition)
{
	Line3 ray;
	Rendering::ObjectId pickedObjectId = Rendering::invalidObjectId;
	Vector3 pickedObjectPosition;

	if (determinePickedObject(screenPosition, ray, pickedObjectId, pickedObjectPosition))
	{
		const Timestamp currentTimestamp(true);

		Interaction::Manager::get().onMouseRelease(userInterface, engine_, std::string(), screenPosition, ray, pickedObjectId, pickedObjectPosition, currentTimestamp);
	}
}

void XRPlaygroundCommon::keyEventPressed(const Interaction::UserInterface& userInterface, const std::string& key)
{
	const Timestamp currentTimestamp(true);

	Interaction::Manager::get().onKeyPress(userInterface, engine_, key, currentTimestamp);
}

void XRPlaygroundCommon::keyEventReleased(const Interaction::UserInterface& userInterface, const std::string& key)
{
	const Timestamp currentTimestamp(true);

	Interaction::Manager::get().onKeyRelease(userInterface, engine_, key, currentTimestamp);
}

void XRPlaygroundCommon::initializeNetworking()
{
	const Platform::Meta::Login::LoginTypes priorityLoginTypes = // we try logins with this order
	{
		Platform::Meta::Login::LT_FACEBOOK,
		Platform::Meta::Login::LT_OCULUS,
		Platform::Meta::Login::LT_META,
	};

	bool vertsInitialized = false;

	for (const Platform::Meta::Login::LoginType priorityLoginType : priorityLoginTypes)
	{
		std::string userId;
		std::string userToken;

		if (!Platform::Meta::Login::get().login(priorityLoginType, userId, userToken))
		{
			continue;
		}

		if (!vertsInitialized && !userToken.empty())
		{
			Network::Verts::Manager::get().initialize("XRPlayground", userToken);
			vertsInitialized = true;
		}

#ifdef OCEAN_PLATFORM_BUILD_MOBILE
		if (!userId.empty() && !userToken.empty())
		{
			uint64_t valueUserId;
			if (String::isUnsignedInteger64(userId, &valueUserId))
			{
				Platform::Meta::Avatars::Manager::UserType userType = translateLoginType(priorityLoginType);

				if (userType != Platform::Meta::Avatars::Avatar::UT_UNKNOWN)
				{
#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS)
					constexpr ovrAvatar2Platform platform = ovrAvatar2Platform_iOS;
#elif defined(OCEAN_PLATFORM_BUILD_ANDROID)
					constexpr ovrAvatar2Platform platform = ovrAvatar2Platform_Android;
#else
					constexpr ovrAvatar2Platform platform = ovrAvatar2Platform_PC;
#endif

					if (Platform::Meta::Avatars::Manager::get().initialize(platform, userType, valueUserId, userToken))
					{
						Log::info() << "Avatar2 initialized for local user " << valueUserId;

						for (const Platform::Meta::Login::LoginType backupLoginType : Platform::Meta::Login::get().loginTypes())
						{
							if (backupLoginType != priorityLoginType)
							{
								userType = translateLoginType(backupLoginType);
								userToken = Platform::Meta::Login::get().userToken(backupLoginType);

								if (userType != Platform::Meta::Avatars::Avatar::UT_UNKNOWN && !userToken.empty())
								{
									Platform::Meta::Avatars::Manager::get().setAccessToken(userType, userToken);
								}
							}
						}
					}
				}
				else
				{
					Log::error() << "Unknown user type, Avatar2 was not initialized";
				}

				break;
			}
		}
#endif // OCEAN_PLATFORM_BUILD_MOBILE
	}

	if (!vertsInitialized)
	{
		Log::error() << "Failed to initialize VERTS due to missing access token";
	}
}

bool XRPlaygroundCommon::determinePickedObject(const Vector2& screenPosition, Line3& ray, Rendering::ObjectId& pickedObjectId, Vector3& pickedObjectPosition) const
{
	const Rendering::EngineRef engine(engine_);
	const Rendering::FramebufferRef framebuffer(framebuffer_);

	if (engine.isNull() || framebuffer.isNull())
	{
		return false;
	}

	const Rendering::PerspectiveViewRef perspectiveView = framebuffer->view();

	if (perspectiveView.isNull())
	{
		return false;
	}

	unsigned int viewportLeft = 0u;
	unsigned int viewportTop = 0u;
	unsigned int viewportWidth = 0u;
	unsigned int viewportHeight = 0u;
	framebuffer->viewport(viewportLeft, viewportTop, viewportWidth, viewportHeight);

	if (screenPosition.x() < 0 || screenPosition.x() > Scalar(viewportWidth)
			|| screenPosition.y() < 0 || screenPosition.y() > Scalar(viewportHeight))
	{
		return false;
	}

	ray = perspectiveView->viewingRay(screenPosition.x(), screenPosition.y(), viewportWidth, viewportHeight);

	pickedObjectId = Rendering::invalidObjectId;
	pickedObjectPosition = Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());

	return true;
}

#ifdef OCEAN_PLATFORM_BUILD_MOBILE

Platform::Meta::Avatars::Manager::UserType XRPlaygroundCommon::translateLoginType(const Platform::Meta::Login::LoginType loginType)
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

#endif // OCEAN_PLATFORM_BUILD_MOBILE

}

}
