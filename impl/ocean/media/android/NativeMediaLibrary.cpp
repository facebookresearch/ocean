/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/android/NativeMediaLibrary.h"

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 21

#include <dirent.h>
#include <dlfcn.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

NativeMediaLibrary::NativeMediaLibrary()
{
	// nothing to do here
}

NativeMediaLibrary::ScopedSubscription NativeMediaLibrary::initialize()
{
	const ScopedLock scopedLock(lock_);

	if (initializationCounter_ != 0u)
	{
		ocean_assert(libraryHandle_ != nullptr);

		++initializationCounter_;

		return ScopedSubscription(0u, std::bind(&NativeMediaLibrary::uninitialize, this, std::placeholders::_1));
	}

	libraryHandle_ = dlopen("libmediandk.so", RTLD_LAZY);

	if (libraryHandle_ == nullptr)
	{
		Log::error() << "Failed to load libmediandk.so library";
		return ScopedSubscription();
	}

#if __ANDROID_API__ >= 24

	/// Loading all function pointers of AImage

	ocean_assert(AImage_delete_ == nullptr);
	AImage_delete_ = (Function_AImage_delete*)(dlsym(libraryHandle_, "AImage_delete"));
	ocean_assert(AImage_delete_ != nullptr);

	ocean_assert(AImage_getWidth_ == nullptr);
	AImage_getWidth_ = (Function_AImage_getWidth*)(dlsym(libraryHandle_, "AImage_getWidth"));
	ocean_assert(AImage_getWidth_ != nullptr);

	ocean_assert(AImage_getHeight_ == nullptr);
	AImage_getHeight_ = (Function_AImage_getHeight*)(dlsym(libraryHandle_, "AImage_getHeight"));
	ocean_assert(AImage_getHeight_ != nullptr);

	ocean_assert(AImage_getFormat_ == nullptr);
	AImage_getFormat_ = (Function_AImage_getFormat*)(dlsym(libraryHandle_, "AImage_getFormat"));
	ocean_assert(AImage_getFormat_ != nullptr);

	ocean_assert(AImage_getCropRect_ == nullptr);
	AImage_getCropRect_ = (Function_AImage_getCropRect*)(dlsym(libraryHandle_, "AImage_getCropRect"));
	ocean_assert(AImage_getCropRect_ != nullptr);

	ocean_assert(AImage_getTimestamp_ == nullptr);
	AImage_getTimestamp_ = (Function_AImage_getTimestamp*)(dlsym(libraryHandle_, "AImage_getTimestamp"));
	ocean_assert(AImage_getTimestamp_ != nullptr);

	ocean_assert(AImage_getNumberOfPlanes_ == nullptr);
	AImage_getNumberOfPlanes_ = (Function_AImage_getNumberOfPlanes*)(dlsym(libraryHandle_, "AImage_getNumberOfPlanes"));
	ocean_assert(AImage_getNumberOfPlanes_ != nullptr);

	ocean_assert(AImage_getPlanePixelStride_ == nullptr);
	AImage_getPlanePixelStride_ = (Function_AImage_getPlanePixelStride*)(dlsym(libraryHandle_, "AImage_getPlanePixelStride"));
	ocean_assert(AImage_getPlanePixelStride_ != nullptr);

	ocean_assert(AImage_getPlaneRowStride_ == nullptr);
	AImage_getPlaneRowStride_ = (Function_AImage_getPlaneRowStride*)(dlsym(libraryHandle_, "AImage_getPlaneRowStride"));
	ocean_assert(AImage_getPlaneRowStride_ != nullptr);

	ocean_assert(AImage_getPlaneData_ == nullptr);
	AImage_getPlaneData_ = (Function_AImage_getPlaneData*)(dlsym(libraryHandle_, "AImage_getPlaneData"));
	ocean_assert(AImage_getPlaneData_ != nullptr);

	if (AImage_delete_ == nullptr || AImage_getWidth_ == nullptr || AImage_getHeight_ == nullptr || AImage_getFormat_ == nullptr || AImage_getCropRect_ == nullptr
			|| AImage_getTimestamp_ == nullptr || AImage_getNumberOfPlanes_ == nullptr || AImage_getPlanePixelStride_ == nullptr || AImage_getPlaneRowStride_ == nullptr || AImage_getPlaneData_ == nullptr)
	{
		release();
		return ScopedSubscription();
	}


	/// Loading all function pointers of AImageReader

	ocean_assert(AImageReader_new_ == nullptr);
	AImageReader_new_ = (Function_AImageReader_new*)(dlsym(libraryHandle_, "AImageReader_new"));
	ocean_assert(AImageReader_new_ != nullptr);

	ocean_assert(AImageReader_delete_ == nullptr);
	AImageReader_delete_ = (Function_AImageReader_delete*)(dlsym(libraryHandle_, "AImageReader_delete"));
	ocean_assert(AImageReader_delete_ != nullptr);

	ocean_assert(AImageReader_getWindow_ == nullptr);
	AImageReader_getWindow_ = (Function_AImageReader_getWindow*)(dlsym(libraryHandle_, "AImageReader_getWindow"));
	ocean_assert(AImageReader_getWindow_ != nullptr);

	ocean_assert(AImageReader_getWidth_ == nullptr);
	AImageReader_getWidth_ = (Function_AImageReader_getWidth*)(dlsym(libraryHandle_, "AImageReader_getWidth"));
	ocean_assert(AImageReader_getWidth_ != nullptr);

	ocean_assert(AImageReader_getHeight_ == nullptr);
	AImageReader_getHeight_ = (Function_AImageReader_getHeight*)(dlsym(libraryHandle_, "AImageReader_getHeight"));
	ocean_assert(AImageReader_getHeight_ != nullptr);

	ocean_assert(AImageReader_getFormat_ == nullptr);
	AImageReader_getFormat_ = (Function_AImageReader_getFormat*)(dlsym(libraryHandle_, "AImageReader_getFormat"));
	ocean_assert(AImageReader_getFormat_ != nullptr);

	ocean_assert(AImageReader_getMaxImages_ == nullptr);
	AImageReader_getMaxImages_ = (Function_AImageReader_getMaxImages*)(dlsym(libraryHandle_, "AImageReader_getMaxImages"));
	ocean_assert(AImageReader_getMaxImages_ != nullptr);

	ocean_assert(AImageReader_acquireNextImage_ == nullptr);
	AImageReader_acquireNextImage_ = (Function_AImageReader_acquireNextImage*)(dlsym(libraryHandle_, "AImageReader_acquireNextImage"));
	ocean_assert(AImageReader_acquireNextImage_ != nullptr);

	ocean_assert(AImageReader_acquireLatestImage_ == nullptr);
	AImageReader_acquireLatestImage_ = (Function_AImageReader_acquireLatestImage*)(dlsym(libraryHandle_, "AImageReader_acquireLatestImage"));
	ocean_assert(AImageReader_acquireLatestImage_ != nullptr);

	ocean_assert(AImageReader_setImageListener_ == nullptr);
	AImageReader_setImageListener_ = (Function_AImageReader_setImageListener*)(dlsym(libraryHandle_, "AImageReader_setImageListener"));
	ocean_assert(AImageReader_setImageListener_ != nullptr);

	if (AImageReader_new_ == nullptr || AImageReader_delete_ == nullptr || AImageReader_getWindow_ == nullptr || AImageReader_getWidth_ == nullptr || AImageReader_getHeight_ == nullptr
			|| AImageReader_getFormat_ == nullptr || AImageReader_getMaxImages_ == nullptr || AImageReader_acquireNextImage_ == nullptr || AImageReader_acquireLatestImage_ == nullptr || AImageReader_setImageListener_ == nullptr)
	{
		release();
		return ScopedSubscription();
	}

#endif // __ANDROID_API__ >= 24

	/// Loading all function pointers of AMediaCodec

	ocean_assert(AMediaCodec_createDecoderByType_ == nullptr);
	AMediaCodec_createDecoderByType_ = (Function_AMediaCodec_createDecoderByType*)(dlsym(libraryHandle_, "AMediaCodec_createDecoderByType"));
	ocean_assert(AMediaCodec_createDecoderByType_ != nullptr);

	ocean_assert(AMediaCodec_createEncoderByType_ == nullptr);
	AMediaCodec_createEncoderByType_ = (Function_AMediaCodec_createEncoderByType*)(dlsym(libraryHandle_, "AMediaCodec_createEncoderByType"));
	ocean_assert(AMediaCodec_createEncoderByType_ != nullptr);

	ocean_assert(AMediaCodec_configure_ == nullptr);
	AMediaCodec_configure_ = (Function_AMediaCodec_configure*)(dlsym(libraryHandle_, "AMediaCodec_configure"));
	ocean_assert(AMediaCodec_configure_ != nullptr);

	ocean_assert(AMediaCodec_delete_ == nullptr);
	AMediaCodec_delete_ = (Function_AMediaCodec_delete*)(dlsym(libraryHandle_, "AMediaCodec_delete"));
	ocean_assert(AMediaCodec_delete_ != nullptr);

	ocean_assert(AMediaCodec_dequeueInputBuffer_ == nullptr);
	AMediaCodec_dequeueInputBuffer_ = (Function_AMediaCodec_dequeueInputBuffer*)(dlsym(libraryHandle_, "AMediaCodec_dequeueInputBuffer"));
	ocean_assert(AMediaCodec_dequeueInputBuffer_ != nullptr);

	ocean_assert(AMediaCodec_dequeueOutputBuffer_ == nullptr);
	AMediaCodec_dequeueOutputBuffer_ = (Function_AMediaCodec_dequeueOutputBuffer*)(dlsym(libraryHandle_, "AMediaCodec_dequeueOutputBuffer"));
	ocean_assert(AMediaCodec_dequeueOutputBuffer_ != nullptr);

	ocean_assert(AMediaCodec_flush_ == nullptr);
	AMediaCodec_flush_ = (Function_AMediaCodec_flush*)(dlsym(libraryHandle_, "AMediaCodec_flush"));
	ocean_assert(AMediaCodec_flush_ != nullptr);

	ocean_assert(AMediaCodec_getBufferFormat_ == nullptr);
	AMediaCodec_getBufferFormat_ = (Function_AMediaCodec_getBufferFormat*)(dlsym(libraryHandle_, "AMediaCodec_getBufferFormat"));
	ocean_assert(AMediaCodec_getBufferFormat_ != nullptr);

	ocean_assert(AMediaCodec_getInputBuffer_ == nullptr);
	AMediaCodec_getInputBuffer_ = (Function_AMediaCodec_getInputBuffer*)(dlsym(libraryHandle_, "AMediaCodec_getInputBuffer"));
	ocean_assert(AMediaCodec_getInputBuffer_ != nullptr);

	ocean_assert(AMediaCodec_getInputFormat_ == nullptr);
	AMediaCodec_getInputFormat_ = (Function_AMediaCodec_getInputFormat*)(dlsym(libraryHandle_, "AMediaCodec_getInputFormat"));
	ocean_assert(AMediaCodec_getInputFormat_ != nullptr);

	ocean_assert(AMediaCodec_getOutputBuffer_ == nullptr);
	AMediaCodec_getOutputBuffer_ = (Function_AMediaCodec_getOutputBuffer*)(dlsym(libraryHandle_, "AMediaCodec_getOutputBuffer"));
	ocean_assert(AMediaCodec_getOutputBuffer_ != nullptr);

	ocean_assert(AMediaCodec_getOutputFormat_ == nullptr);
	AMediaCodec_getOutputFormat_ = (Function_AMediaCodec_getOutputFormat*)(dlsym(libraryHandle_, "AMediaCodec_getOutputFormat"));
	ocean_assert(AMediaCodec_getOutputFormat_ != nullptr);

	ocean_assert(AMediaCodec_queueInputBuffer_ == nullptr);
	AMediaCodec_queueInputBuffer_ = (Function_AMediaCodec_queueInputBuffer*)(dlsym(libraryHandle_, "AMediaCodec_queueInputBuffer"));
	ocean_assert(AMediaCodec_queueInputBuffer_ != nullptr);

	ocean_assert(AMediaCodec_releaseOutputBuffer_ == nullptr);
	AMediaCodec_releaseOutputBuffer_ = (Function_AMediaCodec_releaseOutputBuffer*)(dlsym(libraryHandle_, "AMediaCodec_releaseOutputBuffer"));
	ocean_assert(AMediaCodec_releaseOutputBuffer_ != nullptr);

	ocean_assert(AMediaCodec_start_ == nullptr);
	AMediaCodec_start_ = (Function_AMediaCodec_start*)(dlsym(libraryHandle_, "AMediaCodec_start"));
	ocean_assert(AMediaCodec_start_ != nullptr);

	ocean_assert(AMediaCodec_stop_ == nullptr);
	AMediaCodec_stop_ = (Function_AMediaCodec_stop*)(dlsym(libraryHandle_, "AMediaCodec_stop"));
	ocean_assert(AMediaCodec_stop_ != nullptr);

	if (AMediaCodec_createDecoderByType_ == nullptr || AMediaCodec_createEncoderByType_ == nullptr || AMediaCodec_configure_ == nullptr || AMediaCodec_delete_ == nullptr || AMediaCodec_start_ == nullptr || AMediaCodec_stop_ == nullptr)
	{
		release();
		return ScopedSubscription();
	}

	/// Loading all function pointers of AMediaFormat

	ocean_assert(AMediaFormat_delete_ == nullptr);
	AMediaFormat_delete_ = (Function_AMediaFormat_delete*)(dlsym(libraryHandle_, "AMediaFormat_delete"));
	ocean_assert(AMediaFormat_delete_ != nullptr);

	ocean_assert(AMediaFormat_new_ == nullptr);
	AMediaFormat_new_ = (Function_AMediaFormat_new*)(dlsym(libraryHandle_, "AMediaFormat_new"));
	ocean_assert(AMediaFormat_new_ != nullptr);

	ocean_assert(AMediaFormat_getInt32_ == nullptr);
	AMediaFormat_getInt32_ = (Function_AMediaFormat_getInt32*)(dlsym(libraryHandle_, "AMediaFormat_getInt32"));
	ocean_assert(AMediaFormat_getInt32_ != nullptr);

	ocean_assert(AMediaFormat_getInt64_ == nullptr);
	AMediaFormat_getInt64_ = (Function_AMediaFormat_getInt64*)(dlsym(libraryHandle_, "AMediaFormat_getInt64"));
	ocean_assert(AMediaFormat_getInt64_ != nullptr);

	ocean_assert(AMediaFormat_getString_ == nullptr);
	AMediaFormat_getString_ = (Function_AMediaFormat_getString*)(dlsym(libraryHandle_, "AMediaFormat_getString"));
	ocean_assert(AMediaFormat_getString_ != nullptr);

	ocean_assert(AMediaFormat_getRect_ == nullptr);
	AMediaFormat_getRect_ = (Function_AMediaFormat_getRect*)(dlsym(libraryHandle_, "AMediaFormat_getRect"));
	ocean_assert(AMediaFormat_getRect_ != nullptr);

	ocean_assert(AMediaFormat_setBuffer_ == nullptr);
	AMediaFormat_setBuffer_ = (Function_AMediaFormat_setBuffer*)(dlsym(libraryHandle_, "AMediaFormat_setBuffer"));
	ocean_assert(AMediaFormat_setBuffer_ != nullptr);

	ocean_assert(AMediaFormat_setFloat_ == nullptr);
	AMediaFormat_setFloat_ = (Function_AMediaFormat_setFloat*)(dlsym(libraryHandle_, "AMediaFormat_setFloat"));
	ocean_assert(AMediaFormat_setFloat_ != nullptr);

	ocean_assert(AMediaFormat_setInt32_ == nullptr);
	AMediaFormat_setInt32_ = (Function_AMediaFormat_setInt32*)(dlsym(libraryHandle_, "AMediaFormat_setInt32"));
	ocean_assert(AMediaFormat_setInt32_ != nullptr);

	ocean_assert(AMediaFormat_setInt64_ == nullptr);
	AMediaFormat_setInt64_ = (Function_AMediaFormat_setInt64*)(dlsym(libraryHandle_, "AMediaFormat_setInt64"));
	ocean_assert(AMediaFormat_setInt64_ != nullptr);

	ocean_assert(AMediaFormat_setString_ == nullptr);
	AMediaFormat_setString_ = (Function_AMediaFormat_setString*)(dlsym(libraryHandle_, "AMediaFormat_setString"));
	ocean_assert(AMediaFormat_setString_ != nullptr);

	ocean_assert(AMediaFormat_toString_ == nullptr);
	AMediaFormat_toString_ = (Function_AMediaFormat_toString*)(dlsym(libraryHandle_, "AMediaFormat_toString"));
	ocean_assert(AMediaFormat_toString_ != nullptr);

	/// Loading all function pointers of AMediaMuxer

	ocean_assert(AMediaMuxer_addTrack_ == nullptr);
	AMediaMuxer_addTrack_ = (Function_AMediaMuxer_addTrack*)(dlsym(libraryHandle_, "AMediaMuxer_addTrack"));
	ocean_assert(AMediaMuxer_addTrack_ != nullptr);

	ocean_assert(AMediaMuxer_delete_ == nullptr);
	AMediaMuxer_delete_ = (Function_AMediaMuxer_delete*)(dlsym(libraryHandle_, "AMediaMuxer_delete"));
	ocean_assert(AMediaMuxer_delete_ != nullptr);

	ocean_assert(AMediaMuxer_new_ == nullptr);
	AMediaMuxer_new_ = (Function_AMediaMuxer_new*)(dlsym(libraryHandle_, "AMediaMuxer_new"));
	ocean_assert(AMediaMuxer_new_ != nullptr);

	ocean_assert(AMediaExtractor_selectTrack_ == nullptr);
	AMediaExtractor_selectTrack_ = (Function_AMediaExtractor_selectTrack*)(dlsym(libraryHandle_, "AMediaExtractor_selectTrack"));
	ocean_assert(AMediaExtractor_selectTrack_ != nullptr);

	ocean_assert(AMediaMuxer_setLocation_ == nullptr);
	AMediaMuxer_setLocation_ = (Function_AMediaMuxer_setLocation*)(dlsym(libraryHandle_, "AMediaMuxer_setLocation"));
	ocean_assert(AMediaMuxer_setLocation_ != nullptr);

	ocean_assert(AMediaMuxer_setOrientationHint_ == nullptr);
	AMediaMuxer_setOrientationHint_ = (Function_AMediaMuxer_setOrientationHint*)(dlsym(libraryHandle_, "AMediaMuxer_setOrientationHint"));
	ocean_assert(AMediaMuxer_setOrientationHint_ != nullptr);

	ocean_assert(AMediaMuxer_start_ == nullptr);
	AMediaMuxer_start_ = (Function_AMediaMuxer_start*)(dlsym(libraryHandle_, "AMediaMuxer_start"));
	ocean_assert(AMediaMuxer_start_ != nullptr);

	ocean_assert(AMediaMuxer_stop_ == nullptr);
	AMediaMuxer_stop_ = (Function_AMediaMuxer_stop*)(dlsym(libraryHandle_, "AMediaMuxer_stop"));
	ocean_assert(AMediaMuxer_stop_ != nullptr);

	ocean_assert(AMediaMuxer_writeSampleData_ == nullptr);
	AMediaMuxer_writeSampleData_ = (Function_AMediaMuxer_writeSampleData*)(dlsym(libraryHandle_, "AMediaMuxer_writeSampleData"));
	ocean_assert(AMediaMuxer_writeSampleData_ != nullptr);

	if (AMediaMuxer_addTrack_ == nullptr || AMediaMuxer_delete_ == nullptr || AMediaMuxer_new_ == nullptr || AMediaExtractor_selectTrack_ == nullptr || AMediaMuxer_setLocation_ == nullptr || AMediaMuxer_setOrientationHint_ == nullptr || AMediaMuxer_start_ == nullptr || AMediaMuxer_stop_ == nullptr || AMediaMuxer_writeSampleData_ == nullptr)
	{
		release();
		return ScopedSubscription();
	}


	/// Loading all function pointers of AMediaExtractor

	ocean_assert(AMediaExtractor_advance_ == nullptr);
	AMediaExtractor_advance_ = (Function_AMediaExtractor_advance*)(dlsym(libraryHandle_, "AMediaExtractor_advance"));
	ocean_assert(AMediaExtractor_advance_ != nullptr);

	ocean_assert(AMediaExtractor_new_ == nullptr);
	AMediaExtractor_new_ = (Function_AMediaExtractor_new*)(dlsym(libraryHandle_, "AMediaExtractor_new"));
	ocean_assert(AMediaExtractor_new_ != nullptr);

	ocean_assert(AMediaExtractor_delete_ == nullptr);
	AMediaExtractor_delete_ = (Function_AMediaExtractor_delete*)(dlsym(libraryHandle_, "AMediaExtractor_delete"));
	ocean_assert(AMediaExtractor_delete_ != nullptr);

	ocean_assert(AMediaExtractor_setDataSource_ == nullptr);
	AMediaExtractor_setDataSource_ = (Function_AMediaExtractor_setDataSource*)(dlsym(libraryHandle_, "AMediaExtractor_setDataSource"));
	ocean_assert(AMediaExtractor_setDataSource_ != nullptr);

	ocean_assert(AMediaExtractor_setDataSourceFd_ == nullptr);
	AMediaExtractor_setDataSourceFd_ = (Function_AMediaExtractor_setDataSourceFd*)(dlsym(libraryHandle_, "AMediaExtractor_setDataSourceFd"));
	ocean_assert(AMediaExtractor_setDataSourceFd_ != nullptr);

	ocean_assert(AMediaExtractor_getTrackCount_ == nullptr);
	AMediaExtractor_getTrackCount_ = (Function_AMediaExtractor_getTrackCount*)(dlsym(libraryHandle_, "AMediaExtractor_getTrackCount"));
	ocean_assert(AMediaExtractor_getTrackCount_ != nullptr);

	ocean_assert(AMediaExtractor_getTrackFormat_ == nullptr);
	AMediaExtractor_getTrackFormat_ = (Function_AMediaExtractor_getTrackFormat*)(dlsym(libraryHandle_, "AMediaExtractor_getTrackFormat"));
	ocean_assert(AMediaExtractor_getTrackFormat_ != nullptr);

	ocean_assert(AMediaExtractor_readSampleData_ == nullptr);
	AMediaExtractor_readSampleData_ = (Function_AMediaExtractor_readSampleData*)(dlsym(libraryHandle_, "AMediaExtractor_readSampleData"));
	ocean_assert(AMediaExtractor_readSampleData_ != nullptr);

	ocean_assert(AMediaExtractor_getSampleTime_ == nullptr);
	AMediaExtractor_getSampleTime_ = (Function_AMediaExtractor_getSampleTime*)(dlsym(libraryHandle_, "AMediaExtractor_getSampleTime"));
	ocean_assert(AMediaExtractor_getSampleTime_ != nullptr);

	ocean_assert(AMediaExtractor_seekTo_ == nullptr);
	AMediaExtractor_seekTo_ = (Function_AMediaExtractor_seekTo*)(dlsym(libraryHandle_, "AMediaExtractor_seekTo"));
	ocean_assert(AMediaExtractor_seekTo_ != nullptr);

	if (AMediaExtractor_advance_ == nullptr || AMediaExtractor_new_ == nullptr || AMediaExtractor_delete_ == nullptr || AMediaExtractor_setDataSource_ == nullptr || AMediaExtractor_setDataSourceFd_ == nullptr || AMediaExtractor_getTrackCount_ == nullptr || AMediaExtractor_getTrackFormat_ == nullptr || AMediaExtractor_readSampleData_ == nullptr || AMediaExtractor_getSampleTime_ == nullptr || AMediaExtractor_seekTo_ == nullptr)
	{
		release();
		return ScopedSubscription();
	}

	ocean_assert(initializationCounter_ == 0u);
	initializationCounter_ = 1u;

	return ScopedSubscription(0u, std::bind(&NativeMediaLibrary::uninitialize, this, std::placeholders::_1));
}

void NativeMediaLibrary::uninitialize(const unsigned int /*unused*/)
{
	const ScopedLock scopedLock(lock_);

	ocean_assert(initializationCounter_ != 0u);

	if (--initializationCounter_ == 0u)
	{
		release();
	}
}

void NativeMediaLibrary::release()
{
	ocean_assert(libraryHandle_ != nullptr);
	ocean_assert(initializationCounter_ == 0u);

	const bool result = dlclose(libraryHandle_) == 0;
	libraryHandle_ = nullptr;

	ocean_assert_and_suppress_unused(result, result);

#if __ANDROID_API__ >= 24

	AImage_delete_ = nullptr;
	AImage_getWidth_ = nullptr;
	AImage_getHeight_ = nullptr;
	AImage_getFormat_ = nullptr;
	AImage_getCropRect_ = nullptr;
	AImage_getTimestamp_ = nullptr;
	AImage_getNumberOfPlanes_ = nullptr;
	AImage_getPlanePixelStride_ = nullptr;
	AImage_getPlaneRowStride_ = nullptr;
	AImage_getPlaneData_ = nullptr;

	AImageReader_new_ = nullptr;
	AImageReader_delete_ = nullptr;
	AImageReader_getWindow_ = nullptr;
	AImageReader_getWidth_ = nullptr;
	AImageReader_getHeight_ = nullptr;
	AImageReader_getFormat_ = nullptr;
	AImageReader_getMaxImages_ = nullptr;
	AImageReader_acquireNextImage_ = nullptr;
	AImageReader_acquireLatestImage_ = nullptr;
	AImageReader_setImageListener_ = nullptr;

#endif // __ANDROID_API__ >= 24

	AMediaCodec_createDecoderByType_ = nullptr;
	AMediaCodec_createEncoderByType_ = nullptr;
	AMediaCodec_configure_ = nullptr;
	AMediaCodec_delete_ = nullptr;
	AMediaCodec_dequeueInputBuffer_ = nullptr;
	AMediaCodec_dequeueOutputBuffer_ = nullptr;
	AMediaCodec_flush_ = nullptr;
	AMediaCodec_getBufferFormat_ = nullptr;
	AMediaCodec_getInputBuffer_ = nullptr;
	AMediaCodec_getInputFormat_ = nullptr;
	AMediaCodec_getOutputBuffer_ = nullptr;
	AMediaCodec_getOutputFormat_ = nullptr;
	AMediaCodec_queueInputBuffer_ = nullptr;
	AMediaCodec_releaseOutputBuffer_ = nullptr;
	AMediaCodec_start_ = nullptr;
	AMediaCodec_stop_ = nullptr;

	AMediaFormat_delete_ = nullptr;
	AMediaFormat_new_ = nullptr;
	AMediaFormat_getInt32_ = nullptr;
	AMediaFormat_getInt64_ = nullptr;
	AMediaFormat_getString_ = nullptr;
	AMediaFormat_getRect_ = nullptr;
	AMediaFormat_setBuffer_ = nullptr;
	AMediaFormat_setFloat_ = nullptr;
	AMediaFormat_setInt32_ = nullptr;
	AMediaFormat_setInt64_ = nullptr;
	AMediaFormat_setString_ = nullptr;
	AMediaFormat_toString_ = nullptr;

	AMediaMuxer_addTrack_ = nullptr;
	AMediaMuxer_delete_ = nullptr;
	AMediaMuxer_new_ = nullptr;
	AMediaMuxer_setLocation_ = nullptr;
	AMediaMuxer_setOrientationHint_ = nullptr;
	AMediaMuxer_start_ = nullptr;
	AMediaMuxer_stop_ = nullptr;
	AMediaMuxer_writeSampleData_ = nullptr;

	AMediaExtractor_advance_ = nullptr;
	AMediaExtractor_new_ = nullptr;
	AMediaExtractor_delete_ = nullptr;
	AMediaExtractor_selectTrack_ = nullptr;
	AMediaExtractor_setDataSource_ = nullptr;
	AMediaExtractor_setDataSourceFd_ = nullptr;
	AMediaExtractor_getTrackCount_ = nullptr;
	AMediaExtractor_getTrackFormat_ = nullptr;
	AMediaExtractor_readSampleData_ = nullptr;
	AMediaExtractor_getSampleTime_ = nullptr;
	AMediaExtractor_seekTo_ = nullptr;
}

}

}

}

#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 21
