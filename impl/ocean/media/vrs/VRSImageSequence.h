// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_VRS_IMAGE_SEQUENCE_H
#define META_OCEAN_MEDIA_VRS_IMAGE_SEQUENCE_H

#include "ocean/media/vrs/VRS.h"
#include "ocean/media/vrs/VRSFiniteMedium.h"
#include "ocean/media/vrs/VRSFrameMedium.h"

#include "ocean/base/Thread.h"

#include "ocean/media/ImageSequence.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

/**
 * This class implements a VRS image sequence object.
 * @ingroup mediavrs
 */
class OCEAN_MEDIA_VRS_EXPORT VRSImageSequence :
	virtual public VRSFiniteMedium,
	virtual public VRSFrameMedium,
	virtual public ImageSequence,
	protected Thread
{
	friend class VRSLibrary;

	public:

		/**
		 * Returns the url of the current image.
		 * @see ImageSequence::currentUrl().
		 */
		std::string currentUrl() const override;

		/**
		 * Returns the number of images part of the sequence.
		 * @see ImageSequence::images().
		 */
		unsigned int images() const override;

		/**
		 * Starts the medium.
		 * The internalStart() function will be called inside.
		 * @see Medium::start()
		 */
		bool start() override;

		/**
		 * Pauses the medium.
		 * The internalPause() function will be called inside.
		 * @see Medium::pause()
		 */
		bool pause() override;

		/**
		 * Stops the medium.
		 * The internalStop() function will be called inside.
		 * @see Medium::stop()
		 */
		bool stop() override;

		/**
		 * Sets the sequence mode of this image sequence.
		 * @see ImageSequence::setMode().
		 */
		bool setMode(const SequenceMode mode) override;

		/**
		 * Clones this movie medium and returns a new independent instance of this medium.
		 * @see Medium::clone()
		 */
		MediumRef clone() const override;

		/**
		 * Forces the loading of the next image in the sequence.
		 * @see ImageSequence::forceNextFrame().
		 */
		bool forceNextFrame() override;

	protected:

		/**
		 * Creates a new image sequence by a given url.
		 * @param url Url of the image sequence
		 */
		explicit VRSImageSequence(const std::string& url);

		/**
		 * Destructs a VRSImageSequence object.
		 */
		~VRSImageSequence() override;

		/**
		 * This function has to be overloaded in derivated class.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	protected:

		/// The number of images available in the VRS file.
		unsigned int images_ = 0u;

		/// True, if the next frame is foreced.
		bool forceNextFrame_ = false;
};

}

}

}

#endif // META_OCEAN_MEDIA_VRS_IMAGE_SEQUENCE_H
