/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_STRING_APPLE_H
#define META_OCEAN_BASE_STRING_APPLE_H

#include "ocean/base/Base.h"
#include "ocean/base/String.h"

#ifndef __APPLE__
	#error This library is available on Apple platforms only!
#endif

#include <CoreFoundation/CoreFoundation.h>

#ifdef __OBJC__
	#include <Foundation/Foundation.h>
#endif

namespace Ocean
{

/**
 * This class provides function allowing to convert Apple-platform specific string objects.
 * Beware: This class is implemented for Apple-platforms only and therefore, it is recommended to use this class in non-header files only to reduce issues with the Objective C compiler.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT StringApple
{
	public:

#ifdef __OBJC__

		/**
		 * Converts a given string to a NSString object.
		 * @param value The string to convert, may be empty
		 * @return The resulting converted string object
		 */
		static NSString* toNSString(const std::string& value);

		/**
		 * Converts a given string to a NSString object.
		 * @param value The string to convert, may be empty
		 * @return The resulting converted string object
		 */
		static NSString* toNSString(const std::wstring& value);

		/**
		 * Converts a given NSString object to a string with UTF8 encoding.
		 * @param object The string object to convert, may be nullptr
		 * @return The resulting string with UTF8 encoding
		 */
		static std::string toUTF8(NSString* object);

		/**
		 * Converts a given NSString object to a string with 32 bit character encoding.
		 * @param object The string object to convert, may be nullptr
		 * @return The resulting string with 32 bit character encoding
		 */
		static std::wstring toWString(NSString* object);

#endif

		/**
		 * Converts a given CFStringRef object to a string with UTF8 encoding.
		 * @param object The string object to convert, may be nullptr
		 * @return The resulting string with UTF8 encoding
		 */
		static std::string toUTF8(CFStringRef object);

		/**
		 * Converts a given CFStringRef object to a string with 32 bit character encoding.
		 * @param object The string object to convert, may be nullptr
		 * @return The resulting string with 32 bit character encoding
		 */
		static std::wstring toWString(CFStringRef object);
};

}

#endif // META_OCEAN_BASE_STRING_APPLE_H
