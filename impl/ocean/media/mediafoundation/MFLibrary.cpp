// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/mediafoundation/MFLibrary.h"
#include "ocean/media/mediafoundation/MFAudio.h"
#include "ocean/media/mediafoundation/MFMovie.h"
#include "ocean/media/mediafoundation/MFMovieRecorder.h"

namespace Ocean
{

namespace Media
{

namespace MediaFoundation
{

MFLibrary::MFLibrary() :
	Library(nameMediaFoundationLibrary(), 105u)
{
	HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	releaseCOM_ = result != RPC_E_CHANGED_MODE;

	result = MFStartup(MF_VERSION, MFSTARTUP_FULL);
	ocean_assert(result == S_OK);

	registerNotSupportedExtension("bmp");
	registerNotSupportedExtension("jpeg");
	registerNotSupportedExtension("jpg");
	registerNotSupportedExtension("png");
	registerNotSupportedExtension("tiff");
}

MFLibrary::~MFLibrary()
{
	const HRESULT result = MFShutdown();
	ocean_assert_and_suppress_unused(result == S_OK, result);

	if (releaseCOM_)
	{
		CoUninitialize();
	}
}

bool MFLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<MFLibrary>(nameMediaFoundationLibrary());
}

bool MFLibrary::unregisterLibrary()
{
	return Manager::get().unregisterLibrary(nameMediaFoundationLibrary());
}

MediumRef MFLibrary::newMedium(const std::string& url, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	MediumRef medium(newMovie(url, useExclusive));

	if (medium.isNull())
	{
		medium = newAudio(url, useExclusive);
	}

	return medium;
}

MediumRef MFLibrary::newMedium(const std::string& url, const Medium::Type type, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	switch (type)
	{
		case Medium::AUDIO:
			return newAudio(url, useExclusive);

		case Medium::MOVIE:
			return newMovie(url, useExclusive);

		case Medium::FRAME_MEDIUM:
			return newMovie(url, useExclusive);

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

RecorderRef MFLibrary::newRecorder(const Recorder::Type type)
{
	if (type == Recorder::MOVIE_RECORDER)
	{
		return RecorderRef(new MFMovieRecorder());
	}

	return RecorderRef();
}

Library::Definitions MFLibrary::selectableMedia() const
{
	const ScopedLock scopedLock(lock);

	Definitions entireDefinitions;

	Definitions definitions(selectableMedia(Medium::LIVE_AUDIO));
	entireDefinitions.insert(entireDefinitions.end(), definitions.begin(), definitions.end());

	definitions = selectableMedia(Medium::LIVE_VIDEO);
	entireDefinitions.insert(entireDefinitions.end(), definitions.begin(), definitions.end());

	return entireDefinitions;
}

Library::Definitions MFLibrary::selectableMedia(const Medium::Type /*type*/) const
{
	return Definitions();
}

Medium::Type MFLibrary::supportedTypes() const
{
	return Medium::Type(Medium::AUDIO | Medium::MOVIE);
}

LibraryRef MFLibrary::create()
{
	return LibraryRef(new MFLibrary());
}

MediumRef MFLibrary::newAudio(const std::string& url, bool useExclusive)
{
	MFAudio* medium = new MFAudio(url);
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

MediumRef MFLibrary::newMovie(const std::string& url, bool useExclusive)
{
	MFMovie* medium = new MFMovie(url);
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
