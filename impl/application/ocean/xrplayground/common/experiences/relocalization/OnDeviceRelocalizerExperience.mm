// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/relocalization/OnDeviceRelocalizerExperience.h"

#include "ocean/base/StringApple.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/platform/apple/Utilities.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

using namespace Ocean;

@interface OnDeviceRelocalizerExperienceInteractionView : UIView <UITextFieldDelegate>
{
	/// The start button.
	UIButton* buttonStart_;

	/// The stop button.
	UIButton* buttonStop_;

	/// The text field for the filename
	UITextField* textFieldFilename_;

	/// The currently defined filename
	std::string filename_;

	/// The owner of this view.
	XRPlayground::OnDeviceRelocalizerExperience* owner_;
}

@end

@implementation OnDeviceRelocalizerExperienceInteractionView

- (id)initWithFrame:(CGRect)frame withOwner:(XRPlayground::OnDeviceRelocalizerExperience*)owner
{
	if (self = [super initWithFrame:frame])
	{
		constexpr float textFieldHeight = 40;
		constexpr float buttonHeight = 50;

		buttonStart_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.25f, frame.size.height * 0.20f, frame.size.width * 0.5f, buttonHeight)];
		[buttonStart_ addTarget:self action:@selector(onButtonClickedStart:) forControlEvents:UIControlEventTouchUpInside];
		[buttonStart_ setTitle:@"Start" forState:UIControlStateNormal];
		buttonStart_.titleLabel.font = [UIFont systemFontOfSize:buttonHeight - 10];
		buttonStart_.titleLabel.font = [UIFont systemFontOfSize:40];
		buttonStart_.backgroundColor = [UIColor whiteColor];
		[buttonStart_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		buttonStart_.layer.borderWidth = 1.0f;
		buttonStart_.layer.cornerRadius = 10.0f;
		buttonStart_.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
		buttonStart_.hidden = NO;
		[self addSubview:buttonStart_];

		buttonStop_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.25f, frame.size.height * 0.10f, frame.size.width * 0.5f, buttonHeight)];
		[buttonStop_ addTarget:self action:@selector(onButtonClickedStop:) forControlEvents:UIControlEventTouchUpInside];
		[buttonStop_ setTitle:@"Stop" forState:UIControlStateNormal];
		buttonStop_.titleLabel.font = [UIFont systemFontOfSize:buttonHeight - 10];
		buttonStop_.titleLabel.font = [UIFont systemFontOfSize:40];
		buttonStop_.backgroundColor = [UIColor whiteColor];
		[buttonStop_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		buttonStop_.layer.borderWidth = 1.0f;
		buttonStop_.layer.cornerRadius = 10.0f;
		buttonStop_.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
		buttonStop_.hidden = YES;
		[self addSubview:buttonStop_];

		textFieldFilename_ = [[UITextField alloc] initWithFrame:CGRectMake(frame.size.width * 0.05f, frame.size.height * 0.10f, frame.size.width * 0.90f, textFieldHeight)];
		[textFieldFilename_ addTarget:self action:@selector(textFieldAddressChanged:) forControlEvents:UIControlEventEditingChanged];
		textFieldFilename_.font = [UIFont systemFontOfSize:textFieldHeight - 10];
		textFieldFilename_.placeholder = @"  Enter Filename  ";
		textFieldFilename_.backgroundColor = [UIColor whiteColor];
		textFieldFilename_.layer.borderWidth = 0.5f;
		textFieldFilename_.layer.cornerRadius = 10.0f;
		textFieldFilename_.textAlignment = NSTextAlignmentCenter;
		textFieldFilename_.delegate = self;
		textFieldFilename_.hidden = NO;
		[self addSubview:textFieldFilename_];

		owner_ = owner;
	}

	return self;
}

- (void)onButtonClickedStart:(UIButton*)sender
{
	ocean_assert(sender == buttonStart_);

	if (!filename_.empty())
	{
		IO::File localFile = IO::File(filename_);
		if (localFile.extension() != "ocean_map")
		{
			localFile = IO::File(filename_ + ".ocean_map");
		}

		const IO::Directory directory(IO::Directory(Platform::Apple::Utilities::documentDirectory()) + IO::Directory("maps"));
		const IO::File absoluteFile(directory + localFile);

		if (absoluteFile.exists())
		{
			if (owner_->start(absoluteFile()))
			{
				[textFieldFilename_ resignFirstResponder];

				buttonStart_.hidden = YES;
				textFieldFilename_.hidden = YES;
				buttonStop_.hidden = NO;
			}
		}
		else
		{
			Log::error() << "The map does not exist";
		}
	}
}

- (void)onButtonClickedStop:(UIButton*)sender
{
	ocean_assert(sender == buttonStop_);

	if (owner_->stop())
	{
		buttonStop_.hidden = YES;
	}
}

- (void)textFieldAddressChanged:(UITextField*)sender
{
	ocean_assert(sender == textFieldFilename_);

	filename_ = String::trim(StringApple::toUTF8([textFieldFilename_ text]));

	if (filename_.empty())
	{
		buttonStart_.enabled = NO;
	}
	else
	{
		buttonStart_.enabled = YES;
	}
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
	ocean_assert(textField == textFieldFilename_);

	if (!filename_.empty())
	{
		[textFieldFilename_ resignFirstResponder];
		return YES;
	}

	return NO;
}

@end

namespace Ocean
{

namespace XRPlayground
{

void OnDeviceRelocalizerExperience::showUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	ocean_assert(userInterface.isIOS());

	UIViewController* viewController = userInterface.viewController();

	OnDeviceRelocalizerExperienceInteractionView* view = [[OnDeviceRelocalizerExperienceInteractionView alloc] initWithFrame:viewController.view.frame withOwner:this];
	[viewController.view addSubview:view];
}

void OnDeviceRelocalizerExperience::unloadUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	UIViewController* viewController = userInterface.viewController();

	for (OnDeviceRelocalizerExperienceInteractionView* interactionView in [viewController.view subviews])
	{
		ocean_assert(interactionView);

		[interactionView removeFromSuperview];
	}
}

}

}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
