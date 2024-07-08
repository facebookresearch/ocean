/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_MASK_ANALYZER_H
#define META_OCEAN_CV_SEGMENTATION_MASK_ANALYZER_H

#include "ocean/cv/segmentation/Segmentation.h"

#include "ocean/base/Memory.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelBoundingBox.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/cv/segmentation/PixelContour.h"

#include <map>
#include <set>
#include <vector>

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

/**
 * This class implements functions analyzing masks, determining specific pixels in relation to masks and allowing to convert pixels to contours.
 * @ingroup cvsegmentation
 */
class OCEAN_CV_SEGMENTATION_EXPORT MaskAnalyzer
{
	public:

		/**
		 * This class implements a simple information for a block/area of mask pixels.
		 */
		class OCEAN_CV_SEGMENTATION_EXPORT MaskBlock
		{
			public:

				/**
				 * Creates an invalid block object.
				 */
				inline MaskBlock();

				/**
				 * Creates a new block object.
				 * @param position A seed point inside the mask block
				 * @param id The id of the block
				 * @param size The size of the mask block in pixels, with range [0, infinity)
				 * @param border Image border block
				 */
				inline MaskBlock(const PixelPosition& position, const unsigned int id, const unsigned int size, const bool border = true);

				/**
				 * Returns the one position of this block.
				 * @return Block position
				 */
				inline const PixelPosition& position() const;

				/**
				 * Returns the id of this block.
				 * @return Block id
				 */
				inline unsigned int id() const;

				/**
				 * Returns the size of this block in pixel.
				 * @return Block size
				 */
				inline unsigned int size() const;

				/**
				 * Returns whether this block intersects with the image border.
				 * @return True, if so
				 */
				inline bool border() const;

				/**
				 * Returns whether the size of this block is smaller than the size of the given block.
				 * @param block Second block to compare
				 * @return True, if so
				 */
				inline bool operator<(const MaskBlock& block) const;

			private:

				/// One position inside the mask block.
				PixelPosition blockPosition;

				/// Id of the mask block.
				unsigned int blockId;

				/// Size of the mask block in pixel.
				unsigned int blockSize;

				/// True, if image border block.
				bool blockBorder;
		};

		/**
		 * Definition of a vector holding mask block objects.
		 */
		typedef std::vector<MaskBlock> MaskBlocks;

	protected:

		/**
		 * Definition of process directions.
		 */
		enum ProcessDirection
		{
			/// North.
			PD_N,
			/// North west.
			PD_NW,
			/// West.
			PD_W,
			/// South west.
			PD_SW,
			/// South.
			PD_S,
			/// South east.
			PD_SE,
			/// East.
			PD_E,
			/// North east.
			PD_NE
		};

		/**
		 * Definition of a set holding pixel positions.
		 */
		typedef std::set<PixelPosition> PixelPositionSet;

		/**
		 * Definition of a vector holding index pair vectors.
		 */
		typedef std::vector<IndexPairs32> IndexPairGroups;

		/// Chessboard distance for vertical and horizontal steps
		static constexpr uint32_t distanceVerticalHorizontalC = 1u;
		/// Chessboard distance for diagonal steps
		static constexpr uint32_t distanceDiagonalC = 1u;

		/// L1 distance  for vertical and horizontal steps
		static constexpr uint32_t distanceVerticalHorizontalL1 = 1u;
		/// L1 distance for diagonal steps
		static constexpr uint32_t distanceDiagonalL1 = 2u;

		/// L2 distance for vertical and horizontal steps
		static constexpr float distanceVerticalHorizontalL2 = 0.95509f;
		/// L2 distance for diagonal steps
		static constexpr float distanceDiagonalL2 = 1.3693f;

		/**
		 * This class implements a mask island used in a sweep algorithm.
		 */
		class SweepMaskIsland
		{
			protected:

				/**
				 * Definition of a pair combining the horizontal start position (inclusive), and the horizontal end position (exclusive).
				 */
				typedef std::pair<unsigned int, unsigned int> RowSegment;

				/**
				 * Definition of a vector holding row segments.
				 */
				typedef std::vector<RowSegment> RowSegments;

			public:

				/**
				 * Default constructor.
				 */
				inline SweepMaskIsland() = default;

				/**
				 * Creates a new island object starting at a given row with given mask segment.
				 * @param currentRow The current row at which the segment appears for the first time, with range [0, infinity)
				 * @param start The horizontal start location (inclusive) of the segment within the row, with range [0, infinity)
				 * @param end The horizontal end location (exclusive) of the island within the row, with range [start + 1, infinity)
				 */
				inline SweepMaskIsland(const unsigned int currentRow, const unsigned int start, const unsigned int end);

				/**
				 * Joins a given sweep mask island with this island.
				 * The function mainly copies the row segments and updates the bounding box of this island with the union bounding box.
				 * @param sweepMask The sweep mask to join with this mask
				 */
				inline void join(const SweepMaskIsland& sweepMask);

				/**
				 * Checks whether this island intersects with a given row segment.
				 * @param start The horizontal start location (inclusive) of the segment within the row, with range [0, infinity)
				 * @param end The horizontal end location (exclusive) of the island within the row, with range [start + 1, infinity)
				 * @param useNeighborhood4 True, to use a 4-connected neighborhood when determining the mask islands; False, to use a 8-connected neighborhood
				 */
				inline bool hasIntersection(const unsigned int start, const unsigned int end, const bool useNeighborhood4) const;

				/**
				 * Adds a new row segment to this island.
				 * @param currentRow The current row at which the segment appears for the first time, with range [0, infinity)
				 * @param start The horizontal start location (inclusive) of the segment within the row, with range [0, infinity)
				 * @param end The horizontal end location (exclusive) of the island within the row, with range [start + 1, infinity)
				 */
				inline void addSegment(const unsigned int currentRow, const unsigned int start, const unsigned int end);

				/**
				 * Ends segment handling for the current row and prepares the mask for the next row.
				 * This function must be called after each row is handled.
				 */
				inline void nextRow();

				/**
				 * Returns the bounding box of this mask island.
				 * @return The mask's bounding box
				 */
				inline const PixelBoundingBox& boundingBox() const;

			protected:

				/// The segments located in the previous row.
				RowSegments previousRowSegments_;

				/// The segments located in the current row.
				RowSegments currentRowSegments_;

				/// The bounding box of this mask.
				PixelBoundingBox boundingBox_;
		};

		/**
		 * Definition of a vector holding SweepMaskIsland objects.
		 */
		typedef std::vector<SweepMaskIsland> SweepMaskIslands;

	public:

		/**
		 * Determines whether at least one neighbor pixel in the 4-neighborhood is or is not a mask pixel.
		 * The position itself is not tested.
		 * @param mask The mask frame, must be valid
		 * @param width The width of the mask in pixel, with range [1, infinity)
		 * @param height The height of the mask in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param position The position to be checked, with range [0, width)x[0, height)
		 * @param testValue The mask value to be used for testing
		 * @return True, if 'tMaskValueIsEqual && anyMaskPixel == testValue' or '!tMaskValueIsEqual && anyMaskPixel != testValue'
		 * @tparam tMaskValueIsEqual True, to check for mask values equal to 'testValue'; False, to check for mask values not equal to 'testValue'
		 * @tparam T The data type of each mask pixel
		 */
		template <bool tMaskValueIsEqual, typename T>
		static inline bool hasMaskNeighbor4(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue);

		/**
		 * Determines whether at least one neighbor pixel in the 4-neighborhood is a mask pixel with the given test position has all 8 neighbors.
		 * The position itself is not tested.
		 * @param mask The mask frame, must be valid
		 * @param width The width of the mask in pixel, with range [3, infinity)
		 * @param height The height of the mask in pixel, with range [3, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param position The position to be checked inside the frame, with range [1, width - 1)x[1, height - 1)
		 * @param testValue The mask value of a non-mask pixel
		 * @return True, if 'tMaskValueIsEqual && anyMaskPixel == testValue' or '!tMaskValueIsEqual && anyMaskPixel != testValue'
		 * @tparam T The data type of each mask pixel
		 * @tparam tMaskValueIsEqual True, to check for mask values equal to 'testValue'; False, to check for mask values not equal to 'testValue'
		 */
		template <bool tMaskValueIsEqual, typename T>
		static inline bool hasMaskNeighbor4Center(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue);

		/**
		 * Determines whether at least one neighbor pixel in the 4-neighborhood (+ center pixel) is or is not a mask pixel.
		 * The position itself is not tested.
		 * @param mask The mask frame, must be valid
		 * @param width The width of the mask in pixel, with range [1, infinity)
		 * @param height The height of the mask in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param position The position to be checked, with range [0, width)x[0, height)
		 * @param testValue The mask value to be used for testing
		 * @return True, if 'tMaskValueIsEqual && anyMaskPixel == testValue' or '!tMaskValueIsEqual && anyMaskPixel != testValue'
		 * @tparam T The data type of each mask pixel
		 * @tparam tMaskValueIsEqual True, to check for mask values equal to 'testValue'; False, to check for mask values not equal to 'testValue'
		 */
		template <bool tMaskValueIsEqual, typename T>
		static inline bool hasMaskNeighbor5(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue);

		/**
		 * Determines whether at least one neighbor pixel in the 4-neighborhood (+ center pixel) is a mask pixel with the given test position has all 8 neighbors.
		 * The position itself is not tested.
		 * @param mask The mask frame, must be valid
		 * @param width The width of the mask in pixel, with range [3, infinity)
		 * @param height The height of the mask in pixel, with range [3, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param position The position to be checked inside the frame, with range [1, width - 1)x[1, height - 1)
		 * @param testValue The mask value of a non-mask pixel
		 * @return True, if 'tMaskValueIsEqual && anyMaskPixel == testValue' or '!tMaskValueIsEqual && anyMaskPixel != testValue'
		 * @tparam T The data type of each mask pixel
		 * @tparam tMaskValueIsEqual True, to check for mask values equal to 'testValue'; False, to check for mask values not equal to 'testValue'
		 */
		template <bool tMaskValueIsEqual, typename T>
		static inline bool hasMaskNeighbor5Center(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue);

		/**
		 * Determines whether at least one neighbor pixel in the 8-neighborhood is or is not a mask pixel.
		 * The position itself is not tested.
		 * @param mask The mask frame, must be valid
		 * @param width The width of the mask in pixel, with range [1, infinity)
		 * @param height The height of the mask in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param position The position to be checked, with range [0, width)x[0, height)
		 * @param testValue The mask value to be used for testing
		 * @return True, if 'tMaskValueIsEqual && anyMaskPixel == testValue' or '!tMaskValueIsEqual && anyMaskPixel != testValue'
		 * @tparam T The data type of each mask pixel
		 * @tparam tMaskValueIsEqual True, to check for mask values equal to 'testValue'; False, to check for mask values not equal to 'testValue'
		 */
		template <bool tMaskValueIsEqual, typename T>
		static inline bool hasMaskNeighbor8(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue);

		/**
		 * Determines whether at least one neighbor pixel in the 8-neighborhood is a mask pixel with the given test position has all 8 neighbors.
		 * The position itself is not tested.
		 * @param mask The mask frame, must be valid
		 * @param width The width of the mask in pixel, with range [3, infinity)
		 * @param height The height of the mask in pixel, with range [3, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param position The position to be checked inside the frame, with range [1, width - 1)x[1, height - 1)
		 * @param testValue The mask value of a non-mask pixel
		 * @return True, if 'tMaskValueIsEqual && anyMaskPixel == testValue' or '!tMaskValueIsEqual && anyMaskPixel != testValue'
		 * @tparam T The data type of each mask pixel
		 * @tparam tMaskValueIsEqual True, to check for mask values equal to 'testValue'; False, to check for mask values not equal to 'testValue'
		 */
		template <bool tMaskValueIsEqual, typename T>
		static inline bool hasMaskNeighbor8Center(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue);

		/**
		 * Determines whether at least one neighbor pixel in the 8-neighborhood (+ center pixel) is or is not a mask pixel.
		 * The position itself is not tested.
		 * @param mask The mask frame, must be valid
		 * @param width The width of the mask in pixel, with range [1, infinity)
		 * @param height The height of the mask in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param position The position to be checked, with range [0, width)x[0, height)
		 * @param testValue The mask value to be used for testing
		 * @return True, if 'tMaskValueIsEqual && anyMaskPixel == testValue' or '!tMaskValueIsEqual && anyMaskPixel != testValue'
		 * @tparam T The data type of each mask pixel
		 * @tparam tMaskValueIsEqual True, to check for mask values equal to 'testValue'; False, to check for mask values not equal to 'testValue'
		 */
		template <bool tMaskValueIsEqual, typename T>
		static inline bool hasMaskNeighbor9(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue);

		/**
		 * Determines whether at least one neighbor pixel in the 8-neighborhood (+ center pixel) is a mask pixel with the given test position has all 8 neighbors.
		 * The position itself is not tested.
		 * @param mask The mask frame, must be valid
		 * @param width The width of the mask in pixel, with range [3, infinity)
		 * @param height The height of the mask in pixel, with range [3, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param position The position to be checked inside the frame, with range [1, width - 1)x[1, height - 1)
		 * @param testValue The mask value of a non-mask pixel
		 * @return True, if 'tMaskValueIsEqual && anyMaskPixel == testValue' or '!tMaskValueIsEqual && anyMaskPixel != testValue'
		 * @tparam T The data type of each mask pixel
		 * @tparam tMaskValueIsEqual True, to check for mask values equal to 'testValue'; False, to check for mask values not equal to 'testValue'
		 */
		template <bool tMaskValueIsEqual, typename T>
		static inline bool hasMaskNeighbor9Center(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue);

		/**
		 * Computes a per pixel Chessboard distance to the nearest pixel with a reference value using 3-by-3 neighborhood
		 * @note This function is similar to `cv::distanceTransform()` from OpenCV
		 * @param source The frame for which a distance map will be computed, must be valid
		 * @param width The width of the source frame, range: [1, infinity)
		 * @param height The height of the source frame, range: [1, infinity)
		 * @param target The location where the computed distance map will be stored, must be valid and have the same size the source frame, this data will only be valid if the return status is `Success`
		 * @param buffer An optional buffer of the size `(width + 2) * (height + 2)`. If not provided, memory will be allocated internally (which is more expensive!)
		 * @param referenceValue Distance values pixels will be computed to the closest pixel with this reference value, range: [0, 255]
		 * @param sourcePaddingElements Optional number of padding elements in the source frame, range: [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements in the target frame, range: [0, infinity)
		 * @return True on success, otherwise false
		 */
		static inline bool computeChessboardDistanceTransform8Bit(const uint8_t* source, const uint32_t width, const uint32_t height, uint32_t* target, uint32_t* buffer = nullptr, const uint8_t referenceValue = 0u, const uint32_t sourcePaddingElements = 0u, const uint32_t targetPaddingElements = 0u);

		/**
		 * Computes a per pixel L1 distance to the nearest pixel with a reference value using 3-by-3 neighborhood
		 * @note This function is similar to `cv::distanceTransform()` from OpenCV
		 * @param source The frame for which a distance map will be computed, must be valid
		 * @param width The width of the source frame, range: [1, infinity)
		 * @param height The height of the source frame, range: [1, infinity)
		 * @param target The location where the computed distance map will be stored, must be valid and have the same size the source frame, this data will only be valid if the return status is `Success`
		 * @param buffer An optional buffer of the size `(width + 2) * (height + 2)`. If not provided, memory will be allocated internally (which is more expensive!)
		 * @param referenceValue Distance values pixels will be computed to the closest pixel with this reference value, range: [0, 255]
		 * @param sourcePaddingElements Optional number of padding elements in the source frame, range: [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements in the target frame, range: [0, infinity)
		 * @return True on success, otherwise false
		 */
		static inline bool computeL1DistanceTransform8Bit(const uint8_t* source, const uint32_t width, const uint32_t height, uint32_t* target, uint32_t* buffer = nullptr, const uint8_t referenceValue = 0u, const uint32_t sourcePaddingElements = 0u, const uint32_t targetPaddingElements = 0u);

		/**
		 * Computes a per pixel (approximated) L2 distance to the nearest pixel with a reference value using 3-by-3 neighborhood
		 * @note This function is similar to `cv::distanceTransform()` from OpenCV
		 * @param source The frame for which a distance map will be computed, must be valid
		 * @param width The width of the source frame, range: [1, infinity)
		 * @param height The height of the source frame, range: [1, infinity)
		 * @param target The location where the computed distance map will be stored, must be valid and have the same size the source frame, this data will only be valid if the return status is `Success`
		 * @param buffer An optional buffer of the size `(width + 2) * (height + 2)`. If not provided, memory will be allocated internally (which is more expensive!)
		 * @param referenceValue Distance values pixels will be computed to the closest pixel with this reference value, range: [0, 255]
		 * @param sourcePaddingElements Optional number of padding elements in the source frame, range: [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements in the target frame, range: [0, infinity)
		 * @return True on success, otherwise false
		 */
		static inline bool computeL2DistanceTransform8Bit(const uint8_t* source, const uint32_t width, const uint32_t height, float* target, uint32_t* buffer = nullptr, const uint8_t referenceValue = 0u, const uint32_t sourcePaddingElements = 0u, const uint32_t targetPaddingElements = 0u);

		/**
		 * Analyzes an 8 bit binary mask frame and separates the pixels into individual blocks of joined sub-masks.
		 * Two neighboring mask blocks count as two separate sub-masks if both mask do not share a common mask pixel in a 4-neighborhood.<br>
		 * Beware: This function does not seek for individual mask values but for not joined sub-masks.
		 * @param mask Given binary 8 bit mask frame, pixel values not equal to 0xFF count as mask pixels, must be valid
		 * @param width The width of the entire mask frame in pixel, with range [1, infinity)
		 * @param height The height of the entire mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param separation Resulting separation frame (1 channel 32 bit) holding the mask block ids, with same dimension and pixel origin as the mask frame
		 * @param separationPaddingElements The number of padding elements at the end of each separation row, in elements, with range [0, infinity)
		 * @param blocks Resulting separation block information, one object for each found mask block
		 * @see analyzeNonMaskSeparation8Bit().
		 */
		static void analyzeMaskSeparation8Bit(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, uint32_t* separation, const unsigned int separationPaddingElements, MaskBlocks& blocks);

		/**
		 * Analyzes an 8 bit binary mask frame and separates the pixels into individual blocks of not joined sub-areas covering non-masks.
		 * Two neighboring non-mask blocks count as two separate sub-masks if both areas do not share a common non-mask pixel in a 4-neighborhood.<br>
		 * Beware: This function does not seek for individual non-mask values but for not joined sub-areas.
		 * @param mask Given binary 8 bit mask frame, pixel values not equal to 0xFF count as mask pixels, must be valid
		 * @param width The width of the entire mask frame in pixel, with range [1, infinity)
		 * @param height The height of the entire mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param separation Resulting separation frame (1 channel 32 bit) holding the mask block ids, with same dimension and pixel origin as the mask frame
		 * @param separationPaddingElements The number of padding elements at the end of each separation row, in elements, with range [0, infinity)
		 * @param blocks Resulting separation block information, one object for each found non-mask block
		 * @see analyzeMaskSeparation8Bit().
		 */
		static void analyzeNonMaskSeparation8Bit(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, uint32_t* separation, const unsigned int separationPaddingElements, MaskBlocks& blocks);

		/**
		 * Determines all outline-4 pixels in an 8 bit mask frame.
		 * @param mask The binary 8 bit mask frame in which the outline-4 pixels will be determined, pixel values not equal to 0xFF count as mask pixels, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param outlinePixels4 Resulting outline-4 pixels, the pixel itself is not a mask pixel but has at least one neighbor mask pixel in the four-neighborhood
		 * @param boundingBox Optional bounding box object shrinking the operation area, outline pixels will be detected inside the given bounding box and one pixel outside the given bounding box
		 * @param nonMaskValue The pixel value of a mask pixel not belonging to the mask
		 */
		static void findOutline4(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions& outlinePixels4, const PixelBoundingBox& boundingBox = PixelBoundingBox(), const uint8_t nonMaskValue = 0xFF);

		/**
		 * Determines all border pixels in an 8 bit mask frame for a 4-neighborhood.
		 * A border pixel itself is a mask pixel but not all 4-neighborhood pixels are mask pixels; a mask pixel at the frame's border is a mask-border pixel.
		 * @param mask Given binary 8 bit mask frame, pixel values not equal to `nonMaskValue` count as mask pixels, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param borderPixels Resulting border pixels
		 * @param boundingBox Optional bounding box object shrinking the operation area
		 * @param worker Optional worker object to distribute the computation
		 * @param nonMaskValue Optional pixel value that is to be interpreted as not belonging to the mask and which will not be enclosed by the border pixels, range: [0, 255], default: 255
		 */
		static void findBorderPixels4(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions& borderPixels, const PixelBoundingBox& boundingBox = PixelBoundingBox(), Worker* worker = nullptr, const uint8_t nonMaskValue = 255u);

		/**
		 * Determines all border pixels in an 8 bit mask frame for a 8-neighborhood.
		 * A border pixel itself is a mask pixel but not all 8-neighborhood pixels are mask pixels; a mask pixel at the frame's border is a mask-border pixel.
		 * @param mask Given binary 8 bit mask frame, pixel values not equal to `nonMaskValue` count as mask pixels, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param borderPixels Resulting border pixels
		 * @param boundingBox Optional bounding box object shrinking the operation area
		 * @param worker Optional worker object to distribute the computation
		 * @param nonMaskValue Optional pixel value that is to be interpreted as not belonging to the mask and which will not be enclosed by the border pixels, range: [0, 255], default: 255
		 */
		static void findBorderPixels8(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions& borderPixels, const PixelBoundingBox& boundingBox = PixelBoundingBox(), Worker* worker = nullptr, const uint8_t nonMaskValue = 255u);

		/**
		 * Determines the pixels in an 8 bit mask frame not having the identical pixel values in an 4-neighborhood.
		 * @param mask Given binary 8 bit mask frame, must be valid
		 * @param width The width of the entire mask frame in pixel, with range [2, infinity)
		 * @param height The height of the entire mask frame in pixel, with range [2, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param nonUniquePixels The resulting non unique pixels
		 * @param boundingBox Optional bounding box object shrinking the operation area
		 */
		static void findNonUniquePixels4(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions& nonUniquePixels, const PixelBoundingBox& boundingBox = PixelBoundingBox());

		/**
		 * Determines the pixels in an 8 bit mask frame not having the identical pixel values in an 8-neighborhood.
		 * @param mask Given binary 8 bit mask frame, must be valid
		 * @param width The width of the entire mask frame in pixel, with range [2, infinity)
		 * @param height The height of the entire mask frame in pixel, with range [2, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param nonUniquePixels The resulting non unique pixels
		 * @param boundingBox Optional bounding box object shrinking the operation area
		 */
		static void findNonUniquePixels8(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions& nonUniquePixels, const PixelBoundingBox& boundingBox = PixelBoundingBox());

		/**
		 * Converts an unordered set of pixel positions (providing all pixels at the mask border either inside the mask or outside the mask) to one inner our one outer mask contours.
		 * The given pixel positions can either be border pixels (mask pixels in the 4-neighborhood not being a mask pixel) or can be outline-4 pixels of several binary masks.<br>
		 * The resulting contour will include the most left pixel of the provided pixel positions.<br>
		 * Beware: If the mask is connected/intersects the boundary of a frame, then provided pixels must also include the pixel position exactly lying on the boundary of the frame.
		 * However, boundary contour pixels will not be included in the final contour.<br>
		 * Nevertheless, potential pixels lying on the frame boundary will not be part of the resulting contour, there will be a gap for all of these pixel positions.
		 * @param pixels The unordered set of pixel positions for which the inner or outer contour will be determined, with range [-1, width]x[-1, height]
		 * @param width The width of the frame in which the pixel positions (and the resulting contour) is defined, in pixel, with range [1, infinity)
		 * @param height The height of the frame in which the pixel positions (and the resulting contour) is defined, in pixel with range [1, infinity)
		 * @param contour Resulting mask contour including the most left pixel position
		 * @param remainingPixels Optional resulting set of pixels that are not part of the resulting contour and that may be part of another contour
		 * @return True, if succeeded
		 * @see pixels2contours().
		 */
		static bool pixels2contour(const PixelPositions& pixels, const unsigned int width, const unsigned int height, PixelPositions& contour, PixelPositions* remainingPixels = nullptr);

		/**
		 * Converts an unordered set of pixel positions (providing all pixels at the mask border either inside the mask or outside the mask) to inner and outer mask contours.
		 * The given pixel positions can either be border pixels (mask pixels in the 4-neighborhood not being a mask pixel) or can be outline-4 pixels of several binary masks.<br>
		 * The mask can include holds with non-mask pixels/areas.<br>
		 * Beware: If the mask is connected/intersects the boundary of a frame, then provided pixels must also include the pixel position exactly lying on the boundary of the frame.<br>
		 * Nevertheless, potential pixels lying on the frame boundary will not be part of the resulting contour, there will be a gap for all of these pixel positions.
		 * @param mask The mask frame for which the contours will be determined, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param pixels The unordered set of pixel positions for which the inner and outer contours will be determined
		 * @param outerContours The resulting set of outer mask contours, outer contours enclose a mask and are not defined within a mask
		 * @param innerContours The resulting set of inner mask contours, inner contours are defined within a mask and cut a hole (positive or negative into the outer mask)
		 * @param maskValue Optional pixel value that is to be interpreted as belonging to the mask and which will be enclosed by the border pixels, range: [0, 255], default: 0
		 * @return True, if succeeded
		 * @see pixels2contour(), MaskCreator::denseContour2inclusiveMaskXOR(), MaskCreator::denseContour2exclusiveMaskXOR().
		 */
		static bool pixels2contours(const uint8_t* mask, const unsigned int width, const unsigned int height, const PixelPositions& pixels, PixelContours& outerContours, PixelContours& innerContours, const uint8_t maskValue = 0u);

		/**
		 * This functions checks whether a given contour is an outer contour or an inner contour.
		 * A contour is an outer contour if the left start position is at the left frame border, or if the west pixel of the most left pixel is a non-mask pixel.
		 * @param mask The mask frame where pixels with value `maskValue` define mask pixels, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param contour Pixel contour that will be checked
		 * @param maskValue Optional pixel value that is to be interpreted as belonging to the mask and which will not enclosed by the border pixels, range: [0, 255], default: 0
		 * @return True, if so
		 */
		static inline bool isOuterContour(const uint8_t* mask, const unsigned int width, const PixelContour& contour, const uint8_t maskValue = 0u);

		/**
		 * Counts the number of mask pixels.
		 * @param mask The mask frame in which the mask pixels will be counted, pixels with 0xFF value are non-mask pixels, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param boundingBox Optional bounding box to speedup the computation
		 * @param nonMaskValue The mask value of a non-mask pixel, with range [0, 255]
		 * @return Number of mask pixels, with range [0, width * height]
		 */
		static unsigned int countMaskPixels(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelBoundingBox& boundingBox = PixelBoundingBox(), const uint8_t nonMaskValue = 0xFFu);

		/**
		 * Determines the distance to the mask border in an 8 bit mask frame.<br>
		 * The 8 bit mask frame can hold two different pixel values 0x00 and 0xFF. A mask pixel is defined by 0x00.<br>
		 * The minimal distance for all mask pixels (with value 0x00) to their nearest non-mask pixels (with value 0xFF) is determined.<br>
		 * All mask pixels will receive their distance values: 1 means that the nearest mask border is one pixel off, 2 means two pixels, ...<br>
		 * Distances larger than the specified iteration number are not determined.<br>
		 * Thus, resulting mask pixels with value 0x00 have a larger distance to the mask border than the specified iteration number.
		 * @param mask The 8 bit mask frame the distances are calculated for (and assigned), must be valid
		 * @param width The width of the mask frame in pixel, with range [3, infinity)
		 * @param height The height of the mask frame in pixel, with range  [3, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param iterations Number of maximal border distance to be calculated, with range [0, 254]
		 * @param assignFinal True, to assign each mask pixel a minimal distance after the final iteration
		 * @param boundingBox Optional bounding box to speed up the computation, invalid to process the entire mask
		 * @param worker Optional worker object to distribute the computation
		 */
		static void determineDistancesToBorder8Bit(uint8_t* mask, const unsigned int width, const unsigned int height, unsigned int maskPaddingElements, const unsigned int iterations, const bool assignFinal, const PixelBoundingBox& boundingBox, Worker* worker = nullptr);

		/**
		 * Determines the axis-aligned bounding boxes of all isolated mask islands in a binary (but 8-bit) mask frame.
		 * The function determines the individual bounding boxes of connected components (mask pixels).
		 * This function applies a row-sweep approach to determine the individual mask blocks in individual rows and joins the results from the current row with previous rows.
		 * @param mask The binary mask in which the mask islands are located, must be valid
		 * @param width The width of the mask frame, in pixel, with range [1, infinity)
		 * @param height The height of the mask frame, in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param maskValue The value of a mask pixel, with range [0, 255]
		 * @param useNeighborhood4 True, to use a 4-connected neighborhood when determining the mask islands; False, to use a 8-connected neighborhood
		 * @return The resulting bounding boxes of all isolated mask islands
		 * @see CV::MaskAnalyzer::detectBoundingBox().
		 */
		static PixelBoundingBoxes detectBoundingBoxes(const uint8_t* const mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t maskValue, const bool useNeighborhood4 = true);

	protected:

		/**
		 * Determines the border pixels in a subset of a 8 bit mask frame for a 4-neighborhood.
		 * @param mask Given binary 8 bit mask frame, pixel values not equal `nonMaskValue` count as mask pixels
		 * @param width The width of the entire mask frame in pixel
		 * @param height The height of the entire mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param borderPixelsArray Array of resulting border pixels, the pixel itself is a mask pixel but not all 4-neighborhood pixels are mask pixels
		 * @param nonMaskValue Pixel value that is to be interpreted as not belonging to the mask and which will not be enclosed by the border pixels, range: [0, 255]
		 * @param firstColumn First column to be handled, with range [0, width - 1]
		 * @param numberColumns Number of columns to be handled, with range [1u, width - firstColumn]
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1u, height - firstRow]
		 * @param threadId Id of the execution thread, 0 if no multi-threading is applied
		 */
		static void findBorderPixels4Subset(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions* borderPixelsArray, const uint8_t nonMaskValue, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, const unsigned int threadId);

		/**
		 * Determines the border pixels in a subset of a 8 bit mask frame for a 8-neighborhood.
		 * @param mask Given binary 8 bit mask frame, pixel values not equal `nonMaskValue` count as mask pixels
		 * @param width The width of the entire mask frame in pixel
		 * @param height The height of the entire mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param borderPixelsArray Array of resulting border pixels, the pixel itself is a mask pixel but not all 8-neighborhood pixels are mask pixels
		 * @param nonMaskValue Pixel value that is to be interpreted as not belonging to the mask and which will not be enclosed by the border pixels, range: [0, 255]
		 * @param firstColumn First column to be handled, with range [0, width - 1]
		 * @param numberColumns Number of columns to be handled, with range [1u, width - firstColumn]
		 * @param firstRow First row to be handled, with range [0, height - 1]
		 * @param numberRows Number of rows to be handled, with range [1u, height - firstRow]
		 * @param threadId Id of the execution thread, 0 if no multi-threading is applied
		 */
		static void findBorderPixels8Subset(const uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, PixelPositions* borderPixelsArray, const uint8_t nonMaskValue, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows, const unsigned int threadId);

		/**
		 * Determines the distance to the mask border in an 8 bit mask frame.<br>
		 * The 8 bit mask frame may hold two different pixel values 0x00 and 0xFF. A mask pixel is defined by 0x00.<br>
		 * The minimal distance for all mask pixels (with value 0x00) to their nearest non-mask pixels (with value 0xFF) are determined.<br>
		 * All mask pixels will receive their distance values: 1 means that the nearest mask border is one pixel off, 2 means two pixels, ...<br>
		 * Distances larger than the specified iteration number are not determined.<br>
		 * Thus, resulting mask pixels with value 0x00 have a larger distance to the mask border than the specified iteration number.
		 * @param mask The 8 bit mask frame the distances are calculated for (and assigned), must be valid
		 * @param width The width of the mask frame in pixel, with range [3, infinity)
		 * @param height The height of the mask frame in pixel, with range  [3, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param searchValue Pixel value to be searched
		 * @param resultValue Resulting value for neighboring pixels of the search value
		 * @param firstColumn First column to be handled
		 * @param numberColumns Number of columns to be handled
		 * @param firstRow First row to be handled
		 * @param numberRows Number of rows to be handled
		 */
		static void determineDistancesToBorder8BitSubset(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t searchValue, const uint8_t resultValue, const unsigned int firstColumn, const unsigned int numberColumns, const unsigned int firstRow, const unsigned int numberRows);

		/**
		 * Computes a per pixel distance to the nearest pixel with a reference value using 3-by-3 neighborhood
		 * @param source The frame for which a distance map will be computed, must be valid
		 * @param width The width of the source frame, range: [1, infinity)
		 * @param height The height of the source frame, range: [1, infinity)
		 * @param target The location where the computed distance map will be stored, must be valid and have the same size the source frame, this data will only be valid if the return status is `Success`
		 * @param buffer An optional buffer of the size `(width + 2) * (height + 2)`. If not provided, memory will be allocated internally (which is more expensive!)
		 * @param distanceVerticalHorizontal The cost of a step in horizontal/vertical direction, range: (1, infinity)
		 * @param distanceDiagonal The cost of a step in diagonal direction, range: (1, infinity)
		 * @param referenceValue Distance values pixels will be computed to the closest pixel with this reference value, range: [0, 255]
		 * @param sourcePaddingElements Optional number of padding elements in the source frame, range: [0, infinity)
		 * @param targetPaddingElements Optional number of padding elements in the target frame, range: [0, infinity)
		 * @return True on success, otherwise false
		 * @tparam TDistanceType The data type of the resulting distance map, must be integral or floating-point
		 */
		template <typename TDistanceType>
		static bool computeDistanceTransform8Bit(const uint8_t* source, const uint32_t width, const uint32_t height, TDistanceType* target, uint32_t* buffer, const TDistanceType distanceVerticalHorizontal, const TDistanceType distanceDiagonal, const uint8_t referenceValue = 0u, const uint32_t sourcePaddingElements = 0u, const uint32_t targetPaddingElements = 0u);

		/**
		 * Returns whether two values are identical or not identical.
		 * @param valueA The first value to compare
		 * @param valueB The second value to compare
		 * @return True, if so
		 * @tparam True, if both values need to be identical; False, if both values must not be identical
		 */
		template <typename T, bool tMaskValueIsEqual>
		static inline bool compareValues(const T valueA, const T valueB);
};

inline MaskAnalyzer::MaskBlock::MaskBlock() :
	blockPosition(),
	blockId(0u),
	blockSize(0u)
{
	// nothing to do here
}

inline MaskAnalyzer::MaskBlock::MaskBlock(const PixelPosition& position, const unsigned int id, const unsigned int size, const bool border) :
	blockPosition(position),
	blockId(id),
	blockSize(size),
	blockBorder(border)
{
	// nothing to do here
}

inline const PixelPosition& MaskAnalyzer::MaskBlock::position() const
{
	return blockPosition;
}

inline unsigned int MaskAnalyzer::MaskBlock::id() const
{
	return blockId;
}

inline unsigned int MaskAnalyzer::MaskBlock::size() const
{
	return blockSize;
}

inline bool MaskAnalyzer::MaskBlock::border() const
{
	return blockBorder;
}

inline bool MaskAnalyzer::MaskBlock::operator<(const MaskBlock& block) const
{
	return blockSize < block.blockSize;
}

inline MaskAnalyzer::SweepMaskIsland::SweepMaskIsland(const unsigned int currentRow, const unsigned int start, const unsigned int end)
{
	ocean_assert(start < end);

	previousRowSegments_.reserve(8);
	currentRowSegments_.reserve(8);

	addSegment(currentRow, start, end);
}

inline void MaskAnalyzer::SweepMaskIsland::join(const SweepMaskIsland& sweepMaskIsland)
{
	boundingBox_ = boundingBox_ || sweepMaskIsland.boundingBox_;

	previousRowSegments_.insert(previousRowSegments_.cend(), sweepMaskIsland.previousRowSegments_.cbegin(), sweepMaskIsland.previousRowSegments_.cend());
	currentRowSegments_.insert(currentRowSegments_.cend(), sweepMaskIsland.currentRowSegments_.cbegin(), sweepMaskIsland.currentRowSegments_.cend());
}

inline bool MaskAnalyzer::SweepMaskIsland::hasIntersection(const unsigned int start, const unsigned int end, const bool useNeighborhood4) const
{
	ocean_assert(start < end);

	for (const RowSegment& previousRowSegment : previousRowSegments_)
	{
		const unsigned int& previousSegmentStart = previousRowSegment.first;
		const unsigned int& previousSegmentEnd = previousRowSegment.second;

		if ((useNeighborhood4 && start < previousSegmentEnd && end > previousSegmentStart) || (!useNeighborhood4 && start <= previousSegmentEnd && end >= previousSegmentStart))
		{
			return true;
		}
	}

	return false;
}

inline void MaskAnalyzer::SweepMaskIsland::addSegment(const unsigned int currentRow, const unsigned int start, const unsigned int end)
{
	ocean_assert(start < end);

	currentRowSegments_.emplace_back(start, end);

	boundingBox_ += PixelPosition(start, currentRow);
	boundingBox_ += PixelPosition(end - 1u, currentRow);
}

inline void MaskAnalyzer::SweepMaskIsland::nextRow()
{
	std::swap(currentRowSegments_, previousRowSegments_);

	currentRowSegments_.clear();
}

inline const PixelBoundingBox& MaskAnalyzer::SweepMaskIsland::boundingBox() const
{
	return boundingBox_;
}

template <bool tMaskValueIsEqual, typename T>
inline bool MaskAnalyzer::hasMaskNeighbor4(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);
	ocean_assert(position.x() < width && position.y() < height);

	ocean_assert((position.x() - 1u < width - 2u && position.y() - 1u < height - 2u) == (position.x() >= 1u && position.x() + 1u < width && position.y() >= 1u && position.y() + 1u < height));
	if (position.x() - 1u < width - 2u && position.y() - 1u < height - 2u)
	{
		return hasMaskNeighbor4Center<tMaskValueIsEqual, T>(mask, width, height, maskPaddingElements, position, testValue);
	}

	const unsigned int maskStrideElements = width + maskPaddingElements;

	mask += position.y() * maskStrideElements + position.x();

	if (position.x() != 0u && compareValues<T, tMaskValueIsEqual>(*(mask - 1), testValue))
	{
		return true;
	}

	if (position.x() + 1u < width && compareValues<T, tMaskValueIsEqual>(*(mask + 1), testValue))
	{
		return true;
	}

	if (position.y() != 0u && compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements), testValue))
	{
		return true;
	}

	if (position.y() + 1u < height && compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements), testValue))
	{
		return true;
	}

	return false;
}

template <bool tMaskValueIsEqual, typename T>
inline bool MaskAnalyzer::hasMaskNeighbor4Center(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue)
{
	ocean_assert(mask != nullptr && height >= 3u && height >= 3u);
	ocean_assert(position.x() >= 1u && position.x() + 1u < width);
	ocean_assert_and_suppress_unused(position.y() >= 1u && position.y() + 1u < height, height);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	mask += position.y() * maskStrideElements + position.x();

	return compareValues<T, tMaskValueIsEqual>(*(mask - 1), testValue)
				|| compareValues<T, tMaskValueIsEqual>(*(mask + 1), testValue)
				|| compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements), testValue)
				|| compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements), testValue);
}

template <bool tMaskValueIsEqual, typename T>
inline bool MaskAnalyzer::hasMaskNeighbor5(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);
	ocean_assert(position.x() < width && position.y() < height);

	ocean_assert((position.x() - 1u < width - 2u && position.y() - 1u < height - 2u) == (position.x() >= 1u && position.x() + 1u < width && position.y() >= 1u && position.y() + 1u < height));
	if (position.x() - 1u < width - 2u && position.y() - 1u < height - 2u)
	{
		return hasMaskNeighbor5Center<tMaskValueIsEqual, T>(mask, width, height, maskPaddingElements, position, testValue);
	}

	const unsigned int maskStrideElements = width + maskPaddingElements;

	mask += position.y() * maskStrideElements + position.x();

	if (compareValues<T, tMaskValueIsEqual>(*mask, testValue))
	{
		return true;
	}

	if (position.x() != 0u && compareValues<T, tMaskValueIsEqual>(*(mask - 1), testValue))
	{
		return true;
	}

	if (position.x() + 1u < width && compareValues<T, tMaskValueIsEqual>(*(mask + 1), testValue))
	{
		return true;
	}

	if (position.y() != 0u && compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements), testValue))
	{
		return true;
	}

	if (position.y() + 1u < height && compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements), testValue))
	{
		return true;
	}

	return false;
}

template <bool tMaskValueIsEqual, typename T>
inline bool MaskAnalyzer::hasMaskNeighbor5Center(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue)
{
	ocean_assert(mask != nullptr && height >= 3u && height >= 3u);
	ocean_assert(position.x() >= 1u && position.x() + 1u < width);
	ocean_assert_and_suppress_unused(position.y() >= 1u && position.y() + 1u < height, height);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	mask += position.y() * maskStrideElements + position.x();

	return compareValues<T, tMaskValueIsEqual>(*mask, testValue)
				|| compareValues<T, tMaskValueIsEqual>(*(mask - 1), testValue)
				|| compareValues<T, tMaskValueIsEqual>(*(mask + 1), testValue)
				|| compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements), testValue)
				|| compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements), testValue);
}

template <bool tMaskValueIsEqual, typename T>
inline bool MaskAnalyzer::hasMaskNeighbor8(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);
	ocean_assert(position.x() < width && position.y() < height);

	ocean_assert((position.x() - 1u < width - 2u && position.y() - 1u < height - 2u) == (position.x() >= 1u && position.x() + 1u < width && position.y() >= 1u && position.y() + 1u < height));
	if (position.x() - 1u < width - 2u && position.y() - 1u < height - 2u)
	{
		return hasMaskNeighbor8Center<tMaskValueIsEqual, T>(mask, width, height, maskPaddingElements, position, testValue);
	}

	const unsigned int maskStrideElements = width + maskPaddingElements;

	mask += position.y() * maskStrideElements + position.x();

	if (position.x() != 0u)
	{
		if (compareValues<T, tMaskValueIsEqual>(*(mask - 1), testValue))
		{
			return true;
		}

		if (position.y() != 0u && compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements - 1u), testValue))
		{
			return true;
		}

		if (position.y() + 1u < height && compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements - 1u), testValue))
		{
			return true;
		}
	}

	if (position.x() + 1u < width)
	{
		if (compareValues<T, tMaskValueIsEqual>(*(mask + 1), testValue))
		{
			return true;
		}

		if (position.y() != 0u && compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements + 1u), testValue))
		{
			return true;
		}

		if (position.y() + 1u < height && compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements + 1u), testValue))
		{
			return true;
		}
	}

	if (position.y() != 0u && compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements), testValue))
	{
		return true;
	}

	if (position.y() + 1u < height && compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements), testValue))
	{
		return true;
	}

	return false;
}

template <bool tMaskValueIsEqual, typename T>
inline bool MaskAnalyzer::hasMaskNeighbor8Center(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue)
{
	ocean_assert(mask != nullptr && height >= 3u && height >= 3u);
	ocean_assert(position.x() >= 1u && position.x() + 1u < width);
	ocean_assert_and_suppress_unused(position.y() >= 1u && position.y() + 1u < height, height);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	mask += position.y() * maskStrideElements + position.x();

	return compareValues<T, tMaskValueIsEqual>(*(mask - 1), testValue) || compareValues<T, tMaskValueIsEqual>(*(mask + 1), testValue)
				|| compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements - 1), testValue) || compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements), testValue) || compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements + 1), testValue)
				|| compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements - 1), testValue) || compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements), testValue) || compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements + 1), testValue);
}

template <bool tMaskValueIsEqual, typename T>
inline bool MaskAnalyzer::hasMaskNeighbor9(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);
	ocean_assert(position.x() < width && position.y() < height);

	ocean_assert((position.x() - 1u < width - 2u && position.y() - 1u < height - 2u) == (position.x() >= 1u && position.x() + 1u < width && position.y() >= 1u && position.y() + 1u < height));
	if (position.x() - 1u < width - 2u && position.y() - 1u < height - 2u)
	{
		return hasMaskNeighbor9Center<tMaskValueIsEqual, T>(mask, width, height, maskPaddingElements, position, testValue);
	}

	const unsigned int maskStrideElements = width + maskPaddingElements;

	mask += position.y() * maskStrideElements + position.x();

	if (compareValues<T, tMaskValueIsEqual>(*mask, testValue))
	{
		return true;
	}

	if (position.x() != 0u)
	{
		if (compareValues<T, tMaskValueIsEqual>(*(mask - 1), testValue))
		{
			return true;
		}

		if (position.y() != 0u && compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements - 1u), testValue))
		{
			return true;
		}

		if (position.y() + 1u < height && compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements - 1u), testValue))
		{
			return true;
		}
	}

	if (position.x() + 1u < width)
	{
		if (compareValues<T, tMaskValueIsEqual>(*(mask + 1), testValue))
		{
			return true;
		}

		if (position.y() != 0u && compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements + 1u), testValue))
		{
			return true;
		}

		if (position.y() + 1u < height && compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements + 1u), testValue))
		{
			return true;
		}
	}

	if (position.y() != 0u && compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements), testValue))
	{
		return true;
	}

	if (position.y() + 1u < height && compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements), testValue))
	{
		return true;
	}

	return false;
}

template <bool tMaskValueIsEqual, typename T>
inline bool MaskAnalyzer::hasMaskNeighbor9Center(const T* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelPosition& position, const T testValue)
{
	ocean_assert(mask != nullptr && height >= 3u && height >= 3u);
	ocean_assert(position.x() >= 1u && position.x() + 1u < width);
	ocean_assert_and_suppress_unused(position.y() >= 1u && position.y() + 1u < height, height);

	const unsigned int maskStrideElements = width + maskPaddingElements;

	mask += position.y() * maskStrideElements + position.x();

	return compareValues<T, tMaskValueIsEqual>(*(mask - 1), testValue) || compareValues<T, tMaskValueIsEqual>(*mask, testValue) || compareValues<T, tMaskValueIsEqual>(*(mask + 1), testValue)
				|| compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements - 1), testValue) || compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements), testValue) || compareValues<T, tMaskValueIsEqual>(*(mask - maskStrideElements + 1), testValue)
				|| compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements - 1), testValue) || compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements), testValue) || compareValues<T, tMaskValueIsEqual>(*(mask + maskStrideElements + 1), testValue);
}

inline bool MaskAnalyzer::isOuterContour(const uint8_t* mask, const unsigned int width, const PixelContour& contour, const uint8_t maskValue)
{
	ocean_assert(mask && width >= 1u);
	ocean_assert(contour);

	const PixelPosition& mostLeftPixel = contour[contour.indexLeftPosition()];

	ocean_assert(mostLeftPixel.x() < width);
	return mostLeftPixel.x() == 0u || (mask[mostLeftPixel.y() * width + mostLeftPixel.x() - 1u] != maskValue);
}

inline bool MaskAnalyzer::computeChessboardDistanceTransform8Bit(const uint8_t* source, const uint32_t width, const uint32_t height, uint32_t* target, uint32_t* buffer, const uint8_t referenceValue, const uint32_t sourcePaddingElements, const uint32_t targetPaddingElements)
{
	return computeDistanceTransform8Bit(source, width, height, target, buffer, distanceVerticalHorizontalC, distanceDiagonalC, referenceValue, sourcePaddingElements, targetPaddingElements);
}

inline bool MaskAnalyzer::computeL1DistanceTransform8Bit(const uint8_t* source, const uint32_t width, const uint32_t height, uint32_t* target, uint32_t* buffer, const uint8_t referenceValue, const uint32_t sourcePaddingElements, const uint32_t targetPaddingElements)
{
	return computeDistanceTransform8Bit(source, width, height, target, buffer, distanceVerticalHorizontalL1, distanceDiagonalL1, referenceValue, sourcePaddingElements, targetPaddingElements);
}

inline bool MaskAnalyzer::computeL2DistanceTransform8Bit(const uint8_t* source, const uint32_t width, const uint32_t height, float* target, uint32_t* buffer, const uint8_t referenceValue, const uint32_t sourcePaddingElements, const uint32_t targetPaddingElements)
{
	return computeDistanceTransform8Bit(source, width, height, target, buffer, distanceVerticalHorizontalL2, distanceDiagonalL2, referenceValue, sourcePaddingElements, targetPaddingElements);
}

template <typename TDistanceType>
bool MaskAnalyzer::computeDistanceTransform8Bit(const uint8_t* source, const uint32_t width, const uint32_t height, TDistanceType* target, uint32_t* buffer, const TDistanceType distanceVerticalHorizontal, const TDistanceType distanceDiagonal, const uint8_t referenceValue, const uint32_t sourcePaddingElements, const uint32_t targetPaddingElements)
{
	if (source == nullptr || width == 0u || height == 0u || target == nullptr)
	{
		ocean_assert(false && "Invalid input data");
		return false;
	}

	const uint32_t bufferWidth = width + 2u;
	const uint32_t bufferHeight = height + 2u;

	Memory memory;

	if (buffer == nullptr)
	{
		memory = Memory(bufferWidth * bufferHeight * sizeof(uint32_t));
		buffer = memory.data<uint32_t>();
	}

	if (distanceVerticalHorizontal <= 0 || distanceDiagonal <= 0)
	{
		ocean_assert(false && "Distance values must be positive");
		return false;
	}

	const uint32_t sourceStrideElements = width + sourcePaddingElements;
	const uint32_t targetStrideElements = width + targetPaddingElements;

	const uint8_t* const sourceEnd = source + (height - 1u) * sourceStrideElements + width;
	const TDistanceType* const targetEnd = target + (height - 1u) * targetStrideElements + width;

	const uint32_t* bufferEnd = buffer + bufferHeight * bufferWidth;

	// In case of floating-point numbers, all intermediate values are be stored as fixed-point values with 16-bit precision. Integral types remain unchanged.
	constexpr TDistanceType fixedPointScalingFactor = TDistanceType(std::is_floating_point<TDistanceType>::value ? 1u << 16u : 1u);
	constexpr TDistanceType fixedPointUnscalingFactor = TDistanceType(1) / fixedPointScalingFactor;
	static_assert(fixedPointScalingFactor > 0 && fixedPointUnscalingFactor > 0, "Fixed-point scaling factors must be positive");
	static_assert(std::is_floating_point<TDistanceType>::value || (fixedPointScalingFactor == TDistanceType(1u) && fixedPointUnscalingFactor == TDistanceType(1)), "For integral types scaling factor must be 1");

	const uint32_t distanceVerticalHorizontal_q = uint32_t((distanceVerticalHorizontal * fixedPointScalingFactor));
	const uint32_t distanceDiagonal_q = uint32_t(distanceDiagonal * fixedPointScalingFactor);

	constexpr uint32_t boundaryValue = NumericT<uint32_t>::maxValue() / 2u;

	// Initialize the top and bottom row of the buffer memory (the left-most and right-most columns will be initialized
	// during the forward pass)
	{
		uint32_t* bufferTopRow = buffer;
		uint32_t* bufferBottomRow = buffer + (bufferHeight - 1u) * bufferWidth;
		for (uint32_t x = 0u; x < bufferWidth; ++x)
		{
			*bufferTopRow = boundaryValue;
			*bufferBottomRow = boundaryValue;

			++bufferTopRow;
			++bufferBottomRow;
		}
	}

	// Forward and backward passes using a 3x3 neighborhood:
	//
	//  0 1 2
	//  3 X 4
	//  5 6 7
	//
	// The forward pass uses the neighbors 0-3 and iterates through the image from the top-left corner to the
	// bottom-right corner; the backward pass uses the neighbors 4-7 and iterates from the bottom-right corner back to
	// top-left corner.
	//
	// Example for distanceDiagonal = distanceVerticalHorizontal = 1 (<=> distance metric C):
	//
	// Input:                  Forward pass:          Backward pass:
	// -  -  -  -  -  -  -     -  -  -  -  -  -  -    3  3  3  3  3  3  3
	// -  -  -  -  -  -  -     -  -  -  -  -  -  -    3  2  2  2  2  2  3
	// -  -  -  -  -  -  -     -  -  -  -  -  -  -    3  2  1  1  1  2  3
	// -  -  -  0  -  -  -     -  -  -  0  1  2  3    3  2  1  0  1  2  3
	// -  -  -  -  -  -  -     -  -  1  1  1  2  3    3  2  1  1  1  2  3
	// -  -  -  -  -  -  -     -  2  2  2  2  2  3    3  2  2  2  2  2  3
	// -  -  -  -  -  -  -     3  3  3  3  3  3  3    3  3  3  3  3  3  3

	bool foundReferenceValue = false;

	// Forward pass
	for (uint32_t y = 0u; y < height; ++y)
	{
		const uint8_t* sourceRow = source + y * sourceStrideElements;
		uint32_t* bufferRow = buffer + (y + 1u) * bufferWidth;

		// Initialize the left-most and right-most columns of the current row of the buffer memory
		*bufferRow = boundaryValue;
		*(bufferRow + bufferWidth - 1u) = boundaryValue;

		++bufferRow;

		for (uint32_t x = 0u; x < width; ++x)
		{
			ocean_assert_and_suppress_unused(sourceRow >= source && sourceRow < sourceEnd, sourceEnd);
			ocean_assert_and_suppress_unused(bufferRow - bufferWidth - 1u >= buffer && bufferRow < bufferEnd, bufferEnd);

			if (*sourceRow == referenceValue)
			{
				*bufferRow =  0u;
				foundReferenceValue = true;
			}
			else
			{
				const uint32_t neighbor0_q = *(bufferRow - bufferWidth - 1u) + distanceDiagonal_q;
				const uint32_t neighbor1_q = *(bufferRow - bufferWidth) + distanceVerticalHorizontal_q;
				const uint32_t neighbor2_q = *(bufferRow - bufferWidth + 1u) + distanceDiagonal_q;
				const uint32_t neighbor3_q = *(bufferRow - 1u) + distanceVerticalHorizontal_q;

				*bufferRow = std::min(std::min(neighbor0_q, neighbor1_q), std::min(neighbor2_q, neighbor3_q));
			}

			++sourceRow;
			++bufferRow;
		}
	}

	if (foundReferenceValue == false)
	{
		return false;
	}

	// Backward pass
	for (uint32_t y = height - 1u; y < height; --y)
	{
		TDistanceType* targetRow = target + y * targetStrideElements + width - 1u;
		uint32_t* bufferRow = buffer + (y + 1u) * bufferWidth + width;

		for (uint32_t x = width - 1u; x < width; --x)
		{
			ocean_assert_and_suppress_unused(targetRow >= target && targetRow < targetEnd, targetEnd);
			ocean_assert(bufferRow >= buffer && bufferRow + bufferWidth + 1u < bufferEnd);

			const uint32_t neighbor4_q = *(bufferRow + 1u) + distanceVerticalHorizontal_q;
			const uint32_t neighbor5_q = *(bufferRow + bufferWidth - 1u) + distanceDiagonal_q;
			const uint32_t neighbor6_q = *(bufferRow + bufferWidth) + distanceVerticalHorizontal_q;
			const uint32_t neighbor7_q = *(bufferRow + bufferWidth + 1u) + distanceDiagonal_q;

			*bufferRow = std::min(*bufferRow, std::min(std::min(neighbor4_q, neighbor5_q), std::min(neighbor6_q, neighbor7_q)));

			if (std::is_floating_point<TDistanceType>::value)
			{
				*targetRow = TDistanceType(*bufferRow) * fixedPointUnscalingFactor;
			}
			else
			{
				*targetRow = TDistanceType(*bufferRow);
			}


			--targetRow;
			--bufferRow;
		}
	}

	return true;
}

template <typename T, bool tMaskValueIsEqual>
inline bool MaskAnalyzer::compareValues(const T valueA, const T valueB)
{
	if constexpr (tMaskValueIsEqual)
	{
		return valueA == valueB;
	}
	else
	{
		return valueA != valueB;
	}
}

} // namespace Segmentation

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_SEGMENTATION_MASK_ANALYZER_H
