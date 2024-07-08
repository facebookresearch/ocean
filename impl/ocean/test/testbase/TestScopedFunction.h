/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_SCOPED_FUNCTION_H
#define META_OCEAN_TEST_TESTBASE_TEST_SCOPED_FUNCTION_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implement a test for the TestScopedFunction class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestScopedFunction
{
	protected:

		/**
		 * Helper class allowing to determine whether a function was invoked or not.
		 */
		class Object
		{
			public:

				/**
				 * Default constructor.
				 */
				Object() = default;

				/**
				 * Invokes a function.
				 */
				inline void invokeFunction();

				/**
				 * Returns the number of times the function has been invoked
				 * @return The number of function calls, with range [0, infinity)
				 */
				inline size_t number() const;

			protected:

				/**
				 * Disabled copy constructor.
				 */
				Object(const Object&) = delete;

				/**
				 * Disabled assign operator.
				 * @return Reference to this object
				 */
				Object& operator=(const Object&) = delete;

			protected:

				/// The number of times the function has been invoked.
				size_t number_ = 0;
		};

	public:

		/**
		 * Tests the ScopedFunction class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the release functionality.
		 * @return True, if succeeded
		 */
		static bool testRelease();
};

inline void TestScopedFunction::Object::invokeFunction()
{
	++number_;
}

inline size_t TestScopedFunction::Object::number() const
{
	return number_;
}

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_SCOPED_FUNCTION_H
