/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/io/Path.h"

namespace Ocean
{

namespace IO
{

Path::Path(const std::string& value) :
	pathValue_(value)
{
	// nothing to do here
}

void Path::checkPath(const Type expectType)
{
	pathType_ = TYPE_INVALID;

#ifdef _WINDOWS

	if (pathValue_.length() > 2 && pathValue_[0] == SEPARATOR_BACKSLASH && pathValue_[1] == SEPARATOR_BACKSLASH)
	{
		// a network path value (\\*)
		pathType_ = Type(TYPE_NETWORK | TYPE_ABSOLUTE);
	}
	else if (pathValue_.length() > 2 && isalpha(pathValue_[0]) != 0 && pathValue_[1] == ':' && isSeparator(pathValue_[2]))
	{
		// an absolute path value (c:\*, d:\*, ...)
		pathType_ = TYPE_ABSOLUTE;
	}
	else if (pathValue_.length() > 0 && isSeparator(pathValue_[0]))
	{
		// a relative path value (\*)
		if (pathValue_.length() == 1 || (pathValue_.length() > 1 && !isSeparator(pathValue_[1])))
		{
			pathType_ = TYPE_RELATIVE;
			pathValue_ = pathValue_.substr(1);
		}
	}

#else

	if (pathValue_.length() > 2 && pathValue_[0] == SEPARATOR_SLASH)
	{
		// an absolute path value (/*)
		pathType_ = TYPE_ABSOLUTE;
	}
	else if (pathValue_.length() > 0 && pathValue_[0] != SEPARATOR_SLASH)
	{
		// a relative path value (*)
		pathType_ = TYPE_RELATIVE;
	}

#endif

	else if (pathValue_.length() > 1 && pathValue_[0] == '.' && isSeparator(pathValue_[1]))
	{
		// a relative path value (".\*)
		if (pathValue_.length() == 2 || (pathValue_.length() > 2 && !isSeparator(pathValue_[2])))
		{
			pathType_ = TYPE_RELATIVE;
			pathValue_ = pathValue_.substr(2);
		}
	}
	else if (pathValue_.length() > 2 && pathValue_[0] == '.' && pathValue_[1] == '.' && isSeparator(pathValue_[2]))
	{
		// a relative path value (..\*);
		if (pathValue_.length() == 3 || (pathValue_.length() > 3 && !isSeparator(pathValue_[3])))
		{
			pathType_ = TYPE_RELATIVE;
		}
	}
	else if (!pathValue_.empty() && !isSeparator(pathValue_[0]))
	{
		pathType_ = TYPE_RELATIVE;
	}

	if (pathType_ != TYPE_INVALID)
	{
		ocean_assert(!pathValue_.empty());

		// check for a directory ("*/)

		if (isSeparator(pathValue_[pathValue_.length() - 1]))
		{
			if (expectType & TYPE_FILE)
			{
				pathType_ = TYPE_INVALID;
			}
			else
			{
				pathType_ = Type(pathType_ | TYPE_DIRECTORY);
			}
		}
		else
		{
			if (expectType & TYPE_FILE)
			{
				pathType_ = Type(pathType_ | TYPE_FILE);
			}
			else
			{
				pathType_ = Type(pathType_ | TYPE_DIRECTORY);
				pathValue_ += defaultSeparator();
			}
		}

		trim();
	}
}

void Path::trim()
{
	// **TODO** Missing implementation

	return;
#if 0
	if (pathType_ == TYPE_INVALID)
	{
		return;
	}

	std::string::size_type pos = 1;

	while ((pos = pathValue_.find("..")) != std::string::npos)
	{
		if (pos + 2 >= pathValue_.length())
		{
			pathType_ = TYPE_INVALID;
			return;
		}

		if (isSeparator(pathValue_[pos - 1]) && isSeparator(pathValue_[pos + 2]) && pos > 1)
		{
			std::string::size_type leftPos = std::string::npos;

			for (std::string::size_type n = pos - 2; n < pos; --n)
			{
				if (isSeparator(pathValue_[n]))
				{
					leftPos = n;
					break;
				}
			}

			if (pos - leftPos == 2)
			{
				pathType_ = TYPE_INVALID;
				break;
			}

			pathValue_ = pathValue_.substr(0, leftPos) + pathValue_.substr(pos + 2);
			pos = leftPos + 1;
		}
		else
		{
			pathType_ = TYPE_INVALID;
			return;
		}
	}
#endif
}

bool Path::isSeparator(const char character)
{
#if defined(_WINDOWS)

	return character == SEPARATOR_BACKSLASH || character == SEPARATOR_SLASH;

#elif defined(_ANDROID) || defined(__APPLE__) || defined(__linux__) || defined(__EMSCRIPTEN__)

	return character == SEPARATOR_SLASH;

#else

	#error Check the implementation for this platform.
	return character == SEPARATOR_BACKSLASH || character == SEPARATOR_SLASH;

#endif
}

char Path::defaultSeparator()
{
#if defined(_WINDOWS)

	return '\\';

#elif defined(_ANDROID) || defined(__APPLE__) || defined(__linux__) || defined(__EMSCRIPTEN__)

	return '/';

#else

	#error Check the implementation for this platform.
	return '/';

#endif
}

bool Path::operator==(const Path& right) const
{
	if (type() != right.type())
	{
		return false;
	}

	if (pathValue_.length() != right.pathValue_.length())
	{
		return false;
	}

#if defined(_WINDOWS)

	// on windows platform paths are not case sensitive
	bool caseSensitive = false;

#elif defined(_ANDROID) || defined(__APPLE__) || defined(__linux__) || defined(__EMSCRIPTEN__)

	// on linux platform paths are case sensitive
	bool caseSensitive = true;

#else

	#error Check the implementation for this platform.

#endif


	std::string::const_iterator iR = right.pathValue_.begin();
	for (std::string::const_iterator iL = pathValue_.begin(); iL != pathValue_.end(); ++iL)
	{
		ocean_assert(iR != right.pathValue_.end());

		const bool ls = isSeparator(*iL);
		const bool rs = isSeparator(*iR);

		if (ls != rs || (!ls && !rs && ((caseSensitive && *iL != *iR) || (!caseSensitive && tolower(*iL) != tolower(*iR)))))
		{
			return false;
		}
	}

	return true;
}

} // namespace IO

} // namespace Ocean
