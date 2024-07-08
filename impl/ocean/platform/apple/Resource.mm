/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/Resource.h"
#include "ocean/platform/apple/Utilities.h"

#include "ocean/base/StringApple.h"

#include "ocean/media/Utilities.h"

#include <CoreFoundation/CoreFoundation.h>

#include <CoreGraphics/CoreGraphics.h>

#include <Foundation/Foundation.h>

#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE==1
	#include <UIKit/UIKit.h>
#else
	#include <AppKit/AppKit.h>
#endif

namespace Ocean
{

namespace Platform
{

namespace Apple
{

bool Resource::extractResource(const std::wstring& bundle, const std::wstring& name, const std::wstring& type, Buffer& buffer)
{
	CFBundleRef bundleObject = nullptr; // does not need to be released

	if (bundle.empty())
	{
		bundleObject = CFBundleGetMainBundle();
	}
	else
	{
		const std::string aBundle = String::toAString(bundle);

		const ScopedCFStringRef resourceBundle(CFStringCreateWithCString(nullptr, aBundle.c_str(), kCFStringEncodingUTF8));
		ocean_assert(resourceBundle.object() != nullptr);

		bundleObject = CFBundleGetBundleWithIdentifier(resourceBundle.object());
	}

	if (bundleObject == nullptr)
	{
		return false;
	}

	const std::string aName = String::toAString(name);
	const std::string aType = String::toAString(type);

	const ScopedCFStringRef resourceName(CFStringCreateWithCString(nullptr, aName.c_str(), kCFStringEncodingUTF8));
	ocean_assert(resourceName.object() != nullptr);

	const ScopedCFStringRef resourceType(CFStringCreateWithCString(nullptr, aType.c_str(), kCFStringEncodingUTF8));
	ocean_assert(resourceType.object() != nullptr);

	const ScopedCFURLRef url(CFBundleCopyResourceURL(bundleObject, resourceName.object(), resourceType.object(), nullptr));

	if (url.object() == nullptr)
	{
		return false;
	}

	// Beware: we must not release the data provider here CGDataProviderRelease(provider), this would cause segmetation faults!
	// Instead, we release the data object instead.
	CGDataProviderRef dataProvider = CGDataProviderCreateWithURL(url.object());

	if (dataProvider == nullptr)
	{
		return false;
	}

	const ScopedCFDataRef data(CGDataProviderCopyData(dataProvider));

	if (data.object() == nullptr)
	{
		return false;
	}

	const signed long dataLength = CFDataGetLength(data.object());
	const uint8_t* dataBuffer = CFDataGetBytePtr(data.object());

	if (dataLength < 0)
	{
		return false;
	}

	buffer.resize(size_t(dataLength));
	memcpy(buffer.data(), dataBuffer, dataLength);

	return true;
}

std::wstring Resource::resourcePath(const std::wstring& name, const std::wstring& type, const std::wstring& bundle)
{
	NSBundle* bundleObject = bundle.empty() ? [NSBundle mainBundle] : [NSBundle bundleWithIdentifier:StringApple::toNSString(bundle)];

	return StringApple::toWString([bundleObject pathForResource:StringApple::toNSString(name) ofType:StringApple::toNSString(type)]);
}

std::vector<std::wstring> Resource::resourcePaths(const std::wstring& type, const std::wstring& directory, const std::wstring& bundle)
{
	NSBundle* bundleObject = bundle.empty() ? [NSBundle mainBundle] : [NSBundle bundleWithIdentifier:StringApple::toNSString(bundle)];

	NSArray<NSString*>* nsPaths = [bundleObject pathsForResourcesOfType:StringApple::toNSString(type) inDirectory:StringApple::toNSString(directory)];

	std::vector<std::wstring> paths;
	paths.reserve([nsPaths count]);

	for (NSUInteger n = 0u; n < [nsPaths count]; ++n)
	{
		paths.emplace_back(StringApple::toWString([nsPaths objectAtIndex:n]));
	}

	return paths;
}

FrameRef ResourceManager::frame(const std::wstring& name, const std::wstring& type, const std::wstring& bundle)
{
	const ScopedLock scopedLock(lock_);

	FrameMap::iterator iFrame = frames_.find(ResourcePair(name, type));
	if (iFrame != frames_.end())
	{
		return iFrame->second;
	}

	Resource::Buffer buffer;
	if (Resource::extractResource(bundle, name, type, buffer))
	{
		Frame frame = Media::Utilities::loadImage(buffer.data(), buffer.size(), String::toAString(type));

		if (frame.isValid())
		{
			iFrame = frames_.insert(std::make_pair(ResourcePair(name, type), FrameRef(new Frame(std::move(frame))))).first;
		}
	}

	if (iFrame == frames_.end())
	{
		return FrameRef();
	}

	return iFrame->second;
}

void ResourceManager::release()
{
	const ScopedLock scopedLock(lock_);

	frames_.clear();
}

}

}

}
