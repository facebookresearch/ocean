// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/media/quest/QuestLibrary.h"
#include "ocean/media/quest/QuestLiveVideo.h"
#include "ocean/media/quest/QuestMicrophone.h"

namespace Ocean
{

namespace Media
{

namespace Quest
{

QuestLibrary::QuestLibrary() :
	Library(nameQuestLibrary(), 80u)
{
	registerNotSupportedExtension("avi");
	registerNotSupportedExtension("bmp");
	registerNotSupportedExtension("gif");
	registerNotSupportedExtension("jpeg");
	registerNotSupportedExtension("jpg");
	registerNotSupportedExtension("mpeg");
	registerNotSupportedExtension("mpg");
	registerNotSupportedExtension("mov");
	registerNotSupportedExtension("tiff");
}

QuestLibrary::~QuestLibrary()
{
	// nothing to do here
}

bool QuestLibrary::registerLibrary()
{
	return Manager::get().registerLibrary<QuestLibrary>(nameQuestLibrary());
}

bool QuestLibrary::unregisterLibrary()
{
	return Manager::get().unregisterLibrary(nameQuestLibrary());
}

MediumRef QuestLibrary::newMedium(const std::string& url, bool useExclusive)
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

	return newLiveVideo(url, useExclusive);
}

MediumRef QuestLibrary::newMedium(const std::string& url, const Medium::Type type, bool useExclusive)
{
	const ScopedLock scopedLock(lock);

	if (type == Medium::LIVE_VIDEO || type == Medium::FRAME_MEDIUM)
	{
		return newLiveVideo(url, useExclusive);
	}

	if (type == Medium::MICROPHONE)
	{
		return newMicrophone(url, useExclusive);
	}

	return MediumRef();
}

RecorderRef QuestLibrary::newRecorder(const Recorder::Type type)
{
	return RecorderRef();
}

Medium::Type QuestLibrary::supportedTypes() const
{
	return Medium::Type(Medium::LIVE_VIDEO | Medium::MICROPHONE);
}

LibraryRef QuestLibrary::create()
{
	return LibraryRef(new QuestLibrary());
}

MediumRef QuestLibrary::newLiveVideo(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, name(), Medium::LIVE_VIDEO));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	QuestLiveVideo* medium = new QuestLiveVideo(url);
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

MediumRef QuestLibrary::newMicrophone(const std::string& url, bool useExclusive)
{
	if (useExclusive == false)
	{
		MediumRef mediumRef(MediumRefManager::get().medium(url, name(), Medium::MICROPHONE));

		if (mediumRef)
		{
			return mediumRef;
		}
	}

	QuestMicrophone* medium = new QuestMicrophone(url);
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
