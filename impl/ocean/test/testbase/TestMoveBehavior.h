/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_MOVE_BEHAVIOR_H
#define META_OCEAN_TEST_TESTBASE_TEST_MOVE_BEHAVIOR_H

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
 * This class tests the move behavior of the underlying C++ std implementation.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestMoveBehavior
{
	protected:

		/**
		 * Simple helper class counting operations.
		 */
		class OperationCounter : public Singleton<OperationCounter>
		{
			friend class Singleton<OperationCounter>;

			public:

				/**
				 * Increments individual operation counters.
				 * @param constructor True, to increment the constructor counter
				 * @param copyConstructor True, to increment the copyConstructor counter
				 * @param moveConstructor True, to increment the moveConstructor counter
				 * @param assignOperator True, to increment the assignOperator counter
				 * @param moveOperator True, to increment the moveOperator counter
				 */
				void increment(const bool constructor, const bool copyConstructor, const bool moveConstructor, const bool assignOperator, const bool moveOperator);

				/**
				 * Returns the constructor counter.
				 * @return The constructor counter
				 */
				size_t constructor() const;

				/**
				 * Returns the copyConstructor counter.
				 * @return The copyConstructor counter
				 */
				size_t copyConstructor() const;

				/**
				 * Returns the moveConstructor counter.
				 * @return The moveConstructor counter
				 */
				size_t moveConstructor() const;

				/**
				 * Returns the assignOperator counter.
				 * @return The assignOperator counter
				 */
				size_t assignOperator() const;

				/**
				 * Returns the moveOperator counter.
				 * @return The moveOperator counter
				 */
				size_t moveOperator() const;

				/**
				 * Resets all counters.
				 */
				void reset();

			protected:

				/**
				 * Default conststructor.
				 */
				OperationCounter();

			protected:

				/// The constructor counter.
				size_t constructor_;

				/// The copyConstructor counter.
				size_t copyConstructor_;

				/// The moveConstructor counter.
				size_t moveConstructor_;

				/// The assignOperator counter.
				size_t assignOperator_;

				/// The moveOperator counter.
				size_t moveOperator_;

				/// The lock object.
				mutable Lock lock_;
		};

		/**
		 * Simple object with copy and move semantic without 'noexcept' keywords.
		 */
		class Object
		{
			public:

				/**
				 * Crates a new object.
				 * @param member The member value to be set
				 */
				Object(const int member);

				/**
				 * Copy constructor.
				 * @param object The object to be copied
				 */
				Object(const Object& object);

				/**
				 * Move constructor.
				 * @param object The object to be moved
				 */
				Object(Object&& object);

				/**
				 * Copy constructor.
				 * @param object The object to be copied
				 * @return The reference to this object
				 */
				Object& operator=(const Object& object);

				/**
				 * Move constructor.
				 * @param object The object to be moved
				 * @return The reference to this object
				 */
				Object& operator=(Object&& object);

			protected:

				/// The member of this object;
				int member_ = 0;
		};

		/**
		 * Simple object with copy and move semantic with 'noexcept' keywords.
		 */
		class NonExceptObject
		{
			public:

				/**
				 * Crates a new object.
				 * @param member The member value to be set
				 */
				NonExceptObject(const int member);

				/**
				 * Copy constructor.
				 * @param object The object to be copied
				 */
				NonExceptObject(const NonExceptObject& object);

				/**
				 * Move constructor.
				 * @param object The object to be moved
				 */
				NonExceptObject(NonExceptObject&& object) noexcept;

				/**
				 * Copy constructor.
				 * @param object The object to be copied
				 * @return The reference to this object
				 */
				NonExceptObject& operator=(const NonExceptObject& object);

				/**
				 * Move constructor.
				 * @param object The object to be moved
				 * @return The reference to this object
				 */
				NonExceptObject& operator=(NonExceptObject&& object) noexcept;

			protected:

				/// The member of this object;
				int member_ = 0;
		};

		/**
		 * Definition of a vector holding objects.
		 */
		typedef std::vector<Object> Objects;

		/**
		 * Definition of a vector holding objects.
		 */
		typedef std::vector<NonExceptObject> NonExceptObjects;

	public:
	
		/**
		 * Tests all lock functions.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the behavior for the default object.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDefaultObject(const double testDuration);

		/**
		 * Tests the behavior for the object with nonexcept keyword.
		 * @param testDuration The number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNonExceptObject(const double testDuration);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_MOVE_BEHAVIOR_H
