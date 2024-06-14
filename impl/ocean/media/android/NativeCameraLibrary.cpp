/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/NativeCameraLibrary.h"

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#include <dirent.h>
#include <dlfcn.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

NativeCameraLibrary::ScopedACameraIdList::ScopedACameraIdList(ACameraManager* cameraManager)
{
	ocean_assert(cameraManager != nullptr);

	NativeCameraLibrary& nativeCameraLibrary = NativeCameraLibrary::get();

	ACameraIdList* cameraIdList = nullptr;
	if (nativeCameraLibrary.ACameraManager_getCameraIdList(cameraManager, &cameraIdList) == ACAMERA_OK)
	{
		object_ = cameraIdList;
		releaseFunction_ = std::bind(&NativeCameraLibrary::ACameraManager_deleteCameraIdList, &nativeCameraLibrary, cameraIdList);

		ocean_assert(isValid());
	}
	else
	{
		ocean_assert(!isValid());
	}
}

NativeCameraLibrary::ScopedACaptureSessionOutput::ScopedACaptureSessionOutput(ANativeWindow* nativeWindow)
{
	ocean_assert(nativeWindow != nullptr);

	NativeCameraLibrary& nativeCameraLibrary = NativeCameraLibrary::get();

	ACaptureSessionOutput* captureSessionOutput = nullptr;
	if (nativeCameraLibrary.ACaptureSessionOutput_create(nativeWindow, &captureSessionOutput) == ACAMERA_OK)
	{
		object_ = captureSessionOutput;
		releaseFunction_ = std::bind(&NativeCameraLibrary::ACaptureSessionOutput_free, &nativeCameraLibrary, captureSessionOutput);

		ocean_assert(isValid());
	}
	else
	{
		ocean_assert(!isValid());
	}
}

NativeCameraLibrary::NativeCameraLibrary()
{
	// nothing to do here
}

NativeCameraLibrary::ScopedSubscription NativeCameraLibrary::initialize()
{
	const ScopedLock scopedLock(lock_);

	if (initializationCounter_ != 0u)
	{
		ocean_assert(libraryHandle_ != nullptr);

		++initializationCounter_;

		return ScopedSubscription(0u, std::bind(&NativeCameraLibrary::uninitialize, this, std::placeholders::_1));
	}

	if (libraryHandle_ != nullptr)
	{
		ocean_assert(false && "The library is initialized already!");
		return ScopedSubscription();
	}

	libraryHandle_ = dlopen("libcamera2ndk.so", RTLD_LAZY);

	if (libraryHandle_ == nullptr)
	{
		Log::error() << "Failed to load camera2ndk.so library";
		return ScopedSubscription();
	}

	// Loading all function pointers of ACameraDevice

	ocean_assert(ACameraDevice_close_ == nullptr);
	ACameraDevice_close_ = (Function_ACameraDevice_close*)(dlsym(libraryHandle_, "ACameraDevice_close"));
	ocean_assert(ACameraDevice_close_ != nullptr);

	ocean_assert(ACameraDevice_getId_ == nullptr);
	ACameraDevice_getId_ = (Function_ACameraDevice_getId*)(dlsym(libraryHandle_, "ACameraDevice_getId"));
	ocean_assert(ACameraDevice_getId_ != nullptr);

	ocean_assert(ACameraDevice_createCaptureRequest_ == nullptr);
	ACameraDevice_createCaptureRequest_ = (Function_ACameraDevice_createCaptureRequest*)(dlsym(libraryHandle_, "ACameraDevice_createCaptureRequest"));
	ocean_assert(ACameraDevice_createCaptureRequest_ != nullptr);

	ocean_assert(ACaptureSessionOutputContainer_create_ == nullptr);
	ACaptureSessionOutputContainer_create_ = (Function_ACaptureSessionOutputContainer_create*)(dlsym(libraryHandle_, "ACaptureSessionOutputContainer_create"));
	ocean_assert(ACaptureSessionOutputContainer_create_ != nullptr);

	ocean_assert(ACaptureSessionOutputContainer_free_ == nullptr);
	ACaptureSessionOutputContainer_free_ = (Function_ACaptureSessionOutputContainer_free*)(dlsym(libraryHandle_, "ACaptureSessionOutputContainer_free"));
	ocean_assert(ACaptureSessionOutputContainer_free_ != nullptr);

	ocean_assert(ACaptureSessionOutput_create_ == nullptr);
	ACaptureSessionOutput_create_ = (Function_ACaptureSessionOutput_create*)(dlsym(libraryHandle_, "ACaptureSessionOutput_create"));
	ocean_assert(ACaptureSessionOutput_create_ != nullptr);

	ocean_assert(ACaptureSessionOutput_free_ == nullptr);
	ACaptureSessionOutput_free_ = (Function_ACaptureSessionOutput_free*)(dlsym(libraryHandle_, "ACaptureSessionOutput_free"));
	ocean_assert(ACaptureSessionOutput_free_ != nullptr);

	ocean_assert(ACaptureSessionOutputContainer_add_ == nullptr);
	ACaptureSessionOutputContainer_add_ = (Function_ACaptureSessionOutputContainer_add*)(dlsym(libraryHandle_, "ACaptureSessionOutputContainer_add"));
	ocean_assert(ACaptureSessionOutputContainer_add_ != nullptr);

	ocean_assert(ACaptureSessionOutputContainer_remove_ == nullptr);
	ACaptureSessionOutputContainer_remove_ = (Function_ACaptureSessionOutputContainer_remove*)(dlsym(libraryHandle_, "ACaptureSessionOutputContainer_remove"));
	ocean_assert(ACaptureSessionOutputContainer_remove_ != nullptr);

	ocean_assert(ACameraDevice_createCaptureSession_ == nullptr);
	ACameraDevice_createCaptureSession_ = (Function_ACameraDevice_createCaptureSession*)(dlsym(libraryHandle_, "ACameraDevice_createCaptureSession"));
	ocean_assert(ACameraDevice_createCaptureSession_ != nullptr);

	if (ACameraDevice_close_ == nullptr || ACameraDevice_getId_ == nullptr || ACameraDevice_createCaptureRequest_ == nullptr || ACaptureSessionOutputContainer_create_ == nullptr || ACaptureSessionOutputContainer_free_ == nullptr
		|| ACaptureSessionOutput_create_ == nullptr || ACaptureSessionOutput_free_ == nullptr || ACaptureSessionOutputContainer_add_ == nullptr || ACaptureSessionOutputContainer_remove_ == nullptr || ACameraDevice_createCaptureSession_ == nullptr)
	{
		release();
		return ScopedSubscription();
	}


	// Loading all function pointers of ACameraManager

	ocean_assert(ACameraManager_create_ == nullptr);
	ACameraManager_create_ = (Function_ACameraManager_create*)(dlsym(libraryHandle_, "ACameraManager_create"));
	ocean_assert(ACameraManager_create_ != nullptr);

	ocean_assert(ACameraManager_delete_ == nullptr);
	ACameraManager_delete_ = (Function_ACameraManager_delete*)(dlsym(libraryHandle_, "ACameraManager_delete"));
	ocean_assert(ACameraManager_delete_ != nullptr);

	ocean_assert(ACameraManager_getCameraIdList_ == nullptr);
	ACameraManager_getCameraIdList_ = (Function_ACameraManager_getCameraIdList*)(dlsym(libraryHandle_, "ACameraManager_getCameraIdList"));
	ocean_assert(ACameraManager_getCameraIdList_ != nullptr);

	ocean_assert(ACameraManager_deleteCameraIdList_ == nullptr);
	ACameraManager_deleteCameraIdList_ = (Function_ACameraManager_deleteCameraIdList*)(dlsym(libraryHandle_, "ACameraManager_deleteCameraIdList"));
	ocean_assert(ACameraManager_deleteCameraIdList_ != nullptr);

	ocean_assert(ACameraManager_registerAvailabilityCallback_ == nullptr);
	ACameraManager_registerAvailabilityCallback_ = (Function_ACameraManager_registerAvailabilityCallback*)(dlsym(libraryHandle_, "ACameraManager_registerAvailabilityCallback"));
	ocean_assert(ACameraManager_registerAvailabilityCallback_ != nullptr);

	ocean_assert(ACameraManager_unregisterAvailabilityCallback_ == nullptr);
	ACameraManager_unregisterAvailabilityCallback_ = (Function_ACameraManager_unregisterAvailabilityCallback*)(dlsym(libraryHandle_, "ACameraManager_unregisterAvailabilityCallback"));
	ocean_assert(ACameraManager_unregisterAvailabilityCallback_ != nullptr);

	ocean_assert(ACameraManager_getCameraCharacteristics_ == nullptr);
	ACameraManager_getCameraCharacteristics_ = (Function_ACameraManager_getCameraCharacteristics*)(dlsym(libraryHandle_, "ACameraManager_getCameraCharacteristics"));
	ocean_assert(ACameraManager_getCameraCharacteristics_ != nullptr);

	ocean_assert(ACameraManager_openCamera_ == nullptr);
	ACameraManager_openCamera_ = (Function_ACameraManager_openCamera*)(dlsym(libraryHandle_, "ACameraManager_openCamera"));
	ocean_assert(ACameraManager_openCamera_ != nullptr);

	if (ACameraManager_create_ == nullptr || ACameraManager_delete_ == nullptr || ACameraManager_getCameraIdList_ == nullptr || ACameraManager_deleteCameraIdList_ == nullptr
			|| ACameraManager_registerAvailabilityCallback_ == nullptr || ACameraManager_unregisterAvailabilityCallback_ == nullptr || ACameraManager_getCameraCharacteristics_ == nullptr || ACameraManager_openCamera_ == nullptr)
	{
		release();
		return ScopedSubscription();
	}


	// Loading all function pointers of ACameraMetadata

	ocean_assert(ACameraMetadata_getConstEntry_ == nullptr);
	ACameraMetadata_getConstEntry_ = (Function_ACameraMetadata_getConstEntry*)(dlsym(libraryHandle_, "ACameraMetadata_getConstEntry"));
	ocean_assert(ACameraMetadata_getConstEntry_ != nullptr);

	ocean_assert(ACameraMetadata_getAllTags_ == nullptr);
	ACameraMetadata_getAllTags_ = (Function_ACameraMetadata_getAllTags*)(dlsym(libraryHandle_, "ACameraMetadata_getAllTags"));
	ocean_assert(ACameraMetadata_getAllTags_ != nullptr);

	ocean_assert(ACameraMetadata_copy_ == nullptr);
	ACameraMetadata_copy_ = (Function_ACameraMetadata_copy*)(dlsym(libraryHandle_, "ACameraMetadata_copy"));
	ocean_assert(ACameraMetadata_copy_ != nullptr);

	ocean_assert(ACameraMetadata_free_ == nullptr);
	ACameraMetadata_free_ = (Function_ACameraMetadata_free*)(dlsym(libraryHandle_, "ACameraMetadata_free"));
	ocean_assert(ACameraMetadata_free_ != nullptr);

	if (ACameraMetadata_getConstEntry_ == nullptr || ACameraMetadata_getAllTags_ == nullptr || ACameraMetadata_copy_ == nullptr || ACameraMetadata_free_ == nullptr)
	{
		release();
		return ScopedSubscription();
	}


	// Loading all function pointers for capture requests

	ocean_assert(ACameraOutputTarget_create_ == nullptr);
	ACameraOutputTarget_create_ = (Function_ACameraOutputTarget_create*)(dlsym(libraryHandle_, "ACameraOutputTarget_create"));
	ocean_assert(ACameraOutputTarget_create_ != nullptr);

	ocean_assert(ACameraOutputTarget_free_ == nullptr);
	ACameraOutputTarget_free_ = (Function_ACameraOutputTarget_free*)(dlsym(libraryHandle_, "ACameraOutputTarget_free"));
	ocean_assert(ACameraOutputTarget_free_ != nullptr);

	ocean_assert(ACaptureRequest_addTarget_ == nullptr);
	ACaptureRequest_addTarget_ = (Function_ACaptureRequest_addTarget*)(dlsym(libraryHandle_, "ACaptureRequest_addTarget"));
	ocean_assert(ACaptureRequest_addTarget_ != nullptr);

	ocean_assert(ACaptureRequest_removeTarget_ == nullptr);
	ACaptureRequest_removeTarget_ = (Function_ACaptureRequest_removeTarget*)(dlsym(libraryHandle_, "ACaptureRequest_removeTarget"));
	ocean_assert(ACaptureRequest_removeTarget_ != nullptr);

	ocean_assert(ACaptureRequest_getConstEntry_ == nullptr);
	ACaptureRequest_getConstEntry_ = (Function_ACaptureRequest_getConstEntry*)(dlsym(libraryHandle_, "ACaptureRequest_getConstEntry"));
	ocean_assert(ACaptureRequest_getConstEntry_ != nullptr);

	ocean_assert(ACaptureRequest_getAllTags_ == nullptr);
	ACaptureRequest_getAllTags_ = (Function_ACaptureRequest_getAllTags*)(dlsym(libraryHandle_, "ACaptureRequest_getAllTags"));
	ocean_assert(ACaptureRequest_getAllTags_ != nullptr);

	ocean_assert(ACaptureRequest_setEntry_u8_ == nullptr);
	ACaptureRequest_setEntry_u8_ = (Function_ACaptureRequest_setEntry_u8*)(dlsym(libraryHandle_, "ACaptureRequest_setEntry_u8"));
	ocean_assert(ACaptureRequest_setEntry_u8_ != nullptr);

	ocean_assert(ACaptureRequest_setEntry_i32_ == nullptr);
	ACaptureRequest_setEntry_i32_ = (Function_ACaptureRequest_setEntry_i32*)(dlsym(libraryHandle_, "ACaptureRequest_setEntry_i32"));
	ocean_assert(ACaptureRequest_setEntry_i32_ != nullptr);

	ocean_assert(ACaptureRequest_setEntry_float_ == nullptr);
	ACaptureRequest_setEntry_float_ = (Function_ACaptureRequest_setEntry_float*)(dlsym(libraryHandle_, "ACaptureRequest_setEntry_float"));
	ocean_assert(ACaptureRequest_setEntry_float_ != nullptr);

	ocean_assert(ACaptureRequest_setEntry_i64_ == nullptr);
	ACaptureRequest_setEntry_i64_ = (Function_ACaptureRequest_setEntry_i64*)(dlsym(libraryHandle_, "ACaptureRequest_setEntry_i64"));
	ocean_assert(ACaptureRequest_setEntry_i64_ != nullptr);

	ocean_assert(ACaptureRequest_setEntry_double_ == nullptr);
	ACaptureRequest_setEntry_double_ = (Function_ACaptureRequest_setEntry_double*)(dlsym(libraryHandle_, "ACaptureRequest_setEntry_double"));
	ocean_assert(ACaptureRequest_setEntry_double_ != nullptr);

	ocean_assert(ACaptureRequest_setEntry_rational_ == nullptr);
	ACaptureRequest_setEntry_rational_ = (Function_ACaptureRequest_setEntry_rational*)(dlsym(libraryHandle_, "ACaptureRequest_setEntry_rational"));
	ocean_assert(ACaptureRequest_setEntry_rational_ != nullptr);

	ocean_assert(ACaptureRequest_free_ == nullptr);
	ACaptureRequest_free_ = (Function_ACaptureRequest_free*)(dlsym(libraryHandle_, "ACaptureRequest_free"));
	ocean_assert(ACaptureRequest_free_ != nullptr);

	if (ACameraOutputTarget_create_ == nullptr || ACameraOutputTarget_free_ == nullptr || ACaptureRequest_addTarget_ == nullptr || ACaptureRequest_removeTarget_ == nullptr || ACaptureRequest_getConstEntry_ == nullptr
			|| ACaptureRequest_getAllTags_ == nullptr || ACaptureRequest_setEntry_u8_ == nullptr || ACaptureRequest_setEntry_i32_ == nullptr || ACaptureRequest_setEntry_float_ == nullptr
			|| ACaptureRequest_setEntry_i64_ == nullptr || ACaptureRequest_setEntry_double_ == nullptr || ACaptureRequest_setEntry_rational_ == nullptr || ACaptureRequest_free_ == nullptr)
	{
		release();
		return ScopedSubscription();
	}


	// Loading all function pointers for ACameraCaptureSession

	ocean_assert(ACameraCaptureSession_close_ == nullptr);
	ACameraCaptureSession_close_ = (Function_ACameraCaptureSession_close*)(dlsym(libraryHandle_, "ACameraCaptureSession_close"));
	ocean_assert(ACameraCaptureSession_close_ != nullptr);

	ocean_assert(ACameraCaptureSession_getDevice_ == nullptr);
	ACameraCaptureSession_getDevice_ = (Function_ACameraCaptureSession_getDevice*)(dlsym(libraryHandle_, "ACameraCaptureSession_getDevice"));
	ocean_assert(ACameraCaptureSession_getDevice_ != nullptr);

	ocean_assert(ACameraCaptureSession_capture_ == nullptr);
	ACameraCaptureSession_capture_ = (Function_ACameraCaptureSession_capture*)(dlsym(libraryHandle_, "ACameraCaptureSession_capture"));
	ocean_assert(ACameraCaptureSession_capture_ != nullptr);

	ocean_assert(ACameraCaptureSession_setRepeatingRequest_ == nullptr);
	ACameraCaptureSession_setRepeatingRequest_ = (Function_ACameraCaptureSession_setRepeatingRequest*)(dlsym(libraryHandle_, "ACameraCaptureSession_setRepeatingRequest"));
	ocean_assert(ACameraCaptureSession_setRepeatingRequest_ != nullptr);

	ocean_assert(ACameraCaptureSession_stopRepeating_ == nullptr);
	ACameraCaptureSession_stopRepeating_ = (Function_ACameraCaptureSession_stopRepeating*)(dlsym(libraryHandle_, "ACameraCaptureSession_stopRepeating"));
	ocean_assert(ACameraCaptureSession_stopRepeating_ != nullptr);

	ocean_assert(ACameraCaptureSession_abortCaptures_ == nullptr);
	ACameraCaptureSession_abortCaptures_ = (Function_ACameraCaptureSession_abortCaptures*)(dlsym(libraryHandle_, "ACameraCaptureSession_abortCaptures"));
	ocean_assert(ACameraCaptureSession_abortCaptures_ != nullptr);

	if (ACameraCaptureSession_close_ == nullptr || ACameraCaptureSession_getDevice_ == nullptr || ACameraCaptureSession_capture_ == nullptr || ACameraCaptureSession_setRepeatingRequest_ == nullptr || ACameraCaptureSession_stopRepeating_ == nullptr || ACameraCaptureSession_abortCaptures_ == nullptr)
	{
		release();
		return ScopedSubscription();
	}

	ocean_assert(initializationCounter_ == 0u);
	initializationCounter_ = 1u;

	return ScopedSubscription(0u, std::bind(&NativeCameraLibrary::uninitialize, this, std::placeholders::_1));
}

void NativeCameraLibrary::uninitialize(const unsigned int /*unused*/)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(initializationCounter_ != 0u);

	if (--initializationCounter_ == 0u)
	{
		release();
	}
}


void NativeCameraLibrary::release()
{
	ocean_assert(libraryHandle_ != nullptr);
	ocean_assert(initializationCounter_ == 0u);

	const bool result = dlclose(libraryHandle_) == 0;
	libraryHandle_ = nullptr;

	ocean_assert_and_suppress_unused(result, result);

	ACameraDevice_close_ = nullptr;
	ACameraDevice_getId_ = nullptr;
	ACameraDevice_createCaptureRequest_ = nullptr;
	ACaptureSessionOutputContainer_create_ = nullptr;
	ACaptureSessionOutputContainer_free_ = nullptr;
	ACaptureSessionOutput_create_ = nullptr;
	ACaptureSessionOutput_free_ = nullptr;
	ACaptureSessionOutputContainer_add_ = nullptr;
	ACaptureSessionOutputContainer_remove_ = nullptr;
	ACameraDevice_createCaptureSession_ = nullptr;

	ACameraManager_create_ = nullptr;
	ACameraManager_delete_ = nullptr;
	ACameraManager_getCameraIdList_ = nullptr;
	ACameraManager_deleteCameraIdList_ = nullptr;
	ACameraManager_registerAvailabilityCallback_ = nullptr;
	ACameraManager_unregisterAvailabilityCallback_ = nullptr;
	ACameraManager_getCameraCharacteristics_ = nullptr;
	ACameraManager_openCamera_ = nullptr;

	ACameraMetadata_getConstEntry_ = nullptr;
	ACameraMetadata_getAllTags_ = nullptr;
	ACameraMetadata_copy_ = nullptr;
	ACameraMetadata_free_ = nullptr;

	ACameraOutputTarget_create_ = nullptr;
	ACameraOutputTarget_free_ = nullptr;
	ACaptureRequest_addTarget_ = nullptr;
	ACaptureRequest_removeTarget_ = nullptr;
	ACaptureRequest_getConstEntry_ = nullptr;
	ACaptureRequest_getAllTags_ = nullptr;
	ACaptureRequest_setEntry_u8_ = nullptr;
	ACaptureRequest_setEntry_i32_ = nullptr;
	ACaptureRequest_setEntry_float_ = nullptr;
	ACaptureRequest_setEntry_i64_ = nullptr;
	ACaptureRequest_setEntry_double_ = nullptr;
	ACaptureRequest_setEntry_rational_ = nullptr;
	ACaptureRequest_free_ = nullptr;

	ACameraCaptureSession_close_ = nullptr;
	ACameraCaptureSession_getDevice_ = nullptr;
	ACameraCaptureSession_capture_ = nullptr;
	ACameraCaptureSession_setRepeatingRequest_ = nullptr;
	ACameraCaptureSession_stopRepeating_ = nullptr;
	ACameraCaptureSession_abortCaptures_ = nullptr;
}

}

}

}

#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 24
