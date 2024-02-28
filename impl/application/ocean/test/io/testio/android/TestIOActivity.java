// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.app.test.io.testio.android;

import com.facebook.ocean.base.BaseJni;
import com.facebook.ocean.platform.android.application.TextActivity;

import android.app.Activity;
import android.os.Bundle;
import android.widget.ScrollView;
import android.widget.TextView;
import android.view.WindowManager;

public class TestIOActivity extends TextActivity
{
	// load the library
	static
	{
		System.loadLibrary("OceanTestIO");
	}

	/**
	 * Native interface function to invoke the cv test.
	 * @param outputFilename Optional filename for the output information
	 * @param testDuration Number of seconds for each test
	 */
	private native String invokeTest(String outputFilename, double testDuration);

	/**
	 * Native interface function to invoke the test.
	 * @param testDuration Number of seconds for each test, with range (0, infinity)
	 * @param testFunctions Optional name of test functions to be executed
	 */
	private native void invokeTest(String testFunctions, String outputfilename);

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

		String nativeValue = invokeTest(testOutput, testDuration);
		TextView textView = new TextView(this);
		textView.setText(nativeValue);
		ScrollView scrollView = new ScrollView(this);
		scrollView.addView(textView);
		setContentView(scrollView);
	}
}
