/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_MEDIA_IMAGE_SEQUENCE_H
#define META_OCEAN_MEDIA_IMAGE_SEQUENCE_H

#include "ocean/media/Media.h"
#include "ocean/media/FiniteMedium.h"
#include "ocean/media/FrameMedium.h"

#include "ocean/base/Thread.h"

namespace Ocean
{

namespace Media
{

// Forward declaration.
class ImageSequence;

/**
 * Definition of a smart medium reference holding an image sequence object.
 * @see SmartMediumRef, ImageSequence.
 * @ingroup media
 */
typedef SmartMediumRef<ImageSequence> ImageSequenceRef;

/**
 * This class is the base class for all image sequences.
 * An image sequence provides a sequence of several images.<br>
 * This media object will provide all images that are part of the entire image sequence.<br>
 * Two individual modes exist to receive the images: automatic and explicit.
 *
 * The automatic mode (default) delivers frames automatically.<br>
 * By default the image sequence will deliver one frame per second (default fps = 1).<br>
 * Change the preferred frame rate as desired but be aware the actual frame rate can be lower.<br>
 * If a frame rate of 0 fps is specified, the medium will pause after each new frame and will need to be restarted afterwards.<br>
 *
 * The explicit mode does not deliver frames based on a specified frame rate (fps) but provides a new frame each time forceNextFrame() is called.<br>
 * Thus, the explicit mode allows to receive frames as fast as possible (based on the system performance) or can deliver frames very uneven - whatever is needed.
 * @see Image, PixelImage, mode(), setMode(), forceFrame().
 * @ingroup media
 */
class OCEAN_MEDIA_EXPORT ImageSequence :
	public virtual FiniteMedium,
	public virtual FrameMedium
{
	public:

		/**
		 * Defines different sequence modes.
		 */
		enum SequenceMode
		{
			/// Invalid sequence mode.
			SM_INVALID,
			/// Automatic sequence mode, frames will be delivered regarding the preferred fps.
			SM_AUTOMATIC,
			/// Explicit sequence mode, frames will be delivered by explicit request.
			SM_EXPLICIT
		};

	public:

		/**
		 * Returns the mode of this image sequence object.
		 * @return Sequence mode, SM_AUTOMATIC by default
		 * @see forceNextFrame().
		 */
		virtual SequenceMode mode() const;

		/**
		 * Returns the index of the current image.
		 * The index of the frame may not be related with the digits within the corresponding filename,<br>
		 * as the first available frame of this sequence has index 0.
		 * @return Current image index, with range [0, images())
		 * @see images().
		 */
		virtual unsigned int index() const;

		/**
		 * Returns the url of the current image.
		 * @return Current image url
		 */
		virtual std::string currentUrl() const = 0;

		/**
		 * Sets the sequence mode of this image sequence.
		 * The explicit loading is allowed for explicit sequence mode only.
		 * @see mode(), forceNextFrame().
		 * @return True, if succeeded
		 */
		virtual bool setMode(const SequenceMode mode);

		/**
		 * Returns the number of images part of the sequence.
		 * @return Number of images, with range [0, infinity)
		 * @see index().
		 */
		virtual unsigned int images() const = 0;

		/**
		 * Sets the preferred frame pixel format.
		 * @see FrameMedium::setPreferredFramePixelFormat().
		 */
		bool setPreferredFramePixelFormat(const FrameType::PixelFormat format) override;

		/**
		 * Sets the preferred frame frequency in Hz.
		 * @see FrameMedium::setPreferredFrameFrequency().
		 */
		bool setPreferredFrameFrequency(const FrameFrequency frequency) override;

		/**
		 * Forces the loading of the next image in the sequence.
		 * This function must be called whenever a new frame should be delivered.<br>
		 * Beware: Do not call this function unless the sequence mode is SM_EXPLICIT.
		 * @return True, if a new frame will be delivered; False, if the end of the image sequence has been reached or if this medium is invalid
		 */
		virtual bool forceNextFrame() = 0;

	protected:

		/**
		 * Creates a new image sequence by a given url.
		 * @param url Url of the image
		 */
		explicit ImageSequence(const std::string& url);

		/**
		 * Destructs an image sequence object.
		 */
		~ImageSequence() override;

	protected:

		/// Index of the first frame.
		unsigned int mediumFrameStartIndex;

		/// Index of the recent frame.
		unsigned int mediumFrameIndex;

		/// Number of sequence frames.
		mutable unsigned int mediumImages;

		/// State determining whether the sequence filenames have a constant length.
		unsigned int mediumFilenameIndexLength;

		/// Sequence mode of this image sequence object.
		SequenceMode mediumSequenceMode;

		/// State determining whether the explicit sequence mode has been started.
		bool mediumExplicitSequenceModeStarted;
};

}

}

#endif // META_OCEAN_MEDIA_IMAGE_SEQUENCE_H
