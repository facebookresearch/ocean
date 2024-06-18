/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.tracking.featuretracker.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.platform.android.application.*;

import android.os.Bundle;

import android.view.ViewGroup;

/**
 * This class implements the main Activity object for the Feature Tracker (Android).
 * @ingroup applicationdemotrackingfeaturetrackerandroid
 */
public class FeatureTrackerActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoTrackingFeatureTracker");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		messageOutput_ = BaseJni.MessageOutput.OUTPUT_QUEUED.value();

		super.onCreate(savedInstanceState);

		addContentView(new MessengerView(this, true), new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 200));

		Assets.copyFiles(getAssets(), getExternalFilesDir(null) + "/", true);		
	}

	@Override
	protected void onCameraPermissionGranted()
	{
		initializeFeatureTracker("LiveVideoId:0", getExternalFilesDir(null) + "/tropical-island-with-toucans_640x640.jpg", "1920x1080");
	}

	/**
	 * Java native interface function to set or change the view's background media object.
	 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
	 * @param pattern The filename of the pattern to be used for tracking
	 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	 * @return True, if succeeded
	 */
	 public static native boolean initializeFeatureTracker(String inputMedium, String pattern, String resolution);
}
