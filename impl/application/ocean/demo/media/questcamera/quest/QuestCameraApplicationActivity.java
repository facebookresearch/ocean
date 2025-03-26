/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.media.questcamera.quest;

import com.meta.ocean.base.BaseJni;

import android.os.Bundle;

public class QuestCameraApplicationActivity extends android.app.NativeActivity
{
	static
	{
		System.loadLibrary("openxr_loader");
		System.loadLibrary("OceanDemoMediaQuestCamera");
	}
}
