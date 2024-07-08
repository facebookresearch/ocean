/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include "ocean/cv/detector/qrcodes/QRCodeDetector2D.h"
#include "ocean/cv/detector/qrcodes/FinderPatternDetector.h"
#include "ocean/cv/detector/qrcodes/LegacyQRCodeDetector2D.h"

#include "ocean/base/Worker.h"

#include "ocean/cv/Bresenham.h"
#include "ocean/cv/Canvas.h"

#include "ocean/math/AnyCamera.h"
#include "ocean/math/Vector2.h"

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/**
 * Definition of utility functions related to the detection of QR codes
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT Utilities
{
	public:

		/**
		 * Definition of return codes of the parsing function for the Wi-Fi configurations
		 */
		enum ParsingStatus : uint32_t
		{
			/// Indicates that parsing was successful
			PS_SUCCESS = 0u,
			/// Indicates that the input has an invalid prefix
			PS_ERROR_INVALID_PREFIX,
			/// Indicates that a field uses an invalid format, for example it isn't closed correctly
			PS_ERROR_INVALID_FIELD_FORMAT,
			/// Indicates that the content of a field is invalid, missing, or could not be processed correctly
			PS_ERROR_INVALID_FIELD_DATA,
			/// Indicates that field type is unknown or invalid
			PS_ERROR_INVALID_FIELD_TYPE,
			/// Indicates that a field has been found multiple times when it should have been unique
			PS_ERROR_DUPLICATE_FIELD_TYPE,
			/// Indicates that the config is terminated correctly
			PS_ERROR_INVALID_TERMINATION
		};

		/**
		 * Definition of a helper class to convert between the coordinate systems that are used for QR codes.
		 *
		 * The default coordinate system (code space) for the 2D locations of the modules is defined as follows (similar to images):
		 *   * the x-axis points right,
		 *   * the y-axis points down, and
		 *   * the origin is in the top-left corner of the QR code
		 *   * the pixel origin is in the top-left corner of each pixel
		 *
		 * In the example below, `s = 4 * version + 17` is the number of modules per side:
		 *
		 * <pre>
		 *   (0, 0)          (s, 0)
		 *          o-------+----> x-axis
		 *          |       |
		 *          |       |
		 *          |       |
		 *          +-------+
		 *   (0, s) |         (s, s)
		 *          v
		 *            y-axis
		 * </pre>
		 *
		 * The coordinate system for 3D locations of modules in its (normalized) object space is defined as follows:
		 *  * the x-axis points right in the QR code plane
		 *  * the y-axis points up in the QR code plane
		 *  * the z-axis points upwards (normal of the QR code plane, pointing towards the camera)
		 *  * the origin is in the center of the QR code
		 *  * the pixel origin is in the center of each pixel
		 *
		 * <pre>
		 *               y
		 *   (-1, 1, 0)  ^    (1, 1, 0)
		 *           +---|---+
		 *           |   |   |
		 *           |   o-----> x
		 *           |       |
		 *           +-------+
		 *  (-1, -1, 0)        (1, -1, 0)
		 * </pre>
		 *
		 * The conversion from the first coordinate system (2D) to normalized object space (3D) is defined as follows:
		 *
		 * <pre>
		 *  x' = ((2 / s) * x) - 1
		 *  y' = ((-2 / s) * y) + 1
		 *  z' = 0
		 * </pre>
		 */
		class OCEAN_CV_DETECTOR_QRCODES_EXPORT CoordinateSystem
		{
			public:

				/**
				 * Constructs an coordinate system object
				 * Coordinates are normalized to the range `[-1, 1]` or `[-scale, scale]` for `scale != 1`.
				 * @param version The version for which the coordinate system should be prepared, range: [1, 40]
				 * @param scale Optional scaling factor for the coordinates in the object space, range: (0, infinity)
				 */
				CoordinateSystem(const unsigned int version, const Scalar scale = Scalar(1));

				/**
				 * Returns the scaling factor that is used for coordinate scaling
				 * @return The scaling factor
				 */
				inline Scalar scale() const;

				/**
				 * Converts an x-coordinate from code space to object space
				 * @param xCodeSpace The coordinate that will be converted from code to object space, pixel origin is assumed to be in the pixel center (add 0.5, if applicable)
				 * @return The coordinate converted to object space
				 */
				inline Scalar convertCodeSpaceToObjectSpaceX(const Scalar xCodeSpace) const;

				/**
				 * Converts an y-coordinate from code space to object space
				 * @param yCodeSpace The coordinate that will be converted from code to object space, pixel origin is assumed to be in the pixel center (add 0.5, if applicable)
				 * @return The coordinate converted to object space
				 */
				inline Scalar convertCodeSpaceToObjectSpaceY(const Scalar yCodeSpace) const;

				/**
				 * Compute the locations of the four corners of a code in object space
				 * Coordinates are normalized to the range `[-1, 1]` or `[-scale, scale]` for `scale != 1`.
				 * @param scale Optional scaling factor for the coordinates in the object space, range: (0, infinity)
				 * @return The 4 corners in object space in the order top-left, bottom-left, bottom-right, and top-right.
				 */
				static inline Vectors3 computeCornersInObjectSpace(const Scalar scale = Scalar(1));

				/**
				 * Computes the locations of the centers of the 3 finder patterns for a specific QR code version in object space
				 * Coordinates are normalized to the range `[-1, 1]` or `[-scale, scale]` for `scale != 1`.
				 * @param version The version for which the locations of the finder pattern centers will be computed, range: [1, 40]
				 * @param scale Optional scaling factor for the coordinates in the object space, range: (0, infinity)
				 * @return The 3D locations of the finder pattern centers in object space, will have 3 elements
				 */
				static Vectors3 computeFinderPatternCentersInObjectSpace(const unsigned int version, const Scalar scale = Scalar(1));

				/**
				 * Computes the locations of alignment patterns for a specific QR code version in object space
				 * Coordinates are normalized to the range `[-1, 1]`.
				 * @param version The version for which the locations of the alignment patterns will be computed, range: [1, 40]
				 * @param scale Optional scaling factor for the coordinates in the object space, range: (0, infinity)
				 * @return The 3D locations of the alignment patterns in object space in row-wise order
				 */
				static std::vector<Vectors3> computeAlignmentPatternsInObjectSpace(const unsigned int version, const Scalar scale = Scalar(1));

				/**
				 * Computes the locations of the version information fields for a specific QR code version in object space
				 * Coordinates are normalized to the range `[-1, 1]`.
				 * <pre>
				 *                                version information 1 (6 x 3 modules)
				 *                                |
				 *                                v
				 *   ##############           0 1 2  ##############
				 *   ##          ##           3 4 5  ##          ##
				 *   ##  ######  ##           6 7 8  ##  ######  ##
				 *   ##  ######  ##           9 A B  ##  ######  ##
				 *   ##  ######  ##           C D E  ##  ######  ##
				 *   ##          ##           F G H  ##          ##
				 *   ##############  ##  ##  ##  ##  ##############
				 *
				 *               ##
				 *
				 *               ##
				 *
				 *   0 3 6 9 C F ##
				 *   1 4 7 A D G <-- version information 2 (3 x 6 modules)
				 *   2 5 8 B E H ##
				 *
				 *   ##############
				 *   ##          ##
				 *   ##  ######  ##
				 *   ##  ######  ##
				 *   ##  ######  ##
				 *   ##          ##
				 *   ##############
				 * </pre>
				 *
				 * @param version The version for which the locations of the alignment patterns will be computed, range: [7, 40]
				 * @param versionInformation1 If true, the module locations of the first field in the top right corner will be returned, otherwise the ones of the second field in the lower left corner
				 * @return The 3D locations of the 18 modules of the selected version information field in object space (in the order that they will need to be read)
				 */
				static Vectors3 computeVersionInformationModulesInObjectSpace(const unsigned int version, const bool versionInformation1);

			protected:

				/// Global factor for coordinate scaling
				Scalar scale_;

				/// The scale factor for x-coordinates
				Scalar xScale_;

				/// The scale factor for y-coordinates
				Scalar yScale_;
		};

	public:

		/**
		 * Draws an unscaled image of a QR code (FORMAT_Y8), i.e. one modules corresponds to one pixel
		 * @param code The QR code that will be drawn, must be valid
		 * @param border The border that should be drawn around the QR codes in multiples of modules, range: [0, infinity)
		 * @param foregroundColor The color that the foreground modules (i.e. module value is 1) should have, range: [0, 255], this value must differ from `backgroundColor` by at least 30 intensity steps
		 * @param backgroundColor The color that the background modules (i.e. module value is 0) should have, range: [0, 255], this value must differ from `foregroundColor` by at least 30 intensity steps
		 * @return The frame holding the image of the QR code, will be invalid in case of a failure
		 */
		static Frame draw(const QRCode& code, const unsigned int border, const uint8_t foregroundColor = 0u, const uint8_t backgroundColor = 255u);

		/**
		 * Draws a scaled image of a QR code (FORMAT_Y8)
		 * @param code The QR code that will be drawn, must be valid
		 * @param frameSize The width and height of the resulting frame, range: [qrcode.modulesCount() + 2u * border, infinity)
		 * @param allowTrueMultiple True, then the value of `frameSize` may be increased to smallest multiple of `qrcode.modulesCount() + 2u * border` that is larger or equal to `frameSize`, otherwise the specified value of `frameSize` will be used
		 * @param border The border that should be drawn around the QR codes in multiple of modules, range: [0, infinity)
		 * @param worker Optional worker that is used to draw the QR code
		 * @param foregroundColor The color that the foreground modules (i.e. module value is 1) should have, range: [0, 255], this value must differ from `backgroundColor` by at least 30 intensity steps
		 * @param backgroundColor The color that the background modules (i.e. module value is 0) should have, range: [0, 255], this value must differ from `foregroundColor` by at least 30 intensity steps
		 * @return The frame holding the image of the QR code, will be invalid in case of a failure
		 */
		static Frame draw(const QRCode& code, const unsigned int frameSize, const bool allowTrueMultiple, const unsigned int border, Worker* worker = nullptr, const uint8_t foregroundColor = 0u, const uint8_t backgroundColor = 255u);

		/**
		 * Draws an observation of a QR code into a given frame.
		 * @param frame The frame in which the observation of a QR codes will be drawn, must be valid
		 * @param observation The observation of a QR codes that will be drawn, must be valid
		 * @param code The corresponding QR code
		 */
		static inline void drawObservation(Frame& frame, const LegacyQRCodeDetector2D::Observation& observation, const QRCode& code);

		/**
		 * Draws an observation of a QR code into a given frame.
		 * @param frame The frame in which the observation of a QR codes will be drawn, must be valid
		 * @param frame_H_code The homography transformation that maps from the coordinate frame of the code into the frame, i.e. `pointInFrame = frame_H_code pointInCode`, must be valid
		 * @param finderPatterns The three detected finder patterns that are part of the QR code and which will be drawn into the frame
		 * @param version The version of the QR code, range: [1, 40]
		 * @param modules The modules of the QR codes that will be drawn into the frame, must `QRCode::modulesPerSide(version) * QRCode::modulesPerSide(version)` elements
		 */
		static void drawObservation(Frame& frame, const SquareMatrix3& frame_H_code, const FinderPatternTriplet& finderPatterns, const unsigned int version, const std::vector<uint8_t>& modules);

		/**
		 * Draws observations of QR codes into a given frame.
		 * @param frame The frame in which the observations of the QR codes will be drawn, must be valid
		 * @param observations The observations of QR codes that will be drawn, must be valid
		 * @param codes The corresponding QR codes, must have the same size as `observations`
		 */
		static inline void drawObservations(Frame& frame, const LegacyQRCodeDetector2D::Observations& observations, const QRCodes& codes);

		/**
		 * Draws observations of QR codes into a given frame.
		 * @param anyCamera The camera that was used for the detection of the QR codes, must be valid
		 * @param frame The frame into which observations of the QR codes will be drawn, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with RGB24
		 * @param observations The observations of QR codes that will be drawn, must have the same size as `codes`
		 * @param codes The corresponding QR codes, must have the same size as `observations`
		 */
		static void drawObservations(const AnyCamera& anyCamera, Frame& frame, const QRCodeDetector2D::Observations& observations, const QRCodes& codes);

		/**
		 * Draw the location of a finder pattern in a given frame.
		 * @param frame The frame in which the finder pattern will be drawn, must be valid
		 * @param finderPattern The finder pattern that will be drawn
		 * @param color The color to be used for drawing, one value for each channel of `frame`, must be valid
		 */
		static void drawFinderPattern(Frame& frame, const FinderPattern& finderPattern, const uint8_t* color);

		/**
		 * Draws a line between two 2D points into a frame with lens distortion
		 * @param anyCamera The camera profile that will be used to draw the line, must be valid
		 * @param frame The frame into which the line will be drawn, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with RGB24
		 * @param pointA The first end point of the line segment
		 * @param pointB The second end point of the line segment
		 * @param color Optional color of the line segments
		 * @param steps Optional number of segments that the line will be split into, range: [1, infinity)
		 * @tparam tLineWidth The line of the width, range: [1, infinity), must be an odd value
		 */
		template <unsigned int tLineWidth>
		static void drawLine(const AnyCamera& anyCamera, Frame& frame, const Vector2& pointA, const Vector2& pointB, const uint8_t* color = nullptr, const size_t steps = 10);

		/**
		 * Draws a line between two 3D points into a frame with lens distortion
		 * @param anyCamera The camera profile that will be used to draw the line, must be valid
		 * @param frame The frame into which the line will be drawn, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with RGB24
		 * @param pointA The first end point of the line segment
		 * @param pointB The second end point of the line segment
		 * @param color Optional color of the line segments
		 * @param steps Optional number of segments that the line will be split into, range: [1, infinity)
		 * @tparam tLineWidth The line of the width, range: [1, infinity), must be an odd value
		 */
		template <unsigned int tLineWidth>
		static void drawLine(const AnyCamera& anyCamera, Frame& frame, const Vector3& pointA, const Vector3& pointB, const uint8_t* color = nullptr, const size_t steps = 10);

		/**
		 * Draws a (projected) 3D line into a given frame.
		 * @param frame The frame in which the line will be painted, must be valid
		 * @param flippedCamera_T_world The transformation transforming world to the flipped camera, the flipped camera points towards the positive z-space with y-axis down, must be valid
		 * @param anyCamera The camera profile defining the projection, must be valid
		 * @param objectPoint0 The start 3D object point of the 3D line, defined in world
		 * @param objectPoint1 The end 3D object point of the 3D line, defined in world
		 * @param segments The number of segments in which the line will be separated, with range [1, infinity), the more segments the better the adjustment to the camera distortion (if any)
		 * @param foregroundColor The foreground color of the line, nullptr to skip the painting with the foreground color
		 * @param backgroundColor The background color of the line, nullptr to skip the painting with the background color
		 * @tparam tForegroundLineWidth The line width of the foreground line, range: [1, infinity); must be an odd value
		 * @tparam tBackgroundLineWidth The line width of the background line, range: [0, infinity); for value 0 no background line is drawn, otherwise this value should be larger than `tForegroundLineWidth` and must be an odd value,
		 */
		template <unsigned int tForegroundLineWidth, unsigned int tBackgroundLineWidth>
		static void drawLineIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const Vector3& objectPoint0, const Vector3& objectPoint1, const unsigned int segments, const uint8_t* foregroundColor, const uint8_t* backgroundColor);

		/**
		 * Draws a 3D coordinate system (projected) into a frame.
		 * If the frame is an RGB 24bit frame, than the axes are painted in red (x), green (y), and blue (z); otherwise the axes are painted black.<br>
		 * @param frame The frame in which the coordinate system is painted, must be valid
		 * @param flippedCamera_T_world The camera pose converting world to the flipped camera coordinate system (a camera coordinate system pointing towards the positive z-space), must be valid
		 * @param anyCamera The camera profile that is used to render the coordinate system
		 * @param world_T_coordinateSystem The transformation of the coordinate system that transforms points defined in the local coordinate system (which will be rendered) into points defined in the world coordinate system, must be valid
		 * @param length The length of the three axes of the coordinate system, defined in the units of the local coordinate system (coordinateSystem)
		 * @tparam tForegroundLineWidth The line width of the foreground line, range: [1, infinity); must be an odd value
		 * @tparam tBackgroundLineWidth The line width of the background line, range: [0, infinity); for value 0 no background line is drawn, otherwise this value should larger than `tForegroundLineWidth` and must be an odd value,
		 */
		template <unsigned int tForegroundLineWidth, unsigned int tBackgroundLineWidth>
		static void drawCoordinateSystemIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_coordinateSystem, const Scalar length);

#if defined(OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)

		/**
		 * Draws the outline of a QR code given its pose and version
		 * @note This function is only for development purposes; do not use it in production code because it will be removed again
		 * @param anyCamera The camera profile that will be used for drawing, must be valid
		 * @param frame The frame into which the line will be drawn, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with RGB24
		 * @param version The version number of the QR code, range: [1, 40]
		 * @param code_T_camera The transformation that maps coordinates in the reference frame of the camera to that of the code, must be valid
		 */
		static void drawQRCodeOutline(const AnyCamera& anyCamera, Frame& frame, const unsigned int version, const HomogenousMatrix4& code_T_camera);

#endif // OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED

		/**
		 * Converts a QR code into string (aka ASCII art)
		 * @param code The QR code that will be converted into string, must be valid
		 * @param border The border in multiples of modules around the ASCII representation of the QR code, range: [0, infinity)
		 * @return The QR code in ASCII representation; will be empty in case of a failure
		 */
		static std::string toString(const QRCode& code, const unsigned int border);

		/**
		 * Computes the number of pixel per module for a given observation of a QR code
		 * @param anyCamera The camera what was used for this observation, must be valid.
		 * @param world_T_camera The world pose of the camera, must be valid
		 * @param world_T_code The world pose of the QR code, must be valid
		 * @param codeSize The (display) size of the code in the real world (in meters), range: (0, infinity)
		 * @param version The version number of the QR code, range: [1, 40]
		 * @param minNumberPixelsPerModule The resulting minimum value for the number of pixels per module that has been found, will be ignored if `nullptr`
		 * @param maxNumberPixelsPerModule The resulting maximum value for the number of pixels per module that has been found, will be ignored if `nullptr`
		 * @param medianNumberPixelsPerModule The resulting median of all values for the number of pixels per module, will be ignored if `nullptr`
		 * @param avgNumberPixelsPerModule The resulting average of all values for the number of pixels per module, will be ignored if `nullptr`
		 * @return True if the computation was successful, otherwise false
		 */
		static bool computeNumberPixelsPerModule(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_code, const Scalar codeSize, const unsigned int version, Scalar* minNumberPixelsPerModule, Scalar* maxNumberPixelsPerModule, Scalar* medianNumberPixelsPerModule, Scalar* avgNumberPixelsPerModule);

		/**
		 * Computes the average diagonal length of a module in pixels for a given coordinate system of a QR code
		 * @param anyCamera The camera what was used for this observation, must be valid
		 * @param flippedCamera_T_code The flipped camera pose of the code, must be valid
		 * @param coordinateSystem The coordinate system of the QR code, must be valid
		 * @param xModule The horizontal coordinate of the module, range: [0, no. modules of the corresponding code)
		 * @param yModule The vertical coordinate of the module, range: [0, no. modules of the corresponding code)
		 * @return The length of the module diagonal in pixels, range: [0, infinity)
		 */
		static Scalar computeModuleDiagonalLength(const AnyCamera& anyCamera, const HomogenousMatrix4& flippedCamera_T_code, const CoordinateSystem& coordinateSystem, const unsigned int xModule, const unsigned int yModule);

		/**
		 * Computes the contrast between fore- and background modules for a given observation of a QR code.
		 * @param anyCamera The camera what was used for this observation, must be valid.
		 * @param yFrame The grayscale frame in which the QR code has been observed, must be valid
		 * @param world_T_camera The world pose of the camera, must be valid
		 * @param code The code for which the contrast will be computed, must be valid
		 * @param world_T_code The world pose of the QR code, must be valid
		 * @param codeSize The (display) size of the code in the real world (in meters), range: (0, infinity)
		 * @param medianContrast The resulting construct of the median fore- and background values, will be ignored if `nullptr`
		 * @param averageContrast The resulting construct of the average fore- and background values, will be ignored if `nullptr`
		 * @return True if the computation was successful, otherwise false
		 */
		static bool computeContrast(const AnyCamera& anyCamera, const Frame& yFrame, const HomogenousMatrix4& world_T_camera, const QRCode& code, const HomogenousMatrix4& world_T_code, const Scalar codeSize, unsigned int* medianContrast, unsigned int* averageContrast);

		/**
		 * Computes the tilt and view angles for an observation of a QR code
		 * The tilt angle is defined as the angle, `A`, between the normal on the code and the direction from the code center to the camera origin.
		 * \code
		 *    camera x
		 *    origin  .
		 *             .
		 *              *     * code normal
		 *               \    |
		 * direction to   \ +-|--------+
		 * camera origin   \ A|       /
		 *                / \ |      /
		 *               /    x     /
		 *              /  code    /
		 *             /  center  /
		 *            +----------+
		 * \endcode
		 * The view angle, `A`, is defined as the angle between the ray pointing from the camera center to the code center and the view direction of the camera.
		 * \code
		 *    camera x------* view direction (neg. z-axis)
		 *    origin  \ A
		 *             \
		 *              \
		 *  direction to *
		 *  code center   .+----------+
		 *                 .         /
		 *               /  .       /
		 *              /    x     /
		 *             /  code    /
		 *            /  center  /
		 *           +----------+
		 * \endcode
		 * @param world_T_camera The world pose of the camera, must be valid
		 * @param world_T_code The world pose of the QR code, must be valid
		 * @param tiltAngle The resulting angle at which the QR code is tilted away from the camera center, in radian
		 * @param viewAngle The resulting angle at which the camera is looking at the QR code, in radian
		 * @param distance The optionally resulting distance from the camera center to the code center, will be ignored if `nullptr`
		 * @return True if the computation was successful, otherwise false
		 */
		static bool computeCodeTiltAndViewAngles(const HomogenousMatrix4& world_T_camera, const HomogenousMatrix4& world_T_code, Scalar& tiltAngle, Scalar& viewAngle, Scalar* distance = nullptr);

		/**
		 * Checks if a given QR code exists in a list of QR codes given their 6DOF poses and a stereo camera
		 * This function will check if the payload of a new code is identical with the payload of any code in a list of codes; for pairs of codes with
		 * identical payload, function checks if the projection of a new code overlaps with the projection of the other QR code. If both checks are true
		 * the function will return `true`.
		 * @param sharedAnyCameras The cameras that produced the input images, must have 2 elements, all elements must be valid
		 * @param world_T_device The transformation that maps points in the device coordinate system points to world points, must be valid
		 * @param device_T_cameras The transformation that converts points in the camera coordinate systems to device coordinates, `devicePoint = device_T_cameras[i] * cameraPoint`, must have the same number of elements as `yFrames`, all elements must be valid
		 * @param codes The list of existing QR codes that will be checked against, each element must be a valid QR code
		 * @param world_T_codes The world poses the of the above list of QR codes, number of elements will be identical to `codes`
		 * @param codeSizes The edge lengths of the above list of QR codes in meters, number of elements will be identical to `codes`, range of each value: (0, infinity)
		 * @param newCode The new QR code that will be checked, must be valid
		 * @param world_T_newCode The world pose the of the above QR code, must be valid
		 * @param newCodeSize The edge length of the above QR code in meters, range: (0, infinity)
		 * @param index The optionally returned index of the code in `codes` that is identical to `newCode`, will be ignored if `nullptr`
		 * @return True if an identical code was found, otherwise false
		 */
		static bool containsCodeStereo(const SharedAnyCameras& sharedAnyCameras, const HomogenousMatrix4& world_T_device, const HomogenousMatrices4& device_T_cameras, const QRCodes& codes, const HomogenousMatrices4& world_T_codes, const Scalars& codeSizes, const QRCode& newCode, const HomogenousMatrix4& world_T_newCode, const Scalar newCodeSize, unsigned int* index = nullptr);

		/**
		 * Checks if a given QR code exists in a list of QR codes given their 6DOF poses and a mono camera
		 * This function will check if the payload of a new code is identical with the payload of any code in a list of codes; for pairs of codes with
		 * identical payload, function checks if the projection of a new code overlaps with the projection of the other QR code. If both checks are true
		 * the function will return `true`.
		 * @param anyCamera The camera that produced the input image, must be valid
		 * @param world_T_camera The transformation that maps points in the camera coordinate system points to world points, must be valid
		 * @param codes The list of existing QR codes that will be checked against, each element must be a valid QR code
		 * @param world_T_codes The world poses the of the above list of QR codes, number of elements will be identical to `codes`
		 * @param codeSizes The edge lengths of the above list of QR codes in meters, number of elements will be identical to `codes`, range of each value: (0, infinity)
		 * @param newCode The new QR code that will be checked, must be valid
		 * @param world_T_newCode The world pose the of the above QR code, must be valid
		 * @param newCodeSize The edge length of the above QR code in meters, range: (0, infinity)
		 * @param index The optionally returned index of the code in `codes` that is identical to `newCode`, will be ignored if `nullptr`
		 * @return True if an identical code was found, otherwise false
		 */
		static bool containsCodeMono(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const QRCodes& codes, const HomogenousMatrices4& world_T_codes, const Scalars& codeSizes, const QRCode& newCode, const HomogenousMatrix4& world_T_newCode, const Scalar newCodeSize, unsigned int* index = nullptr);

		/**
		 * Checks if a given QR code exists in a list of QR codes
		 * This function will check if the payload of a new code is identical with the payload of any code in a list of codes; if any is found, the function will return `true`.
		 * @param codes The list of existing QR codes that will be checked against, each element must be a valid QR code
		 * @param newCode The new QR code that will be checked, must be valid
		 * @return True if an identical code was found, otherwise false
		 */
		static bool containsCode(const QRCodes& codes, const QRCode& newCode);

		/**
		 * Parses a Wi-Fi configuration from a QR code
		 * The following format is expected:
		 *
		 * <pre>
		 * WIFI:S:\<SSID>;T:<WEP|WPA|blank>;P:\<PASSWORD>;H:<true|false|blank>;;
		 * </pre>
		 *
		 * Special characters \;,": should be escaped with a backslash (\\). More details can be found
		 * here: https://github.com/zxing/zxing/wiki/Barcode-Contents#wi-fi-network-config-android-ios-11
		 * @param configString The string containing the configuration, must be valid and of the format mentioned above
		 * @param ssid The resulting SSID of the Wi-Fi network
		 * @param password The resulting password of the Wi-Fi network
		 * @param encryption The optionally resulting encryption type of the Wi-Fi network (either "", "WEP", or "WPA")
		 * @param isSsidHidden The optionally resulting value whether the SSID of the Wi-Fi network is hidden
		 * @return `PS_SUCCESS` if parsing finished successfully, otherwise a correspond error code
		 */
		static ParsingStatus parseWifiConfig(const std::string& configString, std::string& ssid, std::string& password, std::string* encryption = nullptr, bool* isSsidHidden = nullptr);

		/**
		 * Returns a human-readable string for each possible parsing status
		 * @param status The status to be converted to a string
		 * @return The human-readable string
		 */
		static std::string parsingStatusToString(const ParsingStatus status);

	protected:

		/**
		 * Escapes selected characters in a string
		 * @param string The string which will be checked for characters that need escaping, must be valid
		 * @param specialCharacters The string defining which characters to escape; duplicates will be ignored; must be valid
		 * @return A string with escaped characters
		 * @sa parseWifiConfig()
		 */
		static std::string escapeSpecialCharacters(const std::string& string, const std::string& specialCharacters = "\\;,\":");

		/**
		 * Unescapes selected character from a string
		 * @param escapedString The string to be unescaped, must be valid
		 * @param string The resulting string without escaped characters
		 * @param specialCharacters The string defining which characters to unescape; duplicates will be ignored; must be valid
		 * @return True if the unescaping process was successful, otherwise false
		 * @sa parseWifiConfig()
		 */
		static bool unescapeSpecialCharacters(const std::string& escapedString, std::string& string, const std::string& specialCharacters = "\\;,\":");

		/**
		 * Computes the image location of the center of a QR code given its world pose
		 * @param anyCamera The camera profile that will be used to draw the line, must be valid
		 * @param world_T_camera The transformation that maps points in the camera coordinate system points to world points, must be valid
		 * @param code The code for which the image location will be computed, must be valid
		 * @param world_T_code The world pose the QR code, must be valid
		 * @param codeSize The edge lengths of the QR code in meters, range: (0, infinity)
		 * @param imageCodeCenter The returning image location of the center of a QR code; value is only valid if this function returns `true`
		 * @param maxSquareRadius The optionally returning maximum square distance in pixels of the image location of the code and its four corners; value is only valid if this function returns `true`
		 * @return True, if code is in front of the camera and the image location of its center is inside the image boundaries, otherwise false
		 */
		static bool computeCodeCenterInImage(const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_camera, const QRCode& code, const HomogenousMatrix4& world_T_code, const Scalar codeSize, Vector2& imageCodeCenter, Scalar* maxSquareRadius = nullptr);
};

inline Scalar Utilities::CoordinateSystem::scale() const
{
	return scale_;
}

inline Scalar Utilities::CoordinateSystem::convertCodeSpaceToObjectSpaceX(const Scalar xCodeSpace) const
{
	const Scalar xObjectSpace = xScale_ * xCodeSpace - Scalar(scale_);
	ocean_assert(Numeric::isInsideRange(Scalar(-scale_), xObjectSpace, Scalar(scale_)));

	return xObjectSpace;
}

inline Scalar Utilities::CoordinateSystem::convertCodeSpaceToObjectSpaceY(const Scalar yCodeSpace) const
{
	const Scalar yObjectSpace = yScale_ * yCodeSpace + Scalar(scale_);
	ocean_assert(Numeric::isInsideRange(Scalar(-scale_), yObjectSpace, Scalar(scale_)));

	return yObjectSpace;
}

inline Vectors3 Utilities::CoordinateSystem::computeCornersInObjectSpace(const Scalar scale)
{
	ocean_assert(scale > Scalar(0));

	Vectors3 corners =
	{
		Vector3(-Scalar(scale),  Scalar(scale), Scalar(0)), // top-left
		Vector3(-Scalar(scale), -Scalar(scale), Scalar(0)), // bottom-left
		Vector3( Scalar(scale), -Scalar(scale), Scalar(0)), // bottom-right
		Vector3( Scalar(scale),  Scalar(scale), Scalar(0)), // top-right
	};

	return corners;
}

inline void Utilities::drawObservation(Frame& frame, const LegacyQRCodeDetector2D::Observation& observation, const QRCode& code)
{
	ocean_assert(frame.isValid());
	ocean_assert(code.isValid());

	drawObservation(frame, observation.frame_H_code(), observation.finderPatterns(), code.version(), code.modules());
}

inline void Utilities::drawObservations(Frame& frame, const LegacyQRCodeDetector2D::Observations& observations, const QRCodes& codes)
{
	ocean_assert(frame.isValid());

	if (observations.size() != codes.size())
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	for (size_t i = 0; i < observations.size(); ++i)
	{
		drawObservation(frame, observations[i].frame_H_code(), observations[i].finderPatterns(), codes[i].version(), codes[i].modules());
	}
}

template <unsigned int tLineWidth>
void Utilities::drawLine(const AnyCamera& anyCamera, Frame& frame, const Vector2& pointA, const Vector2& pointB, const uint8_t* color, const size_t steps)
{
	ocean_assert(anyCamera.isValid());
	ocean_assert(frame.isValid());
	ocean_assert(steps != 0);

	return Utilities::drawLine<tLineWidth>(anyCamera, frame, anyCamera.vector(pointA), anyCamera.vector(pointB), color, steps);
}

template <unsigned int tLineWidth>
void Utilities::drawLine(const AnyCamera& anyCamera, Frame& frame, const Vector3& pointA, const Vector3& pointB, const uint8_t* color, const size_t steps)
{
	static_assert(tLineWidth != 0 && tLineWidth % 2u == 1u, "Line width must be non-zero and odd");

	ocean_assert(anyCamera.isValid());
	ocean_assert(frame.isValid() && frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT && Frame::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_RGB24));
	ocean_assert(steps != 0);

	const Vector3 step = (pointB - pointA) * (Scalar(1) / Scalar(steps));
	ocean_assert(!step.isNull());

	color = color == nullptr ? CV::Canvas::green(frame.pixelFormat()) : color;

	Vector3 previousPlanePoint = pointA;
	Vector2 previousImagePoint = anyCamera.projectToImage(previousPlanePoint);

	for (size_t s = 0; s < steps; ++s)
	{
		const Vector3 currentPlanePoint = previousPlanePoint + step;
		const Vector2 currentImagePoint = anyCamera.projectToImage(currentPlanePoint);

		CV::Canvas::line<tLineWidth>(frame, previousImagePoint, currentImagePoint, color);

		previousPlanePoint = currentPlanePoint;
		previousImagePoint = currentImagePoint;
	}
}

template <unsigned int tForegroundLineWidth, unsigned int tBackgroundLineWidth>
void Utilities::drawLineIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const Vector3& objectPoint0, const Vector3& objectPoint1, const unsigned int segments, const uint8_t* foregroundColor, const uint8_t* backgroundColor)
{
	static_assert(tForegroundLineWidth != 0u && tForegroundLineWidth % 2u == 1u, "The line width must be non-zero and odd.");
	static_assert(tBackgroundLineWidth == 0u || tBackgroundLineWidth % 2u == 1u, "The line width must be either zero (in which case it will not be drawn) or non-zero and odd.");

	ocean_assert(frame && flippedCamera_T_world.isValid() && anyCamera.isValid());
	ocean_assert(frame.width() == anyCamera.width() && frame.height() == anyCamera.height());
	ocean_assert(segments >= 1u);

	const Scalar segmentFactor = Scalar(1) / Scalar(segments);

	if (backgroundColor && tBackgroundLineWidth != 0u)
	{
		Vector2 projectedStart = anyCamera.projectToImageIF(flippedCamera_T_world, objectPoint0);

		for (unsigned int n = 0u; n < segments; ++n)
		{
			const Vector3 end = objectPoint0 + (objectPoint1 - objectPoint0) * Scalar(n + 1) * segmentFactor;
			const Vector2 projectedEnd = anyCamera.projectToImageIF(flippedCamera_T_world, end);

			CV::Canvas::line<tBackgroundLineWidth>(frame, projectedStart.x(), projectedStart.y(), projectedEnd.x(), projectedEnd.y(), backgroundColor);

			projectedStart = projectedEnd;
		}
	}

	if (foregroundColor)
	{
		Vector2 projectedStart = anyCamera.projectToImageIF(flippedCamera_T_world, objectPoint0);

		for (unsigned int n = 0u; n < segments; ++n)
		{
			const Vector3 end = objectPoint0 + (objectPoint1 - objectPoint0) * Scalar(n + 1) * segmentFactor;
			const Vector2 projectedEnd = anyCamera.projectToImageIF(flippedCamera_T_world, end);

			CV::Canvas::line<tForegroundLineWidth>(frame, projectedStart.x(), projectedStart.y(), projectedEnd.x(), projectedEnd.y(), foregroundColor);

			projectedStart = projectedEnd;
		}
	}
}

template <unsigned int tForegroundLineWidth, unsigned int tBackgroundLineWidth>
void Utilities::drawCoordinateSystemIF(Frame& frame, const HomogenousMatrix4& flippedCamera_T_world, const AnyCamera& anyCamera, const HomogenousMatrix4& world_T_coordinateSystem, const Scalar length)
{
	static_assert(tForegroundLineWidth != 0u && tForegroundLineWidth % 2u == 1u, "The line width must be non-zero and odd.");
	static_assert(tBackgroundLineWidth == 0u || tBackgroundLineWidth % 2u == 1u, "The line width must be either zero (in which case it will not be drawn) or non-zero and odd.");

	ocean_assert(frame && flippedCamera_T_world.isValid() && anyCamera.isValid() && world_T_coordinateSystem.isValid());
	ocean_assert(frame.width() == anyCamera.width() && frame.height() == frame.height());

	const uint8_t* const red = CV::Canvas::red(frame.pixelFormat());
	const uint8_t* const green = CV::Canvas::green(frame.pixelFormat());
	const uint8_t* const blue = CV::Canvas::blue(frame.pixelFormat());
	const uint8_t* const black = CV::Canvas::black(frame.pixelFormat());

	if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, world_T_coordinateSystem.translation()))
	{
		const Vector3 xAxis = world_T_coordinateSystem * Vector3(length, 0, 0);
		const Vector3 yAxis = world_T_coordinateSystem * Vector3(0, length, 0);
		const Vector3 zAxis = world_T_coordinateSystem * Vector3(0, 0, length);

		if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, xAxis))
		{
			drawLineIF<tForegroundLineWidth, tBackgroundLineWidth>(frame, flippedCamera_T_world, anyCamera, world_T_coordinateSystem.translation(), xAxis, /* segments */ 15u, red, black);
		}

		if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, yAxis))
		{
			drawLineIF<tForegroundLineWidth, tBackgroundLineWidth>(frame, flippedCamera_T_world, anyCamera, world_T_coordinateSystem.translation(), yAxis, /* segments */ 15u, green, black);
		}

		if (AnyCamera::isObjectPointInFrontIF(flippedCamera_T_world, zAxis))
		{
			drawLineIF<tForegroundLineWidth, tBackgroundLineWidth>(frame, flippedCamera_T_world, anyCamera, world_T_coordinateSystem.translation(), zAxis, /* segments */ 15u, blue, black);
		}
	}
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
