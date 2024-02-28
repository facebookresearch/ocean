// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/PhoneCameraExperience.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

PhoneCameraExperience::~PhoneCameraExperience()
{
	// nothing to do here
}

bool PhoneCameraExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	// by default, the background is showing the back-facing camera, we just stop the video stream

	if (!engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());
			undistortedBackground->setMedium(Media::FrameMediumRef());
			undistortedBackground->setVisible(false);
		}
	}

	const Media::Library::Definitions definitions = Media::Manager::get().selectableMedia(Media::Medium::LIVE_VIDEO);

	std::vector<std::string> cameraNames;
	cameraNames.reserve(definitions.size());

	for (const Media::Library::Definition& definition : definitions)
	{
		cameraNames.emplace_back(definition.url);
	}

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	if (userInterface.isIOS())
	{
		showUserInterinterfaceIOS(userInterface, cameraNames);
	}

	return true;
#else
	return false;
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
}

bool PhoneCameraExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	// resetting, the background is showing the back-facing camera, we just stop the video stream

	if (!engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());

			Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium("LiveVideoId:0");

			if (frameMedium)
			{
				undistortedBackground->setVisible(true);
				undistortedBackground->setMedium(frameMedium);
				frameMedium->start();
			}
			else
			{
				Log::error() << "Failed to reset background video";
			}
		}
	}

	return true;
}

Timestamp PhoneCameraExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);
		const std::string newCameraName(std::move(newCameraName_));
		newCameraName_ = std::string();
	scopedLock.release();

	if (!newCameraName.empty())
	{
		experienceScene()->clear();
		frameMedium_.release();

		frameMedium_ = Media::Manager::get().newMedium(newCameraName, Media::Medium::LIVE_VIDEO);

		if (frameMedium_)
		{
			const HomogenousMatrixD4 device_T_camera = frameMedium_->device_T_camera();

			Rendering::TransformRef transformBox = Rendering::Utilities::createBox(engine, Vector3(1, 1, 1), frameMedium_);
			transformBox->setTransformation(HomogenousMatrix4(Vector3(0, 0, -2)) * Quaternion(device_T_camera.rotation()));

			experienceScene()->addChild(transformBox);

			frameMedium_->start();
		}
	}

	return timestamp;
}

void PhoneCameraExperience::switchCamera(const std::string& cameraName)
{
	ocean_assert(!cameraName.empty());

	const ScopedLock scopedLock(lock_);

	if (frameMedium_ && frameMedium_->url() == cameraName)
	{
		return;
	}

	newCameraName_ = cameraName;
}

std::unique_ptr<XRPlaygroundExperience> PhoneCameraExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new PhoneCameraExperience());
}

}

}
