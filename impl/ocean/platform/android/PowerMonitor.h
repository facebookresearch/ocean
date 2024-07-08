/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_POWER_MONITOR_H
#define META_OCEAN_PLATFORM_ANDROID_POWER_MONITOR_H

#include "ocean/platform/android/Android.h"

#include "ocean/base/ScopedSubscription.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Timestamp.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

class OCEAN_PLATFORM_ANDROID_EXPORT PowerMonitor final : protected Thread
{
	public:

		/**
		 * Definition of a scoped subscription object.
		 */
		using ScopedSubscription = ScopedSubscriptionT<bool, PowerMonitor>;

	protected:

		/**
		 * This class holds the relevant information of one measurement.
		 */
		class Measurement
		{
			public:

				/**
				 * Creates a new measurement object.
				 * @param timestamp The timestamp at which the measurement was done, must be valid
				 * @param current The current value, with range (-infinity, infinity)
				 * @param voltage The voltage value, with range [0, infinity)
				 */
				inline Measurement(const Timestamp& timestamp, const double current, const double voltage);

			public:

				/// The measurement's timestamp.
				Timestamp timestamp_;

				/// The measurement's current value.
				double current_;

				/// The measurement's voltage value.
				double voltage_;
		};

		/**
		 * Definition of a vector holding measurement objects.
		 */
		using Measurements = std::vector<Measurement>;

	public:

		/**
		 * Default constructor.
		 * @param updateFrequency The update frequency to be used, in Hz, with range (0, infinity)
		 */
		explicit PowerMonitor(const double updateFrequency);

		/**
		 * Destructs this monitor.
		 */
		~PowerMonitor() override;

		/**
		 * Returns whether this monitor is valid and ready to be used.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns the update frequency the monitor uses.
		 * @return The monitor's update frequency, in Hz
		 */
		double updateFrequency() const;

		/**
		 * Resets the monitor.
		 * Stop all measurements before calling this function.
		 * @return True, if succeeded
		 */
		bool reset();

		/**
		 * Starts a new measurement.
		 * @return The resulting subscription object, the measurement is active as long as the object exists
		 */
		[[nodiscard]] ScopedSubscription startMeasurement();

		/**
		 * Returns the average power consumption.
		 * @param power The resulting averaged power consumption
		 * @param standardDeviation Optional resulting standard deviation of the average power, nullptr if not of interest
		 * @param numberMeasurements Optional resulting number of measurements which have been used to determine the average power, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool averagePower(double& power, double* standardDeviation = nullptr, size_t* numberMeasurements = nullptr) const;

		/**
		 * Returns the median power consumption.
		 * @param power The resulting median power consumption
		 * @param numberMeasurements Optional resulting number of measurements which have been used to determine the average power, nullptr if not of interest
		 * @return True, if succeeded
		 */
		bool medianPower(double& power, size_t* numberMeasurements = nullptr) const;

		/**
		 * Determines the ideal update frequency the monitor should use.
		 * @param testDuration The test duration, in seconds, with range (0, infinity)
		 * @return The resulting update frequency, in Hz, -1 in case of an error
		 */
		static double determineIdealUpdateFrequency(const double testDuration = 5.0);

	protected:

		/**
		 * Stops a measurement.
		 */
		void stopMeasurement(const bool&);

		/**
		 * The thread run function.
		 */
		void threadRun() override;

		/**
		 * Determines the ideal update frequency the monitor should use for a specific monitor value.
		 * @param updateFrequency The resulting update frequency, in Hz
		 * @param testDuration The test duration, in seconds, with range (0, infinity)
		 * @return True, if succeeded
		 * @tparam tForCurrent True, to determine the update frequency for the current value; False, to determine the update frequency for the voltage value
		 */
		template <bool tForCurrent>
		static bool determineIdealUpdateFrequency(double& updateFrequency, const double testDuration = 5.0);

		/**
		 * Determines the ideal update frequency from a given set of timestamps.
		 * @param timestamps The timestamps for which the update frequency will be determined
		 * @param updateFrequency The resulting update frequency, in Hz
		 * @return True, if succeeded
		 */
		static bool determineIdealUpdateFrequency(const Timestamps& timestamps, double& updateFrequency);

	protected:

		/// The update frequency to be used, in Hz.
		double updateFrequency_ = -1.0;

		/// True, if the monitor is active.
		bool isActive_ = false;

		/// The measurements.
		Measurements measurements_;

		/// The monitor's lock.
		mutable Lock lock_;
};

inline PowerMonitor::Measurement::Measurement(const Timestamp& timestamp, const double current, const double voltage) :
	timestamp_(timestamp),
	current_(current),
	voltage_(voltage)
{
	// nothing to do here
}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_POWER_MONITOR_H
