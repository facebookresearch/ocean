// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/network/tigon/TigonClient.h"

#include "ocean/platform/meta/Login.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	#import <FBNetworker/FBHttpExecutor+Tigon.h>
	#import <FBSessionNetworker/FBSessionNetworker.h>
#endif

namespace Ocean
{

namespace Network
{

namespace Tigon
{

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

std::shared_ptr<facebook::tigon::TigonService> TigonClient::createTigonServiceAppleIOS()
{
	FBUserSession* userSession = Platform::Meta::Login::get().userSession(Platform::Meta::Login::LT_FACEBOOK);

	if (userSession == nullptr)
	{
		Log::error() << "Missing user session";
		return nullptr;
	}

	return [FBHttpUpperStackWithSession(userSession) service];
}

#endif // OCEAN_PLATFORM_BUILD_APPLE

}

}

}
