/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/devices/android/AndroidEventDevice.h"
#include "ocean/devices/android/AndroidFactory.h"

#include "ocean/platform/android/Utilities.h"

namespace Ocean
{

namespace Devices
{

namespace Android
{

AndroidEventDevice::LooperManager::LooperManager() :
	looper_(nullptr)
{
	startThread();
}

ALooper* AndroidEventDevice::LooperManager::looper()
{
	while (looper_ == nullptr)
	{
		sleep(1u);
	}

	return looper_;
}

void AndroidEventDevice::LooperManager::threadRun()
{
	ocean_assert(looper_ == nullptr);
	looper_ = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
	ocean_assert(looper_ != nullptr);

	while (shouldThreadStop() == false)
	{
		ALooper_pollOnce(1, nullptr, nullptr, nullptr);
	}
}

AndroidEventDevice::AndroidEventDevice(const std::string& name, const DeviceType type) :
	Device(name, type),
	AndroidDevice(name, type),
	Measurement(name, type),
	timestampConverter_(timestampConverter())
{
	static_assert(int(ASENSOR_TYPE_ACCELEROMETER) == int(AST_ACCELEROMETER), "Invalid sensor type!");
	static_assert(int(ASENSOR_TYPE_MAGNETIC_FIELD) == int(AST_MAGNETIC_FIELD), "Invalid sensor type!");
	static_assert(int(ASENSOR_TYPE_GYROSCOPE) == int(AST_GYROSCOPE), "Invalid sensor type!");
	static_assert(int(ASENSOR_TYPE_LIGHT) == int(AST_LIGHT), "Invalid sensor type!");
	static_assert(int(ASENSOR_TYPE_PROXIMITY) == int(AST_PROXIMITY), "Invalid sensor type!");

#if defined(__ANDROID_API__) && __ANDROID_API__ >= 24
	static_assert(int(ASENSOR_TYPE_GYROSCOPE_UNCALIBRATED) == int(AST_GYROSCOPE_UNCALIBRATED), "Invalid sensor type!");
#endif

	sensorObjectId_ = addUniqueObjectId(name);

	sensorManager_ = sensorManager();
	ocean_assert(sensorManager_);
}

bool AndroidEventDevice::start()
{
	const ScopedLock scopedLock(deviceLock);

	if (isStarted_)
	{
		return true;
	}

	if (sensor_ == nullptr || eventQueue_ == nullptr)
	{
		return false;
	}

	const int result = ASensorEventQueue_enableSensor(eventQueue_, sensor_);

	// we want the highest update frequency possible
	const int eventRateResult = ASensorEventQueue_setEventRate(eventQueue_, sensor_, ASensor_getMinDelay(sensor_));
	ocean_assert_and_suppress_unused(eventRateResult == 0, eventRateResult);

	isStarted_ = result >= 0;

	return isStarted_;
}

bool AndroidEventDevice::pause()
{
	return stop();
}

bool AndroidEventDevice::stop()
{
	const ScopedLock scopedLock(deviceLock);

	if (!isStarted_)
	{
		return true;
	}

	if (sensor_ == nullptr || eventQueue_ == nullptr)
	{
		return true;
	}

	const int result = ASensorEventQueue_disableSensor(eventQueue_, sensor_);
	ocean_assert_and_suppress_unused(result >= 0, result);

	isStarted_ = false;
	return true;
}

ASensorManager* AndroidEventDevice::sensorManager()
{
#if defined(__ANDROID_API__) && __ANDROID_API__ >= 26

	static Lock lock;

	const ScopedLock scopedLock(lock);

	static std::string packageName;

	if (packageName.empty())
	{
		if (!Platform::Android::Utilities::determinePackageName(packageName))
		{
			Log::error() << "AndroidEventDevice: Failed to determine package name, using backup package name";

			packageName = "com.meta.ocean";
		}
	}

	ocean_assert(!packageName.empty());
	ASensorManager* sensorManager = ASensorManager_getInstanceForPackage(packageName.c_str());

	if (sensorManager == nullptr)
	{
		Log::error() << "AndroidEventDevice: Failed to access sensor manager";
		ocean_assert(false && "Failed to access sensor manager");
	}

	return sensorManager;

#else

	return ASensorManager_getInstance();

#endif
}

bool AndroidEventDevice::registerForEventFunction(ASensorManager* sensorManager)
{
	ocean_assert(sensorManager);

	ocean_assert(eventQueue_ == nullptr);
	eventQueue_ = ASensorManager_createEventQueue(sensorManager, LooperManager::get().looper(), ALOOPER_POLL_CALLBACK, onEventFunctionStatic, dynamic_cast<AndroidDevice*>(this));
	ocean_assert(eventQueue_);

	return eventQueue_ != nullptr;
}

Timestamp AndroidEventDevice::convertTimestamp(const ASensorEvent& sensorEvent, Timestamp& relativeTimestamp)
{

#ifdef OCEAN_DEBUG
	double debugDistance;
	if (!timestampConverter_.isWithinRange(sensorEvent.timestamp, 0.01, &debugDistance))
	{
		Log::debug() << "AndroidEventDevice: Timestamp is not within range of 10ms, actual distance: " << debugDistance * 1000.0 << "ms";

		Log::debug() << "Raw sensor timestamp: " << sensorEvent.timestamp;
		Log::debug() << "Current domain timestamp: " << timestampConverter_.currentDomainTimestampNs();

		Log::debug() << "Monotonic: " << TimestampConverter::currentDomainTimestampNs(TimestampConverter::TD_MONOTONIC);

#ifdef OCEAN_BASE_TIMESTAMP_BOOTTIME_AVAILABLE
		Log::debug() << "Boot time: " << TimestampConverter::currentDomainTimestampNs(TimestampConverter::TD_BOOTTIME);
#endif // OCEAN_BASE_TIMESTAMP_BOOTTIME_AVAILABLE

#ifdef OCEAN_BASE_TIMESTAMP_UPTIMERAW_AVAILABLE
		Log::debug() << "Up time raw: " << TimestampConverter::currentDomainTimestampNs(TimestampConverter::TD_UPTIME_RAW);
#endif // OCEAN_BASE_TIMESTAMP_UPTIMERAW_AVAILABLE

#ifdef OCEAN_BASE_TIMESTAMP_VIRTUAL_COUNTER_REGISTER_AVAILABLE
		Log::debug() << "Virtual counter register: " << TimestampConverter::currentDomainTimestampNs(TimestampConverter::TD_VIRTUAL_COUNTER_REGISTER);
#endif // OCEAN_BASE_TIMESTAMP_VIRTUAL_COUNTER_REGISTER_AVAILABLE

		Log::debug() << " ";
	}
	else
	{
#ifdef OCEAN_INTENSIVE_DEBUG
		Log::debug() << "Sensor timestamp distance: " << sensorEvent.type << ": " << debugDistance * 1000.0 << "ms";
#endif
	}
#endif // OCEAN_DEBUG

	relativeTimestamp = Timestamp(Timestamp::nanoseconds2seconds(sensorEvent.timestamp));

	return timestampConverter_.toUnix(sensorEvent.timestamp);
}

TimestampConverter& AndroidEventDevice::timestampConverter()
{
	return AndroidFactory::timestampConverter();
}

}

}

}
