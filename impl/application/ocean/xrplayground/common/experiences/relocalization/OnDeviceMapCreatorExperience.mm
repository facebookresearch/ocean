// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/relocalization/OnDeviceMapCreatorExperience.h"

#include "ocean/base/StringApple.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/platform/apple/Utilities.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

using namespace Ocean;

@interface OnDeviceMapCreatorExperienceInteractionView : UIView <UITextFieldDelegate>
{
	/// The start button.
	UIButton* buttonStart_;

	/// The stop button.
	UIButton* buttonStop_;

	/// The save raw button.
	UIButton* buttonSaveRaw_;

	/// The save optimized button.
	UIButton* buttonSaveOptimized_;

	/// The cancel button.
	UIButton* buttonCancel_;

	/// The text field for the filename
	UITextField* textFieldFilename_;

	/// The currently defined filename
	std::string filename_;

	/// The owner of this view.
	XRPlayground::OnDeviceMapCreatorExperience* owner_;
}

@end

@implementation OnDeviceMapCreatorExperienceInteractionView

- (id)initWithFrame:(CGRect)frame withOwner:(XRPlayground::OnDeviceMapCreatorExperience*)owner
{
	if (self = [super initWithFrame:frame])
	{
		constexpr float textFieldHeight = 40;
		constexpr float buttonHeight = 50;

		buttonStart_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.25f, frame.size.height * 0.10f, frame.size.width * 0.5f, buttonHeight)];
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

		buttonSaveRaw_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.20f, frame.size.width * 0.80f, buttonHeight)];
		[buttonSaveRaw_ addTarget:self action:@selector(onButtonClickedSave:) forControlEvents:UIControlEventTouchUpInside];
		[buttonSaveRaw_ setTitle:@"Save raw" forState:UIControlStateNormal];
		buttonSaveRaw_.titleLabel.font = [UIFont systemFontOfSize:buttonHeight - 10];
		buttonSaveRaw_.titleLabel.font = [UIFont systemFontOfSize:40];
		buttonSaveRaw_.backgroundColor = [UIColor whiteColor];
		[buttonSaveRaw_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		[buttonSaveRaw_ setTitleColor:[UIColor grayColor] forState:UIControlStateDisabled];
		buttonSaveRaw_.layer.borderWidth = 1.0f;
		buttonSaveRaw_.layer.cornerRadius = 10.0f;
		buttonSaveRaw_.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
		buttonSaveRaw_.hidden = YES;
		buttonSaveRaw_.enabled = NO;
		[self addSubview:buttonSaveRaw_];

		buttonSaveOptimized_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.28f, frame.size.width * 0.80f, buttonHeight)];
		[buttonSaveOptimized_ addTarget:self action:@selector(onButtonClickedSave:) forControlEvents:UIControlEventTouchUpInside];
		[buttonSaveOptimized_ setTitle:@"Save optimized" forState:UIControlStateNormal];
		buttonSaveOptimized_.titleLabel.font = [UIFont systemFontOfSize:buttonHeight - 10];
		buttonSaveOptimized_.titleLabel.font = [UIFont systemFontOfSize:40];
		buttonSaveOptimized_.backgroundColor = [UIColor whiteColor];
		[buttonSaveOptimized_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		[buttonSaveOptimized_ setTitleColor:[UIColor grayColor] forState:UIControlStateDisabled];
		buttonSaveOptimized_.layer.borderWidth = 1.0f;
		buttonSaveOptimized_.layer.cornerRadius = 10.0f;
		buttonSaveOptimized_.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
		buttonSaveOptimized_.hidden = YES;
		buttonSaveOptimized_.enabled = NO;
		[self addSubview:buttonSaveOptimized_];

		buttonCancel_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.25f, frame.size.height * 0.80f, frame.size.width * 0.50f, buttonHeight)];
		[buttonCancel_ addTarget:self action:@selector(onButtonClickedCancel:) forControlEvents:UIControlEventTouchUpInside];
		[buttonCancel_ setTitle:@"Cancel" forState:UIControlStateNormal];
		buttonCancel_.titleLabel.font = [UIFont systemFontOfSize:buttonHeight - 10];
		buttonCancel_.titleLabel.font = [UIFont systemFontOfSize:40];
		buttonCancel_.backgroundColor = [UIColor whiteColor];
		[buttonCancel_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		buttonCancel_.layer.borderWidth = 1.0f;
		buttonCancel_.layer.cornerRadius = 10.0f;
		buttonCancel_.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
		buttonCancel_.hidden = YES;
		[self addSubview:buttonCancel_];

		textFieldFilename_ = [[UITextField alloc] initWithFrame:CGRectMake(frame.size.width * 0.05f, frame.size.height * 0.10f, frame.size.width * 0.90f, textFieldHeight)];
		[textFieldFilename_ addTarget:self action:@selector(textFieldAddressChanged:) forControlEvents:UIControlEventEditingChanged];
		textFieldFilename_.font = [UIFont systemFontOfSize:textFieldHeight - 10];
		textFieldFilename_.placeholder = @"  Enter Filename  ";
		textFieldFilename_.backgroundColor = [UIColor whiteColor];
		textFieldFilename_.layer.borderWidth = 0.5f;
		textFieldFilename_.layer.cornerRadius = 10.0f;
		textFieldFilename_.textAlignment = NSTextAlignmentCenter;
		textFieldFilename_.delegate = self;
		textFieldFilename_.hidden = YES;
		[self addSubview:textFieldFilename_];

		owner_ = owner;
	}

	return self;
}

- (void)onButtonClickedStart:(UIButton*)sender
{
	ocean_assert(sender == buttonStart_);

	if (owner_->start())
	{
		buttonStart_.hidden = YES;
		buttonStop_.hidden = NO;
	}
}

- (void)onButtonClickedStop:(UIButton*)sender
{
	ocean_assert(sender == buttonStop_);

	if (owner_->stop())
	{
		buttonStop_.hidden = YES;
		buttonSaveRaw_.hidden = NO;
		buttonSaveRaw_.enabled = NO;
		buttonSaveOptimized_.hidden = NO;
		buttonSaveOptimized_.enabled = NO;
		buttonCancel_.hidden = NO;

		textFieldFilename_.hidden = NO;
		textFieldFilename_.text = @"";
	}
}

- (void)onButtonClickedSave:(UIButton*)sender
{
	ocean_assert(sender == buttonSaveRaw_ || sender == buttonSaveOptimized_);

	if (!filename_.empty())
	{
		const IO::Directory directory(IO::Directory(Platform::Apple::Utilities::documentDirectory()) + IO::Directory("maps"));

		if (!directory.exists())
		{
			directory.create();
		}

		const IO::File file(directory + IO::File(filename_ + ".ocean_map"));

		if (file.exists())
		{
			Log::error() << "The selected file exists already";
			return;
		}

		const bool optimized = sender == buttonSaveOptimized_;

		if (owner_->save(file(), optimized))
		{
			[textFieldFilename_ resignFirstResponder];

			buttonStart_.hidden = NO;
			textFieldFilename_.hidden = YES;
			buttonSaveRaw_.hidden = YES;
			buttonSaveOptimized_.hidden = YES;
			buttonCancel_.hidden = YES;
		}
	}
}

- (void)onButtonClickedCancel:(UIButton*)sender
{
	ocean_assert(sender == buttonCancel_);

	[textFieldFilename_ resignFirstResponder];

	buttonStart_.hidden = NO;
	textFieldFilename_.hidden = YES;
	buttonSaveRaw_.hidden = YES;
	buttonSaveOptimized_.hidden = YES;
	buttonCancel_.hidden = YES;
}

- (void)textFieldAddressChanged:(UITextField*)sender
{
	ocean_assert(sender == textFieldFilename_);

	filename_ = String::trim(StringApple::toUTF8([textFieldFilename_ text]));

	if (filename_.empty())
	{
		buttonSaveRaw_.enabled = NO;
		buttonSaveOptimized_.enabled = NO;
	}
	else
	{
		buttonSaveRaw_.enabled = YES;
		buttonSaveOptimized_.enabled = YES;
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

void OnDeviceMapCreatorExperience::showUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	ocean_assert(userInterface.isIOS());

	UIViewController* viewController = userInterface.viewController();

	OnDeviceMapCreatorExperienceInteractionView* view = [[OnDeviceMapCreatorExperienceInteractionView alloc] initWithFrame:viewController.view.frame withOwner:this];
	[viewController.view addSubview:view];
}

void OnDeviceMapCreatorExperience::unloadUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	UIViewController* viewController = userInterface.viewController();

	for (OnDeviceMapCreatorExperienceInteractionView* interactionView in [viewController.view subviews])
	{
		ocean_assert(interactionView);

		[interactionView removeFromSuperview];
	}
}

}

}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
