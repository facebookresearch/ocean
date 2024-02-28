// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/avatars/MetaAvatarsQuestExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/RandomI.h"

#include "ocean/devices/Manager.h"

#include "ocean/media/Manager.h"

#include "ocean/network/verts/Manager.h"

#include "ocean/rendering/Utilities.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/platformsdk/Multiplayer.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

using namespace Platform::Meta;
using namespace Platform::Meta::Quest;
using namespace Platform::Meta::Quest::VrApi::Application;

MetaAvatarsQuestExperience::~MetaAvatarsQuestExperience()
{
	// nothing to do here
}

bool MetaAvatarsQuestExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties)
{
	if (!MetaAvatarsExperience::load(userInterface, engine, timestamp, properties))
	{
		return false;
	}

	ocean_assert(renderingTransformRemoteAvatars_);
	experienceScene()->addChild(renderingTransformRemoteAvatars_);

	vrTableMenu_ = VRTableMenu(engine, engine->framebuffers().front());

	if (properties == "guest")
	{
		applicationState_ = AS_JOINING_PUBLIC_ZONE;
	}
	else
	{
		// let's check whether the experience has been started due to a join intent with lobby session id to a private zone

		PlatformSDK::Multiplayer::JoinIntent joinIntent;
		if (PlatformSDK::Multiplayer::get().hasLatestJointIntent(joinIntent))
		{
			if (joinIntent.destinationApiName_ == "xrplayground_meta_avatars" && !joinIntent.lobbySessionId_.empty())
			{
				sessionId_ = joinIntent.lobbySessionId_;
				applicationState_ = AS_JOINING_PRIVATE_ZONE;
			}
		}
	}

	floorTracker6DOF_ = Devices::Manager::get().device("Floor 6DOF Tracker");

	if (floorTracker6DOF_)
	{
		floorTracker6DOF_->start();
	}

	return true;
}

bool MetaAvatarsQuestExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	scopedGroupPresenceFuture_ = std::future<PlatformSDK::Multiplayer::ScopedGroupPresence>();
	scopedGroupPresence_.release();

	floorTracker6DOF_.release();

	vrTableMenu_.release();

	return MetaAvatarsExperience::unload(userInterface, engine, timestamp);
}

Timestamp MetaAvatarsQuestExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	MetaAvatarsExperience::preUpdate(userInterface, engine, view, timestamp);

	if (applicationState_ == AS_IDLE)
	{
		VRTableMenu::Entries menuEntries =
		{
			VRTableMenu::Entry("Just stay in the session alone", "STAY_ALONE"),
			VRTableMenu::Entry("Make a clone of me", "MAKE_CLONE"),
			VRTableMenu::Entry("Invite someone to this session", "INVITE_SOMEONE"),
			VRTableMenu::Entry("Wait until someone invites me", "WAIT_FOR_INVITE"),
			VRTableMenu::Entry("Join public zone", "PUBLIC_ZONE")
		};

		const VRTableMenu::Group menuGroup("What do you want to do", std::move(menuEntries));

		vrTableMenu_.setMenuEntries(menuGroup);
		vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1)), view->transformation());

		applicationState_ = AS_USER_SELECTING_MODE;
	}
	else if (applicationState_ == AS_USER_SELECTING_MODE)
	{
		ocean_assert(vrTableMenu_.isShown());

		std::string entryUrl;
		if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
		{
			vrTableMenu_.hide();

			if (entryUrl == "STAY_ALONE")
			{
				applicationState_ = AS_STAYING_ALONE;
			}
			else if (entryUrl == "MAKE_CLONE")
			{
				Avatars::Manager::AvatarScopedSubscription avatarScopedSubscription = Avatars::Manager::get().createAvatar(userId_, std::bind(&MetaAvatarsQuestExperience::onAvatarCreated, this, std::placeholders::_1, std::placeholders::_2));

				if (avatarScopedSubscription)
				{
					avatarScopedSubscriptions_.emplace_back(std::move(avatarScopedSubscription));

					applicationState_ = AS_MAKE_CLONE;
				}
			}
			else if (entryUrl == "INVITE_SOMEONE")
			{
				ocean_assert(sessionId_.empty());
				sessionId_ = String::toAString(RandomI::random64());

				Log::info() << "Creating new group presence with id '" << sessionId_ << "' ...";

				scopedGroupPresenceFuture_ = PlatformSDK::Multiplayer::get().setActiveGroupPresence("xrplayground_meta_avatars", sessionId_);

				applicationState_ = AS_INVITING_SOMEONE;
			}
			else
			{
				ocean_assert(entryUrl == "PUBLIC_ZONE");

				applicationState_ = AS_JOINING_PUBLIC_ZONE;
			}
		}
	}
	else if (applicationState_ == AS_INVITING_SOMEONE)
	{
		if (scopedGroupPresenceFuture_.valid())
		{
			if (scopedGroupPresenceFuture_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				scopedGroupPresence_ = scopedGroupPresenceFuture_.get();
				ocean_assert(!scopedGroupPresenceFuture_.valid());

				sentInvitesFuture_ = PlatformSDK::Multiplayer::get().launchInvitePanel();

				if (!sentInvitesFuture_.valid())
				{
					Log::error() << "Failed to launch invite panel!";

					applicationState_ = AS_IDLE;
				}
			}
		}

		if (sentInvitesFuture_.valid())
		{
			if (sentInvitesFuture_.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
			{
				const bool atLeastOneInviteSent = sentInvitesFuture_.get();
				ocean_assert(!sentInvitesFuture_.valid());

				if (atLeastOneInviteSent)
				{
					const std::string zoneName = sessionIdZoneName(sessionId_);

					zoneScopedSubscription_ = Avatars::Manager::get().joinZone(zoneName);

					scopedGroupPresenceFuture_ = std::future<PlatformSDK::Multiplayer::ScopedGroupPresence>();

					applicationState_ = AS_JOINED_ZONE;
				}
				else
				{
					// we start over again

					scopedGroupPresence_.release();
					sessionId_.clear();

					applicationState_ = AS_IDLE;
				}
			}
		}
	}
	else if (applicationState_ == AS_JOINING_PRIVATE_ZONE)
	{
		ocean_assert(!sessionId_.empty());

		scopedGroupPresenceFuture_ = PlatformSDK::Multiplayer::get().setActiveGroupPresence("xrplayground_meta_avatars", sessionId_);

		const std::string zoneName = sessionIdZoneName(sessionId_);

		zoneScopedSubscription_ = Avatars::Manager::get().joinZone(zoneName);

		applicationState_ = AS_JOINED_ZONE;
	}
	else if (applicationState_ == AS_JOINING_PUBLIC_ZONE)
	{
		zoneScopedSubscription_ = Avatars::Manager::get().joinZone(vertsPublicZoneName_);

		applicationState_ = AS_JOINED_ZONE;
	}
	else if (applicationState_ == AS_JOINED_ZONE)
	{
		// nothing to do here
	}

	if (yFloorElevation_ == Numeric::minValue())
	{
		const Devices::Tracker6DOF::Tracker6DOFSampleRef sample(floorTracker6DOF_->sample());

		if (sample && !sample->objectIds().empty())
		{
			yFloorElevation_ = sample->positions().front().y();

			Avatars::Manager::get().setGroundPlaneElevation(yFloorElevation_);

			floorTracker6DOF_.release(); // don't need the tracker anymore as floor level will not change
		}
	}

	return timestamp;
}

std::unique_ptr<XRPlaygroundExperience> MetaAvatarsQuestExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new MetaAvatarsQuestExperience());
}

void MetaAvatarsQuestExperience::onAvatarCreated(const uint64_t userId, Rendering::TransformRef avatarTransform)
{
	if (avatarTransform)
	{
		if (userId == userId_)
		{
			if (applicationState_ == AS_MAKE_CLONE)
			{
				avatarTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -1.5), Quaternion(Vector3(0, 1, 0), Numeric::pi())));
			}

			PlatformSpecific::get().vrHandVisualizer().hide();
			PlatformSpecific::get().vrControllerVisualizer().hide();

			experienceScene()->addChild(avatarTransform);
		}
		else
		{
			renderingTransformRemoteAvatars_->addChild(avatarTransform);
		}
	}
	else
	{
		Log::error() << "Failed to create rendering instance for user " << userId;
	}
}

std::string MetaAvatarsQuestExperience::sessionIdZoneName(const std::string& sessionId)
{
	return "XRPlayground://META_AVATARS_EXPERIENCE_SESSION_ID_" + sessionId;
}

#else // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

MetaAvatarsQuestExperience::~MetaAvatarsQuestExperience()
{
	// nothing to do here
}

std::unique_ptr<XRPlaygroundExperience> MetaAvatarsQuestExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new MetaAvatarsQuestExperience());
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

}

}
