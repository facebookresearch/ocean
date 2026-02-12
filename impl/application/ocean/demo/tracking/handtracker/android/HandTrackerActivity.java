/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.tracking.handtracker.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.platform.android.application.*;

import com.google.mediapipe.framework.image.BitmapImageBuilder;
import com.google.mediapipe.framework.image.MPImage;
import com.google.mediapipe.tasks.components.containers.Category;
import com.google.mediapipe.tasks.components.containers.NormalizedLandmark;
import com.google.mediapipe.tasks.core.BaseOptions;
import com.google.mediapipe.tasks.core.Delegate;
import com.google.mediapipe.tasks.vision.core.RunningMode;
import com.google.mediapipe.tasks.vision.gesturerecognizer.GestureRecognizer;
import com.google.mediapipe.tasks.vision.gesturerecognizer.GestureRecognizerResult;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;
import android.view.ViewGroup;

import java.util.List;

/**
 * This class implements the main Activity object for the Hand Tracker (Android).
 * @ingroup applicationdemotrackinghandtrackerandroid
 */
public class HandTrackerActivity extends GLFrameViewActivity
{
	static
	{
		System.loadLibrary("OceanDemoTrackingHandTracker");
	}

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		this_ = this;

		super.onCreate(savedInstanceState);

		addContentView(new MessengerView(this, true), new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, 200));

		setupRecognitionPipeline();
	}

	@Override
	protected void onCameraPermissionGranted()
	{
		initializeHandTracker("LiveVideoId:0", "1280x720");
	}

	protected boolean setupRecognitionPipeline()
	{
		try
		{
			BaseOptions.Builder baseOptionsBuilder = BaseOptions.builder();
			baseOptionsBuilder.setDelegate(Delegate.GPU);
			baseOptionsBuilder.setModelAssetPath("gesture_recognizer.task");

			BaseOptions baseOptions = baseOptionsBuilder.build();

			GestureRecognizer.GestureRecognizerOptions.Builder recognizerOptionsBuilder = GestureRecognizer.GestureRecognizerOptions.builder();
			recognizerOptionsBuilder.setBaseOptions(baseOptions);
			recognizerOptionsBuilder.setMinHandDetectionConfidence(0.5f);
			recognizerOptionsBuilder.setMinTrackingConfidence(0.5f);
			recognizerOptionsBuilder.setMinHandPresenceConfidence(0.5f);
			recognizerOptionsBuilder.setRunningMode(RunningMode.IMAGE);

			GestureRecognizer.GestureRecognizerOptions options = recognizerOptionsBuilder.build();

			gestureRecognizer_ = GestureRecognizer.createFromOptions(this, options);
		}
		catch (Exception exception)
		{
			Log.e("Ocean", "Failed to create gesture recognizer: " + exception.getMessage());
			Log.i("Ocean", "Does the file gesture_recognizer.task exist in the assets folder?");

			return false;
		}

		return true;
	}

	public static String processImage(Bitmap bitmap)
	{
		String result = "";

		if (this_ == null || this_.gestureRecognizer_ == null)
		{
			Log.e("Ocean", "Failed to process image: Gesture recognizer is not initialized.");
			return "Error";
		}

		BitmapImageBuilder bitmapImageBuilder = new BitmapImageBuilder(bitmap);

		MPImage mpImage = bitmapImageBuilder.build();

		try
		{
			GestureRecognizerResult gestureRecognizerResult = this_.gestureRecognizer_.recognize(mpImage);

			List<List<NormalizedLandmark>> landmarks = gestureRecognizerResult.landmarks();

			Log.d("Ocean", "Number of hands: " + landmarks.size());

			for (List<NormalizedLandmark> hand : landmarks)
			{
				Log.d("Ocean", "Hand with " + hand.size() + " landmarks");
			}

			List<List<Category>> categories = gestureRecognizerResult.gestures();

			Log.d("Ocean", "Number of gestures: " + categories.size());

			for (List<Category> category : categories)
			{
				for (Category c : category)
				{
					Log.d("Ocean", "Category: " + c.categoryName() + " - Score: " + c.score());

					if (!c.categoryName().equals("None") && result.isEmpty())
					{
						result = c.categoryName();
					}
				}
			}
		}
		catch (Exception exception)
		{
			Log.e("Ocean", "Failed to process image: " + exception.getMessage());
			return "Error";
		}

		return result;
	}

	/**
	 * Java native interface function to set or change the view's background media object.
	 * @param inputMedium The URL of the input medium (e.g., "LiveVideoId:0")
	 * @param resolution The resolution of the input medium (e.g., "640x480", "1280x720", "1920x1080")
	 * @return True, if succeeded
	 */
	public static native boolean initializeHandTracker(String inputMedium, String resolution);

	/// The gesture recognizer to be used.
	protected GestureRecognizer gestureRecognizer_;

	/// The activity object.
	protected static HandTrackerActivity this_ = null;
}
