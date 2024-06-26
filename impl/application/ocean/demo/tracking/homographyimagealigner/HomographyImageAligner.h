/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYIMAGEALIGNER_HOMOGRAPHY_IMAGE_ALIGNER_H
#define FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYIMAGEALIGNER_HOMOGRAPHY_IMAGE_ALIGNER_H

#include "application/ocean/demo/tracking/ApplicationDemoTracking.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Timestamp.h"

#include "ocean/cv/FramePyramid.h"

#include "ocean/media/FrameMedium.h"

/**
 * @ingroup applicationdemotracking
 * @defgroup applicationdemotrackinghomographyimagealigner Homography Image Aligner
 * @{
 * The demo application shows how successive video frames (e.g., from a live video/webcam) can be aligned w.r.t. a homography.<br>
 * The user has the possibility to define the video input source (with preferred frame dimension) via the commando line parameter.<br>
 * The application will blend the current video frame with the previous video frame (while the previous frame is transformed by application of the determined homography).<br>
 * The implementation of the basic image aligner functionality is platform independent, thus is can be used on any platform.
 * @}
 */

using namespace Ocean;

/**
 * This class implements the platform independent homography image aligner functionality which will be used/shared by/across platform specific applications.
 * Beware: You must not have more than one aligner object within one application.
 * @ingroup applicationdemotrackinghomographyimagealigner
 */
class HomographyImageAligner
{
	public:

		/**
		 * Creates an invalid aligner object.
		 */
		HomographyImageAligner() = default;

		/**
		 * Move constructor.
		 * @param homographyImageAligner The aligner object to be moved
		 */
		inline HomographyImageAligner(HomographyImageAligner&& homographyImageAligner);

		/**
		 * Creates a new aligner object by a given set of command arguments.
		 * @param commandArguments The command arguments that configure the properties of the aligner
		 */
		explicit HomographyImageAligner(const std::vector<std::wstring>& commandArguments);

		/**
		 * Destructs an aligner object.
		 */
		~HomographyImageAligner();

		/**
		 * Explicitly releases this aligner object.
		 */
		void release();

		/**
		 * Checks if the medium holds a new frame and if so applies the alignment for the frame.
		 * Additionally, this function can return all point correspondences that have been determined between both frames,<br>
		 * or only those point correspondences that have been verified by a homography-based RANSAC.<br>
		 * Beware: Provide a valid 'validPointIndices' parameter and you will receive any point correspondences (stored in 'previousPoints' and 'currentPoints').<br>
		 * Instead, setting the 'validPointIndices' parameter to nullptr will return RANSAC-verified point correspondences only (stored in 'previousPoints' and 'currentPoints').<br>
		 * Ensure that 'previousPoints' and 'currentPoints' are defined if 'validPointIndices' is defined.
		 * @param frame The resulting frame showing the blending between the current video frame and the previous frame, may be invalid if the tracker fails
		 * @param time The time the aligner needed to process the frame, averaged over 20 iterations, in seconds, with range [0, infinity)
		 * @param currentHomographyPrevious Optional resulting homography that has been determined, transforming points defined in the previous frame to points defined in the current frame (currentPoint = currentHomographyPrevious * previousPoint), nullptr if not of interest
		 * @param previousPoints Optional resulting image points located in the previous frame that have been either tracked and/or used for homography determination, the behavior depends whether 'validPointIndices' is defined or not, nullptr if not of interest
		 * @param currentPoints Optional resulting image points located in the current frame that have been either tracked and/or used for homograph determination, one for each point in the previous frame, nullptr if not of interest
		 * @param validPointIndices Optional resulting indices of all point correspondences that have been verified by a homography-based RANSAC, nullptr if not of interest
		 * @param lastFrameReached Optional resulting state whether the last frame (of the input medium) has been reached, nullptr if not of interest
		 * @return True, if a new frame was available
     * @see Subset::subset(), Subset::indices2statements().
		 */
		bool alignNewFrame(Frame& frame, double& time, SquareMatrix3* currentHomographyPrevious = nullptr, Vectors2* previousPoints = nullptr, Vectors2* currentPoints = nullptr, Indices32* validPointIndices = nullptr, bool* lastFrameReached = nullptr);

		/**
		 * Returns the frame medium providing the visual information for the aligner.
		 * @return The aligners's frame medium
		 */
		inline const Media::FrameMediumRef frameMedium() const;

		/**
		 * Move operator.
		 * @param homographyImageAligner The aligner object to be moved
		 * @return Reference to this object
		 */
		HomographyImageAligner& operator=(HomographyImageAligner&& homographyImageAligner);

	protected:

		/**
		 * Disabled copy constructor.
		 */
		HomographyImageAligner(HomographyImageAligner&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		HomographyImageAligner& operator=(HomographyImageAligner&) = delete;

	protected:

		/// The frame medium providing the visual information for this aligner.
		Media::FrameMediumRef frameMedium_;

		/// The number of feature points to be used during the alignment, with range [10, infinity)
		unsigned int trackingNumberFeaturePoints_ = 150u;

		/// The size of the image patches to be applied during tracking in pixel, possible values {5, 7, 15, 31}.
		unsigned int trackingPatchSize_ = 15u;

		/// The number of sub-pixel iterations, providing a tracking precision of 0.5 ^ iterations, with range [0, infinity).
		unsigned int trackingSubPixelIterations_ = 4u;

		/// The maximal offset of corresponding feature points between two successive frames in pixels, with range [2u, infinity).
		unsigned int trackingMaximalOffset_ = 128u;

		/// The search radius on the coarsest pyramid layer in pixel (all other layers use a radius of two), with range [2, infinity).
		unsigned int trackingCoarsestLayerRadius_ = 4u;

		/// RANSAC outlier radius
		Scalar pixelErrorThreshold_ = Scalar(3.0);

		/// The pixel format to be used for sparse point tracking.
		FrameType::PixelFormat trackingPixelFormat_ = FrameType::FORMAT_RGB24;

		/// True, so that the sparse point tracker apply a mean extraction before tracking; False, to avoid the mean extraction.
		bool trackingZeroMean_ = true;

		/// The frame pyramid of the current frame, may have a different pixel format than RGB24.
		CV::FramePyramid currentFramePyramid_;

		/// The frame pyramid of the previous frame, with same pixel format as the current frame pyramid.
		CV::FramePyramid previousFramePyramid_;

		/// The RGB24 frame of the previous frame.
		Frame rgbPreviousFrame_;

		/// The timestamp of the last frame that has been handled.
		Timestamp frameTimestamp_;

		/// The performance measurement object.
		HighPerformanceStatistic performance_;
};

inline const Media::FrameMediumRef HomographyImageAligner::frameMedium() const
{
	return frameMedium_;
}

inline HomographyImageAligner::HomographyImageAligner(HomographyImageAligner&& homographyImageAligner)
{
	*this = std::move(homographyImageAligner);
}

#endif // FACEBOOK_APPLICATION_OCEAN_DEMO_TRACKING_HOMOGRAPHYIMAGEALIGNER_HOMOGRAPHY_IMAGE_ALIGNER_H
