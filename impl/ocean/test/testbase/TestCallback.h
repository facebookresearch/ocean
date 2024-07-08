/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TEST_TESTBASE_TEST_CALLBACK_H
#define META_OCEAN_TEST_TESTBASE_TEST_CALLBACK_H

#include "ocean/test/testbase/TestBase.h"

#include "ocean/base/Callback.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

/**
 * This class implements a callback test.
 * @ingroup testbase
 */
class OCEAN_TEST_BASE_EXPORT TestCallback
{
	protected:

		/**
		 * This class implements a helper class holding member functions.
		 */
		class Object
		{
			public:

				/**
				 * Member function with no parameter.
				 */
				void function0();

				/**
				 * Member function with one parameter.
				 * @param p0 First parameter
				 */
				void function1(int p0);

				/**
				 * Member function with two parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 */
				void function2(int p0, int p1);

				/**
				 * Member function with three parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 */
				void function3(int p0, int p1, int p2);

				/**
				 * Member function with four parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 */
				void function4(int p0, int p1, int p2, int p3);

				/**
				 * Member function with five parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @param p4 Fifth parameter
				 */
				void function5(int p0, int p1, int p2, int p3, int p4);

				/**
				 * Member function with six parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @param p4 Fifth parameter
				 * @param p5 Sixth parameter
				 */
				void function6(int p0, int p1, int p2, int p3, int p4, int p5);

				/**
				 * Member function with seven parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @param p4 Fifth parameter
				 * @param p5 Sixth parameter
				 * @param p6 Seventh parameter
				 */
				void function7(int p0, int p1, int p2, int p3, int p4, int p5, int p6);

				/**
				 * Member function with eight parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @param p4 Fifth parameter
				 * @param p5 Sixth parameter
				 * @param p6 Seventh parameter
				 * @param p7 Eighth parameter
				 */
				void function8(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7);

				/**
				 * Member function with nine parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @param p4 Fifth parameter
				 * @param p5 Sixth parameter
				 * @param p6 Seventh parameter
				 * @param p7 Eighth parameter
				 * @param p8 Ninth parameter
				 */
				void function9(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8);

				/**
				 * Member function with ten parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @param p4 Fifth parameter
				 * @param p5 Sixth parameter
				 * @param p6 Seventh parameter
				 * @param p7 Eighth parameter
				 * @param p8 Ninth parameter
				 * @param p9 Tenth parameter
				 */
				void function10(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9);

				/**
				 * Member function with 11 parameter.
				 */
				void function11(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10);

				/**
				 * Member function with 12 parameter.
				 */
				void function12(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11);

				/**
				 * Member function with 13 parameter.
				 */
				void function13(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12);

				/**
				 * Member function with 14 parameter.
				 */
				void function14(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13);

				/**
				 * Member function with 15 parameter.
				 */
				void function15(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14);

				/**
				 * Member function with 16 parameter.
				 */
				void function16(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15);

				/**
				 * Member function with 17 parameter.
				 */
				void function17(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16);

				/**
				 * Member function with 18 parameter.
				 */
				void function18(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16, int p17);

				/**
				 * Member function with 19 parameter.
				 */
				void function19(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16, int p17, int p18);

				/**
				 * Member function with 20 parameter.
				 */
				void function20(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16, int p17, int p18, int p19);

				/**
				 * Member function with no parameter.
				 * @return True, if succeeded
				 */
				int functionInt0();

				/**
				 * Member function with one parameter.
				 * @param p0 First parameter
				 * @return True, if succeeded
				 */
				int functionInt1(int p0);

				/**
				 * Member function with two parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @return True, if succeeded
				 */
				int functionInt2(int p0, int p1);

				/**
				 * Member function with three parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @return True, if succeeded
				 */
				int functionInt3(int p0, int p1, int p2);

				/**
				 * Member function with four parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @return True, if succeeded
				 */
				int functionInt4(int p0, int p1, int p2, int p3);

				/**
				 * Member function with five parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @param p4 Fifth parameter
				 * @return True, if succeeded
				 */
				int functionInt5(int p0, int p1, int p2, int p3, int p4);

				/**
				 * Member function with six parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @param p4 Fifth parameter
				 * @param p5 Sixth parameter
				 * @return True, if succeeded
				 */
				int functionInt6(int p0, int p1, int p2, int p3, int p4, int p5);

				/**
				 * Member function with seven parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @param p4 Fifth parameter
				 * @param p5 Sixth parameter
				 * @param p6 Seventh parameter
				 * @return True, if succeeded
				 */
				int functionInt7(int p0, int p1, int p2, int p3, int p4, int p5, int p6);

				/**
				 * Member function with eight parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @param p4 Fifth parameter
				 * @param p5 Sixth parameter
				 * @param p6 Seventh parameter
				 * @param p7 Eighth parameter
				 * @return True, if succeeded
				 */
				int functionInt8(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7);

				/**
				 * Member function with nine parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @param p4 Fifth parameter
				 * @param p5 Sixth parameter
				 * @param p6 Seventh parameter
				 * @param p7 Eighth parameter
				 * @param p8 Ninth parameter
				 * @return True, if succeeded
				 */
				int functionInt9(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8);

				/**
				 * Member function with ten parameter.
				 * @param p0 First parameter
				 * @param p1 Second parameter
				 * @param p2 Third parameter
				 * @param p3 Fourth parameter
				 * @param p4 Fifth parameter
				 * @param p5 Sixth parameter
				 * @param p6 Seventh parameter
				 * @param p7 Eighth parameter
				 * @param p8 Ninth parameter
				 * @param p9 Tenth parameter
				 * @return True, if succeeded
				 */
				int functionInt10(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9);

				/**
				 * Member function with 11 parameter.
				 */
				int functionInt11(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10);

				/**
				 * Member function with 12 parameter.
				 */
				int functionInt12(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11);

				/**
				 * Member function with 13 parameter.
				 */
				int functionInt13(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12);

				/**
				 * Member function with 14 parameter.
				 */
				int functionInt14(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13);

				/**
				 * Member function with 15 parameter.
				 */
				int functionInt15(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14);

				/**
				 * Member function with 16 parameter.
				 */
				int functionInt16(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15);

				/**
				 * Member function with 17 parameter.
				 */
				int functionInt17(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16);

				/**
				 * Member function with 18 parameter.
				 */
				int functionInt18(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16, int p17);

				/**
				 * Member function with 19 parameter.
				 */
				int functionInt19(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16, int p17, int p18);

				/**
				 * Member function with 20 parameter.
				 */
				int functionInt20(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16, int p17, int p18, int p19);
		};

	public:

		/**
		 * Tests the entire callback functionality.
		 * @return True, if succeeded
		 */
		static bool test();

		/**
		 * Tests the callback object with member functions.
		 * @return True, if succeeded
		 */
		static bool testCallbackMembers();

		/**
		 * Tests the callback object with static functions.
		 * @return True, if succeeded
		 */
		static bool testCallbackStatics();
	
	private:

		/**
		 * Static function with no parameter.
		 */
		static void function0();

		/**
		 * Static function with one parameter.
		 * @param p0 First parameter
		 */
		static void function1(int p0);

		/**
		 * Static function with two parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 */
		static void function2(int p0, int p1);

		/**
		 * Static function with three parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 */
		static void function3(int p0, int p1, int p2);

		/**
		 * Static function with four parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 */
		static void function4(int p0, int p1, int p2, int p3);

		/**
		 * Static function with five parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @param p4 Fifth parameter
		 */
		static void function5(int p0, int p1, int p2, int p3, int p4);

		/**
		 * Static function with six parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @param p4 Fifth parameter
		 * @param p5 Sixth parameter
		 */
		static void function6(int p0, int p1, int p2, int p3, int p4, int p5);

		/**
		 * Static function with seven parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @param p4 Fifth parameter
		 * @param p5 Sixth parameter
		 * @param p6 Seventh parameter
		 */
		static void function7(int p0, int p1, int p2, int p3, int p4, int p5, int p6);

		/**
		 * Static function with eight parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @param p4 Fifth parameter
		 * @param p5 Sixth parameter
		 * @param p6 Seventh parameter
		 * @param p7 Eighth parameter
		 */
		static void function8(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7);

		/**
		 * Static function with nine parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @param p4 Fifth parameter
		 * @param p5 Sixth parameter
		 * @param p6 Seventh parameter
		 * @param p7 Eighth parameter
		 * @param p8 Ninth parameter
		 */
		static void function9(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8);

		/**
		 * Static function with ten parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @param p4 Fifth parameter
		 * @param p5 Sixth parameter
		 * @param p6 Seventh parameter
		 * @param p7 Eighth parameter
		 * @param p8 Ninth parameter
		 * @param p9 Tenth parameter
		 */
		static void function10(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9);

		/**
		 * Static function with 11 parameter.
		 */
		static void function11(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10);

		/**
		 * Static function with 12 parameter.
		 */
		static void function12(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11);

		/**
		 * Static function with 13 parameter.
		 */
		static void function13(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12);

		/**
		 * Static function with 14 parameter.
		 */
		static void function14(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13);

		/**
		 * Static function with 15 parameter.
		 */
		static void function15(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14);

		/**
		 * Static function with 16 parameter.
		 */
		static void function16(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15);

		/**
		 * Static function with 17 parameter.
		 */
		static void function17(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16);

		/**
		 * Static function with 18 parameter.
		 */
		static void function18(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16, int p17);

		/**
		 * Static function with 19 parameter.
		 */
		static void function19(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16, int p17, int p18);

		/**
		 * Static function with 20 parameter.
		 */
		static void function20(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16, int p17, int p18, int p19);

		/**
		 * Static function with no parameter.
		 * @return True, if succeeded
		 */
		static int functionInt0();

		/**
		 * Static function with one parameter.
		 * @param p0 First parameter
		 * @return True, if succeeded
		 */
		static int functionInt1(int p0);

		/**
		 * Static function with two parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @return True, if succeeded
		 */
		static int functionInt2(int p0, int p1);

		/**
		 * Static function with three parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @return True, if succeeded
		 */
		static int functionInt3(int p0, int p1, int p2);

		/**
		 * Static function with four parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @return True, if succeeded
		 */
		static int functionInt4(int p0, int p1, int p2, int p3);

		/**
		 * Static function with five parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @param p4 Fifth parameter
		 * @return True, if succeeded
		 */
		static int functionInt5(int p0, int p1, int p2, int p3, int p4);

		/**
		 * Static function with six parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @param p4 Fifth parameter
		 * @param p5 Sixth parameter
		 * @return True, if succeeded
		 */
		static int functionInt6(int p0, int p1, int p2, int p3, int p4, int p5);

		/**
		 * Static function with seven parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @param p4 Fifth parameter
		 * @param p5 Sixth parameter
		 * @param p6 Seventh parameter
		 * @return True, if succeeded
		 */
		static int functionInt7(int p0, int p1, int p2, int p3, int p4, int p5, int p6);

		/**
		 * Static function with eight parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @param p4 Fifth parameter
		 * @param p5 Sixth parameter
		 * @param p6 Seventh parameter
		 * @param p7 Eighth parameter
		 * @return True, if succeeded
		 */
		static int functionInt8(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7);

		/**
		 * Static function with nine parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @param p4 Fifth parameter
		 * @param p5 Sixth parameter
		 * @param p6 Seventh parameter
		 * @param p7 Eighth parameter
		 * @param p8 Ninth parameter
		 * @return True, if succeeded
		 */
		static int functionInt9(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8);

		/**
		 * Static function with ten parameter.
		 * @param p0 First parameter
		 * @param p1 Second parameter
		 * @param p2 Third parameter
		 * @param p3 Fourth parameter
		 * @param p4 Fifth parameter
		 * @param p5 Sixth parameter
		 * @param p6 Seventh parameter
		 * @param p7 Eighth parameter
		 * @param p8 Ninth parameter
		 * @param p9 Tenth parameter
		 * @return True, if succeeded
		 */
		static int functionInt10(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9);

		/**
		 * Static function with 11 parameter.
		 */
		static int functionInt11(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10);

		/**
		 * Static function with 12 parameter.
		 */
		static int functionInt12(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11);

		/**
		 * Static function with 13 parameter.
		 */
		static int functionInt13(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12);

		/**
		 * Static function with 14 parameter.
		 */
		static int functionInt14(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13);

		/**
		 * Static function with 15 parameter.
		 */
		static int functionInt15(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14);

		/**
		 * Static function with 16 parameter.
		 */
		static int functionInt16(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15);

		/**
		 * Static function with 17 parameter.
		 */
		static int functionInt17(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16);

		/**
		 * Static function with 18 parameter.
		 */
		static int functionInt18(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16, int p17);

		/**
		 * Static function with 19 parameter.
		 */
		static int functionInt19(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16, int p17, int p18);

		/**
		 * Static function with 20 parameter.
		 */
		static int functionInt20(int p0, int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8, int p9, int p10, int p11, int p12, int p13, int p14, int p15, int p16, int p17, int p18, int p19);
};

}

}

}

#endif // META_OCEAN_TEST_TESTBASE_TEST_CALLBACK_H
