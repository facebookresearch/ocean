/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/imageio/IIOLibrary.h"
#include "ocean/media/imageio/IIOBufferImage.h"
#include "ocean/media/imageio/IIOBufferImageRecorder.h"
#include "ocean/media/imageio/IIOImage.h"
#include "ocean/media/imageio/IIOImageRecorder.h"
#include "ocean/media/imageio/IIOImageSequence.h"

namespace Ocean
{

namespace Media
{

namespace ImageIO
{

IIOLibrary::IIOLibrary() :
	Library(nameImageIOLibrary(), 85u)
{
	registerNotSupportedExtension("avi");
	registerNotSupportedExtension("mpeg");
	registerNotSupportedExtension("mpg");
	registerNotSupportedExtension("mov");
	registerNotSupportedExtension("mp4");
}

IIOLibrary::~IIOLibrary()
{
	// nothing to do here
}

bool IIOLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<IIOLibrary>(nameImageIOLibrary());
}

bool IIOLibrary::unregisterLibrary()
{
	return Manager::get().unregisterLibrary(nameImageIOLibrary());
}

MediumRef IIOLibrary::newMedium(const std::string& url, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	return newImage(url, useExclusive);
}

MediumRef IIOLibrary::newMedium(const std::string& url, const Medium::Type type, bool useExclusive)
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

RecorderRef IIOLibrary::newRecorder(const Recorder::Type type)
{
	switch (type)
	{
		case Recorder::BUFFER_IMAGE_RECORDER:
			return RecorderRef(new IIOBufferImageRecorder());
			
		case Recorder::FILE_RECORDER:
		case Recorder::FRAME_RECORDER:
		case Recorder::IMAGE_RECORDER:
			return RecorderRef(new IIOImageRecorder());
			
		default:
			break;
	}
	
	return RecorderRef();
}

Medium::Type IIOLibrary::supportedTypes() const
{
	return Medium::Type(Medium::IMAGE | Medium::BUFFER_IMAGE | Medium::IMAGE_SEQUENCE);
}

std::set<std::string> IIOLibrary::supportedImageTypes()
{
	std::set<std::string> result = {"bmp", "jpg", "png", "tif"};
	return result;
}

LibraryRef IIOLibrary::create()
{
	return LibraryRef(new IIOLibrary());
}

MediumRef IIOLibrary::newBufferImage(const std::string& url, bool useExclusive)
{
	IIOBufferImage* bufferImage = new IIOBufferImage(url);
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

MediumRef IIOLibrary::newImage(const std::string& url, bool useExclusive)
{
	IIOImage* medium = new IIOImage(url);
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
	
MediumRef IIOLibrary::newImageSequence(const std::string& url, bool useExclusive)
{
	IIOImageSequence* medium = new IIOImageSequence(url);
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
