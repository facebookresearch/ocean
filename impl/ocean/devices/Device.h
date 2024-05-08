/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_DEVICE_H
#define META_OCEAN_DEVICES_DEVICE_H

#include "ocean/devices/Devices.h"

#include "ocean/base/ObjectRef.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/Value.h"

namespace Ocean
{

namespace Devices
{

/**
 * This class is the base class for all devices of any type.
 * @ingroup devices
 */
class OCEAN_DEVICES_EXPORT Device
{
	friend class Factory;
	friend class Ocean::ObjectRef<Device>;

	public:

		/**
		 * Definition of all major device types.
		 */
		enum MajorType : uint32_t
		{
			/// Invalid major type.
			DEVICE_INVALID = 0u,
			/// Measurement device.
			DEVICE_MEASUREMENT = (1u << 0u),
			/// Sensor device.
			DEVICE_SENSOR = (1u << 1u) | DEVICE_MEASUREMENT,
			/// Tracker device.
			DEVICE_TRACKER = (1u << 2u) | DEVICE_MEASUREMENT
		};

		/**
		 * Base definition of a minor type.
		 */
		enum MinorType : uint32_t
		{
			/// Invalid minor type.
			MINOR_INVALID = 0u
		};

		/**
		 * Definition of a class holding the major and minor device type.
		 */
		class OCEAN_DEVICES_EXPORT DeviceType
		{
			public:

				/**
				 * Creates a new invalid device type.
				 */
				DeviceType() = default;

				/**
				 * Creates a new device type with defined major type only.
				 * @param major The major device type
				 */
				inline explicit DeviceType(const MajorType major);

				/**
				 * Creates a new device type.
				 * @param major The major device type
				 * @param minor The minor device type
				 */
				inline DeviceType(const MajorType major, const int minor);

				/**
				 * Creates a new device type by a given device type and a further minor type.
				 * @param type Device type to be extended
				 * @param minor The minor type extending the already existing minor type
				 */
				inline DeviceType(const DeviceType& type, const int minor);

				/**
				 * Returns the major device type.
				 * @return Major device type
				 */
				inline MajorType majorType() const;

				/**
				 * Returns the minor device type.
				 * @return Minor device type
				 */
				inline int minorType() const;

				/**
				 * Returns whether two device types are equal.
				 * @param type Second device type
				 * @return True, if so
				 */
				inline bool operator==(const DeviceType& type) const;

				/**
				 * Returns whether two device types are not equal.
				 * @param type Second device type
				 * @return True, if so
				 */
				inline bool operator!=(const DeviceType& type) const;

				/**
				 * Returns whether the right device type is a subset of the left device type.
				 * @param right The right device type
				 * @return True, if so
				 */
				inline bool operator>=(const DeviceType& right) const;

				/**
				 * Returns whether this device type holds a valid major and minor type.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

				/**
				 * Translates the major and minor devices type from a readable string to a DeviceType object.
				 * @param majorType The major device type to translate
				 * @param minorType The major device type to translate
				 * @return The resulting device type
				 */
				static DeviceType translateDeviceType(const std::string& majorType, const std::string& minorType);

			private:

				/// Major device type.
				MajorType major_ = DEVICE_INVALID;

				/// Minor device type.
				uint32_t minor_ = MINOR_INVALID;
		};

	protected:

		/**
		 * Definition of a subscription id for event callbacks.
		 */
		typedef unsigned int SubscriptionId;

	public:

		/**
		 * Returns the name of this device.
		 * @return Device name
		 */
		inline const std::string& name() const;

		/**
		 * Returns the name of the owner library.
		 * @return Library name
		 */
		virtual const std::string& library() const = 0;

		/**
		 * Returns whether this device is active.
		 * @return True, if so
		 */
		virtual bool isStarted() const;

		/**
		 * Returns whether this device is valid.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this device can be use exclusively.
		 * @return True, if so
		 */
		bool isExclusive() const;

		/**
		 * Starts the device.
		 * @return True, if the device could be started successfully
		 */
		virtual bool start();

		/**
		 * Pauses the device.
		 * @return True, if the device could be paused successfully
		 */
		virtual bool pause();

		/**
		 * Stops the device.
		 * @return True, if the device could be stopped successfully
		 */
		virtual bool stop();

		/**
		 * Sets an abstract parameter of this device.
		 * @param parameter Name of the parameter to set
		 * @param value Abstract value to be set
		 * @return True, if the value has been accepted
		 */
		virtual bool setParameter(const std::string& parameter, const Value& value);

		/**
		 * Returns an abstract parameter of this device.
		 * @param parameter Name of the parameter to return the value for
		 * @param value Resulting value
		 * @return True, if succeeded
		 */
		virtual bool parameter(const std::string& parameter, Value& value);

		/**
		 * Returns the major and minor type of this device.
		 * @return Device type
		 */
		inline DeviceType type() const;

		/**
		 * Translates the major devices type to a readable string.
		 * @param majorType The major device type to translate
		 * @return The readable string, empty if the device type is unknown
		 */
		static std::string translateMajorType(const MajorType majorType);

		/**
		 * Translates the major devices type from a readable string to a value.
		 * @param majorType The major device type to translate
		 * @return The translated value
		 */
		static MajorType translateMajorType(const std::string& majorType);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param device Object which would be copied
		 */
		Device(const Device& device) = delete;

		/**
		 * Creates a new device by is name.
		 * @param name The name of the device
		 * @param type Major and minor device type of the device
		 */
		Device(const std::string& name, const DeviceType type);

		/**
		 * Destructs a device.
		 */
		virtual ~Device();

		/**
		 * Disabled copy operator.
		 * @param device Object which would be copied
		 * @return Reference to this object
		 */
		Device& operator=(const Device& device) = delete;

		/**
		 * Returns an invalid subscription id.
		 * @return Id of an invalid subscription
		 */
		static constexpr SubscriptionId invalidSubscriptionId();

	protected:

		/// Name of this device.
		std::string deviceName;

		/// Major and minor type of this device.
		DeviceType deviceType;

		/// Flag determining whether this device is valid.
		bool deviceIsValid;

		/// Device lock.
		mutable Lock deviceLock;
};

inline Device::DeviceType::DeviceType(const MajorType major) :
	major_(major),
	minor_(MINOR_INVALID)
{
	// nothing to do here
}

inline Device::DeviceType::DeviceType(const MajorType major, const int minor) :
	major_(major),
	minor_(minor)
{
	// nothing to do here
}

inline Device::DeviceType::DeviceType(const DeviceType& type, const int minor) :
	major_(type.major_),
	minor_(type.minor_ | minor)
{
	// nothing to do here
}

inline Device::MajorType Device::DeviceType::majorType() const
{
	return major_;
}

inline bool Device::DeviceType::operator==(const DeviceType& type) const
{
	return major_ == type.major_ && minor_ == type.minor_;
}

inline bool Device::DeviceType::operator!=(const DeviceType& type) const
{
	return !(*this == type);
}

inline bool Device::DeviceType::operator>=(const DeviceType& right) const
{
	static_assert(std::is_same<uint32_t, std::underlying_type<MinorType>::type>::value, "Invalid data type!");

	return ((major_ & right.majorType()) == right.majorType()) && ((minor_ & uint32_t(right.minorType())) == uint32_t(right.minorType()));
}

inline Device::DeviceType::operator bool() const
{
	return major_ != DEVICE_INVALID && minor_ != MINOR_INVALID;
}

inline int Device::DeviceType::minorType() const
{
	return minor_;
}

inline const std::string& Device::name() const
{
	return deviceName;
}

inline Device::DeviceType Device::type() const
{
	return deviceType;
}

inline bool Device::isValid() const
{
	return deviceIsValid;
}

constexpr Device::SubscriptionId Device::invalidSubscriptionId()
{
	return SubscriptionId(-1);
}

} // namespace Devices

} // namespace Ocean

#endif // META_OCEAN_DEVICES_DEVICE_H
