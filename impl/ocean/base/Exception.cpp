/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Exception.h"

namespace Ocean
{

OceanException::OceanException(OceanException&& exception) noexcept
{
	*this = std::move(exception);
}

OceanException::OceanException(const OceanException& exception) noexcept
{
	*this = exception;
}

OceanException::OceanException(const char* message)
{
	if (message != nullptr)
	{
		setMessage(message);
	}
}

OceanException::OceanException(const std::string& message) :
	OceanException(message.c_str())
{
	// nothing to do here
}

OceanException::~OceanException()
{
	release();
}

const char* OceanException::what() const noexcept
{
	return message_;
}

OceanException& OceanException::operator=(OceanException&& exception) noexcept
{
	if (this != &exception)
	{
		release();

		message_ = exception.message_;
		exception.message_ = nullptr;
	}

	return *this;
}

OceanException& OceanException::operator=(const OceanException& exception) noexcept
{
	if (this != &exception)
	{
		release();

		if (exception.what() != nullptr)
		{
			setMessage(exception.what());
		}
	}

	return *this;
}

void OceanException::setMessage(const char* message)
{
	ocean_assert(message != nullptr);
	ocean_assert(message_ == nullptr);

	const size_t size = strlen(message);

	if (size != 0)
	{
		message_ = (char*)(malloc(size + 1));

		if (message_ != nullptr)
		{
			memcpy(message_, message, size);
			message_[size] = '\0';
		}
	}
}

void OceanException::release()
{
	free(message_);
}

NotImplementedException::NotImplementedException() :
	OceanException("Not implemented!")
{
	// nothing to do here
}

NotImplementedException::NotImplementedException(const char* message) :
	OceanException(message)
{
	// nothing to do here
}

NotImplementedException::NotImplementedException(const std::string& message) :
	OceanException(message.c_str())
{
	// nothing to do here
}

MissingImplementationException::MissingImplementationException() :
	OceanException("Missing implementation!")
{
	// nothing to do here
}

MissingImplementationException::MissingImplementationException(const char* message) :
	OceanException(message)
{
	// nothing to do here
}

MissingImplementationException::MissingImplementationException(const std::string& message) :
	OceanException(message.c_str())
{
	// nothing to do here
}

NotSupportedException::NotSupportedException() :
	OceanException("Not supported!")
{
	// nothing to do here
}

NotSupportedException::NotSupportedException(const char* message) :
	OceanException(message)
{
	// nothing to do here
}

NotSupportedException::NotSupportedException(const std::string& message) :
	OceanException(message)
{
	// nothing to do here
}

OutOfMemoryException::OutOfMemoryException() :
	OceanException("Out of memory!")
{
	// nothing to do here
}

OutOfMemoryException::OutOfMemoryException(const char* message) :
	OceanException(message)
{
	// nothing to do here
}

OutOfMemoryException::OutOfMemoryException(const std::string& message) :
	OceanException(message)
{
	// nothing to do here
}

}
