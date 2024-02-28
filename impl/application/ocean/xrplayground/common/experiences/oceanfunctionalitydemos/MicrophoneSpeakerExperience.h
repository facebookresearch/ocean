// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MICROPHONE_SPEAKER_EXPERIENCE_H
#define META_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MICROPHONE_SPEAKER_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/media/LiveAudio.h"
#include "ocean/media/Microphone.h"

#include "ocean/rendering/Text.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how the microphone can be used.
 * @ingroup xrplayground
 */
class MicrophoneSpeakerExperience : public XRPlaygroundExperience
{
	protected:

		/**
		 * Definition of a pair holding microphone urls and descriptors.
		 */
		typedef std::pair<std::string, std::string> MicrophoneUrlPair;

		/**
		 * Definition of a vector holding microphone url pairs.
		 */
		typedef std::vector<MicrophoneUrlPair> MicrophoneUrlPairs;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~MicrophoneSpeakerExperience() override;

		/**
		 * Loads this experience.
		 * @see Experience::load().
		 */
		bool load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties) override;

		/**
		 * Unloads this experience.
		 * @see Experience::unload().
		 */
		bool unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Key press function.
		 * @see Experience::onKeyPress().
		 */
		void onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

		/**
		 * Mouse press event function.
		 * @see Experience::onMousePress().
		 */
		void onMousePress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp) override;

		/**
		 * Creates a new UserProfileExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		MicrophoneSpeakerExperience() = default;

		/**
		 * Switches to the next microphone configuration.
		 */
		void nextMicrophone();

		/**
		 * Event function for new microphone samples.
		 * @param samplesType The type of the samples
		 * @param data The samples data, must be valid
		 * @param size The size of the data, in bytes, with range [1, infinity)
		 */
		void onMicrophoneSamples(const Media::Microphone::SamplesType samplesType, const void* data, const size_t size);

	protected:

		/// The rendering Text node showing some information about the current microphone.
		Rendering::TextRef renderingText_;

		/// The microphone of this experience.
		Media::MicrophoneRef microphone_;

		/// The speaker of this experience.
		Media::LiveAudioRef liveAudio_;

		/// The subscription for microphone samples for local avatars.
		Media::Microphone::SamplesScopedSubscription microphoneSubscription_;

		/// The pairs with microphone urls.
		MicrophoneUrlPairs microphoneUrlPairs_;

		/// The index of the next microphone to be used.
		size_t newMicrophoneUrlIndex_ = 0;
};

}

}

#endif // META_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_MICROPHONE_SPEAKER_EXPERIENCE_H
