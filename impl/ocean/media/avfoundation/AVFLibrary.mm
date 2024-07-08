/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/avfoundation/AVFLibrary.h"
#include "ocean/media/avfoundation/AVFAudio.h"
#include "ocean/media/avfoundation/AVFDevices.h"
#include "ocean/media/avfoundation/AVFLiveAudio.h"
#include "ocean/media/avfoundation/AVFLiveVideo.h"
#include "ocean/media/avfoundation/AVFMicrophone.h"
#include "ocean/media/avfoundation/AVFMovie.h"
#include "ocean/media/avfoundation/AVFMovieRecorder.h"

#include "ocean/base/String.h"

#include "ocean/platform/apple/System.h"

namespace Ocean
{

namespace Media
{

namespace AVFoundation
{

AVFLibrary::AVFLibrary() :
	Library(nameAVFLibrary(), 90u)
{
	registerNotSupportedExtension("bmp");
	registerNotSupportedExtension("jpeg");
	registerNotSupportedExtension("jpg");
	registerNotSupportedExtension("png");
	registerNotSupportedExtension("tiff");

#ifdef OCEAN_DEBUG
	if (!Platform::Apple::System::isBundleApp())
	{
		Log::debug() << "Beware: AVFoundation is used in a command line app (CLI), you need to call Platform::Apple::System::runMainLoop() regularly (out of the main thread)";
	}
#endif // OCEAN_DEBUG
}

AVFLibrary::~AVFLibrary()
{
	// nothing to do here
}

bool AVFLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<AVFLibrary>(nameAVFLibrary());
}

bool AVFLibrary::unregisterLibrary()
{
	return Manager::get().unregisterLibrary(nameAVFLibrary());
}

MediumRef AVFLibrary::newMedium(const std::string& url, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	if (String::toLower(url).find("microphone") != std::string::npos)
	{
		MediumRef medium = newMicrophone(url, useExclusive);

		if (medium)
		{
			return medium;
		}
	}

	MediumRef medium(newMovie(url, useExclusive));

	if (medium.isNull())
	{
		medium = newLiveVideo(url, useExclusive);
	}

	if (medium.isNull())
	{
		medium = newAudio(url, useExclusive);
	}

	return medium;
}

MediumRef AVFLibrary::newMedium(const std::string& url, const Medium::Type type, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	switch (type)
	{
		case Medium::AUDIO:
		{
			return newAudio(url, useExclusive);
		}

		case Medium::MOVIE:
		{
			return newMovie(url, useExclusive);
		}

		case Medium::FRAME_MEDIUM:
		{
			MediumRef medium(newMovie(url, useExclusive));

			if (medium.isNull())
			{
				medium = newLiveVideo(url, useExclusive);
			}

			return medium;
		}

		case Medium::LIVE_VIDEO:
		{
			return newLiveVideo(url, useExclusive);
		}

		case Medium::MICROPHONE:
		{
			return newMicrophone(url, useExclusive);
		}

		case Medium::LIVE_AUDIO:
		{
			return newLiveAudio(url, useExclusive);
		}

		default:
			break;
	}

	return MediumRef();
}

RecorderRef AVFLibrary::newRecorder(const Recorder::Type type)
{
	if (type == Recorder::MOVIE_RECORDER)
	{
		return RecorderRef(new AVFMovieRecorder());
	}

	return RecorderRef();
}

Library::Definitions AVFLibrary::selectableMedia() const
{
	return selectableMedia(Medium::LIVE_VIDEO);
}

Library::Definitions AVFLibrary::selectableMedia(const Medium::Type type) const
{
	Definitions definitions;

	if (type == Medium::LIVE_VIDEO)
	{
		const AVFDevices::Devices devices(AVFDevices::videoDevices());

		definitions.reserve(devices.size());

		for (size_t n = 0; n < devices.size(); ++n)
		{
			definitions.emplace_back(String::toAString(devices[n].friendlyName()), Medium::LIVE_VIDEO, name());
		}
	}

	return definitions;
}

Medium::Type AVFLibrary::supportedTypes() const
{
	return Medium::Type(Medium::AUDIO | Medium::MOVIE | Medium::LIVE_VIDEO | Medium::MICROPHONE | Medium::LIVE_AUDIO);
}

LibraryRef AVFLibrary::create()
{
	return LibraryRef(new AVFLibrary());
}

MediumRef AVFLibrary::newAudio(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, nameAVFLibrary(), Medium::AUDIO));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	AVFAudio* medium = new AVFAudio(url);
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

MediumRef AVFLibrary::newMovie(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, nameAVFLibrary(), Medium::MOVIE));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	AVFMovie* medium = new AVFMovie(url);
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

MediumRef AVFLibrary::newLiveAudio(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, nameAVFLibrary(), Medium::LIVE_AUDIO));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	AVFLiveAudio* medium = new AVFLiveAudio(url);
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

MediumRef AVFLibrary::newLiveVideo(const std::string& url, bool useExclusive)
{
	std::string newUrl(url);

	if (newUrl.find("LiveVideoId:") == 0)
	{
		int index = -1;
		if (String::isInteger32(newUrl.substr(12), &index) && index >= 0)
		{
			const AVFDevices::Devices devices(AVFDevices::videoDevices());

			if (size_t(index) >= devices.size())
			{
				return MediumRef();
			}

			newUrl = String::toAString(devices[size_t(index)].friendlyName());
		}
	}

	if (!useExclusive)
	{
		MediumRef medium(MediumRefManager::get().medium(newUrl, nameAVFLibrary(), Medium::LIVE_VIDEO));

		if (medium)
		{
			return medium;
		}
	}

	AVFLiveVideo* medium = new AVFLiveVideo(newUrl);
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


MediumRef AVFLibrary::newMicrophone(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, name(), Medium::MICROPHONE));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	AVFMicrophone* medium = new AVFMicrophone(url);
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
