// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceanfunctionalitydemos/MicrophoneSpeakerExperience.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

MicrophoneSpeakerExperience::~MicrophoneSpeakerExperience()
{
	// nothing to do here
}

bool MicrophoneSpeakerExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	microphoneUrlPairs_ = MicrophoneUrlPairs
	{
		{"Voice Microphone", "\n Voice communication microphone \n Audio feedback should be reduced \n"},
		{"Gained Voice Microphone", "\n Gained voice communication microphone \n Audio feedback should be reduced \n"},

		{"Performance Microphone", "\n Performance microphone \n Expect audio feedback without headphones \n"},
		{"Gained Performance Microphone", "\n Gained performance microphone \n Expect audio feedback without headphones \n"},

		{"External Microphone", "\n External microphone \n Use headphones with microphone \n"}
	};

	liveAudio_ = Media::Manager::get().newMedium("Speaker", Media::Medium::LIVE_AUDIO, true /*useExclusive*/);

	if (liveAudio_)
	{
		if (!liveAudio_->start())
		{
			Log::error() << "Failed to start live audio";
		}
	}
	else
	{
		Log::error() << "Failed to access live audio";
	}

	const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, "", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.7f, 0.7f, 0.7f), false, 0, 0, Scalar(0.15), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string(), &renderingText_);
	textTransform->setTransformation(HomogenousMatrix4(Vector3(0, 0, -5)));
	experienceScene()->addChild(textTransform);

	nextMicrophone();

	return true;
}

bool MicrophoneSpeakerExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	microphoneSubscription_.release();
	microphone_.release();

	liveAudio_.release();

	return true;
}

void MicrophoneSpeakerExperience::onKeyPress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& key, const Timestamp /*timestamp*/)
{
	if (key == "A" || key == "X")
	{
		nextMicrophone();
	}
}

void MicrophoneSpeakerExperience::onMousePress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& /*button*/, const Vector2& /*screenPosition*/, const Line3& /*ray*/, const Timestamp /*timestamp*/)
{
	nextMicrophone();
}

std::unique_ptr<XRPlaygroundExperience> MicrophoneSpeakerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new MicrophoneSpeakerExperience());
}

void MicrophoneSpeakerExperience::nextMicrophone()
{
	microphoneSubscription_.release();
	microphone_.release();

	ocean_assert(newMicrophoneUrlIndex_ < microphoneUrlPairs_.size());

	const std::string& microphoneUrl = microphoneUrlPairs_[newMicrophoneUrlIndex_].first;
	std::string microphoneDescription = microphoneUrlPairs_[newMicrophoneUrlIndex_].second;

	microphone_ = Media::Manager::get().newMedium(microphoneUrl, Media::Medium::MICROPHONE);

	if (microphone_)
	{
		microphoneSubscription_ = microphone_->addSamplesCallback(std::bind(&MicrophoneSpeakerExperience::onMicrophoneSamples, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		ocean_assert(microphoneSubscription_.isValid());

		if (!microphone_->start())
		{
			Log::error() << "Failed to start microphone";
		}
	}
	else
	{
		Log::error() << "Failed to access microphone";

		microphoneDescription = " Failed to create \n " + microphoneDescription;
	}

	ocean_assert(renderingText_);
	renderingText_->setText(microphoneDescription);

	newMicrophoneUrlIndex_ = (newMicrophoneUrlIndex_ + 1) % microphoneUrlPairs_.size();
}

void MicrophoneSpeakerExperience::onMicrophoneSamples(const Media::Microphone::SamplesType samplesType, const void* data, const size_t size)
{
	if (samplesType != Media::Microphone::ST_INTEGER_16_MONO_48)
	{
		Log::error() << "Microphone samples type is not supported";
		return;
	}

	if (liveAudio_)
	{
		liveAudio_->addSamples(Media::LiveAudio::ST_INTEGER_16_MONO_48, data, size);
	}
}

}

}
