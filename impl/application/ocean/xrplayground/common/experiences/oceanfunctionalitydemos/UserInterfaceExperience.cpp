// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/UserInterfaceExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/rendering/Utilities.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID
	#include "ocean/platform/android/Utilities.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

UserInterfaceExperience::~UserInterfaceExperience()
{
	// nothing to do here
}

bool UserInterfaceExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	instance() = this;

	menuShowTimestamp_ = timestamp + 1.0; // showing the menu in 1 seconds

	return true;
}

bool UserInterfaceExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	instance() = nullptr;

	return true;
}

Timestamp UserInterfaceExperience::preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	TemporaryScopedLock scopedLock(lock_);

		if (!recentSelection_.empty())
		{
			const RGBAColor color = recentSelection_ == "A" ? RGBAColor(1.0f, 0.0f, 0.0f) : RGBAColor(0.0f, 1.0f, 0.0f);

			absoluteTransform_->addChild(Rendering::Utilities::createText(*engine, " You selected " + recentSelection_ + " ", color, RGBAColor(0.0f, 0.0f, 0.0f), false, Scalar(0.5), 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE));

			recentSelection_.clear();
		}

	scopedLock.release();

	if (menuShowTimestamp_.isValid() && timestamp >= menuShowTimestamp_)
	{
		menuShowTimestamp_.toInvalid();

		absoluteTransform_ = engine->factory().createAbsoluteTransform();
		absoluteTransform_->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
		absoluteTransform_->setTransformation(HomogenousMatrix4(Vector3(0, 0, -1)));

		experienceScene()->addChild(absoluteTransform_);

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
		if (userInterface.isIOS())
		{
			showMenuIOS(userInterface);
			return timestamp;
		}
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID
		if (PlatformSpecific::get().startActivity("UserInterfaceExperienceActivity"))
		{
			return timestamp;
		}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID

		absoluteTransform_->addChild(Rendering::Utilities::createText(*engine, " No Menu supported ", RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), false, Scalar(0.5), 0, 0, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE));
	}

	return timestamp;
}

std::unique_ptr<XRPlaygroundExperience> UserInterfaceExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new UserInterfaceExperience());
}

void UserInterfaceExperience::onSelection(const std::string& selection)
{
	UserInterfaceExperience* userInterfaceExperience = instance();

	ocean_assert(userInterfaceExperience != nullptr);
	if (userInterfaceExperience != nullptr)
	{
		const ScopedLock scopedLock(userInterfaceExperience->lock_);

		userInterfaceExperience->recentSelection_ = selection;
	}
}

UserInterfaceExperience*& UserInterfaceExperience::instance()
{
	static UserInterfaceExperience* userInterfaceExperience = nullptr;
	return userInterfaceExperience;
}

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID

void Java_com_facebook_ocean_app_xrplayground_android_UserInterfaceExperienceActivity_onSelection(JNIEnv* env, jobject javaThis, jstring selection)
{
	const std::string cSelection = Platform::Android::Utilities::toAString(env, selection);

	UserInterfaceExperience::onSelection(cSelection);
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_PHONE_ANDROID

}

}
