/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestStackHeapVector.h"

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/StackHeapVector.h"
#include "ocean/base/Timestamp.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestStackHeapVector::test(const double testDuration, const TestSelector& selector)
{
	ocean_assert(testDuration > 0.0);

	TestResult testResult("StackHeapVector test");
	Log::info() << " ";

	if (selector.shouldRun("defaultconstructor"))
	{
		testResult = testDefaultConstructor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("sizeconstructor"))
	{
		testResult = testSizeConstructor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("sizeelementconstructor"))
	{
		testResult = testSizeElementConstructor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("moveconstructorfromvector"))
	{
		testResult = testMoveConstructorFromVector(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("copyconstructorfromvector"))
	{
		testResult = testCopyConstructorFromVector(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("initializerlistconstructor"))
	{
		testResult = testInitializerListConstructor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("copyconstructor"))
	{
		testResult = testCopyConstructor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("moveconstructor"))
	{
		testResult = testMoveConstructor(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("copyassignment"))
	{
		testResult = testCopyAssignment(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("moveassignment"))
	{
		testResult = testMoveAssignment(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("equality"))
	{
		testResult = testEquality(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("assign"))
	{
		testResult = testAssign(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("pushback"))
	{
		testResult = testPushBack(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("resize"))
	{
		testResult = testResize(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("emplaceback"))
	{
		testResult = testEmplaceBack(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("popback"))
	{
		testResult = testPopBack(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("frontback"))
	{
		testResult = testFrontBack(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("reserve"))
	{
		testResult = testReserve(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	if (selector.shouldRun("performance"))
	{
		testResult = testPerformance(testDuration);

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestStackHeapVector, DefaultConstructor)
{
	EXPECT_TRUE(TestStackHeapVector::testDefaultConstructor(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, SizeConstructor)
{
	EXPECT_TRUE(TestStackHeapVector::testSizeConstructor(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, SizeElementConstructor)
{
	EXPECT_TRUE(TestStackHeapVector::testSizeElementConstructor(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, MoveConstructorFromVector)
{
	EXPECT_TRUE(TestStackHeapVector::testMoveConstructorFromVector(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, CopyConstructorFromVector)
{
	EXPECT_TRUE(TestStackHeapVector::testCopyConstructorFromVector(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, InitializerListConstructor)
{
	EXPECT_TRUE(TestStackHeapVector::testInitializerListConstructor(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, CopyConstructor)
{
	EXPECT_TRUE(TestStackHeapVector::testCopyConstructor(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, MoveConstructor)
{
	EXPECT_TRUE(TestStackHeapVector::testMoveConstructor(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, CopyAssignment)
{
	EXPECT_TRUE(TestStackHeapVector::testCopyAssignment(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, MoveAssignment)
{
	EXPECT_TRUE(TestStackHeapVector::testMoveAssignment(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, Equality)
{
	EXPECT_TRUE(TestStackHeapVector::testEquality(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, Assign)
{
	EXPECT_TRUE(TestStackHeapVector::testAssign(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, PushBack)
{
	EXPECT_TRUE(TestStackHeapVector::testPushBack(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, Resize)
{
	EXPECT_TRUE(TestStackHeapVector::testResize(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, EmplaceBack)
{
	EXPECT_TRUE(TestStackHeapVector::testEmplaceBack(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, PopBack)
{
	EXPECT_TRUE(TestStackHeapVector::testPopBack(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, FrontBack)
{
	EXPECT_TRUE(TestStackHeapVector::testFrontBack(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, Reserve)
{
	EXPECT_TRUE(TestStackHeapVector::testReserve(GTEST_TEST_DURATION));
}

TEST(TestStackHeapVector, Performance)
{
	EXPECT_TRUE(TestStackHeapVector::testPerformance(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestStackHeapVector::testDefaultConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing default constructor:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testDefaultConstructor<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testDefaultConstructor<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testDefaultConstructor<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testDefaultConstructor<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testDefaultConstructor<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testDefaultConstructor<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testDefaultConstructor<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testDefaultConstructor<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testDefaultConstructor<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testDefaultConstructor<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testDefaultConstructor<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testSizeConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing size constructor:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testSizeConstructor<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeConstructor<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeConstructor<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeConstructor<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeConstructor<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeConstructor<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeConstructor<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeConstructor<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeConstructor<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeConstructor<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeConstructor<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testSizeElementConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing size-element constructor:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testSizeElementConstructor<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeElementConstructor<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeElementConstructor<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeElementConstructor<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeElementConstructor<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeElementConstructor<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeElementConstructor<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeElementConstructor<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeElementConstructor<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeElementConstructor<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testSizeElementConstructor<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testMoveConstructorFromVector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing move constructor:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testMoveConstructorFromVector<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructorFromVector<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructorFromVector<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructorFromVector<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructorFromVector<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructorFromVector<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructorFromVector<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructorFromVector<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructorFromVector<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructorFromVector<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructorFromVector<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testCopyConstructorFromVector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing copy constructor from vector:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testCopyConstructorFromVector<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructorFromVector<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructorFromVector<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructorFromVector<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructorFromVector<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructorFromVector<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructorFromVector<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructorFromVector<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructorFromVector<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructorFromVector<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructorFromVector<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testInitializerListConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing initializer list constructor:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testInitializerListConstructor<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testInitializerListConstructor<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testInitializerListConstructor<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testInitializerListConstructor<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testInitializerListConstructor<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testInitializerListConstructor<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testInitializerListConstructor<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testInitializerListConstructor<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testInitializerListConstructor<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testInitializerListConstructor<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testInitializerListConstructor<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testCopyConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing copy constructor:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testCopyConstructor<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructor<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructor<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructor<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructor<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructor<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructor<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructor<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructor<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructor<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyConstructor<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testMoveConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing move constructor:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testMoveConstructor<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructor<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructor<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructor<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructor<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructor<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructor<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructor<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructor<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructor<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveConstructor<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testCopyAssignment(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing copy assignment:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testCopyAssignment<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyAssignment<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyAssignment<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyAssignment<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyAssignment<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyAssignment<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyAssignment<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyAssignment<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyAssignment<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyAssignment<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testCopyAssignment<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testMoveAssignment(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing move assignment:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testMoveAssignment<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveAssignment<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveAssignment<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveAssignment<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveAssignment<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveAssignment<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveAssignment<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveAssignment<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveAssignment<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveAssignment<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testMoveAssignment<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testEquality(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing equality:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testEquality<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEquality<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEquality<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEquality<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEquality<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEquality<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEquality<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEquality<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEquality<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEquality<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEquality<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testAssign(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing assign:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testAssign<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testAssign<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testAssign<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testAssign<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testAssign<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testAssign<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testAssign<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testAssign<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testAssign<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testAssign<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testAssign<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testPushBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing push back:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testPushBack<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPushBack<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPushBack<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPushBack<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPushBack<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPushBack<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPushBack<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPushBack<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPushBack<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPushBack<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPushBack<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testResize(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing resize:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testResize<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testResize<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testResize<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testResize<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testResize<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testResize<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testResize<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testResize<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testResize<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testResize<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testResize<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testEmplaceBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing emplace back:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testEmplaceBack<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEmplaceBack<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEmplaceBack<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEmplaceBack<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEmplaceBack<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEmplaceBack<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEmplaceBack<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEmplaceBack<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEmplaceBack<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEmplaceBack<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testEmplaceBack<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testPopBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing pop back:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testPopBack<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPopBack<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPopBack<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPopBack<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPopBack<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPopBack<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPopBack<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPopBack<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPopBack<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPopBack<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPopBack<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testFrontBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing front/back:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testFrontBack<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testFrontBack<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testFrontBack<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testFrontBack<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testFrontBack<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testFrontBack<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testFrontBack<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testFrontBack<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testFrontBack<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testFrontBack<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testFrontBack<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testReserve(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing reserve:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testReserve<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testReserve<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testReserve<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testReserve<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testReserve<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testReserve<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testReserve<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testReserve<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testReserve<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testReserve<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testReserve<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

bool TestStackHeapVector::testPerformance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "Testing performance:";

	Validation validation;

	OCEAN_EXPECT_TRUE(validation, testPerformance<1>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPerformance<2>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPerformance<3>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPerformance<4>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPerformance<5>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPerformance<6>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPerformance<7>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPerformance<8>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPerformance<31>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPerformance<32>(testDuration));
	OCEAN_EXPECT_TRUE(validation, testPerformance<64>(testDuration));

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testDefaultConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			const StackHeapVector<uint64_t, tStackCapacity> defaultVector;

			OCEAN_EXPECT_TRUE(validation, defaultVector.isEmpty());
			OCEAN_EXPECT_EQUAL(validation, defaultVector.size(), size_t(0));
		}

		{
			const StackHeapVector<Frame, tStackCapacity> defaultVector;

			OCEAN_EXPECT_TRUE(validation, defaultVector.isEmpty());
			OCEAN_EXPECT_EQUAL(validation, defaultVector.size(), size_t(0));
		}

		{
			const StackHeapVector<std::string, tStackCapacity> defaultVector;

			OCEAN_EXPECT_TRUE(validation, defaultVector.isEmpty());
			OCEAN_EXPECT_EQUAL(validation, defaultVector.size(), size_t(0));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testSizeConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// size-only constructor (default-initialized elements)

			const size_t numberElements = size_t(RandomI::random(randomGenerator, 1000u));

			const StackHeapVector<std::string, tStackCapacity> stackHeapVector(numberElements);

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), numberElements);
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.isEmpty(), numberElements == 0);

			for (size_t n = 0; n < numberElements; ++n)
			{
				// Default-constructed strings should be empty
				OCEAN_EXPECT_TRUE(validation, stackHeapVector[n].empty());
			}

			size_t counter = 0;

			for (const std::string& element : stackHeapVector)
			{
				OCEAN_EXPECT_TRUE(validation, element.empty());
				++counter;
			}

			OCEAN_EXPECT_EQUAL(validation, counter, numberElements);
		}

		{
			// size-only constructor with uint64_t (should be zero-initialized)

			const size_t numberElements = size_t(RandomI::random(randomGenerator, 1000u));

			const StackHeapVector<uint64_t, tStackCapacity> stackHeapVector(numberElements);

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), numberElements);

			for (size_t n = 0; n < numberElements; ++n)
			{
				// Default-constructed uint64_t should be zero
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], uint64_t(0));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testSizeElementConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const size_t numberElements = size_t(RandomI::random(randomGenerator, 1000u));

		std::string value;

		if (RandomI::boolean(randomGenerator))
		{
			value = String::toAString(RandomI::random64(randomGenerator));
		}

		const StackHeapVector<std::string, tStackCapacity> stackHeapVector(numberElements, value);

		OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), numberElements);
		OCEAN_EXPECT_EQUAL(validation, stackHeapVector.isEmpty(), numberElements == 0);

		for (size_t n = 0; n < numberElements; ++n)
		{
			if (stackHeapVector[n] != value)
			{
				OCEAN_SET_FAILED(validation);
			}
		}

		size_t counter = 0;

		for (const std::string& element : stackHeapVector)
		{
			OCEAN_EXPECT_EQUAL(validation, element, value);

			++counter;
		}

		OCEAN_EXPECT_EQUAL(validation, counter, numberElements);

		counter = 0;

		StackHeapVector<std::string, tStackCapacity> copyStackHeapVector(stackHeapVector);

		for (std::string& element : copyStackHeapVector)
		{
			OCEAN_EXPECT_EQUAL(validation, element, value);

			++counter;
		}

		OCEAN_EXPECT_EQUAL(validation, counter, numberElements);

		copyStackHeapVector.clear();

		OCEAN_EXPECT_TRUE(validation, copyStackHeapVector.isEmpty());
		OCEAN_EXPECT_EQUAL(validation, copyStackHeapVector.size(), size_t(0));
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testMoveConstructorFromVector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const size_t numberElements = size_t(RandomI::random(randomGenerator, 1000u));

		std::vector<TestElement> vector;
		vector.reserve(numberElements);

		for (size_t nElement = 0; nElement < numberElements; ++nElement)
		{
			vector.emplace_back(nElement);
		}

		const StackHeapVector<TestElement, tStackCapacity> stackHeapVector(std::move(vector));

		for (size_t nElement = 0; nElement < numberElements; ++nElement)
		{
			const size_t value = stackHeapVector[nElement].value();

			OCEAN_EXPECT_EQUAL(validation, value, nElement + 1000);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testCopyConstructorFromVector(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const size_t numberElements = size_t(RandomI::random(randomGenerator, 1000u));

		std::vector<TestElement> vector;
		vector.reserve(numberElements);

		for (size_t nElement = 0; nElement < numberElements; ++nElement)
		{
			vector.emplace_back(nElement);
		}

		const StackHeapVector<TestElement, tStackCapacity> stackHeapVector(vector);

		for (size_t nElement = 0; nElement < numberElements; ++nElement)
		{
			const size_t value = stackHeapVector[nElement].value();

			OCEAN_EXPECT_EQUAL(validation, value, nElement + 2000);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testInitializerListConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const StackHeapVector<TestElement, tStackCapacity> stackHeapVector({TestElement(0), TestElement(1), TestElement(2), TestElement(3), TestElement(4), TestElement(5), TestElement(6), TestElement(7), TestElement(8), TestElement(9)});

		for (size_t nElement = 0; nElement < 10; ++nElement)
		{
			const size_t value = stackHeapVector[nElement].value();

			OCEAN_EXPECT_EQUAL(validation, value, nElement + 2000);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testCopyConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			const size_t numberElements = size_t(RandomI::random(randomGenerator, 1000u));

			StackHeapVector<std::string, tStackCapacity> originalVector;

			for (size_t nElement = 0; nElement < numberElements; ++nElement)
			{
				originalVector.pushBack(String::toAString(nElement));
			}

			const StackHeapVector<std::string, tStackCapacity> copiedVector(originalVector);

			OCEAN_EXPECT_EQUAL(validation, originalVector, copiedVector);

			// Verify independence: modify original and ensure copy is unchanged

			if (numberElements > 0)
			{
				const std::string originalFirstValue = originalVector[0];
				originalVector[0] = "modified";

				OCEAN_EXPECT_EQUAL(validation, copiedVector[0], originalFirstValue);
				OCEAN_EXPECT_NOT_EQUAL(validation, copiedVector[0], originalVector[0]);
			}
		}

		{
			const size_t numberElements = size_t(RandomI::random(randomGenerator, 1000u));

			StackHeapVector<TestElement, tStackCapacity> originalVector;

			for (size_t nElement = 0; nElement < numberElements; ++nElement)
			{
				originalVector.emplaceBack(nElement);
			}

			const StackHeapVector<TestElement, tStackCapacity> copiedVector(originalVector);

			OCEAN_EXPECT_EQUAL(validation, copiedVector.size(), numberElements);

			// Verify all elements were copied correctly
			// Note: TestElement copy constructor adds copyOffset_ (2000) to the value
			for (size_t nElement = 0; nElement < numberElements; ++nElement)
			{
				const size_t copiedValue = copiedVector[nElement].value();
				const size_t originalValue = originalVector[nElement].value();

				// originalVector has values created by emplaceBack, which start at nElement + 2000 (from TestElement constructor assignment)
				// copiedVector will have those values + 2000 more from the copy constructor
				OCEAN_EXPECT_EQUAL(validation, copiedValue, originalValue + TestElement::copyOffset_);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testMoveConstructor(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const size_t numberElements = size_t(RandomI::random(randomGenerator, 1000u));

		StackHeapVector<std::string, tStackCapacity> originalVector;

		for (size_t nElement = 0; nElement < numberElements; ++nElement)
		{
			originalVector.pushBack(String::toAString(nElement));
		}

		const size_t originalSize = originalVector.size();

		StackHeapVector<std::string, tStackCapacity> movedVector(std::move(originalVector));

		OCEAN_EXPECT_EQUAL(validation, movedVector.size(), originalSize);
		OCEAN_EXPECT_EQUAL(validation, originalVector.size(), size_t(0));

		for (size_t nElement = 0; nElement < movedVector.size(); ++nElement)
		{
			OCEAN_EXPECT_EQUAL(validation, movedVector[nElement], String::toAString(nElement));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testCopyAssignment(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test copy assignment with strings
		const size_t numberElements1 = size_t(RandomI::random(randomGenerator, 1000u));
		const size_t numberElements2 = size_t(RandomI::random(randomGenerator, 1000u));

		StackHeapVector<std::string, tStackCapacity> vector1;
		StackHeapVector<std::string, tStackCapacity> vector2;

		for (size_t nElement = 0; nElement < numberElements1; ++nElement)
		{
			vector1.pushBack(String::toAString(nElement));
		}

		for (size_t nElement = 0; nElement < numberElements2; ++nElement)
		{
			vector2.pushBack(String::toAString(nElement + 1000));
		}

		vector2 = vector1;

		OCEAN_EXPECT_EQUAL(validation, vector1, vector2);
		OCEAN_EXPECT_EQUAL(validation, vector2.size(), numberElements1);

		// Verify independence: modify vector1 and ensure vector2 is unchanged
		if (numberElements1 > 0)
		{
			const std::string originalValue = vector1[0];
			vector1[0] = "modified";

			OCEAN_EXPECT_EQUAL(validation, vector2[0], originalValue);
			OCEAN_EXPECT_NOT_EQUAL(validation, vector2[0], vector1[0]);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testMoveAssignment(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const size_t numberElements1 = size_t(RandomI::random(randomGenerator, 1000u));
		const size_t numberElements2 = size_t(RandomI::random(randomGenerator, 1000u));

		StackHeapVector<std::string, tStackCapacity> vector1;
		StackHeapVector<std::string, tStackCapacity> vector2;

		for (size_t nElement = 0; nElement < numberElements1; ++nElement)
		{
			vector1.pushBack(String::toAString(nElement));
		}

		for (size_t nElement = 0; nElement < numberElements2; ++nElement)
		{
			vector2.pushBack(String::toAString(nElement + 1000));
		}

		const size_t originalVector1Size = vector1.size();

		vector2 = std::move(vector1);

		OCEAN_EXPECT_EQUAL(validation, vector2.size(), originalVector1Size);
		OCEAN_EXPECT_EQUAL(validation, vector1.size(), size_t(0));

		for (size_t nElement = 0; nElement < vector2.size(); ++nElement)
		{
			OCEAN_EXPECT_EQUAL(validation, vector2[nElement], String::toAString(nElement));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testEquality(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		{
			// Test equality with identical vectors

			const size_t numberElements = size_t(RandomI::random(randomGenerator, 1000u));

			StackHeapVector<uint64_t, tStackCapacity> vector1;
			StackHeapVector<uint64_t, tStackCapacity> vector2;

			for (size_t n = 0; n < numberElements; ++n)
			{
				const uint64_t value = RandomI::random64(randomGenerator);
				vector1.pushBack(value);
				vector2.pushBack(value);
			}

			OCEAN_EXPECT_EQUAL(validation, vector1, vector2);
			OCEAN_EXPECT_EQUAL(validation, vector2, vector1);

			OCEAN_EXPECT_EQUAL(validation, vector1, vector2);
			OCEAN_EXPECT_EQUAL(validation, vector2, vector1);
		}

		{
			// Test equality with different sizes

			const size_t size1 = size_t(RandomI::random(randomGenerator, 1u, 1000u));
			const size_t size2 = size_t(RandomI::random(randomGenerator, 1u, 1000u));

			if (size1 != size2)
			{
				StackHeapVector<uint64_t, tStackCapacity> vector1(size1, uint64_t(0));
				StackHeapVector<uint64_t, tStackCapacity> vector2(size2, uint64_t(0));

				OCEAN_EXPECT_NOT_EQUAL(validation, vector1, vector2);

				OCEAN_EXPECT_NOT_EQUAL(validation, vector1, vector2);
			}
		}

		{
			// Test equality with same size but different elements

			const size_t numberElements = size_t(RandomI::random(randomGenerator, 10u, 1000u));

			StackHeapVector<std::string, tStackCapacity> vector1;
			StackHeapVector<std::string, tStackCapacity> vector2;

			for (size_t n = 0; n < numberElements; ++n)
			{
				vector1.pushBack(String::toAString(n));
				vector2.pushBack(String::toAString(n));
			}

			// Modify one element
			const size_t modifyIndex = size_t(RandomI::random(randomGenerator, (unsigned int)(numberElements - 1)));
			vector2[modifyIndex] = "different";

			OCEAN_EXPECT_NOT_EQUAL(validation, vector1, vector2);

			OCEAN_EXPECT_NOT_EQUAL(validation, vector1, vector2);
		}

		{
			// Test equality with empty vectors

			const StackHeapVector<uint64_t, tStackCapacity> emptyVector1;
			const StackHeapVector<uint64_t, tStackCapacity> emptyVector2;

			OCEAN_EXPECT_EQUAL(validation, emptyVector1, emptyVector2);

			OCEAN_EXPECT_EQUAL(validation, emptyVector1, emptyVector2);
		}

		{
			// Test self-equality

			const size_t numberElements = size_t(RandomI::random(randomGenerator, 1000u));

			StackHeapVector<uint64_t, tStackCapacity> vector;

			for (size_t n = 0; n < numberElements; ++n)
			{
				vector.pushBack(RandomI::random64(randomGenerator));
			}

			OCEAN_EXPECT_EQUAL(validation, vector, vector);

			OCEAN_EXPECT_EQUAL(validation, vector, vector);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testAssign(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		StackHeapVector<uint64_t, tStackCapacity> stackHeapVector;

		{
			const size_t capacity = size_t(RandomI::random(randomGenerator, 100u));

			stackHeapVector.reserve(capacity);

			const size_t expectedCapacity = std::max(tStackCapacity, capacity);

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.capacity(), expectedCapacity);

			const uint64_t value = RandomI::random64(randomGenerator);

			const size_t size = size_t(RandomI::random(randomGenerator, 100u));

			stackHeapVector.assign(size, value);

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), size);

			for (size_t n = 0; n < stackHeapVector.size(); ++n)
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], value);
			}

			for (const uint64_t element : stackHeapVector)
			{
				OCEAN_EXPECT_EQUAL(validation, element, value);
			}
		}

		{
			const size_t capacity = size_t(RandomI::random(randomGenerator, 100u));

			const size_t expectedCapacity = std::max(std::max(tStackCapacity, capacity), std::max(stackHeapVector.size(), stackHeapVector.capacity()));

			stackHeapVector.reserve(capacity);

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.capacity(), expectedCapacity);

			const size_t size = size_t(RandomI::random(randomGenerator, 100u));

			const uint64_t value = RandomI::random64(randomGenerator);

			stackHeapVector.assign(size, value);

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), size);

			for (size_t n = 0; n < stackHeapVector.size(); ++n)
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], value);
			}

			for (const uint64_t element : stackHeapVector)
			{
				OCEAN_EXPECT_EQUAL(validation, element, value);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testPushBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		StackHeapVector<std::string, tStackCapacity> stackHeapVector;

		const size_t insertSize = size_t(RandomI::random(randomGenerator, 10));

		for (size_t n = 0; n < insertSize; ++n)
		{
			stackHeapVector.pushBack(String::toAString(n));

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), n + 1);
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], String::toAString(n));
		}

		const size_t removeSize = size_t(RandomI::random(randomGenerator, (unsigned int)(insertSize)));

		for (size_t n = 0; n < removeSize; ++n)
		{
			stackHeapVector.popBack();

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), insertSize - n - 1);
		}

		const size_t expectedSize = insertSize - removeSize;
		OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), expectedSize);

		const size_t insertSize2 = size_t(RandomI::random(randomGenerator, 10));

		for (size_t n = 0; n < insertSize2; ++n)
		{
			stackHeapVector.emplaceBack(String::toAString(n));

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), expectedSize + n + 1);
		}

		OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), expectedSize + insertSize2);

		for (size_t n = 0; n < stackHeapVector.size(); ++n)
		{
			if (n < expectedSize)
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], String::toAString(n));
			}
			else
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], String::toAString(n - expectedSize));
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testResize(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		const size_t initialSize = size_t(RandomI::random(randomGenerator, 10));

		StackHeapVector<std::string, tStackCapacity> stackHeapVector(initialSize, std::string("i"));

		OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), initialSize);

		for (const std::string& element : stackHeapVector)
		{
			OCEAN_EXPECT_EQUAL(validation, element, std::string("i"));
		}

		const size_t resize0 = size_t(RandomI::random(randomGenerator, 10));

		stackHeapVector.resize(resize0);

		OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), resize0);

		for (size_t n = 0; n < stackHeapVector.size(); ++n)
		{
			if (n < initialSize)
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], std::string("i"));
			}
			else
			{
				OCEAN_EXPECT_TRUE(validation, stackHeapVector[n].empty());
			}
		}

		const size_t resize1 = size_t(RandomI::random(randomGenerator, 10));

		stackHeapVector.resize(resize1);

		OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), resize1);

		for (size_t n = 0; n < stackHeapVector.size(); ++n)
		{
			if (n < std::min(initialSize, resize0))
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], std::string("i"));
			}
			else
			{
				OCEAN_EXPECT_TRUE(validation, stackHeapVector[n].empty());
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testEmplaceBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test emplaceBack with TestElement
		// Note: Due to the use of a default-constructed array for stack storage,
		// emplaceBack must use assignment for stack elements, which adds offsets in TestElement
		{
			StackHeapVector<TestElement, tStackCapacity> stackHeapVector;

			// Emplace elements both in stack and heap regions
			const size_t numberElements = RandomI::random(randomGenerator, 1u, 100u);

			for (size_t n = 0; n < numberElements; ++n)
			{
				stackHeapVector.emplaceBack(n);
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), n + 1);
			}

			// Verify all elements exist
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), numberElements);
		}

		// Test emplaceBack with strings to ensure proper forwarding
		{
			StackHeapVector<std::string, tStackCapacity> stackHeapVector;

			for (size_t n = 0; n < 20; ++n)
			{
				const std::string value = String::toAString(RandomI::random64(randomGenerator));
				std::string& element = stackHeapVector.emplaceBack(value);

				OCEAN_EXPECT_EQUAL(validation, element, value);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testPopBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test that popBack properly clears stack elements
		{
			StackHeapVector<std::string, tStackCapacity> stackHeapVector;

			// Fill the vector
			const size_t maxSize = tStackCapacity + 10;
			for (size_t n = 0; n < maxSize; ++n)
			{
				stackHeapVector.pushBack(String::toAString(n));
			}

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), maxSize);

			// Pop back all heap elements
			while (stackHeapVector.size() > tStackCapacity)
			{
				const size_t oldSize = stackHeapVector.size();
				stackHeapVector.popBack();
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), oldSize - 1);
			}

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), tStackCapacity);

			// Pop back stack elements and verify they're cleared
			for (size_t n = tStackCapacity; n > 0; --n)
			{
				stackHeapVector.popBack();
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), n - 1);
			}

			OCEAN_EXPECT_TRUE(validation, stackHeapVector.isEmpty());

			// Now re-add elements and verify they're properly initialized
			for (size_t n = 0; n < tStackCapacity; ++n)
			{
				stackHeapVector.pushBack(String::toAString(n + 1000));
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], String::toAString(n + 1000));
			}
		}

		// Test popBack with primitive types
		{
			StackHeapVector<uint64_t, tStackCapacity> stackHeapVector;

			for (size_t n = 0; n < tStackCapacity + 5; ++n)
			{
				stackHeapVector.pushBack(n);
			}

			while (!stackHeapVector.isEmpty())
			{
				const size_t expectedSize = stackHeapVector.size() - 1;
				stackHeapVector.popBack();
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), expectedSize);
			}
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testFrontBack(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test front() and back() at various sizes, especially at stack/heap boundary
		for (size_t testSize = 1; testSize <= tStackCapacity + 10; ++testSize)
		{
			StackHeapVector<size_t, tStackCapacity> stackHeapVector;

			for (size_t n = 0; n < testSize; ++n)
			{
				stackHeapVector.pushBack(n);
			}

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.front(), size_t(0));
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.back(), testSize - 1);

			// Only test modification if there's more than one element
			if (testSize > 1)
			{
				// Modify via front() and back()
				stackHeapVector.front() = 999;
				stackHeapVector.back() = 888;

				OCEAN_EXPECT_EQUAL(validation, stackHeapVector.front(), size_t(999));
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector.back(), size_t(888));
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[0], size_t(999));
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[testSize - 1], size_t(888));

				// Test const front() and back()
				const StackHeapVector<size_t, tStackCapacity>& constVector = stackHeapVector;
				OCEAN_EXPECT_EQUAL(validation, constVector.front(), size_t(999));
				OCEAN_EXPECT_EQUAL(validation, constVector.back(), size_t(888));
			}
		}

		// Specifically test the boundary when size == tStackCapacity
		{
			StackHeapVector<std::string, tStackCapacity> stackHeapVector;

			for (size_t n = 0; n < tStackCapacity; ++n)
			{
				stackHeapVector.pushBack(String::toAString(n));
			}

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), tStackCapacity);
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.front(), String::toAString(0));
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.back(), String::toAString(tStackCapacity - 1));
		}

		// Test when size == tStackCapacity + 1 (first element on heap)
		{
			StackHeapVector<std::string, tStackCapacity> stackHeapVector;

			for (size_t n = 0; n < tStackCapacity + 1; ++n)
			{
				stackHeapVector.pushBack(String::toAString(n));
			}

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), tStackCapacity + 1);
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.front(), String::toAString(0));
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.back(), String::toAString(tStackCapacity));
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testReserve(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	RandomGenerator randomGenerator;
	Validation validation(randomGenerator);

	const Timestamp startTimestamp(true);

	do
	{
		// Test reserve before adding elements
		{
			StackHeapVector<uint64_t, tStackCapacity> stackHeapVector;

			const size_t reserveCapacity = RandomI::random(randomGenerator, 1u, 200u);
			stackHeapVector.reserve(reserveCapacity);

			const size_t expectedCapacity = std::max(tStackCapacity, reserveCapacity);
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.capacity(), expectedCapacity);
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), size_t(0));
		}

		// Test reserve with existing elements
		{
			StackHeapVector<uint64_t, tStackCapacity> stackHeapVector;

			const size_t initialSize = RandomI::random(randomGenerator, 1u, 50u);
			for (size_t n = 0; n < initialSize; ++n)
			{
				stackHeapVector.pushBack(n);
			}

			const size_t oldCapacity = stackHeapVector.capacity();
			const size_t reserveCapacity = RandomI::random(randomGenerator, 1u, 200u);

			stackHeapVector.reserve(reserveCapacity);

			const size_t expectedCapacity = std::max({tStackCapacity, reserveCapacity, oldCapacity});
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.capacity(), expectedCapacity);
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), initialSize);

			// Verify elements are unchanged
			for (size_t n = 0; n < initialSize; ++n)
			{
				OCEAN_EXPECT_EQUAL(validation, stackHeapVector[n], uint64_t(n));
			}
		}

		// Test reserve with capacity less than current size (should do nothing)
		{
			StackHeapVector<uint64_t, tStackCapacity> stackHeapVector;

			const size_t initialSize = RandomI::random(randomGenerator, 20u, 50u);
			for (size_t n = 0; n < initialSize; ++n)
			{
				stackHeapVector.pushBack(n);
			}

			const size_t oldCapacity = stackHeapVector.capacity();
			stackHeapVector.reserve(5); // Reserve less than current size

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.capacity(), oldCapacity);
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.size(), initialSize);
		}

		// Test reserve exactly at stack capacity
		{
			StackHeapVector<std::string, tStackCapacity> stackHeapVector;

			stackHeapVector.reserve(tStackCapacity);
			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.capacity(), tStackCapacity);

			// Fill to stack capacity
			for (size_t n = 0; n < tStackCapacity; ++n)
			{
				stackHeapVector.pushBack(String::toAString(n));
			}

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.capacity(), tStackCapacity);
		}

		// Test reserve beyond stack capacity
		{
			StackHeapVector<std::string, tStackCapacity> stackHeapVector;

			const size_t reserveCapacity = tStackCapacity + RandomI::random(randomGenerator, 10u, 50u);
			stackHeapVector.reserve(reserveCapacity);

			OCEAN_EXPECT_EQUAL(validation, stackHeapVector.capacity(), reserveCapacity);
		}
	}
	while (!startTimestamp.hasTimePassed(testDuration));

	return validation.succeeded();
}

template <size_t tStackCapacity>
bool TestStackHeapVector::testPerformance(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	constexpr size_t iterations = 1000;

	Log::info() << "... with stack size " << tStackCapacity << ":";

	RandomGenerator randomGenerator;

	for (const bool stayBelowStackSize : {false, true})
	{
		if (stayBelowStackSize)
		{
			Log::info() << "Staying below stack size:";
		}
		else
		{
			Log::info() << "Extending stack size:";
		}

		HighPerformanceStatistic performanceDefault;
		HighPerformanceStatistic performanceStackHeap;

		const Timestamp startTimestamp(true);

		do
		{
			const size_t size = stayBelowStackSize ? RandomI::random(randomGenerator, 1u, (unsigned int)(tStackCapacity)) : RandomI::random(randomGenerator, (unsigned int)(tStackCapacity + 1u), (unsigned int)(tStackCapacity * 2u));
			const bool emplace = RandomI::boolean(randomGenerator);

			performanceStackHeap.start();

			for (size_t i = 0; i < iterations; ++i)
			{
				StackHeapVector<Frame::Plane, tStackCapacity> stackHeapVector;

				if (emplace)
				{
					for (size_t n = 0; n < size; ++n)
					{
						stackHeapVector.emplaceBack(Frame::Plane());
					}
				}
				else
				{
					for (size_t n = 0; n < size; ++n)
					{
						stackHeapVector.pushBack(Frame::Plane());
					}
				}
			}

			performanceStackHeap.stop();


			performanceDefault.start();

			for (size_t i = 0; i < iterations; ++i)
			{
				std::vector<Frame::Plane> vector;

				if (emplace)
				{
					for (size_t n = 0; n < size; ++n)
					{
						vector.emplace_back(Frame::Plane());
					}
				}
				else
				{
					for (size_t n = 0; n < size; ++n)
					{
						vector.push_back(Frame::Plane());
					}
				}
			}

			performanceDefault.stop();
		}
		while (!startTimestamp.hasTimePassed(testDuration));

		Log::info() << "Default performance: " << performanceDefault;
		Log::info() << "Stack-heap performance: " << performanceStackHeap;
	}

	Log::info() << " ";

	return true;
}

}

}

}
