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

	bool allSucceeded = true;

	CommandArguments commandArguments;
	commandArguments.registerParameter("first", "f", "First parameter");
	commandArguments.registerParameter("second", "s", "Second parameter", Value(5));
	commandArguments.registerParameter("third", "t", "", Value("Default value"));

	const std::string commandLine = "--second 37 nameLessValue -t";

	if (commandArguments.parse(commandLine.c_str()))
	{
		if (commandArguments.hasValue("first"))
		{
			allSucceeded = false;
		}

		if (commandArguments.value("first"))
		{
			allSucceeded = false;
		}

		Value secondValue;
		if (commandArguments.hasValue("second", &secondValue) == false)
		{
			allSucceeded = false;
		}
		else
		{
			if (secondValue.isNull())
			{
				allSucceeded = false;
			}

			if (!secondValue.isInt() || secondValue.intValue() != 37)
			{
				allSucceeded = false;
			}
		}

		Value thirdValue;
		if (commandArguments.hasValue("third", &thirdValue) == false)
		{
			allSucceeded = false;
		}
		else
		{
			if (thirdValue.isNull() == false)
			{
				allSucceeded = false;
			}
		}

		const Strings& namelessValues = commandArguments.namelessValues();

		if (namelessValues.size() == 1)
		{
			if (namelessValues[0] != "nameLessValue")
			{
				allSucceeded = false;
			}
		}
		else
		{
			allSucceeded = false;
		}
	}
	else
	{
		allSucceeded = false;
	}

	if (allSucceeded)
	{
		Log::info() << "Validation: succeeded.";
	}
	else
	{
		Log::info() << "Validation: FAILED!";
	}

	return allSucceeded;
}

}

}

}
