/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_HOMOGRAPHY_IMAGE_ALIGNMENT_SPARSE_H
#define META_OCEAN_TRACKING_HOMOGRAPHY_IMAGE_ALIGNMENT_SPARSE_H

#include "ocean/tracking/Tracking.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/SubRegion.h"

#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements image alignment functions internally using sparse image information like e.g., feature points.
 * @see HomographyTracker.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT HomographyImageAlignmentSparse
{
	public:

		/**
		 * Determines the global homography between two successive frames by applying a tracking of sparse feature points.
		 * The function tries to track sparse feature points scattered over the entire image (or sub-region) by detecting even quite weak feature points.<br>
		 * The resulting homography transforms image points defined in the previous image to image points defined in the current image (currentPoint = H * previousPoint).<br>
		 * Additionally, this function can return all point correspondences that have been determined between both frames,<br>
		 * or only those point correspondences that have been verified by a homography-based RANSAC.<br>
		 * Beware: Provide a valid 'validPointIndices' parameter and you will receive any point correspondences (stored in 'previousPoints' and 'currentPoints').<br>
		 * Instead, setting the 'validPointIndices' parameter to nullptr will return RANSAC-verified point correspondences only (stored in 'previousPoints' and 'currentPoints').<br>
		 * Ensure that 'previousPoints' and 'currentPoints' are defined if 'validPointIndices' is defined.
		 * @param previousFrame The previous frame which is used to determine the feature points, should have 8 bit grayscale pixel format to improve the performance, otherwise it will be converted internally, can be previousFramePyramid.finestLayer()
		 * @param previousFramePyramid The pyramid of the previous frame, all pyramid layers will be used for tracking, with same pixel origin as 'previousFrame'
		 * @param currentFramePyramid The pyramid of the current frame, with same pixel format, pixel original and layer number as 'previousFramePyramid'
		 * @param previousSubRegion The sub-region specified in the previous frame, image content outside this sub-region will not be used for tracking, define an invalid sub-region to use the entire image information
		 * @param numberFeaturePoints The number of feature points to be used, with range [1, infinity)
		 * @param patchSize The size of the image patches used for point tracking, possible values are 5, 7, 15, 31
		 * @param coarsestLayerRadius The search radius on the coarsest pyramid layer in pixel, with range [1, infinity)
		 * @param determineFullHomography True, to determine an 8-DOF homography; False, to determine a 2-DOF translational homography
		 * @param subPixelIterations Number of sub-pixel iterations that will be applied, each iteration doubles the sub-pixel accuracy, 1 = 0.5, 2 = 0.25, 3 = 0.125, ..., with range [1, infinity)
		 * @param homography Resulting homography transforming points in the previous image to points in the current image (currentPoint = H * previousPoint)
		 * @param worker Optional worker object to distribute the computation
		 * @param previousPoints Optional resulting image points located in the previous frame that have been either tracked and/or used for homography determination, the behavior depends whether 'validPointIndices' is defined or not, nullptr otherwise
		 * @param currentPoints Optional resulting image points located in the current frame that have been either tracked and/or used for homograph determination, one for each point in the previous frame, nullptr otherwise
		 * @param validPointIndices Optional resulting indices of all point correspondences that have been verified by a homography-based RANSAC, nullptr otherwise
		 * @param squarePixelErrorThreshold Maximal square pixel error between a right point and a transformed left point so that a point correspondence counts as valid, with range (0, infinity)
		 * @param zeroMean True, to subtract the color intensities from the corresponding mean intensity before determining the error; False, to determine the error directly
		 * @return True, if succeeded
		 * @see Subset::subset(), Subset::indices2statements().* @see Subset::subset(), Subset::indices2statements().
		 */
		static bool determineHomographyWithPoints(const Frame& previousFrame, const CV::FramePyramid& previousFramePyramid, const CV::FramePyramid& currentFramePyramid, const CV::SubRegion& previousSubRegion, const size_t numberFeaturePoints, const unsigned int patchSize, const unsigned int coarsestLayerRadius, const bool determineFullHomography, const unsigned int subPixelIterations, SquareMatrix3& homography, Worker* worker = nullptr, Vectors2* previousPoints = nullptr, Vectors2* currentPoints = nullptr, Indices32* validPointIndices = nullptr, const Scalar squarePixelErrorThreshold = Scalar(3 * 3), const bool zeroMean = true);
};

}

}

#endif // META_OCEAN_TRACKING_HOMOGRAPHY_IMAGE_ALIGNMENT_SPARSE_H
