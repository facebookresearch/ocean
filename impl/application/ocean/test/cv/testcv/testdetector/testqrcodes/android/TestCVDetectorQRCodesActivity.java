// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

package com.meta.ocean.app.test.cv.testcv.testdetector.testqrcodes.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.Assets;
import com.meta.ocean.platform.android.application.TextActivity;

import android.view.WindowManager;

import java.text.SimpleDateFormat;
import java.util.Calendar;

/**
 * This class implements the main Activity of the test application of the Computer Vision Detector QR Codes library.
 * @ingroup applicationtestcvtestcvdetectorqrcodesandroid
 */
public class TestCVDetectorQRCodesActivity extends TextActivity
{
	// load the library
	static
	{
		System.loadLibrary("OceanTestCVDetectorQRCodes");
	}

	/**
	 * Native interface function to invoke the test.
	 * @param testDuration Duration of each test in seconds, with range (0, infinity)
	 * @param testFunctions Name of test functions to be executed
	 */
	private native void invokeTest(double testDuration, String testFunctions);

	/**
	 * Called when the activity is becoming visible to the user.
	 */
	public void onStart()
	{
		super.onStart();

		// ensuring that the screenn does not turn off during execution
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		Assets.copyFiles(getAssets(), getExternalFilesDir(null) + "/", true);

		// Use "" to provide the test result in the text view
		// Use "STANDARD" to use the LogCat for the test result
		// Use getExternalFilesDir(null) + "/"filename.txt" for a file output

		// stream output to file with timestamp suffix
		// String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(Calendar.getInstance().getTime());
		// final String testOutput = getExternalFilesDir(null) + "/"testCVDetectorQRCodes-" + timeStamp + ".txt";
		final String testOutput = "";

		BaseJni.initialize(testOutput);

		// Define the test duration in seconds.
		final double testDuration = 2.0;

		// Use "" to execute the entire test
		// Use e.g. "harrisdetector" to execute only the sum of squared differences test
		// Use e.g. "linedetector, harrisdetector" to execute more than one test
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
