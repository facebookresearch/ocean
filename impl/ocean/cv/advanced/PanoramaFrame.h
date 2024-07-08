/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_ADVANCED_PANORAMA_FRAME_H
#define META_OCEAN_CV_ADVANCED_PANORAMA_FRAME_H

#include "ocean/cv/advanced/Advanced.h"
#include "ocean/cv/advanced/AdvancedFrameInterpolatorBilinear.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Worker.h"

#include "ocean/cv/FramePyramid.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Lookup2.h"
#include "ocean/math/Numeric.h"
#include "ocean/math/PinholeCamera.h"
#include "ocean/math/SquareMatrix3.h"

namespace Ocean
{

namespace CV
{

namespace Advanced
{

/**
 * This class implements a panorama frame with spherical projection model.
 * The resolution of the panorama frame is clamped by a maximal number of pixels.<br>
 * However, internally a sub-frame of the panorama frame is stored and iteratively extended if necessary.<br>
 * The resulting panorama frame is a combination of several camera frames which have been projected into the inner side of a sphere.<br>
 * The default camera orientation is looking into the negative z-axis and has the y-axis as up-vector.<br>
 * An horizontal orientation is a rotation around the y-axis.<br>
 * A frame with default orientation is mapped to the center position of the panorama frame.<br>
 * The maximal possible panorama frame covers an horizontal angle of [-180, 180) degrees (from left to right),<br>
 * and a vertical angle of [90, -90) degrees from top to bottom).<br>
 * @ingroup cvadvanced
 */
class OCEAN_CV_ADVANCED_EXPORT PanoramaFrame
{
	public:

		/**
		 * Definition of individual frame update modes.
		 */
		enum UpdateMode
		{
			/// Invalid update mode.
			UM_INVALID,
			/// Sets only pixels in the panorama frame which haven't been set before.
			UM_SET_NEW,
			/// Overwrites all pixels in the panorama frame.
			UM_SET_ALL,
			/// Updates all pixels in the panorama frame by averaging the current pixel value with the new pixel value.
			UM_AVERAGE_LOCAL,
			/// Updates all pixels in the panorama frame by averaging all pixel values.
			UM_AVERAGE_GLOBAL
		};

	protected:

		/**
		 * Definition of a lookup table for 2D vectors.
		 */
		typedef LookupCorner2<Vector2> LookupTable;

	public:

		/**
		 * Creates an invalid panorama frame instance.
		 */
		PanoramaFrame() = default;

		/**
		 * Creates a new panorama frame instance.
		 * @param width The width of the entire panorama frame (the maximal possible size) representing horizontal 360 degrees, in pixel with range [1, infinity)
		 * @param height The height of the entire panorama frame (the maximal possible size) representing vertical 180 degrees, in pixel with range [1, infinity)
		 * @param maskValue The mask value defining the 8 bit pixel value of valid pixels
		 * @param updateMode The update mode of this panorama frame
		 */
		inline PanoramaFrame(const unsigned int width, const unsigned int height, const uint8_t maskValue, const UpdateMode updateMode);

		/**
		 * Creates a new panorama frame instance by an already existing (entire) panorama frame.
		 * @param frame The already known (entire) panorama frame covering the visual content of the entire sphere, must be valid
		 * @param mask The mask corresponding to the given frame, with same frame dimension and pixel orientation, must be valid
		 * @param maskValue The mask value defining the 8 bit pixel value of valid pixels
		 * @param updateMode The update mode of this panorama frame
		 * @param worker Optional worker to distribute the computation, necessary for UM_AVERAGE_GLOBAL update mode only
		 */
		PanoramaFrame(const Frame& frame, const Frame& mask, const uint8_t maskValue, const UpdateMode updateMode, Worker* worker = nullptr);

		/**
		 * Creates a new panorama frame instance by an already existing (entire) panorama frame.
		 * @param frame The already known (entire) panorama frame covering the visual content of the entire sphere, will be moved, must be valid
		 * @param mask The mask corresponding to the given frame, with same frame dimension and pixel orientation, will be moved, must be valid
		 * @param maskValue The mask value defining the 8 bit pixel value of valid pixels
		 * @param updateMode The update mode of this panorama frame
		 * @param worker Optional worker to distribute the computation, necessary for UM_AVERAGE_GLOBAL update mode only
		 */
		PanoramaFrame(Frame&& frame, Frame&& mask, const uint8_t maskValue, const UpdateMode updateMode, Worker* worker = nullptr);

		/**
		 * Default destructor
		 */
		virtual ~PanoramaFrame() = default;

		/**
		 * Returns the mask value defining the 8 bit pixel value of valid pixels.
		 * @return The mask value of this panorama frame.
		 */
		inline uint8_t maskValue() const;

		/**
		 * Returns the update mode of this panorama frame.
		 * @return The update mode
		 */
		inline UpdateMode updateMode() const;

		/**
		 * Returns the current panorama sub-frame (not the maximal possible panorama frame).
		 * @return The current panorama frame
		 * @see frameTopLeft().
		 */
		inline const Frame& frame() const;

		/**
		 * Returns the mask of the current panorama frame.
		 * @return The panorama frame mask
		 */
		inline const Frame& mask() const;

		/**
		 * Returns the top left position of the current panorama sub-frame.
		 * @return the current top left position of the panorama sub-frame, with range [0, dimensionWidth())x[0, dimensionHeight())
		 */
		inline const PixelPosition& frameTopLeft() const;

		/**
		 * Returns the maximal possible width of the entire panorama frame representing horizontal 360 degrees, in pixel.
		 * @return The maximal width of the panorama frame (if the entire frame is covered with pixel information), with range [1, infinity)
		 */
		inline unsigned int dimensionWidth() const;

		/**
		 * Returns the maximal possible height of the entire panorama frame representing vertical 180 degrees, in pixel.
		 * @return The maximal height of the panorama frame (if the entire frame is covered with pixel information), with range [1, infinity)
		 */
		inline unsigned int dimensionHeight() const;

		/**
		 * Sets or resets the panorama sub-frame (not the maximal possible panorama frame).
		 * @param topLeft The top left position of the given panorama sub-frame, with range [0, dimensionWidth() - frame.width())x[0, dimensionHeight() - frame.height())
		 * @param frame The current panorama sub-frame to be set
		 * @param mask Optional mask defining valid and invalid pixels in the given frame, must have the same frame dimension as the given frame if valid, a default mask defines all pixels in the given panorama sub-frame as valid
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool setFrame(const PixelPosition& topLeft, const Frame& frame, const Frame& mask = Frame(), Worker* worker = nullptr);

		/**
		 * Adds a new camera frame to the panorama frame for which the orientation is known.
		 * @param pinholeCamera The pinhole camera profile of the given frame
		 * @param orientation The orientation of the given frame
		 * @param frame The frame to be added
		 * @param mask Optional mask frame defining valid and invalid pixels in the given frame
		 * @param approximationBinSize Optional width of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool addFrame(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, const Frame& frame, const Frame& mask, const unsigned int approximationBinSize = 20u, Worker* worker = nullptr);

		/**
		 * Extracts a camera frame from the panorama frame.
		 * @param pinholeCamera The pinhole camera profile to be used
		 * @param orientation The orientation of the camera
		 * @param frame The resulting camera frame, will receive the frame dimension as provided by the camera profile
		 * @param mask The resulting camera mask, will receive the frame dimension as provided by the camera profile
		 * @param approximationBinSize Optional width of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the extracted frame
		 * @return True, if succeeded
		 */
		inline bool extractFrame(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, Frame& frame, Frame& mask, const unsigned int approximationBinSize = 20u, Worker* worker = nullptr, const LookupTable* fineAdjustment = nullptr) const;

		/**
		 * Converts a given pixel position defined in the current camera frame into the pixel position defined in the entire (maximal possible) panorama frame.
		 * @param pinholeCamera The pinhole camera profile matching with the given pixel position
		 * @param orientation The orientation of the camera frame, must be valid
		 * @param cameraPixel The pixel position in the camera frame, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param referenceAngle The reference angle to which the resulting position will be adjusted (so that the position can be located outside the actual panorama frame), with range (-infinity, infinity)x(-infinity, infinity)
		 * @see cameraPixel2panoramaPixelStrict().
		 */
		inline Vector2 cameraPixel2panoramaPixel(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, const Vector2& cameraPixel, const Vector2& referenceAngle) const;

		/**
		 * Converts a given pixel position defined in the current camera frame into the pixel position defined in the entire (maximal possible) panorama frame.
		 * @param pinholeCamera The pinhole camera profile matching with the given pixel position
		 * @param orientation The orientation of the camera frame,
		 * @param cameraPixel The pixel position in the camera frame, with range [0, camera.width())x[0, camera.height())
		 * @see cameraPixel2panoramaPixel().
		 */
		inline Vector2 cameraPixel2panoramaPixelStrict(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, const Vector2& cameraPixel) const;

		/**
		 * Converts a given pixel position defined in the entire (maximal possible) panorama frame into the pixel position defined in a camera frame.
		 * @param pinholeCamera The pinhole camera profile matching with the resulting pixel position
		 * @param iOrientation The inverted orientation of the camera frame
		 * @param panoramaPixel The pixel position defined in the entire panorama frame, with range [0, dimensionWidth())x[0, dimensionHeight())
		 */
		inline Vector2 panoramaPixel2cameraPixel(const PinholeCamera& pinholeCamera, const SquareMatrix3& iOrientation, const Vector2& panoramaPixel) const;

		/**
		 * Converts an angle in the entire (maximal possible) panorama frame to the corresponding pixel position in the entire panorama frame.
		 * @param angle The horizontal and vertical angle to be converted, with range [-PI, PI)x[PI/2, -PI/2)
		 * @return The resulting pixel position, with range [0, dimensionWidth())[0, dimensionHeight())
		 */
		inline Vector2 angle2pixel(const Vector2& angle) const;

		/**
		 * Converts the pixel position in the entire (maximal possible) panorama frame to the corresponding angle in the entire panorama frame.
		 * @param position The position in the panorama frame, with range [0, dimensionWidth())x[0, dimensionHeight())
		 * @return The resulting angle, with range [-PI, PI)x(PI/2, -PI/2)
		 */
		inline Vector2 pixel2angle(const Vector2& position) const;

		/**
		 * Copies (interpolates) the entire area of a camera frame with specified camera orientation to a sub-frame of an entire panorama frame.
		 * @param pinholeCamera The pinhole camera profile of the resulting camera frame
		 * @param frame The camera frame to be copied
		 * @param mask Optional 8 bit mask frame specifying valid and invalid pixels, a default frame sets all pixels valid
		 * @param orientation The orientation of the given camera frame
		 * @param panoramaSubFrame The resulting sub-frame of the entire panorama frame
		 * @param panoramaSubMask The mask corresponding with the resulting panorama sub frame
		 * @param subFrameTopLeft The top left position of the resulting panorama frame, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param approximationBinSize Optional width of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the given camera frame
		 * @return True, if succeeded
		 */
		bool cameraFrame2panoramaSubFrame(const PinholeCamera& pinholeCamera, const Frame& frame, const Frame& mask, const SquareMatrix3& orientation, Frame& panoramaSubFrame, Frame& panoramaSubMask, PixelPositionI& subFrameTopLeft, const unsigned int approximationBinSize = 20u, Worker* worker = nullptr, const LookupTable* fineAdjustment = nullptr);

		/**
		 * Clears the panorama frame and allows to set a new first camera frame.
		 */
		virtual void clear();

		/**
		 * Returns whether this panorama frame holds valid configuration parameters and thus can be used.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Returns whether this panorama frame holds valid configuration parameters and thus can be used.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Checks whether two camera frames have a guaranteed intersection.
		 * This function uses the inner field of view (the smallest of either the horizontal or vertical field of view)
		 * and checks whether the angular distance between both camera's principal points is smaller than half of the sum of both field of views.
		 * @param pinholeCamera0 The pinhole camera profile of the first camera frame, must be valid
		 * @param orientation0 The orientation of the first camera frame
		 * @param pinholeCamera1 The pinhole camera profile of the second camera frame, must be valid
		 * @param orientation1 The orientation of the second camera frame
		 * @param overlappingPercent Optional resulting overlapping percentage, the larger the closer both frames are located together, with range [0, 1]
		 * @return True, if both camera frames have a guaranteed intersection
		 */
		static bool haveIntersectionByInnerFov(const PinholeCamera& pinholeCamera0, const SquareMatrix3& orientation0, const PinholeCamera& pinholeCamera1, const SquareMatrix3& orientation1, Scalar* overlappingPercent = nullptr);

		/**
		 * Checks whether two camera frames have a guaranteed intersection.
		 * This function shoots rays though the four corners of the camera frame, and through the four center points between neighboring corners.
		 * Thus, for each camera eight rays are given and composed into one viewing frustum.
		 * This function simply counts the number of rays intersecting the viewing frustum of the other camera frame.
		 * Additionally, the viewing frustum can be narrowed by the definition of a border factor to ensure that the intersection area has a specific size.
		 * @param pinholeCamera0 The pinhole camera profile of the first camera frame, must be valid
		 * @param orientation0 The orientation of the first camera frame
		 * @param pinholeCamera1 The pinhole camera profile of the second camera frame, must be valid
		 * @param orientation1 The orientation of the second camera frame
		 * @param borderFactor The border factor allowing to narrow the viewing frustums, a border factor of 0.1 narrows each frustum by 10% on each side, with range [0, 0.5)
		 * @param minimalIntersections The minimal number of rays that must intersect the other camera frustum to count both frames as overlapping, with range [1, 8]
		 * @return True, if the specified number of rays intersect with the other viewing frustum
		 */
		static bool haveIntersectionByRays(const PinholeCamera& pinholeCamera0, const SquareMatrix3& orientation0, const PinholeCamera& pinholeCamera1, const SquareMatrix3& orientation1, const Scalar borderFactor = Scalar(0.1), const unsigned int minimalIntersections = 2u);

		/**
		 * Approximate the area of the intersection of two frames
		 * @param pinholeCamera0 The pinhole camera profile of the first camera frame, must be valid
		 * @param orientation0 The orientation of the first camera frame
		 * @param pinholeCamera1 The pinhole camera profile of the second camera frame, must be valid
		 * @param orientation1 The orientation of the second camera frame
		 * @param intersectionRatio If specified, this is the ratio of the intersection area divided by the resolution (height x width) of camera0, otherwise this parameter will be ignored, range: [0, 1]
		 * @return The area of the intersection, range: [0, min(area(camera0), area(camera1))]
		 */
		static Scalar approximateIntersectionArea(const PinholeCamera& pinholeCamera0, const SquareMatrix3& orientation0, const PinholeCamera& pinholeCamera1, const SquareMatrix3& orientation1, Scalar* intersectionRatio = nullptr);

		/**
		 * Converts an angle in the entire (maximal possible) panorama frame to the corresponding pixel position in the entire panorama frame.
		 * @param angle The horizontal and vertical angle to be converted, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param panoramaDimensionWidth The width of the maximal possible panorama frame in pixel, [1, infinity)
		 * @param panoramaDimensionHeight THe height of the maximal possible panorama frame in pixel, [1, infinity)
		 * @return The resulting pixel position, with range (-infinity, infinity)x(-infinity, infinity)
		 * @see angle2pixelStrict().
		 */
		static inline Vector2 angle2pixel(const Vector2& angle, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight);

		/**
		 * Converts an angle in the entire (maximal possible) panorama frame to the corresponding pixel position in the entire panorama frame.
		 * @param angle The horizontal and vertical angle to be converted, with range [-PI, PI)x[PI/2, -PI/2)
		 * @param panoramaDimensionWidth The width of the maximal possible panorama frame in pixel, [1, infinity)
		 * @param panoramaDimensionHeight THe height of the maximal possible panorama frame in pixel, [1, infinity)
		 * @return The resulting pixel position, with range [0, panoramaDimensionWidth)[0, panoramaDimensionHeight)
		 * @see angle2pixel().
		 */
		static inline Vector2 angle2pixelStrict(const Vector2& angle, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight);

		/**
		 * Converts a pixel position in the entire (maximal possible) panorama frame (or outside this panorama frame) to the corresponding angle in the entire panorama frame.
		 * @param position The pixel position in the entire maximal possible panorama frame in pixel, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param invPanoramaDimensionWidth The inverse of the width of the maximal possible panorama frame in pixel, with range (0, 1]
		 * @param invPanoramaDimensionHeight The inverse of the height of the maximal possible panorama frame in pixel, with range (0, 1]
		 * @return The resulting angle, with range (-infinity, infinity)x(-infinity, infinity)
		 * @see pixel2angleStrict().
		 */
		static inline Vector2 pixel2angle(const Vector2& position, const Scalar invPanoramaDimensionWidth, const Scalar invPanoramaDimensionHeight);

		/**
		 * Converts a pixel position in the entire (maximal possible) panorama frame to the corresponding angle in the entire panorama frame.
		 * @param position The pixel position in the entire maximal possible panorama frame in pixel, with range [0, panoramaDimensionWidth)x[0, panoramaDimensionHeight)
		 * @param invPanoramaDimensionWidth The inverse of the width of the maximal possible panorama frame in pixel, with range (0, 1]
		 * @param invPanoramaDimensionHeight The inverse of the height of the maximal possible panorama frame in pixel, with range (0, 1]
		 * @return The resulting angle, with range [-PI, PI)x(PI/2, -PI/2)
		 * @see pixel2angle().
		 */
		static inline Vector2 pixel2angleStrict(const Vector2& position, const Scalar invPanoramaDimensionWidth, const Scalar invPanoramaDimensionHeight);

		/**
		 * Converts a unit ray starting at the panorama frame's center to the corresponding angle.
		 * @param ray The 3D ray to be transformed
		 * @param referenceAngle The reference angle to which the resulting angle will be adjusted, with range (-infinity, infinity)x(-infinity, infinity)
		 * @return The corresponding angle, with range (-infinity, infinity)x(-infinity, infinity)
		 * @see ray2angleStrict().
		 */
		static inline Vector2 ray2angle(const Vector3& ray, const Vector2& referenceAngle);

		/**
		 * Converts a unit ray starting at the panorama frame's center to the corresponding angle.
		 * @param ray The 3D ray to be transformed
		 * @return The corresponding angle, with range [-PI, PI)x(PI/2, -PI/2)
		 * @see ray2angle().
		 */
		static inline Vector2 ray2angleStrict(const Vector3& ray);

		/**
		 * Converts an angle in the panorama frame to a unit ray starting at the panorama frame's center.
		 * @param angle The angle to be transformed, with range [-infinity, infinity)x(-infinity, -infinity)
		 * @return The corresponding unit ray in the panorama frame
		 * @see angle2rayStrict().
		 */
		static inline Vector3 angle2ray(const Vector2& angle);

		/**
		 * Converts an angle in the panorama frame to a unit ray starting at the panorama frame's center.
		 * @param angle The angle to be transformed, with range [-PI, PI)x(PI/2, -PI/2)
		 * @return The corresponding unit ray in the panorama frame
		 * @see angle2ray().
		 */
		static inline Vector3 angle2rayStrict(const Vector2& angle);

		/**
		 * Converts a camera pixel position defined in one frame into a camera pixel position defined in another frame.
		 * @param inputCamera The camera profile of the input camera in which the pixel position is given
		 * @param inputOrientation The orientation of the input camera
		 * @param inputPosition The pixel position defined in the input camera which will be converted to the corresponding pixel position in the output camera
		 * @param outputCamera The camera profile of the output camera
		 * @param outputOrientation The orientation of the output camera
		 * @return Resulting pixel position in the output camera which corresponds to the given input pixel position
		 */
		static Vector2 cameraPixel2cameraPixel(const PinholeCamera& inputCamera, const SquareMatrix3& inputOrientation, const Vector2& inputPosition, const PinholeCamera& outputCamera, const SquareMatrix3& outputOrientation);

		/**
		 * Converts a given input camera frame captured with a given orientation into a corresponding camera frame with different orientation.
		 * @param inputCamera The camera profile of the given input frame
		 * @param inputOrientation The orientation of the given input frame (and input camera)
		 * @param inputFrame The input frame which will be converted, the frame dimension must match with the given camera profile
		 * @param inputMask Optional mask defining valid and invalid pixels in the input frame, a default frame sets all pixels as valid
		 * @param outputCamera The camera profile of the resulting output frame
		 * @param outputOrientation The orientation of the resulting output frame
		 * @param outputFrame The resulting output frame, the frame dimension will be set to the given output camera profile
		 * @param outputMask The resulting mask of the output frame defining valid and invalid pixels
		 * @param maskValue The mask value defining valid pixels in the mask frames
		 * @param approximationBinSize Optional width of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		static bool cameraFrame2cameraFrame(const PinholeCamera& inputCamera, const SquareMatrix3& inputOrientation, const Frame& inputFrame, const Frame& inputMask, const PinholeCamera& outputCamera, const SquareMatrix3& outputOrientation, Frame& outputFrame, Frame& outputMask, const uint8_t maskValue, const unsigned int approximationBinSize, Worker* worker);

		/**
		 * Copies (interpolates) a section from the entire panorama frame to a camera frame with specified camera orientation.
		 * @param pinholeCamera The pinhole camera profile of the resulting camera frame
		 * @param panoramaFrame The sub-frame of the entire (possible maximal) panorama frame from which the resulting camera frame is created
		 * @param panoramaMask The mask frame corresponding with the given panorama frame (with same dimension)
		 * @param panoramaDimensionWidth The maximal width of the entire maximal possible panorama frame, in pixel
		 * @param panoramaDimensionHeight The maximal height of the entire maximal possible panorama frame, in pixel
		 * @param panoramaFrameTopLeft The top left position of the given sub-frame of the panorama frame
		 * @param orientation The orientation of the resulting camera frame
		 * @param cameraFrame The resulting camera frame, will receive the frame dimension as provided by the camera profile
		 * @param cameraMask The resulting camera mask, will receive the frame dimension as provided by the camera profile
		 * @param maskValue The mask value defining a valid mask pixel
		 * @param approximationBinSize Optional width/height of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the camera frame
		 * @return True, if succeeded
		 */
		static bool panoramaFrame2cameraFrame(const PinholeCamera& pinholeCamera, const Frame& panoramaFrame, const Frame& panoramaMask, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPosition& panoramaFrameTopLeft, const SquareMatrix3& orientation, Frame& cameraFrame, Frame& cameraMask, const uint8_t maskValue = 0xFFu, const unsigned int approximationBinSize = 20u, Worker* worker = nullptr, const LookupTable* fineAdjustment = nullptr);

		/**
		 * Copies (interpolates) the entire area of a camera frame with specified camera orientation to a section of an entire panorama frame.
		 * @param pinholeCamera The pinhole camera profile of the resulting camera frame
		 * @param cameraFrame The camera frame to be copied
		 * @param orientation The orientation of the given camera frame
		 * @param panoramaDimensionWidth The maximal width of the entire maximal possible panorama frame, in pixel
		 * @param panoramaDimensionHeight The maximal height of the entire maximal possible panorama frame, in pixel
		 * @param panoramaFrameTopLeft The top left position of the resulting sub-frame of the panorama frame
		 * @param panoramaFrame The resulting sub-frame of the entire (possible maximal) panorama frame
		 * @param panoramaMask The mask frame corresponding with the resulting panorama frame (with same dimension)
		 * @param maskValue The mask value defining a valid mask pixel
		 * @param approximationBinSize Optional width/height of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the given camera frame
		 * @return True, if succeeded
		 */
		static bool cameraFrame2panoramaFrame(const PinholeCamera& pinholeCamera, const Frame& cameraFrame, const SquareMatrix3& orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPositionI& panoramaFrameTopLeft, Frame& panoramaFrame, Frame& panoramaMask, const uint8_t maskValue = 0xFFu, const unsigned int approximationBinSize = 20u, Worker* worker = nullptr, const LookupTable* fineAdjustment = nullptr);

		/**
		 * Copies (interpolates) the a subset of a camera frame (specified by a mask) with specified camera orientation to a section of an entire panorama frame.
		 * @param pinholeCamera The pinhole camera profile of the resulting camera frame
		 * @param cameraFrame The camera frame to be copied
		 * @param cameraMask The mask frame corresponding with the camera frame specifying all valid pixels
		 * @param orientation The orientation of the given camera frame
		 * @param panoramaDimensionWidth The maximal width of the entire maximal possible panorama frame, in pixel
		 * @param panoramaDimensionHeight The maximal height of the entire maximal possible panorama frame, in pixel
		 * @param panoramaFrameTopLeft The top left position of the resulting sub-frame of the panorama frame
		 * @param panoramaFrame The resulting sub-frame of the entire (possible maximal) panorama frame
		 * @param panoramaMask The mask frame corresponding with the resulting panorama frame (with same dimension)
		 * @param maskValue The mask value defining a valid mask pixel
		 * @param approximationBinSize Optional width/height of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the given camera frame
		 * @return True, if succeeded
		 */
		static bool cameraFrame2panoramaFrame(const PinholeCamera& pinholeCamera, const Frame& cameraFrame, const Frame& cameraMask, const SquareMatrix3& orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPositionI& panoramaFrameTopLeft, Frame& panoramaFrame, Frame& panoramaMask, const uint8_t maskValue = 0xFFu, const unsigned int approximationBinSize = 20u, Worker* worker = nullptr, const LookupTable* fineAdjustment = nullptr);

		/**
		 * Converts a given input camera frame with 8 bit per channel captured with a given orientation into a corresponding camera frame with different orientation.
		 * @param inputCamera The camera profile of the given input frame
		 * @param inputOrientation The orientation of the given input frame (and input camera)
		 * @param inputFrame The input frame which will be converted, the frame dimension must match with the given camera profile
		 * @param inputMask Optional mask defining valid and invalid pixels in the input frame, a default frame sets all pixels as valid
		 * @param inputFramePaddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param inputMaskPaddingElements The number of padding elements of the input mask, range: [0, infinity)
		 * @param outputCamera The camera profile of the resulting output frame
		 * @param outputOrientation The orientation of the resulting output frame
		 * @param outputFrame The resulting output frame
		 * @param outputMask The resulting mask of the output frame defining valid and invalid pixels
		 * @param outputFramePaddingElements The number of padding elements of the output frame, range: [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements of the output mask, range: [0, infinity)
		 * @param maskValue The mask value defining valid pixels in the mask frames
		 * @param approximationBinSize Optional width of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static inline void cameraFrame2cameraFrame8BitPerChannel(const PinholeCamera& inputCamera, const SquareMatrix3& inputOrientation, const uint8_t* inputFrame, const uint8_t* inputMask, const unsigned int inputFramePaddingElements, const unsigned int inputMaskPaddingElements, const PinholeCamera& outputCamera, const SquareMatrix3& outputOrientation, uint8_t* outputFrame, uint8_t* outputMask, const unsigned int outputFramePaddingElements, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int approximationBinSize, Worker* worker);

		/**
		 * Copies (interpolates) a section from the entire panorama frame with 8 bit per data channel to a camera frame with specified camera orientation.
		 * @param pinholeCamera The pinhole camera profile of the resulting camera frame
		 * @param panoramaFrame The sub-frame of the entire (possible maximal) panorama frame from which the resulting camera frame is created
		 * @param panoramaMask The mask frame corresponding with the given panorama frame (with same dimension)
		 * @param panoramaFrameWidth The width of the given panorama frame in pixel, with range [1, panoramaDimensionWidth]
		 * @param panoramaFrameHeight The height of the given panorama frame in pixel, with range [1, panoramaDimensionHeight]
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param panoramaDimensionWidth The maximal width of the entire maximal possible panorama frame, in pixel
		 * @param panoramaDimensionHeight The maximal height of the entire maximal possible panorama frame, in pixel
		 * @param panoramaFrameTopLeft The top left position of the given sub-frame of the panorama frame
		 * @param orientation The orientation of the resulting camera frame
		 * @param cameraFrame The resulting camera frame with frame dimension as provided by the camera profile
		 * @param cameraMask The resulting camera mask with frame dimension as provided by the camera profile
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param cameraMaskPaddingElements The number of padding elements at the end of each camera mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value defining a valid mask pixel
		 * @param approximationBinSize Optional width/height of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the camera frame
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static inline void panoramaFrame2cameraFrame8BitPerChannel(const PinholeCamera& pinholeCamera, const uint8_t* panoramaFrame, const uint8_t* panoramaMask, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPosition& panoramaFrameTopLeft, const SquareMatrix3& orientation, uint8_t* cameraFrame, uint8_t* cameraMask, const unsigned int cameraFramePaddingElements, const unsigned int cameraMaskPaddingElements, const uint8_t maskValue = 0xFFu, const unsigned int approximationBinSize = 20u, Worker* worker = nullptr, const LookupTable* fineAdjustment = nullptr);

		/**
		 * Copies (interpolates) the entire area of an 8 bit per data channel camera frame with specified camera orientation to a section of an entire panorama frame.
		 * @param pinholeCamera The pinhole camera profile of the resulting camera frame
		 * @param cameraFrame The camera frame to be copied
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param orientation The orientation of the given camera frame
		 * @param panoramaDimensionWidth The maximal width of the entire maximal possible panorama frame, in pixel
		 * @param panoramaDimensionHeight The maximal height of the entire maximal possible panorama frame, in pixel
		 * @param panoramaFrameTopLeft The top left position of the resulting sub-frame of the panorama frame
		 * @param panoramaFrame The resulting sub-frame of the entire (possible maximal) panorama frame
		 * @param panoramaMask The mask frame corresponding with the resulting panorama frame (with same dimension)
		 * @param panoramaFrameWidth The width of the resulting panorama frame in pixel, with range [1, panoramaDimensionWidth]
		 * @param panoramaFrameHeight The height of the resulting panorama frame in pixel, with range [1, panoramaDimensionHeight]
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value defining a valid mask pixel
		 * @param approximationBinSize Optional width/height of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the given camera frame
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static inline void cameraFrame2panoramaFrame8BitPerChannel(const PinholeCamera& pinholeCamera, const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const SquareMatrix3& orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPositionI& panoramaFrameTopLeft, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const uint8_t maskValue = 0xFFu, const unsigned int approximationBinSize = 20u, Worker* worker = nullptr, const LookupTable* fineAdjustment = nullptr);

		/**
		 * Copies (interpolates) a subset of an 8 bit per data channel camera frame (specified by a mask) with specified camera orientation to a section of an entire panorama frame.
		 * @param pinholeCamera The pinhole camera profile of the resulting camera frame
		 * @param cameraFrame The camera frame to be copied
		 * @param cameraMask The mask of camera frame specifying valid pixels
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param cameraMaskPaddingElements The number of padding elements at the end of each camera mask row, in elements, with range [0, infinity)
		 * @param orientation The orientation of the given camera frame
		 * @param panoramaDimensionWidth The maximal width of the entire maximal possible panorama frame, in pixel
		 * @param panoramaDimensionHeight The maximal height of the entire maximal possible panorama frame, in pixel
		 * @param panoramaFrameTopLeft The top left position of the resulting sub-frame of the panorama frame
		 * @param panoramaFrame The resulting sub-frame of the entire (possible maximal) panorama frame
		 * @param panoramaMask The mask frame corresponding with the resulting panorama frame (with same dimension)
		 * @param panoramaFrameWidth The width of the resulting panorama frame in pixel, with range [1, panoramaDimensionWidth]
		 * @param panoramaFrameHeight The height of the resulting panorama frame in pixel, with range [1, panoramaDimensionHeight]
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value defining a valid mask pixel
		 * @param approximationBinSize Optional width/height of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the given camera frame
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static inline void cameraFrame2panoramaFrameMask8BitPerChannel(const PinholeCamera& pinholeCamera, const uint8_t* cameraFrame, const uint8_t* cameraMask, const unsigned int cameraFramePaddingElements, const unsigned int cameraMaskPaddingElements, const SquareMatrix3& orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPositionI& panoramaFrameTopLeft, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const uint8_t maskValue = 0xFFu, const unsigned int approximationBinSize = 20u, Worker* worker = nullptr, const LookupTable* fineAdjustment = nullptr);

	protected:

		/**
		 * Determines the bounding box for a camera frame with given orientation in the panorama frame.
		 * @param pinholeCamera The pinhole camera profile (also defining the dimension of the camera frame)
		 * @param orientation The orientation of the camera frame
		 * @return The resulting bounding box of the camera frame projected in the panorama frame, may be located outside the entire panorama frame
		 */
		Box2 panoramaSubFrameBoundingBox(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation) const;

		/**
		 * Updates the panorama frame data with a new camera frame.
		 * @param pinholeCamera The pinhole camera profile of the camera frame
		 * @param frame The camera frame to update the panorama frame
		 * @param mask Optional mask frame specifying valid and invalid pixels, a default frame sets all pixels as valid
		 * @param orientation The orientation of the camera frame
		 * @param approximationBinSize Optional width/height of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the given camera frame, with size identical to the camera dimension
		 */
		bool update(const PinholeCamera& pinholeCamera, const Frame& frame, const Frame& mask, const SquareMatrix3& orientation, const unsigned int approximationBinSize = 20u, Worker* worker = nullptr, const LookupTable* fineAdjustment = nullptr);

		/**
		 * Resets the panorama frame of this object by one new single camera frame.
		 * @param pinholeCamera The pinhole camera profile of the camera frame
		 * @param frame The new camera frame which will define the new panorama frame
		 * @param orientation The orientation of the given camera frame
		 * @param approximationBinSize Optional width/height of a bin in a lookup table to speedup the in interpolation in pixel, 0u to avoid the application of a lookup table
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool reset(const PinholeCamera& pinholeCamera, const Frame& frame, const SquareMatrix3& orientation, const unsigned int approximationBinSize = 20u, Worker* worker = nullptr);

		/**
		 * Resets the panorama frame of this object by one new panorama sub-frame.
		 * @param topLeft The top left position of the given panorama sub frame
		 * @param frame The new panorama sub-frame to be set
		 * @param mask Optional the mask defining valid and invalid pixels in the given sub-frame, must match the dimension of the panoramaFrame if valid, a default frame set all panorama pixels valid
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool reset(const PixelPosition& topLeft, const Frame& frame, const Frame& mask, Worker* worker = nullptr);

		/**
		 * Resizes the internal panorama sub-frame.
		 * @param topLeft The new top left position of the panorama sub-frame
		 * @param width The new width of the panorama sub-frame, with range [1, dimensionWidth() - topLeft.x())
		 * @param height The new height of the panorama sub-frame, with range [1, dimensionHeight() - topLeft.y())
		 */
		void resize(const PixelPosition& topLeft, const unsigned int width, const unsigned int height);

		/**
		 * Merges a given panorama sub-frame with given top left start position with the panorama frame of this object by application of the update mode of this object.
		 * @param panoramaSubFrame The panorama sub-frame which will be merged with the current panorama frame
		 * @param panoramaSubMask The mask matching with the given sub-frame specifying valid pixels
		 * @param subTopLeft The top left position of the given panorama sub-frame
		 * @param worker Optional worker object to distribute the computation
		 * @return True, if succeeded
		 */
		bool merge(const Frame& panoramaSubFrame, const Frame& panoramaSubMask, const PixelPosition& subTopLeft, Worker* worker = nullptr);

		/**
		 * Creates a 2D lookup table allowing to interpolate pixel locations defined in one camera frame to pixel locations defined in another camera frame.
		 * @param inputCamera The camera profile of the input camera frame, must be valid
		 * @param world_R_input The orientation between the input camera and world, must be valid
		 * @param outputCamera The camera profile of the output camera frame, must be valid
		 * @param world_R_output The orientation between the output camera and world, must be valid
		 * @param input_LT_output The resulting lookup table transforming output locations to input locations
		 */
		static void cameraFrame2cameraFrameLookupTable(const PinholeCamera& inputCamera, const SquareMatrix3& world_R_input, const PinholeCamera& outputCamera, const SquareMatrix3& world_R_output, LookupTable& input_LT_output);

		/**
		 * Creates a 2D lookup table allowing to interpolate positions defined in the camera frame to positions defined in the panorama sub-frame.
		 * @param pinholeCamera The pinhole camera profile of the camera frame
		 * @param orientation The orientation of the camera
		 * @param panoramaDimensionWidth The maximal width of the entire maximal possible panorama frame, in pixel
		 * @param panoramaDimensionHeight The maximal height of the entire maximal possible panorama frame, in pixel
		 * @param panoramaFrameTopLeft The top left position of the panorama sub-frame
		 * @param lookupTable The resulting lookup table
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the camera frame
		 */
		static void panoramaFrame2cameraFrameLookupTable(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPosition& panoramaFrameTopLeft, LookupTable& lookupTable, const LookupTable* fineAdjustment);

		/**
		 * Creates a 2D lookup table allowing to interpolate positions defined in the entire panorama frame to positions defined in the camera frame.
		 * @param pinholeCamera The pinhole camera profile of the camera frame
		 * @param orientation The orientation of the camera
		 * @param panoramaDimensionWidth The maximal width of the entire maximal possible panorama frame, in pixel
		 * @param panoramaDimensionHeight The maximal height of the entire maximal possible panorama frame, in pixel
		 * @param panoramaFrameTopLeft The top left position of the panorama sub-frame within the entire panorama frame, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param lookupTable The resulting lookup table
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the given camera frame
		 */
		static void cameraFrame2panoramaFrameLookupTable(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPositionI& panoramaFrameTopLeft, LookupTable& lookupTable, const LookupTable* fineAdjustment);

		/**
		 * Merges a given panorama sub frame with the already existing panorama frame by setting all valid pixels of the sub frame.
		 * @param panoramaSubFrame The panorama sub frame which will be merged with the existing panorama frame
		 * @param panoramaSubMask The mask of the panorama sub frame defining valid and invalid pixels
		 * @param subFrameWidth The width of the panorama sub frame in pixel, with range [1, panoramaWidth - subTopLeft.x()]
		 * @param subFrameHeight The height of the panorama sub frame in pixel, with range [1, panoramaHeight - subTopLeft.y()]
		 * @param panoramaSubFramePaddingElements The number of padding elements at the end of each sub frame row, in elements, with range [0, infinity)
		 * @param panoramaSubMaskPaddingElements The number of padding elements at the end of each sub mask row, in elements, with range [0, infinity)
		 * @param subTopLeft The top left position of the sub frame, with range [0, panoramaTopLeft.x() + panoramaWidth)x[0, panoramaTopLeft.y() + panoramaHeight)
		 * @param panoramaFrame The existing panorama frame which will receive the new image information
		 * @param panoramaMask The mask of the existing panorama frame
		 * @param panoramaWidth The width of the panorama frame in pixel
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param panoramaTopLeft The top left position of the panorama frame
		 * @param maskValue The mask value defining valid pixels
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of frame data channels
		 * @see mergeSetAll8BitPerChannelSubset().
		 */
		template <unsigned int tChannels>
		static inline void mergeSetAll8BitPerChannel(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int panoramaSubFramePaddingElements, const unsigned panoramaSubMaskPaddingElements, const PixelPosition& subTopLeft, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const PixelPosition& panoramaTopLeft, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Merges a given panorama sub frame with the already existing panorama frame by setting only pixels which haven't been set before.
		 * @param panoramaSubFrame The panorama sub frame which will be merged with the existing panorama frame
		 * @param panoramaSubMask The mask of the panorama sub frame defining valid and invalid pixels
		 * @param subFrameWidth The width of the panorama sub frame in pixel, with range [1, panoramaWidth - subTopLeft.x()]
		 * @param subFrameHeight The height of the panorama sub frame in pixel, with range [1, panoramaHeight - subTopLeft.y()]
		 * @param panoramaSubFramePaddingElements The number of padding elements at the end of each sub frame row, in elements, with range [0, infinity)
		 * @param panoramaSubMaskPaddingElements The number of padding elements at the end of each sub mask row, in elements, with range [0, infinity)
		 * @param subTopLeft The top left position of the sub frame, with range [0, panoramaTopLeft.x() + panoramaWidth)x[0, panoramaTopLeft.y() + panoramaHeight)
		 * @param panoramaFrame The existing panorama frame which will receive the new image information
		 * @param panoramaMask The mask of the existing panorama frame
		 * @param panoramaWidth The width of the panorama frame in pixel
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param panoramaTopLeft The top left position of the panorama frame
		 * @param maskValue The mask value defining valid pixels
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of frame data channels
		 * @see mergeSetNew8BitPerChannelSubset().
		 */
		template <unsigned int tChannels>
		static inline void mergeSetNew8BitPerChannel(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int panoramaSubFramePaddingElements, const unsigned panoramaSubMaskPaddingElements, const PixelPosition& subTopLeft, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const PixelPosition& panoramaTopLeft, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Merges a given panorama sub frame with the already existing panorama frame by averaging the current pixel value with the given pixel value.
		 * @param panoramaSubFrame The panorama sub frame which will be merged with the existing panorama frame
		 * @param panoramaSubMask The mask of the panorama sub frame defining valid and invalid pixels
		 * @param subFrameWidth The width of the panorama sub frame in pixel, with range [1, panoramaWidth - subTopLeft.x()]
		 * @param subFrameHeight The height of the panorama sub frame in pixel, with range [1, panoramaHeight - subTopLeft.y()]
		 * @param panoramaSubFramePaddingElements The number of padding elements at the end of each sub frame row, in elements, with range [0, infinity)
		 * @param panoramaSubMaskPaddingElements The number of padding elements at the end of each sub mask row, in elements, with range [0, infinity)
		 * @param subTopLeft The top left position of the sub frame, with range [0, panoramaTopLeft.x() + panoramaWidth)x[0, panoramaTopLeft.y() + panoramaHeight)
		 * @param panoramaFrame The existing panorama frame which will receive the new image information
		 * @param panoramaMask The mask of the existing panorama frame
		 * @param panoramaWidth The width of the panorama frame in pixel
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param panoramaTopLeft The top left position of the panorama frame
		 * @param maskValue The mask value defining valid pixels
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of frame data channels
		 * @see mergeAverageLocal8BitPerChannelSubset().
		 */
		template <unsigned int tChannels>
		static inline void mergeAverageLocal8BitPerChannel(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int panoramaSubFramePaddingElements, const unsigned panoramaSubMaskPaddingElements, const PixelPosition& subTopLeft, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const PixelPosition& panoramaTopLeft, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Merges a given panorama sub frame with the already existing panorama frame by averaging the all pixel values from the past and the current pixel values.
		 * @param panoramaSubFrame The panorama sub frame which will be merged with the existing panorama frame
		 * @param panoramaSubMask The mask of the panorama sub frame defining valid and invalid pixels
		 * @param subFrameWidth The width of the panorama sub frame in pixel, with range [1, panoramaWidth - subTopLeft.x()]
		 * @param subFrameHeight The height of the panorama sub frame in pixel, with range [1, panoramaHeight - subTopLeft.y()]
		 * @param panoramaSubFramePaddingElements The number of padding elements at the end of each sub frame row, in elements, with range [0, infinity)
		 * @param panoramaSubMaskPaddingElements The number of padding elements at the end of each sub mask row, in elements, with range [0, infinity)
		 * @param subTopLeft The top left position of the sub frame, with range [0, panoramaTopLeft.x() + panoramaWidth)x[0, panoramaTopLeft.y() + panoramaHeight)
		 * @param panoramaNominatorFrame The nominator frame of the panorama frame summing all individual pixels values
		 * @param panoramaDenominatorFrame The denominator frame of the panorama frame storing the number of summed pixels individually for every pixel
		 * @param panoramaFrame The existing panorama frame which will receive the new image information
		 * @param panoramaMask The mask of the existing panorama frame
		 * @param panoramaWidth The width of the panorama frame in pixel
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param panoramaTopLeft The top left position of the panorama frame
		 * @param maskValue The mask value defining valid pixels
		 * @param worker Optional worker object to distribute the computation
		 * @tparam tChannels The number of frame data channels
		 * @see mergeAverageGlobal8BitPerChannelSubset().
		 */
		template <unsigned int tChannels>
		static inline void mergeAverageGlobal8BitPerChannel(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int panoramaSubFramePaddingElements, const unsigned panoramaSubMaskPaddingElements, const PixelPosition& subTopLeft, uint32_t* panoramaNominatorFrame, uint32_t* panoramaDenominatorFrame, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const PixelPosition& panoramaTopLeft, const uint8_t maskValue, Worker* worker = nullptr);

		/**
		 * Merges a subset of a given panorama sub frame with the already existing panorama frame by setting all valid pixels of the sub frame.
		 * @param panoramaSubFrame The panorama sub frame which will be merged with the existing panorama frame
		 * @param panoramaSubMask The mask of the panorama sub frame defining valid and invalid pixels
		 * @param subFrameWidth The width of the panorama sub frame in pixel, with range [1, panoramaWidth - subTopLeft.x()]
		 * @param panoramaSubFramePaddingElements The number of padding elements at the end of each sub frame row, in elements, with range [0, infinity)
		 * @param panoramaSubMaskPaddingElements The number of padding elements at the end of each sub mask row, in elements, with range [0, infinity)
		 * @param subTopLeftX The horizontal top left position of the sub frame, with range [0, panoramaTopLeft.x() + panoramaWidth)
		 * @param subTopLeftY The vertical top left position of the sub frame, with range [0, panoramaTopLeft.x() + panoramaWidth)
		 * @param panoramaFrame The existing panorama frame which will receive the new image information
		 * @param panoramaMask The mask of the existing panorama frame
		 * @param panoramaWidth The width of the panorama frame in pixel
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param panoramaTopLeftX The horizontal top left position of the panorama frame
		 * @param panoramaTopLeftY The vertical top left position of the panorama frame
		 * @param maskValue The mask value defining valid pixels
		 * @param firstSubRow The first sub row to be handled
		 * @param numberSubRows The number of sub rows to be handled
		 * @tparam tChannels The number of frame data channels
		 * @see mergeSetAll8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static void mergeSetAll8BitPerChannelSubset(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int panoramaSubFramePaddingElements, const unsigned int panoramaSubMaskPaddingElements, const unsigned int subTopLeftX, const unsigned int subTopLeftY, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaTopLeftX, const unsigned int panoramaTopLeftY, const uint8_t maskValue, const unsigned int firstSubRow, const unsigned int numberSubRows);

		/**
		 * Merges a subset of a given panorama sub frame with the already existing panorama frame by setting only pixels which haven't been set before.
		 * @param panoramaSubFrame The panorama sub frame which will be merged with the existing panorama frame
		 * @param panoramaSubMask The mask of the panorama sub frame defining valid and invalid pixels
		 * @param subFrameWidth The width of the panorama sub frame in pixel, with range [1, panoramaWidth - subTopLeft.x()]
		 * @param panoramaSubFramePaddingElements The number of padding elements at the end of each sub frame row, in elements, with range [0, infinity)
		 * @param panoramaSubMaskPaddingElements The number of padding elements at the end of each sub mask row, in elements, with range [0, infinity)
		 * @param subTopLeftX The horizontal top left position of the sub frame, with range [0, panoramaTopLeft.x() + panoramaWidth)
		 * @param subTopLeftY The vertical top left position of the sub frame, with range [0, panoramaTopLeft.x() + panoramaWidth)
		 * @param panoramaFrame The existing panorama frame which will receive the new image information
		 * @param panoramaMask The mask of the existing panorama frame
		 * @param panoramaWidth The width of the panorama frame in pixel
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param panoramaTopLeftX The horizontal top left position of the panorama frame
		 * @param panoramaTopLeftY The vertical top left position of the panorama frame
		 * @param maskValue The mask value defining valid pixels
		 * @param firstSubRow The first sub row to be handled
		 * @param numberSubRows The number of sub rows to be handled
		 * @tparam tChannels The number of frame data channels
		 * @see mergeSetNew8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static void mergeSetNew8BitPerChannelSubset(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int panoramaSubFramePaddingElements, const unsigned int panoramaSubMaskPaddingElements, const unsigned int subTopLeftX, const unsigned int subTopLeftY, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaTopLeftX, const unsigned int panoramaTopLeftY, const uint8_t maskValue, const unsigned int firstSubRow, const unsigned int numberSubRows);

		/**
		 * Merges a subset of a given panorama sub frame with the already existing panorama frame by averaging the current pixel value with the given pixel value.
		 * @param panoramaSubFrame The panorama sub frame which will be merged with the existing panorama frame
		 * @param panoramaSubMask The mask of the panorama sub frame defining valid and invalid pixels
		 * @param subFrameWidth The width of the panorama sub frame in pixel, with range [1, panoramaWidth - subTopLeft.x()]
		 * @param panoramaSubFramePaddingElements The number of padding elements at the end of each sub frame row, in elements, with range [0, infinity)
		 * @param panoramaSubMaskPaddingElements The number of padding elements at the end of each sub mask row, in elements, with range [0, infinity)
		 * @param subTopLeftX The horizontal top left position of the sub frame, with range [0, panoramaTopLeft.x() + panoramaWidth)
		 * @param subTopLeftY The vertical top left position of the sub frame, with range [0, panoramaTopLeft.x() + panoramaWidth)
		 * @param panoramaFrame The existing panorama frame which will receive the new image information
		 * @param panoramaMask The mask of the existing panorama frame
		 * @param panoramaWidth The width of the panorama frame in pixel
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param panoramaTopLeftX The horizontal top left position of the panorama frame
		 * @param panoramaTopLeftY The vertical top left position of the panorama frame
		 * @param maskValue The mask value defining valid pixels
		 * @param firstSubRow The first sub row to be handled
		 * @param numberSubRows The number of sub rows to be handled
		 * @tparam tChannels The number of frame data channels
		 * @see mergeAverageLocal8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static void mergeAverageLocal8BitPerChannelSubset(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int panoramaSubFramePaddingElements, const unsigned int panoramaSubMaskPaddingElements, const unsigned int subTopLeftX, const unsigned int subTopLeftY, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaTopLeftX, const unsigned int panoramaTopLeftY, const uint8_t maskValue, const unsigned int firstSubRow, const unsigned int numberSubRows);

		/**
		 * Merges a subset of a given panorama sub frame with the already existing panorama frame by averaging the all pixel values from the past and the current pixel values.
		 * @param panoramaSubFrame The panorama sub frame which will be merged with the existing panorama frame
		 * @param panoramaSubMask The mask of the panorama sub frame defining valid and invalid pixels
		 * @param subFrameWidth The width of the panorama sub frame in pixel, with range [1, panoramaWidth - subTopLeft.x()]
		 * @param panoramaSubFramePaddingElements The number of padding elements at the end of each sub frame row, in elements, with range [0, infinity)
		 * @param panoramaSubMaskPaddingElements The number of padding elements at the end of each sub mask row, in elements, with range [0, infinity)
		 * @param subTopLeftX The horizontal top left position of the sub frame, with range [0, panoramaTopLeft.x() + panoramaWidth)
		 * @param subTopLeftY The vertical top left position of the sub frame, with range [0, panoramaTopLeft.x() + panoramaWidth)
		 * @param panoramaNominatorFrame The nominator frame of the panorama frame summing all individual pixels values
		 * @param panoramaDenominatorFrame The denominator frame of the panorama frame storing the number of summed pixels individually for every pixel
		 * @param panoramaFrame The existing panorama frame which will receive the new image information
		 * @param panoramaMask The mask of the existing panorama frame
		 * @param panoramaWidth The width of the panorama frame in pixel
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param panoramaTopLeftX The horizontal top left position of the panorama frame
		 * @param panoramaTopLeftY The vertical top left position of the panorama frame
		 * @param maskValue The mask value defining valid pixels
		 * @param firstSubRow The first sub row to be handled
		 * @param numberSubRows The number of sub rows to be handled
		 * @tparam tChannels The number of frame data channels
		 * @see mergeAverageGlobal8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static void mergeAverageGlobal8BitPerChannelSubset(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int panoramaSubFramePaddingElements, const unsigned int panoramaSubMaskPaddingElements, const unsigned int subTopLeftX, const unsigned int subTopLeftY, uint32_t* panoramaNominatorFrame, uint32_t* panoramaDenominatorFrame, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaTopLeftX, const unsigned int panoramaTopLeftY, const uint8_t maskValue, const unsigned int firstSubRow, const unsigned int numberSubRows);

		/**
		 * Converts a subset of a given input camera frame with 8 bit per channel captured with a given orientation into a corresponding camera frame with different orientation.
		 * @param inputCamera The camera profile of the given input frame
		 * @param inputOrientation The orientation of the given input frame (and input camera)
		 * @param inputFrame The input frame which will be converted, the frame dimension must match with the given camera profile
		 * @param inputFramePaddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param outputCamera The camera profile of the resulting output frame
		 * @param outputOrientation The orientation of the resulting output frame
		 * @param outputFrame The resulting output frame
		 * @param outputMask The resulting mask of the output frame defining valid and invalid pixels
		 * @param outputFramePaddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements of the input mask, range: [0, infinity)
		 * @param maskValue The mask value defining valid pixels in the mask frames
		 * @param firstOutputRow The first output row to be handled
		 * @param numberOutputRows The number of output rows to be handled
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static void cameraFrame2cameraFrame8BitPerChannelSubset(const PinholeCamera* inputCamera, const SquareMatrix3* inputOrientation, const uint8_t* inputFrame, const unsigned int inputFramePaddingElements, const PinholeCamera* outputCamera, const SquareMatrix3* outputOrientation, uint8_t* outputFrame, uint8_t* outputMask, const unsigned int outputFramePaddingElements, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Converts a subset of a given input camera frame with 8 bit per channel captured with a given orientation into a corresponding camera frame with different orientation.
		 * This function created the final frame by application of a lookup table avoiding the expensive transformation calculations for each individual pixel.<br>
		 * @param lookupTable The lookup table allowing to determine the approximated transformation function between input and output frame
		 * @param inputFrame The input frame which will be converted, the frame dimension must match with the given camera profile
		 * @param inputWidth The width of the input frame, in pixel
		 * @param inputHeight The height of the input frame, in pixel
		 * @param inputFramePaddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param outputFrame The resulting output frame
		 * @param outputMask The resulting mask of the output frame defining valid and invalid pixels
		 * @param maskValue The mask value defining valid pixels in the mask frames
		 * @param outputFramePaddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements of the input mask, range: [0, infinity)
		 * @param firstOutputRow The first output row to be handled
		 * @param numberOutputRows The number of output rows to be handled
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static void cameraFrame2cameraFrameLookup8BitPerChannelSubset(const LookupTable* lookupTable, const uint8_t* inputFrame, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputFramePaddingElements, uint8_t* outputFrame, uint8_t* outputMask, const unsigned int outputFramePaddingElements, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Converts a subset of a given input camera frame with 8 bit per channel captured with a given orientation into a corresponding camera frame with different orientation.
		 * @param inputCamera The camera profile of the given input frame
		 * @param inputOrientation The orientation of the given input frame (and input camera)
		 * @param inputFrame The input frame which will be converted, the frame dimension must match with the given camera profile
		 * @param inputMask Mask defining valid and invalid pixels in the input frame, a default frame sets all pixels as valid
		 * @param inputFramePaddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param inputMaskPaddingElements The number of padding elements of the input mask, range: [0, infinity)
		 * @param outputCamera The camera profile of the resulting output frame
		 * @param outputOrientation The orientation of the resulting output frame
		 * @param outputFrame The resulting output frame
		 * @param outputMask The resulting mask of the output frame defining valid and invalid pixels
		 * @param outputFramePaddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements of the input mask, range: [0, infinity)
		 * @param maskValue The mask value defining valid pixels in the mask frames
		 * @param firstOutputRow The first output row to be handled
		 * @param numberOutputRows The number of output rows to be handled
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static void cameraFrame2cameraFrameMask8BitPerChannelSubset(const PinholeCamera* inputCamera, const SquareMatrix3* inputOrientation, const uint8_t* inputFrame, const uint8_t* inputMask, const unsigned int inputFramePaddingElements, const unsigned int inputMaskPaddingElements, const PinholeCamera* outputCamera, const SquareMatrix3* outputOrientation, uint8_t* outputFrame, uint8_t* outputMask, const unsigned int outputFramePaddingElements, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Converts a subset of a given input camera frame with 8 bit per channel captured with a given orientation into a corresponding camera frame with different orientation.
		 * This function created the final frame by application of a lookup table avoiding the expensive transformation calculations for each individual pixel.<br>
		 * @param lookupTable The lookup table allowing to determine the approximated transformation function between input and output frame
		 * @param inputFrame The input frame which will be converted, the frame dimension must match with the given camera profile
		 * @param inputMask Mask defining valid and invalid pixels in the input frame, a default frame sets all pixels as valid
		 * @param inputWidth The width of the input frame, in pixel
		 * @param inputHeight The height of the input frame, in pixel
		 * @param inputFramePaddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param inputMaskPaddingElements The number of padding elements of the input mask, range: [0, infinity)
		 * @param outputFrame The resulting output frame
		 * @param outputMask The resulting mask of the output frame defining valid and invalid pixels
		 * @param outputFramePaddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param outputMaskPaddingElements The number of padding elements of the input mask, range: [0, infinity)
		 * @param maskValue The mask value defining valid pixels in the mask frames
		 * @param firstOutputRow The first output row to be handled
		 * @param numberOutputRows The number of output rows to be handled
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static void cameraFrame2cameraFrameMaskLookup8BitPerChannelSubset(const LookupTable* lookupTable, const uint8_t* inputFrame, const uint8_t* inputMask, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputFramePaddingElements, const unsigned int inputMaskPaddingElements, uint8_t* outputFrame, uint8_t* outputMask, const unsigned int outputFramePaddingElements, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int firstOutputRow, const unsigned int numberOutputRows);

		/**
		 * Copies (interpolates) a subset of a section from the entire panorama frame with 8 bit per data channel to a camera frame with specified camera orientation.
		 * @param pinholeCamera The pinhole camera profile of the resulting camera frame
		 * @param panoramaFrame The sub-frame of the entire (possible maximal) panorama frame from which the resulting camera frame is created
		 * @param panoramaMask The mask frame corresponding with the given panorama frame (with same dimension)
		 * @param panoramaFrameWidth The width of the given panorama frame in pixel, with range [1, panoramaDimensionWidth]
		 * @param panoramaFrameHeight The height of the given panorama frame in pixel, with range [1, panoramaDimensionHeight]
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param panoramaDimensionWidth The maximal width of the entire maximal possible panorama frame, in pixel
		 * @param panoramaDimensionHeight The maximal height of the entire maximal possible panorama frame, in pixel
		 * @param panoramaFrameTopLeftX The horizontal top left position of the given sub-frame of the panorama frame
		 * @param panoramaFrameTopLeftY The vertical top left position of the given sub-frame of the panorama frame
		 * @param orientation The orientation of the resulting camera frame
		 * @param cameraFrame The resulting camera frame, will receive the frame dimension as provided by the camera profile
		 * @param cameraMask The resulting camera mask, will receive the frame dimension as provided by the camera profile
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param cameraMaskPaddingElements The number of padding elements at the end of each camera mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value defining a valid mask pixel
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the camera frame
		 * @param firstCameraRow First camera row to be handled
		 * @param numberCameraRows The number of camera rows to be handled
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static void panoramaFrame2cameraFrame8BitPerChannelSubset(const PinholeCamera* pinholeCamera, const uint8_t* panoramaFrame, const uint8_t* panoramaMask, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const unsigned int panoramaFrameTopLeftX, const unsigned int panoramaFrameTopLeftY, const SquareMatrix3* orientation, uint8_t* cameraFrame, uint8_t* cameraMask, const unsigned int cameraFramePaddingElements, const unsigned int cameraMaskPaddingElements, const uint8_t maskValue, const LookupTable* fineAdjustment, const unsigned int firstCameraRow, const unsigned int numberCameraRows);

		/**
		 * Copies (interpolates) a subset of a section from the entire panorama frame with 8 bit per data channel to a camera frame with specified camera orientation.
		 * @param lookupTable The 2D lookup table for the camera frame mapping positions defined in the camera frame to positions defined in the panorama frame
		 * @param panoramaFrame The sub-frame of the entire (possible maximal) panorama frame from which the resulting camera frame is created
		 * @param panoramaMask The mask frame corresponding with the given panorama frame (with same dimension)
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param panoramaDimensionWidth The maximal width of the entire maximal possible panorama frame, in pixel
		 * @param panoramaDimensionHeight The maximal height of the entire maximal possible panorama frame, in pixel
		 * @param panoramaFrameTopLeftX The horizontal top left position of the resulting sub-frame of the panorama frame
		 * @param panoramaFrameTopLeftY The vertical top left position of the resulting sub-frame of the panorama frame
		 * @param panoramaFrameWidth The width of the given panorama frame in pixel, with range [1, panoramaDimensionWidth]
		 * @param panoramaFrameHeight The height of the given panorama frame in pixel, with range [1, panoramaDimensionHeight]
		 * @param cameraFrame The resulting camera frame, will receive the frame dimension as provided by the camera profile
		 * @param cameraMask The resulting camera mask, will receive the frame dimension as provided by the camera profile
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param cameraMaskPaddingElements The number of padding elements at the end of each camera mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value defining a valid mask pixel
		 * @param firstCameraRow First camera row to be handled
		 * @param numberCameraRows The number of camera rows to be handled
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static void panoramaFrame2cameraFrameLookup8BitPerChannelSubset(const LookupTable* lookupTable, const uint8_t* panoramaFrame, const uint8_t* panoramaMask, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const unsigned int panoramaFrameTopLeftX, const unsigned int panoramaFrameTopLeftY, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, uint8_t* cameraFrame, uint8_t* cameraMask, const unsigned int cameraFramePaddingElements, const unsigned int cameraMaskPaddingElements, const uint8_t maskValue, const unsigned int firstCameraRow, const unsigned int numberCameraRows);

		/**
		 * Copies (interpolates) the entire area of an 8 bit per data channel camera frame with specified camera orientation to a subset of a section of an entire panorama frame.
		 * @param pinholeCamera The pinhole camera profile of the resulting camera frame
		 * @param cameraFrame The camera frame to be copied, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param orientation The orientation of the given camera frame
		 * @param panoramaDimensionWidth The maximal width of the entire maximal possible panorama frame, in pixel
		 * @param panoramaDimensionHeight The maximal height of the entire maximal possible panorama frame, in pixel
		 * @param panoramaFrameTopLeftX The horizontal top left position of the resulting sub-frame of the panorama frame
		 * @param panoramaFrameTopLeftY The vertical top left position of the resulting sub-frame of the panorama frame
		 * @param panoramaFrame The resulting sub-frame of the entire (possible maximal) panorama frame
		 * @param panoramaMask The mask frame corresponding with the resulting panorama frame (with same dimension)
		 * @param panoramaFrameWidth The width of the resulting panorama frame in pixel, with range [1, panoramaDimensionWidth]
		 * @param panoramaFrameHeight The height of the resulting panorama frame in pixel, with range [1, panoramaDimensionHeight]
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value defining a valid mask pixel
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the given camera frame
		 * @param firstPanoramaRow First panorama frame row to be handled
		 * @param numberPanoramaRows The number of panorama frame rows to be handled
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static void cameraFrame2panoramaFrame8BitPerChannelSubset(const PinholeCamera* pinholeCamera, const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const SquareMatrix3* orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const int panoramaFrameTopLeftX, const int panoramaFrameTopLeftY, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const uint8_t maskValue, const LookupTable* fineAdjustment, const unsigned int firstPanoramaRow, const unsigned int numberPanoramaRows);

		/**
		 * Copies (interpolates) the entire area of an 8 bit per data channel camera frame with specified camera orientation to a subset of a section of an entire panorama frame.
		 * @param lookupTable The 2D lookup table for the camera frame mapping positions defined in the camera frame to positions defined in the panorama frame
		 * @param cameraFrame The camera frame to be copied
		 * @param cameraFrameWidth The width of the camera frame in pixel
		 * @param cameraFrameHeight The height of the camera frame in pixel
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param panoramaFrame The resulting sub-frame of the entire (possible maximal) panorama frame
		 * @param panoramaMask The mask frame corresponding with the resulting panorama frame (with same dimension)
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value defining a valid mask pixel
		 * @param firstPanoramaRow First panorama frame row to be handled
		 * @param numberPanoramaRows The number of panorama frame rows to be handled
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static void cameraFrame2panoramaFrameLookup8BitPerChannelSubset(const LookupTable* lookupTable, const uint8_t* cameraFrame, const unsigned int cameraFrameWidth, const unsigned int cameraFrameHeight, const unsigned int cameraFramePaddingElements, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const uint8_t maskValue, const unsigned int firstPanoramaRow, const unsigned int numberPanoramaRows);

		/**
		 * Copies (interpolates) a subset of an 8 bit per data channel camera frame (specified by a mask) with specified camera orientation to subset of a section of an entire panorama frame.
		 * @param pinholeCamera The pinhole camera profile of the resulting camera frame
		 * @param cameraFrame The camera frame to be copied, must be valid
		 * @param cameraMask The mask of camera frame specifying valid pixels, must be valid
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param cameraMaskPaddingElements The number of padding elements at the end of each camera mask row, in elements, with range [0, infinity)
		 * @param orientation The orientation of the given camera frame
		 * @param panoramaDimensionWidth The maximal width of the entire maximal possible panorama frame, in pixel
		 * @param panoramaDimensionHeight The maximal height of the entire maximal possible panorama frame, in pixel
		 * @param panoramaFrameTopLeftX The horizontal top left position of the resulting sub-frame of the panorama frame
		 * @param panoramaFrameTopLeftY The vertical top left position of the resulting sub-frame of the panorama frame
		 * @param panoramaFrame The resulting sub-frame of the entire (possible maximal) panorama frame
		 * @param panoramaMask The mask frame corresponding with the resulting panorama frame (with same dimension)
		 * @param panoramaFrameWidth The width of the resulting panorama frame in pixel, with range [1, panoramaDimensionWidth]
		 * @param panoramaFrameHeight The height of the resulting panorama frame in pixel, with range [1, panoramaDimensionHeight]
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value defining a valid mask pixel
		 * @param fineAdjustment Optional transformation lookup table with relative offsets providing a fine adjustment for the given camera frame
		 * @param firstPanoramaRow First panorama frame row to be handled
		 * @param numberPanoramaRows The number of panorama frame rows to be handled
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static void cameraFrame2panoramaFrameMask8BitPerChannelSubset(const PinholeCamera* pinholeCamera, const uint8_t* cameraFrame, const uint8_t* cameraMask, const unsigned int cameraFramePaddingElements, const unsigned int cameraMaskPaddingElements, const SquareMatrix3* orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const int panoramaFrameTopLeftX, const int panoramaFrameTopLeftY, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const uint8_t maskValue, const LookupTable* fineAdjustment, const unsigned int firstPanoramaRow, const unsigned int numberPanoramaRows);

		/**
		 * Copies (interpolates) a subset of an 8 bit per data channel camera frame (specified by a mask) with specified camera orientation to subset of a section of an entire panorama frame.
		 * @param lookupTable The 2D lookup table for the camera frame mapping positions defined in the camera frame to positions defined in the panorama frame
		 * @param cameraFrame The camera frame to be copied, must be valid
		 * @param cameraMask The mask of camera frame specifying valid pixels, must be valid
		 * @param cameraFrameWidth The width of the camera frame in pixel
		 * @param cameraFrameHeight The height of the camera frame in pixel
		 * @param cameraFramePaddingElements The number of padding elements at the end of each camera frame row, in elements, with range [0, infinity)
		 * @param cameraMaskPaddingElements The number of padding elements at the end of each camera mask row, in elements, with range [0, infinity)
		 * @param panoramaFrame The resulting sub-frame of the entire (possible maximal) panorama frame
		 * @param panoramaMask The mask frame corresponding with the resulting panorama frame (with same dimension)
		 * @param panoramaFramePaddingElements The number of padding elements at the end of each panorama frame row, in elements, with range [0, infinity)
		 * @param panoramaMaskPaddingElements The number of padding elements at the end of each panorama mask row, in elements, with range [0, infinity)
		 * @param maskValue The mask value defining a valid mask pixel
		 * @param firstPanoramaRow First panorama frame row to be handled
		 * @param numberPanoramaRows The number of panorama frame rows to be handled
		 * @tparam tChannels The number of frame data channels
		 */
		template <unsigned int tChannels>
		static void cameraFrame2panoramaFrameMaskLookup8BitPerChannelSubset(const LookupTable* lookupTable, const uint8_t* cameraFrame, const uint8_t* cameraMask, const unsigned int cameraFrameWidth, const unsigned int cameraFrameHeight, const unsigned int cameraFramePaddingElements, const unsigned int cameraMaskPaddingElements, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const uint8_t maskValue, const unsigned int firstPanoramaRow, const unsigned int numberPanoramaRows);

		/**
		 * Creates are contour of (the outside of) a camera frame
		 *
		 * @param width The width of the camera frame, range: [1, infinity)
		 * @param height The height of the camera frame, range: [1, infinity)
		 * @param count Number of segments each edge is split into, range: [1, infinity)
		 * @return The contour in counter-clockwise order
		 */
		static Vectors2 generateFrameContour(const unsigned int width, const unsigned int height, const unsigned int count);

	protected:

		/// The actual panorama frame, may be a sub-frame of the entire panorama frame.
		Frame frame_;

		/// The mask of the panorama frame defining valid and invalid pixels.
		Frame mask_;

		/// The optional nominator frame of the panorama frame, necessary if UM_AVERAGE_GLOBAL is set as update mode.
		Frame nominatorFrame_;

		/// The optional denominator frame of the panorama frame, necessary if UM_AVERAGE_GLOBAL is set as update mode.
		Frame denominatorFrame_;

		/// The maximal width of the panorama frame representing horizontal 360 degrees, in pixel.
		unsigned int dimensionWidth_ = 0u;

		/// The maximal height of the panorama frame representing vertical 180 degrees, in pixel.
		unsigned int dimensionHeight_ = 0u;

		/// The inverse of the maximal width of the panorama frame, panoramaInvDimensionWidth = (1 / panoramaDimensionWidth).
		Scalar invertedDimensionWidth_ = Scalar(0);

		/// The inverse of the maximal height of the panorama frame, panoramaInvDimensionHeight = (1 / panoramaDimensionHeight).
		Scalar invertedDimensionHeight_ = Scalar(0);

		/// The top left position of the sub-frame of the entire panorama frame.
		PixelPosition frameTopLeft_ = CV::PixelPosition((unsigned int)(-1), (unsigned int)(-1));

		/// The mask value defining the mask value of valid pixels.
		uint8_t maskValue_ = 0xFFu;

		/// The update mode of this panorama frame.
		UpdateMode updateMode_ = UM_INVALID;
};

inline PanoramaFrame::PanoramaFrame(const unsigned int dimensionWidth, const unsigned int dimensionHeight, const uint8_t maskValue, const UpdateMode updateMode) :
	dimensionWidth_(dimensionWidth),
	dimensionHeight_(dimensionHeight),
	invertedDimensionWidth_(dimensionWidth != 0u ? Scalar(1) / Scalar(dimensionWidth) : 0),
	invertedDimensionHeight_(dimensionHeight != 0u ? Scalar(1) / Scalar(dimensionHeight) : 0),
	frameTopLeft_((unsigned int)(-1), (unsigned int)(-1)),
	maskValue_(maskValue),
	updateMode_(updateMode)
{
	ocean_assert(updateMode_ != UM_INVALID);
}

inline const Frame& PanoramaFrame::frame() const
{
	return frame_;
}

inline const Frame& PanoramaFrame::mask() const
{
	return mask_;
}

inline uint8_t PanoramaFrame::maskValue() const
{
	return maskValue_;
}

inline PanoramaFrame::UpdateMode PanoramaFrame::updateMode() const
{
	return updateMode_;
}

inline const PixelPosition& PanoramaFrame::frameTopLeft() const
{
	return frameTopLeft_;
}

inline unsigned int PanoramaFrame::dimensionWidth() const
{
	return dimensionWidth_;
}

inline unsigned int PanoramaFrame::dimensionHeight() const
{
	return dimensionHeight_;
}

inline bool PanoramaFrame::extractFrame(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, Frame& frame, Frame& mask, const unsigned int approximationBinSize, Worker* worker, const LookupTable* fineAdjustment) const
{
	ocean_assert(frame_.isValid() && pinholeCamera.isValid() && !orientation.isSingular());
	if (!frame_.isValid() || !pinholeCamera.isValid() || orientation.isSingular())
	{
		return false;
	}

	return panoramaFrame2cameraFrame(pinholeCamera, frame_, mask_, dimensionWidth_, dimensionHeight_, frameTopLeft_, orientation, frame, mask, maskValue_, approximationBinSize, worker, fineAdjustment);
}

inline Vector2 PanoramaFrame::cameraPixel2panoramaPixel(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, const Vector2& cameraPixel, const Vector2& referenceAngle) const
{
	ocean_assert(pinholeCamera.isValid() && !orientation.isSingular());

	ocean_assert(cameraPixel.x() >= Scalar(0) && cameraPixel.x() < Scalar(pinholeCamera.width()));
	ocean_assert(cameraPixel.y() >= Scalar(0) && cameraPixel.y() < Scalar(pinholeCamera.height()));

	const Vector3 ray(orientation * pinholeCamera.vector(pinholeCamera.undistort<true>(cameraPixel)));

	const Vector2 angle(ray2angle(ray, referenceAngle));
	const Vector2 panoramaPosition(angle2pixel(angle, dimensionWidth_, dimensionHeight_));

	return panoramaPosition;
}

inline Vector2 PanoramaFrame::cameraPixel2panoramaPixelStrict(const PinholeCamera& pinholeCamera, const SquareMatrix3& orientation, const Vector2& cameraPixel) const
{
	ocean_assert(pinholeCamera.isValid() && !orientation.isSingular());

	ocean_assert(cameraPixel.x() >= Scalar(0) && cameraPixel.x() < Scalar(pinholeCamera.width()));
	ocean_assert(cameraPixel.y() >= Scalar(0) && cameraPixel.y() < Scalar(pinholeCamera.height()));

	const Vector3 ray(orientation * pinholeCamera.vector(pinholeCamera.undistort<true>(cameraPixel)));

	const Vector2 angle(ray2angleStrict(ray));
	const Vector2 panoramaPosition(angle2pixel(angle, dimensionWidth_, dimensionHeight_));

	return panoramaPosition;
}

inline Vector2 PanoramaFrame::panoramaPixel2cameraPixel(const PinholeCamera& pinholeCamera, const SquareMatrix3& iOrientation, const Vector2& panoramaPixel) const
{
	ocean_assert(pinholeCamera.isValid() && !iOrientation.isSingular());

	const Vector2 angle(pixel2angle(panoramaPixel, invertedDimensionWidth_, invertedDimensionHeight_));
	const Vector3 ray(iOrientation * angle2ray(angle));
	ocean_assert(ray.z() < 0);

	const Vector3 rayOnPlane(ray * Scalar(-1) / ray.z());
	ocean_assert(rayOnPlane.z() < 0);

	const Vector2 cameraPosition(pinholeCamera.normalizedImagePoint2imagePoint<true>(Vector2(rayOnPlane.x(), -rayOnPlane.y()), true));

	return cameraPosition;
}

inline bool PanoramaFrame::isValid() const
{
	return dimensionWidth_ != 0u && dimensionHeight_ != 0u;
}

inline PanoramaFrame::operator bool() const
{
	return isValid();
}

inline Vector2 PanoramaFrame::angle2pixel(const Vector2& angle) const
{
	return angle2pixel(angle, dimensionWidth_, dimensionHeight_);
}

inline Vector2 PanoramaFrame::pixel2angle(const Vector2& position) const
{
	ocean_assert(position.x() >= 0 && position.x() < Scalar(dimensionWidth_));
	ocean_assert(position.y() >= 0 && position.y() < Scalar(dimensionHeight_));

	ocean_assert(invertedDimensionWidth_ == Scalar(1) / Scalar(dimensionWidth_));
	ocean_assert(invertedDimensionHeight_ == Scalar(1) / Scalar(dimensionHeight_));

	return pixel2angle(position, invertedDimensionWidth_, invertedDimensionHeight_);
}

inline Vector2 PanoramaFrame::angle2pixel(const Vector2& angle, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight)
{
	const Scalar x = ((angle[0] + Numeric::pi()) * Scalar(panoramaDimensionWidth)) * Scalar(0.15915494309189533576888376337251); // 1 / 2PI
	const Scalar y = ((-angle[1] + Numeric::pi_2()) * Scalar(panoramaDimensionHeight)) * Scalar(0.31830988618379067153776752674503); // 1 / PI

	return Vector2(x, y);
}

inline Vector2 PanoramaFrame::angle2pixelStrict(const Vector2& angle, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight)
{
	ocean_assert(angle[0] >= -Numeric::pi() && angle[0] < Numeric::pi());
	ocean_assert(angle[1] >= -Numeric::pi_2() && angle[1] <= Numeric::pi_2());

	const Scalar x = ((angle[0] + Numeric::pi()) * Scalar(panoramaDimensionWidth)) * Scalar(0.15915494309189533576888376337251); // 1 / 2PI
	const Scalar y = ((-angle[1] + Numeric::pi_2()) * Scalar(panoramaDimensionHeight)) * Scalar(0.31830988618379067153776752674503); // 1 / PI

	ocean_assert(x >= 0 && x < Scalar(panoramaDimensionWidth));
	ocean_assert(y >= 0 && y < Scalar(panoramaDimensionHeight));

	return Vector2(x, y);
}

inline Vector2 PanoramaFrame::pixel2angle(const Vector2& position, const Scalar invPanoramaDimensionWidth, const Scalar invPanoramaDimensionHeight)
{
	ocean_assert(invPanoramaDimensionWidth > 0 && invPanoramaDimensionWidth <= 1);
	ocean_assert(invPanoramaDimensionHeight > 0 && invPanoramaDimensionHeight <= 1);

	const Scalar latitude = (position.x() * Numeric::pi2()) * invPanoramaDimensionWidth - Numeric::pi();
	const Scalar longitude = Numeric::pi_2() - (position.y() * Numeric::pi()) * invPanoramaDimensionHeight;

	return Vector2(latitude, longitude);
}

inline Vector2 PanoramaFrame::pixel2angleStrict(const Vector2& position, const Scalar invPanoramaDimensionWidth, const Scalar invPanoramaDimensionHeight)
{
	ocean_assert(invPanoramaDimensionWidth > 0 && invPanoramaDimensionWidth <= 1);
	ocean_assert(invPanoramaDimensionHeight > 0 && invPanoramaDimensionHeight <= 1);

	ocean_assert(position.x() >= 0 && position.x() <= Scalar(1) / Scalar(invPanoramaDimensionWidth));
	ocean_assert(position.y() >= 0 && position.y() <= Scalar(1) / Scalar(invPanoramaDimensionHeight));

	const Scalar latitude = (position.x() * Numeric::pi2()) * invPanoramaDimensionWidth - Numeric::pi();
	const Scalar longitude = Numeric::pi_2() - (position.y() * Numeric::pi()) * invPanoramaDimensionHeight;

	ocean_assert(latitude >= -Numeric::pi() && latitude < Numeric::pi());
	ocean_assert(longitude >= -Numeric::pi_2() && longitude <= Numeric::pi_2());

	return Vector2(latitude, longitude);
}

inline Vector2 PanoramaFrame::ray2angle(const Vector3& ray, const Vector2& referenceAngle)
{
	ocean_assert(Numeric::isEqual(ray.length(), 1));

	Scalar longitude = (Numeric::isEqualEps(ray.x()) && Numeric::isEqualEps(-ray.z())) ? Scalar(0) : Numeric::atan2(ray.x(), -ray.z());
	Scalar latitude = Numeric::asin(ray.y());

	ocean_assert(longitude >= -Numeric::pi() && longitude <= Numeric::pi());
	ocean_assert(latitude >= -Numeric::pi_2() && latitude <= Numeric::pi_2());

	const Scalar longitudeMinus = longitude - Numeric::pi2();
	const Scalar longitudePlus = longitude + Numeric::pi2();

	if (Numeric::abs(referenceAngle.x() - longitude) > Numeric::abs(referenceAngle.x() - longitudeMinus))
	{
		longitude = longitudeMinus;
	}
	if (Numeric::abs(referenceAngle.x() - longitude) > Numeric::abs(referenceAngle.x() - longitudePlus))
	{
		longitude = longitudePlus;
	}

	const Scalar latitudeMinus = latitude - Numeric::pi();
	const Scalar latitudePlus = latitude + Numeric::pi();

	if (Numeric::abs(referenceAngle.y() - latitude) > Numeric::abs(referenceAngle.y() - latitudeMinus))
	{
		latitude = latitudeMinus;
	}
	if (Numeric::abs(referenceAngle.y() - latitude) > Numeric::abs(referenceAngle.y() - latitudePlus))
	{
		latitude = latitudePlus;
	}

	return Vector2(longitude, latitude);
}

inline Vector2 PanoramaFrame::ray2angleStrict(const Vector3& ray)
{
	const Scalar longitude = (Numeric::isEqualEps(ray.x()) && Numeric::isEqualEps(-ray.z())) ? Scalar(0) : Numeric::atan2(ray.x(), -ray.z());
	const Scalar latitude = Numeric::asin(ray.y());

	ocean_assert(longitude >= -Numeric::pi() && longitude <= Numeric::pi());
	ocean_assert(latitude >= -Numeric::pi_2() && latitude <= Numeric::pi_2());

	if (longitude == Numeric::pi())
	{
		return Vector2(-Numeric::pi(), latitude);
	}
	else
	{
		return Vector2(longitude, latitude);
	}
}

inline Vector3 PanoramaFrame::angle2ray(const Vector2& angle)
{
	const Scalar diagonal = Numeric::cos(angle[1]);

	const Vector3 ray(diagonal * Numeric::sin(angle[0]), Numeric::sin(angle[1]), -diagonal * Numeric::cos(angle[0]));
	ocean_assert(Numeric::isEqual(ray.length(), 1));

	return ray;
}

inline Vector3 PanoramaFrame::angle2rayStrict(const Vector2& angle)
{
	ocean_assert(angle[0] >= -Numeric::pi() && angle[0] < Numeric::pi());
	ocean_assert(angle[1] >= -Numeric::pi_2() && angle[1] <= Numeric::pi_2());

	const Scalar diagonal = Numeric::cos(angle[1]);

	const Vector3 ray(diagonal * Numeric::sin(angle[0]), Numeric::sin(angle[1]), -diagonal * Numeric::cos(angle[0]));
	ocean_assert(Numeric::isEqual(ray.length(), 1));

	return ray;
}

template <unsigned int tChannels>
inline void PanoramaFrame::cameraFrame2cameraFrame8BitPerChannel(const PinholeCamera& inputCamera, const SquareMatrix3& inputOrientation, const uint8_t* inputFrame, const uint8_t* inputMask, const unsigned int inputFramePaddingElements, const unsigned int inputMaskPaddingElements, const PinholeCamera& outputCamera, const SquareMatrix3& outputOrientation, uint8_t* outputFrame, uint8_t* outputMask, const unsigned int outputFramePaddingElements, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int approximationBinSize, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(inputCamera.isValid() && !inputOrientation.isSingular());
	ocean_assert(outputCamera.isValid() && !outputOrientation.isSingular());

	ocean_assert(inputFrame != nullptr && outputFrame != nullptr && outputMask != nullptr);

	if (approximationBinSize <= 1u)
	{
		if (inputMask != nullptr)
		{
			if (worker != nullptr)
			{
				worker->executeFunction(Worker::Function::createStatic(&PanoramaFrame::cameraFrame2cameraFrameMask8BitPerChannelSubset<tChannels>, &inputCamera, &inputOrientation, inputFrame, inputMask, inputFramePaddingElements, inputMaskPaddingElements, &outputCamera, &outputOrientation, outputFrame, outputMask, outputFramePaddingElements, outputMaskPaddingElements, maskValue, 0u, 0u), 0u, outputCamera.height());
			}
			else
			{
				cameraFrame2cameraFrameMask8BitPerChannelSubset<tChannels>(&inputCamera, &inputOrientation, inputFrame, inputMask, inputFramePaddingElements, inputMaskPaddingElements, &outputCamera, &outputOrientation, outputFrame, outputMask, outputFramePaddingElements, outputMaskPaddingElements, maskValue, 0u, outputCamera.height());
			}
		}
		else
		{
			if (worker != nullptr)
			{
				worker->executeFunction(Worker::Function::createStatic(&PanoramaFrame::cameraFrame2cameraFrame8BitPerChannelSubset<tChannels>, &inputCamera, &inputOrientation, inputFrame, inputFramePaddingElements, &outputCamera, &outputOrientation, outputFrame, outputMask, outputFramePaddingElements, outputMaskPaddingElements, maskValue, 0u, 0u), 0u, outputCamera.height());
			}
			else
			{
				cameraFrame2cameraFrame8BitPerChannelSubset<tChannels>(&inputCamera, &inputOrientation, inputFrame, inputFramePaddingElements, &outputCamera, &outputOrientation, outputFrame, outputMask, outputFramePaddingElements, outputMaskPaddingElements, maskValue, 0u, outputCamera.height());
			}
		}
	}
	else
	{
		const unsigned int binsX = min(outputCamera.width() / approximationBinSize, outputCamera.width() / 4u);
		const unsigned int binsY = min(outputCamera.height() / approximationBinSize, outputCamera.height() / 4u);
		LookupTable lookupTable(outputCamera.width(), outputCamera.height(), binsX, binsY);

		cameraFrame2cameraFrameLookupTable(inputCamera, inputOrientation, outputCamera, outputOrientation, lookupTable);

		if (inputMask != nullptr)
		{
			if (worker != nullptr)
			{
				worker->executeFunction(Worker::Function::createStatic(&cameraFrame2cameraFrameMaskLookup8BitPerChannelSubset<tChannels>, (const LookupTable*)&lookupTable, inputFrame, inputMask, inputCamera.width(), inputCamera.height(), inputFramePaddingElements, inputMaskPaddingElements, outputFrame, outputMask, outputFramePaddingElements, outputMaskPaddingElements, maskValue, 0u, 0u), 0u, outputCamera.height());
			}
			else
			{
				cameraFrame2cameraFrameMaskLookup8BitPerChannelSubset<tChannels>((const LookupTable*)&lookupTable, inputFrame, inputMask, inputCamera.width(), inputCamera.height(), inputFramePaddingElements, inputMaskPaddingElements, outputFrame, outputMask, outputFramePaddingElements, outputMaskPaddingElements, maskValue, 0u, outputCamera.height());
			}
		}
		else
		{
			if (worker != nullptr)
			{
				worker->executeFunction(Worker::Function::createStatic(&cameraFrame2cameraFrameLookup8BitPerChannelSubset<tChannels>, (const LookupTable*)&lookupTable, inputFrame, inputCamera.width(), inputCamera.height(), inputFramePaddingElements, outputFrame, outputMask, outputFramePaddingElements, outputMaskPaddingElements, maskValue, 0u, 0u), 0u, outputCamera.height());
			}
			else
			{
				cameraFrame2cameraFrameLookup8BitPerChannelSubset<tChannels>((const LookupTable*)&lookupTable, inputFrame, inputCamera.width(), inputCamera.height(), inputFramePaddingElements, outputFrame, outputMask, outputFramePaddingElements, outputMaskPaddingElements, maskValue, 0u, outputCamera.height());
			}
		}
	}
}

template <unsigned int tChannels>
inline void PanoramaFrame::panoramaFrame2cameraFrame8BitPerChannel(const PinholeCamera& pinholeCamera, const uint8_t* panoramaFrame, const uint8_t* panoramaMask, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPosition& panoramaFrameTopLeft, const SquareMatrix3& orientation, uint8_t* cameraFrame, uint8_t* cameraMask, const unsigned int cameraFramePaddingElements, const unsigned int cameraMaskPaddingElements, const uint8_t maskValue, const unsigned int approximationBinSize, Worker* worker, const LookupTable* fineAdjustment)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(pinholeCamera.isValid() && !orientation.isSingular());
	ocean_assert(panoramaFrame != nullptr && panoramaMask != nullptr);
	ocean_assert(panoramaFrameWidth != 0u && panoramaFrameHeight != 0u);
	ocean_assert(cameraFrame != nullptr && cameraMask != nullptr);

	if (approximationBinSize <= 1u)
	{
		if (worker != nullptr)
		{
			worker->executeFunction(Worker::Function::createStatic(&PanoramaFrame::panoramaFrame2cameraFrame8BitPerChannelSubset<tChannels>, &pinholeCamera, panoramaFrame, panoramaMask, panoramaFrameWidth, panoramaFrameHeight, panoramaFramePaddingElements, panoramaMaskPaddingElements, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft.x(), panoramaFrameTopLeft.y(), &orientation, cameraFrame, cameraMask, cameraFramePaddingElements, cameraMaskPaddingElements, maskValue, fineAdjustment, 0u, 0u), 0u, pinholeCamera.height());
		}
		else
		{
			panoramaFrame2cameraFrame8BitPerChannelSubset<tChannels>(&pinholeCamera, panoramaFrame, panoramaMask, panoramaFrameWidth, panoramaFrameHeight, panoramaFramePaddingElements, panoramaMaskPaddingElements, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft.x(), panoramaFrameTopLeft.y(), &orientation, cameraFrame, cameraMask, cameraFramePaddingElements, cameraMaskPaddingElements, maskValue, fineAdjustment, 0u, pinholeCamera.height());
		}
	}
	else
	{
		const unsigned int binsX = min(pinholeCamera.width() / approximationBinSize, pinholeCamera.width() / 4u);
		const unsigned int binsY = min(pinholeCamera.height() / approximationBinSize, pinholeCamera.height() / 4u);
		LookupTable lookupTable(pinholeCamera.width(), pinholeCamera.height(), binsX, binsY);

		panoramaFrame2cameraFrameLookupTable(pinholeCamera, orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, lookupTable, fineAdjustment);

		if (worker != nullptr)
		{
			worker->executeFunction(Worker::Function::createStatic(&panoramaFrame2cameraFrameLookup8BitPerChannelSubset<tChannels>, (const LookupTable*)(&lookupTable), panoramaFrame, panoramaMask, panoramaFramePaddingElements, panoramaMaskPaddingElements, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft.x(), panoramaFrameTopLeft.y(), panoramaFrameWidth, panoramaFrameHeight, cameraFrame, cameraMask, cameraFramePaddingElements, cameraMaskPaddingElements, maskValue, 0u, 0u), 0u, pinholeCamera.height());
		}
		else
		{
			panoramaFrame2cameraFrameLookup8BitPerChannelSubset<tChannels>((const LookupTable*)(&lookupTable), panoramaFrame, panoramaMask, panoramaFramePaddingElements, panoramaMaskPaddingElements, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft.x(), panoramaFrameTopLeft.y(), panoramaFrameWidth, panoramaFrameHeight, cameraFrame, cameraMask, cameraFramePaddingElements, cameraMaskPaddingElements, maskValue, 0u, pinholeCamera.height());
		}
	}
}

template <unsigned int tChannels>
inline void PanoramaFrame::cameraFrame2panoramaFrame8BitPerChannel(const PinholeCamera& pinholeCamera, const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const SquareMatrix3& orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPositionI& panoramaFrameTopLeft, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const uint8_t maskValue, const unsigned int approximationBinSize, Worker* worker, const LookupTable* fineAdjustment)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(pinholeCamera.isValid() && !orientation.isSingular());
	ocean_assert(cameraFrame != nullptr);
	ocean_assert(panoramaFrame != nullptr && panoramaMask != nullptr);
	ocean_assert(panoramaFrameWidth != 0u && panoramaFrameHeight != 0u);

	if (approximationBinSize <= 1u)
	{
		if (worker != nullptr)
		{
			worker->executeFunction(Worker::Function::createStatic(&cameraFrame2panoramaFrame8BitPerChannelSubset<tChannels>, &pinholeCamera, cameraFrame, cameraFramePaddingElements, &orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft.x(), panoramaFrameTopLeft.y(), panoramaFrame, panoramaMask, panoramaFrameWidth, panoramaFrameHeight, panoramaFramePaddingElements, panoramaMaskPaddingElements, maskValue, fineAdjustment, 0u, 0u), 0u, panoramaFrameHeight);
		}
		else
		{
			cameraFrame2panoramaFrame8BitPerChannelSubset<tChannels>(&pinholeCamera, cameraFrame, cameraFramePaddingElements, &orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft.x(), panoramaFrameTopLeft.y(), panoramaFrame, panoramaMask, panoramaFrameWidth, panoramaFrameHeight, panoramaFramePaddingElements, panoramaMaskPaddingElements, maskValue, fineAdjustment, 0u, panoramaFrameHeight);
		}
	}
	else
	{
		const unsigned int binSize = approximationBinSize != 0u ? approximationBinSize : 20u;

		const unsigned int binsX = min(panoramaFrameWidth / binSize, panoramaFrameWidth / 4u);
		const unsigned int binsY = min(panoramaFrameHeight / binSize, panoramaFrameHeight / 4u);
		LookupTable lookupTable(panoramaFrameWidth, panoramaFrameHeight, binsX, binsY);

		cameraFrame2panoramaFrameLookupTable(pinholeCamera, orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, lookupTable, fineAdjustment);

		if (worker != nullptr)
		{
			worker->executeFunction(Worker::Function::createStatic(&cameraFrame2panoramaFrameLookup8BitPerChannelSubset<tChannels>, (const LookupTable*)(&lookupTable), cameraFrame, pinholeCamera.width(), pinholeCamera.height(), cameraFramePaddingElements, panoramaFrame, panoramaMask, panoramaFramePaddingElements, panoramaMaskPaddingElements, maskValue, 0u, 0u), 0u, panoramaFrameHeight);
		}
		else
		{
			cameraFrame2panoramaFrameLookup8BitPerChannelSubset<tChannels>(&lookupTable, cameraFrame, pinholeCamera.width(), pinholeCamera.height(), cameraFramePaddingElements, panoramaFrame, panoramaMask, panoramaFramePaddingElements, panoramaMaskPaddingElements, maskValue, 0u, panoramaFrameHeight);
		}
	}
}

template <unsigned int tChannels>
inline void PanoramaFrame::cameraFrame2panoramaFrameMask8BitPerChannel(const PinholeCamera& pinholeCamera, const uint8_t* cameraFrame, const uint8_t* cameraMask, const unsigned int cameraFramePaddingElements, const unsigned int cameraMaskPaddingElements, const SquareMatrix3& orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const PixelPositionI& panoramaFrameTopLeft, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const uint8_t maskValue, const unsigned int approximationBinSize, Worker* worker, const LookupTable* fineAdjustment)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(pinholeCamera.isValid() && !orientation.isSingular());

	ocean_assert(cameraFrame != nullptr && cameraMask != nullptr);
	ocean_assert(panoramaFrame != nullptr && panoramaMask != nullptr);
	ocean_assert(panoramaFrameWidth != 0u && panoramaFrameHeight != 0u);

	if (approximationBinSize <= 1u)
	{
		if (worker != nullptr)
		{
			worker->executeFunction(Worker::Function::createStatic(&cameraFrame2panoramaFrameMask8BitPerChannelSubset<tChannels>, &pinholeCamera, cameraFrame, cameraMask, cameraFramePaddingElements, cameraMaskPaddingElements, &orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft.x(), panoramaFrameTopLeft.y(), panoramaFrame, panoramaMask, panoramaFrameWidth, panoramaFrameHeight, panoramaFramePaddingElements, panoramaMaskPaddingElements, maskValue, fineAdjustment, 0u, 0u), 0u, panoramaFrameHeight);
		}
		else
		{
			cameraFrame2panoramaFrameMask8BitPerChannelSubset<tChannels>(&pinholeCamera, cameraFrame, cameraMask, cameraFramePaddingElements, cameraMaskPaddingElements, &orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft.x(), panoramaFrameTopLeft.y(), panoramaFrame, panoramaMask, panoramaFrameWidth, panoramaFrameHeight, panoramaFramePaddingElements, panoramaMaskPaddingElements, maskValue, fineAdjustment, 0u, panoramaFrameHeight);
		}
	}
	else
	{
		const unsigned int binSize = approximationBinSize != 0u ? approximationBinSize : 20u;

		const unsigned int binsX = min(panoramaFrameWidth / binSize, panoramaFrameWidth / 4u);
		const unsigned int binsY = min(panoramaFrameHeight / binSize, panoramaFrameHeight / 4u);
		LookupTable lookupTable(panoramaFrameWidth, panoramaFrameHeight, binsX, binsY);

		cameraFrame2panoramaFrameLookupTable(pinholeCamera, orientation, panoramaDimensionWidth, panoramaDimensionHeight, panoramaFrameTopLeft, lookupTable, fineAdjustment);

		if (worker != nullptr)
		{
			worker->executeFunction(Worker::Function::createStatic(&cameraFrame2panoramaFrameMaskLookup8BitPerChannelSubset<tChannels>, (const LookupTable*)(&lookupTable), cameraFrame, cameraMask, pinholeCamera.width(), pinholeCamera.height(), cameraFramePaddingElements, cameraMaskPaddingElements, panoramaFrame, panoramaMask, panoramaFramePaddingElements, panoramaMaskPaddingElements, maskValue, 0u, 0u), 0u, panoramaFrameHeight);
		}
		else
		{
			cameraFrame2panoramaFrameMaskLookup8BitPerChannelSubset<tChannels>((const LookupTable*)(&lookupTable), cameraFrame, cameraMask, pinholeCamera.width(), pinholeCamera.height(), cameraFramePaddingElements, cameraMaskPaddingElements, panoramaFrame, panoramaMask, panoramaFramePaddingElements, panoramaMaskPaddingElements, maskValue, 0u, panoramaFrameHeight);
		}
	}
}

template <unsigned int tChannels>
inline void PanoramaFrame::mergeSetAll8BitPerChannel(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int panoramaSubFramePaddingElements, const unsigned panoramaSubMaskPaddingElements, const PixelPosition& subTopLeft, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const PixelPosition& panoramaTopLeft, const uint8_t maskValue, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	if (worker != nullptr)
	{
		worker->executeFunction(Worker::Function::createStatic(&PanoramaFrame::mergeSetAll8BitPerChannelSubset<tChannels>, panoramaSubFrame, panoramaSubMask, subFrameWidth, panoramaSubFramePaddingElements, panoramaSubMaskPaddingElements, subTopLeft.x(), subTopLeft.y(), panoramaFrame, panoramaMask, panoramaWidth, panoramaFramePaddingElements, panoramaMaskPaddingElements, panoramaTopLeft.x(), panoramaTopLeft.y(), maskValue, 0u, 0u), 0u, subFrameHeight, 15u, 16u, 20u);
	}
	else
	{
		mergeSetAll8BitPerChannelSubset<tChannels>(panoramaSubFrame, panoramaSubMask, subFrameWidth, panoramaSubFramePaddingElements, panoramaSubMaskPaddingElements, subTopLeft.x(), subTopLeft.y(), panoramaFrame, panoramaMask, panoramaWidth, panoramaFramePaddingElements, panoramaMaskPaddingElements, panoramaTopLeft.x(), panoramaTopLeft.y(), maskValue, 0u, subFrameHeight);
	}
}

template <unsigned int tChannels>
inline void PanoramaFrame::mergeSetNew8BitPerChannel(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int panoramaSubFramePaddingElements, const unsigned panoramaSubMaskPaddingElements, const PixelPosition& subTopLeft, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const PixelPosition& panoramaTopLeft, const uint8_t maskValue, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	if (worker != nullptr)
	{
		worker->executeFunction(Worker::Function::createStatic(&PanoramaFrame::mergeSetNew8BitPerChannelSubset<tChannels>, panoramaSubFrame, panoramaSubMask, subFrameWidth, panoramaSubFramePaddingElements, panoramaSubMaskPaddingElements, subTopLeft.x(), subTopLeft.y(), panoramaFrame, panoramaMask, panoramaWidth, panoramaFramePaddingElements, panoramaMaskPaddingElements, panoramaTopLeft.x(), panoramaTopLeft.y(), maskValue, 0u, 0u), 0u, subFrameHeight, 15u, 16u, 20u);
	}
	else
	{
		mergeSetNew8BitPerChannelSubset<tChannels>(panoramaSubFrame, panoramaSubMask, subFrameWidth, panoramaSubFramePaddingElements, panoramaSubMaskPaddingElements, subTopLeft.x(), subTopLeft.y(), panoramaFrame, panoramaMask, panoramaWidth, panoramaFramePaddingElements, panoramaMaskPaddingElements, panoramaTopLeft.x(), panoramaTopLeft.y(), maskValue, 0u, subFrameHeight);
	}
}

template <unsigned int tChannels>
inline void PanoramaFrame::mergeAverageLocal8BitPerChannel(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int panoramaSubFramePaddingElements, const unsigned panoramaSubMaskPaddingElements, const PixelPosition& subTopLeft, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const PixelPosition& panoramaTopLeft, const uint8_t maskValue, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	if (worker != nullptr)
	{
		worker->executeFunction(Worker::Function::createStatic(&PanoramaFrame::mergeAverageLocal8BitPerChannelSubset<tChannels>, panoramaSubFrame, panoramaSubMask, subFrameWidth, panoramaSubFramePaddingElements, panoramaSubMaskPaddingElements, subTopLeft.x(), subTopLeft.y(), panoramaFrame, panoramaMask, panoramaWidth, panoramaFramePaddingElements, panoramaMaskPaddingElements, panoramaTopLeft.x(), panoramaTopLeft.y(), maskValue, 0u, 0u), 0u, subFrameHeight, 15u, 16u, 20u);
	}
	else
	{
		mergeAverageLocal8BitPerChannelSubset<tChannels>(panoramaSubFrame, panoramaSubMask, subFrameWidth, panoramaSubFramePaddingElements, panoramaSubMaskPaddingElements, subTopLeft.x(), subTopLeft.y(), panoramaFrame, panoramaMask, panoramaWidth, panoramaFramePaddingElements, panoramaMaskPaddingElements, panoramaTopLeft.x(), panoramaTopLeft.y(), maskValue, 0u, subFrameHeight);
	}
}

template <unsigned int tChannels>
inline void PanoramaFrame::mergeAverageGlobal8BitPerChannel(const uint8_t* panoramaSubFrame, const uint8_t* panoramaSubMask, const unsigned int subFrameWidth, const unsigned int subFrameHeight, const unsigned int panoramaSubFramePaddingElements, const unsigned int panoramaSubMaskPaddingElements, const PixelPosition& subTopLeft, uint32_t* panoramaNominatorFrame, uint32_t* panoramaDenominatorFrame, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaWidth, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const PixelPosition& panoramaTopLeft, const uint8_t maskValue, Worker* worker)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	if (worker != nullptr)
	{
		worker->executeFunction(Worker::Function::createStatic(&PanoramaFrame::mergeAverageGlobal8BitPerChannelSubset<tChannels>, panoramaSubFrame, panoramaSubMask, subFrameWidth, panoramaSubFramePaddingElements, panoramaSubMaskPaddingElements, subTopLeft.x(), subTopLeft.y(), panoramaNominatorFrame, panoramaDenominatorFrame, panoramaFrame, panoramaMask, panoramaWidth, panoramaFramePaddingElements, panoramaMaskPaddingElements, panoramaTopLeft.x(), panoramaTopLeft.y(), maskValue, 0u, 0u), 0u, subFrameHeight, 17u, 18u, 20u);
	}
	else
	{
		mergeAverageGlobal8BitPerChannelSubset<tChannels>(panoramaSubFrame, panoramaSubMask, subFrameWidth, panoramaSubFramePaddingElements, panoramaSubMaskPaddingElements, subTopLeft.x(), subTopLeft.y(), panoramaNominatorFrame, panoramaDenominatorFrame, panoramaFrame, panoramaMask, panoramaWidth, panoramaFramePaddingElements, panoramaMaskPaddingElements, panoramaTopLeft.x(), panoramaTopLeft.y(), maskValue, 0u, subFrameHeight);
	}
}

template <unsigned int tChannels>
void PanoramaFrame::cameraFrame2cameraFrame8BitPerChannelSubset(const PinholeCamera* inputCamera, const SquareMatrix3* inputOrientation, const uint8_t* inputFrame, const unsigned int inputFramePaddingElements, const PinholeCamera* outputCamera, const SquareMatrix3* outputOrientation, uint8_t* outputFrame, uint8_t* outputMask, const unsigned int outputFramePaddingElements, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(inputCamera != nullptr && inputCamera->isValid());
	ocean_assert(inputOrientation != nullptr && !inputOrientation->isSingular());

	ocean_assert(outputCamera != nullptr && outputCamera->isValid());
	ocean_assert(outputOrientation != nullptr && !outputOrientation->isSingular());

	ocean_assert(inputFrame != nullptr && outputFrame != nullptr && outputMask != nullptr);

	ocean_assert(firstOutputRow + numberOutputRows <= outputCamera->height());

	const SquareMatrix3 outputOrientationF(*outputOrientation * PinholeCamera::flipMatrix3());
	const SquareMatrix3 inputOrientationF(*inputOrientation * PinholeCamera::flipMatrix3());

	const SquareMatrix3 transformation(inputCamera->intrinsic() * inputOrientationF.inverted() * outputOrientationF * outputCamera->invertedIntrinsic());

	const unsigned int outputFrameStrideElements = outputCamera->width() * tChannels + outputFramePaddingElements;
	const unsigned int outputMaskStrideElements = outputCamera->width() + outputMaskPaddingElements;

	outputFrame += firstOutputRow * outputFrameStrideElements;
	outputMask += firstOutputRow * outputMaskStrideElements;

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		for (unsigned int x = 0u; x < outputCamera->width(); ++x)
		{
			const Vector2 inputPosition_05 = inputCamera->distort<true>(transformation * outputCamera->undistort<true>(Vector2(Scalar(x), Scalar(y)))) + Vector2(Scalar(0.5), Scalar(0.5));

			if (inputPosition_05.x() >= 0 && inputPosition_05.x() <= Scalar(inputCamera->width()) && inputPosition_05.y() >= 0 && inputPosition_05.y() <= Scalar(inputCamera->height()))
			{
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, PC_CENTER>(inputFrame, inputCamera->width(), inputCamera->height(), inputFramePaddingElements, inputPosition_05, outputFrame);
				*outputMask = maskValue;
			}
			else
			{
				*outputMask = 0xFFu - maskValue;
			}

			outputFrame += tChannels;
			outputMask++;
		}

		outputFrame += outputFramePaddingElements;
		outputMask += outputMaskPaddingElements;
	}
}

template <unsigned int tChannels>
void PanoramaFrame::cameraFrame2cameraFrameLookup8BitPerChannelSubset(const LookupTable* lookupTable, const uint8_t* inputFrame, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputFramePaddingElements, uint8_t* outputFrame, uint8_t* outputMask, const unsigned int outputFramePaddingElements, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(lookupTable && !lookupTable->isEmpty());
	ocean_assert(inputFrame);
	ocean_assert(outputFrame && outputMask);

	const unsigned int outputFrameStrideElements = (unsigned int)(lookupTable->sizeX()) * tChannels + outputFramePaddingElements;
	const unsigned int outputMaskStrideElements = (unsigned int)(lookupTable->sizeX()) + outputMaskPaddingElements;

	outputFrame += firstOutputRow * outputFrameStrideElements;
	outputMask += firstOutputRow * outputMaskStrideElements;

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		for (unsigned int x = 0u; x < lookupTable->sizeX(); ++x)
		{
			const Vector2 inputPosition_05 = lookupTable->bilinearValue(Scalar(x), Scalar(y)) + Vector2(Scalar(0.5), Scalar(0.5));

			if (inputPosition_05.x() >= 0 && inputPosition_05.x() <= Scalar(inputWidth) && inputPosition_05.y() >= 0 && inputPosition_05.y() <= Scalar(inputHeight))
			{
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, PC_CENTER>(inputFrame, inputWidth, inputHeight, inputFramePaddingElements, inputPosition_05, outputFrame);
				*outputMask = maskValue;
			}
			else
			{
				*outputMask = 0xFFu - maskValue;
			}

			outputFrame += tChannels;
			outputMask++;
		}

		outputFrame += outputFramePaddingElements;
		outputMask += outputMaskPaddingElements;
	}
}

template <unsigned int tChannels>
void PanoramaFrame::cameraFrame2cameraFrameMask8BitPerChannelSubset(const PinholeCamera* inputCamera, const SquareMatrix3* inputOrientation, const uint8_t* inputFrame, const uint8_t* inputMask, const unsigned int inputFramePaddingElements, const unsigned int inputMaskPaddingElements,const PinholeCamera* outputCamera, const SquareMatrix3* outputOrientation, uint8_t* outputFrame, uint8_t* outputMask, const unsigned int outputFramePaddingElements, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(inputCamera != nullptr  && inputCamera->isValid());
	ocean_assert(inputOrientation != nullptr  && !inputOrientation->isSingular());

	ocean_assert(outputCamera != nullptr  && outputCamera->isValid());
	ocean_assert(outputOrientation != nullptr  && !outputOrientation->isSingular());

	ocean_assert(inputFrame != nullptr  && inputMask != nullptr  && outputFrame != nullptr  && outputMask != nullptr );

	ocean_assert(firstOutputRow + numberOutputRows <= outputCamera->height());

	const SquareMatrix3 outputOrientationF(*outputOrientation * PinholeCamera::flipMatrix3());
	const SquareMatrix3 inputOrientationF(*inputOrientation * PinholeCamera::flipMatrix3());

	const SquareMatrix3 transformation(inputCamera->intrinsic() * inputOrientationF.inverted() * outputOrientationF * outputCamera->invertedIntrinsic());

	const unsigned int outputFrameStrideElements = outputCamera->width() * tChannels + outputFramePaddingElements;
	const unsigned int outputMaskStrideElements = outputCamera->width() + outputMaskPaddingElements;

	outputFrame += firstOutputRow * outputFrameStrideElements;
	outputMask += firstOutputRow * outputMaskStrideElements;

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		for (unsigned int x = 0u; x < outputCamera->width(); ++x)
		{
			const Vector2 inputPosition = inputCamera->distort<true>(transformation * outputCamera->undistort<true>(Vector2(Scalar(x), Scalar(y))));
			AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<tChannels, PC_TOP_LEFT>(inputFrame, inputMask, inputCamera->width(), inputCamera->height(), inputFramePaddingElements, inputMaskPaddingElements, inputPosition, outputFrame, outputMask[x], maskValue);

			outputFrame += tChannels;
		}

		outputFrame += outputFramePaddingElements;
		outputMask += outputMaskStrideElements;
	}
}

template <unsigned int tChannels>
void PanoramaFrame::cameraFrame2cameraFrameMaskLookup8BitPerChannelSubset(const LookupTable* lookupTable, const uint8_t* inputFrame, const uint8_t* inputMask, const unsigned int inputWidth, const unsigned int inputHeight, const unsigned int inputFramePaddingElements, const unsigned int inputMaskPaddingElements, uint8_t* outputFrame, uint8_t* outputMask, const unsigned int outputFramePaddingElements, const unsigned int outputMaskPaddingElements, const uint8_t maskValue, const unsigned int firstOutputRow, const unsigned int numberOutputRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(lookupTable != nullptr && !lookupTable->isEmpty());
	ocean_assert(inputFrame != nullptr && inputMask != nullptr);
	ocean_assert(outputFrame != nullptr && outputMask != nullptr);

	const unsigned int outputFrameStrideElements = (unsigned int)(lookupTable->sizeX()) * tChannels + outputFramePaddingElements;
	const unsigned int outputMaskStrideElements = (unsigned int)(lookupTable->sizeX()) + outputMaskPaddingElements;

	outputFrame += firstOutputRow * outputFrameStrideElements;
	outputMask += firstOutputRow * outputMaskStrideElements;

	for (unsigned int y = firstOutputRow; y < firstOutputRow + numberOutputRows; ++y)
	{
		for (unsigned int x = 0u; x < lookupTable->sizeX(); ++x)
		{
			const Vector2 inputPosition = lookupTable->bilinearValue(Scalar(x), Scalar(y));

			AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<tChannels, PC_TOP_LEFT>(inputFrame, inputMask, inputWidth, inputHeight, inputFramePaddingElements, inputMaskPaddingElements, inputPosition, outputFrame, outputMask[x], maskValue);

			outputFrame += tChannels;
		}

		outputFrame += outputFramePaddingElements;
		outputMask += outputMaskStrideElements;
	}
}

template <unsigned int tChannels>
void PanoramaFrame::panoramaFrame2cameraFrame8BitPerChannelSubset(const PinholeCamera* pinholeCamera, const uint8_t* panoramaFrame, const uint8_t* panoramaMask, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const unsigned int panoramaFrameTopLeftX, const unsigned int panoramaFrameTopLeftY, const SquareMatrix3* orientation, uint8_t* cameraFrame, uint8_t* cameraMask, const unsigned int cameraFramePaddingElements, const unsigned int cameraMaskPaddingElements, const uint8_t maskValue, const LookupTable* fineAdjustment, const unsigned int firstCameraRow, const unsigned int numberCameraRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(pinholeCamera != nullptr && pinholeCamera->isValid());
	ocean_assert(orientation != nullptr && !orientation->isSingular());
	ocean_assert(firstCameraRow + numberCameraRows <= pinholeCamera->height());

	ocean_assert(panoramaFrameTopLeftX + panoramaFrameWidth <= panoramaDimensionWidth);
	ocean_assert(panoramaFrameTopLeftY + panoramaFrameHeight <= panoramaDimensionHeight);
	const Vector2 frameTopLeft_ = Vector2(Scalar(panoramaFrameTopLeftX), Scalar(panoramaFrameTopLeftY));

	const unsigned int cameraFrameStrideElements = pinholeCamera->width() * tChannels + cameraFramePaddingElements;
	const unsigned int cameraMaskStrideElements = pinholeCamera->width() + cameraMaskPaddingElements;

	for (unsigned int y = firstCameraRow; y < firstCameraRow + numberCameraRows; ++y)
	{
		uint8_t* cameraFrameRow = cameraFrame + y * cameraFrameStrideElements;
		uint8_t* cameraMaskRow = cameraMask + y * cameraMaskStrideElements;

		for (unsigned int x = 0u; x < pinholeCamera->width(); ++x)
		{
			Vector2 cameraPosition = Vector2(Scalar(x), Scalar(y));

			if (fineAdjustment)
			{
				cameraPosition += fineAdjustment->bilinearValue(cameraPosition.x(), cameraPosition.y());
			}

			const Vector3 ray(*orientation * pinholeCamera->vector(pinholeCamera->undistort<true>(cameraPosition)));

			const Vector2 angle(ray2angleStrict(ray));
			const Vector2 panoramaPosition(angle2pixel(angle, panoramaDimensionWidth, panoramaDimensionHeight));

			const Vector2 localPanoramaPosition(panoramaPosition - frameTopLeft_);

			AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<tChannels, PC_TOP_LEFT>(panoramaFrame, panoramaMask, panoramaFrameWidth, panoramaFrameHeight, panoramaFramePaddingElements, panoramaMaskPaddingElements, localPanoramaPosition, cameraFrameRow, cameraMaskRow[x], maskValue);

			cameraFrameRow += tChannels;
		}
	}
}

template <unsigned int tChannels>
void PanoramaFrame::panoramaFrame2cameraFrameLookup8BitPerChannelSubset(const LookupTable* lookupTable, const uint8_t* panoramaFrame, const uint8_t* panoramaMask, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const unsigned int panoramaFrameTopLeftX, const unsigned int panoramaFrameTopLeftY, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, uint8_t* cameraFrame, uint8_t* cameraMask, const unsigned int cameraFramePaddingElements, const unsigned int cameraMaskPaddingElements, const uint8_t maskValue, const unsigned int firstCameraRow, const unsigned int numberCameraRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(lookupTable != nullptr && *lookupTable);
	ocean_assert(firstCameraRow + numberCameraRows <= lookupTable->sizeY());

	const unsigned int cameraFrameStrideElements = (unsigned int)(lookupTable->sizeX()) * tChannels + cameraFramePaddingElements;
	const unsigned int cameraMaskStrideElements = (unsigned int)(lookupTable->sizeX()) + cameraMaskPaddingElements;

	const Scalar panoramaDimensionWidth_2 = Scalar(panoramaDimensionWidth) * Scalar(0.5);

	Vector2 topLeft, topRight, bottomLeft, bottomRight;
	Scalar factorTopLeft, factorTopRight, factorBottomLeft, factorBottomRight;

	for (unsigned int y = firstCameraRow; y < firstCameraRow + numberCameraRows; ++y)
	{
		uint8_t* cameraFrameRow = cameraFrame + y * cameraFrameStrideElements;
		uint8_t* cameraMaskRow = cameraMask + y * cameraMaskStrideElements;

		for (unsigned int x = 0u; x < lookupTable->sizeX(); ++x)
		{
			lookupTable->bilinearValue(Scalar(x), Scalar(y), topLeft, topRight, bottomLeft, bottomRight, factorTopLeft, factorTopRight, factorBottomLeft, factorBottomRight);

			if (topRight.x() > topLeft.x() && topRight.x() - topLeft.x() > panoramaDimensionWidth_2)
			{
				topRight.x() -= Scalar(panoramaDimensionWidth);
			}
			else if (topRight.x() < topLeft.x() && topLeft.x() - topRight.x() > panoramaDimensionWidth_2)
			{
				topRight.x() += Scalar(panoramaDimensionWidth);
			}

			if (bottomLeft.x() > topLeft.x() && bottomLeft.x() - topLeft.x() > panoramaDimensionWidth_2)
			{
				bottomLeft.x() -= Scalar(panoramaDimensionWidth);
			}
			else if (bottomLeft.x() < topLeft.x() && topLeft.x() - bottomLeft.x() > panoramaDimensionWidth_2)
			{
				bottomLeft.x() += Scalar(panoramaDimensionWidth);
			}

			if (bottomRight.x() > topLeft.x() && bottomRight.x() - topLeft.x() > panoramaDimensionWidth_2)
			{
				bottomRight.x() -= Scalar(panoramaDimensionWidth);
			}
			else if (bottomRight.x() < topLeft.x() && topLeft.x() - bottomRight.x() > panoramaDimensionWidth_2)
			{
				bottomRight.x() += Scalar(panoramaDimensionWidth);
			}

			Vector2 localPanoramaPosition = topLeft * factorTopLeft + topRight * factorTopRight + bottomLeft * factorBottomLeft + bottomRight * factorBottomRight;

			if (localPanoramaPosition.y() >= Scalar(panoramaDimensionHeight))
			{
				localPanoramaPosition.y() = Scalar(panoramaDimensionHeight * 2u) - localPanoramaPosition.y();

				if (localPanoramaPosition.x() > panoramaDimensionWidth_2)
				{
					localPanoramaPosition.x() -= panoramaDimensionWidth_2;
				}
				else
				{
					localPanoramaPosition.x() += panoramaDimensionWidth_2;
				}
			}
			else if (localPanoramaPosition.y() < Scalar(0))
			{
				localPanoramaPosition.y() = -localPanoramaPosition.y();

				if (localPanoramaPosition.x() > panoramaDimensionWidth_2)
				{
					localPanoramaPosition.x() -= panoramaDimensionWidth_2;
				}
				else
				{
					localPanoramaPosition.x() += panoramaDimensionWidth_2;
				}
			}

			ocean_assert(localPanoramaPosition.y() >= Scalar(0) && localPanoramaPosition.y() <= Scalar(panoramaDimensionHeight));

			if (localPanoramaPosition.x() >= Scalar(panoramaDimensionWidth))
			{
				localPanoramaPosition.x() -= Scalar(panoramaDimensionWidth);
			}
			else if (localPanoramaPosition.x() < Scalar(0))
			{
				localPanoramaPosition.x() += Scalar(panoramaDimensionWidth);
			}

			ocean_assert(localPanoramaPosition.x() >= Scalar(0) && localPanoramaPosition.x() < Scalar(panoramaDimensionWidth));

			localPanoramaPosition.x() -= Scalar(panoramaFrameTopLeftX);
			localPanoramaPosition.y() -= Scalar(panoramaFrameTopLeftY);

			// **TODO** should pixel center be incorporated also above during mirroring/modulo calculation?

			AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<tChannels, PC_TOP_LEFT>(panoramaFrame, panoramaMask, panoramaFrameWidth, panoramaFrameHeight, panoramaFramePaddingElements, panoramaMaskPaddingElements, localPanoramaPosition, cameraFrameRow, cameraMaskRow[x], maskValue);

			cameraFrameRow += tChannels;
		}
	}
}

template <unsigned int tChannels>
void PanoramaFrame::cameraFrame2panoramaFrame8BitPerChannelSubset(const PinholeCamera* pinholeCamera, const uint8_t* cameraFrame, const unsigned int cameraFramePaddingElements, const SquareMatrix3* orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const int panoramaFrameTopLeftX, const int panoramaFrameTopLeftY, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const uint8_t maskValue, const LookupTable* fineAdjustment, const unsigned int firstPanoramaRow, const unsigned int numberPanoramaRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(pinholeCamera != nullptr  && pinholeCamera->isValid());
	ocean_assert(orientation != nullptr  && !orientation->isSingular());
	ocean_assert_and_suppress_unused(firstPanoramaRow + numberPanoramaRows <= panoramaFrameHeight, panoramaFrameHeight);

	const Vector2 frameTopLeft_ = Vector2(Scalar(panoramaFrameTopLeftX), Scalar(panoramaFrameTopLeftY));

	ocean_assert(panoramaDimensionWidth != 0u && panoramaDimensionHeight != 0u);
	const Scalar invPanoramaDimensionWidth = Scalar(1) / Scalar(panoramaDimensionWidth);
	const Scalar invPanoramaDimensionHeight = Scalar(1) / Scalar(panoramaDimensionHeight);

	const SquareMatrix3 invOrientation(orientation->inverted());

	const unsigned int panoramaFrameStrideElements = panoramaFrameWidth * tChannels + panoramaFramePaddingElements;
	const unsigned int panoramaMaskStrideElements = panoramaFrameWidth + panoramaMaskPaddingElements;

	const Vector3 roughPrincipalRay(*orientation * pinholeCamera->vector(Vector2(pinholeCamera->principalPointX(), pinholeCamera->principalPointY())));

	for (unsigned int y = firstPanoramaRow; y < firstPanoramaRow + numberPanoramaRows; ++y)
	{
		uint8_t* panoramaFrameRow = panoramaFrame + y * panoramaFrameStrideElements;
		uint8_t* panoramaMaskRow = panoramaMask + y * panoramaMaskStrideElements;

		for (unsigned int x = 0u; x < panoramaFrameWidth; ++x)
		{
			const Vector2 localPanoramaPosition = Vector2(Scalar(x), Scalar(y));
			const Vector2 panoramaPosition = Vector2(localPanoramaPosition + frameTopLeft_);

			const Vector2 angle(pixel2angle(panoramaPosition, invPanoramaDimensionWidth, invPanoramaDimensionHeight));

			const Vector3 ray(angle2ray(angle));

			if (roughPrincipalRay * ray <= 0)
			{
				// workaround for camera frames cutting the panorama frame into two pieces
				*panoramaMaskRow = 0xFFu - maskValue;
			}
			else
			{
				const Vector3 invRay(invOrientation * ray);
				const Vector3 rayOnPlane(invRay * Scalar(-1) / invRay.z());

				Vector2 cameraPosition(pinholeCamera->normalizedImagePoint2imagePoint<true>(Vector2(rayOnPlane.x(), -rayOnPlane.y()), true));

				if (fineAdjustment)
				{
					cameraPosition += fineAdjustment->clampedBilinearValue(cameraPosition.x(), cameraPosition.y());
				}

				const Vector2 cameraPosition_05 = cameraPosition + Vector2(Scalar(0.5), Scalar(0.5));

				if (cameraPosition_05.x() >= 0 && cameraPosition_05.x() <= Scalar(pinholeCamera->width()) && cameraPosition_05.y() >= 0 && cameraPosition_05.y() <= Scalar(pinholeCamera->height()))
				{
					FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, PC_CENTER>(cameraFrame, pinholeCamera->width(), pinholeCamera->height(), cameraFramePaddingElements, cameraPosition_05, panoramaFrameRow);
					*panoramaMaskRow = maskValue;
				}
				else
				{
					*panoramaMaskRow = 0xFFu - maskValue;
				}
			}

			panoramaFrameRow += tChannels;
			++panoramaMaskRow;
		}
	}
}

template <unsigned int tChannels>
void PanoramaFrame::cameraFrame2panoramaFrameLookup8BitPerChannelSubset(const LookupTable* lookupTable, const uint8_t* cameraFrame, const unsigned int cameraFrameWidth, const unsigned int cameraFrameHeight, const unsigned int cameraFramePaddingElements, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const uint8_t maskValue, const unsigned int firstPanoramaRow, const unsigned int numberPanoramaRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(lookupTable != nullptr && !lookupTable->isEmpty());
	ocean_assert(firstPanoramaRow + numberPanoramaRows <= lookupTable->sizeY());

	const unsigned int panoramaFrameStrideElements = (unsigned int)(lookupTable->sizeX()) * tChannels + panoramaFramePaddingElements;
	const unsigned int panoramaMaskStrideElements = (unsigned int)(lookupTable->sizeX()) + panoramaMaskPaddingElements;

	for (unsigned int y = firstPanoramaRow; y < firstPanoramaRow + numberPanoramaRows; ++y)
	{
		uint8_t* panoramaFrameRow = panoramaFrame + y * panoramaFrameStrideElements;
		uint8_t* panoramaMaskRow = panoramaMask + y * panoramaMaskStrideElements;

		for (unsigned int x = 0u; x < lookupTable->sizeX(); ++x)
		{
			const Vector2 cameraPosition(lookupTable->bilinearValue(Scalar(x), Scalar(y)));
			const Vector2 cameraPosition_05 = cameraPosition + Vector2(Scalar(0.5), Scalar(0.5));

			if (cameraPosition_05.x() >= 0 && cameraPosition_05.x() <= Scalar(cameraFrameWidth) && cameraPosition_05.y() >= 0 && cameraPosition_05.y() <= Scalar(cameraFrameHeight))
			{
				FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<tChannels, PC_CENTER>(cameraFrame, cameraFrameWidth, cameraFrameHeight, cameraFramePaddingElements, cameraPosition_05, panoramaFrameRow);
				*panoramaMaskRow = maskValue;
			}
			else
			{
				*panoramaMaskRow = 0xFFu - maskValue;
			}

			panoramaFrameRow += tChannels;
			++panoramaMaskRow;
		}
	}
}

template <unsigned int tChannels>
void PanoramaFrame::cameraFrame2panoramaFrameMask8BitPerChannelSubset(const PinholeCamera* pinholeCamera, const uint8_t* cameraFrame, const uint8_t* cameraMask, const unsigned int cameraFramePaddingElements, const unsigned int cameraMaskPaddingElements, const SquareMatrix3* orientation, const unsigned int panoramaDimensionWidth, const unsigned int panoramaDimensionHeight, const int panoramaFrameTopLeftX, const int panoramaFrameTopLeftY, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaFrameWidth, const unsigned int panoramaFrameHeight, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const uint8_t maskValue, const LookupTable* fineAdjustment, const unsigned int firstPanoramaRow, const unsigned int numberPanoramaRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(pinholeCamera && pinholeCamera->isValid());
	ocean_assert(orientation && !orientation->isSingular());
	ocean_assert_and_suppress_unused(firstPanoramaRow + numberPanoramaRows <= panoramaFrameHeight, panoramaFrameHeight);
	ocean_assert(fineAdjustment == nullptr || ((unsigned int)fineAdjustment->sizeX() == pinholeCamera->width() && (unsigned int)fineAdjustment->sizeY() == pinholeCamera->height()));

	const Vector2 frameTopLeft_ = Vector2(Scalar(panoramaFrameTopLeftX), Scalar(panoramaFrameTopLeftY));

	ocean_assert(panoramaDimensionWidth != 0u && panoramaDimensionHeight != 0u);
	const Scalar invPanoramaDimensionWidth = Scalar(1) / Scalar(panoramaDimensionWidth);
	const Scalar invPanoramaDimensionHeight = Scalar(1) / Scalar(panoramaDimensionHeight);

	const SquareMatrix3 invOrientation(orientation->inverted());

	const unsigned int panoramaFrameStrideElements = panoramaFrameWidth * tChannels + panoramaFramePaddingElements;
	const unsigned int panoramaMaskStrideElements = panoramaFrameWidth + panoramaMaskPaddingElements;

	for (unsigned int y = firstPanoramaRow; y < firstPanoramaRow + numberPanoramaRows; ++y)
	{
		uint8_t* panoramaFrameRow = panoramaFrame + y * panoramaFrameStrideElements;
		uint8_t* panoramaMaskRow = panoramaMask + y * panoramaMaskStrideElements;

		for (unsigned int x = 0u; x < panoramaFrameWidth; ++x)
		{
			const Vector2 localPanoramaPosition = Vector2(Scalar(x), Scalar(y));
			const Vector2 panoramaPosition = Vector2(localPanoramaPosition + frameTopLeft_);

			const Vector2 angle(pixel2angle(panoramaPosition, invPanoramaDimensionWidth, invPanoramaDimensionHeight));
			const Vector3 ray(invOrientation * angle2ray(angle));
			ocean_assert(ray.z() < 0);

			const Vector3 rayOnPlane(ray * Scalar(-1) / ray.z());
			ocean_assert(rayOnPlane.z() < 0);

			Vector2 cameraPosition(pinholeCamera->normalizedImagePoint2imagePoint<true>(Vector2(rayOnPlane.x(), -rayOnPlane.y()), true));

			if (fineAdjustment)
			{
				cameraPosition += fineAdjustment->clampedBilinearValue(cameraPosition.x(), cameraPosition.y());
			}

			AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<tChannels, PC_TOP_LEFT>(cameraFrame, cameraMask, pinholeCamera->width(), pinholeCamera->height(), cameraFramePaddingElements, cameraMaskPaddingElements, cameraPosition, panoramaFrameRow, panoramaMaskRow[x], maskValue);

			panoramaFrameRow += tChannels;
		}
	}
}

template <unsigned int tChannels>
void PanoramaFrame::cameraFrame2panoramaFrameMaskLookup8BitPerChannelSubset(const LookupTable* lookupTable, const uint8_t* cameraFrame, const uint8_t* cameraMask, const unsigned int cameraFrameWidth, const unsigned int cameraFrameHeight, const unsigned int cameraFramePaddingElements, const unsigned int cameraMaskPaddingElements, uint8_t* panoramaFrame, uint8_t* panoramaMask, const unsigned int panoramaFramePaddingElements, const unsigned int panoramaMaskPaddingElements, const uint8_t maskValue, const unsigned int firstPanoramaRow, const unsigned int numberPanoramaRows)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(lookupTable != nullptr && !lookupTable->isEmpty());
	ocean_assert(firstPanoramaRow + numberPanoramaRows <= lookupTable->sizeY());

	const unsigned int panoramaFrameStrideElements = (unsigned int)(lookupTable->sizeX()) * tChannels + panoramaFramePaddingElements;
	const unsigned int panoramaMaskStrideElements = (unsigned int)(lookupTable->sizeX()) + panoramaMaskPaddingElements;

	for (unsigned int y = firstPanoramaRow; y < firstPanoramaRow + numberPanoramaRows; ++y)
	{
		uint8_t* panoramaFrameRow = panoramaFrame + y * panoramaFrameStrideElements;
		uint8_t* panoramaMaskRow = panoramaMask + y * panoramaMaskStrideElements;

		for (unsigned int x = 0u; x < lookupTable->sizeX(); ++x)
		{
			const Vector2 cameraPosition(lookupTable->bilinearValue(Scalar(x), Scalar(y)));

			AdvancedFrameInterpolatorBilinear::interpolatePixelWithMask8BitPerChannel<tChannels, PC_TOP_LEFT>(cameraFrame, cameraMask, cameraFrameWidth, cameraFrameHeight, cameraFramePaddingElements, cameraMaskPaddingElements, cameraPosition, panoramaFrameRow, panoramaMaskRow[x], maskValue);

			panoramaFrameRow += tChannels;
		}
	}
}

}

}

}

#endif // META_OCEAN_TRACKING_OFFLINE_PANORAMA_FRAME_H
