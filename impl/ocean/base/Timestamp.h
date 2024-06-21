/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_TIMESTAMP_H
#define META_OCEAN_BASE_TIMESTAMP_H

#include "ocean/base/Base.h"

#include <cfloat>

namespace Ocean
{

// Forward declaration.
class Timestamp;

/**
 * Definition of a vector holding Timestamp objects.
 * @see Timestamp
 * @ingroup base
 */
typedef std::vector<Timestamp> Timestamps;

/**
 * This class implements a timestamp.
 * The timestamp is specified by the number of seconds since 01.01.1970 00:00:00 in UTC time.<br>
 * Depending on the underlying hardware the accuracy (update rate) of the timestamps can vary.<br>
 * The timestamp class wraps a floating value with 64 bit precision.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Timestamp
{
	public:

		/**
		 * Creates a new timestamp with invalid time.
		 */
		Timestamp() = default;

		/**
		 * Creates a new timestamp.
		 * @param toNow Determines whether the timestamp holds the seconds since 01.01.1970 00:00:00 in UTC time, otherwise the timestamp will be initialized as invalid
		 */
		explicit Timestamp(const bool toNow);

		/**
		 * Creates a new timestamp with a given value.
		 * @param timestamp Timestamp value
		 */
		inline explicit Timestamp(const double timestamp);

		/**
		 * Sets the timestamp to the current time.
		 * The timestamp holds the seconds since 01.01.1970 00:00:00 in UTC time.
		 * @return Reference to this object
		 */
		Timestamp& toNow();

		/**
		 * Sets the timestamp to invalid.
		 * @return Reference to this object
		 */
		inline Timestamp& toInvalid();

		/**
		 * Returns this timestamp in nanoseconds.
		 * @return The timestamp in nanoseconds, with range (-infinity, infinity)
		 */
		inline int64_t nanoseconds() const;

		/**
		 * Returns whether a specified amount of time has passed since this timestamp.
		 * This function returns 'thisTimestamp + seconds <= currentTimestamp'.
		 * @param seconds The number of seconds defining the time to check, with range [0, infinity)
		 * @param currentTimestamp The current timestamp to use for comparison
		 * @return True, if the time has passed or if this timestamp is invalid
		 */
		inline bool hasTimePassed(const double seconds, const Timestamp& currentTimestamp = Timestamp(true)) const;

		/**
		 * Returns whether the timestamp holds a valid time.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether the timestamp holds an invalid time.
		 * @return True, if so
		 */
		inline bool isInvalid() const;

		/**
		 * Assign a new value.
		 * @param timestamp Value to assign
		 * @return Reference to this timestamp
		 */
		inline Timestamp& operator=(const double timestamp);

		/**
		 * Adds two timestamps.
		 * @param right The right timestamp
		 * @return New timestamp
		 */
		inline Timestamp operator+(const Timestamp& right) const;

		/**
		 * Adds and assigns two timestamps.
		 * @param right The right timestamp
		 * @return Reference to this timestamp
		 */
		inline Timestamp& operator+=(const Timestamp& right);

		/**
		 * Adds seconds to this timestamps.
		 * @param seconds The number of seconds to add, with range (-infinity, infinity)
		 * @return New timestamp
		 */
		inline Timestamp operator+(const double seconds) const;

		/**
		 * Adds and assigns seconds to this timestamps.
		 * @param seconds The number of seconds to add, with range (-infinity, infinity)
		 * @return Reference to this timestamp
		 */
		inline Timestamp& operator+=(const double seconds);

		/**
		 * Subtracts two timestamps.
		 * @param right The right timestamp
		 * @return New timestamp
		 */
		inline Timestamp operator-(const Timestamp& right) const;

		/**
		 * Subtracts and assigns two timestamps.
		 * @param right The right timestamp
		 * @return Reference to this timestamp
		 */
		inline Timestamp& operator-=(const Timestamp& right);

		/**
		 * Subtracts seconds from this timestamp.
		 * @param seconds The number of seconds to subtract, with range (-infinity, infinity)
		 * @return New timestamp
		 */
		inline Timestamp operator-(const double seconds) const;

		/**
		 * Subtracts and assigns seconds from this timestamp.
		 * @param seconds The number of seconds to subtract, with range (-infinity, infinity)
		 * @return Reference to this timestamp
		 */
		inline Timestamp& operator-=(const double seconds);

		/**
		 * Returns whether the this timestamp is lesser than the right one.
		 * @param right The right timestamp
		 * @return True, if so
		 */
		inline bool operator<(const Timestamp& right) const;

		/**
		 * Returns whether the this timestamp is lesser or equal to the right one.
		 * @param right The right timestamp
		 * @return True, if so
		 */
		inline bool operator<=(const Timestamp& right) const;

		/**
		 * Returns whether the this timestamp is greater than the right one.
		 * @param right The right timestamp
		 * @return True, if so
		 */
		inline bool operator>(const Timestamp& right) const;

		/**
		 * Returns whether the this timestamp is greater or equal to the right one.
		 * @param right The right timestamp
		 * @return True, if so
		 */
		inline bool operator>=(const Timestamp& right) const;

		/**
		 * Returns whether two timestamps are identical.
		 * @param right The right timestamp
		 * @return True, if so
		 */
		inline bool operator==(const Timestamp& right) const;

		/**
		 * Returns whether two timestamps are not identical.
		 * @param right The right timestamp
		 * @return True, if so
		 */
		inline bool operator!=(const Timestamp& right) const;

		/**
		 * Cast operator for the timestamp value.
		 * @return Timestamp
		 */
		explicit inline operator double() const;

		/**
		 * Converts seconds to milliseconds.
		 * @param seconds The seconds to convert, with range (-infinity, infinity)
		 * @return The resulting milliseconds
		 */
		static inline int64_t seconds2milliseconds(const double seconds);

		/**
		 * Converts seconds to microseconds.
		 * @param seconds The seconds to convert, with range (-infinity, infinity)
		 * @return The resulting microseconds
		 */
		static inline int64_t seconds2microseconds(const double seconds);

		/**
		 * Converts seconds to nanoseconds.
		 * @param seconds The seconds to convert, with range (-infinity, infinity)
		 * @return The resulting nanoseconds
		 */
		static inline int64_t seconds2nanoseconds(const double seconds);

		/**
		 * Converts milliseconds to seconds.
		 * @param milliseconds The milliseconds to convert, with range (-infinity, infinity)
		 * @return The resulting seconds
		 */
		static inline double milliseconds2seconds(const int64_t milliseconds);

		/**
		 * Converts microseconds to seconds.
		 * @param microseconds The microseconds to convert, with range (-infinity, infinity)
		 * @return The resulting seconds
		 */
		static inline double microseconds2seconds(const int64_t microseconds);

		/**
		 * Converts nanoseconds to seconds.
		 * @param nanoseconds The nanoseconds to convert, with range (-infinity, infinity)
		 * @return The resulting seconds
		 */
		static inline double nanoseconds2seconds(const int64_t nanoseconds);

	protected:

		/**
		 * Returns the of an invalid timestamp.
		 * @return Invalid timestamp value
		 */
		static constexpr double invalidTimestampValue();

	protected:

		/// Timestamp value.
		double value_ = invalidTimestampValue();
};

inline Timestamp::Timestamp(const double timestamp) :
	value_(timestamp)
{
	// nothing to do here
}

inline Timestamp& Timestamp::toInvalid()
{
	value_ = invalidTimestampValue();

	return *this;
}

inline int64_t Timestamp::nanoseconds() const
{
	ocean_assert(isValid());

	return seconds2nanoseconds(value_);
}

inline bool Timestamp::hasTimePassed(const double seconds, const Timestamp& currentTimestamp) const
{
	ocean_assert(seconds >= 0.0);
	ocean_assert(currentTimestamp.isValid());

	if (!isValid())
	{
		return true;
	}

	return double(*this) + seconds <= double(currentTimestamp);
}

inline bool Timestamp::isValid() const
{
	return value_ != invalidTimestampValue();
}

inline bool Timestamp::isInvalid() const
{
	return value_ == invalidTimestampValue();
}

inline Timestamp& Timestamp::operator=(const double timestamp)
{
	value_ = timestamp;

	return *this;
}

inline Timestamp Timestamp::operator+(const Timestamp& right) const
{
	return Timestamp(value_ + right.value_);
}

inline Timestamp& Timestamp::operator+=(const Timestamp& right)
{
	value_ += right.value_;

	return *this;
}

inline Timestamp Timestamp::operator+(const double seconds) const
{
	return Timestamp(value_ + seconds);
}

inline Timestamp& Timestamp::operator+=(const double seconds)
{
	value_ += seconds;

	return *this;
}

inline Timestamp Timestamp::operator-(const Timestamp& right) const
{
	return Timestamp(value_ - right.value_);
}

inline Timestamp& Timestamp::operator-=(const Timestamp& right)
{
	value_ -= right.value_;

	return *this;
}

inline Timestamp Timestamp::operator-(const double seconds) const
{
	return Timestamp(value_ - seconds);
}

inline Timestamp& Timestamp::operator-=(const double seconds)
{
	value_ -= seconds;

	return *this;
}

inline bool Timestamp::operator<(const Timestamp& right) const
{
	return value_ < right.value_;
}

inline bool Timestamp::operator<=(const Timestamp& right) const
{
	return value_ <= right.value_;
}

inline bool Timestamp::operator>(const Timestamp& right) const
{
	return value_ > right.value_;
}

inline bool Timestamp::operator>=(const Timestamp& right) const
{
	return value_ >= right.value_;
}

inline bool Timestamp::operator==(const Timestamp& right) const
{
	return value_ == right.value_;
}

inline bool Timestamp::operator!=(const Timestamp& right) const
{
	return value_ != right.value_;
}

inline Timestamp::operator double() const
{
	return value_;
}

inline int64_t Timestamp::seconds2milliseconds(const double seconds)
{
	// 1000 milliseconds  =  1 second

	if (seconds >= 0.0)
	{
		return int64_t(seconds * 1.0e3 + 0.5);
	}
	else
	{
		return int64_t(seconds * 1.0e3 - 0.5);
	}
}

inline int64_t Timestamp::seconds2microseconds(const double seconds)
{
	// 1000 milliseconds  =  1 second
	// 1000 microseconds  =  1 milliseconds

	if (seconds >= 0.0)
	{
		return int64_t(seconds * 1.0e6 + 0.5);
	}
	else
	{
		return int64_t(seconds * 1.0e6 - 0.5);
	}
}

inline int64_t Timestamp::seconds2nanoseconds(const double seconds)
{
	// 1000 milliseconds  =  1 second
	// 1000 microseconds  =  1 milliseconds
	// 1000 nanoseconds   =  1 microseconds

	if (seconds >= 0.0)
	{
		return int64_t(seconds * 1.0e9 + 0.5);
	}
	else
	{
		return int64_t(seconds * 1.0e9 - 0.5);
	}
}

inline double Timestamp::milliseconds2seconds(const int64_t milliseconds)
{
	return double(milliseconds) / 1.0e3;
}

inline double Timestamp::microseconds2seconds(const int64_t microseconds)
{
	return double(microseconds) / 1.0e6;
}

inline double Timestamp::nanoseconds2seconds(const int64_t nanoseconds)
{
	return double(nanoseconds) / 1.0e9;
}

constexpr double Timestamp::invalidTimestampValue()
{
	return -DBL_MAX;
}

}

#endif // META_OCEAN_BASE_TIMESTAMP_H
