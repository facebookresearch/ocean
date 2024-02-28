// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/avatars/MetaAvatarsExperience.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/math/Random.h"

namespace Ocean
{

namespace XRPlayground
{

#ifdef OCEAN_PLATFORM_BUILD_MOBILE

using namespace Platform::Meta;

bool MetaAvatarsExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp /*timestamp*/, const std::string& /*properties*/)
{
	renderingTransformRemoteAvatars_ = engine->factory().createTransform();

	changedAvatarsScopedSubscription_ = Avatars::Manager::get().addChangedAvatarsCallback(std::bind(&MetaAvatarsExperience::onChangedAvatars, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	userId_ = Avatars::Manager::get().userId(); // id of local user

	if (userId_ != 0ull)
	{
		Avatars::Manager::AvatarScopedSubscription avatarScopedSubscription = Avatars::Manager::get().createAvatar(userId_, std::bind(&MetaAvatarsExperience::onAvatarCreated, this, std::placeholders::_1, std::placeholders::_2));
		ocean_assert(avatarScopedSubscription);

		if (avatarScopedSubscription)
		{
			avatarScopedSubscriptions_.emplace_back(std::move(avatarScopedSubscription));

			// placing each user around the world's origin

			RandomGenerator randomGenerator((unsigned int)(userId_));
			const Scalar randomAngle = Random::scalar(randomGenerator, Scalar(0), Numeric::pi2());

			const HomogenousMatrix4 remoteHeadsetWorld_T_remoteAvatar(HomogenousMatrix4(Quaternion(Vector3(0, 1, 0), randomAngle)) * HomogenousMatrix4(Vector3(0, 0, 1)));

			Avatars::Manager::get().setRemoteHeadsetWorld_T_remoteAvatar(remoteHeadsetWorld_T_remoteAvatar);

			return true;
		}
	}

	Log::error() << "Failed to determine local user";

	return false;
}

bool MetaAvatarsExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	avatarScopedSubscriptions_.clear();
	zoneScopedSubscription_.release();

	changedAvatarsScopedSubscription_.release();

	renderingTransformRemoteAvatars_.release();

	return true;
}

Timestamp MetaAvatarsExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(renderingTransformRemoteAvatars_);
	renderingTransformRemoteAvatars_->setTransformation(Avatars::Manager::get().remoteHeadsetWorld_T_remoteAvatar(userId_).inverted());

	return timestamp;
}

void MetaAvatarsExperience::onChangedAvatars(const std::string& zoneName, const Avatars::Manager::UserPairs& addedAvatars, const Indices64& removedAvatars)
{
	for (const Avatars::Manager::UserPair& userPair : addedAvatars)
	{
		const uint64_t userId = userPair.first;
		const Avatars::Manager::UserType userType = userPair.second;

		Avatars::Manager::AvatarScopedSubscription avatarScopedSubscription = Avatars::Manager::get().createAvatar(userId, std::bind(&MetaAvatarsExperience::onAvatarCreated, this, std::placeholders::_1, std::placeholders::_2), userType);

		if (avatarScopedSubscription)
		{
			avatarScopedSubscriptions_.emplace_back(std::move(avatarScopedSubscription));
		}
	}
}

#endif // #ifdef OCEAN_PLATFORM_BUILD_MOBILE

}

}
