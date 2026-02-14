/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestCommandArguments.h"

#include "ocean/base/CommandArguments.h"

#include "ocean/test/TestResult.h"
#include "ocean/test/TestSelector.h"
#include "ocean/test/Validation.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestCommandArguments::test(const double /*testDuration*/, const TestSelector& selector)
{
	TestResult testResult("Command arguments test");

	Log::info() << " ";

	if (selector.shouldRun("parse"))
	{
		testResult = testParse();

		Log::info() << " ";
		Log::info() << "-";
		Log::info() << " ";
	}

	Log::info() << " ";

	Log::info() << testResult;

	return testResult.succeeded();
}

#ifdef OCEAN_USE_GTEST

TEST(TestCommandArguments, Parse)
{
	EXPECT_TRUE(TestCommandArguments::testParse());
}

#endif // OCEAN_USE_GTEST

bool TestCommandArguments::testParse()
{
	Log::info() << "Parse test:";

	Validation validation;

	CommandArguments commandArguments;
	commandArguments.registerParameter("first", "f", "First parameter");
	commandArguments.registerParameter("second", "s", "Second parameter", Value(5));
	commandArguments.registerParameter("third", "t", "", Value("Default value"));

	const std::string commandLine = "--second 37 nameLessValue -t";

	if (commandArguments.parse(commandLine.c_str()))
	{
		OCEAN_EXPECT_FALSE(validation, commandArguments.hasValue("first"));

		OCEAN_EXPECT_TRUE(validation, !commandArguments.value("first"));

		Value secondValue;
		if (commandArguments.hasValue("second", &secondValue) == false)
		{
			OCEAN_SET_FAILED(validation);
		}
		else
		{
			OCEAN_EXPECT_FALSE(validation, secondValue.isNull());

			OCEAN_EXPECT_TRUE(validation, secondValue.isInt() && secondValue.intValue() == 37);
		}

		Value thirdValue;
		if (commandArguments.hasValue("third", &thirdValue) == false)
		{
			OCEAN_SET_FAILED(validation);
		}
		else
		{
			OCEAN_EXPECT_TRUE(validation, thirdValue.isNull());
		}

		const Strings& namelessValues = commandArguments.namelessValues();

		if (namelessValues.size() == 1)
		{
			OCEAN_EXPECT_EQUAL(validation, namelessValues[0], std::string("nameLessValue"));
		}
		else
		{
			OCEAN_SET_FAILED(validation);
		}
	}
	else
	{
		OCEAN_SET_FAILED(validation);
	}

	Log::info() << "Validation: " << validation;

	return validation.succeeded();
}

}

}

}
