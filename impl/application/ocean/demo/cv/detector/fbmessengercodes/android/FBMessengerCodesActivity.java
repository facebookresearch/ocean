// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.meta.ocean.app.demo.cv.detector.fbmessengercodes.android;

import com.facebook.ocean.platform.android.*;
import com.facebook.ocean.platform.android.application.*;

import android.os.Bundle;

/**
 * This class implements the main Activity object for the FB Messenger codes (Android).
 * @ingroup applicationdemocvdetectorfbmessengercodesandroid
 */
public class FBMessengerCodesActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoCVDetectorFBMessengerCodes");
	}

	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		String assetDirectory = getExternalFilesDir(null).getAbsolutePath() + "/";
		Assets.copyFiles(getAssets(), assetDirectory, true);

		initializeFBMessengerCodes("LiveVideoId:0", "1280x720", "Y8",
			assetDirectory + "messenger_code_badge_template.bmp",
			assetDirectory + "messenger_code_bullseye_template.bmp",
			assetDirectory + "MessengerCodeDetectClassifier1.xml");
	}

	/**
	 * Java native interface function to set or change the view's background media object.
	 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
	 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	 * @param orientation The orientation of the input medium in relation to the device around the z-axis in radian, with range [0, 2PI)
	 * @return True, if succeeded
	 */
	public static native boolean initializeFBMessengerCodes(String inputMedium, String resolution, String pixelFormat, String badge, String bullseye, String classifier);
}
