// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/TexturedSceneTrackerExperience.h"

#include "ocean/base/DateTime.h"
#include "ocean/base/StringApple.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/platform/apple/Utilities.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

using namespace Ocean;

@interface TexturedSceneTrackerExperienceInteractionView : UIView <UITextFieldDelegate>
{
	/// The start scanning button.
	UIButton* buttonStart_;

	/// The stop texturing button.
	UIButton* buttonStop_;

	/// The export mesh button.
	UIButton* buttonExport_;

	/// The owner of this view.
	XRPlayground::TexturedSceneTrackerExperience* owner_;
}

@end

@implementation TexturedSceneTrackerExperienceInteractionView

- (id)initWithFrame:(CGRect)frame withOwner:(XRPlayground::TexturedSceneTrackerExperience*)owner
{
	if (self = [super initWithFrame:frame])
	{
		constexpr float buttonHeight = 50;

		buttonStart_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.1f, frame.size.width * 0.8f, buttonHeight)];
		[buttonStart_ addTarget:self action:@selector(onButtonClickedStart:) forControlEvents:UIControlEventTouchUpInside];
		[buttonStart_ setTitle:@"Start" forState:UIControlStateNormal];
		buttonStart_.titleLabel.font = [UIFont systemFontOfSize:buttonHeight - 10];
		buttonStart_.titleLabel.font = [UIFont systemFontOfSize:40];
		buttonStart_.backgroundColor = [UIColor whiteColor];
		[buttonStart_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		[buttonStart_ setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
		buttonStart_.titleLabel.textColor = [UIColor whiteColor];
		buttonStart_.layer.borderWidth = 1.0f;
		buttonStart_.layer.cornerRadius = 10.0f;
		buttonStart_.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
		buttonStart_.hidden = NO;
		[self addSubview:buttonStart_];

		buttonStop_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.1f, frame.size.width * 0.8f, buttonHeight)];
		[buttonStop_ addTarget:self action:@selector(onButtonClickedStart:) forControlEvents:UIControlEventTouchUpInside];
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

		buttonExport_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.1f, frame.size.width * 0.8f, buttonHeight)];
		[buttonExport_ addTarget:self action:@selector(onButtonClickedStart:) forControlEvents:UIControlEventTouchUpInside];
		[buttonExport_ setTitle:@"Export Mesh" forState:UIControlStateNormal];
		buttonExport_.titleLabel.font = [UIFont systemFontOfSize:buttonHeight - 10];
		buttonExport_.titleLabel.font = [UIFont systemFontOfSize:40];
		buttonExport_.backgroundColor = [UIColor whiteColor];
		[buttonExport_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		[buttonExport_ setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
		buttonExport_.titleLabel.textColor = [UIColor whiteColor];
		buttonExport_.layer.borderWidth = 1.0f;
		buttonExport_.layer.cornerRadius = 10.0f;
		buttonExport_.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
		buttonExport_.hidden = YES;
		[self addSubview:buttonExport_];

		owner_ = owner;
	}

	return self;
}

- (void)onButtonClickedStart:(UIButton*)sender
{
	if (sender == buttonStart_)
	{
		if (owner_->start())
		{
			buttonStart_.hidden = YES;
			buttonStop_.hidden = NO;
		}
	}
	else if (sender == buttonStop_)
	{
		if (owner_->stop())
		{
			buttonStop_.hidden = YES;
			buttonExport_.hidden = NO;
		}
	}
	else
	{
		ocean_assert(sender == buttonExport_);

		const IO::Directory directory(IO::Directory(Platform::Apple::Utilities::documentDirectory()) + IO::Directory("textured_meshes"));

		if (!directory.exists())
		{
			directory.create();
		}

		std::string filename = "textured_mesh_" + DateTime::localStringDate('-') + "__" + DateTime::localStringTime(false, '-') + ".x3dv";

		const IO::File file(directory + IO::File(filename));

		if (file.exists())
		{
			Log::error() << "The mesh file '" << filename << "' exists already";
			return;
		}

		if (owner_->exportMesh(file()))
		{
			Log::info() << "Successfully wrote mesh file " << filename;

			buttonExport_.hidden = YES;
		}
		else
		{
			Log::error() << "Failed to export mesh " << filename;
		}
	}
}

@end

namespace Ocean
{

namespace XRPlayground
{

void TexturedSceneTrackerExperience::showUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	ocean_assert(userInterface.isIOS());

	UIViewController* viewController = userInterface.viewController();

	TexturedSceneTrackerExperienceInteractionView* view = [[TexturedSceneTrackerExperienceInteractionView alloc] initWithFrame:viewController.view.frame withOwner:this];
	[viewController.view addSubview:view];
}

void TexturedSceneTrackerExperience::unloadUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	UIViewController* viewController = userInterface.viewController();

	for (TexturedSceneTrackerExperienceInteractionView* interactionView in [viewController.view subviews])
	{
		ocean_assert(interactionView);

		[interactionView removeFromSuperview];
	}
}

}

}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
