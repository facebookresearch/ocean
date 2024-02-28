// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/PhoneCameraExperience.h"

#include "ocean/base/StringApple.h"

#include "ocean/platform/apple/Utilities.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

using namespace Ocean;

@interface PhoneCameraExperienceInteractionView : UIView <UITextFieldDelegate>
{
	std::vector<UIButton*> buttons_;

	/// The owner of this view.
	XRPlayground::PhoneCameraExperience* owner_;
}

@end

@implementation PhoneCameraExperienceInteractionView

- (id)initWithFrame:(CGRect)frame withOwner:(XRPlayground::PhoneCameraExperience*)owner withCameraNames:(const std::vector<std::string>&)cameraNames
{
	if (self = [super initWithFrame:frame])
	{
		constexpr float buttonHeight = 50;

		float y = frame.size.height * 0.1f;

		for (const std::string& cameraName : cameraNames)
		{
			UIButton* button = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, y, frame.size.width * 0.8f, buttonHeight)];
			[button addTarget:self action:@selector(onButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
			[button setTitle:StringApple::toNSString(cameraName) forState:UIControlStateNormal];
			button.titleLabel.font = [UIFont systemFontOfSize:20];
			button.backgroundColor = [UIColor whiteColor];
			[button setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
			[button setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
			button.titleLabel.textColor = [UIColor whiteColor];
			button.layer.borderWidth = 1.0f;
			button.layer.cornerRadius = 10.0f;
			button.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
			button.hidden = NO;
			[self addSubview:button];

			y += buttonHeight * 1.2f;
		}

		owner_ = owner;
	}

	return self;
}

- (void)onButtonClicked:(UIButton*)sender
{
	const std::string buttonTitle = StringApple::toUTF8(sender.currentTitle);

	owner_->switchCamera(buttonTitle);
}

@end

namespace Ocean
{

namespace XRPlayground
{

void PhoneCameraExperience::showUserInterinterfaceIOS(const Interaction::UserInterface& userInterface, const std::vector<std::string>& cameraNames)
{
	ocean_assert(userInterface.isIOS());

	UIViewController* viewController = userInterface.viewController();

	PhoneCameraExperienceInteractionView* view = [[PhoneCameraExperienceInteractionView alloc] initWithFrame:viewController.view.frame withOwner:this withCameraNames:cameraNames];
	[viewController.view addSubview:view];
}

void PhoneCameraExperience::unloadUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	UIViewController* viewController = userInterface.viewController();

	for (PhoneCameraExperienceInteractionView* interactionView in [viewController.view subviews])
	{
		ocean_assert(interactionView);

		[interactionView removeFromSuperview];
	}
}

}

}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
