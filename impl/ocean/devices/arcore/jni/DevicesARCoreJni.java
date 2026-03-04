/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.devices.arcore;

/**
 * This class defines the native interface to the Devices::ARCore library.
 * @ingroup devicesarcore
 */
public class DevicesARCoreJni
{
	/**
	 * Java native interface function to register the library.
	 * @return True, if succeeded
	 * @see unregisterLibrary().
	 */
	public static native boolean registerLibrary();

	/**
	 * Java native interface function to unregister the library.
	 * @return True, if succeeded
	 * @see registerLibrary.
	 */
	public static native boolean unregisterLibrary();
}
