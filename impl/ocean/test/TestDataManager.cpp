/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
		return iData->second.second;
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

	TestDataCollectionMap::iterator iData = testDataCollectionMap_.find(name);

	if (testDataCollectionMap_.find(name) == testDataCollectionMap_.cend())
	{
		iData = testDataCollectionMap_.emplace(name, TestDataCollectionPair(0u, nullptr)).first;
	}
	else
	{
		Log::debug() << "Test data collection '" << name << "' << already registered, updating the data";
	}

	ocean_assert(iData != testDataCollectionMap_.cend());

	TestDataCollectionPair& testDataCollectionPair = iData->second;

	++testDataCollectionPair.first;
	testDataCollectionPair.second = std::move(testDataCollection);

	return ScopedSubscription(name, std::bind(&TestDataManager::unregisterTestDataCollection, this, std::placeholders::_1));
}

void TestDataManager::unregisterTestDataCollection(const std::string& name)
{
	ocean_assert(!name.empty());

	const ScopedLock scopedLock(lock_);

	TestDataCollectionMap::iterator iData = testDataCollectionMap_.find(name);
	ocean_assert(iData != testDataCollectionMap_.end());

	if (iData != testDataCollectionMap_.end())
	{
		TestDataCollectionPair& testDataCollectionPair = iData->second;

		ocean_assert(testDataCollectionPair.first >= 1u);

		if (--testDataCollectionPair.first == 0u)
		{
			testDataCollectionMap_.erase(iData);
		}
	}
}

}

}
