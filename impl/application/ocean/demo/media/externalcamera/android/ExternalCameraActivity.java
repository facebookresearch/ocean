/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.media.externalcamera.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.media.usb.MediaUSBJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.platform.android.application.*;
import com.meta.ocean.rendering.glescenegraph.RenderingGLESceneGraphJni;

import com.meta.ocean.system.usb.android.OceanUSBManager;

import android.app.Activity;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;
import android.view.Window;

/**
 * This class implements the main Activity object for the shark viewer.
 * @ingroup comoceanshark
 */
public class ExternalCameraActivity extends Activity
{
	static
	{
		System.loadLibrary("OceanDemoMediaExternalCamera");
	}

	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		android.os.Process.setThreadPriority(-20);

		BaseJni.initialize("STANDARD, QUEUED");
		MediaUSBJni.registerLibrary();
		RenderingGLESceneGraphJni.registerLibrary();

		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

		GLFrameView viewObject = new GLFrameView(getApplication(), true, 8, 0);
		viewObject.setKeepScreenOn(true);

		setContentView(viewObject);

		requestPermissions(new String[]{"android.permission.CAMERA"}, 0);

		oceanUSBManager_ = new OceanUSBManager(this);

		if (oceanUSBManager_.isValid())
		{
			BaseJni.information("OceanUSBManager initialized");
		}
		else
		{
			BaseJni.error("Failed to initialized OceanUSBManager");
		}
	}

	/**
	 * Callback for the result from requesting permissions.
	 */
	@Override
	public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults)
	{
		super.onRequestPermissionsResult(requestCode, permissions, grantResults);

		for (int i = 0; i < permissions.length; ++i)
		{
			if (permissions[i].equals("android.permission.CAMERA"))
			{
				if (grantResults[i] == PackageManager.PERMISSION_GRANTED)
				{
					BaseJni.information("The app has permission to access the camera");

					// use a different url to use a different camera (e.g., "LiveVideoId:1" for the second external camera)

					GLFrameView.setFrameMedium("LiveVideoId:0", "LIVE_VIDEO", 1280, 720, true);
				}
				else
				{
					BaseJni.warning("The app does not have permission to access the camera");
				}
			}
		}
	}

	/// Ocean's USB manager providing the JAVA-side logic to access to external USB cameras.
	private OceanUSBManager oceanUSBManager_;
}
