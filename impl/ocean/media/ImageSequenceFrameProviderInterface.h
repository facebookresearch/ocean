/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGE_SEQUENCE_FRAME_PROVIDER_INTERFACE_H
#define META_OCEAN_MEDIA_IMAGE_SEQUENCE_FRAME_PROVIDER_INTERFACE_H

#include "ocean/media/Media.h"
#include "ocean/media/ImageSequence.h"

#include "ocean/cv/FrameProviderInterface.h"

namespace Ocean
{

namespace Media
{

/**
 * This class implements a frame provider interface specialization using an image sequence medium object.
 * @see MovieFrameProviderInterface.
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT ImageSequenceFrameProviderInterface : public CV::FrameProviderInterface
{
	public:

		/**
		 * Creates a new image sequence provider interface.
		 * @param imageSequence Image seqeunce that is used as frame source
		 */
		ImageSequenceFrameProviderInterface(const ImageSequenceRef& imageSequence);

		/**
		 * Destructs this image sequence provider and waits until all asynchronous requests have been processed.
		 */
		~ImageSequenceFrameProviderInterface() override;

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
		void asynchronFrameRequest(const unsigned int index, const bool priority = false) override;

		/**
		 * Invokes a synchronous frame request.
		 * @see CV::FrameProviderInterface::synchronFrameRequest().
		 */
		FrameRef synchronFrameRequest(const unsigned int index, const double timeout = 10.0, bool* abort = nullptr) override;

		/**
		 * Invokes an asynchronous frame number request.
		 * @see CV::FrameProviderInterface::asynchronFrameNumberRequest().
		 */
		void asynchronFrameNumberRequest() override;

		/**
		 * Invokes a synchronous frame number request.
		 * @see CV::FrameProviderInterface::synchronFrameNumberRequest().
		 */
		unsigned int synchronFrameNumberRequest(const double timeout = 10.0, bool* abort = nullptr) override;

		/**
		 * Invokes an asynchronous frame type request.
		 * @see CV::FrameProviderInterface::asynchronFrameTypeRequest().
		 */
		void asynchronFrameTypeRequest() override;

		/**
		 * Invokes a synchronous frame type request.
		 * @see CV::FrameProviderInterface::synchronFrameTypeRequest().
		 */
		FrameType synchronFrameTypeRequest(const double timeout = 10.0, bool* abort = nullptr) override;

	protected:

		/**
		 * Internal function to handle asynchronous frame requests.
		 * @param index The index of the frame that has been requested
		 */
		void handleAsynchronFrameRequest(const unsigned int index);

		/**
		 * Internal function to handle asynchronous frame number requests.
		 */
		void handleAsynchronFrameNumberRequest();

		/**
		 * Internal function to handle asynchronous frame type requests.
		 */
		void handleAsynchronFrameTypeRequest();

	protected:

		/// Image sequence used as frame source.
		ImageSequenceRef imageSequence_;

		/// The number of pending asynchronous requests.
		unsigned int pendingAsynchronousRequests_ = 0u;

		/// The preferred pixel format of the resulting images, if any.
		FrameType::PixelFormat preferredPixelFormat_ = FrameType::FORMAT_UNDEFINED;

		/// The preferred pixel origin of the resulting images, if any.
		FrameType::PixelOrigin preferredPixelOrigin_ = FrameType::ORIGIN_INVALID;

		/// Interface lock.
		Lock lock_;
};

}

}

#endif // META_OCEAN_MEDIA_IMAGE_SEQUENCE_FRAME_PROVIDER_INTERFACE_H
