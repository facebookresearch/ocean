/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.test.cv.testcv.testopencv.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.application.TextActivity;

import android.view.WindowManager;

/**
 * This class implements the main Activity of the test application of the Base library.
 * @ingroup applicationtestcvtestcvopencvandroid
 */
public class TestCVOpenCVActivity extends TextActivity
{
	// load the library
	static
	{
		 System.loadLibrary("OceanTestCVOpenCV");
	}

	/**
	 * Native interface function to invoke the test.
	 * @param testDuration Duration of each test in seconds, with range (0, infinity)
	 * @param imageFilename The name of the image to be used for testing, if any
	 * @param testFunctions Name of test functions to be executed
	 */
	private native void invokeTest(double testDuration, String imageFilename, String testFunctions);

	/**
	 * Called when the activity is becoming visible to the user.
	 */
	public void onStart()
	{
		super.onStart();
		android.os.Process.setThreadPriority(-20);

		// ensuring that the screenn does not turn off during execution
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		// The output to which the application's log will be written
		// use "STANDARD" to show the log output in 'adb logcat -s "Ocean"'
		// use "QUEUED" to queue the logs and to allow popping them later
		// use "filename" to write the log output into a file (e.g., with getExternalFilesDir(null) + "/filename.txt")
		final String applicationOutput = "STANDARD, QUEUED";

		BaseJni.initialize(applicationOutput);

		/**
		 * Define the test duration in seconds.
		 */
		final double testDuration = 2.0;

		/**
		 * Define the name of the test image.
		 */
		final String imageFilename = "";

		/**
		 * Use "" to execute the entire test
		 * Use e.g. "frameinterpolatornearestneighbor"
		 */
		final String testFunctions = "";

		invokeTest(testDuration, imageFilename, testFunctions);
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
