/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/math/cameradistortion/win/CameraDistortionMainWindow.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

CameraDistortionMainWindow::CameraDistortionMainWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	distortedWindow_(instance, L"Distorted frame"),
	largeDistortedWindowSmallCamera_(instance, L"Small distorted frame"),
	largeDistortedWindowLargeCamera_(instance, L"Large distorted frame"),
	largeDistortedWindowClampedAtBorderLargeCamera_(instance, L"Large distorted frame with clamped distortion"),
	largeDistortedWindowDampedLargeCamera_(instance, L"Large distorted frame with damped distortion")
{
	// nothing to do here
}

void CameraDistortionMainWindow::onInitialized()
{
	constexpr unsigned int width = 640u;
	constexpr unsigned int height = 480u;

	const PinholeCamera perfectPinholeCamera(width, height, Numeric::deg2rad(65), Scalar(280), Scalar(275));

	PinholeCamera distortedPinholeCamera(perfectPinholeCamera);
	distortedPinholeCamera.setRadialDistortion(PinholeCamera::DistortionPair(Scalar(0.26), Scalar(-0.134)));
	distortedPinholeCamera.setTangentialDistortion(PinholeCamera::DistortionPair(Scalar(0.011), -Scalar(0.075)));

	// normal frame
	Frame undistortedFrame(FrameType(width, height, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
	undistortedFrame.setValue(0xD0);

	// large frame
	Frame largeUndistortedFrame(FrameType(width * 3u, height * 3u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT));
	largeUndistortedFrame.setValue(0xFF);
	const uint8_t gray[3] = {0xD0, 0xD0, 0xD0};

	CV::Canvas::rectangle(largeUndistortedFrame, width, height, width, height, gray);

	constexpr unsigned int binSize = 20u;

	// build the grid for the normal frame
	for (unsigned int y = 0u; y < undistortedFrame.height(); ++y)
	{
		for (unsigned int x = 0u; x < undistortedFrame.width(); ++x)
		{
			if (x != 0u && (x % binSize) == 0u || y != 0u && (y % binSize) == 0u)
			{
				uint8_t* pixel = undistortedFrame.pixel<uint8_t>(x, y);
				pixel[0] = 0u;
				pixel[1] = 0u;
				pixel[2] = 0u;
			}
		}
	}

	// build the grid for the large frame
	for (unsigned int y = 0u; y < largeUndistortedFrame.height(); ++y)
	{
		for (unsigned int x = 0u; x < largeUndistortedFrame.width(); ++x)
		{
			if (x != 0u && (x % binSize) == 0u || y != 0u && (y % binSize) == 0u)
			{
				uint8_t* pixel = largeUndistortedFrame.pixel<uint8_t>(x, y);
				pixel[0] = 0u;
				pixel[1] = 0u;
				pixel[2] = 0u;
			}
		}
	}

	// build the distorted frame, actually flipping distorted/undistorted cameras for a nicer visual effect
	Frame distortedFrame;
	if (!CV::FrameInterpolatorBilinear::Comfort::resampleCameraImage(undistortedFrame, AnyCameraPinhole(distortedPinholeCamera), SquareMatrix3(true), AnyCameraPinhole(perfectPinholeCamera), distortedFrame, nullptr, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return;
	}

	// build the large distorted frame
	Frame largeDistortedFrameSmallCamera(largeUndistortedFrame.frameType());
	Frame largeDistortedFrameLargeCamera(largeUndistortedFrame.frameType());
	Frame largeDistortedFrameClampedAtBorderLargeCamera(largeUndistortedFrame.frameType());
	Frame largeDistortedFrameDampedLargeCamera(largeUndistortedFrame.frameType());

	largeDistortedFrameSmallCamera.setValue(0x00u);
	largeDistortedFrameLargeCamera.setValue(0x00u);
	largeDistortedFrameClampedAtBorderLargeCamera.setValue(0x00u);
	largeDistortedFrameDampedLargeCamera.setValue(0x00u);

	for (unsigned int y = 0u; y < largeDistortedFrameLargeCamera.height(); ++y)
	{
		for (unsigned int x = 0u; x < largeDistortedFrameLargeCamera.width(); ++x)
		{
			const Vector2 undistorted(Scalar(x) - Scalar(width), Scalar(y) - Scalar(height));

			const Vector2 distorted(distortedPinholeCamera.distort<false>(undistorted));

			if (distorted.x() >= Scalar(0) && distorted.x() <= Scalar(width - 1u) && distorted.y() >= Scalar(0) && distorted.y() <= Scalar(height- 1u))
			{
				const Vector2 largeDistorted(distorted + Vector2(Scalar(width), Scalar(height)));
				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_TOP_LEFT>(largeUndistortedFrame.data<uint8_t>(), largeUndistortedFrame.width(), largeUndistortedFrame.height(), largeUndistortedFrame.paddingElements(), largeDistorted, largeDistortedFrameSmallCamera.pixel<uint8_t>(x, y));
			}

			if (distorted.x() >= -Scalar(width) && distorted.x() <= Scalar(width * 2u - 1u) && distorted.y() >= -Scalar(height) && distorted.y() <= Scalar(height * 2u - 1u))
			{
				const Vector2 largeDistorted(distorted + Vector2(Scalar(width), Scalar(height)));
				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_TOP_LEFT>(largeUndistortedFrame.data<uint8_t>(), largeUndistortedFrame.width(), largeUndistortedFrame.height(), largeUndistortedFrame.paddingElements(), largeDistorted, largeDistortedFrameLargeCamera.pixel<uint8_t>(x, y));
			}

			const Vector2 distortedClamped(distortedPinholeCamera.distort<true>(undistorted));

			if (distortedClamped.x() >= -Scalar(width) && distortedClamped.x() <= Scalar(width * 2u - 1u) && distortedClamped.y() >= -Scalar(height) && distortedClamped.y() <= Scalar(height * 2u - 1u))
			{
				const Vector2 largeDistortedClamped(distortedClamped + Vector2(Scalar(width), Scalar(height)));
				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_TOP_LEFT>(largeUndistortedFrame.data<uint8_t>(), largeUndistortedFrame.width(), largeUndistortedFrame.height(), largeUndistortedFrame.paddingElements(), largeDistortedClamped, largeDistortedFrameClampedAtBorderLargeCamera.pixel<uint8_t>(x, y));
			}

			const Vector2 distortedDamped(distortedPinholeCamera.distortDamped(undistorted, 1));

			if (distortedDamped.x() >= -Scalar(width) && distortedDamped.x() <= Scalar(width * 2u - 1u) && distortedDamped.y() >= -Scalar(height) && distortedDamped.y() <= Scalar(height * 2u - 1u))
			{
				const Vector2 largeDistorted(distortedDamped + Vector2(Scalar(width), Scalar(height)));
				CV::FrameInterpolatorBilinear::interpolatePixel8BitPerChannel<3u, CV::PC_TOP_LEFT>(largeUndistortedFrame.data<uint8_t>(), largeUndistortedFrame.width(), largeUndistortedFrame.height(), largeUndistortedFrame.paddingElements(), largeDistorted, largeDistortedFrameDampedLargeCamera.pixel<uint8_t>(x, y));
			}
		}
	}

	const uint8_t* const red = CV::Canvas::red(largeDistortedFrameSmallCamera.pixelFormat());

	CV::Canvas::line(largeDistortedFrameSmallCamera, int(width), int(height), int(width), int(height * 2u), red);
	CV::Canvas::line(largeDistortedFrameSmallCamera, int(width), int(height * 2u), int(width * 2u), int(height * 2u), red);
	CV::Canvas::line(largeDistortedFrameSmallCamera, int(width * 2u), int(height * 2u), int(width * 2u), int(height), red);
	CV::Canvas::line(largeDistortedFrameSmallCamera, int(width * 2u), int(height), int(width), int(height), red);

	CV::Canvas::line(largeDistortedFrameLargeCamera, int(width), int(height), int(width), int(height * 2u), red);
	CV::Canvas::line(largeDistortedFrameLargeCamera, int(width), int(height * 2u), int(width * 2u), int(height * 2u), red);
	CV::Canvas::line(largeDistortedFrameLargeCamera, int(width * 2u), int(height * 2u), int(width * 2u), int(height), red);
	CV::Canvas::line(largeDistortedFrameLargeCamera, int(width * 2u), int(height), int(width), int(height), red);

	CV::Canvas::line(largeDistortedFrameClampedAtBorderLargeCamera, int(width), int(height), int(width), int(height * 2u), red);
	CV::Canvas::line(largeDistortedFrameClampedAtBorderLargeCamera, int(width), int(height * 2u), int(width * 2u), int(height * 2u), red);
	CV::Canvas::line(largeDistortedFrameClampedAtBorderLargeCamera, int(width * 2u), int(height * 2u), int(width * 2u), int(height), red);
	CV::Canvas::line(largeDistortedFrameClampedAtBorderLargeCamera, int(width * 2u), int(height), int(width), int(height), red);

	CV::Canvas::line(largeDistortedFrameDampedLargeCamera, int(width), int(height), int(width), int(height * 2u), red);
	CV::Canvas::line(largeDistortedFrameDampedLargeCamera, int(width), int(height * 2u), int(width * 2u), int(height * 2u), red);
	CV::Canvas::line(largeDistortedFrameDampedLargeCamera, int(width * 2u), int(height * 2u), int(width * 2u), int(height), red);
	CV::Canvas::line(largeDistortedFrameDampedLargeCamera, int(width * 2u), int(height), int(width), int(height), red);

	distortedWindow_.setParent(handle());
	distortedWindow_.initialize();
	distortedWindow_.show();

	largeDistortedWindowSmallCamera_.setParent(handle());
	largeDistortedWindowSmallCamera_.initialize();
	largeDistortedWindowSmallCamera_.show();

	largeDistortedWindowLargeCamera_.setParent(handle());
	largeDistortedWindowLargeCamera_.initialize();
	largeDistortedWindowLargeCamera_.show();

	largeDistortedWindowClampedAtBorderLargeCamera_.setParent(handle());
	largeDistortedWindowClampedAtBorderLargeCamera_.initialize();
	largeDistortedWindowClampedAtBorderLargeCamera_.show();

	largeDistortedWindowDampedLargeCamera_.setParent(handle());
	largeDistortedWindowDampedLargeCamera_.initialize();
	largeDistortedWindowDampedLargeCamera_.show();

	setFrame(undistortedFrame);
	distortedWindow_.setFrame(distortedFrame);
	largeDistortedWindowSmallCamera_.setFrame(largeDistortedFrameSmallCamera);
	largeDistortedWindowLargeCamera_.setFrame(largeDistortedFrameLargeCamera);
	largeDistortedWindowClampedAtBorderLargeCamera_.setFrame(largeDistortedFrameClampedAtBorderLargeCamera);
	largeDistortedWindowDampedLargeCamera_.setFrame(largeDistortedFrameDampedLargeCamera);
}
