/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.tracking.homographytracker.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.platform.android.application.*;

import android.os.Bundle;

import android.view.ViewGroup;

/**
 * This class implements the main Activity object for the Homography Tracker (Android).
 * @ingroup applicationdemotrackinghomographytrackerandroid
 */
public class HomographyTrackerActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoTrackingHomographyTracker");
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
		initializeHomographyTracker("LiveVideoId:0", "1280x720");
	}

	/**
	 * Java native interface function to initialize the homography tracker.
	 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
	 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	 * @return True, if succeeded
	 */
	public static native boolean initializeHomographyTracker(String inputMedium, String resolution);
}
