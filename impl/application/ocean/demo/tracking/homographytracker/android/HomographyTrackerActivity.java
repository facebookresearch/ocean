// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.app.demo.tracking.homographytracker.android;

import com.facebook.ocean.base.BaseJni;
import com.facebook.ocean.platform.android.*;
import com.facebook.ocean.platform.android.application.*;

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

		protected void onCreate(Bundle savedInstanceState)
		{
			messageOutput_ = BaseJni.MessageOutput.OUTPUT_QUEUED.value();

			super.onCreate(savedInstanceState);

			initializeHomographyTracker("LiveVideoId:0", "1280x720");

			addContentView(new MessengerView(this, true), new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 200));
		}

		/**
		 * Java native interface function to initialize the homography tracker.
		 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
		 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
		 * @return True, if succeeded
		 */
		public static native boolean initializeHomographyTracker(String inputMedium, String resolution);
}