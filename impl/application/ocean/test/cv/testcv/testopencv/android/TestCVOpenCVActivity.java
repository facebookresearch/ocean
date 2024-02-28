// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.app.test.cv.testcv.testopencv.android;

import com.facebook.ocean.base.BaseJni;
import com.facebook.ocean.platform.android.application.TextActivity;

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

		/**
		 * Use "" to provide the test result in the text view
		 * Use "STANDARD" to use the LogCat for the test result
		 * Use getExternalFilesDir(null) + "/filename.txt" for a file output
		 */
		final String testOutput = "";

		BaseJni.initialize(testOutput);

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
