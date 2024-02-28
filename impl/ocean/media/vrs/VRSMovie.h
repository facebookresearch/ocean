// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_VRS_MOVIE_H
#define META_OCEAN_MEDIA_VRS_MOVIE_H

#include "ocean/media/vrs/VRS.h"
#include "ocean/media/vrs/VRSFiniteMedium.h"
#include "ocean/media/vrs/VRSFrameMedium.h"

#include "ocean/base/Thread.h"

#include "ocean/media/Movie.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

/**
 * This class implements a VRS movie object.
 * @ingroup mediavrs
 */
class OCEAN_MEDIA_VRS_EXPORT VRSMovie :
	virtual public VRSFiniteMedium,
	virtual public VRSFrameMedium,
	virtual public Movie,
	protected Thread
{
	friend class VRSLibrary;

	public:

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
		 * Clones this movie medium and returns a new independent instance of this medium.
		 * @see Medium::clone()
		 */
		MediumRef clone() const override;

		/**
		 * Returns the volume of the sound in db.
		 * @see SoundMedium::soundVolume()
		 */
		float soundVolume() const override;

		/**
		 * Returns whether the movie medium is in a mute state.
		 * @see SoundMedium::soundMute()
		 */
		bool soundMute() const override;

		/**
		 * Sets the volume of the sound in db.
		 * @see SoundMedium::setSoundVolume()
		 */
		bool setSoundVolume(const float volume) override;

		/**
		 * Sets or unsets the movie medium to a mute state.
		 * @see SoundMedium::setSoundMute()
		 */
		bool setSoundMute(const bool mute) override;

	protected:

		/**
		 * Creates a new movie by a given url.
		 * @param url Url of the movie
		 */
		explicit VRSMovie(const std::string& url);

		/**
		 * Destructs a VRSMovie object.
		 */
		~VRSMovie() override;

		/**
		 * This function has to be overloaded in derivated class.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;
};

}

}

}

#endif // META_OCEAN_MEDIA_VRS_MOVIE_H
