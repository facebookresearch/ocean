/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.tracking.slam.slamtracker.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.platform.android.application.*;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.Window;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;

/**
 * This class implements the main Activity object for the SLAM Tracker (Android).
 * @ingroup applicationdemotrackingslamslamtrackerandroid
 */
public class SLAMTrackerActivity extends GLFrameViewActivity
{
	/// True, to allow making a recording; False, to remove recording capabilities
	private boolean allowRecording_ = false;

	/// The recording button
	private Button recordingButton_;

	/// State whether recording is active
	private boolean isRecording_ = false;

	static
	{
		System.loadLibrary("OceanDemoTrackingSLAMTracker");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		messageOutput_ = BaseJni.MessageOutput.OUTPUT_STANDARD.value() | BaseJni.MessageOutput.OUTPUT_QUEUED.value();

		super.onCreate(savedInstanceState);

		addContentView(new MessengerView(this, true), new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 200));

		// Add debug indicator in top right corner if compiled in debug mode
		if (BaseJni.isDebug())
		{
			TextView debugTextView = new TextView(this);
			debugTextView.setText("(debug)");
			debugTextView.setTextColor(Color.BLACK);
			debugTextView.setShadowLayer(1.0f, 1.0f, 1.0f, Color.WHITE);
			debugTextView.setTextSize(TypedValue.COMPLEX_UNIT_SP, 10);

			FrameLayout.LayoutParams layoutParams = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
			layoutParams.gravity = Gravity.TOP | Gravity.END;
			layoutParams.setMargins(0, 10, 30, 0);

			addContentView(debugTextView, layoutParams);
		}

		if (allowRecording_)
		{
			recordingButton_ = new Button(this);
			recordingButton_.setText("Start Recording");
			recordingButton_.setTextSize(TypedValue.COMPLEX_UNIT_SP, 18);
			recordingButton_.setTextColor(Color.WHITE);
			recordingButton_.setBackgroundColor(0xCC00CC00);

			FrameLayout.LayoutParams buttonLayoutParams = new FrameLayout.LayoutParams(dpToPx(200), dpToPx(50));
			buttonLayoutParams.gravity = Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL;
			buttonLayoutParams.setMargins(0, 0, 0, dpToPx(80));

			recordingButton_.setOnClickListener(v -> onRecordingButtonClicked());

			addContentView(recordingButton_, buttonLayoutParams);
		}
	}

	@Override
	protected void onCameraPermissionGranted()
	{
		initializeSLAMTracker("LiveVideoId:2", "640x480");
	}

	/**
	 * Called when the recording button is clicked.
	 */
	private void onRecordingButtonClicked()
	{
		if (!isRecording_)
		{
			if (startRecording())
			{
				isRecording_ = true;
				recordingButton_.setText("Stop Recording");
				recordingButton_.setBackgroundColor(0xCCCC0000);

				Log.i("SLAMTrackerActivity", "Recording started");
			}
			else
			{
				Log.e("SLAMTrackerActivity", "Failed to start recording");
			}
		}
		else
		{
			if (stopRecording())
			{
				isRecording_ = false;
				recordingButton_.setText("Start Recording");
				recordingButton_.setBackgroundColor(0xCC00CC00);

				Log.i("SLAMTrackerActivity", "Recording stopped");
			}
			else
			{
				Log.e("SLAMTrackerActivity", "Failed to stop recording");
			}
		}
	}

	/**
	 * Helper method to convert dp to pixels.
	 * @param dp The value in dp
	 * @return The value in pixels
	 */
	private int dpToPx(int dp)
	{
		return (int)TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, dp, getResources().getDisplayMetrics());
	}

	/**
	 * Java native interface function to set or change the view's background media object.
	 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
	 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	 * @return True, if succeeded
	 */
	public static native boolean initializeSLAMTracker(String inputMedium, String resolution);

	/**
	 * Java native interface function to start recording.
	 * @return True, if succeeded
	 */
	public static native boolean startRecording();

	/**
	 * Java native interface function to stop recording.
	 * @return True, if succeeded
	 */
	public static native boolean stopRecording();
}
