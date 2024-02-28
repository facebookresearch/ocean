// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/vrs/VRSLibrary.h"
#include "ocean/media/vrs/VRSImageSequence.h"
#include "ocean/media/vrs/VRSMovie.h"

namespace Ocean
{

namespace Media
{

namespace VRS
{

VRSLibrary::VRSLibrary() :
	Library(nameVRSLibrary(), 5u)
{
	registerNotSupportedExtension("bmp");
	registerNotSupportedExtension("jpeg");
	registerNotSupportedExtension("jpg");
	registerNotSupportedExtension("png");
	registerNotSupportedExtension("tiff");
	registerNotSupportedExtension("avi");
	registerNotSupportedExtension("mp4");
	registerNotSupportedExtension("mpg");
	registerNotSupportedExtension("mpeg");
}

VRSLibrary::~VRSLibrary()
{
	// nothing to do here
}

bool VRSLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<VRSLibrary>(nameVRSLibrary());
}

bool VRSLibrary::unregisterLibrary()
{
	return Manager::get().unregisterLibrary(nameVRSLibrary());
}

MediumRef VRSLibrary::newMedium(const std::string& url, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	const MediumRef medium(newMovie(url, useExclusive));

	return medium;
}

MediumRef VRSLibrary::newMedium(const std::string& url, const Medium::Type type, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	switch (type)
	{
		case Medium::MOVIE:
			return newMovie(url, useExclusive);

		case Medium::IMAGE_SEQUENCE:
			return newImageSequence(url, useExclusive);

		default:
			break;
	};

	return MediumRef();
}

RecorderRef VRSLibrary::newRecorder(const Recorder::Type type)
{
	return RecorderRef();
}

Medium::Type VRSLibrary::supportedTypes() const
{
	return Medium::Type(Medium::MOVIE | Medium::IMAGE_SEQUENCE);
}

LibraryRef VRSLibrary::create()
{
	return LibraryRef(new VRSLibrary());
}

MediumRef VRSLibrary::newMovie(const std::string& url, bool useExclusive)
{
	VRSMovie* medium = new VRSMovie(url);
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

MediumRef VRSLibrary::newImageSequence(const std::string& url, bool useExclusive)
{
	VRSImageSequence* medium = new VRSImageSequence(url);
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
