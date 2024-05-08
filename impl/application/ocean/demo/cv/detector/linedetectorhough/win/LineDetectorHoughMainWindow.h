/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINE_DETECTOR_WIN_LINE_DETECTOR_HOUGH_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINE_DETECTOR_WIN_LINE_DETECTOR_HOUGH_MAIN_WINDOW_H

#include "application/ocean/demo/cv/detector/linedetectorhough/win/LineDetectorHough.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/detector/LineDetectorHough.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvdetectorlinedetectorhoughwin
 */
class LineDetectorHoughMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	protected:

		/**
		 * Definition of individual detector modes.
		 */
		enum DetectorMode
		{
			/// An 24 bit frame is used for line detection, otherwise a 8 bit frame is used for line detection.
			DM_FRAME_24BIT = (1 << 0),

			/// The Scharr filter is used as filter type, otherwise the Sobel filter.
			DM_SCHARR = (1 << 1),

			/// Horizontal, vertical and diagonal filter responses are used, otherwise horizontal and vertical filter responses are used.
			DM_HORIZONTAL_VERTICAL_DIAGONAL = (1 << 2),

			/// Finite lines are detected, otherwise infinite lines are detected.
			DM_FINITE_LINES = (1 << 3),

			/// The orientation and location of lines are optimized, otherwise lines will no be optimized.
			DM_OPTIMIZED_LINES = (1 << 4),

			/// Lines are detected by application by an adaptive threshold, otherwise lines are detected by a simple threshold.
			DM_ADAPTIVE_THRESHOLD = (1 << 5),

			/// The peak maximum is used for each line, otherwise the exact peak maxiumum is determined with sub-bin accuracy.
			DM_BIN_ACCURATE_PEAK = (1 << 6),

			/// The last (invalid) mode.
			DM_INVALID = (1 << 6)
		};

	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param filename Optional name of the media file to be used
		 */
		LineDetectorHoughMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& filename = std::string());

		/**
		 * Destructs the main window.
		 */
		~LineDetectorHoughMainWindow() override;

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
		 * Paints the given infinite lines into a given frame.
		 * @param frame The frame in which the lines will be painted, must be valid
		 * @param lines The lines to be painted
		 * @param strengths The strengths values of the lines, one value for each line
		 * @param worker Optional worker to distribute the computation
		 */
		static void paintLines(Frame& frame, const Lines2& lines, const Scalars& strengths, Worker* worker = nullptr);

		/**
		 * Paints the given infinite lines into a given frame.
		 * @param frame The frame in which the lines will be painted, must be valid
		 * @param lines The lines to be painted
		 * @param worker Optional worker to distribute the computation
		 */
		static void paintLines(Frame& frame, const FiniteLines2& lines, Worker* worker = nullptr);

	protected:

		/**
		 * Paints a subset of the given infinite lines into a given frame.
		 * @param frame The frame in which the lines will be pained, must be valid
		 * @param lines The lines to be painted
		 * @param strengths The strengths values of the lines, one value for each line
		 * @param minStrength The minimal strength value of all lines
		 * @param maxStrength The maximal strength value of all lines
		 * @param firstLine The first line to be handled
		 * @param numberLines The number of lines to be handled
		 */
		static void paintLines(Frame* frame, const Line2* lines, const Scalar* strengths, const Scalar minStrength, const Scalar maxStrength, const unsigned int firstLine, const unsigned int numberLines);

		/**
		 * Paints a subset of the given infinite lines into a given frame.
		 * @param frame The frame in which the lines will be pained, must be valid
		 * @param lines The lines to be painted
		 * @param firstLine The first line to be handled
		 * @param numberLines The number of lines to be handled
		 */
		static void paintLines(Frame* frame, const FiniteLine2* lines, const unsigned int firstLine, const unsigned int numberLines);

		/**
		 * Translates the application mode.
		 * @param mode The application mode to be translated
		 * @return The resulting translated application mode
		 */
		static std::string translateApplicationMode(const unsigned int mode);

	protected:

		/// The current mode of the application, which is a bitset of possible detector modes.
		unsigned int mode_ = 0u;

		/// The frame medium object providing the visual content.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_ = Timestamp(false);

		/// Worker object.
		Worker worker_;

		/// Optional media file to be used.
		std::string mediaFilename_;

		/// State determining whether the frame timestamp will be ignored.
		bool ignoreTimestamp_ = false;

		/// The strength threshold value for the line detector, with range (0, infinity)
		Scalar strengthThreshold_ = Scalar(8);
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_DETECTOR_LINE_DETECTOR_WIN_LINE_DETECTOR_HOUGH_MAIN_WINDOW_H
