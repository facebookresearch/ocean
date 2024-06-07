/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.platform.meta.quest.openxr.vrnativeapplication.quest;

public class OpenXRVRNativeApplicationActivity extends android.app.NativeActivity
{
	static
	{
		System.loadLibrary("openxr_loader");
		System.loadLibrary("OceanDemoPlatformMetaQuestOpenXRVRNativeApplication");
	}
}
