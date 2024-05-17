/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_TRACKING_OCULUSTAGS_UTILITIES_H
#define META_OCEAN_TRACKING_OCULUSTAGS_UTILITIES_H

#include "ocean/tracking/oculustags/OculusTags.h"
#include "ocean/tracking/oculustags/OculusTag.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"

#include "ocean/cv/advanced/AdvancedMotion.h"
#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/FiniteLine2.h"
#include "ocean/math/SquareMatrix2.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

#include "ocean/tracking/oculustags/QuadDetector.h"

#include <type_traits>

namespace Ocean
{

namespace Tracking
{

namespace OculusTags
{

/**
 * This class is a collection of Oculus Tag-related utility functions.
 * @ingroup trackingoculustags
 */
class OCEAN_TRACKING_OCULUSTAGS_EXPORT Utilities
{
	public:

		/**
		 * Draws a single boundary pattern (tag candidate)
		 * @param frame The frame into which the boundary patterns will be drawn, must be valid
		 * @param color Optional color that is used to draw the boundary patterns
		 * @param boundaryPattern The boundary pattern that will be drawn into the frame
		 */
		static void drawQuad(Frame& frame, const QuadDetector::Quad& boundaryPattern, const uint8_t* color = nullptr);

		/**
		 * Draws boundary patterns (tag candidates)
		 * @param frame The frame into which the boundary patterns will be drawn, must be valid
		 * @param boundaryPatterns The boundary patterns that will be drawn into the frame
		 */
		static void drawQuads(Frame& frame, const QuadDetector::Quads& boundaryPatterns);

		/**
		 * Draws a single Oculus Tag
		 * @param frame The frame into which the boundary patterns will be drawn, must be valid
		 * @param anyCamera The camera that corresponding to the input frame, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_camera The transformation that converts camera coordinates to device coordinates, `deviceCoord = device_T_camera * cameraCoord`, must be valid
		 * @param colorOutline Optional parameter to change the default color of the outline of the painted tag
		 * @param tag The tag that will be drawn
		 */
		static void drawOculusTag(Frame& frame, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const OculusTag& tag, const uint8_t* colorOutline = nullptr);

		/**
		 * Draws Oculus Tags (monocular detection)
		 * @param frame The frame into which the boundary patterns will be drawn, must be valid
		 * @param anyCamera The camera that was used to detect the tags, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_camera The transformation that converts camera coordinates to device coordinates, `deviceCoord = device_T_camera * cameraCoord`, must be valid
		 * @param tags The vector of tags that will be drawn
		 */
		static void drawOculusTags(Frame& frame, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_camera, const OculusTags& tags);

		/**
		 * Draws a single Oculus Tag into two frames
		 * @param frameA The first frame into which the tag will be drawn, must be valid
		 * @param frameB The second frame into which the tag will be drawn, must be valid
		 * @param anyCameraA The first camera that was used to detect the tags, must be valid
		 * @param anyCameraB The second camera that was used to detect the tags, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_cameraA The transformation from the first camera to the center of the device, must be valid
		 * @param device_T_cameraB The transformation from the second camera to the center of the device, must be valid
		 * @param tag The tag that will be drawn
		 */
		static void drawOculusTag(Frame& frameA, Frame& frameB, const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const OculusTag& tag);

		/**
		 * Draws Oculus Tags into two frames
		 * @param frameA The first frame into which the tag will be drawn, must be valid
		 * @param frameB The second frame into which the tag will be drawn, must be valid
		 * @param anyCameraA The first camera that was used to detect the tags, must be valid
		 * @param anyCameraB The second camera that was used to detect the tags, must be valid
		 * @param world_T_device The transformation that converts device points to world points, must be valid
		 * @param device_T_cameraA The transformation from the first camera to the center of the device, must be valid
		 * @param device_T_cameraB The transformation from the second camera to the center of the device, must be valid
		 * @param tags The vector of tags that will be drawn
		 */
		static void drawOculusTags(Frame& frameA, Frame& frameB, const AnyCamera& anyCameraA, const AnyCamera& anyCameraB, const HomogenousMatrix4& world_T_device, const HomogenousMatrix4& device_T_cameraA, const HomogenousMatrix4& device_T_cameraB, const OculusTags& tags);

		/**
		 * Draws the axes of a coordinate into an image
		 * By default the coordinate system is drawn into the origin but an arbitrary translation can be applied to it.
		 * (x - red, y - green, z - blue)
		 * @param frame The frame into which the boundary patterns will be drawn, must be valid
		 * @param anyCamera The camera that is used to project the 3D points of the coordinate system into the image, must be valid
		 * @param tag_T_camera The 6DOF pose defining the origin at which the coordinate system will be drawn, must be valid
		 * @param tagSize The size of the corresponding tag, range: (0, infinity)
		 * @param offset An optional translation that is applied to the coordinate system
		 */
		static void drawCoordinateSystem(Frame& frame, const AnyCamera& anyCamera, const HomogenousMatrix4& tag_T_camera, const Scalar tagSize, const Vector3& offset = Vector3(0, 0, 0));

		/**
		 * Creates a rectified image from a fisheye image
		 * @param anyCamera The fisheye camera that is defined for the fisheye frame, must be valid
		 * @param fisheyeFrame The fisheye frame of which a quadrilateral area will be rectified, must be valid
		 * @param world_T_camera The transformation from the camera to the world reference frame, must be valid
		 * @param world_T_tag The transformation from the tag to the world reference frame, must be valid
		 * @param tagSize The size of the tag in the world, range: (0, infinity)
		 * @param tagObjectCorners The four outer corners of the tag in the reference frame of the tag, size: 4
		 * @param rectifiedTagImageWidth The width of the rectified frame, range: [1, infinity)
		 * @param rectifiedTagImageHeight The height of the rectified frame, range: [1, infinity)
		 * @param rectifiedFrame The frame that will hold the generated rectified image, will be initialized internally
		 * @param extraBorder Optional number of pixels around the rectified area that will be added, range: [0, infinity)
		 * @param fisheye_OLT_rectified Optional output of the lookup table (from rectified to fisheye location)
		 * @param stopIfPartiallyOutside If true, the function will abort and return false if not all points of the quadrilateral area are inside the area of the input frame
		 * @return True if the rectification was successful, otherwise false
		 * @tparam tChannels Number of channels used by the input and output frames, range: [1, 4]
		 */
		template <uint32_t tChannels>
		static bool createRectifiedFrameFromFisheyeFrame(const AnyCamera& anyCamera, const Frame& fisheyeFrame, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_tag, const Scalar tagSize, const Vectors3& tagObjectCorners, const unsigned int rectifiedTagImageWidth, const unsigned int rectifiedTagImageHeight, Frame& rectifiedFrame, const unsigned int extraBorder = 0u, LookupCorner2<Vector2>* fisheye_OLT_rectified = nullptr, const bool stopIfPartiallyOutside = false);

#if defined(OCN_OCULUSTAG_DEBUGGING_ENABLED)

		/**
		 * Create a rectified image using a homography computed from the corners a boundary pattern
		 */
		static bool debugCreateRectifiedFrameFromHomograpy(const Frame& frame, const QuadDetector::Quad& boundaryPattern, const uint32_t rectifiedTagImageWidth, const uint32_t rectifiedTagImageHeight, Frame& rectifiedFrame, const uint32_t extraBorder = 0u, SquareMatrix3* rectifiedFrame_H_boundaryPattern = nullptr);

		/**
		 * Maps fisheye image points to rectified image coordinates
		 */
		static bool debugFisheyeImagePointsToRectifiedImagePoints(const AnyCamera& anyCamera, const HomogenousMatrix4& tag_T_camera, const Vectors3& tagObjectCorners, const Vectors2& rectifiedImageCorners, const Vectors2& fisheyeImagePoints, Vectors2& rectifiedImagePoints);

		/**
		 * Maps rectified image points to fisheye image coordinates
		 */
		static bool debugRectifiedImagePointsToFisheyeImagePoints(const AnyCamera& anyCamera, const HomogenousMatrix4& tag_T_camera, const Vectors3& tagObjectCorners, const Vectors2& rectifiedImageCorners, const Vectors2& rectifiedImagePoints, Vectors2& fisheyeImagePoints);

#endif // OCN_OCULUSTAG_DEBUGGING_ENABLED

		/**
		 * Draws the Oculus logo into an image
		 * @note Currently this logo must be fully comprised inside the image area, otherwise nothing will be drawn (return false)
		 * @param frame The frame into which the logo will be drawn, must be valid
		 * @param position The location of the top-left corner of the bounding box around the Oculus logo, must be inside the image
		 * @param width The width in pixels that the logo will have in the destination image, range: [0, frame.width() - position.x()]
		 * @param color The color in which the logo will be drawn, must be valid and have `frame.channels()` elements
		 * @return True if the logo has been successfully drawn into the image, otherwise false
		 */
		static bool drawOculusLogo(Frame& frame, const CV::PixelPosition& position, const uint32_t width, const uint8_t* color = nullptr);

		/**
		 * Generates and draws an Oculus tag (black-white or white-black only)
		 * @param tagID The tag ID that will be encoded and stored in the Oculus tag, range: [0, 1024)
		 * @param reflectanceType Determines the looks of the tag (must be normal or inverted)
		 * @param width Desired width of the image in pixels. The actual width may be larger in order to avoid interpolation artefacts, range: [numberOfModules, infinity)
		 * @param borderModuleMultiple A border (or quiet zone) that will drawn around the tag. This value is specified in multiples of the width of modules (bits), range: [0, infinity)
		 * @return The frame that contains an image of the generated. Will be empty on failure. TODOX Make this function return a boolean instead
		 */
		static Frame generateTagImage(const uint32_t& tagID, const OculusTag::ReflectanceType reflectanceType, const uint32_t width, const uint32_t borderModuleMultiple = 0u);

		/**
		 * Subpixel corner refinement (Foerstner algorithm)
		 * @param yFrame The frame that will be used for the refinement of the corner, must be a valid 8-bit, 1-channel image
		 * @param corner The initial location of the corner that will be refined, must be located inside the image such that `corner +/- (tNeighbor, tNeightbor)` is also inside the image (i.e. the search window around the corner must be fully inside the input frame)
		 * @param neighbors Number of pixel neighbors to consider; defines the size of the search window: `s = 2 * tNeighbors + 1`, range: [1, 5]
		 * @param direction Optional output of normalized direction the sum of the image gradients over the search window at the final corner location, only valid if this function returns true
		 * @param errors Optional output of the errors of the optimization
		 * @return True if the corner refinement was successful, otherwise false
		 * @tparam tUseKernel11 If true a `[-1 +1]` will be used, otherwise `[-1 0 +1]` kernel.
		 */
		template <bool tUseKernel11 = true>
		static bool refineCorner(const Frame& yFrame, Vector2& corner, const uint32_t neighbors, Vector2* direction = nullptr, Scalars* errors = nullptr);

		/**
		 * Refine location of a point that was mapped to another (similar) frame
		 * @param yFrameA The first input frame, must be valid and contiguous
		 * @param yFrameB The second input frame, must be valid and contiguous
		 * @param locationA The location of the image point in the first frame (and which will be refined in the second frame), must be valid
		 * @param predictedLocationB The predicted location of the image point in the first image when mapped to the second image, must be valid
		 * @param searchRadius The radius of the search area in the second image in which the refined location will be searched, range: (0, min(yframeB.width(), yFrameB.height()))
		 * @param locationB The refined location of the image point in the second frame
		 * @return The distance between the predicted and the matched location in the second frame
		 */
		static inline Scalar verifyStereoLocations(const Frame& yFrameA, const Frame& yFrameB, const Vector2& locationA, const Vector2& predictedLocationB, const unsigned int searchRadius, Vector2& locationB);

		/**
		 * Refine location of a point that was mapped to another (similar) frame
		 * @param yFrameA The first input frame, must be valid
		 * @param yFrameB The second input frame, must be valid
		 * @param locationsA The locations of image points in the first frame (which will be refined in the second frame), must be valid and non-empty
		 * @param predictedLocationsB The predicted locations of the image point in the first image when mapped to the second image, must have the same number of elements as locationA
		 * @param searchRadius The radius of the search area in the second image in which the refined location will be searched, range: (0, min(yframeB.width(), yFrameB.height()))
		 * @param maximalAverageError The maximum average distance between the predicted and the refined image location, range: (0, infinity)
		 * @param locationsB The refined location of the image point in the second frame
		 * @return True if the sum of all distances between predicted and refined image point locations are not larger than the threshold `maximalAverageError`, otherwise false
		 */
		static inline bool verifyStereoLocations(const Frame& yFrameA, const Frame& yFrameB, const Vectors2& locationsA, const Vectors2& predictedLocationsB, const unsigned int searchRadius, const Scalar maximalAverageError, Vector2* locationsB);

		/**
		* Encodes data (in order to store as a data matrix/draw the tag)
		* @param tagID The tag ID, range: [0, 1024)
		* @param dataMatrix The resulting data matrix which stores the encoded tag ID
		* @return True, if the data has been successfully encoded, otherwise false
		*/
		static bool encode(const uint32_t tagID, OculusTag::DataMatrix& dataMatrix);

		/**
		* Decodes the data matrix of a tag and returns the clear-text data
		* @param dataMatrix A data matrix, e.g., from a detector tag
		* @param tagID The (decoded) tagID that is stored in the input data matrix
		* @return True, if the data matrix has been successfully decoded, otherwise false
		*/
		static bool decode(const OculusTag::DataMatrix& dataMatrix, uint32_t& tagID);

		/**
		 * Serialize tags into a string
		 * @param tags The tags that will be serialized, must be non-empty and tags must be valid
		 * @param buffer The string that will be used as buffer to hold the serialized tags
		 * @return True if the tags were successfully serialized, otherwise false
		 */
		static bool serializeOculusTags(const OculusTags& tags, std::string& buffer);

		/**
		 * Deserialize tags from a string
		 * @param buffer The string holding the serialized tags, must be non-empty
		 * @param tags Will hold the deserialized tags
		 * @return True if the tags were successfully deserialized, otherwise false
		 */
		static bool deserializeOculusTags(const std::string& buffer, OculusTags& tags);

		/**
		 * Serialize tag sizes
		 * @param tagSizeMap A map storing the size of tags in meters for specific tag IDs, value range: (0, infinity)
		 * @param defaultTagSize The default tag size in meters that should be assumed if no specific size for a tag is defined, range: (0, infinity)
		 * @param buffer The string buffer that the serialized tag sizes will be written to
		 * @return True if the serialization was successful.
		 */
		static bool serializeTagSizeMap(const std::unordered_map<uint32_t, Scalar>& tagSizeMap, const Scalar defaultTagSize, std::string& buffer);

		/**
		 * Deserialize tag sizes from a string
		 * @param buffer The string holding the serialized tags, must be non-empty
		 * @param tagSizeMap The resulting map that will be storing the size of tags in meters for specific tag IDs, value range: (0, infinity)
		 * @param defaultTagSize The resulting default tag size in meters that should be assumed if no specific size for a tag is defined, range: (0, infinity)
		 * @return True if the deserialization was successful.
		 */
		static bool deserializeTagSizeMap(const std::string& buffer, std::unordered_map<uint32_t, Scalar>& tagSizeMap, Scalar& defaultTagSize);

	private:

		/**
		 * Generates and draws an Oculus tag (any colors)
		 * @param tagID The tag ID that will be encoded and stored in the Oculus tag, range: [0, 1024)
		 * @param bit1Color Color that is used for modules with binary value `1`, must be valid and have `tChannels` elements
		 * @param bit0Color Color that is used for modules with binary value `0`, must be valid and have `tChannels` elements
		 * @param width Desired width of the image in pixels. The actual width may be larger in order to avoid interpolation artifacts, range: [numberOfModules, infinity)
		 * @param borderModuleMultiple A border (or quiet zone) that will drawn around the tag. This value is specified in multiples of the width of modules (bits), range: [0, infinity)
		 * @return The frame that contains an image of the generated. Will be empty on failure. TODOX Make this function return a boolean instead
		 * @tparam tChannels Number of channels that the generated image of the Oculus Tag should have, range: [1, 4]
		 */
		template <uint8_t tChannels>
		static Frame generateTagImage(const uint32_t& tagID, const uint8_t* bit1Color, const uint8_t* bit0Color, const uint32_t width, const uint32_t borderModuleMultiple = 0u);
};

template <uint32_t tChannels>
bool Utilities::createRectifiedFrameFromFisheyeFrame(const AnyCamera& anyCamera, const Frame& fisheyeFrame, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_tag, const Scalar tagSize, const Vectors3& tagObjectCorners, const unsigned int rectifiedTagImageWidth, const unsigned int rectifiedTagImageHeight, Frame& rectifiedFrame, const unsigned int extraBorder, LookupCorner2<Vector2>* fisheye_OLT_rectified, const bool stopIfPartiallyOutside)
{
	static_assert(tChannels != 0u && tChannels <= 4u, "Number of channels must be in the range: [1, 4]");
	ocean_assert(anyCamera.isValid() && fisheyeFrame.isValid());
	ocean_assert(anyCamera.width() == fisheyeFrame.width() && anyCamera.height() == fisheyeFrame.height());
	ocean_assert(world_T_camera.isValid() && world_T_tag.isValid());
	ocean_assert(tagObjectCorners.size() == 4);
	ocean_assert(rectifiedTagImageWidth >= 1u);
	ocean_assert(rectifiedTagImageHeight >= 1u);

	if (!fisheyeFrame.isValid()
		|| !world_T_camera.isValid() || !world_T_tag.isValid()
		|| tagObjectCorners.size() != 4
		|| rectifiedTagImageWidth == 0u || rectifiedTagImageHeight == 0u
		|| tChannels != fisheyeFrame.channels())
	{
		ocean_assert(false && "This shouldn't happen");
		return false;
	}

	const unsigned int rectifiedFrameWidth = rectifiedTagImageWidth + extraBorder * 2u;
	const unsigned int rectifiedFrameHeight = rectifiedTagImageHeight + extraBorder * 2u;

	const Scalar extendedTagFactorX = Scalar(rectifiedFrameWidth) / Scalar(rectifiedTagImageWidth);
	const Scalar extendedTagFactorY = Scalar(rectifiedFrameHeight) / Scalar(rectifiedTagImageHeight);

	// Scale and re-center the tag
	const Scalar tx = Scalar(0.5) * (extendedTagFactorX - Scalar(1)) * tagSize;
	const Scalar ty = Scalar(0.5) * (extendedTagFactorY - Scalar(1)) * tagSize;

	Vector3 tagObjectCornersExtended[4];
	for (unsigned int n = 0u; n < 4u; ++n)
	{
		tagObjectCornersExtended[n] = Vector3(tagObjectCorners[n].x() * extendedTagFactorX - tx, tagObjectCorners[n].y() * extendedTagFactorY + ty, tagObjectCorners[n].z());
	}

	const HomogenousMatrix4 camera_T_tag = world_T_camera.inverted() * world_T_tag;

	const size_t binsX = std::max(1u, rectifiedFrameWidth / 4u);
	const size_t binsY = std::max(1u, rectifiedFrameHeight / 4u);
	CV::FrameInterpolatorBilinear::LookupTable lookupTable(rectifiedFrameWidth, rectifiedFrameHeight, binsX, binsY);

	for (size_t yBin = 0; yBin <= lookupTable.binsY(); ++yBin)
	{
		const Scalar yBottomFactor = Scalar(yBin) / Scalar(lookupTable.binsY());
		ocean_assert(yBottomFactor >= 0 && yBottomFactor <= 1);

		const Scalar yTopFactor = Scalar(1) - yBottomFactor;
		ocean_assert(yTopFactor >= 0 && yTopFactor <= 1);

		for (size_t xBin = 0; xBin <= lookupTable.binsX(); ++xBin)
		{
			const Vector2 cornerPosition = lookupTable.binTopLeftCornerPosition(xBin, yBin);

			const Scalar xRightFactor = Scalar(xBin) / Scalar(lookupTable.binsX());
			ocean_assert(xRightFactor >= 0 && xRightFactor <= 1);

			const Scalar xLeftFactor = Scalar(1) - xRightFactor;
			ocean_assert(xLeftFactor >= 0 && xLeftFactor <= 1);

			const Scalar topLeftFactor = yTopFactor * xLeftFactor;
			const Scalar bottomLeftFactor = yBottomFactor * xLeftFactor;
			const Scalar bottomRightFactor = yBottomFactor * xRightFactor;
			const Scalar topRightFactor = yTopFactor * xRightFactor;

			ocean_assert(Numeric::isEqual(topLeftFactor + bottomLeftFactor + bottomRightFactor + topRightFactor, 1));

			const Vector3 tagPoint = tagObjectCornersExtended[0] * topLeftFactor + tagObjectCornersExtended[1] * bottomLeftFactor + tagObjectCornersExtended[2] * bottomRightFactor + tagObjectCornersExtended[3] * topRightFactor;

			const Vector3 cameraPoint = camera_T_tag * tagPoint;
			const Vector3 cameraPointF = Vector3(cameraPoint.x(), -cameraPoint.y(), -cameraPoint.z());

			if (cameraPointF.z() > Numeric::eps())
			{
				const Vector2 projectedPoint = anyCamera.projectToImageIF(cameraPointF);

				if (stopIfPartiallyOutside && !anyCamera.isInside(projectedPoint))
				{
					return false;
				}

				lookupTable.setBinTopLeftCornerValue(xBin, yBin, projectedPoint - cornerPosition);
			}
			else
			{
				if (stopIfPartiallyOutside)
				{
					return false;
				}

				lookupTable.setBinTopLeftCornerValue(xBin, yBin, Vector2(Scalar(fisheyeFrame.width() * 10u), Scalar(fisheyeFrame.height() * 10u)));
			}
		}
	}

	if (!rectifiedFrame.set(FrameType(fisheyeFrame, rectifiedFrameWidth, rectifiedFrameHeight), false /*forceOwner*/, true /*forceWritable*/))
	{
		return false;
	}

	CV::FrameInterpolatorBilinear::lookup<uint8_t, (unsigned int)tChannels>(fisheyeFrame.constdata<uint8_t>(), fisheyeFrame.width(), fisheyeFrame.height(), lookupTable, true, nullptr, rectifiedFrame.data<uint8_t>(), fisheyeFrame.paddingElements(), rectifiedFrame.paddingElements(),  nullptr /*no worker*/);

	if (fisheye_OLT_rectified)
	{
		*fisheye_OLT_rectified = std::move(lookupTable);
	}

	return true;
}

template <bool tUseKernel11>
bool Utilities::refineCorner(const Frame& yFrame, Vector2& corner, const uint32_t neighbors, Vector2* direction, Scalars* errors)
{
	ocean_assert(yFrame.isValid() && FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::genericPixelFormat<FrameType::DT_UNSIGNED_INTEGER_8, 1u>()));
	ocean_assert(neighbors != 0u);

	// Search window + image patch
	//
	// Example: neighbors = 2
	//
	// kernel: [-1 +1]
	//              ^-- kernel center
	//
	// ```
	// B B B B B
	// B W W W W
	// B W X W W
	// B W W C W
	// B W W W W
	// ```
	//
	// kernel: [-1 0 +1]
	//             ^-- kernel center
	//
	// ```
	// B B B B B B B
	// B W W W W W B
	// B W W W W W B
	// B W W C W W B
	// B W W W W W B
	// B W W W W W B
	// B B B B B B B
	// ```
	//
	// `B` - extra pixels required to compute the gradients inside the window
	// `W` - pixels of the actual search window
	// `C` - center pixel of the search window (or the location of the corner)
	// `X` - sample location to extract the buffer/image patch (only for kernel [-1 +1], shifted because the search window is not symmetric)

	uint32_t windowSize = 0u;
	uint32_t bufferSize = 0u;

	if constexpr (tUseKernel11)
	{
		windowSize = 2u * neighbors;
		bufferSize = windowSize + 1u;
		ocean_assert(windowSize != 0 && windowSize % 2u == 0u && bufferSize % 2u == 1u);
	}
	else
	{
		windowSize = 2u * neighbors + 1u;
		bufferSize = windowSize + 2u;
		ocean_assert(windowSize != 0 && windowSize % 2u == 1u && bufferSize % 2u == 1u);
	}

	ocean_assert(bufferSize % 2u == 1u);
	Memory buffer(bufferSize * bufferSize);

	if (yFrame.width() < bufferSize || yFrame.height() < bufferSize)
	{
		return false;
	}

	// Normal distributed weights for each element inside the search window
	// TODO Use pre-computed weights for neighbors=1...5?
	std::vector<Scalar> weights(windowSize * windowSize);
	const Scalar neighborsInv = Scalar(1) / Scalar(neighbors);

	for (uint32_t y = 0u; y < windowSize; ++y)
	{
		const Scalar factorY = (Scalar(y) - Scalar(neighbors)) * neighborsInv;
		const Scalar weightY = NumericT<Scalar>::exp(-(factorY * factorY));

		for (uint32_t x = 0u; x < windowSize; ++x)
		{
			const Scalar factorX = (Scalar(x) - Scalar(neighbors)) * neighborsInv;
			weights[y * windowSize + x] = weightY * NumericT<Scalar>::exp(-(factorX * factorX));
		}
	}

	// Determine the corner location by minimizing the objective function:
	//
	//   E(c) = \sum_{p \in N} (\delta I_p^T (p - c))^2
	//
	// c - corner location
	// N - neighborhood/search window around c
	// p - point in the search window
	// \delta I_p - image gradient at the location p
	//
	// Partial derivatives:
	//
	//     dE / dc = 0
	//
	// <=> | sum_{p \in N} (gx * gx)  sum_{p \in N} (gx * gy) | * | cx | = | sum_{p \in N} (gx * gx) * px + (gx * gy) * py |
	//     | sum_{p \in N} (gx * gy)  sum_{p \in N} (gy * gy) |   | cy |   | sum_{p \in N} (gx * gy) * px + (gy * gy) * py |
	//
	// <=> | a00 a01 | * | cx | = | bx |
	//     | a01 a11 |   | cy |   | by |
	//
	// <=> Ac = b

	constexpr uint32_t maxIterations = 100u;
	Scalar localErrors[maxIterations] = { 0 };

	uint32_t iteration = 0u;
	Vector2 refinedCorner = corner;
	Vector2 sampleCenter = tUseKernel11 ? (refinedCorner - Vector2(1, 1)) : refinedCorner;

	const Scalar leftBorder = Scalar(bufferSize / 2u);
	const Scalar rightBorder = Scalar(yFrame.width() - bufferSize / 2u - 1u);
	const Scalar topBorder = Scalar(bufferSize / 2u);
	const Scalar bottomBorder = Scalar(yFrame.height() - bufferSize / 2u - 1u);

	if (refinedCorner.x() < leftBorder || refinedCorner.x() >= rightBorder || refinedCorner.y() < topBorder || refinedCorner.y() >= bottomBorder
		|| sampleCenter.x() < leftBorder || sampleCenter.x() >= rightBorder || sampleCenter.y() < topBorder || sampleCenter.y() >= bottomBorder)
	{
		return false;
	}

	bool refinementSuccessful = true;
	const Scalar maxSquaredTranslation = Scalar(0.9 * 0.9) * Scalar(neighbors * neighbors);
	Scalar step = 1;

	int32_t sumGradientX = 0;
	int32_t sumGradientY = 0;

	do
	{
		ocean_assert(sampleCenter.x() >= Scalar(bufferSize / 2u));
		ocean_assert(sampleCenter.y() >= Scalar(bufferSize / 2u));

		ocean_assert(sampleCenter.x() < Scalar(yFrame.width() - bufferSize / 2u - 1u));
		ocean_assert(sampleCenter.y() < Scalar(yFrame.height() - bufferSize / 2u - 1u));

		CV::Advanced::AdvancedFrameInterpolatorBilinear::interpolatePatch8BitPerChannel<1u, CV::PC_TOP_LEFT>(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.paddingElements(), buffer.data<uint8_t>(), sampleCenter, bufferSize, bufferSize);

		Scalar a00 = 0;
		Scalar a01 = 0;
		Scalar a11 = 0;
		Scalar bX = 0;
		Scalar bY = 0;

		sumGradientX = 0;
		sumGradientY = 0;

		for (uint32_t y = 0u; y < windowSize; ++y)
		{
			const Scalar pY = Scalar(y) - Scalar(neighbors);

			const uint8_t* bufferRow = buffer.data<uint8_t>() + (y + 1u) * bufferSize + 1u;

			for (uint32_t x = 0u; x < windowSize; ++x)
			{
				int32_t gradientX = 0;
				int32_t gradientY = 0;

				if constexpr (tUseKernel11)
				{
					// Kernel: [-1 +1]
					//              ^-- kernel center
					gradientX = int32_t(*(bufferRow)) - int32_t(*(bufferRow - 1u));
					gradientY = int32_t(*(bufferRow)) - int32_t(*(bufferRow - bufferSize));
				}
				else
				{
					// Kernel: [-1 0 +1]
					//             ^-- kernel center
					gradientX = int32_t(*(bufferRow + 1u)) - int32_t(*(bufferRow - 1u));
					gradientY = int32_t(*(bufferRow + bufferSize)) - int32_t(*(bufferRow - bufferSize));
				}

				sumGradientX += gradientX;
				sumGradientY += gradientY;

				const Scalar weight = weights[y * windowSize + x];
				const Scalar gradientXX = weight * Scalar(gradientX * gradientX);
				const Scalar gradientXY = weight * Scalar(gradientX * gradientY);
				const Scalar gradientYY = weight * Scalar(gradientY * gradientY);

				a00 += gradientXX;
				a01 += gradientXY;
				a11 += gradientYY;

				const Scalar pX = Scalar(x) - Scalar(neighbors);

				bX += gradientXX * pX + gradientXY * pY;
				bY += gradientXY * pX + gradientYY * pY;

				++bufferRow;
			}
		}

		const SquareMatrix2 A(a00, a01, a01, a11);
		Vector2 update;

		if (A.solve(Vector2(bX, bY), update) == false)
		{
			break;
		}

		// The actual error, E(c). is expensive to compute. Using the square distance
		// that the corner is shifted as an approximation.
		const Scalar error = update.sqr();

		localErrors[iteration] = error;

		// If the updated corner location is outside the search window, then this suggests
		// that the size of the search window should be larger
		if (error > maxSquaredTranslation)
		{
			refinementSuccessful = false;
			break;
		}

		if (iteration != 0u && error >= localErrors[iteration - 1u])
		{
			step *= Scalar(0.5);
		}

		refinedCorner = refinedCorner + update * step;
		sampleCenter = tUseKernel11 ? (refinedCorner - Vector2(1, 1)) : refinedCorner;

		if (refinedCorner.x() < leftBorder || refinedCorner.x() >= rightBorder || refinedCorner.y() < topBorder || refinedCorner.y() >= bottomBorder
			|| sampleCenter.x() < leftBorder || sampleCenter.x() >= rightBorder || sampleCenter.y() < topBorder || sampleCenter.y() >= bottomBorder)
		{
			refinementSuccessful = false;
			break;
		}

		++iteration;
	}
	while (iteration < maxIterations && step > Scalar(0.01) && localErrors[iteration - 1u] >= Numeric::weakEps());

	if (errors != nullptr)
	{
		*errors = Scalars(localErrors, localErrors + iteration);
	}

	if (sumGradientX == 0 && sumGradientY == 0)
	{
		// If the gradient is zero, this location cannot be a corner
		return false;
	}

	if (direction != nullptr)
	{
		ocean_assert(sumGradientX != 0 || sumGradientY != 0);
		*direction = Vector2(Scalar(sumGradientX), Scalar(sumGradientY)).normalizedOrZero();
	}

	if (refinementSuccessful && iteration < maxIterations)
	{
		corner = refinedCorner;
		return true;
	}

	return false;
}

inline Scalar Utilities::verifyStereoLocations(const Frame& yFrameA, const Frame& yFrameB, const Vector2& locationA, const Vector2& predictedLocationB, const unsigned int searchRadius, Vector2& locationB)
{
	ocean_assert(yFrameA.isValid() && yFrameB.isValid());
	ocean_assert(searchRadius >= 1u);

	locationB = CV::Advanced::AdvancedMotionZeroMeanSSD::trackPointSubPixelMirroredBorder<1u, 15u>(yFrameA.constdata<uint8_t>(), yFrameB.constdata<uint8_t>(), yFrameA.width(), yFrameA.height(), yFrameB.width(), yFrameB.height(), yFrameA.paddingElements(), yFrameB.paddingElements(), locationA, searchRadius, searchRadius, predictedLocationB, 4u);

	Utilities::refineCorner(yFrameB, locationB, 2u);

	return predictedLocationB.distance(locationB);
}

inline bool Utilities::verifyStereoLocations(const Frame& yFrameA, const Frame& yFrameB, const Vectors2& locationsA, const Vectors2& predictedLocationsB, const unsigned int searchRadius, const Scalar maximalAverageError, Vector2* locationsB)
{
	ocean_assert(yFrameA.isValid() && yFrameB.isValid());

	ocean_assert(yFrameA.paddingElements() == 0u && yFrameB.paddingElements() == 0u);

	ocean_assert(!locationsA.empty());
	ocean_assert(locationsA.size() == predictedLocationsB.size());

	ocean_assert(searchRadius >= 1u);
	ocean_assert(maximalAverageError >= Scalar(0));

	const Scalar maxSumError = maximalAverageError * Scalar(locationsA.size());

	Scalar sumError = Scalar(0);

	for (size_t n = 0; n < locationsA.size(); ++n)
	{
		Vector2 locationB;
		const Scalar error = verifyStereoLocations(yFrameA, yFrameB, locationsA[n], predictedLocationsB[n], searchRadius, locationB);

		sumError += error;

		if (sumError > maxSumError)
		{
			return false;
		}

		if (locationsB)
		{
			locationsB[n] = locationB;
		}
	}

	return sumError <= maxSumError;
}

} // namespace Tracking

} // namespace OculusTags

} // namespace Ocean

#endif // META_OCEAN_TRACKING_OCULUSTAGS_UTILITIES_H
