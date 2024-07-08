/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/Battery.h"

#include "ocean/platform/linux/Utilities.h"

#include <array>
#include <cstdlib>
#include <iostream>

namespace Ocean
{

namespace Platform
{

namespace Android
{

float Battery::currentCapacity()
{

#ifdef OCEAN_SUPPORT_EXCEPTIONS
	try
	{
#endif // OCEAN_SUPPORT_EXCEPTIONS

		std::ifstream stream;
		stream.open("/sys/class/power_supply/battery/capacity", std::ios::binary);

		if (stream.fail())
		{
#ifdef OCEAN_DEBUG
			if (!Platform::Linux::Utilities::checkSecurityEnhancedLinuxStateIsPermissive())
			{
				static bool once = true;

				if (once)
				{
					once = false;
					Log::debug() << "Current battery capacity could not be checked as the security enhanced Linux state is not permissive.";
				}
			}
#endif
			return -1.0f;
		}

		float value;
		stream >> value;

		if (stream.fail())
		{
			return -1.0f;
		}

		return value;

#ifdef OCEAN_SUPPORT_EXCEPTIONS
	}
	catch(...)
	{
		// nothing to do here
	}

	return -1.0f;
#endif // OCEAN_SUPPORT_EXCEPTIONS

}

float Battery::currentTemperature()
{

#ifdef OCEAN_SUPPORT_EXCEPTIONS
	try
	{
#endif // OCEAN_SUPPORT_EXCEPTIONS

		{
			std::ifstream stream;
			stream.open("/sys/class/power_supply/battery/batt_temp", std::ios::binary);

			if (!stream.fail())
			{
				float value;
				stream >> value;

				if (!stream.fail())
				{
					return value * 0.1f;
				}
			}
		}

		{
			std::ifstream stream;
			stream.open("/sys/class/power_supply/battery/temp", std::ios::binary);

			if (!stream.fail())
			{
				float value;
				stream >> value;

				if (!stream.fail())
				{
					return value * 0.1f;
				}
			}
		}

#ifdef OCEAN_DEBUG
		if (!Platform::Linux::Utilities::checkSecurityEnhancedLinuxStateIsPermissive())
		{
			static bool once = true;

			if (once)
			{
				once = false;
				Log::debug() << "Current battery temperature could not be checked as the security enhanced Linux state is not permissive.";
			}
		}
#endif

#ifdef OCEAN_SUPPORT_EXCEPTIONS
	}
	catch(...)
	{
		// nothing to do here
	}

#endif // OCEAN_SUPPORT_EXCEPTIONS
	return -1.0f;
}

bool Battery::currentCurrent(double& current)
{
#ifdef OCEAN_SUPPORT_EXCEPTIONS
	try
	{
#endif // OCEAN_SUPPORT_EXCEPTIONS

		/*
		 * current_now
		 * Reports an instant, single IBAT current reading for the battery. This value is not averaged/smoothed.
		 * Represented in microamps
		 */

		std::ifstream stream;
		stream.open("/sys/class/power_supply/battery/current_now", std::ios::binary);

		if (stream.fail())
		{
#ifdef OCEAN_DEBUG
			if (!Platform::Linux::Utilities::checkSecurityEnhancedLinuxStateIsPermissive())
			{
				static bool once = true;

				if (once)
				{
					once = false;
					Log::debug() << "Current battery current could not be read as the security enhanced Linux state is not permissive.";
				}
			}
#endif
			return false;
		}

		double microamps;
		stream >> microamps;

		if (stream.fail())
		{
			return false;
		}

		current = microamps * 1.0e-6;

		return true;

#ifdef OCEAN_SUPPORT_EXCEPTIONS
	}
	catch(...)
	{
		// nothing to do here
	}

	return false;
#endif // OCEAN_SUPPORT_EXCEPTIONS
}

bool Battery::currentVoltage(double& voltage)
{
#ifdef OCEAN_SUPPORT_EXCEPTIONS
	try
	{
#endif // OCEAN_SUPPORT_EXCEPTIONS

		/*
		 * voltage_now
		 * Reports an instant, single VBAT voltage reading for the battery. This value is not averaged/smoothed.
		 * Represented in microvolts
		 */

		std::ifstream stream;
		stream.open("/sys/class/power_supply/battery/voltage_now", std::ios::binary);

		if (stream.fail())
		{
#ifdef OCEAN_DEBUG
			if (!Platform::Linux::Utilities::checkSecurityEnhancedLinuxStateIsPermissive())
			{
				static bool once = true;

				if (once)
				{
					once = false;
					Log::debug() << "Current battery current could not be read as the security enhanced Linux state is not permissive.";
				}
			}
#endif
			return false;
		}

		double microvolts;
		stream >> microvolts;

		if (stream.fail())
		{
			return false;
		}

		voltage = microvolts * 1.0e-6;

		return true;

#ifdef OCEAN_SUPPORT_EXCEPTIONS
	}
	catch(...)
	{
		// nothing to do here
	}

	return false;
#endif // OCEAN_SUPPORT_EXCEPTIONS
}

bool Battery::isCharging(bool& state)
{
	ScopedFILE file(popen("dumpsys battery", "r"));

	if (!file.isValid())
	{
		Log::error() << "Failed to execute 'dumpsys battery' command";
		return false;
	}

	std::string result;
	result.reserve(128);

	std::array<char, 64> buffer;
	while (fgets(buffer.data(), buffer.size() - 1, *file) != nullptr)
	{
		result += buffer.data();

		if (result.size() > 1024 * 1024)
		{
			return false;
		}
	}

	if (result.empty())
	{
		return false;
	}

	for (const char* item : {"AC powered: true", "USB powered: true", "Wireless powered: true"})
	{
		if (result.find(item) != std::string::npos)
		{
			state = true;
			return true;
		}
	}

	for (const char* item : {"AC powered: false", "USB powered: false", "Wireless powered: false"})
	{
		if (result.find(item) == std::string::npos)
		{
			return false;
		}
	}

	state = false;
	return true;
}

}

}

}
