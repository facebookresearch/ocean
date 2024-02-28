// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.app.shark.android;

import com.facebook.ocean.devices.android.DevicesAndroidJni;
import com.facebook.ocean.devices.pattern.DevicesPatternJni;
import com.facebook.ocean.media.openimagelibraries.MediaOpenImageLibrariesJni;
import com.facebook.ocean.platform.android.*;
import com.facebook.ocean.platform.android.application.*;
import com.facebook.ocean.scenedescription.sdl.obj.SceneDescriptionSDLOBJJni;
import com.facebook.ocean.scenedescription.sdx.x3d.SceneDescriptionSDXX3DJni;
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
