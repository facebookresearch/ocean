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
import android.os.Handler;

import android.util.Log;

import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;

import java.util.Set;
import java.util.HashSet;

import com.meta.ocean.media.android.MediaAndroidJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.rendering.glescenegraph.RenderingGLESceneGraphJni;

/**
 * This class implements the an Activity with live video background based on a basic Activity.
 * @see GLViewAppCompatActivity.
 * @ingroup platformandroid
 */
public class GLViewActivity extends OceanActivity
{
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		Log.d("Ocean", "GLViewActivity::onStart()");

		super.onCreate(savedInstanceState);

		MediaAndroidJni.registerLibrary();
		RenderingGLESceneGraphJni.registerLibrary();

		glView = new GLView(getApplication(), true /*translucent*/, 24 /*depth*/, 0 /*stencil*/);
		glView.setKeepScreenOn(true);
		setContentView(glView);

		requestPermission("android.permission.CAMERA");
	}

	@Override
	protected void onPermissionGranted(String permission)
	{
		Log.d("Ocean", "GLViewActivity::onPermissionGranted(): " + permission);

		super.onPermissionGranted(permission);

		GLView.onPermissionGranted(permission);
	}

	/**
	 * Called after onRestoreInstanceState(Bundle), onRestart(), or onPause(), for your activity to start interacting with the user.
	 * This is a good place to begin animations, open exclusive-access devices.
	 */
	@Override
	public void onResume()
	{
		Log.d("Ocean", "GLViewActivity::onResume()");

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
		Log.d("Ocean", "GLViewActivity::onPause()");

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
		Log.d("Ocean", "GLViewActivity::onStop()");

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
		Log.d("Ocean", "GLViewActivity::onDestroy()");

		MediaAndroidJni.releaseResources();

		GLFrameView.release();

		RenderingGLESceneGraphJni.unregisterLibrary();
		MediaAndroidJni.unregisterLibrary();

		super.onDestroy();
	}

	/// The OpenGLES View of this activity.
	protected GLView glView = null;
}
