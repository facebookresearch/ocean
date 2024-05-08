/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_DATE_TIME_H
#define META_OCEAN_BASE_DATE_TIME_H

#include "ocean/base/Base.h"

namespace Ocean
{

/**
 * This class provides basic functionalities for date and time handling.<br>
 * All functions are available for UTC and local time.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT DateTime
{
	public:

		/**
		 * Returns the seconds since 1970.01.01 00::00::00 to current UTC time
		 * @return Seconds
		 */
		static double timestamp();

		/**
		 * Returns the microseconds since 1970.01.01 00::00::00 to current UTC time
		 * @return Microseconds
		 */
		static long long microseconds();

		/**
		 * Returns the string of the UTC date and time.
		 * @param addMilliseconds True, to add the milliseconds to the string
		 * @return Date and time string
		 */
		static std::string string(const bool addMilliseconds = false);

		/**
		 * Returns the string of a given unix timestamp.
		 * @param timestamp The unix timestamp (seconds since 1970.01.01 00:00:00) to be converted, with range [0, infinity)
		 * @param addMilliseconds True, to add the milliseconds to the string
		 * @return Date and time string
		 */
		static std::string string(const double timestamp, const bool addMilliseconds = false);

		/**
		 * Returns the string of the UTC date.
		 * @param delimiter The delimiter to be used
		 * @return Date string, with pattern YYYY.MM.DD, if delimiter is '.'
		 */
		static std::string stringDate(const char delimiter = '.');

		/**
		 * Returns the string of a given unix timestamp.
		 * @param timestamp The unix timestamp (seconds since 1970.01.01 00:00:00) to be converted, with range [0, infinity)
		 * @param nameMonths True, to receive a date with named months; False, to receive a date with digits only (but with constant length)
		 * @return Date string, with pattern YYYY.MM.DD (2016.08.05) for nameMonths=false; or MMMMMMMMMM D/DD, YYYY (May 5, 2016) for nameMonths=true
		 */
		static std::string stringDate(const double timestamp, const bool nameMonths = false);

		/**
		 * Returns the string of the UTC time.
		 * The resulting string has the following format: HH-MM-SS, or HH-MM-SS-mmmm if the milliseconds are added.
		 * @param addMilliseconds True, to add the milliseconds to the string
		 * @param delimiter The delimiter to be used
		 * @return Time string of UTC time
		 */
		static std::string stringTime(const bool addMilliseconds = false, const char delimiter = '-');

		/**
		 * Returns the string of a given unix timestamp.
		 * The resulting string has the following format: HH-MM-SS, or HH-MM-SS-mmmm if the milliseconds are added.
		 * @param timestamp The unix timestamp (seconds since 1970.01.01 00:00:00) to be converted, with range [0, infinity)
		 * @param addMilliseconds True, to add the milliseconds to the string
		 * @param delimiter The delimiter to be used
		 * @return Time string of the UTC time
		 */
		static std::string stringTime(const double timestamp, const bool addMilliseconds = false, const char delimiter = '-');

		/**
		 * Returns the seconds since 1970.01.01 00::00::00 to current local time
		 * @return Seconds
		 */
		static double localTimestamp();

		/**
		 * Returns the microseconds since 1970.01.01 00::00::00 to current local time
		 * @return Microseconds
		 */
		static long long localMicroseconds();

		/**
		 * Returns the string of the local date and time.
		 * @return Date and time string
		 */
		static std::string localString();

		/**
		 * Returns the string of the local date.
		 * @param delimiter The delimiter to be used
		 * @return Date string of local date, with format YYYY.MM.DD, if delimiter is '.'
		 */
		static std::string localStringDate(const char delimiter = '.');

		/**
		 * Converts a date to the seconds since 1970.01.01 00:00:00.
		 * @param year The year, with range [1970, 2037)
		 * @param month The month, with range [1, 12]
		 * @param day The day, with range [1, 31]
		 * @param hour The hour, with range [0, 23]
		 * @param minute The minute, with range [0, 59]
		 * @param second The second, with range [0, 59]
		 * @param millisecond The millisecond, with range [0, 999]
		 * @return The unix timestamp, -1 if the given date is invalid
		 */
		static double date2timestamp(const unsigned int year, const unsigned int month, const unsigned int day, const unsigned int hour, const unsigned int minute, const unsigned int second, const unsigned int millisecond = 0u);

		/**
		 * Converts a unix timestamp (the seconds since 1970.01.01 00:00:00) to a date.
		 * @param timestamp The unix timestamp to convert
		 * @param year The resulting year, with range [1970, 2037)
		 * @param month The resulting month, with range [1, 12]
		 * @param day The resulting date, with range [1, 31]
		 * @param hour The resulting hour, with range [0, 23]
		 * @param minute The resulting minute, with range [0, 59]
		 * @param second The resulting second, with range [0, 59]
		 * @param millisecond Optional resulting millisecond, with range [0, 999]
		 */
		static void timestamp2date(const double timestamp, unsigned int& year, unsigned int& month, unsigned int& day, unsigned int& hour, unsigned int& minute, unsigned int& second, unsigned int* millisecond = nullptr);

		/**
		 * Converts value of seconds to a time string showing the hours, minutes and seconds.
		 * The resulting string has the following format: HH:MM:SS, or HH:MM:SS:mmmm if the milliseconds are added; or H hours, M minutes, S seconds or H hours, M minutes, S seconds, m milliseconds if the description is added.
		 * @param totalSeconds The number of seconds to convert, with range [0, infinity)
		 * @param addDescription True, to add the description text to the string
		 * @param addMilliseconds True, to add the milliseconds to the string
		 * @return Time string of local time
		 */
		static std::string seconds2string(const double totalSeconds, const bool addDescription, const bool addMilliseconds = false);

		/**
		 * Returns the string of the local time.
		 * The resulting string has the following format: HH:MM:SS, or HH:MM:SS:mmmm if the milliseconds are added.
		 * @param addMilliseconds True, to add the milliseconds to the string
		 * @param delimiter The delimiter to be used
		 * @return Time string of local time
		 */
		static std::string localStringTime(const bool addMilliseconds = false, const char delimiter = ':');

		/**
		 * Returns whether a given year is a leap year.
		 * @param year The year to check
		 */
		static inline unsigned int isLeapYear(const unsigned int year);

		/**
		 * Returns whether a given date is a valid date (e.g., in no leap years the 29th February is an invalid date)
		 * @param year The year of the date, with range [0, infinity)
		 * @param month The month of the date, with range [1, 12]
		 * @param day The day of the date, with range [1, 31]
		 * @return True, if so
		 */
		static bool isDateValid(const unsigned int year, const unsigned int month, const unsigned int day);
};

inline unsigned int DateTime::isLeapYear(const unsigned int year)
{
	return (year % 4u) == 0u && ((year % 100u != 0u) || (year % 400u == 0u));
}

}

#endif // META_OCEAN_BASE_DATE_TIME_H
