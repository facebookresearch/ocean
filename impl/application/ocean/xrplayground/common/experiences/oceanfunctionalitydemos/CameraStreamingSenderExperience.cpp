// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/CameraStreamingSenderExperience.h"

#include "ocean/media/Utilities.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

#include "metaonly/ocean/network/verts/Manager.h"

#include "metaonly/ocean/network/tigon/TigonClient.h"

namespace Ocean
{

using namespace Network;

namespace XRPlayground
{

CameraStreamingSenderExperience::~CameraStreamingSenderExperience()
{
	// nothing to do here
}

bool CameraStreamingSenderExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	if (!engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());

			frameMedium_ = undistortedBackground->medium();
		}
	}

	const Rendering::AbsoluteTransformRef absoluteTransform = engine->factory().createAbsoluteTransform();
	absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
	absoluteTransform->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.065)));
	experienceScene()->addChild(absoluteTransform);

	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, " Start the Camera Streaming Receiver \n experience on your headset ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0.005, 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingText_);

	absoluteTransform->addChild(textTransform);

	if (frameMedium_.isNull())
	{
		ocean_assert(false && "This should never happen!");

		renderingText_->setText("Failed to access camera stream");
	}

	// we need to determine a zone name which is unique for the user (user id does not work as user may be logged in with individual ids like Meta, Facebook, Oculus)

	std::string userName;
	if (!Network::Tigon::TigonClient::get().determineUserName(userName))
	{
		renderingText_->setText(" Failed to determine user name \n User needs to be logged in ");
		return true;
	}

	const std::string vertsZoneName = "XRPlayground://CAMERA_STREAMING_ZONE_FOR_" + String::toAString(std::hash<std::string>()(userName));

	vertsDriver_ = Verts::Manager::get().driver(vertsZoneName);

	if (vertsDriver_)
	{
		changedUsersScopedSubscription_ = vertsDriver_->addChangedUsersCallback(std::bind(&CameraStreamingSenderExperience::onChangedUsers, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}

	return true;
}

bool CameraStreamingSenderExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	changedUsersScopedSubscription_.release();
	vertsDriver_ = nullptr;

	return true;
}

Timestamp CameraStreamingSenderExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);
		const bool streamData = !userIds_.empty();
	scopedLock.release();

	if (streamData)
	{
		if (frameMedium_ && vertsDriver_ && vertsDriver_->isInitialized())
		{
			const FrameRef frame = frameMedium_->frame();

			if (frame && frame->timestamp() > lastCameraTimestamp_)
			{
				Media::Utilities::Buffer buffer;
				if (Media::Utilities::saveImage(*frame, "jpg", buffer))
				{
					vertsDriver_->sendContainer("CAMERA", cameraFrameCounter_++, buffer.data(), buffer.size());

					renderingText_->setText("Connection started");
				}

				lastCameraTimestamp_ = frame->timestamp() + 0.5;
			}
		}
	}
	else if (cameraFrameCounter_ != 0u)
	{
		renderingText_->setText("Connection stopped");
	}

	return timestamp;
}

std::unique_ptr<XRPlaygroundExperience> CameraStreamingSenderExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new CameraStreamingSenderExperience());
}

void CameraStreamingSenderExperience::onChangedUsers(Network::Verts::Driver& driver, const UnorderedIndexSet64& addedUsers, const UnorderedIndexSet64& removedUsers)
{
	const ScopedLock scopedLock(lock_);

	for (const Index64 userId : addedUsers)
	{
		userIds_.emplace(userId);
	}

	for (const Index64 userId : removedUsers)
	{
		userIds_.erase(userId);
	}
}

}

}
