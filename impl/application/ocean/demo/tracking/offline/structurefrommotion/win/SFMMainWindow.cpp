/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/offline/structurefrommotion/win/SFMMainWindow.h"
#include "application/ocean/demo/tracking/offline/structurefrommotion/win/SFMMaintenance.h"

#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/Maintenance.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"

#include "ocean/io/Bitstream.h"
#include "ocean/io/File.h"
#include "ocean/io/Utilities.h"

#include "ocean/math/Random.h"

#include "ocean/media/ImageSequence.h"
#include "ocean/media/ImageSequenceFrameProviderInterface.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Movie.h"
#include "ocean/media/MovieFrameProvider.h"
#include "ocean/media/MovieFrameProviderInterface.h"

#include "ocean/tracking/Solver3.h"
#include "ocean/tracking/Utilities.h"

#include "ocean/tracking/offline/PointPaths.h"
#include "ocean/tracking/offline/SLAMTracker.h"

#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/Utilities.h"

using namespace Ocean;

SFMMainWindow::SFMMainWindow(HINSTANCE instance, const std::wstring& name, const CommandArguments& commandArguments) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	commandArguments_(commandArguments),
	applicationState_(AS_IDLE),
	frameIndex_(0u),
	frameNumber_(0u),
	lowerPoseBorder_((unsigned int)(-1)),
	upperPoseBorder_((unsigned int)(-1)),
	frameSlowDownFactor_(0u),
	world_T_plane_(false),
	planeSize_(0),
	planeMoveObjectPoint_(Numeric::minValue(), Numeric::minValue(), Numeric::minValue())
{
	mediaFile_ = commandArguments_.value("input", false, 0).stringValue();
}

SFMMainWindow::~SFMMainWindow()
{
	// nothing to do here
}

void SFMMainWindow::onInitialized()
{
	if (!mediaFile_.empty())
	{
		const Media::MovieRef movie = Media::Manager::get().newMedium(mediaFile_, Media::Medium::MOVIE, true);

		if (movie)
		{
			const Media::MovieFrameProviderRef frameProvider(new Media::MovieFrameProvider(true, 1000, 200));
			frameProvider->setMovie(movie);

			frameProviderInterface_ = CV::FrameProviderInterfaceRef(new Media::MovieFrameProviderInterface(frameProvider));
		}
		else
		{
			const Media::ImageSequenceRef imageSequence = Media::Manager::get().newMedium(mediaFile_, Media::Medium::IMAGE_SEQUENCE, true);

			if (imageSequence)
			{
				frameProviderInterface_ = CV::FrameProviderInterfaceRef(new Media::ImageSequenceFrameProviderInterface(imageSequence));
			}
		}
	}

	if (!commandArguments_.hasValue("noDatabaseInput"))
	{
		const std::string dataFilename = IO::File(mediaFile_).base() + std::string(".database");
		if (readDatabase(dataFilename, camera_, database_) && database_)
		{
			if (commandArguments_.hasValue("skipProcessing"))
			{
				applicationState_ = AS_PAINT_FINAL_OBJECT_POINTS;
			}
			else
			{
				applicationState_ = AS_PAINT_FEATURE_PATHS;

				// resetting the relevant data to restart SLAM from scratch

				camera_ = PinholeCamera();

				database_.setObjectPoints<false>();
				database_.setPoses<false>(HomogenousMatrix4(false));
			}
		}
	}
}

void SFMMainWindow::onIdle()
{
	if (frameProviderInterface_.isNull())
	{
		Platform::Win::Utilities::textOutput(dc(), 5, 5, "Failed to open " + mediaFile_);

		Thread::sleep(1);
		return;
	}

	if (!frameProviderInterface_->isInitialized())
	{
		Platform::Win::Utilities::textOutput(dc(), 5, 5, "Initializing video ...");

		Thread::sleep(1);
		return;
	}

	if (frameNumber_ == 0u)
	{
		ocean_assert(frameIndex_ == 0u && frameNumber_ == 0u);

		frameProviderInterface_->setPreferredFrameType(FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT);
		frameNumber_ = frameProviderInterface_->synchronFrameNumberRequest();

		Log::info() << "Total frame number: " << frameNumber_ << " [0, " << frameNumber_ - 1u << "]";

		if (frameNumber_ != 0u)
		{
			lowerPoseBorder_ = 0u;
			upperPoseBorder_ = frameNumber_ - 1u;
			frameIndex_ = 0u;
		}
		else
		{
			lowerPoseBorder_ = (unsigned int)(-1);
			upperPoseBorder_ = (unsigned int)(-1);
		}
	}

	if (database_.isEmpty<false>())
	{
		const FrameType frameType = frameProviderInterface_->synchronFrameTypeRequest();
		ocean_assert(frameType.isValid());

		camera_ = PinholeCamera(frameType.width(), frameType.height(), Numeric::deg2rad(60));

		Platform::Win::Utilities::textOutput(dc(), 5, 5, "Determining point paths...");
		repaint();

		const Tracking::Offline::PointPaths::TrackingConfiguration trackingConfiguration(Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_15, binSize, binSize, strength, coarsestLayerRadius, pyramidLayers);
		Tracking::Offline::PointPaths::determinePointPaths(*frameProviderInterface_, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, trackingConfiguration, 0u, 0u, frameNumber_ - 1u, 20u, true, database_, WorkerPool::get().scopedWorker()());

		frameIndex_ = 0u;
		applicationState_ = AS_PAINT_FEATURE_PATHS;
	}

	if (!camera_.isValid())
	{
		const FrameType frameType = frameProviderInterface_->synchronFrameTypeRequest();
		ocean_assert(frameType.isValid());

		camera_ = PinholeCamera(frameType.width(), frameType.height(), Numeric::deg2rad(60));
	}

	switch (applicationState_)
	{
		case AS_PAINT_FEATURE_PATHS:
		{
			if (!paintFrame("Feature paths", false))
			{
				applicationState_ = ApplicationState(applicationState_ + 1);
			}

			break;
		}

		case AS_DETERMINE_INITIAL_OBJECT_POINTS:
		{
			if (!Tracking::Offline::SLAMTracker::determineInitialObjectPoints(camera_, database_, randomGenerator_, 0u, nullptr, frameNumber_ - 1u, CV::SubRegion(), false, &lowerPoseBorder_, &upperPoseBorder_))
			{
				applicationState_ = AS_BREAK;
				return;
			}

			maintenanceSendEnvironment();

			PinholeCamera optimizedCamera;
			Tracking::Database optimizedDatabase;
			if (Tracking::Offline::SLAMTracker::optimizeCamera(camera_, database_, 0u, frameNumber_ - 1u, true, PinholeCamera::OS_INTRINSIC_PARAMETERS_DISTORTIONS, 25u, optimizedCamera, optimizedDatabase))
			{
				camera_ = optimizedCamera;
				database_ = std::move(optimizedDatabase);
			}

			maintenanceSendEnvironment();

			frameIndex_ = lowerPoseBorder_;
			applicationState_ = ApplicationState(applicationState_ + 1);

			break;
		}

		case AS_PAINT_INITIAL_OBJECT_POINTS:
		{
			if (!paintFrame("Initial object points", true))
			{
				applicationState_ = ApplicationState(applicationState_ + 1);
			}

			break;
		}

		case AS_EXTEND_INITIAL_OBJECT_POINTS:
		{
			if (!Tracking::Offline::SLAMTracker::extendInitialObjectPoints(camera_, database_, 0u, frameNumber_ - 1u, Tracking::Solver3::RelativeThreshold(10u, Scalar(0.3), 25u), &lowerPoseBorder_, &upperPoseBorder_))
			{
				applicationState_ = AS_BREAK;
				return;
			}

			maintenanceSendEnvironment();

			determineCameraMotion(0u, frameNumber_ - 1u);

			frameIndex_ = lowerPoseBorder_;
			applicationState_ = ApplicationState(applicationState_ + 1);

			break;
		}

		case AS_PAINT_EXTENDED_INITIAL_OBJECT_POINTS:
		{
			if (!paintFrame("Extended initial object points", true))
			{
				applicationState_ = ApplicationState(applicationState_ + 1);
			}

			break;
		}

		case AS_EXTEND_FINAL_INITIAL_OBJECT_POINTS:
		{
			if (!Tracking::Offline::SLAMTracker::extendStableObjectPoints(camera_, database_, randomGenerator_, 0u, frameNumber_ - 1u, Tracking::Solver3::CM_UNKNOWN, Tracking::Solver3::RelativeThreshold(10u, Scalar(0.3), 25u), &lowerPoseBorder_, &upperPoseBorder_))
			{
				applicationState_ = AS_BREAK;
				return;
			}

			if (!commandArguments_.hasValue("noDatabaseOutput"))
			{
				const std::string dataFilename = IO::File(mediaFile_).base() + std::string(".database");
				writeDatabase(camera_, database_, dataFilename);
			}

			maintenanceSendEnvironment();

			frameIndex_ = lowerPoseBorder_;
			applicationState_ = ApplicationState(applicationState_ + 1);

			break;
		}

		case AS_PAINT_FINAL_OBJECT_POINTS:
		{
			paintFrame("Final object points", true, true);

			const bool exitWhenDone = commandArguments_.hasValue("exitWhenDone");

			if (exitWhenDone && frameIndex_ == upperPoseBorder_)
			{
				close();
			}

			break;
		}

		case AS_BREAK:
		{
			Platform::Win::Utilities::textOutput(dc(), 5, 5, "FAILED!");

			const bool exitWhenDone = commandArguments_.hasValue("exitWhenDone");

			if (exitWhenDone)
			{
				close();
			}
			break;
		}

		case AS_DEFINE_PLANE:
		{
			paintFrame("Defining pose", true, true);
			break;
		}

		default:
		{
			Thread::sleep(1);
			frameIndex_ = 0u;

			break;
		}
	}
}

void SFMMainWindow::onKeyDown(const int key)
{
	if (key == 'S')
	{
		frameSlowDownFactor_ = modulo<unsigned int>(frameSlowDownFactor_ + 1u, 4u);
	}
	else if (key == 'A')
	{
		frameSlowDownFactor_ = modulo<int>(frameSlowDownFactor_ - 1, 4);
	}

	if (applicationState_ == AS_PAINT_FINAL_OBJECT_POINTS)
	{
		if (key == 'P')
		{
			applicationState_ = AS_DEFINE_PLANE;
		}
	}
	else if (applicationState_ == AS_DEFINE_PLANE)
	{
		if (key == 'F')
		{
			applicationState_ = AS_PAINT_FINAL_OBJECT_POINTS;
		}
		else if (key == 'M')
		{
			if (world_T_plane_.isValid())
			{
				// we create a new database with camera poses in relation to the plane

				const HomogenousMatrix4 plane_T_world(world_T_plane_.inverted());

				Tracking::Database newDatabase;

				for (size_t poseIndex = 0; poseIndex < database_.poseNumber<false>(); ++poseIndex)
				{
					HomogenousMatrix4 world_T_camera(false);
					if (database_.hasPose<false>(Index32(poseIndex), &world_T_camera))
					{
						newDatabase.addPose<false>(Index32(poseIndex), plane_T_world * world_T_camera);
					}
				}

				const std::string dataFilename = IO::File(mediaFile_).base() + std::string(".database_with_plane");
				writeDatabase(camera_, newDatabase, dataFilename);
			}
		}
		else
		{
			std::string keyDescription;
			if (Platform::Win::Keyboard::translateVirtualkey(key, keyDescription))
			{
				if (keyDescription == "left")
				{
					if (frameIndex_ > lowerPoseBorder_)
					{
						--frameIndex_;
					}
				}
				else if (keyDescription == "right")
				{
					if (frameIndex_ < upperPoseBorder_)
					{
						++frameIndex_;
					}
				}
			}
		}
	}
}

void SFMMainWindow::onMouseDown(const MouseButton button, const int x, const int y)
{
	if (applicationState_ == AS_DEFINE_PLANE)
	{
		const FrameType frameType = frameProviderInterface_->synchronFrameTypeRequest(0.1);

		if (button == BUTTON_LEFT)
		{
			if (frameType.isValid())
			{
				maskFrame_.set(FrameType(frameType, FrameType::FORMAT_Y8), true /*forceOwner*/, true /*foreWritable*/);
				maskFrame_.setValue(0xFF);
			}

			world_T_plane_.toNull();
		}

		if (button == BUTTON_RIGHT && world_T_plane_.isValid())
		{
			int frameX, frameY;
			Vector3 objectPoint;

			if (window2bitmap(x, y, frameX, frameY) && determinePointOnPlane(Plane3(world_T_plane_), Vector2(Scalar(frameX), Scalar(frameY)), objectPoint))
			{
				planeMoveObjectPoint_ = objectPoint;
			}
		}
	}
}

void SFMMainWindow::onMouseMove(const MouseButton buttons, const int x, const int y)
{
	if (applicationState_ == AS_DEFINE_PLANE)
	{
		if ((buttons & BUTTON_LEFT) == BUTTON_LEFT)
		{
			ocean_assert(maskFrame_.isValid());

			int bitmapX;
			int bitmapY;
			if (window2bitmap(x, y, bitmapX, bitmapY))
			{
				const CV::PixelPosition maskCenter((unsigned int)(bitmapX), (unsigned int)(bitmapY));

				const unsigned int ellipseRadius = (std::max(maskFrame_.width(), maskFrame_.height()) * 8u / 100u) | 1u;

				if (maskCenter.x() < maskFrame_.width() && maskCenter.y() < maskFrame_.height())
				{
					CV::Canvas::ellipse(maskFrame_, maskCenter, ellipseRadius, ellipseRadius, CV::Canvas::black(maskFrame_.pixelFormat()));
				}
			}
		}
		else if ((buttons & BUTTON_RIGHT) == BUTTON_RIGHT && world_T_plane_.isValid() && planeMoveObjectPoint_.x() != Numeric::minValue())
		{
			int frameX, frameY;
			Vector3 objectPoint;

			if (window2bitmap(x, y, frameX, frameY) && determinePointOnPlane(Plane3(world_T_plane_), Vector2(Scalar(frameX), Scalar(frameY)), objectPoint))
			{
				const Vector3 offset(objectPoint - planeMoveObjectPoint_);

				world_T_plane_.setTranslation(world_T_plane_.translation() + offset);

				planeMoveObjectPoint_ = objectPoint;
			}
		}
	}
}

void SFMMainWindow::onMouseUp(const MouseButton button, const int /*x*/, const int /*y*/)
{
	if (applicationState_ == AS_DEFINE_PLANE)
	{
		if (button == BUTTON_LEFT)
		{
			if (maskFrame_.isValid())
			{
				Plane3 plane;

				const CV::SubRegion subRegion(std::move(maskFrame_));

				if (!Tracking::Solver3::determinePlane(database_, camera_, lowerPoseBorder_, frameIndex_, upperPoseBorder_, subRegion, randomGenerator_, plane, camera_.hasDistortionParameters()))
				{
					Log::error() << "Plane could not be determined";
					return;
				}

				Vector3 objectPoint;
				if (!determinePointOnPlane(plane, subRegion.boundingBox().center(), objectPoint))
				{
					Log::error() << "No valid intersection point";
					return;
				}

				if (!plane.transformation(objectPoint, Vector3(0, 1, 0), world_T_plane_))
				{
					Log::error() << "Failed to determine plane transformation";
					world_T_plane_.toNull();
					return;
				}

				planeSize_ = Scalar(0.5);
			}
		}
		else if (button == BUTTON_RIGHT)
		{
			planeMoveObjectPoint_ = Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());
		}
	}
}

void SFMMainWindow::onMouseWheel(const MouseButton /*buttons*/, const int wheel, const int /*x*/, const int /*y*/)
{
	if (applicationState_ == AS_DEFINE_PLANE)
	{
		if (Platform::Win::Keyboard::currentKeyState() & Platform::Keyboard::KEY_CONTROL)
		{
			if (planeSize_ > 0)
			{
				planeSize_ = std::max(Scalar(0.001), planeSize_ + Scalar(wheel) * Scalar(0.0001));
			}
		}
		else
		{
			if (world_T_plane_.isValid())
			{
				world_T_plane_ *= Quaternion(Vector3(0, 0, 1), Numeric::deg2rad(Scalar(wheel) * Scalar(0.01)));
			}
		}
	}
}

bool SFMMainWindow::paintFrame(const std::string& text, const bool showCorrespondences, const bool showImagePoints)
{
	ocean_assert(lowerPoseBorder_ != (unsigned int)(-1) && upperPoseBorder_ != (unsigned int)(-1));
	ocean_assert(lowerPoseBorder_ <= frameIndex_ && frameIndex_ <= upperPoseBorder_);

	HighPerformanceTimer paintTimer;

	const FrameRef frameRef = frameProviderInterface_->synchronFrameRequest(frameIndex_);
	if (frameRef.isNull())
	{
		ocean_assert(false && "This should never happen!");
		frameIndex_ = lowerPoseBorder_;
		return false;
	}

	Frame frame(*frameRef, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	if (showCorrespondences)
	{
		const HomogenousMatrix4& pose = database_.pose<false>(frameIndex_);

		if (pose.isValid())
		{
			if (Maintenance::get().isActive())
			{
				Maintenance::get().send("EXTRINSIC_CAMERA_MATRIX_16", IO::Utilities::encodeHomogenousMatrix4(pose));
			}

			const uint8_t* red = CV::Canvas::red(frame.pixelFormat());
			const uint8_t* green = CV::Canvas::green(frame.pixelFormat());
			const uint8_t* blue = CV::Canvas::blue(frame.pixelFormat());

			Vectors2 imagePoints;
			Vectors3 objectPoints;
			Indices32 objectPointIds;
			database_.imagePointsObjectPoints<false, false>(frameIndex_, imagePoints, objectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()), 0, nullptr, &objectPointIds);

			if (showImagePoints)
			{
				Tracking::Utilities::paintCorrespondences<7u, 3u>(frame, AnyCameraPinhole(camera_), pose, objectPoints.data(), imagePoints.data(), objectPoints.size(), Scalar(3.5 * 3.5), green, blue, red, blue, true, true, true, WorkerPool::get().scopedWorker()());
			}
			else
			{
				Tracking::Utilities::paintCorrespondences<5u, 3u>(frame, AnyCameraPinhole(camera_), pose, objectPoints.data(), imagePoints.data(), objectPoints.size(), Scalar(3.5 * 3.5), green, blue, red, blue, true, false, false, WorkerPool::get().scopedWorker()());
			}
		}
	}
	else
	{
		const Tracking::Database::IdIdPointPairsMap featurePointsMap = database_.imagePoints<false>(frameIndex_, true, 1, 20);

		ImagePointGroups imagePointGroups;
		imagePointGroups.reserve(featurePointsMap.size());

		for (Tracking::Database::IdIdPointPairsMap::const_iterator i = featurePointsMap.begin(); i != featurePointsMap.end(); ++i)
		{
			Vectors2 imagePoints;
			imagePoints.reserve(i->second.size());

			for (Tracking::Database::IdPointPairs::const_iterator iI = i->second.begin(); iI != i->second.end(); ++iI)
			{
				imagePoints.push_back(iI->second);
			}

			imagePointGroups.push_back(std::move(imagePoints));
		}

		Tracking::Utilities::paintPaths<1u>(frame, imagePointGroups.data(), imagePointGroups.size(), CV::Canvas::green(frame.pixelFormat()), WorkerPool::get().scopedWorker()());
	}

	if (applicationState_ == AS_DEFINE_PLANE && maskFrame_.isValid())
	{
		frame = Tracking::Utilities::paintMask(frame, maskFrame_, 0x00, WorkerPool::get().scopedWorker()());
	}

	if (world_T_plane_.isValid())
	{
		HomogenousMatrix4 world_T_camera(false);
		if (database_.hasPose<false>(frameIndex_, &world_T_camera))
		{
			Tracking::Utilities::paintPlaneIF(frame, AnyCamera::standard2InvertedFlipped(world_T_camera), AnyCameraPinhole(camera_), world_T_plane_, planeSize_, 20u, CV::Canvas::white(frame.pixelFormat()), CV::Canvas::black(frame.pixelFormat()));
		}
	}

	setFrame(frame);

	if (!text.empty())
	{
		Platform::Win::Utilities::textOutput(bitmap_.dc(), 5, 5, text + std::string(", Index ") + String::toAString(frameIndex_) + std::string(" with ") + String::toAString(database_.numberCorrespondences<false, false, true>(frameIndex_, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()))) + std::string (" points"));
	}

	repaint();

	if (applicationState_ != AS_DEFINE_PLANE)
	{
		if (++frameIndex_ > upperPoseBorder_)
		{
			frameIndex_ = lowerPoseBorder_;
			return false;
		}

		if (frameSlowDownFactor_ == 3u)
		{
			Thread::sleep((unsigned int)max(0.0, 2000.0 - paintTimer.mseconds()));
		}
		else if (frameSlowDownFactor_ == 2u)
		{
			Thread::sleep((unsigned int)max(0.0, 1000.0 - paintTimer.mseconds()));
		}
		else if (frameSlowDownFactor_ == 1u)
		{
			Thread::sleep((unsigned int)max(0.0, 500.0 - paintTimer.mseconds()));
		}
		else
		{
			Thread::sleep((unsigned int)max(0.0, 33.0 - paintTimer.mseconds()));
		}
	}

	return true;
}

bool SFMMainWindow::determineCameraMotion(const unsigned int lowerFrame, const unsigned int upperFrame)
{
	Index32 poseId;
	if (!database_.poseWithMostCorrespondences<false, false, true>(lowerFrame, upperFrame, &poseId, nullptr, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue())))
	{
		return false;
	}

	unsigned int lowerValidPose;
	unsigned int upperValidPose;
	if (!database_.validPoseRange<false>(lowerFrame, poseId, upperFrame, lowerValidPose, upperValidPose))
	{
		return false;
	}

	const Tracking::Solver3::CameraMotion cameraMotion = Tracking::Solver3::determineCameraMotion(database_, camera_, lowerValidPose, upperValidPose, true, WorkerPool::get().scopedWorker()());

	Log::info() << " ";
	Log::info() << "Detected camera motion:";

	Log::info() << Tracking::Solver3::translateCameraMotion(cameraMotion);

	Log::info() << " ";

	return true;
}

void SFMMainWindow::maintenanceSendEnvironment()
{
	if (Maintenance::get().isActive())
	{
		Index32 poseId;
		if (!database_.poseWithMostCorrespondences<false, false, true>(0u, frameNumber_ - 1u, &poseId, nullptr, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue())))
		{
			return;
		}

		const FrameRef frame = frameProviderInterface_->synchronFrameRequest(poseId);

		Vectors2 frameImagePoints;
		Vectors3 frameObjectPoints;
		database_.imagePointsObjectPoints<false, false>(poseId, frameImagePoints, frameObjectPoints, Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

		const Vectors3 objectPoints = database_.objectPoints<false, false>(Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue()));

		const HomogenousMatrix4& pose = database_.pose<false>(poseId);

		Maintenance::get().send("ENVIRONMENT", SFMMaintenance::encodeEnvironment(frame ? *frame : Frame(), frameImagePoints, frameObjectPoints, pose, objectPoints));
	}
}

bool SFMMainWindow::determinePointOnPlane(const Plane3& plane, const Vector2& imagePoint, Vector3& objectPoint)
{
	ocean_assert(plane.isValid());

	HomogenousMatrix4 world_T_camera(false);
	if (!database_.hasPose<false>(frameIndex_, &world_T_camera))
	{
		return false;
	}

	const Line3 ray = camera_.ray(imagePoint, world_T_camera);

	if (!plane.intersection(ray, objectPoint))
	{
		return false;
	}

	return true;
}

bool SFMMainWindow::writeDatabase(const PinholeCamera& camera, const Tracking::Database& database, const std::string& filename)
{
	std::ofstream stream;
	stream.open(filename, std::ios::binary);

	IO::OutputBitstream outputStream(stream);

	if (!Tracking::Utilities::writeCamera(camera, outputStream))
	{
		return false;
	}

	if (!Tracking::Utilities::writeDatabase(database, outputStream))
	{
		return false;
	}

	return true;
}

bool SFMMainWindow::readDatabase(const std::string& filename, PinholeCamera& camera, Tracking::Database& database)
{
	std::ifstream stream;
	stream.open(filename, std::ios::binary);

	IO::InputBitstream inputStream(stream);

	if (!Tracking::Utilities::readCamera(inputStream, camera))
	{
		return false;
	}

	if (!Tracking::Utilities::readDatabase(inputStream, database))
	{
		return false;
	}

	return true;
}
