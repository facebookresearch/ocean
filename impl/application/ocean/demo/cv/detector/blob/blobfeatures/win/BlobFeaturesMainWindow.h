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

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvdetectorblobblobfeatureswin
 */
class BlobFeaturesMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name name of the main window
		 * @param inputMediaName The media used as input source
		 * @param inputResolution Optional preferred resolution of the input source
		 */
		BlobFeaturesMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& inputMediaName = std::string(), const std::string& inputResolution = std::string());

		/**
		 * Destructs the main window.
		 */
		virtual ~BlobFeaturesMainWindow();

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
		 * Function for keyboard button down events.
		 * @see Window::onKeyDonw().
		 */
		void onKeyDown(const int key) override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

		/**
		 * Paints the blob features into a given frame.
		 * @param frame The frame in which the features will be painted, must have a pixel format FORMAT_RGB24
		 * @param features The features to be painted
		 */
		static void paintBlobFeatures(Frame& frame, const CV::Detector::Blob::BlobFeatures& features);

	protected:

		/// Input medium.
		Media::FrameMediumRef inputMedium_;

		/// Timestamp of the most recent frame of the input medium.
		Timestamp frameTimestamp_;

		/// Name of the media used as input.
		std::string inputMediaName_;

		/// The preferred resolution of the input media.
		std::string inputResolution_;

		/// Feature threshold.
		Scalar blobFeatureThreshold_;

		/// State determining whether the frame timestamp will be ignored.
		bool ignoreTimestamp_;

		/// State determining whether orientation and descriptor will be calculated.
		bool calculateOrientationAndDescriptor_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_BLOB_WIN_BLOBFEATURES_MAINWINDOW_H
