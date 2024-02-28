// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/crossplatformhangout/CrossPlatformHangoutExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/base/RandomGenerator.h"
#include "ocean/base/RandomI.h"

#include "ocean/math/HSVAColor.h"
#include "ocean/math/Random.h"

#include "ocean/rendering/Utilities.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/platformsdk/Multiplayer.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef OCEAN_PLATFORM_BUILD_MOBILE

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	using namespace Platform::Meta::Quest;
	using namespace Platform::Meta::Quest::VrApi::Application;
#endif

CrossPlatformHangoutExperience::~CrossPlatformHangoutExperience()
{
	// nothing to do here
}

bool CrossPlatformHangoutExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties)
{
	if (!XRPlaygroundSharedSpaceExperience::load(userInterface, engine, timestamp, properties))
	{
		return false;
	}

	Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false, 0, 0, Scalar(0.025), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingText_);
	textTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, Scalar(-0.5))));
	experienceScene()->addChild(std::move(textTransform));

	textTransform = Rendering::Utilities::createText(*engine, "", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false, 0, 0, Scalar(0.025), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingTextUserMovement_);
	textTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, Scalar(-0.5))));
	experienceScene()->addChild(std::move(textTransform));

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

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
			if (joinIntent.destinationApiName_ == "xrplayground_cross_platform_hangout" && !joinIntent.lobbySessionId_.empty())
			{
				sessionId_ = joinIntent.lobbySessionId_;
				applicationState_ = AS_JOINING_PRIVATE_ZONE;
			}
		}
	}

	vrTableMenu_ = VRTableMenu(engine, engine->framebuffers().front());

	setUserMovementEnabled(true);

#else

	// on mobile, we always join the public zone
	applicationState_ = AS_JOINING_PUBLIC_ZONE;

	renderingText_->setText("\n              Move your phone to             \n              determine floor level,             \n\n\n swipe from top to \n toggle AR/VR \n");

#endif

	Network::Verts::NodeSpecification& contentNodeSpecification = Network::Verts::NodeSpecification::newNodeSpecification("Content");
	contentNodeSpecification.registerField<std::string>("url");

	return true;
}

bool CrossPlatformHangoutExperience::unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	vrTableMenu_.release();

	scopedGroupPresenceFuture_ = std::future<PlatformSDK::Multiplayer::ScopedGroupPresence>();
	scopedGroupPresence_.release();
#else

	if (!engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			view->background()->setVisible(true);
		}
	}

#endif

	newEntityScopedSubscription_.release();

	renderingText_.release();
	renderingTextUserMovement_.release();

	return XRPlaygroundSharedSpaceExperience::unload(userInterface, engine, timestamp);
}

Timestamp CrossPlatformHangoutExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	const Timestamp updateTimestamp = XRPlaygroundSharedSpaceExperience::preUpdate(userInterface, engine, view, timestamp);

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	if (applicationState_ == AS_IDLE)
	{
		VRTableMenu::Entries menuEntries =
		{
			VRTableMenu::Entry("Invite someone to this room", "INVITE_SOMEONE"),
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

			if (entryUrl == "INVITE_SOMEONE")
			{
				ocean_assert(sessionId_.empty());
				sessionId_ = String::toAString(RandomI::random64());

				scopedGroupPresenceFuture_ = PlatformSDK::Multiplayer::get().setActiveGroupPresence("xrplayground_cross_platform_hangout", sessionId_);

				if (scopedGroupPresenceFuture_.valid())
				{
					applicationState_ = AS_INVITING_SOMEONE;
				}
				else
				{
					Log::error() << "Failed to set active group presence";

					applicationState_ = AS_IDLE;
				}
			}
			else if (entryUrl == "PUBLIC_ZONE")
			{
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

					if (initializeNetworkZone(zoneName))
					{
						Log::info() << "Initialized private network zone with name '" << zoneName << "'";
					}
					else
					{
						Log::error() << "Failed to initialize private network zone";
					}

					applicationState_ = AS_IN_SHARED_SPACE;
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

		scopedGroupPresenceFuture_ = PlatformSDK::Multiplayer::get().setActiveGroupPresence("xrplayground_cross_platform_hangout", sessionId_);

		const std::string zoneName = sessionIdZoneName(sessionId_);

		if (initializeNetworkZone(zoneName))
		{
			Log::info() << "Joined private network zone with name '" << zoneName << "'";

			switchContent("DEFAULT", engine);
		}
		else
		{
			Log::error() << "Failed to initialize private network zone";
		}

		applicationState_ = AS_IN_SHARED_SPACE;
	}
	else
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	{
		if (applicationState_ == AS_JOINING_PUBLIC_ZONE)
		{
			if (initializeNetworkZone(vertsPublicZoneName_))
			{
				Log::info() << "Initialized public network zone with name '" << vertsPublicZoneName_ << "'";

				switchContent("DEFAULT", engine);
			}
			else
			{
				Log::error() << "Failed to initialize public network zone";
			}

			applicationState_ = AS_IN_SHARED_SPACE;
		}
		else if (applicationState_ == AS_IN_SHARED_SPACE)
		{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

			if (showContentMenu_)
			{
				showContentMenu_ = false;

				VRTableMenu::Entries menuEntries =
				{
					VRTableMenu::Entry("Default hangout environment", "DEFAULT"),
					VRTableMenu::Entry("Dexter 2nd floor (needs several seconds)", "assetId:421646863325271")
				};

				const VRTableMenu::Group menuGroup("To which environment to you want to switch?", std::move(menuEntries));

				vrTableMenu_.setMenuEntries(menuGroup);
				vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1)), view->transformation());
			}

			std::string entryUrl;
			if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
			{
				vrTableMenu_.hide();

				if (!vertsContentNode_)
				{
					Network::Verts::SharedEntity entity = vertsDriver_->newEntity({"Content"});

					if (entity)
					{
						vertsContentNode_ = entity->node("Content");
					}
				}

				if (vertsContentNode_)
				{
					vertsContentNode_->setField<std::string>("url", entryUrl);
				}
			}

#endif
		}
	}

#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	if (toggleMode_)
	{
		toggleMode_ = false;

		if (view && view->background())
		{
			view->background()->setVisible(!view->background()->visible());
		}
	}
#endif

	if (hasValidFloor())
	{
		renderingText_->setText("");
	}

	if (!contentPair_.first.empty())
	{
		if (timestamp >= contentPair_.second)
		{
			switchContent(contentPair_.first, engine);

			contentPair_ = ContentPair();
		}
	}
	else
	{
		if (vertsContentNode_ && vertsContentNode_->hasChanged())
		{
			std::string url = vertsContentNode_->field<std::string>("url");

			if (!url.empty())
			{
				Log::info() << "Received new content url '" << url << "'";

				contentPair_ = ContentPair(std::move(url), timestamp + 0.5);
			}
		}
	}

	return updateTimestamp;
}

void CrossPlatformHangoutExperience::onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp)
{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	if (key == "A")
	{
		if (applicationState_ == AS_IN_SHARED_SPACE)
		{
			if (vrTableMenu_.isShown())
			{
				vrTableMenu_.hide();
			}
			else
			{
				showContentMenu_ = true;
			}
		}
	}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
}

void CrossPlatformHangoutExperience::onMousePress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& screenPosition, const Line3& /*ray*/, const Timestamp /*timestamp*/)
{
	screenPositionPressed_ = screenPosition;
}

void CrossPlatformHangoutExperience::onMouseRelease(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& screenPosition, const Line3& /*ray*/, const Timestamp /*timestamp*/)
{
#ifndef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	if (hasValidFloor())
	{
		if (screenPosition.sqrDistance(screenPositionPressed_) < Numeric::sqr(10))
		{
			setUserMovementEnabled(!isUserMovementEnabled());

			if (isUserMovementEnabled())
			{
				renderingTextUserMovement_->setText("\n             Free movement active             \n");
			}
			else
			{
				renderingTextUserMovement_->setText("");
			}
		}
		else if (Numeric::sqr(screenPositionPressed_.x() - screenPosition.x()) < Numeric::sqr(100) && screenPosition.y() - screenPositionPressed_.y() > 800)
		{
			// the user swiped from top to bottom

			toggleMode_ = true;
		}
	}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	screenPositionPressed_ = Vector2(Numeric::minValue(), Numeric::minValue());
}

std::unique_ptr<XRPlaygroundExperience> CrossPlatformHangoutExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new CrossPlatformHangoutExperience());
}

void CrossPlatformHangoutExperience::onInitializeLocalUser(HomogenousMatrix4& remoteHeadsetWorld_T_remoteAvatar)
{
	// placing each user around the world's origin

	RandomGenerator randomGenerator((unsigned int)(localUserId_));
	const Scalar randomAngle = Random::scalar(randomGenerator, Scalar(0), Numeric::pi2());

	remoteHeadsetWorld_T_remoteAvatar = HomogenousMatrix4(HomogenousMatrix4(Quaternion(Vector3(0, 1, 0), randomAngle)) * HomogenousMatrix4(Vector3(0, 0, 1)));
}

bool CrossPlatformHangoutExperience::initializeNetworkZone(const std::string& zoneName)
{
	if (!XRPlaygroundSharedSpaceExperience::initializeNetworkZone(zoneName))
	{
		return false;
	}

	ocean_assert(vertsDriver_);

	newEntityScopedSubscription_ = vertsDriver_->addNewEntityCallback(std::bind(&CrossPlatformHangoutExperience::onNewEntity, this, std::placeholders::_1, std::placeholders::_2), "Content");

	return true;
}

void CrossPlatformHangoutExperience::switchContent(const std::string& content, const Rendering::EngineRef& engine)
{
	ocean_assert(!content.empty() && engine);

	if (content == "DEFAULT")
	{
		if (!loadContent(createDefaultEnvironment(engine)))
		{
			Log::error() << "Failed to load default environment";
		}
	}
	else
	{
		if (!loadContent(content))
		{
			Log::error() << "Failed to load content '" << content << "'";
		}
	}
}

void CrossPlatformHangoutExperience::onNewEntity(Network::Verts::Driver& driver, const Network::Verts::SharedEntity& entity)
{
	vertsContentNode_ = entity->node("Content");
	ocean_assert(vertsContentNode_);
}

std::string CrossPlatformHangoutExperience::sessionIdZoneName(const std::string& sessionId)
{
	return "XRPlayground://CROSS_PLATFORM_HANGOUT_EXPERIENCE_SESSION_ID_" + sessionId;
}

Rendering::TransformRef CrossPlatformHangoutExperience::createDefaultEnvironment(const Rendering::EngineRef& engine)
{
	ocean_assert(engine);

	Rendering::TransformRef environment = engine->factory().createTransform();

	environment->addChild(Rendering::Utilities::createCoordinateSystem(engine, Scalar(1), Scalar(0.05), Scalar(0.025)));

	constexpr int gridSize_2 = 3;
	constexpr int gridSize = gridSize_2 * 2 + 1;

	constexpr Scalar invGridSize = Scalar(1) / Scalar(gridSize - 1);

	const RGBAColor rgbaColor00(1.0f, 0.0f, 0.0f);
	const RGBAColor rgbaColor01(0.0f, 1.0f, 0.0f);
	const RGBAColor rgbaColor10(0.0f, 0.0f, 1.0f);
	const RGBAColor rgbaColor11(1.0f, 1.0f, 1.0f);

	const HSVAColor hsvaColor00(rgbaColor00);
	const HSVAColor hsvaColor01(rgbaColor01);
	const HSVAColor hsvaColor10(rgbaColor10);
	const HSVAColor hsvaColor11(rgbaColor11);

	for (int x = -gridSize_2; x <= gridSize_2; ++x)
	{
		const Scalar xFactor = Scalar(x + gridSize_2) * invGridSize;

		const HSVAColor xHSVAColor0(hsvaColor10.interpolate(hsvaColor00, float(xFactor)));
		const HSVAColor xHSVAColor1(hsvaColor11.interpolate(hsvaColor01, float(xFactor)));

		for (int z = -gridSize_2; z <= gridSize_2; ++z)
		{
			const Scalar zFactor = float(z + gridSize_2) * invGridSize;

			const HSVAColor hsvaColor(xHSVAColor1.interpolate(xHSVAColor0, float(zFactor)));

			Rendering::TransformRef tiles = Rendering::Utilities::createBox(engine, Vector3(1, 0, 1), RGBAColor(hsvaColor));
			tiles->setTransformation(HomogenousMatrix4(Vector3(Scalar(x * 2), 0, Scalar(z * 2))));

			environment->addChild(std::move(tiles));
		}
	}

	Rendering::TransformRef box = Rendering::Utilities::createBox(engine, Vector3(1, 1, 1), rgbaColor00);
	box->setTransformation(HomogenousMatrix4(Vector3(Scalar(gridSize_2 * 2), 1, Scalar(gridSize_2 * 2))));
	environment->addChild(std::move(box));

	Rendering::TransformRef sphere = Rendering::Utilities::createSphere(engine, Scalar(0.5), rgbaColor01);
	sphere->setTransformation(HomogenousMatrix4(Vector3(Scalar(gridSize_2 * 2), 1, Scalar(-gridSize_2 * 2))));
	environment->addChild(std::move(sphere));

	Rendering::TransformRef cylinder = Rendering::Utilities::createCylinder(engine, Scalar(0.5), Scalar(1), rgbaColor10);
	cylinder->setTransformation(HomogenousMatrix4(Vector3(Scalar(-gridSize_2 * 2), 1, Scalar(gridSize_2 * 2))));
	environment->addChild(std::move(cylinder));

	Rendering::TransformRef cone = Rendering::Utilities::createCone(engine, Scalar(0.5), Scalar(1), rgbaColor11);
	cone->setTransformation(HomogenousMatrix4(Vector3(Scalar(-gridSize_2 * 2), 1, Scalar(-gridSize_2 * 2))));
	environment->addChild(std::move(cone));

	constexpr Scalar environmentScale = Scalar(0.5);

	environment->setTransformation(HomogenousMatrix4(Vector3(0, 0, 0), Vector3(1, 1, 1) * environmentScale));

	return environment;
}

#else // OCEAN_PLATFORM_BUILD_MOBILE

CrossPlatformHangoutExperience::~CrossPlatformHangoutExperience()
{
	// nothing to do here
}

std::unique_ptr<XRPlaygroundExperience> CrossPlatformHangoutExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new CrossPlatformHangoutExperience());
}

#endif // OCEAN_PLATFORM_BUILD_MOBILE

}

}
