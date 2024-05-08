/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestCaller.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

TestCaller::Object::Object()
{
	// nothing to do here
}

void TestCaller::Object::function()
{
	Log::info() << "void function(), 0 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/)
{
	Log::info() << "void function(), 1 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/)
{
	Log::info() << "void function(), 2 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/)
{
	Log::info() << "void function(), 3 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/)
{
	Log::info() << "void function(), 4 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/)
{
	Log::info() << "void function(), 5 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/)
{
	Log::info() << "void function(), 6 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/)
{
	Log::info() << "void function(), 7 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/)
{
	Log::info() << "void function(), 8 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/)
{
	Log::info() << "void function(), 9 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/)
{
	Log::info() << "void function(), 10 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/)
{
	Log::info() << "void function(), 11 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/)
{
	Log::info() << "void function(), 12 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/)
{
	Log::info() << "void function(), 13 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/)
{
	Log::info() << "void function(), 14 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/)
{
	Log::info() << "void function(), 15 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/)
{
	Log::info() << "void function(), 16 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/)
{
	Log::info() << "void function(), 17 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/, Param17* /*p17*/)
{
	Log::info() << "void function(), 18 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/, Param17* /*p17*/, Param18* /*p18*/)
{
	Log::info() << "void function(), 19 parameter(s)";
}

void TestCaller::Object::function(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/, Param17* /*p17*/, Param18* /*p18*/, Param19* /*p19*/)
{
	Log::info() << "void function(), 20 parameter(s)";
}

int TestCaller::Object::functionInt() const
{
	Log::info() << "int function(), 0 parameter(s)";
	return 0;
}

int TestCaller::Object::functionInt(Param0* /*p0*/) const
{
	Log::info() << "int function(), 1 parameter(s)";
	return 1;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/) const
{
	Log::info() << "int function(), 2 parameter(s)";
	return 2;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/) const
{
	Log::info() << "int function(), 3 parameter(s)";
	return 3;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/) const
{
	Log::info() << "int function(), 4 parameter(s)";
	return 4;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/) const
{
	Log::info() << "int function(), 5 parameter(s)";
	return 5;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/) const
{
	Log::info() << "int function(), 6 parameter(s)";
	return 6;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/) const
{
	Log::info() << "int function(), 7 parameter(s)";
	return 7;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/) const
{
	Log::info() << "int function(), 8 parameter(s)";
	return 8;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/) const
{
	Log::info() << "int function(), 9 parameter(s)";
	return 9;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/) const
{
	Log::info() << "int function(), 10 parameter(s)";
	return 10;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/) const
{
	Log::info() << "int function(), 11 parameter(s)";
	return 11;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/) const
{
	Log::info() << "int function(), 12 parameter(s)";
	return 12;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/) const
{
	Log::info() << "int function(), 13 parameter(s)";
	return 13;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/) const
{
	Log::info() << "int function(), 14 parameter(s)";
	return 14;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/) const
{
	Log::info() << "int function(), 15 parameter(s)";
	return 15;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/) const
{
	Log::info() << "int function(), 16 parameter(s)";
	return 16;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/) const
{
	Log::info() << "int function(), 17 parameter(s)";
	return 17;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/, Param17* /*p17*/) const
{
	Log::info() << "int function(), 18 parameter(s)";
	return 18;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/, Param17* /*p17*/, Param18* /*p18*/) const
{
	Log::info() << "int function(), 19 parameter(s)";
	return 19;
}

int TestCaller::Object::functionInt(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/, Param17* /*p17*/, Param18* /*p18*/, Param19* /*p19*/) const
{
	Log::info() << "int function(), 20 parameter(s)";
	return 20;
}

int TestCaller::Object::constantFunctionIndividualParameters0(int parameter0, bool* parameter1, float* parameter2, std::string parameter3) const
{
	ocean_assert(parameter1 != nullptr && parameter2 != nullptr);

	parameter0++;
	OCEAN_SUPPRESS_UNUSED_WARNING(parameter0);

	*parameter1 = !*parameter1;
	*parameter2 += 10.0f;
	parameter3.clear();

	return -1;
}

int TestCaller::Object::constantFunctionIndividualParameters1(int parameter0, bool* parameter1, float* parameter2, double& parameter3, std::string parameter4, std::string& parameter5, const Object& /*parameter6*/) const
{
	ocean_assert(parameter1 && parameter2);

	parameter0++;
	OCEAN_SUPPRESS_UNUSED_WARNING(parameter0);

	*parameter1 = !*parameter1;
	*parameter2 += 10.0f;
	parameter3 += 20.0;
	parameter4.clear();

	for (size_t n = 0; n < parameter5.size() / 2; ++n)
	{
		std::swap(parameter5[n], parameter5[parameter5.size() - n - 1]);
	}

	return -1;
}

int TestCaller::Object::nonConstantFunctionIndividualParameters0(int parameter0, bool* parameter1, float* parameter2, std::string parameter3)
{
	ocean_assert(parameter1 != nullptr && parameter2 != nullptr);

	parameter0++;
	OCEAN_SUPPRESS_UNUSED_WARNING(parameter0);

	*parameter1 = !*parameter1;
	*parameter2 += 10.0f;
	parameter3.clear();

	return -1;
}

int TestCaller::Object::nonConstantFunctionIndividualParameters1(int parameter0, bool* parameter1, float* parameter2, double& parameter3, std::string parameter4, std::string& parameter5, const Object& /*parameter6*/)
{
	ocean_assert(parameter1 != nullptr && parameter2 != nullptr);

	parameter0++;
	OCEAN_SUPPRESS_UNUSED_WARNING(parameter0);

	*parameter1 = !*parameter1;
	*parameter2 += 10.0f;
	parameter3 += 20.0;
	parameter4.clear();

	for (size_t n = 0; n < parameter5.size() / 2; ++n)
	{
		std::swap(parameter5[n], parameter5[parameter5.size() - n - 1]);
	}

	return -1;
}

bool TestCaller::test()
{
	Log::info() << "---   Caller tests:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testCallerMembers() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCallerStatics() && allSucceeded;

	Log::info() << " ";
	Log::info() << "-";
	Log::info() << " ";

	allSucceeded = testCallerParameters() && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
		Log::info() << "Caller test succeeded.";
	else
		Log::info() << "Caller test FAILED!";

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestCaller, CallerMembers) {
	EXPECT_TRUE(TestCaller::testCallerMembers());
}

TEST(TestCaller, CallerStatics) {
	EXPECT_TRUE(TestCaller::testCallerStatics());
}

TEST(TestCaller, CallerParameters) {
	EXPECT_TRUE(TestCaller::testCallerParameters());
}

#endif // OCEAN_USE_GTEST

bool TestCaller::testCallerMembers()
{
	typedef Ocean::Caller<void> CallerFunction;
	typedef Ocean::Caller<int> CallerFunctionInt;

	Object object;

	Param0 param0;
	Param1 param1;
	Param2 param2;
	Param3 param3;
	Param4 param4;
	Param5 param5;
	Param6 param6;
	Param7 param7;
	Param8 param8;
	Param9 param9;
	Param10 param10;
	Param11 param11;
	Param12 param12;
	Param13 param13;
	Param14 param14;
	Param15 param15;
	Param16 param16;
	Param17 param17;
	Param18 param18;
	Param19 param19;


	CallerFunction caller0 = CallerFunction::create(object, &Object::function);
	CallerFunction caller1 = CallerFunction::create(object, &Object::function, &param0);
	CallerFunction caller2 = CallerFunction::create(object, &Object::function, &param0, &param1);
	CallerFunction caller3 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2);
	CallerFunction caller4 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3);
	CallerFunction caller5 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4);
	CallerFunction caller6 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5);
	CallerFunction caller7 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6);
	CallerFunction caller8 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7);
	CallerFunction caller9 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8);
	CallerFunction caller10 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9);
	CallerFunction caller11 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10);
	CallerFunction caller12 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11);
	CallerFunction caller13 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12);
	CallerFunction caller14 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13);
	CallerFunction caller15 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14);
	CallerFunction caller16 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15);
	CallerFunction caller17 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16);
	CallerFunction caller18 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16, &param17);
	CallerFunction caller19 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16, &param17, &param18);
	CallerFunction caller20 = CallerFunction::create(object, &Object::function, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16, &param17, &param18, &param19);

	CallerFunctionInt callerInt0 = CallerFunctionInt::create(object, &Object::functionInt);
	CallerFunctionInt callerInt1 = CallerFunctionInt::create(object, &Object::functionInt, &param0);
	CallerFunctionInt callerInt2 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1);
	CallerFunctionInt callerInt3 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2);
	CallerFunctionInt callerInt4 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3);
	CallerFunctionInt callerInt5 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4);
	CallerFunctionInt callerInt6 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5);
	CallerFunctionInt callerInt7 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6);
	CallerFunctionInt callerInt8 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7);
	CallerFunctionInt callerInt9 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8);
	CallerFunctionInt callerInt10 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9);
	CallerFunctionInt callerInt11 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10);
	CallerFunctionInt callerInt12 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11);
	CallerFunctionInt callerInt13 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12);
	CallerFunctionInt callerInt14 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13);
	CallerFunctionInt callerInt15 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14);
	CallerFunctionInt callerInt16 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15);
	CallerFunctionInt callerInt17 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16);
	CallerFunctionInt callerInt18 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16, &param17);
	CallerFunctionInt callerInt19 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16, &param17, &param18);
	CallerFunctionInt callerInt20 = CallerFunctionInt::create(object, &Object::functionInt, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16, &param17, &param18, &param19);

	Log::info() << "Testing static functions without return value:";

	caller0();
	caller1();
	caller2();
	caller3();
	caller4();
	caller5();
	caller6();
	caller7();
	caller8();
	caller9();
	caller10();
	caller11();
	caller12();
	caller13();
	caller14();
	caller15();
	caller16();
	caller17();
	caller18();
	caller19();
	caller20();

	Log::info() << " ";
	Log::info() << "Testing static functions with return value:";

	const int result0 = callerInt0();
	const int result1 = callerInt1();
	const int result2 = callerInt2();
	const int result3 = callerInt3();
	const int result4 = callerInt4();
	const int result5 = callerInt5();
	const int result6 = callerInt6();
	const int result7 = callerInt7();
	const int result8 = callerInt8();
	const int result9 = callerInt9();
	const int result10 = callerInt10();
	const int result11 = callerInt11();
	const int result12 = callerInt12();
	const int result13 = callerInt13();
	const int result14 = callerInt14();
	const int result15 = callerInt15();
	const int result16 = callerInt16();
	const int result17 = callerInt17();
	const int result18 = callerInt18();
	const int result19 = callerInt19();
	const int result20 = callerInt20();

	const bool allSucceeded = result0 == 0 && result1 == 1 && result2 == 2 && result3 == 3 && result4 == 4 && result5 == 5 && result6 == 6 && result7 == 7 && result8 == 8 && result9 == 9
								&& result10 == 10 && result11 == 11 && result12 == 12 && result13 == 13 && result14 == 14 && result15 == 15 && result16 == 16 && result17 == 17 && result18 == 18 && result19 == 19 && result20 == 20;

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED.";

	return allSucceeded;
}

bool TestCaller::testCallerStatics()
{
	typedef Ocean::Caller<void> CallerFunction;
	typedef Ocean::Caller<int> CallerFunctionInt;

	Param0 param0;
	Param1 param1;
	Param2 param2;
	Param3 param3;
	Param4 param4;
	Param5 param5;
	Param6 param6;
	Param7 param7;
	Param8 param8;
	Param9 param9;
	Param10 param10;
	Param11 param11;
	Param12 param12;
	Param13 param13;
	Param14 param14;
	Param15 param15;
	Param16 param16;
	Param17 param17;
	Param18 param18;
	Param19 param19;

	CallerFunction caller0 = CallerFunction::createStatic(&TestCaller::function0);
	CallerFunction caller1 = CallerFunction::createStatic(&TestCaller::function1, &param0);
	CallerFunction caller2 = CallerFunction::createStatic(&TestCaller::function2, &param0, &param1);
	CallerFunction caller3 = CallerFunction::createStatic(&TestCaller::function3, &param0, &param1, &param2);
	CallerFunction caller4 = CallerFunction::createStatic(&TestCaller::function4, &param0, &param1, &param2, &param3);
	CallerFunction caller5 = CallerFunction::createStatic(&TestCaller::function5, &param0, &param1, &param2, &param3, &param4);
	CallerFunction caller6 = CallerFunction::createStatic(&TestCaller::function6, &param0, &param1, &param2, &param3, &param4, &param5);
	CallerFunction caller7 = CallerFunction::createStatic(&TestCaller::function7, &param0, &param1, &param2, &param3, &param4, &param5, &param6);
	CallerFunction caller8 = CallerFunction::createStatic(&TestCaller::function8, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7);
	CallerFunction caller9 = CallerFunction::createStatic(&TestCaller::function9, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8);
	CallerFunction caller10 = CallerFunction::createStatic(&TestCaller::function10, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9);
	CallerFunction caller11 = CallerFunction::createStatic(&TestCaller::function11, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10);
	CallerFunction caller12 = CallerFunction::createStatic(&TestCaller::function12, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11);
	CallerFunction caller13 = CallerFunction::createStatic(&TestCaller::function13, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12);
	CallerFunction caller14 = CallerFunction::createStatic(&TestCaller::function14, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13);
	CallerFunction caller15 = CallerFunction::createStatic(&TestCaller::function15, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14);
	CallerFunction caller16 = CallerFunction::createStatic(&TestCaller::function16, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15);
	CallerFunction caller17 = CallerFunction::createStatic(&TestCaller::function17, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16);
	CallerFunction caller18 = CallerFunction::createStatic(&TestCaller::function18, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16, &param17);
	CallerFunction caller19 = CallerFunction::createStatic(&TestCaller::function19, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16, &param17, &param18);
	CallerFunction caller20 = CallerFunction::createStatic(&TestCaller::function20, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16, &param17, &param18, &param19);

	CallerFunctionInt callerInt0 = CallerFunctionInt::createStatic(&TestCaller::functionInt0);
	CallerFunctionInt callerInt1 = CallerFunctionInt::createStatic(&TestCaller::functionInt1, &param0);
	CallerFunctionInt callerInt2 = CallerFunctionInt::createStatic(&TestCaller::functionInt2, &param0, &param1);
	CallerFunctionInt callerInt3 = CallerFunctionInt::createStatic(&TestCaller::functionInt3, &param0, &param1, &param2);
	CallerFunctionInt callerInt4 = CallerFunctionInt::createStatic(&TestCaller::functionInt4, &param0, &param1, &param2, &param3);
	CallerFunctionInt callerInt5 = CallerFunctionInt::createStatic(&TestCaller::functionInt5, &param0, &param1, &param2, &param3, &param4);
	CallerFunctionInt callerInt6 = CallerFunctionInt::createStatic(&TestCaller::functionInt6, &param0, &param1, &param2, &param3, &param4, &param5);
	CallerFunctionInt callerInt7 = CallerFunctionInt::createStatic(&TestCaller::functionInt7, &param0, &param1, &param2, &param3, &param4, &param5, &param6);
	CallerFunctionInt callerInt8 = CallerFunctionInt::createStatic(&TestCaller::functionInt8, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7);
	CallerFunctionInt callerInt9 = CallerFunctionInt::createStatic(&TestCaller::functionInt9, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8);
	CallerFunctionInt callerInt10 = CallerFunctionInt::createStatic(&TestCaller::functionInt10, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9);
	CallerFunctionInt callerInt11 = CallerFunctionInt::createStatic(&TestCaller::functionInt11, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10);
	CallerFunctionInt callerInt12 = CallerFunctionInt::createStatic(&TestCaller::functionInt12, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11);
	CallerFunctionInt callerInt13 = CallerFunctionInt::createStatic(&TestCaller::functionInt13, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12);
	CallerFunctionInt callerInt14 = CallerFunctionInt::createStatic(&TestCaller::functionInt14, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13);
	CallerFunctionInt callerInt15 = CallerFunctionInt::createStatic(&TestCaller::functionInt15, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14);
	CallerFunctionInt callerInt16 = CallerFunctionInt::createStatic(&TestCaller::functionInt16, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15);
	CallerFunctionInt callerInt17 = CallerFunctionInt::createStatic(&TestCaller::functionInt17, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16);
	CallerFunctionInt callerInt18 = CallerFunctionInt::createStatic(&TestCaller::functionInt18, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16, &param17);
	CallerFunctionInt callerInt19 = CallerFunctionInt::createStatic(&TestCaller::functionInt19, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16, &param17, &param18);
	CallerFunctionInt callerInt20 = CallerFunctionInt::createStatic(&TestCaller::functionInt20, &param0, &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8, &param9, &param10, &param11, &param12, &param13, &param14, &param15, &param16, &param17, &param18, &param19);

	Log::info() << "Testing static functions without return value:";

	caller0();
	caller1();
	caller2();
	caller3();
	caller4();
	caller5();
	caller6();
	caller7();
	caller8();
	caller9();
	caller10();
	caller11();
	caller12();
	caller13();
	caller14();
	caller15();
	caller16();
	caller17();
	caller18();
	caller19();
	caller20();

	Log::info() << " ";
	Log::info() << "Testing static functions with return value:";

	const int result0 = callerInt0();
	const int result1 = callerInt1();
	const int result2 = callerInt2();
	const int result3 = callerInt3();
	const int result4 = callerInt4();
	const int result5 = callerInt5();
	const int result6 = callerInt6();
	const int result7 = callerInt7();
	const int result8 = callerInt8();
	const int result9 = callerInt9();
	const int result10 = callerInt10();
	const int result11 = callerInt11();
	const int result12 = callerInt12();
	const int result13 = callerInt13();
	const int result14 = callerInt14();
	const int result15 = callerInt15();
	const int result16 = callerInt16();
	const int result17 = callerInt17();
	const int result18 = callerInt18();
	const int result19 = callerInt19();
	const int result20 = callerInt20();

	const bool allSucceeded = result0 == 0 && result1 == 1 && result2 == 2 && result3 == 3 && result4 == 4 && result5 == 5 && result6 == 6 && result7 == 7 && result8 == 8 && result9 == 9
									&& result10 == 10 && result11 == 11 && result12 == 12 && result13 == 13 && result14 == 14 && result15 == 15 && result16 == 16 && result17 == 17 && result18 == 18 && result19 == 19 && result20 == 20;

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED.";

	return allSucceeded;
}

bool TestCaller::testCallerParameters()
{
	Log::info() << "Testing mixed parameter types:";

	typedef Caller<int> MixedCaller;

	bool allSucceeded = true;

	{
		Log::info() << "Static function without reference parameters";

		int parameter0 = 7;
		bool parameter1 = true;
		float parameter2 = 5.5f;
		std::string parameter3("test");

		MixedCaller mixedCallerStatic0 = MixedCaller::createStatic(&functionIndividualParameters0, parameter0, &parameter1, &parameter2, parameter3);
		mixedCallerStatic0.setParameter<std::string>(3u, "abcd");
		mixedCallerStatic0();

		if (parameter0 != 7)
			allSucceeded = false;
		if (parameter1 != false)
			allSucceeded = false;
		if (parameter2 != 15.5f)
			allSucceeded = false;
		if (parameter3 != "test") // the (outside) parameter has not chaned
			allSucceeded = false;

		const std::string parameter3_determined = mixedCallerStatic0.parameter<std::string>(3u);

		if (parameter3_determined != "abcd")
			allSucceeded = false;
	}

	{
		Log::info() << "Static function with reference parameters";

		int parameter0 = 7;
		bool parameter1 = true;
		float parameter2 = 15.2f;
		double parameter3 = 7.4;
		std::string parameter4("test");
		std::string parameter5("test");
		const Object parameter6;

		MixedCaller mixedCallerStatic1 = MixedCaller::createStatic<int, bool*, float*, double&, std::string, std::string&, const Object&>(&functionIndividualParameters1, parameter0, &parameter1, &parameter2, parameter3, parameter4, parameter5, parameter6);
		mixedCallerStatic1();

		if (parameter0 != 7)
			allSucceeded = false;
		if (parameter1 != false)
			allSucceeded = false;
		if (parameter2 != 25.2f)
			allSucceeded = false;
		if (parameter3 != 27.4)
			allSucceeded = false;
		if (parameter4 != "test")
			allSucceeded = false;
		if (parameter5 != "tset")
			allSucceeded = false;
	}

	{
		Log::info() << "Non-constant member function without reference parameters";

		int parameter0 = 7;
		bool parameter1 = true;
		float parameter2 = 5.5f;
		std::string parameter3("test");

		Object object;
		MixedCaller mixedCallerStatic0 = MixedCaller::create(object, &Object::nonConstantFunctionIndividualParameters0, parameter0, &parameter1, &parameter2, parameter3);
		mixedCallerStatic0.setParameter<std::string>(3u, "abcd");
		mixedCallerStatic0();

		if (parameter0 != 7)
			allSucceeded = false;
		if (parameter1 != false)
			allSucceeded = false;
		if (parameter2 != 15.5f)
			allSucceeded = false;
		if (parameter3 != "test") // the (outside) parameter has not chaned
			allSucceeded = false;

		const std::string parameter3_determined = mixedCallerStatic0.parameter<std::string>(3u);

		if (parameter3_determined != "abcd")
			allSucceeded = false;
	}

	{
		Log::info() << "Non-constant member function with reference parameters";

		int parameter0 = 7;
		bool parameter1 = true;
		float parameter2 = 15.2f;
		double parameter3 = 7.4;
		std::string parameter4("test");
		std::string parameter5("test");
		const Object parameter6;

		Object object;
		MixedCaller mixedCallerStatic1 = MixedCaller::create<Object, int, bool*, float*, double&, std::string, std::string&, const Object&>(object, &Object::nonConstantFunctionIndividualParameters1, parameter0, &parameter1, &parameter2, parameter3, parameter4, parameter5, parameter6);
		mixedCallerStatic1();

		if (parameter0 != 7)
			allSucceeded = false;
		if (parameter1 != false)
			allSucceeded = false;
		if (parameter2 != 25.2f)
			allSucceeded = false;
		if (parameter3 != 27.4)
			allSucceeded = false;
		if (parameter4 != "test")
			allSucceeded = false;
		if (parameter5 != "tset")
			allSucceeded = false;
	}

	{
		Log::info() << "Constant member function without reference parameters";

		int parameter0 = 7;
		bool parameter1 = true;
		float parameter2 = 5.5f;
		std::string parameter3("test");

		const Object object;
		MixedCaller mixedCallerStatic0 = MixedCaller::create(object, &Object::constantFunctionIndividualParameters0, parameter0, &parameter1, &parameter2, parameter3);
		mixedCallerStatic0.setParameter<std::string>(3u, "abcd");
		mixedCallerStatic0();

		if (parameter0 != 7)
			allSucceeded = false;
		if (parameter1 != false)
			allSucceeded = false;
		if (parameter2 != 15.5f)
			allSucceeded = false;
		if (parameter3 != "test") // the (outside) parameter has not chaned
			allSucceeded = false;

		const std::string parameter3_determined = mixedCallerStatic0.parameter<std::string>(3u);

		if (parameter3_determined != "abcd")
			allSucceeded = false;
	}

	{
		Log::info() << "Constant member function with reference parameters";

		int parameter0 = 7;
		bool parameter1 = true;
		float parameter2 = 15.2f;
		double parameter3 = 7.4;
		std::string parameter4("test");
		std::string parameter5("test");
		const Object parameter6;

		const Object object;
		MixedCaller mixedCallerStatic1 = MixedCaller::create<Object, int, bool*, float*, double&, std::string, std::string&, const Object&>(object, &Object::constantFunctionIndividualParameters1, parameter0, &parameter1, &parameter2, parameter3, parameter4, parameter5, parameter6);
		mixedCallerStatic1();

		if (parameter0 != 7)
			allSucceeded = false;
		if (parameter1 != false)
			allSucceeded = false;
		if (parameter2 != 25.2f)
			allSucceeded = false;
		if (parameter3 != 27.4)
			allSucceeded = false;
		if (parameter4 != "test")
			allSucceeded = false;
		if (parameter5 != "tset")
			allSucceeded = false;
	}

	if (allSucceeded)
		Log::info() << "Validation: succeeded.";
	else
		Log::info() << "Validation: FAILED!";

	return allSucceeded;
}

void TestCaller::function0()
{
	Log::info() << "void function(), 0 parameter(s)";
}

void TestCaller::function1(Param0* /*p0*/)
{
	Log::info() << "void function(), 1 parameter(s)";
}

void TestCaller::function2(Param0* /*p0*/, Param1* /*p1*/)
{
	Log::info() << "void function(), 2 parameter(s)";
}

void TestCaller::function3(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/)
{
	Log::info() << "void function(), 3 parameter(s)";
}

void TestCaller::function4(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/)
{
	Log::info() << "void function(), 4 parameter(s)";
}

void TestCaller::function5(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/)
{
	Log::info() << "void function(), 5 parameter(s)";
}

void TestCaller::function6(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/)
{
	Log::info() << "void function(), 6 parameter(s)";
}

void TestCaller::function7(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/)
{
	Log::info() << "void function(), 7 parameter(s)";
}

void TestCaller::function8(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/)
{
	Log::info() << "void function(), 8 parameter(s)";
}

void TestCaller::function9(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/)
{
	Log::info() << "void function(), 9 parameter(s)";
}

void TestCaller::function10(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/)
{
	Log::info() << "void function(), 10 parameter(s)";
}

void TestCaller::function11(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/)
{
	Log::info() << "void function(), 11 parameter(s)";
}

void TestCaller::function12(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/)
{
	Log::info() << "void function(), 12 parameter(s)";
}

void TestCaller::function13(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/)
{
	Log::info() << "void function(), 13 parameter(s)";
}

void TestCaller::function14(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/)
{
	Log::info() << "void function(), 14 parameter(s)";
}

void TestCaller::function15(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/)
{
	Log::info() << "void function(), 15 parameter(s)";
}

void TestCaller::function16(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/)
{
	Log::info() << "void function(), 16 parameter(s)";
}

void TestCaller::function17(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/)
{
	Log::info() << "void function(), 17 parameter(s)";
}

void TestCaller::function18(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/, Param17* /*p17*/)
{
	Log::info() << "void function(), 18 parameter(s)";
}

void TestCaller::function19(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/, Param17* /*p17*/, Param18* /*p18*/)
{
	Log::info() << "void function(), 19 parameter(s)";
}

void TestCaller::function20(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/, Param17* /*p17*/, Param18* /*p18*/, Param19* /*p19*/)
{
	Log::info() << "void function(), 20 parameter(s)";
}

int TestCaller::functionInt0()
{
	Log::info() << "int function(), 0 parameter(s)";
	return 0;
}

int TestCaller::functionInt1(Param0* /*p0*/)
{
	Log::info() << "int function(), 1 parameter(s)";
	return 1;
}

int TestCaller::functionInt2(Param0* /*p0*/, Param1* /*p1*/)
{
	Log::info() << "int function(), 2 parameter(s)";
	return 2;
}

int TestCaller::functionInt3(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/)
{
	Log::info() << "int function(), 3 parameter(s)";
	return 3;
}

int TestCaller::functionInt4(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/)
{
	Log::info() << "int function(), 4 parameter(s)";
	return 4;
}

int TestCaller::functionInt5(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/)
{
	Log::info() << "int function(), 5 parameter(s)";
	return 5;
}

int TestCaller::functionInt6(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/)
{
	Log::info() << "int function(), 6 parameter(s)";
	return 6;
}

int TestCaller::functionInt7(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/)
{
	Log::info() << "int function(), 7 parameter(s)";
	return 7;
}

int TestCaller::functionInt8(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/)
{
	Log::info() << "int function(), 8 parameter(s)";
	return 8;
}

int TestCaller::functionInt9(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/)
{
	Log::info() << "int function(), 9 parameter(s)";
	return 9;
}

int TestCaller::functionInt10(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/)
{
	Log::info() << "int function(), 10 parameter(s)";
	return 10;
}

int TestCaller::functionInt11(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/)
{
	Log::info() << "int function(), 11 parameter(s)";
	return 11;
}

int TestCaller::functionInt12(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/)
{
	Log::info() << "int function(), 12 parameter(s)";
	return 12;
}

int TestCaller::functionInt13(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/)
{
	Log::info() << "int function(), 13 parameter(s)";
	return 13;
}

int TestCaller::functionInt14(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/)
{
	Log::info() << "int function(), 14 parameter(s)";
	return 14;
}

int TestCaller::functionInt15(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/)
{
	Log::info() << "int function(), 15 parameter(s)";
	return 15;
}

int TestCaller::functionInt16(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/)
{
	Log::info() << "int function(), 16 parameter(s)";
	return 16;
}

int TestCaller::functionInt17(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/)
{
	Log::info() << "int function(), 17 parameter(s)";
	return 17;
}

int TestCaller::functionInt18(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/, Param17* /*p17*/)
{
	Log::info() << "int function(), 18 parameter(s)";
	return 18;
}

int TestCaller::functionInt19(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/, Param17* /*p17*/, Param18* /*p18*/)
{
	Log::info() << "int function(), 19 parameter(s)";
	return 19;
}

int TestCaller::functionInt20(Param0* /*p0*/, Param1* /*p1*/, Param2* /*p2*/, Param3* /*p3*/, Param4* /*p4*/, Param5* /*p5*/, Param6* /*p6*/, Param7* /*p7*/, Param8* /*p8*/, Param9* /*p9*/, Param10* /*p10*/, Param11* /*p11*/, Param12* /*p12*/, Param13* /*p13*/, Param14* /*p14*/, Param15* /*p15*/, Param16* /*p16*/, Param17* /*p17*/, Param18* /*p18*/, Param19* /*p19*/)
{
	Log::info() << "int function(), 20 parameter(s)";
	return 20;
}

int TestCaller::functionIndividualParameters0(int parameter0, bool* parameter1, float* parameter2, std::string parameter3)
{
	ocean_assert(parameter1 != nullptr && parameter2 != nullptr);

	parameter0++;
	OCEAN_SUPPRESS_UNUSED_WARNING(parameter0);

	*parameter1 = !*parameter1;
	*parameter2 += 10.0f;
	parameter3.clear();

	return -1;
}

int TestCaller::functionIndividualParameters1(int parameter0, bool* parameter1, float* parameter2, double& parameter3, std::string parameter4, std::string& parameter5, const Object& /*parameter6*/)
{
	ocean_assert(parameter1 != nullptr && parameter2 != nullptr);

	parameter0++;
	OCEAN_SUPPRESS_UNUSED_WARNING(parameter0);

	*parameter1 = !*parameter1;
	*parameter2 += 10.0f;
	parameter3 += 20.0;
	parameter4.clear();

	for (size_t n = 0; n < parameter5.size() / 2; ++n)
	{
		std::swap(parameter5[n], parameter5[parameter5.size() - n - 1]);
	}

	return -1;
}

}

}

}
