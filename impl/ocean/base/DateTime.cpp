/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/DateTime.h"
#include "ocean/base/String.h"
#include "ocean/base/Timestamp.h"

#if defined(_WINDOWS)
	#include <winsock2.h>
	#include <windows.h>
#elif defined(__APPLE__) || defined(__linux__) || defined(__EMSCRIPTEN__)
	#include <sys/time.h>
	#include <math.h>
#else
	#include <time.h>
#endif

#include <ctime>
#include <stdio.h>

namespace Ocean
{

double DateTime::timestamp()
{

#if defined(_WINDOWS)

	ocean_assert(sizeof(FILETIME) == sizeof(long long));

	SYSTEMTIME systemTime;
	long long time;

	GetSystemTime(&systemTime);
	SystemTimeToFileTime(&systemTime, (FILETIME*)&time);

	// Set 0.0 to 00:00:00 1970.01.01
	return double(time) * 0.0000001 - 11644473600.0;

#else

	timeval currentTime;
	gettimeofday(&currentTime, nullptr);

	return double(currentTime.tv_sec) + double(currentTime.tv_usec) * 0.000001;

#endif

}

long long DateTime::microseconds()
{

#if defined(_WINDOWS)

	ocean_assert(sizeof(FILETIME) == sizeof(long long));

	SYSTEMTIME systemTime;
	long long time;

	GetSystemTime(&systemTime);
	SystemTimeToFileTime(&systemTime, (FILETIME*)&time);

	// Set 0.0 to 00:00:00 1970.01.01
	return (long long)(double(time) * 0.1 - 11644473600000000.0);

#else

	timeval currentTime;
	gettimeofday(&currentTime, nullptr);

	return (long long)(currentTime.tv_sec) * 1000000ll + (long long)(currentTime.tv_usec);

#endif

}

std::string DateTime::string(const bool addMilliseconds)
{
	return stringDate() + std::string(" ") + stringTime(addMilliseconds);
}

std::string DateTime::string(const double timestamp, const bool addMilliseconds)
{
	unsigned int year = 0u, month = 0u, day = 0u, hour = 0u, minute = 0u, second = 0u, millisecond = 0u;
	timestamp2date(timestamp, year, month, day, hour, minute, second, &millisecond);

	if (year == 0u)
		return std::string();

	if (addMilliseconds)
	{
		return String::toAString(year) + std::string(".") + String::toAString(month, 2u) + std::string(".") + String::toAString(day, 2u)
					+ std::string(" ") + String::toAString(hour, 2u) + std::string("-") + String::toAString(minute, 2u)
					+ std::string("-") + String::toAString(second, 2u) + std::string("-") + String::toAString(millisecond, 4u);
	}

	return String::toAString(year) + std::string(".") + String::toAString(month, 2u) + std::string(".") + String::toAString(day, 2u)
					+ std::string(" ") + String::toAString(hour, 2u) + std::string("-") + String::toAString(minute, 2u) + std::string("-") + String::toAString(second, 2u);
}

std::string DateTime::stringDate(const char delimiter)
{

#if defined(_WINDOWS)

	SYSTEMTIME systemTime;
	GetSystemTime(&systemTime);

	const unsigned int year = systemTime.wYear;
	const unsigned int month = systemTime.wMonth;
	const unsigned int day = systemTime.wDay;

#elif defined(__linux__)

	// On Linux, the result of time() vs. gettimeofday() may be different, for
	// details see https://stackoverflow.com/a/23597725. To avoid problems, only
	// use gettimeofday() on Linux.
	const Timestamp timestamp(true);
	unsigned int year = 0u, month = 0u, day = 0u, hour = 0u, minute = 0u, second = 0u;
	timestamp2date(double(timestamp), year, month, day, hour, minute, second);

#else

	time_t currentTime;
	time(&currentTime);

	const tm* value = gmtime(&currentTime);
	ocean_assert(value != nullptr);

	const unsigned int year = 1900 + value->tm_year;
	const unsigned int month = value->tm_mon + 1;
	const unsigned int day = value->tm_mday;

#endif

	return String::toAString(year) + std::string(1, delimiter) + String::toAString(month, 2u) + std::string(1, delimiter) + String::toAString(day, 2u);
}

std::string DateTime::stringDate(const double timestamp, const bool nameMonths)
{
	ocean_assert(timestamp >= 0.0);

	unsigned int year = 0u, month = 0u, day = 0u, hour = 0u, minute = 0u, second = 0u;
	timestamp2date(timestamp, year, month, day, hour, minute, second);

	if (year == 0u)
		return std::string();

	if (nameMonths)
	{
		const std::string monthNames[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

		ocean_assert(month >= 1u && month <= 12u);
		return monthNames[month - 1u] + std::string(" ") + String::toAString(day) + std::string(", ") + String::toAString(year);
	}

	return String::toAString(year) + std::string(".") + String::toAString(month, 2u) + std::string(".") + String::toAString(day, 2u);
}

std::string DateTime::stringTime(const bool addMilliseconds, const char delimiter)
{

#if defined(_WINDOWS)

	SYSTEMTIME systemTime;
	GetSystemTime(&systemTime);

	const unsigned int hour = systemTime.wHour;
	const unsigned int minute = systemTime.wMinute;
	const unsigned int second = systemTime.wSecond;
	const unsigned int millisecond = systemTime.wMilliseconds;

#elif defined(__linux__)

	// On Linux, the result of time() vs. gettimeofday() may be different, for
	// details see https://stackoverflow.com/a/23597725. To avoid problems, only
	// use gettimeofday() on Linux.
	const Timestamp timestamp(true);
	unsigned int year = 0u, month = 0u, day = 0u, hour = 0u, minute = 0u, second = 0u, millisecond = 0u;
	timestamp2date(double(timestamp), year, month, day, hour, minute, second, &millisecond);

#else

	time_t t = time(nullptr);
	const tm *timeStruct = gmtime(&t);
	ocean_assert(timeStruct);

	const unsigned int hour = timeStruct->tm_hour;
	const unsigned int minute = timeStruct->tm_min;
	const unsigned int second = timeStruct->tm_sec;

	const Timestamp timestamp(true);
	const unsigned int millisecond = (unsigned int)((double(timestamp) - floor(double(timestamp))) * 1000.0);

#endif

	if (addMilliseconds)
	{
		return String::toAString(hour, 2u) + std::string(1, delimiter) + String::toAString(minute, 2u)
					+ std::string(1, delimiter) + String::toAString(second, 2u) + std::string(1, delimiter) + String::toAString(millisecond, 4u);
	}

	return String::toAString(hour, 2u) + std::string(1, delimiter) + String::toAString(minute, 2u) + std::string(1, delimiter) + String::toAString(second, 2u);
}

std::string DateTime::stringTime(const double timestamp, const bool addMilliseconds, const char delimiter)
{
	unsigned int year = 0u, month = 0u, day = 0u, hour = 0u, minute = 0u, second = 0u, millisecond = 0u;
	timestamp2date(timestamp, year, month, day, hour, minute, second, &millisecond);

	if (year == 0u)
	{
		return std::string();
	}

	if (addMilliseconds)
	{
		return String::toAString(hour, 2u) + std::string(1, delimiter) + String::toAString(minute, 2u)
					+ std::string(1, delimiter) + String::toAString(second, 2u) + std::string(1, delimiter) + String::toAString(millisecond, 4u);
	}

	return String::toAString(hour, 2u) + std::string(1, delimiter) + String::toAString(minute, 2u) + std::string(1, delimiter) + String::toAString(second, 2u);
}

double DateTime::localTimestamp()
{

#if defined(_WINDOWS)

	ocean_assert(sizeof(FILETIME) == sizeof(long long));

	SYSTEMTIME systemTime;
	long long time;

	GetLocalTime(&systemTime);
	SystemTimeToFileTime(&systemTime, (FILETIME*)&time);

	// Set 0.0 to 00:00:00 1970.01.01
	return double(time) * 0.0000001 - 11644473600.0;

#else

	timeval utcTime;
	gettimeofday(&utcTime, nullptr);

	const time_t localSeconds = mktime(localtime((time_t*)&utcTime.tv_sec));

	return double(localSeconds) + double(utcTime.tv_usec) * 0.000001;

#endif

}

long long DateTime::localMicroseconds()
{

#if defined(_WINDOWS)

	ocean_assert(sizeof(FILETIME) == sizeof(long long));

	SYSTEMTIME systemTime;
	long long time;

	GetLocalTime(&systemTime);
	SystemTimeToFileTime(&systemTime, (FILETIME*)&time);

	// Set 0.0 to 00:00:00 1970.01.01
	return (long long)(double(time) * 0.1 - 11644473600000000.0);

#else

	timeval utcTime;
	gettimeofday(&utcTime, nullptr);

	const time_t localSeconds = mktime(localtime((time_t*)&utcTime.tv_sec));

	return (long long)(localSeconds) * 1000000ll + (long long)(utcTime.tv_usec);

#endif

}

std::string DateTime::localString()
{
	return localStringDate() + std::string(" ") + localStringTime();
}

std::string DateTime::localStringDate(const char delimiter)
{

#if defined(_WINDOWS)

	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	const unsigned int year = systemTime.wYear;
	const unsigned int month = systemTime.wMonth;
	const unsigned int day = systemTime.wDay;

#elif defined(__linux__)

	// On Linux, the result of time() vs. gettimeofday() may be different, for
	// details see https://stackoverflow.com/a/23597725. To avoid problems, only
	// use gettimeofday() on Linux.
	const Timestamp timestamp(true);
	unsigned int year = 0u, month = 0u, day = 0u, hour = 0u, minute = 0u, second = 0u;
	timestamp2date(double(timestamp), year, month, day, hour, minute, second);

#else

	time_t currentTime;
	time(&currentTime);

	const tm* value = localtime(&currentTime);
	ocean_assert(value != nullptr);

	const unsigned int year = 1900 + value->tm_year;
	const unsigned int month = value->tm_mon + 1;
	const unsigned int day = value->tm_mday;


#endif

	return String::toAString(year) + std::string(1, delimiter) + String::toAString(month, 2u) + std::string(1, delimiter) + String::toAString(day, 2u);
}

double DateTime::date2timestamp(const unsigned int year, const unsigned int month, const unsigned int day, const unsigned int hour, const unsigned int minute, const unsigned int second, const unsigned int millisecond)
{
	ocean_assert(year >= 1970 && year <= 2037 && month >= 1u && month <= 12u && day >= 1u && day <= 31u);
	ocean_assert(hour <= 23u && minute <= 59u && second <= 59u && millisecond <= 999u);

#ifdef _WINDOWS

	SYSTEMTIME systemTime;
	systemTime.wYear = WORD(year);
	systemTime.wMonth = WORD(month);
	systemTime.wDay = WORD(day);
	systemTime.wHour = WORD(hour);
	systemTime.wMinute = WORD(minute);
	systemTime.wSecond = WORD(second);
	systemTime.wMilliseconds = WORD(millisecond);
	systemTime.wDayOfWeek = 0;

	long long time;
	if (SystemTimeToFileTime(&systemTime, (FILETIME*)&time) == FALSE)
		return -1;

	// Set 0.0 to 00:00:00 1970.01.01
	return double(time) * 0.0000001 - 11644473600.0;

#else

	if (!isDateValid(year, month, day))
		return -1;

	tm time;
	memset(&time, 0, sizeof(tm));

	time.tm_year = int(year) - 1900;
	time.tm_mon = int(month) - 1;
	time.tm_mday = int(day);
	time.tm_hour = int(hour);
	time.tm_min = int(minute);
	time.tm_sec = int(second);

	const time_t timeSeconds = mktime(&time);
	const time_t seconds = timeSeconds -timezone;

	return double(seconds) + double(millisecond) * double(0.001);

#endif

}

void DateTime::timestamp2date(const double timestamp, unsigned int& year, unsigned int& month, unsigned int& day, unsigned int& hour, unsigned int& minute, unsigned int& second, unsigned int* millisecond)
{
	ocean_assert(timestamp >= 0);

#ifdef _WINDOWS

	// Set 0.0 to 00:00:00 1970.01.01
	const long long time = (long long)((timestamp + 11644473600.0005) * 10000000);

	SYSTEMTIME systemTime;
	const BOOL result = FileTimeToSystemTime((FILETIME*)(&time), &systemTime);
	ocean_assert_and_suppress_unused(result == TRUE, result);

	year = (unsigned int)(systemTime.wYear);
	month = (unsigned int)(systemTime.wMonth);
	day = (unsigned int)(systemTime.wDay);
	hour = (unsigned int)(systemTime.wHour);
	minute = (unsigned int)(systemTime.wMinute);
	second = (unsigned int)(systemTime.wSecond);

	if (millisecond)
	{
		*millisecond = (unsigned int)(systemTime.wMilliseconds);
	}

#else

	const long long seconds = (long long)(timestamp);

	if (millisecond)
	{
		*millisecond = (unsigned int)((timestamp - double(seconds)) * 1000.0 + 0.5);
	}

	const time_t sTime = (time_t)seconds;

	tm time;
	gmtime_r(&sTime, &time);

	year = time.tm_year + 1900;
	month = time.tm_mon + 1;
	day = time.tm_mday;
	hour = time.tm_hour;
	minute = time.tm_min;
	second = time.tm_sec;

#endif

}

std::string DateTime::seconds2string(const double totalSeconds, const bool addDescription, const bool addMilliseconds)
{
	ocean_assert(totalSeconds >= 0.0);
	ocean_assert(totalSeconds < 15461882265600.0);

	double value = totalSeconds;

	// 1 hour = 3.600s
	const unsigned long long hours = (unsigned long long)(value) / 3600ull;
	value -= double(hours) * 3600.0;
	ocean_assert(value >= 0.0);

	// 1 minute = 60s
	const unsigned int minutes = (unsigned int)(value) / 60u;
	value -= double(minutes) * 60.0;
	ocean_assert(value >= 0.0);

	const unsigned int seconds = (unsigned int)(value);

	value -= double(seconds);
	ocean_assert(value >= 0.0);

	const unsigned int milliseconds = (unsigned int)(value * 1000.0);

	ocean_assert(minutes < 60u);
	ocean_assert(seconds < 60u);
	ocean_assert(milliseconds < 1000u);

	if (addDescription)
	{
		std::string result(String::toAString(hours) + std::string(" hours, ") + String::toAString(minutes) + std::string(" minutes, ") + String::toAString(seconds) + std::string(" seconds"));

		if (addMilliseconds)
		{
			result += std::string(", ") + String::toAString(milliseconds) + std::string(" milliseconds");
		}

		return result;
	}

	std::string result((hours > 100u) ? String::toAString(hours) : String::toAString((unsigned int)hours, 2u) + std::string(":") + String::toAString(minutes, 2u) + std::string(":") + String::toAString(seconds, 2u));

	if (addMilliseconds)
	{
		result += std::string(", ") + String::toAString(milliseconds, 4u);
	}

	return result;
}

std::string DateTime::localStringTime(const bool addMilliseconds, const char delimiter)
{

#if defined(_WINDOWS)

	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	const unsigned int hour = systemTime.wHour;
	const unsigned int minute = systemTime.wMinute;
	const unsigned int second = systemTime.wSecond;
	const unsigned int millisecond = systemTime.wMilliseconds;

#elif defined(__linux__)

	// On Linux, the result of time() vs. gettimeofday() may be different, for
	// details see https://stackoverflow.com/a/23597725. To avoid problems, only
	// use gettimeofday() on Linux.
	const Timestamp timestamp(true);
	unsigned int year = 0u, month = 0u, day = 0u, hour = 0u, minute = 0u, second = 0u, millisecond = 0u;
	timestamp2date(double(timestamp), year, month, day, hour, minute, second, &millisecond);

#else

	time_t t = time(nullptr);
	const tm *timeStruct = localtime(&t);
	ocean_assert(timeStruct);

	const unsigned int hour = timeStruct->tm_hour;
	const unsigned int minute = timeStruct->tm_min;
	const unsigned int second = timeStruct->tm_sec;

	const Timestamp timestamp(true);
	const unsigned int millisecond = (unsigned int)((double(timestamp) - floor(double(timestamp))) * 1000.0);

#endif

	if (addMilliseconds)
	{
		return String::toAString(hour, 2u) + std::string(1, delimiter) + String::toAString(minute, 2u)
					+ std::string(1, delimiter) + String::toAString(second, 2u) + std::string(1, delimiter) + String::toAString(millisecond, 4u);
	}

	return String::toAString(hour, 2u) + std::string(1, delimiter) + String::toAString(minute, 2u) + std::string(1, delimiter) + String::toAString(second, 2u);
}

bool DateTime::isDateValid(const unsigned int year, const unsigned int month, const unsigned int day)
{
	ocean_assert(month >= 1u && month <= 12u);
	ocean_assert(day >= 1u && day <= 31u);

	switch (month)
	{
		case 1u:
			return day >= 1u && day <= 31u;

		case 2u:
			if (isLeapYear(year))
			{
				return day >= 1u && day <= 29u;
			}
			else
			{
				return day >= 1u && day <= 28u;
			}

		case 3u:
			return day >= 1u && day <= 31u;

		case 4u:
			return day >= 1u && day <= 30u;

		case 5u:
			return day >= 1u && day <= 31u;

		case 6u:
			return day >= 1u && day <= 30u;

		case 7u:
			return day >= 1u && day <= 31u;

		case 8u:
			return day >= 1u && day <= 31u;

		case 9u:
			return day >= 1u && day <= 30u;

		case 10u:
			return day >= 1u && day <= 31u;

		case 11u:
			return day >= 1u && day <= 30u;

		case 12u:
			return day >= 1u && day <= 31u;
	}

	ocean_assert(false && "Date is out of range!");
	return false;
}

}
