/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include "ocean/cv/detector/qrcodes/QRCodes.h"

#include "ocean/cv/detector/qrcodes/FinderPatternDetector.h"
#include "ocean/cv/detector/qrcodes/MicroQRCode.h"
#include "ocean/cv/detector/qrcodes/MicroQRCodeDetector.h"

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
 * This class implements a detector for Micro QR Codes.
 * @ingroup cvdetectorqrcodes
 */
class OCEAN_CV_DETECTOR_QRCODES_EXPORT MicroQRCodeDetector2D : public MicroQRCodeDetector
{
	public:

		/**
		 * Definition of an observation of Micro QR code in 2D
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
				 * @param code_T_camera The transformation that maps 3D coordinates in the Micro QR code grid to the camera frame of reference, i.e., `imagePoint = anyCamera.projectToImage(code_T_camera, codePoint)`
				 * @param finderPattern The finder patterns of the Micro QR Code
				 */
				inline explicit Observation(const HomogenousMatrix4& code_T_camera, const FinderPattern& finderPattern);

				/**
				 * Returns if the observation is valid
				 * @return True if the observation is valid, otherwise false
				 */
				inline bool isValid() const;

				/**
				 * Returns the transformation that maps coordinates in the Micro QR code grid to coordinates in the reference frame of the camera
				 * @return The transformation
				 */
				inline const HomogenousMatrix4& code_T_camera() const;

				/**
				 * Returns a pointer to the finder patterns
				 * @return The finder patterns
				 */
				inline const FinderPattern& finderPattern() const;

			protected:

				/// The transformation that maps 3D coordinates in the Micro QR code grid to the camera frame of reference, i.e., `imagePoint = anyCamera.projectToImage(code_T_camera, codePoint)`
				HomogenousMatrix4 code_T_camera_ = HomogenousMatrix4(false);

				/// The finder pattern of the Micro QR code
				FinderPattern finderPattern_;
		};

		/// Definition of a vector of observations
		typedef std::vector<Observation> Observations;

	public:

		/**
		 * Detects Micro QR codes in an 8-bit grayscale image without lens distortion.
		 * Use this function for images without lens distortion, for example from pinhole cameras, screenshots, or similar.
		 * @param yFrame The frame in which Micro QR codes will be detected, must be valid, have its origin in the upper left corner, and have a pixel format that is compatible with Y8, minimum size is 15 x 15 pixels
		 * @param observations Optional observations of the detected Micro QR codes that will be returned, will be ignored for `nullptr`
		 * @param worker Optional worker instance for parallelization
		 * @param anyCamera The optionally returned camera profile that has been assumed internally
		 * @return The list of detected Micro QR codes
		 */
		static inline MicroQRCodes detectMicroQRCodes(const Frame& yFrame, Observations* observations = nullptr, Worker* worker = nullptr, SharedAnyCamera* anyCamera = nullptr);

		/**
		 * Detects Micro QR codes in an 8-bit grayscale image with lens distortions
		 * Use this function for images with lens distortions, for example fisheye lenses on head-mounted devices (HMD). This requires a calibrated camera.
		 * @param anyCamera The camera profile that produced the input image, must be valid
		 * @param yFrame The frame in which Micro QR codes will be detected, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with Y8, minimum size is 15 x 15 pixels
		 * @param observations Optional observations of the detected Micro QR codes that will be returned, will be ignored for `nullptr`
		 * @param worker Optional worker instance for parallelization
		 * @return The list of detected Micro QR codes
		 */
		static inline MicroQRCodes detectMicroQRCodes(const AnyCamera& anyCamera, const Frame& yFrame, Observations* observations = nullptr, Worker* worker = nullptr);

		/**
		 * Detects Micro QR codes in an 8-bit grayscale image
		 * @param anyCamera The camera profile that produced the input image, must be valid
		 * @param yFrame The frame in which Micro QR codes will be detected, must be valid, match the camera size, have its origin in the upper left corner, and have a pixel format that is compatible with Y8
		 * @param width The width of the input frame, range: [15, infinity)
		 * @param height The height of the input frame, range: [15, infinity)
		 * @param paddingElements The number of padding elements of the input frame, range: [0, infinity)
		 * @param observations Optional observations of the detected Micro QR codes that will be returned, will be ignored for `nullptr`
		 * @param worker Optional worker instance for parallelization
		 * @return The list of detected Micro QR codes
		 */
		static MicroQRCodes detectMicroQRCodes(const AnyCamera& anyCamera, const uint8_t* const yFrame, const unsigned int width, const unsigned int height, const unsigned int paddingElements, Observations* observations = nullptr, Worker* worker = nullptr);
};

inline MicroQRCodeDetector2D::Observation::Observation(const HomogenousMatrix4& code_T_camera, const FinderPattern& finderPattern) :
	code_T_camera_(code_T_camera),
	finderPattern_(finderPattern)
{
	ocean_assert(isValid());
}

inline bool MicroQRCodeDetector2D::Observation::isValid() const
{
	return code_T_camera_.isValid();
}

inline const HomogenousMatrix4& MicroQRCodeDetector2D::Observation::code_T_camera() const
{
	return code_T_camera_;
}

inline const FinderPattern& MicroQRCodeDetector2D::Observation::finderPattern() const
{
	return finderPattern_;
}

inline MicroQRCodes MicroQRCodeDetector2D::detectMicroQRCodes(const Frame& yFrame, Observations* observations, Worker* worker, SharedAnyCamera* sharedAnyCamera)
{
	if (!yFrame.isValid() || !FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::FORMAT_Y8) || yFrame.pixelOrigin() != FrameType::ORIGIN_UPPER_LEFT)
	{
		ocean_assert(false && "Frame must be valid and an 8 bit grayscale image and the pixel origin must be the upper left corner");
		return MicroQRCodes();
	}

	Scalar fovX = Numeric::deg2rad(60);

	if (yFrame.height() > yFrame.width())
	{
		// Avoid large FOV values for pinhole cameras.
		fovX = AnyCamera::fovY2X(fovX, Scalar(yFrame.width()) / Scalar(yFrame.height()));
	}

	ocean_assert(fovX > Scalar(0));

	AnyCameraPinhole anyCamera(PinholeCamera(yFrame.width(), yFrame.height(), fovX));

	MicroQRCodes codes = detectMicroQRCodes(anyCamera, yFrame, observations, worker);

	if (sharedAnyCamera)
	{
		*sharedAnyCamera = std::make_shared<AnyCameraPinhole>(std::move(anyCamera));
	}

	return codes;
}

inline MicroQRCodes MicroQRCodeDetector2D::detectMicroQRCodes(const AnyCamera& anyCamera, const Frame& yFrame, Observations* observations, Worker* worker)
{
	if (!yFrame.isValid() || !FrameType::arePixelFormatsCompatible(yFrame.pixelFormat(), FrameType::FORMAT_Y8) || yFrame.pixelOrigin() != FrameType::ORIGIN_UPPER_LEFT)
	{
		ocean_assert(false && "Frame must be valid and an 8 bit grayscale image and the pixel origin must be the upper left corner");
		return MicroQRCodes();
	}

	return detectMicroQRCodes(anyCamera, yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), observations, worker);
}

} // namespace QRCodes

} // namespace Detector

} // namespace CV

} // namespace Ocean
