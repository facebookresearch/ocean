/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_SEGMENTATION_MASK_CREATOR_H
#define META_OCEAN_CV_SEGMENTATION_MASK_CREATOR_H

#include "ocean/cv/segmentation/Segmentation.h"
#include "ocean/cv/segmentation/PixelContour.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/advanced/PixelLine.h"
#include "ocean/cv/advanced/PixelTriangle.h"

namespace Ocean
{

namespace CV
{

namespace Segmentation
{

/**
 * This class implements functions allowing to create or to modify masks.
 * @ingroup cvsegmentation
 */
class OCEAN_CV_SEGMENTATION_EXPORT MaskCreator
{
	public:

		/**
		 * Creates a binary 8 bit (inclusive) mask for one given triangle, actually the triangle is painted into the frame with a specified color value.
		 * Inclusive means that the pixels inside the triangle as well as the pixels converted by the triangle (the corners, the lines between the corners) will be set to mask pixels.<br>
		 * Beware: The given triangle must entirely fit into the frame!
		 * @param mask The mask frame receiving the mask, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param triangle The triangle to create a mask from, must be valid but may be located outside the frame
		 * @param maskValue Mask value to be set, with range [0, 255]
		 * @tparam T The data type that is used to store the elements of a pixel coordinate, possible types are 'unsigned int' and 'int'
		 */
		template <typename T>
		static void triangle2inclusiveMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const Advanced::PixelTriangleT<T>& triangle, const uint8_t maskValue);

		/**
		 * Creates a binary 8 bit mask for several given triangles, actually the triangles will be painted into the frame with a specified color value.
		 * Inclusive means that the pixels inside the triangles as well as the pixels converted by the triangles (the corners, the lines between the corners) will be set to mask pixels.<br>
		 * Beware: All triangles must entirely fit into the frame!
		 * @param mask The mask frame receiving the mask, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param triangles The triangles to create a mask from all fitting entirely into the mask frame
		 * @param size The number of provided triangles, with rang [1, infinity)
		 * @param maskValue Mask value to be set, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation
		 */
		template <typename T>
		static inline void triangles2inclusiveMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const Advanced::PixelTriangleT<T>* triangles, const size_t size, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Smooths a given 8 bit mask frame by adding additional outline-4 mask pixels with incrementing values within several iterations.
		 * @param mask The mask frame to be smoothed, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param iterations Number of smoothing iterations (equivalent to the number of new border pixels around the original mask), with range [1, 255]
		 * @param incrementValue Mask value offset to be added for each iteration, with range [1, 255 / iterations]
		 */
		static void smoothMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const unsigned int iterations, const unsigned int incrementValue);

		/**
		 * Creates a binary 8 bit (inclusive) mask for a given sparse or dense contour.
		 * The contour is triangulated and than converted to a mask.<br>
		 * Due to performance reasons and due to irregular coordinates a sparse contours is recommended.<br>
		 * In any case, the provided pixel contour should be simplified.
		 * @param mask The mask frame receiving the mask, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param simplifiedContour The (simplified) contour from which the mask will be created, must entirely fit into the frame, e.g., pixelContour.boundingBox().right() < width, pixelContour.boundingBox().bottom() < height
		 * @param maskValue Mask value to be assigned, with range [0, 255]
		 * @param worker Optional worker object to distribute the computation
		 * @param triangulationForced Optional resulting statement whether the contour needed to be forced to a triangulation (so that the resulting mask may be faulty); True, if so
		 * @return True, if succeeded
		 * @see denseContour2inclusiveMaskHotizontallyConvex().
		 */
		static bool contour2inclusiveMaskByTriangulation(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelContour& simplifiedContour, const unsigned int maskValue, Worker* worker = nullptr, bool* triangulationForced = nullptr);

		/**
		 * Creates a binary 8 bit horizontally-convex (inclusive) mask from a given dense contour.
		 * As the mask will be inclusive all contour pixels (and the pixels inside the contour) will become mask pixels.<br>
		 * This function does not triangulate the given contour so that the function is faster than contour2inclusiveMaskByTriangulation().<br>
		 * Instead, this function determines the first and last mask pixel in every row of the frame and sets all pixel within the ranges to specified mask value.<br>
		 * However, as for each row only one start and end point is determined (the extreme values), vertical holes will be filled with mask pixel.
		 * @param mask 8 bit frame receiving the mask, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param densePixelPositions Dense contour pixels, successive pixels must be in the direct 8-neighborhood, each pixel position with range [0, infinity)x[0, infinity)
		 * @param maskValue Mask value to be assigned, with range [0, 255]
		 * @see denseContour2inclusiveMask(), contour2inclusiveMaskByTriangulation().
		 */
		static void denseContour2inclusiveMaskHotizontallyConvex(uint8_t* mask, const unsigned int width, const unsigned int height, const PixelPositions& densePixelPositions, const uint8_t maskValue);

		/**
		 * Creates a binary 8 bit (inclusive) mask from a given dense and distinct contour.
		 * As the mask will be inclusive all contour pixels (and the pixels inside the contour) will become mask pixels.<br>
		 * This function does not triangulate the given contour so that the function is faster than contour2inclusiveMaskByTriangulation().<br>
		 * Instead, this function determines horizontal sections of joined mask pixels in every row of the frame and sets all pixel within the ranges to specified mask value.<br>
		 * This function creates concave (and convex) masks and therefore is more precisely as denseContour2inclusiveMaskHotizontallyConvex(), but therefore slightly slower.
		 * @param mask 8 bit frame receiving the mask, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param denseContour Dense and distinct contour for which the mask will be created, each pixel position with range [0, width)x[0, height)
		 * @param maskValue Mask value to be assigned, with range [0, 255]
		 * @see denseContour2exclusiveMask(), contour2inclusiveMaskByTriangulation().
		 */
		static inline void denseContour2inclusiveMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelContour& denseContour, const uint8_t maskValue);

		/**
		 * Creates a binary 8 bit (exclusive) mask from a given dense and distinct contour.
		 * As the mask will be exclusive, pixels inside the contour will become mask pixels but not the pixels of the contour.<br>
		 * This function does not triangulate the given contour so that the function is faster than contour2inclusiveMaskByTriangulation().<br>
		 * Instead, this function determines horizontal sections of joined mask pixels in every row of the frame and sets all pixel within the ranges to specified mask value.<br>
		 * This function creates concave (and convex) masks and therefore is more precisely as denseContour2inclusiveMaskHotizontallyConvex(), but therefore slightly slower.
		 * @param mask 8 bit frame receiving the mask, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param denseContour Dense and distinct contour for which the mask will be created, each pixel position with range [0, width)x[0, height)
		 * @param maskValue Mask value to be assigned, with range [0, 255]
		 * @see denseContour2inclusiveMask(), contour2inclusiveMaskByTriangulation().
		 */
		static inline void denseContour2exclusiveMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelContour& denseContour, const uint8_t maskValue);

		/**
		 * Creates a binary 8 bit (inclusive) mask from a given dense and distinct contour.
		 * As the mask will be inclusive all contour pixels (and the pixels inside the contour) will be XORed with a specified reference values.<br>
		 * Instead, this function determines horizontal sections of joined mask pixels in every row of the frame and handles all pixel within the ranges with XOR.
		 * @param mask 8 bit frame receiving the mask, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param denseContour Dense and distinct contour for which the mask will be created, each pixel position with range [0, width)x[0, height)
		 * @param xorReference The reference value for the XOR operator, with range [0, 255]
		 * @see denseContour2exclusiveMaskXOR(), denseContour2exclusiveMask(), contour2inclusiveMaskByTriangulation().
		 */
		static inline void denseContour2inclusiveMaskXOR(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelContour& denseContour, const uint8_t xorReference = 0xFF);

		/**
		 * Creates a binary 8 bit (exclusive) mask from a given dense and distinct contour.
		 * As the mask will be exclusive, pixels inside the contour will be XORed but not the pixels of the contour.<br>
		 * Instead, this function determines horizontal sections of joined mask pixels in every row of the frame and handles all pixel within the ranges with XOR.
		 * @param mask 8 bit frame receiving the mask, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param denseContour Dense and distinct contour for which the mask will be created, each pixel position with range [0, width)x[0, height)
		 * @param xorReference The reference value for the XOR operator, with range [0, 255]
		 * @see denseContour2inclusiveMaskXOR(), denseContour2exclusiveMask(), contour2inclusiveMaskByTriangulation().
		 */
		static inline void denseContour2exclusiveMaskXOR(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelContour& denseContour, const uint8_t xorReference = 0xFF);

		/**
		 * Creates an 8 bit mask from a given 1 channel 32 bit separation frame.
		 * The separation frame holds mask blocks with individual ids identifying individual blocks (the ids allow to separate the blocks).<br>
		 * This function actually does the following:
		 * <pre>
		 * if separation[x, y] == id
		 * {
		 *     mask[x, y] = maskValue
		 * }
		 * </pre>
		 * @param separation The separation frame composed of individual blocks with individual ids, must be valid
		 * @param width The width of the separation frame (and the mask frame) in pixel, with range [1, infinity)
		 * @param height The height of the separation frame (and the mask frame) in pixel, with range [1, infinity)
		 * @param separationPaddingElements The number of padding elements at the end of each separation row, in elements, with range [0, infinity)
		 * @param id The id for which the corresponding mask pixels will be changed/set to a specified mask value
		 * @param mask The mask frame in which the specific pixels will be set to the specified mask value, with same dimension and pixel origin as the separation frame, must be valid
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param maskValue The value to be set to each mask pixel for which the corresponding separation pixel is equal to the specified id
		 */
		static void separation2mask(const uint32_t* separation, const unsigned int width, const unsigned int height, const unsigned int separationPaddingElements, const uint32_t id, uint8_t* mask, const unsigned int maskPaddingElements, const uint8_t maskValue = 0x00);

		/**
		 * Creates an 8 bit mask from a given 1 channel 32 bit separation frame.
		 * The separation frame holds mask blocks with individual ids identifying individual blocks (the ids allow to separate the blocks).<br>
		 * This function takes a lookup table with separation ids defining whether the corresponding mask block receives a new pixel value or not.<br>
		 * This function actually does the following:
		 * <pre>
		 * if ids[ separation[x, y] ] == true
		 * {
		 *     mask[x, y] = maskValue
		 * }
		 * </pre>
		 * @param separation The separation frame composed of individual blocks with individual ids, must be valid
		 * @param width The width of the separation frame (and the mask frame) in pixel, with range [1, infinity)
		 * @param height The height of the separation frame (and the mask frame) in pixel, with range [1, infinity)
		 * @param separationPaddingElements The number of padding elements at the end of each separation row, in elements, with range [0, infinity)
		 * @param ids Id lookup table defining whether the corresponding pixels receive the specified mask value, a lookup value not equal to 0x00 sets the mask pixel
		 * @param numberIds Size of the lookup table, must have one entry for each id from the separation frame, with range [1, infinity)
		 * @param maskValue The value to be set to each mask pixel for which the corresponding separation pixel is equal to the specified id
		 * @param mask The mask frame in which the specific pixels will be set to the specified mask value, with same dimension and pixel origin as the separation frame, must be valid
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 */
		static void separations2mask(const uint32_t* separation, const unsigned int width, const unsigned int height, const unsigned int separationPaddingElements, const uint8_t* ids, const size_t numberIds, const uint8_t maskValue, uint8_t* mask, const unsigned int maskPaddingElements);

		/**
		 * Joins two 8 bit masks with respect to a specific mask value.
		 * This function actually does the following:
		 * <pre>
		 * if mask[x, y] == maskValue || target[x, y] == maskValue
		 * {
		 *     target[x, y] = maskValue
		 * }
		 * </pre>
		 * @param mask The first mask which will be joined with (added to) the second (target) mask, must be valid
		 * @param target The target mask which is the second mask and also the final joined mask with same dimension and pixel origin as the first mask, must be valid
		 * @param width The width of both masks in pixel, with range [1, infinity)
		 * @param height The height of both masks in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements the number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param maskValue The mask pixel value, a pixel in the joined mask is set to this value if the value is either in the first or in the second mask
		 * @param worker Optional worker object to distribute the computation
		 */
		static inline void joinMasks(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const uint8_t maskValue = 0x00, Worker* worker = nullptr);

	private:

		/**
		 * Determines the group of horizontal pixel offset locations for a given dense and distinct contour so that an (inclusive) mask can be created from the offsets afterwards.
		 * @param denseDistinctContour The dense and distinct contour for which the groups of horizontal pixel offsets will be determined
		 * @param offsetGroups The resulting groups of horizontal pixel offsets, the size of the vector must be pre-initialized with the height of the resulting mask frame
		 */
		static void denseContour2inclusiveMaskOffsets(const PixelContour& denseDistinctContour, IndexGroups32& offsetGroups);

		/**
		 * Determines the group of horizontal pixel offset locations for a given dense and distinct contour so that an (exclusive) mask can be created from the offsets afterwards.
		 * @param denseDistinctContour The dense and distinct contour for which the groups of horizontal pixel offsets will be determined
		 * @param offsetGroups The resulting groups of horizontal pixel offsets, the size of the vector must be pre-initialized with the height of the resulting mask frame
		 */
		static void denseContour2exclusiveMaskOffsets(const PixelContour& denseDistinctContour, IndexGroups32& offsetGroups);

		/**
		 * Creates an (inclusive) 8 bit mask for a given group of horizontal pixel offset locations.
		 * For each mask row pixel offset locations define the horizontal start and end points of joined mask ranges while a row may have several start and end points following on each other.<br>
		 * The number of start end points for each row must be even: e.g., [start0, end0] [start1, end1] and so on.<br>
		 * However, start and end points do not need to be sorted according to their horizontal location, this is done internally.
		 * @param offsetGroups The groups of horizontal offset values, with offsets.size() <= height, the individual offsets will be sorted according their horizontal location during the execution
		 * @param mask The mask frame receiving the mask, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value to be set for each pixel within start and end ranges, with range [0, 255]
		 */
		static void inclusiveMaskOffsets2inclusiveMask(IndexGroups32& offsetGroups, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t maskValue);

		/**
		 * Creates an (exclusive) 8 bit mask for a given group of horizontal pixel offset locations.
		 * For each mask row pixel offset locations define the horizontal start and end points of joined mask ranges while a row may have several start and end points following on each other.<br>
		 * The number of start end points for each row must be even: e.g., [start0, end0] [start1, end1] and so on.<br>
		 * However, start and end points do not need to be sorted according to their horizontal location, this is done internally.
		 * @param offsetGroups The groups of horizontal offset values, with offsets.size() <= height, the individual offsets will be sorted according their horizontal location during the execution
		 * @param mask The mask frame receiving the mask, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value to be set for each pixel within start and end ranges, with range [0, 255]
		 */
		static void exclusiveMaskOffsets2exclusiveMask(IndexGroups32& offsetGroups, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t maskValue);

		/**
		 * Creates an (inclusive) 8 bit mask for a given group of horizontal pixel offset locations.
		 * For each mask row pixel offset locations define the horizontal start and end points of joined mask ranges while a row may have several start and end points following on each other.<br>
		 * The number of start end points for each row must be even: e.g., [start0, end0] [start1, end1] and so on.<br>
		 * However, start and end points do not need to be sorted according to their horizontal location, this is done internally.
		 * @param offsetGroups The groups of horizontal offset values, with offsets.size() <= height, the individual offsets will be sorted according their horizontal location during the execution
		 * @param mask The mask frame receiving the mask, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param xorReference The reference value for the XOR operator to be used for each pixel within start and end ranges, with range [0, 255]
		 */
		static void inclusiveMaskOffsets2inclusiveMaskXOR(IndexGroups32& offsetGroups, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t xorReference);

		/**
		 * Creates an (exclusive) 8 bit mask for a given group of horizontal pixel offset locations.
		 * For each mask row pixel offset locations define the horizontal start and end points of joined mask ranges while a row may have several start and end points following on each other.<br>
		 * The number of start end points for each row must be even: e.g., [start0, end0] [start1, end1] and so on.<br>
		 * However, start and end points do not need to be sorted according to their horizontal location, this is done internally.
		 * @param offsetGroups The groups of horizontal offset values, with offsets.size() <= height, the individual offsets will be sorted according their horizontal location during the execution
		 * @param mask The mask frame receiving the mask, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param xorReference The reference value for the XOR operator to be used for each pixel within start and end ranges, with range [0, 255]
		 */
		static void exclusiveMaskOffsets2exclusiveMaskXOR(IndexGroups32& offsetGroups, uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const uint8_t xorReference);

		/**
		 * Creates a binary 8 bit mask for a subset of several given triangles, actually the triangles will be painted into the frame with a specified color value.
		 * Beware: All triangles must entirely fit into the frame!
		 * @param mask The mask frame receiving the mask, must be valid
		 * @param width The width of the mask frame in pixel, with range (0, infinity)
		 * @param height The height of the mask frame in pixel, with range (0, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param triangles The triangles to create a mask from all fitting entirely into the mask frame
		 * @param maskValue Mask value to be set, with range [0, 255]
		 * @param firstTriangle First triangle to be handled, with range [0, 'size')
		 * @param numberTriangles Number of triangles to be handled, with range [1, 'size')
		 * @tparam T The data type that is used to store the elements of a pixel coordinate, possible types are 'unsigned int' and 'int'
		 */
		template <typename T>
		static inline void triangles2inclusiveMaskSubset(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const Advanced::PixelTriangleT<T>* triangles, const uint8_t maskValue, const unsigned int firstTriangle, const unsigned int numberTriangles);

		/**
		 * Joins two 8 bit masks with respect to a specific mask value.
		 * @param mask The first mask which will be joined with (added to) the second (target) mask, must be valid
		 * @param target The target mask which is the second mask and also the final joined mask with same dimension and pixel origin as the first mask, must be valid
		 * @param width The width of both masks in pixel, with range [1, infinity)
		 * @param height The height of both masks in pixel, with range [1, infinity)
		 * @param maskPaddingElements The number of padding elements at the end of each mask row, in elements, with range [0, infinity)
		 * @param targetPaddingElements the number of padding elements at the end of each target row, in elements, with range [0, infinity)
		 * @param maskValue The mask pixel value, a pixel in the joined mask is set to this value if the value is either in the first or in the second mask
		 * @param firstRow The first row to be handled, with range [0, height)
		 * @param numberRows The number of rows to be handled, with range [1, height]
		 * @see joinMasks().
		 */
		static void joinMasksSubset(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const uint8_t maskValue, const unsigned int firstRow, const unsigned int numberRows);
};

template <>
inline void MaskCreator::triangle2inclusiveMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const Advanced::PixelTriangleT<unsigned int>& triangle, const uint8_t maskValue)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);

	if (triangle.left() >= width || triangle.top() >= height)
	{
		return;
	}

	const unsigned int maskStrideElements = width + maskPaddingElements;

	const unsigned int yMin = triangle.top();
	const unsigned int yMax = min(triangle.bottom(), height - 1u);

	const Advanced::PixelLineT<unsigned int> l01(triangle.point0().x(), triangle.point0().y(), triangle.point1().x(), triangle.point1().y());
	const Advanced::PixelLineT<unsigned int> l02(triangle.point0().x(), triangle.point0().y(), triangle.point2().x(), triangle.point2().y());
	const Advanced::PixelLineT<unsigned int> l12(triangle.point1().x(), triangle.point1().y(), triangle.point2().x(), triangle.point2().y());

	unsigned int x01, x02, x12;

	for (unsigned int y = yMin; y <= yMax; ++y)
	{
		unsigned int xMin = 0xFFFFFFFF;
		unsigned int xMax = 0;

		if (l01.horizontalIntersection(y, x01))
		{
			xMin = min(xMin, x01);
			xMax = max(xMax, x01);

			if (l01.isHorizontal())
			{
				xMin = min(xMin, min(l01.p0().x(), l01.p1().x()));
				xMax = max(xMax, max(l01.p0().x(), l01.p1().x()));
			}
		}

		if (l02.horizontalIntersection(y, x02))
		{
			xMin = min(xMin, x02);
			xMax = max(xMax, x02);

			if (l02.isHorizontal())
			{
				xMin = min(xMin, min(l02.p0().x(), l02.p1().x()));
				xMax = max(xMax, max(l02.p0().x(), l02.p1().x()));
			}
		}

		if (l12.horizontalIntersection(y, x12))
		{
			xMin = min(xMin, x12);
			xMax = max(xMax, x12);

			if (l12.isHorizontal())
			{
				xMin = min(xMin, min(l12.p0().x(), l12.p1().x()));
				xMax = max(xMax, max(l12.p0().x(), l12.p1().x()));
			}
		}

		if (xMin <= xMax)
		{
			memset(mask + y * maskStrideElements + xMin, maskValue, xMax - xMin + 1u);
		}
	}
}

template <>
inline void MaskCreator::triangle2inclusiveMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const Advanced::PixelTriangleT<int>& triangle, const uint8_t maskValue)
{
	ocean_assert(mask && width != 0u && height != 0u);

	if (triangle.left() >= int(width) || triangle.right() < 0 || triangle.top() >= int(height) || triangle.bottom() < 0)
	{
		return;
	}

	const unsigned int maskStrideElements = width + maskPaddingElements;

	const int yMin = max(0, triangle.top());
	const int yMax = min(triangle.bottom(), int(height - 1u));

	ocean_assert(yMin >= 0 && yMax < int(height) && yMin < yMax);

	const Advanced::PixelLineT<int> l01(triangle.point0().x(), triangle.point0().y(), triangle.point1().x(), triangle.point1().y());
	const Advanced::PixelLineT<int> l02(triangle.point0().x(), triangle.point0().y(), triangle.point2().x(), triangle.point2().y());
	const Advanced::PixelLineT<int> l12(triangle.point1().x(), triangle.point1().y(), triangle.point2().x(), triangle.point2().y());

	int x01, x02, x12;

	for (int y = yMin; y <= yMax; ++y)
	{
		int xMin = NumericT<int>::maxValue();
		int xMax = NumericT<int>::minValue();

		if (l01.horizontalIntersection(y, x01))
		{
			xMin = min(xMin, x01);
			xMax = max(xMax, x01);

			if (l01.isHorizontal())
			{
				xMin = min(xMin, min(l01.p0().x(), l01.p1().x()));
				xMax = max(xMax, max(l01.p0().x(), l01.p1().x()));
			}
		}

		if (l02.horizontalIntersection(y, x02))
		{
			xMin = min(xMin, x02);
			xMax = max(xMax, x02);

			if (l02.isHorizontal())
			{
				xMin = min(xMin, min(l02.p0().x(), l02.p1().x()));
				xMax = max(xMax, max(l02.p0().x(), l02.p1().x()));
			}
		}

		if (l12.horizontalIntersection(y, x12))
		{
			xMin = min(xMin, x12);
			xMax = max(xMax, x12);

			if (l12.isHorizontal())
			{
				xMin = min(xMin, min(l12.p0().x(), l12.p1().x()));
				xMax = max(xMax, max(l12.p0().x(), l12.p1().x()));
			}
		}

		if (xMin < 0)
		{
			xMin = 0;
		}

		if (xMax >= int(width))
		{
			xMax = int(width) - 1;
		}

		if (xMin <= xMax)
		{
			memset(mask + y * maskStrideElements + xMin, maskValue, xMax - xMin + 1u);
		}
	}
}

template <typename T>
inline void MaskCreator::triangles2inclusiveMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const Advanced::PixelTriangleT<T>* triangles, const size_t size, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(mask != nullptr && width != 0u && height != 0u);
	ocean_assert(triangles != nullptr && size != 0);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&MaskCreator::triangles2inclusiveMaskSubset, mask, width, height, maskPaddingElements, triangles, maskValue, 0u, 0u), 0u, (unsigned int)(size));
	}
	else
	{
		triangles2inclusiveMaskSubset(mask, width, height, maskPaddingElements, triangles, maskValue, 0u, (unsigned int)(size));
	}
}

inline void MaskCreator::denseContour2inclusiveMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelContour& denseContour, const uint8_t maskValue)
{
	ocean_assert(mask && width != 0u && height != 0u);

	IndexGroups32 offsetGroups(height);
	denseContour2inclusiveMaskOffsets(denseContour, offsetGroups);
	inclusiveMaskOffsets2inclusiveMask(offsetGroups, mask, width, height, maskPaddingElements, maskValue);
}

inline void MaskCreator::denseContour2inclusiveMaskXOR(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelContour& denseContour, const uint8_t xorReference)
{
	ocean_assert(mask && width != 0u && height != 0u);

	IndexGroups32 offsetGroups(height);
	denseContour2inclusiveMaskOffsets(denseContour, offsetGroups);
	inclusiveMaskOffsets2inclusiveMaskXOR(offsetGroups, mask, width, height, maskPaddingElements, xorReference);
}

inline void MaskCreator::denseContour2exclusiveMask(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelContour& denseContour, const uint8_t maskValue)
{
	ocean_assert(mask && width != 0u && height != 0u);

	IndexGroups32 offsetGroups(height);
	denseContour2exclusiveMaskOffsets(denseContour, offsetGroups);
	exclusiveMaskOffsets2exclusiveMask(offsetGroups, mask, width, height, maskPaddingElements, maskValue);
}

inline void MaskCreator::denseContour2exclusiveMaskXOR(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const PixelContour& denseContour, const uint8_t xorReference)
{
	ocean_assert(mask && width != 0u && height != 0u);

	IndexGroups32 offsetGroups(height);
	denseContour2exclusiveMaskOffsets(denseContour, offsetGroups);
	exclusiveMaskOffsets2exclusiveMaskXOR(offsetGroups, mask, width, height, maskPaddingElements, xorReference);
}

inline void MaskCreator::joinMasks(const uint8_t* mask, uint8_t* target, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const unsigned int targetPaddingElements, const uint8_t maskValue, Worker* worker)
{
	ocean_assert(mask != nullptr && target != nullptr);
	ocean_assert(width != 0u && height != 0u);

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&MaskCreator::joinMasksSubset, mask, target, width, height, maskPaddingElements, targetPaddingElements, maskValue, 0u, 0u), 0u, height, 7u, 8u, 20u);
	}
	else
	{
		joinMasksSubset(mask, target, width, height, maskPaddingElements, targetPaddingElements, maskValue, 0u, height);
	}
}

template <typename T>
inline void MaskCreator::triangles2inclusiveMaskSubset(uint8_t* mask, const unsigned int width, const unsigned int height, const unsigned int maskPaddingElements, const Advanced::PixelTriangleT<T>* triangles, const uint8_t maskValue, const unsigned int firstTriangle, const unsigned int numberTriangles)
{
	ocean_assert(mask && triangles);

	for (unsigned int n = firstTriangle; n < firstTriangle + numberTriangles; ++n)
	{
		triangle2inclusiveMask(mask, width, height, maskPaddingElements, triangles[n], maskValue);
	}
}

}

}

}

#endif // META_OCEAN_CV_SEGMENTATION_MASK_CREATOR_H
