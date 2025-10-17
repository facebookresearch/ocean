/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "AppDelegate.h"

#include "ocean/base/DateTime.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/StringApple.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"
#include "ocean/io/image/Image.h"

#include "ocean/media/Manager.h"

#include "ocean/media/LiveVideo.h"
#include "ocean/media/Utilities.h"

#include "ocean/media/avfoundation/AVFoundation.h"

#include "ocean/platform/apple/ios/OpenGLFrameMediumViewController.h"
#include "ocean/platform/apple/ios/Utilities.h"

#include "ocean/rendering/glescenegraph/apple/Apple.h"

#include <fstream>

using namespace Ocean;

/**
 * The ViewController adding support for user interaction.
 */
@interface CustomViewController : OpenGLFrameMediumViewController
{

}
@end

@implementation CustomViewController

- (void)update
{
	if (renderingView_ && renderingUndistortedBackground_)
	{
		const Rotation view_R_background(renderingUndistortedBackground_->orientation());

		if (Numeric::isNotEqualEps(view_R_background.angle()))
		{
#ifdef OCEAN_DEBUG
			const Scalar absRotationAngle = Numeric::abs(view_R_background.angle());
			const Vector3 axis = view_R_background.axis();

			ocean_assert(Numeric::isEqual(absRotationAngle, Numeric::pi_2()) || Numeric::isEqual(absRotationAngle, Numeric::pi_2() * Scalar(3)));
			ocean_assert(axis == Vector3(0, 0, 1) || axis == Vector3(0, 0, -1));
#endif // OCEAN_DEBUG
		}

		const bool isNotRoated = Numeric::isEqualEps(view_R_background.angle());

		const PinholeCamera& camera = renderingUndistortedBackground_->camera();

		if (camera.isValid())
		{
			const Scalar backgroundFovX = isNotRoated ? camera.fovX() : camera.fovY();

			constexpr Scalar borderAngle = Numeric::deg2rad(2);
			renderingView_->setFovX(backgroundFovX + borderAngle);
		}
	}

	[super update];
}

@end

@interface AppDelegate ()
{
	CustomViewController* viewController_;
	Media::LiveVideoRef liveVideo_;
	IO::Directory directory_;
	unsigned int pictureCounter_;

	// UI Components
	UIView* cameraSelectionContainer_;
	UILabel* cameraInstructionLabel_;
	UIButton* cameraDropdownButton_;

	UIView* resolutionSelectionContainer_;
	UILabel* resolutionInstructionLabel_;
	UIButton* resolutionDropdownButton_;

	UIView* focusContainer_;
	UILabel* focusLabel_;
	UISlider* focusSlider_;
	UISwitch* stabilizationSwitch_;
	UILabel* stabilizationLabel_;

	UIButton* takeImageButton_;
	UILabel* countdownLabel_;
	UILabel* imageCounterLabel_;

	// State variables
	NSArray<NSString*>* availableCameras_;
	NSArray<NSString*>* availableResolutions_;
	NSString* selectedCamera_;
	NSString* selectedResolution_;
	BOOL cameraSelected_;
	BOOL cameraStarted_;
	NSInteger countdownValue_;
	float initialFocus_;
	float currentFocus_;
	BOOL videoStabilizationEnabled_;
	BOOL settingsFileWritten_;
}
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
	Messenger::get().setOutputType(Messenger::OUTPUT_DEBUG_WINDOW);

	Media::AVFoundation::registerAVFLibrary();
	Rendering::GLESceneGraph::Apple::registerGLESceneGraphEngine();

	self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	self.window.backgroundColor = [UIColor whiteColor];

	viewController_ = [[CustomViewController alloc] init];

	self.window.rootViewController = viewController_;
	[self.window makeKeyAndVisible];

	pictureCounter_ = 0u;
	cameraSelected_ = NO;
	cameraStarted_ = NO;
	initialFocus_ = 0.85f;
	currentFocus_ = 0.85f;
	videoStabilizationEnabled_ = NO;
	settingsFileWritten_ = NO;

	[self setupUI];
	[self loadAvailableCameras];

	return YES;
}

- (void)setupUI
{
	CGRect screenBounds = [[UIScreen mainScreen] bounds];

	// Camera selection container
	cameraSelectionContainer_ = [[UIView alloc] initWithFrame:CGRectMake(25, 100, screenBounds.size.width - 50, 120)];
	cameraSelectionContainer_.backgroundColor = [[UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:0.95] colorWithAlphaComponent:0.95];
	cameraSelectionContainer_.layer.cornerRadius = 12;

	cameraInstructionLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(20, 20, cameraSelectionContainer_.frame.size.width - 40, 30)];
	cameraInstructionLabel_.text = @"Please select a camera:";
	cameraInstructionLabel_.textColor = [UIColor blackColor];
	cameraInstructionLabel_.font = [UIFont systemFontOfSize:18];
	cameraInstructionLabel_.textAlignment = NSTextAlignmentCenter;
	[cameraSelectionContainer_ addSubview:cameraInstructionLabel_];

	cameraDropdownButton_ = [UIButton buttonWithType:UIButtonTypeSystem];
	cameraDropdownButton_.frame = CGRectMake(20, 60, cameraSelectionContainer_.frame.size.width - 40, 44);
	[cameraDropdownButton_ setTitle:@"Select Camera" forState:UIControlStateNormal];
	[cameraDropdownButton_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
	cameraDropdownButton_.backgroundColor = [UIColor whiteColor];
	cameraDropdownButton_.layer.cornerRadius = 8;
	cameraDropdownButton_.layer.borderWidth = 1.0;
	cameraDropdownButton_.layer.borderColor = [UIColor lightGrayColor].CGColor;
	cameraDropdownButton_.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeft;
	cameraDropdownButton_.contentEdgeInsets = UIEdgeInsetsMake(0, 15, 0, 15);
	[cameraDropdownButton_ addTarget:self action:@selector(showCameraDropdown:) forControlEvents:UIControlEventTouchUpInside];
	[cameraSelectionContainer_ addSubview:cameraDropdownButton_];

	[viewController_.view addSubview:cameraSelectionContainer_];

	// Resolution selection container
	resolutionSelectionContainer_ = [[UIView alloc] initWithFrame:CGRectMake(25, 250, screenBounds.size.width - 50, 120)];
	resolutionSelectionContainer_.backgroundColor = [[UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:0.95] colorWithAlphaComponent:0.95];
	resolutionSelectionContainer_.layer.cornerRadius = 12;
	resolutionSelectionContainer_.hidden = YES;

	resolutionInstructionLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(20, 20, resolutionSelectionContainer_.frame.size.width - 40, 30)];
	resolutionInstructionLabel_.text = @"Please select a resolution:";
	resolutionInstructionLabel_.textColor = [UIColor blackColor];
	resolutionInstructionLabel_.font = [UIFont systemFontOfSize:18];
	resolutionInstructionLabel_.textAlignment = NSTextAlignmentCenter;
	[resolutionSelectionContainer_ addSubview:resolutionInstructionLabel_];

	resolutionDropdownButton_ = [UIButton buttonWithType:UIButtonTypeSystem];
	resolutionDropdownButton_.frame = CGRectMake(20, 60, resolutionSelectionContainer_.frame.size.width - 40, 44);
	[resolutionDropdownButton_ setTitle:@"Select Resolution" forState:UIControlStateNormal];
	[resolutionDropdownButton_ setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
	resolutionDropdownButton_.backgroundColor = [UIColor whiteColor];
	resolutionDropdownButton_.layer.cornerRadius = 8;
	resolutionDropdownButton_.layer.borderWidth = 1.0;
	resolutionDropdownButton_.layer.borderColor = [UIColor lightGrayColor].CGColor;
	resolutionDropdownButton_.contentHorizontalAlignment = UIControlContentHorizontalAlignmentLeft;
	resolutionDropdownButton_.contentEdgeInsets = UIEdgeInsetsMake(0, 15, 0, 15);
	[resolutionDropdownButton_ addTarget:self action:@selector(showResolutionDropdown:) forControlEvents:UIControlEventTouchUpInside];
	[resolutionSelectionContainer_ addSubview:resolutionDropdownButton_];

	[viewController_.view addSubview:resolutionSelectionContainer_];

	// Focus container
	focusContainer_ = [[UIView alloc] initWithFrame:CGRectMake(25, 50, screenBounds.size.width - 50, 140)];
	focusContainer_.backgroundColor = [[UIColor colorWithRed:1.0 green:1.0 blue:1.0 alpha:0.95] colorWithAlphaComponent:0.95];
	focusContainer_.layer.cornerRadius = 12;
	focusContainer_.hidden = YES;

	focusLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(20, 15, focusContainer_.frame.size.width - 40, 30)];
	focusLabel_.text = [NSString stringWithFormat:@"Focus: %.2f", initialFocus_];
	focusLabel_.textColor = [UIColor blackColor];
	focusLabel_.font = [UIFont systemFontOfSize:16];
	focusLabel_.textAlignment = NSTextAlignmentCenter;
	[focusContainer_ addSubview:focusLabel_];

	focusSlider_ = [[UISlider alloc] initWithFrame:CGRectMake(20, 50, focusContainer_.frame.size.width - 40, 30)];
	focusSlider_.minimumValue = 0.0;
	focusSlider_.maximumValue = 1.0;
	focusSlider_.value = initialFocus_;
	[focusSlider_ addTarget:self action:@selector(focusSliderChanged:) forControlEvents:UIControlEventValueChanged];
	[focusContainer_ addSubview:focusSlider_];

	stabilizationLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(20, 90, focusContainer_.frame.size.width - 80, 30)];
	stabilizationLabel_.text = @"Video Stabilization";
	stabilizationLabel_.textColor = [UIColor blackColor];
	stabilizationLabel_.font = [UIFont systemFontOfSize:16];
	stabilizationLabel_.textAlignment = NSTextAlignmentLeft;
	[focusContainer_ addSubview:stabilizationLabel_];

	stabilizationSwitch_ = [[UISwitch alloc] initWithFrame:CGRectMake(focusContainer_.frame.size.width - 70, 85, 51, 31)];
	stabilizationSwitch_.on = NO;
	[stabilizationSwitch_ addTarget:self action:@selector(stabilizationSwitchChanged:) forControlEvents:UIControlEventValueChanged];
	[focusContainer_ addSubview:stabilizationSwitch_];

	[viewController_.view addSubview:focusContainer_];

	// Take image button
	takeImageButton_ = [UIButton buttonWithType:UIButtonTypeSystem];
	takeImageButton_.frame = CGRectMake(screenBounds.size.width/2 - 75, screenBounds.size.height - 100, 150, 50);
	[takeImageButton_ setTitle:@"Take Image" forState:UIControlStateNormal];
	[takeImageButton_ setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
	takeImageButton_.titleLabel.font = [UIFont systemFontOfSize:14];
	takeImageButton_.backgroundColor = [UIColor colorWithRed:0.62 green:0.62 blue:0.62 alpha:1.0];
	takeImageButton_.layer.cornerRadius = 25;
	takeImageButton_.enabled = NO;
	takeImageButton_.hidden = YES;
	[takeImageButton_ addTarget:self action:@selector(takeImageButtonPressed:) forControlEvents:UIControlEventTouchUpInside];
	[viewController_.view addSubview:takeImageButton_];

	// Countdown label
	countdownLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(screenBounds.size.width/2 - 50, screenBounds.size.height/2 - 50, 100, 100)];
	countdownLabel_.textColor = [UIColor whiteColor];
	countdownLabel_.font = [UIFont boldSystemFontOfSize:64];
	countdownLabel_.textAlignment = NSTextAlignmentCenter;
	countdownLabel_.backgroundColor = [[UIColor colorWithRed:0.59 green:0.59 blue:0.59 alpha:0.7] colorWithAlphaComponent:0.7];
	countdownLabel_.layer.cornerRadius = 20;
	countdownLabel_.clipsToBounds = YES;
	countdownLabel_.hidden = YES;
	[viewController_.view addSubview:countdownLabel_];

	// Image counter label (bottom right corner)
	imageCounterLabel_ = [[UILabel alloc] initWithFrame:CGRectMake(screenBounds.size.width - 80, screenBounds.size.height - 60, 60, 40)];
	imageCounterLabel_.textColor = [UIColor whiteColor];
	imageCounterLabel_.font = [UIFont boldSystemFontOfSize:24];
	imageCounterLabel_.textAlignment = NSTextAlignmentRight;
	imageCounterLabel_.backgroundColor = [UIColor clearColor];
	imageCounterLabel_.hidden = YES;
	[viewController_.view addSubview:imageCounterLabel_];
}

- (void)loadAvailableCameras
{
	const Media::Library::Definitions definitions = Media::Manager::get().selectableMedia();
	NSMutableArray* cameras = [NSMutableArray arrayWithCapacity:definitions.size()];

	for (const Media::Library::Definition& definition : definitions)
	{
		NSString* cameraName = [NSString stringWithUTF8String:definition.url().c_str()];
		[cameras addObject:cameraName];
	}

	availableCameras_ = [cameras copy];
}

- (void)showCameraDropdown:(UIButton*)sender
{
	if (cameraSelected_)
	{
		return;
	}

	UIAlertController* alertController = [UIAlertController alertControllerWithTitle:@"Select Camera" message:nil preferredStyle:UIAlertControllerStyleActionSheet];

	for (NSString* cameraName in availableCameras_)
	{
		UIAlertAction* action = [UIAlertAction actionWithTitle:cameraName style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull selectedAction) {
				[self->cameraDropdownButton_ setTitle:cameraName forState:UIControlStateNormal];
				[self onCameraSelected:cameraName];
			}];

		[alertController addAction:action];
	}

	UIAlertAction* cancelAction = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:nil];
	[alertController addAction:cancelAction];

	// For iPad, configure popover
	if (alertController.popoverPresentationController)
	{
		alertController.popoverPresentationController.sourceView = sender;
		alertController.popoverPresentationController.sourceRect = sender.bounds;
	}

	[viewController_ presentViewController:alertController animated:YES completion:nil];
}

- (void)onCameraSelected:(NSString*)cameraName
{
	if (cameraSelected_)
	{
		return;
	}

	const std::string cameraNameStr = [cameraName UTF8String];
	liveVideo_ = Media::Manager::get().newMedium(cameraNameStr);

	if (!liveVideo_)
	{
		Log::error() << "Failed to access input medium '" << cameraNameStr << "'";
		return;
	}

	selectedCamera_ = cameraName;
	cameraSelected_ = YES;

	// Disable camera dropdown
	cameraDropdownButton_.userInteractionEnabled = NO;
	cameraDropdownButton_.alpha = 0.5;

	// Load available resolutions
	[self loadAvailableResolutions];

	// Show resolution selection
	resolutionSelectionContainer_.hidden = NO;
}

- (void)loadAvailableResolutions
{
	if (!liveVideo_)
	{
		return;
	}

	const Media::LiveVideo::StreamConfigurations streamConfigurations = liveVideo_->supportedStreamConfigurations(Media::LiveVideo::ST_FRAME);
	NSMutableArray* resolutions = [NSMutableArray array];
	NSMutableSet* uniqueResolutions = [NSMutableSet set];

	for (const Media::LiveVideo::StreamConfiguration& streamConfiguration : streamConfigurations)
	{
		NSString* resolution = [NSString stringWithFormat:@"%ux%u", streamConfiguration.width_, streamConfiguration.height_];

		if (![uniqueResolutions containsObject:resolution])
		{
			[uniqueResolutions addObject:resolution];
			[resolutions addObject:resolution];
		}
	}

	availableResolutions_ = [resolutions copy];
}

- (void)showResolutionDropdown:(UIButton*)sender
{
	if (cameraStarted_)
	{
		return;
	}

	UIAlertController* alertController = [UIAlertController alertControllerWithTitle:@"Select Resolution" message:nil preferredStyle:UIAlertControllerStyleActionSheet];

	for (NSString* resolution in availableResolutions_)
	{
		UIAlertAction* action = [UIAlertAction actionWithTitle:resolution style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull selectedAction) {
				[self->resolutionDropdownButton_ setTitle:resolution forState:UIControlStateNormal];
				[self onResolutionSelected:resolution];
			}];

		[alertController addAction:action];
	}

	UIAlertAction* cancelAction = [UIAlertAction actionWithTitle:@"Cancel" style:UIAlertActionStyleCancel handler:nil];
	[alertController addAction:cancelAction];

	// For iPad, configure popover
	if (alertController.popoverPresentationController)
	{
		alertController.popoverPresentationController.sourceView = sender;
		alertController.popoverPresentationController.sourceRect = sender.bounds;
	}

	[viewController_ presentViewController:alertController animated:YES completion:nil];
}

- (void)onResolutionSelected:(NSString*)resolution
{
	if (cameraStarted_)
	{
		return;
	}

	const std::string resolutionStr = [resolution UTF8String];

	unsigned int preferredWidth = 0u;
	unsigned int preferredHeight = 0u;
	if (Media::Utilities::parseResolution(resolutionStr, preferredWidth, preferredHeight))
	{
		if (liveVideo_->setPreferredFrameDimension(preferredWidth, preferredHeight))
		{
			Log::debug() << "Set preferred resolution " << preferredWidth << "x" << preferredHeight;
		}
		else
		{
			Log::error() << "Failed to set preferred resolution " << preferredWidth << "x" << preferredHeight;
		}
	}
	else
	{
		Log::warning() << "Failed to parse resolution '" << resolutionStr << "'";
	}

	if (!liveVideo_->start())
	{
		Log::error() << "Failed to start input medium '" << liveVideo_->url() << "'";
		return;
	}

	videoStabilizationEnabled_ = liveVideo_->videoStabilization();

	stabilizationSwitch_.on = videoStabilizationEnabled_;

	[viewController_ setFrameMedium:liveVideo_ andAdjustFov:false];

	// Create directory for pictures
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* documentsDirectory = [paths objectAtIndex:0];

	const std::string dateStr = DateTime::localStringDate('-');
	const std::string timeStr = DateTime::localStringTime(false, '-');
	NSString* directoryName = [NSString stringWithFormat:@"%s_%s", dateStr.c_str(), timeStr.c_str()];
	NSString* fullPath = [documentsDirectory stringByAppendingPathComponent:directoryName];

	directory_ = IO::Directory([fullPath UTF8String]);

	if (!directory_.exists() && !directory_.create())
	{
		Log::error() << "Failed to create directory '" << directory_() << "'";
		return;
	}

	selectedResolution_ = resolution;
	cameraStarted_ = YES;

	// Hide selection containers
	cameraSelectionContainer_.hidden = YES;
	resolutionSelectionContainer_.hidden = YES;

	// Set focus
	if (liveVideo_->setFocus(initialFocus_))
	{
		focusContainer_.hidden = NO;
	}

	// Show take image button
	takeImageButton_.hidden = NO;
	takeImageButton_.enabled = YES;
	takeImageButton_.backgroundColor = [UIColor colorWithRed:0.13 green:0.59 blue:0.95 alpha:1.0];
}

- (void)focusSliderChanged:(UISlider*)slider
{
	float focusValue = slider.value;
	currentFocus_ = focusValue;
	focusLabel_.text = [NSString stringWithFormat:@"Focus: %.2f", focusValue];
	focusLabel_.textColor = [UIColor blackColor];

	if (liveVideo_)
	{
		liveVideo_->setFocus(focusValue);
	}
}

- (void)stabilizationSwitchChanged:(UISwitch*)sender
{
	videoStabilizationEnabled_ = sender.on;

	if (liveVideo_)
	{
		liveVideo_->setVideoStabilization(videoStabilizationEnabled_);
	}
}

- (void)takeImageButtonPressed:(UIButton*)sender
{
	takeImageButton_.enabled = NO;
	takeImageButton_.backgroundColor = [UIColor colorWithRed:0.62 green:0.62 blue:0.62 alpha:1.0];
	focusContainer_.hidden = YES;

	countdownValue_ = 3;
	[self runCountdown];
}

- (void)runCountdown
{
	if (countdownValue_ >= 0)
	{
		countdownLabel_.text = [NSString stringWithFormat:@"%ld", (long)countdownValue_];
		countdownLabel_.hidden = NO;

		dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(0.5 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
			self->countdownValue_--;
			[self runCountdown];
		});
	}
	else
	{
		countdownLabel_.hidden = YES;

		takeImageButton_.enabled = YES;
		takeImageButton_.backgroundColor = [UIColor colorWithRed:0.13 green:0.59 blue:0.95 alpha:1.0];

		if ([self takePicture])
		{
			Platform::Apple::IOS::Utilities::triggerVibration();

			// Update image counter
			imageCounterLabel_.text = [NSString stringWithFormat:@"%u", pictureCounter_];
			imageCounterLabel_.hidden = NO;
		}
	}
}

- (bool)takePicture
{
	if (!liveVideo_)
	{
		return false;
	}

	const FrameRef frame = liveVideo_->frame();

	if (!frame)
	{
		return false;
	}

	if (!directory_.isValid())
	{
		return false;
	}

	// Write settings file on first image capture
	if (!settingsFileWritten_)
	{
		const IO::File settingsFile = directory_ + IO::File("camera_settings.txt");

		std::ofstream settingsStream(settingsFile());

		if (settingsStream.is_open())
		{
			settingsStream << "Camera: " << [selectedCamera_ UTF8String] << std::endl;
			settingsStream << "Resolution: " << [selectedResolution_ UTF8String] << std::endl;
			settingsStream << "Focus: " << currentFocus_ << std::endl;
			settingsStream << "Video Stabilization: " << (videoStabilizationEnabled_ ? "Enabled" : "Disabled") << std::endl;

			Log::info() << "Wrote camera settings to '" << settingsFile() << "'";
			settingsFileWritten_ = YES;
		}
		else
		{
			Log::error() << "Failed to write camera settings file";
		}
	}

	const IO::File filename = directory_ + IO::File("image_" + String::toAString(frame->width()) + "x" + String::toAString(frame->height()) + "_" + String::toAString(pictureCounter_++, 3u) + ".png");

	if (!IO::Image::Comfort::writeImage(*frame, filename(), true))
	{
		Log::error() << "Failed to write the picture";
		return false;
	}

	Log::info() << "Wrote picture to '" << filename() << "'";

	return true;
}

- (void)applicationWillTerminate:(UIApplication *)application
{
	liveVideo_.release();
}

@end
