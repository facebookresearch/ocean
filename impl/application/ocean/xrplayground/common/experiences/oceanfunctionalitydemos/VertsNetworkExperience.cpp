// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/VertsNetworkExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/Base.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"

#include "ocean/network/verts/Manager.h"

#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

VertsNetworkExperience::~VertsNetworkExperience()
{
	// nothing to do here
}

bool VertsNetworkExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	const Rendering::TransformRef textTransformA = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), false, 0, 0, Scalar(0.2), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingTextA_);
	textTransformA->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(1.5), -5)));
	experienceScene()->addChild(textTransformA);

	const Rendering::TransformRef textTransformB = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), false, 0, 0, Scalar(0.2), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingTextB_);
	textTransformB->setTransformation(HomogenousMatrix4(Vector3(0, 0, -5)));
	experienceScene()->addChild(textTransformB);

	const Rendering::TransformRef textTransformC = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), false, 0, 0, Scalar(0.2), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_TOP, std::string(), std::string(), &renderingTextC_);
	textTransformC->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(-1.5), -5)));
	experienceScene()->addChild(textTransformC);

	const std::string vertsZoneName = "XRPlayground://VERTS_NETWORK_EXPERIENCE_PUBLIC_ZONE";

	vertsDriver_ = Network::Verts::Manager::get().driver(vertsZoneName);

	if (vertsDriver_)
	{
		receiveOnChannelScopedSubscription_ = vertsDriver_->addReceiveOnChannelCallback(0u, std::bind(&VertsNetworkExperience::onReceiveOnChannel, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		receiveContainerScopedSubscription_ = vertsDriver_->addReceiveContainerCallback(std::bind(&VertsNetworkExperience::onReceiveContainer, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
	}
	else
	{
		Log::error() << "Failed to create VERTS driver";
	}

	return true;
}

bool VertsNetworkExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	receiveContainerScopedSubscription_.release();
	receiveOnChannelScopedSubscription_.release();
	vertsDriver_ = nullptr;

	renderingTextA_.release();
	renderingTextB_.release();

	return true;
}

Timestamp VertsNetworkExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);
		const size_t bytesReceivedOnChannel = bytesReceivedOnChannel_;
		const size_t bytesReceivedAsContainer = bytesReceivedAsContainer_;
	scopedLock.release();

	static Timestamp lastSendTimestamp(false);

	UnorderedIndexSet64 remoteUserIds;

	if (vertsDriver_ && vertsDriver_->isInitialized())
	{
		remoteUserIds = vertsDriver_->remoteUserIds();

		if (sendDataOnChannel_)
		{
			sendDataOnChannel_ = false;

			constexpr unsigned int maximalBytes = 1024u;

			for (unsigned int n = 0u; n < 10u; ++n)
			{
				const unsigned int bytesToSend = RandomI::random32() % maximalBytes + 1u;

				std::vector<uint8_t> buffer(bytesToSend);
				for (uint8_t& value : buffer)
				{
					value = uint8_t(RandomI::random(255u));
				}

				constexpr unsigned int channelIndex = 0u;

				vertsDriver_->sendOnChannel(channelIndex, std::move(buffer));
				bytesSentOnChannel_ += size_t(bytesToSend);

				Log::info() << "Sent " << bytesToSend << " bytes on channel " << channelIndex;
			}

			lastSendTimestamp = timestamp;
		}

		if (sendDataAsContainer_)
		{
			sendDataAsContainer_ = false;

			constexpr unsigned int maximalBytes = 5u * 1024u * 1024u;

			RandomGenerator randomGenerator;

			const unsigned int bytesToSend = RandomI::random32(randomGenerator) % maximalBytes + 1u;

			std::vector<uint8_t> buffer(bytesToSend);
			for (uint8_t& value : buffer)
			{
				value = uint8_t(RandomI::random(randomGenerator, 255u));
			}

			std::string identifier = "random_container_identifier_" + std::string(1, 'A' + char(RandomI::random(randomGenerator, 25)));
			unsigned int version = ++containerVersion_;

			vertsDriver_->sendContainer(identifier, version, buffer.data(), buffer.size());
			bytesSentAsContainer_ += size_t(bytesToSend);

			Log::info() << "Sent version " << version << " of container '" << identifier << "' with " << bytesToSend << " bytes";

			lastSendTimestamp = timestamp;
		}
	}

	const std::string textA = (lastSendTimestamp + 0.1 > timestamp) ? " Sending data " : "";
	renderingTextA_->setText(textA);

	std::string textB = " On channel: \n";
	textB += " Sent " + String::insertCharacter(String::toAString(bytesSentOnChannel_), ',', 3, false) + " bytes \n";
	textB += " Received " + String::insertCharacter(String::toAString(bytesReceivedOnChannel), ',', 3, false) + " bytes \n\n";
	textB += "  As Container: \n";
	textB += " Sent " + String::insertCharacter(String::toAString(bytesSentAsContainer_), ',', 3, false) + " bytes \n";
	textB += " Received " + String::insertCharacter(String::toAString(bytesReceivedAsContainer), ',', 3, false) + " bytes";
	renderingTextB_->setText(textB);

	std::string textC = " Remote user ids: ";
	for (const uint64_t remoteUserId : remoteUserIds)
	{
		textC += "\n " + String::toAString(remoteUserId) + " ";
	}

	renderingTextC_->setText(textC);

	return timestamp;
}

void VertsNetworkExperience::onKeyPress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& key, const Timestamp /*timestamp*/)
{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	if (key == "A")
	{
		sendDataOnChannel_ = true;
	}
	if (key == "X")
	{
		sendDataAsContainer_ = true;
	}
#else
	sendDataOnChannel_ = true;
	sendDataAsContainer_ = true;
#endif
}

void VertsNetworkExperience::onMousePress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& /*screenPosition*/, const Line3& /*ray*/, const Timestamp /*timestamp*/)
{
	sendDataOnChannel_ = true;
	sendDataAsContainer_ = true;
}

std::unique_ptr<XRPlaygroundExperience> VertsNetworkExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new VertsNetworkExperience());
}

void VertsNetworkExperience::onReceiveOnChannel(Network::Verts::Driver& driver, const unsigned int channelIndex, const void* buffer, const size_t size)
{
	Log::info() << "Received " << size << " bytes on channel " << channelIndex;

	const ScopedLock scopedLock(lock_);

	bytesReceivedOnChannel_ += size;
}

void VertsNetworkExperience::onReceiveContainer(Network::Verts::Driver& driver, const uint32_t sessionId, const uint64_t userId, const std::string& identifier, const unsigned int version, const Network::Verts::Driver::SharedBuffer& buffer)
{
	Log::info() << "Received version " << version << " of container '" << identifier << "' with " << buffer->size() << " bytes from user " << userId;

	const ScopedLock scopedLock(lock_);

	bytesReceivedAsContainer_ += buffer->size();
}

}

}
