/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/AudioClip.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/Manager.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

AudioClip::AudioClip(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	SDXUpdateNode(environment),
	X3DTimeDependentNode(environment),
	X3DSoundSourceNode(environment),
	X3DUrlObject(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

AudioClip::NodeSpecification AudioClip::specifyNode()
{
	NodeSpecification specification("AudioClip");

	X3DSoundSourceNode::registerFields(specification);
	X3DUrlObject::registerFields(specification);

	return specification;
}

void AudioClip::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DSoundSourceNode::onInitialize(scene, timestamp);
	X3DUrlObject::onInitialize(scene, timestamp);

	const IO::Files resolvedFiles(resolveUrls());

	if (!resolvedFiles.empty())
	{
		for (const IO::File& resolvedFile : resolvedFiles)
		{
			if (resolvedFile.exists())
			{
				soundMedium_ = Media::Manager::get().newMedium(resolvedFile(), Media::Medium::AUDIO, true);

				if (soundMedium_)
				{
					break;
				}
			}
		}

		if (soundMedium_.isNull())
		{
			Log::warning() << "Failed to load the sound file \"" << resolvedFiles[0]() << "\".";
		}
	}

	const Media::FiniteMediumRef finiteMedium(soundMedium_);

	if (finiteMedium)
	{
		finiteMedium->setSpeed(float(pitch_.value()));
		finiteMedium->setLoop(loop_.value());

		durationChanged_.setValue(Timestamp(finiteMedium->normalDuration()), Timestamp(true));
		forwardThatFieldHasBeenChanged("duration_changed", durationChanged_);
	}

	if (loop_.value() && stopTime_.value() <= startTime_.value())
	{
		startNode(timestamp, timestamp);
	}
}

void AudioClip::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "url")
	{
		const IO::Files resolvedFiles(resolveUrls());

		if (!resolvedFiles.empty())
		{
			for (const IO::File& resolvedFile : resolvedFiles)
			{
				if (resolvedFile.exists())
				{
					soundMedium_ = Media::Manager::get().newMedium(resolvedFile(), Media::Medium::AUDIO, true);

					if (soundMedium_)
					{
						break;
					}
				}
			}

			if (soundMedium_.isNull())
			{
				Log::warning() << "Failed to load the sound file \"" << resolvedFiles[0]() << "\".";
			}
		}

		const Media::FiniteMediumRef finiteMedium(soundMedium_);

		if (finiteMedium)
		{
			finiteMedium->setSpeed(float(pitch_.value()));
			finiteMedium->setLoop(loop_.value());

			durationChanged_.setValue(Timestamp(finiteMedium->normalDuration()), Timestamp(true));
			forwardThatFieldHasBeenChanged("duration_changed", durationChanged_);
		}

		return;
	}
	else if (fieldName == "loop")
	{
		const Media::FiniteMediumRef finiteMedium(soundMedium_);

		if (finiteMedium)
		{
			finiteMedium->setLoop(loop_.value());
		}
	}
	else if (fieldName == "speed")
	{
		const Media::FiniteMediumRef finiteMedium(soundMedium_);

		if (finiteMedium)
		{
			finiteMedium->setSpeed(float(pitch_.value()));
		}
	}

	X3DSoundSourceNode::onFieldChanged(fieldName);
	X3DUrlObject::onFieldChanged(fieldName);
}

void AudioClip::onUpdated(const Timestamp timestamp)
{
	if (isActive_.value() && soundMedium_)
	{
		if (soundMedium_->stopTimestamp().isValid())
		{
			// The scene description's timestamp may be defined in a different domain the medium's timestamp
			// therefore, using the scene description's timestamp

			stopNode(timestamp, timestamp);
		}
	}
}

void AudioClip::onStarted(const Timestamp /*eventTimestamp*/)
{
	if (soundMedium_)
	{
		const Media::FiniteMediumRef finiteMedium(soundMedium_);

		if (finiteMedium)
		{
			finiteMedium->setPosition(0);
		}

		soundMedium_->start();
	}
}

void AudioClip::onPaused(const Timestamp /*eventTimestamp*/)
{
	if (soundMedium_)
	{
		soundMedium_->pause();
	}
}

void AudioClip::onResumed(const Timestamp /*eventTimestamp*/)
{
	if (soundMedium_)
	{
		soundMedium_->start();
	}
}

void AudioClip::onStopped(const Timestamp /*eventTimestamp*/)
{
	if (soundMedium_)
	{
		soundMedium_->stop();
	}
}

size_t AudioClip::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
