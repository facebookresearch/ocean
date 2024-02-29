// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.app.demo.cv.detector.linedetector.android;

import com.facebook.ocean.base.BaseJni;
import com.facebook.ocean.platform.android.*;
import com.facebook.ocean.platform.android.application.*;

import android.os.Bundle;

import android.view.ViewGroup;

/**
 * This class implements the main Activity object for the Line Detector (Android).
 * @ingroup applicationdemocvdetectorlinedetectorandroid
 */
public class LineDetectorActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoCVDetectorLineDetector");
	}

	protected void onCreate(Bundle savedInstanceState)
	{
		messageOutput_ = BaseJni.MessageOutput.OUTPUT_QUEUED.value();

		super.onCreate(savedInstanceState);

		addContentView(new MessengerView(this, true), new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 200));

		initializeLineDetector("LiveVideoId:0", "1280x720");
	}

	/**
	 * Java native interface function to set or change the view's background media object.
	 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
	 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	 * @return True, if succeeded
	 */
	public static native boolean initializeLineDetector(String inputMedium, String resolution);
}
