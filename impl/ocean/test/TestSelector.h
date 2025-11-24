/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TEST_SELECTOR_H
#define META_OCEAN_TEST_TEST_SELECTOR_H

#include "ocean/test/Test.h"

#include "ocean/base/Messenger.h"

#include <string>
#include <unordered_set>

namespace Ocean
{

namespace Test
{

/**
 * This class implements a test selector that parses test function strings and determines which tests should be executed.
 * The selector supports hierarchical test names with dot notation (e.g., "frameconverter.rgb24.normal") and wildcard patterns.
 *
 * Usage examples:
 * - Empty string: runs all tests
 * - "frameconverter": runs the frameconverter test and all its sub-tests
 * - "frameconverter*": runs all tests starting with "frameconverter"
 * - "frameconverterrgb24.rgb24tobgr24": runs only that specific sub-test
 * - "test1,test2,test3": runs test1, test2, and test3
 *
 * Hierarchical matching example:
 * @code
 * TestSelector selector("datatype.char,frame");
 *
 * if (TestSelector subSelector = selector.shouldRun("datatype"))
 * {
 *     // subSelector now contains only "char" (prefix "datatype." is stripped)
 *     if (subSelector.shouldRun("char"))  // This will match!
 *     {
 *         // Run char test
 *     }
 * }
 * @endcode
 * @ingroup test
 */
class OCEAN_TEST_EXPORT TestSelector
{
	protected:

		/// Definition of an unordered set holding test patterns.
		using Patterns = std::unordered_set<std::string>;

	public:

		/**
		 * Creates a new test selector from a comma-separated list of test function names.
		 * Test names are converted to lowercase automatically.
		 * @param testFunctions Comma-separated list of test names, wildcards, or hierarchical test identifiers; empty string means run all tests
		 */
		explicit TestSelector(const std::string& testFunctions = std::string());

		/**
		 * Determines whether a test with the given name should be executed and returns a sub-selector for hierarchical filtering.
		 * @param testName The name of the test to check (case-insensitive), must be valid
		 * @return A TestSelector that can be cast to bool (true if test should run) and contains stripped sub-patterns
		 */
		TestSelector shouldRun(const std::string& testName) const;

		/**
		 * Bool conversion operator to check if this selector indicates the test should run.
		 * @return True if the test should execute, false otherwise
		 */
		explicit operator bool() const;

		/**
		 * Returns whether all tests should be executed.
		 * @return True if no specific patterns were provided (empty string), false otherwise
		 */
		inline bool runAll() const;

	private:

		/**
		 * Creates a sub-selector for hierarchical test filtering.
		 * @param patterns The patterns for this selector
		 * @param shouldExecute Whether tests matching this selector should execute
		 */
		TestSelector(Patterns&& patterns, bool shouldExecute);

		/**
		 * Checks if a pattern matches a test name.
		 * @param pattern The pattern to match against (may contain wildcards), must be valid
		 * @param testName The test name to check, must be valid
		 * @return True if the pattern matches the test name
		 */
		static bool matches(const std::string& pattern, const std::string& testName);

	private:

		/// Set of test patterns parsed from the input string
		Patterns patterns_;

		/// Whether tests matching this selector should execute
		bool shouldExecute_ = true;
};

inline bool TestSelector::runAll() const
{
	return patterns_.empty();
}

/**
 * Writes a test selector scope to a stream.
 * Outputs "Entire" if all tests should run, "Partial" if only selected tests should run.
 * @param stream The stream to write to
 * @param selector The test selector to write
 * @return Reference to the stream
 */
inline std::ostream& operator<<(std::ostream& stream, const TestSelector& selector)
{
	if (selector.runAll())
	{
		stream << "Entire";
	}
	else
	{
		stream << "Partial";
	}

	return stream;
}

/**
 * Writes a test selector scope to a message object.
 * Outputs "Entire" if all tests should run, "Partial" if only selected tests should run.
 * @param messageObject The message object to write to
 * @param selector The test selector to write
 * @return Reference to the message object
 * @tparam tActive True to write the message; false to suppress it
 */
template <bool tActive>
MessageObject<tActive>& operator<<(MessageObject<tActive>& messageObject, const TestSelector& selector)
{
	if (selector.runAll())
	{
		messageObject << "Entire";
	}
	else
	{
		messageObject << "Partial";
	}

	return messageObject;
}

/**
 * Writes a test selector scope to a message object (rvalue reference version).
 * Outputs "Entire" if all tests should run, "Partial" if only selected tests should run.
 * @param messageObject The message object to write to
 * @param selector The test selector to write
 * @return Reference to the message object
 * @tparam tActive True to write the message; false to suppress it
 */
template <bool tActive>
MessageObject<tActive>& operator<<(MessageObject<tActive>&& messageObject, const TestSelector& selector)
{
	if (selector.runAll())
	{
		messageObject << "Entire";
	}
	else
	{
		messageObject << "Partial";
	}

	return messageObject;
}

}

}

#endif // META_OCEAN_TEST_TEST_SELECTOR_H
