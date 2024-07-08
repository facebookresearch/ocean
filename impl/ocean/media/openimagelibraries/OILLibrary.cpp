/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/openimagelibraries/OILLibrary.h"
#include "ocean/media/openimagelibraries/OILBufferImage.h"
#include "ocean/media/openimagelibraries/OILBufferImageRecorder.h"
#include "ocean/media/openimagelibraries/OILImage.h"
#include "ocean/media/openimagelibraries/OILImageRecorder.h"
#include "ocean/media/openimagelibraries/OILImageSequence.h"
#include "ocean/media/openimagelibraries/OILImageSequenceRecorder.h"

namespace Ocean
{

namespace Media
{

namespace OpenImageLibraries
{

OILLibrary::OILLibrary() :
	Library(nameOpenImageLibrariesLibrary(), 91u)
{
	registerNotSupportedExtension("avi");
	registerNotSupportedExtension("mpeg");
	registerNotSupportedExtension("mpg");
	registerNotSupportedExtension("mov");
	registerNotSupportedExtension("mp4");
	registerNotSupportedExtension("gif");
}

OILLibrary::~OILLibrary()
{
	// nothing to do here
}

bool OILLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<OILLibrary>(nameOpenImageLibrariesLibrary());
}

bool OILLibrary::unregisterLibrary()
{
	return Manager::get().unregisterLibrary(nameOpenImageLibrariesLibrary());
}

MediumRef OILLibrary::newMedium(const std::string& url, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	return newImage(url, useExclusive);
}

MediumRef OILLibrary::newMedium(const std::string& url, const Medium::Type type, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	if (type == Medium::BUFFER_IMAGE)
		return newBufferImage(url, useExclusive);

	if (type == Medium::IMAGE_SEQUENCE)
		return newImageSequence(url, useExclusive);

	if (type == Medium::IMAGE || type == Medium::FRAME_MEDIUM)
		return newImage(url, useExclusive);

	return MediumRef();
}

RecorderRef OILLibrary::newRecorder(const Recorder::Type type)
{
	switch (type)
	{
		case Recorder::BUFFER_IMAGE_RECORDER:
			return RecorderRef(new OILBufferImageRecorder());

		case Recorder::FILE_RECORDER:
		case Recorder::FRAME_RECORDER:
		case Recorder::IMAGE_RECORDER:
			return RecorderRef(new OILImageRecorder());

		case Recorder::IMAGE_SEQUENCE_RECORDER:
			return RecorderRef(new OILImageSequenceRecorder());

		default:
			break;
	}

	return RecorderRef();
}

Medium::Type OILLibrary::supportedTypes() const
{
	return Medium::Type(Medium::IMAGE | Medium::IMAGE_SEQUENCE | Medium::BUFFER_IMAGE);
}

std::set<std::string> OILLibrary::supportedImageTypes()
{
	std::set<std::string> result = {"bmp"};

#ifdef OCEAN_MEDIA_OIL_SUPPORT_JPG
	result.insert("jpg");
#endif

#ifdef OCEAN_MEDIA_OIL_SUPPORT_PNG
	result.insert("png");
#endif

#ifdef OCEAN_MEDIA_OIL_SUPPORT_TIF
	result.insert("tif");
#endif

	return result;
}

LibraryRef OILLibrary::create()
{
	return LibraryRef(new OILLibrary());
}

MediumRef OILLibrary::newBufferImage(const std::string& url, bool useExclusive)
{
	OILBufferImage* bufferImage = new OILBufferImage(url);
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

MediumRef OILLibrary::newImage(const std::string& url, bool useExclusive)
{
	OILImage* medium = new OILImage(url);
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

MediumRef OILLibrary::newImageSequence(const std::string& url, bool useExclusive)
{
	OILImageSequence* medium = new OILImageSequence(url);
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
