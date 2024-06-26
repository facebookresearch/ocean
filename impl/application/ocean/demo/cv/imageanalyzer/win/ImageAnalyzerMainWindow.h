/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_IMAGEEANALYZER_WIN_IMAGE_ANALYZER_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_IMAGEEANALYZER_WIN_IMAGE_ANALYZER_MAIN_WINDOW_H

#include "ocean/base/Frame.h"

#include "ocean/io/File.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements the main window of the Frame Analyzer demo app.
 * @ingroup applicationdemocvimageanalyzerwin
 */
class ImageAnalyzerMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance, must be valid
		 * @param name The name of the main window
		 * @param filenames The filenames of all images to be loaded
		 */
		ImageAnalyzerMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::string>& filenames);

		/**
		 * Destructs the main window.
		 */
		~ImageAnalyzerMainWindow() override;

	protected:

		/**
		 * Event function if the windows has been initialized successfully.
		 * @see Window::onInitialized().
		 */
		void onInitialized() override;

		/**
		 * Event function to repaint the window.
		 * @see Window::onPaint().
		 */
		void onPaint() override;

		/**
		 * Function for mouse move event.
		 * @see Window::onMouseMove().
		 */
		void onMouseMove(const MouseButton buttons, const int x, const int y) override;

		/**
		 * Function for keyboard button down events.
		 * @see Window::onKeyDonw().
		 */
		void onKeyDown(const int key) override;

		/**
		 * Event functions for drag&drop events for files.
		 * @see Window::onDragAndDrop().
		 */
		void onDragAndDrop(const Files& files) override;

		/**
		 * Update the image to be analyzed.
		 * @param filename The filename of the new image, must be valid
		 */
		void updateImage(const std::string& filename);

		/**
		 * Updates the image analysis.
		 */
		void updateAnalysis();

		/**
		 * Applies gain to the image to analyze.
		 * @param yImage The image to analyze, without gain, with pixel format FORMAT_Y8, must be valid
		 * @param gain The gain to be applied, with range (0, infinity)
		 * @param yAdjustedImage The resulting image with applied gain, may not be the owner of the image data
		 */
		static void applyGain(const Frame& yImage, const float gain, Frame& yAdjustedImage);

	protected:

		/// The filenames of the images to analyze.
		IO::Files inputFiles_;

		/// The index of the current image to be analyzed, with range [0, inputFiles_.size() - 1]
		size_t currentInputIndex_ = 0;

		/// Window for horizontal pixel intensity graph.
		Platform::Win::BitmapWindow intensityGraphHorizontal_;

		/// Window for vertical pixel intensity graph.
		Platform::Win::BitmapWindow intensityGraphVertical_;

		/// The image to be analyzed.
		Frame yImage_;

		/// The adjusted image to be analyzed.
		Frame yAdjustedImage_;

		/// True, to use a black&white histogram; False, to use a colored histogram.
		bool blackWhiteVisualization_ = true;

		/// True, to analyzed the image in a zoomed area only.
		bool zoomed_ = false;

		/// The horizontal location of the coordinate, with range [0, yImage_.width() - 1], -1 if unknown.
		unsigned int analysisLocationX_ = (unsigned int)(-1);

		/// The vertical location of the coordinate, with range [0, yImage_.height() - 1], -1 if unknown.
		unsigned int analysisLocationY_ = (unsigned int)(-1);

		/// The linear gain factor to apply, with range (0, infinity)
		float linearGain_ = 1.0f;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_IMAGEEANALYZER_WIN_IMAGE_ANALYZER_MAIN_WINDOW_H
