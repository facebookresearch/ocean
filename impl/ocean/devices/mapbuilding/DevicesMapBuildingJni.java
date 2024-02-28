// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.devices.mapbuilding;

/**
 * This class defines the native interface to the Devices::MapBuilding library.
 * @ingroup devicesmapbuilding
 */
public class DevicesMapBuildingJni
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
