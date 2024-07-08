/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_INHERITANCE_H
#define META_OCEAN_TEST_TESTBASE_TEST_INHERITANCE_H

#include "ocean/test/testbase/TestBase.h"

#include <cmath>

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements an inheritance test of classes.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestInheritance
{
	private:

		/**
		 * This class implements a standard base class.
		 */
		class NormalBaseClass
		{
			public:

				/**
				 * Creates a new object.
				 */
				NormalBaseClass() = default;

				/**
				 * Creates a new object.
				 * @param value Value to be assigned to this object
				 */
				explicit NormalBaseClass(const double value);

				/**
				 * Destructs the object.
				 */
				virtual ~NormalBaseClass() = default;

				/**
				 * Non-constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double baseFunction0(const double value);

				/**
				 * Non-constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double baseFunction1(const double value);

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double constBaseFunction0(const double value) const;

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double constBaseFunction1(const double value) const;

				/**
				 * Inline function.
				 * @param value Function parameter
				 * @return Function result
				 */
				inline double inlineBaseFunction0(const double value) const;

				/**
				 * Inline function.
				 * @param value Function parameter
				 * @return Function result
				 */
				inline double inlineBaseFunction1(const double value) const;

			protected:

				/// Object value.
				double classValue_ = -1.0;
		};

		/**
		 * This class implements a standard sub class.
		 */
		class NormalSubClass final : public NormalBaseClass
		{
			public:

				/**
				 * Creates a new object.
				 */
				NormalSubClass() = default;

				/**
				 * Destructs the object.
				 */
				~NormalSubClass() override = default;

				/**
				 * Creates a new object.
				 * @param value Value to be assigned
				 */
				explicit NormalSubClass(const double value);

				/**
				 * Non-constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double subFunction0(const double value);

				/**
				 * Non-constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double subFunction1(const double value);

				/**
				 * Inline function.
				 * @param value Function parameter
				 * @return Function result
				 */
				inline double inlineSubFunction0(const double value) const;

				/**
				 * Inline function.
				 * @param value Function parameter
				 * @return Function result
				 */
				inline double inlineSubFunction1(const double value) const;
		};


		/**
		 * This class implements a base class for a diamond derivation.
		 */
		class DiamondBaseClass
		{
			public:

				/**
				 * Creates a new object.
				 */
				DiamondBaseClass() = default;

				/**
				 * Copy constructor.
				 * @param object Object to be copied
				 */
				DiamondBaseClass(const DiamondBaseClass& object);

				/**
				 * Creates a new object.
				 * @param value Value to be assigned
				 */
				explicit DiamondBaseClass(const double value);

				/**
				 * Destructs the object.
				 */
				virtual ~DiamondBaseClass() = default;

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double baseFunction0(const double value) const;

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double baseFunction1(const double value) const;

				/**
				 * Virtual function.
				 * @param value Function parameter
				 * @return Function result
				 */
				virtual double virtualFunction0(const double value) const;

				/**
				 * Virtual function.
				 * @param value Function parameter
				 * @return Function result
				 */
				virtual double virtualFunction1(const double value) const;

			protected:

				/// Object value.
				double classValue_ = -1.0;
		};

		/**
		 * This class implements an intermediate sub-class for a diamond derivation.
		 */
		class DiamondSubClass0 : virtual public DiamondBaseClass
		{
			public:

				/**
				 * Creates a new object.
				 */
				DiamondSubClass0() = default;

				/**
				 * Copy constructor.
				 * @param object Object to be copied
				 */
				DiamondSubClass0(const DiamondSubClass0& object);

				/**
				 * Creates a new object.
				 * @param value Value to be assigned
				 */
				explicit DiamondSubClass0(const double value);

				/**
				 * Destructs the object.
				 */
				~DiamondSubClass0() override = default;

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double subFunction0_0(const double value) const;

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double subFunction0_1(const double value) const;
		};

		/**
		 * This class implements a second intermediate sub-class for a diamond derivation.
		 */
		class DiamondSubClass1 : virtual public DiamondBaseClass
		{
			public:

				/**
				 * Creates a new object.
				 */
				DiamondSubClass1() = default;

				/**
				 * Copy constructor.
				 * @param object Object to be copied
				 */
				DiamondSubClass1(const DiamondSubClass1& object);

				/**
				 * Creates a new object.
				 * @param value Value to be assigned
				 */
				explicit DiamondSubClass1(const double value);

				/**
				 * Destructs the object.
				 */
				~DiamondSubClass1() override = default;

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double subFunction1_0(const double value) const;

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double subFunction1_1(const double value) const;

				/**
				 * Virtual function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double virtualFunction0(const double value) const override;

				/**
				 * Virtual function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double virtualFunction1(const double value) const override;
		};

		/**
		 * This class implements the final subclass for a diamond derivation.
		 */
		class DiamondSubClass final :
			public DiamondSubClass0,
			public DiamondSubClass1
		{
			public:

				/**
				 * Creates a new object.
				 */
				DiamondSubClass() = default;

				/**
				 * Copy constructor.
				 * @param object Object to be copied
				 */
				DiamondSubClass(const DiamondSubClass& object);

				/**
				 * Creates a new object.
				 * @param value Value to be assigned
				 */
				explicit DiamondSubClass(const double value);

				/**
				 * Destructs the object.
				 */
				~DiamondSubClass() override = default;

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double subFunction0(const double value) const;

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double subFunction1(const double value) const;

				/**
				 * Virtual function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double virtualFunction0(const double value) const override;

				/**
				 * Virtual function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double virtualFunction1(const double value) const override;
		};

		/**
		 * This class implements standard base class defining virtual functions.
		 */
		class VirtualBaseClass
		{
			public:

				/**
				 * Creates a new object.
				 */
				VirtualBaseClass() = default;

				/**
				 * Creates a new object.
				 * @param value Value to be assigned
				 */
				explicit VirtualBaseClass(const double value);

				/**
				 * Destructs the object.
				 */
				virtual ~VirtualBaseClass() = default;

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double constFunction0(const double value) const;

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double constFunction1(const double value) const;

				/**
				 * Virtual function.
				 * @param value Function parameter
				 * @return Function result
				 */
				virtual double virtualBaseFunction0(const double value) const;

				/**
				 * Virtual function.
				 * @param value Function parameter
				 * @return Function result
				 */
				virtual double virtualBaseFunction1(const double value) const;

				/**
				 * Virtual function.
				 * @param value Function parameter
				 * @return Function result
				 */
				virtual double virtualFunction0(const double value) const;

				/**
				 * Virtual function.
				 * @param value Function parameter
				 * @return Function result
				 */
				virtual double virtualFunction1(const double value) const;

			protected:

				/// Object value.
				double classValue_ = -1.0;
		};

		/**
		 * This class implements sub-class defining virtual functions.
		 */
		class VirtualSubClass final : public VirtualBaseClass
		{
			public:

				/**
				 * Creates a new object.
				 */
				VirtualSubClass() = default;

				/**
				 * Creates a new object.
				 * @param value Value to be assigned
				 */
				explicit VirtualSubClass(const double value);

				/**
				 * Destructs the object.
				 */
				~VirtualSubClass() override = default;

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double constFunction0(const double value);

				/**
				 * Constant function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double constFunction1(const double value);

				/**
				 * Virtual function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double virtualFunction0(const double value) const override;

				/**
				 * Virtual function.
				 * @param value Function parameter
				 * @return Function result
				 */
				double virtualFunction1(const double value) const override;
		};

	public:

		/**
		 * Tests all inheritance functions.
		 * @param testDuration Number of test seconds, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool test(const double testDuration);

		/**
		 * Tests the performance of the normal class.
		 * @param testDuration Number of test seconds, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testNormal(const double testDuration);

		/**
		 * Tests the performance of the virtual class.
		 * @param testDuration Number of test seconds, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testVirtual(const double testDuration);

		/**
		 * Tests the performance of the diamond class.
		 * @param testDuration Number of test seconds, with range (0, infinity)
		 * @return True, if succeeded
		 */
		static bool testDiamond(const double testDuration);
};

inline double TestInheritance::NormalBaseClass::inlineBaseFunction0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

inline double TestInheritance::NormalBaseClass::inlineBaseFunction1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

inline double TestInheritance::NormalSubClass::inlineSubFunction0(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sin(classValue_) * cos(value) * sqrt(value);
}

inline double TestInheritance::NormalSubClass::inlineSubFunction1(const double value) const
{
	ocean_assert(classValue_ >= 0.0);
	return sqrt(value) * classValue_;
}

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_INHERITANCE_H
