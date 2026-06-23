/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifdef OCEAN_USE_GTEST

#include "ocean/devices/Manager.h"
#include "ocean/devices/Device.h"
#include "ocean/devices/Factory.h"
#include "ocean/devices/Measurement.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <string>

namespace Ocean
{

namespace Test
{

namespace TestDevices
{

namespace
{

/**
 * Integration-level fixture exercising the `Devices::Manager` singleton
 * together with its bootstrapped `AdhocFactory`. The `Manager` constructor
 * registers the ad-hoc factory; these tests verify the resulting
 * Manager+Factory interaction through the public API only (no private-state
 * manipulation, no mocks).
 */
class ManagerAdhocFactoryIntegrationTest : public ::testing::Test
{
	protected:

		Devices::Manager& manager()
		{
			return Devices::Manager::get();
		}
};

// Components: Devices::Manager + Devices::AdhocFactory.
// The Manager constructor registers the Adhoc factory unconditionally.
// `isRegistered` iterates the Factory vector and matches by name; a
// successful lookup proves both components were wired together at
// singleton construction time.
TEST_F(ManagerAdhocFactoryIntegrationTest, AdhocFactoryIsRegisteredAfterConstruction)
{
	const std::string adhocName = Devices::Manager::nameAdhocFactory();
	EXPECT_FALSE(adhocName.empty());
	EXPECT_EQ(adhocName, std::string("Adhoc"));
	EXPECT_TRUE(manager().isRegistered(adhocName));
}

// Components: Devices::Manager + Devices::AdhocFactory.
// `libraries()` aggregates `Factory::name()` across every registered
// factory. The Adhoc factory must always be among them, with at least
// one entry total.
TEST_F(ManagerAdhocFactoryIntegrationTest, LibrariesAggregatesAdhocFactoryName)
{
	const Strings libraries = manager().libraries();
	ASSERT_FALSE(libraries.empty());

	const std::string adhocName = Devices::Manager::nameAdhocFactory();
	const auto it = std::find(libraries.cbegin(), libraries.cend(), adhocName);
	EXPECT_NE(it, libraries.cend())
		<< "Manager::libraries() must include the Adhoc factory name";

	for (const std::string& libraryName : libraries)
	{
		EXPECT_FALSE(libraryName.empty())
			<< "No registered library may have an empty name";
	}
}

// Components: Devices::Manager + Devices::AdhocFactory.
// `device(name, ...)` iterates the factory vector and asks each Factory
// for the named device. With a name that no factory knows, the Manager
// must propagate an invalid `DeviceRef` back to the caller — this checks
// the negative-path cross-component interaction.
TEST_F(ManagerAdhocFactoryIntegrationTest, DeviceByNameReturnsInvalidRefForUnknownDevice)
{
	const std::string unknownName = "ManagerIntegrationTest_UnknownDevice_X9Z";
	const Devices::DeviceRef sharedRef = manager().device(unknownName, false);
	EXPECT_FALSE(sharedRef);

	const Devices::DeviceRef exclusiveRef = manager().device(unknownName, true);
	EXPECT_FALSE(exclusiveRef);
}

// Components: Devices::Manager + Devices::AdhocFactory.
// `isRegistered` returns false for a library name not matched by any
// registered Factory. Verifies the negative branch of the linear scan.
TEST_F(ManagerAdhocFactoryIntegrationTest, IsRegisteredReturnsFalseForUnknownLibrary)
{
	EXPECT_FALSE(manager().isRegistered("ManagerIntegrationTest_UnknownLibrary_X9Z"));

	const std::string adhocName = Devices::Manager::nameAdhocFactory();
	EXPECT_FALSE(manager().isRegistered(adhocName + "Suffix"))
		<< "Substring matches must not be treated as registered libraries";
}

// Components: Devices::Manager + Devices::Measurement.
// The Manager's `objectDescription` consults the internal
// ObjectId->description multimap that the friend class `Measurement`
// populates via the private `createUniqueObjectId`. For an id that has
// never been registered, the Manager must return an empty string rather
// than throwing or returning garbage.
TEST_F(ManagerAdhocFactoryIntegrationTest, ObjectDescriptionReturnsEmptyForUnknownObjectId)
{
	const Devices::Measurement::ObjectId invalidId = Devices::Measurement::invalidObjectId();
	EXPECT_TRUE(manager().objectDescription(invalidId).empty());

	// Pick an id that is extremely unlikely to have been allocated by
	// any in-process Measurement instance to verify the lookup-miss path.
	const Devices::Measurement::ObjectId arbitraryUnknownId =
		static_cast<Devices::Measurement::ObjectId>(0xFFFFFFFFu) - Devices::Measurement::ObjectId(7);
	EXPECT_TRUE(manager().objectDescription(arbitraryUnknownId).empty());
}

// Components: Devices::Manager + Devices::AdhocFactory.
// `devices()` (no-arg overload) aggregates names from every Factory.
// With the Adhoc factory present (and currently exposing zero ad-hoc
// devices when nothing has been registered by this process), the
// aggregation must traverse without error and yield a Strings whose
// every element is non-empty. The result size must equal the sum of
// individual factory contributions — verified by checking that the
// no-arg overload's size is at least as large as the type-filtered
// overload's size for the same set of factories.
TEST_F(ManagerAdhocFactoryIntegrationTest, DevicesAggregationIsConsistentAcrossOverloads)
{
	const Strings allDevices = manager().devices();
	for (const std::string& name : allDevices)
	{
		EXPECT_FALSE(name.empty())
			<< "Manager::devices() must not surface empty device names";
	}

	// The type-filtered overload is a subset of the unfiltered overload
	// since each factory's typed query returns a subset of its full
	// device list.
	const Strings invalidTypeDevices = manager().devices(Devices::Device::DeviceType());
	EXPECT_LE(invalidTypeDevices.size(), allDevices.size())
		<< "Type-filtered Manager::devices(type) must not return more "
		   "names than the unfiltered Manager::devices()";
}

// Components: Devices::Manager + Devices::AdhocFactory.
// Repeated singleton access must yield the same instance and the same
// already-registered Adhoc factory; this guards against accidental
// re-construction or re-registration regressions in `Singleton<Manager>`.
TEST_F(ManagerAdhocFactoryIntegrationTest, RepeatedAccessYieldsStableManagerState)
{
	Devices::Manager& first = Devices::Manager::get();
	Devices::Manager& second = Devices::Manager::get();
	EXPECT_EQ(&first, &second);

	const Strings firstLibraries = first.libraries();
	const Strings secondLibraries = second.libraries();
	EXPECT_EQ(firstLibraries.size(), secondLibraries.size());

	const std::string adhocName = Devices::Manager::nameAdhocFactory();
	EXPECT_TRUE(first.isRegistered(adhocName));
	EXPECT_TRUE(second.isRegistered(adhocName));
}

} // namespace

} // namespace TestDevices

} // namespace Test

} // namespace Ocean

#endif // OCEAN_USE_GTEST
