// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_BASE_EXCEPTION_H
#define META_OCEAN_BASE_EXCEPTION_H

#include "ocean/base/Base.h"

#include <exception>

namespace Ocean
{

/**
 * Definition of the base exception.
 * @ingroup base
 */
typedef std::exception Exception;

/**
 * Definition of a base exception for the entire Ocean framework.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT OceanException : public Exception
{
	public:

		/**
		 * Creates a new OceanException object without any message.
		 */
		OceanException() = default;

		/**
		 * Move constructor.
		 * @param exception The exception to be moved
		 */
		OceanException(OceanException&& exception);

		/**
		 * Creates a new OceanException object with a message.
		 * @param message Exception message
		 */
		explicit OceanException(const char* message);

		/**
		 * Creates a new OceanException object with a message.
		 * @param message Exception message
		 */
		explicit OceanException(const std::string& message);

		/**
		 * Destructs an exception object.
		 */
		~OceanException() override;

		/**
		 * Returns the exception message.
		 * @return Exception message
		 */
		const char* what() const noexcept override;

	protected:

		/**
		 * Disabled copy constructor.
		 * @param exception The exception which would be copied
		 */
		OceanException(const OceanException& exception) = delete;

	private:

		/// Exception message.
		char* message_ = nullptr;
};

/**
 * Definition of a base exception for not implemented code.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT NotImplementedException : public OceanException
{
	public:

		/**
		 * Creates a new exception for not implemented code without any message.
		 */
		NotImplementedException();

		/**
		 * Creates a new exception for not implemented code with a message.
		 * @param message Exception message
		 */
		explicit NotImplementedException(const char* message);

		/**
		 * Creates a new exception for not implemented code with a message.
		 * @param message Exception message
		 */
		explicit NotImplementedException(const std::string& message);
};

/**
 * Definition of a base exception for missing implementations.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT MissingImplementationException : public OceanException
{
	public:

		/**
		 * Creates a new exception for missing implementations.
		 */
		MissingImplementationException();

		/**
		 * Creates a new exception for missing implementations.
		 * @param message Exception message
		 */
		explicit MissingImplementationException(const char* message);

		/**
		 * Creates a new exception for missing implementations.
		 * @param message Exception message
		 */
		explicit MissingImplementationException(const std::string& message);
};

/**
 * Definition of a base exception for not supported functionalities.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT NotSupportedException : public OceanException
{
	public:

		/**
		 * Creates a new exception for not supported functionalities without any message.
		 */
		NotSupportedException();

		/**
		 * Creates a new exception for not supported functionalities with a message.
		 * @param message Exception message
		 */
		explicit NotSupportedException(const char* message);

		/**
		 * Creates a new exception for not supported functionalities with a message.
		 * @param message Exception message
		 */
		explicit NotSupportedException(const std::string& message);
};

/**
 * Definition of a base exception if an out of memory problem occurs.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT OutOfMemoryException : public OceanException
{
	public:

		/**
		 * Creates a new exception if not enough memory is available without any message.
		 */
		OutOfMemoryException();

		/**
		 * Creates a new exception if not enough memory is available with a message.
		 * @param message Exception message
		 */
		explicit OutOfMemoryException(const char* message);

		/**
		 * Creates a new exception if not enough memory is available with a message.
		 * @param message Exception message
		 */
		explicit OutOfMemoryException(const std::string& message);
};

}

#endif // META_OCEAN_BASE_EXCEPTION_H
