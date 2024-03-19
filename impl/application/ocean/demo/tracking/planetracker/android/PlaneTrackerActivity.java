// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.app.demo.tracking.planetracker.android;

import com.facebook.ocean.base.BaseJni;
import com.facebook.ocean.platform.android.*;
import com.facebook.ocean.platform.android.application.*;

import android.os.Bundle;

import android.view.ViewGroup;

/**
 * This class implements the main Activity object for the Plane Tracker (Android).
 * @ingroup applicationdemotrackingplanetrackerandroid
 */
public class PlaneTrackerActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoTrackingPlaneTracker");
	}

		protected void onCreate(Bundle savedInstanceState)
		{
			messageOutput_ = BaseJni.MessageOutput.OUTPUT_QUEUED.value();

			super.onCreate(savedInstanceState);

			initializePlaneTracker("LiveVideoId:0", "1280x720");

			addContentView(new MessengerView(this, true), new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 200));
		}

		/**
		 * Java native interface function to set or change the view's background media object.
		 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
		 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
		 * @return True, if succeeded
		 */
		public static native boolean initializePlaneTracker(String inputMedium, String resolution);
}
