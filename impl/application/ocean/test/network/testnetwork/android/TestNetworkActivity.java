// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

package com.meta.ocean.app.test.network.testnetwork.android;

import com.facebook.ocean.base.BaseJni;
import com.facebook.ocean.platform.android.application.TextActivity;

import android.view.WindowManager;

/**
 * This class implements the main Activity of the test application of the Network library.
 * @ingroup applicationtestnetworktestnetworkandroid
 */
public class TestNetworkActivity extends TextActivity
{
	// load the library
	static
	{
		System.loadLibrary("OceanTestNetwork");
	}

	/**
	 * Native interface function to invoke the test.
	 * @param testDuration Number of seconds for each test, with range (0, infinity)
	 * @param testFunctions Optional name of test functions to be executed
	 */
	private native void invokeTest(double testDuration, String testFunctions);

	/**
	 * Called when the activity is becoming visible to the user.
	 */
	public void onStart()
	{
		super.onStart();
		android.os.Process.setThreadPriority(-20);

		// ensuring that the screenn does not turn off during execution
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		// Use "" to provide the test result in the text view
		// Use "STANDARD" to use the LogCat for the test result
		// Use getExternalFilesDir(null) + "/filename.txt" for a file output
		final String testOutput = "";

		BaseJni.initialize(testOutput);

		// Define the test duration in seconds.
		final double testDuration = 2.0;

		// Use "" to execute the entire test
		// Use e.g. "data" to execute only the Data test
		// Use e.g. "data, tcpclient" to execute more than one test
		final String testFunctions = "";

		invokeTest(testDuration, testFunctions);
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
