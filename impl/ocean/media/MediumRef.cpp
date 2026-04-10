/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/MediumRef.h"

namespace Ocean
{

namespace Media
{

MediumRefManager::~MediumRefManager()
{
	ocean_assert(mediumMap_.empty());
}

MediumRef MediumRefManager::registerMedium(Medium* medium)
{
	ocean_assert(medium != nullptr);

	const ScopedLock scopedLock(lock_);

	MediumRef mediumRef(medium, MediumRef::ReleaseCallback(MediumRefManager::get(), &MediumRefManager::unregisterMedium));

	mediumMap_.insert(std::make_pair(medium->url(), mediumRef));
	return mediumRef;
}

MediumRef MediumRefManager::medium(const std::string& url)
{
	ocean_assert(url.empty() == false);

	const ScopedLock scopedLock(lock_);

	MediumMap::const_iterator i = mediumMap_.find(url);
	if (i != mediumMap_.end())
		return i->second;

	return MediumRef();
}

MediumRef MediumRefManager::medium(const std::string& url, const Medium::Type type)
{
	ocean_assert(url.empty() == false);

	const ScopedLock scopedLock(lock_);

	MediumMap::const_iterator i = mediumMap_.lower_bound(url);
	MediumMap::const_iterator end = mediumMap_.upper_bound(url);

	while (i != end)
		if (i->second->isType(type))
			return i->second;
		else
			++i;

	return MediumRef();
}

MediumRef MediumRefManager::medium(const std::string& url, const std::string& library, const Medium::Type type)
{
	ocean_assert(url.empty() == false && library.empty() == false);

	const ScopedLock scopedLock(lock_);

	MediumMap::const_iterator i = mediumMap_.lower_bound(url);
	MediumMap::const_iterator end = mediumMap_.upper_bound(url);

	while (i != end)
		if (i->second->isType(type) && i->second->library() == library)
			return i->second;
		else
			++i;

	return MediumRef();
}

bool MediumRefManager::isRegistered(const Medium* medium)
{
	const ScopedLock scopedLock(lock_);

	for (MediumMap::iterator i = mediumMap_.begin(); i != mediumMap_.end(); ++i)
		if (&*i->second == medium)
			return true;

	return false;
}

void MediumRefManager::unregisterMedium(const Medium* medium)
{
	const ScopedLock scopedLock(lock_);

	for (MediumMap::iterator i = mediumMap_.begin(); i != mediumMap_.end(); ++i)
		if (&*i->second == medium)
		{
			mediumMap_.erase(i);
			break;
		}
}

}

}
