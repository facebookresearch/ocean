/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_DETECTOR_SHAPE_DETECTOR_H
#define META_OCEAN_CV_DETECTOR_SHAPE_DETECTOR_H

#include "ocean/cv/detector/Detector.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/NonMaximumSuppression.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Triangle2.h"
#include "ocean/math/Vector2.h"

#include <array>

namespace Ocean
{

namespace CV
{

namespace Detector
{

/**
 * This class is a collection of detectors for geometric shapes.
 * @ingroup cvdetector
 */
class OCEAN_CV_DETECTOR_EXPORT ShapeDetector
{
	public:

		/**
		 * This class implements the base class for all shapes based on two lines.
		 */
		class TwoLineShape
		{
			public:

				/**
				 * Definition of individual shape types.
				 */
				enum ShapeType : unsigned int
				{
					/// An invalid shape type.
					ST_INVALID = 0u,
					/// An L-shape.
					ST_SHAPE_L = 1u,
					/// A T-shape.
					ST_SHAPE_T = 2u,
					/// An X-shape.
					ST_SHAPE_X = 3u
				};

			public:

				/**
				 * Returns the type of the shape.
				 * @return The shape's type
				 */
				inline ShapeType type() const;

				/**
				 * Returns the index of the first finite line.
				 * @return Index of the first line, with range [0, infinity)
				 */
				inline unsigned int finiteLineIndex0() const;

				/**
				 * Returns the index of the second finite line.
				 * @return Index of the second line, with range [0, infinity)
				 */
				inline unsigned int finiteLineIndex1() const;

				/**
				 * Returns the index of the first or second finite line.
				 * @param index The index of the finite line of this shape, range: [0, 1]
				 * @return Index of the line, with range [0, infinity)
				 */
				inline unsigned int finiteLineIndex(const unsigned int index) const;

				/**
				 * Returns the position of this shape.
				 * @return The shape's position
				 */
				inline const Vector2& position() const;

				/**
				 * Sets or changes the position of this shape.
				 * @param position The new position
				 */
				inline void setPosition(const Vector2& position);

				/**
				 * Returns the sore of this shape.
				 * @return The shape's score
				 */
				inline Scalar score() const;

			protected:

				/**
				 * Creates an invalid shape.
				 */
				inline TwoLineShape();

				/**
				 * Creates a new shape object.
				 * @param shapeType The type of the shape, must be valid
				 * @param position The position of the shape, e.g., the corner/intersection of both finites line
				 * @param score The score of the L-shape, e.g,. the response parameter of the detector, with range [0, infinity)
				 */
				inline TwoLineShape(const ShapeType shapeType, const Vector2& position, const Scalar score);

				/**
				 * Creates a new shape object.
				 * @param shapeType The type of the shape, must be valid
				 * @param finiteLineIndex0 The index of the first finite line, with range [0, infinity)
				 * @param finiteLineIndex1 The index of the second finite line, with range [0, infinity)
				 * @param position The position of the shape, e.g., the corner/intersection of both finites line
				 * @param score The score of the L-shape, e.g,. the response parameter of the detector, with range [0, infinity)
				 */
				inline TwoLineShape(const ShapeType shapeType, const unsigned int finiteLineIndex0, const unsigned int finiteLineIndex1, const Vector2& position, const Scalar score);

			protected:

				/// The shape's type.
				ShapeType shapeType_ = ST_INVALID;

				/// The index of the first finite line.
				unsigned int finiteLineIndex0_ = (unsigned int)(-1);

				/// The index of the second finite line.
				unsigned int finiteLineIndex1_ = (unsigned int)(-1);

				/// The position of this shape.
				Vector2 position_;

				/// The score of this L-shape.
				Scalar score_;
		};

		/**
		 * Definition of a vector holding pointers to TwoLineShapes.
		 */
		typedef std::vector<const CV::Detector::ShapeDetector::TwoLineShape*> TwoLineShapes;

		/**
		 * This class implements an L-shape element like a corner of a rectangle.
		 * The L-shape is defined by to finite edges almost perpendicular to each other and having a similar end point.<br>
		 * Instead of storing the actual two finite lines defining the L-shape, this class stores the indices of the lines only.
		 *
		 * Direction of L-shapes:
		 * <pre>
		 *
		 *  leftEdge
		 *  ^
		 *  |   / direction
		 *  |  /
		 *  | /
		 *  +------> rightEdge
		 *
		 * </pre>
		 *
		 * direction = (leftEdge + rightEdge) / ||leftEdge + rightEdge||
		 */
		class LShape : public TwoLineShape
		{
			public:

				/**
				 * Creates an invalid L-shape object.
				 */
				inline LShape();

				/**
				 * Creates a new L-shape object by the L-shape's location, direction and score.
				 * @param position The position of the L-shape, e.g., the corner/intersection of both finites line
				 * @param direction The direction of the L-shape, e.g., the half vector between both finite lines, must have unit length
				 * @param edgeLeft The left edge of the L-shape, left of the direction (half vector), starting at the position, must have unit length
				 * @param edgeRight The right edge of the L-shape, left of the direction (half vector), starting at the position, must have unit length
				 * @param score The score of the L-shape, e.g,. base on the length of the finite lines, with range [0, infinity)
				 */
				inline LShape(const Vector2& position, const Vector2& direction, const Vector2& edgeLeft, const Vector2& edgeRight, const Scalar score);

				/**
				 * Creates a new L-shape object by two indices specifying the finite lines, the L-shape's location, direction and score.
				 * @param finiteLineIndex0 The index of the first finite line, with range [0, infinity)
				 * @param finiteLineIndex1 The index of the second finite line, with range [0, infinity)
				 * @param position The position of the L-shape, e.g., the corner/intersection of both finites line
				 * @param direction The direction of the L-shape, e.g., the half vector between both finite lines, must have unit length
				 * @param edgeLeft The left edge of the L-shape, left of the direction (half vector), starting at the position, must have unit length
				 * @param edgeRight The right edge of the L-shape, left of the direction (half vector), starting at the position, must have unit length
				 * @param score The score of the L-shape, e.g,. base on the length of the finite lines, with range [0, infinity)
				 */
				inline LShape(const unsigned int finiteLineIndex0, const unsigned int finiteLineIndex1, const Vector2& position, const Vector2& direction, const Vector2& edgeLeft, const Vector2& edgeRight, const Scalar score);

				/**
				 * Returns the left edge of this L-shape.
				 * @return The shape's left edge
				 */
				inline const Vector2& edgeLeft() const;

				/**
				 * Returns the right edge of this L-shape.
				 * @return The shape's right edge
				 */
				inline const Vector2& edgeRight() const;

				/**
				 * Returns the direction of this L-shape.
				 * @return The shape's direction
				 */
				inline const Vector2& direction() const;

			protected:

				/// The left edge of this L-shape.
				Vector2 edgeLeft_;

				/// The right edge of this L-shape.
				Vector2 edgeRight_;

				/// The direction of this L-shape.
				Vector2 direction_;
		};

		/**
		 * This class implements a T-shape element like a junction connecting two lines, with one line having the connecting at an end point and the other in the middle of th line.
		 * The direction of a T-shape is defined by the line which is connected at one of the end points and pointing away from the connection:
		 * \code
		 * ------------------
		 *         |
		 *         |
		 *         |
		 *         |
		 *         V
		 *      direction
		 * \endcode
		 */
		class TShape : public TwoLineShape
		{
			public:

				/**
				 * Creates an invalid T-shape object.
				 */
				inline TShape();

				/**
				 * Creates a new T-shape object by the T-shape's location and direction.
				 * @param position The position of the L-shape, e.g., the corner/intersection of both finites line
				 * @param direction The direction of the L-shape, e.g., the half vector between both finite lines, must have unit length
				 * @param score The optional score of the L-shape, e.g,. the response parameter of the detector, with range [0, infinity)
				 */
				inline TShape(const Vector2& position, const Vector2& direction, const Scalar score = Scalar(0));

				/**
				 * Creates a new T-shape object by two indices specifying the finite lines, the T-shape's location and direction.
				 * @param finiteLineIndex0 The index of the first finite line, with range [0, infinity)
				 * @param finiteLineIndex1 The index of the second finite line, with range [0, infinity)
				 * @param position The position of the L-shape, e.g., the corner/intersection of both finites line
				 * @param direction The direction of the L-shape, e.g., the half vector between both finite lines, must have unit length
				 * @param score The optional score of the L-shape, e.g,. the response parameter of the detector, with range [0, infinity)
				 */
				inline TShape(const unsigned int finiteLineIndex0, const unsigned int finiteLineIndex1, const Vector2& position, const Vector2& direction, const Scalar score = Scalar(0));

				/**
				 * Returns the direction of this T-shape.
				 * @return The shape's direction
				 */
				inline const Vector2& direction() const;

			protected:

				/// The direction of this T-shape.
				Vector2 direction_;
		};

		/**
		 * This class implements a X-shape element like a crossing of two lines, with both lines not crossing near to an end point.
		 * The directions of a X-shape are defined by the two lines:
		 * <pre>
		 *         |
		 *         |
		 *         |
		 * -----------------> direction0
		 *         |
		 *         |
		 *         V
		 *      direction1
		 * </pre>
		 */
		class OCEAN_CV_DETECTOR_EXPORT XShape : public TwoLineShape
		{
			public:

				/**
				 * Creates an invalid X-shape object.
				 */
				inline XShape();

				/**
				 * Creates a new X-shape object by the X-shape's location and directions.
				 * @param position The position of the X-shape, e.g., the corner/intersection of both finites line
				 * @param direction0 The first direction of the X-shape, must have unit length
				 * @param direction1 The second direction of the X-shape, must have unit length
				 * @param score The optional score of the L-shape, e.g,. the response parameter of the detector, with range [0, infinity)
				 */
				inline XShape(const Vector2& position, const Vector2& direction0, const Vector2& direction1, const Scalar score = Scalar(0));

				/**
				 * Creates a new X-shape object by two indices specifying the finite lines, the X-shape's location and directions.
				 * @param finiteLineIndex0 The index of the first finite line, with range [0, infinity)
				 * @param finiteLineIndex1 The index of the second finite line, with range [0, infinity)
				 * @param position The position of the X-shape, e.g., the corner/intersection of both finites line
				 * @param direction0 The first direction of the X-shape, must have unit length
				 * @param direction1 The second direction of the X-shape, must have unit length
				 * @param score The optional score of the L-shape, e.g,. the response parameter of the detector, with range [0, infinity)
				 */
				inline XShape(const unsigned int finiteLineIndex0, const unsigned int finiteLineIndex1, const Vector2& position, const Vector2& direction0, const Vector2& direction1, const Scalar score = Scalar(0));

				/**
				 * Verifies whether this X-shape is valid based on underlying image content.
				 * The X-shape is valid if all four edges of the shape have almost the same color.
				 * @param yFrame The frame in which the X-shape is located, must have pixel format FORMAT_Y8, must be valid
				 * @param width The width of the frame in pixel, with range [1, infinity)
				 * @param height The height of the frame in pixel, with range [1, infinity)
				 * @param darkShape True, if the given shape is based on a dark edge; False, if the given shape is based on a bright edge
				 * @param minimalValueRange The minimal value range between the darkest and brightest color intensity to start the investigation, with range [0, 255)
				 * @param sampleOffset The sampling offset to be used for verification, with range [0, infinity)
				 * @param samples The number of samples to be used for verification, with range [1, infinity)
				 * @param yFramePaddingElements Optional number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 * @return True, if succeeded
				 */
				bool verifyShape(const uint8_t* yFrame, const unsigned int width, const unsigned int height, const bool darkShape, const unsigned int minimalValueRange, const unsigned int sampleOffset = 2u, const unsigned int samples = 4u, const unsigned int yFramePaddingElements = 0u) const;

				/**
				 * Returns the first direction of this X-shape.
				 * @return The shape's first direction
				 */
				inline const Vector2& direction0() const;

				/**
				 * Return the second direction of this X-shape.
				 * @return The shape's second direction
				 */
				inline const Vector2& direction1() const;

			protected:

				/// The direction of the first line of this X-shape.
				Vector2 direction0_;

				/// The direction of the second line of this X-shape.
				Vector2 direction1_;
		};

		/**
		 * Definition of a vector holding L-shape objects.
		 */
		typedef std::vector<LShape> LShapes;

		/**
		 * Definition of a vector holding T-shape objects.
		 */
		typedef std::vector<TShape> TShapes;

		/**
		 * Definition of a vector holding X-shape objects.
		 */
		typedef std::vector<XShape> XShapes;

		/**
		 * Definition of an array holding four indices e.g., of L-shape objects.
		 */
		typedef std::array<Index32, 4> IndexedRectangle;

		/**
		 * Definition of a vector holding rectangles.
		 */
		typedef std::vector<IndexedRectangle> IndexedRectangles;

		/**
		 * A rectangle defined by its four corners (counter-clockwise direction)
		 */
		typedef std::array<Vector2, 4> Rectangle;

		/**
		 * A vector of rectangles
		 */
		typedef std::vector<Rectangle> Rectangles;

		/**
		 * This class implements a shape detector mainly based on gradients.
		 */
		class OCEAN_CV_DETECTOR_EXPORT PatternDetectorGradientBased
		{
			public:

				/**
				 * Detects shapes in a given image.
				 * @param yFrame The frame in which the shapes will be detected, must be valid
				 * @param width The width of the frame in pixel, with range [1, infinity)
				 * @param height The height of the frame in pixel, with rang [1, infinity)
				 * @param lShapes The resulting detected L-shapes
				 * @param tShapes The resulting detected T-shapes
				 * @param xShapes The resulting detected X-shapes
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment
				 * @param minimalThreshold The minimal threshold for a detected shape, with range (0, infinity)
				 * @param shapeWidth **TODO** document once finalized
				 * @param shapeHeight **TODO** document once finalized
				 * @param stepSize **TODO** document once finalized
				 * @param topBand **TODO** document once finalized
				 * @param bottomBand **TODO** document once finalized
				 * @param minimalDelta **TODO** document once finalized
				 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 */
				static void detectShapes(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const int sign, const double minimalThreshold = 5.0, const unsigned int shapeWidth = 15u, const unsigned int shapeHeight = 15u, const unsigned int stepSize = 3u, const unsigned int topBand = 4u, const unsigned int bottomBand = 4u, const unsigned int minimalDelta = 5u, const unsigned int framePaddingElements = 0u);

				/**
				 * Determines the detector response for an gradient-based T-shape detector which is axis aligned.
				 * @param linedIntegralHorizontalSignedGradient The lined integral image providing horizontal signed gradients of the original input image, must be valid
				 * @param linedIntegralHorizontalAbsoluteGradient The lined integral image providing horizontal absolute gradients of the original input image, must be valid
				 * @param linedIntegralVerticalSignedGradient The lined integral image providing vertical signed gradients of the original input image, must be valid
				 * @param linedIntegralVerticalAbsoluteGradient The lined integral image providing vertical absolute gradients of the original input image, must be valid
				 * @param imageWidth The width of the actual image which has been used to determine the integral images, in pixel, with range [1, infinity)
				 * @param imageHeight The height of the actual image which has been used to determine the integral images, in pixel, with range [1, infinity)
				 * @param x The horizontal location at which the detector response will be determined, with range [0, imageWidth)
				 * @param y The vertical location at which the detector response will be determined, with range [0, imageHeight)
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment
				 * @param shapeWidth **TODO** document once finalized
				 * @param shapeHeight **TODO** document once finalized
				 * @param stepSize **TODO** document once finalized
				 * @param topBand **TODO** document once finalized
				 * @param bottomBand **TODO** document once finalized
				 * @param minimalDelta **TODO** document once finalized
				 * @param horizontalSignedGradientPaddingElements The number of padding elements at the end of each row of the lined integral image for horizontal signed gradients, in elements, with range [0, infinity)
				 * @param horizontalAbsoluteGradientPaddingElements The number of padding elements at the end of each row of the lined integral image for horizontal absolute gradients, in elements, with range [0, infinity)
				 * @param verticalSignedGradientPaddingElements The number of padding elements at the end of each row of the lined integral image for vertical signed gradients, in elements, with range [0, infinity)
				 * @param verticalAbsoluteGradientPaddingElements The number of padding elements at the end of each row of the lined integral image for vertical absolute gradients, in elements, with range [0, infinity)
				 * @return The response of the T-shape detector, with range [0, infinity)
				 */
				static double tShapeResponse(const int32_t* linedIntegralHorizontalSignedGradient, const uint32_t* linedIntegralHorizontalAbsoluteGradient, const int32_t* linedIntegralVerticalSignedGradient, const uint32_t* linedIntegralVerticalAbsoluteGradient, const unsigned int imageWidth, const unsigned int imageHeight, const unsigned int x, const unsigned int y, const int sign, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int stepSize, const unsigned int topBand, const unsigned int bottomBand, const unsigned int minimalDelta = 5u, const unsigned int horizontalSignedGradientPaddingElements = 0u, const unsigned int horizontalAbsoluteGradientPaddingElements = 0u, const unsigned int verticalSignedGradientPaddingElements = 0u, const unsigned int verticalAbsoluteGradientPaddingElements = 0u);
		};

		/**
		 * This class implements a shape detector mainly based on variance.
		 */
		class OCEAN_CV_DETECTOR_EXPORT PatternDetectorVarianceBased
		{
			public:

				/**
				 * Detects shapes in a given image.
				 * @param yFrame The frame in which the shapes will be detected, must be valid
				 * @param width The width of the frame in pixel, with range [1, infinity)
				 * @param height The height of the frame in pixel, with rang [1, infinity)
				 * @param lShapes The resulting detected L-shapes
				 * @param tShapes The resulting detected T-shapes
				 * @param xShapes The resulting detected X-shapes
				 * @param minimalThreshold The minimal threshold for a detected shape, with range (0, infinity)
				 * @param shapeWidth **TODO** document once finalized
				 * @param shapeHeight **TODO** document once finalized
				 * @param stepSize **TODO** document once finalized
				 * @param topBand **TODO** document once finalized
				 * @param bottomBand **TODO** document once finalized
				 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 */
				static void detectShapes(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const float minimalThreshold = 5.421f, const unsigned int shapeWidth = 15u, const unsigned int shapeHeight = 15u, const unsigned int stepSize = 3u, const unsigned int topBand = 4u, const unsigned int bottomBand = 4u, const unsigned int framePaddingElements = 0u);

				/**
				 * Determines the detector response for an variance-based T-shape detector which is axis aligned.
				 * @param linedIntegral The lined integral image of pixel intensities of the original input image, must be valid
				 * @param linedIntegralSquare The lined integral image of squared pixel intensities of the original input image, must be valid
				 * @param width The width of the actual image which has been used to determine the integral images, in pixel, with range [1, infinity)
				 * @param height The height of the actual image which has been used to determine the integral images, in pixel, with range [1, infinity)
				 * @param x The horizontal location at which the detector response will be determined, with range [0, imageWidth)
				 * @param y The vertical location at which the detector response will be determined, with range [0, imageHeight)
				 * @param shapeWidth **TODO** document once finalized
				 * @param shapeHeight **TODO** document once finalized
				 * @param stepSize **TODO** document once finalized
				 * @param topBand **TODO** document once finalized
				 * @param bottomBand **TODO** document once finalized
				 * @param linedIntegralPaddingElements The number of padding elements at the end of each row of the lined integral image, in elements, with range [0, infinity)
				 * @param linedIntegralSquaredPaddingElements The number of padding elements at the end of each row of the lined integral image for squared pixel intensities, in elements, with range [0, infinity)
				 * @return The response of the T-shape detector, with range [0, infinity)
				 */
				static float tShapeResponse(const uint32_t* linedIntegral, const uint64_t* linedIntegralSquare, const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const unsigned int shapeWidth, const unsigned int shapeHeight, const unsigned int stepSize, const unsigned int topBand, const unsigned int bottomBand, const unsigned int linedIntegralPaddingElements = 0u, const unsigned int linedIntegralSquaredPaddingElements = 0u);
		};

		/**
		 * This class implements a shape detector based on gradients and variance.
		 */
		class OCEAN_CV_DETECTOR_EXPORT PatternDetectorGradientVarianceBased
		{
			protected:

				/// The width of the T-shape in pixel, with range [shapeStepSize_ + 2, infinity), must be odd.
				static constexpr unsigned int shapeWidth_ = 15u;

				/// The height of the T-shape in pixel, with range [shapeStepSize_ + 1, infinity)
				static constexpr unsigned int shapeHeight_ = 15u;

				/// The step size of the T-shape in pixel, with range [1, infinity), must be odd.
				static constexpr unsigned int shapeStepSize_ = 3u;

				/// The band size of the T-shape in pixel, with range [1, infinity).
				static constexpr unsigned int shapeBandSize_ = 4u;

				/// The width of the T-shape divided by 2, in pixel, beware: shapeWidth_2 * 2 + 1 == shapeWidth
				static constexpr unsigned int shapeWidth_2_ = shapeWidth_ / 2u;

				/// The step size of the T-shape divided by 2, in pixel, beware: shapeStepSize_2 * 2 + 1 == shapeStepSize
				static constexpr unsigned int shapeStepSize_2_ = shapeStepSize_ / 2u;

				static_assert(shapeWidth_ >= 1u && shapeWidth_ % 2u == 1u, "Invalid shape width!");
				static_assert(shapeHeight_ >= shapeStepSize_ + 1u, "Invalid shape height!");
				static_assert(shapeStepSize_ >= 1u && shapeStepSize_ % 2u == 1u, "Invalid shape step size!");
				static_assert(shapeBandSize_ >= 1u, "Invalid shape band size!");

				static_assert(shapeWidth_ >= shapeStepSize_ + 2u, "Invalid shape height!");
				static_assert(shapeHeight_ >= shapeStepSize_ + 1u, "Invalid shape height!");

			public:

				/**
				 * Detects shapes in a given image while applying floating-point precision.
				 * @param yFrame The frame in which the shapes will be detected, must be valid
				 * @param width The width of the frame in pixel, with range [1, infinity)
				 * @param height The height of the frame in pixel, with range [1, 2^16 / width)
				 * @param lShapes The resulting detected L-shapes
				 * @param tShapes The resulting detected T-shapes
				 * @param xShapes The resulting detected X-shapes
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment, 0 to accept shapes with both signs
				 * @param minimalThreshold The minimal threshold for a detected shape, with range (0, infinity)
				 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 * @param topDownResponseFrame Optional resulting response frame for the top-down direction
				 * @param bottomUpResponseFrame Optional resulting response frame for the bottom-up direction
				 */
				static void detectShapesF(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const int sign, const float minimalThreshold = 6.0f, const unsigned int framePaddingElements = 0u, Frame* topDownResponseFrame = nullptr, Frame* bottomUpResponseFrame = nullptr);

				/**
				 * Detects shapes in a given image while applying integer precision.
				 * @param yFrame The frame in which the shapes will be detected, must be valid
				 * @param width The width of the frame in pixel, with range [1, infinity)
				 * @param height The height of the frame in pixel, with range [1, 2^16 / width)
				 * @param lShapes The resulting detected L-shapes
				 * @param tShapes The resulting detected T-shapes
				 * @param xShapes The resulting detected X-shapes
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment, 0 to accept shapes with both signs
				 * @param minimalThreshold The minimal threshold for a detected shape, with range (0, infinity)
				 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
				 * @param topDownResponseFrame Optional resulting response frame for the top-down direction
				 * @param bottomUpResponseFrame Optional resulting response frame for the bottom-up direction
				 */
				static void detectShapesI(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const int sign, const float minimalThreshold = 6.0f, const unsigned int framePaddingElements = 0u, Frame* topDownResponseFrame = nullptr, Frame* bottomUpResponseFrame = nullptr);

			protected:

				/**
				 * Returns the width of the horizontal response frame.
				 * @param width The width of the image for which the horizontal response will be determined, in pixel, with range [20, infinity)
				 * @return The width of the horizontal response frame, in pixel, which is: width - shapeWidth + 1
				 */
				static inline unsigned int determineHorizontalResponseWidth(const unsigned int width);

				/**
				 * Returns the height of the horizontal response frame.
				 * @param height The height of the image for which the horizontal response will be determined, in pixel, with range [20, infinity)
				 * @return The height of the horizontal response frame, in pixel, which is: height - (shapeBandSize * 2 + shapeStepSize) + 1
				 */
				static inline unsigned int determineHorizontalResponseHeight(const unsigned int height);

				/**
				 * Returns the width of the vertical response frame.
				 * @param width The width of the image for which the vertical response will be determined, in pixel, with range [20, infinity)
				 * @return The width of the vertical response frame, in pixel, which is: width - (shapeBandSize * 2 + shapeStepSize) + 1
				 */
				static inline unsigned int determineVerticalResponseWidth(const unsigned int width);

				/**
				 * Returns the height of the vertical response frame.
				 * @param height The height of the image for which the vertical response will be determined, in pixel, with range [20, infinity)
				 * @return The height of the vertical response frame, in pixel, which is: height - (shapeHeight - shapeStepSize) + 1
				 */
				static inline unsigned int determineVerticalResponseHeight(const unsigned int height);

				/**
				 * Returns the translation offset in x-direction between the horizontal top-down response location and the frame.
				 * @return The translation between response and frame in x-direction
				 */
				static constexpr int frameX_T_topDownHorizontalResponseX();

				/**
				 * Returns the translation offset in y-direction between the horizontal top-down response location and the frame.
				 * @return The translation between response and frame in y-direction
				 */
				static constexpr int frameY_T_topDownHorizontalResponseY();

				/**
				 * Returns the translation offset in x-direction between the vertical top-down response location and the frame.
				 * @return The translation between response and frame in x-direction
				 */
				static constexpr int frameX_T_topDownVerticalResponseX();

				/**
				 * Returns the translation offset in y-direction between the vertical top-down response location and the frame.
				 * @return The translation between response and frame in y-direction
				 */
				static constexpr int frameY_T_topDownVerticalResponseY();

				/**
				 * Returns the translation offset in x-direction between the top-down response location and the frame.
				 * @return The translation between response and frame in x-direction
				 */
				static constexpr int frameX_T_topDownResponseX();

				/**
				 * Returns the translation offset in y-direction between the top-down response location and the frame.
				 * @return The translation between response and frame in y-direction
				 */
				static constexpr int frameY_T_topDownResponseY();

				/**
				 * Returns the translation offset in x-direction between the horizontal bottom-up response location and the frame.
				 * @return The translation between response and frame in x-direction
				 */
				static constexpr int frameX_T_bottomUpHorizontalResponseX();

				/**
				 * Returns the translation offset in y-direction between the horizontal bottom-up response location and the frame.
				 * @return The translation between response and frame in y-direction
				 */
				static constexpr int frameY_T_bottomUpHorizontalResponseY();

				/**
				 * Returns the translation offset in x-direction between the vertical bottom-up response location and the frame.
				 * @return The translation between response and frame in x-direction
				 */
				static constexpr int frameX_T_bottomUpVerticalResponseX();

				/**
				 * Returns the translation offset in y-direction between the vertical bottom-up response location and the frame.
				 * @return The translation between response and frame in y-direction
				 */
				static constexpr int frameY_T_bottomUpVerticalResponseY();

				/**
				 * Returns the translation offset in x-direction between the bottom-up response location and the frame.
				 * @return The translation between response and frame in x-direction
				 */
				static constexpr int frameX_T_bottomUpResponseX();

				/**
				 * Returns the translation offset in y-direction between the bottom-up response location and the frame.
				 * @return The translation between response and frame in y-direction
				 */
				static constexpr int frameY_T_bottomUpResponseY();

				/**
				 * Determines the integer horizontal responses for the T-shape and stores the results in a given response image/buffer.
				 * The resolution of the response image is smaller than the original yFrame, the buffer does not contain zero responses at the boundaries of the image.<br>
				 * The response is stored at the upper left pixel in the response area, the response area has size (shapeWidth)x(shapeBandSize * 2 + shapeStepSize).<br>
				 * The response offset for the top-down T-shape is (shapeWidth / 2)x(shapeBandSize + shapeStepSize / 2).
				 * @param linedIntegralAndSquared The lined mean and mean squared integral image of the original yFrame, with resolution ((width + 1 * 2)x(height + 1), must be valid
				 * @param width The width of the original yFrame, in pixel, with range [20, infinity)
				 * @param height The height of the original yFrame, in pixel, with range [20, 2^16 / width)
				 * @param horizontalResponses Resulting horizontal response frame, with resolution (width - shapeWidth + 1)x(height - (shapeBandSize * 2 + shapeStepSize) + 1), must be valid
				 * @param linedIntegralAndSquaredPaddingElements Optional number of padding elements at the end of each integral image, in elements, with range [0, infinity)
				 * @param horizontalResponsesPaddingElements Optional number of padding elements at the end of each response image, in elements, with range [0, infinity)
				 * @tparam tUseSIMD True, to use SIMD instructions if possible; False; to use standard compiler result
				 * @see CV::IntegralImage::createLinedImageAndSquared().
				 */
				template <bool tUseSIMD>
				static void determineHorizontalResponsesI(const uint32_t* linedIntegralAndSquared, const unsigned int width, const unsigned int height, int32_t* horizontalResponses, const unsigned int linedIntegralAndSquaredPaddingElements = 0u, const unsigned int horizontalResponsesPaddingElements = 0u);

				/**
				 * Determines the integer horizontal responses for the T-shape and stores the results in a given response image/buffer.
				 * The resolution of the response image is smaller than the original yFrame, the buffer does not contain zero responses at the boundaries of the image.<br>
				 * The response is stored at the upper left pixel in the response area, the response area has size (shapeWidth)x(shapeBandSize * 2 + shapeStepSize).<br>
				 * The response offset for the top-down T-shape is (shapeWidth / 2)x(shapeBandSize + shapeStepSize / 2).
				 * @param linedIntegral The lined mean integral image of the original yFrame, with resolution (width + 1)x(height + 1), must be valid
				 * @param linedIntegralSquared The lined mean squared integral image of the original yFrame, with resolution (width + 1)x(height + 1), must be valid
				 * @param width The width of the original yFrame, in pixel, with range [20, infinity)
				 * @param height The height of the original yFrame, in pixel, with range [20, infinity)
				 * @param horizontalResponses Resulting horizontal response frame, with resolution (width - shapeWidth + 1)x(height - (shapeBandSize * 2 + shapeStepSize) + 1), must be valid
				 * @param linedIntegralPaddingElements Optional number of padding elements at the end of each integral image, in elements, with range [0, infinity)
				 * @param linedIntegralSquaredPaddingElements Optional number of padding elements at the end of each integral squared image, in elements, with range [0, infinity)
				 * @param horizontalResponsesPaddingElements Optional number of padding elements at the end of each response image, in elements, with range [0, infinity)
				 * @tparam tUseSIMD True, to use SIMD instructions if possible; False; to use standard compiler result
				 * @see CV::IntegralImage::createLinedImageAndSquared().
				 */
				template <bool tUseSIMD>
				static void determineHorizontalResponsesI(const uint32_t* linedIntegral, const uint64_t* linedIntegralSquared, const unsigned int width, const unsigned int height, int32_t* horizontalResponses, const unsigned int linedIntegralPaddingElements = 0u, const unsigned int linedIntegralSquaredPaddingElements = 0u, const unsigned int horizontalResponsesPaddingElements = 0u);

				/**
				 * Determines the floating-point horizontal responses for the T-shape and stores the results in a given response image/buffer.
				 * The resolution of the response image is smaller than the original yFrame, the buffer does not contain zero responses at the boundaries of the image.<br>
				 * The response is stored at the upper left pixel in the response area, the response area has size (shapeWidth)x(shapeBandSize * 2 + shapeStepSize).<br>
				 * The response offset for the top-down T-shape is (shapeWidth / 2)x(shapeBandSize + shapeStepSize / 2).
				 * @param linedIntegralAndSquared The lined mean and mean squared integral image of the original yFrame, with resolution ((width + 1 * 2)x(height + 1), must be valid
				 * @param width The width of the original yFrame, in pixel, with range [20, infinity)
				 * @param height The height of the original yFrame, in pixel, with range [20, 2^16 / width)
				 * @param horizontalResponses Resulting horizontal response frame, with resolution (width - shapeWidth + 1)x(height - (shapeBandSize * 2 + shapeStepSize) + 1), must be valid
				 * @param linedIntegralAndSquaredPaddingElements Optional number of padding elements at the end of each integral image, in elements, with range [0, infinity)
				 * @param horizontalResponsesPaddingElements Optional number of padding elements at the end of each response image, in elements, with range [0, infinity)
				 * @tparam tSquaredResponse True, to determine the squared response (avoiding sqrt calculations and normalizing with variances); False, to determine the responses normalized with deviations
				 */
				template <bool tSquaredResponse>
				static void determineHorizontalResponsesF(const uint32_t* linedIntegralAndSquared, const unsigned int width, const unsigned int height, float* horizontalResponses, const unsigned int linedIntegralAndSquaredPaddingElements = 0u, const unsigned int horizontalResponsesPaddingElements = 0u);

				/**
				 * Determines the integer vertical responses for the T-shape and stores the results in a given response image/buffer.
				 * The resolution of the response image is smaller than the original yFrame, the buffer does not contain zero responses at the boundaries of the image.<br>
				 * The response is stored at the upper left pixel in the response area, the response area has size (shapeBandSize * 2 + shapeStepSize)x(shapeHeight - shapeStepSize).<br>
				 * The response offset for the top-down T-shape is (shapeBandSize + shapeStepSize_ / 2)x(-shapeStepSize / 2 - 1).
				 * @param linedIntegralAndSquared The lined mean and mean squared integral image of the original yFrame, with resolution ((width + 1 * 2)x(height + 1), must be valid
				 * @param width The width of the original yFrame, in pixel, with range [20, infinity)
				 * @param height The height of the original yFrame, in pixel, with range [20, 2^16 / width)
				 * @param verticalResponses Resulting vertical response frame, with resolution (width - (shapeBandSize * 2 + shapeStepSize) + 1)x(height - (shapeHeight - shapeStepSize) + 1), must be valid
				 * @param linedIntegralAndSquaredPaddingElements Optional number of padding elements at the end of each integral image, in elements, with range [0, infinity)
				 * @param verticalResponsesPaddingElements Optional number of padding elements at the end of each response image, in elements, with range [0, infinity)
				 * @tparam tUseSIMD True, to use SIMD instructions if possible; False; to use standard compiler result
				 */
				template <bool tUseSIMD>
				static void determineVerticalResponsesI(const uint32_t* linedIntegralAndSquared, const unsigned int width, const unsigned int height, int32_t* verticalResponses, const unsigned int linedIntegralAndSquaredPaddingElements = 0u, const unsigned int verticalResponsesPaddingElements = 0u);

				/**
				 * Determines the integer vertical responses for the T-shape and stores the results in a given response image/buffer.
				 * The resolution of the response image is smaller than the original yFrame, the buffer does not contain zero responses at the boundaries of the image.<br>
				 * The response is stored at the upper left pixel in the response area, the response area has size (shapeBandSize * 2 + shapeStepSize)x(shapeHeight - shapeStepSize).<br>
				 * The response offset for the top-down T-shape is (shapeBandSize + shapeStepSize_ / 2)x(-shapeStepSize / 2 - 1).
				 * @param linedIntegral The lined mean integral image of the original yFrame, with resolution (width + 1)x(height + 1), must be valid
				 * @param linedIntegralSquared The lined mean squared integral image of the original yFrame, with resolution (width + 1)x(height + 1), must be valid
				 * @param width The width of the original yFrame, in pixel, with range [20, infinity)
				 * @param height The height of the original yFrame, in pixel, with range [20, infinity)
				 * @param verticalResponses Resulting vertical response frame, with resolution (width - (shapeBandSize * 2 + shapeStepSize) + 1)x(height - (shapeHeight - shapeStepSize) + 1), must be valid
				 * @param linedIntegralPaddingElements Optional number of padding elements at the end of each integral image, in elements, with range [0, infinity)
				 * @param linedIntegralSquaredPaddingElements Optional number of padding elements at the end of each integral squared image, in elements, with range [0, infinity)
				 * @param verticalResponsesPaddingElements Optional number of padding elements at the end of each response image, in elements, with range [0, infinity)
				 * @tparam tUseSIMD True, to use SIMD instructions if possible; False; to use standard compiler result
				 */
				template <bool tUseSIMD>
				static void determineVerticalResponsesI(const uint32_t* linedIntegral, const uint64_t* linedIntegralSquared, const unsigned int width, const unsigned int height, int32_t* verticalResponses, const unsigned int linedIntegralPaddingElements = 0u, const unsigned int linedIntegralSquaredPaddingElements = 0u, const unsigned int verticalResponsesPaddingElements = 0u);

				/**
				 * Determines the floating-point vertical responses for the T-shape and stores the results in a given response image/buffer.
				 * The resolution of the response image is smaller than the original yFrame, the buffer does not contain zero responses at the boundaries of the image.<br>
				 * The response is stored at the upper left pixel in the response area, the response area has size (shapeBandSize * 2 + shapeStepSize)x(shapeHeight - shapeStepSize).<br>
				 * The response offset for the top-down T-shape is (shapeBandSize + shapeStepSize_ / 2)x(-shapeStepSize / 2 - 1).
				 * @param linedIntegralAndSquared The lined mean and mean squared integral image of the original yFrame, with resolution ((width + 1 * 2)x(height + 1), must be valid
				 * @param width The width of the original yFrame, in pixel, with range [20, infinity)
				 * @param height The height of the original yFrame, in pixel, with range [20, 2^16 / width)
				 * @param verticalResponses Resulting vertical response frame, with resolution (width - (shapeBandSize * 2 + shapeStepSize) + 1)x(height - (shapeHeight - shapeStepSize) + 1), must be valid
				 * @param linedIntegralAndSquaredPaddingElements Optional number of padding elements at the end of each integral image, in elements, with range [0, infinity)
				 * @param verticalResponsesPaddingElements Optional number of padding elements at the end of each response image, in elements, with range [0, infinity)
				 * @tparam tSquaredResponse True, to determine the squared response (avoiding sqrt calculations and normalizing with variances); False, to determine the responses normalized with deviations
				 */
				template <bool tSquaredResponse>
				static void determineVerticalResponsesF(const uint32_t* linedIntegralAndSquared, const unsigned int width, const unsigned int height, float* verticalResponses, const unsigned int linedIntegralAndSquaredPaddingElements = 0u, const unsigned int verticalResponsesPaddingElements = 0u);

				/**
				 * Determines the floating-point T-shape responses for the top-down direction.
				 * @param horizontalResponses The horizontal response frame, frame, with resolution (width - shapeWidth + 1)x(height - (shapeBandSize * 2 + shapeStepSize) + 1), must be valid
				 * @param verticalResponses The vertical response frame, with resolution (width - (shapeBandSize * 2 + shapeStepSize) + 1)x(height - (shapeHeight - shapeStepSize) + 1), must be valid
				 * @param width The width of the original yFrame, in pixel, with range [20, infinity)
				 * @param height The height of the original yFrame, in pixel, with range [20, infinity)
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment, 0 to accept shapes with both signs
				 * @param minimalThreshold The minimal threshold for a detected shape, with range (0, infinity)
				 * @param nonMaximumSuppression The non-maximum-suppression object in which the responses will be stored, with resolution (width)x(height)
				 * @param responseFrame Optional resulting response frame if `tCreateResponseFrame = true`; nullptr if `tCreateResponseFrame = false`
				 * @tparam tSquaredResponse True, to determine the squared response (avoiding sqrt calculations and normalizing with variances); False, to determine the responses normalized with deviations
				 * @tparam tCreateResponseFrame True, to create a response frame
				 */
				template <bool tSquaredResponse, bool tCreateResponseFrame = false>
				static void determineTopDownResponsesF(const float* horizontalResponses, const float* verticalResponses, const unsigned int width, const unsigned int height, const int32_t sign, const float minimalThreshold, CV::NonMaximumSuppression<float>& nonMaximumSuppression, Frame* responseFrame = nullptr);

				/**
				 * Determines the floating-point T-shape responses for the bottom-up direction.
				 * @param horizontalResponses The horizontal response frame, frame, with resolution (width - shapeWidth + 1)x(height - (shapeBandSize * 2 + shapeStepSize) + 1), must be valid
				 * @param verticalResponses The vertical response frame, with resolution (width - (shapeBandSize * 2 + shapeStepSize) + 1)x(height - (shapeHeight - shapeStepSize) + 1), must be valid
				 * @param width The width of the original yFrame, in pixel, with range [20, infinity)
				 * @param height The height of the original yFrame, in pixel, with range [20, infinity)
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment, 0 to accept shapes with both signs
				 * @param minimalThreshold The minimal threshold for a detected shape, with range (0, infinity)
				 * @param nonMaximumSuppression The non-maximum-suppression object in which the responses will be stored, with resolution (width)x(height)
				 * @param responseFrame Optional resulting response frame if `tCreateResponseFrame = true`; nullptr if `tCreateResponseFrame = false`
				 * @tparam tSquaredResponse True, to determine the squared response (avoiding sqrt calculations and normalizing with variances); False, to determine the responses normalized with deviations
				 * @tparam tCreateResponseFrame True, to create a response frame
				 */
				template <bool tSquaredResponse, bool tCreateResponseFrame = false>
				static void determineBottomUpResponsesF(const float* horizontalResponses, const float* verticalResponses, const unsigned int width, const unsigned int height, const int32_t sign, const float minimalThreshold, CV::NonMaximumSuppression<float>& nonMaximumSuppression, Frame* responseFrame = nullptr);

				/**
				 * Determines the integer T-shape responses for the top-down and bottom-up direction.
				 * @param horizontalResponses The horizontal response frame, frame, with resolution (width - shapeWidth + 1)x(height - (shapeBandSize * 2 + shapeStepSize) + 1), must be valid
				 * @param verticalResponses The vertical response frame, with resolution (width - (shapeBandSize * 2 + shapeStepSize) + 1)x(height - (shapeHeight - shapeStepSize) + 1), must be valid
				 * @param width The width of the original yFrame, in pixel, with range [20, infinity)
				 * @param height The height of the original yFrame, in pixel, with range [20, infinity)
				 * @param sign The sign of the shape to be detected, -1 for shapes with dark edges and bright environment, 1 for shapes with bright edges and dark environment, 0 to accept shapes with both signs
				 * @param minimalSqrThreshold The minimal squared threshold for a detected shape, with range (0, infinity)
				 * @param nonMaximumSuppressionTopDown The non-maximum-suppression object in which the top-down responses will be stored, with resolution (width)x(height)
				 * @param nonMaximumSuppressionBottomUp The non-maximum-suppression object in which the bottom-up responses will be stored, with resolution (width)x(height)
				 * @param responseFrameTopDown Optional resulting response frame for top-down responses if `tCreateResponseFrame = true`; nullptr if `tCreateResponseFrame = false`
				 * @param responseFrameBottomUp Optional resulting response frame for bottom-up responses if `tCreateResponseFrame = true`; nullptr if `tCreateResponseFrame = false`
				 * @tparam tCreateResponseFrame True, to create a response frame
				 */
				template <bool tCreateResponseFrame = false>
				static void determineResponsesI(const int32_t* horizontalResponses, const int32_t* verticalResponses, const unsigned int width, const unsigned int height, const int32_t sign, const uint32_t minimalSqrThreshold, CV::NonMaximumSuppression<uint32_t>& nonMaximumSuppressionTopDown, CV::NonMaximumSuppression<uint32_t>& nonMaximumSuppressionBottomUp, Frame* responseFrameTopDown = nullptr, Frame* responseFrameBottomUp = nullptr);

				/**
				 * Returns whether the sign of two responses matches the expected sign.
				 * @param sign the expected sign, -1 for negative values, +1 for positive values, 0 to expect any sign as long both responses have the same sign
				 * @param horizontalResponse The signed horizontal response to check, with range (-infinity, infinity)
				 * @param verticalResponse The signed vertical response to check, with range (-infinity, infinity)
				 * @return True, if so
				 */
				static inline bool haveCorrectSign(const int32_t sign, const float horizontalResponse, const float verticalResponse);

				/**
				 * Returns whether the sign of two responses matches the expected sign.
				 * @param sign the expected sign, -1 for negative values, +1 for positive values, 0 to expect any sign as long both responses have the same sign
				 * @param horizontalResponse The signed horizontal response to check, with range (-infinity, infinity)
				 * @param verticalResponse The signed vertical response to check, with range (-infinity, infinity)
				 * @return True, if so
				 */
				static inline bool haveCorrectSign(const int32_t sign, const int32_t horizontalResponse, const int32_t verticalResponse);
		};

	public:

		/**
		 * Detect approximately axis-aligned rectangles in images
		 * @param yFrame The grayscale image in which rectangles will be detected, optimal size if long edge of image is ~300 px long, must be valid
		 * @param rectangleWidth The expected width of the rectangle in the input image, range: [1, yframe.width())
		 * @param aspectRatio The aspect ratio (width : height) of the rectangles that will be detected, range: (0.01, 100)
		 * @param aspectRatioTolerance A scale factor to define a minimum and maximum acceptable aspect ratio (i.e. `1 - tolerance` and `1 + tolerance`), range: [0, 1)
		 * @param alignmentAngleTolerance The angle in radian specifying how much the orientation of a rectangle may deviate from the being aligned with the x-axis, range: [0, deg2rad(90)]
		 * @param sortRectangles If true, all detected rectangles will by sorted by their area in descending order, otherwise their order will be undefined
		 * @param lineImageBorderDistanceThreshold Optional parameter that defines a minimum distance from the image border in pixels; lines with endpoints below this threshold will be removed, range: [0, infinity)
		 * @param perpendicularSampleDistance Optional parameter which is propagated to optimizeRectangleAlongEdges(), see that func for more details, range: [1, infinity)
		 * @return A collection of detected rectangles
		 */
		static Rectangles detectAlignedRectangles(const Frame& yFrame, const unsigned int rectangleWidth, const Scalar aspectRatio, const Scalar aspectRatioTolerance = Scalar(0.10), const Scalar alignmentAngleTolerance = Numeric::deg2rad(35), const bool sortRectangles = true, const unsigned int lineImageBorderDistanceThreshold = 5u, const unsigned int perpendicularSampleDistance = 5u);

		/**
		 * Determines all possible L-shape elements which can be extracted from a set of finite lines.
		 * @param finiteLines A collection line segments which could form L-shapes, lines that are not fully inside the image boundaries will be ignored
		 * @param width The width of the image from which the line segments were extracted, range: [1, infinity)
		 * @param height The height of the image from which the line segments were extracted, range: [1, infinity)
		 * @param thresholdDistance Threshold specifying the maximum acceptable distance between two ends of a pair of line segments in order to still be counted a L-shape, range: [0, infinity)
		 * @param thresholdAngle Angle specifying the maximum acceptable deviation from perfect orthogonality of two ends of a pair of line segements in order to be counted as a L-shape, range: [0, deg2rad(45)]
		 * @return The L-shapes found in the collection of line segments
		 */
		static LShapes determineLShapes(const FiniteLines2& finiteLines, const unsigned int width, const unsigned int height, const Scalar thresholdDistance = Scalar(15), const Scalar thresholdAngle = Numeric::deg2rad(15));

		/**
		 * Determines all possible L-shapes, T-shapes, and X-shapes which can be extracted from two sets of finite lines.
		 * The lines in both sets are supposed to be perpendicular to each other
		 * @param horizontalFiniteLines The first set of finite lines, e.g., the horizontal finite lines, at least one
		 * @param verticalFiniteLines The second set of finite lines, e.g., the vertical finite lines, at least one
		 * @param width The width of the image from which the line segments were extracted, range: [1, infinity)
		 * @param height The height of the image from which the line segments were extracted, range: [1, infinity)
		 * @param lShapes The resulting L-shapes
		 * @param tShapes The resulting T-shapes
		 * @param xShapes The resulting X-shapes
		 * @param thresholdShortDistance The short distance threshold e.g., for L-shapes, or T-shapes, in pixel, with range [0, infinity)
		 * @param thresholdLongDistance The long distance threshold e.g., for X-shapes, in pixel, with range [0, infinity)
		 * @param thresholdShapeAngle The angle threshold for L-shapes and T-shapes, in radian, with range [0, PI/4)
		 * @param thresholdAngleXShape The angle threshold for X-shapes, in radian, with range [0, thresholdShapeAngle]
		 */
		static void determineShapes(const FiniteLines2& horizontalFiniteLines, const FiniteLines2& verticalFiniteLines, const unsigned int width, const unsigned int height, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const Scalar thresholdShortDistance = Scalar(2), const Scalar thresholdLongDistance = Scalar(5), const Scalar thresholdShapeAngle = Numeric::deg2rad(20), const Scalar thresholdAngleXShape = Numeric::deg2rad(5));

		/**
		 * Post-processes determined shapes and merges similar shapes.
		 * @param width The width of the detection area in pixel, with range [1, infinity)
		 * @param height The height of the detection area in pixel, with range [1, infinity)
		 * @param lShapes The L-shapes to be post-processed
		 * @param tShapes The T-shapes to be post-processed
		 * @param xShapes The X-shapes to be post-processed
		 * @param similarPointDistance The maximal distance between two shapes to count as similar, in pixel, with range [0, infinity)
		 * @param similarAngle The maximal angle between two shape directions to count as similar, in radian, with range [0, PI/2)
		 */
		static void postAdjustShapes(const unsigned int width, const unsigned int height, LShapes& lShapes, TShapes& tShapes, XShapes& xShapes, const Scalar similarPointDistance = Scalar(1.5), const Scalar similarAngle = Numeric::deg2rad(15));

		/**
		 * Filters L-shape objects based on their direction.
		 * @param lShapes A collection of L-shapes which will be filtered
		 * @param alignmentDirection The unit vector specifying the direction in which the L-shapes should point, must be valid and have unit length
		 * @param alignmentAngleThreshold The angle in radian specifying how much the direction of L-Shapes may deviate from the alignment direction, range: [0, deg2rad(90)]
		 * @return The subset of aligned L-shapes
		 */
		template <bool tAllowPerpendicularDirections>
		static LShapes filterLShapesBasedOnDirection(const LShapes& lShapes, const Vector2 alignmentDirection, const Scalar alignmentAngleThreshold = Numeric::deg2rad(15));

		/**
		 * Applies a non-maximum suppression of L-shapes.
		 * @param lShapes A collection of L-shapes which will be filtered
		 * @param width The width of the image from which the line segments were extracted, range: [1, infinity)
		 * @param height The height of the image from which the line segments were extracted, range: [1, infinity)
		 * @param thresholdDistance Maximum distance between two L-shapes in order to be evaluated during non-maximum suppression (otherwise they are said to be too far apart to be neighbors), range: [0, infinity)
		 * @param thresholdAngle Maximum difference between the directions of two L-shapes in order to be evaluated during non-maximum suppresion (otherwise they are said to be to different to be neighbors), range: [0, deg2rad(90)] (in radian!)
		 * @return The subset of non-maximum-suppressed L-shapes
		 */
		static LShapes nonMaximumSuppressionLShapes(const LShapes& lShapes, const unsigned int width, const unsigned int height, const Scalar thresholdDistance = Scalar(10), const Scalar thresholdAngle = Numeric::deg2rad(25));

		/**
		 * Determines rectangles aligned with a specified direction.
		 * @param finiteLines A collection line segments which could form rectangles, must be valid
		 * @param lShapes A collection of L-shapes extracted from the line segments, must be valid
		 * @param topLeftCornerDirection The direction of the L-shape that is located in the top-left corner of a rectangle, must have unit length
		 * @param minDistanceBetweenCorners Minimum distance in pixels between neighboring corners of detected rectangles, range: [0, infinity)
		 * @param thresholdCornerDirectionAngle Angle of maximum deviation from the top-left corner direction, range: [0, pi/4]
		 * @param thresholdConnectedShapesAngle Maximal angle between the directions of two connected L-shapes, range: [0, deg2rad(15)]
		 * @return A collection of detected rectangles
		 * @sa determineLShapes()
		 */
		static IndexedRectangles determineAlignedRectangles(const FiniteLines2& finiteLines, const LShapes& lShapes, const Vector2& topLeftCornerDirection, const Scalar minDistanceBetweenCorners = 10, const Scalar thresholdCornerDirectionAngle = Numeric::deg2rad(15), const Scalar thresholdConnectedShapesAngle = Numeric::deg2rad(5));

		/**
		 * Determines rectangles of specific size and with certain aspect ratios.
		 * @param finiteLines A collection line segments which could form rectangles, must be valid
		 * @param lShapes A collection of L-shapes extracted from the line segments, must be valid
		 * @param rectangles A collection of rectangles extracted from the L-shapes, must be valid
		 * @param aspectRatio The aspect ratio (width : height) of the rectangles that will be filtered out, range: (0.01, 100)
		 * @param minimalWidth The minimum width of rectangles that will be filtered out, in pixels, with range: [10, infinity)
		 * @param maximalWidth The maximum width of the rectangles that will be filtered out, range: [minimalWidth, infinity)
		 * @param aspectRatioTolerance Maximum deviation of the measured aspect ratios from the specified one (`aspectRatio +/- aspectRatioTolerance`), range: [0, 1)
		 * @param orthogonalAngleTolerance Angle defining the maximum deviation from perfectly perpendicular corners of the rectangles , range: [0, pi/2) (in radian!)
		 * @return A collection of rectangles of minimum size and specified aspect ratio
		 * @sa determineAlignedRectangles()
		 */
		static IndexedRectangles determineShapedRectangles(const FiniteLines2& finiteLines, const LShapes& lShapes, const IndexedRectangles& rectangles, const Scalar aspectRatio, const Scalar minimalWidth = 90, const Scalar maximalWidth = 250, const Scalar aspectRatioTolerance = Scalar(0.1), const Scalar orthogonalAngleTolerance = Numeric::deg2rad(10));

		/**
		 * Guesses rectangles of a specific aspect ratio from a set of upper left & right corners (provided as lShapes)
		 * Based on good upper edges, it generates candidates in two ways:
		 * - It will build rectangles with vertical edges orthogonal to the upper edge, and whose length is consistent with the aspect ratio of the keyboard.
		 * - It will build rectangles with vertical edges that follow the directions of the lshapes, and whose length is consistent with the aspect ratio of the keyboard.
		 * @param lShapes A collection of L-shapes extracted from the line segments, must be valid
		 * @param topLeftCornerDirection The direction of the L-shape that is located in the top-left corner of a rectangle, must have unit length
		 * @param aspectRatio The aspect ratio (width : height) of the rectangles that will be filtered out, range: (0.01, 10)
		 * @param imageHeight The height of the images used for detection (needed to check if the generated lower corners are out of bounds), range: (0, infinity)
		 * @param maxNumberOfCandidates The maximum number of candidates that we would like to generate (used to keep compute bounded). Range: [1, infinity)
		 * @param thresholdCornerDirectionAngle Angle of maximum deviation from the top-left corner direction, range: [0, pi/4]
		 * @param thresholdConnectedShapesAngle Maximal angle between the directions of two connected L-shapes, range: [0, deg2rad(15)]
		 * @param minimalRectangleWidth The minimum width of rectangles that will be filtered out, in pixels, with range: [10, infinity)
		 * @param maximalRectangleWidth The maximum width of the rectangles that will be filtered out, in pixels, with range: [minimalWidth, infinity)
		 * @param numCandidatePairsToGeneratePerEdge Once a good upper edge is found we will generate this number of candidate pairs. First candidates use the exact aspect ratio and the following ones build rectangles with increasingly long vertical edges. Range: [1, 10]
		 * @param sideEdgeRatioMultiplier To apply a perturbed aspect ratio of the keyboard model (wrt the nominal value) when generating the candidates, with range: [0.5, 2]
		 * @return A collection of rectangles of minimum size and specified aspect ratio
		 * @sa determineAlignedRectangles()
		 */
		static Rectangles guessShapedRectanglesFromUpperCorners(const LShapes& lShapes, const Vector2& topLeftCornerDirection, const Scalar aspectRatio, const unsigned int imageHeight, const unsigned int maxNumberOfCandidates, const Scalar thresholdCornerDirectionAngle = Numeric::deg2rad(15), const Scalar thresholdConnectedShapesAngle = Numeric::deg2rad(5), const Scalar minimalRectangleWidth = 90, const Scalar maximalRectangleWidth = 250, const unsigned int numCandidatePairsToGeneratePerEdge = 1u, const Scalar sideEdgeRatioMultiplier = 1.0);

		/**
		 * Checks whether two L-shapes are connected based on their orientation and edge alignment.
		 * @param lShapeA First L-shape, must be valid
		 * @param lShapeB Second L-Shape, must be valid
		 * @param directionA The edge of `lShapeA` that points into the direction of `lShapeB`
		 * @param directionB The edge of `lShapeB` that points into the direciton of `lShapeA`
		 * @param thresholdAngleCos The cosine of the angle that defines the maximum difference in the orientation of the direction vectors and the vector connecting the two L-shapes, range: [cos(0), cos(deg2rad(15))]
		 * @return True if the two L-shapes are connected, otherwise false
		 */
		static inline bool areLShapesConnected(const LShape& lShapeA, const LShape& lShapeB, const Vector2& directionA, const Vector2& directionB, const Scalar thresholdAngleCos = Numeric::cos(Numeric::deg2rad(5)));

		/**
		 * Removes finite lines with end points too close to the frame border.
		 * @param finiteLines A collection of line segments that will be filtered based on the distance of their end points to the border of an image, must be valid
		 * @param width The width of the image that these line segments were extracted from, range: (thresholdDistance, infinity)
		 * @param height The height of the image that these line segments were extracted from, range: (thresholdDistance, infinity)
		 * @param thresholdDistance Lines with end points close to the image border than this value will be discarded, in pixels, range: [0, infinity)
		 */
		static void removeLinesTooCloseToBorder(FiniteLines2& finiteLines, const unsigned int width, const unsigned int height, const Scalar thresholdDistance = Scalar(10));

		/**
		 * Optimize the location of a rectangle to its corresponding image edges
		 * Fits the line segments of a rectangle that was detected in an image to the corresponding image edges.
		 * @param yFrame A grayscale image in which the specified line will be fitted to its corresponding image edge, must be valid (1 channel, 8-bit)
		 * @param rectangle The rectangle that will be fitted to its corresponding image edges, must be valid
		 * @param perpendicularSampleDistance Defines the extent of the search direction which is perpendicular to the direction of the line, range: [1, infinity)
		 * @return True if the rectangle has been optimzed, otherwise false
		 */
		static bool optimizeRectangleAlongEdges(const Frame& yFrame, Rectangle& rectangle, const unsigned int perpendicularSampleDistance = 5u);

		/**
		 * Optimize the location of a line to that of a corresponding image edge
		 * For line segments extracted from an image, this function "snaps" the line segments to its corresponding image edges
		 * @param yFrame A grayscale image in which the specified line will be fitted to its corresponding image edge, must be valid (1 channel, 8-bit)
		 * @param line The line that will be fitted to its corresponding image edge, must be valid
		 * @param optimizedLine The resulting optimized line
		 * @param perpendicularSampleDistance Defines the extent of the search direction which is perpendicular to the direction of the line, range: [1, infinity)
		 * @param sampleLocations Number of equi-distant locations along the line on which the line position will be optimized, range: [2, infinity)
		 * @param minimalValidSampleLocations Minimum of required sample locations for an optimization to count as meaningful/successful,range: [2, infinity)
		 * @param sampleLocationsPercent Ignore this; only for development purposes
		 * @return True if the line has been optimzed, otherwise false
		 */
		static bool optimizeLineAlongEdge(const Frame& yFrame, const FiniteLine2& line, Line2& optimizedLine, const unsigned int perpendicularSampleDistance = 5u, const unsigned int sampleLocations = 30u, const unsigned int minimalValidSampleLocations = 5u, const Scalars& sampleLocationsPercent = Scalars());

		/**
		 * Returns true if the area of the first rectangle is larger than that of the second rectangle
		 * @param firstRectangle The first rectangle to used in the comparison
		 * @param secondRectangle The second rectangle to be used in the comparison
		 * @return True if the area of first rectangle is larger than the area of the second rectangle, otherwise false
		 */
		static inline bool hasGreaterArea(const Rectangle& firstRectangle, const Rectangle& secondRectangle);
};

inline ShapeDetector::TwoLineShape::TwoLineShape() :
	shapeType_(ST_INVALID),
	finiteLineIndex0_((unsigned int)(-1)),
	finiteLineIndex1_((unsigned int)(-1)),
	position_(0, 0)
{
	// nothing to do here
}

inline ShapeDetector::TwoLineShape::TwoLineShape(const ShapeType shapeType, const Vector2& position, const Scalar score) :
	shapeType_(shapeType),
	finiteLineIndex0_((unsigned int)(-1)),
	finiteLineIndex1_((unsigned int)(-1)),
	position_(position),
	score_(score)
{
	ocean_assert(shapeType_ != ST_INVALID);

	ocean_assert(score_ >= Scalar(0));
}

inline ShapeDetector::TwoLineShape::TwoLineShape(const ShapeType shapeType, const unsigned int finiteLineIndex0, const unsigned int finiteLineIndex1, const Vector2& position, const Scalar score) :
	shapeType_(shapeType),
	finiteLineIndex0_(finiteLineIndex0),
	finiteLineIndex1_(finiteLineIndex1),
	position_(position),
	score_(score)
{
	ocean_assert(shapeType_ != ST_INVALID);

	ocean_assert(score_ >= Scalar(0));
}

inline ShapeDetector::TwoLineShape::ShapeType ShapeDetector::TwoLineShape::type() const
{
	return shapeType_;
}

inline unsigned int ShapeDetector::TwoLineShape::finiteLineIndex0() const
{
	return finiteLineIndex0_;
}

inline unsigned int ShapeDetector::TwoLineShape::finiteLineIndex1() const
{
	return finiteLineIndex1_;
}

inline unsigned int ShapeDetector::TwoLineShape::finiteLineIndex(const unsigned int index) const
{
	ocean_assert(index <= 1u);
	return index == 0u ? finiteLineIndex0() : finiteLineIndex1();
}

inline const Vector2& ShapeDetector::TwoLineShape::position() const
{
	return position_;
}

inline void ShapeDetector::TwoLineShape::setPosition(const Vector2& position)
{
	position_ = position;
}

inline Scalar ShapeDetector::TwoLineShape::score() const
{
	return score_;
}

inline ShapeDetector::LShape::LShape() :
	TwoLineShape()
{
	// nothing to do here
}

inline ShapeDetector::LShape::LShape(const Vector2& position, const Vector2& direction, const Vector2& edgeLeft, const Vector2& edgeRight, const Scalar score) :
	TwoLineShape(ST_SHAPE_L, position, score),
	edgeLeft_(edgeLeft),
	edgeRight_(edgeRight),
	direction_(direction)
{
	ocean_assert(direction_.isUnit());
	ocean_assert(edgeLeft_.isUnit());
	ocean_assert(edgeRight_.isUnit());

	ocean_assert(edgeLeft_.cross(edgeRight_) >= 0);
	ocean_assert(edgeLeft_.cross(direction_) >= 0);
	ocean_assert(direction_.cross(edgeRight_) >= 0);
}

inline ShapeDetector::LShape::LShape(const unsigned int finiteLineIndex0, const unsigned int finiteLineIndex1, const Vector2& position, const Vector2& direction, const Vector2& edgeLeft, const Vector2& edgeRight, const Scalar score) :
	TwoLineShape(ST_SHAPE_L, finiteLineIndex0, finiteLineIndex1, position, score),
	edgeLeft_(edgeLeft),
	edgeRight_(edgeRight),
	direction_(direction)
{
	ocean_assert(direction_.isUnit());
	ocean_assert(edgeLeft_.isUnit());
	ocean_assert(edgeRight_.isUnit());

	ocean_assert(edgeLeft_.cross(edgeRight_) >= 0);
	ocean_assert(edgeLeft_.cross(direction_) >= 0);
	ocean_assert(direction_.cross(edgeRight_) >= 0);
}

inline const Vector2& ShapeDetector::LShape::edgeLeft() const
{
	return edgeLeft_;
}

inline const Vector2& ShapeDetector::LShape::edgeRight() const
{
	return edgeRight_;
}

inline const Vector2& ShapeDetector::LShape::direction() const
{
	return direction_;
}

inline ShapeDetector::TShape::TShape() :
	TwoLineShape()
{
	// nothing to do here
}

inline ShapeDetector::TShape::TShape(const Vector2& position, const Vector2& direction, const Scalar score) :
	TwoLineShape(ST_SHAPE_T, position, score),
	direction_(direction)
{
	ocean_assert(direction_.isUnit());
}

inline ShapeDetector::TShape::TShape(const unsigned int finiteLineIndex0, const unsigned int finiteLineIndex1, const Vector2& position, const Vector2& direction, const Scalar score) :
	TwoLineShape(ST_SHAPE_T, finiteLineIndex0, finiteLineIndex1, position, score),
	direction_(direction)
{
	ocean_assert(direction_.isUnit());
}

inline const Vector2& ShapeDetector::TShape::direction() const
{
	return direction_;
}

inline ShapeDetector::XShape::XShape() :
	TwoLineShape()
{
	// nothing to do here
}

inline ShapeDetector::XShape::XShape(const Vector2& position, const Vector2& direction0, const Vector2& direction1, const Scalar score) :
	TwoLineShape(ST_SHAPE_X, position, score),
	direction0_(direction0),
	direction1_(direction1)
{
	ocean_assert(direction0_.isUnit());
	ocean_assert(direction1_.isUnit());
	ocean_assert(Numeric::abs(direction0_ * direction1_) < Scalar(0.5));
}

inline ShapeDetector::XShape::XShape(const unsigned int finiteLineIndex0, const unsigned int finiteLineIndex1, const Vector2& position, const Vector2& direction0, const Vector2& direction1, const Scalar score) :
	TwoLineShape(ST_SHAPE_X, finiteLineIndex0, finiteLineIndex1, position, score),
	direction0_(direction0),
	direction1_(direction1)
{
	ocean_assert(direction0_.isUnit());
	ocean_assert(direction1_.isUnit());
	ocean_assert(Numeric::abs(direction0_ * direction1_) < Scalar(0.5));
}

inline const Vector2& ShapeDetector::XShape::direction0() const
{
	return direction0_;
}

inline const Vector2& ShapeDetector::XShape::direction1() const
{
	return direction1_;
}

inline unsigned int ShapeDetector::PatternDetectorGradientVarianceBased::determineHorizontalResponseWidth(const unsigned int width)
{
	ocean_assert(width >= shapeWidth_);
	return width - shapeWidth_ + 1u;
}

inline unsigned int ShapeDetector::PatternDetectorGradientVarianceBased::determineHorizontalResponseHeight(const unsigned int height)
{
	ocean_assert(height >= shapeBandSize_ * 2u + shapeStepSize_);
	return height - (shapeBandSize_ * 2u + shapeStepSize_) + 1u;
}

inline unsigned int ShapeDetector::PatternDetectorGradientVarianceBased::determineVerticalResponseWidth(const unsigned int width)
{
	ocean_assert(width >= shapeBandSize_ * 2u + shapeStepSize_);
	return width - (shapeBandSize_ * 2u + shapeStepSize_) + 1u;
}

inline unsigned int ShapeDetector::PatternDetectorGradientVarianceBased::determineVerticalResponseHeight(const unsigned int height)
{
	ocean_assert(height >= shapeHeight_ - shapeStepSize_);
	return height - (shapeHeight_ - shapeStepSize_) + 1u;
}

constexpr int ShapeDetector::PatternDetectorGradientVarianceBased::frameX_T_topDownHorizontalResponseX()
{
	return int(shapeWidth_2_);
}

constexpr int ShapeDetector::PatternDetectorGradientVarianceBased::frameY_T_topDownHorizontalResponseY()
{
	return int(shapeBandSize_ + shapeStepSize_2_);
}

constexpr int ShapeDetector::PatternDetectorGradientVarianceBased::frameX_T_topDownVerticalResponseX()
{
	return int(shapeBandSize_ + shapeStepSize_2_);
}

constexpr int ShapeDetector::PatternDetectorGradientVarianceBased::frameY_T_topDownVerticalResponseY()
{
	return -int(shapeStepSize_2_ + 1u);
}

constexpr int ShapeDetector::PatternDetectorGradientVarianceBased::frameX_T_topDownResponseX()
{
	// not allowed in constexpr: return std::max(frameX_T_topDownHorizontalResponseX(), frameX_T_topDownVerticalResponseX());

	return frameX_T_topDownHorizontalResponseX() > frameX_T_topDownVerticalResponseX() ? frameX_T_topDownHorizontalResponseX() : frameX_T_topDownVerticalResponseX();
}

constexpr int ShapeDetector::PatternDetectorGradientVarianceBased::frameY_T_topDownResponseY()
{
	// not allowed in constexpr: return std::max(frameY_T_topDownHorizontalResponseY(), frameY_T_topDownVerticalResponseY());

	return frameY_T_topDownHorizontalResponseY() > frameY_T_topDownVerticalResponseY() ? frameY_T_topDownHorizontalResponseY() : frameY_T_topDownVerticalResponseY();
}

constexpr int ShapeDetector::PatternDetectorGradientVarianceBased::frameX_T_bottomUpHorizontalResponseX()
{
	return int(shapeWidth_2_);
}

constexpr int ShapeDetector::PatternDetectorGradientVarianceBased::frameY_T_bottomUpHorizontalResponseY()
{
	return int(shapeBandSize_ + shapeStepSize_2_);
}

constexpr int ShapeDetector::PatternDetectorGradientVarianceBased::frameX_T_bottomUpVerticalResponseX()
{
	return int(shapeBandSize_ + shapeStepSize_2_);
}

constexpr int ShapeDetector::PatternDetectorGradientVarianceBased::frameY_T_bottomUpVerticalResponseY()
{
	return int(shapeHeight_ - shapeStepSize_2_ - 1u);
}

constexpr int ShapeDetector::PatternDetectorGradientVarianceBased::frameX_T_bottomUpResponseX()
{
	// not allowed in constexpr: return std::max(frameX_T_bottomUpHorizontalResponseX(), frameX_T_bottomUpVerticalResponseX());

	return frameX_T_bottomUpHorizontalResponseX() > frameX_T_bottomUpVerticalResponseX() ? frameX_T_bottomUpHorizontalResponseX() : frameX_T_bottomUpVerticalResponseX();
}

constexpr int ShapeDetector::PatternDetectorGradientVarianceBased::frameY_T_bottomUpResponseY()
{
	// not allowed in constexpr: return std::max(frameY_T_bottomUpHorizontalResponseY(), frameY_T_bottomUpVerticalResponseY());

	return frameY_T_bottomUpHorizontalResponseY() > frameY_T_bottomUpVerticalResponseY() ? frameY_T_bottomUpHorizontalResponseY() : frameY_T_bottomUpVerticalResponseY();
}

inline bool ShapeDetector::PatternDetectorGradientVarianceBased::haveCorrectSign(const int32_t sign, const float horizontalResponse, const float verticalResponse)
{
	if (sign < 0)
	{
		return horizontalResponse < 0.0f && verticalResponse < 0.0f;
	}
	else if (sign > 0)
	{
		return horizontalResponse > 0.0f && verticalResponse > 0.0f;
	}
	else
	{
		ocean_assert(sign == 0);
		return (horizontalResponse < 0.0f && verticalResponse < 0.0f) || (horizontalResponse > 0.0f && verticalResponse > 0.0f);
	}
}

inline bool ShapeDetector::PatternDetectorGradientVarianceBased::haveCorrectSign(const int32_t sign, const int32_t horizontalResponse, const int32_t verticalResponse)
{
	if (sign < 0)
	{
		return horizontalResponse < 0 && verticalResponse < 0;
	}
	else if (sign > 0)
	{
		return horizontalResponse > 0 && verticalResponse > 0;
	}
	else
	{
		ocean_assert(sign == 0);
		return (horizontalResponse < 0 && verticalResponse < 0) || (horizontalResponse > 0 && verticalResponse > 0);
	}
}

template <bool tAllowPerpendicularDirections>
ShapeDetector::LShapes ShapeDetector::filterLShapesBasedOnDirection(const LShapes& lShapes, const Vector2 alignmentDirection, const Scalar alignmentAngleThreshold)
{
	ocean_assert(alignmentDirection.isUnit());
	ocean_assert(alignmentAngleThreshold >= 0 && alignmentAngleThreshold <= Numeric::pi_2());

	const Vector2 perpendicularAlignmentDirection = alignmentDirection.perpendicular();
	ocean_assert(perpendicularAlignmentDirection.isUnit());

	const Scalar alginmentAngleThresholdCos = Numeric::cos(alignmentAngleThreshold);

	LShapes filteredLShapes;
	filteredLShapes.reserve(lShapes.size());

	for (const LShape& lShape : lShapes)
	{
		if (Numeric::abs(lShape.direction() * alignmentDirection) >= alginmentAngleThresholdCos
			|| (tAllowPerpendicularDirections && Numeric::abs(lShape.direction() * perpendicularAlignmentDirection) >= alginmentAngleThresholdCos))
		{
			filteredLShapes.push_back(lShape);
		}
	}

	return filteredLShapes;
}

inline bool ShapeDetector::areLShapesConnected(const LShape& lShapeA, const LShape& lShapeB, const Vector2& directionA, const Vector2& directionB, const Scalar thresholdAngleCos)
{
	ocean_assert(thresholdAngleCos >= Numeric::cos(Numeric::deg2rad(15)) && thresholdAngleCos <= Numeric::cos(Numeric::deg2rad(0)));
	ocean_assert(directionA.isUnit() && directionB.isUnit());

	const Vector2 direction = (lShapeB.position() - lShapeA.position()).normalized();

	if (directionA * direction < thresholdAngleCos)
	{
		// non of the two finite lines of the first L-shape has the same direction
		return false;
	}

	if (-(directionB * direction) < thresholdAngleCos)
	{
		// non of the two finite lines of the first L-shape has the same direction
		return false;
	}

	return true;
}

inline bool ShapeDetector::hasGreaterArea(const Rectangle& firstRectangle, const Rectangle& secondRectangle)
{
	ocean_assert(Triangle2(firstRectangle[0], firstRectangle[1], firstRectangle[2]).isValid() && Triangle2(firstRectangle[2], firstRectangle[3], firstRectangle[0]).isValid());
	ocean_assert(Triangle2(secondRectangle[0], secondRectangle[1], secondRectangle[2]).isValid() && Triangle2(secondRectangle[2], secondRectangle[3], secondRectangle[0]).isValid());

	const Scalar areaFirstRectangle = Triangle2(firstRectangle[0], firstRectangle[1], firstRectangle[2]).area2() + Triangle2(firstRectangle[2], firstRectangle[3], firstRectangle[0]).area2();
	const Scalar areaSecondRectangle = Triangle2(secondRectangle[0], secondRectangle[1], secondRectangle[2]).area2() + Triangle2(secondRectangle[2], secondRectangle[3], secondRectangle[0]).area2();

	return areaFirstRectangle > areaSecondRectangle;
}

} // namespace Detector

} // namespace CV

} // namespace Ocean

#endif // META_OCEAN_CV_DETECTOR_SHAPE_DETECTOR_H
