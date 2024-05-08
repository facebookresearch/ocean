/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.base.console.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.application.TextActivity;

import android.view.WindowManager;

/**
 * This class implements the main Activity of the Console demo application for Android.
 * @ingroup applicationdemobaseconsoleandroid
 */
public class DemoConsoleActivity extends TextActivity
{
	// load the library
	static
	{
		System.loadLibrary("OceanDemoBaseConsole");
	}

	/**
	 * Native interface function to invoke the demo application.
	 */
	private native void invokeApplication();

	/**
	 * Called when the activity is becoming visible to the user.
	 */
	public void onStart()
	{
		super.onStart();

		// ensuring that the screenn does not turn off during execution
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		final String applicationOutput = ""; // use e.g., "STANDARD" to show th log output in 'adb logcat -s "Ocean"'

		BaseJni.initialize(applicationOutput);

		invokeApplication();
	}

	/**
	 * Called when the activity is no longer visible to the user, because another activity has been resumed and is covering this one.
	 */
	public void onStop()
	{
		super.onStop();

		BaseJni.exit(0);
	}
}
