// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/QRCodeGeneratorExperience.h"

#include "ocean/base/DateTime.h"
#include "ocean/base/StringApple.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/platform/apple/Utilities.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

using namespace Ocean;

@interface QRCodeGeneratorExperienceInteractionView : UIView <UITextFieldDelegate>
{
	/// The text field where the payload of the code is entered.
	UITextField* textFieldCodePayload_;

	/// A recognizer for taps outside the text field in order to hide the display keyboard.
	UITapGestureRecognizer* tapGestureRecognizer_;

	/// A label to display version information of a code.
	UILabel* labelCodeVersion_;

	/// The owner of this view.
	XRPlayground::QRCodeGeneratorExperience* owner_;
}

@end

@implementation QRCodeGeneratorExperienceInteractionView

- (id)initWithFrame:(CGRect)frame withOwner:(XRPlayground::QRCodeGeneratorExperience*)owner
{
	if (self = [super initWithFrame:frame])
	{
		constexpr float buttonHeight = 50;
		constexpr float textFieldHeight = 40;

		textFieldCodePayload_ = [[UITextField alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.1f, frame.size.width * 0.8f, buttonHeight)];

		[textFieldCodePayload_ addTarget:self action:@selector(textFieldCodePayloadChanged:) forControlEvents:UIControlEventEditingChanged];
		textFieldCodePayload_.font = [UIFont systemFontOfSize:textFieldHeight - 10];
		textFieldCodePayload_.placeholder = @"  Enter Payload  ";
		textFieldCodePayload_.backgroundColor = [UIColor whiteColor];
		textFieldCodePayload_.layer.borderWidth = 0.5f;
		textFieldCodePayload_.layer.cornerRadius = 10.0f;
		textFieldCodePayload_.textAlignment = NSTextAlignmentCenter;
		textFieldCodePayload_.keyboardType = UIKeyboardTypeDefault;
		textFieldCodePayload_.delegate = self;
		textFieldCodePayload_.clearButtonMode = UITextFieldViewModeWhileEditing;
		[self addSubview:textFieldCodePayload_];

		tapGestureRecognizer_ = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(dismissKeyboard)];
		[self addGestureRecognizer:tapGestureRecognizer_];

		labelCodeVersion_ = [[UILabel alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.15f, frame.size.width * 0.8f, buttonHeight)];
		labelCodeVersion_.textAlignment = NSTextAlignmentCenter;
		labelCodeVersion_.hidden = YES;
		[self addSubview:labelCodeVersion_];

		owner_ = owner;
	}

	return self;
}

- (void)textFieldCodePayloadChanged:(UITextField*)sender
{
	const std::string codePayload = String::trim(StringApple::toUTF8([textFieldCodePayload_ text]));

	if (!codePayload.empty())
	{
		unsigned int codeVersion = (unsigned int)(-1);

		if (owner_->generateQRCodeFrame(codePayload, codeVersion))
		{
			if (codeVersion >= 1u && codeVersion <= 40u)
			{
				const unsigned int modulesPerSide = 4u * codeVersion + 17u;

				labelCodeVersion_.text = StringApple::toNSString("Version: " + String::toAString(codeVersion) + " (" + String::toAString(modulesPerSide) + " x " + String::toAString(modulesPerSide) + ")");
				labelCodeVersion_.hidden = NO;

				return;
			}
		}

		Log::error() << "Failed to create the QR code";
	}

	labelCodeVersion_.hidden = YES;
}

-(void)dismissKeyboard
{
	[textFieldCodePayload_ resignFirstResponder];
}

@end

namespace Ocean
{

namespace XRPlayground
{

void QRCodeGeneratorExperience::showUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	ocean_assert(userInterface.isIOS());

	UIViewController* viewController = userInterface.viewController();

	QRCodeGeneratorExperienceInteractionView* view = [[QRCodeGeneratorExperienceInteractionView alloc] initWithFrame:viewController.view.frame withOwner:this];
	[viewController.view addSubview:view];
}

void QRCodeGeneratorExperience::unloadUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	UIViewController* viewController = userInterface.viewController();

	for (QRCodeGeneratorExperienceInteractionView* interactionView in [viewController.view subviews])
	{
		ocean_assert(interactionView);

		[interactionView removeFromSuperview];
	}
}

}

}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
