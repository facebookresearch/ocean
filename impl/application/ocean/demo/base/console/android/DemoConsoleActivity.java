// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.meta.ocean.app.demo.base.console.android;

import com.facebook.ocean.base.BaseJni;
import com.facebook.ocean.platform.android.application.TextActivity;

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

		// stream output to file with timestamp suffix
		// String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(Calendar.getInstance().getTime());
		// final String testOutput = getExternalFilesDir(null) + "/testCV-" + timeStamp + ".txt";
		final String applicationOutput = "";

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
