/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/Manager.h"
#include "ocean/media/PixelImage.h"

#include "ocean/base/String.h"

#include "ocean/io/File.h"

namespace Ocean
{

namespace Media
{

Manager::Manager()
{
	// nothing to do here
}

Manager::~Manager()
{

#ifdef OCEAN_DEBUG
	if (!libraries_.empty())
	{
		Log::debug() << "The following media libraries are still in use:";

		for (const LibraryCounterPair& counterPair : libraries_)
		{
			ocean_assert(counterPair.first);
			Log::debug() << counterPair.first->name();
		}

		// The Manager::release() function should be called earlier
		ocean_assert(false && "Several media libraries are still in use, Manager::release() or better Manager::unregisterLibrary() should be called earlier");
	}
#endif

	release();
}

MediumRef Manager::newMedium(const std::string& url, bool useExclusive)
{
	ocean_assert(url.empty() == false);

	const ScopedLock scopedLock(lock_);

	if (useExclusive == false)
	{
		const MediumRef medium(MediumRefManager::get().medium(url));

		if (medium)
		{
			return medium;
		}
	}

	const IO::File file(url);
	const std::string fileExtension(String::toLower(file.extension()));

	for (Libraries::const_iterator i = libraries_.cbegin(); i != libraries_.cend(); ++i)
	{
		ocean_assert(i->first);

		if (i->first->notSupported(fileExtension) == false)
		{
			const MediumRef medium(i->first->newMedium(url, useExclusive));

			if (medium)
			{
				return medium;
			}
		}
	}

	return MediumRef();
}

MediumRef Manager::newMedium(const std::string& url, const Medium::Type type, bool useExclusive)
{
	ocean_assert(url.empty() == false);

	const ScopedLock scopedLock(lock_);

	if (useExclusive == false)
	{
		const MediumRef medium(MediumRefManager::get().medium(url, type));

		if (medium)
		{
			return medium;
		}
	}

	const IO::File file(url);
	const std::string fileExtension(String::toLower(file.extension()));

	for (Libraries::iterator i = libraries_.begin(); i != libraries_.end(); ++i)
	{
		ocean_assert(i->first);

		if (i->first->supports(type) && i->first->notSupported(fileExtension) == false)
		{
			const MediumRef medium(i->first->newMedium(url, type, useExclusive));

			if (medium)
			{
				return medium;
			}
		}
	}

	if (type == Medium::PIXEL_IMAGE)
	{
		PixelImage* pixelImage = new PixelImage(url);
		ocean_assert(pixelImage != nullptr);

		if (pixelImage->isValid())
		{
			if (useExclusive)
			{
				return MediumRef(pixelImage);
			}

			return MediumRefManager::get().registerMedium(pixelImage);
		}
		else
		{
			delete pixelImage;
		}
	}

	return MediumRef();
}

MediumRef Manager::newMedium(const std::string& url, const std::string& library, const Medium::Type type, bool useExclusive)
{
	ocean_assert(url.empty() == false && library.empty() == false);

	const ScopedLock scopedLock(lock_);

	if (useExclusive == false)
	{
		const MediumRef medium(MediumRefManager::get().medium(url, library, type));

		if (medium)
		{
			return medium;
		}
	}

	const IO::File file(url);
	const std::string fileExtension(String::toLower(file.extension()));

	for (Libraries::const_iterator i = libraries_.cbegin(); i != libraries_.cend(); ++i)
	{
		ocean_assert(i->first);

		if (i->first->name() == library && i->first->supports(type) && i->first->notSupported(fileExtension) == false)
		{
			return i->first->newMedium(url, type, useExclusive);
		}
	}

	return MediumRef();
}

RecorderRef Manager::newRecorder(const Recorder::Type type, const std::string& library)
{
	const ScopedLock scopedLock(lock_);

	for (Libraries::const_iterator i = libraries_.cbegin(); i != libraries_.cend(); ++i)
	{
		ocean_assert(i->first);

		if (!library.empty() && i->first->name() != library)
		{
			continue;
		}

		RecorderRef recorder(i->first->newRecorder(type));

		if (recorder)
		{
			return recorder;
		}
	}

	return RecorderRef();
}

Library::Definitions Manager::selectableMedia() const
{
	const ScopedLock scopedLock(lock_);

	Library::Definitions definitions;

	for (Libraries::const_iterator i = libraries_.cbegin(); i != libraries_.cend(); ++i)
	{
		ocean_assert(i->first);

		Library::Definitions libDefinitions(i->first->selectableMedia());
		definitions.insert(definitions.end(), libDefinitions.begin(), libDefinitions.end());
	}

	return definitions;
}

Library::Definitions Manager::selectableMedia(const Medium::Type type) const
{
	const ScopedLock scopedLock(lock_);

	Library::Definitions definitions;

	for (Libraries::const_iterator i = libraries_.cbegin(); i != libraries_.cend(); ++i)
	{
		ocean_assert(i->first);

		Library::Definitions libDefinitions(i->first->selectableMedia(type));

		definitions.insert(definitions.cend(), libDefinitions.cbegin(), libDefinitions.cend());
	}

	return definitions;
}

Manager::Names Manager::libraries() const
{
	const ScopedLock scopedLock(lock_);

	Names result;
	result.reserve(libraries_.size());

	for (Libraries::const_iterator i = libraries_.cbegin(); i != libraries_.cend(); ++i)
	{
		ocean_assert(i->first);

		result.emplace_back(i->first->name());
	}

	return result;
}

void Manager::release()
{
	const ScopedLock scopedLock(lock_);

	libraries_.clear();
}

bool Manager::unregisterLibrary(const std::string& name)
{
	const ScopedLock scopedLock(lock_);

	for (Libraries::iterator i = libraries_.begin(); i != libraries_.end(); ++i)
	{
		ocean_assert(i->first);

		if (i->first->name() == name)
		{
			ocean_assert(i->second >= 1u);
			i->second--;

			if (i->second == 0u)
			{
				libraries_.erase(i);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	ocean_assert(false && "Library unknown!");

	return false;
}

}

}
