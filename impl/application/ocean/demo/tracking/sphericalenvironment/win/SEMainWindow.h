/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_SPHERICAL_ENVIRONMENT_WIN_SE_MAIN_WINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_SPHERICAL_ENVIRONMENT_WIN_SE_MAIN_WINDOW_H

#include "ocean/base/Frame.h"
#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/media/MovieFrameProviderInterface.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

#include "ocean/tracking/SphericalEnvironment.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemotrackingsphericalenvironmentwin
 */
class SEMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param file Optional media file
		 */
		SEMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file = std::string());

		/**
		 * Destructs the main window.
		 */
		~SEMainWindow() override;

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
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrameOptimizedCamera(const Frame& frame);

		/**
		 * Optimizes the camera profile.
		 */
		void optimizeCameraProfile();

	protected:

		/// The spherical environment object without fine adjustment.
		Tracking::SphericalEnvironment sphericalEnvironment_;

		/// The spherical environment object with fine adjustment
		Tracking::SphericalEnvironment sphericalEnvironmentFineAdjustment_;

		/// The spherical environment object with optimized camera without fine adjustment.
		Tracking::SphericalEnvironment sphericalEnvironmentOptimizedCamera_;

		/// The spherical environment object with optimized camera with fine adjustment.
		Tracking::SphericalEnvironment sphericalEnvironmentOptimizedCameraFineAdjustment_;

		/// The camera profile.
		PinholeCamera camera_;

		/// The orientations of the individual camera frames for the spherical environment without fine adjustment.
		SquareMatrices3 orientations_;

		/// The frame provider interface.
		CV::FrameProviderInterfaceRef frameProviderInterface_;

		/// The current frame index.
		unsigned int frameIndex_ = 0u;

		/// The entire frame number.
		unsigned int frameNumber_ = 0u;

		/// Optional media file to be used.
		std::string mediaFile_;

		/// The panorama frame window without fine adjustment.
		Platform::Win::BitmapWindow panoramaFrameWindow;

		/// The panorama frame window with fine adjustment.
		Platform::Win::BitmapWindow panoramaFrameWindowFineAdjustment;

		/// The panorama frame window with optimized camera without fine adjustment.
		Platform::Win::BitmapWindow panoramaFrameWindowOptimizedCamera;

		/// The panorama frame window with optimized camera with fine adjustment.
		Platform::Win::BitmapWindow panoramaFrameWindowOptimizedCameraFineAdjustment;

		/// Measures the performance of the panorama frame without fine adjustment.
		HighPerformanceStatistic performance_;

		/// Measures the performance of the panorama frame with fine adjustment.
		HighPerformanceStatistic performanceFineAdjustment_;

		/// Measures the performance of the panorama frame with optimized camera without fine adjustment.
		HighPerformanceStatistic performanceOptimizedCamera_;

		/// Measures the performance of the panorama frame with optimized camera with fine adjustment.
		HighPerformanceStatistic performanceOptimizedCameraFineAdjustment_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_SPHERICAL_ENVIRONMENT_WIN_SE_MAIN_WINDOW_H
