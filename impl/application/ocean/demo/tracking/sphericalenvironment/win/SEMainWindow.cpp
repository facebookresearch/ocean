/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/sphericalenvironment/win/SEMainWindow.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/math/Random.h"
#include "ocean/math/SuccessionSubset.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Movie.h"
#include "ocean/media/MovieFrameProvider.h"

#include "ocean/platform/win/Utilities.h"

SEMainWindow::SEMainWindow(HINSTANCE instance, const std::wstring& name, const std::string& file) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	mediaFile_(file),
	panoramaFrameWindow(instance, L"Panorama Frame"),
	panoramaFrameWindowFineAdjustment(instance, L"Panorama Frame (with fine adjustment)"),
	panoramaFrameWindowOptimizedCamera(instance, L"Optimized Panorama Frame"),
	panoramaFrameWindowOptimizedCameraFineAdjustment(instance, L"Optimized Panorama Frame (with fine adjustment)")
{
	// nothing to do here
}

SEMainWindow::~SEMainWindow()
{
	// nothing to do here
}

void SEMainWindow::onInitialized()
{
	Random::initialize();

	if (!mediaFile_.empty())
	{
		const Media::MovieRef movie = Media::Manager::get().newMedium(mediaFile_, Media::Medium::MOVIE, true);

		if (movie)
		{
			const Media::MovieFrameProviderRef frameProvider(new Media::MovieFrameProvider(true, 1500, 1500));
			frameProvider->setMovie(movie);

			frameProviderInterface_ = CV::FrameProviderInterfaceRef(new Media::MovieFrameProviderInterface(frameProvider));
		}
	}

	panoramaFrameWindow.setParent(handle());
	panoramaFrameWindow.initialize();

	panoramaFrameWindowFineAdjustment.setParent(handle());
	panoramaFrameWindowFineAdjustment.initialize();

	panoramaFrameWindowOptimizedCamera.setParent(handle());
	panoramaFrameWindowOptimizedCamera.initialize();

	panoramaFrameWindowOptimizedCameraFineAdjustment.setParent(handle());
	panoramaFrameWindowOptimizedCameraFineAdjustment.initialize();
}

void SEMainWindow::onIdle()
{
	if (frameProviderInterface_.isNull())
	{
		Platform::Win::Utilities::textOutput(dc(), 5, 5, "Failed!");

		Thread::sleep(1);
		return;
	}

	if (!frameProviderInterface_->isInitialized())
	{
		Platform::Win::Utilities::textOutput(dc(), 5, 5, "Initializing...");

		Thread::sleep(1);
		return;
	}

	if (!camera_.isValid())
	{
		ocean_assert(frameIndex_ == 0u && frameNumber_ == 0u);

		frameProviderInterface_->setPreferredFrameType(FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT);

		const FrameType frameType = frameProviderInterface_->synchronFrameTypeRequest();
		ocean_assert(frameType.isValid());

		camera_ = PinholeCamera(frameType.width(), frameType.height(), Numeric::deg2rad(60));

		sphericalEnvironment_ = Tracking::SphericalEnvironment(camera_.width() * 6u, camera_.width() * 3u, 0xFF, CV::Advanced::PanoramaFrame::UM_AVERAGE_GLOBAL);
		sphericalEnvironmentFineAdjustment_ = Tracking::SphericalEnvironment(camera_.width() * 6u, camera_.width() * 3u, 0xFF, CV::Advanced::PanoramaFrame::UM_AVERAGE_GLOBAL);

		sphericalEnvironmentOptimizedCamera_ = Tracking::SphericalEnvironment(camera_.width() * 6u, camera_.width() * 3u, 0xFF, CV::Advanced::PanoramaFrame::UM_AVERAGE_GLOBAL);
		sphericalEnvironmentOptimizedCameraFineAdjustment_ = Tracking::SphericalEnvironment(camera_.width() * 6u, camera_.width() * 3u, 0xFF, CV::Advanced::PanoramaFrame::UM_AVERAGE_GLOBAL);

		frameIndex_ = 0u;
		frameNumber_ = frameProviderInterface_->synchronFrameNumberRequest();

		orientations_ = SquareMatrices3(frameNumber_, SquareMatrix3(false));

		panoramaFrameWindow.show();
		panoramaFrameWindowFineAdjustment.show();
	}

	if (frameIndex_ < frameNumber_)
	{
		const FrameRef frame = frameProviderInterface_->synchronFrameRequest(frameIndex_);

		if (frame.isNull())
		{
			ocean_assert(false && "This should never happen!");
			return;
		}

		if (camera_.hasDistortionParameters())
		{
			onFrameOptimizedCamera(*frame);
		}
		else
		{
			onFrame(*frame);
		}

		frameIndex_++;
	}
	else if (frameIndex_ == frameNumber_)
	{
		// we are at the end of the second iteration
		if (camera_.hasDistortionParameters())
		{
			Thread::sleep(1);
			return;
		}

		optimizeCameraProfile();
		frameIndex_ = 0u;

		panoramaFrameWindowOptimizedCamera.show();
		panoramaFrameWindowOptimizedCameraFineAdjustment.show();
	}
	else
	{
		Thread::sleep(1);
	}
}

void SEMainWindow::onFrame(const Frame& frame)
{
	setFrame(frame);

	performance_.start();
	if (!sphericalEnvironment_.extendEnvironment(camera_, frame, 20u, Geometry::Estimator::ET_INVALID, false, WorkerPool::get().scopedWorker()()))
	{
		performance_.skip();

		Log::error() << "Failed to extend environment";
		return;
	}
	performance_.stop();

	performanceFineAdjustment_.start();
	if (!sphericalEnvironmentFineAdjustment_.extendEnvironment(camera_, frame, 20u, Geometry::Estimator::ET_HUBER, false, WorkerPool::get().scopedWorker()(), &orientations_[frameIndex_], nullptr, Tracking::SphericalEnvironment::FrameCallback::createStatic(Tracking::SphericalEnvironment::nonHomographyMask)))
	{
		performanceFineAdjustment_.skip();

		Log::error() << "Failed to extend environment";
		return;
	}
	performanceFineAdjustment_.stop();

	panoramaFrameWindow.setFrame(sphericalEnvironment_.frame());
	Platform::Win::Utilities::textOutput(panoramaFrameWindow.bitmap().dc(), 5, 5, String::toAString(performance_.lastMseconds()) + std::string("ms, ") + String::toAString(performance_.averageMseconds()) + std::string("ms"));
	panoramaFrameWindow.repaint();

	panoramaFrameWindowFineAdjustment.setFrame(sphericalEnvironmentFineAdjustment_.frame());
	Platform::Win::Utilities::textOutput(panoramaFrameWindowFineAdjustment.bitmap().dc(), 5, 5, String::toAString(performanceFineAdjustment_.lastMseconds()) + std::string("ms, ") + String::toAString(performanceFineAdjustment_.averageMseconds()) + std::string("ms"));
	panoramaFrameWindowFineAdjustment.repaint();

	repaint();
}

void SEMainWindow::onFrameOptimizedCamera(const Frame& frame)
{
	setFrame(frame);

	performanceOptimizedCamera_.start();
		sphericalEnvironmentOptimizedCamera_.extendEnvironment(camera_, frame, 20u, Geometry::Estimator::ET_INVALID, false, WorkerPool::get().scopedWorker()(), nullptr, nullptr, Tracking::SphericalEnvironment::FrameCallback::createStatic(Tracking::SphericalEnvironment::nonHomographyMask));
	performanceOptimizedCamera_.stop();

	performanceOptimizedCameraFineAdjustment_.start();
		sphericalEnvironmentOptimizedCameraFineAdjustment_.extendEnvironment(camera_, frame, 20u, Geometry::Estimator::ET_HUBER, false, WorkerPool::get().scopedWorker()(), nullptr, nullptr, Tracking::SphericalEnvironment::FrameCallback::createStatic(Tracking::SphericalEnvironment::nonHomographyMask));
	performanceOptimizedCameraFineAdjustment_.stop();

	panoramaFrameWindowOptimizedCamera.setFrame(sphericalEnvironmentOptimizedCamera_.frame());
	Platform::Win::Utilities::textOutput(panoramaFrameWindowOptimizedCamera.bitmap().dc(), 5, 5, String::toAString(performanceOptimizedCamera_.lastMseconds()) + std::string("ms, ") + String::toAString(performanceOptimizedCamera_.averageMseconds()) + std::string("ms"));
	panoramaFrameWindowOptimizedCamera.repaint();

	panoramaFrameWindowOptimizedCameraFineAdjustment.setFrame(sphericalEnvironmentOptimizedCameraFineAdjustment_.frame());
	Platform::Win::Utilities::textOutput(panoramaFrameWindowOptimizedCameraFineAdjustment.bitmap().dc(), 5, 5, String::toAString(performanceOptimizedCameraFineAdjustment_.lastMseconds()) + std::string("ms, ") + String::toAString(performanceOptimizedCameraFineAdjustment_.averageMseconds()) + std::string("ms"));
	panoramaFrameWindowOptimizedCameraFineAdjustment.repaint();

	repaint();
}

void SEMainWindow::optimizeCameraProfile()
{
	// we need to find a set of camera frames best representing the entire background

	SuccessionSubset<Scalar, 9> subset((SuccessionSubset<Scalar, 9>::Object*)(orientations_.data()), orientations_.size());

	Frames subsetFrames;
	SquareMatrices3 subsetOrientations;
	std::vector<Vectors2> pointCloud;
	Indices32 indices;

	while (subsetFrames.size() < 10)
	{
		const size_t index = subset.incrementSubset();

		if (!orientations_[index].isSingular())
		{
			const FrameRef frame = frameProviderInterface_->synchronFrameRequest((unsigned int)(index), 10);

			if (frame.isNull())
			{
				ocean_assert(false && "This should never happen!");
				return;
			}

			subsetFrames.emplace_back(*frame, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			subsetOrientations.push_back(orientations_[index]);
		}
	}

	Log::info() << "Starting camera optimization:";
	Log::info() << "Start camera:";
	Log::info() << camera_.width() << "x" << camera_.height() << " with " << Numeric::rad2deg(camera_.fovX()) << "deg fov";
	Log::info() << camera_.intrinsic()(0, 0) << ", " << camera_.intrinsic()(0, 1) << ", " << camera_.intrinsic()(0, 2);
	Log::info() << camera_.intrinsic()(1, 0) << ", " << camera_.intrinsic()(1, 1) << ", " << camera_.intrinsic()(1, 2);
	Log::info() << camera_.intrinsic()(2, 0) << ", " << camera_.intrinsic()(2, 1) << ", " << camera_.intrinsic()(2, 2);
	Log::info() << "Radial distortion: " << camera_.radialDistortion().first << ", " << camera_.radialDistortion().second;
	Log::info() << "Tangential distortion: " << camera_.tangentialDistortion().first << ", " << camera_.tangentialDistortion().second;
	Log::info() << " ";

	PinholeCamera optimizedCamera;
	if (Tracking::SphericalEnvironment::optimizeCamera(camera_, subsetFrames, subsetOrientations, optimizedCamera, WorkerPool::get().scopedWorker()()))
	{
		camera_ = optimizedCamera;
	}

	Log::info() << "Final camera:";
	Log::info() << camera_.width() << "x" << camera_.height() << " with " << Numeric::rad2deg(camera_.fovX()) << "deg fov";
	Log::info() << camera_.intrinsic()(0, 0) << ", " << camera_.intrinsic()(0, 1) << ", " << camera_.intrinsic()(0, 2);
	Log::info() << camera_.intrinsic()(1, 0) << ", " << camera_.intrinsic()(1, 1) << ", " << camera_.intrinsic()(1, 2);
	Log::info() << camera_.intrinsic()(2, 0) << ", " << camera_.intrinsic()(2, 1) << ", " << camera_.intrinsic()(2, 2);
	Log::info() << "Radial distortion: " << camera_.radialDistortion().first << ", " << camera_.radialDistortion().second;
	Log::info() << "Tangential distortion: " << camera_.tangentialDistortion().first << ", " << camera_.tangentialDistortion().second;
	Log::info() << " ";
}
