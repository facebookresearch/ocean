/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/MovieTexture.h"

#include "ocean/media/Movie.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

MovieTexture::MovieTexture(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DAppearanceChildNode(environment),
	X3DTextureNode(environment),
	X3DTexture2DNode(environment),
	X3DChildNode(environment),
	SDXUpdateNode(environment),
	X3DTimeDependentNode(environment),
	X3DSoundSourceNode(environment),
	X3DUrlObject(environment)
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

MovieTexture::NodeSpecification MovieTexture::specifyNode()
{
	NodeSpecification specification("MovieTexture");

	// strange 'speed' field which is not consistent with the abstract base node
	registerField(specification, "speed", pitch_);

	X3DTexture2DNode::registerFields(specification);
	X3DSoundSourceNode::registerFields(specification);
	X3DUrlObject::registerFields(specification);

	return specification;
}

void MovieTexture::onInitialize(const Rendering::SceneRef& scene, const Timestamp timestamp)
{
	X3DTexture2DNode::onInitialize(scene, timestamp);
	X3DSoundSourceNode::onInitialize(scene, timestamp);
	X3DUrlObject::onInitialize(scene, timestamp);

	try
	{
		const IO::Files resolvedFiles(resolveUrls());

		StringVector urls;
		for (IO::Files::const_iterator i = resolvedFiles.begin(); i != resolvedFiles.end(); ++i)
		{
			if (i->exists())
			{
				urls.push_back((*i)());
			}
		}

		applyUrl(urls, Media::Medium::MOVIE, false);

		Media::MovieRef movie(textureMedium_);
		if (movie)
		{
			movie->setLoop(loop_.value());
			movie->setSpeed(float(pitch_.value()));

			durationChanged_.setValue(Timestamp(movie->normalDuration()), Timestamp(true));
			forwardThatFieldHasBeenChanged("duration_changed", durationChanged_);
		}
	}
	catch (const OceanException& exception)
	{
		Log::warning() << exception.what();
	}

	if (loop_.value() && stopTime_.value() <= startTime_.value())
	{
		startNode(timestamp, timestamp);
	}
}

void MovieTexture::onFieldChanged(const std::string& fieldName)
{
	if (fieldName == "url")
	{
		const IO::Files resolvedFiles(resolveUrls());

		StringVector urls;
		for (IO::Files::const_iterator i = resolvedFiles.begin(); i != resolvedFiles.end(); ++i)
		{
			if (i->exists())
			{
				urls.push_back((*i)());
			}
		}

		applyUrl(urls, Media::Medium::MOVIE, false);

		Media::MovieRef movie(textureMedium_);
		if (movie)
		{
			movie->setLoop(loop_.value());
			movie->setSpeed(float(pitch_.value()));

			durationChanged_.setValue(Timestamp(movie->normalDuration()), Timestamp(true));
			forwardThatFieldHasBeenChanged("duration_changed", durationChanged_);
		}

		return;
	}
	else if (fieldName == "loop")
	{
		Media::MovieRef movie(textureMedium_);
		if (movie)
		{
			movie->setLoop(loop_.value());
		}
	}
	else if (fieldName == "speed")
	{
		Media::MovieRef movie(textureMedium_);
		if (movie)
		{
			movie->setSpeed(float(pitch_.value()));
		}
	}

	X3DTexture2DNode::onFieldChanged(fieldName);
	X3DSoundSourceNode::onFieldChanged(fieldName);
	X3DUrlObject::onFieldChanged(fieldName);
}

void MovieTexture::onUpdated(const Timestamp timestamp)
{
	if (isActive_.value() && textureMedium_)
	{
		if (double(textureMedium_->stopTimestamp()) > 0.0 && timestamp >= textureMedium_->stopTimestamp())
		{
			stopNode(textureMedium_->stopTimestamp(), timestamp);
		}
	}
}

void MovieTexture::onStarted(const Timestamp /*eventTimestamp*/)
{
	if (textureMedium_)
	{
		Media::FiniteMediumRef finiteMedium(textureMedium_);
		if (finiteMedium)
		{
			finiteMedium->setPosition(0);
		}

		textureMedium_->start();
	}
}

void MovieTexture::onPaused(const Timestamp /*eventTimestamp*/)
{
	if (textureMedium_)
	{
		textureMedium_->pause();
	}
}

void MovieTexture::onResumed(const Timestamp /*eventTimestamp*/)
{
	if (textureMedium_)
	{
		textureMedium_->start();
	}
}

void MovieTexture::onStopped(const Timestamp /*eventTimestamp*/)
{
	if (textureMedium_)
	{
		textureMedium_->stop();
	}
}

size_t MovieTexture::objectAddress() const
{
	return size_t(this);
}

}

}

}

}
