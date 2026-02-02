/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.rendering.renderer.android;

import com.meta.ocean.platform.android.application.GLRendererViewActivity;

import android.os.Bundle;
import android.util.Log;

/**
 * This class implements the main Activity for the Renderer demo application.
 * @ingroup applicationdemorenderingrenderer
 */
public class RendererActivity extends GLRendererViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoRenderingRenderer");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		Log.d("Ocean", "RendererActivity::onCreate");

		super.onCreate(savedInstanceState);
	}

	@Override
	public void onResume()
	{
		Log.d("Ocean", "RendererActivity::onResume");

		super.onResume();
	}

	@Override
	public void onPause()
	{
		Log.d("Ocean", "RendererActivity::onPause");

		super.onPause();
	}

	@Override
	public void onStop()
	{
		Log.d("Ocean", "RendererActivity::onStop");

		super.onStop();

		finish();
	}

	@Override
	protected void onDestroy()
	{
		Log.d("Ocean", "RendererActivity::onDestroy");

		super.onDestroy();
	}
}
