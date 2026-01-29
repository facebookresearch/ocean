/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.devices.sensors.android;

import com.meta.ocean.devices.android.DevicesAndroidJni;

import com.meta.ocean.platform.android.application.GLRendererViewActivity;

import android.os.Bundle;
import android.util.Log;

/**
 * This class implements the main Activity for the Sensors demo application.
 * @ingroup applicationdemodevicessensors
 */
public class SensorsActivity extends GLRendererViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoDevicesSensors");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		Log.d("Ocean", "SensorsActivity::onCreate");

		super.onCreate(savedInstanceState);

		DevicesAndroidJni.registerLibrary();
	}

	@Override
	public void onResume()
	{
		Log.d("Ocean", "SensorsActivity::onResume");

		super.onResume();
	}

	@Override
	public void onPause()
	{
		Log.d("Ocean", "SensorsActivity::onPause");

		super.onPause();
	}

	@Override
	public void onStop()
	{
		Log.d("Ocean", "SensorsActivity::onStop");

		super.onStop();

		finish();
	}

	@Override
	protected void onDestroy()
	{
		Log.d("Ocean", "SensorsActivity::onDestroy");

		super.onDestroy();

		DevicesAndroidJni.unregisterLibrary();
	}
}
