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

import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;

import java.util.Set;
import java.util.HashSet;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.media.android.MediaAndroidJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.rendering.glescenegraph.RenderingGLESceneGraphJni;

/**
 * This class implements the base class for all activities using a GLFrameView as content view.
 * @ingroup platformandroid
 */
public class GLFrameViewActivity extends OceanActivity
{
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		MediaAndroidJni.registerLibrary();
		RenderingGLESceneGraphJni.registerLibrary();

		glViewFrame = new GLFrameView(getApplication(), true /*translucent*/, 24 /*depth*/, 0 /*stencil*/);
		glViewFrame.setKeepScreenOn(true);
		setContentView(glViewFrame);

		requestPermission("android.permission.CAMERA");
	}

	@Override
	protected void onPermissionGranted(String permission)
	{
		super.onPermissionGranted(permission);

		GLFrameView.onPermissionGranted(permission);

		if (permission.equals("android.permission.CAMERA"))
		{
			BaseJni.debug("GLFrameViewActivity: The app has permission to access the camera");

			onCameraPermissionGranted();
		}
	}

	/**
	 * Event function which will be called when the application has been granted access to a camera.
	 */
	protected void onCameraPermissionGranted()
	{
		// should be implemented in a derived class (in case the application is using a camera as input)
		// the derived class could doe something like

		// GLFrameView.setFrameMedium("LiveVideoId:0", "LIVE_VIDEO", 1280, 720, true);
	}

	/**
	 * Called after onRestoreInstanceState(Bundle), onRestart(), or onPause(), for your activity to start interacting with the user.
	 * This is a good place to begin animations, open exclusive-access devices.
	 */
	@Override
	public void onResume()
	{
		super.onResume();

		MediaAndroidJni.recoverResources();
	}

	/**
	 * Called as part of the activity lifecycle when an activity is going into the background,
	 * but has not (yet) been killed. The counterpart to onResume().
	 */
	@Override
	public void onPause()
	{
		super.onPause();

		MediaAndroidJni.releaseResources();
	}

	/**
	 * Called when you are no longer visible to the user.
	 * You will next receive either onRestart(), onDestroy(), or nothing, depending on later user activity.
	 */
	@Override
	public void onStop()
	{
		super.onStop();

		MediaAndroidJni.releaseResources();
	}

	/**
	 * Perform any final cleanup before an activity is destroyed.
	 * This can happen either because the activity is finishing (someone called finish() on it), or because the system is temporarily destroying this instance of the activity to save space.
	 * You can distinguish between these two scenarios with the isFinishing() method.
	 */
	@Override
	protected void onDestroy()
	{
		MediaAndroidJni.releaseResources();

		GLFrameView.release();

		RenderingGLESceneGraphJni.unregisterLibrary();
		MediaAndroidJni.unregisterLibrary();

		super.onDestroy();
	}

	/// The OpenGLES FrameView of this activity.
	protected GLFrameView glViewFrame = null;
}
