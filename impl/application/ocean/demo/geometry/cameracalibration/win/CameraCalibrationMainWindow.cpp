/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/geometry/cameracalibration/win/CameraCalibrationMainWindow.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"

#include "ocean/geometry/CameraCalibration.h"
#include "ocean/geometry/Homography.h"
#include "ocean/geometry/NonLinearOptimizationCamera.h"
#include "ocean/geometry/RANSAC.h"

#include "ocean/math/Pose.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/platform/win/Utilities.h"

CameraCalibrationMainWindow::CameraCalibrationMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	undistortWindow_(instance, L"Undistorted"),
	undistortGridWindow_(instance, L"Undistorted grid"),
	mediaFile_(file),
	calibrationPatternDetector_(5u, 7u)
{
	ocean_assert(calibrationPatternDetector_.isValid());
}

CameraCalibrationMainWindow::~CameraCalibrationMainWindow()
{
	// nothing to do here
}

void CameraCalibrationMainWindow::onInitialized()
{
	RandomI::initialize();

	if (!mediaFile_.empty())
	{
		frameMedium_ = Media::Manager::get().newMedium(mediaFile_, Media::Medium::FRAME_MEDIUM);
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:1");
	}

	if (frameMedium_.isNull())
	{
		frameMedium_ = Media::Manager::get().newMedium("LiveVideoId:0");
	}

	const Media::FiniteMediumRef finiteMedium(frameMedium_);

	if (finiteMedium)
	{
		finiteMedium->setLoop(true);
	}

	if (frameMedium_)
	{
		frameMedium_->setPreferredFrameDimension(1280u, 720u);
		frameMedium_->start();
	}

	undistortWindow_.setParent(handle());
	undistortWindow_.initialize();
	undistortWindow_.show();

	undistortGridWindow_.setParent(handle());
	undistortGridWindow_.initialize();
	undistortGridWindow_.show();

	ocean_assert(calibrationPatternDetector_.horizontalBoxes() != 0u && calibrationPatternDetector_.verticalBoxes() != 0u);
	ocean_assert(calibrationPatternHorizontalSize_ > 0 && calibrationPatternVerticalSize_ > 0);
	ocean_assert(calibrationPatternObjectPoints_.empty());

	const unsigned int horizontalCorners = calibrationPatternDetector_.horizontalBoxes() * 2u;
	const unsigned int verticalCorners = calibrationPatternDetector_.verticalBoxes() * 2u;

	for (unsigned int y = 0u; y < verticalCorners; ++y)
	{
		for (unsigned int x = 0u; x < horizontalCorners; ++x)
		{
			calibrationPatternObjectPoints_.emplace_back(Scalar(calibrationPatternHorizontalSize_) * Scalar(x) / Scalar(horizontalCorners - 1u), Scalar(calibrationPatternVerticalSize_) * Scalar(y) / Scalar(verticalCorners - 1u), Scalar(0));
		}
	}
}

void CameraCalibrationMainWindow::onIdle()
{
	if (frameMedium_)
	{
		const FrameRef frame(frameMedium_->frame());

		if (frame && *frame && frame->timestamp() != frameTimestamp_)
		{
			onFrame(*frame);

			frameTimestamp_ = frame->timestamp();
			return;
		}
	}

	Sleep(1);
}

void CameraCalibrationMainWindow::onFrame(const Ocean::Frame& frame)
{
	if (!perfectCamera_)
	{
		perfectCamera_ = PinholeCamera(frame.width(), frame.height(), Numeric::deg2rad(60));

		calibrationCamera_ = perfectCamera_;
	}

	ocean_assert(calibrationCamera_.width() == frame.width() && calibrationCamera_.height() == frame.height());

	Frame topLeft;
	if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, topLeft, false, WorkerPool::get().scopedWorker()()))
	{
		return;
	}

	Frame gridFrame(topLeft.frameType());
	gridFrame.setValue(0xFF);

	for (unsigned int y = 10u; y < gridFrame.height(); y += 20)
	{
		memset(gridFrame.row<uint8_t>(y), 0x00, gridFrame.planeWidthBytes(0u));
	}

	for (unsigned int y = 0u; y < gridFrame.height(); ++y)
	{
		uint8_t* gridFrameRow = gridFrame.row<uint8_t>(y);

		for (unsigned int x = 10u; x < gridFrame.width(); x += 20)
		{
			gridFrameRow[3u * x + 0u] = 0x00;
			gridFrameRow[3u * x + 1u] = 0x00;
			gridFrameRow[3u * x + 2u] = 0x00;
		}
	}

	if (calibrationCamera_)
	{
		Frame undistorted;
		if (CV::FrameInterpolatorBilinear::Comfort::resampleCameraImage(topLeft, AnyCameraPinhole(calibrationCamera_), SquareMatrix3(true), AnyCameraPinhole(perfectCamera_), undistorted, nullptr, WorkerPool::get().scopedWorker()()))
		{
			undistortWindow_.setFrame(undistorted);
		}

		if (CV::FrameInterpolatorBilinear::Comfort::resampleCameraImage(gridFrame, AnyCameraPinhole(calibrationCamera_), SquareMatrix3(true), AnyCameraPinhole(perfectCamera_), undistorted, nullptr, WorkerPool::get().scopedWorker()()))
		{
			undistortGridWindow_.setFrame(undistorted);
		}
	}
	else
	{
		undistortWindow_.setFrame(topLeft);
		undistortGridWindow_.setFrame(gridFrame);
	}

	Scalar poseError = Numeric::maxValue();

	if (calibrationPatternDetector_.detectPattern(frame, WorkerPool::get().scopedWorker()()))
	{
		const CV::Detector::CalibrationPatternDetector::Pattern& pattern = calibrationPatternDetector_.pattern();

		const unsigned char green[3] = {0x00, 0xFF, 0x00};

		for (unsigned int n = 0u; n < 4u; ++n)
		{
			CV::Canvas::line<1u>(topLeft, pattern.corners()[n], pattern.corners()[(n + 1) % 4u], green);
		}

		Vectors2 imagePoints;
		imagePoints.reserve(calibrationPatternObjectPoints_.size());

		for (CV::Detector::CalibrationPatternDetector::Pattern::PatternRows::const_iterator iR = pattern.rows().begin(); iR != pattern.rows().end(); ++iR)
		{
			for (CV::Detector::CalibrationPatternDetector::Pattern::PatternRow::const_iterator iC = iR->begin(); iC != iR->end(); ++iC)
			{
				imagePoints.push_back(*iC);
			}
		}

		ocean_assert(imagePoints.size() == calibrationPatternObjectPoints_.size());

		for (Vectors2::const_iterator i = imagePoints.begin(); i != imagePoints.end(); ++i)
		{
			CV::Canvas::point<3u>(topLeft, *i, green);
		}

		HomogenousMatrix4 pose(false);
		if (Geometry::RANSAC::p3p(AnyCameraPinhole(calibrationCamera_), ConstArrayAccessor<Vector3>(calibrationPatternObjectPoints_), ConstArrayAccessor<Vector2>(imagePoints), randomGenerator_, pose))
		{
			ocean_assert(pose.isValid());

			poseError = Geometry::Error::determinePoseError<ConstTemplateArrayAccessor<Vector3>, ConstTemplateArrayAccessor<Vector2>, false, false>(pose, AnyCameraPinhole(calibrationCamera_), ConstTemplateArrayAccessor<Vector3>(calibrationPatternObjectPoints_), ConstTemplateArrayAccessor<Vector2>(imagePoints));

			calibrationPatternImagePointsGroups_.push_back(std::move(imagePoints));
			calibrationPatternPoses_.push_back(pose);
		}
	}

	if (calibrationPatternPoses_.size() > 400)
	{
		calibration();
	}

	setFrame(topLeft);

	Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 5, std::string("Next calibration: ") + String::toAString(calibrationPatternPoses_.size()) + std::string(" / 400"));

	if (poseError != Numeric::maxValue())
	{
		Platform::Win::Utilities::textOutput(bitmap().dc(), 5, 25, std::string("Current error: ") + String::toAString(Numeric::sqrt(poseError)) + std::string("px"));
	}

	repaint();
	undistortWindow_.repaint();
	undistortGridWindow_.repaint();
}

bool CameraCalibrationMainWindow::calibration()
{
	ocean_assert(calibrationPatternPoses_.size() >= 40);

	const PinholeCamera previousCamera(calibrationCamera_);

	// we determine the 40 most different camera poses
	const Indices32 subsetIndices = Pose::representativePoses(calibrationPatternPoses_.data(), calibrationPatternPoses_.size(), 40u);

	Scalar idealFovX = 0;
	if (!Geometry::CameraCalibration::determineBestMatchingFovX(calibrationCamera_.width(), calibrationCamera_.height(), ConstArraySubsetAccessor<HomogenousMatrix4, unsigned int>(calibrationPatternPoses_, subsetIndices), ConstElementAccessor<Vectors3>(subsetIndices.size(), calibrationPatternObjectPoints_), ConstArraySubsetAccessor<Vectors2, unsigned int>(calibrationPatternImagePointsGroups_, subsetIndices), idealFovX, false, Numeric::deg2rad(35), Numeric::deg2rad(85), 30u))
	{
		return false;
	}

	HomogenousMatrices4 optimizedPoses(subsetIndices.size());
	NonconstArrayAccessor<HomogenousMatrix4> optimizedPosesAccessor(optimizedPoses);

	if (!Geometry::CameraCalibration::determineBestMatchingFovX(calibrationCamera_.width(), calibrationCamera_.height(), ConstArraySubsetAccessor<HomogenousMatrix4, unsigned int>(calibrationPatternPoses_, subsetIndices), ConstElementAccessor<Vectors3>(subsetIndices.size(), calibrationPatternObjectPoints_), ConstArraySubsetAccessor<Vectors2, unsigned int>(calibrationPatternImagePointsGroups_, subsetIndices), idealFovX, false, idealFovX - Numeric::deg2rad(5), idealFovX + Numeric::deg2rad(5), 10u, &optimizedPosesAccessor))
	{
		return false;
	}

	Log::info() << "Initial horizontal field of view: " << Numeric::rad2deg(idealFovX) << "deg";

	calibrationCamera_ = PinholeCamera(calibrationCamera_.width(), calibrationCamera_.height(), idealFovX);
	calibrationCamera_.setRadialDistortion(previousCamera.radialDistortion());
	calibrationCamera_.setTangentialDistortion(previousCamera.tangentialDistortion());

#ifdef OCEAN_DEBUG
	Scalar totalPreviousError = 0;
	Scalar totalOptimizedError = 0;
	size_t measurements = 0;

	for (size_t n = 0; n < subsetIndices.size(); ++n)
	{
		const HomogenousMatrix4& previousPose = calibrationPatternPoses_[subsetIndices[n]];
		const HomogenousMatrix4& optimizedPose = optimizedPoses[n];

		const Vectors2& imagePoints = calibrationPatternImagePointsGroups_[subsetIndices[n]];
		ocean_assert(imagePoints.size() == calibrationPatternObjectPoints_.size());

		for (size_t i = 0; i < imagePoints.size(); ++i)
		{
			const Vector2 projectedPreviousObjectPoint(previousCamera.projectToImage<true>(previousPose, calibrationPatternObjectPoints_[i], previousCamera.hasDistortionParameters()));
			const Vector2 projectedOptimizedObjectPoint(calibrationCamera_.projectToImage<true>(optimizedPose, calibrationPatternObjectPoints_[i], previousCamera.hasDistortionParameters()));

			const Scalar previousError = imagePoints[i].sqrDistance(projectedPreviousObjectPoint);
			const Scalar optimizedError = imagePoints[i].sqrDistance(projectedOptimizedObjectPoint);

			totalPreviousError += previousError;
			totalOptimizedError += optimizedError;

			measurements++;
		}
	}

	totalPreviousError /= Scalar(measurements);
	totalOptimizedError /= Scalar(measurements);

	ocean_assert(totalOptimizedError <= totalPreviousError);
#endif

	PinholeCamera optimizedCamera;
	Scalar finalError = Numeric::maxValue();

	if (!Geometry::NonLinearOptimizationCamera::optimizeCameraPoses(calibrationCamera_, ConstArrayAccessor<HomogenousMatrix4>(optimizedPoses), ConstElementAccessor<Vectors3>(optimizedPoses.size(), calibrationPatternObjectPoints_), ConstArraySubsetAccessor<Vectors2, unsigned int>(calibrationPatternImagePointsGroups_, subsetIndices), optimizedCamera, nullptr, 50u, Geometry::Estimator::ET_SQUARE, Scalar(0.001), Scalar(5), true, nullptr, &finalError))
	{
		return false;
	}

	calibrationCamera_ = optimizedCamera;

	Log::info() << "Camera optimization with " << optimizedPoses.size() << " keyframes succeeded with a final pixel error: " << Numeric::sqrt(finalError);
	Log::info() << "Camera parameters optimized:";
	Log::info() << "Width: " << calibrationCamera_.width();
	Log::info() << "Height: " << calibrationCamera_.height();
	Log::info() << "mx: " << String::toAString(calibrationCamera_.principalPointX(), 8u);
	Log::info() << "my: " << String::toAString(calibrationCamera_.principalPointY(), 8u);
	Log::info() << "Fx: " << String::toAString(calibrationCamera_.focalLengthX(), 8u);
	Log::info() << "Fy: " << String::toAString(calibrationCamera_.focalLengthY(), 8u);
	Log::info() << "Radial k1 : " << String::toAString(calibrationCamera_.radialDistortion().first, 8u);
	Log::info() << "Radial k2 : " << String::toAString(calibrationCamera_.radialDistortion().second, 8u);
	Log::info() << "Tangential p1 : " << String::toAString(calibrationCamera_.tangentialDistortion().first, 8u);
	Log::info() << "Tangential p2 : " << String::toAString(calibrationCamera_.tangentialDistortion().second, 8u);
	Log::info() << "FovX : " << Numeric::rad2deg(calibrationCamera_.fovX());
	Log::info() << " ";

	calibrationPatternImagePointsGroups_.clear();
	calibrationPatternPoses_.clear();

	return true;
}
