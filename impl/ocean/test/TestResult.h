/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TEST_RESULT_H
#define META_OCEAN_TEST_TEST_RESULT_H

#include "ocean/test/Test.h"

#include "ocean/base/Messenger.h"

namespace Ocean
{

namespace Test
{

/**
 * This class implements a simple test result accumulator.
 * The test result starts as succeeded (true) and can only be set to failed (false), never back to succeeded.
 * This is useful for accumulating multiple test results where any failure marks the entire test as failed.
 * Additionally, at least one test must be executed for the result to succeed.
 *
 * Usage example:
 * @code
 * bool testFunction()
 * {
 *     TestResult testResult("My test");
 *
 *     testResult = testSubFunction1();  // If false, testResult becomes false
 *     testResult = testSubFunction2();  // If false, testResult becomes false
 *     testResult = testSubFunction3();  // If false, testResult becomes false
 *
 *     Log::info() << testResult;  // Outputs "My test: succeeded." or "My test: FAILED!"
 *
 *     return testResult.succeeded();  // Must be called before destruction
 * }
 * @endcode
 *
 * @see Validation
 * @ingroup test
 */
class TestResult
{
	public:

		/**
		 * Default constructor.
		 */
		TestResult() = default;

		/**
		 * Constructor with test name.
		 * @param testName The name of the test, used for logging output
		 */
		inline explicit TestResult(const std::string& testName);

		/**
		 * Destructs this test result object.
		 */
		inline ~TestResult();

		/**
		 * Assignment operator for bool values.
		 * Only accepts false values; true values are ignored to prevent accidental resetting of a failed state.
		 * Marks that a test was executed.
		 * @param value The boolean value to assign; only false will change the state
		 * @return Reference to this object
		 */
		inline TestResult& operator=(bool value);

		/**
		 * Returns whether this test result has succeeded.
		 * This function must be called before the object is destroyed (checked in debug mode).
		 * @return True if all assigned values were true and at least one test was executed; false otherwise
		 */
		[[nodiscard]] inline bool succeeded() const;

		/**
		 * Returns whether this test result has succeeded so far without marking it as checked.
		 * @return True if all assigned values were true and at least one test was executed; false otherwise
		 */
		[[nodiscard]] inline bool succeededSoFar() const;

		/**
		 * Returns whether any test was executed.
		 * @return True if at least one test result was assigned; false otherwise
		 */
		[[nodiscard]] inline bool anyTestExecuted() const;

		/**
		 * Returns the name of the test.
		 * @return The test name, or empty string if no name was provided
		 */
		[[nodiscard]] inline const std::string& testName() const;

	protected:

		/**
		 * Disabled copy constructor.
		 */
		TestResult(const TestResult&) = delete;

		/**
		 * Disabled move constructor.
		 */
		TestResult(TestResult&&) = delete;

		/**
		 * Disabled copy assignment operator.
		 */
		TestResult& operator=(const TestResult&) = delete;

		/**
		 * Disabled move assignment operator.
		 */
		TestResult& operator=(TestResult&&) = delete;

	protected:

		/// The name of the test.
		std::string testName_;

		/// True if the test has succeeded; false if any test has failed.
		bool succeeded_ = true;

		/// True if at least one test was executed.
		bool anyTestExecuted_ = false;

#ifdef OCEAN_DEBUG
		/// True if the success state of this result has been checked via succeeded().
		mutable bool succeededChecked_ = false;
#endif // OCEAN_DEBUG
};

inline TestResult::~TestResult()
{
#ifdef OCEAN_DEBUG
	ocean_assert(succeededChecked_ && "The test result has not been checked via succeeded()");
#endif
}

inline TestResult::TestResult(const std::string& testName) :
	testName_(testName)
{
	ocean_assert(!testName.empty());

	if (!testName_.empty())
	{
		Log::info() << "---   " << testName_ << ":   ---";
	}
}

inline TestResult& TestResult::operator=(bool value)
{
	anyTestExecuted_ = true;

	// Only accept false values; once failed, always failed
	if (!value)
	{
		succeeded_ = false;
	}

	return *this;
}

inline bool TestResult::succeeded() const
{
#ifdef OCEAN_DEBUG
	succeededChecked_ = true;
#endif

	return succeeded_ && anyTestExecuted_;
}

inline bool TestResult::succeededSoFar() const
{
	return succeeded_ && anyTestExecuted_;
}

inline bool TestResult::anyTestExecuted() const
{
	return anyTestExecuted_;
}

inline const std::string& TestResult::testName() const
{
	return testName_;
}

/**
 * Writes a test result to a stream.
 * @param stream The stream to write to
 * @param testResult The test result to write
 * @return Reference to the stream
 */
inline std::ostream& operator<<(std::ostream& stream, const TestResult& testResult)
{
	if (!testResult.testName().empty())
	{
		stream << testResult.testName() << ": ";
	}

	if (testResult.succeededSoFar())
	{
		stream << "succeeded.";
	}
	else if (testResult.anyTestExecuted())
	{
		stream << "FAILED!";
	}
	else
	{
		stream << "No test was executed!";
	}

	return stream;
}

/**
 * Writes a test result to a message object.
 * @param messageObject The message object to write to
 * @param testResult The test result to write
 * @return Reference to the message object
 * @tparam tActive True to write the message; false to suppress it
 */
template <bool tActive>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const TestResult& testResult)
{
	if (!testResult.testName().empty())
	{
		messageObject << testResult.testName() << ": ";
	}

	if (testResult.succeededSoFar())
	{
		messageObject << "succeeded.";
	}
	else if (testResult.anyTestExecuted())
	{
		messageObject << "FAILED!";
	}
	else
	{
		messageObject << "No test was executed!";
	}

	return messageObject;
}

/**
 * Writes a test result to a message object (rvalue reference version).
 * @param messageObject The message object to write to
 * @param testResult The test result to write
 * @return Reference to the message object
 * @tparam tActive True to write the message; false to suppress it
 */
template <bool tActive>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const TestResult& testResult)
{
	if (!testResult.testName().empty())
	{
		messageObject << testResult.testName() << ": ";
	}

	if (testResult.succeededSoFar())
	{
		messageObject << "succeeded.";
	}
	else if (testResult.anyTestExecuted())
	{
		messageObject << "FAILED!";
	}
	else
	{
		messageObject << "No test was executed!";
	}

	return messageObject;
}

}

}

#endif // META_OCEAN_TEST_TEST_RESULT_H
