// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#ifndef META_OCEAN_DEVICES_BLOB_FACTORY_H
#define META_OCEAN_DEVICES_BLOB_FACTORY_H

#include "ocean/devices/blob/Blob.h"

#include "ocean/devices/Factory.h"

namespace Ocean
{

namespace Devices
{

namespace Blob
{

/**
 * This class implements a device factory for the Blob feature based tracking system.
 * @ingroup devicesblob
 */
class OCEAN_DEVICES_BLOB_EXPORT BlobFactory : public Factory
{
	public:

		/**
		 * Registers this factory at the manager.
		 * Beware: Unregister this factory if not needed anymore.
		 * @return True, if this factory hasn't been registered before
		 */
		static bool registerFactory();

		/**
		 * Unregisters this factory at the manger.
		 * This unregistration should be done after all created devices have been released.
		 * @return True, if this factory chould be unregistered
		 */
		static bool unregisterFactory();

	private:

		/**
		 * Creates a new factory.
		 */
		BlobFactory();

		/**
		 * Creates a BlobTracker6DOF tracker object.
		 * @param name The name of the new tracker, must be valid
		 * @param deviceType The device type of the tracker, must be valid
		 * @return New created device object.
		 */
		static Device* createBlobTracker6DOF(const std::string& name, const Device::DeviceType& deviceType);
};

}

}

}

#endif // META_OCEAN_DEVICES_BLOB_FACTORY_H
