// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.meta.ocean.app.demo.platform.meta.quest.openxr.vrnativeapplication.android;

public class OpenXRVRNativeApplicationActivity extends android.app.NativeActivity
{
	static
	{
		System.loadLibrary("openxr_loader");
		System.loadLibrary("OceanDemoPlatformMetaQuestOpenXRVRNativeApplication");
	}
}
