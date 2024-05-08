/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_MOVIE_FRAME_PROVIDER_INTERFACE_H
#define META_OCEAN_MEDIA_MOVIE_FRAME_PROVIDER_INTERFACE_H

#include "ocean/media/Media.h"
#include "ocean/media/MovieFrameProvider.h"

#include "ocean/cv/FrameProviderInterface.h"

namespace Ocean
{

namespace Media
{

/**
 * This class implements a frame provider interface specialization using a movie frame provider object.
 * Actually, this function is nothing else but a wrapper for a MovieFrameProvider object.<br>
 * We should investigate whether we really need an own MovideFrameProvider class anymore.<br>
 * However, the MovieFrameProvider is able to deliver preview images, which is not part of this interface.
 * @see MovieFrameProvider.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT MovieFrameProviderInterface : public CV::FrameProviderInterface
{
	private:

		/**
		 * Definition of pair combining a frame index with a frame reference.
		 */
		typedef std::pair<unsigned int, FrameRef> FramePair;

		/**
		 * Definition of a queue storing frame pairs.
		 */
		typedef std::queue<FramePair> FrameQueue;

	public:

		/**
		 * Creates a new media frame provider interface.
		 * @param movieFrameProvider Movie frame provider object that is connected with this interface and that provides the individual frames
		 */
		MovieFrameProviderInterface(const MovieFrameProviderRef& movieFrameProvider);

		/**
		 * Destructs this frame provider interface.
		 */
		~MovieFrameProviderInterface() override;

		/**
		 * Returns whether the internal information of this interface has been initialized already and whether request functions can be handled.
		 * @see CV::FrameProviderInterface::isInitialized().
		 */
		bool isInitialized() override;

		/**
		 * Sets a preferred frame type pixel format and pixel origin for this interface.
		 * @see CV::FrameProviderInterface::setPreferredFrameType().
		 */
		bool setPreferredFrameType(const FrameType::PixelFormat pixelFormat, const FrameType::PixelOrigin pixelOrigin) override;

		/**
		 * Invokes an asynchronous frame request.
		 * @see CV::FrameProviderInterface::asynchronFrameRequest().
		 */
		void asynchronFrameRequest(const unsigned int index, const bool priority) override;

		/**
		 * Invokes a synchronous frame request.
		 * @see CV::FrameProviderInterface::synchronFrameRequest().
		 */
		FrameRef synchronFrameRequest(const unsigned int index, const double timeout, bool* abort = nullptr) override;

		/**
		 * Invokes a suggestion to pre-load or to cache some frames that might be requested soon.
		 * @see CV::FrameProviderInterface::frameCacheRequest().
		 */
		void frameCacheRequest(const unsigned int index, const int range) override;

		/**
		 * Invokes an asynchronous frame number request.
		 * @see CV::FrameProviderInterface::asynchronFrameNumberRequest().
		 */
		void asynchronFrameNumberRequest() override;

		/**
		 * Invokes a synchronous frame number request.
		 * @see CV::FrameProviderInterface::synchronFrameNumberRequest().
		 */
		unsigned int synchronFrameNumberRequest(const double timeout, bool* abort = nullptr) override;

		/**
		 * Invokes an asynchronous frame type request.
		 * @see CV::FrameProviderInterface::asynchronFrameTypeRequest().
		 */
		void asynchronFrameTypeRequest() override;

		/**
		 * Invokes a synchronous frame type request.
		 * @see CV::FrameProviderInterface::synchronFrameTypeRequest().
		 */
		FrameType synchronFrameTypeRequest(const double timeout, bool* abort = nullptr) override;

		/**
		 * Releases all associated resources.
		 * @see CV::FrameProviderInterface::release().
		 */
		void release() override;

	private:

		/**
		 * Scheduler event function.
		 */
		void onScheduler();

		/**
		 * Internal callback function for frame requests arriving from the internal media frame provider.
		 * @param frameIndex Index of the frame that recently has been encoded by the media frame provider
		 * @param frameRequested True, if the frame has been requested, false if the frame just has been encoded without explicit request
		 */
		void onFrame(const unsigned int frameIndex, const bool frameRequested);

	private:

		/// The movie frame provider that provides the individual frames for this interface.
		MovieFrameProviderRef movieFrameProvider_;

		/// Frame queue for asynchronous frame requests.
		FrameQueue frameQueue_;

		/// True, if an asynchronous frame number request has been invoked.
		bool asynchronousFrameNumber_ = false;

		/// True, if an asynchronous frame type request has been invoked.
		bool asynchronousFrameType_ = false;

		/// Interface lock.
		Lock lock_;
};

}

}

#endif // META_OCEAN_MEDIA_MOVIE_FRAME_PROVIDER_INTERFACE_H
