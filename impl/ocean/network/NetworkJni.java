// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.network;

/**
 * This class defines the native interface to the Network library of the Ocean framework.
 * @ingroup network
 */
public class NetworkJni
{
	/**
	 * Java native interface function to set the current local IP address.
	 * @param localIPAddress The current local IP address, in big endian notation
	 * @return True, if succeeded
	 */
	public static native boolean setCurrentLocalIPAddress(int localIPAddress);
}
