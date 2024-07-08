/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_UTILITIES_H
#define META_OCEAN_CV_DETECTOR_UTILITIES_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/detector/ShapeDetector.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class is a collection of detector-related utitility functions.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT Utilities
{
	public:

		/**
		 * Draws finite lines into an image.
		 * @param frame The image in which the L-shapes will be painted, must be valid
		 * @param finiteLines The finite lines that will be painted
		 * @param foregroundColor The foreground color in which the finite lines will be painted, must have as many elements as the image has channels, must be valid
		 * @param backgroundColor Optional background color, nullptr to avoid using a background color
		 */
		static void paintLines(Frame& frame, const FiniteLines2& finiteLines, const unsigned char* foregroundColor, const unsigned char* backgroundColor = nullptr);

		/**
		 * Draws L-shapes into an image.
		 * @param frame The image in which the L-shapes will be painted, must be valid
		 * @param finiteLines The finite lines forming the L-shapes
		 * @param lShapes The L-shapes to be painted
		 * @param foregroundColor The foreground color in which the finite lines will be painted, must have as many elements as the image has channels, must be valid
		 * @param backgroundColor Optional background color, nullptr to avoid using a background color
		 */
		static void paintLShapes(Frame& frame, const FiniteLines2& finiteLines, const ShapeDetector::LShapes& lShapes, const unsigned char* foregroundColor, const unsigned char* backgroundColor = nullptr);

		/**
		 * Draws L-shapes into an image.
		 * @param frame The image in which the L-shapes will be painted, must be valid
		 * @param lShapes The L-shapes to be painted
		 * @param length The length of the painted shapes e.g., the directions, in pixel, with range (0, infinity)
		 * @param foregroundColor The foreground color in which the finite lines will be painted, must have as many elements as the image has channels, must be valid
		 * @param backgroundColor Optional background color, nullptr to avoid using a background color
		 * @param offset Optional explicit offset to be added to each shape location before painting
		 */
		static void paintLShapes(Frame& frame, const ShapeDetector::LShapes& lShapes, const Scalar length, const unsigned char* foregroundColor, const unsigned char* backgroundColor = nullptr, const Vector2& offset = Vector2(0, 0));

		/**
		 * Draws T-shapes into an image.
		 * @param frame The image in which the T-shapes will be painted, must be valid
		 * @param tShapes The T-shapes to be painted
		 * @param length The length of the painted shapes e.g., the directions, in pixel, with range (0, infinity)
		 * @param foregroundColor The foreground color in which the finite lines will be painted, must have as many elements as the image has channels, must be valid
		 * @param backgroundColor Optional background color, nullptr to avoid using a background color
		 * @param offset Optional explicit offset to be added to each shape location before painting
		 */
		static void paintTShapes(Frame& frame, const ShapeDetector::TShapes& tShapes, const Scalar length, const unsigned char* foregroundColor, const unsigned char* backgroundColor = nullptr, const Vector2& offset = Vector2(0, 0));

		/**
		 * Draws X-shapes into an image.
		 * @param frame The image in which the X-shapes will be painted, must be valid
		 * @param xShapes The X-shapes to be painted
		 * @param length The length of the painted shapes e.g., the directions, in pixel, with range (0, infinity)
		 * @param foregroundColor The foreground color in which the finite lines will be painted, must have as many elements as the image has channels, must be valid
		 * @param backgroundColor Optional background color, nullptr to avoid using a background color
		 * @param offset Optional explicit offset to be added to each shape location before painting
		 */
		static void paintXShapes(Frame& frame, const ShapeDetector::XShapes& xShapes, const Scalar length, const unsigned char* foregroundColor, const unsigned char* backgroundColor = nullptr, const Vector2& offset = Vector2(0, 0));

		/**
		 * Draws shapes into an image.
		 * @param frame The image in which the shapes will be painted, must be valid
		 * @param lShapes The L-shapes to be painted
		 * @param tShapes The T-shapes to be painted
		 * @param xShapes The X-shapes to be painted
		 * @param length The length of the painted shapes e.g., the directions, in pixel, with range (0, infinity)
		 * @param drawBackground True, to draw a background below the shapes
		 * @param offset Optional explicit offset to be added to each shape location before painting
		 * @param foregroundColor Optional explicit foreground color for all shapes, nullptr to use the default colors for individual shape types
		 */
		static void paintShapes(Frame& frame, const ShapeDetector::LShapes& lShapes, const ShapeDetector::TShapes& tShapes, const ShapeDetector::XShapes& xShapes, const Scalar length, const bool drawBackground = false, const Vector2& offset = Vector2(0, 0), const uint8_t* foregroundColor = nullptr);

		/**
		 * Draws shapes into an image.
		 * @param frame The image in which the shapes will be painted, must be valid
		 * @param shapes The pointers to TwoLineShape object (eigher L-shapes, T-shapes, or X-shapes), must be valid if `size` >= 1
		 * @param size The number of provided shapes, with range [0, infinity)
		 * @param length The length of the painted shapes e.g., the directions, in pixel, with range (0, infinity)
		 * @param drawBackground True, to draw a background below the shapes
		 * @param offset Optional explicit offset to be added to each shape location before painting
		 * @param foregroundColor Optional explicit foreground color for all shapes, nullptr to use the default colors for individual shape types
		 */
		static void paintShapes(Frame& frame, const ShapeDetector::TwoLineShape* const * shapes, size_t size, const Scalar length, const bool drawBackground = false, const Vector2& offset = Vector2(0, 0), const uint8_t* foregroundColor = nullptr);

		/**
		 * Draws rectangles into an image.
		 * @param frame The image in which the rectangles will be painted, must be valid
		 * @param lShapes The L-shapes forming the rectangles
		 * @param rectangles The rectangles to be painted
		 * @param foregroundColor The foreground color in which the finite lines will be painted, must have as many elements as the image has channels, must be valid
		 * @param backgroundColor Optional background color, nullptr to avoid using a background color
		 */
		static void paintRectangles(Frame& frame, const ShapeDetector::LShapes& lShapes, const ShapeDetector::IndexedRectangles& rectangles, const unsigned char* foregroundColor, const unsigned char* backgroundColor = nullptr);

		/**
		 * Visualizes the alignment between two sets of shapes (reference shapes and candidate shapes).
		 * @param frame The frame in which the reference shapes have been determined, must be valid
		 * @param rectifiedWidth The width of the resulting rectified frame showing the alignment, in pixel, with range [1, infinity)
		 * @param rectifiedHeight The height of the resulting rectified frame showing the alignment, in pixel, with range [1, infinity)
		 * @param frame_H_rectified The homography between rectified image and given frame
		 * @param shapeReferences The reference shapes to be painted
		 * @param shapeCandidates The candidate shapes to be painted
		 * @param referenceShapesOffset The optional explicit offset for each reference shape before it will be painted
		 * @param candidateShapesOffset The optional explicit offset for each candidate shape before it will be painted
		 * @return The resulting rectified frame showing the alignment between both sets of shapes
		 */
		static Frame visualizeShapeAlignment(const Frame& frame, unsigned int rectifiedWidth, unsigned int rectifiedHeight, const SquareMatrix3& frame_H_rectified, const std::vector<const ShapeDetector::TwoLineShape*>& shapeReferences, const std::vector<const ShapeDetector::TwoLineShape*>& shapeCandidates, const Vector2& referenceShapesOffset = Vector2(0, 0), const Vector2& candidateShapesOffset = Vector2(0, 0));
};

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_DETECTOR_UTILITIES_H
