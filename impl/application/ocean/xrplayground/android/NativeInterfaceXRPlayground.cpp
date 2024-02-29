// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

// clang-format off
// @nolint

#include "application/ocean/xrplayground/android/NativeInterfaceXRPlayground.h"
#include "application/ocean/xrplayground/android/XRPlaygroundNativeMainView.h"

#include "application/ocean/xrplayground/common/experiences/ExperiencesManager.h"

#include "ocean/platform/android/Utilities.h"

#include "metaonly/ocean/platform/meta/Login.h"

namespace Ocean
{

namespace XRPlayground
{

namespace Android
{

jboolean Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_setUserToken(JNIEnv* env, jobject javaThis, jstring accountType, jstring userId, jstring userToken)
{
	std::string cAccountType(Platform::Android::Utilities::toAString(env, accountType));
	std::string cUserId(Platform::Android::Utilities::toAString(env, userId));
	std::string cUserToken(Platform::Android::Utilities::toAString(env, userToken));

	Platform::Meta::Login::LoginType loginType = Platform::Meta::Login::LT_UNKNOWN;

	if (cAccountType == "com.facebook")
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

	Log::info() << "Updated user and/or access token for account type '" << cAccountType << "': " << cUserId << ", with token '" << cUserToken.substr(0, 7) << "...'";

	Platform::Meta::Login::get().setLogin(loginType, std::move(cUserId), std::move(cUserToken));

	return true;
}

jboolean Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_loadContent(JNIEnv* env, jobject javaThis, jstring filename, jboolean replace)
{
	const std::string cFilename(Platform::Android::Utilities::toAString(env, filename));

	if (cFilename.empty())
	{
		return XRPlaygroundNativeMainView::get<XRPlaygroundNativeMainView>().unloadContent();
	}
	else
	{
		return XRPlaygroundNativeMainView::get<XRPlaygroundNativeMainView>().loadContent(cFilename, replace);
	}
}

jboolean Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_startVRSRecording(JNIEnv* env, jobject javaThis)
{
	return XRPlaygroundNativeMainView::get<XRPlaygroundNativeMainView>().startVRSRecording();
}

jboolean Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_stopVRSRecording(JNIEnv* env, jobject javaThis)
{
	return XRPlaygroundNativeMainView::get<XRPlaygroundNativeMainView>().stopVRSRecording();
}

jobjectArray Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_experienceGroups(JNIEnv* env, jobject javaThis)
{
	const ExperiencesManager::SelectableExperienceGroups selectableExperienceGroups(ExperiencesManager::selectableExperienceGroups(ExperiencesManager::PT_PHONE_ANDROID));

	std::vector<std::string> names;
	names.reserve(selectableExperienceGroups.size());

	for (size_t n = 0; n < selectableExperienceGroups.size(); ++n)
	{
		names.emplace_back(selectableExperienceGroups[n].first);
	}

	return Platform::Android::Utilities::toJavaStringArray(env, names);
}

jobjectArray Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_experienceNamesInGroup(JNIEnv* env, jobject javaThis, jint groupIndex)
{
	const ExperiencesManager::SelectableExperienceGroups selectableExperienceGroups(ExperiencesManager::selectableExperienceGroups(ExperiencesManager::PT_PHONE_ANDROID));

	std::vector<std::string> names;

	if (size_t(groupIndex) < selectableExperienceGroups.size())
	{
		const ExperiencesManager::SelectableExperienceGroup& selectableExperienceGroup = selectableExperienceGroups[groupIndex];

		names.reserve(selectableExperienceGroup.second.size());

		for (const ExperiencesManager::SelectableExperience& selectableExperience : selectableExperienceGroup.second)
		{
			names.emplace_back(selectableExperience.name());
		}
	}
	else
	{
		ocean_assert(false && "Experience group index outside of range");
	}

	return Platform::Android::Utilities::toJavaStringArray(env, names);
}

jobjectArray Java_com_facebook_ocean_app_xrplayground_android_NativeInterfaceXRPlayground_experienceURLsInGroup(JNIEnv* env, jobject javaThis, jint groupIndex)
{
	const ExperiencesManager::SelectableExperienceGroups selectableExperienceGroups(ExperiencesManager::selectableExperienceGroups(ExperiencesManager::PT_PHONE_ANDROID));

	std::vector<std::string> urls;

	if (size_t(groupIndex) < selectableExperienceGroups.size())
	{
		const ExperiencesManager::SelectableExperienceGroup& selectableExperienceGroup = selectableExperienceGroups[groupIndex];

		urls.reserve(selectableExperienceGroup.second.size());

		for (const ExperiencesManager::SelectableExperience& selectableExperience : selectableExperienceGroup.second)
		{
			urls.emplace_back(selectableExperience.url());
		}
	}
	else
	{
		ocean_assert(false && "Experience group index outside of range");
	}

	return Platform::Android::Utilities::toJavaStringArray(env, urls);
}

}

}

}
