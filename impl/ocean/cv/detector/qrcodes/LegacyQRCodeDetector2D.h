/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"
#include "ocean/cv/detector/qrcodes/FinderPatternDetector.h"
#include "ocean/cv/detector/qrcodes/QRCode.h"
#include "ocean/cv/detector/qrcodes/QRCodeDetector.h"
#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"

#include "ocean/base/Frame.h"
#include "ocean/base/Memory.h"

#include "ocean/cv/Bresenham.h"

#include "ocean/geometry/Homography.h"

#include "ocean/math/Box2.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector2.h"

#include <array>
#include <cstdint>

namespace Ocean
{

namespace CV
{

namespace Detector
{

namespace QRCodes
{

/**
 * Deprecated: use QRCodeDetector2D instead
 *
 * This class implements a detector for QR Codes.
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT LegacyQRCodeDetector2D : public QRCodeDetector
{
	public:

		/**
		 * Definition of different detection modes
		 */
		enum DetectionMode : uint32_t
		{
			/// Uses the standard array of checks to detect QR codes
			DM_STANDARD = 1u << 0u,
			/// Will add a small border around each image, e.g., if the QR code occupies the entire image leaving too little space to the border
			DM_EXTRA_BORDER = 1u << 1u,
			/// Will enable blurring of the input image as an additional measure to detect QR codes
			DM_BLUR = 2u << 1u,
			/// Uses a minimum of detection tests for fast detections
			DM_BEST_PERFORMANCE = DM_STANDARD,
			/// Enables all additional checks for best detection results
			DM_BEST_DETECTION = DM_STANDARD | DM_EXTRA_BORDER | DM_BLUR
		};

		/**
		 * Definition of an observation of QR code in 2D
		 */
		class Observation
		{
			public:

				/**
				 * Creates an invalid observation
				 */
				Observation() = default;

				/**
				 * Creates an valid observation
				 * @param frame_H_code The homography that maps coordinates in the QR code grid to image coordinates, i.e., `frameLocation = frame_H_code_ * (codeX + 0.5, codeY + 0.5)^T`
				 * @param finderPatterns The three finder patterns of the QR Codes, elements must be in the order: top-left, bottom-left, top-right
				 */
				inline explicit Observation(const SquareMatrix3& frame_H_code, FinderPatternTriplet&& finderPatterns);

				/**
				 * Returns if the observation is valid
				 * @return True if the observation is valid, otherwise false
				 */
				inline bool isValid() const;

				/**
				 * Returns the homography that maps coordinates in the QR code grid to image coordinates
				 * @return The homography
				 */
				inline const SquareMatrix3& frame_H_code() const;

				/**
				 * Returns a pointer to the finder patterns
				 * @return The finder patterns
				 */
				inline const FinderPatternTriplet& finderPatterns() const;

			protected:

				/// The homography that maps coordinates in the QR code grid to image coordinates, i.e., `frameLocation = frame_H_code_ * (codeX + 0.5, codeY + 0.5)^T`
				SquareMatrix3 frame_H_code_ = SquareMatrix3(false);

				/// The finder patterns of the QR code, order: top-left, bottom-left, top-right
				FinderPatternTriplet finderPatterns_;
		};

		/// Definition of a vector of observations
		typedef std::vector<Observation> Observations;

	public:

		/**
		 * Detects QR codes in a given 8 bit grayscale image.
		 * @param frame The frame in which the QR codes will be detected, must be an 8 bit grayscale image and must be valid
		 * @param worker Optional worker to distribute the computation
		 * @param detectionMode The detection mode that should be used
		 * @param observations If specified, will return the observations of the detected QR codes; the order matches the elements of the return value
		 * @return An array of detected QR codes
		 */
		static inline QRCodes detectQRCodes(const Frame& frame, Worker* worker = nullptr, const DetectionMode detectionMode = DM_STANDARD, Observations* observations = nullptr);

		/**
		 * Detects QR codes in a given 8 bit grayscale image.
		 * @param yFrame The 8 bit grayscale frame in which the QR codes will be detected, must be valid
		 * @param width The width of the given grayscale frame in pixel, with range [29, infinity)
		 * @param height The height of the given grayscale frame in pixel, with range [29, infinity)
		 * @param paddingElements Optional number of padding elements at the end of each image row, in elements, with range [0, infinity)
		 * @param worker Optional worker to distribute the computation
		 * @param detectionMode The detection mode that should be used
		 * @param observations If specified, will return the observations of the detected QR codes; the order matches the elements of the return value
		 * @return An array of detected QR codes
		 */
		static QRCodes detectQRCodes(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements = 0u, Worker* worker = nullptr, const DetectionMode detectionMode = DM_STANDARD, Observations* observations = nullptr);

	protected:

		/**
		 * Determines the three outer corners of a finder pattern triplet in a QR code
		 * Given a (valid) triplet of finder patterns, their corners can be used to determine three (out of four) corners of the QR code, i.e., the top-left corner `(0, 0)`, the bottom-left corner `(0, qrcodesize)`, and the top-right corner `(qrcodesize, 0)`
		 * @note Make sure that the triplet is valid, otherwise the result will be undefined.
		 * @param topLeft The finder pattern in the top-left corner
		 * @param bottomLeft The finder pattern in the bottom-left corner
		 * @param topRight The finder pattern in the top-right corner
		 * @param cornerIndexTopLeft The resulting index of the corner of the top-left finder pattern that is also the top-left corner of the entire QR code symbol, will be in the range: [0, 4) (cf. `QRCode::corner()`)
		 * @param cornerIndexBottomLeft The resulting index of the corner of the bottom-left finder pattern that is also the bottom-left corner of the entire QR code symbol, will be in the range: [0, 4) (cf. `QRCode::corner()`)
		 * @param cornerIndexTopRight The resulting index of the corner of the top-right finder pattern that is also the top-right corner of the entire QR code symbol, will be in the range: [0, 4) (cf. `QRCode::corner()`)
		 * @return True if the three corners were successfully determined, otherwise false
		 */
		static bool determineOuterMostCorners(const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight, unsigned int& cornerIndexTopLeft, unsigned int& cornerIndexBottomLeft, unsigned int& cornerIndexTopRight);

		/**
		 * Extracts the version of a QR code from an image given its known location
		 * @param yFrame Pointer to the input grayscale image, must be valid
		 * @param width The width of the input grayscale image, range: [1, infinity)
		 * @param height The height of the input grayscale image, range:[1, infinity)
		 * @param yFramePaddingElements The number of padding elements of the input grayscale image, range: [0, infinity)
		 * @param topLeft The finder pattern in the top-left corner of the QR code, must be valid
		 * @param bottomLeft The finder pattern in the bottom-left corner of the QR code, must be valid
		 * @param topRight The finder pattern in the top-right corner of the QR code, must be valid
		 * @param estimatedVersion The version number that was estimated from the module size of the finder patterns, range: [7u, 40u]
		 * @param homography A transformation which is used to extract the modules (bits) of the version information, maps QR code location `q = (u, v)` to image coordinates `p = (x, y)`: `p = affineTransform * (u + 0.5, v + 0.5)`
		 * @param version Will hold the resulting version number that is extracted from the image of QR code
		 * @return True if the version was successfully extracted, otherwise false
		 * @sa computeInitialHomography()
		 */
		static bool determineSymbolVersionFromImage(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight, const unsigned int estimatedVersion, const SquareMatrix3& homography, unsigned int& version);

		/**
		 * Detects the timer pattern between (1) the top-left and top-right or (2) the top-left and bottom-left finder patterns
		 * @param yFrame Pointer to the input grayscale image, must be valid
		 * @param width The width of the input grayscale image, range: [1, infinity)
		 * @param height The height of the input grayscale image, range:[1, infinity)
		 * @param yFramePaddingElements The number of padding elements of the input grayscale image, range: [0, infinity)
		 * @param version The version number that was determined for this QR code, range: [1u, 40u]
		 * @param homography Homography transformation that describes the location of the QR code for which the detection of the timing pattern will be run, must be valid
		 * @param topLeft The finder pattern in the top-left corner of the QR code, must be valid
		 * @param bottomLeft The finder pattern in the bottom-left corner of the QR code, must be valid
		 * @param topRight The finder pattern in the top-right corner of the QR code, must be valid
		 * @return True if 1) at least one timer pattern has been detected 100%, or 2) both timer patterns have been detected 80% each, otherwise false
		 */
		static bool detectTimerPatterns(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const unsigned int version, const SquareMatrix3& homography, const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight);

		/**
		 * Extracts the format information of a QR code from an image given its known location
		 * @param yFrame Pointer to the input grayscale image, must be valid
		 * @param width The width of the input grayscale image, range: [1, infinity)
		 * @param height The height of the input grayscale image, range:[1, infinity)
		 * @param yFramePaddingElements The number of padding elements of the input grayscale image, range: [0, infinity)
		 * @param topLeft The finder pattern in the top-left corner of the QR code, must be valid
		 * @param bottomLeft The finder pattern in the bottom-left corner of the QR code, must be valid
		 * @param topRight The finder pattern in the top-right corner of the QR code, must be valid
		 * @param version The version number that was determined for this QR code, range: [1u, 40u]
		 * @param homography A transformation which is used to extract the modules (bits) of the version information, maps QR code location `q = (u, v)` to image coordinates `p = (x, y)`: `p = affineTransform * (u + 0.5, v + 0.5)`
		 * @param errorCorrectionCapacity The resulting error correction capacity that the QR code in the image was created with
		 * @param maskingPattern The resulting masking pattern that was used to XOR the modules of the QR code in the image at its creation time
		 * @return True if the format information was successfully read from image and could be decoded
		 */
		static bool determineSymbolFormat(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight, const unsigned int version, const SquareMatrix3& homography, QRCode::ErrorCorrectionCapacity& errorCorrectionCapacity, QRCodeEncoder::MaskingPattern& maskingPattern);

		/**
		 * Computes an initial transformation to map QR code coordinates to image locations
		 * @note Make sure that the triplet is valid, otherwise the result will be undefined.
		 * @param topLeft The finder pattern in the top-left corner
		 * @param bottomLeft The finder pattern in the bottom-left corner
		 * @param topRight The finder pattern in the top-right corner
		 * @param version The version of the QR code defined by the above triplet of finder patterns, range: [1, 40]
		 * @param cornerIndexTopLeft The index of the corner of the top-left finder pattern that is also the top-left corner of the entire QR code symbol, range: [0, 4) (cf. `QRCode::corner()`), set this to a value outside the range if this index isn't known
		 * @param cornerIndexBottomLeft The index of the corner of the bottom-left finder pattern that is also the bottom-left corner of the entire QR code symbol, range: [0, 4) (cf. `QRCode::corner()`), set this to a value outside the range if this index isn't known
		 * @param cornerIndexTopRight The index of the corner of the top-right finder pattern that is also the top-right corner of the entire QR code symbol, range: [0, 4) (cf. `QRCode::corner()`), set this to a value outside the range if this index isn't known
		 * @param homography The resulting homography that is computed from the triplet of finder patterns. In case not all corners of the finder patterns are known, this transformation will be an affine transformation, not a full homography. The transformation maps QR code coordinates `(u, v)` to image locations, `p = (x, y)`: `p = homography * (u + 0.5, v + 0.5)`
		 * @return True if the computation of the homography was successful, otherwise false
		 */
		static inline bool computeInitialHomography(const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight, const unsigned int& version, const unsigned int cornerIndexTopLeft, const unsigned int cornerIndexBottomLeft, const unsigned int cornerIndexTopRight, SquareMatrix3& homography);

		/**
		 * Computes the homography for a valid triplet finder patterns and known version of the underlying QR code
		 * Locates the alignment patterns of the QR code and uses them in order to refine the estimate of the initial homography
		 * @param yFrame Pointer to a grayscale image of size `width x height` pixels, must be valid
 		 * @param width The width of the input frame, range: [1, infinity)
 		 * @param height The height of the input frame, range: [1, infinity)
 		 * @param yFramePaddingElements Number of padding elements of this frame, range: [0, infinity)
 		 * @param topLeft The finder pattern in the top-left corner of the QR code, must be valid
 		 * @param bottomLeft The finder pattern in bottom-left corner of the QR code, must be valid
 		 * @param topRight The finder pattern in the top-right corner of the QR code, must be valid
		 * @param cornerIndexTopLeft The index of the corner of the top-left finder pattern that is also the top-left corner of the entire QR code symbol, range: [0, 4) (cf. `QRCode::corner()`)
		 * @param cornerIndexBottomLeft The index of the corner of the bottom-left finder pattern that is also the bottom-left corner of the entire QR code symbol, range: [0, 4) (cf. `QRCode::corner()`)
		 * @param cornerIndexTopRight The index of the corner of the top-right finder pattern that is also the top-right corner of the entire QR code symbol, range: [0, 4) (cf. `QRCode::corner()`)
 		 * @param version The version number of the QR code, range: [1, 40]
 		 * @param initialHomography An initial transformation to map QR code coordinates to image locations that will be refined
 		 * @param homography The resulting transformation mapping coordinates in the QR code, `q = (u, v)`, into image coordinate, `p = (x, y)`: `p = homography * q`
		 * @return True if the homography was computed successfully, otherwise false
		 */
		static bool computeRefinedHomography(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight, const unsigned int cornerIndexTopLeft, const unsigned int cornerIndexBottomLeft, const unsigned int cornerIndexTopRight, unsigned int& version, const SquareMatrix3& initialHomography, SquareMatrix3& homography);

		/**
		 * Read the modules of a QR code with known location (homography) from an image
		 * @param yFrame Pointer to a grayscale image of size `width x height` pixels, must be valid
		 * @param width The width of the input frame, range: [1, infinity)
		 * @param height The height of the input frame, range: [1, infinity)
		 * @param yFramePaddingElements Number of padding elements of this frame, range: [0, infinity)
		 * @param grayThreshold The threshold that was used during the detection of this QR code, range: [0, 255]
		 * @param version The version number of the QR code, range: [1, 40]
		 * @param homography The transformation mapping coordinates in the QR code, `q = (u, v)`, into image coordinate, `p = (x, y)`: `p = homography * q`
		 * @param modules The resulting modules (bits) of the QR code that have been identified in the image, `yFrame`. Memory will be allocated inside this function, if necessary
		 * @return True, if the modules were read successfully, otherwise false
		 */
		static bool extractModules(const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int yFramePaddingElements, const unsigned int grayThreshold, const unsigned int version, const SquareMatrix3& homography, std::vector<uint8_t>& modules);
};

inline LegacyQRCodeDetector2D::Observation::Observation(const SquareMatrix3& frame_H_code, FinderPatternTriplet&& finderPatterns) :
 frame_H_code_(frame_H_code),
 finderPatterns_(std::move(finderPatterns))
{
	// Nothing else to do.
}

inline bool LegacyQRCodeDetector2D::Observation::isValid() const
{
	return frame_H_code_.isHomography();
}

inline const SquareMatrix3& LegacyQRCodeDetector2D::Observation::frame_H_code() const
{
	return frame_H_code_;
}

inline const FinderPatternTriplet& LegacyQRCodeDetector2D::Observation::finderPatterns() const
{
	return finderPatterns_;
}

inline QRCodes LegacyQRCodeDetector2D::detectQRCodes(const Frame& frame, Worker* worker, const DetectionMode detectionMode, Observations* observations)
{
	if (frame.isValid() == false || FrameType::arePixelFormatsCompatible(frame.pixelFormat(), FrameType::FORMAT_Y8) == false || frame.pixelOrigin() != FrameType::ORIGIN_UPPER_LEFT)
	{
		ocean_assert(false && "Frame must be valid and an 8 bit grayscale image and the pixel origin must be the upper left corner");
		QRCodes();
	}

	return detectQRCodes(frame.constdata<uint8_t>(), frame.width(), frame.height(), frame.paddingElements(), worker, detectionMode, observations);
}

inline bool LegacyQRCodeDetector2D::computeInitialHomography(const FinderPattern& topLeft, const FinderPattern& bottomLeft, const FinderPattern& topRight, const unsigned int& version, const unsigned int cornerIndexTopLeft, const unsigned int cornerIndexBottomLeft, const unsigned int cornerIndexTopRight, SquareMatrix3& homography)
{
	ocean_assert(version >= 1u && version <= 40u);

	const unsigned int modules = QRCode::modulesPerSide(std::min(40u, version));

	if (topLeft.cornersKnown() && bottomLeft.cornersKnown() && topRight.cornersKnown() && cornerIndexTopLeft < 4u && cornerIndexBottomLeft < 4u && cornerIndexTopRight < 4u)
	{
		// Compute a homography from the corners of the finder patterns (3 x 4 points)
		//
		// Define the initial set of point correspondences. Here, use the four corners of the three finder patterns
		// (top-left, bottom-left, and top-right):
		//
		//   TL               TR
		//     *0--3     9--8*
		//      |  |     |  |
		//      1--2    10--11
		//
		//      7--6
		//      |  |
		//     *4--5
		//   BL
		//
		// The numbers indicate the index in the list of points below. The asterik (*) denotes the outer-most corners
		// that were determined above. Because the corners of the finder patterns are in counter-clockwise order, they
		// can easily be enumerated using `i & 0b0011u` as cheaper equivalent to `i % 4`.

		const Vector2 imagePoints[12] =
		{
			// Corners of the top-left finder pattern
			topLeft.corners()[(cornerIndexTopLeft + 0u) & 0b0011u], // (cornerIndexTopLeft + x) & 0b0011u == (cornerIndexTopLeft + x) % 4
			topLeft.corners()[(cornerIndexTopLeft + 1u) & 0b0011u],
			topLeft.corners()[(cornerIndexTopLeft + 2u) & 0b0011u],
			topLeft.corners()[(cornerIndexTopLeft + 3u) & 0b0011u],

			// Corners of the bottom-left finder pattern
			bottomLeft.corners()[(cornerIndexBottomLeft + 0u) & 0b0011u],
			bottomLeft.corners()[(cornerIndexBottomLeft + 1u) & 0b0011u],
			bottomLeft.corners()[(cornerIndexBottomLeft + 2u) & 0b0011u],
			bottomLeft.corners()[(cornerIndexBottomLeft + 3u) & 0b0011u],

			// Corners of the top-right finder pattern
			topRight.corners()[(cornerIndexTopRight + 0u) & 0b0011u],
			topRight.corners()[(cornerIndexTopRight + 1u) & 0b0011u],
			topRight.corners()[(cornerIndexTopRight + 2u) & 0b0011u],
			topRight.corners()[(cornerIndexTopRight + 3u) & 0b0011u],
		};

		const Vector2 qrcodePoints[12] =
		{
			// Corners of the top-left finder pattern
			Vector2(Scalar(0), Scalar(0)),
			Vector2(Scalar(0), Scalar(7)),
			Vector2(Scalar(7), Scalar(7)),
			Vector2(Scalar(7), Scalar(0)),

			// Corners of the bottom-left finder pattern
			Vector2(Scalar(0), Scalar(modules)),
			Vector2(Scalar(7), Scalar(modules)),
			Vector2(Scalar(7), Scalar(modules - 7u)),
			Vector2(Scalar(0), Scalar(modules - 7u)),

			// Corners of the top-right finder pattern
			Vector2(Scalar(modules), Scalar(0)),
			Vector2(Scalar(modules - 7u), Scalar(0)),
			Vector2(Scalar(modules - 7u), Scalar(7)),
			Vector2(Scalar(modules), Scalar(7)),
		};

		if (Geometry::Homography::homographyMatrixLinearWithOptimizations(qrcodePoints, imagePoints, 12u, homography))
		{
			return true;
		}
	}

	// If the above didn't work, compute an affine transformation from the centers of the finder patterns (3 x 1 points, affine transformations are a special case of homographies)
	const Vector2 imagePoints[3] =
	{
		topLeft.position(),
		bottomLeft.position(),
		topRight.position()
	};

	const Vector2 rectifiedPoints[3] =
	{
		Vector2(Scalar(3.5), Scalar(3.5)),
		Vector2(Scalar(3.5), Scalar(modules) - Scalar(3.5)),
		Vector2(Scalar(modules) - Scalar(3.5), Scalar(3.5))
	};

	return Geometry::Homography::affineMatrix(rectifiedPoints, imagePoints, 3u, homography);
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
