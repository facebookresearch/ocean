// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.meta.ocean.app.demo.tracking.pointtracker.meta.quest.openxr;

public class DemoTrackingPointTrackerActivity extends android.app.NativeActivity {
  static {
		System.loadLibrary("openxr_loader");
    System.loadLibrary("OceanOpenXRPointTracker");
  }
}
