/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_SCOPED_OBJECT_H
#define META_OCEAN_TEST_TESTBASE_TEST_SCOPED_OBJECT_H

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
 * This class implement a test for the ScopedObject class.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestScopedObject
{
	protected:

		class Manager : public Singleton<Manager>
		{
			friend class Singleton<Manager>;

			protected:

				typedef std::unordered_map<uint64_t, uint64_t> IdCounterMap;

			public:

				/**
				 * Returns a thread-safe unique id.
				 * @return The unique id
				 */
				uint64_t uniqueId();

				/**
				 * Returns whether the manager hold at least one object associated with a specific id.
				 * @param id The id to check
				 * @return True, if so
				 */
				bool hasObject(const uint64_t id) const;

				/**
				 * Returns the number of objects associated with a specific id.
				 * @param id The id to check
				 * @return The number of object
				 */
				uint64_t numberObjects(const uint64_t id) const;

				/**
				 * Adds an object with specific id.
				 * @param id The id of the object
				 */
				void addObject(const uint64_t id);

				/**
				 * Removes an object with a specific id.
				 * @param id The id of the object
				 * @return True, if succeeded
				 */
				bool removeObject(const uint64_t id);

			protected:

				/**
				 * Default constructor.
				 */
				Manager() = default;

			protected:

				/// The counter for unique ids.
				uint64_t uniqueIdCounter_ = 0ull;

				/// The map mapping ids to counters.
				IdCounterMap idCounterMap_;

				/// The manager's lock.
				mutable Lock lock_;
		};

		/**
		 * This class implements an object.
		 */
		class Object
		{
			public:

				/**
				 * Default constructor.
				 */
				Object() = default;

				/**
				 * Creates a new object with given id.
				 */
				explicit inline Object(const uint64_t id);

				/**
				 * Returns the id of the object.
				 * @return The object's id
				 */
				inline uint64_t id() const;

			protected:

				/// The id of the object.
				uint64_t id_ = 0ull;
		};

		/**
		 * Definition of a vector holding pointers to objects.
		 */
		typedef std::vector<Object*> ObjectPointers;

		/**
		 * Definition of an unordered map mapping ids to counters.
		 */
		typedef std::unordered_map<int32_t, size_t> CounterMap;

	public:

		/**
		 * Tests the ScopedObject class.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the ScopedObject with runtime release function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testRuntime(const double testDuration);

		/**
		 * Tests the ScopedObject with compile time release function.
		 * @param testDuration Number of seconds for each test, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testCompileTime(const double testDuration);

	protected:

		/**
		 * Creates a new object.
		 * @param id The id of the object
		 * @return The new object
		 */
		static Object* createObject(const uint64_t id);

		/**
		 * Releases an object.
		 * @param object The object to be released, must be valid
		 */
		static void releaseObject(Object* object);

		/**
		 * Increases the counter of the counter map.
		 * @param id The id of the counter to increase.
		 * @return True, if succeeded
		 */
		static bool increaseCounter(const uint32_t id);

		/**
		 * Returns a static map mapping ids to counters.
		 */
		static CounterMap& counterMap();
};

inline TestScopedObject::Object::Object(const uint64_t id) :
	id_(id)
{
	// nothing to do here
}

inline uint64_t TestScopedObject::Object::id() const
{
	return id_;
}

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_SCOPED_OBJECT_H
