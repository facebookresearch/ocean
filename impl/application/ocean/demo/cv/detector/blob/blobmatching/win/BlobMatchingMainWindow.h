/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BLOB_WIN_BLOBFEATURES_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BLOB_WIN_BLOBFEATURES_MAINWINDOW_H

#include "ocean/base/Timestamp.h"

#include "ocean/cv/detector/blob/BlobFeatureDescriptor.h"
#include "ocean/cv/detector/blob/BlobFeatureDetector.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/Bitmap.h"
#include "ocean/platform/win/BitmapWindow.h"

#include "ocean/tracking/blob/FeatureMap.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvdetectorblobblobmatchinngwin
 */
class BlobMatchingMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param patternMediaName The media containing the tracking pattern
		 * @param inputMediaName The media used as input source
		 * @param inputResolution Optional preferred resolution of the input source
		 */
		BlobMatchingMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& patternMediaName = std::string(), const std::string& inputMediaName = std::string(), const std::string& inputResolution = std::string());

		/**
		 * Destructs the main window.
		 */
		~BlobMatchingMainWindow() override;

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Function called by the windows message loop if the process is idling.
		 * @see Window::onIdle().
		 */
		void onIdle() override;

		/**
		 * Function for mouse double click events.
		 * @see Window::onMouseDoubleClick().
		 */
		void onMouseDoubleClick(const MouseButton button, const int x, const int y) override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

		/**
		 * Paints the blob features into a given frame.
		 * @param frame The frame in which the features will be painted, must have a pixel format FORMAT_RGB24
		 * @param features The features to be painted
		 * @param shadow True, to paint the feature points with a shadow; False, to paint the feature points without shadow
		 */
		static void paintBlobFeatures(Frame& frame, const CV::Detector::Blob::BlobFeatures& features, const bool shadow);

	protected:

		/// Input medium.
		Media::FrameMediumRef inputMedium_;

		/// Timestamp of the most recent frame of the input medium.
		Timestamp frameTimestamp_;

		/// Frame containing the pattern.
		Frame patternFrame_;

		/// Feature map of the pattern.
		Tracking::Blob::FeatureMap featureMap_;

		/// Name of the media containing the pattern.
		std::string patternMediaName_;

		/// Name of the media used as input.
		std::string inputMediaName_;

		/// The preferred resolution of the input media.
		std::string inputResolution_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BLOB_WIN_BLOBFEATURES_MAINWINDOW_H
