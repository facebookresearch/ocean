/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_BATTERY_H
#define META_OCEAN_PLATFORM_ANDROID_BATTERY_H

#include "ocean/platform/android/Android.h"

#include "ocean/base/ScopedObject.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

/**
 * This class implements battery functions.
 * @ingroup platformandroid
 */
class OCEAN_PLATFORM_ANDROID_EXPORT Battery
{
	protected:

		/**
		 * Definition of a scoped object for FILE pointers.
		 */
		using ScopedFILE = ScopedObjectCompileTimeT<FILE*, FILE*, int, pclose, 0, false /*tCheckReturnValue*/>;

	public:

		/**
		 * Returns the current battery capacity status in percent.
		 * @return Current battery capacity status, -1 if invalid
		 */
		static float currentCapacity();

		/**
		 * Returns the current battery temperature in Celsius.
		 * @return Current battery temperature, -1 if invalid
		 */
		static float currentTemperature();

		/**
		 * Returns the current battery current in amps.
		 * @param current The resulting single current reading for the battery, in amps
		 * @return True, if succeeded
		 */
		static bool currentCurrent(double& current);

		/**
		 * Returns the current battery voltage in volts.
		 * @param voltage The resulting single voltage reading for the battery, in volts
		 * @return True, if succeeded
		 */
		static bool currentVoltage(double& voltage);

		/**
		 * Returns whether the battery is charging.
		 * @return state The resulting state whether the battery is charging
		 * @return True, if succeeded
		 */
		static bool isCharging(bool& state);
};

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_BATTERY_H
