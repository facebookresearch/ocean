/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_NATIVE_CAMERA_LIBRARY_H
#define META_OCEAN_MEDIA_ANDROID_NATIVE_CAMERA_LIBRARY_H

#include "ocean/media/android/Android.h"

#include "ocean/base/ScopedObject.h"
#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Singleton.h"

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#ifndef OCEAN_MEDIA_ANDROID_NATIVECAMERALIBRARY_AVAILABLE
	#define OCEAN_MEDIA_ANDROID_NATIVECAMERALIBRARY_AVAILABLE
#endif

#include <camera/NdkCaptureRequest.h>

#include <camera/NdkCameraCaptureSession.h>
#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraManager.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

/**
 * This class is an interface for Android's shared native library camera2ndk.
 * @ingroup mediaandroid
 */
class OCEAN_MEDIA_A_EXPORT NativeCameraLibrary : public Singleton<NativeCameraLibrary>
{
	friend class Singleton<NativeCameraLibrary>;

	public:

		/**
		 * Definition of a subscription object.
		 */
		using ScopedSubscription = ScopedSubscriptionT<unsigned int, NativeCameraLibrary>;

		/**
		 * Definition of a scoped object for ACameraManager objects.
		 */
		class ScopedACameraManager : public ScopedObjectT<ACameraManager*, ACameraManager*, std::function<void(ACameraManager*)>>
		{
			public:

				/**
				 * Default constructor creating an invalid object.
				 */
				ScopedACameraManager() = default;

				/**
				 * Creates a new scoped object and takes over the ownership of the given camera manager.
				 * @param cameraManager The camera manager to take over, can be nullptr to create an invalid object.
				 */
				explicit inline ScopedACameraManager(ACameraManager* cameraManager);
		};

		/**
		 * Definition of a scoped object for ACameraIdList objects.
		 */
		class ScopedACameraIdList : public ScopedObjectT<ACameraIdList*, ACameraIdList*, std::function<void(ACameraIdList*)>>
		{
			public:

				/**
				 * Default constructor creating an invalid object.
				 */
				ScopedACameraIdList() = default;

				/**
				 * Creates a new scoped object and takes over the ownership of the created camera id list.
				 * @param cameraManager The camera manager which will be used to create the camera id list, must be valid
				 */
				explicit ScopedACameraIdList(ACameraManager* cameraManager);
		};

		/**
		 * Definition of a scoped object for ACaptureSessionOutputContainer objects.
		 */
		class ScopedACaptureSessionOutputContainer : public ScopedObjectT<ACaptureSessionOutputContainer*, ACaptureSessionOutputContainer*, std::function<void(ACaptureSessionOutputContainer*)>>
		{
			public:

				/**
				 * Default constructor creating an invalid object.
				 */
				ScopedACaptureSessionOutputContainer() = default;

				/**
				 * Creates a new scoped object and takes over the ownership of the created capture session output.
				  * @param captureSessionOutputContainer The capture session output container to take over, can be nullptr to create an invalid object.
				 */
				explicit inline ScopedACaptureSessionOutputContainer(ACaptureSessionOutputContainer* captureSessionOutputContainer);
		};

		/**
		 * Definition of a scoped object for ACaptureSessionOutput objects.
		 */
		class ScopedACaptureSessionOutput : public ScopedObjectT<ACaptureSessionOutput*, ACaptureSessionOutput*, std::function<void(ACaptureSessionOutput*)>>
		{
			public:

				/**
				 * Default constructor creating an invalid object.
				 */
				ScopedACaptureSessionOutput() = default;

				/**
				 * Creates a new scoped object and takes over the ownership of the created capture session output.
				 * @param nativeWindow The native window to which the session output will be attached, must be valid
				 */
				explicit ScopedACaptureSessionOutput(ANativeWindow* nativeWindow);
		};

	protected:

		/**
		 * Definition of the individual function pointers of ACameraDevice.
		 */
		using Function_ACameraDevice_close = camera_status_t (ACameraDevice* device);
		using Function_ACameraDevice_getId = const char* (const ACameraDevice* device);
		using Function_ACameraDevice_createCaptureRequest = camera_status_t (const ACameraDevice* device, ACameraDevice_request_template templateId, ACaptureRequest** request);
		using Function_ACaptureSessionOutputContainer_create = camera_status_t (ACaptureSessionOutputContainer** container);
		using Function_ACaptureSessionOutputContainer_free = void (ACaptureSessionOutputContainer* container);
		using Function_ACaptureSessionOutput_create = camera_status_t (ANativeWindow* anw, ACaptureSessionOutput** output);
		using Function_ACaptureSessionOutput_free = void (ACaptureSessionOutput* output);
		using Function_ACaptureSessionOutputContainer_add = camera_status_t (ACaptureSessionOutputContainer* container, const ACaptureSessionOutput* output);
		using Function_ACaptureSessionOutputContainer_remove = camera_status_t (ACaptureSessionOutputContainer* container, const ACaptureSessionOutput* output);
		using Function_ACameraDevice_createCaptureSession = camera_status_t (ACameraDevice* device, const ACaptureSessionOutputContainer* outputs, const ACameraCaptureSession_stateCallbacks* callbacks, ACameraCaptureSession** session);

		/**
		 * Definition of the individual function pointers of ACameraManager.
		 */
		using Function_ACameraManager_create = ACameraManager*();
		using Function_ACameraManager_delete = void (ACameraManager*);
		using Function_ACameraManager_getCameraIdList = camera_status_t (ACameraManager* manager, ACameraIdList** cameraIdList);
		using Function_ACameraManager_deleteCameraIdList = void (ACameraIdList* cameraIdList);
		using Function_ACameraManager_registerAvailabilityCallback = camera_status_t (ACameraManager* manager, const ACameraManager_AvailabilityCallbacks* callback);
		using Function_ACameraManager_unregisterAvailabilityCallback = camera_status_t (ACameraManager* manager, const ACameraManager_AvailabilityCallbacks* callback);
		using Function_ACameraManager_getCameraCharacteristics = camera_status_t (ACameraManager* manager, const char* cameraId, ACameraMetadata** characteristics);
		using Function_ACameraManager_openCamera = camera_status_t (ACameraManager* manager, const char* cameraId, ACameraDevice_StateCallbacks* callback, ACameraDevice** device);

		/**
		 * Definition of the individual function pointers of ACameraMetadata.
		 */
		using Function_ACameraMetadata_getConstEntry = camera_status_t (const ACameraMetadata* metadata, uint32_t tag, ACameraMetadata_const_entry* entry);
		using Function_ACameraMetadata_getAllTags = camera_status_t (const ACameraMetadata* metadata, int32_t* numEntries, const uint32_t** tags);
		using Function_ACameraMetadata_copy = ACameraMetadata* (const ACameraMetadata* src);
		using Function_ACameraMetadata_free = void (ACameraMetadata* metadata);

		/**
		 * Definition of the individual function pointers for capture requests.
		 */
		using Function_ACameraOutputTarget_create = camera_status_t (ANativeWindow* window, ACameraOutputTarget** output);
		using Function_ACameraOutputTarget_free = void (ACameraOutputTarget* output);
		using Function_ACaptureRequest_addTarget = camera_status_t (ACaptureRequest* request, const ACameraOutputTarget* output);
		using Function_ACaptureRequest_removeTarget = camera_status_t (ACaptureRequest* request, const ACameraOutputTarget* output);
		using Function_ACaptureRequest_getConstEntry = camera_status_t (const ACaptureRequest* request, uint32_t tag, ACameraMetadata_const_entry* entry);
		using Function_ACaptureRequest_getAllTags = camera_status_t (const ACaptureRequest* request, int32_t* numTags, const uint32_t** tags);
		using Function_ACaptureRequest_setEntry_u8 = camera_status_t (ACaptureRequest* request, uint32_t tag, uint32_t count, const uint8_t* data);
		using Function_ACaptureRequest_setEntry_i32 = camera_status_t (ACaptureRequest* request, uint32_t tag, uint32_t count, const int32_t* data);
		using Function_ACaptureRequest_setEntry_float = camera_status_t (ACaptureRequest* request, uint32_t tag, uint32_t count, const float* data);
		using Function_ACaptureRequest_setEntry_i64 = camera_status_t (ACaptureRequest* request, uint32_t tag, uint32_t count, const int64_t* data);
		using Function_ACaptureRequest_setEntry_double = camera_status_t (ACaptureRequest* request, uint32_t tag, uint32_t count, const double* data);
		using Function_ACaptureRequest_setEntry_rational = camera_status_t (ACaptureRequest* request, uint32_t tag, uint32_t count, const ACameraMetadata_rational* data);
		using Function_ACaptureRequest_free = void (ACaptureRequest* request);

		/**
		 * Definition of individual function pointers for ACameraCaptureSession.
		 */
		using Function_ACameraCaptureSession_close = void (ACameraCaptureSession* session);
		using Function_ACameraCaptureSession_getDevice = camera_status_t (ACameraCaptureSession* session, /*out*/ACameraDevice** device);
		using Function_ACameraCaptureSession_capture = camera_status_t (ACameraCaptureSession* session, ACameraCaptureSession_captureCallbacks* callbacks, int numRequests, ACaptureRequest** requests, int* captureSequenceId);
		using Function_ACameraCaptureSession_setRepeatingRequest = camera_status_t (ACameraCaptureSession* session, ACameraCaptureSession_captureCallbacks* callbacks, int numRequests, ACaptureRequest** requests, int* captureSequenceId);
		using Function_ACameraCaptureSession_stopRepeating = camera_status_t (ACameraCaptureSession* session);
		using Function_ACameraCaptureSession_abortCaptures = camera_status_t (ACameraCaptureSession* session);

	public:

		/**
		 * Initializes the camera library.
		 * The library will be initialized as long as the resulting subscription object exists.
		 * @return The subscription object, invalid in case the library could not be initialized
		 */
		[[nodiscard]] ScopedSubscription initialize();

		/**
		 * Returns whether the library is initialized.
		 * @return True, if so
		 */
		inline bool isInitialized() const;

		/**
		 * The individual functions of ACameraDevice.
		 */
		inline camera_status_t ACameraDevice_close(ACameraDevice* device);
		inline const char* ACameraDevice_getId(const ACameraDevice* device);
		inline camera_status_t ACameraDevice_createCaptureRequest(const ACameraDevice* device, ACameraDevice_request_template templateId, ACaptureRequest** request);
		inline camera_status_t ACaptureSessionOutputContainer_create(ACaptureSessionOutputContainer** container);
		inline void ACaptureSessionOutputContainer_free(ACaptureSessionOutputContainer* container);
		inline camera_status_t ACaptureSessionOutput_create(ANativeWindow* anw, ACaptureSessionOutput** output);
		inline void ACaptureSessionOutput_free(ACaptureSessionOutput* output);
		inline camera_status_t ACaptureSessionOutputContainer_add(ACaptureSessionOutputContainer* container, const ACaptureSessionOutput* output);
		inline camera_status_t ACaptureSessionOutputContainer_remove(ACaptureSessionOutputContainer* container, const ACaptureSessionOutput* output);
		inline camera_status_t ACameraDevice_createCaptureSession(ACameraDevice* device, const ACaptureSessionOutputContainer* outputs, const ACameraCaptureSession_stateCallbacks* callbacks, ACameraCaptureSession** session);

		/**
		 * The individual functions of ACameraManager.
		 */
		inline ACameraManager* ACameraManager_create();
		inline void ACameraManager_delete(ACameraManager* manager);
		inline camera_status_t ACameraManager_getCameraIdList(ACameraManager* manager, ACameraIdList** cameraIdList);
		inline void ACameraManager_deleteCameraIdList(ACameraIdList* cameraIdList);
		inline camera_status_t ACameraManager_registerAvailabilityCallback(ACameraManager* manager, const ACameraManager_AvailabilityCallbacks* callback);
		inline camera_status_t ACameraManager_unregisterAvailabilityCallback(ACameraManager* manager, const ACameraManager_AvailabilityCallbacks* callback);
		inline camera_status_t ACameraManager_getCameraCharacteristics(ACameraManager* manager, const char* cameraId, ACameraMetadata** characteristics);
		inline camera_status_t ACameraManager_openCamera(ACameraManager* manager, const char* cameraId, ACameraDevice_StateCallbacks* callback, ACameraDevice** device);

		/**
		 * The individual functions of ACameraMetadata.
		 */
		inline camera_status_t ACameraMetadata_getConstEntry(const ACameraMetadata* metadata, uint32_t tag, ACameraMetadata_const_entry* entry);
		inline camera_status_t ACameraMetadata_getAllTags(const ACameraMetadata* metadata, int32_t* numEntries, const uint32_t** tags);
		inline ACameraMetadata* ACameraMetadata_copy(const ACameraMetadata* src);
		inline void ACameraMetadata_free(ACameraMetadata* metadata);

		/**
		 * The individual functions for capture requests.
		 */
		inline camera_status_t ACameraOutputTarget_create(ANativeWindow* window, ACameraOutputTarget** output);
		inline void ACameraOutputTarget_free(ACameraOutputTarget* output);
		inline camera_status_t ACaptureRequest_addTarget(ACaptureRequest* request, const ACameraOutputTarget* output);
		inline camera_status_t ACaptureRequest_removeTarget(ACaptureRequest* request, const ACameraOutputTarget* output);
		inline camera_status_t ACaptureRequest_getConstEntry(const ACaptureRequest* request, uint32_t tag, ACameraMetadata_const_entry* entry);
		inline camera_status_t ACaptureRequest_getAllTags(const ACaptureRequest* request, int32_t* numTags, const uint32_t** tags);
		inline camera_status_t ACaptureRequest_setEntry_u8(ACaptureRequest* request, uint32_t tag, uint32_t count, const uint8_t* data);
		inline camera_status_t ACaptureRequest_setEntry_i32(ACaptureRequest* request, uint32_t tag, uint32_t count, const int32_t* data);
		inline camera_status_t ACaptureRequest_setEntry_float(ACaptureRequest* request, uint32_t tag, uint32_t count, const float* data);
		inline camera_status_t ACaptureRequest_setEntry_i64(ACaptureRequest* request, uint32_t tag, uint32_t count, const int64_t* data);
		inline camera_status_t ACaptureRequest_setEntry_double(ACaptureRequest* request, uint32_t tag, uint32_t count, const double* data);
		inline camera_status_t ACaptureRequest_setEntry_rational(ACaptureRequest* request, uint32_t tag, uint32_t count, const ACameraMetadata_rational* data);
		inline void ACaptureRequest_free(ACaptureRequest* request);

		/**
		 * The individual functions of ACameraCaptureSession.
		 */
		inline void ACameraCaptureSession_close(ACameraCaptureSession* session);
		inline camera_status_t ACameraCaptureSession_getDevice(ACameraCaptureSession* session, ACameraDevice** device);
		inline camera_status_t ACameraCaptureSession_capture(ACameraCaptureSession* session, ACameraCaptureSession_captureCallbacks* callbacks, int numRequests, ACaptureRequest** requests, int* captureSequenceId);
		inline camera_status_t ACameraCaptureSession_setRepeatingRequest(ACameraCaptureSession* session, ACameraCaptureSession_captureCallbacks* callbacks, int numRequests, ACaptureRequest** requests, int* captureSequenceId);
		inline camera_status_t ACameraCaptureSession_stopRepeating(ACameraCaptureSession* session);
		inline camera_status_t ACameraCaptureSession_abortCaptures(ACameraCaptureSession* session);

	protected:

		/**
		 * Protected default constructor.
		 */
		NativeCameraLibrary();

		/**
		 * Destructs the library
		 */
		inline ~NativeCameraLibrary();

		/**
		 * Uninitializes the library.
		 * @param unused An unused id
		 */
		void uninitialize(const unsigned int unused);

		/**
		 * Releases the library.
		 */
		void release();

	protected:

		/// The handle of the library.
		void* libraryHandle_ = nullptr;

		/// All ACameraDevice function pointers.
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraDevice_close* ACameraDevice_close_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraDevice_getId* ACameraDevice_getId_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraDevice_createCaptureRequest* ACameraDevice_createCaptureRequest_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureSessionOutputContainer_create* ACaptureSessionOutputContainer_create_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureSessionOutputContainer_free* ACaptureSessionOutputContainer_free_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureSessionOutput_create* ACaptureSessionOutput_create_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureSessionOutput_free* ACaptureSessionOutput_free_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureSessionOutputContainer_add* ACaptureSessionOutputContainer_add_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureSessionOutputContainer_remove* ACaptureSessionOutputContainer_remove_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraDevice_createCaptureSession* ACameraDevice_createCaptureSession_ = nullptr;

		/// All ACameraManager function pointers.
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraManager_create* ACameraManager_create_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraManager_delete* ACameraManager_delete_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraManager_getCameraIdList* ACameraManager_getCameraIdList_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraManager_deleteCameraIdList* ACameraManager_deleteCameraIdList_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraManager_registerAvailabilityCallback* ACameraManager_registerAvailabilityCallback_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraManager_unregisterAvailabilityCallback* ACameraManager_unregisterAvailabilityCallback_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraManager_getCameraCharacteristics* ACameraManager_getCameraCharacteristics_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraManager_openCamera* ACameraManager_openCamera_ = nullptr;

		/// All ACameraMetadata function pointers.
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraMetadata_getConstEntry* ACameraMetadata_getConstEntry_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraMetadata_getAllTags* ACameraMetadata_getAllTags_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraMetadata_copy* ACameraMetadata_copy_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraMetadata_free* ACameraMetadata_free_ = nullptr;

		/// All capture request function pointers.
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraOutputTarget_create* ACameraOutputTarget_create_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraOutputTarget_free* ACameraOutputTarget_free_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureRequest_addTarget* ACaptureRequest_addTarget_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureRequest_removeTarget* ACaptureRequest_removeTarget_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureRequest_getConstEntry* ACaptureRequest_getConstEntry_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureRequest_getAllTags* ACaptureRequest_getAllTags_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureRequest_setEntry_u8* ACaptureRequest_setEntry_u8_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureRequest_setEntry_i32* ACaptureRequest_setEntry_i32_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureRequest_setEntry_float* ACaptureRequest_setEntry_float_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureRequest_setEntry_i64* ACaptureRequest_setEntry_i64_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureRequest_setEntry_double* ACaptureRequest_setEntry_double_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureRequest_setEntry_rational* ACaptureRequest_setEntry_rational_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACaptureRequest_free* ACaptureRequest_free_ = nullptr;

		/// All ACameraCaptureSession function pointers.
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraCaptureSession_close* ACameraCaptureSession_close_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraCaptureSession_getDevice* ACameraCaptureSession_getDevice_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraCaptureSession_capture* ACameraCaptureSession_capture_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraCaptureSession_setRepeatingRequest* ACameraCaptureSession_setRepeatingRequest_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraCaptureSession_stopRepeating* ACameraCaptureSession_stopRepeating_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_ACameraCaptureSession_abortCaptures* ACameraCaptureSession_abortCaptures_ = nullptr;

		/// The counter for counting the usage of this library.
		unsigned int initializationCounter_ = 0u;

		/// The library's lock.
		mutable Lock lock_;
};

inline NativeCameraLibrary::ScopedACameraManager::ScopedACameraManager(ACameraManager* cameraManager) :
	ScopedObjectT(cameraManager, std::bind(&NativeCameraLibrary::ACameraManager_delete, &NativeCameraLibrary::get(), cameraManager), cameraManager != nullptr)
{
	// nothing to do here
}

inline NativeCameraLibrary::ScopedACaptureSessionOutputContainer::ScopedACaptureSessionOutputContainer(ACaptureSessionOutputContainer* captureSessionOutputContainer) :
	ScopedObjectT(captureSessionOutputContainer, std::bind(&NativeCameraLibrary::ACaptureSessionOutputContainer_free, &NativeCameraLibrary::get(), captureSessionOutputContainer), captureSessionOutputContainer != nullptr)
{
	// nothing to do here
}

inline NativeCameraLibrary::~NativeCameraLibrary()
{
	ocean_assert(initializationCounter_ == 0u);
}

inline bool NativeCameraLibrary::isInitialized() const
{
	const ScopedLock scopedLock(lock_);

	return initializationCounter_ != 0u;
}

inline camera_status_t NativeCameraLibrary::ACameraDevice_close(ACameraDevice* device)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraDevice_close_ != nullptr);

	return ACameraDevice_close_(device);
}

inline const char* NativeCameraLibrary::ACameraDevice_getId(const ACameraDevice* device)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraDevice_getId_ != nullptr);

	return ACameraDevice_getId_(device);
}

inline camera_status_t NativeCameraLibrary::ACameraDevice_createCaptureRequest(const ACameraDevice* device, ACameraDevice_request_template templateId, ACaptureRequest** request)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraDevice_createCaptureRequest_ != nullptr);

	return ACameraDevice_createCaptureRequest_(device, templateId, request);
}

inline camera_status_t NativeCameraLibrary::ACaptureSessionOutputContainer_create(ACaptureSessionOutputContainer** container)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureSessionOutputContainer_create_ != nullptr);

	return ACaptureSessionOutputContainer_create_(container);
}

inline void NativeCameraLibrary::ACaptureSessionOutputContainer_free(ACaptureSessionOutputContainer* container)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureSessionOutputContainer_free_ != nullptr);

	ACaptureSessionOutputContainer_free_(container);
}

inline camera_status_t NativeCameraLibrary::ACaptureSessionOutput_create(ANativeWindow* anw, ACaptureSessionOutput** output)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureSessionOutput_create_ != nullptr);

	return ACaptureSessionOutput_create_(anw, output);
}

inline void NativeCameraLibrary::ACaptureSessionOutput_free(ACaptureSessionOutput* output)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureSessionOutput_free_ != nullptr);

	ACaptureSessionOutput_free_(output);
}

inline camera_status_t NativeCameraLibrary::ACaptureSessionOutputContainer_add(ACaptureSessionOutputContainer* container, const ACaptureSessionOutput* output)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureSessionOutputContainer_add_ != nullptr);

	return ACaptureSessionOutputContainer_add_(container, output);
}

inline camera_status_t NativeCameraLibrary::ACaptureSessionOutputContainer_remove(ACaptureSessionOutputContainer* container, const ACaptureSessionOutput* output)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureSessionOutputContainer_remove_ != nullptr);

	return ACaptureSessionOutputContainer_remove_(container, output);
}

inline camera_status_t NativeCameraLibrary::ACameraDevice_createCaptureSession(ACameraDevice* device, const ACaptureSessionOutputContainer* outputs, const ACameraCaptureSession_stateCallbacks* callbacks, ACameraCaptureSession** session)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraDevice_createCaptureSession_ != nullptr);

	return ACameraDevice_createCaptureSession_(device, outputs, callbacks, session);
}

inline ACameraManager* NativeCameraLibrary::ACameraManager_create()
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraManager_create_ != nullptr);

	return ACameraManager_create_();
}

inline void NativeCameraLibrary::ACameraManager_delete(ACameraManager* manager)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraManager_delete_ != nullptr);

	ACameraManager_delete_(manager);
}

inline camera_status_t NativeCameraLibrary::ACameraManager_getCameraIdList(ACameraManager* manager, ACameraIdList** cameraIdList)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraManager_getCameraIdList_ != nullptr);

	return ACameraManager_getCameraIdList_(manager, cameraIdList);
}

inline void NativeCameraLibrary::ACameraManager_deleteCameraIdList(ACameraIdList* cameraIdList)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraManager_deleteCameraIdList_ != nullptr);

	ACameraManager_deleteCameraIdList_(cameraIdList);
}

inline camera_status_t NativeCameraLibrary::ACameraManager_registerAvailabilityCallback(ACameraManager* manager, const ACameraManager_AvailabilityCallbacks* callback)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraManager_registerAvailabilityCallback_ != nullptr);

	return ACameraManager_registerAvailabilityCallback_(manager, callback);
}

inline camera_status_t NativeCameraLibrary::ACameraManager_unregisterAvailabilityCallback(ACameraManager* manager, const ACameraManager_AvailabilityCallbacks* callback)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraManager_unregisterAvailabilityCallback_ != nullptr);

	return ACameraManager_unregisterAvailabilityCallback_(manager, callback);
}

inline camera_status_t NativeCameraLibrary::ACameraManager_getCameraCharacteristics(ACameraManager* manager, const char* cameraId, ACameraMetadata** characteristics)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraManager_getCameraCharacteristics_ != nullptr);

	return ACameraManager_getCameraCharacteristics_(manager, cameraId, characteristics);
}

inline camera_status_t NativeCameraLibrary::ACameraManager_openCamera(ACameraManager* manager, const char* cameraId, ACameraDevice_StateCallbacks* callback, ACameraDevice** device)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraManager_openCamera_ != nullptr);

	return ACameraManager_openCamera_(manager, cameraId, callback, device);
}

inline camera_status_t NativeCameraLibrary::ACameraMetadata_getConstEntry(const ACameraMetadata* metadata, uint32_t tag, ACameraMetadata_const_entry* entry)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraMetadata_getConstEntry_ != nullptr);

	return ACameraMetadata_getConstEntry_(metadata, tag, entry);
}

inline camera_status_t NativeCameraLibrary::ACameraMetadata_getAllTags(const ACameraMetadata* metadata, int32_t* numEntries, const uint32_t** tags)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraMetadata_getAllTags_ != nullptr);

	return ACameraMetadata_getAllTags_(metadata, numEntries, tags);
}

inline ACameraMetadata* NativeCameraLibrary::ACameraMetadata_copy(const ACameraMetadata* src)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraMetadata_copy_ != nullptr);

	return ACameraMetadata_copy_(src);
}

inline void NativeCameraLibrary::ACameraMetadata_free(ACameraMetadata* metadata)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraMetadata_free_ != nullptr);

	ACameraMetadata_free_(metadata);
}

inline camera_status_t NativeCameraLibrary::ACameraOutputTarget_create(ANativeWindow* window, ACameraOutputTarget** output)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraOutputTarget_create_ != nullptr);

	return ACameraOutputTarget_create_(window, output);
}

inline void NativeCameraLibrary::ACameraOutputTarget_free(ACameraOutputTarget* output)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraOutputTarget_free_ != nullptr);

	return ACameraOutputTarget_free_(output);
}

inline camera_status_t NativeCameraLibrary::ACaptureRequest_addTarget(ACaptureRequest* request, const ACameraOutputTarget* output)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureRequest_addTarget_ != nullptr);

	return ACaptureRequest_addTarget_(request, output);
}

inline camera_status_t NativeCameraLibrary::ACaptureRequest_removeTarget(ACaptureRequest* request, const ACameraOutputTarget* output)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureRequest_removeTarget_ != nullptr);

	return ACaptureRequest_removeTarget_(request, output);
}

inline camera_status_t NativeCameraLibrary::ACaptureRequest_getConstEntry(const ACaptureRequest* request, uint32_t tag, ACameraMetadata_const_entry* entry)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureRequest_getConstEntry_ != nullptr);

	return ACaptureRequest_getConstEntry_(request, tag, entry);
}

inline camera_status_t NativeCameraLibrary::ACaptureRequest_getAllTags(const ACaptureRequest* request, int32_t* numTags, const uint32_t** tags)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureRequest_getAllTags_ != nullptr);

	return ACaptureRequest_getAllTags_(request, numTags, tags);
}

inline camera_status_t NativeCameraLibrary::ACaptureRequest_setEntry_u8(ACaptureRequest* request, uint32_t tag, uint32_t count, const uint8_t* data)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureRequest_setEntry_u8_ != nullptr);

	return ACaptureRequest_setEntry_u8_(request, tag, count, data);
}

inline camera_status_t NativeCameraLibrary::ACaptureRequest_setEntry_i32(ACaptureRequest* request, uint32_t tag, uint32_t count, const int32_t* data)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureRequest_setEntry_i32_ != nullptr);

	return ACaptureRequest_setEntry_i32_(request, tag, count, data);
}

inline camera_status_t NativeCameraLibrary::ACaptureRequest_setEntry_float(ACaptureRequest* request, uint32_t tag, uint32_t count, const float* data)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureRequest_setEntry_float_ != nullptr);

	return ACaptureRequest_setEntry_float_(request, tag, count, data);
}

inline camera_status_t NativeCameraLibrary::ACaptureRequest_setEntry_i64(ACaptureRequest* request, uint32_t tag, uint32_t count, const int64_t* data)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureRequest_setEntry_i64_ != nullptr);

	return ACaptureRequest_setEntry_i64_(request, tag, count, data);
}

inline camera_status_t NativeCameraLibrary::ACaptureRequest_setEntry_double(ACaptureRequest* request, uint32_t tag, uint32_t count, const double* data)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureRequest_setEntry_double_ != nullptr);

	return ACaptureRequest_setEntry_double_(request, tag, count, data);
}

inline camera_status_t NativeCameraLibrary::ACaptureRequest_setEntry_rational(ACaptureRequest* request, uint32_t tag, uint32_t count, const ACameraMetadata_rational* data)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureRequest_setEntry_rational_ != nullptr);

	return ACaptureRequest_setEntry_rational_(request, tag, count, data);
}

inline void NativeCameraLibrary::ACaptureRequest_free(ACaptureRequest* request)
{
	ocean_assert(isInitialized());
	ocean_assert(ACaptureRequest_free_ != nullptr);

	ACaptureRequest_free_(request);
}

inline void NativeCameraLibrary::ACameraCaptureSession_close(ACameraCaptureSession* session)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraCaptureSession_close_ != nullptr);

	ACameraCaptureSession_close_(session);
}

inline camera_status_t NativeCameraLibrary::ACameraCaptureSession_getDevice(ACameraCaptureSession* session, ACameraDevice** device)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraCaptureSession_getDevice_ != nullptr);

	return ACameraCaptureSession_getDevice_(session, device);
}

inline camera_status_t NativeCameraLibrary::ACameraCaptureSession_capture(ACameraCaptureSession* session, ACameraCaptureSession_captureCallbacks* callbacks, int numRequests, ACaptureRequest** requests, int* captureSequenceId)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraCaptureSession_capture_ != nullptr);

	return ACameraCaptureSession_capture_(session, callbacks, numRequests, requests, captureSequenceId);
}

inline camera_status_t NativeCameraLibrary::ACameraCaptureSession_setRepeatingRequest(ACameraCaptureSession* session, ACameraCaptureSession_captureCallbacks* callbacks, int numRequests, ACaptureRequest** requests, int* captureSequenceId)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraCaptureSession_setRepeatingRequest_ != nullptr);

	return ACameraCaptureSession_setRepeatingRequest_(session, callbacks, numRequests, requests, captureSequenceId);
}

inline camera_status_t NativeCameraLibrary::ACameraCaptureSession_stopRepeating(ACameraCaptureSession* session)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraCaptureSession_stopRepeating_ != nullptr);

	return ACameraCaptureSession_stopRepeating_(session);
}

inline camera_status_t NativeCameraLibrary::ACameraCaptureSession_abortCaptures(ACameraCaptureSession* session)
{
	ocean_assert(isInitialized());
	ocean_assert(ACameraCaptureSession_abortCaptures_ != nullptr);

	return ACameraCaptureSession_abortCaptures_(session);
}

}

}

}

#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 24

#endif // META_OCEAN_MEDIA_ANDROID_NATIVE_CAMERA_LIBRARY_H
