// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_MEDIA_QUEST_MICROPHONE_H
#define META_OCEAN_MEDIA_QUEST_MICROPHONE_H

#include "ocean/media/quest/Quest.h"

#include "ocean/base/Thread.h"

#include "ocean/media/Microphone.h"

#include <OVR_Voip_LowLevel.h>

namespace Ocean
{

namespace Media
{

namespace Quest
{

/**
 * This class implements an microphone for Quest platforms.
 * @ingroup mediaquest
 */
class OCEAN_MEDIA_QUEST_EXPORT QuestMicrophone :
	virtual public Microphone,
	protected Thread
{
	friend class QuestLibrary;
	friend class QuestManager;

	public:

		/**
		 * Returns whether the medium is started currently.
		 * @see Medium::isStarted().
		 */
		bool isStarted() const override;

		/**
		 * Returns the start timestamp.
		 * @see Microphone::startTimestamp().
		 */
		Timestamp startTimestamp() const override;

		/**
		 * Returns the pause timestamp.
		 * @see Microphone::pauseTimestamp().
		 */
		Timestamp pauseTimestamp() const override;

		/**
		 * Returns the stop timestamp.
		 * @see Microphone::stopTimestamp().
		 */
		Timestamp stopTimestamp() const override;

	protected:

		/**
		 * Creates a new medium by a given url.
		 * @param url Url of the medium
		 */
		explicit QuestMicrophone(const std::string& url);

		/**
		 * Destructs the microphone object.
		 */
		~QuestMicrophone() override;

		/**
		 * Starts the medium.
		 * @see Medium::start().
		 */
		bool start() override;

		/**
		 * Pauses the medium.
		 * @see Medium::pause():
		 */
		bool pause() override;

		/**
		 * Stops the medium.
		 * @see Medium::stop().
		 */
		bool stop() override;

		/**
		 * The internal thread function.
		 */
		void threadRun() override;

	protected:

		/// The handle for the microphone.
		ovrMicrophoneHandle microphoneHandle_ = nullptr;

		/// Start timestamp.
		Timestamp startTimestamp_ = Timestamp(false);

		/// Pause timestamp.
		Timestamp pauseTimestamp_ = Timestamp(false);

		/// Stop timestamp.
		Timestamp stopTimestamp_ = Timestamp(false);
};

}

}

}

#endif // META_OCEAN_MEDIA_QUEST_MICROPHONE_H
