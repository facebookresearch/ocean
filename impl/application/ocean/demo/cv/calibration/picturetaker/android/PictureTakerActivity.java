/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.cv.calibration.picturetaker.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.platform.android.application.*;

import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

/**
 * This class implements the main Activity object for the Picture Taker (Android).
 * @ingroup applicationdemocvcalibrationpicturetakerandroid
 */
public class PictureTakerActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoCVCalibrationPictureTaker");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		messageOutput_ = BaseJni.MessageOutput.OUTPUT_STANDARD.value();

		super.onCreate(savedInstanceState);

		addContentView(new MessengerView(this, true), new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 200));

		setupUI();
	}

	private void setupUI()
	{
		cameraSelectionContainer_ = new LinearLayout(this);
		cameraSelectionContainer_.setOrientation(LinearLayout.VERTICAL);
		cameraSelectionContainer_.setPadding(40, 40, 40, 40);

		GradientDrawable cameraContainerBackground = new GradientDrawable();
		cameraContainerBackground.setColor(Color.argb(220, 40, 40, 40));
		cameraContainerBackground.setCornerRadius(24);
		cameraSelectionContainer_.setBackground(cameraContainerBackground);

		FrameLayout.LayoutParams cameraContainerParams = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
		cameraContainerParams.gravity = Gravity.TOP | Gravity.CENTER_HORIZONTAL;
		cameraContainerParams.topMargin = 200;
		cameraContainerParams.leftMargin = 50;
		cameraContainerParams.rightMargin = 50;

		cameraInstructionText_ = new TextView(this);
		cameraInstructionText_.setText("Please select a camera:");
		cameraInstructionText_.setTextColor(Color.WHITE);
		cameraInstructionText_.setTextSize(18);
		cameraInstructionText_.setGravity(Gravity.CENTER);
		cameraInstructionText_.setPadding(0, 0, 0, 20);

		cameraSpinner_ = new Spinner(this);
		cameraSpinner_.setPadding(30, 20, 30, 20);

		GradientDrawable spinnerBackground = new GradientDrawable();
		spinnerBackground.setColor(Color.WHITE);
		spinnerBackground.setCornerRadius(12);
		cameraSpinner_.setBackground(spinnerBackground);

		String[] selectableCameras = availableCameras();
		String[] camerasWithPrompt = new String[selectableCameras.length + 1];
		camerasWithPrompt[0] = "Select Camera";
		System.arraycopy(selectableCameras, 0, camerasWithPrompt, 1, selectableCameras.length);

		ArrayAdapter<String> cameraAdapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, camerasWithPrompt);
		cameraAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
		cameraSpinner_.setAdapter(cameraAdapter);

		cameraSpinner_.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
		{
			@Override
			public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
			{
				if (position > 0 && !cameraSelected_)
				{
					selectedCamera_ = selectableCameras[position - 1];
					onCameraSelectedWithPermission();
				}
			}

			@Override
			public void onNothingSelected(AdapterView<?> parent)
			{
				// nothing to do here
			}
		});

		cameraSelectionContainer_.addView(cameraInstructionText_);
		cameraSelectionContainer_.addView(cameraSpinner_);

		addContentView(cameraSelectionContainer_, cameraContainerParams);

		resolutionSelectionContainer_ = new LinearLayout(this);
		resolutionSelectionContainer_.setOrientation(LinearLayout.VERTICAL);
		resolutionSelectionContainer_.setPadding(40, 40, 40, 40);
		resolutionSelectionContainer_.setVisibility(View.GONE);

		GradientDrawable resolutionContainerBackground = new GradientDrawable();
		resolutionContainerBackground.setColor(Color.argb(220, 40, 40, 40));
		resolutionContainerBackground.setCornerRadius(24);
		resolutionSelectionContainer_.setBackground(resolutionContainerBackground);

		FrameLayout.LayoutParams resolutionContainerParams = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
		resolutionContainerParams.gravity = Gravity.TOP | Gravity.CENTER_HORIZONTAL;
		resolutionContainerParams.topMargin = 520;
		resolutionContainerParams.leftMargin = 50;
		resolutionContainerParams.rightMargin = 50;

		resolutionInstructionText_ = new TextView(this);
		resolutionInstructionText_.setText("Please select a resolution:");
		resolutionInstructionText_.setTextColor(Color.WHITE);
		resolutionInstructionText_.setTextSize(18);
		resolutionInstructionText_.setGravity(Gravity.CENTER);
		resolutionInstructionText_.setPadding(0, 0, 0, 20);

		resolutionSpinner_ = new Spinner(this);
		resolutionSpinner_.setPadding(30, 20, 30, 20);

		GradientDrawable resolutionSpinnerBackground = new GradientDrawable();
		resolutionSpinnerBackground.setColor(Color.WHITE);
		resolutionSpinnerBackground.setCornerRadius(12);
		resolutionSpinner_.setBackground(resolutionSpinnerBackground);

		resolutionSpinner_.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
		{
			@Override
			public void onItemSelected(AdapterView<?> parent, View view, int position, long id)
			{
				if (position > 0 && cameraSelected_ && !cameraStarted_)
				{
					String[] resolutions = (String[]) resolutionSpinner_.getTag();
					if (resolutions != null)
					{
						selectedResolution_ = resolutions[position - 1];
						startCameraWithResolution();
					}
				}
			}

			@Override
			public void onNothingSelected(AdapterView<?> parent)
			{
				// nothing to do here
			}
		});

		resolutionSelectionContainer_.addView(resolutionInstructionText_);
		resolutionSelectionContainer_.addView(resolutionSpinner_);

		addContentView(resolutionSelectionContainer_, resolutionContainerParams);

		FrameLayout.LayoutParams buttonParams = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
		buttonParams.gravity = Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL;
		buttonParams.bottomMargin = 50;

		takeImageButton_ = new Button(this);
		takeImageButton_.setText("Take Image");
		takeImageButton_.setLayoutParams(buttonParams);
		takeImageButton_.setEnabled(false);
		takeImageButton_.setPadding(35, 18, 35, 18);
		takeImageButton_.setTextSize(14);
		takeImageButton_.setVisibility(View.GONE);

		updateButtonState();

		takeImageButton_.setOnClickListener(v -> startCountdown());
		addContentView(takeImageButton_, buttonParams);

		FrameLayout.LayoutParams countdownParams = new FrameLayout.LayoutParams(
			ViewGroup.LayoutParams.WRAP_CONTENT,
			ViewGroup.LayoutParams.WRAP_CONTENT
		);
		countdownParams.gravity = Gravity.CENTER;

		countdownTextView_ = new TextView(this);
		countdownTextView_.setTextSize(96);
		countdownTextView_.setTextColor(Color.WHITE);
		countdownTextView_.setLayoutParams(countdownParams);
		countdownTextView_.setVisibility(TextView.INVISIBLE);

		GradientDrawable countdownBackground = new GradientDrawable();
		countdownBackground.setColor(Color.argb(180, 150, 150, 150));
		countdownBackground.setCornerRadius(20);
		countdownTextView_.setBackground(countdownBackground);
		countdownTextView_.setPadding(60, 40, 60, 40);

		addContentView(countdownTextView_, countdownParams);

		focusContainer_ = new LinearLayout(this);
		focusContainer_.setOrientation(LinearLayout.VERTICAL);
		focusContainer_.setPadding(40, 30, 40, 30);
		focusContainer_.setVisibility(View.GONE);

		GradientDrawable focusContainerBackground = new GradientDrawable();
		focusContainerBackground.setColor(Color.argb(220, 40, 40, 40));
		focusContainerBackground.setCornerRadius(24);
		focusContainer_.setBackground(focusContainerBackground);

		FrameLayout.LayoutParams focusContainerParams = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT);
		focusContainerParams.gravity = Gravity.TOP | Gravity.CENTER_HORIZONTAL;
		focusContainerParams.topMargin = 50;
		focusContainerParams.leftMargin = 50;
		focusContainerParams.rightMargin = 50;

		TextView focusLabel = new TextView(this);
		focusLabel.setText("Focus: " + initialFocus_);
		focusLabel.setTextColor(Color.WHITE);
		focusLabel.setTextSize(16);
		focusLabel.setGravity(Gravity.CENTER);
		focusLabel.setPadding(0, 0, 0, 15);

		focusSeekBar_ = new SeekBar(this);
		focusSeekBar_.setMax(100);
		focusSeekBar_.setProgress((int)(100.0f * initialFocus_));
		focusSeekBar_.setPadding(20, 0, 20, 0);

		focusSeekBar_.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener()
		{
			@Override
			public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser)
			{
				float focusValue = progress / 100.0f;
				focusLabel.setText(String.format("Focus: %.2f", focusValue));
				if (fromUser)
				{
					setFocus(focusValue);
				}
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar)
			{
			}

			@Override
			public void onStopTrackingTouch(SeekBar seekBar)
			{
			}
		});

		focusContainer_.addView(focusLabel);
		focusContainer_.addView(focusSeekBar_);

		addContentView(focusContainer_, focusContainerParams);
	}

	private void updateButtonState()
	{
		GradientDrawable buttonBackground = new GradientDrawable();
		if (takeImageButton_.isEnabled())
		{
			buttonBackground.setColor(Color.rgb(33, 150, 243));
		}
		else
		{
			buttonBackground.setColor(Color.rgb(158, 158, 158));
		}
		buttonBackground.setCornerRadius(30);
		takeImageButton_.setBackground(buttonBackground);
		takeImageButton_.setTextColor(Color.WHITE);
	}

	private void onCameraSelectedWithPermission()
	{
		if (selectedCamera_ != null && checkSelfPermission(android.Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED)
		{
			onCameraSelected();
		}
	}

	private void onCameraSelected()
	{
		if (selectedCamera_ != null && !cameraSelected_)
		{
			boolean success = selectCamera(selectedCamera_);
			if (success)
			{
				cameraSelected_ = true;
				cameraSpinner_.setEnabled(false);
				cameraSpinner_.setAlpha(0.5f);
				
				GradientDrawable disabledSpinnerBackground = new GradientDrawable();
				disabledSpinnerBackground.setColor(Color.rgb(200, 200, 200));
				disabledSpinnerBackground.setCornerRadius(12);
				cameraSpinner_.setBackground(disabledSpinnerBackground);
				
				String[] resolutions = availableResolutions();
				if (resolutions != null && resolutions.length > 0)
				{
					String[] resolutionsWithPrompt = new String[resolutions.length + 1];
					resolutionsWithPrompt[0] = "Select Resolution";
					System.arraycopy(resolutions, 0, resolutionsWithPrompt, 1, resolutions.length);
					
					ArrayAdapter<String> resolutionAdapter = new ArrayAdapter<>(this, android.R.layout.simple_spinner_item, resolutionsWithPrompt);
					resolutionAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
					resolutionSpinner_.setAdapter(resolutionAdapter);
					resolutionSpinner_.setTag(resolutions);
					
					resolutionSelectionContainer_.setVisibility(View.VISIBLE);
				}
			}
		}
	}

	private void startCameraWithResolution()
	{
		if (selectedResolution_ != null && !cameraStarted_)
		{
			Log.d("Ocean", "PictureTakerActivity: Starting camera with resolution: " + selectedResolution_);
			boolean success = startCamera(selectedResolution_);
			Log.d("Ocean", "PictureTakerActivity: Camera start result: " + success);
			
			if (success)
			{
				cameraStarted_ = true;
				
				runOnUiThread(() -> {
					cameraSelectionContainer_.setVisibility(View.GONE);
					resolutionSelectionContainer_.setVisibility(View.GONE);
					
					if (setFocus(initialFocus_))
					{
						focusContainer_.setVisibility(View.VISIBLE);
					}
					
					takeImageButton_.setVisibility(View.VISIBLE);
					takeImageButton_.setEnabled(true);
					updateButtonState();
					Log.d("Ocean", "PictureTakerActivity: UI updated - button should be visible");
				});
			}
			else
			{
				Log.e("Ocean", "PictureTakerActivity: Failed to start camera!");
			}
		}
	}

	private void startCountdown()
	{
		takeImageButton_.setEnabled(false);
		updateButtonState();
		focusContainer_.setVisibility(View.GONE);
		countdownValue_ = 3;
		countdownTextView_.setVisibility(TextView.VISIBLE);
		runCountdown();
	}

	private void runCountdown()
	{
		if (countdownValue_ >= 0)
		{
			countdownTextView_.setText(String.valueOf(countdownValue_));
			handler_.postDelayed(() -> {
				countdownValue_--;
				runCountdown();
			}, 500);
		}
		else
		{
			countdownTextView_.setVisibility(TextView.INVISIBLE);
			
			takeImageButton_.setEnabled(true);
			updateButtonState();
			takePicture();
			
			vibratePhone();
		}
	}

	private void vibratePhone()
	{
		try
		{
			Vibrator vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
			if (vibrator != null && vibrator.hasVibrator())
			{
				if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
				{
					vibrator.vibrate(VibrationEffect.createOneShot(100, VibrationEffect.DEFAULT_AMPLITUDE));
				}
				else
				{
					vibrator.vibrate(100);
				}
			}
		}
		catch (Exception e)
		{
			Log.e("Ocean", "PictureTakerActivity: Failed to vibrate: " + e.getMessage());
		}
	}

	@Override
	protected void onCameraPermissionGranted()
	{
		super.onCameraPermissionGranted();
		onCameraSelected();
	}

	/**
	 * Java native interface function to start the selected camera.
	 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	 * @return True, if succeeded
	 */
	public static native boolean startCamera(String resolution);

	/**
	 * Java native interface function to take a picture.
	 */
	public static native void takePicture();

	/**
	 * Java native interface function to get available cameras.
	 * @return Array of available camera names
	 */
	public static native String[] availableCameras();

	/**
	 * Java native interface function to select a camera.
	 * @param cameraName The name of the camera to select
	 * @return True, if succeeded
	 */
	public static native boolean selectCamera(String cameraName);

	/**
	 * Java native interface function to get available resolutions for the selected camera.
	 * @return Array of available resolutions
	 */
	public static native String[] availableResolutions();

	/**
	 * Java native interface function to set the focus of the camera.
	 * @param focus The focus value between [0, 1]
	 * @return True, if succeeded
	 */
	public static native boolean setFocus(float focus);

	private Button takeImageButton_;
	private TextView countdownTextView_;
	private Spinner cameraSpinner_;
	private Spinner resolutionSpinner_;
	private TextView cameraInstructionText_;
	private TextView resolutionInstructionText_;
	private LinearLayout cameraSelectionContainer_;
	private LinearLayout resolutionSelectionContainer_;
	private LinearLayout focusContainer_;
	private SeekBar focusSeekBar_;
	private Handler handler_ = new Handler();
	private int countdownValue_ = 3;
	private String selectedCamera_ = null;
	private String selectedResolution_ = null;
	private boolean cameraSelected_ = false;
	private boolean cameraStarted_ = false;
	private final float initialFocus_ = 0.85f;
}
