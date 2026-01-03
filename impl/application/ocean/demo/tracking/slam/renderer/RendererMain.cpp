/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/slam/renderer/RendererMain.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/RandomI.h"

#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameShrinker.h"

#include "ocean/devices/Manager.h"

#include "ocean/devices/serialization/SerializerDevicePlayer.h"

#include "ocean/io/File.h"

#include "ocean/media/Manager.h"
#include "ocean/media/MovieRecorder.h"

#include "ocean/rendering/BitmapFramebuffer.h"
#include "ocean/rendering/Manager.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/UndistortedBackground.h"

#include "ocean/scenedescription/Manager.h"
#include "ocean/scenedescription/Scene.h"

#include "ocean/tracking/slam/TrackerMono.h"

#if defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS)
	#include "ocean/media/avfoundation/AVFoundation.h"
	#include "ocean/media/imageio/ImageIO.h"
	#include "ocean/platform/apple/System.h"
	#include "ocean/rendering/glescenegraph/apple/Apple.h"
#endif

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	#include "ocean/media/wic/WIC.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
	#include "ocean/rendering/glescenegraph/windows/Windows.h"
#endif

#include "ocean/scenedescription/sdl/assimp/Assimp.h"
#include "ocean/scenedescription/sdl/obj/OBJ.h"
#include "ocean/scenedescription/sdx/x3d/X3D.h"

using namespace Ocean;
using namespace Ocean::Tracking::SLAM;

/**
 * Loads a scene and adds it to the framebuffer.
 * @param engine The rendering engine to be used, must be valid
 * @param framebuffer The framebuffer to be used, must be valid
 * @param timestamp The timestamp at which the scene is loaded, must be valid
 * @param filename The filename of the scene to be loaded, must be valid
 * @return True, if succeeded
 */
bool loadScene(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer, const Timestamp& timestamp, const std::string& filename);

#if defined(_WINDOWS)
	// main function on Windows platforms
	int wmain(int argc, wchar_t* argv[])
#elif defined(__APPLE__) || defined(__linux__)
	// main function on OSX platforms
	int main(int argc, char* argv[])
#else
	#error Missing implementation.
#endif
{
#ifdef OCEAN_COMPILER_MSC
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	RandomI::initialize();

	CommandArguments commandArguments;
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as input parameter");
	commandArguments.registerParameter("input", "i", "The .osn file to be used as input");
	commandArguments.registerParameter("scene", "s", "The scene file to be loaded (OBJ, X3D, etc.)");
	commandArguments.registerParameter("output", "o", "Explicit movie output file, otherwise a default filename will be used");
	commandArguments.registerParameter("overwrite", "w", "Overwrites the output file if already existing");
	commandArguments.registerParameter("resolution", "r", "The explicit resolution of the output movie, otherwise the resolution of the input is used, e.g., 1280x720");
	commandArguments.registerParameter("downsample", "d", "If specified, the input will be downsampled by a factor of 2 for tracking");
	commandArguments.registerParameter("stopMotionInterval", "smi", "Optional interval in seconds between individual stop-motion frames, otherwise the replay will be in real-time");
	commandArguments.registerParameter("outputFps", "of", "The frames per second of the output video", Value(30.0));
	commandArguments.registerParameter("outputStart", "os", "Explicit start time of the output video in seconds, e.g., 5.0 to skip the first 5 seconds");
	commandArguments.registerParameter("outputDuration", "od", "Explicit duration of the output video in seconds, e.g., 10.0 to create an output video with 10 seconds duration");
	commandArguments.registerParameter("fovx", "f", "Explicit horizontal field of view in degrees, otherwise the rendering engine determines it from the camera");
	commandArguments.registerParameter("help", "h", "Showing this help");

	if (!commandArguments.parse(argv, argc))
	{
		Log::warning() << "Failure when parsing the command arguments";
	}

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		return 1;
	}

	std::string inputValue;
	if (!commandArguments.hasValue("input", inputValue, false, 0u) || inputValue.empty())
	{
		Log::error() << "No input defined";
		return 1;
	}

	const IO::File inputFile(inputValue);

	if (!inputFile.exists() || inputFile.extension() != "osn")
	{
		Log::error() << "The provided input file '" << inputFile() << "' does not exist or does not have the correct extension (.osn)";
		return 1;
	}

	double outputFps = 30.0;
	if (commandArguments.value("outputFps").isFloat64(true))
	{
		outputFps = std::max(1.0, commandArguments.value("outputFps").float64Value(true));
	}

	Scalar explicitFovX = Scalar(-1);
	const double fovX = commandArguments.value<double>("fovx", -1.0, false);

	if (fovX > 0.0 && fovX < 180.0)
	{
		explicitFovX = Numeric::deg2rad(Scalar(fovX));
	}

	const ScopedPlugins scopedPlugins;

	OCEAN_APPLY_IF_APPLE(Platform::Apple::System::runMainLoop(0.0)); // necessary for AVFoundation in CLI apps

	{
		const Rendering::EngineRef engine = Rendering::Manager::get().engine();
		if (engine.isNull())
		{
			Log::error() << "Failed to initialize rendering engine";
			return 1;
		}

		const Rendering::BitmapFramebufferRef framebuffer = engine->createFramebuffer(Rendering::Framebuffer::FRAMEBUFFER_BITMAP);
		if (framebuffer.isNull())
		{
			Log::error() << "Failed to create framebuffer";
			return 1;
		}

		framebuffer->makeCurrent();

		Devices::Serialization::SerializerDevicePlayer devicePlayer;

		if (!devicePlayer.initialize(inputFile()))
		{
			Log::error() << "Failed to initialize device player";
			return 1;
		}

		if (devicePlayer.frameMediums().empty())
		{
			Log::error() << "The device player does not contain any frame mediums";
			return 1;
		}

		Media::FrameMediumRef backgroundFrameMedium = devicePlayer.frameMediums().front();

		unsigned int viewWidth = 0u;
		unsigned int viewHeight = 0u;

		if (commandArguments.value("resolution").isString())
		{
			std::string resolution = commandArguments.value("resolution").stringValue();

			std::string::size_type pos = resolution.find('x');

			if (pos != std::string::npos)
			{
				const std::string widthString = resolution.substr(0, pos);
				const std::string heightString = resolution.substr(pos + 1);

				int width = 0;
				int height = 0;
				if (String::isInteger32(widthString, &width) && String::isInteger32(heightString, &height) && width > 0 && height > 0)
				{
					viewWidth = (unsigned int)(width);
					viewHeight = (unsigned int)(height);
				}
				else
				{
					Log::error() << "Invalid resolution '" << resolution << "'";
					return 1;
				}
			}
		}

		const Rendering::UndistortedBackgroundRef undistortedBackground = engine->factory().createUndistortedBackground();

		undistortedBackground->setMedium(backgroundFrameMedium);
		backgroundFrameMedium->start();

		const Rendering::PerspectiveViewRef view = engine->factory().createPerspectiveView();

		view->setBackgroundColor(RGBAColor(1.0f, 0.0f, 0.0f));
		view->addBackground(undistortedBackground);

		framebuffer->setView(view);

		const bool downsample = commandArguments.hasValue("downsample");
		const double stopMotionInterval = commandArguments.value<double>("stopMotionInterval", 0.0, false);

		if (stopMotionInterval <= 0.0)
		{
			if (!devicePlayer.start(1.0f))
			{
				Log::error() << "Failed to start device player";
				return 1;
			}
		}
		else
		{
			if (!devicePlayer.start(0.0f))
			{
				Log::error() << "Failed to start device player";
				return 1;
			}

			constexpr double stopMotionTolerance = 0.005; // 5ms

			devicePlayer.setStopMotionTolerance(IO::Serialization::DataTimestamp(stopMotionTolerance));
		}

		SensorAccessor sensorAccessor;
		TrackerMono trackerMono;

		Frame yFrame;

		Timestamp firstFrameTimestamp(false);
		Timestamp firstOutputFrameTimestamp(false);
		Timestamp endOutputFrameTimestamp(false);
		Timestamp frameTimestamp(false);
		Timestamp iterationTimestamp(false);

		Media::MovieRecorderRef movieRecorder;

		unsigned int frameCounter = 0u;

		while (devicePlayer.isPlaying())
		{
			SharedAnyCamera camera;
			FrameRef frame;

			if (stopMotionInterval > 0.0)
			{
				frameTimestamp = devicePlayer.playNextFrame();

				if (frameTimestamp.isInvalid())
				{
					// we have reached the end
					break;
				}

				frame = backgroundFrameMedium->frame(frameTimestamp, &camera);
				ocean_assert(frame && frame->timestamp() == frameTimestamp);

				iterationTimestamp.toNow();
			}
			else
			{
				frame = backgroundFrameMedium->frame(&camera);

				if (!frame || frame->timestamp() == frameTimestamp)
				{
					Thread::sleep(1u);
					continue;
				}

				// let's sleep for 5ms to ensure that we have received all necessary device samples
				Thread::sleep(5u);
			}

			if (!camera)
			{
				Log::error() << "No camera for frame medium";
				return 1;
			}

			frameTimestamp = frame->timestamp();

			if (firstFrameTimestamp.isInvalid())
			{
				firstFrameTimestamp = frameTimestamp;

				if (commandArguments.value("outputStart").isFloat64(true))
				{
					firstOutputFrameTimestamp = firstFrameTimestamp + commandArguments.value("outputStart").float64Value(true);
				}
				else
				{
					firstOutputFrameTimestamp = firstFrameTimestamp;
				}

				if (commandArguments.value("outputDuration").isFloat64(true))
				{
					endOutputFrameTimestamp = firstOutputFrameTimestamp + commandArguments.value("outputDuration").float64Value(true) + 1.0 / outputFps;
				}
			}

			ocean_assert(firstOutputFrameTimestamp.isValid());

			// Convert frame to grayscale for tracking
			if (!CV::FrameConverter::Comfort::convert(*frame, FrameType::formatGrayscalePixelFormat(frame->pixelFormat()), FrameType::ORIGIN_UPPER_LEFT, yFrame))
			{
				Log::error() << "Failed to convert frame to grayscale";
				return 1;
			}

			SharedAnyCamera yFrameCamera = camera;

			if (downsample)
			{
				CV::FrameShrinker::downsampleByTwo11(yFrame);
				yFrameCamera = camera->clone(yFrame.width(), yFrame.height());
			}

			const Quaternion device_Q_camera(backgroundFrameMedium->device_T_camera().rotation());

			HomogenousMatrix4 world_T_camera(false);

			const Vector3 cameraGravity = sensorAccessor.cameraGravity(device_Q_camera, frameTimestamp);
			const Quaternion anyWorld_Q_camera = sensorAccessor.anyWorld_Q_camera(device_Q_camera, frameTimestamp);

			const bool handleResult = trackerMono.handleFrame(*yFrameCamera, std::move(yFrame), world_T_camera, cameraGravity, anyWorld_Q_camera, nullptr);
			ocean_assert_and_suppress_unused(handleResult, handleResult);

			if (viewWidth == 0u || viewHeight == 0u)
			{
				viewWidth = frame->width();
				viewHeight = frame->height();
			}

			if (viewWidth == 0u || viewHeight == 0u)
			{
				Log::error() << "Invalid rendering resolution";
				return 1;
			}

			framebuffer->setViewport(0u, 0u, viewWidth, viewHeight);
			view->setAspectRatio(Scalar(viewWidth) / Scalar(viewHeight));

			// Set camera pose from SLAM tracker result
			if (world_T_camera.isValid())
			{
				view->setTransformation(world_T_camera);
			}

			if (movieRecorder.isNull() && frameTimestamp >= firstOutputFrameTimestamp)
			{
				movieRecorder = Media::Manager::get().newRecorder(Media::Recorder::MOVIE_RECORDER);

				if (movieRecorder.isNull())
				{
					Log::error() << "Failed to create movie recorder";
					return 1;
				}

				std::string movieFilename = inputFile.base() + "_rendered.mp4";

				if (commandArguments.value("output").isString())
				{
					movieFilename = commandArguments.value("output").stringValue();
				}

				if (commandArguments.hasValue("overwrite") && IO::File(movieFilename).exists())
				{
					IO::File(movieFilename).remove();
				}

				movieRecorder->setFilename(movieFilename);
				movieRecorder->setFrameFrequency(outputFps);
				movieRecorder->setPreferredFrameType(FrameType(viewWidth, viewHeight, FrameType::FORMAT_RGBA32, FrameType::ORIGIN_LOWER_LEFT));
				movieRecorder->setPreferredBitrate(10u * 1000u * 1000u); // 10 MBit/s

				if (!movieRecorder->start())
				{
					Log::error() << "Failed to start movie recording";
					return 1;
				}
			}

			if (endOutputFrameTimestamp.isValid() && frameTimestamp >= endOutputFrameTimestamp)
			{
				break;
			}

			if (frameCounter == 0u && commandArguments.value("scene").isString())
			{
				// we have loaded the first frame, now we can load the scene file if provided
				loadScene(engine, framebuffer, frameTimestamp, commandArguments.value("scene").stringValue());
			}

			Timestamp timestamp(frameTimestamp);

			timestamp = SceneDescription::Manager::get().preUpdate(view, timestamp);
			SceneDescription::Manager::get().update(view, timestamp);

			engine->update(timestamp);

			if (explicitFovX > Scalar(0))
			{
				view->setFovX(explicitFovX);
			}
			else
			{
				view->updateToIdealFovX();
			}

			Frame renderedFrame;
			if (framebuffer->render(renderedFrame))
			{
				if (movieRecorder)
				{
					Frame recorderFrame;
					if (movieRecorder->lockBufferToFill(recorderFrame, false))
					{
						const bool result = CV::FrameConverter::Comfort::convertAndCopy(renderedFrame, recorderFrame);
						ocean_assert_and_suppress_unused(result, result);

						movieRecorder->unlockBufferToFill();
					}
					else
					{
						Log::error() << "Failed to record rendered frame";
						return 1;
					}
				}
			}
			else
			{
				Log::error() << "Failed to render frame";
				return 1;
			}

			++frameCounter;

			OCEAN_APPLY_IF_APPLE(Platform::Apple::System::runMainLoop(0.0));

			if (iterationTimestamp.isValid())
			{
				ocean_assert(stopMotionInterval > 0.0);
				const double waitTime = stopMotionInterval - double(Timestamp(true) - iterationTimestamp);

				if (waitTime > 0.0)
				{
					Thread::sleep((unsigned int)(double(1000) * waitTime + 0.5));
				}
			}
		}

		if (movieRecorder)
		{
			movieRecorder->stop();
		}

		sensorAccessor.release();

		backgroundFrameMedium.release();

		devicePlayer.release();

		SceneDescription::Manager::get().unloadScenes();

		Log::debug() << "Processed " << trackerMono.frameIndex() + 1u << " frames";
		Log::info() << " ";
		Log::info() << trackerMono.performance();
		Log::info() << " ";
		Log::info() << "Output: " << (commandArguments.value("output").isString() ? commandArguments.value("output").stringValue() : inputFile.base() + "_rendered.mp4");
	}

	Log::info() << "Finished processing recording";

	return 0;
}

ScopedPlugins::ScopedPlugins()
{
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::registerMediaFoundationLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::registerAVFLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());

	Devices::Serialization::registerSerializationLibrary();

	OCEAN_APPLY_IF_APPLE(Rendering::GLESceneGraph::Apple::registerGLESceneGraphEngine());
	OCEAN_APPLY_IF_WINDOWS(Rendering::GLESceneGraph::Windows::registerGLESceneGraphEngine());

	SceneDescription::SDL::OBJ::registerOBJLibrary();
	SceneDescription::SDL::Assimp::registerAssimpLibrary();
	SceneDescription::SDX::X3D::registerX3DLibrary();
}

ScopedPlugins::~ScopedPlugins()
{
	SceneDescription::SDX::X3D::unregisterX3DLibrary();
	SceneDescription::SDL::Assimp::unregisterAssimpLibrary();
	SceneDescription::SDL::OBJ::unregisterOBJLibrary();

	Rendering::GLESceneGraph::unregisterGLESceneGraphEngine();

	Devices::Serialization::unregisterSerializationLibrary();

	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::unregisterAVFLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::unregisterMediaFoundationLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
}

Vector3 SensorAccessor::cameraGravity(const Quaternion& device_Q_camera, const Timestamp& timestamp)
{
	if (!device_Q_camera.isValid())
	{
		return Vector3(0, 0, 0);
	}

	if (gravityTracker_.isNull())
	{
		gravityTracker_ = Devices::Manager::get().device(Devices::GravityTracker3DOF::deviceTypeGravityTracker3DOF());

		if (gravityTracker_)
		{
			gravityTracker_->start();
		}
	}

	if (gravityTracker_)
	{
		const Devices::GravityTracker3DOF::GravityTracker3DOFSampleRef sample = gravityTracker_->sample(timestamp, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

		if (sample)
		{
			ocean_assert(sample->gravities().size() == 1);
			ocean_assert(sample->referenceSystem() == Devices::Tracker::RS_OBJECT_IN_DEVICE);

			constexpr double epsilon = 0.0001; // 0.1 ms

			const double timestampDifference = double(sample->timestamp()) - double(timestamp);

			if (NumericD::isNotEqual(timestampDifference, 0.0, epsilon))
			{
				if (timestampDifference >= 0.0)
				{
					Log::warning() << "Sample timestamp for camera gravity does not fit to request, the sample's timestamp is in the future by " << String::toAString(timestampDifference * 1000.0, 5u) << "ms";
				}
				else
				{
					Log::warning() << "Sample timestamp for camera gravity does not fit to request, the sample's timestamp is in the past by " << String::toAString(-timestampDifference * 1000.0, 5u) << "ms";
				}
			}

			const Vector3& deviceGravity = sample->gravities().front();

			const Quaternion camera_Q_device = device_Q_camera.inverted();

			return camera_Q_device * deviceGravity;
		}
	}

	return Vector3(0, 0, 0);
}

Quaternion SensorAccessor::anyWorld_Q_camera(const Quaternion& device_Q_camera, const Timestamp& timestamp)
{
	if (!device_Q_camera.isValid())
	{
		return Quaternion(false);
	}

	if (orientationTracker_.isNull())
	{
		orientationTracker_ = Devices::Manager::get().device(Devices::OrientationTracker3DOF::deviceTypeOrientationTracker3DOF());

		if (orientationTracker_)
		{
			orientationTracker_->start();
		}
	}

	if (orientationTracker_)
	{
		const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef sample = orientationTracker_->sample(timestamp, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

		if (sample)
		{
			ocean_assert(sample->orientations().size() == 1);
			ocean_assert(sample->referenceSystem() == Devices::Tracker::RS_DEVICE_IN_OBJECT);

			constexpr double epsilon = 0.0001; // 0.1 ms

			const double timestampDifference = double(sample->timestamp()) - double(timestamp);

			if (NumericD::isNotEqual(timestampDifference, 0.0, epsilon))
			{
				if (timestampDifference >= 0.0)
				{
					Log::warning() << "Sample timestamp for anyWorld_Q_camera does not fit to request, the sample's timestamp is in the future by " << String::toAString(timestampDifference * 1000.0, 5u) << "ms";
				}
				else
				{
					Log::warning() << "Sample timestamp for anyWorld_Q_camera does not fit to request, the sample's timestamp is in the past by " << String::toAString(-timestampDifference * 1000.0, 5u) << "ms";
				}
			}

			const Quaternion anyWorld_Q_device = sample->orientations().front();

			return anyWorld_Q_device * device_Q_camera;
		}
	}

	return Quaternion(false);
}

void SensorAccessor::release()
{
	gravityTracker_.release();
	orientationTracker_.release();
}

bool loadScene(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer, const Timestamp& timestamp, const std::string& filename)
{
	ocean_assert(engine && framebuffer);
	ocean_assert(!filename.empty());

	Rendering::SceneRef renderingScene;

	const SceneDescription::SceneRef scene = SceneDescription::Manager::get().load(filename, engine, timestamp, SceneDescription::TYPE_PERMANENT);

	if (scene)
	{
		if (scene->descriptionType() == SceneDescription::TYPE_TRANSIENT)
		{
			const SceneDescription::SDLSceneRef sdlScene(scene);
			ocean_assert(sdlScene);

			renderingScene = sdlScene->apply(engine);
		}
		else
		{
			ocean_assert(scene->descriptionType() == SceneDescription::TYPE_PERMANENT);

			const SceneDescription::SDXSceneRef sdxScene(scene);
			ocean_assert(sdxScene);

			renderingScene = sdxScene->renderingScene();
		}
	}

	if (renderingScene.isNull())
	{
		return false;
	}

	framebuffer->addScene(renderingScene);

	return true;
}
