/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/math/cameraundistortion/win/CameraUndistortionMainWindow.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/media/Utilities.h"

#include "ocean/platform/win/System.h"
#include "ocean/platform/win/Utilities.h"

CameraUndistortionMainWindow::CameraUndistortionMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& mediaFilename) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	undistortedWindow_(instance, L"Undistorted Frame")
{
	if (!mediaFilename.empty())
	{
		distortedFrame_ = Media::Utilities::loadImage(mediaFilename);

		if (distortedFrame_ && !CV::FrameConverter::Comfort::change(distortedFrame_, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
		{
			distortedFrame_.release();
		}
	}
}

void CameraUndistortionMainWindow::onInitialized()
{
	if (!distortedFrame_.isValid())
	{
		return;
	}

	perfectPinholeCamera_ = PinholeCamera(distortedFrame_.width(), distortedFrame_.height(), Numeric::deg2rad(60));
	distortedPinholeCamera_ = perfectPinholeCamera_;

	setFrame(distortedFrame_);

	undistortedWindow_.setParent(handle());
	undistortedWindow_.initialize();
	undistortedWindow_.show();
}

void CameraUndistortionMainWindow::onIdle()
{
	if (distortedFrame_)
	{
		ocean_assert(distortedPinholeCamera_.isValid());
		ocean_assert(perfectPinholeCamera_.isValid());

		Frame undistortedFrame;
		if (CV::FrameInterpolatorBilinear::Comfort::resampleCameraImage(distortedFrame_, AnyCameraPinhole(distortedPinholeCamera_), SquareMatrix3(true), AnyCameraPinhole(perfectPinholeCamera_), undistortedFrame, nullptr, WorkerPool::get().scopedWorker()()))
		{
			undistortedWindow_.setFrame(undistortedFrame);
			undistortedWindow_.repaint();
		}

		Platform::Win::Utilities::textOutput(dc(), 5, 5, std::string("Dimension: ") + String::toAString(distortedPinholeCamera_.width()) + std::string("x") + String::toAString(distortedPinholeCamera_.height()));
		Platform::Win::Utilities::textOutput(dc(), 5, 25, std::string("FovX: ") + String::toAString(Numeric::rad2deg(distortedPinholeCamera_.fovX())));
		Platform::Win::Utilities::textOutput(dc(), 5, 45, std::string("Radial distortion: ") + String::toAString(distortedPinholeCamera_.radialDistortion().first) + std::string(", ") + String::toAString(distortedPinholeCamera_.radialDistortion().second));
		Platform::Win::Utilities::textOutput(dc(), 5, 65, std::string("Tangential distortion: ") + String::toAString(distortedPinholeCamera_.tangentialDistortion().first) + std::string(", ") + String::toAString(distortedPinholeCamera_.tangentialDistortion().second));
	}
	else
	{
		Platform::Win::Utilities::textOutput(dc(), 5, 5, "Invalid frame!");
	}

	Sleep(1);
}

void CameraUndistortionMainWindow::onKeyDown(const int key)
{
	if (!distortedPinholeCamera_.isValid())
	{
		return;
	}

	PinholeCamera::DistortionPair radialDistortion = distortedPinholeCamera_.radialDistortion();
	PinholeCamera::DistortionPair tangentialDistortion = distortedPinholeCamera_.tangentialDistortion();

	switch (key)
	{
		case '1':
			updateStep_ *= Scalar(0.1);
			break;

		case '2':
			updateStep_ *= Scalar(10);
			break;

		case 'Q':
			radialDistortion.first -= updateStep_;
			break;

		case 'W':
			radialDistortion.first += updateStep_;
			break;

		case 'E':
			radialDistortion.second -= updateStep_;
			break;

		case 'R':
			radialDistortion.second += updateStep_;
			break;


		case 'A':
			tangentialDistortion.first -= updateStep_;
			break;

		case 'S':
			tangentialDistortion.first += updateStep_;
			break;

		case 'D':
			tangentialDistortion.second -= updateStep_;
			break;

		case 'F':
			tangentialDistortion.second += updateStep_;
			break;


		case 'Y':
			if (distortedPinholeCamera_.fovX() > Numeric::deg2rad(1))
			{
				distortedPinholeCamera_.setIntrinsic(PinholeCamera(distortedPinholeCamera_.width(), distortedPinholeCamera_.height(), distortedPinholeCamera_.fovX() - Numeric::deg2rad(1)).intrinsic());
			}
			break;

		case 'X':
			if (distortedPinholeCamera_.fovX() < Numeric::deg2rad(179))
			{
				distortedPinholeCamera_.setIntrinsic(PinholeCamera(distortedPinholeCamera_.width(), distortedPinholeCamera_.height(), distortedPinholeCamera_.fovX() + Numeric::deg2rad(1)).intrinsic());
			}
			break;

		case ' ':
			distortedPinholeCamera_ = perfectPinholeCamera_;
			return;
	}

	distortedPinholeCamera_.setRadialDistortion(radialDistortion);
	distortedPinholeCamera_.setTangentialDistortion(tangentialDistortion);
}
