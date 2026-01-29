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
import android.view.KeyEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.Window;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

/**
 * This class implements the main Activity object for the Picture Taker (Android).
 * By default, the app allows to select which camera to be used through a UI.<br>
 * In addition, the user can configure the camera selection through an intent, the app can be started and configured via:<br>
 * <pre>
 * adb shell am start -n com.meta.ocean.app.demo.cv.calibration.picturetaker.android/.PictureTakerActivity --es camera_name '"Back-facing Camera 0"' --es resolution '"1920x1080"' --ef focus 0.75 --ez video_stabilization false
 * </pre>
 * In case the app is started and configured via an intent, any key event is used to trigger the button to take an image.<br>
 * A key event can be triggered via: 'adb shell input keyevent KEYCODE_ENTER'
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

		readIntentExtras();

		setupUI();

		if (autoStartEnabled_)
		{
			Log.d("Ocean", "PictureTakerActivity: Auto-start mode enabled");
			Log.d("Ocean", "PictureTakerActivity: Camera: " + preConfiguredCamera_);
			Log.d("Ocean", "PictureTakerActivity: Resolution: " + preConfiguredResolution_);
			Log.d("Ocean", "PictureTakerActivity: Focus: " + preConfiguredFocus_);
			Log.d("Ocean", "PictureTakerActivity: Video Stabilization: " + preConfiguredVideoStabilization_);

			if (checkSelfPermission(android.Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED)
			{
				startCameraWithPreConfiguration();
			}
		}
	}

	private void readIntentExtras()
	{
		Log.d("Ocean", "PictureTakerActivity: readIntentExtras() called");

		if (getIntent() != null)
		{
			Log.d("Ocean", "PictureTakerActivity: Intent is not null");
			Bundle extras = getIntent().getExtras();

			if (extras != null)
			{
				Log.d("Ocean", "PictureTakerActivity: Extras bundle is not null");
				Log.d("Ocean", "PictureTakerActivity: Extras contents: " + extras.toString());

				if (extras.containsKey(EXTRA_CAMERA_NAME))
				{
					preConfiguredCamera_ = extras.getString(EXTRA_CAMERA_NAME);
					Log.d("Ocean", "PictureTakerActivity: Found camera_name: " + preConfiguredCamera_);
				}
				else
				{
					Log.d("Ocean", "PictureTakerActivity: No camera_name in extras");
				}

				if (extras.containsKey(EXTRA_RESOLUTION))
				{
					preConfiguredResolution_ = extras.getString(EXTRA_RESOLUTION);
					Log.d("Ocean", "PictureTakerActivity: Found resolution: " + preConfiguredResolution_);
				}
				else
				{
					Log.d("Ocean", "PictureTakerActivity: No resolution in extras");
				}

				if (extras.containsKey(EXTRA_FOCUS))
				{
					preConfiguredFocus_ = extras.getFloat(EXTRA_FOCUS, initialFocus_);
					Log.d("Ocean", "PictureTakerActivity: Found focus: " + preConfiguredFocus_);
				}

				if (extras.containsKey(EXTRA_VIDEO_STABILIZATION))
				{
					preConfiguredVideoStabilization_ = extras.getBoolean(EXTRA_VIDEO_STABILIZATION, false);
					Log.d("Ocean", "PictureTakerActivity: Found video_stabilization: " + preConfiguredVideoStabilization_);
				}

				// Enable auto-start only if all required settings are provided
				autoStartEnabled_ = preConfiguredCamera_ != null &&
				                     preConfiguredResolution_ != null;

				Log.d("Ocean", "PictureTakerActivity: autoStartEnabled_ = " + autoStartEnabled_);
			}
			else
			{
				Log.d("Ocean", "PictureTakerActivity: Extras bundle is null");
			}
		}
		else
		{
			Log.d("Ocean", "PictureTakerActivity: Intent is null");
		}
	}

	private void setupUI()
	{
		cameraSelectionContainer_ = new LinearLayout(this);
		cameraSelectionContainer_.setOrientation(LinearLayout.VERTICAL);
		cameraSelectionContainer_.setPadding(40, 40, 40, 40);

		if (autoStartEnabled_)
		{
			cameraSelectionContainer_.setVisibility(View.GONE);
		}

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

		FrameLayout.LayoutParams countdownParams = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
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

		FrameLayout.LayoutParams imageCounterParams = new FrameLayout.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT);
		imageCounterParams.gravity = Gravity.BOTTOM | Gravity.RIGHT;
		imageCounterParams.bottomMargin = 60;
		imageCounterParams.rightMargin = 20;

		imageCounterTextView_ = new TextView(this);
		imageCounterTextView_.setTextSize(24);
		imageCounterTextView_.setTextColor(Color.WHITE);
		imageCounterTextView_.setLayoutParams(imageCounterParams);
		imageCounterTextView_.setVisibility(TextView.INVISIBLE);

		addContentView(imageCounterTextView_, imageCounterParams);

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
				final float focusValue = progress / 100.0f;

				focusLabel.setText(String.format("Focus: %.2f", focusValue));

				if (fromUser)
				{
					setFocus(focusValue);
				}
			}

			@Override
			public void onStartTrackingTouch(SeekBar seekBar)
			{
				// nothing to do here
			}

			@Override
			public void onStopTrackingTouch(SeekBar seekBar)
			{
				// nothing to do here
			}
		});

		focusContainer_.addView(focusLabel);
		focusContainer_.addView(focusSeekBar_);

		stabilizationCheckBox_ = new CheckBox(this);
		stabilizationCheckBox_.setText("Video Stabilization");
		stabilizationCheckBox_.setTextColor(Color.WHITE);
		stabilizationCheckBox_.setTextSize(16);
		stabilizationCheckBox_.setPadding(20, 10, 20, 0);
		stabilizationCheckBox_.setChecked(false);

		stabilizationCheckBox_.setOnCheckedChangeListener((buttonView, isChecked) -> {
				setVideoStabilization(isChecked);
			});

		focusContainer_.addView(stabilizationCheckBox_);

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

				stabilizationCheckBox_.setChecked(videoStabilization());

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

			if (takePicture())
			{
				imageCounter_++;
				imageCounterTextView_.setText(String.valueOf(imageCounter_));
				imageCounterTextView_.setVisibility(TextView.VISIBLE);

				vibratePhone();
			}
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
		catch (Exception exception)
		{
			Log.e("Ocean", "PictureTakerActivity: Failed to vibrate: " + exception.getMessage());
		}
	}

	private void startCameraWithPreConfiguration()
	{
		selectedCamera_ = preConfiguredCamera_;
		selectedResolution_ = preConfiguredResolution_;

		Log.d("Ocean", "PictureTakerActivity: Starting camera with pre-configuration");

		boolean cameraSelectSuccess = selectCamera(selectedCamera_);
		if (!cameraSelectSuccess)
		{
			Log.e("Ocean", "PictureTakerActivity: Failed to select camera: " + selectedCamera_);
			return;
		}

		cameraSelected_ = true;

		Log.d("Ocean", "PictureTakerActivity: Starting camera with resolution: " + selectedResolution_);
		boolean cameraStartSuccess = startCamera(selectedResolution_);
		Log.d("Ocean", "PictureTakerActivity: Camera start result: " + cameraStartSuccess);

		if (!cameraStartSuccess)
		{
			Log.e("Ocean", "PictureTakerActivity: Failed to start camera!");
			return;
		}

		cameraStarted_ = true;

		float focusValue = preConfiguredFocus_ != null ? preConfiguredFocus_ : initialFocus_;
		boolean focusSuccess = setFocus(focusValue);
		Log.d("Ocean", "PictureTakerActivity: Set focus to " + focusValue + ": " + focusSuccess);

		if (preConfiguredVideoStabilization_ != null)
		{
			boolean stabilizationSuccess = setVideoStabilization(preConfiguredVideoStabilization_);
			Log.d("Ocean", "PictureTakerActivity: Set video stabilization to " + preConfiguredVideoStabilization_ + ": " + stabilizationSuccess);
		}

		runOnUiThread(() -> {
			cameraSelectionContainer_.setVisibility(View.GONE);
			resolutionSelectionContainer_.setVisibility(View.GONE);

			if (focusSuccess)
			{
				focusSeekBar_.setProgress((int)(100.0f * focusValue));
				focusContainer_.setVisibility(View.VISIBLE);
			}

			if (preConfiguredVideoStabilization_ != null)
			{
				stabilizationCheckBox_.setChecked(preConfiguredVideoStabilization_);
			}
			else
			{
				stabilizationCheckBox_.setChecked(videoStabilization());
			}

			takeImageButton_.setVisibility(View.VISIBLE);
			takeImageButton_.setEnabled(true);
			updateButtonState();
			Log.d("Ocean", "PictureTakerActivity: Auto-start complete - ready to take pictures");
		});
	}

	@Override
	protected void onCameraPermissionGranted()
	{
		super.onCameraPermissionGranted();

		if (autoStartEnabled_)
		{
			startCameraWithPreConfiguration();
		}
		else
		{
			onCameraSelected();
		}
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event)
	{
		if (autoStartEnabled_)
		{
			String keyName = KeyEvent.keyCodeToString(keyCode);

			Log.d("Ocean", "PictureTakerActivity: Key DOWN, Code " + keyCode + " (" + keyName + ")");

			if (takeImageButton_ != null && takeImageButton_.isEnabled() && takeImageButton_.getVisibility() == View.VISIBLE)
			{
				Log.d("Ocean", "PictureTakerActivity: Triggering take picture via key press");

				runOnUiThread(() -> takeImageButton_.performClick());
				return true;
			}
		}

		return super.onKeyDown(keyCode, event);
	}

	@Override
	public boolean onKeyUp(int keyCode, KeyEvent event)
	{
		if (autoStartEnabled_)
		{
			String keyName = KeyEvent.keyCodeToString(keyCode);
			Log.d("Ocean", "PictureTakerActivity: Key UP, Code " + keyCode + " (" + keyName + ")");
		}

		return super.onKeyUp(keyCode, event);
	}

	/**
	 * Java native interface function to start the selected camera.
	 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	 * @return True, if succeeded
	 */
	public static native boolean startCamera(String resolution);

	/**
	 * Java native interface function to take a picture.
	 * @return True, if succeeded
	 */
	public static native boolean takePicture();

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

	/**
	 * Java native interface function to set video stabilization.
	 * @param enabled True to enable stabilization, false to disable
	 * @return True, if succeeded
	 */
	public static native boolean setVideoStabilization(boolean enabled);

	/**
	 * Java native interface function to get current video stabilization status.
	 * @return True if stabilization is enabled
	 */
	public static native boolean videoStabilization();

	private Button takeImageButton_;
	private TextView countdownTextView_;
	private TextView imageCounterTextView_;
	private Spinner cameraSpinner_;
	private Spinner resolutionSpinner_;
	private TextView cameraInstructionText_;
	private TextView resolutionInstructionText_;
	private LinearLayout cameraSelectionContainer_;
	private LinearLayout resolutionSelectionContainer_;
	private LinearLayout focusContainer_;
	private SeekBar focusSeekBar_;
	private CheckBox stabilizationCheckBox_;
	private Handler handler_ = new Handler();
	private int countdownValue_ = 3;
	private int imageCounter_ = 0;
	private String selectedCamera_ = null;
	private String selectedResolution_ = null;
	private boolean cameraSelected_ = false;
	private boolean cameraStarted_ = false;
	private final float initialFocus_ = 0.85f;

	private boolean autoStartEnabled_ = false;
	private String preConfiguredCamera_ = null;
	private String preConfiguredResolution_ = null;
	private Float preConfiguredFocus_ = null;
	private Boolean preConfiguredVideoStabilization_ = null;

	public static final String EXTRA_CAMERA_NAME = "camera_name";
	public static final String EXTRA_RESOLUTION = "resolution";
	public static final String EXTRA_FOCUS = "focus";
	public static final String EXTRA_VIDEO_STABILIZATION = "video_stabilization";
}
