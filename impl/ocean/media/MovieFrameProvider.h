/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MOVIE_FRAME_PROVIDER_H
#define META_OCEAN_MEDIA_MOVIE_FRAME_PROVIDER_H

#include "ocean/media/Media.h"
#include "ocean/media/Movie.h"

#include "ocean/base/Callback.h"
#include "ocean/base/Frame.h"
#include "ocean/base/RingMap.h"
#include "ocean/base/Thread.h"

#include <list>

namespace Ocean
{

namespace Media
{

// Forward declaration.
class MovieFrameProvider;

/**
 * Definition of an object reference holding a frame provider.
 * @see MovieFrameProvider.
 * @ingroup media
 */
typedef ObjectRef<MovieFrameProvider> MovieFrameProviderRef;

/**
 * This class implements a frame provider for movie mediums.
 * In addition to allowing access to the movie's frame, this provider also allows access to smaller preview frames.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT MovieFrameProvider : protected Thread
{
	public:

		/**
		 * Definition of individual event types.
		 */
		enum EventType
		{
			/// Invalid event type.
			ET_INVALID,
			/// New media object assigned, the parameter is zero.
			ET_NEW_MEDIA,
			/// The determination of all preview frames has been completed, the parameter stores the number of frames.
			ET_PREVIEW_COMPLETED,
			/// The size of the frames has been changed, the parameters stores the width in the upper 32 bit and the height in the lower 32 bit.
			ET_FRAME_SIZE_CHANGED,
			/// A requested frame cannot be delivered, the parameter stores the index of the frame.
			ET_REQUESTED_FRAME_FAILED
		};

		/**
		 * Definition of frame callback function.
		 * The first parameter defines the index of the frame.<br>
		 * The second parameter states whether the frames has been explicitly requested.
		 */
		typedef Callback<void, const unsigned int, const bool> FrameCallback;

		/**
		 * Definition of a preview frame callback function.
		 * The first parameter defines the index of the preview frame.<br>
		 */
		typedef Callback<void, const unsigned int> PreviewFrameCallback;

		/**
		 * Definition of an event callback function.
		 * The first parameter specifies the event type.<br>
		 * The second parameter holds an optional event parameter.<br>
		 */
		typedef Callback<void, const EventType, const unsigned long long> EventCallback;

		/**
		 * Definition of a pair of sizes.
		 */
		typedef IndexPair32 Dimension;

	private:

		/**
		 * Definition of a non-thread-safe ring map mapping frame indices to frame references.
		 */
		typedef RingMapT<unsigned int, FrameRef, false> FrameDatabase;

		/**
		 * Definition of a preview frame composed of a finished-state and the frame data.
		 */
		typedef std::pair<bool, FrameRef> PreviewFrame;

		/**
		 * Definition of a vector holding preview frames.
		 */
		typedef std::vector<PreviewFrame> PreviewFrames;

		/**
		 * Definition of a queue holding frames.
		 */
		typedef std::queue<FrameRef> FrameQueue;

		/**
		 * Definition of a list holding frame indices.
		 */
		typedef std::list<unsigned int> RequestList;

		/**
		 * Definition of a callback container storing frame callbacks.
		 */
		typedef ConcurrentCallbacks<FrameCallback> FrameCallbacks;

		/**
		 * Definition of a callback container storing preview frame callbacks.
		 */
		typedef ConcurrentCallbacks<PreviewFrameCallback> PreviewFrameCallbacks;

		/**
		 * Definition of a callback container storing event callbacks.
		 */
		typedef ConcurrentCallbacks<EventCallback> EventCallbacks;

	public:

		/**
		 * Creates a new movie frame provider.
		 * @param enabled True, to enable the provider directly
		 * @param maximalFrameStorage The maximal number of frames that will be stored inside the frame provider concurrently
		 * @param maximalQueueStorage The maximal number of frames that will be queued inside the frame provider concurrently
		 */
		explicit MovieFrameProvider(const bool enabled = true, const unsigned int maximalFrameStorage = 150u, const unsigned int maximalQueueStorage = 50u);

		/**
		 * Destructs the movie frame provider.
		 */
		~MovieFrameProvider() override;

		/**
		 * Returns whether the provider is enabled.
		 * @return True, if so
		 */
		inline bool isEnabled() const;

		/**
		 * Enables or disables the provider.
		 * @param state True, to enable the provider
		 * @return True, if succeeded
		 */
		bool setEnabled(const bool state);

		/**
		 * Returns the url of the media.
		 * @return Media url of the media
		 */
		std::string url() const;

		/**
		 * Sets the movie providing the frames.
		 * The movie must be exclusive so that the frame provider can use the movie's resources alone.<br>
		 * @param movie The movie to set
		 * @return True, if succeeded
		 * @see Medium::isExlusive(), Manager::newMedium().
		 */
		bool setMovie(const MovieRef& movie);

		/**
		 * Sets the preferred frame type of the frames of this provider.
		 * However, there is no guarantee that this interface will be able to provided the requested frame type.<br>
		 * @param pixelFormat The preferred pixel format
		 * @param pixelOrigin The preferred pixel origin
		 * @return True, if succeeded
		 */
		bool setPreferredFrameType(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin);

		/**
		 * Sets the dimension of the preview frames.
		 * Only one parameter has to be provided, either the width or the size.<br>
		 * The other parameter has to be zero.
		 * @param width The width of each preview frame in pixel, with range [0u, infinity)
		 * @param height the height of each preview frame in pixel, with range [0u, infinity)
		 * @return True, if succeeded
		 */
		bool setPreferredPreviewDimension(const unsigned int width, const unsigned int height);

		/**
		 * Returns the current duration of the media in seconds.
		 * @return Duration time in seconds
		 */
		inline double duration() const;

		/**
		 * Returns the frame dimension of the frames of this provider.
		 * @return Frame dimension, width and height in pixel
		 */
		inline Dimension frameDimension() const;

		/**
		 * Returns the frame type of the frames of this provider.
		 * @return Frame type
		 */
		inline const FrameType& frameType() const;

		/**
		 * Returns the preferred frame dimension of the preview frames.
		 * @return Preferred frame dimension, width and high in pixel
		 */
		Dimension preferredPreviewDimension() const;

		/**
		 * Returns the capacity of frames that can be stored concurrently.
		 * The capacity does not reflect the preview frames.<br>
		 * @return Number of frames
		 * @see setFrameCapacity().
		 */
		inline size_t frameCapacity() const;

		/**
		 * Sets or changes the capacity of the frames to be stored concurrently.
		 * The capacity does not reflect the preview frames.
		 * @param capacity The capacity to be set, with range [1, infinity)
		 * @return True, if succeeded
		 * @see frameCapacity().
		 */
		bool setFrameCapacity(const size_t capacity);

		/**
		 * Adds a new frame callback event function.
		 * Each callback has to be removed before this frame provider is disposed.<br>
		 * @param callback The callback function to be added
		 * @see removeFrameCallback().
		 */
		inline void addFrameCallback(const FrameCallback& callback);

		/**
		 * Adds a new preview frame callback event function.
		 * Each callback has to be removed before this frame provider is disposed.<br>
		 * @param callback The callback function to be added
		 * @see removePreviewFrameCallback().
		 */
		inline void addPreviewFrameCallback(const PreviewFrameCallback& callback);

		/**
		 * Adds a new event callback event function.
		 * Each callback has to be removed before this frame provider is disposed.<br>
		 * @param callback The callback function to be added
		 * @see removeEventCallback().
		 */
		inline void addEventCallback(const EventCallback& callback);

		/**
		 * Removes a frame callback event function.
		 * @param callback The callback function to be removed
		 * @see addFrameCallback().
		 */
		inline void removeFrameCallback(const FrameCallback& callback);

		/**
		 * Removes a preview frame callback event function.
		 * @param callback The callback function to be removed
		 * @see addPreviewFrameCallback().
		 */
		inline void removePreviewFrameCallback(const PreviewFrameCallback& callback);

		/**
		 * Removes an event callback event function.
		 * @param callback The callback function to be removed
		 * @see addEventCallback().
		 */
		inline void removeEventCallback(const EventCallback& callback);

		/**
		 * Returns the number of frames the media actually provides.
		 * The number of actual frames is unknown until all preview frames have been determined.<br>
		 * @return Number of actual frames
		 * @see estimatedFrameNumber(), currentFrameNumber(), frameNumber().
		 */
		unsigned int actualFrameNumber();

		/**
		 * Returns the estimated number of frames that can be provided.
		 * This number is an estimation due to the media information and may be an approximation only.<br>
		 * @return Number of estimated frames
		 * @see actualFrameNumber(), currentFrameNumber(), frameNumber().
		 */
		unsigned int estimatedFrameNumber();

		/**
		 * Returns the current number of frames that can be provided.
		 * This number is identical to the number of existing preview frames.<br>
		 * @return Number of current frames
		 * @see actualFrameNumber(), estimatedFrameNumber(), frameNumber().
		 */
		unsigned int currentFrameNumber();

		/**
		 * Returns the best guess of the number of frames of this provider.
		 * If the actual number of frames is known, this number is returned.<br>
		 * Otherwise the highest value of estimatedFrameNumber() and currentFrameNumber() is returned.<br>
		 * @return Number of frames
		 * @see actualFrameNumber(), estimatedFrameNumber(), currentFrameNumber().
		 */
		unsigned int frameNumber();

		/**
		 * Requests a frame synchronously.
		 * The function blocks until the requested frame can be provided.<br>
		 * If the frame cannot be provided, the function returns directly after the timeout has been reached or the abort statement has been set.
		 * @param index The index of the frame that is requested
		 * @param timeout Time period in that the function will wait if the requested frame cannot be provided directly, with range [0, infinity)
		 * @param abort Optional abort statement allowing to abort the frame request at any time; set the value True to abort the request
		 * @return Requested frame, if it can be provided within the specified timeout period
		 * @see asynchronFrameRequest().
		 */
		FrameRef synchronFrameRequest(const unsigned int index, const double timeout = 10.0, bool* abort = nullptr);

		/**
		 * Requests a frame asynchronously.
		 * If the frame is available the frame can be accessed by e.g. the frame() or frameRequest() function.<br>
		 * If a frame event callback has been defined, an event will occur if the requested frame is available.<br>
		 * @param index The index of the frame that is requested
		 * @param priority True, to receive the frame as fast as possible
		 * @see frame(), frameRequest(), synchronFrameRequest().
		 */
		void asynchronFrameRequest(const unsigned int index, const bool priority = true);

		/**
		 * Returns a frame directly if the frame is currently available.
		 * If the frame does not exist, nothing happens.<br>
		 * @param index The index of the frame that is requested
		 * @return Resulting frame, if it currently exists
		 * @see frameRequest()
		 */
		FrameRef frame(const unsigned index);

		/**
		 * Returns a frame directly if the frame is currently available, otherwise the frame will be requested asynchronously (by internally calling asynchronously).
		 * If the frame is requested asynchronously the frame will be provided after this function returns.<br>
		 * If the frame is provided asynchronously, the an event will occur if the requested frame is available.<br>
		 * @param index The index of the frame that is requested
		 * @return Resulting frame, if it currently exists
		 * @see frame(), asynchronFrameRequest().
		 */
		FrameRef frameRequest(const unsigned int index);

		/**
		 * Returns a preview frame.
		 * The provided index should lie inside the number of expected media frame.<br>
		 * @param index The index of the preview frame, with range [0, infinity)
		 * @return Resulting preview frame, if it does exist already
		 */
		FrameRef previewFrame(const unsigned int index);

		/**
		 * Returns several preview frames.
		 * The provided index should lie inside the number of expected media frame.<br>
		 * A zooming factor may be used to return e.g. each 2nd, 3rd or each 10th preview frame (beginning at the specified index).<br>
		 * @param index The index of the first preview frame to be returned
		 * @param size Number of preview frames to be returned
		 * @param zoom The zoom factor, with range [1, infinity)
		 * @return Resulting preview frames
		 */
		FrameRefs previewFrames(const unsigned int index, const unsigned int size, const unsigned int zoom = 1u);

		/**
		 * Determines the current preview dimension for the current frame size and specified preferred preview dimension.
		 * @return Resulting preview dimension, zero if invalid
		 */
		Dimension determinePreviewDimensions() const;

		/**
		 * Returns the preview progress of this frame provider.
		 * @return Preview progress in percent
		 */
		unsigned int previewProgress() const;

	protected:

		/**
		 * Releases the frame provider.
		 */
		void release();

		/**
		 * Thread run function.
		 */
		void threadRun() override;

		/**
		 * Internal event function for new preview frames.
		 * @param frame The new preview frame, will be valid
		 * @param camera The camera profile associated with the frame, invalid if unknown
		 */
		void onPreviewFrame(const Frame& frame, const SharedAnyCamera& camera);

		/**
		 * Internal event function for new frames.
		 * @param frame The new frame, will be valid
		 * @param camera The camera profile associated with the frame, invalid if unknown
		 */
		void onFrame(const Frame& frame, const SharedAnyCamera& camera);

		/**
		 * Creates a new preview frame by a given original frame.
		 * @param frame The frame for that a preview frame has to be created
		 * @param index The index of the given frame
		 * @param previewWidth Width of the resulting preview frame in pixel, with range [1, infinity)
		 * @param previewHeight Height of the resulting preview frame in pixel, with range [1, infinity)
		 * @return True, if succeeded
		 */
		bool handlePreviewFrame(const Frame& frame, unsigned int index, const unsigned int previewWidth, const unsigned int previewHeight);

		/**
		 * Determines the frame index for a given timestamp.
		 * If the frame preview creation has not been completed yet, the index is incremented with each new function call.
		 * @param timestamp Timestamp for that the index has to be determined
		 * @param lookup True, ensure that the index is returned only if a corresponding preview frame exists already
		 * @return Resulting frame index
		 */
		unsigned int timestamp2index(const double timestamp, const bool lookup = false);

		/**
		 * Converts the index of a frame into the corresponding timestamp.
		 * @param index The index for that the timestamp has to be found
		 * @return Resulting timestamp, -1 if the index is invalid
		 */
		double index2timestamp(const unsigned int index);

	protected:

		/// Provider enabled state.
		bool enabled_ = false;

		/// State that is set directly if the provider has been invoked to be disposed.
		bool released_ = false;

		/// Movie providing the preview frames.
		MovieRef moviePreview_;

		/// Movie providing the frames.
		MovieRef movie_;

		/// The subscription objects for previewing frame callback events.
		FrameMedium::FrameCallbackScopedSubscription scopedSubscriptionPreviewFrames_;

		/// The subscription objects for frame callback events.
		FrameMedium::FrameCallbackScopedSubscription scopedSubscriptionFrames_;

		/// Duration of the media in seconds.
		double mediaDuration_ = 0.0;

		/// Frequency of the media in Hz.
		double mediaFrameFrequency_ = 0.0;

		/// Average time for one frame in the media.
		double mediaFrameTime_ = 0.0;

		/// Actual number of frames that the provider organizes.
		unsigned int actualFrameNumber_ = 0u;

		/// Estimated number of frames that the provider will organize.
		unsigned int estimatedFrameNumber_ = 0u;

		/// Number of frames that the provider currently organizes.
		unsigned int currentFrameNumber_ = 0u;

		/// Maximal size of the internal intermediate frame queues.
		unsigned int maximalQueueSize_ = 0u;

		/// The preferred pixel format of the provider's frames.
		FrameType::PixelFormat preferredPixelFormat_ = FrameType::FORMAT_UNDEFINED;

		/// The preferred pixel origin of the provider's frames.
		FrameType::PixelOrigin preferredPixelOrigin_ = FrameType::ORIGIN_INVALID;

		/// Intermediate queue for preview frames.
		FrameQueue previewFrameQueue_;

		/// Intermediate queue for frames.
		FrameQueue frameQueue_;

		/// Database holding the media frames.
		FrameDatabase frames_;

		/// Vector holding the preview frames.
		PreviewFrames previewFrames_;

		/// List of frames that are explicitly requested.
		RequestList requestList_;

		/// Frame type of the media frames.
		FrameType frameType_;

		/// Preferred width of the preview frames in pixel.
		unsigned int preferredPreviewWidth_ = 0u;

		/// Preferred height of the preview frames in pixel.
		unsigned int preferredPreviewHeight_ = 100u;

		/// Frame event callback functions.
		FrameCallbacks frameCallbacks_;

		/// Preview frame event callback functions.
		PreviewFrameCallbacks previewFrameCallbacks_;

		/// Event callback functions.
		EventCallbacks eventCallbacks_;

		/// Index of the currently requested frame.
		unsigned int frameRequestIndex_ = (unsigned int)(-1);

		/// Timestamp as the last frame has been arrived.
		Timestamp lastFrameEventTimestamp_;

		/// Timestamp that stops the standard frame medium.
		Timestamp movieStopTimestamp_;

		/// Time period that may exceed until a frame is expected to be unavailable, in seconds, with range (0, infinity)
		double frameRequestTimeout_ = 1.0;

		/// Provider lock.
		mutable Lock lock_;

		/// Frame lock.
		Lock frameLock_;
};

inline bool MovieFrameProvider::isEnabled() const
{
	return enabled_;
}

inline double MovieFrameProvider::duration() const
{
	return mediaDuration_;
}

MovieFrameProvider::Dimension MovieFrameProvider::frameDimension() const
{
	const ScopedLock scopedLock(lock_);
	return Dimension(frameType_.width(), frameType_.height());
}

inline const FrameType& MovieFrameProvider::frameType() const
{
	const ScopedLock scopedLock(lock_);
	return frameType_;
}

inline size_t MovieFrameProvider::frameCapacity() const
{
	return frames_.capacity();
}

inline void MovieFrameProvider::addFrameCallback(const FrameCallback& callback)
{
	frameCallbacks_.addCallback(callback);
}

inline void MovieFrameProvider::addPreviewFrameCallback(const PreviewFrameCallback& callback)
{
	previewFrameCallbacks_.addCallback(callback);
}

inline void MovieFrameProvider::addEventCallback(const EventCallback& callback)
{
	eventCallbacks_.addCallback(callback);
}

inline void MovieFrameProvider::removeFrameCallback(const FrameCallback& callback)
{
	frameCallbacks_.removeCallback(callback);
}

inline void MovieFrameProvider::removePreviewFrameCallback(const PreviewFrameCallback& callback)
{
	previewFrameCallbacks_.removeCallback(callback);
}

inline void MovieFrameProvider::removeEventCallback(const EventCallback& callback)
{
	eventCallbacks_.removeCallback(callback);
}

}

}

#endif // META_OCEAN_MEDIA_MOVIE_FRAME_PROVIDER_H
