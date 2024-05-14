/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_UTILITIES_H
#define META_OCEAN_TRACKING_UTILITIES_H

#include "ocean/tracking/Tracking.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Maintenance.h"
#include "ocean/base/Thread.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/PixelBoundingBox.h"
#include "ocean/cv/Canvas.h"

#include "ocean/io/Bitstream.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Box3.h"
#include "ocean/math/Cone3.h"
#include "ocean/math/Cylinder3.h"
#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Triangle3.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"

#include "ocean/tracking/Database.h"

namespace Ocean
{

namespace Tracking
{

/**
 * This class implements utility functions allowing e.g., for a more comfortable visualization of tracking data.
 * @ingroup tracking
 */
class OCEAN_TRACKING_EXPORT Utilities
{
	public:

		/**
		 * Blends two given frames having the same frame type.
		 * The resulting frame holds pixel intensity values of fifty percent of each frame.
		 * @param frame0 The first frame that will be blended, must be valid
		 * @param frame1 The second frame that will be blended, with same frame type as the first frame, must be valid
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting blended frame
		 */
		static Frame blendFrames(const Frame& frame0, const Frame& frame1, Worker* worker = nullptr);

		/**
		 * Blends two given frames with same pixel origin.
		 * In case the image resolution varies, both images will be aligned at the image center and missing image content is added at the border.
		 * The resulting frame holds pixel intensity values of fifty percent of each frame.<br>
		 * @param frame0 The first frame that will be blended, must be valid
		 * @param frame1 The second frame that will be blended, with same frame type as the first frame, must be valid
		 * @param offset0 The location of the top-left corner of the first frame within the blended frame, with range [0, 0]x(infinity, infinity)
		 * @param offset1 The location of the top-left corner of the second frame within the blended frame, with range [0, 0]x(infinity, infinity)
		 * @param pixelFormat The pixel format the resulting blended image will have, FORMAT_UNDEFINED to use the pixel format of the given images (which must be identical in that case)
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting blended frame
		 */
		static Frame blendFrames(const Frame& frame0, const Frame& frame1, Vector2& offset0, Vector2& offset1, const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED, Worker* worker = nullptr);

		/**
		 * Paints a line into a given frame.
		 * @param frame The frame in which is drawn
		 * @param startPosition Start position of line
		 * @param stopPosition Stop position of line
		 * @param color The color that will be used to paint the points, ensure that one value is provided for each frame channel, otherwise black is used
		 * @param subPixel True, to paint the line with sub-pixel accuracy; False, to paint the line with pixel accuracy
		 */
		static inline void paintLine(Frame& frame, const Vector2& startPosition, const Vector2& stopPosition, const uint8_t* color = nullptr, const bool subPixel = true);

		/**
		 * Paints a set of lines into a given frame.
		 * @param frame The frame in which is drawn
		 * @param startPositions Start positions of the lines
		 * @param stopPositions Stop positions of the lines, each end position must have a corresponding start position
		 * @param numberLines The number of lines, with range [0, infinity)
		 * @param color The color that will be used to paint the points, ensure that one value is provided for each frame channel, nullptr to apply black
		 * @param worker Optional worker object to distribute the computation
		 * @param subPixel True, to paint the lines with sub-pixel accuracy; False, to paint the lines with pixel accuracy
		 * @param offsetStartPositions The offset which will be added to each start position before painting the line, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param offsetStopPositions The offset which will be added to each stop position before painting the line, with range (-infinity, infinity)x(-infinity, infinity)
		 */
		static inline void paintLines(Frame& frame, const Vector2* startPositions, const Vector2* stopPositions, const size_t numberLines, const uint8_t* color = nullptr, Worker* worker = nullptr, const bool subPixel = true, const Vector2& offsetStartPositions = Vector2(0, 0), const Vector2& offsetStopPositions = Vector2(0, 0));

		/**
		 * Paints a set of lines into a given frame with sub-pixel accuracy.
		 * @param frame The frame in which is drawn, must be valid
		 * @param startPositions Start positions of the lines
		 * @param stopPositions Stop positions of the lines, each end position must have a corresponding start position
		 * @param numberLines The number of lines, with range [0, infinity)
		 * @param color The color that will be used to paint the points, ensure that one value is provided for each frame channel, nullptr to apply black
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tSize The thickness of the lines in pixel, must be odd with range [1, infinity)
		 */
		template <unsigned int tSize>
		static inline void paintLines(Frame& frame, const Vector2* startPositions, const Vector2* stopPositions, const size_t numberLines, const uint8_t* color = nullptr, Worker* worker = nullptr);

		/**
		 * Paints a set of lines with (foreground and) background color into a given frame with sub-pixel accuracy.
		 * @param frame The frame in which is drawn, must be valid
		 * @param startPositions Start positions of the lines
		 * @param stopPositions Stop positions of the lines, each end position must have a corresponding start position
		 * @param numberLines The number of lines, with range [0, infinity)
		 * @param colorForeground Foreground color that will be used to paint the points, ensure that one value is provided for each frame channel, nullptr to use black
		 * @param colorBackground Background color that will be used to paint the points, ensure that one value is provided for each frame channel, nullptr to use black
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tSizeForeground The thickness of the foreground lines in pixel, must be odd with range [1, infinity)
		 * @tparam tSizeBackground The thickness of the background lines in pixel, must be odd with range (tSizeForeground, infinity)
		 */
		template <unsigned int tSizeForeground, unsigned int tSizeBackground>
		static inline void paintLines(Frame& frame, const Vector2* startPositions, const Vector2* stopPositions, const size_t numberLines, const uint8_t* colorForeground = nullptr, const uint8_t* colorBackground = nullptr, Worker* worker = nullptr);

		/**
		 * Paints several paths into a given frame with sub-pixel accuracy.
		 * A path is a chain of connected image points with arbitrary length (start and end point are not connected).
		 * @param frame The frame in which the paths will be painted
		 * @param paths The individual paths to be painted
		 * @param size The number of given paths, with range [0, infinity)
		 * @param color The color that will be used to paint the points, ensure that one value is provided for each frame channel, nullptr to apply black
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tSize The thickness of the paths in pixel, must be odd with range [1, infinity)
		 */
		template <unsigned int tSize>
		static inline void paintPaths(Frame& frame, const Vectors2* paths, const size_t size, const uint8_t* color = nullptr, Worker* worker = nullptr);

		/**
		 * Paints several paths into a given frame with sub-pixel accuracy.
		 * A path is a chain of connected image points with arbitrary length (start and end point are not connected).<br>
		 * The colors of the paths are determined by an interpolation between two separate color values.
		 * @param frame The frame in which the paths will be painted
		 * @param paths The individual paths to be painted
		 * @param size The number of given paths, with range [0, infinity)
		 * @param color0 The first color value, ensure that one value is provided for each frame channel
		 * @param color1 The second color value, ensure that one value is provided for each frame channel
		 * @param factors The interpolation factor, one factor for each path, with range [0, 1]
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tSize The thickness of the paths in pixel, must be odd with range [1, infinity)
		 */
		template <unsigned int tSize>
		static inline void paintPaths(Frame& frame, const Vectors2* paths, const size_t size, const uint8_t* color0, const uint8_t* color1, const Scalar* factors, Worker* worker = nullptr);

		/**
		 * Paints a 2D triangle into a given frame with sub-pixel accuracy.
		 * @param frame The frame receiving the triangles
		 * @param triangle The triangle that will be painted, must be valid
		 * @param color The color that will be used to paint the triangle edges, ensure that one value is provided for each frame channel, otherwise black is used
		 * @tparam tSize The thickness of the triangle edges in pixels, must be odd with range [1, infinity)
		 */
		template <unsigned int tSize = 1u>
		static void paintTriangle(Frame& frame, const Triangle2& triangle, const uint8_t* color = nullptr);

		/**
		 * Paints a set of 2D triangles into a given frame with sub-pixel accuracy.
		 * @param frame The frame receiving the triangles
		 * @param triangles The triangles that will be painted
		 * @param color The color that will be used to paint the triangle edges, ensure that one value is provided for each frame channel, otherwise black is used
		 * @param worker Optional worker to distribute the computation
		 * @tparam tSize The thickness of the triangle edges in pixels, must be odd with range [1, infinity)
		 */
		template <unsigned int tSize = 1u>
		static void paintTriangles(Frame& frame, const Triangles2& triangles, const uint8_t* color = nullptr, Worker* worker = nullptr);

		/**
		 * Paints a set of 2D image points into a given frame with sub-pixel accuracy.
		 * @param frame The frame in which the image points will be painted
		 * @param imagePoints The image points which will be painted
		 * @param size The number of given 2D points, with range [0, infinity)
		 * @param color The color for the image points
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tPointSize The radius of the image points, must be odd with range [1, infinity)
		 */
		template <unsigned int tPointSize>
		static inline void paintImagePoints(Frame& frame, const Vector2* imagePoints, const size_t size, const uint8_t* color, Worker* worker = nullptr);

		/**
		 * Paints a set of 3D object points (rather their projected 2D counterparts respectively) into a given frame with sub-pixel accuracy.
		 * @param frame The frame in which the projected object points will be painted
		 * @param anyCamera The camera profile defining the projection between 3D object points and camera plane
		 * @param world_T_camera The camera pose from which the 3D object points are observed with default viewing direction towards the negative z-space and y-axis as up vector, transforming camera to world, must be valid
		 * @param objectPoints The object points which will be projected into the camera frame, defined in world
		 * @param size The number of given 3D points, with range [0, infinity)
		 * @param color The color for the object points
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tPointSize The radius of the object points, must be odd with range [1, infinity)
		 */
		template <unsigned int tPointSize>
		static inline void paintObjectPoints(Frame& frame, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Vector3* objectPoints, const size_t size, const uint8_t* color, Worker* worker = nullptr);

		/**
		 * Paints a feature point having a radius (a scale) and orientation.
		 * @param frame The frame in which the projected object points will be painted, must be valid
		 * @param position The position of the feature point defined in the pixel domain of the given frame, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param radius The radius (scale) of the feature point in pixel, with range (0, infinity)
		 * @param orientation The orientation of the feature point as CCW angle in radian, with range [0, 2PI)
		 * @param color The color for the feature point, one value for each frame data channel
		 * @param shadowColor The outer color for the object points, one value for each frame data channel or nullptr to skip painting the shadow
		 */
		static void paintFeaturePoint(Frame& frame, const Vector2& position, const Scalar radius, const Scalar orientation, const uint8_t* color, const uint8_t* shadowColor);

		/**
		 * Paints a feature point having a radius (a scale) and orientation.
		 * @param frame The frame in which the feature points will be painted, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param position The position of the feature point defined in the pixel domain of the given frame, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param radius The radius (scale) of the feature point in pixel, with range (0, infinity)
		 * @param orientation The orientation of the feature point as CCW angle in radian, with range [0, 2PI)
		 * @param color The color for the feature point, one value for each frame data channel
		 * @param shadowColor The outer color for the object points, one value for each frame data channel or nullptr to skip painting the shadow
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void paintFeaturePoint8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2& position, const Scalar radius, const Scalar orientation, const uint8_t* color, const uint8_t* shadowColor, const unsigned int framePaddingElements = 0u);

		/**
		 * Paints feature points having a radius (a scale) and orientation.
		 * @param frame The frame in which the projected object points will be painted, must be valid
		 * @param positions The positions of the feature points defined in the pixel domain of the given frame, with range (-infinity, infinity)x(-infinity, infinity), can be nullptr if size is 0
		 * @param radii The radii (scale) of the feature points in pixel, with range (0, infinity), one for each position, can be nullptr if size is 0
		 * @param orientations The orientations of the feature points as CCW angle in radian, with range [0, 2PI), one for each position, can be nullptr if size is 0
		 * @param size The number of feature points to be painted, with range [0, infinity)
		 * @param color The color for the feature point, one value for each frame data channel
		 * @param shadowColor The outer color for the object points, one value for each frame data channel or nullptr to skip painting the shadow
		 * @param explicitOffset Optional explicit offset which will be added to every feature point location before the point is painted, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param worker Optional worker object to distribute the computation
		 */
		static void paintFeaturePoints(Frame& frame, const Vector2* positions, const Scalar* radii, const Scalar* orientations, const size_t size, const uint8_t* color, const uint8_t* shadowColor, const Vector2& explicitOffset = Vector2(0, 0), Worker* worker = nullptr);

		/**
		 * Paints feature points having a radius (a scale) and orientation.
		 * @param frame The frame in which the feature points will be painted, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param positions The positions of the feature points defined in the pixel domain of the given frame, with range (-infinity, infinity)x(-infinity, infinity), can be nullptr if size is 0
		 * @param radii The radii (scale) of the feature points in pixel, with range (0, infinity), one for each position, can be nullptr if size is 0
		 * @param orientations The orientations of the feature points as CCW angle in radian, with range [0, 2PI), one for each position, can be nullptr if size is 0
		 * @param size The number of feature points to be painted, with range [0, infinity)
		 * @param color The color for the feature point, one value for each frame data channel
		 * @param shadowColor The outer color for the object points, one value for each frame data channel or nullptr to skip painting the shadow
		 * @param explicitOffset Optional explicit offset which will be added to every feature point location before the point is painted, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void paintFeaturePoints8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2* positions, const Scalar* radii, const Scalar* orientations, const size_t size, const uint8_t* color, const uint8_t* shadowColor, const Vector2& explicitOffset, const unsigned int framePaddingElements, Worker* worker = nullptr);

		/**
		 * Paints a set of correspondences between 2D image points and 3D object points (rather their projected 2D counterparts respectively) into a given frame with sub-pixel accuracy.
		 * The projected object points will be painted first, followed by their corresponding image points.<br>
		 * Further, a connection between the projected object points and the corresponding image points can be painted, which can be helpful if the given pose is not accurate.
		 * @param frame The frame in which the projected object points and image points will be painted
		 * @param camera The camera profile defining the projection between 3D object points and camera plane
		 * @param model_T_camera The camera pose transforming the camera coordinate system to the coordinate system of the object points (with viewing direction along the negative z-axis and y-axis as up vector), must be valid
		 * @param objectPoints The object points which will be projected into the camera frame
		 * @param imagePoints The image points corresponding to the given object points, the smaller the distance between image points and projected object points the more accurate the camera pose
		 * @param correspondences The number of given 2D/3D point correspondences, with range [0, infinity)
		 * @param maxSqrError The maximal square pixel error between a projected object point and the corresponding image point to count as valid, with range [0, infinity)
		 * @param colorValidObjectPoints The color for valid object points
		 * @param colorValidImagePoints The color for valid object points
		 * @param colorInvalidObjectPoints The color for invalid object points
		 * @param colorInvalidImagePoints The color for invalid image points
		 * @param drawObjectPoints True, to draw the object points
		 * @param drawImagePoints True, to draw the image points
		 * @param drawConnections True, to draw the connections between projected object points and the corresponding image points
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tObjectPointSize The radius of the object points, must be odd with range [tImagePointSize, infinity)
		 * @tparam tImagePointSize The radius of the image points, must be odd, with range [1, infinity)
		 */
		template <unsigned int tObjectPointSize, unsigned int tImagePointSize>
		static inline void paintCorrespondences(Frame& frame, const AnyCamera& camera, const HomogenousMatrix4& model_T_camera, const Vector3* objectPoints, const Vector2* imagePoints, const size_t correspondences, const Scalar maxSqrError, const uint8_t* colorValidObjectPoints, const uint8_t* colorValidImagePoints, const uint8_t* colorInvalidObjectPoints, const uint8_t* colorInvalidImagePoints, const bool drawObjectPoints = true, const bool drawImagePoints = true, const bool drawConnections = true, Worker* worker = nullptr);

		/**
		 * Blends two corresponding frames each with a ratio of fifty percent and paints a set of given feature correspondences.
		 * Both frames must have the same pixel origin, while internally both images will be converted to FORMAT_RGB24 images.
		 * @param frame0 The first frame, must be valid
		 * @param frame1 The second frame, with same frame type as the first frame
		 * @param points0 The positions of the feature correspondences in the first frame
		 * @param points1 The positions of the feature correspondences in the second frame, each point as a corresponding point in the first frame
		 * @param numberPoints The number of given feature correspondences, with range [0, infinity)
		 * @param rgbColor The color that will be used to paint the points, ensure that three values are provided (with order: R, G, B), otherwise nullptr to use black
		 * @param worker Optional worker object to distribute the computation
		 * @return Returns the resulting blend frame showing the feature correspondences, with pixel format FORMAT_RGB24
		 */
		static Frame paintCorrespondencesBlend(const Frame& frame0, const Frame& frame1, const Vector2* points0, const Vector2* points1, const size_t numberPoints, const uint8_t* rgbColor = nullptr, Worker* worker = nullptr);

		/**
		 * Joins two corresponding frames horizontally and paints a set of given feature correspondences.
		 * Both frame can have individual frame dimensions (as long as the pixel origin is identical).<br>
		 * Internally both images will be converted to FORMAT_RGB24 images.
		 * @param frame0 The first frame, must be valid
		 * @param frame1 The second frame, with same pixel origin as the first frame, must be valid
		 * @param points0 The positions of the feature correspondences in the first frame
		 * @param points1 The positions of the feature correspondences in the second frame, each point as a corresponding point in the first frame
		 * @param numberPoints The number of given feature correspondences, with range [0, infinity)
		 * @param color The color that will be used to paint the points, ensure that three values are provided (with order R, G, B), otherwise nullptr to use black
		 * @param worker Optional worker object to distribute the computation
		 * @return Returns the resulting joined frame showing the feature correspondences, with pixel format FORMAT_RGB24
		 */
		static Frame paintCorrespondencesHorizontal(const Frame& frame0, const Frame& frame1, const Vector2* points0, const Vector2* points1, const size_t numberPoints, const uint8_t* color = nullptr, Worker* worker = nullptr);

		/**
		 * Joins two corresponding frames vertically and paints a set of given feature correspondences.
		 * Both frame can have individual frame dimensions, while internally both images will be converted to FORMAT_RGB24 images.
		 * @param frame0 The first frame, must be valid
		 * @param frame1 The second frame, with pixel origin as the first frame, must be valid
		 * @param points0 The positions of the feature correspondences in the first frame
		 * @param points1 The positions of the feature correspondences in the second frame, each point as a corresponding point in the first frame
		 * @param numberPoints The number of given feature correspondences, with range [0, infinity)
		 * @param color The color that will be used to paint the points, ensure that three values are provided (with order: R, G, B), otherwise nullptr to use black
		 * @param worker Optional worker object to distribute the computation
		 * @return Returns the resulting joined frame showing the feature correspondences, with pixel format FORMAT_RGB24
		 */
		static Frame paintCorrespondencesVertical(const Frame& frame0, const Frame& frame1, const Vector2* points0, const Vector2* points1, const size_t numberPoints, const uint8_t* color = nullptr, Worker* worker = nullptr);

		/**
		 * Joins two corresponding frames by application of a homography and paints a set of given feature correspondences.
		 * The second frame will be aligned so that it matches with the first frame (by application of the homography).<br>
		 * Both frame can have individual frame dimensions.
		 * @param frame0 The first frame, must be valid
		 * @param frame1 The second frame, with pixel origin as the first frame, must be valid
		 * @param points1_H_points0 The homography transforming points defined in the first frame to points defined in the second frame, must be valid
		 * @param points0 The positions of the feature correspondences in the first frame, with range [0, frame0.width())x[0, frame0.height())
		 * @param points1 The positions of the feature correspondences in the second frame, each point as a corresponding point in the first frame, with range [0, frame1.width())x[0, frame1.height())
		 * @param numberPoints The number of given feature correspondences, with range [0, infinity)
		 * @param fullCoverage True, to create a frame fully covering both frames; False, to create a frame covering the first frame only
		 * @param result Returns the resulting joined frame showing the feature correspondences
		 * @param foregroundColor The foreground color that will be used to paint the lines, ensure that one value is provided for each frame channel, nullptr to use black
		 * @param backgroundColor Optional background color that will used for each line, nullptr to avoid the usage of a background color
		 * @param startColor Optional color of the start points of each line, nullptr to avoid the usage of a color for the start point
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool paintCorrespondencesHomography(const Frame& frame0, const Frame& frame1, const SquareMatrix3& points1_H_points0, const Vector2* points0, const Vector2* points1, const size_t numberPoints, const bool fullCoverage, Frame& result, const uint8_t* foregroundColor = nullptr, const uint8_t* backgroundColor = nullptr, const uint8_t* startColor = nullptr, Worker* worker = nullptr);

		/**
		 * Joins two corresponding frames by application of their orientations and paints a set of given feature correspondences.
		 * @param pinholeCamera The pinhole camera profile defining the project, must be valid
		 * @param frame0 The first frame with frame dimension as the camera dimension, must be valid
		 * @param frame1 The second frame, with frame type as the first frame, must be valid
		 * @param orientation0 The orientation of the first frame, must be valid
		 * @param orientation1 The orientation of the second frame, must be valid
		 * @param points0 The positions of the feature correspondences in the first frame, with range [0, frame0.width())x[0, frame0.height())
		 * @param points1 The positions of the feature correspondences in the second frame, each point as a corresponding point in the first frame, with range [0, frame1.width())x[0, frame1.height())
		 * @param numberPoints The number of given feature correspondences, with range [0, infinity)
		 * @param result Returns the resulting joined frame showing the feature correspondences
		 * @param foregroundColor The foreground color that will be used to paint the lines, ensure that one value is provided for each frame channel, nullptr to use black
		 * @param backgroundColor Optional background color that will used for each line, nullptr to avoid the usage of a background color
		 * @param startColor Optional color of the start points of each line, nullptr to avoid the usage of a color for the start point
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool paintCorrespondencesOrientations(const PinholeCamera& pinholeCamera, const Frame& frame0, const Frame& frame1, const SquareMatrix3& orientation0, const SquareMatrix3& orientation1, const Vector2* points0, const Vector2* points1, const size_t numberPoints, Frame& result, const uint8_t* foregroundColor = nullptr, const uint8_t* backgroundColor = nullptr, const uint8_t* startColor = nullptr, Worker* worker = nullptr);

		/**
		 * Paints a set of image points into a given frame.
		 * @param frame The frame receiving the points
		 * @param imagePoints Image points that will be painted
		 * @param number The number of provided image points, with range [0, infinity)
		 * @param radius The radius of the paintings in pixel, with range [0, infinity)
		 * @param colorInner Inner color that will be used, ensure that one value is provided for each frame channel, otherwise white is used
		 * @param colorOuter Outer color that will be used, ensure that one value is provided for each frame channel, otherwise black is used
		 */
		static void paintPoints(Frame& frame, const Vector2* imagePoints, const size_t number, const unsigned int radius, const uint8_t* colorInner = nullptr, const uint8_t* colorOuter = nullptr);

		/**
		 * Paints (projected) object points and image points into a given frame.
		 * @param frame The frame receiving the points
		 * @param world_T_camera The transformations between camera and the world, must be valid
		 * @param pinholeCamera The pinhole camera profile defining the projection, with dimension equal to the frame dimension
		 * @param objectPoints The object points that will be painted, defined in world
		 * @param numberObjectPoints Number of provided object points, with range [0, infinity)
		 * @param imagePoints Image points that will be painted
		 * @param numberImagePoints Number of provided image points, with range [0, infinity)
		 * @param distortProjectedObjectPoints True, to apply the distortion parameters of the camera to the projected object points
		 * @param radiusObjectPoints Radius of the object point paintings in pixel
		 * @param radiusImagePoints Radius of the image point paintings in pixel
		 * @param colorObjectPoints Color that will be used to paint the object points, ensure that one value is provided for each frame channel, otherwise white is used
		 * @param colorImagePoints Color that will be used to paint the image points, ensure that one value is provided for each frame channel, otherwise white is used
		 */
		static inline void paintPoints(Frame& frame, const HomogenousMatrix4& world_T_camera, const PinholeCamera& pinholeCamera, const Vector3* objectPoints, const size_t numberObjectPoints, const Vector2* imagePoints, const size_t numberImagePoints, const bool distortProjectedObjectPoints, const unsigned int radiusObjectPoints, const unsigned int radiusImagePoints, const uint8_t* colorObjectPoints = nullptr, const uint8_t* colorImagePoints = nullptr);

		/**
		 * Paints (projected) object points and image points into a given frame.
		 * @param frame The frame receiving the points, must be valid
		 * @param flippedCamera_T_world The transformation between the world and the flipped camera, must be valid
		 * @param pinholeCamera The pinhole camera profile defining the projection, with dimension equal to the frame dimension
		 * @param objectPoints The object points that will be painted, defined in world
		 * @param numberObjectPoints Number of provided object points, with range [0, infinity)
		 * @param imagePoints Image points that will be painted
		 * @param numberImagePoints Number of provided image points, with range [0, infinity)
		 * @param distortProjectedObjectPoints True, to apply the distortion parameters of the camera to the projected object points
		 * @param radiusObjectPoints Radius of the object point paintings in pixel
		 * @param radiusImagePoints Radius of the image point paintings in pixel
		 * @param colorObjectPoints Color that will be used to paint the object points, ensure that one value is provided for each frame channel, otherwise white is used
		 * @param colorImagePoints Color that will be used to paint the image points, ensure that one value is provided for each frame channel, otherwise white is used
		 */
		static void paintPointsIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const PinholeCamera& pinholeCamera, const Vector3* objectPoints, const size_t numberObjectPoints, const Vector2* imagePoints, const size_t numberImagePoints, const bool distortProjectedObjectPoints, const unsigned int radiusObjectPoints, const unsigned int radiusImagePoints, const uint8_t* colorObjectPoints = nullptr, const uint8_t* colorImagePoints = nullptr);

		/**
		 * Paints a (projected) 3D axis aligned bounding box into a given frame.
		 * @param frame The frame receiving the points, must be valid
		 * @param flippedCamera_T_world The transformation between world and the flipped camera, must be valid
		 * @param anyCamera The camera profile defining the projection, with dimension equal to the frame dimension
		 * @param boundingBox The bounding box that will be painted, defined in world
		 * @param foregroundColor The foreground color of the plane, nullptr to skip the painting with the foreground color
		 * @param backgroundColor The background color of the plane, nullptr to skip the painting with the background color
		 * @param edgesOnly True, to paint the edges of the bounding box only; False, to paint also the diagonal connections
		 */
		static void paintBoundingBoxIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const Box3& boundingBox, const uint8_t* foregroundColor, const uint8_t* backgroundColor, const bool edgesOnly = true);

		/**
		 * Paints a (projected) wire-frame cone into a given frame.
		 * @param frame The frame being drawn to, must be valid
		 * @param flippedCamera_T_cone Inverted and flipped pose of the camera w.r.t the cone
		 * @param pinholeCamera The pinhole camera profile defining the projection, with dimension equal to the frame dimension
		 * @param cone The cone that will be painted
		 * @param distortProjectedObjectPoints True, to apply the distortion parameters of the camera to the projected object points
		 * @param worker Optional worker to distribute the computation
		 * @param color The color for the drawn lines
		 * @param numCircles Number of axis-slicing circles to draw along the cone's vertical span, with range [2, infinity)
		 * @param numVerticalLines Number axis-parallel lines to draw around the cone, with range [0, infinity)
		 * @param numSamples Number of lines to draw to approximate the projection of each circle, with range [3, infinity)
		 */
		static void paintWireframeConeIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_cone, const PinholeCamera& pinholeCamera, const Cone3& cone, const bool distortProjectedObjectPoints = true, Worker* worker = nullptr, const uint8_t* color = CV::Canvas::yellow(), const unsigned int numCircles = 6u, const unsigned int numVerticalLines = 4u, const unsigned int numSamples = 72u);

		/**
		 * Paints a (projected) wire-frame cylinder into a given frame.
		 * @param frame The frame being drawn to, must be valid
		 * @param flippedCamera_T_cylinder Inverted and flipped pose of the camera w.r.t the cylinder
		 * @param pinholeCamera The pinhole camera profile defining the projection, with dimension equal to the frame dimension
		 * @param cylinder The cylinder that will be painted
		 * @param distortProjectedObjectPoints True, to apply the distortion parameters of the camera to the projected object points
		 * @param worker Optional worker to distribute the computation
		 * @param color The color for the drawn lines
		 * @param numCircles Number of axis-slicing circles to draw along the cylinder's vertical span, with range [2, infinity)
		 * @param numVerticalLines Number axis-parallel lines to draw around the cylinder, with range [0, infinity)
		 * @param numSamples Number of lines to draw to approximate the projection of each circle, with range [3, infinity)
		 */
		static void paintWireframeCylinderIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_cylinder, const PinholeCamera& pinholeCamera, const Cylinder3& cylinder, const bool distortProjectedObjectPoints = true, Worker* worker = nullptr, const uint8_t* color = CV::Canvas::yellow(), const unsigned int numCircles = 6u, const unsigned int numVerticalLines = 4u, const unsigned int numSamples = 72u);

		/**
		 * Paints a (projected) 3D triangle into a given frame.
		 * @param frame The frame in which the triangle will be painted, must be valid
		 * @param flippedCamera_T_world The transformation transforming world to the flipped camera, the flipped camera points towards the positive z-space with y-axis down, must be valid
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param triangle The triangle that will be painted
		 * @param color The color to be used to paint the triangle edges, nullptr to use black
		 * @tparam tSize The thickness of the triangle edges in pixels, must be odd with range [1, infinity)
		 */
		template <unsigned int tSize = 1u>
		static inline void paintTriangleIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const Triangle3& triangle, const uint8_t* color = nullptr);

		/**
		 * Paints (projected) 3D triangles into a given frame.
		 * @param frame The frame in which the triangles will be painted, must be valid
		 * @param flippedCamera_T_world The transformation transforming world to the flipped camera, the flipped camera points towards the positive z-space with y-axis down, must be valid
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param triangles The 3D triangles that will be painted, defined in world, can be nullptr if 'numberTriangles == 0'
		 * @param numberTriangles Number of triangles that will be painted, with range [0, infinity)
		 * @param color The color to be used to paint the triangle edges, nullptr to use black
		 * @param worker Optional worker to distribute the computation
		 * @tparam tSize The thickness of the triangle edges in pixels, must be odd with range [1, infinity)
		 */
		template <unsigned int tSize = 1u>
		static void paintTrianglesIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const Triangle3* triangles, const size_t numberTriangles, const uint8_t* color = nullptr, Worker* worker = nullptr);

		/**
		 * Paints a (projected) 3D line into a given frame.
		 * @param frame The frame in which the triangle will be painted, must be valid
		 * @param flippedCamera_T_world The transformation transforming world to the flipped camera, the flipped camera points towards the positive z-space with y-axis down, must be valid
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param objectPoint0 The start 3D object point of the 3D line, defined in world
		 * @param objectPoint1 The end 3D object point of the 3D line, defined in world
		 * @param segments The number of segments in which the line will be separated, with range [1, infinity), the more segments the better the adjustment to the camera distortion (if any)
		 * @param foregroundColor The foreground color of the plane, nullptr to skip the painting with the foreground color
		 * @param backgroundColor The background color of the plane, nullptr to skip the painting with the background color
		 */
		static void paintLineIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const Vector3& objectPoint0, const Vector3& objectPoint1, const unsigned int segments, const uint8_t* foregroundColor, const uint8_t* backgroundColor);

		/**
		 * Paints a 3D coordinate system (projected) into a frame.
		 * If the frame in an RGB 24bit frame, than the axis are painted in red (x), green (y), and blue (z); otherwise axis with color zero are.<br>
		 * @param frame The frame in which the coordinate system is painted, must be valid
		 * @param flippedCamera_T_world The camera posed converting world to the flipped camera coordinate system (a camera coordinate system pointing towards the positive z-space), must be valid
		 * @param anyCamera The camera profile that is used to render the coordinate system
		 * @param world_T_coordinateSystem The transformation of the coordinate system which transformed points defined in the local coordinate system (which will be rendered) into points defined in the world coordinate system, must be valid
		 * @param length The length of the three axis of the coordinate system, defined in the units of the local coordinate system (coordinateSystem)
		 */
		static void paintCoordinateSystemIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_coordinateSystem, const Scalar length);

		/**
		 * Paints a 3D plane into the frame, further the origin of the plane is painted.
		 * This function determines a worthwhile expansion of the plane best matching with the scene.
		 * @param frame The frame in which the plane is painted
		 * @param world_T_camera The camera pose from which the 3D object points are observed with default viewing direction towards the negative z-space and y-axis as up vector, transforming camera to world, must be valid
		 * @param camera The camera profile defining the project, must be valid
		 * @param planeTransformation The transformation having the origin on the plane and the z-axis parallel to the plane's normal
		 * @param bins The number of bins in both direction that will be painted, with range [1, infinity)
		 * @param foregroundColor The foreground color of the plane, nullptr to skip the painting with the foreground color
		 * @param backgroundColor The background color of the plane, nullptr to skip the painting with the background color
		 * @param expansion Optional resulting plane's expansion in the scene in x-axis and y-axis defined in world coordinates, with range (0, infinity)
		 * @return True, if the plane is perpendicular to the given camera frame
		 */
		static bool paintPlane(Frame& frame, const HomogenousMatrix4& world_T_camera, const AnyCamera& camera, const HomogenousMatrix4& planeTransformation, const unsigned int bins, const uint8_t* foregroundColor, const uint8_t* backgroundColor, Scalar* expansion = nullptr);

		/**
		 * Paints a 3D plane into the frame, further the origin of the plane is painted.
		 * @param frame The frame in which the plane is painted
		 * @param flippedCamera_T_world The camera posed converting world to the flipped camera coordinate system (a camera coordinate system pointing towards the positive z-space), must be valid
		 * @param camera The camera profile defining the project, must be valid
		 * @param planeTransformation The transformation having the origin on the plane and the z-axis parallel to the plane's normal
		 * @param expansion The plane's expansion for the scene in x-axis and y-axis defined in world coordinates, with range (0, infinity)
		 * @param bins The number of bins in both direction that will be painted, with range [1, infinity)
		 * @param foregroundColor The foreground color of the plane, nullptr to skip the painting with the foreground color
		 * @param backgroundColor The background color of the plane, nullptr to skip the painting with the background color
		 * @return True, if the plane is perpendicular to the given camera frame
		 */
		static bool paintPlaneIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& camera, const HomogenousMatrix4& planeTransformation, const Scalar expansion, const unsigned int bins, const uint8_t* foregroundColor, const uint8_t* backgroundColor);

		/**
		 * Paints quads that are located on a 3D plane into a given frame.
		 * @param frame The frame receiving the points
		 * @param world_T_camera The camera pose from which the 3D object points are observed with default viewing direction towards the negative z-space and y-axis as up vector, transforming camera to world, must be valid
		 * @param camera The camera profile defining the projection, with dimension equal to the frame dimension
		 * @param quadOrigin Origin of the upper left quad position, defined in world
		 * @param quadHorizontal Vector starting at the quad origin and defining the horizontal direction of the quads, defined in world
		 * @param quadVertical Vector starting at the quad origin and defining the vertical direction of the quads, defined in world
		 * @param horizontalBins Number of horizontal bins that will be painted
		 * @param verticalBins Number of vertical bins that will be painted
		 * @param color The color that will be used to paint the points, ensure that one value is provided for each frame channel, otherwise black is used
		 */
		static inline void paintQuads(Frame& frame, const HomogenousMatrix4& world_T_camera, const AnyCamera& camera, const Vector3& quadOrigin, const Vector3& quadHorizontal, const Vector3& quadVertical, const unsigned int horizontalBins, const unsigned int verticalBins, const uint8_t* color = nullptr);

		/**
		 * Paints quads that are located on a 3D plane into a given frame.
		 * @see paintQuads().
		 */
		static void paintQuadsIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& camera, const Vector3& quadOrigin, const Vector3& quadHorizontal, const Vector3& quadVertical, const unsigned int horizontalBins, const unsigned int verticalBins, const uint8_t* color = nullptr);

		/**
		 * Paints / blends a binary 8 bit mask into a given frame with identical frame dimension.
		 * @param frame The frame in which the mask will be painted, must be valid
		 * @param mask The binary mask frame, with frame dimension matching with the given frame, must be valid
		 * @param maskValue The mask value defining pixels which will be painted as masked, may be 0x00 or 0xFF
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting frame with blended mask
		 */
		static Frame paintMask(const Frame& frame, const Frame& mask, const uint8_t maskValue = uint8_t(0xFFu), Worker* worker = nullptr);

		/**
		 * Paints / blends a bounding box into a given frame.
		 * @param frame The frame in which the bounding box will be painted, must be valid
		 * @param boundingBox The bounding box, must be valid and must be defined inside the frame
		 * @param worker Optional worker object to distribute the computation
		 * @return The resulting frame with blended mask
		 */
		static Frame paintBoundingBox(const Frame& frame, const CV::PixelBoundingBox& boundingBox, Worker* worker = nullptr);

		/**
		 * Aligns two frames connected by a given homography into one frame while the resulting frame covers the area of one given frame only (the fixed frame).
		 * @param fixedFrame The fixed frame which will be untouched while the dynamic frame will be transformed by application of the homography and aligned to this frame, must be valid
		 * @param dynamicFrame The dynamic frame which will be transformed and then aligned to the fixed frame, with same pixel format and pixel orientation as the fixed frame
		 * @param dynamic_H_fixed The homography transforming points defined in the fixed frame to points defined in the dynamic frame, must be valid
		 * @param result The resulting frame with aligned frames, the frame type will be adjusted/set internally
		 * @param blend True, to blend both frames; False, to overwrite each pixel for which a corresponding pixel in the dynamic frame exist
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool alignFramesHomography(const Frame& fixedFrame, const Frame& dynamicFrame, const SquareMatrix3& dynamic_H_fixed, Frame& result, const bool blend, Worker* worker = nullptr);

		/**
		 * Aligns two frames connected by a given homography into one frame entirely covering the frame content of both images.
		 * @param fixedFrame The fixed frame which will be untouched while the dynamic frame will be transformed by application of the homography and aligned to this frame, must be valid
		 * @param dynamicFrame The dynamic frame which will be transformed and then aligned to the fixed frame, with same pixel format and pixel orientation as the fixed frame
		 * @param dynamic_H_fixed The homography transforming points defined in the fixed frame to points defined in the dynamic frame, must be valid
		 * @param result The resulting frame with aligned frames converting the image content of both frames, the frame type will be adjusted/set internally
		 * @param blend True, to blend both frames; False, to overwrite each pixel for which a corresponding pixel in the dynamic frame exist
		 * @param worker Optional worker object to distribute the computation
		 * @param maximalWidth The maximal width of the resulting frame to ensure that an extreme homography does not create an extreme large result, in pixel with range [1, infinity)
		 * @param maximalHeight The maximal height of the resulting frame to ensure that an extreme homography does not create an extreme large result, in pixel with range [1, infinity)
		 * @param fixedFrameLeft Optional resulting horizontal location of the top left pixel of the fixed frame in the resulting aligned frame, with range [0, result.width() - fixedFrame.width()]
		 * @param fixedFrameTop Optional resulting vertical location of the top left pixel of the fixed frame in the resulting aligned frame, with range [0, result.height() - fixedFrame.height()]
		 * @param dynamicFrameLeft Optional resulting horizontal location of the top left pixel of the dynamic frame in relation to 'fixedFrameLeft', with range (-infinity, infinity)
		 * @param dynamicFrameTop Optional resulting horizontal location of the top left pixel of the dynamic frame in relation to 'fixedFrameTop', with range (-infinity, infinity)
		 * @param fullFixedFrame Optional resulting frame covering the fixed frame only, but with same frame dimension as the resulting aligned frame
		 * @param fullDynamicFrame Optional resulting frame covering the transformed dynamic frame only, but with same frame dimension as the resulting aligned frame
		 * @return True, if succeeded
		 */
		static bool alignFramesHomographyFullCoverage(const Frame& fixedFrame, const Frame& dynamicFrame, const SquareMatrix3& dynamic_H_fixed, Frame& result, const bool blend, Worker* worker = nullptr, unsigned int maximalWidth = 16384u, const unsigned int maximalHeight = 16384u, unsigned int* fixedFrameLeft = nullptr, unsigned int* fixedFrameTop = nullptr, Scalar* dynamicFrameLeft = nullptr, Scalar* dynamicFrameTop = nullptr, Frame* fullFixedFrame = nullptr, Frame* fullDynamicFrame = nullptr);

		/**
		 * Visualizes image point information of a tracking database for a specific pose id.
		 * @param database The database holding the tracking information to be visualized
		 * @param poseId The id of the pose for which the visualization will be created
		 * @param frame The frame receiving the visualization of the database
		 * @param colorImagePoints The color for feature points, ensure that one value is provided for each frame channel
		 * @param colorImagePointsInstable The color for instable paths, ensure that one value is provided for each frame channel
		 * @param colorImagePointsStable The color for stable paths, ensure that one value is provided for each frame channel
		 * @param maximalPathLength The maximal length (number of concurrent frames) of paths of feature points that will be visualized, with range [0, infinity)
		 * @param stablePathLength The length (number of concurrent frames) of paths so that they count as stable, with range [1, infinity)
		 * @param transformation The transformation matrix which will be applied to each feature point position before the position is visualized
		 * @param worker Optional worker to distribute the computation
		 */
		static void visualizeDatabase(const Database& database, const Index32 poseId, Frame& frame, const uint8_t* colorImagePoints, const uint8_t* colorImagePointsInstable, const uint8_t* colorImagePointsStable, const unsigned int maximalPathLength = 20u, const unsigned int stablePathLength = 100u, const SquareMatrix3& transformation = SquareMatrix3(true), Worker* worker = nullptr);

		/**
		 * Writes a camera profile to a binary output stream.
		 * @param pinholeCamera The pinhole camera profile to be written
		 * @param outputStream The output stream receiving the information
		 * @return True, if succeeded
		 * @see readCamera().
		 */
		static bool writeCamera(const PinholeCamera& pinholeCamera, IO::OutputBitstream& outputStream);

		/**
		 * Reads a camera profile from a binary input stream.
		 * @param inputStream The input stream providing the information
		 * @param pinholeCamera The resulting pinhole camera profile
		 * @see writeCamera().
		 */
		static bool readCamera(IO::InputBitstream& inputStream, PinholeCamera& pinholeCamera);

		/**
		 * Writes the information of a database to a given output stream as binary information.
		 * @param database The database to be written
		 * @param outputStream The output stream receiving the information
		 * @return True, if succeeded
		 * @see readDatabase(), writeCamera().
		 */
		static bool writeDatabase(const Database& database, IO::OutputBitstream& outputStream);

		/**
		 * Reads a database information from a binary input stream.
		 * @param inputStream The input stream providing the information
		 * @param database The database receiving the information, the given database will cleared before the information is assigned
		 * @return True, if succeeded
		 * @see writeDatabase(), readCamera().
		 */
		static bool readDatabase(IO::InputBitstream& inputStream, Database& database);

		/**
		 * Encodes the tracking environment composes of a frame mesh (a frame with correspondences of 2D image points and 3D object points), a camera pose from which the frame has been captured and an independent set of 3D object points.<br>
		 * @param frame The frame to encode
		 * @param frameImagePoints The image points located in the given frame
		 * @param frameObjectPoints The object points, one object point for each image point
		 * @param framePose The camera pose to encode
		 * @param objectPoints The independent object points to encode
		 * @return The encoded buffer
		 */
		static Maintenance::Buffer encodeEnvironment(const Frame& frame, const Vectors2& frameImagePoints, const Vectors3& frameObjectPoints, const HomogenousMatrix4& framePose, const Vectors3& objectPoints);

	protected:

		/**
		 * Paints a subset of a set of lines into a given frame.
		 * @param frame The frame in which is drawn
		 * @param startPositions Start positions of the lines
		 * @param stopPositions Stop positions of the lines, each end position must have a corresponding start position
		 * @param color The color that will be used to paint the lines, ensure that one value is provided for each frame channel, nullptr to use black
		 * @param subPixel True, to paint the lines with sub-pixel accuracy; False, to paint the lines with pixel accuracy
		 * @param offsetStartPositions The offset which will be added to each start position before painting the line, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param offsetStopPositions The offset which will be added to each stop position before painting the line, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param firstLine First line to be handled
		 * @param numberLines The number of lines to be handled
		 */
		static inline void paintLinesSubset(Frame* frame, const Vector2* startPositions, const Vector2* stopPositions, const uint8_t* color, const bool subPixel, const Vector2* offsetStartPositions, const Vector2* offsetStopPositions, const unsigned int firstLine, const unsigned int numberLines);

		/**
		 * Paints a subset of a set of lines into a given frame with sub-pixel accuracy.
		 * @param frame The frame in which is drawn, must be valid
		 * @param startPositions Start positions of the lines
		 * @param stopPositions Stop positions of the lines, each end position must have a corresponding start position
		 * @param color The color that will be used to paint the lines, ensure that one value is provided for each frame channel, nullptr to use black
		 * @param firstLine First line to be handled
		 * @param numberLines The number of lines to be handled
		 * @tparam tSize The thickness of the lines in pixel, must be odd with range [1, infinity)
		 */
		template <unsigned int tSize>
		static inline void paintLinesSubset(Frame* frame, const Vector2* startPositions, const Vector2* stopPositions, const uint8_t* color, const unsigned int firstLine, const unsigned int numberLines);

		/**
		 * Paints a subset of a set of lines into a given frame with sub-pixel accuracy.
		 * @param frame The frame in which is drawn, must be valid
		 * @param startPositions Start positions of the lines
		 * @param stopPositions Stop positions of the lines, each end position must have a corresponding start position
		 * @param colorForeground Foreground color that will be used to paint the points, ensure that one value is provided for each frame channel, nullptr to use black
		 * @param colorBackground Background color that will be used to paint the points, ensure that one value is provided for each frame channel, nullptr to use black
		 * @param firstLine First line to be handled
		 * @param numberLines The number of lines to be handled
		 * @tparam tSizeForeground The thickness of the foreground lines in pixel, must be odd with range [1, infinity)
		 * @tparam tSizeBackground The thickness of the background lines in pixel, must be odd with range (tSizeForeground, infinity)
		 */
		template <unsigned int tSizeForeground, unsigned int tSizeBackground>
		static inline void paintLinesSubset(Frame* frame, const Vector2* startPositions, const Vector2* stopPositions, const uint8_t* colorForeground, const uint8_t* colorBackground, const unsigned int firstLine, const unsigned int numberLines);

		/**
		 * Paints a subset of several paths into a given frame with sub-pixel accuracy.
		 * @param frame The frame in which the paths will be painted
		 * @param paths The individual paths to be painted
		 * @param color The color that will be used to paint the points, ensure that one value is provided for each frame channel, nullptr to apply black
		 * @param firstPath The first path to handled
		 * @param numberPaths The number of paths to handle
		 * @tparam tSize The thickness of the paths in pixel, must be odd with range [1, infinity)
		 * @see paintPaths().
		 */
		template <unsigned int tSize>
		static inline void paintPathsSubset(Frame* frame, const Vectors2* paths, const uint8_t* color, const unsigned int firstPath, const unsigned int numberPaths);

		/**
		 * Paints a subset of several paths into a given frame with sub-pixel accuracy.
		 * The colors of the paths are determined by an interpolation between two separate color values.
		 * @param frame The frame in which the paths will be painted
		 * @param paths The individual paths to be painted
		 * @param color0 The first color value, ensure that one value is provided for each frame channel
		 * @param color1 The second color value, ensure that one value is provided for each frame channel
		 * @param factors The interpolation factor, one factor for each path, with range [0, 1]
		 * @param firstPath The first path to handled
		 * @param numberPaths The number of paths to handle
		 * @tparam tSize The thickness of the paths in pixel, must be odd with range [1, infinity)
		 * @see paintPaths().
		 */
		template <unsigned int tSize>
		static inline void paintPathsAdvancedSubset(Frame* frame, const Vectors2* paths, const uint8_t* color0, const uint8_t* color1, const Scalar* factors, const unsigned int firstPath, const unsigned int numberPaths);

		/**
		 * Paints a subset of 2D triangles into a given frame with sub-pixel accuracy.
		 * @param frame The frame receiving the triangles
		 * @param triangles The triangles that will be painted
		 * @param color The color that will be used to paint the triangle points, ensure that one value is provided for each frame channel, otherwise black is used
		 * @param firstTriangle The first triangle to handled
		 * @param numberTriangles The number of triangle to handle
		 * @tparam tSize The thickness of the triangle edges in pixels, must be odd with range [1, infinity)
		 */
		template <unsigned int tSize = 1u>
		static inline void paintTrianglesSubset(Frame* frame, const Triangle2* triangles, const uint8_t* color, const unsigned int firstTriangle, const unsigned int numberTriangles);

		/**
		 * Projects and paints a subset of 3D triangles into a given frame with sub-pixel accuracy.
		 * @param frame The frame in which the triangle will be painted, must be valid
		 * @param flippedCamera_T_world The transformation transforming world to the flipped camera, the flipped camera points towards the positive z-space with y-axis down, must be valid
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param triangles The triangles that will be painted, defined in world, must be valid
		 * @param color The color to be used to paint the triangle edges, nullptr to use black
		 * @param firstTriangle The first triangle to handled
		 * @param numberTriangles The number of triangle to handle
		 * @tparam tSize The thickness of the triangle edges in pixels, must be odd with range [1, infinity)
		 */
		template <unsigned int tSize = 1u>
		static inline void paintTrianglesIFSubset(Frame* frame, const HomogenousMatrix4* flippedCamera_T_world, const AnyCamera* anyCamera, const Triangle3* triangles, const uint8_t* color, const unsigned int firstTriangle, const unsigned int numberTriangles);

		/**
		 * Paints subset of a set of 2D image points into a given frame with sub-pixel accuracy.
		 * @param frame The frame in which the image points will be painted
		 * @param imagePoints The image points which will be painted
		 * @param color The color for the image points
		 * @param firstImagePoint The first image point to handle
		 * @param numberImagePoints The number of image points to handle
		 * @tparam tPointSize The radius of the image points, must be odd with range [1, infinity)
		 */
		template <unsigned int tPointSize>
		static inline void paintImagePointsSubset(Frame* frame, const Vector2* imagePoints, const uint8_t* color, const unsigned int firstImagePoint, const unsigned int numberImagePoints);

		/**
		 * Paints a subset of set of 3D object points (rather their projected 2D counterparts respectively) into a given frame with sub-pixel accuracy.
		 * @param frame The frame in which the projected object points will be painted
		 * @param anyCamera The camera profile defining the projection between 3D object points and camera plane, must be valid
		 * @param flippedCamera_T_world The inverted and flipped camera pose from which the 3D object points are observed, with default camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param objectPoints The object points which will be projected into the camera frame
		 * @param color The color for the object points
		 * @param firstObjectPoint The first object point to handle
		 * @param numberObjectPoints The number of object points to handle
		 * @tparam tPointSize The radius of the object points, must be odd with range [1, infinity)
		 */
		template <unsigned int tPointSize>
		static inline void paintObjectPointsSubset(Frame* frame, const AnyCamera* anyCamera, const HomogenousMatrix4* flippedCamera_T_world, const Vector3* objectPoints, const uint8_t* color, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints);

		/**
		 * Paints a subset of feature points having a radius (a scale) and orientation.
		 * @param frame The frame in which the feature points will be painted, must be valid
		 * @param width The width of the frame in pixel, with range (0, infinity)
		 * @param height The height of the frame in pixel, with range (0, infinity)
		 * @param positions The positions of the feature points defined in the pixel domain of the given frame, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param radii The radii (scale) of the feature points in pixel, with range (0, infinity), one for each position
		 * @param orientations The orientations of the feature points as CCW angle in radian, with range [0, 2PI), one for each position
		 * @param color The color for the feature point, one value for each frame data channel
		 * @param shadowColor The outer color for the object points, one value for each frame data channel or nullptr to skip painting the shadow
		 * @param offsetX Explicit horizontal offset which will be added to every feature point location before the point is painted, with range (-infinity, infinity)
		 * @param offsetY Explicit vertical offset which will be added to every feature point location before the point is painted, with range (-infinity, infinity)
		 * @param framePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param firstFeaturePoint The first feature point to be handled, with range [0, 'size')
		 * @param numberFeaturePoints The number of feature points to be handled, with range [1, 'size']
		 * @tparam tChannels The number of data channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static inline void paintFeaturePoints8BitPerChannelSubset(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2* positions, const Scalar* radii, const Scalar* orientations, const uint8_t* color, const uint8_t* shadowColor, const Scalar offsetX, const Scalar offsetY, const unsigned int framePaddingElements, const unsigned int firstFeaturePoint, const unsigned int numberFeaturePoints);

		/**
		 * Paints a subset of a set of correspondences between 2D image points and 3D object points (rather their projected 2D counterparts respectively) into a given frame with sub-pixel accuracy.
		 * The projected object points will be painted first, followed by their corresponding image points.<br>
		 * Further, a connection between the projected object points and the corresponding image points can be painted, which can be helpful if the given pose is not accurate.
		 * @param frame The frame in which the projected object points and image points will be painted
		 * @param camera The camera profile defining the projection between 3D object points and camera plane
		 * @param flippedCamera_T_model The inverted and flipped camera pose transforming points located in coordinate system of the model to points located in the coordinate system of the flipped camera, must be valid
		 * @param objectPoints The object points which will be projected into the camera frame
		 * @param imagePoints The image points corresponding to the given object points, the smaller the distance between image points and projected object points the more accurate the camera pose
		 * @param maxSqrError The maximal square pixel error between a projected object point and the corresponding image point to count as valid, with range [0, infinity)
		 * @param colorValidObjectPoints The color for valid object points
		 * @param colorValidImagePoints The color for valid object points
		 * @param colorInvalidObjectPoints The color for invalid object points
		 * @param colorInvalidImagePoints The color for invalid image points
		 * @param drawObjectPoints True, to draw the object points
		 * @param drawImagePoints True, to draw the image points
		 * @param drawConnections True, to draw the connections between projected object points and the corresponding image points
		 * @param firstCorrespondence The first correspondence to handle
		 * @param numberCorrespondences The number of correspondences to handle
		 * @tparam tObjectPointSize The radius of the object points, must be odd with range [tImagePointSize, infinity)
		 * @tparam tImagePointSize The radius of the image points, must be odd, with range [1, infinity)
		 * @see paintCorrespondences().
		 */
		template <unsigned int tObjectPointSize, unsigned int tImagePointSize>
		static void paintCorrespondencesSubset(Frame* frame, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_model, const Vector3* objectPoints, const Vector2* imagePoints, const Scalar maxSqrError, const uint8_t* colorValidObjectPoints, const uint8_t* colorValidImagePoints, const uint8_t* colorInvalidObjectPoints, const uint8_t* colorInvalidImagePoints, const bool drawObjectPoints, const bool drawImagePoints, const bool drawConnections, const unsigned int firstCorrespondence, const unsigned int numberCorrespondences);

		/**
		 * Paints / blends a binary 8 bit mask pixel into a given pixel with 8 bit per channel.
		 * @param pixel The pixel data which will receive the mask
		 * @param blendFactor The blend factor for the given pixel
		 * @tparam tBlendChannel The index of the data channel which will be blended, with range [0, tChannels)
		 * @tparam tChannels The number of data channels of the given pixel
		 */
		template <unsigned int tBlendChannel, unsigned int tChannels>
		static inline void blendPixel(uint8_t* pixel, const uint8_t blendFactor);
};

inline void Utilities::paintLine(Frame& frame, const Vector2& startPosition, const Vector2& stopPosition, const uint8_t* color, const bool subPixel)
{
	if (subPixel)
	{
		CV::Canvas::line<1u>(frame, startPosition.x(), startPosition.y(), stopPosition.x(), stopPosition.y(), color);
	}
	else
	{
		CV::Canvas::line(frame, Numeric::round32(startPosition.x()), Numeric::round32(startPosition.y()), Numeric::round32(stopPosition.x()), Numeric::round32(stopPosition.y()), color);
	}
}

inline void Utilities::paintLines(Frame& frame, const Vector2* startPositions, const Vector2* stopPositions, const size_t numberLines, const uint8_t* color, Worker* worker, const bool subPixel, const Vector2& offsetStartPositions, const Vector2& offsetStopPositions)
{
	if (numberLines == 0)
	{
		return;
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintLinesSubset, &frame, startPositions, stopPositions, color, subPixel, &offsetStartPositions, &offsetStopPositions, 0u, 0u), 0u, (unsigned int)(numberLines));
	}
	else
	{
		paintLinesSubset(&frame, startPositions, stopPositions, color, subPixel, &offsetStartPositions, &offsetStopPositions, 0u, (unsigned int)(numberLines));
	}
}

template <unsigned int tSize>
inline void Utilities::paintLines(Frame& frame, const Vector2* startPositions, const Vector2* stopPositions, const size_t numberLines, const uint8_t* color, Worker* worker)
{
	if (numberLines == 0)
	{
		return;
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintLinesSubset<tSize>, &frame, startPositions, stopPositions, color, 0u, 0u), 0u, (unsigned int)numberLines);
	}
	else
	{
		paintLinesSubset<tSize>(&frame, startPositions, stopPositions, color, 0u, (unsigned int)numberLines);
	}
}

template <unsigned int tSizeForeground, unsigned int tSizeBackground>
inline void Utilities::paintLines(Frame& frame, const Vector2* startPositions, const Vector2* stopPositions, const size_t numberLines, const uint8_t* foregroundColor, const uint8_t* backgroundColor, Worker* worker)
{
	if (numberLines == 0)
	{
		return;
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintLinesSubset<tSizeForeground, tSizeBackground>, &frame, startPositions, stopPositions, foregroundColor, backgroundColor, 0u, 0u), 0u, (unsigned int)numberLines);
	}
	else
	{
		paintLinesSubset<tSizeForeground, tSizeBackground>(&frame, startPositions, stopPositions, foregroundColor, backgroundColor, 0u, (unsigned int)numberLines);
	}
}

template <unsigned int tSize>
inline void Utilities::paintPaths(Frame& frame, const Vectors2* paths, const size_t size, const uint8_t* color, Worker* worker)
{
	if (size == 0)
	{
		return;
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintPathsSubset<tSize>, &frame, paths, color, 0u, 0u), 0u, (unsigned int)size);
	}
	else
	{
		paintPathsSubset<tSize>(&frame, paths, color, 0u, (unsigned int)size);
	}
}

template <unsigned int tSize>
inline void Utilities::paintPaths(Frame& frame, const Vectors2* paths, const size_t size, const uint8_t* color0, const uint8_t* color1, const Scalar* factors, Worker* worker)
{
	if (size == 0)
	{
		return;
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintPathsAdvancedSubset<tSize>, &frame, paths, color0, color1, factors, 0u, 0u), 0u, (unsigned int)size);
	}
	else
	{
		paintPathsAdvancedSubset<tSize>(&frame, paths, color0, color1, factors, 0u, (unsigned int)size);
	}
}

template <unsigned int tSize>
void Utilities::paintTriangle(Frame& frame, const Triangle2& triangle, const uint8_t* color)
{
	paintTrianglesSubset<tSize>(&frame, &triangle, color, 0u, 1u);
}

template <unsigned int tSize>
void Utilities::paintTriangles(Frame& frame, const Triangles2& triangles, const uint8_t* color, Worker* worker)
{
	if (triangles.empty())
	{
		return;
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintTrianglesSubset<tSize>, &frame, triangles.data(), color, 0u, 0u), 0u, (unsigned int)triangles.size());
	}
	else
	{
		paintTrianglesSubset<tSize>(&frame, triangles.data(), color, 0u, (unsigned int)triangles.size());
	}
}

template <unsigned int tPointSize>
inline void Utilities::paintImagePoints(Frame& frame, const Vector2* imagePoints, const size_t size, const uint8_t* color, Worker* worker)
{
	static_assert(tPointSize % 2u == 1u, "Invalid point size!");

	if (size == 0)
	{
		return;
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintImagePointsSubset<tPointSize>, &frame, imagePoints, color, 0u, 0u), 0u, (unsigned int)size);
	}
	else
	{
		paintImagePointsSubset<tPointSize>(&frame, imagePoints, color, 0u, (unsigned int)size);
	}
}

template <unsigned int tPointSize>
inline void Utilities::paintObjectPoints(Frame& frame, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const Vector3* objectPoints, const size_t size, const uint8_t* color, Worker* worker)
{
	static_assert(tPointSize % 2u == 1u, "Invalid point size!");

	ocean_assert(frame.isValid() && anyCamera.isValid());
	ocean_assert(frame.width() == anyCamera.width() && frame.height() == anyCamera.height());

	if (size == 0)
	{
		return;
	}

	const HomogenousMatrix4 flippedCamera_T_world(AnyCamera::standard2InvertedFlipped(world_T_camera));

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintObjectPointsSubset<tPointSize>, &frame, &anyCamera, &flippedCamera_T_world, objectPoints, color, 0u, 0u), 0u, (unsigned int)(size));
	}
	else
	{
		paintObjectPointsSubset<tPointSize>(&frame, &anyCamera, &flippedCamera_T_world, objectPoints, color, 0u, (unsigned int)(size));
	}
}

template <unsigned int tChannels>
void Utilities::paintFeaturePoint8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2& position, const Scalar radius, const Scalar orientation, const uint8_t* color, const uint8_t* shadowColor, const unsigned int framePaddingElements)
{
	ocean_assert(frame && width != 0u && height != 0u && color);
	ocean_assert(radius >= Numeric::eps() && orientation >= 0 && orientation <= Numeric::pi2());

	const Quaternion rotation(Vector3(0, 0, 1), orientation);

	const Vector3 direction = rotation * Vector3(radius, 0, 0);
	const Vector3 leftTop = rotation * Vector3(-radius, -radius, 0);
	const Vector3 rightTop = rotation * Vector3(radius, -radius, 0);
	const Vector3 rightBottom = rotation * Vector3(radius, radius, 0);
	const Vector3 leftBottom = rotation * Vector3(-radius, radius, 0);

	if (shadowColor)
	{
		CV::Canvas::line8BitPerChannel<tChannels, 3u>(frame, width, height, position.x(), position.y(), position.x() + direction.x(), position.y() + direction.y(), shadowColor, framePaddingElements);

		CV::Canvas::line8BitPerChannel<tChannels, 3u>(frame, width, height, position.x() + leftTop.x(), position.y() + leftTop.y(), position.x() + leftBottom.x(), position.y() + leftBottom.y(), shadowColor, framePaddingElements);
		CV::Canvas::line8BitPerChannel<tChannels, 3u>(frame, width, height, position.x() + leftBottom.x(), position.y() + leftBottom.y(), position.x() + rightBottom.x(), position.y() + rightBottom.y(), shadowColor, framePaddingElements);
		CV::Canvas::line8BitPerChannel<tChannels, 3u>(frame, width, height, position.x() + rightBottom.x(), position.y() + rightBottom.y(), position.x() + rightTop.x(), position.y() + rightTop.y(), shadowColor, framePaddingElements);
		CV::Canvas::line8BitPerChannel<tChannels, 3u>(frame, width, height, position.x() + rightTop.x(), position.y() + rightTop.y(), position.x() + leftTop.x(), position.y() + leftTop.y(), shadowColor, framePaddingElements);
	}

	CV::Canvas::line8BitPerChannel<tChannels, 1u>(frame, width, height, position.x(), position.y(), position.x() + direction.x(), position.y() + direction.y(), color, framePaddingElements);

	CV::Canvas::line8BitPerChannel<tChannels, 1u>(frame, width, height, position.x() + leftTop.x(), position.y() + leftTop.y(), position.x() + leftBottom.x(), position.y() + leftBottom.y(), color, framePaddingElements);
	CV::Canvas::line8BitPerChannel<tChannels, 1u>(frame, width, height, position.x() + leftBottom.x(), position.y() + leftBottom.y(), position.x() + rightBottom.x(), position.y() + rightBottom.y(), color, framePaddingElements);
	CV::Canvas::line8BitPerChannel<tChannels, 1u>(frame, width, height, position.x() + rightBottom.x(), position.y() + rightBottom.y(), position.x() + rightTop.x(), position.y() + rightTop.y(), color, framePaddingElements);
	CV::Canvas::line8BitPerChannel<tChannels, 1u>(frame, width, height, position.x() + rightTop.x(), position.y() + rightTop.y(), position.x() + leftTop.x(), position.y() + leftTop.y(), color, framePaddingElements);
}

template <unsigned int tChannels>
inline void Utilities::paintFeaturePoints8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2* positions, const Scalar* radii, const Scalar* orientations, const size_t size, const uint8_t* color, const uint8_t* shadowColor, const Vector2& explicitOffset, const unsigned int framePaddingElements, Worker* worker)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame && width != 0u && height != 0u);

	if (size == 0)
	{
		return;
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintFeaturePoints8BitPerChannelSubset<tChannels>, frame, width, height, positions, radii, orientations, color, shadowColor, explicitOffset.x(), explicitOffset.y(), framePaddingElements, 0u, 0u), 0u, (unsigned int)(size), 11u, 12u, 20u);
	}
	else
	{
		paintFeaturePoints8BitPerChannelSubset<tChannels>(frame, width, height, positions, radii, orientations, color, shadowColor, explicitOffset.x(), explicitOffset.y(), framePaddingElements, 0u, (unsigned int)(size));
	}
}

template <unsigned int tObjectPointSize, unsigned int tImagePointSize>
inline void Utilities::paintCorrespondences(Frame& frame, const AnyCamera& camera, const HomogenousMatrix4& model_T_camera, const Vector3* objectPoints, const Vector2* imagePoints, const size_t correspondences, const Scalar maxSqrError, const uint8_t* colorValidObjectPoints, const uint8_t* colorValidImagePoints, const uint8_t* colorInvalidObjectPoints, const uint8_t* colorInvalidImagePoints, const bool drawObjectPoints, const bool drawImagePoints, const bool drawConnections, Worker* worker)
{
	static_assert(tObjectPointSize >= tImagePointSize, "Invalid point size!");
	static_assert(tObjectPointSize % 2u == 1u, "Invalid point size!");
	static_assert(tImagePointSize % 2u == 1u, "Invalid point size!");

	if (correspondences == 0)
	{
		return;
	}

	const HomogenousMatrix4 flippedCamera_T_model(AnyCamera::standard2InvertedFlipped(model_T_camera));

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintCorrespondencesSubset<tObjectPointSize, tImagePointSize>, &frame, &camera, &flippedCamera_T_model, objectPoints, imagePoints, maxSqrError, colorValidObjectPoints, colorValidImagePoints, colorInvalidObjectPoints, colorInvalidImagePoints, drawObjectPoints, drawImagePoints, drawConnections, 0u, 0u), 0u, (unsigned int)correspondences);
	}
	else
	{
		paintCorrespondencesSubset<tObjectPointSize, tImagePointSize>(&frame, &camera, &flippedCamera_T_model, objectPoints, imagePoints, maxSqrError, colorValidObjectPoints, colorValidImagePoints, colorInvalidObjectPoints, colorInvalidImagePoints, drawObjectPoints, drawImagePoints, drawConnections, 0u, (unsigned int)correspondences);
	}
}

inline void Utilities::paintPoints(Frame& frame, const HomogenousMatrix4& world_T_camera, const PinholeCamera& pinholeCamera, const Vector3* objectPoints, const size_t numberObjectPoints, const Vector2* imagePoints, const size_t numberImagePoints, const bool distortProjectedObjectPoints, const unsigned int radiusObjectPoints, const unsigned int radiusImagePoints, const uint8_t* colorObjectPoints, const uint8_t* colorImagePoints)
{
	paintPointsIF(frame, PinholeCamera::standard2InvertedFlipped(world_T_camera), pinholeCamera, objectPoints, numberObjectPoints, imagePoints, numberImagePoints, distortProjectedObjectPoints, radiusObjectPoints, radiusImagePoints, colorObjectPoints, colorImagePoints);
}

inline void Utilities::paintQuads(Frame& frame, const HomogenousMatrix4& world_T_camera, const AnyCamera& camera, const Vector3& quadOrigin, const Vector3& quadHorizontal, const Vector3& quadVertical, const unsigned int horizontalBins, const unsigned int verticalBins, const uint8_t* color)
{
	paintQuadsIF(frame, AnyCamera::standard2InvertedFlipped(world_T_camera), camera, quadOrigin, quadHorizontal, quadVertical, horizontalBins, verticalBins, color);
}

template <unsigned int tSize>
void inline Utilities::paintTriangleIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const Triangle3& triangle, const uint8_t* color)
{
	paintTrianglesIFSubset<tSize>(&frame, &flippedCamera_T_world, &anyCamera, &triangle, color, 0u, 1u);
}

template <unsigned int tSize>
void Utilities::paintTrianglesIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const Triangle3* triangles, const size_t numberTriangles, const uint8_t* color, Worker* worker)
{
	if (numberTriangles == 0)
	{
		return;
	}

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(&paintTrianglesIFSubset<tSize>, &frame, &flippedCamera_T_world, &anyCamera, triangles, color, 0u, 0u), 0u, (unsigned int)(numberTriangles));
	}
	else
	{
		paintTrianglesIFSubset<tSize>(&frame, &flippedCamera_T_world, &anyCamera, triangles, color, 0u, (unsigned int)(numberTriangles));
	}
}

inline void Utilities::paintLinesSubset(Frame* frame, const Vector2* startPositions, const Vector2* stopPositions, const uint8_t* color, const bool subPixel, const Vector2* offsetStartPositions, const Vector2* offsetStopPositions, const unsigned int firstLine, const unsigned int numberLines)
{
	ocean_assert(frame != nullptr);
	ocean_assert(startPositions != nullptr && stopPositions != nullptr);
	ocean_assert(offsetStartPositions != nullptr && offsetStopPositions != nullptr);

	for (unsigned int n = firstLine; n < firstLine + numberLines; ++n)
	{
		paintLine(*frame, startPositions[n] + *offsetStartPositions, stopPositions[n] + *offsetStopPositions, color, subPixel);
	}
}

template <unsigned int tSize>
inline void Utilities::paintLinesSubset(Frame* frame, const Vector2* startPositions, const Vector2* stopPositions, const uint8_t* color, const unsigned int firstLine, const unsigned int numberLines)
{
	ocean_assert(frame && startPositions && stopPositions);

	for (unsigned int n = firstLine; n < firstLine + numberLines; ++n)
	{
		CV::Canvas::line<tSize>(*frame, startPositions[n].x(), startPositions[n].y(), stopPositions[n].x(), stopPositions[n].y(), color);
	}
}

template <unsigned int tSizeForeground, unsigned int tSizeBackground>
inline void Utilities::paintLinesSubset(Frame* frame, const Vector2* startPositions, const Vector2* stopPositions, const uint8_t* colorForeground, const uint8_t* colorBackground, const unsigned int firstLine, const unsigned int numberLines)
{
	static_assert(tSizeForeground < tSizeBackground, "Invalid line size");
	ocean_assert(frame && startPositions && stopPositions);

	for (unsigned int n = firstLine; n < firstLine + numberLines; ++n)
	{
		CV::Canvas::line<tSizeBackground>(*frame, startPositions[n].x(), startPositions[n].y(), stopPositions[n].x(), stopPositions[n].y(), colorBackground);
		CV::Canvas::line<tSizeForeground>(*frame, startPositions[n].x(), startPositions[n].y(), stopPositions[n].x(), stopPositions[n].y(), colorForeground);
	}
}

template <unsigned int tSize>
inline void Utilities::paintPathsSubset(Frame* frame, const Vectors2* paths, const uint8_t* color, const unsigned int firstPath, const unsigned int numberPaths)
{
	static_assert(tSize % 2u == 1u, "Invalid size parameter.");

	ocean_assert(frame != nullptr);

	for (unsigned int n = firstPath; n < firstPath + numberPaths; ++n)
	{
		const Vectors2& path = paths[n];

		for (size_t i = 1; i < path.size(); ++i)
		{
			CV::Canvas::line<tSize>(*frame, path[i - 1], path[i], color);
		}
	}
}

template <unsigned int tSize>
inline void Utilities::paintPathsAdvancedSubset(Frame* frame, const Vectors2* paths, const uint8_t* color0, const uint8_t* color1, const Scalar* factors, const unsigned int firstPath, const unsigned int numberPaths)
{
	static_assert(tSize % 2u == 1u, "Invalid size parameter.");

	ocean_assert(frame != nullptr);

	std::vector<uint8_t> color(frame->channels());

	for (unsigned int n = firstPath; n < firstPath + numberPaths; ++n)
	{
		Numeric::isInsideRange(0, factors[n], 1);

		for (unsigned int c = 0u; c < frame->channels(); ++c)
		{
			color[c] = uint8_t(Scalar(color0[c]) * (1 - factors[n]) + Scalar(color1[c]) * factors[n]);
		}

		const Vectors2& path = paths[n];

		for (size_t i = 1; i < path.size(); ++i)
		{
			CV::Canvas::line<tSize>(*frame, path[i - 1], path[i], color.data());
		}
	}
}

template <unsigned int tSize>
inline void Utilities::paintTrianglesSubset(Frame* frame, const Triangle2* triangles, const uint8_t* color, const unsigned int firstTriangle, const unsigned int numberTriangles)
{
	static_assert(tSize % 2u == 1u, "Invalid line width!");

	ocean_assert(frame && triangles);

	for (unsigned int n = firstTriangle; n < firstTriangle + numberTriangles; ++n)
	{
		const Triangle2& triangle = triangles[n];
		ocean_assert(triangle.isValid());

		CV::Canvas::line<tSize>(*frame, triangle.point0().x(), triangle.point0().y(), triangle.point1().x(), triangle.point1().y(), color);
		CV::Canvas::line<tSize>(*frame, triangle.point1().x(), triangle.point1().y(), triangle.point2().x(), triangle.point2().y(), color);
		CV::Canvas::line<tSize>(*frame, triangle.point2().x(), triangle.point2().y(), triangle.point0().x(), triangle.point0().y(), color);
	}
}

template <unsigned int tSize>
inline void Utilities::paintTrianglesIFSubset(Frame* frame, const HomogenousMatrix4* flippedCamera_T_world, const AnyCamera* anyCamera, const Triangle3* triangles, const uint8_t* color, const unsigned int firstTriangle, const unsigned int numberTriangles)
{
	static_assert(tSize % 2u == 1u, "Invalid line width!");

	ocean_assert(frame != nullptr && flippedCamera_T_world != nullptr && anyCamera != nullptr && triangles != nullptr);

	for (unsigned int n = firstTriangle; n < firstTriangle + numberTriangles; ++n)
	{
		if (AnyCamera::isObjectPointInFrontIF(*flippedCamera_T_world, triangles[n].point0()) && AnyCamera::isObjectPointInFrontIF(*flippedCamera_T_world, triangles[n].point1()) && AnyCamera::isObjectPointInFrontIF(*flippedCamera_T_world, triangles[n].point2()))
		{
			const Vector2 imagePoint0 = anyCamera->projectToImageIF(*flippedCamera_T_world, triangles[n][0]);
			const Vector2 imagePoint1 = anyCamera->projectToImageIF(*flippedCamera_T_world, triangles[n][1]);
			const Vector2 imagePoint2 = anyCamera->projectToImageIF(*flippedCamera_T_world, triangles[n][2]);

			CV::Canvas::line<tSize>(*frame, imagePoint0.x(), imagePoint0.y(), imagePoint1.x(), imagePoint1.y(), color);
			CV::Canvas::line<tSize>(*frame, imagePoint1.x(), imagePoint1.y(), imagePoint2.x(), imagePoint2.y(), color);
			CV::Canvas::line<tSize>(*frame, imagePoint2.x(), imagePoint2.y(), imagePoint0.x(), imagePoint0.y(), color);
		}
	}
}

template <unsigned int tPointSize>
inline void Utilities::paintImagePointsSubset(Frame* frame, const Vector2* imagePoints, const uint8_t* color, const unsigned int firstImagePoint, const unsigned int numberImagePoints)
{
	static_assert(tPointSize % 2u == 1u, "Invalid point size!");

	ocean_assert(frame != nullptr && imagePoints != nullptr);

	for (unsigned int n = firstImagePoint; n < firstImagePoint + numberImagePoints; ++n)
	{
		CV::Canvas::point<tPointSize>(*frame, imagePoints[n], color);
	}
}

template <unsigned int tPointSize>
inline void Utilities::paintObjectPointsSubset(Frame* frame, const AnyCamera* anyCamera, const HomogenousMatrix4* flippedCamera_T_world, const Vector3* objectPoints, const uint8_t* color, const unsigned int firstObjectPoint, const unsigned int numberObjectPoints)
{
	static_assert(tPointSize % 2u == 1u, "Invalid point size!");

	ocean_assert(frame != nullptr && anyCamera != nullptr && flippedCamera_T_world != nullptr && objectPoints != nullptr);

	for (unsigned int n = firstObjectPoint; n < firstObjectPoint + numberObjectPoints; ++n)
	{
		if (AnyCamera::isObjectPointInFrontIF(*flippedCamera_T_world, objectPoints[n]))
		{
			const Vector2 projectedObjectPoint(anyCamera->projectToImageIF(*flippedCamera_T_world, objectPoints[n]));

			CV::Canvas::point<tPointSize>(*frame, projectedObjectPoint, color);
		}
	}
}

template <unsigned int tChannels>
inline void Utilities::paintFeaturePoints8BitPerChannelSubset(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2* positions, const Scalar* radii, const Scalar* orientations, const uint8_t* color, const uint8_t* shadowColor, const Scalar offsetX, const Scalar offsetY, const unsigned int framePaddingElements, const unsigned int firstFeaturePoint, const unsigned int numberFeaturePoints)
{
	ocean_assert(frame && width != 0u && height != 0u);
	ocean_assert(positions && radii && orientations && color);

	for (unsigned int n = firstFeaturePoint; n < firstFeaturePoint + numberFeaturePoints; ++n)
	{
		paintFeaturePoint8BitPerChannel<tChannels>(frame, width, height, positions[n] + Vector2(offsetX, offsetY), radii[n], orientations[n], color, shadowColor, framePaddingElements);
	}
}

template <unsigned int tObjectPointSize, unsigned int tImagePointSize>
void Utilities::paintCorrespondencesSubset(Frame* frame, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_model, const Vector3* objectPoints, const Vector2* imagePoints, const Scalar maxSqrError, const uint8_t* colorValidObjectPoints, const uint8_t* colorValidImagePoints, const uint8_t* colorInvalidObjectPoints, const uint8_t* colorInvalidImagePoints, const bool drawObjectPoints, const bool drawImagePoints, const bool drawConnections, const unsigned int firstCorrespondence, const unsigned int numberCorrespondences)
{
	static_assert(tObjectPointSize >= tImagePointSize, "Invalid point size!");
	static_assert(tObjectPointSize % 2u == 1u, "Invalid point size!");
	static_assert(tImagePointSize % 2u == 1u, "Invalid point size!");

	ocean_assert(frame != nullptr && camera != nullptr  && flippedCamera_T_model != nullptr  && objectPoints != nullptr  && imagePoints != nullptr );

	for (unsigned int n = firstCorrespondence; n < firstCorrespondence + numberCorrespondences; ++n)
	{
		const Vector2 projectedObjectPoint(camera->projectToImageIF(*flippedCamera_T_model, objectPoints[n]));

		const Scalar sqrDistance = imagePoints[n].sqrDistance(projectedObjectPoint);

		const uint8_t* colorObjectPoint = (sqrDistance <= maxSqrError) ? colorValidObjectPoints : colorInvalidObjectPoints;
		const uint8_t* colorImagePoint = (sqrDistance <= maxSqrError) ? colorValidImagePoints : colorInvalidImagePoints;

		if (drawObjectPoints)
		{
			CV::Canvas::point<tObjectPointSize>(*frame, projectedObjectPoint, colorObjectPoint);
		}

		if (drawConnections && sqrDistance > 2 * 2)
		{
			CV::Canvas::line<1u>(*frame, projectedObjectPoint, imagePoints[n], colorImagePoint);
		}

		if (drawImagePoints)
		{
			CV::Canvas::point<tImagePointSize>(*frame, imagePoints[n], colorImagePoint);
		}
	}
}

template <unsigned int tBlendChannel, unsigned int tChannels>
inline void Utilities::blendPixel(uint8_t* pixel, const uint8_t blendFactor)
{
	static_assert(tBlendChannel < tChannels, "Invalid blend channel!");

	ocean_assert(pixel);

	if (blendFactor == 0xFF)
	{
		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			if (n == tBlendChannel)
			{
				pixel[n] = 0xFF;
			}
			else
			{
				pixel[n] >>= 1;
			}
		}
	}
}

}

}

#endif // META_OCEAN_TRACKING_UTILITIES_H
