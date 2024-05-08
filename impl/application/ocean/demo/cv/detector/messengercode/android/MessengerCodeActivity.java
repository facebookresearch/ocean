// Copyright (c) Meta Platforms, Inc. and affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

package com.meta.ocean.app.demo.cv.detector.messengercode.android;

import com.meta.ocean.platform.android.*;
import com.meta.ocean.platform.android.application.*;

import android.os.Bundle;

/**
 * This class implements the main Activity object for the Messenger code (Android).
 * @ingroup applicationdemocvdetectormessengercodeandroid
 */
public class MessengerCodeActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoCVDetectorMessengerCode");
	}

	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		String assetDirectory = getExternalFilesDir(null).getAbsolutePath() + "/";
		Assets.copyFiles(getAssets(), assetDirectory, true);

		initializeMessengerCode("LiveVideoId:0", "1280x720", "Y8");
	}

	/**
	 * Java native interface function to set or change the view's background media object.
	 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
	 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	 * @param orientation The orientation of the input medium in relation to the device around the z-axis in radian, with range [0, 2PI)
	 * @return True, if succeeded
	 */
	public static native boolean initializeMessengerCode(String inputMedium, String resolution, String pixelFormat);
}
