// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

package com.facebook.ocean.app.demo.tracking.uvtexturemapping.android;

import com.facebook.ocean.base.BaseJni;
import com.facebook.ocean.platform.android.*;
import com.facebook.ocean.platform.android.application.*;

import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.ViewGroup;

/**
 * This class implements the main Activity object for the UV texture mapping (Android).
 * @ingroup applicationdemotrackinguvtexturemappingandroid
 */
public class UVTextureMappingActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoTrackingUVTextureMapping");
	}

	protected void onCreate(Bundle savedInstanceState)
	{
		messageOutput_ = BaseJni.MessageOutput.OUTPUT_QUEUED.value();

		super.onCreate(savedInstanceState);

		addContentView(new MessengerView(this, true), new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 200)	);

		Assets.copyFiles(getAssets(), getExternalFilesDir(null) + "/", true);

		requestPermission("android.permission.CAMERA");
	}

	@Override
	protected void onPermissionGranted(String permission)
	{
		super.onPermissionGranted(permission);

		if (permission == "android.permission.CAMERA")
		{
			if (!initializeUVTextureMapping("LiveVideoId:0", getExternalFilesDir(null) + "/sift640x512.bmp", "1920x1080"))
			{
				Log.e("UVTextureMappingActivity", "Failed to initialize UV texture mapping!");
			}
		}
	}


	/**
	 * Java native interface function to set or change the view's background media object.
	 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
	 * @param pattern The filename of the pattern to be used for tracking
	 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	 * @return True, if succeeded
	 */
	 public static native boolean initializeUVTextureMapping(String inputMedium, String pattern, String resolution);
}
