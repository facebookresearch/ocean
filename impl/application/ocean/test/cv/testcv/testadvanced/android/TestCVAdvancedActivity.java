// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

package com.meta.ocean.app.test.cv.testcv.testadvanced.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.application.TextActivity;

import android.view.WindowManager;

/**
 * This class implements the main Activity of the test application of the Computer Vision Advanced library.
 * @ingroup applicationtestcvtestcvadvancedandroid
 */
public class TestCVAdvancedActivity extends TextActivity
{
	// load the library
	static
	{
		System.loadLibrary("OceanTestCVAdvanced");
	}

	/**
	 * Native interface function to invoke the Computer Vision Advanced test.
	 * @param testDuration Duration of each test in seconds, with range (0, infinity)
	 * @param testWidth Width of the test frame in pixel, with range [32, infinity)
	 * @param testHeight Height of the test frame in pixel, with range [32, infinity)
	 * @param testFunctions Name of test functions to be executed
	 */
	private native void invokeTest(double testDuration, int testWidth, int testHeight, String testFunctions);

	/**
	 * Called when the activity is becoming visible to the user.
	 */
	public void onStart()
	{
		super.onStart();
		android.os.Process.setThreadPriority(-20);

		// ensuring that the screenn does not turn off during execution
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		// stream output to file with timestamp suffix
		// String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(Calendar.getInstance().getTime());
		// final String testOutput = getExternalFilesDir(null) + "/testCVAdvandedDetector-" + timeStamp + ".txt";
		final String testOutput = "";

		BaseJni.initialize(testOutput);

		// Define the dimension of the default test frame in pixel, with range [32, infinity)x[32, infinity)
		final int testWidth = 1920;
		final int testHeight = 1080;

		// Define the test duration in seconds.
		final double testDuration = 2.0;

		// Use "" to execute the entire test
		// Use e.g. "advancedframefilterbinomial" to execute only the advanced frame filter binomail test
		// Use e.g. "advancedframefilterbinomial, advancedframeshrinker" to execute more than one test
		final String testFunctions = "";

		invokeTest(testDuration, testWidth, testHeight, testFunctions);
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
