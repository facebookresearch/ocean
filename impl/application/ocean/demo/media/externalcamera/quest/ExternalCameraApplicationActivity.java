/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.media.externalcamera.quest;

import com.meta.ocean.base.BaseJni;

import com.meta.ocean.system.usb.android.OceanUSBManager;

import android.os.Bundle;

public class ExternalCameraApplicationActivity extends android.app.NativeActivity
{
	static
	{
		System.loadLibrary("openxr_loader");
		System.loadLibrary("OceanDemoMediaExternalCamera");
	}

	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		BaseJni.initialize("STANDARD, QUEUED");

		oceanUSBManager_ = new OceanUSBManager(this);

		if (oceanUSBManager_.isValid())
		{
			BaseJni.information("OceanUSBManager initialized");
		}
		else
		{
			BaseJni.error("Failed to initialized OceanUSBManager");
		}
	}

	/// Ocean's USB manager providing the JAVA-side logic to access to external USB cameras.
	private OceanUSBManager oceanUSBManager_;
}
