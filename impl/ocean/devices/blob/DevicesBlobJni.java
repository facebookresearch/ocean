// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.devices.blob;

/**
 * This class defines the Java native interface to the Devices::Blob library.
 * @ingroup devicesblob
 */
public class DevicesBlobJni
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
