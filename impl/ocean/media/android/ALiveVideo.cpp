/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/ALiveVideo.h"

#include "ocean/base/String.h"

#include "ocean/cv/FrameConverterY_U_V12.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/platform/android/NativeInterfaceManager.h"
#include "ocean/platform/android/Utilities.h"

namespace Ocean
{

namespace Media
{

namespace Android
{

#ifdef OCEAN_MEDIA_ANDROID_NATIVECAMERALIBRARY_AVAILABLE

std::string ALiveVideo::Device::readableLensFacing(const acamera_metadata_enum_android_lens_facing_t lensFacing)
{
	switch (lensFacing)
	{
		case ACAMERA_LENS_FACING_FRONT:
			return "Front-facing";

		case ACAMERA_LENS_FACING_BACK:
			return "Back-facing";

		case ACAMERA_LENS_FACING_EXTERNAL:
			return "External";
	}

	ocean_assert(false && "Invalid lens facing!");
	return "Unknown";
}

ALiveVideo::ALiveVideo(const std::string& url, const std::string& id) :
	Medium(url),
	AMedium(url),
	FrameMedium(url),
	ConfigMedium(url),
	LiveMedium(url),
	LiveVideo(url)
{
	libraryName_ = nameAndroidLibrary();

	isValid_ = false;

	if (NativeMediaLibrary::get().isInitialized() && NativeCameraLibrary::get().isInitialized())
	{
		NativeCameraLibrary::ScopedACameraManager cameraManager(NativeCameraLibrary::get().ACameraManager_create());

		if (cameraManager.isValid())
		{
			FrameType frameType;
			StreamConfigurations streamConfigurations;
			HomogenousMatrixD4 device_T_camera(false);

			cameraId_ = cameraIdForMedium(*cameraManager, url_, id, preferredFrameType_, frameType, &streamConfigurations, &device_T_camera);

			if (!cameraId_.empty())
			{
				// we have a valid camera id, this is enough to rate this medium as valid

				frameCollection_ = FrameCollection(10);
				availableStreamConfigurations_ = std::move(streamConfigurations);
				device_T_camera_ = device_T_camera;

				isValid_ = true;
			}
			else
			{
				Log::error() << "Failed to determine camera id for medium '" << url_ << "'";
			}
		}
	}
}

ALiveVideo::~ALiveVideo()
{
	release();

	if (sessionCurrentlyClosing_)
	{
		// the session is currently closing, so we wait until the session is closed

		const Timestamp startTimestamp(true);

		while (sessionCurrentlyClosing_)
		{
			if (startTimestamp.hasTimePassed(5.0))
			{
				Log::info() << "ALiveVideo::~ALiveVideo(): Waiting for session to be closed timed out";
				break;
			}

			Thread::sleep(1u);
		}
	}
}

bool ALiveVideo::isStarted() const
{
	const ScopedLock scopedLock(lock_);

	return startTimestamp_.isValid();
}

Timestamp ALiveVideo::startTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(startTimestamp_);

	return timestamp;
}

Timestamp ALiveVideo::pauseTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(pauseTimestamp_);

	return timestamp;
}

Timestamp ALiveVideo::stopTimestamp() const
{
	const ScopedLock scopedLock(lock_);

	Timestamp timestamp(stopTimestamp_);

	return timestamp;
}

HomogenousMatrixD4 ALiveVideo::device_T_camera() const
{
	ocean_assert(device_T_camera_.isValid());
	return device_T_camera_;
}

ALiveVideo::StreamTypes ALiveVideo::supportedStreamTypes() const
{
	const ScopedLock scopedLock(lock_);

	bool frameIsSupported = false;
	bool mjpegIsSupported = false;
	constexpr bool codedIsSupported = false;

	for (const StreamConfiguration& availableStreamConfiguration : availableStreamConfigurations_)
	{
		if (availableStreamConfiguration.streamType_ == ST_FRAME)
		{
			frameIsSupported = true;
		}
		else if (availableStreamConfiguration.streamType_ == ST_MJPEG)
		{
			mjpegIsSupported = true;
		}
	}

	StreamTypes streamTypes;
	streamTypes.reserve(2);

	if (frameIsSupported)
	{
		streamTypes.push_back(ST_FRAME);
	}

	if (mjpegIsSupported)
	{
		streamTypes.push_back(ST_MJPEG);
	}

	if (codedIsSupported)
	{
		streamTypes.push_back(ST_CODEC);
	}

	return streamTypes;
}

ALiveVideo::StreamConfigurations ALiveVideo::supportedStreamConfigurations(const StreamType streamType) const
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

double ALiveVideo::exposureDuration(double* minDuration, double* maxDuration, ControlMode* exposureMode) const
{
	const ScopedLock scopedLock(lock_);

	if (minDuration != nullptr)
	{
		*minDuration = exposureDurationMin_;
	}

	if (maxDuration != nullptr)
	{
		*maxDuration = exposureDurationMax_;
	}

	if (exposureMode != nullptr)
	{
		*exposureMode = exposureMode_;
	}

	return exposureDuration_;
}

float ALiveVideo::iso(float* minISO, float* maxISO, ControlMode* isoMode) const
{
	const ScopedLock scopedLock(lock_);

	if (minISO != nullptr)
	{
		*minISO = isoMin_;
	}

	if (maxISO != nullptr)
	{
		*maxISO = isoMax_;
	}

	if (isoMode != nullptr)
	{
		*isoMode = isoMode_;
	}

	return iso_;
}

float ALiveVideo::focus(ControlMode* focusMode) const
{
	const ScopedLock scopedLock(lock_);

	if (focusMode != nullptr)
	{
		*focusMode = focusMode_;
	}

	return focusPosition_;
}

bool ALiveVideo::setPreferredStreamType(const StreamType streamType)
{
	return false;
}

bool ALiveVideo::setPreferredStreamConfiguration(const StreamConfiguration& streamConfiguration)
{
	ocean_assert(streamConfiguration.isValid());
	if (!streamConfiguration.isValid())
	{
		return false;
	}

	const ScopedLock scopedLock(lock_);

	return setPreferredFrameDimension(streamConfiguration.width_, streamConfiguration.height_) && setPreferredFramePixelFormat(streamConfiguration.framePixelFormat_);
}

bool ALiveVideo::setExposureDuration(const double duration, const bool /*allowShorterExposure*/)
{
	const ScopedLock scopedLock(lock_);

	if (duration == exposureDuration_)
	{
		return true;
	}

	if (exposureDurationMin_ != -1.0 && exposureDurationMax_ != -1.0)
	{
		if (duration < exposureDurationMin_ || duration > exposureDurationMax_)
		{
			return false;
		}
	}

	if (captureRequest_ == nullptr)
	{
		return false;
	}

	const bool needsRestart = startTimestamp_.isValid();

	if (needsRestart)
	{
		stop();
	}

	if (duration > 0.0)
	{
		uint8_t mode =  ACAMERA_CONTROL_MODE_OFF;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_MODE, 1, &mode) != ACAMERA_OK)
		{
			return false;
		}

		mode = ACAMERA_CONTROL_AE_MODE_OFF;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_AE_MODE, 1, &mode) != ACAMERA_OK)
		{
			return false;
		}

		const int64_t exposureDurationNS = Timestamp::seconds2nanoseconds(duration);
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_i64(captureRequest_, ACAMERA_SENSOR_EXPOSURE_TIME, 1, &exposureDurationNS) != ACAMERA_OK)
		{
			return false;
		}
	}
	else
	{
		uint8_t mode = ACAMERA_CONTROL_MODE_AUTO;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_MODE, 1, &mode) != ACAMERA_OK)
		{
			return false;
		}

		mode = ACAMERA_CONTROL_AE_MODE_ON;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_AE_MODE, 1, &mode) != ACAMERA_OK)
		{
			return false;
		}
	}

	return !needsRestart || start();
}

bool ALiveVideo::setISO(const float iso)
{
	const ScopedLock scopedLock(lock_);

	if (iso == iso_)
	{
		return true;
	}

	if (isoMin_ != -1.0 && isoMax_ != -1.0)
	{
		if (iso < isoMin_ || iso > isoMax_)
		{
			return false;
		}
	}

	if (captureRequest_ == nullptr)
	{
		return false;
	}

	const bool needsRestart = startTimestamp_.isValid();

	if (needsRestart)
	{
		stop();
	}

	if (iso > 0.0f)
	{
		uint8_t mode =  ACAMERA_CONTROL_MODE_OFF;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_MODE, 1, &mode) != ACAMERA_OK)
		{
			return false;
		}

		mode = ACAMERA_CONTROL_AE_MODE_OFF;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_AE_MODE, 1, &mode) != ACAMERA_OK)
		{
			return false;
		}

		const int32_t isoValue = int32_t(iso);
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_i32(captureRequest_, ACAMERA_SENSOR_SENSITIVITY, 1, &isoValue) != ACAMERA_OK)
		{
			return false;
		}
	}
	else
	{
		uint8_t mode = ACAMERA_CONTROL_MODE_AUTO;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_MODE, 1, &mode) != ACAMERA_OK)
		{
			return false;
		}

		mode = ACAMERA_CONTROL_AE_MODE_ON;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_AE_MODE, 1, &mode) != ACAMERA_OK)
		{
			return false;
		}
	}

	return !needsRestart || start();
}

bool ALiveVideo::setFocus(const float position)
{
	const ScopedLock scopedLock(lock_);

	if (focusPosition_ == position)
	{
		return true;
	}

	if (captureRequest_ == nullptr)
	{
		return false;
	}

	if (position >= 0.0f && position <= 1.0f)
	{
		if (focusPositionMin_ == -1.0f)
		{
			return false;
		}

		const uint8_t mode = ACAMERA_CONTROL_AF_MODE_OFF;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_AF_MODE, 1, &mode) != ACAMERA_OK)
		{
			return false;
		}

		// lens focus distance = [0, focusPositionMin_] ~ [infinity, closest]

		const float focusValue = (1.0f - position) * focusPositionMin_;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_float(captureRequest_, ACAMERA_LENS_FOCUS_DISTANCE, 1, &focusValue) != ACAMERA_OK)
		{
			return false;
		}
	}
	else
	{
		const uint8_t mode = ACAMERA_CONTROL_AF_MODE_CONTINUOUS_VIDEO;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_AF_MODE, 1, &mode) != ACAMERA_OK)
		{
			return false;
		}
	}

	return setRepeatingRequest();
}

bool ALiveVideo::videoStabilization() const
{
	const ScopedLock scopedLock(lock_);

	return videoStabilizationEnabled_;
}

bool ALiveVideo::setVideoStabilization(const bool enable)
{
	const ScopedLock scopedLock(lock_);

	if (videoStabilizationEnabled_ == enable)
	{
		return true;
	}

	videoStabilizationEnabled_ = enable;

	if (captureRequest_ == nullptr)
	{
		// The setting will be applied when the capture session is created
		return true;
	}

	// Apply the video stabilization setting to the capture request
	const uint8_t mode = enable ? ACAMERA_CONTROL_VIDEO_STABILIZATION_MODE_ON : ACAMERA_CONTROL_VIDEO_STABILIZATION_MODE_OFF;
	if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_VIDEO_STABILIZATION_MODE, 1, &mode) != ACAMERA_OK)
	{
		return false;
	}

	// Update the repeating request if the capture session is running
	if (startTimestamp_.isValid())
	{
		return setRepeatingRequest();
	}

	return true;
}

void ALiveVideo::feedNewFrame(Frame&& frame, SharedAnyCamera&& anyCamera, const ControlMode exposureMode, const double exposureDuration, const ControlMode isoMode, const float iso, const ControlMode focusMode, const float focusValue)
{
	// several parameters are unknown in case the camera is fed from an external source

	TemporaryScopedLock scopedLock(lock_);

		exposureMode_ = exposureMode;
		exposureDuration_ = exposureDuration;

		isoMode_ = isoMode;
		iso_ = iso;

		focusMode_ = focusMode;

		if (focusValue != -1.0f && focusPositionMin_ > NumericF::eps())
		{
			ocean_assert(focusValue >= 0.0f && focusValue <= focusPositionMin_);

			focusPosition_ = 1.0f - focusValue / focusPositionMin_;
		}
		else
		{
			focusPosition_ = -1.0f;
		}

	scopedLock.release();

	onNewFrame(std::move(frame), std::move(anyCamera));
}

bool ALiveVideo::forceRestart()
{
	const ScopedLock scopedLock(lock_);

	if (captureSession_ == nullptr || captureRequest_ == nullptr)
	{
		return false;
	}

	if (startTimestamp_.isInvalid())
	{
		return false;
	}

	const camera_status_t stopStatus = NativeCameraLibrary::get().ACameraCaptureSession_stopRepeating(captureSession_);

	if (stopStatus != ACAMERA_OK && stopStatus != ACAMERA_ERROR_SESSION_CLOSED)
	{
		Log::error() << "ALiveVideo: Failed to stop capture session, error " << int(stopStatus);
		return false;
	}

	releaseCaptureSession();
	releaseImageReader();
	releaseCamera();

	if (!initialize())
	{
		Log::error() << "ALiveVideo: Failed to re-initialize capture session";
		return false;
	}

	if (!setRepeatingRequest())
	{
		Log::error() << "ALiveVideo: Failed to restart capture session";
		return false;
	}

	return true;
}

bool ALiveVideo::start()
{
	const ScopedLock scopedLock(lock_);

	if (cameraDevice_ == nullptr)
	{
		if (!initialize())
		{
			return false;
		}
	}

	if (captureSession_ == nullptr || captureRequest_ == nullptr)
	{
		return false;
	}

	if (startTimestamp_.isValid())
	{
		return true;
	}

	return setRepeatingRequest();
}

bool ALiveVideo::pause()
{
	// Android cameras devices cannot be paused
	return false;
}

bool ALiveVideo::stop()
{
	const ScopedLock scopedLock(lock_);

	if (captureSession_ == nullptr)
	{
		return false;
	}

	const camera_status_t stopStatus = NativeCameraLibrary::get().ACameraCaptureSession_stopRepeating(captureSession_);

	if (stopStatus != ACAMERA_OK && stopStatus != ACAMERA_ERROR_SESSION_CLOSED)
	{
		Log::error() << "ALiveVideo: Failed to stop capture session, error " << int(stopStatus);
		return false;
	}

	releaseCaptureSession();
	releaseImageReader();
	releaseCamera();

	return true;
}

bool ALiveVideo::initialize()
{
	const ScopedLock scopedLock(lock_);

	FrameType frameType;
	if (createCamera(frameType))
	{
		if (createImageReader(frameType))
		{
			if (createCaptureSession())
			{
				return true;
			}
		}
	}

	releaseCaptureSession();
	releaseImageReader();
	releaseCamera();

	return false;
}

bool ALiveVideo::release()
{
	const ScopedLock scopedLock(lock_);

	releaseCaptureSession();
	releaseImageReader();

	return releaseCamera();
}

bool ALiveVideo::createCamera(FrameType& frameType)
{
	NativeCameraLibrary::ScopedACameraManager cameraManager(NativeCameraLibrary::get().ACameraManager_create());

	if (!cameraManager.isValid())
	{
		return false;
	}

	HomogenousMatrixD4 device_T_camera(false);
	std::string cameraId = cameraIdForMedium(*cameraManager, url_, cameraId_, preferredFrameType_, frameType, nullptr, &device_T_camera);

	if (cameraId.empty())
	{
		return false;
	}

	if (cameraExposureDurationRange(*cameraManager, cameraId, exposureDurationMin_, exposureDurationMax_))
	{
		Log::debug() << "camera " << cameraId << ", Exposure duration range [" << exposureDurationMin_ * 1000.0 << ", " << exposureDurationMax_ * 1000.0 << "]ms";
	}

	if (cameraISORange(*cameraManager, cameraId, isoMin_, isoMax_))
	{
		Log::debug() << "camera " << cameraId << ", ISO range [" << isoMin_ << ", " << isoMax_ << "]";
	}

	ControlModes availableFocusModes;
	if (cameraAvailableFocusModes(*cameraManager, cameraId, availableFocusModes, focusPositionMin_))
	{
		Log::debug() << "camera " << cameraId << ", Min focus distance value " << focusPositionMin_ << ", which corresponds to a supported closest object in " << NumericF::ratio(1.0f, focusPositionMin_, -1.0f) << "m";
	}

	float cameraSensorPhysicalSizeX = -1.0f;
	if (cameraSensorPhysicalSize(*cameraManager, cameraId, cameraSensorPhysicalSizeX))
	{
		Log::debug() << "camera " << cameraId << ", Physical sensor size: " << cameraSensorPhysicalSizeX;
	}

	ocean_assert(frameType.isValid());

	ACameraDevice_stateCallbacks cameraDeviceCallbacks;
	cameraDeviceCallbacks.context = this;
	cameraDeviceCallbacks.onDisconnected = onCameraDisconnectedStatic;
	cameraDeviceCallbacks.onError = onCameraErrorStatic;

	ocean_assert(cameraDevice_ == nullptr);
	const camera_status_t status = NativeCameraLibrary::get().ACameraManager_openCamera(*cameraManager, cameraId.data(), &cameraDeviceCallbacks, &cameraDevice_);

	if (status != ACAMERA_OK)
	{
		Log::error() << "ALiveVideo: Failed to open camera: " << translateCameraStatus(status);
		return false;
	}

	// we update the camera id (which was set in the constructor, most likely it has not changed)
	cameraId_ = std::move(cameraId);
	cameraSensorPhysicalSizeX_ = cameraSensorPhysicalSizeX;

	device_T_camera_ = device_T_camera;

	return true;
}

bool ALiveVideo::releaseCamera()
{
	if (imageReader_)
	{
		NativeMediaLibrary::get().AImageReader_delete(imageReader_);
		imageReader_ = nullptr;
	}

	bool result = true;

	if (cameraDevice_)
	{
		result = NativeCameraLibrary::get().ACameraDevice_close(cameraDevice_) == ACAMERA_OK;
		ocean_assert(result);

		cameraDevice_ = nullptr;
	}

	cameraSensorPhysicalSizeX_ = -1.0f;

	// we intentially do not clear the 'cameraId_' parameter to ensure that some functions can still use this value even if the camera has been stopped

	return result;
}

bool ALiveVideo::createImageReader(const FrameType& frameType)
{
	ocean_assert(imageReader_ == nullptr);
	if (NativeMediaLibrary::get().AImageReader_new(int32_t(frameType.width()), int32_t(frameType.height()), pixelFormatToAndroidFormat(frameType.pixelFormat()), 2, &imageReader_) == AMEDIA_OK)
	{
		AImageReader_ImageListener imageListener;
		imageListener.context = this;
		imageListener.onImageAvailable = onCameraImageCallbackStatic;

		if (NativeMediaLibrary::get().AImageReader_setImageListener(imageReader_, &imageListener) != AMEDIA_OK)
		{
			return false;
		}
	}

	ocean_assert(nativeWindow_ == nullptr);
	NativeMediaLibrary::get().AImageReader_getWindow(imageReader_, &nativeWindow_);

	return true;
}

void ALiveVideo::releaseImageReader()
{
	if (imageReader_ != nullptr)
	{
		NativeMediaLibrary::get().AImageReader_delete(imageReader_);
		imageReader_ = nullptr;
	}

	nativeWindow_ = nullptr; // not calling ANativeWindow_release(), as the object was received via AImageReader_getWindow()
}

bool ALiveVideo::createCaptureSession()
{
	ocean_assert(nativeWindow_ != nullptr && cameraDevice_ != nullptr);

	if (nativeWindow_ == nullptr || cameraDevice_ == nullptr)
	{
		return false;
	}

	bool noError = true;

	ocean_assert(!sessionOutput_.isValid());
	sessionOutput_ = NativeCameraLibrary::ScopedACaptureSessionOutput(nativeWindow_);

	if (!sessionOutput_.isValid())
	{
		noError = false;
	}

	ocean_assert(!sessionOutputContainer_.isValid());

	if (noError)
	{
		ACaptureSessionOutputContainer* sessionOutputContainer = nullptr;
		if (NativeCameraLibrary::get().ACaptureSessionOutputContainer_create(&sessionOutputContainer) == ACAMERA_OK)
		{
			sessionOutputContainer_ = NativeCameraLibrary::ScopedACaptureSessionOutputContainer(sessionOutputContainer);
		}
		else
		{
			noError = false;
		}
	}

	if (noError && NativeCameraLibrary::get().ACaptureSessionOutputContainer_add(*sessionOutputContainer_, *sessionOutput_) != ACAMERA_OK)
	{
		noError = false;
	}

	if (noError)
	{
		ACameraCaptureSession_stateCallbacks sessionStateCallbacks;
		sessionStateCallbacks.context = this;
		sessionStateCallbacks.onActive = onSessionActiveStatic;
		sessionStateCallbacks.onReady = onSessionReadyStatic;
		sessionStateCallbacks.onClosed = onSessionClosedStatic;

		ocean_assert(captureSession_ == nullptr);
		if (noError && NativeCameraLibrary::get().ACameraDevice_createCaptureSession(cameraDevice_, *sessionOutputContainer_, &sessionStateCallbacks, &captureSession_) != ACAMERA_OK)
		{
			noError = false;
		}
	}

	ocean_assert(captureRequest_ == nullptr);
	if (noError && NativeCameraLibrary::get().ACameraDevice_createCaptureRequest(cameraDevice_, TEMPLATE_PREVIEW, &captureRequest_) != ACAMERA_OK)
	{
		noError = false;
	}

	ocean_assert(outputTarget_ == nullptr);
	if (noError && NativeCameraLibrary::get().ACameraOutputTarget_create(nativeWindow_, &outputTarget_) != ACAMERA_OK)
	{
		noError = false;
	}

	if (noError && NativeCameraLibrary::get().ACaptureRequest_addTarget(captureRequest_, outputTarget_) != ACAMERA_OK)
	{
		noError = false;
	}

	// Configure video stabilization based on the current setting
	if (noError)
	{
		const uint8_t videoStabilizationMode = videoStabilizationEnabled_ ? ACAMERA_CONTROL_VIDEO_STABILIZATION_MODE_ON : ACAMERA_CONTROL_VIDEO_STABILIZATION_MODE_OFF;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_VIDEO_STABILIZATION_MODE, 1, &videoStabilizationMode) != ACAMERA_OK)
		{
			Log::warning() << "ALiveVideo: Failed to set video stabilization mode, device may not support this feature";
			// We don't fail here as not all devices support video stabilization
		}
	}

	if (noError == false)
	{
		// we do not know which of the previous iterations succeeded, so we release the entire capture session (which can handle this situation)
		releaseCaptureSession();
		return false;
	}

	return true;
}

bool ALiveVideo::setRepeatingRequest()
{
	if (captureSession_ == nullptr || captureRequest_ == nullptr)
	{
		return false;
	}

	ACameraCaptureSession_captureCallbacks captureCallbacks;
	captureCallbacks.context = this;
	captureCallbacks.onCaptureStarted = onCaptureStartedStatic;
	captureCallbacks.onCaptureProgressed = nullptr;
	captureCallbacks.onCaptureCompleted = onCaptureCompletedStatic;
	captureCallbacks.onCaptureFailed = onCaptureFailedStatic;
	captureCallbacks.onCaptureSequenceCompleted = onCaptureSequenceCompletedStatic;
	captureCallbacks.onCaptureSequenceAborted = onCaptureSequenceAbortedStatic;
	captureCallbacks.onCaptureBufferLost = nullptr;

	if (NativeCameraLibrary::get().ACameraCaptureSession_setRepeatingRequest(captureSession_, &captureCallbacks, 1, &captureRequest_, nullptr) != ACAMERA_OK)
	{
		Log::error() << "ALiveVideo: Failed to set/update repeating capture request";
		return false;
	}

	return true;
}

void ALiveVideo::releaseCaptureSession()
{
	if (captureSession_)
	{
		NativeCameraLibrary::get().ACameraCaptureSession_stopRepeating(captureSession_);
	}

	if (outputTarget_ != nullptr)
	{
		NativeCameraLibrary::get().ACameraOutputTarget_free(outputTarget_);
		outputTarget_ = nullptr;
	}

	if (captureRequest_ != nullptr)
	{
		NativeCameraLibrary::get().ACaptureRequest_free(captureRequest_);
		captureRequest_ = nullptr;
	}

	if (captureSession_ != nullptr)
	{
		sessionCurrentlyClosing_ = true;

		NativeCameraLibrary::get().ACameraCaptureSession_close(captureSession_);
		captureSession_ = nullptr;
	}

	sessionOutputContainer_.release();

	sessionOutput_.release();
}

void ALiveVideo::onCameraImageCallback(AImageReader* /*imageReader*/)
{
	// nothing to do here, we extract the image in onCaptureCompleted()
}

void ALiveVideo::onCameraDisconnected(ACameraDevice* /*cameraDevice*/)
{
	// nothing to do here
}

void ALiveVideo::onCameraError(ACameraDevice* cameraDevice, int error)
{
	ocean_assert(cameraDevice != nullptr);

	switch (error)
	{
		case ERROR_CAMERA_DEVICE:
			// The camera device has encountered a fatal error.
			// The camera device needs to be re-opened to be used again.
			Log::error() << "ALiveVideo: ERROR_CAMERA_DEVICE";
			break;

		case ERROR_CAMERA_DISABLED:
			// The camera is disabled due to a device policy, and cannot be opened.
			Log::error() << "ALiveVideo: ERROR_CAMERA_DISABLED";
			break;

		case ERROR_CAMERA_IN_USE:
			// The camera device is in use already.
			Log::error() << "ALiveVideo: ERROR_CAMERA_IN_USE";
			break;

		case ERROR_CAMERA_SERVICE:
			// The camera service has encountered a fatal error.
			// The Android device may need to be shut down and restarted to restore camera function, or there may be a persistent hardware problem.
			// An attempt at recovery may be possible by closing the CameraDevice and the CameraManager, and trying to acquire all resources again from scratch.
			Log::error() << "ALiveVideo: ERROR_CAMERA_SERVICE";
			break;

		case ERROR_MAX_CAMERAS_IN_USE:
			// The system-wide limit for number of open cameras or camera resources has been reached, and more camera devices cannot be opened until previous instances are closed.
			Log::error() << "ALiveVideo: ERROR_MAX_CAMERAS_IN_USE";
			break;

		default:
			Log::error() << "ALiveVideo: Unknown error";
			break;
	}
}

void ALiveVideo::onSessionActive(ACameraCaptureSession* session)
{
	ocean_assert(session != nullptr);

	const ScopedLock scopedLock(lock_);

	startTimestamp_.toNow();
	stopTimestamp_.toInvalid();
}

void ALiveVideo::onSessionClosed(ACameraCaptureSession* session)
{
	ocean_assert(session != nullptr);

	const ScopedLock scopedLock(lock_);

	startTimestamp_.toInvalid();
	stopTimestamp_.toNow();

	sessionCurrentlyClosing_ = false;
}

void ALiveVideo::onCaptureStarted(ACameraCaptureSession* /*session*/, const ACaptureRequest* /*request*/, int64_t /*timestamp*/)
{
	// nothing to do here
}

void ALiveVideo::onCaptureCompleted(ACameraCaptureSession* session, ACaptureRequest* request, const ACameraMetadata* result)
{
	ocean_assert(session != nullptr);
	ocean_assert(request != nullptr);
	ocean_assert(result != nullptr);

	ControlMode exposureMode = CM_INVALID;
	double exposureDuration = -1.0;

	ControlMode isoMode = CM_INVALID;
	float iso = -1.0f;

	ACameraMetadata_const_entry constEntry = {};
	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_CONTROL_MODE, &constEntry) == ACAMERA_OK)
	{
		const uint8_t controlMode = constEntry.data.u8[0];

		if (controlMode == ACAMERA_CONTROL_MODE_OFF)
		{
			exposureMode = CM_FIXED;
			isoMode = CM_FIXED;
		}
		else
		{
			exposureMode = CM_DYNAMIC;
			isoMode = CM_DYNAMIC;
		}
	}

	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_SENSOR_EXPOSURE_TIME, &constEntry) == ACAMERA_OK)
	{
		const int64_t sensorExposure = constEntry.data.i64[0];
		exposureDuration = Timestamp::nanoseconds2seconds(sensorExposure);
	}

	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_SENSOR_SENSITIVITY, &constEntry) == ACAMERA_OK)
	{
		const int32_t sensorSensitivity = constEntry.data.i32[0];
		iso = float(sensorSensitivity);
	}

	ControlMode focusMode = CM_INVALID;
	float focusValue = -1.0f;

	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_CONTROL_AF_MODE, &constEntry) == ACAMERA_OK)
	{
		if (constEntry.data.u8[0] == ACAMERA_CONTROL_AF_MODE_OFF)
		{
			focusMode = CM_FIXED;
		}
		else
		{
			focusMode = CM_DYNAMIC;
		}
	}

	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_LENS_FOCUS_DISTANCE, &constEntry) == ACAMERA_OK)
	{
		focusValue = constEntry.data.f[0];
	}

#if 0

	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_SENSOR_ROLLING_SHUTTER_SKEW, &constEntry) == ACAMERA_OK)
	{
		// Duration between the start of first row exposure and the start of last row exposure.
		// For typical camera sensors that use rolling shutters, this is also equivalent to the frame readout time.

		const int64_t shutterSkew = constEntry.data.i64[0];

		Log::info() << "Shutter skew: " << double(shutterSkew) / 1000000.0 << "ms";
	}

	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_SENSOR_EXPOSURE_TIME, &constEntry) == ACAMERA_OK)
	{
		// Duration each pixel is exposed to light.

		const int64_t sensorExposure = constEntry.data.i64[0];

		Log::info() << "Sensor exposure: " << double(sensorExposure) / 1000000.0 << "ms";
	}

	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_SENSOR_FRAME_DURATION, &constEntry) == ACAMERA_OK)
	{
		// Duration from start of frame exposure to start of next frame exposure.

		const int64_t frameDuration = constEntry.data.i64[0];

		Log::info() << "Frame duration: " << double(frameDuration) / 1000000.0 << "ms";
	}

	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_SENSOR_SENSITIVITY, &constEntry) == ACAMERA_OK)
	{
		// The amount of gain applied to sensor data before processing.

		const int32_t sensorSensitivity = constEntry.data.i32[0];

		Log::info() << "ISO: " << sensorSensitivity;
	}

#endif

	TemporaryScopedLock scopedLock(lock_);

		exposureMode_ = exposureMode;
		exposureDuration_ = exposureDuration;

		isoMode_ = isoMode;
		iso_ = iso;

		focusMode_ = focusMode;

		if (focusValue != -1.0f && focusPositionMin_ > NumericF::eps())
		{
			ocean_assert(focusValue >= 0.0f && focusValue <= focusPositionMin_);

			focusPosition_ = 1.0f - focusValue / focusPositionMin_;
		}
		else
		{
			focusPosition_ = -1.0f;
		}

	scopedLock.release();

	AImage* image = nullptr;
	if (NativeMediaLibrary::get().AImageReader_acquireNextImage(imageReader_, &image) != AMEDIA_OK)
	{
		return;
	}

	Frame frame;
	int64_t timestampNs;

	if (frameFromImage(image, frame, timestampNs))
	{
		/*
		 * AImage_getTimestamp: For images generated by camera, the timestamp value will match ACAMERA_SENSOR_TIMESTAMP of the ACameraMetadata.
		 *
		 * When ACAMERA_SENSOR_INFO_TIMESTAMP_SOURCE == REALTIME, the timestamps measure time in the same timebase as SystemClock::elapsedRealtimeNanos,
		 * and they can be compared to other timestamps from other subsystems that are using that base.
		 *
		 * SystemClock::elapsedRealtimeNanos(): Returns nanoseconds since boot, including time spent in sleep.
		 */
		ocean_assert(timestampConverter_.timeDomain() == TimestampConverter::TD_BOOTTIME);

		const Timestamp timestamp = timestampConverter_.toUnix(timestampNs);

		const Timestamp relativeTimestamp(Timestamp::nanoseconds2seconds(timestampNs));

		frame.setTimestamp(timestamp);
		frame.setRelativeTimestamp(relativeTimestamp);

		SharedAnyCamera anyCamera;
		if (cameraSensorPhysicalSizeX_ > 0.0f)
		{
			float fovX = 0.0f;
			if (horizontalFieldOfView(cameraSensorPhysicalSizeX_, result, fovX))
			{
				anyCamera = std::make_shared<AnyCameraPinhole>(PinholeCamera(frame.width(), frame.height(), Scalar(fovX)));
			}
		}

		onNewFrame(std::move(frame), std::move(anyCamera));
	}

	NativeMediaLibrary::get().AImage_delete(image);
}

bool ALiveVideo::onNewFrame(Frame&& frame, SharedAnyCamera&& anyCamera)
{
	return deliverNewFrame(std::move(frame), std::move(anyCamera));
}

bool ALiveVideo::horizontalFieldOfView(const float cameraSensorPhysicalSizeX, const ACameraMetadata* cameraMetadata, float& fovX)
{
	ocean_assert(cameraSensorPhysicalSizeX > 0.0f);
	ocean_assert(cameraMetadata != nullptr);

	ACameraMetadata_const_entry constEntry = {};
	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_LENS_FOCAL_LENGTH, &constEntry) != ACAMERA_OK)
	{
		return false;
	}

	const float focalLength = constEntry.data.f[0];

	if (focalLength < NumericF::eps())
	{
		return false;
	}

	fovX = NumericF::atan2(cameraSensorPhysicalSizeX * 0.5f, focalLength) * 2.0f;

	return true;
}

bool ALiveVideo::frameFromImage(AImage* image, Frame& frame, int64_t& timestampNs)
{
	ocean_assert(image != nullptr);

	int32_t width = 0;
	int32_t height = 0;
	timestampNs = 0;
	int32_t format = 0;
	int32_t numberPlanes = 0;

	if (NativeMediaLibrary::get().AImage_getWidth(image, &width) != AMEDIA_OK
		|| NativeMediaLibrary::get().AImage_getHeight(image, &height) != AMEDIA_OK
		|| NativeMediaLibrary::get().AImage_getTimestamp(image, &timestampNs) != AMEDIA_OK
		|| NativeMediaLibrary::get().AImage_getFormat(image, &format) != AMEDIA_OK
		|| NativeMediaLibrary::get().AImage_getNumberOfPlanes(image, &numberPlanes) != AMEDIA_OK)
	{
		return false;
	}

	if (width <= 0 || height <= 0 || numberPlanes <= 0)
	{
		return false;
	}

	const FrameType::PixelFormat pixelFormat = androidFormatToPixelFormat(format);

	switch (pixelFormat)
	{
		case FrameType::FORMAT_Y_U_V12:
		{
			if (numberPlanes != 3 || width % 2 != 0 || height % 2 != 0)
			{
				return false;
			}

			const int32_t width_2 = width / 2;
			const int32_t height_2 = height / 2;

			uint8_t* sources[3] = {nullptr, nullptr, nullptr};
			unsigned int sourcePaddingElements[3] = {0u, 0u, 0u};
			unsigned int sourcePixelStrides[3] = {0u, 0u, 0u};

			for (int32_t planeIndex = 0; planeIndex < 3; ++planeIndex)
			{
				const int32_t planeWidth = planeIndex == 0 ? width : width_2;
				const int32_t planeHeight = planeIndex == 0 ? height : height_2;

				int32_t rowStrideBytes = 0;
				if (NativeMediaLibrary::get().AImage_getPlaneRowStride(image, planeIndex, &rowStrideBytes) != AMEDIA_OK)
				{
					return false;
				}

				if (rowStrideBytes < planeWidth)
				{
					return false;
				}

				sourcePaddingElements[planeIndex] = (unsigned int)(rowStrideBytes - planeWidth);

				int32_t pixelStride = 0;
				if (NativeMediaLibrary::get().AImage_getPlanePixelStride(image, planeIndex, &pixelStride) != AMEDIA_OK)
				{
					return false;
				}

				if (pixelStride <= 0)
				{
					return false;
				}

				sourcePixelStrides[planeIndex] = (unsigned int)(pixelStride);

				int sourceLength = 0;
				if (NativeMediaLibrary::get().AImage_getPlaneData(image, planeIndex, sources + planeIndex, &sourceLength) != AMEDIA_OK)
				{
					return false;
				}

				if (sources[planeIndex] == nullptr || sourceLength < rowStrideBytes * (planeHeight - 1) + planeWidth)
				{
					return false;
				}
			}

			if (!frame.set(FrameType((unsigned int)(width), (unsigned int)(height), FrameType::FORMAT_Y_UV12, FrameType::ORIGIN_UPPER_LEFT), false /*forceOwner*/, true /*forceWritable*/))
			{
				ocean_assert(false && "This should never happen!");
				return false;
			}

			CV::FrameConverterY_U_V12::convertY_U_V12ToY_UV12(sources[0], sources[1], sources[2], frame.data<uint8_t>(0u), frame.data<uint8_t>(1u), frame.width(), frame.height(), sourcePaddingElements[0], sourcePaddingElements[1], sourcePaddingElements[2], frame.paddingElements(0u), frame.paddingElements(1u), sourcePixelStrides[0], sourcePixelStrides[1], sourcePixelStrides[2]);

			return true;
		}

		case FrameType::FORMAT_RGB24:
		case FrameType::FORMAT_RGBA32:
		case FrameType::FORMAT_RGB32:
		case FrameType::FORMAT_BGR565:
		case FrameType::FORMAT_RGB565:
		{
			int32_t rowStrideBytes = 0;
			if (NativeMediaLibrary::get().AImage_getPlaneRowStride(image, 0, &rowStrideBytes) != AMEDIA_OK)
			{
				return false;
			}

			if (numberPlanes != 1)
			{
				return false;
			}

			uint8_t *data = nullptr;
			int dataLength = 0;
			if (NativeMediaLibrary::get().AImage_getPlaneData(image, 0, &data, &dataLength) != AMEDIA_OK || data == nullptr || dataLength <= 0)
			{
				return false;
			}

			const FrameType frameType((unsigned int)(width), (unsigned int)(height), pixelFormat, FrameType::ORIGIN_UPPER_LEFT);

			unsigned int dataPaddingElements;
			if (!Frame::strideBytes2paddingElements(frameType.pixelFormat(), frameType.width(), (unsigned int)(rowStrideBytes), dataPaddingElements))
			{
				return false;
			}

			frame = Frame(frameType, (void*)(data), Frame::CM_COPY_REMOVE_PADDING_LAYOUT, dataPaddingElements);

			return true;
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

HomogenousMatrixD4 ALiveVideo::determineCameraTransformation(ACameraMetadata* cameraMetadata)
{
	ocean_assert(cameraMetadata);

	if (cameraMetadata == nullptr)
	{
		return HomogenousMatrixD4(false);
	}

	// let's check the special case that we have a Quest Passthrough camera
	// com.meta.extra_metadata.camera_source == 0x80004d00, For Passthrough RGB camera this will always have value ‘0’
	// https://developers.meta.com/horizon/documentation/native/android/pca-native-overview/

	ACameraMetadata_const_entry constCameraSourceEntry = {};
	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, 0x80004d00, &constCameraSourceEntry) == ACAMERA_OK)
	{
		if (constCameraSourceEntry.type == ACAMERA_TYPE_BYTE && constCameraSourceEntry.count == 1)
		{
			if (constCameraSourceEntry.data.u8[0] == 0u)
			{
				// we have a passthrough camera

				ACameraMetadata_const_entry constPositionEntry = {};
				ACameraMetadata_const_entry constRotationEntry = {};

				if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_LENS_POSE_TRANSLATION, &constPositionEntry) == ACAMERA_OK
						&& NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_LENS_POSE_ROTATION, &constRotationEntry) == ACAMERA_OK)
				{
					ocean_assert(constPositionEntry.type == ACAMERA_TYPE_FLOAT && constPositionEntry.count == 3);
					ocean_assert(constRotationEntry.type == ACAMERA_TYPE_FLOAT && constRotationEntry.count == 4);

					if (constPositionEntry.type == ACAMERA_TYPE_FLOAT && constPositionEntry.count == 3
							&& constRotationEntry.type == ACAMERA_TYPE_FLOAT && constRotationEntry.count == 4)
					{
						const VectorF3 position(constPositionEntry.data.f[0], constPositionEntry.data.f[1], constPositionEntry.data.f[2]);
						const QuaternionF rotation(constRotationEntry.data.f[3], constRotationEntry.data.f[0], constRotationEntry.data.f[1], constRotationEntry.data.f[2]);
						ocean_assert(rotation.isValid());

						const HomogenousMatrixD4 device_T_flippedCamera = HomogenousMatrixD4(VectorD3(position), QuaternionD(rotation));

						return CameraD::flippedTransformationRightSide(device_T_flippedCamera);
					}
				}
			}
			else
			{
				return HomogenousMatrixD4(true);
			}
		}
	}

	ACameraMetadata_const_entry constEntry = {};
	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_LENS_FACING, &constEntry) == ACAMERA_OK)
	{
		const acamera_metadata_enum_android_lens_facing_t lensFacing = acamera_metadata_enum_android_lens_facing_t(constEntry.data.u8[0]);

		if (lensFacing == ACAMERA_LENS_FACING_BACK)
		{
			// the camera on all Android devices is rotated by 90 degree in relation to the screen

			return HomogenousMatrixD4(QuaternionD(VectorD3(0.0, 0.0, 1.0), -NumericD::pi_2()));
		}
		else if (lensFacing == ACAMERA_LENS_FACING_FRONT)
		{
			// the user facing camera has a different orientation than the user facing camera on iOS platforms

			return HomogenousMatrixD4(QuaternionD(VectorD3(0.0, 0.0, 1.0), -NumericD::pi_2()) * QuaternionD(VectorD3(0.0, 1.0, 0.0), NumericD::pi()));
		}
	}

	Log::warning() << "ALiveVideo: Camera transformation could not be determined using identity instead";

	return HomogenousMatrixD4(true);
}

FrameType::PixelFormat ALiveVideo::androidFormatToPixelFormat(const int32_t androidFormat)
{
	switch (androidFormat)
	{
		case AIMAGE_FORMAT_RGB_565:
			return FrameType::FORMAT_BGR565;

		case AIMAGE_FORMAT_RGB_888:
			return FrameType::FORMAT_RGB24;

		case AIMAGE_FORMAT_RGBA_8888:
			return FrameType::FORMAT_RGBA32;

		case AIMAGE_FORMAT_RGBX_8888:
			return FrameType::FORMAT_RGB32;

		case AIMAGE_FORMAT_YUV_420_888:
			return FrameType::FORMAT_Y_U_V12;
	}

	return FrameType::FORMAT_UNDEFINED;
}

std::string ALiveVideo::androidFormatToString(const int32_t androidFormat)
{
	switch (androidFormat)
	{
		case AIMAGE_FORMAT_RGBA_8888:
			return "AIMAGE_FORMAT_RGBA_8888";

		case AIMAGE_FORMAT_RGBX_8888:
			return "AIMAGE_FORMAT_RGBX_8888";

		case AIMAGE_FORMAT_RGB_888:
			return "AIMAGE_FORMAT_RGB_888";

		case AIMAGE_FORMAT_RGB_565:
			return "AIMAGE_FORMAT_RGB_565";

		case AIMAGE_FORMAT_RGBA_FP16:
			return "AIMAGE_FORMAT_RGBA_FP16";

		case AIMAGE_FORMAT_YUV_420_888:
			return "AIMAGE_FORMAT_YUV_420_888";

		case AIMAGE_FORMAT_JPEG:
			return "AIMAGE_FORMAT_JPEG";

		case AIMAGE_FORMAT_RAW16:
			return "AIMAGE_FORMAT_RAW16";

		case AIMAGE_FORMAT_RAW_PRIVATE:
			return "AIMAGE_FORMAT_RAW_PRIVATE";

		case AIMAGE_FORMAT_RAW10:
			return "AIMAGE_FORMAT_RAW10";

		case AIMAGE_FORMAT_RAW12:
			return "AIMAGE_FORMAT_RAW12";

		case AIMAGE_FORMAT_DEPTH16:
			return "AIMAGE_FORMAT_DEPTH16";

		case AIMAGE_FORMAT_DEPTH_POINT_CLOUD:
			return "AIMAGE_FORMAT_DEPTH_POINT_CLOUD";

		case AIMAGE_FORMAT_PRIVATE:
			return "AIMAGE_FORMAT_PRIVATE";

		case AIMAGE_FORMAT_YCBCR_P010:
			return "AIMAGE_FORMAT_YCBCR_P010";
	}

	ocean_assert(false && "Unknown Android format");
	return "Unknown";
}

int32_t ALiveVideo::pixelFormatToAndroidFormat(const FrameType::PixelFormat pixelFormat)
{
	switch (pixelFormat)
	{
		case FrameType::FORMAT_BGR565:
			return AIMAGE_FORMAT_RGB_565;

		case FrameType::FORMAT_RGB24:
			return AIMAGE_FORMAT_RGB_888;

		case FrameType::FORMAT_RGBA32:
			return AIMAGE_FORMAT_RGBA_8888;

		case FrameType::FORMAT_RGB32:
			return AIMAGE_FORMAT_RGBX_8888;

		case FrameType::FORMAT_Y_U_V12:
			return AIMAGE_FORMAT_YUV_420_888;

		default:
			break;
	}

	return 0;
}

ALiveVideo::StreamConfigurations ALiveVideo::determineAvailableStreamConfigurations(const ACameraMetadata* cameraMetadata)
{
	ocean_assert(cameraMetadata != nullptr);

	StreamConfigurations streamConfigurations;

	ACameraMetadata_const_entry constEntry = {};
	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &constEntry) == ACAMERA_OK)
	{
		ocean_assert(constEntry.count % 4u == 0u);

		if (constEntry.count % 4u == 0u)
		{
			streamConfigurations.reserve(constEntry.count / 4);

			for (uint32_t entryIndex = 0u; entryIndex < constEntry.count; entryIndex += 4u)
			{
				const bool isOutputStream = constEntry.data.i32[entryIndex + 3] == 0;

				if (isOutputStream)
				{
					StreamType streamType = ST_INVALID;

					const int32_t androidFormat = constEntry.data.i32[entryIndex + 0];
					const int32_t width = constEntry.data.i32[entryIndex + 1];
					const int32_t height = constEntry.data.i32[entryIndex + 2];

					const FrameType::PixelFormat pixelFormat(androidFormatToPixelFormat(androidFormat));

					if (pixelFormat == FrameType::FORMAT_UNDEFINED)
					{
						if (androidFormat == AIMAGE_FORMAT_JPEG)
						{
							streamType = ST_MJPEG;
						}
					}
					else
					{
						streamType = ST_FRAME;
					}

					if (streamType != ST_INVALID && width > 0 && height > 0)
					{
						std::vector<double> frameRates;

						constexpr CodecType codecType = CT_INVALID;

						streamConfigurations.emplace_back(streamType, (unsigned int)(width), (unsigned int)(height), std::move(frameRates), pixelFormat, codecType);
					}
				}
			}
		}
	}

	return streamConfigurations;
}

ALiveVideo::Devices ALiveVideo::selectableDevices()
{
	NativeCameraLibrary::ScopedACameraManager cameraManager(NativeCameraLibrary::get().ACameraManager_create());

	if (!cameraManager.isValid())
	{
		return Devices();
	}

	const NativeCameraLibrary::ScopedACameraIdList cameraIdList(*cameraManager);

	if (!cameraIdList.isValid())
	{
		return Devices();
	}

	Devices devices;

	Log::debug() << "ALiveVideo: Found " << cameraIdList->numCameras << " cameras:";

	using TypeCounterMap = std::unordered_map<std::string, unsigned int>;

	TypeCounterMap typeCounterMap;

	for (int nCamera = 0; nCamera < cameraIdList->numCameras; ++nCamera)
	{
		const char* id = cameraIdList->cameraIds[nCamera];
		ocean_assert(id != nullptr);

		if (id == nullptr)
		{
			ocean_assert(false && "This should never happen!");
			continue;
		}

		Device device;

		acamera_metadata_enum_android_lens_facing_t lensFacing = acamera_metadata_enum_android_lens_facing_t(-1);

		Device::MetadataMap metadataMap;

		const NativeCameraLibrary::ScopedACameraMetadata cameraMetadata(*cameraManager, id);

		if (cameraMetadata.isValid())
		{
			ACameraMetadata_const_entry constEntry = {};
			if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(*cameraMetadata, ACAMERA_LENS_FACING, &constEntry) == ACAMERA_OK)
			{
				lensFacing = acamera_metadata_enum_android_lens_facing_t(constEntry.data.u8[0]);
			}

			int32_t numberTags = 0u;
			const uint32_t* tags = nullptr;

			if (NativeCameraLibrary::get().ACameraMetadata_getAllTags(*cameraMetadata, &numberTags, &tags) == ACAMERA_OK)
			{
				for (int32_t n = 0u; n < numberTags; ++n)
				{
					constEntry = {};
					if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(*cameraMetadata, tags[n], &constEntry) == ACAMERA_OK)
					{
						switch (constEntry.type)
						{
							case ACAMERA_TYPE_BYTE:
							{
								if (constEntry.count == 1)
								{
									metadataMap.emplace(tags[n], Value(int32_t(constEntry.data.u8[0])));
								}
								else if (constEntry.count > 1)
								{
									std::string value(constEntry.count, ' ');

									for (unsigned int n = 0u; n < constEntry.count; ++n)
									{
										value[n] = char(constEntry.data.u8[n]);
									}

									metadataMap.emplace(tags[n], Value(value));
								}

								break;
							}

							case ACAMERA_TYPE_INT32:
							{
								if (constEntry.count == 1)
								{
									metadataMap.emplace(tags[n], Value(constEntry.data.i32[0]));
								}

								break;
							}

							case ACAMERA_TYPE_FLOAT:
							{
								if (constEntry.count == 1)
								{
									metadataMap.emplace(tags[n], Value(constEntry.data.f[0]));
								}

								break;
							}

							case ACAMERA_TYPE_INT64:
							{
								if (constEntry.count == 1)
								{
									metadataMap.emplace(tags[n], Value(constEntry.data.i64[0]));
								}

								break;
							}

							case ACAMERA_TYPE_DOUBLE:
							{
								if (constEntry.count == 1)
								{
									metadataMap.emplace(tags[n], Value(constEntry.data.d[0]));
								}

								break;
							}

							case ACAMERA_TYPE_RATIONAL:
							{
								if (constEntry.count == 1)
								{
									const ACameraMetadata_rational& rotational = constEntry.data.r[0];

									if (rotational.denominator != 0)
									{
										metadataMap.emplace(tags[n], Value(double(rotational.numerator) / double(rotational.denominator)));
									}
								}

								break;
							}
						}
					}
				}
			}
		}

		std::string name;

		// special handling for Quest cameras
		// https://developers.meta.com/horizon/documentation/native/android/pca-native-overview

		const ALiveVideo::Device::MetadataMap::const_iterator iMetaCameraSource = metadataMap.find(0x80004d00); // com.meta.extra_metadata.camera_source
		const ALiveVideo::Device::MetadataMap::const_iterator iMetaCameraPosition = metadataMap.find(0x80004d01); // com.meta.extra_metadata.position

		if (iMetaCameraSource != metadataMap.cend() && iMetaCameraPosition != metadataMap.cend())
		{
			// the camera is a Quest camera available through Passthrough API

			const Value& cameraSource = iMetaCameraSource->second;
			const Value& cameraPosition = iMetaCameraPosition->second;

			ocean_assert(cameraSource.type() == Value::VT_INT_32 && cameraPosition.type() == Value::VT_INT_32);

			if (cameraSource.intValue() == 0 && cameraPosition.intValue() == 0)
			{
				name = "Left Quest RGB Camera";
			}
			else if (cameraSource.intValue() == 0 && cameraPosition.intValue() == 1)
			{
				name = "Right Quest RGB Camera";
			}
			else
			{
				ocean_assert(false && "Invalid camera source or position!");
			}
		}

		if (name.empty())
		{
			// the camera is not a Quest camera
			// let's create a suitable name for the camera

			const std::string readableLenseFacing = Device::readableLensFacing(lensFacing);

			unsigned int& counter = typeCounterMap[readableLenseFacing];

			name = readableLenseFacing + " Camera " + String::toAString(counter);

			++counter;
		}

		ocean_assert(!name.empty());

		device = Device(id, name, lensFacing, std::move(metadataMap));

		if (device.isValid())
		{
			Log::debug() << "ALiveVideo: Camera " << nCamera << ": '" << device.name() << "', with id " << device.id() << ", has type '" << Device::readableLensFacing(device.lensFacing()) << "'";

			devices.emplace_back(std::move(device));
		}
		else
		{
			Log::error() << "Failed to extract device information for camera with index " << nCamera;
		}
	}

	return devices;
}

std::string ALiveVideo::cameraIdForMedium(ACameraManager* cameraManager, const std::string& url, const std::string& id, const FrameType& preferredFrameType, FrameType& frameType, StreamConfigurations* streamConfigurations, HomogenousMatrixD4* device_T_camera)
{
	ocean_assert(cameraManager != nullptr);

	const NativeCameraLibrary::ScopedACameraIdList cameraIdList(cameraManager);

	if (!cameraIdList.isValid())
	{
		return std::string();
	}

	const std::string cameraId = cameraIdForMedium(*cameraIdList, url, id);

	if (cameraId.empty())
	{
		return std::string();
	}

	constexpr unsigned int defaultPreferredFrameWidth = 1280u;
	constexpr unsigned int defaultPreferredFrameHeight = 720u;

	const unsigned int preferredFrameWidth = preferredFrameType.width() != 0u ? preferredFrameType.width() : defaultPreferredFrameWidth;
	const unsigned int preferredFrameHeight = preferredFrameType.height() != 0u ? preferredFrameType.height() : defaultPreferredFrameHeight;

	const NativeCameraLibrary::ScopedACameraMetadata cameraMetadata(cameraManager, cameraId.c_str());
	if (!cameraMetadata.isValid())
	{
		return std::string();
	}

#ifdef OCEAN_DEBUG
	{
		ACameraMetadata_const_entry constEntry = {};
		if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(*cameraMetadata, ACAMERA_SENSOR_EXPOSURE_TIME, &constEntry) == ACAMERA_OK)
		{
			const int64_t exposure = constEntry.data.i64[0];
			Log::debug() << "Current exposure: " << exposure;
		}
	}

	{
		ACameraMetadata_const_entry constEntry = {};
		if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(*cameraMetadata, ACAMERA_LENS_FACING, &constEntry) == ACAMERA_OK)
		{
			const acamera_metadata_enum_android_lens_facing_t lensFacing = acamera_metadata_enum_android_lens_facing_t(constEntry.data.u8[0]);

			if (lensFacing == ACAMERA_LENS_FACING_FRONT)
			{
				Log::debug() << "ALiveVideo: Front-facing camera";
			}
			else if (lensFacing == ACAMERA_LENS_FACING_BACK)
			{
				Log::debug() << "ALiveVideo: Back-facing camera";
			}
		}
	}

	{
		ACameraMetadata_const_entry constEntry = {};
		if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(*cameraMetadata, ACAMERA_SENSOR_INFO_TIMESTAMP_SOURCE, &constEntry) == ACAMERA_OK)
		{
			const acamera_metadata_enum_android_sensor_info_timestamp_source_t timestampSource = acamera_metadata_enum_android_sensor_info_timestamp_source_t(constEntry.data.u8[0]);

			if (timestampSource == ACAMERA_SENSOR_INFO_TIMESTAMP_SOURCE_UNKNOWN)
			{
				Log::debug() << "ALiveVideo: Unknown timestamp source";
			}
			else if (timestampSource == ACAMERA_SENSOR_INFO_TIMESTAMP_SOURCE_REALTIME)
			{
				Log::debug() << "ALiveVideo: Realtime timestamp";
			}
		}
	}
#endif // OCEAN_DEBUG

	// we check whether the camera provides a compatible stream

	FrameType bestFrameType;
	unsigned int bestSizeDelta = (unsigned int)(-1);

	StreamConfigurations availableStreamConfigurations = determineAvailableStreamConfigurations(*cameraMetadata);

	Log::debug() << "Camera " << cameraId << " supports " <<  availableStreamConfigurations.size() << " streams:";

	for (const StreamConfiguration& availableStreamConfiguration : availableStreamConfigurations)
	{
		Log::debug() << availableStreamConfiguration.toString();

		if (availableStreamConfiguration.framePixelFormat_ == FrameType::FORMAT_Y_U_V12)
		{
			if (availableStreamConfiguration.width_ == preferredFrameWidth && availableStreamConfiguration.height_ == preferredFrameHeight)
			{
				bestFrameType = FrameType(availableStreamConfiguration.width_, availableStreamConfiguration.height_, FrameType::FORMAT_Y_U_V12, FrameType::ORIGIN_UPPER_LEFT);
				break;
			}
			else if (availableStreamConfiguration.width_ >= preferredFrameWidth && availableStreamConfiguration.height_ >= preferredFrameHeight)
			{
				const unsigned int sizeDelta = std::max(availableStreamConfiguration.width_ - preferredFrameWidth, availableStreamConfiguration.height_ - preferredFrameHeight);

				if (sizeDelta < bestSizeDelta)
				{
					bestFrameType = FrameType(availableStreamConfiguration.width_, availableStreamConfiguration.height_, FrameType::FORMAT_Y_U_V12, FrameType::ORIGIN_UPPER_LEFT);
					bestSizeDelta = sizeDelta;
				}
			}
		}
	}

	if (bestFrameType.isValid())
	{
		frameType = bestFrameType;

		if (streamConfigurations != nullptr)
		{
			*streamConfigurations = std::move(availableStreamConfigurations);
		}

		if (device_T_camera != nullptr)
		{
			*device_T_camera = determineCameraTransformation(*cameraMetadata);
		}

		return cameraId;
	}

	return std::string();
}

std::string ALiveVideo::cameraIdForMedium(ACameraIdList* cameraIdList, const std::string& url, const std::string& id)
{
	ocean_assert(cameraIdList != nullptr);

	// first, we check whether we have a perfect match

	if (!id.empty())
	{
		for (int nCamera = 0; nCamera < cameraIdList->numCameras; ++nCamera)
		{
			const char* cameraId = cameraIdList->cameraIds[nCamera];
			ocean_assert(cameraId != nullptr);

			if (cameraId == nullptr)
			{
				continue;
			}

			if (id == cameraId)
			{
				return id;
			}
		}
	}

	// second, we check whether the user specified the camera via a LiveVideoId

	if (url.find("LiveVideoId:") == 0 && url.size() >= 13)
	{
		int oceanLiveVideoId = -1;
		if (String::isInteger32(url.substr(12), &oceanLiveVideoId) && oceanLiveVideoId >= 0)
		{
			if (size_t(oceanLiveVideoId) < cameraIdList->numCameras)
			{
				const char* cameraId = cameraIdList->cameraIds[oceanLiveVideoId];
				ocean_assert(cameraId != nullptr);

				if (cameraId != nullptr)
				{
					return cameraId;
				}
			}
		}
	}

	const Devices devices = selectableDevices();

	for (const Device& device : devices)
	{
		if (device.name() == url)
		{
			return device.id();
		}
	}

	return std::string();
}

bool ALiveVideo::cameraExposureDurationRange(ACameraManager* cameraManager, const std::string& cameraId, double& minExposureDuration, double& maxExposureDuration)
{
	ocean_assert(cameraManager != nullptr && !cameraId.empty());

	bool result = false;

	const NativeCameraLibrary::ScopedACameraMetadata cameraMetadata(cameraManager, cameraId.c_str());

	if (cameraMetadata.isValid())
	{
		ACameraMetadata_const_entry constEntry = {};
		if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(*cameraMetadata, ACAMERA_SENSOR_INFO_EXPOSURE_TIME_RANGE, &constEntry) == ACAMERA_OK)
		{
			const int64_t minExposure = constEntry.data.i64[0];
			const int64_t maxExposure = constEntry.data.i64[1];

			minExposureDuration = Timestamp::nanoseconds2seconds(minExposure);
			maxExposureDuration = Timestamp::nanoseconds2seconds(maxExposure);

			result = true;
		}
	}

	return result;
}

bool ALiveVideo::cameraISORange(ACameraManager* cameraManager, const std::string& cameraId, float& minISO, float& maxISO)
{
	ocean_assert(cameraManager != nullptr && !cameraId.empty());

	bool result = false;

	const NativeCameraLibrary::ScopedACameraMetadata cameraMetadata(cameraManager, cameraId.c_str());

	if (cameraMetadata.isValid())
	{
		ACameraMetadata_const_entry constEntry = {};
		if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(*cameraMetadata, ACAMERA_SENSOR_INFO_SENSITIVITY_RANGE, &constEntry) == ACAMERA_OK)
		{
			minISO = float(constEntry.data.i32[0]);
			maxISO = float(constEntry.data.i32[1]);

			result = true;
		}
	}

	return result;
}

bool ALiveVideo::cameraAvailableFocusModes(ACameraManager* cameraManager, const std::string& cameraId, ControlModes& focusModes, float& minFocusPosition)
{
	ocean_assert(cameraManager != nullptr && !cameraId.empty());

	const NativeCameraLibrary::ScopedACameraMetadata cameraMetadata(cameraManager, cameraId.c_str());

	if (!cameraMetadata.isValid())
	{
		return false;
	}

	// from Android documentation:
	// ... the focus distance value will still be in the range of [0, ACAMERA_LENS_INFO_MINIMUM_FOCUS_DISTANCE], where 0 represents the farthest focus

	ACameraMetadata_const_entry constEntry = {};
	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(*cameraMetadata, ACAMERA_CONTROL_AF_AVAILABLE_MODES, &constEntry) != ACAMERA_OK)
	{
		return false;
	}

	focusModes.clear();

	for (unsigned int n = 0u; n < constEntry.count; ++n)
	{
		if (constEntry.data.u8[n] == ACAMERA_CONTROL_AF_MODE_OFF)
		{
			focusModes.push_back(ControlMode::CM_FIXED);
		}
		else if (constEntry.data.u8[n] == ACAMERA_CONTROL_AF_MODE_AUTO)
		{
			focusModes.push_back(ControlMode::CM_DYNAMIC);
		}
	}

	ocean_assert(focusModes.size() <= 2);

	constEntry = {};
	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(*cameraMetadata, ACAMERA_LENS_INFO_MINIMUM_FOCUS_DISTANCE, &constEntry) != ACAMERA_OK)
	{
		return false;
	}

	minFocusPosition = constEntry.data.f[0];

	return true;
}

bool ALiveVideo::cameraSensorPhysicalSize(ACameraManager* cameraManager, const std::string& cameraId, float& cameraSensorPhysicalSizeX)
{
	ocean_assert(cameraManager != nullptr && !cameraId.empty());

	bool result = false;

	const NativeCameraLibrary::ScopedACameraMetadata cameraMetadata(cameraManager, cameraId.c_str());

	if (cameraMetadata.isValid())
	{
		ACameraMetadata_const_entry constEntryPhysicalSize = {};
		if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(*cameraMetadata, ACAMERA_SENSOR_INFO_PHYSICAL_SIZE, &constEntryPhysicalSize) == ACAMERA_OK)
		{
			cameraSensorPhysicalSizeX = constEntryPhysicalSize.data.f[0];

			result = true;
		}
	}

	return result;
}

void ALiveVideo::onCameraImageCallbackStatic(void* context, AImageReader* imageReader)
{
	ocean_assert(context != nullptr);

	reinterpret_cast<ALiveVideo*>(context)->onCameraImageCallback(imageReader);
}

void ALiveVideo::onCameraDisconnectedStatic(void* context, ACameraDevice* cameraDevice)
{
	ocean_assert(context != nullptr);

	reinterpret_cast<ALiveVideo*>(context)->onCameraDisconnected(cameraDevice);
}

void ALiveVideo::onCameraErrorStatic(void* context, ACameraDevice* cameraDevice, int error)
{
	ocean_assert(context != nullptr);

	reinterpret_cast<ALiveVideo*>(context)->onCameraError(cameraDevice, error);
}

void ALiveVideo::onSessionActiveStatic(void* context, ACameraCaptureSession* session)
{
	ocean_assert(context != nullptr);

	reinterpret_cast<ALiveVideo*>(context)->onSessionActive(session);
}

void ALiveVideo::onSessionReadyStatic(void* /*context*/, ACameraCaptureSession* /*session*/)
{
	// nothing to do here (yet)
}

void ALiveVideo::onSessionClosedStatic(void* context, ACameraCaptureSession* session)
{
	ocean_assert(context != nullptr);

	reinterpret_cast<ALiveVideo*>(context)->onSessionClosed(session);
}

void ALiveVideo::onCaptureFailedStatic(void* /*context*/, ACameraCaptureSession* /*session*/, ACaptureRequest* /*request*/, ACameraCaptureFailure* /*failure*/)
{
	// nothing to do here (yet)
}

void ALiveVideo::onCaptureSequenceCompletedStatic(void* /*context*/, ACameraCaptureSession* /*session*/, int /*sequenceId*/, int64_t /*frameNumber*/)
{
	// nothing to do here (yet)
}

void ALiveVideo::onCaptureSequenceAbortedStatic(void* /*context*/, ACameraCaptureSession* /*session*/, int /*sequenceId*/)
{
	// nothing to do here (yet)
}

void ALiveVideo::onCaptureStartedStatic(void* context, ACameraCaptureSession* session, const ACaptureRequest* request, int64_t timestamp)
{
	ocean_assert(context != nullptr);

	reinterpret_cast<ALiveVideo*>(context)->onCaptureStarted(session, request, timestamp);
}

void ALiveVideo::onCaptureCompletedStatic(void* context, ACameraCaptureSession* session, ACaptureRequest* request, const ACameraMetadata* result)
{
	ocean_assert(context != nullptr);

	reinterpret_cast<ALiveVideo*>(context)->onCaptureCompleted(session, request, result);
}

std::string ALiveVideo::translateCameraStatus(const camera_status_t cameraStatus)
{
	switch (cameraStatus)
	{
		case ACAMERA_ERROR_BASE:
			return "ACAMERA_ERROR_BASE";

		case ACAMERA_ERROR_CAMERA_DEVICE:
		 		return "ACAMERA_ERROR_CAMERA_DEVICE";

		case ACAMERA_ERROR_CAMERA_DISABLED:
			return "ACAMERA_ERROR_CAMERA_DISABLED";

		case ACAMERA_ERROR_CAMERA_DISCONNECTED:
			return "ACAMERA_ERROR_CAMERA_DISCONNECTED";

		case ACAMERA_ERROR_CAMERA_IN_USE:
			return "ACAMERA_ERROR_CAMERA_IN_USE";

		case ACAMERA_ERROR_CAMERA_SERVICE:
			return "ACAMERA_ERROR_CAMERA_SERVICE";

		case ACAMERA_ERROR_INVALID_OPERATION:
			return "ACAMERA_ERROR_INVALID_OPERATION";

		case ACAMERA_ERROR_INVALID_PARAMETER:
			return "ACAMERA_ERROR_INVALID_PARAMETER";

		case ACAMERA_ERROR_MAX_CAMERA_IN_USE:
			return "ACAMERA_ERROR_MAX_CAMERA_IN_USE";

		case ACAMERA_ERROR_METADATA_NOT_FOUND:
			return "ACAMERA_ERROR_METADATA_NOT_FOUND";

		case ACAMERA_ERROR_NOT_ENOUGH_MEMORY:
			return "ACAMERA_ERROR_NOT_ENOUGH_MEMORY";

		case ACAMERA_ERROR_PERMISSION_DENIED:
			return "ACAMERA_ERROR_PERMISSION_DENIED";

		case ACAMERA_ERROR_SESSION_CLOSED:
			return "ACAMERA_ERROR_SESSION_CLOSED";

		case ACAMERA_ERROR_STREAM_CONFIGURE_FAIL:
			return "ACAMERA_ERROR_STREAM_CONFIGURE_FAIL";

		case ACAMERA_OK:
			return "ACAMERA_OK";

		default:
			break;
	}

	ocean_assert(false && "Unknown status!");
	return "Unknown";
}

#endif // OCEAN_MEDIA_ANDROID_NATIVECAMERALIBRARY_AVAILABLE

}

}

}
