/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_SEED_SEGMENTATION_H
#define META_OCEAN_CV_SEGMENTATION_SEED_SEGMENTATION_H

#include "ocean/cv/segmentation/Segmentation.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelBoundingBox.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

/**
 * This class implements basic seed-based segmentation functions.
 * @ingroup cvsegmentation
 */
class OCEAN_CV_SEGMENTATION_EXPORT SeedSegmentation
{
	public:

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_SEGMENTATION_EXPORT Comfort
		{
			public:

				/**
				 * Determines the seed segmentation in a frame.<br>
				 * Neighboring area costs must not exceed the local threshold.<br>
				 * Optional a global threshold can be defined ensuring that the cost between seed location and any candidate location does not exceed a (second) threshold.<br>
				 * The thresholds will be applied to each color channel separately.
				 * @param frame The frame holding the frame data in which the seed segmentation is determined, must be valid
				 * @param mask Resulting 8 bit binary mask defining the segmentation, a value of 0x00 defines a mask pixel, 0xFF defines a non-mask pixel, see setMaskFrameType
				 * @param seed The seed position in the frame, with range ([0, width)x[0, height))
				 * @param localThreshold The local threshold for neighboring pixels, with range [0, infinity)
				 * @param globalThreshold Optional global threshold for the seed pixel and any candidate pixel, with range [0, infinity), 0 to disable the global threshold
				 * @param boundingBox Optional resulting bounding box covering the entire mask area
				 * @param setMaskFrameType True, to change/modify the mask frame type internally if necessary
				 * @return Number of selected mask pixels defining the segmentation, with range [0, frame.pixels()]
				 */
				static unsigned int seedSegmentation(const Frame& frame, Frame& mask, const PixelPosition& seed, const uint8_t localThreshold, const uint8_t globalThreshold = 0, PixelBoundingBox* boundingBox = nullptr, const bool setMaskFrameType = false);

				/**
				 * Determines the seed segmentation in a frame within several iterations.<br>
				 * Neighboring area costs must not exceed the local threshold.<br>
				 * A global threshold ensures that the cost between seed location and any candidate location does not exceed a (second) threshold.<br>
				 * In addition, the global threshold will be increased within a specified value range resulting in different corresponding masks.<br>
				 * The mask determination stops in the moment the number of mask pixels (the size of the mask) exceed the previous number of mask pixel by a specified factor.<br>
				 * The thresholds will be applied to each color channel separately.
				 * @param frame The frame holding the frame data in which the seed segmentation is determined, must be valid
				 * @param mask Resulting 8 bit binary mask defining the segmentation, a value of 0x00 defines a mask pixel, 0xFF defines a non-mask pixel, see setMaskFrameType
				 * @param seed The seed position in the frame, with range ([0, width)x[0, height))
				 * @param localThreshold The local threshold for neighboring pixels, with range [0, infinity)
				 * @param minimalGlobalThreshold The initial global threshold for the seed pixel and any candidate pixel, with range [0, infinity)
				 * @param maximalGlobalThreshold The maximal global threshold for the seed pixel and any candidate pixel, with range [minimalGlobalThreshold, infinity)
				 * @param maximalIncreaseFactor The maximal increase factor between the size of two successive mask so that the iterative process goes on
				 * @param boundingBox Optional resulting bounding box covering the entire mask area
				 * @param setMaskFrameType True, to change/modify the mask frame type internally if necessary
				 * @param worker Optional worker object to distribute the computation
				 * @return Number of selected mask pixels defining the segmentation, with range [0, width * height]
				 */
				static unsigned int iterativeSeedSegmentation(const Frame& frame, Frame& mask, const PixelPosition& seed, const unsigned char localThreshold, const unsigned char minimalGlobalThreshold, const unsigned char maximalGlobalThreshold, const unsigned int maximalIncreaseFactor, PixelBoundingBox* boundingBox = nullptr, const bool setMaskFrameType = false, Worker* worker = nullptr);
		};

	protected:

		/**
		 * This class extends the pixel position by another parameter holding the pixel index of the reference pixel.
		 * The index of the reference pixel is defined by the absolute pixel position of the reference pixel in a frame with a row aligned buffer.
		 */
		class PixelCandidate : public PixelPosition
		{
			public:

				/**
				 * Creates a new pixel candidate object.
				 * @param x Horizontal position
				 * @param y Vertical position
				 * @param reference Index of the reference pixel
				 */
				inline PixelCandidate(const unsigned int x, const unsigned int y, const unsigned int reference);

				/**
				 * Returns the absolute pixel position of the reference pixel.
				 * @return Reference pixel
				 */
				inline unsigned int reference() const;

			private:

				/// Holds the position of the reference pixel.
				unsigned int reference_;
		};

		/**
		 * Definition of a vector holding pixel candidate objects (will be used as stack).
		 */
		typedef std::vector<PixelCandidate> PixelCandidates;

		/// Mask value for unvisited mask pixels.
		static constexpr uint8_t unvisitedMaskValue_ = 0xFFu;

		/// Mask value for visited mask pixels.
		static constexpr uint8_t visitedMaskValue_ = 0x00u;

	public:

		/**
		 * Determines the seed segmentation in a frame.<br>
		 * Neighboring area costs must not exceed the local threshold.<br>
		 * Optional a global threshold can be defined ensuring that the cost between seed location and any candidate location does not exceed a (second) threshold.<br>
		 * The thresholds will be applied to each color channel separately.
		 * @param frame The frame holding the frame data in which the seed segmentation is determined, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param seed The seed position in the frame, with range ([0, width)x[0, height))
		 * @param localThreshold The local threshold for neighboring pixels, with range [0, infinity)
		 * @param globalThreshold Optional global threshold for the seed pixel and any candidate pixel, with range [0, infinity), 0 to disable the global threshold
		 * @param mask Resulting 8 bit binary mask defining the segmentation, a value of 0x00 defines a mask pixel, 0xFF defines a non-mask pixel
		 * @param boundingBox Optional resulting bounding box covering the entire mask area
		 * @return Number of selected mask pixels defining the segmentation, with range [0, width * height]
		 * @tparam T Data type of each channel pixel value
		 * @tparam tChannels The number of data channels of the given frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static unsigned int seedSegmentation(const T* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const PixelPosition& seed, const T localThreshold, const T globalThreshold = T(0), PixelBoundingBox* boundingBox = nullptr);

		/**
		 * Determines the seed segmentation in a frame within several iterations.<br>
		 * Neighboring area costs must not exceed the local threshold.<br>
		 * A global threshold ensures that the cost between seed location and any candidate location does not exceed a (second) threshold.<br>
		 * In addition, the global threshold will be increased within a specified value range resulting in different corresponding masks.<br>
		 * The mask determination stops in the moment the number of mask pixels (the size of the mask) exceed the previous number of mask pixel by a specified factor.<br>
		 * The thresholds will be applied to each color channel separately.
		 * @param frame The frame holding the frame data in which the seed segmentation is determined, must be valid
		 * @param mask Resulting 8 bit binary mask defining the segmentation, a value of 0x00 defines a mask pixel, 0xFF defines a non-mask pixel
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param seed The seed position in the frame, with range ([0, width)x[0, height))
		 * @param localThreshold The local threshold for neighboring pixels, with range [0, infinity)
		 * @param minimalGlobalThreshold The initial global threshold for the seed pixel and any candidate pixel, with range [0, infinity)
		 * @param maximalGlobalThreshold The maximal global threshold for the seed pixel and any candidate pixel, with range [minimalGlobalThreshold, infinity)
		 * @param maximalIncreaseFactor The maximal increase factor between the size of two successive mask so that the iterative process goes on
		 * @param boundingBox Optional resulting bounding box covering the entire mask area
		 * @param worker Optional worker object to distribute the computation
		 * @return Number of selected mask pixels defining the segmentation, with range [0, width * height]
		 * @tparam T Data type of each channel pixel value
		 * @tparam tChannels The number of data channels of the given frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static unsigned int iterativeSeedSegmentation(const T* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const PixelPosition& seed, const T localThreshold, const T minimalGlobalThreshold, const T maximalGlobalThreshold, const unsigned int maximalIncreaseFactor, PixelBoundingBox* boundingBox = nullptr, Worker* worker = nullptr);

		/**
		 * Determines the seed segmentation in a frame while using a mean-area around each pixel to increase the robustness.<br>
		 * Neighboring area costs must not exceed the local threshold.<br>
		 * The cost between the seed area and any other area must not exceed the global threshold.<br>
		 * The thresholds will be applied to each color channel separately.
		 * @param borderedIntegral Bordered integral image of the frame to find an area segmentation for, with 'tChannels' channels, must be valid
		 * @param width The width of the original frame (not the integral frame) in pixel, with range [1, infinity)
		 * @param height The height of the original frame (not the integral frame) in pixel, with range [1, infinity)
		 * @param integralBorder Border size of the integral frame in pixel, while the size of the mean area will be (integralBorder * 2 + 1), with range [1, infinity)
		 * @param areaSize The size of the surrounding area around each pixel, with range [1, integralBorder * 2 + 1], must be odd
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param seed The seed Position in the original frame, with range ([0, width), [0, height))
		 * @param localThreshold The local threshold for neighboring pixels, with range [0, infinity)
		 * @param globalThreshold Optional global threshold for the seed pixel and any candidate pixel, with range [0, infinity), 0 to disable the global threshold
		 * @param mask Resulting 8 bit binary mask defining the segmentation, a value of 0x00 defines a mask pixel, 0xFF defines a non-mask pixel
		 * @param boundingBox Optional resulting bounding box covering the entire mask area
		 * @return Number of selected mask pixels defining the segmentation
		 * @tparam tChannels The number of data channel the information has, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static unsigned int seedSegmentationArea8BitPerChannel(const uint32_t* borderedIntegral, const unsigned int width, const unsigned int height, const unsigned int integralBorder, const unsigned int areaSize, const unsigned int maskPaddingElements, const PixelPosition& seed, const unsigned char localThreshold, const unsigned char globalThreshold, uint8_t* mask, PixelBoundingBox* boundingBox = nullptr);

	private:

		/**
		 * Tests whether all channel-wise SSD values between two pixels are below a given threshold.
		 * Each channel is tested separately, this function fails if one channel exceeds the threshold.
		 * @param image0 Position of the first pixel
		 * @param image1 Position of the second pixel
		 * @param sqrThreshold The maximal value a channel-wise SSD value can have, with range [0, 255 * 255)
		 * @return True, if so
		 * @tparam T The data type of each pixel channel value
		 * @tparam tChannels The number of data channel the information has, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static inline bool ssdBelowThreshold(const T* image0, const T* image1, const typename SquareValueTyper<T>::Type sqrThreshold);

		/**
		 * Tests whether the SSD between two areas is below a given threshold.
		 * Each channel is tested separately, this function fails if one channel exceeds the threshold.
		 * @param borderedIntegral0 Top left position in the bordered integral image for the first area
		 * @param borderedIntegral1 Top left position in the bordered integral image for the second area
		 * @param integralWidth Width of the integral frame (including the two borders and one extra pixel for the zero-column) in pixel with, range [1, infinity)
		 * @param size The size of the area in the integral image, with range [1, integralWidth)
		 * @param sqrThreshold The maximal value a channel-wise SSD value can have, with range [0, 255 * 255)
		 * @return True, if succeeded
		 * @tparam tChannels The number of data channel the information has, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline bool ssdBelowThreshold8BitPerChannel(const unsigned int* borderedIntegral0, const unsigned int* borderedIntegral1, const unsigned int integralWidth, const unsigned int size, const unsigned int sqrThreshold);
};

inline SeedSegmentation::PixelCandidate::PixelCandidate(const unsigned int x, const unsigned int y, const unsigned int reference) :
	PixelPosition(x, y),
	reference_(reference)
{
	// nothing to do here
}

inline unsigned int SeedSegmentation::PixelCandidate::reference() const
{
	return reference_;
}

template <typename T, unsigned int tChannels>
unsigned int SeedSegmentation::seedSegmentation(const T* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const PixelPosition& seed, const T localThreshold, const T globalThreshold, PixelBoundingBox* boundingBox)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);

	if (seed.x() >= width || seed.y() >= height)
	{
		return 0u;
	}

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int maskStrideElements = width + maskPaddingElements;

	// setting all mask values to unvisited

	Frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), mask, Frame::CM_USE_KEEP_LAYOUT, maskPaddingElements).setValue(unvisitedMaskValue_);

	// we use a vector and not a std::stack as the stack implementation is significant slower
	PixelCandidates stack;
	stack.reserve(width * height / 32u);

	unsigned int counter = 1u;

	if (boundingBox)
	{
		*boundingBox = PixelBoundingBox(seed);
	}

	const unsigned int frameSeedOffset = seed.y() * frameStrideElements + seed.x() * tChannels;

	stack.emplace_back(seed.x(), seed.y(), frameSeedOffset);

	typedef typename SquareValueTyper<T>::Type SqrType;

	const SqrType sqrLocalThreshold = localThreshold * localThreshold;
	const SqrType sqrGlobalThreshold = globalThreshold * globalThreshold;

	while (!stack.empty())
	{
		const PixelCandidate pixel = stack.back();
		stack.pop_back();

		const unsigned int maskTestOffset = pixel.y() * maskStrideElements + pixel.x();
		const unsigned int frameTestOffset = pixel.y() * frameStrideElements + pixel.x() * tChannels;

		// test whether the new pixel can be accepted
		if (mask[maskTestOffset] == unvisitedMaskValue_ // checking unvisited state again (as we may have visited this state already from a different path
			&& ssdBelowThreshold<T, tChannels>(frame + frameTestOffset, frame + pixel.reference(), sqrLocalThreshold)
			&& (sqrGlobalThreshold == SqrType(0) || ssdBelowThreshold<T, tChannels>(frame + frameTestOffset, frame + frameSeedOffset, sqrGlobalThreshold)))
		{
			if (boundingBox)
			{
				*boundingBox += pixel;
			}

			mask[maskTestOffset] = visitedMaskValue_;

			// top
			if (pixel.y() > 0u && mask[maskTestOffset - maskStrideElements] != visitedMaskValue_)
			{
				stack.emplace_back(pixel.x(), pixel.y() - 1u, frameTestOffset);
			}

			// bottom
			if (pixel.y() + 1u < height && mask[maskTestOffset + maskStrideElements] != visitedMaskValue_)
			{
				stack.emplace_back(pixel.x(), pixel.y() + 1u, frameTestOffset);
			}

			// left
			if (pixel.x() > 0u && mask[maskTestOffset - 1u] != visitedMaskValue_)
			{
				stack.emplace_back(pixel.x() - 1u, pixel.y(), frameTestOffset);
			}

			// right
			if (pixel.x() + 1u < width && mask[maskTestOffset + 1u] != visitedMaskValue_)
			{
				stack.emplace_back(pixel.x() + 1u, pixel.y(), frameTestOffset);
			}

			++counter;
		}
	}

	return counter;
}

template <typename T, unsigned int tChannels>
unsigned int SeedSegmentation::iterativeSeedSegmentation(const T* frame, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const PixelPosition& seed, const T localThreshold, const T minimalGlobalThreshold, const T maximalGlobalThreshold, const unsigned int maximalIncreaseFactor, PixelBoundingBox* boundingBox, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != nullptr && mask != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(seed.x() < width && seed.y() < height);

	ocean_assert(minimalGlobalThreshold != T(0));
	ocean_assert(minimalGlobalThreshold < maximalGlobalThreshold);

	if (seed.x() >= width || seed.y() >= height || minimalGlobalThreshold > maximalGlobalThreshold)
	{
		return 0u;
	}

	// we use a vector and not a std::stack as the stack implementation is significant slower
	PixelCandidates stack;
	stack.reserve((width * height) / 16u);

	PixelBoundingBox tmpBoundingBox;
	if (boundingBox == nullptr)
	{
		boundingBox = &tmpBoundingBox;
	}

	const typename SquareValueTyper<T>::Type sqrLocalThreshold = localThreshold * localThreshold;

	// first iteration with local threshold and global threshold

	unsigned int maskPixelCounter = seedSegmentation<T, tChannels>(frame, mask, width, height, framePaddingElements, maskPaddingElements, seed, localThreshold, maximalGlobalThreshold, boundingBox);

	// in the following iterations we increase the global threshold and stop if the number of mask pixels increase too much between two iterations

	Frame secondMaskFrame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	uint8_t* const secondMask = secondMaskFrame.data<uint8_t>();

	const unsigned int secondMaskPaddingElements = secondMaskFrame.paddingElements();

	Frame maskFrame(secondMaskFrame.frameType(), mask, Frame::CM_USE_KEEP_LAYOUT, maskPaddingElements);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;
	const unsigned int secondMaskStrideElements = secondMaskFrame.strideElements();

	const unsigned int frameSeedOffset = seed.y() * frameStrideElements + seed.x() * tChannels;

	PixelPositions borderPixels8;
	unsigned int maximalIterationMaskPixelCounter = (unsigned int)(-1);

	for (T t = minimalGlobalThreshold + 1u; t <= maximalGlobalThreshold; ++t)
	{
		const typename SquareValueTyper<T>::Type sqrIterationGlobalThreshold = t * t;

		secondMaskFrame.copy(0, 0, maskFrame);

		PixelBoundingBox iterationBoundingBox(*boundingBox);
		unsigned int iterationMaskPixelCounter = maskPixelCounter;

		borderPixels8.clear();
		MaskAnalyzer::findBorderPixels8(secondMask, width, height, secondMaskPaddingElements, borderPixels8, iterationBoundingBox, worker);

		for (PixelPositions::const_iterator i = borderPixels8.begin(); i != borderPixels8.end(); ++i)
		{
			const unsigned int frameBorderOffset = i->y() * frameStrideElements + i->x() * tChannels;
			const unsigned int secondMaskBorderOffset = i->y() * secondMaskStrideElements + i->x();

			ocean_assert(secondMask[secondMaskBorderOffset] == visitedMaskValue_);

			// top
			if (i->y() > 0u && secondMask[secondMaskBorderOffset - secondMaskStrideElements] != visitedMaskValue_)
			{
				stack.emplace_back(i->x(), i->y() - 1u, frameBorderOffset);
			}

			// bottom
			if (i->y() + 1u < height && secondMask[secondMaskBorderOffset + secondMaskStrideElements] != visitedMaskValue_)
			{
				stack.emplace_back(i->x(), i->y() + 1u, frameBorderOffset);
			}

			// left
			if (i->x() > 0u && secondMask[secondMaskBorderOffset - 1u] != visitedMaskValue_)
			{
				stack.emplace_back(i->x() - 1u, i->y(), frameBorderOffset);
			}

			// right
			if (i->x() + 1u < width && secondMask[secondMaskBorderOffset + 1u] != visitedMaskValue_)
			{
				stack.emplace_back(i->x() + 1u, i->y(), frameBorderOffset);
			}
		}

		while (!stack.empty())
		{
			const PixelCandidate pixel = stack.back();
			stack.pop_back();

			const unsigned int frameTestOffset = pixel.y() * frameStrideElements + pixel.x() * tChannels;
			const unsigned int secondMaskTestOffset = pixel.y() * secondMaskStrideElements + pixel.x();

			// test whether the new pixel can be accepted
			if (secondMask[secondMaskTestOffset] == unvisitedMaskValue_ // checking unvisited state again (as we may have visited this state already from a different path
					&& ssdBelowThreshold<T, tChannels>(frame + frameTestOffset, frame + pixel.reference(), sqrLocalThreshold)
					&& ssdBelowThreshold<T, tChannels>(frame + frameTestOffset, frame + frameSeedOffset, sqrIterationGlobalThreshold))
			{
				iterationBoundingBox += pixel;
				secondMask[secondMaskTestOffset] = visitedMaskValue_;

				// top
				if (pixel.y() > 0 && secondMask[secondMaskTestOffset - secondMaskStrideElements] != visitedMaskValue_)
				{
					stack.emplace_back(pixel.x(), pixel.y() - 1u, frameTestOffset);
				}

				// bottom
				if (pixel.y() + 1u < height && secondMask[secondMaskTestOffset + secondMaskStrideElements] != visitedMaskValue_)
				{
					stack.emplace_back(pixel.x(), pixel.y() + 1u, frameTestOffset);
				}

				// left
				if (pixel.x() > 0 && secondMask[secondMaskTestOffset - 1u] != visitedMaskValue_)
				{
					stack.emplace_back(pixel.x() - 1u, pixel.y(), frameTestOffset);
				}

				// right
				if (pixel.x() + 1u < width && secondMask[secondMaskTestOffset + 1u] != visitedMaskValue_)
				{
					stack.emplace_back(pixel.x() + 1u, pixel.y(), frameTestOffset);
				}

				++iterationMaskPixelCounter;
			}
		}

		if (iterationMaskPixelCounter <= maximalIterationMaskPixelCounter)
		{
			ocean_assert(iterationMaskPixelCounter >= maskPixelCounter);
			maximalIterationMaskPixelCounter = max(iterationMaskPixelCounter + maximalIncreaseFactor * (iterationMaskPixelCounter - maskPixelCounter), iterationMaskPixelCounter * 105u / 100u);

			maskPixelCounter = iterationMaskPixelCounter;
			*boundingBox = iterationBoundingBox;

			maskFrame.copy(0, 0, secondMaskFrame);
		}
		else
		{
			break;
		}
	}

	return maskPixelCounter;
}

template <unsigned int tChannels>
unsigned int SeedSegmentation::seedSegmentationArea8BitPerChannel(const uint32_t* borderedIntegral, const unsigned int width, const unsigned int height, const unsigned int integralBorder, const unsigned int areaSize, const unsigned int maskPaddingElements, const PixelPosition& seed, const unsigned char localThreshold, const unsigned char globalThreshold, uint8_t* mask, PixelBoundingBox* boundingBox)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(borderedIntegral != nullptr && mask != nullptr);
	ocean_assert(integralBorder >= areaSize / 2u);
	ocean_assert(areaSize % 2u == 1u);

	if (seed.x() >= width || seed.y() >= height)
	{
		return 0u;
	}

	const unsigned int maskStrideElements = width + maskPaddingElements;

	const unsigned int areaHalf = areaSize;
	const unsigned int integralWidth = width + integralBorder * 2u + 1u;

	// setting all mask values to unvisited

	Frame(FrameType(width, height, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), mask, Frame::CM_USE_KEEP_LAYOUT, maskPaddingElements).setValue(unvisitedMaskValue_);

	// we use a vector and not a std::stack as the stack implementation is significant slower
	PixelCandidates stack;
	stack.reserve((width * height) / 16u);

	unsigned int counter = 1u;

	PixelBoundingBox tmpBoundingBox;
	if (boundingBox == nullptr)
	{
		boundingBox = &tmpBoundingBox;
	}

	*boundingBox = PixelBoundingBox(seed);

	const unsigned int integralIndex = ((seed.y() + integralBorder - areaHalf) * integralWidth + seed.x() + integralBorder - areaHalf) * tChannels;

	stack.emplace_back(seed.x(), seed.y(), integralIndex);

	const unsigned int sqrLocalThreshold = localThreshold * localThreshold;
	const unsigned int sqrGlobalThreshold = globalThreshold * globalThreshold;

	while (!stack.empty())
	{
		const PixelCandidate pixel = stack.back();
		stack.pop_back();

		const unsigned int maskTestOffset = pixel.y() * maskStrideElements + pixel.x();
		const unsigned int testIntegralIndex = ((pixel.y() + integralBorder - areaHalf) * integralWidth + pixel.x() + integralBorder - areaHalf) * tChannels;

		// test whether the new pixel can be accepted
		if (mask[maskTestOffset] == unvisitedMaskValue_ // checking unvisited state again (as we may have visited this state already from a different path
			&& ssdBelowThreshold8BitPerChannel<tChannels>(borderedIntegral + testIntegralIndex, borderedIntegral + pixel.reference(), integralWidth, areaSize, sqrLocalThreshold)
			&& (sqrGlobalThreshold == 0u || ssdBelowThreshold8BitPerChannel<tChannels>(borderedIntegral + testIntegralIndex, borderedIntegral + integralIndex, integralWidth, areaSize, sqrGlobalThreshold)))
		{
			*boundingBox += pixel;

			mask[maskTestOffset] = visitedMaskValue_;

			// top
			if (pixel.y() > 0u && mask[maskTestOffset - width] != unvisitedMaskValue_)
			{
				stack.emplace_back(pixel.x(), pixel.y() - 1u, testIntegralIndex);
			}

			// bottom
			if (pixel.y() + 1u < height && mask[maskTestOffset + width] != unvisitedMaskValue_)
			{
				stack.emplace_back(pixel.x(), pixel.y() + 1u, testIntegralIndex);
			}

			// left
			if (pixel.x() > 0u && mask[maskTestOffset - 1u] != unvisitedMaskValue_)
			{
				stack.emplace_back(pixel.x() - 1u, pixel.y(), testIntegralIndex);
			}

			// right
			if (pixel.x() + 1u < width && mask[maskTestOffset + 1u] != unvisitedMaskValue_)
			{
				stack.emplace_back(pixel.x() + 1u, pixel.y(), testIntegralIndex);
			}

			++counter;
		}
	}

	return counter;
}

template <typename T, unsigned int tChannels>
inline bool SeedSegmentation::ssdBelowThreshold(const T* image0, const T* image1, const typename SquareValueTyper<T>::Type sqrThreshold)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(image0 && image1);
	ocean_assert(sqrThreshold <= 255u * 255u);

	typename DifferenceValueTyper<T>::Type value;

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		value = image0[n] - image1[n];

		if ((typename SquareValueTyper<T>::Type)(value * value) > sqrThreshold)
		{
			return false;
		}
	}

	return true;
}

template <unsigned int tChannels>
inline bool SeedSegmentation::ssdBelowThreshold8BitPerChannel(const unsigned int* borderedIntegral0, const unsigned int* borderedIntegral1, const unsigned int integralWidth, const unsigned int size, const unsigned int sqrThreshold)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(borderedIntegral0 && borderedIntegral1);
	ocean_assert(integralWidth >= 1u);
	ocean_assert(size < integralWidth);
	ocean_assert(sqrThreshold <= 255u * 255u);

	const unsigned int sqrThresholdArea = sqrThreshold * size * size;

	for (unsigned int n = 0u; n < tChannels; ++n)
	{
		const int value0 = *(borderedIntegral0 + n) - *(borderedIntegral0 + tChannels * size + n) - *(borderedIntegral0 + tChannels * size * integralWidth + n) + *(borderedIntegral0 + tChannels * (size * integralWidth + size) + n);
		const int value1 = *(borderedIntegral1 + n) - *(borderedIntegral1 + tChannels * size + n) - *(borderedIntegral1 + tChannels * size * integralWidth + n) + *(borderedIntegral1 + tChannels * (size * integralWidth + size) + n);

		const int value = value0 - value1;

		if ((unsigned int)(value * value) > sqrThresholdArea)
		{
			return false;
		}
	}

	return true;
}

}

}

}

#endif // META_OCEAN_CV_SEGMENTATION_SEED_SEGMENTATION_H
