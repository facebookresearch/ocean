/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/AVFLiveVideo.h"
#include "ocean/media/avfoundation/AVFDevices.h"
#include "ocean/media/avfoundation/AVFLibrary.h"
#include "ocean/media/avfoundation/PixelBufferAccessor.h"

#include "ocean/base/StringApple.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/system/Performance.h"

#import <Foundation/Foundation.h>

/**
 * Definition of a sample buffer delegate object.
 */
@interface AVFLiveVideoSampleBufferDelegate : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>
{
	/// The callback function for new samples
	@private Ocean::Media::AVFoundation::AVFLiveVideo::OnNewSampleCallback delegateOnNewSampleCallback;
}

/**
 * Initialize the delegate object by a given callback function for new samples.
 * @param onNewSampleCallback Callback function for new samples
 * @return The instance of the delegate object
 */
- (id) initWithCallback:(Ocean::Media::AVFoundation::AVFLiveVideo::OnNewSampleCallback)onNewSampleCallback;

/**
 * Event function for new Samples.
 * Called, whenever the capture output (this delegate object is connected to) captures and outputs a new video sample.
 * @param captureOutput The capture output object
 * @param sampleBuffer A buffer containing the sample data and additional information about the sample
 * @param connection The connection from which the video was received
 */
- (void) captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection;

@end

@implementation AVFLiveVideoSampleBufferDelegate

- (id)initWithCallback:(Ocean::Media::AVFoundation::AVFLiveVideo::OnNewSampleCallback)onNewSampleCallback
{
	if (self = [super init])
	{
		delegateOnNewSampleCallback = onNewSampleCallback;
  	}

	return self;
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
	const NSTimeInterval uptime = [NSProcessInfo processInfo].systemUptime;
	const NSTimeInterval unixTimestamp = [[NSDate date] timeIntervalSince1970];

	if (sampleBuffer)
	{
		CVPixelBufferRef frameBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
		CVPixelBufferRef pixelBuffer = CVBufferRetain(frameBuffer);
		CMTime presentationTime = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);

		if (pixelBuffer)
		{
			ocean_assert(presentationTime.timescale != 0);
			const double frameUptime = double(presentationTime.value) / double(presentationTime.timescale);

			// adusting the unix timestamp by the offset between the current up-time and the sample's up-time
			const double frameUnixTimestamp = frameUptime - double(uptime) + double(unixTimestamp);

			delegateOnNewSampleCallback(pixelBuffer, Ocean::SharedAnyCamera(), frameUnixTimestamp, frameUptime);

			CVBufferRelease(pixelBuffer);
		}
	}
}

@end

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

AVFLiveVideo::AVFLiveVideo(const std::string& url) :
	Medium(url),
	AVFMedium(url),
	FrameMedium(url),
	AVFFrameMedium(url),
	ConfigMedium(url),
	LiveMedium(url),
	LiveVideo(url)
{
	isValid_ = createCaptureDevice();
}

AVFLiveVideo::~AVFLiveVideo()
{
	stop();

	captureDevice_ = nil;
	releaseCaptureSession();
}

MediumRef AVFLiveVideo::clone() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(isValid_);
	if (isValid_)
	{
		return AVFLibrary::newLiveVideo(url_, true);
	}

	return MediumRef();
}

HomogenousMatrixD4 AVFLiveVideo::device_T_camera() const
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(device_T_camera_.isValid());
	return device_T_camera_;
}

double AVFLiveVideo::exposureDuration(double* minDuration, double* maxDuration) const
{
	const ScopedLock scopedLock(lock_);

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
	if (captureDevice_ != nullptr)
	{
		if (minDuration)
		{
			*minDuration = CMTimeGetSeconds([[captureDevice_ activeFormat] minExposureDuration]);
		}

		if (maxDuration)
		{
			*maxDuration = CMTimeGetSeconds([[captureDevice_ activeFormat] maxExposureDuration]);
		}

		const AVCaptureExposureMode captureExposureMode = [captureDevice_ exposureMode];

		if (captureExposureMode == AVCaptureExposureModeContinuousAutoExposure)
		{
			return 0.0;
		}

		const CMTime value = [captureDevice_ exposureDuration];

		if (CMTIME_IS_VALID(value))
		{
			const double exposureDuration = CMTimeGetSeconds([captureDevice_ exposureDuration]);
			ocean_assert(exposureDuration > 0.0);

			return exposureDuration;
		}

		return -1.0;
	}
	else
#endif // TARGET_OS_IPHONE
	{
		if (minDuration)
		{
			*minDuration = -1.0;
		}

		if (maxDuration)
		{
			*maxDuration = -1.0;
		}

		return -1.0;
	}
}

float AVFLiveVideo::iso(float* minISO, float* maxISO) const
{
	const ScopedLock scopedLock(lock_);

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
	if (captureDevice_ != nullptr)
	{
		if (minISO)
		{
			*minISO = [[captureDevice_ activeFormat] minISO];
		}

		if (maxISO)
		{
			*maxISO = [[captureDevice_ activeFormat] maxISO];
		}

		return [captureDevice_ ISO];
	}
	else
#endif // TARGET_OS_IPHONE
	{
		if (minISO)
		{
			*minISO = -1.0f;
		}

		if (maxISO)
		{
			*maxISO = -1.0f;
		}

		return -1.0f;
	}
}

float AVFLiveVideo::focus() const
{
	float result = -1.0f;

	const ScopedLock scopedLock(lock_);

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1

	if (captureDevice_ != nullptr)
	{
		if ([captureDevice_ focusMode] != AVCaptureFocusModeContinuousAutoFocus)
		{
			result = captureDevice_.lensPosition;
		}
	}

#endif // TARGET_OS_IPHONE

	return result;
}

bool AVFLiveVideo::setExposureDuration(const double duration)
{
	bool result = false;

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1

	const ScopedLock scopedLock(lock_);

	if (captureDevice_ != nullptr)
	{
		if ([captureDevice_ lockForConfiguration:nullptr])
		{
			if (duration <= -1.0)
			{
				// this is an intermediate solution until the interface of LiveVideo supports any possible combination of exposure/ios/white balance modes

				[captureDevice_ setExposureMode:AVCaptureExposureModeAutoExpose];
				[captureDevice_ setWhiteBalanceMode:AVCaptureWhiteBalanceModeLocked];

				result = true;
			}
			else if (duration == 0.0)
			{
				[captureDevice_ setExposureMode:AVCaptureExposureModeContinuousAutoExposure];
				result = true;
			}
			else
			{
				const CMTime exposureDuration = CMTimeMake(int64_t(duration * 1000.0 + 0.5), 1000);

				[captureDevice_ setExposureModeCustomWithDuration:exposureDuration ISO:[captureDevice_ ISO] completionHandler:nil];
				[captureDevice_ setExposureMode:AVCaptureExposureModeLocked];
				result = true;
			}

			[captureDevice_ unlockForConfiguration];
		}
	}

#endif // TARGET_OS_IPHONE

	return result;
}

bool AVFLiveVideo::setISO(const float iso)
{
	bool result = false;

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1

	const ScopedLock scopedLock(lock_);

	if (captureDevice_ != nullptr)
	{
		if ([captureDevice_ lockForConfiguration:nullptr])
		{
			if (iso <= 0.0f)
			{
				[captureDevice_ setExposureMode:AVCaptureExposureModeContinuousAutoExposure];
				result = true;
			}
			else
			{
				[captureDevice_ setExposureModeCustomWithDuration:[captureDevice_ exposureDuration] ISO:iso completionHandler:nil];
				[captureDevice_ setExposureMode:AVCaptureExposureModeLocked];
				result = true;
			}

			[captureDevice_ unlockForConfiguration];
		}
	}

#endif // TARGET_OS_IPHONE

	return result;
}

bool AVFLiveVideo::setFocus(const float position)
{
	ocean_assert(position <= 1.0f);

	bool result = false;

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1

	const ScopedLock scopedLock(lock_);

	if (captureDevice_ != nullptr)
	{
		if ([captureDevice_ lockForConfiguration:nullptr])
		{
			if (position < 0.0f)
			{
				if ([captureDevice_ isFocusModeSupported:AVCaptureFocusModeContinuousAutoFocus])
				{
					[captureDevice_ setFocusMode:AVCaptureFocusModeContinuousAutoFocus];
					result = true;
				}
				else if ([captureDevice_ isFocusModeSupported:AVCaptureFocusModeAutoFocus])
				{
					[captureDevice_ setFocusMode:AVCaptureFocusModeAutoFocus];
					result = true;
				}
				else
				{
					Log::warning() << "Failed to set focus to auto mode.";
				}
			}
			else
			{
				if (captureDevice_.isLockingFocusWithCustomLensPositionSupported)
				{
					[captureDevice_ setFocusModeLockedWithLensPosition:position completionHandler:nil];
					[captureDevice_ setFocusMode:AVCaptureFocusModeContinuousAutoFocus];
					result = true;
				}
			}

			[captureDevice_ unlockForConfiguration];
		}
	}

#endif // TARGET_OS_IPHONE

	return result;
}

void AVFLiveVideo::feedNewSample(CVPixelBufferRef pixelBuffer, SharedAnyCamera anyCamera, const double unixTimestamp, const double sampleTime)
{
	onNewSample(pixelBuffer, std::move(anyCamera), unixTimestamp, sampleTime);
}

bool AVFLiveVideo::createCaptureDevice()
{
	ocean_assert(captureDevice_ == nullptr && captureSession_ == nullptr && captureDeviceInput_ == nullptr && captureVideoDataOutput_ == nullptr && sampleBufferDelegate_ == nullptr);

	const AVFDevices::Devices devices(AVFDevices::videoDevices());

	std::wstring uniqueDeviceId;

	for (size_t n = 0; n < devices.size(); ++n)
	{
		if (devices[n].friendlyName() == String::toWString(url_))
		{
			uniqueDeviceId = devices[n].uniqueId();
			break;
		}
	}

	if (uniqueDeviceId.empty())
	{
		return false;
	}

	captureDevice_ = [AVCaptureDevice deviceWithUniqueID:StringApple::toNSString(uniqueDeviceId)];

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
	// the camera on all iOS devices is rotated by 90 degree in relation to the screen

	if (captureDevice_.position == AVCaptureDevicePositionBack)
	{
		device_T_camera_ = HomogenousMatrixD4(QuaternionD(VectorD3(0.0, 0.0, 1.0), -NumericD::pi_2()));
	}
	else if (captureDevice_.position == AVCaptureDevicePositionFront)
	{
		// the user facing camera has a different orientation than the user facing camera on Android platforms
		device_T_camera_ = HomogenousMatrixD4(QuaternionD(VectorD3(0.0, 0.0, 1.0), NumericD::pi_2()) * QuaternionD(VectorD3(0.0, 1.0, 0.0), NumericD::pi()));
	}
#else

	if (captureDevice_.position == AVCaptureDevicePositionFront)
	{
		device_T_camera_ = HomogenousMatrixD4(QuaternionD(VectorD3(0.0, 1.0, 0.0), NumericD::pi()));
	}

#endif

	ocean_assert(device_T_camera_.isValid());

	return captureDevice_ != nullptr;
}

bool AVFLiveVideo::createCaptureSession()
{
	ocean_assert(captureDevice_ != nullptr);
	ocean_assert(captureSession_ == nullptr && captureDeviceInput_ == nullptr && captureVideoDataOutput_ == nullptr && sampleBufferDelegate_ == nullptr);

	NSError* errorValue;
	captureDeviceInput_ = [[AVCaptureDeviceInput alloc] initWithDevice:captureDevice_ error:&errorValue];

	if (errorValue)
	{
		return false;
	}

	captureVideoDataOutput_ = [[AVCaptureVideoDataOutput alloc] init];
	captureVideoDataOutput_.alwaysDiscardsLateVideoFrames = YES;

	OnNewSampleCallback callback(*this, &AVFLiveVideo::onNewSample);
	sampleBufferDelegate_ = [[AVFLiveVideoSampleBufferDelegate alloc] initWithCallback:callback];

	dispatch_queue_t frameQueue = dispatch_queue_create("liveVideoFrameQueue", nullptr);
	[captureVideoDataOutput_ setSampleBufferDelegate:sampleBufferDelegate_ queue:frameQueue];

	NSArray* availablePixelFormatTypes = [captureVideoDataOutput_ availableVideoCVPixelFormatTypes];

#ifdef OCEAN_DEBUG
	const unsigned int rgbValue = (unsigned int)kCVPixelFormatType_24RGB; // 24
	const unsigned int bgrValue = (unsigned int)kCVPixelFormatType_24BGR; // 842285639

	const unsigned int rgbaValue = (unsigned int)kCVPixelFormatType_32RGBA; // 1380401729
	const unsigned int bgraValue = (unsigned int)kCVPixelFormatType_32BGRA; // 1111970369

	const unsigned int argbValue = (unsigned int) kCVPixelFormatType_32ARGB; // 32

	const unsigned int vuy2Value = kCVPixelFormatType_422YpCbCr8; // 846624121
	const unsigned int yuvsValue = kCVPixelFormatType_422YpCbCr8_yuvs; // 2037741171

	const unsigned int y_uvVideoValue = (unsigned int)kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange; // 875704438
	const unsigned int y_uvFullValue = (unsigned int)kCVPixelFormatType_420YpCbCr8BiPlanarFullRange; // 875704422

	OCEAN_SUPPRESS_UNUSED_WARNING(rgbValue);
	OCEAN_SUPPRESS_UNUSED_WARNING(bgrValue);
	OCEAN_SUPPRESS_UNUSED_WARNING(rgbaValue);
	OCEAN_SUPPRESS_UNUSED_WARNING(bgraValue);
	OCEAN_SUPPRESS_UNUSED_WARNING(argbValue);
	OCEAN_SUPPRESS_UNUSED_WARNING(vuy2Value);
	OCEAN_SUPPRESS_UNUSED_WARNING(yuvsValue);
	OCEAN_SUPPRESS_UNUSED_WARNING(y_uvVideoValue);
	OCEAN_SUPPRESS_UNUSED_WARNING(y_uvFullValue);
#endif

	// now we selected the most suitable pixel format type from the list of available formats

	NSNumber* pixelFormatType = nullptr;

	if (preferredFrameType_.pixelFormat() != FrameType::FORMAT_UNDEFINED)
	{
		for (size_t i = 0; pixelFormatType == nullptr && i < [availablePixelFormatTypes count]; ++i)
		{
			OSType availablePixelFormatType = [[availablePixelFormatTypes objectAtIndex:i] intValue];

			if (PixelBufferAccessor::translatePixelFormat(availablePixelFormatType) == preferredFrameType_.pixelFormat())
			{
				pixelFormatType = [NSNumber numberWithUnsignedInt:availablePixelFormatType];
			}
		}
	}

	if (pixelFormatType == nullptr)
	{
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
		for (const FrameType::PixelFormat& ourPreferredPixelFormat : {FrameType::FORMAT_Y_UV12, FrameType::FORMAT_RGB24, FrameType::FORMAT_BGR24, FrameType::FORMAT_RGBA32, FrameType::FORMAT_BGRA32})
#else
		for (const FrameType::PixelFormat& ourPreferredPixelFormat : {FrameType::FORMAT_RGB24, FrameType::FORMAT_BGR24, FrameType::FORMAT_RGBA32, FrameType::FORMAT_BGRA32, FrameType::FORMAT_Y_UV12})
#endif
		{
			for (size_t i = 0; pixelFormatType == nullptr && i < [availablePixelFormatTypes count]; ++i)
			{
				OSType availablePixelFormatType = [[availablePixelFormatTypes objectAtIndex:i] intValue];

				if (PixelBufferAccessor::translatePixelFormat(availablePixelFormatType) == ourPreferredPixelFormat)
				{
					pixelFormatType = [NSNumber numberWithUnsignedInt:availablePixelFormatType];
				}
			}
		}
	}

	if (pixelFormatType == nullptr)
	{
		pixelFormatType = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_24RGB];
	}

	NSDictionary* videoSettings = [NSDictionary dictionaryWithObject:pixelFormatType forKey:(NSString*)kCVPixelBufferPixelFormatTypeKey];

	[captureVideoDataOutput_ setVideoSettings:videoSettings];

	captureSession_ = [[AVCaptureSession alloc] init];

	if (![captureSession_ canAddInput:captureDeviceInput_] || ![captureSession_ canAddOutput:captureVideoDataOutput_])
	{
		return false;
	}

	// now we check whether we can set a preferred frame dimension

	NSString* sessionPreset = nullptr;
	unsigned int sessionPresetWidth = 0u;
	unsigned int sessionPresetHeight = 0u;

	if (preferredFrameType_.width() != 0u && preferredFrameType_.height() != 0u)
	{
		// we first check whether our device is able to provide the exact dimension
		sessionPreset = determineExactPreset(preferredFrameType_.width(), preferredFrameType_.height(), sessionPresetWidth, sessionPresetHeight);

		if (sessionPreset == nullptr)
		{
			sessionPreset = determineNextLargerPreset(preferredFrameType_.width(), preferredFrameType_.height(), sessionPresetWidth, sessionPresetHeight);
		}
	}

	if (sessionPreset == nullptr)
	{
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
		sessionPreset = AVCaptureSessionPreset640x480;
		sessionPresetWidth = 640u;
		sessionPresetHeight = 480u;
#else
		sessionPreset = AVCaptureSessionPresetHigh;
		sessionPresetWidth = 1920u;
		sessionPresetHeight = 1080u;
#endif
	}

	ocean_assert(sessionPreset != nullptr);
	[captureSession_ setSessionPreset:sessionPreset];

	[captureSession_ addInput:captureDeviceInput_];
	[captureSession_ addOutput:captureVideoDataOutput_];

	return true;
}

void AVFLiveVideo::releaseCaptureSession()
{
	if (captureSession_)
	{
		if (captureDeviceInput_)
		{
			[captureSession_ removeInput:captureDeviceInput_];
		}

		if (captureDeviceInput_)
		{
			[captureSession_ removeOutput:captureVideoDataOutput_];
		}
	}

	captureSession_ = nil;
	captureDeviceInput_ = nil;
	captureVideoDataOutput_ = nil;
	sampleBufferDelegate_ = nil;
}

bool AVFLiveVideo::internalStart()
{
	if (captureDevice_ == nullptr)
	{
		return false;
	}

	if (captureSession_ == nullptr)
	{
		if (!createCaptureSession())
		{
			releaseCaptureSession();
			return false;
		}
	}

	ocean_assert(captureSession_ != nullptr);
	[captureSession_ startRunning];

	return true;
}

bool AVFLiveVideo::internalPause()
{
	if (captureSession_ == nullptr)
	{
		return false;
	}

	[captureSession_ stopRunning];

	return true;
}

bool AVFLiveVideo::internalStop()
{
	if (captureSession_ == nullptr)
	{
		return false;
	}

	[captureSession_ stopRunning];

	releaseCaptureSession();

	return true;
}

void AVFLiveVideo::onNewSample(CVPixelBufferRef pixelBuffer, SharedAnyCamera anyCamera, const double unixTimestamp, const double frameTime)
{
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
	isoSum_ += [captureDevice_ ISO];

	const CMTime exposureDuration = [captureDevice_ exposureDuration];

	if (exposureDuration.timescale != CMTimeScale(0))
	{
		exposureDurationSum_ += float(exposureDuration.value) / float(exposureDuration.timescale);
	}

	isoExposureMeasurements_++;

	if (isoExposureMeasurements_ >= 400u)
	{
		isoExposureMeasurements_ = 0u;
		isoSum_ = 0.0f;
		exposureDurationSum_ = 0.0f;
	}
#endif

	const unsigned int width = (unsigned int)(CVPixelBufferGetWidth(pixelBuffer));
	const unsigned int height = (unsigned int)(CVPixelBufferGetHeight(pixelBuffer));

	if (!anyCamera)
	{
		if (!approximatedAnyCamera_ || width != approximatedAnyCamera_->width() || height != approximatedAnyCamera_->height())
		{
			const double fovX = bestMatchingFieldOfView(captureDevice_, width, height);

			if (fovX > 0.0)
			{
				approximatedAnyCamera_ = std::make_shared<AnyCameraPinhole>(PinholeCamera(width, height, Scalar(fovX)));
			}
			else
			{
				approximatedAnyCamera_ = nullptr;
			}
		}
	}

	AVFFrameMedium::onNewSample(pixelBuffer, anyCamera ? std::move(anyCamera) : approximatedAnyCamera_, unixTimestamp, frameTime);
}

NSString* AVFLiveVideo::determineExactPreset(const unsigned int width, const unsigned int height, unsigned int& presetWidth, unsigned int& presetHeight)
{
	ocean_assert(width != 0u && height != 0u);

	presetWidth = 0u;
	presetHeight = 0u;

	if (width == 352u && height == 288u)
	{
		presetWidth = width;
		presetHeight = height;
		return AVCaptureSessionPreset352x288;
	}

	if (width == 640u && height == 480u)
	{
		presetWidth = width;
		presetHeight = height;
		return AVCaptureSessionPreset640x480;
	}

	if (width == 1280u && height == 720u)
	{
		presetWidth = width;
		presetHeight = height;
		return AVCaptureSessionPreset1280x720;
	}

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
	if (width == 1920u && height == 1080u)
	{
		presetWidth = width;
		presetHeight = height;
		return AVCaptureSessionPreset1920x1080;
	}
#endif

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1 && defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && __IPHONE_OS_VERSION_MAX_ALLOWED >= 90000
	if (width == 3840u && height == 2160u)
	{
		presetWidth = width;
		presetHeight = height;
		return AVCaptureSessionPreset3840x2160;
	}
#endif

#if !defined(TARGET_OS_IPHONE) || TARGET_OS_IPHONE == 0
	if (width == 320u && height == 240u)
	{
		presetWidth = width;
		presetHeight = height;
		return AVCaptureSessionPreset320x240;
	}

	if (width == 960u && height == 540u)
	{
		presetWidth = width;
		presetHeight = height;
		return AVCaptureSessionPreset960x540;
	}
#endif

	return nullptr;
}

NSString* AVFLiveVideo::determineNextLargerPreset(const unsigned int width, const unsigned int height, unsigned int& presetWidth, unsigned int& presetHeight)
{
	ocean_assert(width != 0u && height != 0u);

	const unsigned int pixels = width * height;

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1

	#if defined(__IPHONE_OS_VERSION_MAX_ALLOWED) && __IPHONE_OS_VERSION_MAX_ALLOWED >= 90000
		if (pixels > 1920u * 1080u)
		{
			presetWidth = 3840u;
			presetHeight = 2160u;
			return AVCaptureSessionPreset3840x2160;
		}
	#endif

	if (pixels > 1280u * 720u)
	{
		presetWidth = 1920u;
		presetHeight = 1080u;
		return AVCaptureSessionPreset1920x1080;
	}

	if (pixels > 640u * 480u)
	{
		presetWidth = 1280u;
		presetHeight = 720u;
		return AVCaptureSessionPreset1280x720;
	}

	if (pixels > 352u * 288u)
	{
		presetWidth = 640u;
		presetHeight = 480u;
		return AVCaptureSessionPreset640x480;
	}

	presetWidth = 352u;
	presetHeight = 288u;
	return AVCaptureSessionPreset352x288;

#else

	if (pixels >= 1920u * 1080u)
	{
		// we do not know whether 1920x1080 is the correct dimension matching to 'PresetHigh'; however, we do not have any better choice
		presetWidth = 1920u;
		presetHeight = 1080u;
		return AVCaptureSessionPresetHigh;
	}

	if (pixels > 960u * 540u)
	{
		presetWidth = 1280u;
		presetHeight = 720u;
		return AVCaptureSessionPreset1280x720;
	}

	if (pixels > 640u * 480u)
	{
		presetWidth = 960u;
		presetHeight = 540u;
		return AVCaptureSessionPreset960x540;
	}

	if (pixels > 352u * 288u)
	{
		presetWidth = 640u;
		presetHeight = 480u;
		return AVCaptureSessionPreset640x480;
	}

	if (pixels > 320u * 240u)
	{
		presetWidth = 352u;
		presetHeight = 288u;
		return AVCaptureSessionPreset352x288;
	}

	presetWidth = 320u;
	presetHeight = 240u;
	return AVCaptureSessionPreset320x240;

#endif

}

double AVFLiveVideo::bestMatchingFieldOfView(AVCaptureDevice* device, const unsigned int width, const unsigned int height)
{
	ocean_assert(device != nullptr && width != 0u && height != 0u);

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1

	NSArray* availableFormats = [device formats];

	// in the first iteration we check for the precise frame dimension
	for (size_t n = 0; n < [availableFormats count]; ++n)
	{
		AVCaptureDeviceFormat* format = [availableFormats objectAtIndex:n];

		CMFormatDescriptionRef formatDescription = [format formatDescription];
		CMVideoDimensions videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDescription);

		const double fovX = NumericD::deg2rad(double(format.videoFieldOfView));

		if (fovX > 0.0 && width == (unsigned int)(videoDimensions.width) && height == (unsigned int)(videoDimensions.height))
		{
			return fovX;
		}
	}

	// in the second iteration we check for any frame dimension larger than our frame dimension (which may not be the ideal solution as e.g., frames with different aspect ratio may have different fov
	for (size_t n = 0; n < [availableFormats count]; ++n)
	{
		AVCaptureDeviceFormat* format = [availableFormats objectAtIndex:n];

		CMFormatDescriptionRef formatDescription = [format formatDescription];
		CMVideoDimensions videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDescription);

		const double fovX = NumericD::deg2rad(double(format.videoFieldOfView));
		const unsigned int pixels = (unsigned int)(videoDimensions.width) * (unsigned int)(videoDimensions.height);

		if (fovX > 0.0 && pixels >= width * height)
		{
			return fovX;
		}
	}

	// in the last iteration we check for any largest frame dimension
	unsigned int bestPixels = 0u;
	double bestFovX = -1.0;

	for (size_t n = 0; n < [availableFormats count]; ++n)
	{
		AVCaptureDeviceFormat* format = [availableFormats objectAtIndex:n];

		CMFormatDescriptionRef formatDescription = [format formatDescription];
		CMVideoDimensions videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDescription);

		const double fovX = NumericD::deg2rad(double(format.videoFieldOfView));
		const unsigned int pixels = (unsigned int)videoDimensions.width * (unsigned int)videoDimensions.height;

		if (fovX > 0.0 && pixels > bestPixels)
		{
			bestFovX = fovX;
			bestPixels = pixels;
		}
	}

	return bestFovX;

#endif

	return -1.0;
}

}

}

}
