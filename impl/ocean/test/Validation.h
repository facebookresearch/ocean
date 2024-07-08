/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_VALIDATION_H
#define META_OCEAN_TEST_VALIDATION_H

#include "ocean/test/Test.h"

#include "ocean/base/RandomGenerator.h"

namespace Ocean
{

namespace Test
{

/**
 * This class implements a helper class to validate tests.
 *
 * The following example shows how to use Validation and calling the functions directly.<br>
 * In case of an error, no further information will be provided:
 * @code
 * bool testFunction()
 * {
 *     Log::info() << "Running a test ...";
 *
 *     Validation validation;
 *
 *     validation.expectTrue(4 + 4 == 8);
 *
 *     validation.expectFalse(4 + 4 == 7);
 *
 *     validation.expectEqual(4 + 4, 8);
 *
 *     if (4 + 4 == 7)
 *     {
 *        validation.setFailed();
 *     }
 *
 *     Log::info() << "Validation: " << validation;
 *
 *     return validation.succeeded();
 * }
 * @endcode
 *
 * The following example shows how to use Validation and calling the functions directly but also providing __FILE__ and __LINE__ macro parameters.<br>
 * In case of an error, additional in information about the location will be provided:
 * @code
 * bool testFunction()
 * {
 *     Log::info() << "Running a test ...";
 *
 *     Validation validation;
 *
 *     validation.expectTrue(4 + 4 == 8, __FILE__, __LINE__);
 *
 *     validation.expectFalse(4 + 4 == 7, __FILE__, __LINE__);
 *
 *     validation.expectEqual(4 + 4, 8, __FILE__, __LINE__);
 *
 *     if (4 + 4 == 7)
 *     {
 *        validation.setFailed(__FILE__, __LINE__);
 *     }
 *
 *     Log::info() << "Validation: " << validation;
 *
 *     return validation.succeeded();
 * }
 * @endcode
 *
 * The following example shows how to use Validation while not calling the object's functions directly but using the corresponding macros.<br>
 * In case of an error, additional in information about the location will be provided:
 * @code
 * bool testFunction()
 * {
 *     Log::info() << "Running a test ...";
 *
 *     Validation validation;
 *
 *     OCEAN_EXPECT_TRUE(validation, 4 + 4 == 8);
 *
 *     OCEAN_EXPECT_FALSE(validation, 4 + 4 == 7);
 *
 *     OCEAN_EXPECT_EQUAL(validation, 4 + 4, 8);
 *
 *     if (4 + 4 == 7)
 *     {
 *        OCEAN_SET_FAILED(validation);
 *     }
 *
 *     Log::info() << "Validation: " << validation;
 *
 *     return validation.succeeded();
 * }
 * @endcode
 * @see PrecisionValidation.
 * @ingroup test
 */
class Validation
{
	public:

		/**
		 * Default constructor, by default the verified has succeeded.
		 */
		Validation() = default;

		/**
		 * Creates a new validation object associated with a random generator, by default the verified has succeeded.
		 * @param randomGenerator The random generator which will be used during verification
		 */
		explicit inline Validation(RandomGenerator& randomGenerator);

		/**
		 * Destructs this validation object.
		 */
		inline ~Validation();

		/**
		 * Informs this validation object that a value is expected to be True.
		 * In case the value is False, this validation object will not succeed.
		 * @param value The value to be expected True
		 * @see succeeded().
		 */
		inline void expectTrue(const bool value);

		/**
		 * Informs this validation object that a value is expected to be True.
		 * In case the value is False, this validation object will not succeed.<br>
		 * This function will also write a message to the error log.
		 * @param value The value to be expected True
		 * @param file The source file in which the function call happens, e.g., __FILE__, must be valid
		 * @param line The line in the source file in which the function call happens, e.g., __LINE__, must be valid
		 * @see succeeded().
		 */
		inline void expectTrue(const bool value, const char* file, const int line);

		/**
		 * Informs this validation object that a value is expected to be False.
		 * In case the value is True, this validation object will not succeed.
		 * @param value The value to be expected False
		 * @see succeeded().
		 */
		inline void expectFalse(const bool value);

		/**
		 * Informs this validation object that a value is expected to be False.
		 * In case the value is True, this validation object will not succeed.<br>
		 * This function will also write a message to the error log.
		 * @param value The value to be expected False
		 * @param file The source file in which the function call happens, e.g., __FILE__, must be valid
		 * @param line The line in the source file in which the function call happens, e.g., __LINE__, must be valid
		 * @see succeeded().
		 */
		inline void expectFalse(const bool value, const char* file, const int line);

		/**
		 * Informs this validation object that a value is expected to be equal to another value.
		 * In case the both values are not identical, this validation object will not succeed.
		 * @param value0 The first value to compare
		 * @param value1 The second value to compare
		 * @see succeeded().
		 * @tparam T The data type of both values
		 */
		template <typename T>
		inline void expectEqual(const T& value0, const T& value1);

		/**
		 * Informs this validation object that a value is expected to be equal to another value.
		 * In case the both values are not identical, this validation object will not succeed.<br>
		 * This function will also write a message to the error log.
		 * @param value0 The first value to compare
		 * @param value1 The second value to compare
		 * @param file The source file in which the function call happens, e.g., __FILE__, must be valid
		 * @param line The line in the source file in which the function call happens, e.g., __LINE__, must be valid
		 * @see succeeded().
		 * @tparam T The data type of both values
		 */
		template <typename T>
		inline void expectEqual(const T& value0, const T& value1, const char* file, const int line);

		/**
		 * Informs this validation object that a value is expected to be less than another value.
		 * In case 'value0 < value1' is false, the validation object will not succeed.
		 * @param value0 The first value to compare
		 * @param value1 The second value to compare
		 * @see succeeded().
		 * @tparam T The data type of both values
		 */
		template <typename T>
		inline void expectLess(const T& value0, const T& value1);

		/**
		 * Informs this validation object that a value is expected to be less than another value.
		 * In case 'value0 < value1' is false, the validation object will not succeed.<br>
		 * This function will also write a message to the error log.
		 * @param value0 The first value to compare
		 * @param value1 The second value to compare
		 * @param file The source file in which the function call happens, e.g., __FILE__, must be valid
		 * @param line The line in the source file in which the function call happens, e.g., __LINE__, must be valid
		 * @see succeeded().
		 * @tparam T The data type of both values
		 */
		template <typename T>
		inline void expectLess(const T& value0, const T& value1, const char* file, const int line);

		/**
		 * Informs this validation object that a value is expected to be less than or equal to another value.
		 * In case 'value0 <= value1' is false, the validation object will not succeed.
		 * @param value0 The first value to compare
		 * @param value1 The second value to compare
		 * @see succeeded().
		 * @tparam T The data type of both values
		 */
		template <typename T>
		inline void expectLessEqual(const T& value0, const T& value1);

		/**
		 * Informs this validation object that a value is expected to be less than or equal to another value.
		 * In case 'value0 <= value1' is false, the validation object will not succeed.<br>
		 * This function will also write a message to the error log.
		 * @param value0 The first value to compare
		 * @param value1 The second value to compare
		 * @param file The source file in which the function call happens, e.g., __FILE__, must be valid
		 * @param line The line in the source file in which the function call happens, e.g., __LINE__, must be valid
		 * @see succeeded().
		 * @tparam T The data type of both values
		 */
		template <typename T>
		inline void expectLessEqual(const T& value0, const T& value1, const char* file, const int line);

		/**
		 * Informs this validation object that a value is expected to be greater than another value.
		 * In case 'value0 > value1' is false, the validation object will not succeed.
		 * @param value0 The first value to compare
		 * @param value1 The second value to compare
		 * @see succeeded().
		 * @tparam T The data type of both values
		 */
		template <typename T>
		inline void expectGreater(const T& value0, const T& value1);

		/**
		 * Informs this validation object that a value is expected to be greater than another value.
		 * In case 'value0 > value1' is false, the validation object will not succeed.<br>
		 * This function will also write a message to the error log.
		 * @param value0 The first value to compare
		 * @param value1 The second value to compare
		 * @param file The source file in which the function call happens, e.g., __FILE__, must be valid
		 * @param line The line in the source file in which the function call happens, e.g., __LINE__, must be valid
		 * @see succeeded().
		 * @tparam T The data type of both values
		 */
		template <typename T>
		inline void expectGreater(const T& value0, const T& value1, const char* file, const int line);

		/**
		 * Informs this validation object that a value is expected to be greater than or equal to another value.
		 * In case 'value0 >= value1' is false, the validation object will not succeed.
		 * @param value0 The first value to compare
		 * @param value1 The second value to compare
		 * @see succeeded().
		 * @tparam T The data type of both values
		 */
		template <typename T>
		inline void expectGreaterEqual(const T& value0, const T& value1);

		/**
		 * Informs this validation object that a value is expected to be greater than or equal to another value.
		 * In case 'value0 >= value1' is false, the validation object will not succeed.<br>
		 * This function will also write a message to the error log.
		 * @param value0 The first value to compare
		 * @param value1 The second value to compare
		 * @param file The source file in which the function call happens, e.g., __FILE__, must be valid
		 * @param line The line in the source file in which the function call happens, e.g., __LINE__, must be valid
		 * @see succeeded().
		 * @tparam T The data type of both values
		 */
		template <typename T>
		inline void expectGreaterEqual(const T& value0, const T& value1, const char* file, const int line);

		/**
		 * Explicitly sets the validation to be failed.
		 * @see succeeded().
		 */
		inline void setFailed();

		/**
		 * Explicitly sets the validation to be failed.
		 * This function will also write a message to the error log.
		 * @param file The source file in which the function call happens, e.g., __FILE__, must be valid
		 * @param line The line in the source file in which the function call happens, e.g., __LINE__, must be valid
		 * @see succeeded().
		 */
		inline void setFailed(const char* file, const int line);

		/**
		 * Returns if this validation has succeeded.
		 * @return True, if so; False, if the validation has failed
		 */
		[[nodiscard]] inline bool succeeded() const;

		/**
		 * Returns a string containing the random generator's initial seed, if any
		 * @return The string with initial seed test, empty if no random generator is associated with this validation object
		 */
		inline std::string randomGeneratorOutput() const;

	protected:

		/**
		 * Disabled copy constructor.
		 */
		Validation(const Validation&) = delete;

		/**
		 * Sets the succeeded state to false.
		 */
		inline void setSucceededFalse();

	protected:

		/// True, if the validation has succeeded; False, if the validation has failed.
		bool succeeded_ = true;

		/// Optional random generator object which will be used during validation.
		RandomGenerator* randomGenerator_ = nullptr;

#ifdef OCEAN_DEBUG
		/// True, if the success state of this validation has been checked.
		mutable bool succeededChecked_ = false;
#endif // OCEAN_DEBUG
};

#ifndef OCEAN_EXPECT_TRUE
	#define OCEAN_EXPECT_TRUE(validation, value) validation.expectTrue(value, __FILE__, __LINE__)
#endif

#ifndef OCEAN_EXPECT_FALSE
	#define OCEAN_EXPECT_FALSE(validation, value) validation.expectFalse(value, __FILE__, __LINE__)
#endif

#ifndef OCEAN_EXPECT_EQUAL
	#define OCEAN_EXPECT_EQUAL(validation, value0, value1) validation.expectEqual(value0, value1, __FILE__, __LINE__)
#endif

#ifndef OCEAN_EXPECT_LESS
	#define OCEAN_EXPECT_LESS(validation, value0, value1) validation.expectLess(value0, value1, __FILE__, __LINE__)
#endif

#ifndef OCEAN_EXPECT_LESS_EQUAL
	#define OCEAN_EXPECT_LESS_EQUAL(validation, value0, value1) validation.expectLessEqual(value0, value1, __FILE__, __LINE__)
#endif

#ifndef OCEAN_EXPECT_GREATER
	#define OCEAN_EXPECT_GREATER(validation, value0, value1) validation.expectGreater(value0, value1, __FILE__, __LINE__)
#endif

#ifndef OCEAN_EXPECT_GREATER_EQUAL
	#define OCEAN_EXPECT_GREATER_EQUAL(validation, value0, value1) validation.expectGreaterEqual(value0, value1, __FILE__, __LINE__)
#endif

#ifndef OCEAN_SET_FAILED
	#define OCEAN_SET_FAILED(validation) validation.setFailed(__FILE__, __LINE__);
#endif

inline Validation::Validation(RandomGenerator& randomGenerator) :
	randomGenerator_(&randomGenerator)
{
	// nothing to do here
}

inline Validation::~Validation()
{
#ifdef OCEAN_DEBUG
	ocean_assert(succeededChecked_ && "The validation has not been check for success");
#endif
}

inline void Validation::expectTrue(const bool value)
{
	if (!value)
	{
		setSucceededFalse();

		Log::debug() << "Validation::expectTrue() failed at unknown location" << randomGeneratorOutput();
	}
}

inline void Validation::expectTrue(const bool value, const char* file, const int line)
{
	if (!value)
	{
		setSucceededFalse();

		ocean_assert(file != nullptr);
		if (file != nullptr)
		{
#ifdef OCEAN_USE_GTEST
			std::cerr << "\nValidation::expectTrue() failed in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
#else
			Log::error() << "Validation::expectTrue() failed in '" << file << "', in line " << line << randomGeneratorOutput();
#endif
		}
	}
}

inline void Validation::expectFalse(const bool value)
{
	if (value)
	{
		setSucceededFalse();

		Log::debug() << "Validation::expectFalse() failed at unknown location" << randomGeneratorOutput();
	}
}

inline void Validation::expectFalse(const bool value, const char* file, const int line)
{
	if (value)
	{
		setSucceededFalse();

		ocean_assert(file != nullptr);
		if (file != nullptr)
		{
#ifdef OCEAN_USE_GTEST
			std::cerr << "\nValidation::expectFalse() failed in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
#else
			Log::error() << "Validation::expectFalse() failed in '" << file << "', in line " << line << randomGeneratorOutput();
#endif
		}
	}
}

template <typename T>
inline void Validation::expectEqual(const T& value0, const T& value1)
{
	if (value0 != value1)
	{
		setSucceededFalse();

		if constexpr (Log::isSupported<T>())
		{
			Log::debug() << "Validation::expectEqual(" << value0 << ", " << value1 <<") failed at unknown location" << randomGeneratorOutput();
		}
		else
		{
			Log::debug() << "Validation::expectEqual() failed at unknown location" << randomGeneratorOutput();
		}
	}
}

template <typename T>
inline void Validation::expectEqual(const T& value0, const T& value1, const char* file, const int line)
{
	if (value0 != value1)
	{
		setSucceededFalse();

		ocean_assert(file != nullptr);
		if (file != nullptr)
		{
#ifdef OCEAN_USE_GTEST
			if constexpr (Log::isSupported<T, std::ostream>())
			{
				std::cerr << "\nValidation::expectEqual(" << value0 << ", " << value1 <<") failed in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
			}
			else
			{
				std::cerr << "\nValidation::expectEqual() failed in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
			}
#else
			if constexpr (Log::isSupported<T>())
			{
				Log::error() << "Validation::expectEqual(" << value0 << ", " << value1 <<") failed in '" << file << "', in line " << line << randomGeneratorOutput();
			}
			else
			{
				Log::error() << "Validation::expectEqual() failed in '" << file << "', in line " << line << randomGeneratorOutput();
			}
#endif // OCEAN_USE_GTEST
		}
	}
}

template <typename T>
inline void Validation::expectLess(const T& value0, const T& value1)
{
	if (!(value0 < value1))
	{
		setSucceededFalse();

		if constexpr (Log::isSupported<T>())
		{
			Log::debug() << "Validation::expectLess(" << value0 << ", " << value1 <<") failed at unknown location" << randomGeneratorOutput();
		}
		else
		{
			Log::debug() << "Validation::expectLess() failed at unknown location" << randomGeneratorOutput();
		}
	}
}

template <typename T>
inline void Validation::expectLess(const T& value0, const T& value1, const char* file, const int line)
{
	if (!(value0 < value1))
	{
		setSucceededFalse();

		ocean_assert(file != nullptr);
		if (file != nullptr)
		{
#ifdef OCEAN_USE_GTEST
			if constexpr (Log::isSupported<T, std::ostream>())
			{
				std::cerr << "\nValidation::expectLess(" << value0 << ", " << value1 <<") failed in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
			}
			else
			{
				std::cerr << "\nValidation::expectLess() failed in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
			}
#else
			if constexpr (Log::isSupported<T>())
			{
				Log::error() << "Validation::expectLess(" << value0 << ", " << value1 <<") failed in '" << file << "', in line " << line << randomGeneratorOutput();
			}
			else
			{
				Log::error() << "Validation::expectLess() failed in '" << file << "', in line " << line << randomGeneratorOutput();
			}
#endif // OCEAN_USE_GTEST
		}
	}
}

template <typename T>
inline void Validation::expectLessEqual(const T& value0, const T& value1)
{
	if (!(value0 <= value1))
	{
		setSucceededFalse();

		if constexpr (Log::isSupported<T>())
		{
			Log::debug() << "Validation::expectLessEqual(" << value0 << ", " << value1 <<") failed at unknown location" << randomGeneratorOutput();
		}
		else
		{
			Log::debug() << "Validation::expectLessEqual() failed at unknown location" << randomGeneratorOutput();
		}
	}
}

template <typename T>
inline void Validation::expectLessEqual(const T& value0, const T& value1, const char* file, const int line)
{
	if (!(value0 <= value1))
	{
		setSucceededFalse();

		ocean_assert(file != nullptr);
		if (file != nullptr)
		{
#ifdef OCEAN_USE_GTEST
			if constexpr (Log::isSupported<T, std::ostream>())
			{
				std::cerr << "\nValidation::expectLessEqual(" << value0 << ", " << value1 <<") failed in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
			}
			else
			{
				std::cerr << "\nValidation::expectLessEqual() failed in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
			}
#else
			if constexpr (Log::isSupported<T>())
			{
				Log::error() << "Validation::expectLessEqual(" << value0 << ", " << value1 <<") failed in '" << file << "', in line " << line << randomGeneratorOutput();
			}
			else
			{
				Log::error() << "Validation::expectLessEqual() failed in '" << file << "', in line " << line << randomGeneratorOutput();
			}
#endif // OCEAN_USE_GTEST
		}
	}
}

template <typename T>
inline void Validation::expectGreater(const T& value0, const T& value1)
{
	if (!(value0 > value1))
	{
		setSucceededFalse();

		if constexpr (Log::isSupported<T>())
		{
			Log::debug() << "Validation::expectGreater(" << value0 << ", " << value1 <<") failed at unknown location" << randomGeneratorOutput();
		}
		else
		{
			Log::debug() << "Validation::expectGreater() failed at unknown location" << randomGeneratorOutput();
		}
	}
}

template <typename T>
inline void Validation::expectGreater(const T& value0, const T& value1, const char* file, const int line)
{
	if (!(value0 > value1))
	{
		setSucceededFalse();

		ocean_assert(file != nullptr);
		if (file != nullptr)
		{
#ifdef OCEAN_USE_GTEST
			if constexpr (Log::isSupported<T, std::ostream>())
			{
				std::cerr << "\nValidation::expectGreater(" << value0 << ", " << value1 <<") failed in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
			}
			else
			{
				std::cerr << "\nValidation::expectGreater() failed in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
			}
#else
			if constexpr (Log::isSupported<T>())
			{
				Log::error() << "Validation::expectGreater(" << value0 << ", " << value1 <<") failed in '" << file << "', in line " << line << randomGeneratorOutput();
			}
			else
			{
				Log::error() << "Validation::expectGreater() failed in '" << file << "', in line " << line << randomGeneratorOutput();
			}
#endif // OCEAN_USE_GTEST
		}
	}
}

template <typename T>
inline void Validation::expectGreaterEqual(const T& value0, const T& value1)
{
	if (!(value0 >= value1))
	{
		setSucceededFalse();

		if constexpr (Log::isSupported<T>())
		{
			Log::debug() << "Validation::expectGreaterEqual(" << value0 << ", " << value1 <<") failed at unknown location" << randomGeneratorOutput();
		}
		else
		{
			Log::debug() << "Validation::expectGreaterEqual() failed at unknown location" << randomGeneratorOutput();
		}
	}
}

template <typename T>
inline void Validation::expectGreaterEqual(const T& value0, const T& value1, const char* file, const int line)
{
	if (!(value0 >= value1))
	{
		setSucceededFalse();

		ocean_assert(file != nullptr);
		if (file != nullptr)
		{
#ifdef OCEAN_USE_GTEST
			if constexpr (Log::isSupported<T, std::ostream>())
			{
				std::cerr << "\nValidation::expectGreaterEqual(" << value0 << ", " << value1 <<") failed in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
			}
			else
			{
				std::cerr << "\nValidation::expectGreaterEqual() failed in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
			}
#else
			if constexpr (Log::isSupported<T>())
			{
				Log::error() << "Validation::expectGreaterEqual(" << value0 << ", " << value1 <<") failed in '" << file << "', in line " << line << randomGeneratorOutput();
			}
			else
			{
				Log::error() << "Validation::expectGreaterEqual() failed in '" << file << "', in line " << line << randomGeneratorOutput();
			}
#endif // OCEAN_USE_GTEST
		}
	}
}

inline void Validation::setFailed()
{
	setSucceededFalse();

	Log::debug() << "Validation::setFailed() at unknown location" << randomGeneratorOutput();
}

inline void Validation::setFailed(const char* file, const int line)
{
	setSucceededFalse();

	ocean_assert(file != nullptr);
	if (file != nullptr)
	{
#ifdef OCEAN_USE_GTEST
		std::cerr << "\nValidation::setFailed() in '" << file << "', in line " << line << randomGeneratorOutput() << "\n" << std::endl;
#else
		Log::error() << "Validation::setFailed() in '" << file << "', in line " << line << randomGeneratorOutput();
#endif
	}
}

inline bool Validation::succeeded() const
{
#ifdef OCEAN_DEBUG
	succeededChecked_ = true;
#endif

	return succeeded_;
}

inline std::string Validation::randomGeneratorOutput() const
{
	if (randomGenerator_ != nullptr)
	{
		return ", with random generator initial seed '" + String::toAString(randomGenerator_->initialSeed()) + "'";
	}

	return std::string();
}

inline void Validation::setSucceededFalse()
{
	succeeded_ = false;
}

inline std::ostream& operator<<(std::ostream& stream, const Validation& validation)
{
	if (validation.succeeded())
	{
		stream << "succeeded.";
	}
	else
	{
		stream << "FAILED!";
	}

	return stream;
}

template <bool tActive>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const Validation& validation)
{
	if (validation.succeeded())
	{
		messageObject << "succeeded.";
	}
	else
	{
		messageObject << "FAILED!";
	}

	return messageObject;
}

template <bool tActive>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const Validation& validation)
{
	if (validation.succeeded())
	{
		messageObject << "succeeded.";
	}
	else
	{
		messageObject << "FAILED!";
	}

	return messageObject;
}

}

}

#endif // META_OCEAN_TEST_VALIDATION_H
