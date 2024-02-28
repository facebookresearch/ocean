// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/UserInterfaceExperience.h"

#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

void UserInterfaceExperience::showMenuIOS(const Interaction::UserInterface& userInterface)
{
	ocean_assert(userInterface.isIOS());

	UIAlertController* uiAlertController = [UIAlertController alertControllerWithTitle:@"Menu" message:@"Please make a selection" preferredStyle:UIAlertControllerStyleAlert];

	[uiAlertController addAction:[UIAlertAction actionWithTitle:@"Selection A" style:UIAlertActionStyleDefault handler:^(UIAlertAction* action) {
		UserInterfaceExperience::onSelection("A");
	}]];
	[uiAlertController addAction:[UIAlertAction actionWithTitle:@"Selection B" style:UIAlertActionStyleDefault handler:^(UIAlertAction* action) {
		UserInterfaceExperience::onSelection("B");
	}]];

	[userInterface.viewController() presentViewController:uiAlertController animated:YES completion:nil];
}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

}

}
