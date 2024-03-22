// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.app.demo.tracking.pointtracker.android;

import com.facebook.ocean.platform.android.*;
import com.facebook.ocean.platform.android.application.*;

import android.os.Bundle;

/**
 * This class implements the main Activity object for the Point Tracker (Android).
 * @ingroup applicationdemotrackingpointtrackerandroid
 */
public class PointTrackerActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoTrackingPointTracker");
	}

    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        initializePointTracker("LiveVideoId:0", "1280x720");
    }

    /**
	  * Java native interface function to set or change the view's background media object.
	  * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
	  * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	  * @return True, if succeeded
	  */
	public static native boolean initializePointTracker(String inputMedium, String resolution);
}
