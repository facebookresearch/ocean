// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/metaportation/LegacyMetaportationScannerExperience.h"

#include "ocean/base/StringApple.h"

#include "ocean/network/Port.h"
#include "ocean/network/Resolver.h"

#include "ocean/rendering/Utilities.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

using namespace Ocean;

@interface LegacyMetaportationScannerExperienceInteractionView : UIView <UITextFieldDelegate>
{
	/// The text field for th target IP address.
	UITextField* textFieldAddress_;

	/// The text field for the target port.
	UITextField* textFieldPort_;

	/// The current target IP address.
	Network::Address4 address_;

	/// The current target port.
	Network::Port port_;

	/// The start textured button.
	UIButton* buttonStartTextured_;

	/// The start colored button.
	UIButton* buttonStartColored_;

	/// The stop button.
	UIButton* buttonStop_;

	/// The owner of this view.
	XRPlayground::LegacyMetaportationScannerExperience* owner_;
}

@end

@implementation LegacyMetaportationScannerExperienceInteractionView

- (id)initWithFrame:(CGRect)frame withOwner:(XRPlayground::LegacyMetaportationScannerExperience*)owner
{
	if (self = [super initWithFrame:frame])
	{
		constexpr float textFieldHeight = 40;
		constexpr float buttonHeight = 50;

		textFieldAddress_ = [[UITextField alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.15f, frame.size.width * 0.8f, textFieldHeight)];
		[textFieldAddress_ addTarget:self action:@selector(textFieldAddressChanged:) forControlEvents:UIControlEventEditingChanged];
		textFieldAddress_.font = [UIFont systemFontOfSize:textFieldHeight - 10];

		const Network::Resolver::Addresses4 localAddresses = Network::Resolver::get().localAddresses();

		if (!localAddresses.empty())
		{
			const unsigned int addressNumber = localAddresses.front();

			const unsigned int sub0 = addressNumber & 0xFF;
			const unsigned int sub1 = (addressNumber >> 8) & 0xFF;
			const unsigned int sub2 = (addressNumber >> 16) & 0xFF;

			const std::string localNet = String::toAString(sub0) + std::string(".") + String::toAString(sub1) + std::string(".") + String::toAString(sub2) + std::string(".");

			textFieldAddress_.text = StringApple::toNSString(localNet);
		}
		else
		{
			textFieldAddress_.placeholder = @"  Enter IP Address  ";
		}

		textFieldAddress_.backgroundColor = [UIColor whiteColor];
		textFieldAddress_.layer.borderWidth = 0.5f;
		textFieldAddress_.layer.cornerRadius = 10.0f;
		textFieldAddress_.textAlignment = NSTextAlignmentCenter;
		textFieldAddress_.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
		textFieldAddress_.delegate = self;
		[self addSubview:textFieldAddress_];

		textFieldPort_ = [[UITextField alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.22f, frame.size.width * 0.8f, textFieldHeight)];
		[textFieldPort_ addTarget:self action:@selector(textFieldPortChanged:) forControlEvents:UIControlEventEditingChanged];
		textFieldPort_.font = [UIFont systemFontOfSize:textFieldHeight - 10];

		if (!localAddresses.empty())
		{
			textFieldPort_.text = @"6000";
			port_ = Network::Port(6000, Network::Port::TYPE_READABLE);
		}
		else
		{
			textFieldPort_.placeholder = @"  Enter Port  ";
		}

		textFieldPort_.backgroundColor = [UIColor whiteColor];
		textFieldPort_.layer.borderWidth = 0.5f;
		textFieldPort_.layer.cornerRadius = 10.0f;
		textFieldPort_.textAlignment = NSTextAlignmentCenter;
		textFieldPort_.keyboardType = UIKeyboardTypeNumbersAndPunctuation;
		textFieldPort_.delegate = self;
		[self addSubview:textFieldPort_];

		buttonStartTextured_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.35f, frame.size.width * 0.8f, buttonHeight)];
		[buttonStartTextured_ addTarget:self action:@selector(onButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
		[buttonStartTextured_ setTitle:@"Start Textured" forState:UIControlStateNormal];
		buttonStartTextured_.titleLabel.font = [UIFont systemFontOfSize:buttonHeight - 10];
		buttonStartTextured_.titleLabel.font = [UIFont systemFontOfSize:40];
		buttonStartTextured_.backgroundColor = [UIColor whiteColor];
		[buttonStartTextured_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		[buttonStartTextured_ setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
		buttonStartTextured_.titleLabel.textColor = [UIColor whiteColor];
		buttonStartTextured_.layer.borderWidth = 1.0f;
		buttonStartTextured_.layer.cornerRadius = 10.0f;
		buttonStartTextured_.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
		buttonStartTextured_.hidden = YES;
		[self addSubview:buttonStartTextured_];

		buttonStartColored_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.43f, frame.size.width * 0.8f, buttonHeight)];
		[buttonStartColored_ addTarget:self action:@selector(onButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
		[buttonStartColored_ setTitle:@"Start Colored" forState:UIControlStateNormal];
		buttonStartColored_.titleLabel.font = [UIFont systemFontOfSize:buttonHeight - 10];
		buttonStartColored_.titleLabel.font = [UIFont systemFontOfSize:40];
		buttonStartColored_.backgroundColor = [UIColor whiteColor];
		[buttonStartColored_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		[buttonStartColored_ setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
		buttonStartColored_.titleLabel.textColor = [UIColor whiteColor];
		buttonStartColored_.layer.borderWidth = 1.0f;
		buttonStartColored_.layer.cornerRadius = 10.0f;
		buttonStartColored_.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
		buttonStartColored_.hidden = YES;
		[self addSubview:buttonStartColored_];

		buttonStop_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.15f, frame.size.width * 0.8f, buttonHeight)];
		[buttonStop_ addTarget:self action:@selector(onButtonClicked:) forControlEvents:UIControlEventTouchUpInside];
		[buttonStop_ setTitle:@"Stop" forState:UIControlStateNormal];
		buttonStop_.titleLabel.font = [UIFont systemFontOfSize:buttonHeight - 10];
		buttonStop_.titleLabel.font = [UIFont systemFontOfSize:40];
		buttonStop_.backgroundColor = [UIColor whiteColor];
		[buttonStop_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		[buttonStop_ setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
		buttonStop_.titleLabel.textColor = [UIColor whiteColor];
		buttonStop_.layer.borderWidth = 1.0f;
		buttonStop_.layer.cornerRadius = 10.0f;
		buttonStop_.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
		buttonStop_.hidden = YES;
		[self addSubview:buttonStop_];

		owner_ = owner;
	}

	return self;
}

- (void)textFieldAddressChanged:(UITextField*)sender
{
	const std::string textAddressString = String::trim(StringApple::toUTF8([textFieldAddress_ text]));

	const Network::Address4 address = Network::Resolver::resolveFirstIp4(textAddressString);

	if (address.isValid())
	{
		address_ = address;

		if (port_.isValid())
		{
			buttonStartTextured_.hidden = NO;
			buttonStartColored_.hidden = NO;
			return;
		}
	}

	buttonStartTextured_.hidden = YES;
	buttonStartColored_.hidden = YES;
}

- (void)textFieldPortChanged:(UITextField*)sender
{
	const std::string textPortString = String::trim(StringApple::toUTF8([textFieldPort_ text]));

	int portValue = 0;
	if (String::isInteger32(textPortString, &portValue) && portValue > 0 && portValue <= 65536)
	{
		const Network::Port port((unsigned short)(portValue), Network::Port::TYPE_READABLE);

		if (port.isValid())
		{
			port_ = port;

			if (address_.isValid())
			{
				buttonStartTextured_.hidden = NO;
				buttonStartColored_.hidden = NO;
				return;
			}
		}
	}

	buttonStartTextured_.hidden = YES;
	buttonStartColored_.hidden = YES;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
	if (textField == textFieldAddress_)
	{
		if (address_.isValid())
		{
			[textFieldAddress_ resignFirstResponder];
			return YES;
		}
	}
	else
	{
		ocean_assert(textField == textFieldPort_);

		if (port_.isValid())
		{
			[textFieldPort_ resignFirstResponder];
			return YES;
		}
	}

	return NO;
}

- (void)onButtonClicked:(UIButton*)sender
{
	ocean_assert(address_.isValid() && port_.isValid());

	if (sender == buttonStartTextured_ || sender == buttonStartColored_)
	{
		if (address_.isValid() && port_.isValid())
		{
			const XRPlayground::LegacyMetaportationScannerExperience::ScanningMode scanningMode = sender == buttonStartTextured_ ? XRPlayground::LegacyMetaportationScannerExperience::SM_TEXTURED : XRPlayground::LegacyMetaportationScannerExperience::SM_PER_VERTEX_COLORS;

			if (owner_->start(scanningMode, address_, port_))
			{
				[textFieldAddress_ resignFirstResponder];
				[textFieldPort_ resignFirstResponder];

				textFieldAddress_.hidden = YES;
				textFieldPort_.hidden = YES;
				buttonStartTextured_.hidden = YES;
				buttonStartColored_.hidden = YES;
				buttonStop_.hidden = NO;
			}
		}
	}
	else
	{
		ocean_assert(sender == buttonStop_);
		if (owner_->stop())
		{
			buttonStop_.hidden = YES;
		}
	}
}

@end

namespace Ocean
{

namespace XRPlayground
{

void LegacyMetaportationScannerExperience::showUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	ocean_assert(userInterface.isIOS());

	UIViewController* viewController = userInterface.viewController();

	LegacyMetaportationScannerExperienceInteractionView* view = [[LegacyMetaportationScannerExperienceInteractionView alloc] initWithFrame:viewController.view.frame withOwner:this];
	[viewController.view addSubview:view];
}

void LegacyMetaportationScannerExperience::unloadUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	UIViewController* viewController = userInterface.viewController();

	for (LegacyMetaportationScannerExperienceInteractionView* interactionView in [viewController.view subviews])
	{
		ocean_assert(interactionView);

		[interactionView removeFromSuperview];
	}
}

}

}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
