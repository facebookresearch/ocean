// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_AUDIO_EXPERIENCE_H
#define META_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_AUDIO_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/media/Audio.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/vrapi/application/VRTableMenu.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how play audio files.
 * @ingroup xrplayground
 */
class AudioExperience : public XRPlaygroundExperience
{
	protected:

		/**
		 * Definition of an unordered map mapping strings to audio objects.
		 */
		typedef std::unordered_map<std::string, Media::AudioRef> AudioMap;

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~AudioExperience() override;

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
		 * Pre update interaction function which allows to adjust any rendering object before it gets rendered.
		 * @see Experience::preUpdate().
		 */
		Timestamp preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Creates a new UserProfileExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		AudioExperience() = default;

	protected:

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/// The table menu allowing to show a simple user interface.
		Platform::Meta::Quest::VrApi::Application::VRTableMenu vrTableMenu_;

#endif

		/// The map holding all audio files.
		AudioMap audioMap_;
};

}

}

#endif // META_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_AUDIO_EXPERIENCE_H
