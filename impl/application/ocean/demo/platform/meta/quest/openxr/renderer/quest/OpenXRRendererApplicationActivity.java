/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.platform.meta.quest.openxr.renderer.quest;

public class OpenXRRendererApplicationActivity extends android.app.NativeActivity
{
	static
	{
		System.loadLibrary("openxr_loader");
		System.loadLibrary("OceanDemoPlatformMetaQuestOpenXRRendererApplication");
	}
}
