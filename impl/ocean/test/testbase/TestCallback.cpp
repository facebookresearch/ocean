/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestCallback.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

void TestCallback::Object::function0()
{
	Log::info() << "void function(), 0 parameter(s)";
}

void TestCallback::Object::function1(int /*p0*/)
{
	Log::info() << "void function(), 1 parameter(s)";
}

void TestCallback::Object::function2(int /*p0*/, int /*p1*/)
{
	Log::info() << "void function(), 2 parameter(s)";
}

void TestCallback::Object::function3(int /*p0*/, int /*p1*/, int /*p2*/)
{
	Log::info() << "void function(), 3 parameter(s)";
}

void TestCallback::Object::function4(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/)
{
	Log::info() << "void function(), 4 parameter(s)";
}

void TestCallback::Object::function5(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/)
{
	Log::info() << "void function(), 5 parameter(s)";
}

void TestCallback::Object::function6(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/)
{
	Log::info() << "void function(), 6 parameter(s)";
}

void TestCallback::Object::function7(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/)
{
	Log::info() << "void function(), 7 parameter(s)";
}

void TestCallback::Object::function8(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/)
{
	Log::info() << "void function(), 8 parameter(s)";
}

void TestCallback::Object::function9(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/)
{
	Log::info() << "void function(), 9 parameter(s)";
}

void TestCallback::Object::function10(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/)
{
	Log::info() << "void function(), 10 parameter(s)";
}

void TestCallback::Object::function11(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/)
{
	Log::info() << "void function(), 11 parameter(s)";
}

void TestCallback::Object::function12(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/)
{
	Log::info() << "void function(), 12 parameter(s)";
}

void TestCallback::Object::function13(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/)
{
	Log::info() << "void function(), 13 parameter(s)";
}

void TestCallback::Object::function14(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/)
{
	Log::info() << "void function(), 14 parameter(s)";
}

void TestCallback::Object::function15(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/)
{
	Log::info() << "void function(), 15 parameter(s)";
}

void TestCallback::Object::function16(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/)
{
	Log::info() << "void function(), 16 parameter(s)";
}

void TestCallback::Object::function17(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/)
{
	Log::info() << "void function(), 17 parameter(s)";
}

void TestCallback::Object::function18(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/, int /*p17*/)
{
	Log::info() << "void function(), 18 parameter(s)";
}

void TestCallback::Object::function19(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/, int /*p17*/, int /*p18*/)
{
	Log::info() << "void function(), 19 parameter(s)";
}

void TestCallback::Object::function20(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/, int /*p17*/, int /*p18*/, int /*p19*/)
{
	Log::info() << "void function(), 20 parameter(s)";
}

int TestCallback::Object::functionInt0()
{
	Log::info() << "int function(), 0 parameter(s)";
	return 0;
}

int TestCallback::Object::functionInt1(int /*p0*/)
{
	Log::info() << "int function(), 1 parameter(s)";
	return 1;
}

int TestCallback::Object::functionInt2(int /*p0*/, int /*p1*/)
{
	Log::info() << "int function(), 2 parameter(s)";
	return 2;
}

int TestCallback::Object::functionInt3(int /*p0*/, int /*p1*/, int /*p2*/)
{
	Log::info() << "int function(), 3 parameter(s)";
	return 3;
}

int TestCallback::Object::functionInt4(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/)
{
	Log::info() << "int function(), 4 parameter(s)";
	return 4;
}

int TestCallback::Object::functionInt5(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/)
{
	Log::info() << "int function(), 5 parameter(s)";
	return 5;
}

int TestCallback::Object::functionInt6(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/)
{
	Log::info() << "int function(), 6 parameter(s)";
	return 6;
}

int TestCallback::Object::functionInt7(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/)
{
	Log::info() << "int function(), 7 parameter(s)";
	return 7;
}

int TestCallback::Object::functionInt8(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/)
{
	Log::info() << "int function(), 8 parameter(s)";
	return 8;
}

int TestCallback::Object::functionInt9(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/)
{
	Log::info() << "int function(), 9 parameter(s)";
	return 9;
}

int TestCallback::Object::functionInt10(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/)
{
	Log::info() << "int function(), 10 parameter(s)";
	return 10;
}

int TestCallback::Object::functionInt11(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/)
{
	Log::info() << "int function(), 11 parameter(s)";
	return 11;
}

int TestCallback::Object::functionInt12(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/)
{
	Log::info() << "int function(), 12 parameter(s)";
	return 12;
}

int TestCallback::Object::functionInt13(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/)
{
	Log::info() << "int function(), 13 parameter(s)";
	return 13;
}

int TestCallback::Object::functionInt14(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/)
{
	Log::info() << "int function(), 14 parameter(s)";
	return 14;
}

int TestCallback::Object::functionInt15(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/)
{
	Log::info() << "int function(), 15 parameter(s)";
	return 15;
}

int TestCallback::Object::functionInt16(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/)
{
	Log::info() << "int function(), 16 parameter(s)";
	return 16;
}

int TestCallback::Object::functionInt17(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/)
{
	Log::info() << "int function(), 17 parameter(s)";
	return 17;
}

int TestCallback::Object::functionInt18(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/, int /*p17*/)
{
	Log::info() << "int function(), 18 parameter(s)";
	return 18;
}

int TestCallback::Object::functionInt19(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/, int /*p17*/, int /*p18*/)
{
	Log::info() << "int function(), 19 parameter(s)";
	return 19;
}

int TestCallback::Object::functionInt20(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/, int /*p17*/, int /*p18*/, int /*p19*/)
{
	Log::info() << "int function(), 20 parameter(s)";
	return 20;
}

bool TestCallback::test()
{
	Log::info() << "---   Callback tests:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testCallbackMembers() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCallbackStatics() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Callback test succeeded.";
	else
		Log::info() << "Callback test FAILED!";

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestCallback, CallbackMembers) {
	EXPECT_TRUE(TestCallback::testCallbackMembers());
}

TEST(TestCallback, CallbackStatics) {
	EXPECT_TRUE(TestCallback::testCallbackStatics());
}

#endif // OCEAN_USE_GTEST

bool TestCallback::testCallbackMembers()
{
	typedef Ocean::Callback<void> Callback0;
	typedef Ocean::Callback<void, int> Callback1;
	typedef Ocean::Callback<void, int, int> Callback2;
	typedef Ocean::Callback<void, int, int, int> Callback3;
	typedef Ocean::Callback<void, int, int, int, int> Callback4;
	typedef Ocean::Callback<void, int, int, int, int, int> Callback5;
	typedef Ocean::Callback<void, int, int, int, int, int, int> Callback6;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int> Callback7;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int> Callback8;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int> Callback9;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int> Callback10;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int> Callback11;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int> Callback12;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback13;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback14;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback15;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback16;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback17;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback18;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback19;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback20;

	typedef Ocean::Callback<int> CallbackInt0;
	typedef Ocean::Callback<int, int> CallbackInt1;
	typedef Ocean::Callback<int, int, int> CallbackInt2;
	typedef Ocean::Callback<int, int, int, int> CallbackInt3;
	typedef Ocean::Callback<int, int, int, int, int> CallbackInt4;
	typedef Ocean::Callback<int, int, int, int, int, int> CallbackInt5;
	typedef Ocean::Callback<int, int, int, int, int, int, int> CallbackInt6;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int> CallbackInt7;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int> CallbackInt8;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int> CallbackInt9;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int> CallbackInt10;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt11;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt12;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt13;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt14;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt15;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt16;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt17;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt18;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt19;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt20;

	Object object;

	Callback0 callback0(object, &Object::function0);
	Callback1 callback1(object, &Object::function1);
	Callback2 callback2(object, &Object::function2);
	Callback3 callback3(object, &Object::function3);
	Callback4 callback4(object, &Object::function4);
	Callback5 callback5(object, &Object::function5);
	Callback6 callback6(object, &Object::function6);
	Callback7 callback7(object, &Object::function7);
	Callback8 callback8(object, &Object::function8);
	Callback9 callback9(object, &Object::function9);
	Callback10 callback10(object, &Object::function10);
	Callback11 callback11(object, &Object::function11);
	Callback12 callback12(object, &Object::function12);
	Callback13 callback13(object, &Object::function13);
	Callback14 callback14(object, &Object::function14);
	Callback15 callback15(object, &Object::function15);
	Callback16 callback16(object, &Object::function16);
	Callback17 callback17(object, &Object::function17);
	Callback18 callback18(object, &Object::function18);
	Callback19 callback19(object, &Object::function19);
	Callback20 callback20(object, &Object::function20);

	CallbackInt0 callbackInt0(object, &Object::functionInt0);
	CallbackInt1 callbackInt1(object, &Object::functionInt1);
	CallbackInt2 callbackInt2(object, &Object::functionInt2);
	CallbackInt3 callbackInt3(object, &Object::functionInt3);
	CallbackInt4 callbackInt4(object, &Object::functionInt4);
	CallbackInt5 callbackInt5(object, &Object::functionInt5);
	CallbackInt6 callbackInt6(object, &Object::functionInt6);
	CallbackInt7 callbackInt7(object, &Object::functionInt7);
	CallbackInt8 callbackInt8(object, &Object::functionInt8);
	CallbackInt9 callbackInt9(object, &Object::functionInt9);
	CallbackInt10 callbackInt10(object, &Object::functionInt10);
	CallbackInt11 callbackInt11(object, &Object::functionInt11);
	CallbackInt12 callbackInt12(object, &Object::functionInt12);
	CallbackInt13 callbackInt13(object, &Object::functionInt13);
	CallbackInt14 callbackInt14(object, &Object::functionInt14);
	CallbackInt15 callbackInt15(object, &Object::functionInt15);
	CallbackInt16 callbackInt16(object, &Object::functionInt16);
	CallbackInt17 callbackInt17(object, &Object::functionInt17);
	CallbackInt18 callbackInt18(object, &Object::functionInt18);
	CallbackInt19 callbackInt19(object, &Object::functionInt19);
	CallbackInt20 callbackInt20(object, &Object::functionInt20);

	Log::info() << "Testing static functions without return value:";

	callback0();
	callback1(1);
	callback2(1, 2);
	callback3(1, 2, 3);
	callback4(1, 2, 3, 4);
	callback5(1, 2, 3, 4, 5);
	callback6(1, 2, 3, 4, 5, 6);
	callback7(1, 2, 3, 4, 5, 6, 7);
	callback8(1, 2, 3, 4, 5, 6, 7, 8);
	callback9(1, 2, 3, 4, 5, 6, 7, 8, 9);
	callback10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	callback11(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
	callback12(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
	callback13(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
	callback14(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
	callback15(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	callback16(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
	callback17(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17);
	callback18(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18);
	callback19(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19);
	callback20(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);

	Log::info() << " ";
	Log::info() << "Testing static functions with return value:";

	const int result0 = callbackInt0();
	const int result1 = callbackInt1(1);
	const int result2 = callbackInt2(1, 2);
	const int result3 = callbackInt3(1, 2, 3);
	const int result4 = callbackInt4(1, 2, 3, 4);
	const int result5 = callbackInt5(1, 2, 3, 4, 5);
	const int result6 = callbackInt6(1, 2, 3, 4, 5, 6);
	const int result7 = callbackInt7(1, 2, 3, 4, 5, 6, 7);
	const int result8 = callbackInt8(1, 2, 3, 4, 5, 6, 7, 8);
	const int result9 = callbackInt9(1, 2, 3, 4, 5, 6, 7, 8, 9);
	const int result10 = callbackInt10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	const int result11 = callbackInt11(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
	const int result12 = callbackInt12(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
	const int result13 = callbackInt13(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
	const int result14 = callbackInt14(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
	const int result15 = callbackInt15(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	const int result16 = callbackInt16(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
	const int result17 = callbackInt17(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17);
	const int result18 = callbackInt18(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18);
	const int result19 = callbackInt19(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19);
	const int result20 = callbackInt20(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);

	const bool allSucceeded = result0 == 0 && result1 == 1 && result2 == 2 && result3 == 3 && result4 == 4 && result5 == 5 && result6 == 6 && result7 == 7 && result8 == 8 && result9 == 9
								&& result10 == 10 && result11 == 11 && result12 == 12 && result13 == 13 && result14 == 14 && result15 == 15 && result16 == 16 && result17 == 17 && result18 == 18 && result19 == 19 && result20 == 20;

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED.";

	return allSucceeded;
}

bool TestCallback::testCallbackStatics()
{
	typedef Ocean::Callback<void> Callback0;
	typedef Ocean::Callback<void, int> Callback1;
	typedef Ocean::Callback<void, int, int> Callback2;
	typedef Ocean::Callback<void, int, int, int> Callback3;
	typedef Ocean::Callback<void, int, int, int, int> Callback4;
	typedef Ocean::Callback<void, int, int, int, int, int> Callback5;
	typedef Ocean::Callback<void, int, int, int, int, int, int> Callback6;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int> Callback7;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int> Callback8;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int> Callback9;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int> Callback10;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int> Callback11;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int> Callback12;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback13;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback14;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback15;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback16;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback17;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback18;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback19;
	typedef Ocean::Callback<void, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> Callback20;

	typedef Ocean::Callback<int> CallbackInt0;
	typedef Ocean::Callback<int, int> CallbackInt1;
	typedef Ocean::Callback<int, int, int> CallbackInt2;
	typedef Ocean::Callback<int, int, int, int> CallbackInt3;
	typedef Ocean::Callback<int, int, int, int, int> CallbackInt4;
	typedef Ocean::Callback<int, int, int, int, int, int> CallbackInt5;
	typedef Ocean::Callback<int, int, int, int, int, int, int> CallbackInt6;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int> CallbackInt7;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int> CallbackInt8;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int> CallbackInt9;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int> CallbackInt10;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt11;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt12;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt13;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt14;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt15;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt16;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt17;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt18;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt19;
	typedef Ocean::Callback<int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int, int> CallbackInt20;

	Callback0 callback0(&TestCallback::function0);
	Callback1 callback1(&TestCallback::function1);
	Callback2 callback2(&TestCallback::function2);
	Callback3 callback3(&TestCallback::function3);
	Callback4 callback4(&TestCallback::function4);
	Callback5 callback5(&TestCallback::function5);
	Callback6 callback6(&TestCallback::function6);
	Callback7 callback7(&TestCallback::function7);
	Callback8 callback8(&TestCallback::function8);
	Callback9 callback9(&TestCallback::function9);
	Callback10 callback10(&TestCallback::function10);
	Callback11 callback11(&TestCallback::function11);
	Callback12 callback12(&TestCallback::function12);
	Callback13 callback13(&TestCallback::function13);
	Callback14 callback14(&TestCallback::function14);
	Callback15 callback15(&TestCallback::function15);
	Callback16 callback16(&TestCallback::function16);
	Callback17 callback17(&TestCallback::function17);
	Callback18 callback18(&TestCallback::function18);
	Callback19 callback19(&TestCallback::function19);
	Callback20 callback20(&TestCallback::function20);

	CallbackInt0 callbackInt0(&TestCallback::functionInt0);
	CallbackInt1 callbackInt1(&TestCallback::functionInt1);
	CallbackInt2 callbackInt2(&TestCallback::functionInt2);
	CallbackInt3 callbackInt3(&TestCallback::functionInt3);
	CallbackInt4 callbackInt4(&TestCallback::functionInt4);
	CallbackInt5 callbackInt5(&TestCallback::functionInt5);
	CallbackInt6 callbackInt6(&TestCallback::functionInt6);
	CallbackInt7 callbackInt7(&TestCallback::functionInt7);
	CallbackInt8 callbackInt8(&TestCallback::functionInt8);
	CallbackInt9 callbackInt9(&TestCallback::functionInt9);
	CallbackInt10 callbackInt10(&TestCallback::functionInt10);
	CallbackInt11 callbackInt11(&TestCallback::functionInt11);
	CallbackInt12 callbackInt12(&TestCallback::functionInt12);
	CallbackInt13 callbackInt13(&TestCallback::functionInt13);
	CallbackInt14 callbackInt14(&TestCallback::functionInt14);
	CallbackInt15 callbackInt15(&TestCallback::functionInt15);
	CallbackInt16 callbackInt16(&TestCallback::functionInt16);
	CallbackInt17 callbackInt17(&TestCallback::functionInt17);
	CallbackInt18 callbackInt18(&TestCallback::functionInt18);
	CallbackInt19 callbackInt19(&TestCallback::functionInt19);
	CallbackInt20 callbackInt20(&TestCallback::functionInt20);

	Log::info() << "Testing static functions without return value:";

	callback0();
	callback1(1);
	callback2(1, 2);
	callback3(1, 2, 3);
	callback4(1, 2, 3, 4);
	callback5(1, 2, 3, 4, 5);
	callback6(1, 2, 3, 4, 5, 6);
	callback7(1, 2, 3, 4, 5, 6, 7);
	callback8(1, 2, 3, 4, 5, 6, 7, 8);
	callback9(1, 2, 3, 4, 5, 6, 7, 8, 9);
	callback10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	callback11(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
	callback12(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
	callback13(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
	callback14(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
	callback15(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	callback16(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
	callback17(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17);
	callback18(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18);
	callback19(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19);
	callback20(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);

	Log::info() << " ";
	Log::info() << "Testing static functions with return value:";

	const int result0 = callbackInt0();
	const int result1 = callbackInt1(1);
	const int result2 = callbackInt2(1, 2);
	const int result3 = callbackInt3(1, 2, 3);
	const int result4 = callbackInt4(1, 2, 3, 4);
	const int result5 = callbackInt5(1, 2, 3, 4, 5);
	const int result6 = callbackInt6(1, 2, 3, 4, 5, 6);
	const int result7 = callbackInt7(1, 2, 3, 4, 5, 6, 7);
	const int result8 = callbackInt8(1, 2, 3, 4, 5, 6, 7, 8);
	const int result9 = callbackInt9(1, 2, 3, 4, 5, 6, 7, 8, 9);
	const int result10 = callbackInt10(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	const int result11 = callbackInt11(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
	const int result12 = callbackInt12(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
	const int result13 = callbackInt13(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
	const int result14 = callbackInt14(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
	const int result15 = callbackInt15(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
	const int result16 = callbackInt16(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
	const int result17 = callbackInt17(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17);
	const int result18 = callbackInt18(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18);
	const int result19 = callbackInt19(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19);
	const int result20 = callbackInt20(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);

	const bool allSucceeded = result0 == 0 && result1 == 1 && result2 == 2 && result3 == 3 && result4 == 4 && result5 == 5 && result6 == 6 && result7 == 7 && result8 == 8 && result9 == 9
									&& result10 == 10	&& result11 == 11 && result12 == 12 && result13 == 13 && result14 == 14 && result15 == 15 && result16 == 16 && result17 == 17 && result18 == 18 && result19 == 19 && result20 == 20;

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED.";

	return allSucceeded;
}

void TestCallback::function0()
{
	Log::info() << "void function(), 0 parameter(s)";
}

void TestCallback::function1(int /*p0*/)
{
	Log::info() << "void function(), 1 parameter(s)";
}

void TestCallback::function2(int /*p0*/, int /*p1*/)
{
	Log::info() << "void function(), 2 parameter(s)";
}

void TestCallback::function3(int /*p0*/, int /*p1*/, int /*p2*/)
{
	Log::info() << "void function(), 3 parameter(s)";
}

void TestCallback::function4(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/)
{
	Log::info() << "void function(), 4 parameter(s)";
}

void TestCallback::function5(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/)
{
	Log::info() << "void function(), 5 parameter(s)";
}

void TestCallback::function6(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/)
{
	Log::info() << "void function(), 6 parameter(s)";
}

void TestCallback::function7(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/)
{
	Log::info() << "void function(), 7 parameter(s)";
}

void TestCallback::function8(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/)
{
	Log::info() << "void function(), 8 parameter(s)";
}

void TestCallback::function9(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/)
{
	Log::info() << "void function(), 9 parameter(s)";
}

void TestCallback::function10(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/)
{
	Log::info() << "void function(), 10 parameter(s)";
}

void TestCallback::function11(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/)
{
	Log::info() << "void function(), 11 parameter(s)";
}

void TestCallback::function12(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/)
{
	Log::info() << "void function(), 12 parameter(s)";
}

void TestCallback::function13(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/)
{
	Log::info() << "void function(), 13 parameter(s)";
}

void TestCallback::function14(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/)
{
	Log::info() << "void function(), 14 parameter(s)";
}

void TestCallback::function15(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/)
{
	Log::info() << "void function(), 15 parameter(s)";
}

void TestCallback::function16(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/)
{
	Log::info() << "void function(), 16 parameter(s)";
}

void TestCallback::function17(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/)
{
	Log::info() << "void function(), 17 parameter(s)";
}

void TestCallback::function18(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/, int /*p17*/)
{
	Log::info() << "void function(), 18 parameter(s)";
}

void TestCallback::function19(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/, int /*p17*/, int /*p18*/)
{
	Log::info() << "void function(), 19 parameter(s)";
}

void TestCallback::function20(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/, int /*p17*/, int /*p18*/, int /*p19*/)
{
	Log::info() << "void function(), 20 parameter(s)";
}

int TestCallback::functionInt0()
{
	Log::info() << "int function(), 0 parameter(s)";
	return 0;
}

int TestCallback::functionInt1(int /*p0*/)
{
	Log::info() << "int function(), 1 parameter(s)";
	return 1;
}

int TestCallback::functionInt2(int /*p0*/, int /*p1*/)
{
	Log::info() << "int function(), 2 parameter(s)";
	return 2;
}

int TestCallback::functionInt3(int /*p0*/, int /*p1*/, int /*p2*/)
{
	Log::info() << "int function(), 3 parameter(s)";
	return 3;
}

int TestCallback::functionInt4(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/)
{
	Log::info() << "int function(), 4 parameter(s)";
	return 4;
}

int TestCallback::functionInt5(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/)
{
	Log::info() << "int function(), 5 parameter(s)";
	return 5;
}

int TestCallback::functionInt6(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/)
{
	Log::info() << "int function(), 6 parameter(s)";
	return 6;
}

int TestCallback::functionInt7(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/)
{
	Log::info() << "int function(), 7 parameter(s)";
	return 7;
}

int TestCallback::functionInt8(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/)
{
	Log::info() << "int function(), 8 parameter(s)";
	return 8;
}

int TestCallback::functionInt9(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/)
{
	Log::info() << "int function(), 9 parameter(s)";
	return 9;
}

int TestCallback::functionInt10(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/)
{
	Log::info() << "int function(), 10 parameter(s)";
	return 10;
}

int TestCallback::functionInt11(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/)
{
	Log::info() << "int function(), 11 parameter(s)";
	return 11;
}

int TestCallback::functionInt12(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/)
{
	Log::info() << "int function(), 12 parameter(s)";
	return 12;
}

int TestCallback::functionInt13(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/)
{
	Log::info() << "int function(), 13 parameter(s)";
	return 13;
}

int TestCallback::functionInt14(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/)
{
	Log::info() << "int function(), 14 parameter(s)";
	return 14;
}

int TestCallback::functionInt15(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/)
{
	Log::info() << "int function(), 15 parameter(s)";
	return 15;
}

int TestCallback::functionInt16(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/)
{
	Log::info() << "int function(), 16 parameter(s)";
	return 16;
}

int TestCallback::functionInt17(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/)
{
	Log::info() << "int function(), 17 parameter(s)";
	return 17;
}

int TestCallback::functionInt18(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/, int /*p17*/)
{
	Log::info() << "int function(), 18 parameter(s)";
	return 18;
}

int TestCallback::functionInt19(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/, int /*p17*/, int /*p18*/)
{
	Log::info() << "int function(), 19 parameter(s)";
	return 19;
}

int TestCallback::functionInt20(int /*p0*/, int /*p1*/, int /*p2*/, int /*p3*/, int /*p4*/, int /*p5*/, int /*p6*/, int /*p7*/, int /*p8*/, int /*p9*/, int /*p10*/, int /*p11*/, int /*p12*/, int /*p13*/, int /*p14*/, int /*p15*/, int /*p16*/, int /*p17*/, int /*p18*/, int /*p19*/)
{
	Log::info() << "int function(), 20 parameter(s)";
	return 20;
}

}

}

}
