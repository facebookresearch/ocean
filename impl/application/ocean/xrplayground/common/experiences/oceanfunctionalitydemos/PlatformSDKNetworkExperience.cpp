// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/PlatformSDKNetworkExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/RandomI.h"

#include "ocean/rendering/Utilities.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/platformsdk/Room.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

using namespace Platform::Meta::Quest;
using namespace Platform::Meta::Quest::VrApi::Application;

PlatformSDKNetworkExperience::~PlatformSDKNetworkExperience()
{
	// nothing to do here
}

bool PlatformSDKNetworkExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	const Rendering::TransformRef textTransformA = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), false, 0, 0, Scalar(0.2), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingTextA_);
	textTransformA->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.5), -5)));
	experienceScene()->addChild(textTransformA);

	const Rendering::TransformRef textTransformB = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), false, 0, 0, Scalar(0.2), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingTextB_);
	textTransformB->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(-0.5), -5)));
	experienceScene()->addChild(textTransformB);

	vrTableMenu_ = VRTableMenu(engine, engine->framebuffers().front());

	connectionSubscription_ = PlatformSDK::Network::get().addConnectionCallback(std::bind(&PlatformSDKNetworkExperience::onConnection, this, std::placeholders::_1));
	receiveSubscription_ = PlatformSDK::Network::get().addReceiveCallback(PlatformSDK::Network::CI_CUSTOM, std::bind(&PlatformSDKNetworkExperience::onReceive, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5));

	PlatformSDK::Room::get().createAndJoin();

	return true;
}

bool PlatformSDKNetworkExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	connectionSubscription_.release();
	receiveSubscription_.release();

	renderingTextA_.release();
	renderingTextB_.release();
	vrTableMenu_.release();

	return true;
}

Timestamp PlatformSDKNetworkExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	uint64_t roomId = 0ull;
	if (PlatformSDK::Room::get().hasLatestCreateAndJoinResult(roomId))
	{
		PlatformSDK::Room::get().invitableUsers(roomId);
	}

	if (PlatformSDK::Room::get().hasLatestInvitableUsersResult(invitableUsers_))
	{
		VRTableMenu::Entries menuEntries;
		menuEntries.reserve(invitableUsers_.size());

		for (size_t n = 0u; n < invitableUsers_.size(); ++n)
		{
			const PlatformSDK::Room::User& invitableUser = invitableUsers_[n];
			menuEntries.emplace_back(invitableUser.oculusId(), String::toAString(n));
		};

		const VRTableMenu::Group menuGroup("Connect with one of the following users", std::move(menuEntries));

		vrTableMenu_.setMenuEntries(menuGroup);
		vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1)));
	}

	if (vrTableMenu_.isShown())
	{
		std::string entryUrl;
		if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
		{
			vrTableMenu_.hide();

			if (!entryUrl.empty())
			{
				int index = 0;
				if (String::isInteger32(entryUrl, &index) && index >= 0 && index <= int(invitableUsers_.size()))
				{
					remoteUserId_ = invitableUsers_[index].userId();
				}
			}

			if (remoteUserId_ != 0ull)
			{
				if (PlatformSDK::Network::get().connect(remoteUserId_))
				{
					Log::info() << "Connection established to user " << remoteUserId_;

					nextSendTimestamp_ = timestamp;
				}
				else
				{
					Log::error() << "Failed to connect to user " << remoteUserId_;

					remoteUserId_ = 0ull;
				}
			}
			else
			{
				Log::error() << "Failed to select a user";
			}
		}
	}

	TemporaryScopedLock scopedLock(lock_);
		const uint64_t remoteUserId = remoteUserId_;
		const size_t bytesReceived = bytesReceived_;
	scopedLock.release();

	if (PlatformSpecific::get().trackedRemoteDevice().buttonsPressed() & (ovrButton_A | ovrButton_X))
	{
		sendingActive_ = !sendingActive_;
	}

	std::string textA = sendingActive_ ? " Sending is active " : " Sending is paused ";
	renderingTextA_->setText(textA);

	std::string textB = " Sent: " + String::insertCharacter(String::toAString(bytesSent_ / 1024), ',', 3, false) + "KB \n";
	textB += " Received: " + String::insertCharacter(String::toAString(bytesReceived / 1024), ',', 3, false) + "KB \n\n";
	textB += " Send Queue size: " + String::toAString(PlatformSDK::Network::get().sendQueueSizeTCP()) + "\n ";
	textB += " Receive Queue size: " + String::toAString(PlatformSDK::Network::get().receiveQueueSizeTCP()) + " ";
	renderingTextB_->setText(textB);

	if (remoteUserId != 0ull)
	{
		if (sendingActive_ && timestamp >= nextSendTimestamp_)
		{
			constexpr unsigned int maximalBytes = 500u * 1024u; // 500KB

			const unsigned int bytesToSend = RandomI::random32() % maximalBytes + 1u;

			std::vector<uint8_t> buffer(bytesToSend);
			for (uint8_t& value : buffer)
			{
				value = uint8_t(RandomI::random(255u));
			}

			if (PlatformSDK::Network::get().sendToUserTCP(remoteUserId, PlatformSDK::Network::CI_CUSTOM, std::move(buffer)))
			{
				bytesSent_ += size_t(bytesToSend);
			}
			else
			{
				Log::error() << "Failed to send TCP data";
			}

			nextSendTimestamp_ = timestamp + 0.05;
		}
	}

	return timestamp;
}

std::unique_ptr<XRPlaygroundExperience> PlatformSDKNetworkExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new PlatformSDKNetworkExperience());
}

bool PlatformSDKNetworkExperience::onConnection(const uint64_t remoteUserId)
{
	const ScopedLock scopedLock(lock_);

	if (remoteUserId_ != 0ull && remoteUserId_ != remoteUserId)
	{
		Log::error() << "Received connection request from wrong user " << remoteUserId;
		return false;
	}

	Log::info() << "Accepted connection request from " << remoteUserId;

	vrTableMenu_.hide();

	remoteUserId_ = remoteUserId;
	return true;
}

void PlatformSDKNetworkExperience::onReceive(const uint64_t senderUserId, const Platform::Meta::Quest::PlatformSDK::Network::ComponentId componentId, const void* data, const size_t size, const Platform::Meta::Quest::PlatformSDK::Network::ConnectionType connectionType)
{
	const ScopedLock scopedLock(lock_);

	if (senderUserId != remoteUserId_)
	{
		Log::error() << "Received data from wrong user " << senderUserId;
	}

	bytesReceived_ += size;
}

#else // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

PlatformSDKNetworkExperience::~PlatformSDKNetworkExperience()
{
	// nothing to do here
}

std::unique_ptr<XRPlaygroundExperience> PlatformSDKNetworkExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new PlatformSDKNetworkExperience());
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

}
