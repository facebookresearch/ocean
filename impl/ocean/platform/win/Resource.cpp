/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/win/Resource.h"

#include "ocean/base/String.h"

#include "ocean/media/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

bool Resource::extractResource(const HMODULE module, const unsigned int resourceId, const std::wstring& type, Buffer& buffer)
{
	HRSRC resourceInformationHandle = FindResourceW(module, MAKEINTRESOURCEW(resourceId), type.c_str());

	if (resourceInformationHandle == nullptr)
	{
		return false;
	}

	buffer.clear();

	const DWORD resourceSize = SizeofResource(module, resourceInformationHandle);
	if (resourceSize == 0)
	{
		return true;
	}

	const HGLOBAL resourceHandle = LoadResource(module, resourceInformationHandle);

	if (resourceHandle == nullptr)
	{
		return false;
	}

	const void* resourceData = LockResource(resourceHandle);

	buffer.resize(resourceSize);
	memcpy(buffer.data(), resourceData, resourceSize);

	return true;
}

FrameRef ResourceManager::frame(const unsigned int resourceId, const std::wstring& type, const HMODULE module)
{
	const ScopedLock scopedLock(lock_);

	FrameMap::const_iterator i = frameMap_.find(resourceId);
	if (i != frameMap_.end())
	{
		return i->second;
	}

	Resource::Buffer buffer;
	if (Resource::extractResource(module, resourceId, type, buffer))
	{
		Frame frame = Media::Utilities::loadImage(buffer.data(), buffer.size(), String::toAString(type));

		if (frame.isValid())
		{
			i = frameMap_.insert(std::make_pair(resourceId, FrameRef(new Frame(std::move(frame))))).first;
		}
	}

	if (i == frameMap_.end())
	{
		return FrameRef();
	}

	return i->second;
}

void ResourceManager::release()
{
	const ScopedLock scopedLock(lock_);

	frameMap_.clear();
}

}

}

}
