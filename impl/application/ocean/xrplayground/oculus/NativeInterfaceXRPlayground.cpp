// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

// clang-format off
// @nolint

#include "application/ocean/xrplayground/oculus/NativeInterfaceXRPlayground.h"
#include "application/ocean/xrplayground/oculus/XRPlaygroundApplication.h"

#include "ocean/platform/android/Utilities.h"

#include "metaonly/ocean/platform/meta/Login.h"

namespace Ocean
{

namespace XRPlayground
{

namespace Oculus
{

jboolean Java_com_facebook_ocean_app_xrplayground_oculus_NativeInterfaceXRPlayground_setUserToken(JNIEnv* env, jobject javaThis, jstring accountType, jstring userId, jstring userToken)
{
	std::string cAccountType(Platform::Android::Utilities::toAString(env, accountType));
	std::string cUserId(Platform::Android::Utilities::toAString(env, userId));
	std::string cUserToken(Platform::Android::Utilities::toAString(env, userToken));

	Platform::Meta::Login::LoginType loginType = Platform::Meta::Login::LT_UNKNOWN;

	if (cAccountType == "com.facebook" || cAccountType == "com.facebook.sso")
	{
		loginType = Platform::Meta::Login::LT_FACEBOOK;
	}
	else if (cAccountType == "com.meta")
	{
		loginType = Platform::Meta::Login::LT_META;
	}
	else if (cAccountType == "com.oculus")
	{
		loginType = Platform::Meta::Login::LT_OCULUS;
	}
	else
	{
		Log::error() << "Invalid account type '" << cAccountType << "'";
		return false;
	}

	if (cUserId.empty() || cUserToken.empty())
	{
		return false;
	}

	Log::debug() << "Updated user and/or access token for account type '" << cAccountType << ": " << cUserId << ", with token '" << cUserToken.substr(0, 7) << "...'";

	Platform::Meta::Login::get().setLogin(loginType, std::move(cUserId), std::move(cUserToken));

	return true;
}

jboolean Java_com_facebook_ocean_app_xrplayground_oculus_NativeInterfaceXRPlayground_setLaunchExperience(JNIEnv* env, jobject javaThis, jstring experience)
{
	std::string cExperience(Platform::Android::Utilities::toAString(env, experience));

	if (cExperience.empty())
	{
		return false;
	}

	XRPlaygroundApplication::setLaunchExperience(std::move(cExperience));

	return true;
}

}

}

}
