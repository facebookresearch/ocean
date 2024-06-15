/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.tracking.similaritytracker.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.platform.android.application.*;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.Window;

/**
 * This class implements the main Activity object for the Similarity Tracker (Android).
 * @ingroup applicationdemotrackingsimilaritytrackerandroid
 */
public class SimilarityTrackerActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoTrackingSimilarityTracker");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		messageOutput_ = BaseJni.MessageOutput.OUTPUT_QUEUED.value();

		super.onCreate(savedInstanceState);

		addContentView(new MessengerView(this, true), new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 200));
	}

	@Override
	protected void onCameraPermissionGranted()
	{
		initializeSimilarityTracker("LiveVideoId:0", "1280x720");
	}

	/**
	 * Java native interface function to set or change the view's background media object.
	 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
	 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	 * @return True, if succeeded
	 */
	public static native boolean initializeSimilarityTracker(String inputMedium, String resolution);
}
