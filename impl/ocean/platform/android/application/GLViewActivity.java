/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.platform.android.application;

import com.meta.ocean.platform.android.*;

import android.app.Activity;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;

/**
 * This class implements an Activity with basic OpenGL ES support.
 * @see GLViewAppCompatActivity.
 * @ingroup platformandroid
 */
public class GLViewActivity extends OceanActivity
{
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		Log.d("Ocean", "GLViewActivity::onCreate()");

		super.onCreate(savedInstanceState);

		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

		glView_= createGLView();
		glView_.setKeepScreenOn(true);
		setContentView(glView_);

		WindowManager.LayoutParams windowParams = getWindow().getAttributes();
		windowParams.screenBrightness = 1;
		getWindow().setAttributes(windowParams);

		DisplayMetrics displayMetrics = new DisplayMetrics();
		getWindowManager().getDefaultDisplay().getMetrics(displayMetrics);
		displayWidth_ = displayMetrics.widthPixels;
		displayHeight_ = displayMetrics.heightPixels;
	}

	/**
	 * Creates the OpenGLES View of this activity.
	 * Override this function to provide a custom GLView.
	 */
	protected GLView createGLView()
	{
		Log.d("Ocean", "GLViewActivity::createGLView()");

		return new GLView(getApplication(), true /*translucent*/, 24 /*depth*/, 0 /*stencil*/);
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

		GLFrameView.release();

		super.onDestroy();
	}

	/// The OpenGLES View of this activity.
	protected GLView glView_ = null;

	/// The width of the display in pixels.
	protected int displayWidth_;

	/// The height of the display in pixels.
	protected int displayHeight_;
}
