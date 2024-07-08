/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_POISSON_BLENDING_H
#define META_OCEAN_CV_ADVANCED_POISSON_BLENDING_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelBoundingBox.h"

#include "ocean/math/SparseMatrix.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * Provides image blending based on Poisson Blending, a seamless image composition algorithm from Perez, et al. ("Poisson Image Editing", 2003).
 * This class implements seamless cloning of a region of a source image into a target image.<br>
 * This is performed by minimizing the difference between the Laplacians of source and result image region while using the region border of target pixels as boundary condition.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT PoissonBlending
{
	public:

		/**
		 * Performs Poisson Blending of the specified source frame into the specified target frame.
		 * Only source pixels defined by the specified source mask are inserted into the target frame.<br>
		 * The source frame in must not overlap with the border of the target frame for the specified insert position.<br>
		 * Source frame and source mask frame should have the same dimensions.<br>
		 * Only pixels with the specified mask value are considered to be inside of the source mask.
		 * @param source The source frame, pixel format must be 8 bit per color channel, must be valid
		 * @param sourceMask Mask of source frame, pixel format must be Y8, must be valid
		 * @param target The target frame into which the source frame is blended, the pixel format must be identical to the source frame, must be valid
		 * @param left Specifies the horizontal position of the left border of the inserted source frame in pixel, with range (-infinity, infinity)
		 * @param top Specifies the vertical position of the top border of the inserted source frame in pixel, with range (-infinity, infinity)
		 * @param maskValue The value of mask pixels to be considered inside of the source content that will be blended
		 * @param worker Optional worker object to distribute the computational load
		 */
		static void poissonBlending(const Frame& source, const Frame& sourceMask, Frame& target, const int left, const int top, const uint8_t maskValue = 0xFFu, Worker* worker = nullptr);

	protected:

		/**
		 * Performs separate Poisson blending calculations for a range of color channels.
		 * @param indexLookup Index lookup frame for masked source pixels
		 * @param source The source frame, pixel format must be 8 bit per color channel
		 * @param target The target frame into which the source frame is blended, Pixel format must be identical to the source frame
		 * @param matrixA Sparse matrix representing the relations between neighbored source mask pixels
		 * @param sourceBoundingBox The source bounding box, must be valid
		 * @param targetLeft Left border of insert region in the target frame
		 * @param targetTop Top border of insert region in the target frame
		 * @param firstChannel Index of the first color channel to be handled, with range [0, target.channels() - channelCount]
		 * @param channelCount Number of color channels to be handled, with range [1, target.channels()]
		 */
		static void poissonBlendingSubset(const Frame* indexLookup, const Frame* source, Frame* target, const SparseMatrixF* matrixA, const CV::PixelBoundingBox* sourceBoundingBox, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int firstChannel, const unsigned int channelCount);

		/**
		 * Creates an index lookup frame for masked source pixels.
		 * Pixels are considered to be part of the mask if they have the specified mask value.
		 * @param sourceMask Frame mask for which an index lookup is created, pixel format should be 8bit grayscale
		 * @param sourceBoundingBox The source bounding box, must be valid
		 * @param maskValue value of pixels to be considered inside of the source mask
		 * @param indexLookup Receives the created index lookup frame
		 * @return The number of pixels within the mask
		 */
		static size_t createIndexLookup(const Frame& sourceMask, const CV::PixelBoundingBox& sourceBoundingBox, const uint8_t maskValue, Frame& indexLookup);

		/**
		 * Creates a sparse matrix representing the relations between neighbored source mask pixels.
		 * @param indexLookup Index lookup frame
		 * @param sourceBoundingBox The source bounding box, must be valid
		 * @param targetLeft Left border of insert region in the target frame, with range [0, targetWidth)
		 * @param targetTop Top border of insert region in the target frame, with range [0, targetHeight)
		 * @param targetWidth Width of target frame in pixels
		 * @param targetHeight Height of target frame in pixels
		 * @param matrixA Receives the sparse matrix representing the relations between neighbored source mask pixels
		 */
		static void createMaskNeighborRelationsMatrix(const Frame& indexLookup, const CV::PixelBoundingBox& sourceBoundingBox, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int targetWidth, const unsigned int targetHeight, SparseMatrixF& matrixA);

		/**
		 * Creates the vector representing the sum of border pixels and source frame laplacian for each pixel within the source mask.
		 * @param indexLookup Index lookup frame for masked source pixels
		 * @param source The source frame, pixel format must be 8 bit per color channel
		 * @param target The target frame into which the source frame is blended, Pixel format must be identical to the source frame
		 * @param sourceBoundingBox The source bounding box, must be valid
		 * @param targetLeft Left border of insert region in the target frame
		 * @param targetTop Top border of insert region in the target frame
		 * @param channelIndex The index of the color channel, with range [0, target.channels()]
		 * @param vectorB Receives the vector representing the sum of border pixels and source frame laplacian for each pixel within the source mask.
		 */
		static void createSummedBorderLaplacianVector(const Frame& indexLookup, const Frame& source, const Frame& target, const CV::PixelBoundingBox& sourceBoundingBox, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int channelIndex, MatrixF& vectorB);

		/**
		 * Inserts the solved matrix into the output frame for the specified channel number.
		 * @param indexLookup Index lookup frame for masked source pixels
		 * @param vectorX Result vector containing the calculated Poisson blended values
		 * @param sourceBoundingBox The source bounding box, must be valid
		 * @param targetLeft Left border of insert region in the target frame
		 * @param targetTop Top border of insert region in the target frame
		 * @param channelIndex The index of the color channel, with range [0, target.channels()]
		 * @param target The target frame into which the source frame is blended, Pixel format must be identical to the source frame
		 */
		static void insertResultDataToChannel(const Frame& indexLookup, const MatrixF& vectorX, const CV::PixelBoundingBox& sourceBoundingBox, const unsigned int targetLeft, const unsigned int targetTop, const unsigned int channelIndex, Frame& target);
};

}

}

}

#endif // META_OCEAN_CV_ADVANCED_POISSON_BLENDING_H
