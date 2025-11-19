/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTIO_TEST_JSON_PARSER_H
#define META_OCEAN_TEST_TESTIO_TEST_JSON_PARSER_H

#include "ocean/test/testio/TestIO.h"

#include "ocean/base/RandomGenerator.h"

#include "ocean/io/JSONParser.h"

#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestIO
{

/**
 * This class implements a JSON parser test.
 * @ingroup testio
 */
class OCEAN_TEST_IO_EXPORT TestJSONParser
{
	protected:

		/**
		 * This class holds both the JSON string and the expected parsed value.
		 */
		class JSONTestData
		{
			public:

				/**
				 * Generates a random JSON string with random content.
				 * @param randomGenerator The random generator to use
				 * @param maxLength Maximum length of the string
				 * @return The generated JSON-safe string
				 */
				static std::string randomString(RandomGenerator& randomGenerator, const size_t maxLength = 20);

				/**
				 * Generates a random JSON value (primitive, array, or object) with expected value.
				 * @param randomGenerator The random generator to use
				 * @param remainingDepth Remaining nesting depth allowed
				 * @return The generated JSON test data
				 */
				static JSONTestData randomValue(RandomGenerator& randomGenerator, const unsigned int remainingDepth);

				/**
				 * Generates a random JSON array with expected value.
				 * @param randomGenerator The random generator to use
				 * @param remainingDepth Remaining nesting depth allowed
				 * @return The generated JSON test data
				 */
				static JSONTestData randomArray(RandomGenerator& randomGenerator, const unsigned int remainingDepth);

				/**
				 * Generates a random JSON object with expected value.
				 * @param randomGenerator The random generator to use
				 * @param remainingDepth Remaining nesting depth allowed
				 * @return The generated JSON test data
				 */
				static JSONTestData randomObject(RandomGenerator& randomGenerator, const unsigned int remainingDepth);

			public:

				/// The JSON string representation.
				std::string jsonString_;

				/// The expected parsed value.
				IO::JSONParser::JSONValue expectedValue_;
		};

	public:

		/**
		 * Tests all JSON parser functions.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests parsing of primitive JSON values (null, boolean, number, string).
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testPrimitives(const double testDuration);

		/**
		 * Tests parsing of JSON arrays.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testArrays(const double testDuration);

		/**
		 * Tests parsing of JSON objects.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testObjects(const double testDuration);

		/**
		 * Tests parsing of nested JSON structures.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testNestedStructures(const double testDuration);

		/**
		 * Tests error handling with invalid JSON.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testErrorHandling(const double testDuration);

		/**
		 * Tests parsing from different input sources (file, buffer, stream).
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testInputSources(const double testDuration);

		/**
		 * Tests parsing of randomly generated JSON structures.
		 * @param testDuration The number of seconds for each test
		 * @return True, if succeeded
		 */
		static bool testRandomJSON(const double testDuration);

		/**
		* Tests strict and lenient parsing modes (trailing commas).
		* @param testDuration The number of seconds for each test
		* @return True, if succeeded
		*/
		static bool testStrictAndLenientParsing(const double testDuration);

	protected:

		/**
		 * Compares two JSON values for equality.
		 * @param validation The validation object
		 * @param parsed The parsed JSON value
		 * @param expected The expected JSON value
		 * @return True if they match
		 */
		static bool compareJSONValues(Validation& validation, const IO::JSONParser::JSONValue& parsed, const IO::JSONParser::JSONValue& expected);
};

} // namespace TestIO

} // namespace Test

} // namespace Ocean

#endif // META_OCEAN_TEST_TESTIO_TEST_JSON_PARSER_H
