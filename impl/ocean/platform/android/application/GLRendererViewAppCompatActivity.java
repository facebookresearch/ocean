/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.platform.android.application;

import android.os.Bundle;

import android.util.Log;

import com.meta.ocean.platform.android.*;
import com.meta.ocean.rendering.glescenegraph.RenderingGLESceneGraphJni;

/**
 * This class implements an AppCompatActivity with with GLRenderView to render 3D content..
 * @see GLViewActivity.
 * @ingroup platformandroid
 */
public class GLRendererViewAppCompatActivity extends GLViewAppCompatActivity
{
	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		Log.d("Ocean", "GLRendererViewAppCompatActivity::onCreate()");

		RenderingGLESceneGraphJni.registerLibrary();

		super.onCreate(savedInstanceState);
	}

	/**
	 * Creates the OpenGLES View of this activity.
	 * @see GLViewAppCompatActivity::createGLView().
	 */
	@Override
	protected GLView createGLView()
	{
		Log.d("Ocean", "GLRendererViewAppCompatActivity::createGLView()");

		return new GLRendererView(getApplication(), true /*translucent*/, 24 /*depth*/, 0 /*stencil*/);
	}

	/**
	 * Called after onRestoreInstanceState(Bundle), onRestart(), or onPause(), for your activity to start interacting with the user.
	 * This is a good place to begin animations, open exclusive-access devices.
	 */
	@Override
	public void onResume()
	{
		Log.d("Ocean", "GLRendererViewAppCompatActivity::onResume()");

		super.onResume();
	}

	/**
	 * Called as part of the activity lifecycle when an activity is going into the background,
	 * but has not (yet) been killed. The counterpart to onResume().
	 */
	@Override
	public void onPause()
	{
		Log.d("Ocean", "GLRendererViewAppCompatActivity::onPause()");

		super.onPause();
	}

	/**
	 * Called when you are no longer visible to the user.
	 * You will next receive either onRestart(), onDestroy(), or nothing, depending on later user activity.
	 */
	@Override
	public void onStop()
	{
		Log.d("Ocean", "GLRendererViewAppCompatActivity::onStop()");

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
		Log.d("Ocean", "GLRendererViewAppCompatActivity::onDestroy()");

		super.onDestroy();

		RenderingGLESceneGraphJni.unregisterLibrary();
	}
}
