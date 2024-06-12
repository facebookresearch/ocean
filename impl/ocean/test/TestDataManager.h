/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TEST_DATA_MANAGER_H
#define META_OCEAN_TEST_TEST_DATA_MANAGER_H

#include "ocean/test/Test.h"
#include "ocean/test/TestDataCollection.h"

#include "ocean/base/Lock.h"
#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Singleton.h"

namespace Ocean
{

namespace Test
{

/**
 * The class implements the manager for test data collections as a singleton object.
 * @ingroup test
 */
class OCEAN_TEST_EXPORT TestDataManager : public Singleton<TestDataManager>
{
	friend class Singleton<TestDataManager>;
	friend class TestDataCollection;

	public:

		/**
		 * Definition of a scoped subscription object for for data collections.
		 */
		using ScopedSubscription = ScopedSubscriptionT<std::string, TestDataManager>;

		/**
		 * Definition of a vector holding scoped subscription objects.
		 */
		using ScopedSubscriptions = std::vector<ScopedSubscription>;

	protected:

		/**
		 * Definition of a pair combining a counter with a shared pointer.
		 */
		using TestDataCollectionPair = std::pair<unsigned int, std::shared_ptr<TestDataCollection>>;

		/**
		 * Definition of an unordered map mapping collection names to collection objects.
		 */
		using TestDataCollectionMap = std::unordered_map<std::string, TestDataCollectionPair>;

	public:

		/**
		 * Returns a specific test data collection.
		 * @param name The name of the test data collection to return, must be valid
		 * @return The requested test data collection, nullptr if it does not exist
		 */
		SharedTestDataCollection testDataCollection(const std::string& name) const;

		/**
		 * Registers a new test data collection.
		 * @param name The name of the new collection, be valid, must not exist already
		 * @param testDataCollection The new test data collection to register, must be valid
		 * @return The scoped subscription object associated with the data collection, the collection will be registered as long as the scoped subscription object exists
		 */
		[[nodiscard]] ScopedSubscription registerTestDataCollection(const std::string& name, std::unique_ptr<TestDataCollection>&& testDataCollection);

	protected:

		/**
		 * Protected default constructor.
		 */
		TestDataManager();

		/**
		 * Unregisters a registered data collection.
		 * @param name The name of the data collection to unregister, must be valid
		 */
		void unregisterTestDataCollection(const std::string& name);

	protected:

		/// The map mapping collection names to collection objects.
		TestDataCollectionMap testDataCollectionMap_;

		/// The manager's lock.
		mutable Lock lock_;
};

}

}

#endif // META_OCEAN_TEST_TEST_DATA_MANAGER_H
