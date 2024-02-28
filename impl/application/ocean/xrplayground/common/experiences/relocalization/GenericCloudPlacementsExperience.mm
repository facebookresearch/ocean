// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/relocalization/GenericCloudPlacementsExperience.h"

#include "ocean/base/StringApple.h"

namespace Ocean
{

namespace XRPlayground
{

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

void GenericCloudPlacementsExperience::showConfigurationsIOS(const Interaction::UserInterface& userInterface, const std::vector<std::pair<std::string, std::string>>& configurations)
{
	ocean_assert(userInterface.isIOS());

	UIAlertController* uiAlertController = [UIAlertController alertControllerWithTitle:@"Select Configuration" message:@"Select the configuration to start the experience" preferredStyle:UIAlertControllerStyleAlert];

	for (std::pair<std::string, std::string> configuration : configurations)
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
