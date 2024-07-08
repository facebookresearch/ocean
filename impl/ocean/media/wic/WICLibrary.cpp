/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/wic/WICLibrary.h"
#include "ocean/media/wic/WICBufferImage.h"
#include "ocean/media/wic/WICBufferImageRecorder.h"
#include "ocean/media/wic/WICImage.h"
#include "ocean/media/wic/WICImageRecorder.h"
#include "ocean/media/wic/WICImageSequence.h"

namespace Ocean
{

namespace Media
{

namespace WIC
{

WICLibrary::ComInitializer::ComInitializer() :
	initialized_(false)
{
	// nothing to do here
}

void WICLibrary::ComInitializer::initialize()
{
	const ScopedLock scopedLock(lock_);

	if (initialized_)
	{
		return;
	}

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	initialized_ = true;
}

WICLibrary::WICLibrary() :
	Library(nameWICLibrary(), 85u)
{
	ComInitializer::get().initialize();

	registerNotSupportedExtension("avi");
	registerNotSupportedExtension("mpeg");
	registerNotSupportedExtension("mpg");
	registerNotSupportedExtension("mov");
	registerNotSupportedExtension("mp4");
}

WICLibrary::~WICLibrary()
{
	// nothing to do here
}

bool WICLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<WICLibrary>(nameWICLibrary());
}

bool WICLibrary::unregisterLibrary()
{
	return Manager::get().unregisterLibrary(nameWICLibrary());
}

MediumRef WICLibrary::newMedium(const std::string& url, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	return newImage(url, useExclusive);
}

MediumRef WICLibrary::newMedium(const std::string& url, const Medium::Type type, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	if (type == Medium::BUFFER_IMAGE)
	{
		return newBufferImage(url, useExclusive);
	}

	if (type == Medium::IMAGE_SEQUENCE)
	{
		return newImageSequence(url, useExclusive);
	}

	if (type == Medium::IMAGE || type == Medium::FRAME_MEDIUM)
	{
		return newImage(url, useExclusive);
	}

	return MediumRef();
}

RecorderRef WICLibrary::newRecorder(const Recorder::Type type)
{
	switch (type)
	{
		case Recorder::BUFFER_IMAGE_RECORDER:
			return RecorderRef(new WICBufferImageRecorder());

		case Recorder::FILE_RECORDER:
		case Recorder::FRAME_RECORDER:
		case Recorder::IMAGE_RECORDER:
			return RecorderRef(new WICImageRecorder());

		default:
			break;
	}

	return RecorderRef();
}

Medium::Type WICLibrary::supportedTypes() const
{
	return Medium::Type(Medium::IMAGE | Medium::BUFFER_IMAGE | Medium::IMAGE_SEQUENCE);
}

LibraryRef WICLibrary::create()
{
	return LibraryRef(new WICLibrary());
}

MediumRef WICLibrary::newBufferImage(const std::string& url, bool useExclusive)
{
	WICBufferImage* bufferImage = new WICBufferImage(url);
	ocean_assert(bufferImage != nullptr);

	if (bufferImage->isValid())
	{
		if (useExclusive)
		{
			return MediumRef(bufferImage);
		}

		return MediumRefManager::get().registerMedium(bufferImage);
	}
	else
	{
		delete bufferImage;
	}

	return MediumRef();
}

MediumRef WICLibrary::newImage(const std::string& url, bool useExclusive)
{
	WICImage* medium = new WICImage(url);
	ocean_assert(medium != nullptr);

	if (medium->isValid() == false)
	{
		delete medium;
		return MediumRef();
	}

	if (useExclusive)
	{
		return MediumRef(medium);
	}

	return MediumRefManager::get().registerMedium(medium);
}

MediumRef WICLibrary::newImageSequence(const std::string& url, bool useExclusive)
{
	WICImageSequence* medium = new WICImageSequence(url);
	ocean_assert(medium != nullptr);

	if (medium->isValid() == false)
	{
		delete medium;
		return MediumRef();
	}

	if (useExclusive)
	{
		return MediumRef(medium);
	}

	return MediumRefManager::get().registerMedium(medium);
}

}

}

}
