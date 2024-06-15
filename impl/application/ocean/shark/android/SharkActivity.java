/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.shark.android;

import com.meta.ocean.devices.android.DevicesAndroidJni;
import com.meta.ocean.devices.pattern.DevicesPatternJni;
import com.meta.ocean.media.openimagelibraries.MediaOpenImageLibrariesJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.platform.android.application.*;
import com.meta.ocean.scenedescription.sdl.obj.SceneDescriptionSDLOBJJni;
import com.meta.ocean.scenedescription.sdx.x3d.SceneDescriptionSDXX3DJni;
import android.os.Bundle;

/**
 * This class implements the main Activity object for the Shark viewer.
 * @ingroup sharkandroid
 */
public class SharkActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanShark");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		MediaOpenImageLibrariesJni.registerLibrary();

		DevicesAndroidJni.registerLibrary();
		DevicesPatternJni.registerLibrary();

		SceneDescriptionSDLOBJJni.registerLibrary();
		SceneDescriptionSDXX3DJni.registerLibrary();

		final String assetDir = getExternalFilesDir(null) + "/";
		Assets.copyFiles(getAssets(), assetDir, true);
		NativeInterfaceShark.loadScene(assetDir + "dinosaur.ox3dv", true);
	}

	@Override
	protected void onCameraPermissionGranted()
	{
		GLFrameView.setFrameMedium("LiveVideoId:0", "LIVE_VIDEO", 1280, 720, true);
	}

	@Override
	protected void onDestroy()
	{
		SceneDescriptionSDXX3DJni.unregisterLibrary();
		SceneDescriptionSDLOBJJni.unregisterLibrary();

		DevicesPatternJni.unregisterLibrary();
		DevicesAndroidJni.unregisterLibrary();

		MediaOpenImageLibrariesJni.unregisterLibrary();

		super.onDestroy();
	}
}
