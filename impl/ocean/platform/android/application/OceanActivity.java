/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.platform.android.application;

import android.app.Activity;

import android.content.pm.PackageManager;

import android.os.Bundle;

import android.util.Log;
import android.util.DisplayMetrics;

import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;

import java.util.Set;
import java.util.HashSet;

import com.meta.ocean.base.BaseJni;

import com.meta.ocean.platform.android.*;

/**
 * This class implements the base class for all Ocean activities using a basic Activity.
 * @see OceanAppCompatActivity.
 * @ingroup platformandroid
 */
public class OceanActivity extends Activity
{
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		android.os.Process.setThreadPriority(-20);

		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

		BaseJni.initializeWithMessageOutput(messageOutput_, messageOutputFile_);
		BaseJni.information("Device name: " + android.os.Build.DEVICE);
		BaseJni.setWorkerPoolCapacity(2);

		WindowManager.LayoutParams windowParams = getWindow().getAttributes();
		windowParams.screenBrightness = 1;
		getWindow().setAttributes(windowParams);

		DisplayMetrics displayMetrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
		displayWidth_ = displayMetrics.widthPixels;
		displayHeight_ = displayMetrics.heightPixels;
	}

	public void requestPermission(String permission)
	{
		if (checkSelfPermission(permission) == PackageManager.PERMISSION_GRANTED)
		{
			onPermissionGranted(permission);
		}
		else
		{
			Log.i("Ocean", "Requested permission: " + permission);

			if (pendingPermissionRequests_.add(permission))
			{
				if (pendingPermissionRequests_.size() == 1)
				{
					requestPermissions(new String[]{permission}, OCEAN_ACTIVITY_PERMISSION_CODE);
				}
			}
		}
	}

	@Override
	public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults)
	{
		super.onRequestPermissionsResult(requestCode, permissions, grantResults);

		if (requestCode != OCEAN_ACTIVITY_PERMISSION_CODE)
		{
			return;
		}

		if (grantResults.length == 0)
		{
			// Empty permissions and results arrays should be treated as a cancellation.
			Log.w("Ocean", "A permission request was interrupted/canceled.");
			return;
		}

		for (int n = 0; n < grantResults.length; ++n)
		{
			if (grantResults[n] == PackageManager.PERMISSION_GRANTED)
			{
				Log.i("Ocean", "Granted permission: " + permissions[n]);

				onPermissionGranted(permissions[n]);
			}
			else
			{
				Log.w("Ocean", "Not granted permission: " + permissions[n]);
			}

			pendingPermissionRequests_.remove(permissions[n]);
		}

		if (pendingPermissionRequests_.size() >= 1)
		{
			requestPermissions(new String[]{pendingPermissionRequests_.iterator().next()}, OCEAN_ACTIVITY_PERMISSION_CODE);
		}
	}

	protected void onPermissionGranted(String permission)
	{
		// nothing to do here
	}

	/**
	 * Perform any final cleanup before an activity is destroyed.
	 * This can happen either because the activity is finishing (someone called finish() on it), or because the system is temporarily destroying this instance of the activity to save space.
	 * You can distinguish between these two scenarios with the isFinishing() method.
	 */
	@Override
	protected void onDestroy()
	{
		Log.d("Ocean", "OceanActivity::onDestroy()");

		super.onDestroy();
	}

	/**
	 * Returns the width of the display when the activity was created.
	 * @return The display's width, in pixel, with range [1, infinity)
	 */
	public int displayWidth()
	{
		return displayWidth_;
	}

	/**
	 * Returns the height of the display when the activity was created.
	 * @return The display's height, in pixel, with range [1, infinity)
	 */
	public int displayHeight()
	{
		return displayHeight_;
	}

	/// The output type for all log messages.
	protected int messageOutput_ = BaseJni.MessageOutput.OUTPUT_STANDARD.value();

	/// The file of the output message, STANDARD to use Logcat, empty to cache all messages.
	protected String messageOutputFile_ = "";

	/// The width of the display in pixels.
	protected int displayWidth_;

	/// The height of the display in pixels.
	protected int displayHeight_;

	/// The set of all pending permissions.
	private Set<String> pendingPermissionRequests_ = new HashSet<String>();

	// The unique id for permission requests.
	private static final int OCEAN_ACTIVITY_PERMISSION_CODE = 0;
}
