/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_ANDROID_NATIVE_MEDIA_LIBRARY_H
#define META_OCEAN_MEDIA_ANDROID_NATIVE_MEDIA_LIBRARY_H

#include "ocean/media/android/Android.h"

#include "ocean/base/ScopedObject.h"
#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Singleton.h"

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 21

#ifndef OCEAN_MEDIA_ANDROID_NATIVEMEDIALIBRARY_AVAILABLE
	#define OCEAN_MEDIA_ANDROID_NATIVEMEDIALIBRARY_AVAILABLE
#endif

#if __ANDROID_API__ >= 24
	#include <media/NdkImageReader.h>
#endif // __ANDROID_API__ >= 24

#include <media/NdkMediaCodec.h>
#include <media/NdkMediaExtractor.h>
#include <media/NdkMediaFormat.h>
#include <media/NdkMediaMuxer.h>

namespace Ocean
{

namespace Media
{

namespace Android
{

/**
 * This class is an interface for Android's shared native library mediandk.
 * @ingroup mediaandroid
 */
class OCEAN_MEDIA_A_EXPORT NativeMediaLibrary : public Singleton<NativeMediaLibrary>
{
	friend class Singleton<NativeMediaLibrary>;

	public:

		/**
		 * Definition of a subscription object.
		 */
		using ScopedSubscription = ScopedSubscriptionT<unsigned int, NativeMediaLibrary>;

		/**
		 * Definition of a scoped object for AMediaFormat objects.
		 */
		class ScopedAMediaFormat : public ScopedObjectT<AMediaFormat*, AMediaFormat*, std::function<media_status_t(AMediaFormat*)>>
		{
			public:

				/**
				 * Default constructor creating an invalid object.
				 */
				ScopedAMediaFormat() = default;

				/**
				 * Creates a new scoped object and takes over the ownership of the given media format.
				 * @param mediaFormat The media format to take over, can be nullptr to create an invalid object.
				 */
				explicit inline ScopedAMediaFormat(AMediaFormat* mediaFormat);
		};

		/**
		 * Definition of a scoped object for AMediaCodec objects.
		 */
		class ScopedAMediaCodec : public ScopedObjectT<AMediaCodec*, AMediaCodec*, std::function<media_status_t(AMediaCodec*)>>
		{
			public:

				/**
				 * Default constructor creating an invalid object.
				 */
				ScopedAMediaCodec() = default;

				/**
				 * Creates a new scoped object and takes over the ownership of the given media codec.
				 * @param mediaCodec The media codec to take over, can be nullptr to create an invalid object.
				 */
				explicit inline ScopedAMediaCodec(AMediaCodec* mediaCodec);
		};

		/**
		 * Definition of individual MediaFormat keys.
		 */
		static constexpr const char* AMEDIAFORMAT_KEY_MIME = "mime";
		static constexpr const char* AMEDIAFORMAT_KEY_COLOR_FORMAT = "color-format";
		static constexpr const char* AMEDIAFORMAT_KEY_COLOR_RANGE = "color-range";
		static constexpr const char* AMEDIAFORMAT_KEY_WIDTH = "width";
		static constexpr const char* AMEDIAFORMAT_KEY_HEIGHT = "height";
		static constexpr const char* AMEDIAFORMAT_KEY_STRIDE = "stride";
		static constexpr const char* AMEDIAFORMAT_KEY_SLICE_HEIGHT = "slice-height";
		static constexpr const char* AMEDIAFORMAT_KEY_DURATION = "durationUs";
		static constexpr const char* AMEDIAFORMAT_KEY_BITS_PER_SAMPLE = "bits-per-sample";
		static constexpr const char* AMEDIAFORMAT_KEY_BIT_RATE = "bitrate";
		static constexpr const char* AMEDIAFORMAT_KEY_SAMPLE_RATE = "sample-rate";
		static constexpr const char* AMEDIAFORMAT_KEY_CHANNEL_COUNT = "channel-count";
		static constexpr const char* AMEDIAFORMAT_KEY_PCM_ENCODING = "pcm-encoding";
		static constexpr const char* AMEDIAFORMAT_KEY_DISPLAY_HEIGHT = "display-height";
		static constexpr const char* AMEDIAFORMAT_KEY_DISPLAY_CROP = "crop";
		static constexpr const char* AMEDIAFORMAT_KEY_CAPTURE_RATE = "capture-rate";
		static constexpr const char* AMEDIAFORMAT_KEY_FRAME_RATE = "frame-rate";
		static constexpr const char* AMEDIAFORMAT_KEY_I_FRAME_INTERVAL = "i-frame-interval";

	protected:

#if __ANDROID_API__ >= 24

		/**
		 * Definition of the individual function pointers of AImage.
		 */
		using Function_AImage_delete = void (AImage* image);
		using Function_AImage_getWidth = media_status_t (const AImage* image, int32_t* width);
		using Function_AImage_getHeight = media_status_t (const AImage* image, int32_t* height);
		using Function_AImage_getFormat = media_status_t (const AImage* image, int32_t* format);
		using Function_AImage_getCropRect = media_status_t (const AImage* image, AImageCropRect* rect);
		using Function_AImage_getTimestamp = media_status_t (const AImage* image, int64_t* timestampNs);
		using Function_AImage_getNumberOfPlanes = media_status_t (const AImage* image, int32_t* numPlanes);
		using Function_AImage_getPlanePixelStride = media_status_t (const AImage* image, int planeIdx, int32_t* pixelStride);
		using Function_AImage_getPlaneRowStride = media_status_t (const AImage* image, int planeIdx, int32_t* rowStride);
		using Function_AImage_getPlaneData = media_status_t (const AImage* image, int planeIdx, uint8_t** data, int* dataLength);

		/**
		 * Definition of the individual function pointers of AImageReader.
		 */
		using Function_AImageReader_new = media_status_t (int32_t width, int32_t height, int32_t format, int32_t maxImages, AImageReader** reader);
		using Function_AImageReader_delete = void (AImageReader* reader);
		using Function_AImageReader_getWindow = media_status_t (AImageReader* reader, ANativeWindow** window);
		using Function_AImageReader_getWidth = media_status_t (const AImageReader* reader, int32_t* width);
		using Function_AImageReader_getHeight = media_status_t (const AImageReader* reader, int32_t* height);
		using Function_AImageReader_getFormat = media_status_t (const AImageReader* reader, int32_t* format);
		using Function_AImageReader_getMaxImages = media_status_t (const AImageReader* reader, int32_t* maxImages);
		using Function_AImageReader_acquireNextImage = media_status_t (AImageReader* reader, AImage** image);
		using Function_AImageReader_acquireLatestImage = media_status_t (AImageReader* reader, AImage** image);
		using Function_AImageReader_setImageListener = media_status_t (AImageReader* reader, AImageReader_ImageListener* listener);

#endif // __ANDROID_API__ >= 24

		/**
		 * Definition of the individual function pointers of AMediaCodec.
		 */
		using Function_AMediaCodec_createDecoderByType = AMediaCodec* (const char* mime_type);
		using Function_AMediaCodec_createEncoderByType = AMediaCodec* (const char* mime_type);
		using Function_AMediaCodec_configure = media_status_t (AMediaCodec* codec, const AMediaFormat* format, ANativeWindow* surface, AMediaCrypto* crypto, uint32_t flags);
		using Function_AMediaCodec_delete = media_status_t (AMediaCodec* codec);
		using Function_AMediaCodec_dequeueInputBuffer = ssize_t (AMediaCodec* codec, int64_t timeoutUs);
		using Function_AMediaCodec_dequeueOutputBuffer = ssize_t (AMediaCodec* codec, AMediaCodecBufferInfo* info, int64_t timeoutUs);
		using Function_AMediaCodec_flush = media_status_t (AMediaCodec* codec);
		using Function_AMediaCodec_getBufferFormat = AMediaFormat* (AMediaCodec* codec, size_t index);
		using Function_AMediaCodec_getInputBuffer = uint8_t* (AMediaCodec* codec, size_t idx, size_t* out_size);
		using Function_AMediaCodec_getInputFormat = AMediaFormat* (AMediaCodec* codec);
		using Function_AMediaCodec_getOutputBuffer = uint8_t* (AMediaCodec* codec, size_t idx, size_t* out_size);
		using Function_AMediaCodec_getOutputFormat = AMediaFormat* (AMediaCodec* codec);
		using Function_AMediaCodec_queueInputBuffer = media_status_t (AMediaCodec* codec, size_t idx, unsigned int offset, size_t size, uint64_t time, uint32_t flags);
		using Function_AMediaCodec_releaseOutputBuffer = media_status_t (AMediaCodec* codec, size_t idx, bool render);
		using Function_AMediaCodec_start = media_status_t (AMediaCodec* codec);
		using Function_AMediaCodec_stop = media_status_t (AMediaCodec* codec);

		/**
		 * Definition of the individual function pointers of AMediaFormat.
		 */
		using Function_AMediaFormat_delete = media_status_t (AMediaFormat* format);
		using Function_AMediaFormat_new = AMediaFormat*();
		using Function_AMediaFormat_getInt32 = bool (AMediaFormat* format, const char* name, int32_t* out);
		using Function_AMediaFormat_getInt64 = bool (AMediaFormat* format, const char* name, int64_t* out);
		using Function_AMediaFormat_getString = bool (AMediaFormat* format, const char* name, const char** out);
		using Function_AMediaFormat_getRect = bool (AMediaFormat* format, const char *name, int32_t *left, int32_t *top, int32_t *right, int32_t *bottom);
		using Function_AMediaFormat_setBuffer = void (AMediaFormat* format, const char* name, const void* data, size_t size);
		using Function_AMediaFormat_setFloat = void (AMediaFormat* format, const char* name, float value);
		using Function_AMediaFormat_setInt32 = void (AMediaFormat* format, const char* name, int32_t value);
		using Function_AMediaFormat_setInt64 = void (AMediaFormat* format, const char* name, int64_t value);
		using Function_AMediaFormat_setString = void (AMediaFormat* format, const char* name, const char* value);
		using Function_AMediaFormat_toString = const char* (AMediaFormat* format);

		/**
		 * Definition of the individual function pointers of AMediaMuxer.
		 */
		using Function_AMediaMuxer_addTrack = ssize_t (AMediaMuxer* muxer, const AMediaFormat* format);
		using Function_AMediaMuxer_delete = media_status_t (AMediaMuxer* muxer);
		using Function_AMediaMuxer_new = AMediaMuxer* (int fd, OutputFormat format);
		using Function_AMediaMuxer_setLocation = media_status_t (AMediaMuxer* muxer, float latitude, float longitude);
		using Function_AMediaMuxer_setOrientationHint = media_status_t (AMediaMuxer* muxer, int degrees);
		using Function_AMediaMuxer_start = media_status_t (AMediaMuxer* muxer);
		using Function_AMediaMuxer_stop = media_status_t (AMediaMuxer* muxer);
		using Function_AMediaMuxer_writeSampleData = media_status_t (AMediaMuxer* muxer, size_t trackIdx, const uint8_t* data, const AMediaCodecBufferInfo* info);

		/**
		 * Definition of the individual function pointers of AMediaExtractor.
		 */
		using Function_AMediaExtractor_advance = bool(AMediaExtractor* mediaExtractor);
		using Function_AMediaExtractor_delete = media_status_t (AMediaExtractor* mediaExtractor);
		using Function_AMediaExtractor_getSampleTime = int64_t (AMediaExtractor* mediaExtractor);
		using Function_AMediaExtractor_getTrackCount = size_t (AMediaExtractor* mediaExtractor);
		using Function_AMediaExtractor_getTrackFormat = AMediaFormat* (AMediaExtractor* mediaExtractor, size_t idx);
		using Function_AMediaExtractor_new = AMediaExtractor* ();
		using Function_AMediaExtractor_readSampleData = ssize_t (AMediaExtractor* mediaExtractor, uint8_t* buffer, size_t capacity);
		using Function_AMediaExtractor_seekTo = media_status_t (AMediaExtractor* mediaExtractor, int64_t seekPosUs, SeekMode mode);
		using Function_AMediaExtractor_selectTrack = media_status_t (AMediaExtractor* mediaExtractor, size_t idx);
		using Function_AMediaExtractor_setDataSource = media_status_t (AMediaExtractor* mediaExtractor, const char* location);
		using Function_AMediaExtractor_setDataSourceFd = media_status_t (AMediaExtractor* mediaExtractor, int fd, off64_t offset, off64_t length);

	public:

		/**
		 * Initializes the media library.
		 * The library will be initialized as long as the resulting subscription object exists.
		 * @return The subscription object, invalid in case the library could not be initialized
		 */
		[[nodiscard]] ScopedSubscription initialize();

		/**
		 * Returns whether the library is initialized.
		 * @return True, if so
		 */
		inline bool isInitialized() const;

#if __ANDROID_API__ >= 24

		/**
		 * The individual functions of AImage.
		 */
		inline void AImage_delete(AImage* image) const;
		inline media_status_t AImage_getWidth(const AImage* image, int32_t* width) const;
		inline media_status_t AImage_getHeight(const AImage* image, int32_t* height) const;
		inline media_status_t AImage_getFormat(const AImage* image, int32_t* format) const;
		inline media_status_t AImage_getCropRect(const AImage* image, AImageCropRect* rect) const;
		inline media_status_t AImage_getTimestamp(const AImage* image, int64_t* timestampNs) const;
		inline media_status_t AImage_getNumberOfPlanes(const AImage* image, int32_t* numPlanes) const;
		inline media_status_t AImage_getPlanePixelStride(const AImage* image, int planeIdx, int32_t* pixelStride) const;
		inline media_status_t AImage_getPlaneRowStride(const AImage* image, int planeIdx, int32_t* rowStride) const;
		inline media_status_t AImage_getPlaneData(const AImage* image, int planeIdx, uint8_t** data, int* dataLength) const;

		/**
		 * The individual functions of AImageReader.
		 */
		inline media_status_t AImageReader_new(int32_t width, int32_t height, int32_t format, int32_t maxImages, AImageReader** reader) const;
		inline void AImageReader_delete(AImageReader* reader) const;
		inline media_status_t AImageReader_getWindow(AImageReader* reader, ANativeWindow** window) const;
		inline media_status_t AImageReader_getWidth(const AImageReader* reader, int32_t* width) const;
		inline media_status_t AImageReader_getHeight(const AImageReader* reader, int32_t* height) const;
		inline media_status_t AImageReader_getFormat(const AImageReader* reader, int32_t* format) const;
		inline media_status_t AImageReader_getMaxImages(const AImageReader* reader, int32_t* maxImages) const;
		inline media_status_t AImageReader_acquireNextImage(AImageReader* reader, AImage** image) const;
		inline media_status_t AImageReader_acquireLatestImage(AImageReader* reader, AImage** image) const;
		inline media_status_t AImageReader_setImageListener(AImageReader* reader, AImageReader_ImageListener* listener) const;

#endif // __ANDROID_API__ >= 24

		/**
		 * The individual functions of AMediaCodec.
		 */
		inline AMediaCodec* AMediaCodec_createDecoderByType(const char* mime_type) const;
		inline AMediaCodec* AMediaCodec_createEncoderByType(const char* mime_type) const;
		inline media_status_t AMediaCodec_configure(AMediaCodec* codec, const AMediaFormat* format, ANativeWindow* surface, AMediaCrypto* crypto, uint32_t flags) const;
		inline media_status_t AMediaCodec_delete(AMediaCodec* codec) const;
		inline ssize_t AMediaCodec_dequeueInputBuffer(AMediaCodec* codec, int64_t timeoutUs) const;
		inline ssize_t AMediaCodec_dequeueOutputBuffer(AMediaCodec* codec, AMediaCodecBufferInfo* info, int64_t timeoutUs) const;
		inline media_status_t AMediaCodec_flush(AMediaCodec* codec) const;
		inline AMediaFormat* AMediaCodec_getBufferFormat(AMediaCodec* codec, size_t index) const;
		inline uint8_t* AMediaCodec_getInputBuffer(AMediaCodec* codec, size_t idx, size_t* out_size) const;
		inline AMediaFormat* AMediaCodec_getInputFormat(AMediaCodec* codec) const;
		inline uint8_t* AMediaCodec_getOutputBuffer(AMediaCodec* codec, size_t idx, size_t* out_size) const;
		inline AMediaFormat* AMediaCodec_getOutputFormat(AMediaCodec* codec) const;
		inline media_status_t AMediaCodec_queueInputBuffer(AMediaCodec* codec, size_t idx, unsigned int offset, size_t size, uint64_t time, uint32_t flags) const;
		inline media_status_t AMediaCodec_releaseOutputBuffer(AMediaCodec* codec, size_t idx, bool render) const;
		inline media_status_t AMediaCodec_start(AMediaCodec* codec) const;
		inline media_status_t AMediaCodec_stop(AMediaCodec* codec) const;

		/**
		 * The individual functions of AMediaFormat.
		 */
		inline media_status_t AMediaFormat_delete(AMediaFormat* format) const;
		inline AMediaFormat* AMediaFormat_new() const;
		inline bool AMediaFormat_getInt32(AMediaFormat* mediaFormat, const char* name, int32_t* out) const;
		inline bool AMediaFormat_getInt64(AMediaFormat* mediaFormat, const char* name, int64_t* out) const;
		inline bool AMediaFormat_getString(AMediaFormat* mediaFormat, const char* name, const char** out) const;
		inline bool AMediaFormat_getRect(AMediaFormat *, const char *name, int32_t *left, int32_t *top, int32_t *right, int32_t *bottom) const;
		inline void AMediaFormat_setBuffer(AMediaFormat* format, const char* name, const void* data, size_t size) const;
		inline void AMediaFormat_setFloat(AMediaFormat* format, const char* name, float value) const;
		inline void AMediaFormat_setInt32(AMediaFormat* format, const char* name, int32_t value) const;
		inline void AMediaFormat_setInt64(AMediaFormat* format, const char* name, int64_t value) const;
		inline void AMediaFormat_setString(AMediaFormat* format, const char* name, const char* value) const;
		inline const char* AMediaFormat_toString(AMediaFormat* format) const;

		/**
		 * The individual functions of AMediaMuxer.
		 */
		inline ssize_t AMediaMuxer_addTrack(AMediaMuxer* muxer, const AMediaFormat* format) const;
		inline media_status_t AMediaMuxer_delete(AMediaMuxer* muxer) const;
		inline AMediaMuxer* AMediaMuxer_new(int fd, OutputFormat format) const;
		inline media_status_t AMediaMuxer_setLocation(AMediaMuxer* muxer, float latitude, float longitude) const;
		inline media_status_t AMediaMuxer_setOrientationHint(AMediaMuxer* muxer, int degrees) const;
		inline media_status_t AMediaMuxer_start(AMediaMuxer* muxer) const;
		inline media_status_t AMediaMuxer_stop(AMediaMuxer* muxer) const;
		inline media_status_t AMediaMuxer_writeSampleData(AMediaMuxer* muxer, size_t trackIdx, const uint8_t* data, const AMediaCodecBufferInfo* info) const;

		/**
		 * The individual functions of AMediaExtractor.
		 */
		inline bool AMediaExtractor_advance(AMediaExtractor* mediaExtractor) const;
		inline media_status_t AMediaExtractor_delete(AMediaExtractor* mediaExtractor) const;
		inline int64_t AMediaExtractor_getSampleTime(AMediaExtractor* mediaExtractor) const;
		inline size_t AMediaExtractor_getTrackCount(AMediaExtractor* mediaExtractor) const;
		inline AMediaFormat* AMediaExtractor_getTrackFormat(AMediaExtractor* mediaExtractor, size_t idx) const;
		inline AMediaExtractor* AMediaExtractor_new() const;
		inline ssize_t AMediaExtractor_readSampleData(AMediaExtractor* mediaExtractor, uint8_t* buffer, size_t capacity) const;
		inline media_status_t AMediaExtractor_seekTo(AMediaExtractor* mediaExtractor, int64_t seekPosUs, SeekMode mode) const;
		inline media_status_t AMediaExtractor_selectTrack(AMediaExtractor* mediaExtractor, size_t idx) const;
		inline media_status_t AMediaExtractor_setDataSource(AMediaExtractor* mediaExtractor, const char* location) const;
		inline media_status_t AMediaExtractor_setDataSourceFd(AMediaExtractor* mediaExtractor, int fd, off64_t offset, off64_t length) const;

	protected:

		/**
		 * Protected default constructor.
		 */
		NativeMediaLibrary();

		/**
		 * Destructs the library
		 */
		inline ~NativeMediaLibrary();

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

#if __ANDROID_API__ >= 24

		/// All AImage function pointers.
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImage_delete* AImage_delete_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImage_getWidth* AImage_getWidth_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImage_getHeight* AImage_getHeight_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImage_getFormat* AImage_getFormat_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImage_getCropRect* AImage_getCropRect_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImage_getTimestamp* AImage_getTimestamp_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImage_getNumberOfPlanes* AImage_getNumberOfPlanes_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImage_getPlanePixelStride* AImage_getPlanePixelStride_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImage_getPlaneRowStride* AImage_getPlaneRowStride_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImage_getPlaneData* AImage_getPlaneData_ = nullptr;

		/// All AImageReader function pointers.
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImageReader_new* AImageReader_new_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImageReader_delete* AImageReader_delete_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImageReader_getWindow* AImageReader_getWindow_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImageReader_getWidth* AImageReader_getWidth_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImageReader_getHeight* AImageReader_getHeight_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImageReader_getFormat* AImageReader_getFormat_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImageReader_getMaxImages* AImageReader_getMaxImages_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImageReader_acquireNextImage* AImageReader_acquireNextImage_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImageReader_acquireLatestImage* AImageReader_acquireLatestImage_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AImageReader_setImageListener* AImageReader_setImageListener_ = nullptr;

#endif // __ANDROID_API__ >= 24

		/// All AMediaCodec function pointers.
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_createDecoderByType* AMediaCodec_createDecoderByType_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_createEncoderByType* AMediaCodec_createEncoderByType_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_configure* AMediaCodec_configure_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_delete* AMediaCodec_delete_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_dequeueInputBuffer* AMediaCodec_dequeueInputBuffer_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_dequeueOutputBuffer* AMediaCodec_dequeueOutputBuffer_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_flush* AMediaCodec_flush_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_getBufferFormat* AMediaCodec_getBufferFormat_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_getInputBuffer* AMediaCodec_getInputBuffer_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_getInputFormat* AMediaCodec_getInputFormat_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_getOutputBuffer* AMediaCodec_getOutputBuffer_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_getOutputFormat* AMediaCodec_getOutputFormat_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_queueInputBuffer* AMediaCodec_queueInputBuffer_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_releaseOutputBuffer* AMediaCodec_releaseOutputBuffer_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_start* AMediaCodec_start_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaCodec_stop* AMediaCodec_stop_ = nullptr;

		// All AMediaFormat function pointers.
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaFormat_delete* AMediaFormat_delete_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaFormat_new* AMediaFormat_new_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaFormat_getInt32* AMediaFormat_getInt32_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaFormat_getInt64* AMediaFormat_getInt64_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaFormat_getString* AMediaFormat_getString_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaFormat_getRect* AMediaFormat_getRect_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaFormat_setBuffer* AMediaFormat_setBuffer_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaFormat_setFloat* AMediaFormat_setFloat_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaFormat_setInt32* AMediaFormat_setInt32_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaFormat_setInt64* AMediaFormat_setInt64_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaFormat_setString* AMediaFormat_setString_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaFormat_toString* AMediaFormat_toString_ = nullptr;

		/// All AMediaMixer function pointers.
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaMuxer_addTrack* AMediaMuxer_addTrack_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaMuxer_delete* AMediaMuxer_delete_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaMuxer_new* AMediaMuxer_new_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaMuxer_setLocation* AMediaMuxer_setLocation_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaMuxer_setOrientationHint* AMediaMuxer_setOrientationHint_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaMuxer_start* AMediaMuxer_start_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaMuxer_stop* AMediaMuxer_stop_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaMuxer_writeSampleData* AMediaMuxer_writeSampleData_ = nullptr;

		/// All AMediaExtractor function pointers.
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaExtractor_advance* AMediaExtractor_advance_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaExtractor_delete* AMediaExtractor_delete_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaExtractor_getSampleTime* AMediaExtractor_getSampleTime_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaExtractor_getTrackCount* AMediaExtractor_getTrackCount_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaExtractor_getTrackFormat* AMediaExtractor_getTrackFormat_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaExtractor_new* AMediaExtractor_new_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaExtractor_readSampleData* AMediaExtractor_readSampleData_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaExtractor_seekTo* AMediaExtractor_seekTo_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaExtractor_selectTrack* AMediaExtractor_selectTrack_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaExtractor_setDataSource* AMediaExtractor_setDataSource_ = nullptr;
		__attribute__((annotate("dynamic_fn_ptr"))) Function_AMediaExtractor_setDataSourceFd* AMediaExtractor_setDataSourceFd_ = nullptr;

		/// The counter for counting the usage of this library.
		unsigned int initializationCounter_ = 0u;

		/// The library's lock.
		mutable Lock lock_;
};

inline NativeMediaLibrary::ScopedAMediaFormat::ScopedAMediaFormat(AMediaFormat* mediaFormat) :
	ScopedObjectT(mediaFormat, std::bind(&NativeMediaLibrary::AMediaFormat_delete, &NativeMediaLibrary::get(), mediaFormat), mediaFormat != nullptr)
{
	// nothing to do here
}

inline NativeMediaLibrary::ScopedAMediaCodec::ScopedAMediaCodec(AMediaCodec* mediaCodec) :
	ScopedObjectT(mediaCodec, std::bind(&NativeMediaLibrary::AMediaCodec_delete, &NativeMediaLibrary::get(), mediaCodec), mediaCodec != nullptr)
{
	// nothing to do here
}

inline NativeMediaLibrary::~NativeMediaLibrary()
{
	ocean_assert(initializationCounter_ == 0u);
}

inline bool NativeMediaLibrary::isInitialized() const
{
	const ScopedLock scopedLock(lock_);

	return initializationCounter_ != 0u;
}

#if __ANDROID_API__ >= 24

inline void NativeMediaLibrary::AImage_delete(AImage* image) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImage_delete_ != nullptr);

	AImage_delete_(image);
}

inline media_status_t NativeMediaLibrary::AImage_getWidth(const AImage* image, int32_t* width) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImage_getWidth_ != nullptr);

	return AImage_getWidth_(image, width);
}

inline media_status_t NativeMediaLibrary::AImage_getHeight(const AImage* image, int32_t* height) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImage_getHeight_ != nullptr);

	return AImage_getHeight_(image, height);
}

inline media_status_t NativeMediaLibrary::AImage_getFormat(const AImage* image, int32_t* format) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImage_getFormat_ != nullptr);

	return AImage_getFormat_(image, format);
}

inline media_status_t NativeMediaLibrary::AImage_getCropRect(const AImage* image, AImageCropRect* rect) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImage_getCropRect_ != nullptr);

	return AImage_getCropRect_(image, rect);
}

inline media_status_t NativeMediaLibrary::AImage_getTimestamp(const AImage* image, int64_t* timestampNs) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImage_getTimestamp_ != nullptr);

	return AImage_getTimestamp_(image, timestampNs);
}

inline media_status_t NativeMediaLibrary::AImage_getNumberOfPlanes(const AImage* image, int32_t* numPlanes) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImage_getNumberOfPlanes_ != nullptr);

	return AImage_getNumberOfPlanes_(image, numPlanes);
}

inline media_status_t NativeMediaLibrary::AImage_getPlanePixelStride(const AImage* image, int planeIdx, int32_t* pixelStride) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImage_getPlanePixelStride_ != nullptr);

	return AImage_getPlanePixelStride_(image, planeIdx, pixelStride);
}

inline media_status_t NativeMediaLibrary::AImage_getPlaneRowStride(const AImage* image, int planeIdx, int32_t* rowStride) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImage_getPlaneRowStride_ != nullptr);

	return AImage_getPlaneRowStride_(image, planeIdx, rowStride);
}

inline media_status_t NativeMediaLibrary::AImage_getPlaneData(const AImage* image, int planeIdx, uint8_t** data, int* dataLength) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImage_getPlaneData_ != nullptr);

	return AImage_getPlaneData_(image, planeIdx, data, dataLength);
}

inline media_status_t NativeMediaLibrary::AImageReader_new(int32_t width, int32_t height, int32_t format, int32_t maxImages, AImageReader** reader) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImageReader_new_ != nullptr);

	return AImageReader_new_(width, height, format, maxImages, reader);
}

inline void NativeMediaLibrary::AImageReader_delete(AImageReader* reader) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImageReader_delete_ != nullptr);

	AImageReader_delete_(reader);
}

inline media_status_t NativeMediaLibrary::AImageReader_getWindow(AImageReader* reader, ANativeWindow** window) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImageReader_getWindow_ != nullptr);

	return AImageReader_getWindow_(reader, window);
}

inline media_status_t NativeMediaLibrary::AImageReader_getWidth(const AImageReader* reader, int32_t* width) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImageReader_getWidth_ != nullptr);

	return AImageReader_getWidth_(reader, width);
}

inline media_status_t NativeMediaLibrary::AImageReader_getHeight(const AImageReader* reader, int32_t* height) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImageReader_getHeight_ != nullptr);

	return AImageReader_getHeight_(reader, height);
}

inline media_status_t NativeMediaLibrary::AImageReader_getFormat(const AImageReader* reader, int32_t* format) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImageReader_getFormat_ != nullptr);

	return AImageReader_getFormat_(reader, format);
}

inline media_status_t NativeMediaLibrary::AImageReader_getMaxImages(const AImageReader* reader, int32_t* maxImages) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImageReader_getMaxImages_ != nullptr);

	return AImageReader_getMaxImages_(reader, maxImages);
}

inline media_status_t NativeMediaLibrary::AImageReader_acquireNextImage(AImageReader* reader, AImage** image) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImageReader_acquireNextImage_ != nullptr);

	return AImageReader_acquireNextImage_(reader, image);
}

inline media_status_t NativeMediaLibrary::AImageReader_acquireLatestImage(AImageReader* reader, AImage** image) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImageReader_acquireLatestImage_ != nullptr);

	return AImageReader_acquireLatestImage_(reader, image);
}

inline media_status_t NativeMediaLibrary::AImageReader_setImageListener(AImageReader* reader, AImageReader_ImageListener* listener) const
{
	ocean_assert(isInitialized());
	ocean_assert(AImageReader_setImageListener_ != nullptr);

	return AImageReader_setImageListener_(reader, listener);
}

#endif // __ANDROID_API__ >= 24

inline AMediaCodec* NativeMediaLibrary::AMediaCodec_createDecoderByType(const char* mime_type) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_createDecoderByType_ != nullptr);

	return AMediaCodec_createDecoderByType_(mime_type);
}

inline AMediaCodec* NativeMediaLibrary::AMediaCodec_createEncoderByType(const char* mime_type) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_createEncoderByType_ != nullptr);

	return AMediaCodec_createEncoderByType_(mime_type);
}

inline media_status_t NativeMediaLibrary::AMediaCodec_configure(AMediaCodec* codec, const AMediaFormat* format, ANativeWindow* surface, AMediaCrypto* crypto, uint32_t flags) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_configure_ != nullptr);

	return AMediaCodec_configure_(codec, format, surface, crypto, flags);
}

inline media_status_t NativeMediaLibrary::AMediaCodec_delete(AMediaCodec* codec) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_delete_ != nullptr);

	return AMediaCodec_delete_(codec);
}

inline ssize_t NativeMediaLibrary::AMediaCodec_dequeueInputBuffer(AMediaCodec* codec, int64_t timeoutUs) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_dequeueInputBuffer_ != nullptr);

	return AMediaCodec_dequeueInputBuffer_(codec, timeoutUs);
}

inline ssize_t NativeMediaLibrary::AMediaCodec_dequeueOutputBuffer(AMediaCodec* codec, AMediaCodecBufferInfo* info, int64_t timeoutUs) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_dequeueOutputBuffer_ != nullptr);

	return AMediaCodec_dequeueOutputBuffer_(codec, info, timeoutUs);
}

inline media_status_t NativeMediaLibrary::AMediaCodec_flush(AMediaCodec* codec) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_flush_ != nullptr);

	return AMediaCodec_flush_(codec);
}

inline AMediaFormat* NativeMediaLibrary::AMediaCodec_getBufferFormat(AMediaCodec* codec, size_t index) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_getBufferFormat_ != nullptr);

	return AMediaCodec_getBufferFormat_(codec, index);
}

inline uint8_t* NativeMediaLibrary::AMediaCodec_getInputBuffer(AMediaCodec* codec, size_t idx, size_t* out_size) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_getInputBuffer_ != nullptr);

	return AMediaCodec_getInputBuffer_(codec, idx, out_size);
}

inline AMediaFormat*  NativeMediaLibrary::AMediaCodec_getInputFormat(AMediaCodec* codec) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_getInputFormat_ != nullptr);

	return AMediaCodec_getInputFormat_(codec);
}

inline uint8_t* NativeMediaLibrary::AMediaCodec_getOutputBuffer(AMediaCodec* codec, size_t idx, size_t* out_size) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_getOutputBuffer_ != nullptr);

	return AMediaCodec_getOutputBuffer_(codec, idx, out_size);
}

inline AMediaFormat* NativeMediaLibrary::AMediaCodec_getOutputFormat(AMediaCodec* codec) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_getOutputFormat_ != nullptr);

	return AMediaCodec_getOutputFormat_(codec);
}

inline media_status_t NativeMediaLibrary::AMediaCodec_queueInputBuffer(AMediaCodec* codec, size_t idx, unsigned int offset, size_t size, uint64_t time, uint32_t flags) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_queueInputBuffer_ != nullptr);

	return AMediaCodec_queueInputBuffer_(codec, idx, offset, size, time, flags);
}

inline media_status_t NativeMediaLibrary::AMediaCodec_releaseOutputBuffer(AMediaCodec* codec, size_t idx, bool render) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_releaseOutputBuffer_ != nullptr);

	return AMediaCodec_releaseOutputBuffer_(codec, idx, render);
}

inline media_status_t NativeMediaLibrary::AMediaCodec_start(AMediaCodec* codec) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_start_ != nullptr);

	return AMediaCodec_start_(codec);
}

inline media_status_t NativeMediaLibrary::AMediaCodec_stop(AMediaCodec* codec) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaCodec_stop_ != nullptr);

	return AMediaCodec_stop_(codec);
}

inline media_status_t NativeMediaLibrary::AMediaFormat_delete(AMediaFormat* format) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaFormat_delete_ != nullptr);

	return AMediaFormat_delete_(format);
}

inline AMediaFormat* NativeMediaLibrary::AMediaFormat_new() const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaFormat_new_ != nullptr);

	return AMediaFormat_new_();
}

bool NativeMediaLibrary::AMediaFormat_getInt32(AMediaFormat* mediaFormat, const char* name, int32_t* out) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaFormat_getInt32_ != nullptr);

	return AMediaFormat_getInt32_(mediaFormat, name, out);
}

bool NativeMediaLibrary::AMediaFormat_getInt64(AMediaFormat* mediaFormat, const char* name, int64_t* out) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaFormat_getInt64_ != nullptr);

	return AMediaFormat_getInt64_(mediaFormat, name, out);
}

bool NativeMediaLibrary::AMediaFormat_getString(AMediaFormat* mediaFormat, const char* name, const char** out) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaFormat_getString_ != nullptr);

	return AMediaFormat_getString_(mediaFormat, name, out);
}

bool NativeMediaLibrary::AMediaFormat_getRect(AMediaFormat* mediaFormat, const char* name, int32_t* left, int32_t* top, int32_t* right, int32_t* bottom) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaFormat_getRect_ != nullptr);

	return AMediaFormat_getRect_(mediaFormat, name, left, top, right, bottom);
}

inline void NativeMediaLibrary::AMediaFormat_setBuffer(AMediaFormat* format, const char* name, const void* data, size_t size) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaFormat_setBuffer_ != nullptr);

	return AMediaFormat_setBuffer_(format, name, data, size);
}

inline void NativeMediaLibrary::AMediaFormat_setFloat(AMediaFormat* format, const char* name, float value) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaFormat_setFloat_ != nullptr);

	return AMediaFormat_setFloat_(format, name, value);
}

inline void NativeMediaLibrary::AMediaFormat_setInt32(AMediaFormat* format, const char* name, int32_t value) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaFormat_setInt32_ != nullptr);

	return AMediaFormat_setInt32_(format, name, value);
}

inline void NativeMediaLibrary::AMediaFormat_setInt64(AMediaFormat* format, const char* name, int64_t value) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaFormat_setInt64_ != nullptr);

	return AMediaFormat_setInt64_(format, name, value);
}

inline void NativeMediaLibrary::AMediaFormat_setString(AMediaFormat* format, const char* name, const char* value) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaFormat_setString_ != nullptr);

	return AMediaFormat_setString_(format, name, value);
}

inline const char* NativeMediaLibrary::AMediaFormat_toString(AMediaFormat* format) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaFormat_toString_ != nullptr);

	return AMediaFormat_toString_(format);
}

inline ssize_t NativeMediaLibrary::AMediaMuxer_addTrack(AMediaMuxer* muxer, const AMediaFormat* format) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaMuxer_addTrack_ != nullptr);

	return AMediaMuxer_addTrack_(muxer, format);
}

inline media_status_t NativeMediaLibrary::AMediaMuxer_delete(AMediaMuxer* muxer) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaMuxer_delete_ != nullptr);

	return AMediaMuxer_delete_(muxer);
}

inline AMediaMuxer* NativeMediaLibrary::AMediaMuxer_new(int fd, OutputFormat format) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaMuxer_new_ != nullptr);

	return AMediaMuxer_new_(fd, format);
}

inline media_status_t NativeMediaLibrary::AMediaMuxer_setLocation(AMediaMuxer* muxer, float latitude, float longitude) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaMuxer_setLocation_ != nullptr);

	return AMediaMuxer_setLocation_(muxer, latitude, longitude);
}

inline media_status_t NativeMediaLibrary::AMediaMuxer_setOrientationHint(AMediaMuxer* muxer, int degrees) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaMuxer_setOrientationHint_ != nullptr);

	return AMediaMuxer_setOrientationHint_(muxer, degrees);
}

inline media_status_t NativeMediaLibrary::AMediaMuxer_start(AMediaMuxer* muxer) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaMuxer_start_ != nullptr);

	return AMediaMuxer_start_(muxer);
}

inline media_status_t NativeMediaLibrary::AMediaMuxer_stop(AMediaMuxer* muxer) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaMuxer_stop_ != nullptr);

	return AMediaMuxer_stop_(muxer);
}

inline media_status_t NativeMediaLibrary::AMediaMuxer_writeSampleData(AMediaMuxer* muxer, size_t trackIdx, const uint8_t* data, const AMediaCodecBufferInfo* info) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaMuxer_writeSampleData_ != nullptr);

	return AMediaMuxer_writeSampleData_(muxer, trackIdx, data, info);
}

inline bool NativeMediaLibrary::AMediaExtractor_advance(AMediaExtractor* mediaExtractor) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaExtractor_advance_ != nullptr);

	return AMediaExtractor_advance_(mediaExtractor);
}

inline media_status_t NativeMediaLibrary::AMediaExtractor_delete(AMediaExtractor* mediaExtractor) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaExtractor_delete_ != nullptr);

	return AMediaExtractor_delete_(mediaExtractor);
}

size_t NativeMediaLibrary::AMediaExtractor_getTrackCount(AMediaExtractor* mediaExtractor) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaExtractor_getTrackCount_ != nullptr);

	return AMediaExtractor_getTrackCount_(mediaExtractor);
}

AMediaFormat* NativeMediaLibrary::AMediaExtractor_getTrackFormat(AMediaExtractor* mediaExtractor, size_t idx) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaExtractor_getTrackFormat_ != nullptr);

	return AMediaExtractor_getTrackFormat_(mediaExtractor, idx);
}

inline AMediaExtractor* NativeMediaLibrary::AMediaExtractor_new() const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaExtractor_new_ != nullptr);

	return AMediaExtractor_new_();
}

ssize_t NativeMediaLibrary::AMediaExtractor_readSampleData(AMediaExtractor* mediaExtractor, uint8_t* buffer, size_t capacity) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaExtractor_readSampleData_ != nullptr);

	return AMediaExtractor_readSampleData_(mediaExtractor, buffer, capacity);
}

int64_t NativeMediaLibrary::AMediaExtractor_getSampleTime(AMediaExtractor* mediaExtractor) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaExtractor_getSampleTime_ != nullptr);

	return AMediaExtractor_getSampleTime_(mediaExtractor);
}

inline media_status_t NativeMediaLibrary::AMediaExtractor_seekTo(AMediaExtractor* mediaExtractor, int64_t seekPosUs, SeekMode mode) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaExtractor_seekTo_ != nullptr);

	return AMediaExtractor_seekTo_(mediaExtractor, seekPosUs, mode);
}

inline media_status_t NativeMediaLibrary::AMediaExtractor_selectTrack(AMediaExtractor* mediaExtractor, size_t idx) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaExtractor_selectTrack_ != nullptr);

	return AMediaExtractor_selectTrack_(mediaExtractor, idx);
}

inline media_status_t NativeMediaLibrary::AMediaExtractor_setDataSource(AMediaExtractor* mediaExtractor, const char* location) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaExtractor_setDataSource_ != nullptr);

	return AMediaExtractor_setDataSource_(mediaExtractor, location);
}

inline media_status_t NativeMediaLibrary::AMediaExtractor_setDataSourceFd(AMediaExtractor* mediaExtractor, int fd, off64_t offset, off64_t length) const
{
	ocean_assert(isInitialized());
	ocean_assert(AMediaExtractor_setDataSourceFd_ != nullptr);

	return AMediaExtractor_setDataSourceFd_(mediaExtractor, fd, offset, length);
}

}

}

}

#endif // defined(__ANDROID_API__) && __ANDROID_API__ >= 21

#endif // META_OCEAN_MEDIA_ANDROID_NATIVE_MEDIA_LIBRARY_H
