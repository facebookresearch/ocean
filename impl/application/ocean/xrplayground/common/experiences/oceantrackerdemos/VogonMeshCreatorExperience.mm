// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/VogonMeshCreatorExperience.h"

#include "ocean/base/DateTime.h"
#include "ocean/base/StringApple.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/platform/apple/Utilities.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

using namespace Ocean;

@interface VogonMeshCreatorExperienceInteractionView : UIView <UITextFieldDelegate>
{
	/// The slider for the voxel size.
	UISlider* sliderVoxelSize_;

	/// The label for the voxel size slider.
	UILabel* labelVoxelSize_;

	/// The start scanning without color button.
	UIButton* buttonStartWithoutColor_;

	/// The start scanning with per-vertex color button.
	UIButton* buttonStartWithPerVertexColor_;

	/// The stop scanning button.
	UIButton* buttonStop_;

	/// The export mesh button.
	UIButton* buttonExport_;

	/// The owner of this view.
	XRPlayground::VogonMeshCreatorExperience* owner_;
}

@end

@implementation VogonMeshCreatorExperienceInteractionView

- (id)initWithFrame:(CGRect)frame withOwner:(XRPlayground::VogonMeshCreatorExperience*)owner
{
	if (self = [super initWithFrame:frame])
	{
		constexpr float buttonHeight = 50;

		sliderVoxelSize_ = [[UISlider alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.1f, frame.size.width * 0.8f, buttonHeight)];
		[sliderVoxelSize_ addTarget:self action:@selector(onSliderChangedVoxelSize:) forControlEvents:UIControlEventValueChanged];
		sliderVoxelSize_.minimumValue = 0.001f;
		sliderVoxelSize_.maximumValue = 0.1f;
		sliderVoxelSize_.value = 0.03f;
		sliderVoxelSize_.continuous = true;
		sliderVoxelSize_.hidden = NO;
		[self addSubview:sliderVoxelSize_];

		labelVoxelSize_ = [[UILabel alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.06f, frame.size.width * 0.8f, buttonHeight)];
		labelVoxelSize_.text = @"Voxel size: 3cm";
		labelVoxelSize_.textAlignment = NSTextAlignmentCenter;
		labelVoxelSize_.hidden = NO;
		[self addSubview:labelVoxelSize_];

		buttonStartWithoutColor_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.2f, frame.size.width * 0.8f, buttonHeight)];
		[buttonStartWithoutColor_ addTarget:self action:@selector(onButtonClickedStart:) forControlEvents:UIControlEventTouchUpInside];
		[buttonStartWithoutColor_ setTitle:@"Start without color" forState:UIControlStateNormal];
		buttonStartWithoutColor_.titleLabel.font = [UIFont systemFontOfSize:20];
		buttonStartWithoutColor_.backgroundColor = [UIColor whiteColor];
		[buttonStartWithoutColor_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		[buttonStartWithoutColor_ setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
		buttonStartWithoutColor_.titleLabel.textColor = [UIColor whiteColor];
		buttonStartWithoutColor_.layer.borderWidth = 1.0f;
		buttonStartWithoutColor_.layer.cornerRadius = 10.0f;
		buttonStartWithoutColor_.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
		buttonStartWithoutColor_.hidden = NO;
		[self addSubview:buttonStartWithoutColor_];

		buttonStartWithPerVertexColor_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.3f, frame.size.width * 0.8f, buttonHeight)];
		[buttonStartWithPerVertexColor_ addTarget:self action:@selector(onButtonClickedStart:) forControlEvents:UIControlEventTouchUpInside];
		[buttonStartWithPerVertexColor_ setTitle:@"Start with vertex color" forState:UIControlStateNormal];
		buttonStartWithPerVertexColor_.titleLabel.font = [UIFont systemFontOfSize:20];
		buttonStartWithPerVertexColor_.backgroundColor = [UIColor whiteColor];
		[buttonStartWithPerVertexColor_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
		[buttonStartWithPerVertexColor_ setTitleColor:[UIColor whiteColor] forState:UIControlStateHighlighted];
		buttonStartWithPerVertexColor_.titleLabel.textColor = [UIColor whiteColor];
		buttonStartWithPerVertexColor_.layer.borderWidth = 1.0f;
		buttonStartWithPerVertexColor_.layer.cornerRadius = 10.0f;
		buttonStartWithPerVertexColor_.contentEdgeInsets = UIEdgeInsetsMake(8, 8, 8, 8);
		buttonStartWithPerVertexColor_.hidden = NO;
		[self addSubview:buttonStartWithPerVertexColor_];

		buttonStop_ = [[UIButton alloc] initWithFrame:CGRectMake(frame.size.width * 0.1f, frame.size.height * 0.1f, frame.size.width * 0.8f, buttonHeight)];
		[buttonStop_ addTarget:self action:@selector(onButtonClickedStart:) forControlEvents:UIControlEventTouchUpInside];
		[buttonStop_ setTitle:@"Stop" forState:UIControlStateNormal];
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

- (void)onSliderChangedVoxelSize:(UISlider*)sender
{
	const float voxelSizeInCm = sliderVoxelSize_.value * 100.0f;

	labelVoxelSize_.text = StringApple::toNSString("Voxel size: " + String::toAString(voxelSizeInCm, 2u) + "cm");
}

- (void)onButtonClickedStart:(UIButton*)sender
{
	if (sender == buttonStartWithoutColor_ || sender == buttonStartWithPerVertexColor_)
	{
		const float voxelSizeInMeter = sliderVoxelSize_.value;

		const bool createPerVertexColors = sender == buttonStartWithPerVertexColor_;

		if (owner_->start(voxelSizeInMeter, createPerVertexColors))
		{
			buttonStartWithoutColor_.hidden = YES;
			buttonStartWithPerVertexColor_.hidden = YES;

			sliderVoxelSize_.hidden = YES;
			labelVoxelSize_.hidden = YES;
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

		const IO::Directory directory(IO::Directory(Platform::Apple::Utilities::documentDirectory()) + IO::Directory("meshes"));

		if (!directory.exists())
		{
			directory.create();
		}

		std::string filename = "vogon_mesh_" + DateTime::localStringDate('-') + "__" + DateTime::localStringTime(false, '-') + ".x3dv";

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
	}
}

@end

namespace Ocean
{

namespace XRPlayground
{

void VogonMeshCreatorExperience::showUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	ocean_assert(userInterface.isIOS());

	UIViewController* viewController = userInterface.viewController();

	VogonMeshCreatorExperienceInteractionView* view = [[VogonMeshCreatorExperienceInteractionView alloc] initWithFrame:viewController.view.frame withOwner:this];
	[viewController.view addSubview:view];
}

void VogonMeshCreatorExperience::unloadUserInterinterfaceIOS(const Interaction::UserInterface& userInterface)
{
	UIViewController* viewController = userInterface.viewController();

	for (VogonMeshCreatorExperienceInteractionView* interactionView in [viewController.view subviews])
	{
		ocean_assert(interactionView);

		[interactionView removeFromSuperview];
	}
}

}

}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
