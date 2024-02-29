// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/PrototypeDevices.h"

#include "ocean/base/CommandArguments.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/devices/Tracker.h"

#include "ocean/devices/mapbuilding/MapBuilding.h"

#include "ocean/devices/pattern/Pattern.h"

#include "ocean/devices/vrs/DevicePlayer.h"
#include "ocean/devices/vrs/DeviceRecorder.h"
#include "ocean/devices/vrs/VRSVisualTracker6DOF.h"

#include "ocean/io/File.h"

#include "ocean/media/ImageSequence.h"
#include "ocean/media/Manager.h"
#include "ocean/media/MovieRecorder.h"

#include "ocean/rendering/BitmapFramebuffer.h"
#include "ocean/rendering/Manager.h"
#include "ocean/rendering/PerspectiveView.h"
#include "ocean/rendering/UndistortedBackground.h"

#include "metaonly/ocean/platform/meta/Login.h"

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	#include "ocean/rendering/glescenegraph/windows/Windows.h"
#elif defined(OCEAN_PLATFORM_BUILD_APPLE)
	#include "ocean/rendering/glescenegraph/apple/Apple.h"
#else
	#error Missing platform
#endif

#include "ocean/scenedescription/Manager.h"
#include "ocean/scenedescription/Scene.h"

#include "ocean/scenedescription/sdl/assimp/Assimp.h"
#include "ocean/scenedescription/sdl/obj/OBJ.h"
#include "ocean/scenedescription/sdx/x3d/X3D.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(__APPLE__)
		#include "ocean/media/imageio/ImageIO.h"
		#include "ocean/media/avfoundation/AVFoundation.h"
	#endif
#endif

using namespace Ocean;

/**
 * Helper class to initialize and release the plugins.
 */
class ScopedPlugins
{
	public:

		/**
		 * Creates a new scoped plugin object and initializes all plugins.
		 */
		ScopedPlugins();

		/**
		 * Destructs the object and releases all plugins.
		 */
		~ScopedPlugins();
};

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
	int wmain(int argc, wchar_t* argv[])
#else
	int main(int argc, char* argv[])
#endif
{
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	CommandArguments commandArguments;
	commandArguments.registerNamelessParameters("Optional the first parameter is interpreted as input parameter");
	commandArguments.registerParameter("input", "i", "The input VRS file to be processed, or an image sequence to be used as background medium");
	commandArguments.registerParameter("scene", "s", "The scene file to be loaded");
	commandArguments.registerParameter("output", "o", "Explicit movie output file, otherwise a default filename will be used");
	commandArguments.registerParameter("overwrite", "w", "Overwrites the output file if already existing");
	commandArguments.registerParameter("resolution", "r", "The explicit resolution of the output movie, otherwise the resolution of the media in the VRS recording is used, e.g., 1280x720, or 1080x1920");
	commandArguments.registerParameter("userId", "u", "The user id to be used for logging into FB services");
	commandArguments.registerParameter("userToken", "t", "The user token to be used for logging into FB services");
	commandArguments.registerParameter("timePerFrame", "tpf", "Explicit wait time per frame to give real-time components (e.g., trackers) enough time for processing, in seconds");
	commandArguments.registerParameter("outputStart", "os", "Explicit start time of the output video in seconds, e.g., 5.0 to skip the first 5 seconds of the input recording");
	commandArguments.registerParameter("outputDuration", "od", "Explicit duration of the output video in seconds, e.g., 10.0 to create an output video with 10 seconds duration");
	commandArguments.registerParameter("outputFps", "of", "The frames per second of the output video", Value(30.0));
	commandArguments.registerParameter("help", "h", "Show this help output");

	commandArguments.parse(argv, argc);

	if (commandArguments.hasValue("help", nullptr, false))
	{
		Log::info() << commandArguments.makeSummary();
		return 0;
	}

	Value inputValue = commandArguments.value("input", false, 0);

	if (!inputValue.isString())
	{
		Log::error() << "Need input file";
		return 1;
	}

	std::string userId;
	std::string userToken;

	const Value userTokenValue = commandArguments.value("userToken");

	if (userTokenValue.isString())
	{
		userToken = userTokenValue.stringValue();
	}

	const Value userIdValue = commandArguments.value("userId");

	if (userIdValue.isFloat64(true) || userIdValue.isString())
	{
		userId = userIdValue.isString() ? userIdValue.stringValue() : String::toAString(int64_t(userIdValue.float64Value(true)));
	}

	Platform::Meta::Login::get().setLogin(Platform::Meta::Login::LT_FACEBOOK, std::move(userId), std::move(userToken));

	double outputFps = 30.0;
	if (commandArguments.value("outputFps").isFloat64(true))
	{
		outputFps = std::max(1.0, commandArguments.value("outputFps").float64Value(true));
	}

	XRPlayground::PrototypeDevices::registerPrototypeDevices();

	const ScopedPlugins scopedPlugins;

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

		Media::FrameMediumRef backgroundFrameMedium;

		Devices::VRS::DevicePlayer devicePlayer;

		const IO::File inputFile(inputValue.stringValue());

		if (inputFile.extension() == "vrs")
		{
			if (!devicePlayer.loadRecording(inputFile()))
			{
				Log::error() << "Could not load input VRS file '" << inputFile() << "'";
				return 1;
			}

			if (devicePlayer.frameMediums().empty())
			{
				Log::error() << "The VRS recording does not contain a media file, we have nothing to create a video from";
				return 1;
			}

			backgroundFrameMedium = devicePlayer.frameMediums().front();
		}
		else
		{
			Media::ImageSequenceRef imageSequence = Media::Manager::get().newMedium(inputFile(), Media::Medium::IMAGE_SEQUENCE);

			if (imageSequence.isNull())
			{
				Log::error() << "The input could not be used as background input medium";
				return 1;
			}

			imageSequence->setMode(Media::ImageSequence::SM_EXPLICIT);

			backgroundFrameMedium = imageSequence;
		}

		ocean_assert(backgroundFrameMedium);

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

		const double secondsPerFrame = commandArguments.value("timePerFrame").isFloat64() ? commandArguments.value("timePerFrame").float64Value() : 0.0;

		if (secondsPerFrame < 0.0)
		{
			Log::error() << "Invalid time per frame";
			return 1;
		}

		devicePlayer.start(0.0f /*speed*/);

		Timestamp firstFrameTimestamp(false);

		Timestamp firstOutputFrameTimestamp(false);
		Timestamp endOutputFrameTimestamp(false);

		Media::MovieRecorderRef movieRecorder;

		unsigned int frameCounter = 0u;

		while (true)
		{
			Timestamp frameTimestamp(false);

			if (devicePlayer.isValid())
			{
				ocean_assert(backgroundFrameMedium->type() != Media::Medium::IMAGE_SEQUENCE);
				frameTimestamp = devicePlayer.playNextFrame();
			}
			else
			{
				ocean_assert(backgroundFrameMedium->type() == Media::Medium::IMAGE_SEQUENCE);
				frameTimestamp = double(frameCounter) / outputFps;
			}

			if (frameTimestamp.isInvalid())
			{
				break;
			}

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

			if (secondsPerFrame > 0.0)
			{
				Thread::sleep((unsigned int)(secondsPerFrame * 1000.0));
			}

			if (viewWidth == 0u || viewHeight == 0u)
			{
				const FrameRef& currentFrame = backgroundFrameMedium->frame();
				ocean_assert(currentFrame);

				viewWidth = currentFrame->width();
				viewHeight = currentFrame->height();
			}

			if (viewWidth == 0u || viewHeight == 0u)
			{
				Log::error() << "Invalid rendering resolution";
				return 1;
			}

			framebuffer->setViewport(0u, 0u, viewWidth, viewHeight);
			view->setAspectRatio(Scalar(viewWidth) / Scalar(viewHeight));

			if (movieRecorder.isNull() && frameTimestamp >= firstOutputFrameTimestamp)
			{
				movieRecorder = Media::Manager::get().newRecorder(Media::Recorder::MOVIE_RECORDER);

				if (movieRecorder.isNull())
				{
					Log::error() << "Failed to create movie recorder";
					return 1;
				}

				std::string movieFilename = IO::File(inputValue.stringValue()).base() + "_output.mp4";

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

			view->updateToIdealFovX();

			Frame frame;
			if (framebuffer->render(frame))
			{
				if (movieRecorder)
				{
					Frame recorderFrame;
					if (movieRecorder->lockBufferToFill(recorderFrame, false))
					{
						const bool result = CV::FrameConverter::Comfort::convertAndCopy(frame, recorderFrame);
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

			if (Media::ImageSequenceRef imageSequence = backgroundFrameMedium)
			{
				ocean_assert(!devicePlayer.isValid());

				if (!imageSequence->forceNextFrame())
				{
					break;
				}
			}
		}

		if (movieRecorder)
		{
			movieRecorder->stop();
		}

		devicePlayer.stop();

		SceneDescription::Manager::get().unloadScenes();

		Log::info() << "Stopped processing the recording after " << frameCounter << " frames";
	}

	Log::info() << "Finished processing VRS recording";

	return 0;
}

ScopedPlugins::ScopedPlugins()
{
#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::registerAVFLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());

	Devices::VRS::registerVRSLibrary();
	Devices::Pattern::registerPatternLibrary();
	Devices::MapBuilding::registerMapBuildingLibrary();

	OCEAN_APPLY_IF_APPLE(Rendering::GLESceneGraph::Apple::registerGLESceneGraphEngine());
	OCEAN_APPLY_IF_WINDOWS(Rendering::GLESceneGraph::Windows::registerGLESceneGraphEngine());

	SceneDescription::SDL::OBJ::registerOBJLibrary();
	SceneDescription::SDL::Assimp::registerAssimpLibrary();
	SceneDescription::SDX::X3D::registerX3DLibrary();
#endif
}

ScopedPlugins::~ScopedPlugins()
{
#ifdef OCEAN_RUNTIME_STATIC
	SceneDescription::SDX::X3D::unregisterX3DLibrary();
	SceneDescription::SDL::Assimp::unregisterAssimpLibrary();
	SceneDescription::SDL::OBJ::unregisterOBJLibrary();

	Rendering::GLESceneGraph::unregisterGLESceneGraphEngine();

	Devices::MapBuilding::unregisterMapBuildingLibrary();
	Devices::Pattern::unregisterPatternLibrary();
	Devices::VRS::unregisterVRSLibrary();

	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::unregisterAVFLibrary());
#endif
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
