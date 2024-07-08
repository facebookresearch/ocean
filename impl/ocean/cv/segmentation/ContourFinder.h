/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_CONTOUR_FINDER_H
#define META_OCEAN_CV_SEGMENTATION_CONTOUR_FINDER_H

#include "ocean/cv/segmentation/Segmentation.h"
#include "ocean/cv/segmentation/PixelContour.h"

#include "ocean/base/Frame.h"
#include "ocean/base/RandomGenerator.h"
#include "ocean/base/Worker.h"

#include <vector>

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

/**
 * This class implements functions finding contours.
 * @ingroup cvsegmentation
 */
class OCEAN_CV_SEGMENTATION_EXPORT ContourFinder
{
	protected:

		/**
		 * This class implements a fingerprint based on arbitrary visual information which can be used to determine similarity or dissimilarity between other visual information (e.g., specific locations in a frame).
		 * The fingerprint stores 8 bit per data channel.
		 * @tparam tChannels The number of data channels each fingerprint holds, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		class Fingerprint
		{
			public:

				/**
				 * Definition of a vector holding fingerprint objects.
				 */
				typedef typename std::vector<Fingerprint<tChannels>> Fingerprints;

			protected:

				/**
				 * Definition of a data type storing the information of one fingerprint.
				 */
				typedef typename DataType<uint8_t, tChannels>::Type Type;

			public:

				/**
				 * Creates a new fingerprint object while the fingerprint's data is extracted from a specified frame (or buffer) with 'tChannels' data channels at a specified location.
				 * @param frame The frame from which the fingerprint information is extracted, with 'tChannels' data channels, must be valid
				 * @param frameStrideElements The number of elements between two frame row, in elements, with range [width * tChannels, infinity)
				 * @param fingerprintLocation The  location of the fingerprint, with range [0, width)x[0, height)
				 */
				inline Fingerprint(const uint8_t* const frame, const unsigned int frameStrideElements, const PixelPosition& fingerprintLocation);

				/**
				 * Returns whether this fingerprint is similar to the visual information in a frame at a specified location.
				 * Similarity is determined based on the channel-wise square distance between the fingerprint data and the data of the frame.<br>
				 * The square distance between the information of each fingerprint channel and frame channel must be below the individual channel-based threshold parameters.
				 * @param frame The frame which will be used for the comparison, with 'tChannels' data channels, must be valid
				 * @param frameStrideElements The number of elements between two frame row, in elements, with range [width * tChannels, infinity)
				 * @param position The position inside the frame where the fingerprint comparison will be applied, with range [0, width)x[0, height)
				 * @param maximalSqrDistances The maximal square distances between frame data and fingerprint data, one for each data channel, with range [0, 255 * 255)
				 */
				inline bool isSimilar(const uint8_t* const frame, const unsigned int frameStrideElements, const PixelPosition& position, const unsigned int maximalSqrDistances[tChannels]) const;

				/**
				 * Returns whether this fingerprint is similar to the visual information at a specific frame location.
				 * Similarity is determined based on the channel-wise square distance between the fingerprint data and the data of the frame.<br>
				 * The square distance between the information of each fingerprint channel and frame channel must be below the individual channel-based threshold parameters.
				 * @param frameLocation The location in the frame which will be used for the comparison, with 'tChannels' data channels, must be valid
				 * @param maximalSqrDistances The maximal square distances between frame data and fingerprint data, one for each data channel, with range [0, 255 * 255)
				 */
				inline bool isSimilar(const uint8_t* const frameLocation, const unsigned int maximalSqrDistances[tChannels]) const;

				/**
				 * Returns the data of this finger print.
				 * @return The fingerprint's data
				 */
				const Type& data() const;

				/**
				 * Returns the fingerprint value of one specific data channel.
				 * @param index The index of the data channel for which the value will be returned, with range [0, tChannels)
				 * @return The value of the fingerprint's channel
				 */
				inline uint8_t operator[](const unsigned int index) const;

			protected:

				/// The data information of this fingerprint.
				Type data_;
		};

	public:

		/**
		 * Determines the contour of an object visually different from the visual content around a rough contour (very roughly) surrounding the object.
		 * This function actually invoked similarityContour8BitPerChannel() depending on the number of frame channels.
		 * @param frame The frame showing the object, in which the rough contour is defined and for which the fine contour will be determined
		 * @param window The window size for the mean filter that is applied internally before the contour is determined, with range [1, infinity), must be odd
		 * @param roughContour The rough contour (very roughly) enclosing the object for which the fine contour will be determined
		 * @param extraOffset The explicit additional offset between the actual object and the final resulting contour in pixel, with range [0, infinity)
		 * @param randomGenerator Random number generator object
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting accurate and dense contour, an empty contour if the contour determination failed
		 * @see similarityContour8BitPerChannel().
		 */
		static PixelContour similarityContour(const Frame& frame, const unsigned int window, const PixelContour& roughContour, const unsigned int extraOffset, RandomGenerator& randomGenerator, Worker* worker = nullptr);

		/**
		 * Determines the contour of an object visually different from the visual content around a rough contour (very roughly) surrounding the object.
		 * @param frame The frame showing the object, in which the rough contour is defined and for which the fine contour will be determined
		 * @param width The width of the given frame in pixel, with range [1, infinity)
		 * @param height The height of the given frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param window The window size for the mean filter that is applied internally before the contour is determined, with range [1, infinity), must be odd
		 * @param roughContour The rough contour (very roughly) enclosing the object for which the fine contour will be determined
		 * @param extraOffset The explicit additional offset between the actual object and the final resulting contour in pixel, with range [0, infinity)
		 * @param randomGenerator Random number generator object
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting accurate and dense contour, an empty contour if the contour determination failed
		 * @tparam tChannels The number of channels the provided frame has, with range [1, infinity)
		 * @see similarityContour().
		 */
		template <unsigned int tChannels>
		static PixelContour similarityContour8BitPerChannel(const uint8_t* frame, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int window, const PixelContour& roughContour, const unsigned int extraOffset, RandomGenerator& randomGenerator, Worker* worker = nullptr);

		/**
		 * Determines fixed landmark locations around the border of an object within a frame and within a rough contour.
		 * This function uses fingerprints around a specified rough contour to identify the landmarks.
		 * @param frame The frame in which the object is visible and for which the landmarks will be determined, must be valid
		 * @param roughMask The rough 8 bit mask covering the object, with same frame dimension and pixel origin as the provided frame, must be valid
		 * @param roughContour The rough contour around the object, with ranges [0, frame.width())x[0, frame.height())
		 * @param extraOffset The explicit additional offset between the actual object and the final resulting contour in pixel, with range [0, infinity)
		 * @param landmarks The resulting locations of the landmarks
		 * @return True, if succeeded
		 * @see findBorderLandmarks8BitPerChannel().
		 */
		static bool findBorderLandmarks(const Frame& frame, const Frame& roughMask, const PixelContour& roughContour, const unsigned int extraOffset, Vectors2& landmarks);

		/**
		 * Determines fixed landmark locations around the border of an object within a frame and within a rough contour.
		 * This function uses fingerprints around a specified rough contour to identify the landmarks.
		 * @param frame The frame in which the object is visible and for which the landmarks will be determined, must be valid
		 * @param roughMask The rough 8 bit mask covering the object, with same frame dimension and pixel origin as the provided frame, must be valid
		 * @param width The width of the given frame (and mask) in pixel, with range [1, infinity)
		 * @param height The height of the given frame (and mask) in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param roughMaskPaddingElements The number of padding elements at the end of each rough mask row, in elements, with range [0, infinity)
		 * @param roughContour The rough contour around the object, with ranges [0, width)x[0, height)
		 * @param extraOffset The explicit additional offset between the actual object and the final resulting contour in pixel, with range [0, infinity)
		 * @param landmarks The resulting locations of the landmarks
		 * @return True, if succeeded
		 * @tparam tChannels The number of channels the provided frame has, with range [1, infinity)
		 * @see findBorderLandmarks().
		 */
		template <unsigned int tChannels>
		static bool findBorderLandmarks8BitPerChannel(const uint8_t* frame, const uint8_t* roughMask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int roughMaskPaddingElements, const PixelContour& roughContour, const unsigned int extraOffset, Vectors2& landmarks);

		/**
		 * Adjusts the location and shape of a given contour to a set of given landmark locations.
		 * @param contour The contour which will be adjusted
		 * @param landmarks The landmarks used to adjust the contour
		 * @param adjustedContour The resulting adjusted contour
		 * @param aggressive True, to apply an aggressive adjustment; False, to apply a conservative adjustment
		 * @return True, if succeeded
		 */
		static bool adjustContourWithLandmarks(const Vectors2& contour, const Vectors2& landmarks, Vectors2& adjustedContour, const bool aggressive = false);

	protected:

		/**
		 * Determines the similarity mask of an object visible in a frame which's visual information is not similar to the visual content around a given rough contour around the object.
		 * This function is based on a pyramid (coarse to fine) approach where the initial mask is determined on the coarsest layer and then propagated to the next finer layer afterwards.
		 * @param frame The frame for which the mask will be determined, must be valid
		 * @param roughMask The already known very rough 8 bit mask covering the object, with frame dimension and pixel origin as the provided frame, must be valid
		 * @param roughContour The rough contour around the object which will be used for the similarity comparison, with ranges [0, width)x[0, height)
		 * @param variances The channel-wise maximal square distances between visual information of the frame and the fingerprints distributed around the rough contour so that a visual information counts as similar
		 * @param oneObject True, to receive a resulting mask composed of one large mask block (the largest block that can be determined); False, to receive a resulting mask which may be composed of several not joined blocks
		 * @param layers The number of pyramid layers to be used for the mask determination, with range [0, infinity)
		 * @param fineMaskResult The resulting fine mask for the finest pyramid layer (for the given frame)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of channels the given frame holds, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void similarityMaskPyramid8BitPerChannel(const Frame& frame, const Frame& roughMask, const PixelContour& roughContour, const unsigned int variances[tChannels], const bool oneObject, const unsigned int layers, Frame& fineMaskResult, Worker* worker);

		/**
		 * Determines the similarity mask of an object visible in a frame which's visual information is not similar to the visual content around a given rough contour around the object.
		 * @param frame The frame for which the mask will be determined, must be valid
		 * @param roughMask The already known very rough 8 bit mask covering the object, with frame dimension and pixel origin as the provided frame, must be valid
		 * @param fineMask The resulting precise 8 bit mask covering the object, with frame dimension and pixel origin as the provided frame, must be valid
		 * @param width The width of the provided frame (and both mask frames) in pixel, with range [1, infinity)
		 * @param height The height of the provided frame (and both mask frames) in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements after each frame row, in elements, with range [0, infinity)
		 * @param roughMaskPaddingElements The number of padding elements after each rough mask row, in elements, with range [0, infinity)
		 * @param fineMaskPaddingElements The number of padding elements after each fine mask row, in elements, with range [0, infinity)
		 * @param roughContour The rough contour around the object which will be used for the similarity comparison, with ranges [0, width)x[0, height)
		 * @param variances The channel-wise maximal square distances between visual information of the frame and the fingerprints distributed around the rough contour so that a visual information counts as similar
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of channels the given frame holds, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void similarityMask8BitPerChannel(const uint8_t* frame, const uint8_t* roughMask, uint8_t* fineMask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int roughMaskPaddingElements, const unsigned int fineMaskPaddingElements, const PixelContour& roughContour, const unsigned int variances[tChannels], Worker* worker);

		/**
		 * Finetunes specific locations in a mask (e.g., for pixels at the mask's border) currently rated as mask pixels in the rough mask.
		 * This function uses fingerprints (located at the pixel positions of the rough contour) for finetuning.
		 * @param frame The frame which will be compared with the fingerprints, must be valid
		 * @param mask The already known rough mask that will be tuned (matching with the given frame), with same frame dimension and pixel origin as the given frame, must be valid
		 * @param width The width of the given frame (and both masks) in pixel, with range [1, infinity)
		 * @param height The height of the given frame (and both masks) in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param roughContour The rough (but distinct) contour around the rough mask which's pixel locations define the locations for the fingerprints to be used for finetuning, with ranges [0, width)x[0, height)
		 * @param positions The specific locations in the mask for which the finetuning will be done, with ranges [0, width)x[0, height)
		 * @param variances The maximal channel-based square distances between a fingerprint and an image information so that the visual content counts as similar
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels of the provided frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void finetuneSimilarityMask8BitPerChannel(const uint8_t* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const PixelContour& roughContour, const CV::PixelPositions& positions, const unsigned int variances[tChannels], Worker* worker);

		/**
		 * Determines the similarity mask of an object visible in a subset of a frame which's visual information is not similar to the visual content around a given rough contour around the object.
		 * @param frame The frame for which the mask will be determined, must be valid
		 * @param roughMask The already known very rough 8 bit mask covering the object, with frame dimension and pixel origin as the provided frame, must be valid
		 * @param fineMask The resulting precise 8 bit mask covering the object, with frame dimension and pixel origin as the provided frame, must be valid
		 * @param width The width of the provided frame (and both mask frames) in pixel, with range [1, infinity)
		 * @param height The height of the provided frame (and both mask frames) in pixel, with range [1, infinity)
		 * @param frameStrideElements The number of elements between two frame rows, in elements, with range [width * tChannels, infinity)
		 * @param roughMaskStrideElements The number of elements between two rough mask rows, in elements, with range [width, infinity)
		 * @param fineMaskStrideElements The number of elements between two fine mask rows, in elements, with range [width, infinity)
		 * @param fingerprints The fingerprints distributed around the object which will be used for similarity comparison
		 * @param numberFingerprints The number of provided fingerprints, with range [1, infinity)
		 * @param variances The channel-wise maximal square distances between visual information of the frame and the fingerprints distributed around the rough contour so that a visual information counts as similar
		 * @param firstColumn The first frame column to be handled allowing to shrink the area of application to improve the execution time, with range [0, width)
		 * @param numberColumns The number of columns to be handled, with range [1, width]
		 * @param firstRow The first row to be handled, with range [0, height)
		 * @param numberRows The number of rows to be handled, with range [1, height]
		 * @tparam tChannels The number of channels the given frame holds, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void similarityMask8BitPerChannelSubset(const uint8_t* frame, const uint8_t* roughMask, uint8_t* fineMask, const unsigned int width, const unsigned int height, const unsigned int frameStrideElements, const unsigned int roughMaskStrideElements, const unsigned int fineMaskStrideElements, const Fingerprint<tChannels>* fingerprints, const size_t numberFingerprints, const unsigned int variances[tChannels], const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Finetunes a subset of specific locations in a mask (e.g., for pixels at the mask's border) currently rated as mask pixels in the rough mask.
		 * This function uses fingerprints (located at the pixel positions of the rough contour) for finetuning.
		 * @param frame The frame which will be compared with the fingerprints, must be valid
		 * @param mask The already known rough mask that will be tuned (matching with the given frame), with same frame dimension and pixel origin as the given frame, must be valid
		 * @param frameStrideElements The number of elements between two frame rows, in elements, with range [width * tChannels, infinity)
		 * @param maskStrideElements The number of elements between two mask rows, in elements, with range [width, infinity)
		 * @param fingerprints The fingerprints distributed around the object which will be used for similarity comparison
		 * @param numberFingerprints The number of provided fingerprints, with range [1, infinity)
		 * @param variances The maximal channel-based square distances between a fingerprint and an image information so that the visual content counts as similar
		 * @param positions The specific locations in the mask for which the finetuning will be done, with ranges [0, width)x[0, height)
		 * @param firstPosition The first position to be handled, with range [0, positions.size())
		 * @param numberPositions The number of positions to be handled, with range [1, positions.size()]
		 * @tparam tChannels The number of data channels of the provided frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void finetuneSimilarityMask8BitPerChannelSubset(const uint8_t* frame, uint8_t* mask, const unsigned int frameStrideElements, const unsigned int maskStrideElements, const Fingerprint<tChannels>* fingerprints, const size_t numberFingerprints, const unsigned int variances[tChannels], const CV::PixelPosition* positions, const unsigned int firstPosition, const unsigned int numberPositions);
};

template <unsigned int tChannels>
inline ContourFinder::Fingerprint<tChannels>::Fingerprint(const uint8_t* const frame, const unsigned int frameStrideElements, const PixelPosition& fingerprintLocation)
{
	ocean_assert(frame != nullptr);
	ocean_assert(fingerprintLocation.x() < frameStrideElements); // we avoid to test location.y() < height);

	data_ = *(const Type*)(frame + fingerprintLocation.y() * frameStrideElements + fingerprintLocation.x() * tChannels);
}

template <unsigned int tChannels>
inline bool ContourFinder::Fingerprint<tChannels>::isSimilar(const uint8_t* const frame, const unsigned int frameStrideElements, const PixelPosition& position, const unsigned int maximalSqrDistances[tChannels]) const
{
	ocean_assert(frame != nullptr && frameStrideElements != 0u && maximalSqrDistances);
	ocean_assert(position.x() < frameStrideElements); // we avoid to test: position.y() < height

	const uint8_t* const fingerprint = (uint8_t*)(&data_);
	const uint8_t* const frameData = frame + position.y() + frameStrideElements + position.x() * tChannels;

	// we simply determine the square distance between each fingerprint and frame channel and measure whether the value is below the specified thresholds

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		if (sqrDistance(fingerprint[n], frameData[n]) > maximalSqrDistances[n])
		{
			return false;
		}
	}

	return true;
}

template <unsigned int tChannels>
inline bool ContourFinder::Fingerprint<tChannels>::isSimilar(const uint8_t* const frameLocation, const unsigned int maximalSqrDistances[tChannels]) const
{
	ocean_assert(frameLocation && maximalSqrDistances);

	const uint8_t* const fingerprint = (uint8_t*)(&data_);

	// we simply determine the square distance between each fingerprint and frame channel and measure whether the value is below the specified thresholds

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		if (sqrDistance(fingerprint[n], frameLocation[n]) > maximalSqrDistances[n])
		{
			return false;
		}
	}

	return true;
}

template <unsigned int tChannels>
const typename ContourFinder::Fingerprint<tChannels>::Type& ContourFinder::Fingerprint<tChannels>::data() const
{
	return data_;
}

template <unsigned int tChannels>
inline uint8_t ContourFinder::Fingerprint<tChannels>::operator[](const unsigned int index) const
{
	ocean_assert(index < tChannels);

	return ((const uint8_t*)(&data_))[index];
}

}

}

}

#endif // META_OCEAN_CV_SEGMENTATION_CONTOUR_FINDER_H
