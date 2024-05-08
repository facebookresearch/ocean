/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_BLOB_BLOB_H
#define META_OCEAN_DEVICES_BLOB_BLOB_H

#include "ocean/devices/Devices.h"

namespace Ocean
{

namespace Devices
{

namespace Blob
{

/**
 * @ingroup devices
 * @defgroup devicesblob Ocean Devices Blob Library
 * @{
 * The Ocean Devices Blob Library provides a Blob feature based tracking library.
 * This device plugin provides a 6DOF feature tracker with unique name <strong>"Blob Feature Based 6DOF Tracker"</strong>.<br>
 * Do not create a tracker object directory, but use the Devices::Manager object instead.<br>
 * The library is platform independent.
 * @}
 */

/**
 * @namespace Ocean::Devices::Blob Namespace of the Devices Blob library.<p>
 * The Namespace Ocean::Devices::Blob is used in the entire Ocean Devices Blob Library.
 */

// Defines OCEAN_DEVICES_BLOB_EXPORT for dll export and import.
#if defined(_WINDOWS) && defined(OCEAN_RUNTIME_SHARED)
	#ifdef USE_OCEAN_DEVICES_BLOB_EXPORT
		#define OCEAN_DEVICES_BLOB_EXPORT __declspec(dllexport)
	#else
		#define OCEAN_DEVICES_BLOB_EXPORT __declspec(dllimport)
	#endif
#else
	#define OCEAN_DEVICES_BLOB_EXPORT
#endif

/**
 * Returns the name of this devices library.
 * @ingroup devicesblob
 */
OCEAN_DEVICES_BLOB_EXPORT std::string nameBlobLibrary();

#if defined(OCEAN_RUNTIME_STATIC)

/**
 * Registers this Blob device library at the global device manager.
 * This function calls BlobFactory::registerFactory() only.
 * @ingroup devicesblob
 */
void registerBlobLibrary();

/**
 * Unregisters this Blob device library at the global device manager.
 * This function calls BlobFactory::unregisterFactory() only.
 * @return True, if succeeded
 * @ingroup devicesblob
 */
bool unregisterBlobLibrary();

#endif // OCEAN_RUNTIME_STATIC

}

}

}

#endif // META_OCEAN_DEVICES_BLOB_BLOB_H
