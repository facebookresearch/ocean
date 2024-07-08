/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_PROCESSOR_MONITOR_H
#define META_OCEAN_PLATFORM_ANDROID_PROCESSOR_MONITOR_H

#include "ocean/platform/android/Android.h"
#include "ocean/platform/android/Processor.h"

#include "ocean/base/Singleton.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Thread.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

/**
 * This class implements a processor monitor.
 * @ingroup platformandroid
 */
class OCEAN_PLATFORM_ANDROID_EXPORT ProcessorMonitor :
	public Singleton<ProcessorMonitor>,
	protected Thread
{
	friend class Singleton<ProcessorMonitor>;

	public:

		/**
		 * Definition of a vector holding processor operation frequencies.
		 */
		typedef std::vector<Processor::Frequency> Frequencies;

		/**
		 * Definition of a pair combining a timestamp with frequencies.
		 */
		typedef std::pair<Timestamp, Frequencies> FrequencyPair;

		/**
		 * Definition of a vector holding frequency pairs.
		 */
		typedef std::vector<FrequencyPair> FrequencyPairs;

		/**
		 * Definition of an ordered map mapping timestamps to frequencies.
		 */
		typedef std::map<Timestamp, Frequencies> FrequencyMap;

	public:

		/**
		 * Returns the number of CPU cores this monitor controls.
		 * @return Number of CPU cores
		 */
		inline unsigned int installedCores() const;

		/**
		 * Returns the update rate of this monitor in seconds.
		 * @return Update rate in seconds, 0.01 by default
		 */
		inline double updateRate() const;

		/**
		 * Sets the update rate of this monitor in seconds.
		 * @param rate Update rate in seconds to be set, with range (0, infinity)
		 */
		inline void setUpdateRate(const double rate);

		/**
		 * Returns the gathered monitor values between two timestamp.
		 * @param beginTimestamp Begin timestamp, -1 to return entire information database
		 * @param endTimestamp End timestamp
		 * @return Gathered information
		 */
		FrequencyPairs monitorData(const Timestamp beginTimestamp, const Timestamp endTimestamp);

		/**
		 * Clears the monitor information.
		 */
		void clear();

	private:

		/**
		 * Creates a new processor monitor object.
		 */
		ProcessorMonitor();

		/**
		 * Destructs a processor monitor object.
		 */
		~ProcessorMonitor() override;

		/**
		 * Either this function has to be overloaded in derived classes or the thread run callback functions must be set.
		 * @see Thread::threadRun().
		 */
		void threadRun() override;

	private:

		/// Monitor CPU core number.
		unsigned int installedCores_ = 0u;

		/// Monitor update rate in seconds.
		double updateRate_ = 0.01;

		/// Previous information timestamp.
		Timestamp previousTimestamp_;

		/// Monitor frequency map.
		FrequencyMap frequencyMap_;

		/// Monitor lock object.
		Lock lock_;
};

/**
 * This class implements a processor statistic object.
 * @ingroup platformandroid
 */
class OCEAN_PLATFORM_ANDROID_EXPORT ProcessorStatistic
{
	public:

		/**
		 * Creates a new statistic object.
		 */
		ProcessorStatistic() = default;

		/**
		 * Starts the statistic.
		 */
		void start();

		/**
		 * Stops the statistic.
		 */
		void stop();

		/**
		 * Resets the statistic.
		 */
		void reset();

		/**
		 * Returns the measurement duration.
		 * @return Measurement duration
		 */
		inline double duration() const;

		/**
		 * Returns the number of measurements.
		 * @return Measurement number
		 */
		inline unsigned int measurements() const;

		/**
		 * Returns the average number of active CPU cores.
		 * @return Average active CPU cores
		 */
		inline double averageActiveCores() const;

		/**
		 * Returns the average (of all active cores) CPU frequencies.
		 * @return Average CPU frequency
		 */
		inline double averageFrequency() const;

		/**
		 * Returns the ratio between average CPU frequency and maximal possible CPU frequency.
		 * @return Average CPU performance factor.
		 */
		inline double averagePerformanceRate() const;

		/**
		 * Returns the minimal (of all active cores) CPU frequency.
		 * @return Minimal CPU frequency
		 */
		inline double minimalFrequency() const;

		/**
		 * Returns the maximal (of all active cores) CPU frequency.
		 * @return Maximal CPU frequency
		 */
		inline double maximalFrequency() const;

	private:

		/// Start timestamp of the statistic.
		Timestamp startTimestamp_;

		/// Measurement duration.
		double duration_ = 0.0;

		/// Average active CPU cores.
		double averageActiveCores_ = -1.0;

		/// Average total CPU frequency.
		double averageFrequency_ = -1.0;

		/// Minimal CPU frequency.
		double minimalFrequency_ = -1.0;

		/// Maximal CPU frequency.
		double maximalFrequency_ = -1.0;

		/// Measurement number.
		unsigned int measurements_ = 0u;

		/// Pairs of timestamps and frequencies.
		ProcessorMonitor::FrequencyPairs frequencyPairs_;
};

inline unsigned int ProcessorMonitor::installedCores() const
{
	ocean_assert(installedCores_ > 0u);
	return installedCores_;
}

inline double ProcessorMonitor::updateRate() const
{
	return updateRate_;
}

inline void ProcessorMonitor::setUpdateRate(const double rate)
{
	ocean_assert(rate > 0.0);
	updateRate_ = rate;
}

inline double ProcessorStatistic::duration() const
{
	return duration_;
}

inline unsigned int ProcessorStatistic::measurements() const
{
	return measurements_;
}

inline double ProcessorStatistic::averageActiveCores() const
{
	return averageActiveCores_;
}

inline double ProcessorStatistic::averageFrequency() const
{
	return averageFrequency_;
}

inline double ProcessorStatistic::averagePerformanceRate() const
{
	ocean_assert(maximalFrequency_ > 0.0);
	if (maximalFrequency_ <= 0.0)
	{
		return -1.0;
	}

	const double maxPossibleFrequency = Processor::maxFrequency();
	if (maxPossibleFrequency <= 0.0)
	{
		return -1;
	}

	ocean_assert(maxPossibleFrequency >= averageFrequency_);

	return averageFrequency_ / maxPossibleFrequency;
}

inline double ProcessorStatistic::minimalFrequency() const
{
	return minimalFrequency_;
}

inline double ProcessorStatistic::maximalFrequency() const
{
	return maximalFrequency_;
}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_PROCESSOR_MONITOR_H
