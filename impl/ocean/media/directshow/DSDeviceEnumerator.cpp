/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/media/directshow/DSDeviceEnumerator.h"

#include "ocean/base/String.h"

DISABLE_WARNINGS_BEGIN
	#include <Strmif.h>
DISABLE_WARNINGS_END

namespace Ocean
{

namespace Media
{

namespace DirectShow
{

DSDeviceEnumerator::DSDeviceEnumerator(const GUID& identifier) :
	classIdentifier_(identifier)
{
	refresh();
}

DSDeviceEnumerator::~DSDeviceEnumerator()
{
	release();
}

bool DSDeviceEnumerator::refresh()
{
	if (classIdentifier_ == GUID_NULL)
	{
		return false;
	}

	release();

	ScopedICreateDevEnum systemDeviceEnumerator;
	if (S_OK != CoCreateInstance(CLSID_SystemDeviceEnum, nullptr, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void**)(&systemDeviceEnumerator.resetObject())))
	{
		return false;
	}

	ScopedIEnumMoniker deviceEnumerator;
	if (S_OK != systemDeviceEnumerator->CreateClassEnumerator(classIdentifier_, &deviceEnumerator.resetObject(), 0))
	{
		return false;
	}

	ScopedIMoniker moniker;
	ULONG fetched = 0;

	while (deviceEnumerator->Next(1, &moniker.resetObject(), &fetched) == S_OK)
	{
		ScopedIPropertyBag propertyBag;
		if (S_OK == moniker->BindToStorage(0, 0, IID_IPropertyBag, (void**)(&propertyBag.resetObject())))
		{
			VARIANT friendlyName, description, devicePath;

			VariantInit(&friendlyName);
			VariantInit(&description);
			VariantInit(&devicePath);

			Object object;
			object.moniker_ = std::move(moniker);

			if (S_OK == propertyBag->Read(L"FriendlyName", &friendlyName, nullptr))
			{
				std::string name(String::toAString(friendlyName.bstrVal));

				if (name.length() > 2 && name[0] == '\"' && name[name.length() - 1] == '\"')
				{
					name = name.substr(1, name.length() - 2);
				}

				object.friendlyName_ = name;
			}

			if (S_OK == propertyBag->Read(L"Description", &description, nullptr))
			{
				object.deviceDescription_ = String::toAString(description.bstrVal);
			}

			if (S_OK == propertyBag->Read(L"DevicePath", &devicePath, nullptr))
			{
				object.deviceName_ = String::toAString(devicePath.bstrVal);
			}

			VariantClear(&friendlyName);
			VariantClear(&description);
			VariantClear(&devicePath);

			if (exist(object.friendlyName_))
			{
				unsigned int deviceCounter = 1;

				while (++deviceCounter < 100)
				{
					std::string newName = object.friendlyName_ + std::string(" (") + String::toAString(deviceCounter) + std::string(")");

					if (exist(newName) == false)
					{
						object.friendlyName_ = newName;
						break;
					}
				}
			}

			objects_.emplace_back(std::move(object));
		}
	}

	return true;
}

bool DSDeviceEnumerator::exist(const std::string& name) const
{
	for (const Object& object : objects_)
	{
		if (object.friendlyName_ == name)
		{
			return true;
		}
	}

	return false;
}

ScopedIMoniker DSDeviceEnumerator::moniker(const std::string& name) const
{
	for (const Object& object : objects_)
	{
		if (object.friendlyName_ == name)
		{
			IMoniker* copyMoniker = *object.moniker_;
			copyMoniker->AddRef();

			return ScopedIMoniker(copyMoniker);
		}
	}

	return ScopedIMoniker();
}

DSDeviceEnumerator::Names DSDeviceEnumerator::names() const
{
	Names names;
	names.reserve(objects_.size());

	for (const Object& object : objects_)
	{
		names.emplace_back(object.friendlyName_);
	}

	sort(names.begin(), names.end());

	return names;
}

void DSDeviceEnumerator::release()
{
	objects_.clear();
}

const GUID& DSDeviceEnumerator::identifier() const
{
	return classIdentifier_;
}

DSEnumerators::~DSEnumerators()
{
	ocean_assert(enumerators_.empty());

	release();
}

DSDeviceEnumerator& DSEnumerators::enumerator(const GUID& identifier)
{
	const ScopedLock scopedLock(lock_);

	for (const std::shared_ptr<DSDeviceEnumerator>& enumetrator : enumerators_)
	{
		ocean_assert(enumetrator != nullptr);

		if (identifier == enumetrator->identifier())
		{
			return *enumetrator;
		}
	}

	enumerators_.emplace_back(new DSDeviceEnumerator(identifier));

	return *enumerators_.back();
}

void DSEnumerators::release()
{
	const ScopedLock scopedLock(lock_);

	enumerators_.clear();
}

}

}

}
