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

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.media.android.MediaAndroidJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.rendering.glescenegraph.RenderingGLESceneGraphJni;

/**
 * This class implements the base class for all activities using a GLFrameView as content view (a view allowing to render a video background).
 * @ingroup platformandroid
 */
public class GLFrameViewActivity extends GLRendererViewActivity
{
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		Log.d("Ocean", "GLFrameViewActivity::onCreate()");

		MediaAndroidJni.registerLibrary();

		super.onCreate(savedInstanceState);

		requestPermission("android.permission.CAMERA");
	}

	/**
	 * Creates the OpenGLES View of this activity.
	 * @see GLViewActivity::createGLView().
	 */
	@Override
	protected GLView createGLView()
	{
		Log.d("Ocean", "GLFrameViewActivity::createGLView()");

		return new GLFrameView(getApplication(), true /*translucent*/, 24 /*depth*/, 0 /*stencil*/);
	}

	@Override
	protected void onPermissionGranted(String permission)
	{
		super.onPermissionGranted(permission);

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
	}

	/**
	 * Called as part of the activity lifecycle when an activity is going into the background,
	 * but has not (yet) been killed. The counterpart to onResume().
	 */
	@Override
	public void onPause()
	{
		super.onPause();
	}

	/**
	 * Called when you are no longer visible to the user.
	 * You will next receive either onRestart(), onDestroy(), or nothing, depending on later user activity.
	 */
	@Override
	public void onStop()
	{
		super.onStop();
	}

	/**
	 * Perform any final cleanup before an activity is destroyed.
	 * This can happen either because the activity is finishing (someone called finish() on it), or because the system is temporarily destroying this instance of the activity to save space.
	 * You can distinguish between these two scenarios with the isFinishing() method.
	 */
	@Override
	protected void onDestroy()
	{
		super.onDestroy();

		MediaAndroidJni.unregisterLibrary();
	}
}
