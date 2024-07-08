/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_FRAME_RECTIFICATION_H
#define META_OCEAN_CV_ADVANCED_FRAME_RECTIFICATION_H

#include "ocean/cv/advanced/Advanced.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/Lookup2.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/Triangle2.h"
#include "ocean/math/Triangle3.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements functions creating rectified images from frame areas or objects located in a frame.
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT FrameRectification
{
	public:

		/**
		 * Definition of a lookup table storing 2D vectors as elements.
		 */
		typedef LookupCorner2<Vector2> LookupTable;

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_ADVANCED_EXPORT Comfort
		{
			public:

				/**
				 * Given a camera image, a planar 3D rectangle defined in world, and a sampling resolution on the rectangle, projects the camera image onto the rectangle to create a resampled 'rectified image'.
				 * Rectified pixels lying outside the camera image will be assigned with a unique color value which can be specified.
				 * @param cameraFrame The frame that captures the 3D plane, must be valid
				 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
				 * @param world_T_camera The transformation between camera and world, with camera pointing towards the negative z-space with y-axis up, must be valid
				 * @param rectangleOrigin Origin of the planar rectangle object, defined in world
				 * @param rectangleHorizontal Vector defining the horizontal edge of the rectangle object (beginning at the origin), defined in world
				 * @param rectangleVertical Vector defining the vertical edge of the rectangle object (beginning at the origin and perpendicular to the rectangleHorizontal vector), defined in world
				 * @param rectifiedFrame The resulting rectified image, with same pixel format as 'cameraFrame', must be valid
				 * @param worker Optional worker object to distribute the computation
				 * @param outsideFrameColor Optional color that is assigned to pixels that map outside the camera frame, if nullptr is used 0x00 will be assigned to each frame channel
				 * @param approximationBinSize Optional width (and height) of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
				 * @return True, if succeeded
				 * @see arbitraryRectangleObject(), planarRectangleObjectIF8BitPerChannel().
				 */
				static bool planarRectangleObject(const Frame& cameraFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector3& rectangleOrigin, const Vector3& rectangleHorizontal, const Vector3& rectangleVertical, Frame& rectifiedFrame, Worker* worker = nullptr, const uint8_t* outsideFrameColor = nullptr, const unsigned int approximationBinSize = 0u);

				/**
				 * Given a camera image, an arbitrary 3D rectangle defined in world, and a sampling resolution on the rectangle, projects the camera image onto the rectangle to create a resampled 'rectified image'.
		 		 * Rectified pixels lying outside the camera image will be assigned with a unique color value which can be specified.
				 * @param cameraFrame The frame that captures the 3D plane, must be valid
				 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
				 * @param world_T_camera The transformation between camera and world, with camera pointing towards the negative z-space with y-axis up, must be valid
				 * @param rectangle0 3D position of the rectangle corresponding to the upper left corner of the resulting rectified frame, in world
				 * @param rectangle1 3D position of the rectangle corresponding to the lower left corner of the resulting rectified frame, in world
				 * @param rectangle2 3D position of the rectangle corresponding to the lower right corner of the resulting rectified frame, in world
				 * @param rectangle3 3D position of the rectangle corresponding to the upper right corner of the resulting rectified frame, in world
				 * @param rectifiedFrame The resulting rectified image, with same pixel format as 'cameraFrame', must be valid
				 * @param worker Optional worker object to distribute the computation
				 * @param outsideFrameColor Optional color that is assigned to pixels that map outside the camera frame, if nullptr is used 0x00 will be assigned to each frame channel
				 * @param approximationBinSize Optional width (and height) of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
				 * @return True, if succeeded
				 * @see planarRectangleObject(), planarRectangleObjectIF8BitPerChannel().
				 */
				static bool arbitraryRectangleObject(const Frame& cameraFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector3& rectangle0, const Vector3& rectangle1, const Vector3& rectangle2, const Vector3& rectangle3, Frame& rectifiedFrame, Worker* worker = nullptr, const uint8_t* outsideFrameColor = nullptr, const unsigned int approximationBinSize = 0u);

				/**
				 * Draws a 2D triangle into an image of a 3D triangle as seen in a camera image.
				 * Pixels lying outside the camera frame will be assigned with a unique color value which can be specified.
				 * @param cameraFrame The frame in which the triangle is visible, must be valid
				 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
				 * @param world_T_camera The transformation between camera and world, with camera pointing towards the negative z-space with y-axis up, must be valid
				 * @param triangle2 2D triangle defined in the target frame, must be valid
				 * @param triangle3 3D triangle defined in the world coordinate system, must be valid
				 * @param targetFrame The target image in which the triangle will be drawn, with same pixel format as 'cameraFrame', must be valid
				 * @param worker Optional worker object to distribute the computation
				 * @param outsideFrameColor Optional color that is assigned to pixels that map outside the camera frame, if nullptr is used 0x00 will be assigned to each frame channel
				 * @param approximationBinSize Optional width (and height) of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
				 * @return True, if succeeded
				 * @see planarRectangleObjectIF8BitPerChannel().
				 */
				static bool triangleObject(const Frame& cameraFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Triangle2& triangle2, const Triangle3& triangle3, Frame& targetFrame, Worker* worker = nullptr, const uint8_t* outsideFrameColor = nullptr, const unsigned int approximationBinSize = 0u);

				/**
				 * Given a camera image, a planar 3D rectangle defined in world, and a sampling resolution on the rectangle, projects the camera image onto the rectangle to create a resampled 'rectified image'.
				 * Rectified pixels lying outside the camera will be masked in the resulting mask frame.
				 * @param cameraFrame The frame that captures the 3D plane, must be valid
				 * @param camera The camera object that defines the projection and must have the same dimension as the given frame, must be valid
				 * @param world_T_camera The camera pose transforming camera to world, must be valid
				 * @param rectangleOrigin Origin of the planar rectangle object in world units
				 * @param rectangleHorizontal Vector defining the horizontal edge of the rectangle object (beginning at the origin) in world units
				 * @param rectangleVertical Vector defining the vertical edge of the rectangle object (beginning at the origin and perpendicular to the rectangleHorizontal vector) in world units
				 * @param targetFrame The resulting target image, with same pixel format as 'cameraFrame', must be valid
				 * @param targetMask The resulting mask separating target pixels which do not have corresponding camera image pixels
				 * @param worker Optional worker object to distribute the computation
				 * @param maskValue The mask value for pixels lying inside the given camera frame, pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
				 * @param approximationBinSize Optional width (and height) of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
				 * @return True, if succeeded
				 * @see arbitraryRectangleObjectMask(), planarRectangleObjectMaskIF8BitPerChannel().
				 */
				static bool planarRectangleObjectMask(const Frame& cameraFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector3& rectangleOrigin, const Vector3& rectangleHorizontal, const Vector3& rectangleVertical, Frame& targetFrame, Frame& targetMask, Worker* worker = nullptr, const uint8_t maskValue = 0xFFu, const unsigned int approximationBinSize = 0u);

				/**
				 * Given a camera image, an arbitrary 3D rectangle defined in world, and a sampling resolution on the rectangle, projects the camera image onto the rectangle to create a resampled 'rectified image'.
				 * Rectified pixels lying outside the camera will be masked in the resulting mask frame.
				 * @param cameraFrame The frame that captures the 3D plane, must be valid
				 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
				 * @param world_T_camera The transformation between camera and world, with camera pointing towards the negative z-space with y-axis up, must be valid
				 * @param rectangle0 3D position of the rectangle corresponding to the upper left corner of the resulting target frame
				 * @param rectangle1 3D position of the rectangle corresponding to the lower left corner of the resulting target frame
				 * @param rectangle2 3D position of the rectangle corresponding to the lower right corner of the resulting target frame
				 * @param rectangle3 3D position of the rectangle corresponding to the upper right corner of the resulting target frame
				 * @param targetFrame The resulting target image, with same pixel format as 'cameraFrame', must be valid
				 * @param targetMask The resulting mask separating target pixels which do not have corresponding camera image pixels
				 * @param worker Optional worker object to distribute the computation
				 * @param maskValue The mask value for pixels lying inside the given camera frame, pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
				 * @param approximationBinSize Optional width (and height) of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
				 * @return True, if succeeded
				 * @see planarRectangleObjectMask(), arbitraryRectangleObjectMaskIF8BitPerChannel().
				 */
				static bool arbitraryRectangleObjectMask(const Frame& cameraFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Vector3& rectangle0, const Vector3& rectangle1, const Vector3& rectangle2, const Vector3& rectangle3, Frame& targetFrame, Frame& targetMask, Worker* worker = nullptr, const uint8_t maskValue = 0xFFu, const unsigned int approximationBinSize = 0u);

				/**
				 * Draws a 2D triangle into an image of a 3D triangle as seen in a camera image.
				 * Pixels lying outside the camera will be masked in the resulting mask frame.
				 * @param cameraFrame The frame that captures the 3D plane, must be valid
				 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
				 * @param world_T_camera The transformation between camera and world, with camera pointing towards the negative z-space with y-axis up, must be valid
				 * @param triangle2 The 2D triangle defined in the target frame, must be valid
				 * @param triangle3 The 3D triangle defined in the world coordinate system, must be valid
				 * @param targetFrame The target image in which the triangle will be drawn, with same pixel format as 'cameraFrame', must be valid
				 * @param targetMask The resulting mask separating target pixels which do not have corresponding camera image pixels
				 * @param worker Optional worker object to distribute the computation
				 * @param maskValue The mask value for pixels lying inside the given camera frame, pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
				 * @param approximationBinSize Optional width (and height) of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
				 * @return True, if succeeded
				 * @see planarRetangleObjectMaskIF8BitPerChannel().
				 */
				static bool triangleObjectMask(const Frame& cameraFrame, const AnyCamera& camera, const HomogenousMatrix4& world_T_camera, const Triangle2& triangle2, const Triangle3& triangle3, Frame& targetFrame, Frame& targetMask, Worker* worker = nullptr, const uint8_t maskValue = 0xFFu, const unsigned int approximationBinSize = 0u);
		};

	public:

		/**
		 * Converts the pixel position defined in the rectified frame to the pixel position defined in the camera frame inside the planar rectangle 3D object.
		 * Beware: The actual center of a pixel is located at the upper left corner of the individual pixels' squares.
		 * @param anyCamera The camera profile to be applied
		 * @param flippedCamera_T_world The inverted and flipped extrinsic camera matrix, must be valid
		 * @param rectangleOrigin Origin of the planar rectangle object in world units
		 * @param rectangleHorizontal Vector defining the horizontal edge of the 3D rectangle object (beginning at the origin) in world units
		 * @param rectangleVertical Vector defining the vertical edge of the 3D rectangle object (beginning at the origin and perpendicular to the rectangleHorizontal vector) in world units
		 * @param rectifiedWidth The width of the rectified frame in pixel, with range [1, infinity)
		 * @param rectifiedHeight The height of the rectified frame in pixel, with range [1, infinity)
		 * @param rectifiedPosition The pixel position in the rectified frame, with range [0, rectifiedWidth)x[0, rectifiedHeight)
		 * @return Resulting pixel position defined in the camera frame space, may be outside the actual frame dimension
		 * @tparam tPixelOriginUpperLeft True, if the pixel origin of the camera frame and rectified frame is in the upper left corner; False if the pixel origin is in the lower left corner
		 * @see PinholeCamera::standard2InvertedFlipped().
		 */
		template <bool tPixelOriginUpperLeft>
		static Vector2 planarRectangleObjectRectifiedPosition2cameraPositionIF(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangleOrigin, const Vector3& rectangleHorizontal, const Vector3& rectangleVertical, const unsigned int rectifiedWidth, const unsigned int rectifiedHeight, const Vector2& rectifiedPosition);

		/**
		 * Given a camera image, a planar 3D rectangle defined in world, and a sampling resolution on the rectangle, projects the camera image onto the rectangle to create a resampled 'rectified image'.
		 * Rectified pixels lying outside the camera image will be assigned with a unique color value which can be specified.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param pixelOrigin The pixel origin of the given frame (and the resulting rectified frame)
		 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with flipped camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param rectangleOrigin Origin of the planar rectangle object in world units
		 * @param rectangleHorizontal Vector defining the horizontal edge of the 2D rectified object (beginning at the origin) in world units
		 * @param rectangleVertical Vector defining the vertical edge of the 2D rectified object (beginning at the origin and perpendicular to the rectangleHorizontal vector) in world units
		 * @param rectifiedFrame The resulting rectified image, must be valid
		 * @param rectifiedFrameWidth Width of the rectified image, in pixel, with range [1, infinity)
		 * @param rectifiedFrameHeight Height of the rectified image, in pixel, with range [1, infinity)
		 * @param rectifiedFramePaddingElements The number of padding elements at the end of each rectified frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param outsideFrameColor Color that is assigned to pixels that map outside the camera frame, if nullptr is used 0x00 will be assigned to each frame channel
		 * @param approximationBinSize Optional width (and height) of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 * @see planarRectangleObject().
		 */
		template <unsigned int tChannels>
		static inline void planarRectangleObjectIF8BitPerChannel(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const FrameType::PixelOrigin pixelOrigin, const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangleOrigin, const Vector3& rectangleHorizontal, const Vector3& rectangleVertical, uint8_t* rectifiedFrame, const unsigned int rectifiedFrameWidth, const unsigned int rectifiedFrameHeight, const unsigned int rectifiedFramePaddingElements, Worker* worker = nullptr, const uint8_t* outsideFrameColor = nullptr, const unsigned int approximationBinSize = 0u);

		/**
		 * Given a camera image, an arbitrary 3D rectangle defined in world, and a sampling resolution on the rectangle, projects the camera image onto the rectangle to create a resampled 'rectified image'.
		 * Rectified pixels lying outside the camera image will be assigned with a unique color value which can be specified.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param pixelOrigin The pixel origin of the given frame (and the resulting rectified frame)
		 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with flipped camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param rectangle0 3D position of the rectangle corresponding to the upper left corner of the resulting rectified frame, in world
		 * @param rectangle1 3D position of the rectangle corresponding to the lower left corner of the resulting rectified frame, in world
		 * @param rectangle2 3D position of the rectangle corresponding to the lower right corner of the resulting rectified frame, in world
		 * @param rectangle3 3D position of the rectangle corresponding to the upper right corner of the resulting rectified frame, in world
		 * @param rectifiedFrame The resulting rectified image, must be valid
		 * @param rectifiedFrameWidth Width of the rectified image, in pixel, with range [1, infinity)
		 * @param rectifiedFrameHeight Height of the rectified image, in pixel, with range [1, infinity)
		 * @param rectifiedFramePaddingElements The number of padding elements at the end of each rectified frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param outsideFrameColor Color that is assigned to pixels that map outside the camera frame, if nullptr is used 0x00 will be assigned to each frame channel
		 * @param approximationBinSize Optional width (and height) of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 * @see arbitraryRectangleObject().
		 */
		template <unsigned int tChannels>
		static inline void arbitraryRectangleObjectIF8BitPerChannel(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const FrameType::PixelOrigin pixelOrigin, const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangle0, const Vector3& rectangle1, const Vector3& rectangle2, const Vector3& rectangle3, uint8_t* rectifiedFrame, const unsigned int rectifiedFrameWidth, const unsigned int rectifiedFrameHeight, const unsigned int rectifiedFramePaddingElements, Worker* worker = nullptr, const uint8_t* outsideFrameColor = nullptr, const unsigned int approximationBinSize = 0u);

		/**
		 * Draws a 2D triangle into an image of a 3D triangle as seen in a camera image.
		 * Pixels lying outside the camera frame will be assigned with a unique color value which can be specified.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param pixelOrigin The pixel origin of the given frame (and the resulting rectified frame)
		 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with flipped camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param triangle2 2D triangle defined in the rectified frame
		 * @param triangle3 3D triangle defined in the world coordinate system
		 * @param targetFrame The target image in which the triangle will be drawn, must be valid
		 * @param targetWidth The width of the target image, in pixel, with range [1, infinity)
		 * @param targetHeight The height of the target image, in pixel, with range [1, infinity)
		 * @param targetPaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param outsideFrameColor Color that is assigned to pixels that map outside the camera frame, if nullptr is used 0x00 will be assigned to each frame channel
		 * @param approximationBinSize Optional width (and height) of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 * @see planarRectangleObject().
		 */
		template <unsigned int tChannels>
		static inline void triangleObjectIF8BitPerChannel(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const FrameType::PixelOrigin pixelOrigin, const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Triangle2& triangle2, const Triangle3& triangle3, uint8_t* targetFrame, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetPaddingElements, Worker* worker = nullptr, const uint8_t* outsideFrameColor = nullptr, const unsigned int approximationBinSize = 0u);

		/**
		 * Given a camera image, a planar 3D rectangle defined in world, and a sampling resolution on the rectangle, projects the camera image onto the rectangle to create a resampled 'rectified image'.
		 * Rectified pixels lying outside the camera will be masked in the resulting mask frame.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param pixelOrigin The pixel origin of the given frame (and the resulting rectified frame)
		 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with flipped camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param rectangleOrigin Origin of the planar rectangle object in world units
		 * @param rectangleHorizontal Vector defining the horizontal edge of the 2D rectified object (beginning at the origin) in world units
		 * @param rectangleVertical Vector defining the vertical edge of the 2D rectified object (beginning at the origin and perpendicular to the rectangleHorizontal vector) in world units
		 * @param targetFrame The resulting target image, must be valid
		 * @param targetMask The resulting mask, must be valid
		 * @param targetWidth Width of the target image, in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target image, in pixel, with range [1, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param targetMaskPaddingElements The number of padding elements at the end of each row target mask row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param maskValue The mask value for pixels lying inside the given camera frame, pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param approximationBinSize Optional width (and height) of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 * @see planarRectangleObjectMask().
		 */
		template <unsigned int tChannels>
		static inline void planarRectangleObjectMaskIF8BitPerChannel(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const FrameType::PixelOrigin pixelOrigin, const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangleOrigin, const Vector3& rectangleHorizontal, const Vector3& rectangleVertical, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, Worker* worker = nullptr, const uint8_t maskValue = 0xFFu, const unsigned int approximationBinSize = 0u);

		/**
		 * Given a camera image, an arbitrary 3D rectangle defined in world, and a sampling resolution on the rectangle, projects the camera image onto the rectangle to create a resampled 'rectified image'.
		 * Rectified pixels lying outside the camera will be masked in the resulting mask frame.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param pixelOrigin The pixel origin of the given frame (and the resulting rectified frame)
		 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with flipped camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param rectangle0 3D position of the rectangle corresponding to the upper left corner of the resulting target frame
		 * @param rectangle1 3D position of the rectangle corresponding to the lower left corner of the resulting target frame
		 * @param rectangle2 3D position of the rectangle corresponding to the lower right corner of the resulting target frame
		 * @param rectangle3 3D position of the rectangle corresponding to the upper right corner of the resulting target frame
		 * @param targetFrame The resulting target image, must be valid
		 * @param targetMask The resulting mask, must be valid
		 * @param targetWidth Width of the target image, in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target image, in pixel, with range [1, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param targetMaskPaddingElements The number of padding elements at the end of each row target mask row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param maskValue 8 bit mask values for rectified pixels lying inside the given camera frame, rectified pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param approximationBinSize Optional width (and height) of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 * @see planarRectangleObjectMask().
		 */
		template <unsigned int tChannels>
		static inline void arbitraryRectangleObjectMaskIF8BitPerChannel(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const FrameType::PixelOrigin pixelOrigin, const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangle0, const Vector3& rectangle1, const Vector3& rectangle2, const Vector3& rectangle3, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, Worker* worker = nullptr, const uint8_t maskValue = 0xFFu, const unsigned int approximationBinSize = 0u);

		/**
		 * Draws a 2D triangle into an image of a 3D triangle as seen in a camera image.
		 * Target pixels lying outside the camera will be masked in the resulting mask frame.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param pixelOrigin The pixel origin of the given frame (and the resulting rectified frame)
		 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with flipped camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param triangle2 2D triangle defined in the target frame, must be valid
		 * @param triangle3 3D triangle defined in the world coordinate system, must be valid
		 * @param targetFrame The resulting target image, must be valid
		 * @param targetMask The resulting mask, must be valid
		 * @param targetWidth Width of the target image, in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target image, in pixel, with range [1, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param targetMaskPaddingElements The number of padding elements at the end of each row target mask row, in elements, with range [0, infinity)
		 * @param worker Optional worker object to distribute the computation
		 * @param maskValue 8 bit mask values for rectified pixels lying inside the given camera frame, rectified pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param approximationBinSize Optional width (and height) of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 * @see planarRectangleObjectMask().
		 */
		template <unsigned int tChannels>
		static inline void triangleObjectMaskIF8BitPerChannel(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const FrameType::PixelOrigin pixelOrigin, const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Triangle2& triangle2, const Triangle3& triangle3, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, Worker* worker = nullptr, const uint8_t maskValue = 0xFFu, const unsigned int approximationBinSize = 0u);

	private:

		/**
		 * Creates an interpolation lookup table for a rectification of a planar rectangle object (a 2D texture in 3D place).
		 * @param anyCamera The camera profile to be applied
		 * @param flippedCamera_T_world The inverted and flipped camera matrix of the standard camera matrix defining the camera pose, must be valid
		 * @param rectangleOrigin Origin of the planar rectangle object in world units
		 * @param rectangleHorizontal Vector defining the horizontal edge of the 3D rectified object (beginning at the origin) in world units
		 * @param rectangleVertical Vector defining the vertical edge of the 3D rectified object (beginning at the origin and perpendicular to the rectangleHorizontal vector) in world units
		 * @param lookupTable The resulting lookup table, must be defined (size and number of bins) before this function can be called
		 * @tparam tPixelOriginUpperLeft True, if the pixel origin of the frame (and the rectified frame) is FrameType::ORIGIN_UPPER_LEFT, False otherwise
		 */
		template <bool tPixelOriginUpperLeft>
		static void planarRectangleObjectIFLookupTable(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangleOrigin, const Vector3& rectangleHorizontal, const Vector3& rectangleVertical, LookupTable& lookupTable);

		/**
		 * Creates an interpolation lookup table for a rectification of an arbitrary rectangle object (a 2D texture in 3D place).
		 * @param anyCamera The camera profile to be applied, must be valid
		 * @param flippedCamera_T_world The transformation between world and the flipped camera, must be valid
		 * @param rectangle0 3D position of the rectangle corresponding to the upper left corner of the resulting frame
		 * @param rectangle1 3D position of the rectangle corresponding to the lower left corner of the resulting frame
		 * @param rectangle2 3D position of the rectangle corresponding to the lower right corner of the resulting frame
		 * @param rectangle3 3D position of the rectangle corresponding to the upper right corner of the resulting frame
		 * @param lookupTable The resulting lookup table, must be defined (size and number of bins) before this function can be called
		 * @tparam tPixelOriginUpperLeft True, if the pixel origin of the frame (and the rectified frame) is FrameType::ORIGIN_UPPER_LEFT, False otherwise
		 */
		template <bool tPixelOriginUpperLeft>
		static void arbitraryRectangleObjectIFLookupTable(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangle0, const Vector3& rectangle1, const Vector3& rectangle2, const Vector3& rectangle3, LookupTable& lookupTable);

		/**
		 * Creates an interpolation lookup table for a triangle object (a 2D texture in 3D place).
		 * @param anyCamera The camera profile to be applied, must be valid
		 * @param flippedCamera_T_world The transformation between world and the flipped camera, must be valid
		 * @param triangle2 The triangle defined in the rectified frame, must be valid
		 * @param triangle3 The triangle defined in the 3D space, must be valid
		 * @param lookupTable The resulting lookup table, must be defined (size and number of bins) before this function can be called
		 * @tparam tPixelOriginUpperLeft True, if the pixel origin of the frame (and the rectified frame) is FrameType::ORIGIN_UPPER_LEFT, False otherwise
		 */
		template <bool tPixelOriginUpperLeft>
		static void triangleObjectIFLookupTable(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world, const Triangle2& triangle2, const Triangle3& triangle3, LookupTable& lookupTable);

		/**
		 * Creates a subset of the rectangular image of an area on a 3D plane as seen in a camera image.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with flipped camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param rectangleOrigin Origin of the planar rectangle object in world units
		 * @param rectangleHorizontal Vector defining the horizontal edge of the 2D rectified object (beginning at the origin) in world units
		 * @param rectangleVertical Vector defining the vertical edge of the 2D rectified object (beginning at the origin and perpendicular to the rectangleHorizontal vector) in world units
		 * @param rectifiedFrame The resulting rectified image, must be valid
		 * @param rectifiedFrameWidth Width of the rectified image, in pixel, with range [1, infinity)
		 * @param rectifiedFrameHeight Height of the rectified image, in pixel, with range [1, infinity)
		 * @param rectifiedFramePaddingElements The number of padding elements at the end of each rectified frame row, in elements, with range [0, infinity)
		 * @param outsideFrameColor Color that is assigned to pixels that map outside the camera frame, if nullptr is used 0x00 will be assigned to each frame channel
		 * @param firstRectifiedFrameRow First rectified row to be handled, with range [0, rectifiedFrameHeight - 1]
		 * @param numberRectifiedFrameRows The number of rectified rows to be handled, with range [1, rectifiedFrameHeight - firstRectifiedFrameRow]
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 * @tparam tPixelOriginUpperLeft True, if the pixel origin of the frame (and the rectified frame) is FrameType::ORIGIN_UPPER_LEFT, False otherwise
		 */
		template <unsigned int tChannels, bool tPixelOriginUpperLeft>
		static void planarRectangleObjectIF8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_world, const Vector3* rectangleOrigin, const Vector3* rectangleHorizontal, const Vector3* rectangleVertical, uint8_t* rectifiedFrame, const unsigned int rectifiedFrameWidth, const unsigned int rectifiedFrameHeight, const unsigned int rectifiedFramePaddingElements, const uint8_t* outsideFrameColor, const unsigned int firstRectifiedFrameRow, const unsigned int numberRectifiedFrameRows);

		/**
		 * Creates a subset of the rectangle image from a specific position in 3D space.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with flipped camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param rectangle The four 3D positions of the rectangle corresponding to the resulting rectified frame, with order top left, bottom left, bottom right, top right, must be valid
		 * @param rectifiedFrame The resulting rectified image, must be valid
		 * @param rectifiedFrameWidth Width of the rectified image, in pixel, with range [1, infinity)
		 * @param rectifiedFrameHeight Height of the rectified image, in pixel, with range [1, infinity)
		 * @param rectifiedFramePaddingElements The number of padding elements at the end of each rectified frame row, in elements, with range [0, infinity)
		 * @param outsideFrameColor Color that is assigned to pixels that map outside the camera frame, if nullptr is used 0x00 will be assigned to each frame channel
		 * @param firstRectifiedFrameRow First rectified row to be handled, with range [0, rectifiedFrameHeight - 1]
		 * @param numberRectifiedFrameRows The number of rectified rows to be handled, with range [1, rectifiedFrameHeight - firstRectifiedFrameRow]
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 * @tparam tPixelOriginUpperLeft True, if the pixel origin of the frame (and the rectified frame) is FrameType::ORIGIN_UPPER_LEFT, False otherwise
		 */
		template <unsigned int tChannels, bool tPixelOriginUpperLeft>
		static void arbitraryRectangleObjectIF8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_world, const Vector3* rectangle, uint8_t* rectifiedFrame, const unsigned int rectifiedFrameWidth, const unsigned int rectifiedFrameHeight, const unsigned int rectifiedFramePaddingElements, const uint8_t* outsideFrameColor, const unsigned int firstRectifiedFrameRow, const unsigned int numberRectifiedFrameRows);

		/**
		 * Draws a subset of a 2D triangle into a target frame.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with flipped camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param triangle2 2D triangle defined in the rectified frame
		 * @param triangle3 3D triangle defined in the world coordinate system
		 * @param targetFrame The target image in which the triangle will be drawn, must be valid
		 * @param targetWidth Width of the target image in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target image in pixel, with range [1, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param outsideFrameColor Color that is assigned to pixels that map outside the camera frame, if nullptr is used 0x00 will be assigned to each frame channel
		 * @param firstTargetRow First target row to be handled
		 * @param numberTargetRows Number of target rows to be handled
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 * @tparam tPixelOriginUpperLeft True, if the pixel origin of the frame (and the rectified frame) is FrameType::ORIGIN_UPPER_LEFT, False otherwise
		 */
		template <unsigned int tChannels, bool tPixelOriginUpperLeft>
		static void triangleObjectIF8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_world, const Triangle2* triangle2, const Triangle3* triangle3, uint8_t* targetFrame, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const uint8_t* outsideFrameColor, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Draws a subset of a 2D triangle into an image of a 3D triangle as seen in a camera image.
		 * @param cameraFrame The frame in which the triangle is visible, must be valid
		 * @param cameraFrameWidth The width of the camera frame, in pixel, with range [1, infinity)
		 * @param cameraFrameHeight The height of the camera frame, in pixel, with range [1, infinity)
		 * @param cameraFramePaddingElements The number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @param lookupTable The lookup table allowing to approximate the triangle positions
		 * @param triangle2 2D triangle defined in the target frame, must be valid
		 * @param targetFrame The target frame in which the triangle will be drawn, must be valid
		 * @param targetFramePaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param outsideFrameColor Color that is assigned to pixels that map outside the camera frame, if nullptr is used 0x00 will be assigned to each frame channel
		 * @param firstTargetRow First target row to be handled
		 * @param numberTargetRows Number of target rows to be handled
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void triangleObjectLookup8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFrameWidth, const unsigned int cameraFrameHeight, const unsigned int cameraFramePaddingElements, const LookupTable* lookupTable, const Triangle2* triangle2, uint8_t* targetFrame, const unsigned int targetFramePaddingElements, const uint8_t* outsideFrameColor, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Creates a subset of the rectangular image of an area on a 3D plane as seen in a camera image.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with flipped camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param rectangleOrigin Origin of the planar rectangle object in world units
		 * @param rectangleHorizontal Vector defining the horizontal edge of the rectangle object (beginning at the origin) in world units
		 * @param rectangleVertical Vector defining the vertical edge of the rectangle object (beginning at the origin and perpendicular to the rectangleHorizontal vector) in world units
		 * @param targetFrame The resulting target image, must be valid
		 * @param targetMask The resulting mask, must be valid
		 * @param targetWidth Width of the target image, in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target image, in pixel, with range [1, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param targetMaskPaddingElements The number of padding elements at the end of each row target mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value for pixels lying inside the given camera frame, pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param firstTargetRow First target row to be handled
		 * @param numberTargetRows Number of target rows to be handled
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 * @tparam tPixelOriginUpperLeft True, if the pixel origin of the frame (and the target frame) is FrameType::ORIGIN_UPPER_LEFT, False otherwise
		 */
		template <unsigned int tChannels, bool tPixelOriginUpperLeft>
		static void planarRectangleObjectMaskIF8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_world, const Vector3* rectangleOrigin, const Vector3* rectangleHorizontal, const Vector3* rectangleVertical, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, const uint8_t maskValue, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Creates a subset of the rectangular image from a specific position in 3D space.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with flipped camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param rectangle The four 3D positions of the rectangle corresponding to the resulting rectified frame, with order top left, bottom left, bottom right, top right, must be valid
		 * @param targetFrame The resulting target image, must be valid
		 * @param targetMask The resulting mask, must be valid
		 * @param targetWidth Width of the target image, in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target image, in pixel, with range [1, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param targetMaskPaddingElements The number of padding elements at the end of each row target mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value for pixels lying inside the given camera frame, pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param firstTargetRow First target row to be handled, with range [0, targetHeight - 1]
		 * @param numberTargetRows The number of target rows to be handled, with range [1, targetHeight - firstTargetRow]
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 * @tparam tPixelOriginUpperLeft True, if the pixel origin of the frame (and the target frame) is FrameType::ORIGIN_UPPER_LEFT, False otherwise
		 */
		template <unsigned int tChannels, bool tPixelOriginUpperLeft>
		static void arbitraryRectangleObjectMaskIF8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_world, const Vector3* rectangle, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, const uint8_t maskValue, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Creates a subset of a 2D triangle in a target image from a specific 3D triangle on a 3D plane.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param camera The camera profile defining the projection of the camera frame, must have the same resolution as the camera frame, must be valid
		 * @param flippedCamera_T_world The transformation between world and flipped camera, with flipped camera pointing towards the positive z-space with y-axis down, must be valid
		 * @param triangle2 2D triangle defined in the target frame, must be valid
		 * @param triangle3 3D triangle defined in the world coordinate system, must be valid
		 * @param targetFrame The resulting target image, must be valid
		 * @param targetMask The resulting mask, must be valid
		 * @param targetWidth Width of the target image, in pixel, with range [1, infinity)
		 * @param targetHeight Height of the target image, in pixel, with range [1, infinity)
		 * @param targetFramePaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param targetMaskPaddingElements The number of padding elements at the end of each row target mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value for pixels lying inside the given camera frame, pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param firstTargetRow First target row to be handled, with range [0, targetHeight - 1]
		 * @param numberTargetRows The number of target rows to be handled, with range [1, targetHeight - firstTargetRow]
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 * @tparam tPixelOriginUpperLeft True, if the pixel origin of the frame (and the target frame) is FrameType::ORIGIN_UPPER_LEFT, False otherwise
		 */
		template <unsigned int tChannels, bool tPixelOriginUpperLeft>
		static void triangleObjectMaskIF8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_world, const Triangle2* triangle2, const Triangle3* triangle3, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, const uint8_t maskValue, const unsigned int firstTargetRow, const unsigned int numberTargetRows);

		/**
		 * Creates a subset of a 2D triangle in a target image from a specific 3D triangle on a 3D plane and applies a lookup table.
		 * @param cameraFrame The frame that captures the 3D plane, must be valid
		 * @param cameraFrameWidth The width of the camera frame, in pixels, with range [1, infinity)
		 * @param cameraFrameHeight The height of the camera frame, in pixels, with range [1, infinity)
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param lookupTable The lookup table allowing to approximate the triangle positions
		 * @param triangle2 2D triangle defined in the target frame
		 * @param targetFrame The resulting target image, must be valid
		 * @param targetMask The resulting mask, must be valid
		 * @param targetFramePaddingElements The number of padding elements at the end of each target frame row, in elements, with range [0, infinity)
		 * @param targetMaskPaddingElements The number of padding elements at the end of each row target mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value for pixels lying inside the given camera frame, pixels lying outside the camera frame will be assigned with (0xFF - maskValue)
		 * @param firstTargetRow First target row to be handled, with range [0, targetHeight - 1]
		 * @param numberTargetRows The number of target rows to be handled, with range [1, targetHeight - firstTargetRow]
		 * @tparam tChannels Number of data channels of the given frame, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void triangleObjectMaskLookup8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFrameWidth, const unsigned int cameraFrameHeight, const unsigned int cameraFramePaddingElements, const LookupTable* lookupTable, const Triangle2* triangle2, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, const uint8_t maskValue, const unsigned int firstTargetRow, const unsigned int numberTargetRows);
};

template <bool tPixelOriginUpperLeft>
Vector2 FrameRectification::planarRectangleObjectRectifiedPosition2cameraPositionIF(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangleOrigin, const Vector3& rectangleHorizontal, const Vector3& rectangleVertical, const unsigned int rectifiedWidth, const unsigned int rectifiedHeight, const Vector2& rectifiedPosition)
{
	ocean_assert(anyCamera.isValid() && flippedCamera_T_world.isValid());
	ocean_assert(rectifiedWidth != 0u && rectifiedHeight != 0u);

	ocean_assert(rectifiedPosition.x() >= 0 && rectifiedPosition.x() < Scalar(rectifiedWidth));
	ocean_assert(rectifiedPosition.y() >= 0 && rectifiedPosition.y() < Scalar(rectifiedHeight));

	const Scalar yCorrected = tPixelOriginUpperLeft ? rectifiedPosition.y() : (Scalar(rectifiedHeight - 1u) - rectifiedPosition.y());

	// the actual pixel position is located at the upper left corner of each individual pixel
	const Vector3 objectPoint(rectangleOrigin + rectangleHorizontal * (rectifiedPosition.x() / Scalar(rectifiedWidth)) + rectangleVertical * (yCorrected / Scalar(rectifiedHeight)));
	const Vector2 cameraPoint(anyCamera.projectToImageIF(flippedCamera_T_world, objectPoint));

	const Vector2 correctedCameraPoint(cameraPoint.x(), tPixelOriginUpperLeft ? cameraPoint.y() : (Scalar(anyCamera.height() - 1u) - cameraPoint.y()));

	return correctedCameraPoint;
}

template <unsigned int tChannels>
inline void FrameRectification::planarRectangleObjectIF8BitPerChannel(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const FrameType::PixelOrigin pixelOrigin, const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangleOrigin, const Vector3& rectangleHorizontal, const Vector3& rectangleVertical, uint8_t* rectifiedFrame, const unsigned int rectifiedFrameWidth, const unsigned int rectifiedFrameHeight, const unsigned int rectifiedFramePaddingElements, Worker* worker, const uint8_t* outsideFrameColor, const unsigned int approximationBinSize)
{
	static_assert(tChannels > 0u, "Invalid frame channel number!");

	const uint8_t zeroValue[tChannels] = {0x00};

	if (approximationBinSize == 0u)
	{
		if (worker)
		{
			if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
			{
				worker->executeFunction(Worker::Function::createStatic(planarRectangleObjectIF8BitPerChannelSubset<tChannels, true>, cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &rectangleOrigin, &rectangleHorizontal, &rectangleVertical, rectifiedFrame, rectifiedFrameWidth, rectifiedFrameHeight, rectifiedFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, 0u), 0u, rectifiedFrameHeight, 12u, 13u, 20u);
			}
			else
			{
				worker->executeFunction(Worker::Function::createStatic(planarRectangleObjectIF8BitPerChannelSubset<tChannels, false>, cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &rectangleOrigin, &rectangleHorizontal, &rectangleVertical, rectifiedFrame, rectifiedFrameWidth, rectifiedFrameHeight, rectifiedFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, 0u), 0u, rectifiedFrameHeight, 12u, 13u, 20u);
			}
		}
		else
		{
			if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
			{
				planarRectangleObjectIF8BitPerChannelSubset<tChannels, true>(cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &rectangleOrigin, &rectangleHorizontal, &rectangleVertical, rectifiedFrame, rectifiedFrameWidth, rectifiedFrameHeight, rectifiedFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, rectifiedFrameHeight);
			}
			else
			{
				planarRectangleObjectIF8BitPerChannelSubset<tChannels, false>(cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &rectangleOrigin, &rectangleHorizontal, &rectangleVertical, rectifiedFrame, rectifiedFrameWidth, rectifiedFrameHeight, rectifiedFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, rectifiedFrameHeight);
			}
		}
	}
	else
	{
		const unsigned int binsX = minmax(1u, rectifiedFrameWidth / approximationBinSize, rectifiedFrameWidth / 4u);
		const unsigned int binsY = minmax(1u, rectifiedFrameHeight / approximationBinSize, rectifiedFrameHeight / 4u);
		LookupTable lookupTable(rectifiedFrameWidth, rectifiedFrameHeight, binsX, binsY);

		if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
		{
			planarRectangleObjectIFLookupTable<true>(camera, flippedCamera_T_world, rectangleOrigin, rectangleHorizontal, rectangleVertical, lookupTable);
		}
		else
		{
			planarRectangleObjectIFLookupTable<false>(camera, flippedCamera_T_world, rectangleOrigin, rectangleHorizontal, rectangleVertical, lookupTable);
		}

		CV::FrameInterpolatorBilinear::lookup<uint8_t, tChannels>(cameraFrame, camera.width(), camera.height(), lookupTable, false, outsideFrameColor ? outsideFrameColor : zeroValue, rectifiedFrame, cameraFramePaddingElements, rectifiedFramePaddingElements, worker);
	}
}

template <unsigned int tChannels>
inline void FrameRectification::arbitraryRectangleObjectIF8BitPerChannel(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const FrameType::PixelOrigin pixelOrigin, const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangle0, const Vector3& rectangle1, const Vector3& rectangle2, const Vector3& rectangle3, uint8_t* rectifiedFrame, const unsigned int rectifiedFrameWidth, const unsigned int rectifiedFrameHeight, const unsigned int rectifiedFramePaddingElements, Worker* worker, const uint8_t* outsideFrameColor, const unsigned int approximationBinSize)
{
	static_assert(tChannels > 0u, "Invalid frame channel number!");

	const uint8_t zeroValue[tChannels] = {0x00};

	if (approximationBinSize == 0u)
	{
		const Vector3 rectangle[4] = {rectangle0, rectangle1, rectangle2, rectangle3};

		if (worker)
		{
			if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
			{
				worker->executeFunction(Worker::Function::createStatic(arbitraryRectangleObjectIF8BitPerChannelSubset<tChannels, true>, cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, rectangle, rectifiedFrame, rectifiedFrameWidth, rectifiedFrameHeight, rectifiedFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, 0u), 0u, rectifiedFrameHeight, 10u, 11u, 20u);
			}
			else
			{
				worker->executeFunction(Worker::Function::createStatic(arbitraryRectangleObjectIF8BitPerChannelSubset<tChannels, false>, cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, rectangle, rectifiedFrame, rectifiedFrameWidth, rectifiedFrameHeight, rectifiedFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, 0u), 0u, rectifiedFrameHeight, 10u, 11u, 20u);
			}
		}
		else
		{
			if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
			{
				arbitraryRectangleObjectIF8BitPerChannelSubset<tChannels, true>(cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, rectangle, rectifiedFrame, rectifiedFrameWidth, rectifiedFrameHeight, rectifiedFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, rectifiedFrameHeight);
			}
			else
			{
				arbitraryRectangleObjectIF8BitPerChannelSubset<tChannels, false>(cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, rectangle, rectifiedFrame, rectifiedFrameWidth, rectifiedFrameHeight, rectifiedFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, rectifiedFrameHeight);
			}
		}
	}
	else
	{
		const unsigned int binsX = minmax(1u, rectifiedFrameWidth / approximationBinSize, rectifiedFrameWidth / 4u);
		const unsigned int binsY = minmax(1u, rectifiedFrameHeight / approximationBinSize, rectifiedFrameHeight / 4u);
		LookupTable lookupTable(rectifiedFrameWidth, rectifiedFrameHeight, binsX, binsY);

		if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
		{
			arbitraryRectangleObjectIFLookupTable<true>(camera, flippedCamera_T_world, rectangle0, rectangle1, rectangle2, rectangle3, lookupTable);
		}
		else
		{
			arbitraryRectangleObjectIFLookupTable<false>(camera, flippedCamera_T_world, rectangle0, rectangle1, rectangle2, rectangle3, lookupTable);
		}

		CV::FrameInterpolatorBilinear::lookup<uint8_t, tChannels>(cameraFrame, camera.width(), camera.height(), lookupTable, false, outsideFrameColor ? outsideFrameColor : zeroValue, rectifiedFrame, cameraFramePaddingElements, rectifiedFramePaddingElements, worker);
	}
}

template <unsigned int tChannels>
inline void FrameRectification::triangleObjectIF8BitPerChannel(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const FrameType::PixelOrigin pixelOrigin, const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Triangle2& triangle2, const Triangle3& triangle3, uint8_t* targetFrame, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, Worker* worker, const uint8_t* outsideFrameColor, const unsigned int approximationBinSize)
{
	static_assert(tChannels > 0u, "Invalid frame channel number!");

	const uint8_t zeroValue[tChannels] = {0x00};

	if (approximationBinSize == 0u)
	{
		if (worker)
		{
			if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
			{
				worker->executeFunction(Worker::Function::createStatic(triangleObjectIF8BitPerChannelSubset<tChannels, true>, cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &triangle2, &triangle3, targetFrame, targetWidth, targetHeight, targetFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, 0u), 0u, targetHeight, 11u, 12u, 20u);
			}
			else
			{
				worker->executeFunction(Worker::Function::createStatic(triangleObjectIF8BitPerChannelSubset<tChannels, false>, cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &triangle2, &triangle3, targetFrame, targetWidth, targetHeight, targetFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, 0u), 0u, targetHeight, 11u, 12u, 20u);
			}
		}
		else
		{
			if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
			{
				triangleObjectIF8BitPerChannelSubset<tChannels, true>(cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &triangle2, &triangle3, targetFrame, targetWidth, targetHeight, targetFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, targetHeight);
			}
			else
			{
				triangleObjectIF8BitPerChannelSubset<tChannels, false>(cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &triangle2, &triangle3, targetFrame, targetWidth, targetHeight, targetFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, targetHeight);
			}
		}
	}
	else
	{
		const unsigned int binsX = minmax(1u, targetWidth / approximationBinSize, targetWidth / 4u);
		const unsigned int binsY = minmax(1u, targetHeight / approximationBinSize, targetHeight / 4u);
		LookupTable lookupTable(targetWidth, targetHeight, binsX, binsY);

		if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
		{
			triangleObjectIFLookupTable<true>(camera, flippedCamera_T_world, triangle2, triangle3, lookupTable);
		}
		else
		{
			triangleObjectIFLookupTable<false>(camera, flippedCamera_T_world, triangle2, triangle3, lookupTable);
		}

		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(triangleObjectLookup8BitPerChannelSubset<tChannels>, cameraFrame, camera.width(), camera.height(), cameraFramePaddingElements, (const LookupTable*)(&lookupTable), &triangle2, targetFrame, targetFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, 0u), 0u, targetHeight, 9u, 10u, 20u);
		}
		else
		{
			triangleObjectLookup8BitPerChannelSubset<tChannels>(cameraFrame, camera.width(), camera.height(), cameraFramePaddingElements, &lookupTable, &triangle2, targetFrame, targetFramePaddingElements, outsideFrameColor ? outsideFrameColor : zeroValue, 0u, targetHeight);
		}
	}
}

template <unsigned int tChannels>
inline void FrameRectification::planarRectangleObjectMaskIF8BitPerChannel(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const FrameType::PixelOrigin pixelOrigin, const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangleOrigin, const Vector3& rectangleHorizontal, const Vector3& rectangleVertical, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, Worker* worker, const uint8_t maskValue, const unsigned int approximationBinSize)
{
	static_assert(tChannels > 0u, "Invalid frame channel number!");

	if (approximationBinSize == 0u)
	{
		if (worker)
		{
			if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
			{
				worker->executeFunction(Worker::Function::createStatic(planarRectangleObjectMaskIF8BitPerChannelSubset<tChannels, true>, cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &rectangleOrigin, &rectangleHorizontal, &rectangleVertical, targetFrame, targetMask, targetWidth, targetHeight, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, 0u), 0u, targetHeight, 14u, 15u, 20u);
			}
			else
			{
				worker->executeFunction(Worker::Function::createStatic(planarRectangleObjectMaskIF8BitPerChannelSubset<tChannels, false>, cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &rectangleOrigin, &rectangleHorizontal, &rectangleVertical, targetFrame, targetMask, targetWidth, targetHeight, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, 0u), 0u, targetHeight, 14u, 15u, 20u);
			}
		}
		else
		{
			if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
			{
				planarRectangleObjectMaskIF8BitPerChannelSubset<tChannels, true>(cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &rectangleOrigin, &rectangleHorizontal, &rectangleVertical, targetFrame, targetMask, targetWidth, targetHeight, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, targetHeight);
			}
			else
			{
				planarRectangleObjectMaskIF8BitPerChannelSubset<tChannels, false>(cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &rectangleOrigin, &rectangleHorizontal, &rectangleVertical, targetFrame, targetMask, targetWidth, targetHeight, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, targetHeight);
			}
		}
	}
	else
	{
		const unsigned int binsX = minmax(1u, targetWidth / approximationBinSize, targetWidth / 4u);
		const unsigned int binsY = minmax(1u, targetHeight / approximationBinSize, targetHeight / 4u);
		LookupTable lookupTable(targetWidth, targetHeight, binsX, binsY);

		if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
		{
			planarRectangleObjectIFLookupTable<true>(camera, flippedCamera_T_world, rectangleOrigin, rectangleHorizontal, rectangleVertical, lookupTable);
		}
		else
		{
			planarRectangleObjectIFLookupTable<false>(camera, flippedCamera_T_world, rectangleOrigin, rectangleHorizontal, rectangleVertical, lookupTable);
		}

		CV::FrameInterpolatorBilinear::lookupMask8BitPerChannel<tChannels>(cameraFrame, camera.width(), camera.height(), lookupTable, false, targetFrame, targetMask, cameraFramePaddingElements, targetFramePaddingElements, targetMaskPaddingElements, worker, maskValue);
	}
}

template <unsigned int tChannels>
inline void FrameRectification::arbitraryRectangleObjectMaskIF8BitPerChannel(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const FrameType::PixelOrigin pixelOrigin, const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangle0, const Vector3& rectangle1, const Vector3& rectangle2, const Vector3& rectangle3, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, Worker* worker, const uint8_t maskValue, const unsigned int approximationBinSize)
{
	static_assert(tChannels > 0u, "Invalid frame channel number!");

	if (approximationBinSize == 0u)
	{
		const Vector3 rectangle[4] = {rectangle0, rectangle1, rectangle2, rectangle3};

		if (worker)
		{
			if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
			{
				worker->executeFunction(Worker::Function::createStatic(arbitraryRectangleObjectMaskIF8BitPerChannelSubset<tChannels, true>, cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, rectangle, targetFrame, targetMask, targetWidth, targetHeight, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, 0u), 0u, targetHeight, 12u, 13u, 20u);
			}
			else
			{
				worker->executeFunction(Worker::Function::createStatic(arbitraryRectangleObjectMaskIF8BitPerChannelSubset<tChannels, false>, cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, rectangle, targetFrame, targetMask, targetWidth, targetHeight, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, 0u), 0u, targetHeight, 12u, 13u, 20u);
			}
		}
		else
		{
			if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
			{
				arbitraryRectangleObjectMaskIF8BitPerChannelSubset<tChannels, true>(cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, rectangle, targetFrame, targetMask, targetWidth, targetHeight, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, targetHeight);
			}
			else
			{
				arbitraryRectangleObjectMaskIF8BitPerChannelSubset<tChannels, false>(cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, rectangle, targetFrame, targetMask, targetWidth, targetHeight, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, targetHeight);
			}
		}
	}
	else
	{
		const unsigned int binsX = minmax(1u, targetWidth / approximationBinSize, targetWidth / 4u);
		const unsigned int binsY = minmax(1u, targetHeight / approximationBinSize, targetHeight / 4u);
		LookupTable lookupTable(targetWidth, targetHeight, binsX, binsY);

		if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
		{
			arbitraryRectangleObjectIFLookupTable<true>(camera, flippedCamera_T_world, rectangle0, rectangle1, rectangle2, rectangle3, lookupTable);
		}
		else
		{
			arbitraryRectangleObjectIFLookupTable<false>(camera, flippedCamera_T_world, rectangle0, rectangle1, rectangle2, rectangle3, lookupTable);
		}

		CV::FrameInterpolatorBilinear::lookupMask8BitPerChannel<tChannels>(cameraFrame, camera.width(), camera.height(), lookupTable, false, targetFrame, targetMask, cameraFramePaddingElements, targetFramePaddingElements, targetMaskPaddingElements, worker, maskValue);
	}
}

template <unsigned int tChannels>
inline void FrameRectification::triangleObjectMaskIF8BitPerChannel(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const FrameType::PixelOrigin pixelOrigin, const AnyCamera& camera, const HomogenousMatrix4& flippedCamera_T_world, const Triangle2& triangle2, const Triangle3& triangle3, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, Worker* worker, const uint8_t maskValue, const unsigned int approximationBinSize)
{
	static_assert(tChannels > 0u, "Invalid frame channel number!");

	if (approximationBinSize == 0u)
	{
		if (worker)
		{
			if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
			{
				worker->executeFunction(Worker::Function::createStatic(triangleObjectMaskIF8BitPerChannelSubset<tChannels, true>, cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &triangle2, &triangle3, targetFrame, targetMask, targetWidth, targetHeight, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, 0u), 0u, targetHeight, 13u, 14u, 20u);
			}
			else
			{
				worker->executeFunction(Worker::Function::createStatic(triangleObjectMaskIF8BitPerChannelSubset<tChannels, false>, cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &triangle2, &triangle3, targetFrame, targetMask, targetWidth, targetHeight, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, 0u), 0u, targetHeight, 13u, 14u, 20u);
			}
		}
		else
		{
			if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
			{
				triangleObjectMaskIF8BitPerChannelSubset<tChannels, true>(cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &triangle2, &triangle3, targetFrame, targetMask, targetWidth, targetHeight, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, targetHeight);
			}
			else
			{
				triangleObjectMaskIF8BitPerChannelSubset<tChannels, false>(cameraFrame, cameraFramePaddingElements, &camera, &flippedCamera_T_world, &triangle2, &triangle3, targetFrame, targetMask, targetWidth, targetHeight, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, targetHeight);
			}
		}
	}
	else
	{
		const unsigned int binsX = minmax(1u, targetWidth / approximationBinSize, targetWidth / 4u);
		const unsigned int binsY = minmax(1u, targetHeight / approximationBinSize, targetHeight / 4u);
		LookupTable lookupTable(targetWidth, targetHeight, binsX, binsY);

		if (pixelOrigin == FrameType::ORIGIN_UPPER_LEFT)
		{
			triangleObjectIFLookupTable<true>(camera, flippedCamera_T_world, triangle2, triangle3, lookupTable);
		}
		else
		{
			triangleObjectIFLookupTable<false>(camera, flippedCamera_T_world, triangle2, triangle3, lookupTable);
		}

		if (worker)
		{
			worker->executeFunction(Worker::Function::createStatic(triangleObjectMaskLookup8BitPerChannelSubset<tChannels>, cameraFrame, camera.width(), camera.height(), cameraFramePaddingElements, (const LookupTable*)&lookupTable, &triangle2, targetFrame, targetMask, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, 0u), 0u, targetHeight, 11u, 12u, 20u);
		}
		else
		{
			triangleObjectMaskLookup8BitPerChannelSubset<tChannels>(cameraFrame, camera.width(), camera.height(), cameraFramePaddingElements, (const LookupTable*)(&lookupTable), &triangle2, targetFrame, targetMask, targetFramePaddingElements, targetMaskPaddingElements, maskValue, 0u, targetHeight);
		}
	}
}

template <bool tPixelOriginUpperLeft>
void FrameRectification::planarRectangleObjectIFLookupTable(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangleOrigin, const Vector3& rectangleHorizontal, const Vector3& rectangleVertical, LookupTable& lookupTable)
{
	ocean_assert(anyCamera.isValid() && flippedCamera_T_world.isValid());

	const Scalar frameHeight1 = Scalar(anyCamera.height() - 1u);

	const Scalar invRectifiedWidth = Scalar(1) / Scalar(lookupTable.sizeX());
	const Scalar invRectifiedHeight = Scalar(1) / Scalar(lookupTable.sizeY());

	Vectors3 objectPoints(lookupTable.binsX() + 1u);
	Vectors2 imagePoints(lookupTable.binsX() + 1u);

	for (unsigned int yBin = 0u; yBin <= lookupTable.binsY(); ++yBin)
	{
		const Scalar y = lookupTable.binTopLeftCornerPositionY(yBin);
		const Scalar yCorrected = tPixelOriginUpperLeft ? y : (Scalar(lookupTable.sizeY() - 1) - Scalar(y));

		for (unsigned int xBin = 0u; xBin <= lookupTable.binsX(); ++xBin)
		{
			const Scalar x = lookupTable.binTopLeftCornerPositionX(xBin);

			objectPoints[xBin] = Vector3(rectangleOrigin + rectangleHorizontal * (Scalar(x) * invRectifiedWidth) + rectangleVertical * (yCorrected * invRectifiedHeight));
		}

		anyCamera.projectToImageIF(flippedCamera_T_world, objectPoints.data(), objectPoints.size(), imagePoints.data());

		for (unsigned int xBin = 0u; xBin <= lookupTable.binsX(); ++xBin)
		{
			const Vector2& imagePoint = imagePoints[xBin];

			const Vector2 correctedImagePoint(imagePoint.x(), tPixelOriginUpperLeft ? imagePoint.y() : (frameHeight1 - imagePoint.y()));

			lookupTable.setBinTopLeftCornerValue(xBin, yBin, correctedImagePoint);
		}
	}
}

template <bool tPixelOriginUpperLeft>
void FrameRectification::arbitraryRectangleObjectIFLookupTable(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world, const Vector3& rectangle0, const Vector3& rectangle1, const Vector3& rectangle2, const Vector3& rectangle3, LookupTable& lookupTable)
{
	ocean_assert(anyCamera.isValid() && flippedCamera_T_world.isValid());

	const Scalar frameHeight1 = Scalar(anyCamera.height() - 1u);

	const Scalar invRectifiedWidth = Scalar(1) / Scalar(lookupTable.sizeX());
	const Scalar invRectifiedHeight = Scalar(1) / Scalar(lookupTable.sizeY());

	for (unsigned int yBin = 0u; yBin <= lookupTable.binsY(); ++yBin)
	{
		const Scalar y = lookupTable.binTopLeftCornerPositionY(yBin);
		const Scalar yCorrected = tPixelOriginUpperLeft ? y : (Scalar(lookupTable.sizeY() - 1) - Scalar(y));

		const Vector3 left(rectangle0 + (rectangle1 - rectangle0) * (Scalar(yCorrected) * invRectifiedHeight));
		const Vector3 right(rectangle3 + (rectangle2 - rectangle3) * (Scalar(yCorrected) * invRectifiedHeight));

		for (unsigned int xBin = 0u; xBin <= lookupTable.binsX(); ++xBin)
		{
			const Scalar x = lookupTable.binTopLeftCornerPositionX(xBin);

			const Vector3 objectPoint(left + (right - left) * (Scalar(x) * invRectifiedWidth));
			const Vector2 imagePoint(anyCamera.projectToImageIF(flippedCamera_T_world, objectPoint));

			const Vector2 correctedImagePoint(imagePoint.x(), tPixelOriginUpperLeft ? imagePoint.y() : (frameHeight1 - imagePoint.y()));

			lookupTable.setBinTopLeftCornerValue(xBin, yBin, correctedImagePoint);
		}
	}
}

template <bool tPixelOriginUpperLeft>
void FrameRectification::triangleObjectIFLookupTable(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_world, const Triangle2& triangle2, const Triangle3& triangle3, LookupTable& lookupTable)
{
	ocean_assert(anyCamera.isValid() && flippedCamera_T_world.isValid());

	const Scalar frameHeight1 = Scalar(anyCamera.height() - 1u);

	for (unsigned int yBin = 0u; yBin <= lookupTable.binsY(); ++yBin)
	{
		const Scalar y = lookupTable.binTopLeftCornerPositionY(yBin);
		const Scalar yCorrected = tPixelOriginUpperLeft ? y : (Scalar(lookupTable.sizeY() - 1) - Scalar(y));

		for (unsigned int xBin = 0u; xBin <= lookupTable.binsX(); ++xBin)
		{
			const Scalar x = lookupTable.binTopLeftCornerPositionX(xBin);

			const Vector3 barycentricTargetPosition(triangle2.cartesian2barycentric(Vector2(x, yCorrected)));

			const Vector3 objectPoint(triangle3.barycentric2cartesian(barycentricTargetPosition));
			const Vector2 imagePoint(anyCamera.projectToImageIF(flippedCamera_T_world, objectPoint));

			const Vector2 correctedImagePoint(imagePoint.x(), tPixelOriginUpperLeft ? imagePoint.y() : (frameHeight1 - imagePoint.y()));

			lookupTable.setBinTopLeftCornerValue(xBin, yBin, correctedImagePoint);
		}
	}
}

template <unsigned int tChannels, bool tPixelOriginUpperLeft>
void FrameRectification::planarRectangleObjectIF8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_world, const Vector3* rectangleOrigin, const Vector3* rectangleHorizontal, const Vector3* rectangleVertical, uint8_t* rectifiedFrame, const unsigned int rectifiedFrameWidth, const unsigned int rectifiedFrameHeight, const unsigned int rectifiedFramePaddingElements, const uint8_t* outsideFrameColor, const unsigned int firstRectifiedFrameRow, const unsigned int numberRectifiedFrameRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(cameraFrame != nullptr && rectifiedFrame != nullptr && rectangleOrigin != nullptr && rectangleHorizontal != nullptr && rectangleVertical != nullptr);
	ocean_assert(camera != nullptr && camera->isValid() && flippedCamera_T_world != nullptr && flippedCamera_T_world->isValid());
	ocean_assert(rectifiedFrameWidth >= 1u && rectifiedFrameHeight >= 1u);

	ocean_assert(firstRectifiedFrameRow + numberRectifiedFrameRows <= rectifiedFrameHeight);

	const Scalar frameWidth1 = Scalar(camera->width() - 1u);
	const Scalar frameHeight1 = Scalar(camera->height() - 1u);

	const Scalar invRectifiedFrameWidth = Scalar(1) / Scalar(rectifiedFrameWidth);
	const Scalar invRectifiedFrameHeight = Scalar(1) / Scalar(rectifiedFrameHeight);

	const unsigned int rectifiedFrameStrideElements = rectifiedFrameWidth * tChannels + rectifiedFramePaddingElements;

	uint8_t* rectifiedPixel = rectifiedFrame + firstRectifiedFrameRow * rectifiedFrameStrideElements;

	for (unsigned int y = firstRectifiedFrameRow; y < firstRectifiedFrameRow + numberRectifiedFrameRows; ++y)
	{
		const Scalar yCorrected = tPixelOriginUpperLeft ? Scalar(y) : (Scalar(rectifiedFrameHeight - 1u) - Scalar(y));

		for (unsigned int x = 0u; x < rectifiedFrameWidth; ++x)
		{
			const Vector3 objectPoint(*rectangleOrigin + *rectangleHorizontal * (Scalar(x) * invRectifiedFrameWidth) + *rectangleVertical * (yCorrected * invRectifiedFrameHeight));
			const Vector2 imagePoint(camera->projectToImageIF(*flippedCamera_T_world, objectPoint));

			const Vector2 correctedImagePoint(imagePoint.x(), tPixelOriginUpperLeft ? imagePoint.y() : (frameHeight1 - imagePoint.y()));

			if (correctedImagePoint.x() >= 0 && correctedImagePoint.y() >= 0 && correctedImagePoint.x() <= frameWidth1 && correctedImagePoint.y() <= frameHeight1)
			{
				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(cameraFrame, camera->width(), camera->height(), cameraFramePaddingElements, correctedImagePoint, rectifiedPixel);
			}
			else
			{
				memcpy(rectifiedPixel, outsideFrameColor, sizeof(uint8_t) * tChannels);
			}

			rectifiedPixel += tChannels;
		}

		rectifiedPixel += rectifiedFramePaddingElements;
	}
}

template <unsigned int tChannels, bool tPixelOriginUpperLeft>
void FrameRectification::arbitraryRectangleObjectIF8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_world, const Vector3* rectangle, uint8_t* rectifiedFrame, const unsigned int rectifiedFrameWidth, const unsigned int rectifiedFrameHeight, const unsigned int rectifiedFramePaddingElements, const uint8_t* outsideFrameColor, const unsigned int firstRectifiedFrameRow, const unsigned int numberRectifiedFrameRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(cameraFrame != nullptr && rectifiedFrame != nullptr && rectangle != nullptr);
	ocean_assert(camera != nullptr && camera->isValid() && flippedCamera_T_world != nullptr && flippedCamera_T_world->isValid());
	ocean_assert(rectifiedFrameWidth >= 1u && rectifiedFrameHeight >= 1u);

	ocean_assert(firstRectifiedFrameRow + numberRectifiedFrameRows <= rectifiedFrameHeight);

	const Scalar frameWidth1 = Scalar(camera->width() - 1u);
	const Scalar frameHeight1 = Scalar(camera->height() - 1u);

	const Scalar invRectifiedFrameWidth = Scalar(1) / Scalar(rectifiedFrameWidth);
	const Scalar invRectifiedFrameHeight = Scalar(1) / Scalar(rectifiedFrameHeight);

	const unsigned int rectifiedFrameStrideElements = rectifiedFrameWidth * tChannels + rectifiedFramePaddingElements;

	uint8_t* rectifiedPixel = rectifiedFrame + firstRectifiedFrameRow * rectifiedFrameStrideElements;

	const Vector3 direction10 = rectangle[1] - rectangle[0];
	const Vector3 direction23 = rectangle[2] - rectangle[3];

	for (unsigned int y = firstRectifiedFrameRow; y < firstRectifiedFrameRow + numberRectifiedFrameRows; ++y)
	{
		const Scalar yCorrected = tPixelOriginUpperLeft ? Scalar(y) : (Scalar(rectifiedFrameHeight - 1u) - Scalar(y));

		const Vector3 left(rectangle[0] + direction10 * (Scalar(yCorrected) * invRectifiedFrameHeight));
		const Vector3 right(rectangle[3] + direction23 * (Scalar(yCorrected) * invRectifiedFrameHeight));

		for (unsigned int x = 0u; x < rectifiedFrameWidth; ++x)
		{
			const Vector3 objectPoint(left + (right - left) * (Scalar(x) * invRectifiedFrameWidth));
			const Vector2 imagePoint(camera->projectToImageIF(*flippedCamera_T_world, objectPoint));

			const Vector2 correctedImagePoint(imagePoint.x(), tPixelOriginUpperLeft ? imagePoint.y() : (frameHeight1 - imagePoint.y()));

			if (correctedImagePoint.x() >= 0 && correctedImagePoint.y() >= 0 && correctedImagePoint.x() <= frameWidth1 && correctedImagePoint.y() <= frameHeight1)
			{
				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(cameraFrame, camera->width(), camera->height(), cameraFramePaddingElements, correctedImagePoint, rectifiedPixel);
			}
			else
			{
				memcpy(rectifiedPixel, outsideFrameColor, sizeof(uint8_t) * tChannels);
			}

			rectifiedPixel += tChannels;
		}

		rectifiedPixel += rectifiedFramePaddingElements;
	}
}

template <unsigned int tChannels, bool tPixelOriginUpperLeft>
void FrameRectification::triangleObjectIF8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_world, const Triangle2* triangle2, const Triangle3* triangle3, uint8_t* targetFrame, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const uint8_t* outsideFrameColor, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(cameraFrame != nullptr && targetFrame != nullptr && triangle2 != nullptr && triangle3 != nullptr);
	ocean_assert(camera != nullptr && camera->isValid() && flippedCamera_T_world != nullptr && flippedCamera_T_world->isValid());
	ocean_assert(targetWidth >= 0u &&targetHeight >= 0u);

	ocean_assert(firstTargetRow + numberTargetRows <= targetHeight);

	const Scalar frameWidth1 = Scalar(camera->width() - 1u);
	const Scalar frameHeight1 = Scalar(camera->height() - 1u);

	const unsigned int targetStrideElements = targetWidth * tChannels + targetFramePaddingElements;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		const Scalar yCorrected = tPixelOriginUpperLeft ? Scalar(y) : (Scalar(targetHeight - 1u) - Scalar(y));

		uint8_t* targetPixel = targetFrame + y * targetStrideElements;

		for (unsigned int x = 0u; x < targetWidth; ++x)
		{
			const Vector3 barycentricTargetPosition(triangle2->cartesian2barycentric(Vector2(Scalar(x), yCorrected)));

			if (triangle2->isBarycentricInside(barycentricTargetPosition))
			{
				const Vector3 objectPoint(triangle3->barycentric2cartesian(barycentricTargetPosition));
				const Vector2 imagePoint(camera->projectToImageIF(*flippedCamera_T_world, objectPoint));

				const Vector2 correctedImagePoint(imagePoint.x(), tPixelOriginUpperLeft ? imagePoint.y() : (frameHeight1 - imagePoint.y()));

				if (correctedImagePoint.x() >= 0 && correctedImagePoint.y() >= 0 && correctedImagePoint.x() <= frameWidth1 && correctedImagePoint.y() <= frameHeight1)
				{
					CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(cameraFrame, camera->width(), camera->height(), cameraFramePaddingElements, correctedImagePoint, targetPixel);
				}
				else
				{
					*((PixelType*)(targetPixel)) = *((const PixelType*)(outsideFrameColor));
				}
			}

			targetPixel += tChannels;
		}
	}
}

template <unsigned int tChannels>
void FrameRectification::triangleObjectLookup8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFrameWidth, const unsigned int cameraFrameHeight, const unsigned int cameraFramePaddingElements, const LookupTable* lookupTable, const Triangle2* triangle2, uint8_t* targetFrame, const unsigned int targetFramePaddingElements, const uint8_t* outsideFrameColor, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(cameraFrame != nullptr && targetFrame != nullptr && triangle2 != nullptr && lookupTable && !lookupTable->isEmpty());
	ocean_assert(cameraFrameWidth >= 1u && cameraFrameHeight >= 1u);

	ocean_assert(firstTargetRow + numberTargetRows <= (unsigned int)(lookupTable->sizeY()));

	const Scalar cameraFrameWidth1 = Scalar(cameraFrameWidth - 1u);
	const Scalar cameraFrameHeight1 = Scalar(cameraFrameHeight - 1u);

	const unsigned int targetFrameWidth = (unsigned int)(lookupTable->sizeX());

	const unsigned int targetFrameStrideElements = targetFrameWidth * tChannels + targetFramePaddingElements;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		uint8_t* targetPixel = targetFrame + y * targetFrameStrideElements;

		for (unsigned int x = 0u; x < targetFrameWidth; ++x)
		{
			if (triangle2->isInside(Vector2(Scalar(x), Scalar(y))))
			{
				const Vector2 inputPosition(lookupTable->bilinearValue(Scalar(x), Scalar(y)));

				if (inputPosition.x() >= 0 && inputPosition.y() >= 0 && inputPosition.x() <= cameraFrameWidth1 && inputPosition.y() <= cameraFrameHeight1)
				{
					CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(cameraFrame, cameraFrameWidth, cameraFrameHeight, cameraFramePaddingElements, inputPosition, targetPixel);
				}
				else
				{
					*((PixelType*)(targetPixel)) = *((PixelType*)(outsideFrameColor));
				}
			}

			targetPixel += tChannels;
		}
	}
}

template <unsigned int tChannels, bool tPixelOriginUpperLeft>
void FrameRectification::planarRectangleObjectMaskIF8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_world, const Vector3* rectangleOrigin, const Vector3* rectangleHorizontal, const Vector3* rectangleVertical, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, const uint8_t maskValue, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(cameraFrame != nullptr);
	ocean_assert(camera != nullptr && camera->isValid());
	ocean_assert(flippedCamera_T_world != nullptr && flippedCamera_T_world->isValid());
	ocean_assert(rectangleOrigin != nullptr && rectangleHorizontal != nullptr && rectangleVertical != nullptr);
	ocean_assert(targetFrame != nullptr && targetMask != nullptr);
	ocean_assert(targetWidth > 0u && targetHeight > 0u);

	ocean_assert(firstTargetRow + numberTargetRows <= targetHeight);

	const unsigned int targetFrameStrideElements = targetWidth * tChannels + targetFramePaddingElements;
	const unsigned int targetMaskStrideElements = targetWidth + targetMaskPaddingElements;

	const Scalar frameWidth1 = Scalar(camera->width() - 1u);
	const Scalar frameHeight1 = Scalar(camera->height() - 1u);

	const Scalar invTargetWidth = Scalar(1) / Scalar(targetWidth);
	const Scalar invTargetHeight = Scalar(1) / Scalar(targetHeight);

	uint8_t* targetPixel = targetFrame + firstTargetRow * targetFrameStrideElements;

	targetMask += firstTargetRow * targetMaskStrideElements;

	Vectors3 objectPoints(targetWidth);
	Vectors2 imagePoints(targetWidth);

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		const Scalar yCorrected = tPixelOriginUpperLeft ? Scalar(y) : (Scalar(targetHeight - 1u) - Scalar(y));

		for (unsigned int x = 0u; x < targetWidth; ++x)
		{
			objectPoints[x] = Vector3(*rectangleOrigin + *rectangleHorizontal * (Scalar(x) * invTargetWidth) + *rectangleVertical * (Scalar(yCorrected) * invTargetHeight));
		}

		camera->projectToImageIF(*flippedCamera_T_world, objectPoints.data(), objectPoints.size(), imagePoints.data());

		for (unsigned int x = 0u; x < targetWidth; ++x)
		{
			const Vector2& imagePoint = imagePoints[x];

			const Vector2 correctedImagePoint(imagePoint.x(), tPixelOriginUpperLeft ? imagePoint.y() : (frameHeight1 - imagePoint.y()));

			if (correctedImagePoint.x() >= 0 && correctedImagePoint.y() >= 0 && correctedImagePoint.x() <= frameWidth1 && correctedImagePoint.y() <= frameHeight1)
			{
				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(cameraFrame, camera->width(), camera->height(), cameraFramePaddingElements, correctedImagePoint, targetPixel);
				*targetMask = maskValue;
			}
			else
			{
				*targetMask = 0xFF - maskValue;
			}

			targetPixel += tChannels;
			++targetMask;
		}

		targetPixel += targetFramePaddingElements;
		targetMask += targetMaskPaddingElements;
	}
}

template <unsigned int tChannels, bool tPixelOriginUpperLeft>
void FrameRectification::arbitraryRectangleObjectMaskIF8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_world, const Vector3* rectangle, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, const uint8_t maskValue, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(cameraFrame != nullptr);
	ocean_assert(camera != nullptr && camera->isValid());
	ocean_assert(flippedCamera_T_world != nullptr && flippedCamera_T_world->isValid());
	ocean_assert(rectangle != nullptr);
	ocean_assert(targetFrame != nullptr && targetMask != nullptr);
	ocean_assert(targetWidth > 0u && targetHeight > 0u);

	ocean_assert(firstTargetRow + numberTargetRows <= targetHeight);

	const unsigned int targetFrameStrideElements = targetWidth * tChannels + targetFramePaddingElements;
	const unsigned int targetMaskStrideElements = targetWidth + targetMaskPaddingElements;

	const Scalar frameWidth1 = Scalar(camera->width() - 1u);
	const Scalar frameHeight1 = Scalar(camera->height() - 1u);

	const Scalar invTargetWidth = Scalar(1) / Scalar(targetWidth);
	const Scalar invTargetHeight = Scalar(1) / Scalar(targetHeight);

	uint8_t* targetPixel = targetFrame + firstTargetRow * targetFrameStrideElements;

	targetMask += firstTargetRow * targetMaskStrideElements;

	Vectors3 objectPoints(targetWidth);
	Vectors2 imagePoints(targetWidth);

	const Vector3 direction10 = rectangle[1] - rectangle[0];
	const Vector3 direction23 = rectangle[2] - rectangle[3];

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		const Scalar yCorrected = tPixelOriginUpperLeft ? Scalar(y) : (Scalar(targetHeight - 1u) - Scalar(y));

		const Vector3 left(rectangle[0] + direction10 * (Scalar(yCorrected) * invTargetHeight));
		const Vector3 right(rectangle[3] + direction23 * (Scalar(yCorrected) * invTargetHeight));

		for (unsigned int x = 0u; x < targetWidth; ++x)
		{
			objectPoints[x] = left + (right - left) * (Scalar(x) * invTargetWidth);
		}

		camera->projectToImageIF(*flippedCamera_T_world, objectPoints.data(), objectPoints.size(), imagePoints.data());

		for (unsigned int x = 0u; x < targetWidth; ++x)
		{
			const Vector2& imagePoint = imagePoints[x];

			const Vector2 correctedImagePoint(imagePoint.x(), tPixelOriginUpperLeft ? imagePoint.y() : (frameHeight1 - imagePoint.y()));

			if (correctedImagePoint.x() >= 0 && correctedImagePoint.y() >= 0 && correctedImagePoint.x() <= frameWidth1 && correctedImagePoint.y() <= frameHeight1)
			{
				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(cameraFrame, camera->width(), camera->height(), cameraFramePaddingElements, correctedImagePoint, targetPixel);
				*targetMask = maskValue;
			}
			else
			{
				*targetMask = 0xFF - maskValue;
			}

			targetPixel += tChannels;
			++targetMask;
		}

		targetPixel += targetFramePaddingElements;
		targetMask += targetMaskPaddingElements;
	}
}

template <unsigned int tChannels, bool tPixelOriginUpperLeft>
void FrameRectification::triangleObjectMaskIF8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const AnyCamera* camera, const HomogenousMatrix4* flippedCamera_T_world, const Triangle2* triangle2, const Triangle3* triangle3, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetWidth, const unsigned int targetHeight, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, const uint8_t maskValue, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(cameraFrame != nullptr && camera != nullptr && camera->isValid());
	ocean_assert(flippedCamera_T_world != nullptr && flippedCamera_T_world->isValid());

	ocean_assert(triangle2 != nullptr && triangle2->isValid());
	ocean_assert(triangle3 != nullptr && triangle2->isValid());

	ocean_assert(targetFrame != nullptr && targetMask != nullptr);

	ocean_assert(targetWidth > 0u && targetHeight > 0u);
	ocean_assert(firstTargetRow + numberTargetRows <= targetHeight);

	const Scalar frameWidth1 = Scalar(camera->width() - 1u);
	const Scalar frameHeight1 = Scalar(camera->height() - 1u);

	const unsigned int targetFrameStrideElements = targetWidth * tChannels + targetFramePaddingElements;
	const unsigned int targetMaskStrideElements = targetWidth + targetMaskPaddingElements;

	uint8_t* targetPixel = targetFrame + firstTargetRow * targetFrameStrideElements;

	targetMask += firstTargetRow * targetMaskStrideElements;

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		const Scalar yCorrected = tPixelOriginUpperLeft ? Scalar(y) : (Scalar(targetHeight - 1u) - Scalar(y));

		for (unsigned int x = 0u; x < targetWidth; ++x)
		{
			const Vector3 barycentricTargetPosition(triangle2->cartesian2barycentric(Vector2(Scalar(x), yCorrected)));

			if (triangle2->isBarycentricInside(barycentricTargetPosition))
			{
				const Vector3 objectPoint(triangle3->barycentric2cartesian(barycentricTargetPosition));
				const Vector2 imagePoint(camera->projectToImageIF(*flippedCamera_T_world, objectPoint));

				const Vector2 correctedImagePoint(imagePoint.x(), tPixelOriginUpperLeft ? imagePoint.y() : (frameHeight1 - imagePoint.y()));

				if (correctedImagePoint.x() >= 0 && correctedImagePoint.y() >= 0 && correctedImagePoint.x() <= frameWidth1 && correctedImagePoint.y() <= frameHeight1)
				{
					CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(cameraFrame, camera->width(), camera->height(), cameraFramePaddingElements, correctedImagePoint, targetPixel);
					*targetMask = maskValue;
				}
				else
				{
					*targetMask = 0xFF - maskValue;
				}
			}

			targetPixel += tChannels;
			++targetMask;
		}

		targetPixel += targetFramePaddingElements;
		targetMask += targetMaskPaddingElements;
	}
}

template <unsigned int tChannels>
void FrameRectification::triangleObjectMaskLookup8BitPerChannelSubset(const uint8_t* cameraFrame, const unsigned int cameraFrameWidth, const unsigned int cameraFrameHeight, const unsigned int cameraFramePaddingElements, const LookupTable* lookupTable, const Triangle2* triangle2, uint8_t* targetFrame, uint8_t* targetMask, const unsigned int targetFramePaddingElements, const unsigned int targetMaskPaddingElements, const uint8_t maskValue, const unsigned int firstTargetRow, const unsigned int numberTargetRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(cameraFrame != nullptr && cameraFrameWidth >= 1u && cameraFrameHeight >= 1u);
	ocean_assert(triangle2 != nullptr && lookupTable && !lookupTable->isEmpty());

	ocean_assert(targetFrame != nullptr && targetMask != nullptr);

	ocean_assert(firstTargetRow + numberTargetRows <= (unsigned int)(lookupTable->sizeY()));

	const unsigned int targetFrameStrideElements = (unsigned int)(lookupTable->sizeX()) * tChannels + targetFramePaddingElements;
	const unsigned int targetMaskStrideElements = (unsigned int)(lookupTable->sizeX()) + targetMaskPaddingElements;

	const Scalar cameraFrameWidth1 = Scalar(cameraFrameWidth - 1u);
	const Scalar cameraFrameHeight1 = Scalar(cameraFrameHeight - 1u);

	uint8_t* targetPixel = targetFrame + firstTargetRow * targetFrameStrideElements;

	targetMask += firstTargetRow * targetMaskStrideElements;

	for (unsigned int y = firstTargetRow; y < firstTargetRow + numberTargetRows; ++y)
	{
		for (unsigned int x = 0u; x < (unsigned int)(lookupTable->sizeX()); ++x)
		{
			if (triangle2->isInside(Vector2(Scalar(x), Scalar(y))))
			{
				const Vector2 inputPosition(lookupTable->bilinearValue(Scalar(x), Scalar(y)));

				if (inputPosition.x() >= 0 && inputPosition.y() >= 0 && inputPosition.x() <= cameraFrameWidth1 && inputPosition.y() <= cameraFrameHeight1)
				{
					CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, CV::PC_TOP_LEFT>(cameraFrame, cameraFrameWidth, cameraFrameHeight, cameraFramePaddingElements, inputPosition, targetPixel);
					*targetMask = maskValue;
				}
				else
				{
					*targetMask = 0xFFu - maskValue;
				}
			}

			targetPixel += tChannels;
			++targetMask;
		}

		targetPixel += targetFramePaddingElements;
		targetMask += targetMaskPaddingElements;
	}
}

}

}

}

#endif // META_OCEAN_CV_ADVANCED_FRAME_RECTIFICATION_H
