/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestInheritance.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include <vector>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

TestInheritance::NormalBaseClass::NormalBaseClass(const double value) :
	classValue_(value)
{
	// nothing to do here
}

double TestInheritance::NormalBaseClass::baseFunction0(const double value)
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::NormalBaseClass::baseFunction1(const double value)
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

double TestInheritance::NormalBaseClass::constBaseFunction0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::NormalBaseClass::constBaseFunction1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

TestInheritance::NormalSubClass::NormalSubClass(const double value) :
	NormalBaseClass(value)
{
	// nothing to do here
}

double TestInheritance::NormalSubClass::subFunction0(const double value)
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::NormalSubClass::subFunction1(const double value)
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

TestInheritance::DiamondBaseClass::DiamondBaseClass(const DiamondBaseClass& object) :
	classValue_(object.classValue_)
{
	// nothing do do here
}

TestInheritance::DiamondBaseClass::DiamondBaseClass(const double value) :
	classValue_(value)
{
	// nothing to do here
}

double TestInheritance::DiamondBaseClass::baseFunction0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::DiamondBaseClass::baseFunction1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

double TestInheritance::DiamondBaseClass::virtualFunction0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::DiamondBaseClass::virtualFunction1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

TestInheritance::DiamondSubClass0::DiamondSubClass0(const DiamondSubClass0& object) :
	DiamondBaseClass(object)
{
	// nothing to do here
}

TestInheritance::DiamondSubClass0::DiamondSubClass0(const double value) :
	DiamondBaseClass(value)
{
	// nothing to do here
}

double TestInheritance::DiamondSubClass0::subFunction0_0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::DiamondSubClass0::subFunction0_1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

TestInheritance::DiamondSubClass1::DiamondSubClass1(const DiamondSubClass1& object) :
	DiamondBaseClass(object)
{
	// nothing to do here
}

TestInheritance::DiamondSubClass1::DiamondSubClass1(const double value) :
	DiamondBaseClass(value)
{
	// nothing to do here
}

double TestInheritance::DiamondSubClass1::subFunction1_0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::DiamondSubClass1::subFunction1_1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

double TestInheritance::DiamondSubClass1::virtualFunction0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::DiamondSubClass1::virtualFunction1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

TestInheritance::DiamondSubClass::DiamondSubClass(const TestInheritance::DiamondSubClass& object) :
	DiamondBaseClass(object),
	DiamondSubClass0(object),
	DiamondSubClass1(object)
{
	// nothing to do here
}

TestInheritance::DiamondSubClass::DiamondSubClass(const double value) :
	DiamondBaseClass(value),
	DiamondSubClass0(value),
	DiamondSubClass1(value)
{
	// nothing to do here
}

double TestInheritance::DiamondSubClass::subFunction0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::DiamondSubClass::subFunction1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

double TestInheritance::DiamondSubClass::virtualFunction0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::DiamondSubClass::virtualFunction1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

TestInheritance::VirtualBaseClass::VirtualBaseClass(const double value) :
	classValue_(value)
{
	// nothing to do here
}

double TestInheritance::VirtualBaseClass::constFunction0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::VirtualBaseClass::constFunction1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

double TestInheritance::VirtualBaseClass::virtualBaseFunction0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::VirtualBaseClass::virtualBaseFunction1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

double TestInheritance::VirtualBaseClass::virtualFunction0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::VirtualBaseClass::virtualFunction1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

TestInheritance::VirtualSubClass::VirtualSubClass(const double value) :
	VirtualBaseClass(value)
{
	// nothing to do here
}

double TestInheritance::VirtualSubClass::constFunction0(const double value)
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::VirtualSubClass::constFunction1(const double value)
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

double TestInheritance::VirtualSubClass::virtualFunction0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

double TestInheritance::VirtualSubClass::virtualFunction1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

bool TestInheritance::test(const double testDuration)
{
	Log::info() << "---   Inheritance test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testNormal(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testVirtual(testDuration) && allSucceeded;

	Log::info() << " ";

	allSucceeded = testDiamond(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "Inheritance test succeeded.";
	}
	else
	{
		Log::info() << "Inheritance test FAILED!";
	}

	return allSucceeded;
}
	
#ifdef OCEAN_USE_GTEST
	
TEST(TestInheritance, Normal)
{
	EXPECT_TRUE(TestInheritance::testNormal(GTEST_TEST_DURATION));
}
	
TEST(TestInheritance, Virtual)
{
	EXPECT_TRUE(TestInheritance::testVirtual(GTEST_TEST_DURATION));
}
	
TEST(TestInheritance, Diamond)
{
	EXPECT_TRUE(TestInheritance::testDiamond(GTEST_TEST_DURATION));
}
	
#endif // OCEAN_USE_GTEST

bool TestInheritance::testNormal(const double testDuration)
{
	Log::info() << "Normal class without virtual functions test:";
	Log::info() << "Size of base class: " << sizeof(NormalBaseClass) << " bytes";
	Log::info() << "Size of sub class: " << sizeof(NormalSubClass) << " bytes";

	typedef std::vector<NormalBaseClass> BaseClassObjects;
	typedef std::vector<NormalSubClass> SubClassObjects;

	typedef std::vector<NormalBaseClass*> BaseClassPointers;
	typedef std::vector<NormalSubClass*> SubClassPointers;

	const size_t size = 1000000;

	BaseClassObjects baseClassObjects(size);
	SubClassObjects subClassObjects(size);

	BaseClassPointers baseClassPointers(size);
	SubClassPointers subClassPointers(size);

	for (size_t n = 0; n < size; ++n)
	{
		baseClassObjects[n] = NormalBaseClass(double(size));
		subClassObjects[n] = NormalSubClass(double(size));

		baseClassPointers[n] = new NormalBaseClass(double(size));
		subClassPointers[n] = new NormalSubClass(double(size));
	}

	HighPerformanceStatistic performanceBaseObjects0, performanceBaseObjects1;
	HighPerformanceStatistic performanceConstBaseObjects0, performanceConstBaseObjects1;
	HighPerformanceStatistic performanceInlineBaseObjects0, performanceInlineBaseObjects1;

	HighPerformanceStatistic performanceSubObjects0, performanceSubObjects1;
	HighPerformanceStatistic performanceConstSubObjects0, performanceConstSubObjects1;
	HighPerformanceStatistic performanceInlineSubObjects0, performanceInlineSubObjects1;

	HighPerformanceStatistic performanceBasePointers0, performanceBasePointers1;
	HighPerformanceStatistic performanceConstBasePointers0, performanceConstBasePointers1;
	HighPerformanceStatistic performanceInlineBasePointers0, performanceInlineBasePointers1;

	HighPerformanceStatistic performanceSubPointers0, performanceSubPointers1;
	HighPerformanceStatistic performanceConstSubPointers0, performanceConstSubPointers1;
	HighPerformanceStatistic performanceInlineSubPointers0, performanceInlineSubPointers1;

	Timestamp startTimestamp(true);

	double parameter = 0.0;
	double result = 0.0;

	do
	{
		// Base class
		performanceBaseObjects0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += baseClassObjects[n].baseFunction0(parameter);
		}
		performanceBaseObjects0.stop();
		result *= 0.00000001;

		performanceBaseObjects1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += baseClassObjects[n].baseFunction1(parameter);
		}
		performanceBaseObjects1.stop();
		result *= 0.00000001;

		performanceConstBaseObjects0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += baseClassObjects[n].constBaseFunction0(parameter);
		}
		performanceConstBaseObjects0.stop();
		result *= 0.00000001;

		performanceConstBaseObjects1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += baseClassObjects[n].constBaseFunction1(parameter);
		}
		performanceConstBaseObjects1.stop();
		result *= 0.00000001;

		performanceInlineBaseObjects0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += baseClassObjects[n].inlineBaseFunction0(parameter);
		}
		performanceInlineBaseObjects0.stop();
		result *= 0.00000001;

		performanceInlineBaseObjects1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += baseClassObjects[n].inlineBaseFunction1(parameter);
		}
		performanceInlineBaseObjects1.stop();
		result *= 0.00000001;


		// Sub class
		performanceSubObjects0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].subFunction0(parameter);
		}
		performanceSubObjects0.stop();
		result *= 0.00000001;

		performanceSubObjects1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].subFunction1(parameter);
		}
		performanceSubObjects1.stop();
		result *= 0.00000001;

		performanceConstSubObjects0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].constBaseFunction0(parameter);
		}
		performanceConstSubObjects0.stop();
		result *= 0.00000001;

		performanceConstSubObjects1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].constBaseFunction1(parameter);
		}
		performanceConstSubObjects1.stop();

		result *= 0.00000001;
		performanceInlineSubObjects0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].inlineSubFunction0(parameter);
		}
		performanceInlineSubObjects0.stop();
		result *= 0.00000001;

		performanceInlineSubObjects1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].inlineSubFunction1(parameter);
		}
		performanceInlineSubObjects1.stop();
		result *= 0.00000001;


		// Base class (pointer)
		performanceBasePointers0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += baseClassPointers[n]->baseFunction0(parameter);
		}
		performanceBasePointers0.stop();
		result *= 0.00000001;

		performanceBasePointers1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += baseClassPointers[n]->baseFunction1(parameter);
		}
		performanceBasePointers1.stop();
		result *= 0.00000001;

		performanceConstBasePointers0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += baseClassPointers[n]->constBaseFunction0(parameter);
		}
		performanceConstBasePointers0.stop();
		result *= 0.00000001;

		performanceConstBasePointers1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += baseClassPointers[n]->constBaseFunction1(parameter);
		}
		performanceConstBasePointers1.stop();
		result *= 0.00000001;

		performanceInlineBasePointers0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += baseClassPointers[n]->inlineBaseFunction0(parameter);
		}
		performanceInlineBasePointers0.stop();
		result *= 0.00000001;

		performanceInlineBasePointers1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += baseClassPointers[n]->inlineBaseFunction1(parameter);
		}
		performanceInlineBasePointers1.stop();
		result *= 0.00000001;


		// Sub class (pointers)
		performanceSubPointers0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->subFunction0(parameter);
		}
		performanceSubPointers0.stop();
		result *= 0.00000001;

		performanceSubPointers1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->subFunction1(parameter);
		}
		performanceSubPointers1.stop();
		result *= 0.00000001;

		performanceConstSubPointers0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->constBaseFunction0(parameter);
		}
		performanceConstSubPointers0.stop();
		result *= 0.00000001;

		performanceConstSubPointers1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->constBaseFunction1(parameter);
		}
		performanceConstSubPointers1.stop();

		result *= 0.00000001;
		performanceInlineSubPointers0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->inlineSubFunction0(parameter);
		}
		performanceInlineSubPointers0.stop();
		result *= 0.00000001;

		performanceInlineSubPointers1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->inlineSubFunction1(parameter);
		}
		performanceInlineSubPointers1.stop();
		result *= 0.00000001;


		parameter += 1.5;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (result > 0 || result <= 0)
	{
		Log::info() << "Objects with " << size << " function calls:";

		Log::info() << "Fast baseclass call:        " << performanceBaseObjects1.averageMseconds() << "ms";
		Log::info() << "Fast baseclass const call:  " << performanceConstBaseObjects1.averageMseconds() << "ms";
		Log::info() << "Fast baseclass inline call: " << performanceInlineBaseObjects1.averageMseconds() << "ms";
		Log::info() << "Fast subclass call:         " << performanceSubObjects1.averageMseconds() << "ms";
		Log::info() << "Fast subclass const call:   " << performanceConstSubObjects1.averageMseconds() << "ms";
		Log::info() << "Fast subclass inline call:  " << performanceInlineSubObjects1.averageMseconds() << "ms";

		Log::info() << "Slow baseclass call:        " << performanceBaseObjects0.averageMseconds() << "ms";
		Log::info() << "Slow baseclass const call:  " << performanceConstBaseObjects0.averageMseconds() << "ms";
		Log::info() << "Slow baseclass inline call: " << performanceInlineBaseObjects0.averageMseconds() << "ms";
		Log::info() << "Slow subclass call:         " << performanceSubObjects0.averageMseconds() << "ms";
		Log::info() << "Slow subclass const call:   " << performanceConstSubObjects0.averageMseconds() << "ms";
		Log::info() << "Slow subclass inline call:  " << performanceInlineSubObjects0.averageMseconds() << "ms";

		Log::info() << "Pointers with " << size << " function calls:";

		Log::info() << "Fast baseclass call:        " << performanceBasePointers1.averageMseconds() << "ms";
		Log::info() << "Fast baseclass const call:  " << performanceConstBasePointers1.averageMseconds() << "ms";
		Log::info() << "Fast baseclass inline call: " << performanceInlineBasePointers1.averageMseconds() << "ms";
		Log::info() << "Fast subclass call:         " << performanceSubPointers1.averageMseconds() << "ms";
		Log::info() << "Fast subclass const call:   " << performanceConstSubPointers1.averageMseconds() << "ms";
		Log::info() << "Fast subclass inline call:  " << performanceInlineSubPointers1.averageMseconds() << "ms";

		Log::info() << "Slow baseclass call:        " << performanceBasePointers0.averageMseconds() << "ms";
		Log::info() << "Slow baseclass const call:  " << performanceConstBasePointers0.averageMseconds() << "ms";
		Log::info() << "Slow baseclass inline call: " << performanceInlineBasePointers0.averageMseconds() << "ms";
		Log::info() << "Slow subclass call:         " << performanceSubPointers0.averageMseconds() << "ms";
		Log::info() << "Slow subclass const call:   " << performanceConstSubPointers0.averageMseconds() << "ms";
		Log::info() << "Slow subclass inline call:  " << performanceInlineSubPointers0.averageMseconds() << "ms";
	}

	for (size_t n = 0; n < size; ++n)
	{
		delete baseClassPointers[n];
		delete subClassPointers[n];
	}

	return true;
}

bool TestInheritance::testVirtual(const double testDuration)
{
	Log::info() << "Class with virtual functions test:";
	Log::info() << "Size of base class: " << sizeof(VirtualBaseClass) << " bytes";
	Log::info() << "Size of sub class: " << sizeof(VirtualSubClass) << " bytes";

	typedef std::vector<VirtualSubClass> VirtualClassObjects;
	typedef std::vector<VirtualBaseClass*> VirtualClassPointers;

	const size_t size = 1000000;

	VirtualClassObjects virtualClassObjects(size);
	VirtualClassPointers virtualClassPointers(size);

	for (size_t n = 0; n < size; ++n)
	{
		virtualClassObjects[n] = VirtualSubClass(double(size));

		virtualClassPointers[n] = (n % 2u == 0u) ? new VirtualSubClass(double(size)) : new VirtualBaseClass(double(size));
	}

	HighPerformanceStatistic performanceConstObjects0, performanceConstObjects1;
	HighPerformanceStatistic performanceBaseObjects0, performanceBaseObjects1;
	HighPerformanceStatistic performanceVirtualObjects0, performanceVirtualObjects1;

	HighPerformanceStatistic performanceConstPointers0, performanceConstPointers1;
	HighPerformanceStatistic performanceBasePointers0, performanceBasePointers1;
	HighPerformanceStatistic performanceVirtualPointers0, performanceVirtualPointers1;

	Timestamp startTimestamp(true);

	double parameter = 0.0;
	double result = 0;

	do
	{
		// Objects
		performanceConstObjects0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += virtualClassObjects[n].constFunction0(parameter);
		}
		performanceConstObjects0.stop();
		result *= 0.00000001;

		performanceConstObjects1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += virtualClassObjects[n].constFunction1(parameter);
		}
		performanceConstObjects1.stop();
		result *= 0.00000001;

		performanceBaseObjects0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += virtualClassObjects[n].virtualBaseFunction0(parameter);
		}
		performanceBaseObjects0.stop();
		result *= 0.00000001;

		performanceBaseObjects1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += virtualClassObjects[n].virtualBaseFunction1(parameter);
		}
		performanceBaseObjects1.stop();
		result *= 0.00000001;

		performanceVirtualObjects0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += virtualClassObjects[n].virtualFunction0(parameter);
		}
		performanceVirtualObjects0.stop();
		result *= 0.00000001;

		performanceVirtualObjects1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += virtualClassObjects[n].virtualFunction1(parameter);
		}
		performanceVirtualObjects1.stop();
		result *= 0.00000001;


		// Pointers
		performanceConstPointers0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += virtualClassPointers[n]->constFunction0(parameter);
		}
		performanceConstPointers0.stop();
		result *= 0.00000001;

		performanceConstPointers1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += virtualClassPointers[n]->constFunction1(parameter);
		}
		performanceConstPointers1.stop();
		result *= 0.00000001;

		performanceBasePointers0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += virtualClassPointers[n]->virtualBaseFunction0(parameter);
		}
		performanceBasePointers0.stop();
		result *= 0.00000001;

		performanceBasePointers1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += virtualClassPointers[n]->virtualBaseFunction1(parameter);
		}
		performanceBasePointers1.stop();
		result *= 0.00000001;

		performanceVirtualPointers0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += virtualClassPointers[n]->virtualFunction0(parameter);
		}
		performanceVirtualPointers0.stop();
		result *= 0.00000001;

		performanceVirtualPointers1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += virtualClassPointers[n]->virtualFunction1(parameter);
		}
		performanceVirtualPointers1.stop();
		result *= 0.00000001;


		parameter += 1.5;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (result > 0 || result <= 0)
	{
		Log::info() << "Objects with " << size << " function calls:";

		Log::info() << "Fast base call:    " << performanceBaseObjects1.averageMseconds() << "ms";
		Log::info() << "Fast const call:   " << performanceConstObjects1.averageMseconds() << "ms";
		Log::info() << "Fast virtual call: " << performanceVirtualObjects1.averageMseconds() << "ms";

		Log::info() << "Slow base call:    " << performanceBaseObjects0.averageMseconds() << "ms";
		Log::info() << "Slow const call:   " << performanceConstObjects0.averageMseconds() << "ms";
		Log::info() << "Slow virtual call: " << performanceVirtualObjects0.averageMseconds() << "ms";

		Log::info() << "Pointers with " << size << " function calls:";

		Log::info() << "Fast base call:    " << performanceBasePointers1.averageMseconds() << "ms";
		Log::info() << "Fast const call:   " << performanceConstPointers1.averageMseconds() << "ms";
		Log::info() << "Fast virtual call: " << performanceVirtualPointers1.averageMseconds() << "ms";

		Log::info() << "Slow base call:    " << performanceBasePointers0.averageMseconds() << "ms";
		Log::info() << "Slow const call:   " << performanceConstPointers0.averageMseconds() << "ms";
		Log::info() << "Slow virtual call: " << performanceVirtualPointers0.averageMseconds() << "ms";
	}

	for (size_t n = 0; n < size; ++n)
	{
		delete virtualClassPointers[n];
	}

	return true;
}

bool TestInheritance::testDiamond(const double testDuration)
{
	Log::info() << "Diamond class with virtual functions test:";
	Log::info() << "Size of base class: " << sizeof(DiamondBaseClass) << " bytes";
	Log::info() << "Size of second subclasses: " << sizeof(DiamondSubClass0) << ", " << sizeof(DiamondSubClass1) << " bytes";
	Log::info() << "Size of final subclass: " << sizeof(DiamondSubClass) << " bytes";

	typedef std::vector<DiamondSubClass> SubClassObjects;
	typedef std::vector<DiamondSubClass*> SubClassPointers;

	const size_t size = 1000000;

	SubClassObjects subClassObjects(size);
	SubClassPointers subClassPointers(size);

	for (size_t n = 0; n < size; ++n)
	{
		subClassObjects[n] = DiamondSubClass(double(size));
		subClassPointers[n] = new DiamondSubClass(double(size));
	}

	HighPerformanceStatistic performanceBaseObjects0, performanceBaseObjects1;
	HighPerformanceStatistic performanceSubObjects0_0, performanceSubObjects0_1;
	HighPerformanceStatistic performanceSubObjects1_0, performanceSubObjects1_1;
	HighPerformanceStatistic performanceSubObjects0, performanceSubObjects1;
	HighPerformanceStatistic performanceVirtualObjects0, performanceVirtualObjects1;

	HighPerformanceStatistic performanceBasePointers0, performanceBasePointers1;
	HighPerformanceStatistic performanceSubPointers0_0, performanceSubPointers0_1;
	HighPerformanceStatistic performanceSubPointers1_0, performanceSubPointers1_1;
	HighPerformanceStatistic performanceSubPointers0, performanceSubPointers1;
	HighPerformanceStatistic performanceVirtualPointers0, performanceVirtualPointers1;

	Timestamp startTimestamp(true);

	double parameter = 0.0;
	double result = 0;

	do
	{
		// Objects
		performanceBaseObjects0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].baseFunction0(parameter);
		}
		performanceBaseObjects0.stop();
		result *= 0.00000001;

		performanceBaseObjects1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].baseFunction1(parameter);
		}
		performanceBaseObjects1.stop();
		result *= 0.00000001;

		performanceSubObjects0_0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].subFunction0_0(parameter);
		}
		performanceSubObjects0_0.stop();
		result *= 0.00000001;

		performanceSubObjects0_1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].subFunction0_1(parameter);
		}
		performanceSubObjects0_1.stop();
		result *= 0.00000001;

		performanceSubObjects1_0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].subFunction1_0(parameter);
		}
		performanceSubObjects1_0.stop();
		result *= 0.00000001;

		performanceSubObjects1_1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].subFunction1_1(parameter);
		}
		performanceSubObjects1_1.stop();
		result *= 0.00000001;

		performanceSubObjects0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].subFunction0(parameter);
		}
		performanceSubObjects0.stop();
		result *= 0.00000001;

		performanceSubObjects1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].subFunction1(parameter);
		}
		performanceSubObjects1.stop();
		result *= 0.00000001;

		performanceVirtualObjects0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].virtualFunction0(parameter);
		}
		performanceVirtualObjects0.stop();
		result *= 0.00000001;

		performanceVirtualObjects1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassObjects[n].virtualFunction1(parameter);
		}
		performanceVirtualObjects1.stop();
		result *= 0.00000001;


		// Pointers
		performanceBasePointers0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->baseFunction0(parameter);
		}
		performanceBasePointers0.stop();
		result *= 0.00000001;

		performanceBasePointers1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->baseFunction1(parameter);
		}
		performanceBasePointers1.stop();
		result *= 0.00000001;

		performanceSubPointers0_0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->subFunction0_0(parameter);
		}
		performanceSubPointers0_0.stop();
		result *= 0.00000001;

		performanceSubPointers0_1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->subFunction0_1(parameter);
		}
		performanceSubPointers0_1.stop();
		result *= 0.00000001;

		performanceSubPointers1_0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->subFunction1_0(parameter);
		}
		performanceSubPointers1_0.stop();
		result *= 0.00000001;

		performanceSubPointers1_1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->subFunction1_1(parameter);
		}
		performanceSubPointers1_1.stop();
		result *= 0.00000001;

		performanceSubPointers0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->subFunction0(parameter);
		}
		performanceSubPointers0.stop();
		result *= 0.00000001;

		performanceSubPointers1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += subClassPointers[n]->subFunction1(parameter);
		}
		performanceSubPointers1.stop();
		result *= 0.00000001;

		performanceVirtualPointers0.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += ((DiamondSubClass*)subClassPointers[n])->virtualFunction0(parameter);
		}
		performanceVirtualPointers0.stop();
		result *= 0.00000001;

		performanceVirtualPointers1.start();
		for (size_t n = 0; n < size; ++n)
		{
			result += ((DiamondSubClass*)subClassPointers[n])->virtualFunction1(parameter);
		}
		performanceVirtualPointers1.stop();
		result *= 0.00000001;


		parameter += 1.5;
	}
	while (startTimestamp + testDuration > Timestamp(true));

	if (result > 0 || result <= 0)
	{
		Log::info() << "Objects with " << size << " function calls:";

		Log::info() << "Fast baseclass call: " << performanceBaseObjects1.averageMseconds() << "ms";
		Log::info() << "Fast subclass0 call: " << performanceSubObjects0_1.averageMseconds() << "ms";
		Log::info() << "Fast subclass1 call: " << performanceSubObjects1_1.averageMseconds() << "ms";
		Log::info() << "Fast subclass call:  " << performanceSubObjects1.averageMseconds() << "ms";
		Log::info() << "Fast virtual call:   " << performanceVirtualObjects1.averageMseconds() << "ms";

		Log::info() << "Slow baseclass call: " << performanceBaseObjects0.averageMseconds() << "ms";
		Log::info() << "Slow subclass0 call: " << performanceSubObjects0_0.averageMseconds() << "ms";
		Log::info() << "Slow subclass1 call: " << performanceSubObjects1_0.averageMseconds() << "ms";
		Log::info() << "Slow subclass call:  " << performanceSubObjects0.averageMseconds() << "ms";
		Log::info() << "Slow virtual call:   " << performanceVirtualObjects0.averageMseconds() << "ms";

		Log::info() << "Pointers with " << size << " function calls:";

		Log::info() << "Fast baseclass call: " << performanceBasePointers1.averageMseconds() << "ms";
		Log::info() << "Fast subclass0 call: " << performanceSubPointers0_1.averageMseconds() << "ms";
		Log::info() << "Fast subclass1 call: " << performanceSubPointers1_1.averageMseconds() << "ms";
		Log::info() << "Fast subclass call:  " << performanceSubPointers1.averageMseconds() << "ms";
		Log::info() << "Fast virtual call:   " << performanceVirtualPointers1.averageMseconds() << "ms";

		Log::info() << "Slow baseclass call: " << performanceBasePointers0.averageMseconds() << "ms";
		Log::info() << "Slow subclass0 call: " << performanceSubPointers0_0.averageMseconds() << "ms";
		Log::info() << "Slow subclass1 call: " << performanceSubPointers1_0.averageMseconds() << "ms";
		Log::info() << "Slow subclass call:  " << performanceSubPointers0.averageMseconds() << "ms";
		Log::info() << "Slow virtual call:   " << performanceVirtualPointers0.averageMseconds() << "ms";
	}

	for (size_t n = 0; n < size; ++n)
	{
		delete subClassPointers[n];
	}

	return true;
}

}

}

}
