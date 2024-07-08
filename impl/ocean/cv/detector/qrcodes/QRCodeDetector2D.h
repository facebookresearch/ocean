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

#if defined(OCEAN_QRCODES_QRCODEDEBUGELEMENTS_ENABLED)
	#include "ocean/cv/detector/qrcodes/QRCodeDebugElements.h"
#endif

#include "ocean/base/Frame.h"

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
 * This class implements a detector for QR Codes.
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT QRCodeDetector2D : public QRCodeDetector
{
	public:

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
				 * @param code_T_camera The transformation that maps 3D coordinates in the QR code grid to the camera frame of reference, i.e., `imagePoint = anyCamera.projectToImage(code_T_camera, codePoint)`
				 * @param finderPatterns The three finder patterns of the QR Codes, elements must be in the order: top-left, bottom-left, top-right
				 */
				inline explicit Observation(const HomogenousMatrix4& code_T_camera, FinderPatternTriplet&& finderPatterns);

				/**
				 * Returns if the observation is valid
				 * @return True if the observation is valid, otherwise false
				 */
				inline bool isValid() const;

				/**
				 * Returns the transformation that maps coordinates in the QR code grid to coordinates in the reference frame of the camera
				 * @return The transformation
				 */
				inline const HomogenousMatrix4& code_T_camera() const;

				/**
				 * Returns a pointer to the finder patterns
				 * @return The finder patterns
				 */
				inline const FinderPatternTriplet& finderPatterns() const;

			protected:

				/// The transformation that maps 3D coordinates in the QR code grid to the camera frame of reference, i.e., `imagePoint = anyCamera.projectToImage(code_T_camera, codePoint)`
				HomogenousMatrix4 code_T_camera_ = HomogenousMatrix4(false);

				/// The finder patterns of the QR code, order: top-left, bottom-left, top-right
				FinderPatternTriplet finderPatterns_;
		};

		/// Definition of a vector of observations
		typedef std::vector<Observation> Observations;

	public:

		/**
		 * Detects QR codes in an 8-bit grayscale image without lens distortion.
		 * Use this function for images without lens distortion, for example from pinhole cameras, screenshots, or similar.
		 * @param yFrame The frame in which QR codes will be detected, must be valid, have its origin in the upper left corner, and have a pixel format that is compatible with Y8, minimum size is 29 x 29 pixels
		 * @param observations Optional observations of the detected QR codes that will be returned, will be ignored for `nullptr`
		 * @param worker Optional worker instance for parallelization
		 * @param anyCamera The optionally returned camera profile that has been assumed internally
		 * @return The list of detected QR codes
		 */
		static inline QRCodes detectQRCodes(const Frame& yFrame, Observations* observations = nullptr, Worker* worker = nullptr, SharedAnyCamera* anyCamera = nullptr);

		/**
		 * Detects QR codes in an 8-bit grayscale image with lens distortions
		 * Use this function for images with lens distortions, for example fisheye lenses on head-mounted devices (HMD). This requires a calibrated camera.
		 * @param anyCamera The camera profile that produced the input image, must be valid
		 * @param yFrame The frame in which QR codes will be detected, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with Y8, minimum size is 29 x 29 pixels
		 * @param observations Optional observations of the detected QR codes that will be returned, will be ignored for `nullptr`
		 * @param worker Optional worker instance for parallelization
		 * @return The list of detected QR codes
		 */
		static inline QRCodes detectQRCodes(const AnyCamera& anyCamera, const Frame& yFrame, Observations* observations = nullptr, Worker* worker = nullptr);

		/**
		 * Detects QR codes in an 8-bit grayscale image
		 * @param anyCamera The camera profile that produced the input image, must be valid
		 * @param yFrame The frame in which QR codes will be detected, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with Y8
		 * @param width The width of the input frame, range: [29, infinity)
		 * @param height The height of the input frame, range: [29, infinity)
		 * @param paddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param observations Optional observations of the detected QR codes that will be returned, will be ignored for `nullptr`
		 * @param worker Optional worker instance for parallelization
		 * @return The list of detected QR codes
		 */
		static QRCodes detectQRCodes(const AnyCamera& anyCamera, const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, Observations* observations = nullptr, Worker* worker = nullptr);
};

inline QRCodeDetector2D::Observation::Observation(const HomogenousMatrix4& code_T_camera, FinderPatternTriplet&& finderPatterns) :
	code_T_camera_(code_T_camera),
	finderPatterns_(std::move(finderPatterns))
{
	ocean_assert(isValid());
}

inline bool QRCodeDetector2D::Observation::isValid() const
{
	return code_T_camera_.isValid() &&
		// Finder pattern locations must be different
		Numeric::isNotEqualEps(finderPatterns_[1].position().sqrDistance(finderPatterns_[0].position())) &&
		Numeric::isNotEqualEps(finderPatterns_[2].position().sqrDistance(finderPatterns_[1].position())) &&
		Numeric::isNotEqualEps(finderPatterns_[0].position().sqrDistance(finderPatterns_[2].position())) &&
		// Ensure counter-clockwise order of the finder patterns
		(finderPatterns_[1].position() - finderPatterns_[0].position()).cross(finderPatterns_[0].position() - finderPatterns_[2].position()) >= Scalar(0) &&
		(finderPatterns_[2].position() - finderPatterns_[1].position()).cross(finderPatterns_[1].position() - finderPatterns_[0].position()) >= Scalar(0) &&
		(finderPatterns_[0].position() - finderPatterns_[2].position()).cross(finderPatterns_[2].position() - finderPatterns_[1].position()) >= Scalar(0);
}

inline const HomogenousMatrix4& QRCodeDetector2D::Observation::code_T_camera() const
{
	return code_T_camera_;
}

inline const FinderPatternTriplet& QRCodeDetector2D::Observation::finderPatterns() const
{
	return finderPatterns_;
}

inline QRCodes QRCodeDetector2D::detectQRCodes(const Frame& yFrame, Observations* observations, Worker* worker, SharedAnyCamera* sharedAnyCamera)
{
	if (!yFrame.isValid() || !FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::FORMAT_Y8) || yFrame.pixelOrigin() != FrameType::ORIGIN_UPPER_LEFT)
	{
		ocean_assert(false && "Frame must be valid and an 8 bit grayscale image and the pixel origin must be the upper left corner");
		return QRCodes();
	}

	Scalar fovX = Numeric::deg2rad(60);

	if (yFrame.height() > yFrame.width())
	{
		// Avoid large FOV values for pinhole cameras.
		fovX = AnyCamera::fovY2X(fovX, Scalar(yFrame.width()) / Scalar(yFrame.height()));
	}

	ocean_assert(fovX > Scalar(0));

	AnyCameraPinhole anyCamera(PinholeCamera(yFrame.width(), yFrame.height(), fovX));

	QRCodes codes = detectQRCodes(anyCamera, yFrame, observations, worker);

	if (sharedAnyCamera)
	{
		*sharedAnyCamera = std::make_shared<AnyCameraPinhole>(std::move(anyCamera));
	}

	return codes;
}

inline QRCodes QRCodeDetector2D::detectQRCodes(const AnyCamera& anyCamera, const Frame& yFrame, Observations* observations, Worker* worker)
{
	if (!yFrame.isValid() || !FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::FORMAT_Y8) || yFrame.pixelOrigin() != FrameType::ORIGIN_UPPER_LEFT)
	{
		ocean_assert(false && "Frame must be valid and an 8 bit grayscale image and the pixel origin must be the upper left corner");
		return QRCodes();
	}

	return detectQRCodes(anyCamera, yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), observations, worker);
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
