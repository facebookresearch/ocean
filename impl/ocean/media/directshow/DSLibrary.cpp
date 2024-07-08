/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSLibrary.h"
#include "ocean/media/directshow/DSAudio.h"
#include "ocean/media/directshow/DSDeviceEnumerator.h"
#include "ocean/media/directshow/DSLiveVideo.h"
#include "ocean/media/directshow/DSMovie.h"
#include "ocean/media/directshow/DSMovieRecorder.h"

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSLibrary::DSLibrary() :
	Library(nameDirectShowLibrary(), 100u)
{
	const HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	libraryReleaseCOM_ = result != RPC_E_CHANGED_MODE;

	registerNotSupportedExtension("bmp");
	registerNotSupportedExtension("jpeg");
	registerNotSupportedExtension("jpg");
	registerNotSupportedExtension("png");
	registerNotSupportedExtension("tiff");
}

DSLibrary::~DSLibrary()
{
	DSEnumerators::get().release();

	if (libraryReleaseCOM_)
	{
		CoUninitialize();
	}
}

bool DSLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<DSLibrary>(nameDirectShowLibrary());
}

bool DSLibrary::unregisterLibrary()
{
	return Manager::get().unregisterLibrary(nameDirectShowLibrary());
}

MediumRef DSLibrary::newMedium(const std::string& url, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	MediumRef medium(newMovie(url, useExclusive));

	if (medium.isNull())
	{
		medium = newAudio(url, useExclusive);
	}

	if (medium.isNull())
	{
		medium = newLiveVideo(url, useExclusive);
	}

	return medium;
}

MediumRef DSLibrary::newMedium(const std::string& url, const Medium::Type type, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	switch (type)
	{
		case Medium::AUDIO:
			return newAudio(url, useExclusive);

		case Medium::MOVIE:
			return newMovie(url, useExclusive);

		case Medium::FRAME_MEDIUM:
		{
			MediumRef medium(newMovie(url, useExclusive));

			if (medium.isNull())
			{
				medium = newLiveVideo(url, useExclusive);
			}

			return medium;
		}

		case Medium::LIVE_AUDIO:
			return newLiveVideo(url, useExclusive);

		case Medium::LIVE_VIDEO:
			return newLiveVideo(url, useExclusive);

		case Medium::LIVE_MEDIUM:
		{
			MediumRef medium(newLiveVideo(url, useExclusive));
		}

		case Medium::SOUND_MEDIUM:
		{
			MediumRef medium(newMovie(url, useExclusive));

			if (medium.isNull())
			{
				medium = newAudio(url, useExclusive);
			}

			return medium;
		}

		default:
			break;
	};

	return MediumRef();
}

RecorderRef DSLibrary::newRecorder(const Recorder::Type type)
{
	switch (type)
	{
		case Recorder::FRAME_RECORDER:
		case Recorder::MOVIE_RECORDER:
			return RecorderRef(new DSMovieRecorder());

		default:
			break;
	}

	return RecorderRef();
}

Library::Definitions DSLibrary::selectableMedia() const
{
	const ScopedLock scopedLock(lock);

	Definitions entireDefinitions;

	Definitions definitions(selectableMedia(Medium::LIVE_AUDIO));
	entireDefinitions.insert(entireDefinitions.end(), definitions.begin(), definitions.end());

	definitions = selectableMedia(Medium::LIVE_VIDEO);
	entireDefinitions.insert(entireDefinitions.end(), definitions.begin(), definitions.end());

	return entireDefinitions;
}

Library::Definitions DSLibrary::selectableMedia(const Medium::Type type) const
{
	const ScopedLock scopedLock(lock);

	Definitions definitions;

	if (type & Medium::LIVE_VIDEO)
	{
		DSDeviceEnumerator& enumerator = DSEnumerators::get().enumerator(CLSID_VideoInputDeviceCategory);
		enumerator.refresh();

		DSDeviceEnumerator::Names names(enumerator.names());

		for (unsigned int n = 0; n < names.size(); n++)
		{
			definitions.emplace_back(std::move(names[n]), Medium::LIVE_VIDEO, name());
		}
	}
	else if (type & Medium::LIVE_AUDIO)
	{
		DSDeviceEnumerator& enumerator = DSEnumerators::get().enumerator(CLSID_AudioInputDeviceCategory);
		enumerator.refresh();

		DSDeviceEnumerator::Names names(enumerator.names());

		for (unsigned int n = 0; n < names.size(); n++)
		{
			definitions.emplace_back(std::move(names[n]), Medium::LIVE_AUDIO, name());
		}
	}

	return definitions;
}

Medium::Type DSLibrary::supportedTypes() const
{
	return Medium::Type(Medium::AUDIO | Medium::LIVE_AUDIO | Medium::LIVE_VIDEO | Medium::MOVIE);
}

LibraryRef DSLibrary::create()
{
	return LibraryRef(new DSLibrary());
}

MediumRef DSLibrary::newAudio(const std::string& url, bool useExclusive)
{
	DSAudio* medium = new DSAudio(url);
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

MediumRef DSLibrary::newLiveVideo(const std::string& url, bool useExclusive)
{
	std::string newUrl(url);

	if (newUrl.find("LiveVideoId:") == 0)
	{
		unsigned int index = atoi(newUrl.substr(12).c_str());

		DSDeviceEnumerator& enumerator = DSEnumerators::get().enumerator(CLSID_VideoInputDeviceCategory);
		DSDeviceEnumerator::Names names(enumerator.names());

		if (index < names.size())
			newUrl = names[index];
	}

	if (!useExclusive)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(newUrl, nameDirectShowLibrary(), Medium::LIVE_VIDEO));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	DSLiveVideo* medium = new DSLiveVideo(newUrl);
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

MediumRef DSLibrary::newMovie(const std::string& url, bool useExclusive)
{
	DSMovie* medium = new DSMovie(url);
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
