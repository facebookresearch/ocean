/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/ALiveVideo.h"

#include "ocean/base/String.h"

#include "ocean/cv/FrameConverterY_U_V12.h"

#include "ocean/io/CameraCalibrationManager.h"

#include "ocean/math/PinholeCamera.h"

#include "ocean/platform/android/NativeInterfaceManager.h"
#include "ocean/platform/android/Utilities.h"

namespace Ocean
{

namespace Media
{

namespace Android
{

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

ALiveVideo::ALiveVideo(const std::string& url) :
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
			cameraId_ = cameraIdForMedium(*cameraManager, url_, preferredFrameType_, frameType);

			if (!cameraId_.empty())
			{
				// we have a valid camera id, this is enough to rate this medium as valid

				frameCollection_ = FrameCollection(10);

				isValid_ = true;
			}
		}
	}
}

ALiveVideo::~ALiveVideo()
{
	release();
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
	if (cameraId_ == "0")
	{
		// back-facing camera
		// the camera on all Android devices is rotated by 90 degree in relation to the screen

		return HomogenousMatrixD4(QuaternionD(VectorD3(0.0, 0.0, 1.0), -NumericD::pi_2()));
	}
	else
	{
		// front-facing camera
		// the user facing camera has a different orientation than the user facing camera on iOS platforms

		return HomogenousMatrixD4(QuaternionD(VectorD3(0.0, 0.0, 1.0), -NumericD::pi_2()) * QuaternionD(VectorD3(0.0, 1.0, 0.0), NumericD::pi()));
	}
}

double ALiveVideo::exposureDuration(double* minDuration, double* maxDuration) const
{
	const ScopedLock scopedLock(lock_);

	if (minDuration)
	{
		*minDuration = exposureDurationMin_;
	}

	if (maxDuration)
	{
		*maxDuration = exposureDurationMax_;
	}

	return exposureDuration_;
}

float ALiveVideo::iso(float* minISO, float* maxISO) const
{
	const ScopedLock scopedLock(lock_);

	if (minISO)
	{
		*minISO = isoMin_;
	}

	if (maxISO)
	{
		*maxISO = isoMax_;
	}

	return iso_;
}

float ALiveVideo::focus() const
{
	const ScopedLock scopedLock(lock_);

	return focusPosition_;
}

bool ALiveVideo::setExposureDuration(const double duration)
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

	const bool needsRestart = startTimestamp_.isValid();

	if (needsRestart)
	{
		stop();
	}

	if (position >= 0.0f && position <= 1.0f)
	{
		if (focusPositionMin_ == -1.0f || focusPositionMax_ == -1.0f || focusPositionMax_ <= focusPositionMin_)
		{
			return false;
		}

		const uint8_t mode = ACAMERA_CONTROL_AF_MODE_OFF;
		if (NativeCameraLibrary::get().ACaptureRequest_setEntry_u8(captureRequest_, ACAMERA_CONTROL_AF_MODE, 1, &mode) != ACAMERA_OK)
		{
			return false;
		}

		const float focusValue = focusPositionMin_ + (focusPositionMax_ - focusPositionMin_) * position;
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

	return !needsRestart || start();
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

	ACameraCaptureSession_captureCallbacks captureCallbacks;
	captureCallbacks.context = this;
	captureCallbacks.onCaptureStarted = onCaptureStartedStatic;
	captureCallbacks.onCaptureProgressed = nullptr;
	captureCallbacks.onCaptureCompleted = onCaptureCompletedStatic;
	captureCallbacks.onCaptureFailed = onCaptureFailedStatic;
	captureCallbacks.onCaptureSequenceCompleted = onCaptureSequenceCompletedStatic;
	captureCallbacks.onCaptureSequenceAborted = onCaptureSequenceAbortedStatic;
	captureCallbacks.onCaptureBufferLost = nullptr;

	return NativeCameraLibrary::get().ACameraCaptureSession_setRepeatingRequest(captureSession_, &captureCallbacks, 1, &captureRequest_, nullptr) == ACAMERA_OK;
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

	std::string cameraId = cameraIdForMedium(*cameraManager, url_, preferredFrameType_, frameType);

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

	if (cameraFocusRange(*cameraManager, cameraId, focusPositionMin_, focusPositionMax_))
	{
		Log::debug() << "camera " << cameraId << ", Focus range [" << focusPositionMin_ << ", " << focusPositionMax_ << "]";
	}

	float cameraSensorPhysicalSizeX = -1.0f;
	if (cameraSensorPysicalSize(*cameraManager, cameraId, cameraSensorPhysicalSizeX))
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

	if (noError == false)
	{
		// we do not know which of the previous iterations succeeded, so we release the entire capture session (which can handle this situation)
		releaseCaptureSession();
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

	ACameraMetadata_const_entry constEntry = {0};
	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_CONTROL_MODE, &constEntry) == ACAMERA_OK)
	{
		const uint8_t controlMode = constEntry.data.u8[0];

		if (controlMode == ACAMERA_CONTROL_MODE_OFF)
		{
			if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_SENSOR_EXPOSURE_TIME, &constEntry) == ACAMERA_OK)
			{
				const int64_t sensorExposure = constEntry.data.i64[0];
				exposureDuration_ = Timestamp::nanoseconds2seconds(sensorExposure);
			}

			if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_SENSOR_SENSITIVITY, &constEntry) == ACAMERA_OK)
			{
				const int32_t sensorSensitivity = constEntry.data.i32[0];
				iso_ = float(sensorSensitivity);
			}
		}
		else
		{
			exposureDuration_ = 0.0;
			iso_ = -1.0;
		}
	}

	if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_CONTROL_AF_MODE, &constEntry) == ACAMERA_OK)
	{
		if (constEntry.data.u8[0] == ACAMERA_CONTROL_AF_MODE_OFF)
		{
			focusPosition_ = -1.0f;
		}
		else
		{
			if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(result, ACAMERA_LENS_FOCUS_DISTANCE, &constEntry) == ACAMERA_OK)
			{
				const float focusValue = constEntry.data.f[0];

				if (focusPositionMin_ != -1.0f && focusPositionMax_ != -1.0f && focusPositionMin_ < focusPositionMax_)
				{
					focusPosition_ = (focusValue - focusPositionMin_) / (focusPositionMax_ - focusPositionMin_);
				}
			}
		}
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

	AImage* image = nullptr;
	if (NativeMediaLibrary::get().AImageReader_acquireNextImage(imageReader_, &image) != AMEDIA_OK)
	{
		return;
	}

	Frame frame;
	if (frameFromImage(image, frame))
	{
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
	if (anyCamera && anyCamera->name() == AnyCameraPinhole::WrappedCamera::name())
	{
		const PinholeCamera& camera = ((const AnyCameraPinhole&)(*anyCamera)).actualCamera();

		IO::CameraCalibrationManager::get().registerCalibration(url_, camera);
	}

	return deliverNewFrame(std::move(frame), std::move(anyCamera));
}

bool ALiveVideo::horizontalFieldOfView(const float cameraSensorPhysicalSizeX, const ACameraMetadata* cameraMetadata, float& fovX)
{
	ocean_assert(cameraSensorPhysicalSizeX > 0.0f);
	ocean_assert(cameraMetadata != nullptr);

	ACameraMetadata_const_entry constEntry = {0};
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

bool ALiveVideo::frameFromImage(AImage* image, Frame& frame)
{
	ocean_assert(image != nullptr);

	int32_t width = 0;
	int32_t height = 0;
	int64_t timestampNanoSeconds = 0;
	int32_t format = 0;
	int32_t numberPlanes = 0;

	if (NativeMediaLibrary::get().AImage_getWidth(image, &width) != AMEDIA_OK
		|| NativeMediaLibrary::get().AImage_getHeight(image, &height) != AMEDIA_OK
		|| NativeMediaLibrary::get().AImage_getTimestamp(image, &timestampNanoSeconds) != AMEDIA_OK
		|| NativeMediaLibrary::get().AImage_getFormat(image, &format) != AMEDIA_OK
		|| NativeMediaLibrary::get().AImage_getNumberOfPlanes(image, &numberPlanes) != AMEDIA_OK)
	{
		return false;
	}

	if (width <= 0 || height <= 0 || numberPlanes <= 0)
	{
		return false;
	}

	const Timestamp timestamp(true); // **TODO** use a more precise timestamp

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

			frame.setTimestamp(timestamp);

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

			frame = Frame(frameType, (void*)(data), Frame::CM_COPY_REMOVE_PADDING_LAYOUT, dataPaddingElements, timestamp);

			return true;
		}

		default:
			break;
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
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

std::string ALiveVideo::cameraIdForMedium(ACameraManager* cameraManager, const std::string& url, const FrameType& preferredFrameType, FrameType& frameType)
{
	ocean_assert(cameraManager != nullptr);

	int oceanLiveVideoId = -1;

	if (url.find("LiveVideoId:") == 0 && url.size() >= 13)
	{
		String::isInteger32(url.substr(12), &oceanLiveVideoId);
	}

	const NativeCameraLibrary::ScopedACameraIdList cameraIdList(cameraManager);

	if (!cameraIdList.isValid())
	{
		return std::string();
	}

#ifdef OCEAN_DEBUG
	Log::debug() << "ALiveVideo: Found " << cameraIdList->numCameras << " cameras:";
	for (int i = 0; i < cameraIdList->numCameras; ++i)
	{
		std::string cameraType = "Unknown";

		const char* id = cameraIdList->cameraIds[i];
		ocean_assert(id != nullptr);

		if (id != nullptr)
		{
			ACameraMetadata* cameraMetadata;
			if (NativeCameraLibrary::get().ACameraManager_getCameraCharacteristics(cameraManager, id, &cameraMetadata) == ACAMERA_OK)
			{
				ACameraMetadata_const_entry constEntry = {0};
				if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_LENS_FACING, &constEntry) == ACAMERA_OK)
				{
					const acamera_metadata_enum_android_lens_facing_t lensFacing = acamera_metadata_enum_android_lens_facing_t(constEntry.data.u8[0]);

					switch (lensFacing)
					{
						case ACAMERA_LENS_FACING_FRONT:
							cameraType = "Front-facing";
							break;

						case ACAMERA_LENS_FACING_BACK:
							cameraType = "Back-facing";
							break;

						case ACAMERA_LENS_FACING_EXTERNAL:
							cameraType = "External";
							break;
					}
				}
			}
		}

		Log::debug() << "ALiveVideo: Camera "  << i << ": " << cameraIdList->cameraIds[i] << ", type: " << cameraType;
	}
#endif // OCEAN_DEBUG

	constexpr unsigned int defaultPreferredFrameWidth = 1280u;
	constexpr unsigned int defaultPreferredFrameHeight = 720u;

	const unsigned int preferredFrameWidth = preferredFrameType.width() != 0u ? preferredFrameType.width() : defaultPreferredFrameWidth;
	const unsigned int preferredFrameHeight = preferredFrameType.height() != 0u ? preferredFrameType.height() : defaultPreferredFrameHeight;

	std::string result;

	for (int i = 0; i < cameraIdList->numCameras; ++i)
	{
		if (oceanLiveVideoId == -1 || oceanLiveVideoId == i)
		{
			const char* id = cameraIdList->cameraIds[i];
			ocean_assert(id != nullptr);

			if (id == nullptr)
			{
				continue;
			}

			ACameraMetadata* cameraMetadata;
			if (NativeCameraLibrary::get().ACameraManager_getCameraCharacteristics(cameraManager, id, &cameraMetadata) == ACAMERA_OK)
			{
				{
					ACameraMetadata_const_entry constEntry = {0};
					if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_SENSOR_EXPOSURE_TIME, &constEntry) == ACAMERA_OK)
					{
						const int64_t exposure = constEntry.data.i64[0];
						Log::info() << "Current exposure: " << exposure;
					}
				}

				{
					ACameraMetadata_const_entry constEntry = {0};
					if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_LENS_FACING, &constEntry) == ACAMERA_OK)
					{
#ifdef OCEAN_DEBUG
						const acamera_metadata_enum_android_lens_facing_t lensFacing = acamera_metadata_enum_android_lens_facing_t(constEntry.data.u8[0]);

						if (lensFacing == ACAMERA_LENS_FACING_FRONT)
						{
							Log::debug() << "Front-facing camera";
						}
						else if (lensFacing == ACAMERA_LENS_FACING_BACK)
						{
							Log::debug() << "Back-facing camera";
						}
#endif // OCEAN_DEBUG
					}
				}

				{
					ACameraMetadata_const_entry constEntry = {0};
					if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_SENSOR_INFO_TIMESTAMP_SOURCE, &constEntry) == ACAMERA_OK)
					{
						const acamera_metadata_enum_android_sensor_info_timestamp_source_t timestampSource = acamera_metadata_enum_android_sensor_info_timestamp_source_t(constEntry.data.u8[0]);

						if (timestampSource == ACAMERA_SENSOR_INFO_TIMESTAMP_SOURCE_UNKNOWN)
						{
							Log::info() << "Unknown timestamp source!";
						}
						else if (timestampSource == ACAMERA_SENSOR_INFO_TIMESTAMP_SOURCE_REALTIME)
						{
							Log::info() << "Realtime timestamp";
						}
					}
				}

				// we check whether the camera provides a compatible stream

				FrameType bestFrameType;
				unsigned int bestSizeDelta = (unsigned int)(-1);

				ACameraMetadata_const_entry constEntry = {0};
				if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, &constEntry) == ACAMERA_OK)
				{
					Log::debug() << "Supported streams: " <<  constEntry.count / 4u;

					for (uint32_t entryIndex = 0u; entryIndex < constEntry.count; entryIndex += 4u)
					{
						const bool isOutputStream = constEntry.data.i32[entryIndex + 3] == 0;

						if (isOutputStream)
						{
							const int32_t format = constEntry.data.i32[entryIndex + 0];
							const int32_t width = constEntry.data.i32[entryIndex + 1];
							const int32_t height = constEntry.data.i32[entryIndex + 2];

							Log::debug() << "" << width << "x" << height << " " << androidFormatToString(format);

							const FrameType::PixelFormat pixelFormat(androidFormatToPixelFormat(format));

							if (pixelFormat == FrameType::FORMAT_Y_U_V12 && width > 0 && height > 0)
							{
								if ((unsigned int)(width) == preferredFrameWidth && (unsigned int)(height) == preferredFrameHeight)
								{
									bestFrameType = FrameType((unsigned int)(width), (unsigned int)(height), FrameType::FORMAT_Y_U_V12, FrameType::ORIGIN_UPPER_LEFT);
									break;
								}
								else if ((unsigned int)(width) >= preferredFrameWidth && (unsigned int)(height) >= preferredFrameHeight)
								{
									const unsigned int sizeDelta = std::max((unsigned int)(width) - preferredFrameWidth, (unsigned int)(height) - preferredFrameHeight);

									if (sizeDelta < bestSizeDelta)
									{
										bestFrameType = FrameType((unsigned int)(width), (unsigned int)(height), FrameType::FORMAT_Y_U_V12, FrameType::ORIGIN_UPPER_LEFT);
										bestSizeDelta = sizeDelta;
									}
								}
							}
						}
					}
				}

				if (bestFrameType.isValid())
				{
					frameType = bestFrameType;
					result = id;

					break;
				}
			}

			NativeCameraLibrary::get().ACameraMetadata_free(cameraMetadata);
		}
	}

	return result;
}

bool ALiveVideo::cameraExposureDurationRange(ACameraManager* cameraManager, const std::string& cameraId, double& minExposureDuration, double& maxExposureDuration)
{
	ocean_assert(cameraManager != nullptr && !cameraId.empty());

	bool result = false;

	ACameraMetadata* cameraMetadata;
	if (NativeCameraLibrary::get().ACameraManager_getCameraCharacteristics(cameraManager, cameraId.c_str(), &cameraMetadata) == ACAMERA_OK)
	{
		ACameraMetadata_const_entry constEntry = {0};
		if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_SENSOR_INFO_EXPOSURE_TIME_RANGE, &constEntry) == ACAMERA_OK)
		{
			const int64_t minExposure = constEntry.data.i64[0];
			const int64_t maxExposure = constEntry.data.i64[1];

			minExposureDuration = Timestamp::nanoseconds2seconds(minExposure);
			maxExposureDuration = Timestamp::nanoseconds2seconds(maxExposure);

			result = true;
		}

		NativeCameraLibrary::get().ACameraMetadata_free(cameraMetadata);
	}

	return result;
}

bool ALiveVideo::cameraISORange(ACameraManager* cameraManager, const std::string& cameraId, float& minISO, float& maxISO)
{
	ocean_assert(cameraManager != nullptr && !cameraId.empty());

	bool result = false;

	ACameraMetadata* cameraMetadata;
	if (NativeCameraLibrary::get().ACameraManager_getCameraCharacteristics(cameraManager, cameraId.c_str(), &cameraMetadata) == ACAMERA_OK)
	{
		ACameraMetadata_const_entry constEntry = {0};
		if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_SENSOR_INFO_SENSITIVITY_RANGE, &constEntry) == ACAMERA_OK)
		{
			minISO = float(constEntry.data.i32[0]);
			maxISO = float(constEntry.data.i32[1]);

			result = true;
		}

		NativeCameraLibrary::get().ACameraMetadata_free(cameraMetadata);
	}

	return result;
}

bool ALiveVideo::cameraFocusRange(ACameraManager* cameraManager, const std::string& cameraId, float& minFocusPosition, float& maxFocusPosition)
{
	ocean_assert(cameraManager != nullptr && !cameraId.empty());

	bool result = false;

	ACameraMetadata* cameraMetadata;
	if (NativeCameraLibrary::get().ACameraManager_getCameraCharacteristics(cameraManager, cameraId.c_str(), &cameraMetadata) == ACAMERA_OK)
	{
		ACameraMetadata_const_entry constEntry = {0};
		if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_LENS_FOCUS_RANGE, &constEntry) == ACAMERA_OK)
		{
			minFocusPosition = constEntry.data.f[0];
			maxFocusPosition = constEntry.data.f[1];

			result = true;
		}

		NativeCameraLibrary::get().ACameraMetadata_free(cameraMetadata);
	}

	return result;
}

bool ALiveVideo::cameraSensorPysicalSize(ACameraManager* cameraManager, const std::string& cameraId, float& cameraSensorPhysicalSizeX)
{
	ocean_assert(cameraManager != nullptr && !cameraId.empty());

	bool result = false;

	ACameraMetadata* cameraMetadata;
	if (NativeCameraLibrary::get().ACameraManager_getCameraCharacteristics(cameraManager, cameraId.c_str(), &cameraMetadata) == ACAMERA_OK)
	{
		ACameraMetadata_const_entry constEntryPysicalSize = {0};
		if (NativeCameraLibrary::get().ACameraMetadata_getConstEntry(cameraMetadata, ACAMERA_SENSOR_INFO_PHYSICAL_SIZE, &constEntryPysicalSize) == ACAMERA_OK)
		{
			cameraSensorPhysicalSizeX = constEntryPysicalSize.data.f[0];

			result = true;
		}

		NativeCameraLibrary::get().ACameraMetadata_free(cameraMetadata);
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

#else // __ANDROID_API__ >= 24

jboolean Java_com_meta_ocean_media_android_MediaAndroidJni_onNewCameraFrame(JNIEnv* env, jobject javaThis, jint cameraId, jbyteArray imageBuffer, jint width, jint height, jint format, jfloat fovX, jint bufferSize, jdouble timestamp)
{
	FrameType::PixelFormat frameFormat(FrameType::FORMAT_UNDEFINED);
	FrameType::PixelOrigin frameOrigin(FrameType::ORIGIN_INVALID);

	switch (format)
	{
		// Android ImageFormat.RGB_565
		case 4:
			frameFormat = FrameType::FORMAT_BGR565;
			frameOrigin = FrameType::ORIGIN_LOWER_LEFT;
			break;

		// Android ImageFormat.NV21 (default preview format)
		case 17:
			frameFormat = FrameType::FORMAT_Y_VU12;
			frameOrigin = FrameType::ORIGIN_UPPER_LEFT;
			break;

		// Android ImageFormat.YUY2
		case 20:
			frameFormat = FrameType::FORMAT_YUYV16;
			frameOrigin = FrameType::ORIGIN_UPPER_LEFT;
			break;

		// Android ImageFormat.YV12
		case 0x32315659:
			frameFormat = FrameType::FORMAT_Y_V_U12;
			frameOrigin = FrameType::ORIGIN_UPPER_LEFT;
			break;
	}

	if (frameFormat == FrameType::FORMAT_UNDEFINED)
	{
		Log::warning() << "Received unknown frame type " << format << " as live video.";
		return false;
	}

	jboolean isCopy;
	jbyte* bytes = env->GetByteArrayElements(imageBuffer, &isCopy);
	ocean_assert(bytes);

	bool result = false;

	if (bytes)
	{
		const uint8_t* data = reinterpret_cast<const uint8_t*>(bytes);

		const Timestamp oceanTimestamp = (timestamp >= 0) ? Timestamp(timestamp) : Timestamp(true);

		const FrameType frameType(width, height, frameFormat, frameOrigin);

		if (bufferSize >= int(frameType.frameTypeSize()))
		{
			Frame::PlaneInitializers<void> planeInitializers;
			planeInitializers.reserve(4);

			constexpr unsigned int dataPaddingElements = 0u;

			for (unsigned int planeIndex = 0u; planeIndex < frameType.numberPlanes(); ++planeIndex)
			{
				unsigned int planeWidth = 0u;
				unsigned int planeHeight = 0u;
				unsigned int planeChannels = 0u;

				if (Frame::planeLayout(frameType, planeIndex, planeWidth, planeHeight, planeChannels))
				{
					planeInitializers.emplace_back((const void*)data, Frame::CM_USE_KEEP_LAYOUT, dataPaddingElements);

					data += planeWidth * planeHeight * planeChannels * frameType.bytesPerDataType();
				}
				else
				{
					Log::error() << "Invalid plane layout!";
				}
			}

			if (planeInitializers.size() == frameType.numberPlanes())
			{
				Frame frame(frameType, planeInitializers, oceanTimestamp);

				SharedAnyCamera anyCamera;

				if (fovX > 0.0f)
				{
					anyCamera = std::make_shared<AnyCameraPinhole>(PinholeCamera(frame.width(), frame.height(), Scalar(fovX)));
				}

				result = ALiveVideo::ALiveVideoManager::get().onNewFrame(cameraId, Frame(frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT), std::move(anyCamera));
			}
		}
		else
		{
			Log::error() << "Invalid image buffer size.";
		}

		env->ReleaseByteArrayElements(imageBuffer, bytes, JNI_ABORT);
	}

	return result;
}

ALiveVideo::ALiveVideoManager::ALiveVideoManager()
{
	// nothing to do here
}

ALiveVideo::ALiveVideoManager::~ALiveVideoManager()
{
	// nothing to do here
}

bool ALiveVideo::ALiveVideoManager::onNewFrame(const int cameraId, Frame&& frame, SharedAnyCamera&& anyCamera)
{
	const ScopedLock scopedLock(managerLock);

	const ObjectMap::const_iterator i = managerObjectMap.find(cameraId);
	ocean_assert(i != managerObjectMap.end());

	if (i != managerObjectMap.end())
	{
		ocean_assert(i->second);
		return i->second->onNewFrame(std::move(frame), std::move(anyCamera));
	}

	return false;
}

int ALiveVideo::ALiveVideoManager::createCamera(ALiveVideo* object, const std::string& url, const unsigned int preferredWidth, const unsigned int preferredHeight, const float preferredFrequency)
{
	JNIEnv* environment = Platform::Android::NativeInterfaceManager::get().environment();

	if (environment)
	{
		jclass cameraClass = environment->FindClass("com/meta/ocean/media/android/MediaAndroidJni");

		if (cameraClass)
		{
			// we want the function id of:
			// static int MediaAndroidJni::createCamera(String url, int preferredWidth, int preferredHeight, float preferredFrequency);
			//
			//  String data type: Lfully-qualified-class;
			// Integer data type: I
			//   Float data type: F

			jmethodID functionId = environment->GetStaticMethodID(cameraClass, "createCamera", "(Ljava/lang/String;IIF)I");

			if (functionId)
			{
				jstring javaString = environment->NewStringUTF(url.c_str());
				const int cameraId = environment->CallStaticIntMethod(cameraClass, functionId, javaString, (int)preferredWidth, (int)preferredHeight, preferredFrequency);

				environment->DeleteLocalRef(javaString);

				const ScopedLock scopedLock(managerLock);

				ocean_assert(managerObjectMap.find(cameraId) == managerObjectMap.end());
				managerObjectMap.insert(std::make_pair(cameraId, object));

				return cameraId;
			}
		}
		else
		{
			Log::error() << "Failed to determined Java class \"MediaAndroidJni\"";
		}
	}

	return -1;
}

void ALiveVideo::ALiveVideoManager::releaseCamera(const int cameraId)
{
	if (cameraId != -1)
	{
		JNIEnv* environment = Platform::Android::NativeInterfaceManager::get().environment();

		if (environment)
		{
			jclass cameraClass = environment->FindClass("com/meta/ocean/media/android/MediaAndroidJni");

			if (cameraClass)
			{
				jmethodID functionId = environment->GetStaticMethodID(cameraClass, "releaseCamera", "(I)Z");

				if (functionId)
				{
					environment->CallStaticBooleanMethod(cameraClass, functionId, cameraId);

					const ScopedLock scopedLock(managerLock);

					ObjectMap::iterator i = managerObjectMap.find(cameraId);
					ocean_assert(i != managerObjectMap.end());

					managerObjectMap.erase(i);
				}
			}
			else
			{
				Log::error() << "Failed to determined Java class \"MediaAndroidJni\"";
			}
		}
	}
}

bool ALiveVideo::ALiveVideoManager::startCamera(const int cameraId)
{
	JNIEnv* environment = Platform::Android::NativeInterfaceManager::get().environment();

	if (environment)
	{
		jclass cameraClass = environment->FindClass("com/meta/ocean/media/android/MediaAndroidJni");

		if (cameraClass)
		{
			jmethodID functionId = environment->GetStaticMethodID(cameraClass, "startCamera", "(I)Z");

			if (functionId)
			{
				const bool result = environment->CallStaticBooleanMethod(cameraClass, functionId, cameraId);
				Log::info() << "Create camera result: " << result;
				return result;
			}
		}
		else
		{
			Log::error() << "Failed to determined Java class \"MediaAndroidJni\"";
		}
	}

	return false;
}

bool ALiveVideo::ALiveVideoManager::pauseCamera(const int cameraId)
{
	JNIEnv* environment = Platform::Android::NativeInterfaceManager::get().environment();

	if (environment)
	{
		jclass cameraClass = environment->FindClass("com/meta/ocean/media/android/MediaAndroidJni");

		if (cameraClass)
		{
			jmethodID functionId = environment->GetStaticMethodID(cameraClass, "pauseCamera", "(I)Z");

			if (functionId)
				return environment->CallStaticBooleanMethod(cameraClass, functionId, cameraId);
		}
		else
		{
			Log::error() << "Failed to determined Java class \"MediaAndroidJni\"";
		}
	}

	return false;
}

bool ALiveVideo::ALiveVideoManager::stopCamera(const int cameraId)
{
	JNIEnv* environment = Platform::Android::NativeInterfaceManager::get().environment();

	if (environment)
	{
		jclass cameraClass = environment->FindClass("com/meta/ocean/media/android/MediaAndroidJni");

		if (cameraClass)
		{
			jmethodID functionId = environment->GetStaticMethodID(cameraClass, "stopCamera", "(I)Z");

			if (functionId)
			{
				return environment->CallStaticBooleanMethod(cameraClass, functionId, cameraId);
			}
		}
		else
		{
			Log::error() << "Failed to determined Java class \"MediaAndroidJni\"";
		}
	}

	return false;
}

ALiveVideo::ALiveVideo(const std::string& url) :
	Medium(url),
	AMedium(url),
	FrameMedium(url),
	ConfigMedium(url),
	LiveMedium(url),
	LiveVideo(url)
{
	std::string brand;
	if (!Platform::Android::Utilities::deviceBrand(Platform::Android::NativeInterfaceManager::get().environment(), brand) || brand != "oculus")
	{
		// ALiveVideo does not exist on Oculus devices

		libraryName_ = nameAndroidLibrary();

		frameCollection_.setCapacity(10);

		isValid_ = true;
	}
}

ALiveVideo::~ALiveVideo()
{
	stop();

	ALiveVideoManager::get().releaseCamera(mediumCameraId);
}

bool ALiveVideo::isStarted() const
{
	return mediumIsStarted;
}

Timestamp ALiveVideo::startTimestamp() const
{
	return mediumStartTimestamp;
}

Timestamp ALiveVideo::pauseTimestamp() const
{
	return mediumPauseTimestamp;
}

Timestamp ALiveVideo::stopTimestamp() const
{
	return mediumStopTimestamp;
}

HomogenousMatrixD4 ALiveVideo::device_T_camera() const
{
	if (url_ == "LiveVideoId:0")
	{
		// back-facing camera
		// the camera on all Android devices is rotated by 90 degree in relation to the screen

		return HomogenousMatrixD4(QuaternionD(VectorD3(0.0, 0.0, 1.0), -NumericD::pi_2()));
	}
	else
	{
		// front-facing camera
		// the user facing camera has a different orientation than the user facing camera on Android platforms

		return HomogenousMatrixD4(QuaternionD(VectorD3(0.0, 0.0, 1.0), -NumericD::pi_2()) * QuaternionD(VectorD3(0.0, 1.0, 0.0), NumericD::pi()));
	}
}

bool ALiveVideo::forceRestart()
{
	const ScopedLock scopedLock(lock_);

	if (mediumCameraId == -1 || !mediumIsStarted)
	{
		return false;
	}

	ALiveVideoManager::get().releaseCamera(mediumCameraId);

	mediumCameraId = ALiveVideoManager::get().createCamera(this, url_, preferredFrameType_.width(), preferredFrameType_.height(), (float)recentFrameFrequency_);

	if (mediumCameraId == -1)
	{
		Log::error() << "ALiveVideo: Failed to recreate camera";
		return false;
	}

	if (!ALiveVideoManager::get().startCamera(mediumCameraId))
	{
		Log::error() << "ALiveVideo: Failed to restart camera";
		return false;
	}

	return true;
}

bool ALiveVideo::start()
{
	const ScopedLock scopedLock(lock_);

	if (mediumIsStarted)
	{
		return true;
	}

	if (mediumCameraId == -1)
	{
		mediumCameraId = ALiveVideoManager::get().createCamera(this, url_, preferredFrameType_.width(), preferredFrameType_.height(), (float)recentFrameFrequency_);

		if (mediumCameraId == -1)
		{
			Log::error() << "Failed to create a new camera object with url: " << url_;
			return false;
		}
	}

	Log::info() << "Try to start live camera";

	mediumIsStarted = ALiveVideoManager::get().startCamera(mediumCameraId);

	if (mediumIsStarted)
	{
		Log::info() << "Succeeded to start the live camera.";

		mediumStartTimestamp.toNow();
		mediumPauseTimestamp = Timestamp();
		mediumStopTimestamp = Timestamp();
	}
	else
	{
		Log::warning() << "Failed to start the live camera.";
	}

	return mediumIsStarted;
}

bool ALiveVideo::pause()
{
	const ScopedLock scopedLock(lock_);

	if (!mediumIsStarted)
	{
		return true;
	}

	if (mediumCameraId == -1)
	{
		return false;
	}

	mediumIsStarted = !ALiveVideoManager::get().pauseCamera(mediumCameraId);

	if (!mediumIsStarted)
	{
		mediumPauseTimestamp.toNow();
		mediumStartTimestamp = Timestamp();
		mediumStopTimestamp = Timestamp();
	}

	return !mediumIsStarted;
}

bool ALiveVideo::stop()
{
	const ScopedLock scopedLock(lock_);

	if (!mediumIsStarted)
	{
		return true;
	}

	if (mediumCameraId == -1)
	{
		return false;
	}

	mediumIsStarted = !ALiveVideoManager::get().stopCamera(mediumCameraId);

	if (!mediumIsStarted)
	{
		mediumStopTimestamp.toNow();
		mediumStartTimestamp = Timestamp();
		mediumPauseTimestamp = Timestamp();
	}

	return !mediumIsStarted;
}

bool ALiveVideo::onNewFrame(Frame&& frame, SharedAnyCamera&& anyCamera)
{
	if (anyCamera && anyCamera->name() == AnyCameraPinhole::WrappedCamera::name())
	{
		const PinholeCamera& camera = ((const AnyCameraPinhole&)(*anyCamera)).actualCamera();

		IO::CameraCalibrationManager::get().registerCalibration(url_, camera);
	}

	ocean_assert(frame.isValid());
	ocean_assert(frame.isOwner());

	return deliverNewFrame(std::move(frame), std::move(anyCamera));
}

#endif // __ANDROID_API__ >= 24

}

}

}
