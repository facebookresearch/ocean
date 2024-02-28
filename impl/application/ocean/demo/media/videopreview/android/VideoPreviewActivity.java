// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.app.demo.media.videopreview.android;

import com.facebook.ocean.base.BaseJni;
import com.facebook.ocean.media.android.MediaAndroidJni;
import com.facebook.ocean.platform.android.*;
import com.facebook.ocean.platform.android.application.*;
import com.facebook.ocean.rendering.glescenegraph.RenderingGLESceneGraphJni;

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

	/// Global shark activity.
	public static VideoPreviewActivity mainActivity = null;

	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		android.os.Process.setThreadPriority(-20);

		BaseJni.initialize("STANDARD");
		MediaAndroidJni.registerLibrary();
		RenderingGLESceneGraphJni.registerLibrary();

		mainActivity = this;

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
