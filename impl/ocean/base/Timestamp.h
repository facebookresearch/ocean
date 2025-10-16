/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_TIMESTAMP_H
#define META_OCEAN_BASE_TIMESTAMP_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"

#include <atomic>
#include <cfloat>
#include <limits>

#if !defined(OCEAN_PLATFORM_BUILD_WINDOWS) && defined(CLOCK_BOOTTIME)
	#define OCEAN_BASE_TIMESTAMP_BOOTTIME_AVAILABLE
#endif

#if defined(OCEAN_PLATFORM_BUILD_APPLE) && defined(CLOCK_UPTIME_RAW)
	#define OCEAN_BASE_TIMESTAMP_UPTIMERAW_AVAILABLE
#endif

namespace Ocean
{

// Forward declaration.
class Timestamp;

/**
 * Definition of a vector holding Timestamp objects.
 * @see Timestamp
 * @ingroup base
 */
using Timestamps = std::vector<Timestamp>;

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
		 * This class is a helper class allowing to converter timestamps defined in a specific time domain to unix timestamps.
		 */
		class OCEAN_BASE_EXPORT TimestampConverter
		{
			public:

				/**
				 * Definition of individual time domains.
				 */
				enum TimeDomain : uint32_t
				{
					/// An invalid time domain.
					TD_INVALID = 0u,
					/// The monotonically increasing time domain defined in nanoseconds, not increasing during system sleep.
					TD_MONOTONIC,

#ifdef OCEAN_BASE_TIMESTAMP_BOOTTIME_AVAILABLE
					/// The monotonically increasing time domain defined in nanoseconds, increasing during system sleep, not available on Windows.
					TD_BOOTTIME,
#endif

#ifdef OCEAN_BASE_TIMESTAMP_UPTIMERAW_AVAILABLE
					/// The monotonically increasing time domain defined in nanoseconds, the time the system has been awake since the last time it was restarted.
					TD_UPTIME_RAW,
#endif
				};

				/**
				 * Definition of an invalid value.
				 */
				static constexpr int64_t invalidValue_ = std::numeric_limits<int64_t>::lowest();

			public:

				/**
				 * Creates an invalid converter object.
				 * @see isValid().
				 */
				TimestampConverter() = default;

				/**
				 * Creates a new converter object for a specific time domain.
				 * @param timeDomain The time domain for which the converter will be created
				 * @param necessaryMeasurements The number of measurements necessary to determine the offset between the domain time and the unix time, with range [1, infinity)
				 */
				explicit TimestampConverter(const TimeDomain timeDomain, const size_t necessaryMeasurements = 100);

				/**
				 * Move constructor.
				 * @param converter The converter to be moved
				 */
				inline TimestampConverter(TimestampConverter&& converter);

				/**
				 * Converts a timestamp defined in the converter's time domain to a unix timestamp.
				 * @param domainTimestampNs The timestamp in the converter's time domain, in nanoseconds, with range (-infinity, infinity)
				 * @return The converted unix timestamp
				 */
				Timestamp toUnix(const int64_t domainTimestampNs);

				/**
				 * Converts a timestamp defined in the converter's time domain to a unix timestamp.
				 * @param domainTimestampSeconds The timestamp in the converter's time domain, in seconds, with range (-infinity, infinity)
				 * @return The converted unix timestamp
				 */
				Timestamp toUnix(const double domainTimestampSeconds);

				/**
				 * Returns whether a given domain timestamp is within a specified range of the current domain timestamp.
				 * @param domainTimestampNs The domain timestamp to check, in nanoseconds, with range (-infinity, infinity)
				 * @param maxDistance The maximal distance between the domain timestamp and the current domain timestamp, in seconds, with range [0, infinity)
				 * @param distance Optional resulting distance between the domain timestamp and the current domain timestamp, in seconds, with range (-infinity, infinity)
				 * @return True, if so
				 */
				bool isWithinRange(const int64_t domainTimestampNs, const double maxDistance = 1.0, double* distance = nullptr);

				/**
				 * Returns the time domain of this converter.
				 * @return The converter's time domain
				 */
				inline TimeDomain timeDomain() const;

				/**
				 * Returns the offset between the domain time and the unix time, in nanoseconds.
				 * Unix time = domain time + domainToUnixOffset
				 * @return The offset between the domain time and the unix time, in nanoseconds, with range (-infinity, infinity)
				 */
				int64_t domainToUnixOffset();

				/**
				 * Returns whether this converter has been initialized with a valid time domain.
				 * @return True, if so
				 */
				inline bool isValid() const;

				/**
				 * Returns whether this converter is valid.
				 * @return True, if so
				 */
				inline operator bool() const;

				/**
				 * Move operator.
				 * @param converter The converter to be moved
				 * @return Reference to this object
				 */
				TimestampConverter& operator=(TimestampConverter&& converter);

				/**
				 * Returns the current timestamp in a specified time domain.
				 * @param timeDomain The time domain for which the current timestamp will be returned
				 * @return The current timestamp in the specified time domain, in nanoseconds
				 */
				static int64_t currentTimestampNs(const TimeDomain timeDomain);

#ifndef OCEAN_PLATFORM_BUILD_WINDOWS

				/**
				 * Return the current timestamp in a specified POSIX clock id.
				 * @param posixClockId The POSIX clock id for which the current timestamp will be returned
				 * @return The current timestamp in the specified POSIX clock id, in nanoseconds
				 */
				static int64_t currentTimestampNs(const int posixClockId);

			protected:

				/**
				 * Returns the POSIX clock id associated with a time domain.
				 * @param timeDomain The time domain for which the associated POSIX clock id will be returned
				 * @return The POSIX clock id associated with the specified time domain, -1 if no associated POSIX clock id exists
				 */
				static int posixClockId(const TimeDomain timeDomain);

#endif // OCEAN_PLATFORM_BUILD_WINDOWS

			protected:

				/// The time domain of this converter.
				TimeDomain timeDomain_ = TD_INVALID;

				/// The offset between the domain time and the unix time, in nanoseconds.
				std::atomic_int64_t domainToUnixOffsetNs_ = invalidValue_;

				/// The initial domain timestamp, in nanoseconds.
				int64_t initialDomainNs_ = invalidValue_;

				/// The initial unix timestamp, in nanoseconds.
				int64_t initialUnixNs_ = invalidValue_;

				/// The measured sum of the domain to unix offsets, in nanoseconds.
				int64_t sumDomainToUnixOffsetNs_ = 0;

				/// The number of measurements.
				size_t measurements_ = 0;

				/// The number of necessary measurements before the converter keeps the determined offset fixed.
				size_t necessaryMeasurements_ = 0;

				/// The converter's lock.
				Lock lock_;

#ifndef OCEAN_PLATFORM_BUILD_WINDOWS
				/// The POSIX clock id associated with the time domain.
				int domainPosixClockId_ = -1;
#endif
		};

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
		 * Hash function.
		 * @param timestamp The timestamp for which the hash value will be determined
		 * @return The resulting hash value
		 */
		inline size_t operator()(const Timestamp& timestamp) const;

		/**
		 * Converts seconds to milliseconds.
		 * @param seconds The seconds to convert, with range (-infinity, infinity)
		 * @return The resulting milliseconds
		 */
		static constexpr int64_t seconds2milliseconds(const double seconds);

		/**
		 * Converts seconds to microseconds.
		 * @param seconds The seconds to convert, with range (-infinity, infinity)
		 * @return The resulting microseconds
		 */
		static constexpr int64_t seconds2microseconds(const double seconds);

		/**
		 * Converts seconds to nanoseconds.
		 * @param seconds The seconds to convert, with range (-infinity, infinity)
		 * @return The resulting nanoseconds
		 */
		static constexpr int64_t seconds2nanoseconds(const double seconds);

		/**
		 * Converts milliseconds to seconds.
		 * @param milliseconds The milliseconds to convert, with range (-infinity, infinity)
		 * @return The resulting seconds
		 */
		static constexpr double milliseconds2seconds(const int64_t milliseconds);

		/**
		 * Converts microseconds to seconds.
		 * @param microseconds The microseconds to convert, with range (-infinity, infinity)
		 * @return The resulting seconds
		 */
		static constexpr double microseconds2seconds(const int64_t microseconds);

		/**
		 * Converts nanoseconds to seconds.
		 * @param nanoseconds The nanoseconds to convert, with range (-infinity, infinity)
		 * @return The resulting seconds
		 */
		static constexpr double nanoseconds2seconds(const int64_t nanoseconds);

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

inline Timestamp::TimestampConverter::TimestampConverter(TimestampConverter&& converter)
{
	*this = std::move(converter);
}

inline Timestamp::TimestampConverter::TimeDomain Timestamp::TimestampConverter::timeDomain() const
{
	return timeDomain_;
}

inline bool Timestamp::TimestampConverter::isValid() const
{
	return timeDomain_ != TD_INVALID && necessaryMeasurements_ != 0;
}

inline Timestamp::TimestampConverter::operator bool() const
{
	return isValid();
}

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

inline size_t Timestamp::operator()(const Timestamp& timestamp) const
{
	return std::hash<double>{}(double(timestamp));
}

constexpr int64_t Timestamp::seconds2milliseconds(const double seconds)
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

constexpr int64_t Timestamp::seconds2microseconds(const double seconds)
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

constexpr int64_t Timestamp::seconds2nanoseconds(const double seconds)
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

constexpr double Timestamp::milliseconds2seconds(const int64_t milliseconds)
{
	return double(milliseconds) / 1.0e3;
}

constexpr double Timestamp::microseconds2seconds(const int64_t microseconds)
{
	return double(microseconds) / 1.0e6;
}

constexpr double Timestamp::nanoseconds2seconds(const int64_t nanoseconds)
{
	return double(nanoseconds) / 1.0e9;
}

constexpr double Timestamp::invalidTimestampValue()
{
	return -DBL_MAX;
}

}

#endif // META_OCEAN_BASE_TIMESTAMP_H
