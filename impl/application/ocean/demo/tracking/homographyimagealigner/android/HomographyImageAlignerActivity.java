// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.app.demo.tracking.homographyimagealigner.android;

import com.facebook.ocean.platform.android.*;
import com.facebook.ocean.platform.android.application.*;

import android.os.Bundle;

/**
 * This class implements the main Activity object for the Homography Image Aligner (Android).
 * @ingroup applicationdemotrackinghomographyimagealignerandroid
 */
public class HomographyImageAlignerActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoTrackingHomographyImageAligner");
	}

    protected void onCreate(Bundle savedInstanceState)
    {
        videoUrl_ = "LiveVideoId:0";
		videoPreferredWidth_ = 1280;
		videoPreferredHeight_ = 720;

        super.onCreate(savedInstanceState);

        initializeHomographyImageAligner(videoUrl_, "1280x720");
    }

    /**
	 * Java native interface function to set or change the view's background media object.
	 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
	 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	 * @return True, if succeeded
	 */
	public static native boolean initializeHomographyImageAligner(String inputMedium, String resolution);
}
