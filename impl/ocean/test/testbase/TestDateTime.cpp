/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/test/testbase/TestDateTime.h"

#include "ocean/base/DateTime.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Test
{

namespace TestBase
{

bool TestDateTime::test(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "---   DateTime test:   ---";
	Log::info() << " ";

	bool allSucceeded = true;

	allSucceeded = testConversion(testDuration) && allSucceeded;

	Log::info() << " ";

	if (allSucceeded)
	{
		Log::info() << "DateTime test succeeded.";
	}
	else
	{
		Log::info() << "DateTime test FAILED!";
	}

	return allSucceeded;
}

#ifdef OCEAN_USE_GTEST

TEST(TestDateTime, Conversion)
{
	EXPECT_TRUE(TestDateTime::testConversion(GTEST_TEST_DURATION));
}

#endif // OCEAN_USE_GTEST

bool TestDateTime::testConversion(const double testDuration)
{
	ocean_assert(testDuration > 0.0);

	Log::info() << "DateTime to Timestamp conversion test:";

	bool allSucceeded = true;

	double dummyValue0 = 0;
	unsigned int dummyValue1 = 0u;

	HighPerformanceStatistic performanceForward, performanceBackward;

	if (DateTime::date2timestamp(2014u, 2u, 28u, 0u, 0u, 0u) == -1.0 || DateTime::date2timestamp(2014u, 2u, 29u, 0u, 0u, 0u) != -1.0
		|| DateTime::date2timestamp(2012u, 2u, 28u, 0u, 0u, 0u) == -1.0 || DateTime::date2timestamp(2012u, 2u, 29u, 0u, 0u, 0u) == -1.0
		|| DateTime::date2timestamp(2012u, 2u, 30u, 0u, 0u, 0u) != -1.0 || DateTime::date2timestamp(2012u, 2u, 31u, 0u, 0u, 0u) != -1.0
		|| DateTime::date2timestamp(2014u, 11u, 30u, 0u, 0u, 0u) == -1.0 || DateTime::date2timestamp(2014u, 11u, 31u, 0u, 0u, 0u) != -1.0)
	{
		allSucceeded = false;
	}

	const Timestamp startTimestamp(true);
	do
	{
		unsigned int year, month, day, hour, minute, second, millisecond;

		Timestamp timestamp(true);

		bool localSucceeded = false;
		for (unsigned int i = 0u; !localSucceeded && i < 2u; ++i)
		{
			const std::string timeString = DateTime::stringTime(false);
			const std::string dateString = DateTime::stringDate();

			timestamp = Timestamp(true);
			DateTime::timestamp2date(double(timestamp), year, month, day, hour, minute, second);

			const std::string _timeString = String::toAString(hour, 2u) + std::string("-") + String::toAString(minute, 2u) + std::string("-") + String::toAString(second, 2u);
			const std::string _dateString = String::toAString(year) + std::string(".") + String::toAString(month, 2u) + std::string(".") + String::toAString(day, 2u);

			localSucceeded = timeString == _timeString && dateString == _dateString;
		}

		if (!localSucceeded)
		{
			allSucceeded = false;
		}

		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			year = RandomI::random(1970u, 2037u);
			month = RandomI::random(1u, 12u);
			day = RandomI::random(1u, 28u);

			hour = RandomI::random(0u, 23u);
			minute = RandomI::random(0u, 59u);
			second = RandomI::random(0u, 59u);

			millisecond = RandomI::random(0u, 999u);

			unsigned int _year, _month, _day, _hour, _minute, _second, _millisecond;

			const double timestampValue = DateTime::date2timestamp(year, month, day, hour, minute, second, millisecond);
			DateTime::timestamp2date(timestampValue, _year, _month, _day, _hour, _minute, _second, &_millisecond);

			ocean_assert(year == _year && month == _month && day == _day);
			ocean_assert(hour == _hour && minute == _minute && second == _second && millisecond == _millisecond);

			if (year != _year || month != _month || day != _day || hour != _hour || minute != _minute || second != _second || millisecond != _millisecond)
			{
				allSucceeded = false;
			}
		}

		year = RandomI::random(1970u, 2037u);
		month = RandomI::random(1u, 12u);
		day = RandomI::random(1u, 28u);

		hour = RandomI::random(0u, 23u);
		minute = RandomI::random(0u, 59u);
		second = RandomI::random(0u, 59u);

		millisecond = RandomI::random(0u, 999u);

		performanceForward.start();
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			dummyValue0 += DateTime::date2timestamp(year, month, day, hour, minute, second, millisecond) * 0.0000001;
		}
		performanceForward.stop();

		performanceBackward.start();
		for (unsigned int n = 0u; n < 1000u; ++n)
		{
			DateTime::timestamp2date(double(timestamp), year, month, day, hour, minute, second, &millisecond);
			dummyValue1 += year;
		}
		performanceBackward.stop();
	}
	while (startTimestamp + testDuration > Timestamp(true));

	Log::info() << "Forward performance: " << performanceForward.averageMseconds() << "mys";
	Log::info() << "Backward performance: " << performanceBackward.averageMseconds() << "mys";

	if (allSucceeded)
	{
		if (dummyValue0 >= 1.0 && dummyValue1 >= 1u) // ensuring that dummy values are used
		{
			Log::info() << "Validation: succeeded.";
		}
		else
		{
			Log::info() << "Validation: succeeded.";
		}
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
