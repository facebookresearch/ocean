/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/StringApple.h"

#import <Foundation/Foundation.h>

#ifndef __APPLE__
    #error This files contains Apple specific functions only
#endif

namespace Ocean
{

NSString* StringApple::toNSString(const std::string& value)
{
	if (value.empty())
	{
		return [[NSString alloc] init];
	}

	return [[NSString alloc] initWithCString:value.c_str() encoding:NSASCIIStringEncoding];
}

NSString* StringApple::toNSString(const std::wstring& value)
{
	if (value.empty())
	{
		return [[NSString alloc] init];
	}

	static_assert(sizeof(wchar_t) == 4, "Invalid data type!");

	return [[NSString alloc] initWithBytes:(const void*)value.c_str() length:sizeof(wchar_t) * value.length() encoding:NSUTF32LittleEndianStringEncoding];
}

std::string StringApple::toUTF8(NSString* object)
{
	if (object == nullptr)
	{
		return std::string();
	}

	const NSUInteger stringLength = [object length];

	const char* value = [object UTF8String];

	ocean_assert(value != nullptr || stringLength == 0u);
	if (value == nullptr)
	{
		return std::string();
	}

	return std::string(value, stringLength);
}

std::wstring StringApple::toWString(NSString* object)
{
	if (object == nullptr)
	{
		return std::wstring();
	}

	static_assert(sizeof(wchar_t) == 4, "Invalid data type!");

	const NSUInteger stringLength = [object length];

	const wchar_t* value = (wchar_t*)[object cStringUsingEncoding:NSUTF32LittleEndianStringEncoding];

	ocean_assert(value != nullptr);
	if (value == nullptr || stringLength == 0u)
	{
		return std::wstring();
	}

	return std::wstring(value, stringLength);
}

std::string StringApple::toUTF8(CFStringRef object)
{
	return toUTF8((__bridge NSString*)object);
}

std::wstring StringApple::toWString(CFStringRef object)
{
	return toWString((__bridge NSString*)object);
}

} // namespace Ocean
