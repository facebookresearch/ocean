// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/relocalization/GenericCloudRelocalizationExperience.h"

#include "ocean/base/StringApple.h"

namespace Ocean
{

namespace XRPlayground
{

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

void GenericCloudRelocalizationExperience::showReleaseEnvironmentsIOS(const Interaction::UserInterface& userInterface, const std::vector<std::pair<std::string, std::string>>& configurations)
{
	ocean_assert(userInterface.isIOS());

	UIAlertController* uiAlertController = [UIAlertController alertControllerWithTitle:@"Select Release Environment" message:@"Select the release environment to start cloud relocalization" preferredStyle:UIAlertControllerStyleAlert];

	for (const std::pair<std::string, std::string>& configuration : configurations)
	{
		NSString* nsString = StringApple::toNSString(configuration.first);

		const std::string jsonConfiguration(configuration.second);

		UIAlertAction* configurationAction = [UIAlertAction actionWithTitle:nsString style:UIAlertActionStyleDefault handler:^(UIAlertAction* action)
		{
			startRelocalization(jsonConfiguration);
		}];

		[uiAlertController addAction:configurationAction];
	}

	[userInterface.viewController() presentViewController:uiAlertController animated:YES completion:nil];
}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

}

}
