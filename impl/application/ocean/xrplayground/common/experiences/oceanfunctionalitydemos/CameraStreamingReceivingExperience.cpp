// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/CameraStreamingReceiverExperience.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/rendering/Utilities.h"

#include "metaonly/ocean/network/tigon/TigonClient.h"

#include "metaonly/ocean/network/verts/Manager.h"

namespace Ocean
{

using namespace Network;

namespace XRPlayground
{

CameraStreamingReceiverExperience::~CameraStreamingReceiverExperience()
{
	// nothing to do here
}

bool CameraStreamingReceiverExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	pixelImage_ = Media::Manager::get().newMedium("PIXEL_IMAGE", Media::Medium::PIXEL_IMAGE);
	pixelImage_->start();

	std::string message = " Failed to determine \n the local network address ";

	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, " Start the Camera Streaming Sender \n experience on your phone ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), true, 2, 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingText_);
	textTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -5)));

	experienceScene()->addChild(textTransform);

	renderingTransformBox_ = Rendering::Utilities::createBox(engine, Vector3(1, 1, 0), pixelImage_);
	renderingTransformBox_->setVisible(false);
	experienceScene()->addChild(renderingTransformBox_);

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
		receiveContainerScopedSubscription_ = vertsDriver_->addReceiveContainerCallback(std::bind(&CameraStreamingReceiverExperience::onReceiveContainer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
	}

	return true;
}

bool CameraStreamingReceiverExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	receiveContainerScopedSubscription_.release();
	vertsDriver_ = nullptr;

	renderingTransformBox_.release();
	renderingText_.release();

	return true;
}

Timestamp CameraStreamingReceiverExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);
		Frame recentFrame = std::move(recentFrame_);
	scopedLock.release();

	if (recentFrame)
	{
		const Vector3 scale(Vector3(Scalar(recentFrame.width()), Scalar(recentFrame.height()), 0) * Scalar(0.004));

		renderingTransformBox_->setTransformation(HomogenousMatrix4(Vector3(0, 0, -6), Quaternion(Vector3(0, 0, 1), -Numeric::pi_2()), scale));
		renderingTransformBox_->setVisible(true);

		recentFrame.setTimestamp(timestamp);
		pixelImage_->setPixelImage(std::move(recentFrame));
	}

	return timestamp;
}

std::unique_ptr<XRPlaygroundExperience> CameraStreamingReceiverExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new CameraStreamingReceiverExperience());
}

void CameraStreamingReceiverExperience::onReceiveContainer(Network::Verts::Driver& driver, const uint32_t sessionId, const uint64_t userId, const std::string& identifier, const unsigned int version, const Network::Verts::Driver::SharedBuffer& buffer)
{
	ocean_assert(pixelImage_);

	Frame frame = Media::Utilities::loadImage(buffer->data(), buffer->size());

	if (frame)
	{
		renderingText_->setText("");

		const ScopedLock scopedLock(lock_);
		recentFrame_ = std::move(frame);
	}
	else
	{
		renderingText_->setText(" Failed to decode image with " + String::toAString(buffer->size()) + " bytes ");
	}
}

}

}
