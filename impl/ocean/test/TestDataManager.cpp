// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/test/TestDataManager.h"

#include "ocean/base/Timestamp.h"
#include "ocean/base/Thread.h"

namespace Ocean
{

namespace Test
{

TestDataManager::TestDataManager()
{
	// nothing to do here
}

SharedTestDataCollection TestDataManager::testDataCollection(const std::string& name) const
{
	ocean_assert(!name.empty());

	if (name.empty())
	{
		return nullptr;
	}

	const ScopedLock scopedLock(lock_);

	const TestDataCollectionMap::const_iterator iData = testDataCollectionMap_.find(name);

	if (iData != testDataCollectionMap_.cend())
	{
		return iData->second;
	}

	return nullptr;
}

TestDataManager::ScopedSubscription TestDataManager::registerTestDataCollection(const std::string& name, std::unique_ptr<TestDataCollection>&& testDataCollection)
{
	ocean_assert(!name.empty());
	ocean_assert(testDataCollection);

if (name.empty() || !testDataCollection)
	{
		return ScopedSubscription();
	}

	const ScopedLock scopedLock(lock_);

	if (testDataCollectionMap_.find(name) != testDataCollectionMap_.cend())
	{
		ocean_assert(false && "The test data collection exists already!");
		return ScopedSubscription();
	}

	testDataCollectionMap_.emplace(name, std::move(testDataCollection));

	return ScopedSubscription(name, std::bind(&TestDataManager::unregisterTestDataCollection, this, std::placeholders::_1));
}

void TestDataManager::unregisterTestDataCollection(const std::string& name)
{
	const ScopedLock scopedLock(lock_);

	const TestDataCollectionMap::const_iterator iData = testDataCollectionMap_.find(name);
	ocean_assert(iData != testDataCollectionMap_.cend());

	if (iData != testDataCollectionMap_.cend())
	{
		testDataCollectionMap_.erase(iData);
	}
}

}

}
