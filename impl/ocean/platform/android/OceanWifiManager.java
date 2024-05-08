/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.platform.android;

import com.meta.ocean.network.NetworkJni;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.util.Log;

import java.nio.ByteOrder;
import java.math.BigInteger;
import java.net.InetAddress;
import java.net.UnknownHostException;

/**
 * This class implements a manager for Wifi connections.
 * @ingroup platformandroid
 */
public class OceanWifiManager
{
	public OceanWifiManager(Context context)
	{
		ipAddress_ = localIPAddress(context);

		if (ipAddress_ != 0)
		{
			NetworkJni.setCurrentLocalIPAddress(ipAddress_);
		}
		else
		{
			Log.w("Ocean", "Failed to determine the Wifi IP Address");
		}
	}

	static public int localIPAddress(Context context)
	{
		WifiManager wifiManager = (WifiManager)context.getSystemService(Context.WIFI_SERVICE);
		return wifiManager.getConnectionInfo().getIpAddress();
	}

	/// The local ip address.
	protected int ipAddress_;
}
