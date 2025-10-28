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

using namespace Ocean;

/**
 * Definition of a sample buffer delegate object.
 */
@interface AVFLiveVideoSampleBufferDelegate : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>
{
	/// The callback function for new samples
	@private Media::AVFoundation::AVFLiveVideo::OnNewSampleCallback delegateOnNewSampleCallback;

	/// The timestamp converter between up-time and unix timestamp
	Timestamp::TimestampConverter timestampConverter_;
}

/**
 * Initialize the delegate object by a given callback function for new samples.
 * @param onNewSampleCallback Callback function for new samples
 * @return The instance of the delegate object
 */
- (id) initWithCallback:(Media::AVFoundation::AVFLiveVideo::OnNewSampleCallback)onNewSampleCallback;

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

- (id)initWithCallback:(Media::AVFoundation::AVFLiveVideo::OnNewSampleCallback)onNewSampleCallback
{
	if (self = [super init])
	{
		delegateOnNewSampleCallback = onNewSampleCallback;

		timestampConverter_ = Timestamp::TimestampConverter(Timestamp::TimestampConverter::TD_UPTIME_RAW);
  	}

	return self;
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
	if (!sampleBuffer)
	{
		return;
	}

	CVPixelBufferRef frameBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
	CVPixelBufferRef pixelBuffer = CVBufferRetain(frameBuffer);

	if (pixelBuffer)
	{
		const CMTime presentationTime = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);

		const int64_t presentationTimeNs = Timestamp::TimestampConverter::timestampInNs(presentationTime.value, presentationTime.timescale);

		const Timestamp frameUnixTimestamp = timestampConverter_.toUnix(presentationTimeNs);
		const double frameUptime = Timestamp::nanoseconds2seconds(presentationTimeNs);

		delegateOnNewSampleCallback(pixelBuffer, SharedAnyCamera(), double(frameUnixTimestamp), frameUptime);

		CVBufferRelease(pixelBuffer);
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

AVFLiveVideo::StreamTypes AVFLiveVideo::supportedStreamTypes() const
{
	const ScopedLock scopedLock(lock_);

	StreamTypes streamTypes;
	streamTypes.reserve(1);

	streamTypes.push_back(ST_FRAME);

	return streamTypes;
}

AVFLiveVideo::StreamConfigurations AVFLiveVideo::supportedStreamConfigurations(const StreamType streamType) const
{
	const ScopedLock scopedLock(lock_);

	if (streamType == ST_INVALID)
	{
		return availableStreamConfigurations_;
	}

	StreamConfigurations streamConfigurations;
	streamConfigurations.reserve(availableStreamConfigurations_.size());

	for (const StreamConfiguration& streamConfiguration : availableStreamConfigurations_)
	{
		if (streamConfiguration.streamType_ == streamType)
		{
			streamConfigurations.push_back(streamConfiguration);
		}
	}

	return streamConfigurations;
}

double AVFLiveVideo::exposureDuration(double* minDuration, double* maxDuration, ControlMode* exposureMode) const
{
	const ScopedLock scopedLock(lock_);

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
	if (captureDevice_ != nullptr)
	{
		if (minDuration != nullptr)
		{
			*minDuration = CMTimeGetSeconds([[captureDevice_ activeFormat] minExposureDuration]);
		}

		if (maxDuration != nullptr)
		{
			*maxDuration = CMTimeGetSeconds([[captureDevice_ activeFormat] maxExposureDuration]);
		}

		if (exposureMode != nullptr)
		{
			const AVCaptureExposureMode captureExposureMode = [captureDevice_ exposureMode];

			if (captureExposureMode == AVCaptureExposureModeContinuousAutoExposure)
			{
				*exposureMode = CM_DYNAMIC;
			}
			else
			{
				// even AVCaptureExposureModeAutoExpose is interpreted as a fixed exposure mode as it is fixed after the first exposure adjustment
				*exposureMode = CM_FIXED;
			}
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
		if (minDuration != nullptr)
		{
			*minDuration = -1.0;
		}

		if (maxDuration != nullptr)
		{
			*maxDuration = -1.0;
		}

		if (exposureMode != nullptr)
		{
			*exposureMode = CM_INVALID;
		}

		return -1.0;
	}
}

float AVFLiveVideo::iso(float* minISO, float* maxISO, ControlMode* isoMode) const
{
	const ScopedLock scopedLock(lock_);

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
	if (captureDevice_ != nullptr)
	{
		if (minISO != nullptr)
		{
			*minISO = [[captureDevice_ activeFormat] minISO];
		}

		if (maxISO != nullptr)
		{
			*maxISO = [[captureDevice_ activeFormat] maxISO];
		}

		if (isoMode != nullptr)
		{
			const AVCaptureExposureMode captureExposureMode = [captureDevice_ exposureMode];

			if (captureExposureMode == AVCaptureExposureModeContinuousAutoExposure)
			{
				*isoMode = CM_DYNAMIC;
			}
			else
			{
				*isoMode = CM_FIXED;
			}
		}

		return [captureDevice_ ISO];
	}
	else
#endif // TARGET_OS_IPHONE
	{
		if (minISO != nullptr)
		{
			*minISO = -1.0f;
		}

		if (maxISO != nullptr)
		{
			*maxISO = -1.0f;
		}

		if (isoMode != nullptr)
		{
			*isoMode = CM_INVALID;
		}

		return -1.0f;
	}
}

float AVFLiveVideo::focus(ControlMode* focusMode) const
{
	float result = -1.0f;
	ControlMode resultFocusMode = CM_INVALID;

	const ScopedLock scopedLock(lock_);

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1

	if (captureDevice_ != nullptr)
	{
		result = captureDevice_.lensPosition;

		if ([captureDevice_ focusMode] == AVCaptureFocusModeContinuousAutoFocus)
		{
			resultFocusMode = CM_DYNAMIC;
		}
		else
		{
			resultFocusMode = CM_FIXED;
		}
	}

#endif // TARGET_OS_IPHONE

	if (focusMode != nullptr)
	{
		*focusMode = resultFocusMode;
	}

	return result;
}

bool AVFLiveVideo::setPreferredStreamType(const StreamType streamType)
{
	return false;
}

bool AVFLiveVideo::setPreferredStreamConfiguration(const StreamConfiguration& streamConfiguration)
{
	ocean_assert(streamConfiguration.isValid());
	if (!streamConfiguration.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	return setPreferredFrameDimension(streamConfiguration.width_, streamConfiguration.height_) && setPreferredFramePixelFormat(streamConfiguration.framePixelFormat_);
}

bool AVFLiveVideo::setExposureDuration(const double duration, const bool allowShorterExposure)
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
				// -1 for a one-time auto exposure

				[captureDevice_ setExposureMode:AVCaptureExposureModeAutoExpose];
				[captureDevice_ setWhiteBalanceMode:AVCaptureWhiteBalanceModeLocked];

				exposureDuration_ = -1.0;

				result = true;
			}
			else if (duration == 0.0 || allowShorterExposure)
			{
				// 0 for auto exposure

				[captureDevice_ setExposureMode:AVCaptureExposureModeContinuousAutoExposure];

				if (duration > 0.0)
				{
					ocean_assert(allowShorterExposure);

					const CMTime exposureDuration = CMTimeMakeWithSeconds(duration, 1000 * 1000 * 10);

					[captureDevice_ setActiveMaxExposureDuration:exposureDuration];
				}

				exposureDuration_ = 0.0;

				result = true;
			}
			else
			{
				const CMTime exposureDuration = CMTimeMakeWithSeconds(duration, 1000 * 1000 * 10);

				[captureDevice_ setExposureMode:AVCaptureExposureModeLocked];

				float iso = AVCaptureISOCurrent;

				if (iso_ > 0.0f)
				{
					iso = iso_;
				}

				[captureDevice_ setExposureModeCustomWithDuration:exposureDuration ISO:iso completionHandler:nil];

				exposureDuration_ = duration;

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

				iso_ = -1.0f;

				result = true;
			}
			else
			{
				[captureDevice_ setExposureMode:AVCaptureExposureModeLocked];

				CMTime exposureDuration = AVCaptureExposureDurationCurrent;

				if (exposureDuration_ > 0.0)
				{
					exposureDuration = CMTimeMakeWithSeconds(exposureDuration_, 1000 * 1000 * 10);
				}

				[captureDevice_ setExposureModeCustomWithDuration:exposureDuration ISO:iso completionHandler:nil];

				iso_ = iso;

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
					[captureDevice_ setFocusMode:AVCaptureFocusModeLocked];
					[captureDevice_ setFocusModeLockedWithLensPosition:position completionHandler:nil];

					result = true;
				}
			}

			[captureDevice_ unlockForConfiguration];
		}
	}

#endif // TARGET_OS_IPHONE

	return result;
}

bool AVFLiveVideo::videoStabilization() const
{
	const ScopedLock scopedLock(lock_);

	return videoStabilizationEnabled_;
}

bool AVFLiveVideo::setVideoStabilization(const bool enable)
{
	const ScopedLock scopedLock(lock_);

	videoStabilizationEnabled_ = enable;

	// If the capture session is already running, we need to update the connection settings
	if (captureVideoDataOutput_ != nullptr && captureSession_ != nullptr)
	{
		AVCaptureConnection* connection = [captureVideoDataOutput_ connectionWithMediaType:AVMediaTypeVideo];
		if (connection != nullptr)
		{
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
			if ([connection isVideoStabilizationSupported])
			{
				if (@available(iOS 8.0, *))
				{
					connection.preferredVideoStabilizationMode = enable ? AVCaptureVideoStabilizationModeStandard : AVCaptureVideoStabilizationModeOff;
					return true;
				}
			}
#endif
		}

		return false;
	}

	// If the capture session is not yet created, the setting will be applied when it is created
	return true;
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
		if (StringApple::toAString(devices[n].friendlyName()) == url_)
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

	if (captureDevice_ != nullptr)
	{
		availableStreamConfigurations_ = determineAvailableStreamConfigurations();

#ifdef OCEAN_DEBUG
		Log::debug() << "The camera device '" << url() << "' provides the following " << availableStreamConfigurations_.size() << " stream configurations:";

		for (const StreamConfiguration& streamConfiguration : availableStreamConfigurations_)
		{
			Log::debug() << streamConfiguration.toString();
		}
#endif
	}

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

	captureSession_ = [[AVCaptureSession alloc] init];

	if (![captureSession_ canAddInput:captureDeviceInput_])
	{
		return false;
	}

	[captureSession_ addInput:captureDeviceInput_];

	double explicitFrameRate = -1.0;
	AVCaptureDeviceFormat* captureDeviceFormat = bestMatchingCaptureDeviceFormat(captureDevice_, preferredFrameType_.width(), preferredFrameType_.height(), preferredFrameType_.frequency(), preferredFrameType_.pixelFormat(), explicitFrameRate);

	if (captureDeviceFormat == nullptr)
	{
		return false;
	}

	if ([captureDevice_ lockForConfiguration:nullptr])
	{
		[captureDevice_ setActiveFormat:captureDeviceFormat];

		if (explicitFrameRate > 0.0)
		{
			const int frameRate = NumericD::round32(explicitFrameRate);
			[captureDevice_ setActiveVideoMinFrameDuration:CMTimeMake(1, frameRate)];
			[captureDevice_ setActiveVideoMaxFrameDuration:CMTimeMake(1, frameRate)];
		}

		[captureDevice_ unlockForConfiguration];
	}
	else
	{
		Log::error() << "AVFLiveVideo: Failed to lock device for configuration";
		return false;
	}

	AVCaptureDeviceFormat* activeFormat = [captureDevice_ activeFormat];

	CMFormatDescriptionRef activeFormatDescription = [activeFormat formatDescription];

#ifdef OCEAN_DEBUG
	{
		CMVideoDimensions videoDimensions = CMVideoFormatDescriptionGetDimensions(activeFormatDescription);

		const unsigned int width = (unsigned int)(videoDimensions.width);
		const unsigned int height = (unsigned int)(videoDimensions.height);

		const FourCharCode fourCharCode = CMFormatDescriptionGetMediaSubType(activeFormatDescription);
		const FrameType::PixelFormat pixelFormat = PixelBufferAccessor::translatePixelFormat(fourCharCode);

		Log::debug() << "AVFLiveVideo: Active format: " << width << "x" << height << ", " << FrameType::translatePixelFormat(pixelFormat) << ", " << [captureDevice_ activeVideoMinFrameDuration].timescale << " fps";
	}
#endif // OCEAN_DEBUG

	captureVideoDataOutput_ = [[AVCaptureVideoDataOutput alloc] init];
	captureVideoDataOutput_.alwaysDiscardsLateVideoFrames = YES;

	NSString* key = (NSString*)kCVPixelBufferPixelFormatTypeKey;
	NSNumber* value = [NSNumber numberWithUnsignedInt:CMFormatDescriptionGetMediaSubType(activeFormatDescription)];
	NSDictionary* videoSettings = [NSDictionary dictionaryWithObject:value forKey:key];
	[captureVideoDataOutput_ setVideoSettings:videoSettings];

	OnNewSampleCallback callback(*this, &AVFLiveVideo::onNewSample);
	sampleBufferDelegate_ = [[AVFLiveVideoSampleBufferDelegate alloc] initWithCallback:callback];

	dispatch_queue_t frameQueue = dispatch_queue_create("liveVideoFrameQueue", nullptr);
	[captureVideoDataOutput_ setSampleBufferDelegate:sampleBufferDelegate_ queue:frameQueue];

	if (![captureSession_ canAddOutput:captureVideoDataOutput_])
	{
		return false;
	}

	[captureSession_ addOutput:captureVideoDataOutput_];

	// Configure video stabilization based on the current setting
	AVCaptureConnection* connection = [captureVideoDataOutput_ connectionWithMediaType:AVMediaTypeVideo];
	if (connection != nullptr)
	{
#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
		if ([connection isVideoStabilizationSupported])
		{
			if (@available(iOS 8.0, *))
			{
				connection.preferredVideoStabilizationMode = videoStabilizationEnabled_ ? AVCaptureVideoStabilizationModeStandard : AVCaptureVideoStabilizationModeOff;
			}
		}
#endif
	}

#ifdef OCEAN_DEBUG
	{
		AVCaptureDeviceFormat* format = [captureDevice_ activeFormat];

		CMFormatDescriptionRef formatDescription = [format formatDescription];
		CMVideoDimensions videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDescription);

		const unsigned int width = (unsigned int)(videoDimensions.width);
		const unsigned int height = (unsigned int)(videoDimensions.height);

		const FourCharCode fourCharCode = CMFormatDescriptionGetMediaSubType(formatDescription);
		const FrameType::PixelFormat pixelFormat = PixelBufferAccessor::translatePixelFormat(fourCharCode);

		Log::debug() << "AVFLiveVideo: Active format after output connection: " << width << "x" << height << ", " << FrameType::translatePixelFormat(pixelFormat) << ", " << [captureDevice_ activeVideoMinFrameDuration].timescale << " fps";
	}
#endif // OCEAN_DEBUG

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

		if (captureVideoDataOutput_)
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

AVCaptureDeviceFormat* AVFLiveVideo::bestMatchingCaptureDeviceFormat(AVCaptureDevice* captureDevice, const unsigned int preferredWidth, const unsigned int preferredHeight, const double preferredFrameFrequency, const FrameType::PixelFormat preferredPixelFormat, double& explicitFrameRate)
{
	ocean_assert(captureDevice != nullptr);
	if (captureDevice == nullptr)
	{
		return nullptr;
	}

	unsigned int targetWidth = preferredWidth;
	unsigned int targetHeight = preferredHeight;

	if (targetWidth == 0u && targetHeight == 0u)
	{
		targetWidth = 1280u;
		targetHeight = 720u;
	}

	double targetFrameFrequency = preferredFrameFrequency;
	FrameType::PixelFormat targetPixelFormat = preferredPixelFormat;

	NSArray* availableFormats = [captureDevice formats];

	if ([availableFormats count] == 0)
	{
		return nullptr;
	}

	constexpr unsigned int maxIterations = 4u;

	for (unsigned int iteration = 0u; iteration < maxIterations; ++iteration)
	{
		if (iteration == 0u)
		{
			// we are looking for the exact match
		}
		else if (iteration == 1u)
		{
			// we need to relax the target pixel format
			if (targetPixelFormat == FrameType::FORMAT_UNDEFINED)
			{
				continue;
			}

			targetPixelFormat = FrameType::FORMAT_UNDEFINED;
		}
		else if (iteration == 2u)
		{
			// we need to relax the target frame rate
			if (targetFrameFrequency <= 0.0)
			{
				continue;
			}

			targetFrameFrequency = -1.0;
		}
		else if (iteration == 3u)
		{
			if (targetWidth == 0u && targetHeight == 0u)
			{
				continue;
			}

			targetWidth = 1280u;
			targetHeight = 720u;
		}
		else
		{
			ocean_assert(false && "This should never happen!");
			break;
		}

		for (size_t nFormat = 0; nFormat < [availableFormats count]; ++nFormat)
		{
			AVCaptureDeviceFormat* format = [availableFormats objectAtIndex:nFormat];

			CMFormatDescriptionRef formatDescription = [format formatDescription];
			CMVideoDimensions videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDescription);

			const unsigned int width = (unsigned int)(videoDimensions.width);
			const unsigned int height = (unsigned int)(videoDimensions.height);

			if (targetWidth != 0u && targetWidth != width)
			{
				continue;
			}

			if (targetHeight != 0u && targetHeight != height)
			{
				continue;
			}

			const FourCharCode fourCharCode = CMFormatDescriptionGetMediaSubType(formatDescription);

			const FrameType::PixelFormat pixelFormat = PixelBufferAccessor::translatePixelFormat(fourCharCode);

			if (pixelFormat == FrameType::FORMAT_UNDEFINED)
			{
				continue;
			}

			if (targetPixelFormat != FrameType::FORMAT_UNDEFINED && targetPixelFormat != pixelFormat)
			{
				continue;
			}

			if (targetFrameFrequency > 0.0)
			{
				NSArray* frameRateRanges = [format videoSupportedFrameRateRanges];

				for (size_t frameRateIndex = 0; frameRateIndex < [frameRateRanges count]; ++frameRateIndex)
				{
					AVFrameRateRange* frameRateRange = [frameRateRanges objectAtIndex:frameRateIndex];

					const double minFrameRate = [frameRateRange minFrameRate];
					const double maxFrameRate = [frameRateRange maxFrameRate];

					if (minFrameRate <= targetFrameFrequency && targetFrameFrequency <= maxFrameRate)
					{
						explicitFrameRate = targetFrameFrequency;
						break;
					}
				}
			}

			Log::debug() << "AVFLiveVideo: Best matching capture device format with index " << nFormat << ": " << width << "x" << height << ", " << FrameType::translatePixelFormat(pixelFormat);

			return format;
		}
	}

	Log::debug() << "AVFLiveVideo: No matching format found, using first format";

	explicitFrameRate = -1.0;
	return [availableFormats objectAtIndex:0];
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

AVFLiveVideo::StreamConfigurations AVFLiveVideo::determineAvailableStreamConfigurations() const
{
	StreamConfigurations streamConfigurations;

	if (captureDevice_ == nullptr)
	{
		return streamConfigurations;
	}

	NSArray* availableFormats = [captureDevice_ formats];

	using ConfigurationMap = std::unordered_map<StreamProperty, std::vector<double>, StreamProperty::Hash>;
	ConfigurationMap configurationMap;

	for (size_t nFormat = 0; nFormat < [availableFormats count]; ++nFormat)
	{
		AVCaptureDeviceFormat* format = [availableFormats objectAtIndex:nFormat];

		CMFormatDescriptionRef formatDescription = [format formatDescription];
		CMVideoDimensions videoDimensions = CMVideoFormatDescriptionGetDimensions(formatDescription);

		const unsigned int width = (unsigned int)(videoDimensions.width);
		const unsigned int height = (unsigned int)(videoDimensions.height);

		const FourCharCode fourCharCode = CMFormatDescriptionGetMediaSubType(formatDescription);

		const FrameType::PixelFormat pixelFormat = PixelBufferAccessor::translatePixelFormat(fourCharCode);

		if (pixelFormat == FrameType::FORMAT_UNDEFINED)
		{
			Log::debug() << "AVFLiveVideo: Unknown pixel format type: " << fourCharCode;

			continue;
		}

		StreamProperty streamProperty(ST_FRAME, width, height, pixelFormat, CT_INVALID);

		NSArray* frameRateRanges = [format videoSupportedFrameRateRanges];

		for (size_t frameRateIndex = 0; frameRateIndex < [frameRateRanges count]; ++frameRateIndex)
		{
			AVFrameRateRange* frameRateRange = [frameRateRanges objectAtIndex:frameRateIndex];

			const double minFrameRate = [frameRateRange minFrameRate];
			const double maxFrameRate = [frameRateRange maxFrameRate];

			std::vector<double>& frameRates = configurationMap[streamProperty];

			if (std::find(frameRates.begin(), frameRates.end(), minFrameRate) == frameRates.end())
			{
				frameRates.push_back(minFrameRate);
			}

			if (std::find(frameRates.begin(), frameRates.end(), maxFrameRate) == frameRates.end())
			{
				frameRates.push_back(maxFrameRate);
			}
		}
	}

	streamConfigurations.reserve(configurationMap.size());

	for (ConfigurationMap::const_iterator i = configurationMap.cbegin(); i != configurationMap.cend(); ++i)
	{
		std::vector<double> frameRates = i->second;
		std::sort(frameRates.begin(), frameRates.end());

		streamConfigurations.emplace_back(i->first, std::move(frameRates));
	}

	return streamConfigurations;
}

}

}

}
