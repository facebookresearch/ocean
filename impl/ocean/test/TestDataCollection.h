/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TEST_DATA_COLLECTION_H
#define META_OCEAN_TEST_TEST_DATA_COLLECTION_H

#include "ocean/test/Test.h"
#include "ocean/test/TestData.h"

namespace Ocean
{

namespace Test
{

// Forward declaration.
class TestDataCollection;

/**
 * Definition of a shared pointer holding data TestDataCollection object.
 * @param test
 */
using SharedTestDataCollection = std::shared_ptr<TestDataCollection>;

/**
 * This class is the base class for all TestDataCollection objects.
 * The class holds a collection of several test data objects all belonging to the same test category.
 * @ingroup test
 */
class OCEAN_TEST_EXPORT TestDataCollection
{
	public:

		/**
		 * Destructs this object.
		 */
		virtual ~TestDataCollection() = default;

		/**
		 * Returns the test data object associated with a specified index.
		 * @param index The index of the test data object, with range [0, size())
		 * @return The specified test data object, nullptr if the object does not exist
		 */
		virtual SharedTestData data(const size_t index) = 0;

		/**
		 * Returns the number of data object objects this collection holds.
		 * @return The collection's test data objects, with range [0, infinity)
		 */
		virtual size_t size() = 0;
};

}

}

#endif // META_OCEAN_TEST_TEST_DATA_COLLECTION_H
