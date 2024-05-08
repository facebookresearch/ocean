/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SYNTHESIS_INITIALIZER_SHRINKING_PATCH_MATCHING_I_1_H
#define META_OCEAN_CV_SYNTHESIS_INITIALIZER_SHRINKING_PATCH_MATCHING_I_1_H

#include "ocean/cv/synthesis/Synthesis.h"
#include "ocean/cv/synthesis/InitializerI.h"
#include "ocean/cv/synthesis/InitializerRandomized.h"
#include "ocean/cv/synthesis/Initializer1.h"
#include "ocean/cv/synthesis/LayerI1.h"

#include "ocean/cv/CVUtilities.h"
#include "ocean/cv/FrameFilterSobel.h"

#include "ocean/cv/advanced/AdvancedSumSquareDifferences.h"

#include "ocean/cv/segmentation/MaskAnalyzer.h"

#include "ocean/math/Random.h"

#include <list>

namespace Ocean
{

namespace CV
{

namespace Synthesis
{

/**
 * This class implements an initializer that shrinks the inpainting mask by the application of images patches.
 * @ingroup cvsynthesis
 */
class OCEAN_CV_SYNTHESIS_EXPORT InitializerShrinkingPatchMatchingI1 :
	virtual public InitializerI,
	virtual public InitializerRandomized,
	virtual public Initializer1
{
	private:

		/**
		 * Definition of a wrapper class for ssd functions.
		 */
		class SSDWrapperMask
		{
			public:

				/**
				 * Wrapper function for AdvancedSumSquareDifferences::patchWithRejectingMask8BitPerChannel().
				 * @see AdvancedSumSquareDifferences::patchWithRejectingMask8BitPerChannel().
				 */
				template <unsigned int tChannels>
				static inline unsigned int determine5x5(const uint8_t* frame, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const CV::PixelPosition& inPosition, const CV::PixelPosition& outPosition);
		};

		/**
		 * Definition of a wrapper class for ssd functions.
		 */
		class SSDWrapper
		{
			public:

				/**
				 * Wrapper function for SumSquareDifferences::patchAtBorder8BitPerChannel.
				 * @see SumSquareDifferences::patchAtBorder8BitPerChannel().
				 */
				template <unsigned int tChannels>
				static inline unsigned int determine5x5(const uint8_t* frame, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const CV::PixelPosition& inPosition, const CV::PixelPosition& outPosition);
		};

		/**
		 * This class implements a single inpainting pixel.
		 */
		class InpaintingPixel : public PixelPosition
		{
			public:

				/**
				 * Creates a new inpainting pixel by a given border direction and image orientation.
				 * @param position Inpainting pixel position
				 * @param borderDirection Direction of the inpainting border
				 * @param imageOrientation Orientation of the surrounding image
				 */
				inline InpaintingPixel(const PixelPosition& position, const VectorI2& borderDirection, const VectorI2& imageOrientation);

				/**
				 * Returns the border direction of this inpainting pixel.
				 * @return Border direction
				 */
				inline const VectorI2& borderDirection() const;

				/**
				 * Returns the image orientation of this inpainting pixel.
				 * @return Image orientation
				 */
				inline const VectorI2& imageOrientation() const;

				/**
				 * Returns the priority of this inpainting pixel.
				 * @return Pixel priority
				 */
				inline unsigned int priority() const;

				/**
				 * Sets the border direction of this inpainting pixel.
				 * @param direction Border direction to be set
				 */
				inline void setBorderDirection(const VectorI2& direction);

				/**
				 * Sets the image orientation of this inpainting pixel.
				 * @param orientation Image orientation to be set
				 */
				inline void setImageOrientation(const VectorI2& orientation);

				/**
				 * Determines the border direction of a border pixel by a 5x5 area.
				 * @param mask The mask frame, must be valid
				 * @param width The width of the mask in pixel, with range [1, infinity)
				 * @param height The height of the mask in pixel, with range [1, infinity)
				 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
				 * @param position The position to determine the direction for, this pixel must be a mask-border pixel, with range [0, with-1]x[0, height-1]
				 * @return Resulting direction
				 */
				static VectorI2 determineBorderDirection5x5(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position);

				/**
				 * Determines the image orientation at a given position.
				 * The template parameter given the depth of the given response
				 * @param sobelResponse The Sobel filter response used to determine the image orientation, must be valid
				 * @param mask The mask frame with same frame dimension as the response frame, must be valid
				 * @param width The width of the frame in pixel, with range [1, infinity)
				 * @param height The height of the frame in pixel, with range [1, infinity)
				 * @param sobelStrideElements The number of elements between two consecutive Sobel response rows, in elements, with range [width*2, infinity)
				 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
				 * @param position The position to determine the image orientation for, this pixel must be a mask-border pixel, with range [0, with-1]x[0, height-1]
				 * @tparam tChannels Number of channels of the frame for which the sobel response is provided, with range [1, infinity)
				 */
				template <unsigned int tChannels>
				static VectorI2 determineImageOrientation5x5(const int16_t* sobelResponse, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int sobelStrideElements, const unsigned int maskPaddingElements, const CV::PixelPosition& position);

				/**
				 * Returns whether this inpainting pixel has a lower priority than a second one.
				 * @param right Second inpainting pixel to be compared
				 * @return True, if so
				 */
				inline bool operator<(const InpaintingPixel& right) const;

				/**
				 * Returns whether two inpainting pixels are identical.
				 * @param right Second inpainting pixel
				 * @return True, if so
				 */
				inline bool operator==(const InpaintingPixel& right) const;

			protected:

				/// Image orientation.
				VectorI2 imageOrientation_ = VectorI2(0, 0);

				/// Border direction.
				VectorI2 borderDirection_ = VectorI2(0, 0);

				/// Inpainting priority.
				unsigned int priority_ = 0u;
		};

		/**
		 * Definition of a list holding inpainting pixels.
		 */
		typedef std::list<InpaintingPixel> InpaintingPixelList;

	public:

		/**
		 * Creates a new initializer object.
		 * @param layer The layer for that the initial mapping has to be provided
		 * @param randomGenerator Random number generator
		 * @param iterations Number of shrinking iterations
		 * @param heuristic True, to apply a heuristic optimization, faster but more inaccurate
		 * @param maximalBoundingBoxOffset Maximal search offset around an inpainting pixel to be used in the non-heuristic mode
		 */
		inline InitializerShrinkingPatchMatchingI1(LayerI1& layer, RandomGenerator& randomGenerator, const unsigned int iterations = 2u, const bool heuristic = false, const unsigned int maximalBoundingBoxOffset = 0xFFFFFFFF);

		/**
		 * Invokes the initialization process.
		 * @see Initializer::invoke().
		 */
		bool invoke(Worker* worker = nullptr) const override;

	private:

		/**
		 * Invokes the initialization process.
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		bool invoke(Worker* worker) const;

		/**
		 * Applies the first iterations of the patch initialization for a frame using 5x5 areas.
		 * @param sobelResponse Horizontal and vertical sobel response for the given frame, must be valid
		 * @param staticMask Static inpainting mask to be filled with 0xFF for non-mask pixels, must be valid
		 * @param randomGenerator The random generator to be used
		 * @return True, if succeeded
		 * @tparam tChannels Number of channels of the frame
		 * @tparam SSD SSD wrapper class that is used to determine the patch similarity
		 */
		template<unsigned int tChannels, typename SSD>
		bool patchInitializationIteration5x5(Frame& sobelResponse, const Frame& staticMask, RandomGenerator& randomGenerator) const;

	private:

		/// Number of initialization iterations.
		const unsigned int iterations_;

		/// Heuristic execution statement of the initializer.
		const bool heuristic_;

		/// Maximal search offset that is applied during the initialization for each inpainting pixel.
		const unsigned int maximalBoundingBoxOffset_;
};

inline InitializerShrinkingPatchMatchingI1::InpaintingPixel::InpaintingPixel(const PixelPosition& position, const VectorI2& borderDirection, const VectorI2& imageOrientation) :
	PixelPosition(position),
	imageOrientation_(imageOrientation),
	borderDirection_(borderDirection),
	priority_(abs(imageOrientation.perpendicular() * borderDirection))
{
	// nothing to do here
}

inline const VectorI2& InitializerShrinkingPatchMatchingI1::InpaintingPixel::borderDirection() const
{
	return borderDirection_;
}

inline const VectorI2& InitializerShrinkingPatchMatchingI1::InpaintingPixel::imageOrientation() const
{
	return imageOrientation_;
}

inline unsigned int InitializerShrinkingPatchMatchingI1::InpaintingPixel::priority() const
{
	return priority_;
}

inline void InitializerShrinkingPatchMatchingI1::InpaintingPixel::setBorderDirection(const VectorI2& direction)
{
	borderDirection_ = direction;
	priority_ = abs(imageOrientation_.perpendicular() * borderDirection_);
}

inline void InitializerShrinkingPatchMatchingI1::InpaintingPixel::setImageOrientation(const VectorI2& orientation)
{
	imageOrientation_ = orientation;
	priority_ = abs(imageOrientation_.perpendicular() * borderDirection_);
}

template <unsigned int tChannels>
VectorI2 InitializerShrinkingPatchMatchingI1::InpaintingPixel::determineImageOrientation5x5(const int16_t* sobelResponse, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int sobelStrideElements, const unsigned int maskPaddingElements, const CV::PixelPosition& position)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(sobelResponse != nullptr && mask != nullptr);

	ocean_assert(sobelStrideElements >= width * 2u);
	ocean_assert(position.x() < width && position.y() < height);

	VectorI2 orientation(0, 0);

	// as the response values will be zero at the image borders border pixels does not need to be investigated

	for (unsigned int y = max(1, int(position.y()) - 2); y < min(position.y() + 3u, height - 1u); ++y)
	{
		for (unsigned int x = max(1, int(position.x()) - 2); x < min(position.x() + 3u, width - 1u); ++x)
		{
			constexpr uint8_t nonMaskPixelValue = 0xFFu;

			if (!Segmentation::MaskAnalyzer::hasMaskNeighbor9<false>(mask, width, height, maskPaddingElements, PixelPosition(x, y), nonMaskPixelValue))
			{
				const int16_t* const sobelPixel = sobelResponse + y * sobelStrideElements + x * tChannels * 2u;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					const int16_t responseX = sobelPixel[n * 2u + 0u];
					const int16_t responseY = sobelPixel[n * 2u + 1u];

					if (responseX >= 0)
					{
						orientation += VectorI2(responseX, responseY);
					}
					else
					{
						orientation -= VectorI2(responseX, responseY);
					}
				}
			}
		}
	}

	return orientation;
}

inline bool InitializerShrinkingPatchMatchingI1::InpaintingPixel::operator<(const InpaintingPixel& right) const
{
	//return pixelPriority < right.pixelPriority;
	if(priority_ < right.priority_)
	{
		return true;
	}

	if (priority_ == right.priority_)
	{
		return index(1000) < right.index(1000);
	}

	return false;
}

inline bool InitializerShrinkingPatchMatchingI1::InpaintingPixel::operator==(const InpaintingPixel& right) const
{
	return (PixelPosition&)*this == (PixelPosition&)right && borderDirection_ == right.borderDirection_ && imageOrientation_ == right.imageOrientation_;
}

inline InitializerShrinkingPatchMatchingI1::InitializerShrinkingPatchMatchingI1(LayerI1& layer, RandomGenerator& randomGenerator, const unsigned int initializations, const bool heuristic, const unsigned int maximalBoundingBoxOffset) :
	Initializer(layer),
	InitializerI(layer),
	InitializerRandomized(layer, randomGenerator),
	Initializer1(layer),
	iterations_(initializations),
	heuristic_(heuristic),
	maximalBoundingBoxOffset_(maximalBoundingBoxOffset)
{
	// nothing to do here
}

template <unsigned int tChannels>
inline unsigned int InitializerShrinkingPatchMatchingI1::SSDWrapperMask::determine5x5(const uint8_t* frame, const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int maskPaddingElements, const CV::PixelPosition& inPosition, const CV::PixelPosition& outPosition)
{
	constexpr uint8_t maskValue = 0x00u;

	return Advanced::AdvancedSumSquareDifferences::patchWithRejectingMask8BitPerChannel<tChannels>(frame, frame, mask, mask, 5u, width, height, width, height, inPosition.x(), inPosition.y(), outPosition.x(), outPosition.y(), framePaddingElements, framePaddingElements, maskPaddingElements, maskPaddingElements, maskValue).first;
}

template <unsigned int tChannels>
inline unsigned int InitializerShrinkingPatchMatchingI1::SSDWrapper::determine5x5(const uint8_t* frame, const uint8_t* /*mask*/, const unsigned int width, const unsigned int height, const unsigned int framePaddingElements, const unsigned int /*maskPaddingElements*/, const CV::PixelPosition& inPosition, const CV::PixelPosition& outPosition)
{
	return SumSquareDifferences::patchAtBorder8BitPerChannel<tChannels, 5u>(frame, frame, width, height, width, height, inPosition.x(), inPosition.y(), outPosition.x(), outPosition.y(), framePaddingElements, framePaddingElements).first;
}

template<unsigned int tChannels, typename SSD>
bool InitializerShrinkingPatchMatchingI1::patchInitializationIteration5x5(Frame& sobelResponse, const Frame& staticMask, RandomGenerator& randomGenerator) const
{
	static_assert(tChannels >= 1u, "Invalid number of frame channels!");

	Frame& frame = layer_.frame();

	MappingI& mapping = layerI_.mapping();

	const unsigned int width = layer_.width();
	const unsigned int height = layer_.height();

	ocean_assert(frame.isValid());
	ocean_assert(sobelResponse.isValid() && staticMask.isValid());

	const CV::PixelBoundingBox& boundingBox = layerI_.boundingBox();

#ifdef OCEAN_DEBUG
	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			const CV::PixelPosition& position = mapping.position(x, y);

			ocean_assert(!position.isValid() || (position.x() < width && position.y() < height));
		}
	}
#endif

	Frame dynamicMask(staticMask, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	uint8_t* const frameData = frame.data<uint8_t>();
	const unsigned int framePaddingElements = frame.paddingElements();
	const unsigned int frameStrideElements = frame.strideElements();

	const uint8_t* const staticMaskData = staticMask.constdata<uint8_t>();
	const unsigned int staticMaskStrideElements = staticMask.strideElements();

	uint8_t* const dynamicMaskData = dynamicMask.data<uint8_t>();
	const unsigned int dynamicMaskPaddingElements = dynamicMask.paddingElements();
	const unsigned int dynamicMaskStrideElements = dynamicMask.strideElements();

	int16_t* const sobelResponseData = sobelResponse.data<int16_t>();
	const unsigned int sobelResponseStrideElements = sobelResponse.strideElements();

	CV::PixelPositions borderPixels;
	borderPixels.reserve(1024);

	Segmentation::MaskAnalyzer::findBorderPixels4(dynamicMask.constdata<uint8_t>(), width, height, dynamicMask.paddingElements(), borderPixels, boundingBox);

	InpaintingPixelList inpaintingPixels;
	for (const CV::PixelPosition& borderPixel : borderPixels)
	{
		const VectorI2 borderDirection(InpaintingPixel::determineBorderDirection5x5(dynamicMaskData, width, height, dynamicMaskPaddingElements, borderPixel));
		const VectorI2 imageOrientation(InpaintingPixel::determineImageOrientation5x5<tChannels>(sobelResponseData, dynamicMaskData, width, height, sobelResponseStrideElements, dynamicMaskPaddingElements, borderPixel));

		inpaintingPixels.emplace_back(borderPixel, borderDirection, imageOrientation);
	}

	inpaintingPixels.sort();

	while (!inpaintingPixels.empty())
	{
		const InpaintingPixel position = inpaintingPixels.back();
		ocean_assert(dynamicMask.constpixel<uint8_t>(position.x(), position.y())[0] != 0xFF);

		inpaintingPixels.pop_back();

		unsigned int ssdBest = (unsigned int)(-1);

		unsigned int xBest = (unsigned int)(-1);
		unsigned int yBest = (unsigned int)(-1);

		if (heuristic_)
		{
			const CV::PixelPosition& center = mapping.position(position);

			// propagation itself
			if (center.isValid())
			{
				for (unsigned int y = max(0, int(center.y()) - 3); y < min(center.y() + 4u, height); ++y)
				{
					for (unsigned int x = max(0, int(center.x()) - 3); x < min(center.x() + 4u, width); ++x)
					{
						if ((y != position.y() || x != position.x()) && staticMaskData[y * staticMaskStrideElements + x] == 0xFF)
						{
							const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, PixelPosition(x, y));

							if (ssd < ssdBest)
							{
								ssdBest = ssd;
								xBest = x;
								yBest = y;
							}
						}
					}
				}
			}

			// propagation left
			if (position.x() != 0u && mapping.position(position.west()).isValid())
			{
				const PixelPosition east(mapping.position(position.west()).east());

				for (unsigned int y = max(0, int(east.y()) - 3); y < min(east.y() + 4u, height); ++y)
				{
					for (unsigned int x = max(0, int(east.x()) - 3); x < min(east.x() + 4u, width); ++x)
					{
						if ((y != position.y() || x != position.x()) && staticMaskData[y * staticMaskStrideElements + x] == 0xFF)
						{
							const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, PixelPosition(x, y));

							if (ssd < ssdBest)
							{
								ssdBest = ssd;
								xBest = x;
								yBest = y;
							}
						}
					}
				}
			}

			// propagation top left
			if (position.x() != 0u && position.y() != 0u && mapping.position(position.northWest()).isValid())
			{
				const PixelPosition southEast(mapping.position(position.northWest()).southEast());

				for (unsigned int y = max(0, int(southEast.y()) - 3); y < min(southEast.y() + 4u, height); ++y)
				{
					for (unsigned int x = max(0, int(southEast.x()) - 3); x < min(southEast.x() + 4u, width); ++x)
					{
						if ((y != position.y() || x != position.x()) && staticMaskData[y * staticMaskStrideElements + x] == 0xFF)
						{
							const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, PixelPosition(x, y));

							if (ssd < ssdBest)
							{
								ssdBest = ssd;
								xBest = x;
								yBest = y;
							}
						}
					}
				}
			}

			// propagation top
			if (position.y() != 0u && mapping.position(position.north()).isValid())
			{
				const PixelPosition south(mapping.position(position.north()).south());

				for (unsigned int y = max(0, int(south.y()) - 3); y < min(south.y() + 4u, height); ++y)
				{
					for (unsigned int x = max(0, int(south.x()) - 3); x < min(south.x() + 4u, width); ++x)
					{
						if ((y != position.y() || x != position.x()) && staticMaskData[y * staticMaskStrideElements + x] == 0xFF)
						{
							const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, PixelPosition(x, y));

							if (ssd < ssdBest)
							{
								ssdBest = ssd;
								xBest = x;
								yBest = y;
							}
						}
					}
				}
			}

			// propagation top right
			if (position.y() != 0u && position.x() + 1u < width && mapping.position(position.northEast()).isValid())
			{
				const PixelPosition southWest(mapping.position(position.northEast()).southWest());

				for (unsigned int y = max(0, int(southWest.y()) - 3); y < min(southWest.y() + 4u, height); ++y)
				{
					for (unsigned int x = max(0, int(southWest.x()) - 3); x < min(southWest.x() + 4u, width); ++x)
					{
						if ((y != position.y() || x != position.x()) && staticMaskData[y * staticMaskStrideElements + x] == 0xFF)
						{
							const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, PixelPosition(x, y));

							if (ssd < ssdBest)
							{
								ssdBest = ssd;
								xBest = x;
								yBest = y;
							}
						}
					}
				}
			}

			// propagation right
			if (position.x() + 1u < width && mapping.position(position.east()).isValid())
			{
				const PixelPosition west(mapping.position(position.east()).west());

				for (unsigned int y = max(0, int(west.y()) - 3); y < min(west.y() + 4u, height); ++y)
				{
					for (unsigned int x = max(0, int(west.x()) - 3); x < min(west.x() + 4u, width); ++x)
					{
						if ((y != position.y() || x != position.x()) && staticMaskData[y * staticMaskStrideElements + x] == 0xFF)
						{
							const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, PixelPosition(x, y));

							if (ssd < ssdBest)
							{
								ssdBest = ssd;
								xBest = x;
								yBest = y;
							}
						}
					}
				}
			}

			// propagation bottom right
			if (position.y() + 1u < height && position.x() + 1u < width && mapping.position(position.southEast()).isValid())
			{
				const PixelPosition northWest(mapping.position(position.southEast()).northWest());

				for (unsigned int y = max(0, int(northWest.y()) - 3); y < min(northWest.y() + 4u, height); ++y)
				{
					for (unsigned int x = max(0, int(northWest.x()) - 3); x < min(northWest.x() + 4u, width); ++x)
					{
						if ((y != position.y() || x != position.x()) && staticMaskData[y * staticMaskStrideElements + x] == 0xFF)
						{
							const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, PixelPosition(x, y));

							if (ssd < ssdBest)
							{
								ssdBest = ssd;
								xBest = x;
								yBest = y;
							}
						}
					}
				}
			}

			// propagation bottom
			if (position.y() + 1u < height && mapping.position(position.south()).isValid())
			{
				const PixelPosition north(mapping.position(position.south()).north());

				for (unsigned int y = max(0, int(north.y()) - 3); y < min(north.y() + 4u, height); ++y)
				{
					for (unsigned int x = max(0, int(north.x()) - 3); x < min(north.x() + 4u, width); ++x)
					{
						if ((y != position.y() || x != position.x()) && staticMaskData[y * staticMaskStrideElements + x] == 0xFF)
						{
							const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, PixelPosition(x, y));

							if (ssd < ssdBest)
							{
								ssdBest = ssd;
								xBest = x;
								yBest = y;
							}
						}
					}
				}
			}

			// propagation bottom left
			if (position.x() != 0u && position.y() + 1u < height && mapping.position(position.southWest()).isValid())
			{
				const PixelPosition northEast(mapping.position(position.southWest()).northEast());

				for (unsigned int y = max(0, int(northEast.y()) - 3); y < min(northEast.y() + 4u, height); ++y)
				{
					for (unsigned int x = max(0, int(northEast.x()) - 3); x < min(northEast.x() + 4u, width); ++x)
					{
						if ((y != position.y() || x != position.x()) && staticMaskData[y * staticMaskStrideElements + x] == 0xFF)
						{
							const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, PixelPosition(x, y));

							if (ssd < ssdBest)
							{
								ssdBest = ssd;
								xBest = x;
								yBest = y;
							}
						}
					}
				}
			}

			Scalar diagonal_4 = min(Vector2(Scalar(width), Scalar(height)).length() * Scalar(0.25), Scalar(10));
			Vector3 normal(Scalar(position.borderDirection().x()), Scalar(position.borderDirection().y()), 0);

			if (normal.normalize())
			{
				for (unsigned int n = 0u; n < 100u; ++n)
				{
					Rotation rotation(0, 0, 1, Random::scalar(randomGenerator, -Numeric::deg2rad(90), Numeric::deg2rad(90)));
					const Scalar length = Random::scalar(randomGenerator, 1, diagonal_4);
					const Vector3 offset = rotation * normal * length;

					const PixelPosition first(position.x() + Numeric::round32(offset.x()), position.y() + Numeric::round32(offset.y()));
					if (first.x() < width && first.y() < height && staticMaskData[first.y() * staticMaskStrideElements + first.x()] == 0xFF)
					{
						const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, first);

						if (ssd < ssdBest)
						{
							ssdBest = ssd;

							xBest = first.x();
							yBest = first.y();
						}
					}

					const PixelPosition second(position.x() - Numeric::round32(offset.x()), position.y() - Numeric::round32(offset.y()));
					if (second.x() < width && second.y() < height && staticMaskData[second.y() * staticMaskStrideElements + second.x()] == 0xFF)
					{
						const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, second);

						if (ssd < ssdBest)
						{
							ssdBest = ssd;

							xBest = second.x();
							yBest = second.y();
						}
					}
				}
			}

			if (ssdBest != (unsigned int)(-1))
			{
				const unsigned int iterations = 200u;
				for (unsigned int n = 0u; n < iterations; ++n)
				{
					const int xRadius = max(1, int(width - (width - 1u) * n / iterations) >> 1);
					const int yRadius = max(1, int(height - (height - 1u) * n / iterations) >> 1);

					const int offsetX = RandomI::random(randomGenerator, -xRadius, xRadius);
					const int offsetY = RandomI::random(randomGenerator, -yRadius, yRadius);

					const unsigned int randomX = xBest + offsetX;
					const unsigned int randomY = yBest + offsetY;

					if (randomX < width && randomY < height && (randomY != position.y() || randomX != position.x()) && staticMaskData[randomY * staticMaskStrideElements + randomX] == 0xFF)
					{
						const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, CV::PixelPosition(randomX, randomY));

						if (ssd < ssdBest)
						{
							ssdBest = ssd;

							xBest = randomX;
							yBest = randomY;
						}
					}
				}
			}
		}

		if (ssdBest == (unsigned int)(-1))
		{
			if (maximalBoundingBoxOffset_ == (unsigned int)(-1))
			{
				// find the best matching patch with brute force
				for (unsigned int y = 0u; y < height; ++y)
				{
					const uint8_t* const staticMaskRow = staticMask.constrow<uint8_t>(y);

					for (unsigned int x = 0u; x < width; ++x)
					{
						if ((y != position.y() || x != position.x()) && staticMaskRow[x] == 0xFF)
						{
							const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, CV::PixelPosition(x, y));

							if (ssd < ssdBest)
							{
								ssdBest = ssd;

								xBest = x;
								yBest = y;
							}
						}
					}
				}
			}
			else
			{
				const unsigned int left = max(0, int(position.x() - maximalBoundingBoxOffset_));
				const unsigned int top = max(0, int(position.y() - maximalBoundingBoxOffset_));

				const unsigned int rightEnd = min(position.x() + maximalBoundingBoxOffset_ + 1u, width);
				const unsigned int bottomEnd = min(position.y() + maximalBoundingBoxOffset_ + 1u, height);

				// find the best matching patch with brute force
				for (unsigned int y = top; y < bottomEnd; ++y)
				{
					for (unsigned int x = left; x < rightEnd; ++x)
					{
						if ((y != position.y() || x != position.x()) && staticMaskData[y * staticMaskStrideElements + x] == 0xFF)
						{
							const unsigned int ssd = SSD::template determine5x5<tChannels>(frameData, dynamicMaskData, width, height, framePaddingElements, dynamicMaskPaddingElements, position, CV::PixelPosition(x, y));

							if (ssd < ssdBest)
							{
								ssdBest = ssd;

								xBest = x;
								yBest = y;
							}
						}
					}
				}
			}
		}

		if (ssdBest == (unsigned int)(-1))
		{
			for (unsigned int y = 0u; ssdBest == (unsigned int)(-1) && y < height; ++y)
			{
				const uint8_t* dynamicMaskRow = dynamicMask.constrow<uint8_t>(y);

				for (unsigned int x = 0u; x < width; ++x)
				{
					if (dynamicMaskRow[x] == 0xFFu)
					{
						ssdBest = (unsigned int)(-2);

						xBest = x;
						yBest = y;

						break;
					}
				}
			}
		}

		if (ssdBest == (unsigned int)(-1))
		{
			return false;
		}

		ocean_assert(xBest < width && yBest < height);

		// update the color frame border pixel itself
		CVUtilities::copyPixel<tChannels>(frameData + position.y() * frameStrideElements + position.x() * tChannels, frameData + yBest * frameStrideElements + xBest * tChannels);

		for (unsigned int y = max(0, int(position.y()) - 1); y < min(position.y() + 2u, height); ++y)
		{
			for (unsigned int x = max(0, int(position.x()) - 1); x < min(position.x() + 2u, width); ++x)
			{
				CV::FrameFilterSobel::filterPixelHorizontalVertical8BitPerChannel<int16_t, tChannels>(frameData, width, height, x, y, sobelResponseData + y * sobelResponseStrideElements + x * tChannels * 2u, framePaddingElements);
			}
		}

		// find new border pixel
		PixelPositions newBorderPixels;
		newBorderPixels.reserve(4u);

		constexpr uint8_t nonMaskPixelValue = 0xFF;

		// check all pixels in the 4-neighborhood without frame border pixels as frame border pixels are mask-border pixels by definition already
		if (position.x() - 1u < width - 2u)
		{
			// north
			if (position.y() > 1u && !Segmentation::MaskAnalyzer::hasMaskNeighbor5<true>(dynamicMaskData, width, height, dynamicMaskPaddingElements, position.north(), nonMaskPixelValue))
			{
				newBorderPixels.emplace_back(position.north());
			}

			// south
			if (position.y() + 2u < height && !Segmentation::MaskAnalyzer::hasMaskNeighbor5<true>(dynamicMaskData, width, height, dynamicMaskPaddingElements, position.south(), nonMaskPixelValue))
			{
				newBorderPixels.emplace_back(position.south());
			}
		}

		if (position.y() - 1u < height - 2u)
		{
			// west
			if (position.x() > 1u && !Segmentation::MaskAnalyzer::hasMaskNeighbor5<true>(dynamicMaskData, width, height, dynamicMaskPaddingElements, position.west(), nonMaskPixelValue))
			{
				newBorderPixels.emplace_back(position.west());
			}

			// east
			if (position.x() + 2u < width && !Segmentation::MaskAnalyzer::hasMaskNeighbor5<true>(dynamicMaskData, width, height, dynamicMaskPaddingElements, position.east(), nonMaskPixelValue))
			{
				newBorderPixels.emplace_back(position.east());
			}
		}

#ifdef OCEAN_DEBUG

		for (const PixelPosition& newBorerPixel : newBorderPixels)
		{
			for (const InpaintingPixel& inpaintingPixel : inpaintingPixels)
			{
				ocean_assert(newBorerPixel != inpaintingPixel);
			}
		}

#endif // OCEAN_DEBUG

		// update the border mask frame
		ocean_assert(dynamicMask.constpixel<uint8_t>(position.x(), position.y())[0] != 0xFFu);
		ocean_assert(staticMask.constpixel<uint8_t>(xBest, yBest)[0] == 0xFFu);

		dynamicMaskData[position.y() * dynamicMaskStrideElements + position.x()] = 0xFFu;

		mapping.setPosition(position.x(), position.y(), PixelPosition(xBest, yBest));

		InpaintingPixelList changedInpaintingPixels;

		// update all inpainting positions inside the direction neighborhood, stop if all pixels have been updated
		for (InpaintingPixelList::iterator i = inpaintingPixels.begin(); i != inpaintingPixels.end(); /* noop */)
		{
			ocean_assert(*i != position);

			// check whether the point is inside the small neighborhood
			const int dx = abs(int(i->x()) - int(position.x()));
			const int dy = abs(int(i->y()) - int(position.y()));

			if (dx <= 3 && dy <= 3)
			{
				// the border direction might have changed
				changedInpaintingPixels.emplace_back(*i, InpaintingPixel::determineBorderDirection5x5(dynamicMaskData, width, height, dynamicMaskPaddingElements, *i), InpaintingPixel::determineImageOrientation5x5<tChannels>(sobelResponseData, dynamicMaskData, width, height, sobelResponseStrideElements, dynamicMaskPaddingElements, *i));

				i = inpaintingPixels.erase(i);
			}
			else
			{
				++i;
			}
		}

		// add new border pixels
		for (const PixelPosition& newBorderPixel : newBorderPixels)
		{
			changedInpaintingPixels.emplace_back(newBorderPixel, InpaintingPixel::determineBorderDirection5x5(dynamicMaskData, width, height, dynamicMaskPaddingElements, newBorderPixel), InpaintingPixel::determineImageOrientation5x5<tChannels>(sobelResponseData, dynamicMaskData, width, height, sobelResponseStrideElements, dynamicMaskPaddingElements, newBorderPixel));
		}

		changedInpaintingPixels.sort();
		inpaintingPixels.merge(changedInpaintingPixels);

#ifdef OCEAN_DEBUG
		for (const InpaintingPixel& inpaintingPixel : inpaintingPixels)
		{
			ocean_assert(inpaintingPixel == InpaintingPixel(inpaintingPixel, InpaintingPixel::determineBorderDirection5x5(dynamicMaskData, width, height, dynamicMaskPaddingElements, inpaintingPixel), InpaintingPixel::determineImageOrientation5x5<tChannels>(sobelResponseData, dynamicMaskData, width, height, sobelResponseStrideElements, dynamicMaskPaddingElements, inpaintingPixel)));
		}
#endif // OCEAN_DEBUG
	}

#ifdef OCEAN_DEBUG
	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			ocean_assert(dynamicMask.constpixel<uint8_t>(x, y)[0] == 0xFFu);
			const CV::PixelPosition& position = mapping.position(x, y);

			if (staticMask.constpixel<uint8_t>(x, y)[0] != 0xFFu)
			{
				ocean_assert(position.isValid() && position.x() < width && position.y() < height);
			}
			else
			{
				ocean_assert(!position.isValid());
			}
		}
	}
#endif

	return true;
}

}

}

}

#endif // META_OCEAN_CV_SYNTHESIS_INITIALIZER_SHRINKING_PATCH_MATCHING_I_1_H
