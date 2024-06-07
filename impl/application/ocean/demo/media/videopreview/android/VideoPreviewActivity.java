/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.media.videopreview.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.media.android.MediaAndroidJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.platform.android.application.*;
import com.meta.ocean.rendering.glescenegraph.RenderingGLESceneGraphJni;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.WindowManager;
import android.view.Window;

/**
 * This class implements the main Activity object for the shark viewer.
 * @ingroup comoceanshark
 */
public class VideoPreviewActivity extends Activity
{
	static
	{
		System.loadLibrary("OceanDemoMediaVideoPreview");
	}

	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		android.os.Process.setThreadPriority(-20);

		BaseJni.initialize("STANDARD, QUEUED");
		MediaAndroidJni.registerLibrary();
		RenderingGLESceneGraphJni.registerLibrary();

		requestWindowFeature(Window.FEATURE_NO_TITLE);
		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

		GLFrameView viewObject = new GLFrameView(getApplication(), true, 8, 0);
		viewObject.setKeepScreenOn(true);

		setContentView(viewObject);

		requestPermissions(new String[]{"android.permission.CAMERA"}, 0);

		GLFrameView.setFrameMedium("LiveVideoId:0", "LIVE_VIDEO", 1280, 720, true);
	}

	/**
	 * Called after onRestoreInstanceState(Bundle), onRestart(), or onPause(), for your activity to start interacting with the user.
	 * This is a good place to begin animations, open exclusive-access devices.
	 */
	public void onResume()
	{
		super.onResume();

		MediaAndroidJni.recoverResources();

		Log.i(getClass().getSimpleName(), "onResume");
	}

	/**
	 * Called as part of the activity lifecycle when an activity is going into the background,
	 * but has not (yet) been killed. The counterpart to onResume().
	 */
	public void onPause()
	{
		MediaAndroidJni.releaseResources();
		super.onPause();

		Log.i(getClass().getSimpleName(), "onPause");
	}

	/**
	 * Called when you are no longer visible to the user.
	 * You will next receive either onRestart(), onDestroy(), or nothing, depending on later user activity.
	 */
	public void onStop()
	{
		MediaAndroidJni.releaseResources();
		GLFrameView.release();

		super.onStop();

		Log.i(getClass().getSimpleName(), "onStop");
	}

	protected void onDestroy()
	{
		MediaAndroidJni.releaseResources();

		super.onDestroy();

		Log.i(getClass().getSimpleName(), "onDestroy");
	}
}
