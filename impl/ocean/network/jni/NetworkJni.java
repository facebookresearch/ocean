/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.network;

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
