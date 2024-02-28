// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_VERTS_MANAGER_H
#define FACEBOOK_NETWORK_VERTS_MANAGER_H

#include "ocean/network/verts/Verts.h"
#include "ocean/network/verts/Driver.h"
#include "ocean/network/verts/Entity.h"

#include "ocean/base/Lock.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/RateCalculator.h"

// Forward declaration
struct verts_component;
struct verts_connection;
struct verts_connection_options;
struct verts_entity;
struct verts_driver;
struct verts_string;

namespace Ocean
{

namespace Network
{

namespace Verts
{

/**
 * This class implements a manager for VERTS related objects.
 * Use the manager to initialize the system and to create/access new drivers.
 * @ingroup networkverts
 */
class OCEAN_NETWORK_VERTS_EXPORT Manager : public Singleton<Manager>
{
	friend class Singleton<Manager>;

	protected:

		/**
		 * Definition of a map mapping driver names to driver objects.
		 */
		typedef std::unordered_map<std::string, WeakDriver> DriverMap;

	public:

		/**
		 * Initializes the manager.
		 * This function should be called once per application.
		 * @param appName The name of the app, must be valid
		 * @param accessToken The access token which will be used to initialize VERTS, must be valid
		 * @return True, if succeeded
		 * @see isInitialized().
		 */
		bool initialize(const std::string& appName, const std::string& accessToken);

		/**
		 * Creates a new driver or returns an existing driver.
		 * Ensure that the manager is initialized before calling this function.<br>
		 * The resulting driver will need some time until it is initialized and until it can be used.
		 * @param zoneName The name of the zone for which the driver will be returned
		 * @param host Explicit host to be used, empty to use the default host, e.g., 'graph.facebook.com' for Facebook universe, or 'graph.<OD-NUMBER>.od.facebook.com' for On-Demand server
		 * @param timeout The timeout in milliseconds, with range [1000, infinity)
		 * @return The specified driver
		 * @see isInitialized(), Driver::isInitialized().
		 */
		SharedDriver driver(const std::string& zoneName, const std::string& host = std::string(), const unsigned int timeout = 5000u);

		/**
		 * Updates the manager.
		 * This function should be called for each new frame.
		 * @param timestamp The update timestamp, must be valid
		 */
		void update(const Timestamp& timestamp);

		/**
		 * Returns true, if the manager is initialized.
		 * @return True, if so
		 */
		inline bool isInitialized() const;

	protected:

		/**
		 * Creates a new manager object.
		 */
		Manager();

		/**
		 * Destructs a manager object.
		 */
		~Manager();

		/**
		 * Releases the manager and all associated resources.
		 */
		void release();

	protected:

		/// True, if the manager is initialized.
		bool isInitialized_ = false;

		/// The name of the app for which VERTS is initialized.
		std::string appName_;

		/// The map mapping driver names to drivers.
		DriverMap driverMap_;

		/// The calculator for the update rate.
		RateCalculator updateRateCalculator_;

		/// The manager's lock.
		mutable Lock lock_;
};

inline bool Manager::isInitialized() const
{
	const ScopedLock scopedLock(lock_);

	return isInitialized_;
}

}

}

}

#endif // FACEBOOK_NETWORK_VERTS_MANAGER_H
