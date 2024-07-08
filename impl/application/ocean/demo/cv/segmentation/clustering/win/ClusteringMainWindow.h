/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_CLUSTERING_WIN_CLUSTERING_MAINWINDOW_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_CLUSTERING_WIN_CLUSTERING_MAINWINDOW_H

#include "application/ocean/demo/cv/segmentation/clustering/win/Clustering.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/platform/win/ApplicationWindow.h"
#include "ocean/platform/win/BitmapWindow.h"

using namespace Ocean;

/**
 * This class implements a main window.
 * @ingroup applicationdemocvsegmentationclusteringwin
 */
class ClusteringMainWindow :
	virtual public Platform::Win::BitmapWindow,
	virtual public Platform::Win::ApplicationWindow
{
	public:

		/**
		 * Creates a new main window.
		 * @param instance Application instance
		 * @param name The name of the main window
		 * @param media Optional media file to be used
		 */
		ClusteringMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& media);

		/**
		 * Destructs the main window.
		 */
		~ClusteringMainWindow() override;

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
		 * Function for mouse button up events.
		 * @see Window::onMouseUp().
		 */
		void onMouseUp(const MouseButton button, const int x, const int y) override;

		/**
		 * Event function if a new frame has arrived.
		 * @param frame New frame
		 */
		void onFrame(const Frame& frame);

		/**
		 * Determines the clustering for a given frame by a random algorithm.
		 * @param frame The frame to determine the clustering for
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		void determineRandomClustering(const Frame& frame);

		/**
		 * Determines the clustering for a given frame by a k-means approach (with indices).
		 * @param frame The frame to determine the clustering for
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		void determineKMeansClusteringIndices(const Frame& frame);

		/**
		 * Determines the clustering for a given frame by a k-means approach (with pointers).
		 * @param frame The frame to determine the clustering for
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		void determineKMeansClusteringPointers(const Frame& frame);

	protected:

		/// Random clustering window.
		Platform::Win::BitmapWindow randomWindow_;

		/// K-means clustering window (with indices).
		Platform::Win::BitmapWindow kMeansWindowIndices_;

		/// K-means clustering window (with pointers).
		Platform::Win::BitmapWindow kMeansWindowPointers_;

		/// Media object.
		Media::FrameMediumRef frameMedium_;

		/// Most recent frame timestamp.
		Timestamp frameTimestamp_;

		/// Worker object.
		Worker worker_;

		/// Random generator.
		RandomGenerator randomGenerator_;

		/// Optional media file to be used.
		std::string mediaFile_;
};

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_CV_SEGMENTATION_CONTOUR_FINDER_WIN_CLUSTERING_MAINWINDOW_H
