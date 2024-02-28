// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/base/Exception.h"

namespace Ocean
{

OceanException::OceanException(OceanException&& exception) :
	message_(exception.message_)
{
	exception.message_ = nullptr;
}

OceanException::OceanException(const char* message) :
	message_(nullptr)
{
	const size_t size = message ? strlen(message) : 0;

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

OceanException::OceanException(const std::string& message) :
	message_(nullptr)
{
	const size_t size = message.length();

	if (size != 0)
	{
		message_ = (char*)(malloc(size + 1));

		if (message_ != nullptr)
		{
			memcpy(message_, message.c_str(), size);
			message_[size] = '\0';
		}
	}
}

OceanException::~OceanException()
{
	free(message_);
}

const char* OceanException::what() const noexcept
{
	return message_;
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
