/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.test.media.testmedia.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.application.TextActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;

/**
 * This class implements the main Activity of the test application of the Media library.
 * By default, the app will run all tests for 2.0 seconds each.<br>
 * In addition, the user can configure the test execution through an intent, the app can be started and configured via:<br>
 * <pre>
 * adb shell am start -n com.meta.ocean.app.test.media.testmedia.android/.TestMediaActivity --es test_functions '"special, android.videoencoderdecoder"' --ed test_duration 5.0
 * </pre>
 * @ingroup applicationtestmediatestmediaandroid
 */
public class TestMediaActivity extends TextActivity
{
	// load the library
	static
	{
		System.loadLibrary("OceanTestMedia");
	}

	/**
	 * Native interface function to invoke the test.
	 * @param testDuration Number of seconds for each test, with range (0, infinity)
	 * @param testFunctions Optional name of test functions to be executed
	 */
	private native void invokeTest(double testDuration, String testFunctions);

	/**
	 * Reads intent extras for test configuration.
	 */
	private void readIntentExtras()
	{
		Log.d("Ocean", "TestMediaActivity: readIntentExtras() called");

		if (getIntent() != null)
		{
			Log.d("Ocean", "TestMediaActivity: Intent is not null");
			Bundle extras = getIntent().getExtras();

			if (extras != null)
			{
				Log.d("Ocean", "TestMediaActivity: Extras bundle is not null");
				Log.d("Ocean", "TestMediaActivity: Extras contents: " + extras.toString());

				if (extras.containsKey(EXTRA_TEST_FUNCTIONS))
				{
					configuredTestFunctions_ = extras.getString(EXTRA_TEST_FUNCTIONS);
					Log.d("Ocean", "TestMediaActivity: Found test_functions: " + configuredTestFunctions_);
				}
				else
				{
					Log.d("Ocean", "TestMediaActivity: No test_functions in extras");
				}

				if (extras.containsKey(EXTRA_TEST_DURATION))
				{
					configuredTestDuration_ = extras.getDouble(EXTRA_TEST_DURATION);
					Log.d("Ocean", "TestMediaActivity: Found test_duration: " + configuredTestDuration_);
				}
				else
				{
					Log.d("Ocean", "TestMediaActivity: No test_duration in extras");
				}
			}
			else
			{
				Log.d("Ocean", "TestMediaActivity: Extras bundle is null");
			}
		}
		else
		{
			Log.d("Ocean", "TestMediaActivity: Intent is null");
		}
	}

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

		// Read intent extras to check for configuration
		readIntentExtras();

		// Define the test duration in seconds.
		// Use "" to execute the entire test

		// Use e.g. "android.videoencoderdecoder" to execute only the Android.VideoEncoderDecoder test
		double testDuration = 2.0;
		String testFunctions = "";

		if (configuredTestDuration_ != null)
		{
			Log.d("Ocean", "TestMediaActivity: Using configured test duration: " + configuredTestDuration_);
			testDuration = configuredTestDuration_;
		}

		if (configuredTestFunctions_ != null)
		{
			Log.d("Ocean", "TestMediaActivity: Using configured test functions: " + configuredTestFunctions_);
			testFunctions = configuredTestFunctions_;
		}

		Log.d("Ocean", "TestMediaActivity: Starting tests with duration=" + testDuration + ", functions='" + testFunctions + "'");

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

	// Intent extra keys
	public static final String EXTRA_TEST_FUNCTIONS = "test_functions";
	public static final String EXTRA_TEST_DURATION = "test_duration";

	// Member variables for configured values from intent
	private String configuredTestFunctions_ = null;
	private Double configuredTestDuration_ = null;
}
