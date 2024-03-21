// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.meta.ocean.app.demo.tracking.bullseyetracker.quest;

public class DemoTrackingBullseyeTrackerActivity extends android.app.NativeActivity
{
	static
	{
		System.loadLibrary("openxr_loader");
		System.loadLibrary("OceanDemoBullseyeTracker");
	}
}
