/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.devices.android;

/**
 * This class defines the Java native interface of the Devices::Android library.
 * @ingroup devicesandroid
 */
public class DevicesAndroidJni
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

	/**
	 * Java native interface function to send a new GPS location to the native side.
	 * @param latitude The position's latitude, in degree, always valid, with range [-90, 90]
	 * @param longitude The position's longitude, in degree, must be valid, with range [-180, 180]
	 * @param altitude The position's altitude, in meter, NumericF::minValue() if unknown
	 * @param direction The travel direction of the device, relative to north, in degree, north is 0 degree, east is 90 degree, with range [0, 360], -1 if unknown
	 * @param speed The device's speed, in meter per second, with range [0, infinity), -1 if unknown
	 * @param accuracy The horizontal accuracy as radius, in meter, with range [0, infinity), -1 if unknown
	 * @param altitudeAccuracy The vertical accuracy in meter, with range [0, infinity), -1 if unknown
	 * @param directionAccuracy The direction accuracy in degree, with range [0, 180], -1 if unknown
	 * @param speedAccuracy The speed accuracy, in meter per second, with range [0, infinity), -1 if unknown
	 * @param timestamp The unix timestamp of the GPS location
	 * @return True, if succeeded
	 * @see registerLibrary.
	 */
	public static native boolean newGPSLocation(double latitude, double longitude, float altitude, float direction, float speed, float accuracy, float altitudeAccuracy, float directionAccuracy, float speedAccuracy, double timestamp);
}
