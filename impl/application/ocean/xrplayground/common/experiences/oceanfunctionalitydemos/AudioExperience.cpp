// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/AudioExperience.h"

#include "application/ocean/xrplayground/common/PlatformSpecific.h"

#include "ocean/io/FileResolver.h"

#include "ocean/media/Manager.h"

namespace Ocean
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	using namespace Platform::Meta::Quest::VrApi::Application;
#endif

namespace XRPlayground
{

AudioExperience::~AudioExperience()
{
	// nothing to do here
}

bool AudioExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	assert(engine);

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	vrTableMenu_ = VRTableMenu(engine, engine->framebuffers().front());

	const std::vector<std::string> audioFiles =
	{
		"laser_sound.mp3",
		"hit_sound.mp3"
	};

	VRTableMenu::Entries menuEntries;
	menuEntries.reserve(audioFiles.size());

	for (const std::string& audioFile : audioFiles)
	{
		const IO::Files resolvedFiles = IO::FileResolver::get().resolve(IO::File(audioFile), true);

		Media::AudioRef audio;

		if (!resolvedFiles.empty())
		{
			audio = Media::Manager::get().newMedium(resolvedFiles.front()(), Media::Medium::AUDIO);
		}

		if (audio)
		{
			menuEntries.emplace_back(audioFile, audioFile);

			audioMap_.emplace(audioFile, std::move(audio));
		}
		else
		{
			Log::error() << "Failed to load audio '" << audioFile << "'";
		}
	}

	if (audioMap_.empty())
	{
		return false;
	}

	const VRTableMenu::Group menuGroup("Which audio do you want to play?", std::move(menuEntries));

	vrTableMenu_.setMenuEntries(menuGroup);
	vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, -1)), PlatformSpecific::get().world_T_device(timestamp));

#else

	ocean_assert(false && "This should never happen!");

#endif

	return true;
}

bool AudioExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	audioMap_.clear();

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	vrTableMenu_.release();
#endif

	return true;
}

Timestamp AudioExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	std::string entryUrl;
	if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
	{
		const AudioMap::const_iterator iAudio = audioMap_.find(entryUrl);
		ocean_assert(iAudio != audioMap_.cend());

		if (iAudio != audioMap_.cend())
		{
			if (iAudio->second->isStarted())
			{
				Log::info() << "The audio '" << iAudio->first << "' is still running";
			}

			if (!iAudio->second->start())
			{
				Log::error() << "Failed to start the audio '" << iAudio->first << "'";
			}
		}
	}

#endif

	return timestamp;
}

std::unique_ptr<XRPlaygroundExperience> AudioExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new AudioExperience());
}

}

}
