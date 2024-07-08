/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/maptexturing/texturegenerator/TextureGeneratorMain.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/ScopedValue.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/devices/Manager.h"

#include "ocean/media/Manager.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Manager.h"

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	#include "ocean/rendering/glescenegraph/windows/Windows.h"
#elif defined(OCEAN_PLATFORM_BUILD_APPLE)
	#include "ocean/rendering/glescenegraph/apple/Apple.h"
#else
	#error Missing platform!
#endif

#include "ocean/tracking/maptexturing/TextureGenerator.h"

#if defined(_WINDOWS)
	#include "ocean/platform/win/System.h"
	#include "ocean/platform/win/Utilities.h"
#endif

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/wic/WIC.h"
	#elif defined(__APPLE__)
		#include "ocean/media/imageio/ImageIO.h"
	#else
		#include "ocean/media/openimagelibraries/OpenImageLibraries.h"
	#endif
#endif

using namespace Ocean;

/**
 * Just a helper class to ensure that all media plugins are unregistered when this object is disposed.
 */
class ScopedPlugin
{
	public:

		/**
		 * Creates a new object and registers all plugins.
		 */
		inline ScopedPlugin();

		/**
		 * Destructs this object and unregisters all plugins.
		 */
		inline ~ScopedPlugin();
};

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

	const ScopedPlugin scopedPlugin;

	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	RandomI::initialize();

	CommandArguments commandArguments;
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as input parameter");
	commandArguments.registerParameter("input", "i", "The recording file to be used as input");
	commandArguments.registerParameter("intermediateOutput", "io", "The number of frames after which an intermediate output will be created", Value(0));
	commandArguments.registerParameter("lastMesh", "lm", "Using only the last mesh instead of the iteratively increasing mesh");
	commandArguments.registerParameter("mode", "m", "Defines which mode is used, options are 'quality' 'speed'", Value("quality"));
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

	Value inputValue;
	if (!commandArguments.hasValue("input", &inputValue, false, 0u) || !inputValue.isString())
	{
		Log::error() << "No input defined";
		return 1;
	}

	const IO::File recordingFile(inputValue.stringValue());

	Devices::SceneTracker6DOF::SharedSceneElements lastSceneElements;

	if (commandArguments.hasValue("lastMesh"))
	{
		Devices::SharedDevicePlayer devicePlayer;

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER
		devicePlayer = TextureGeneratorMain_createExternalDevicePlayer();
#endif

		if (!devicePlayer)
		{
			Log::error() << "No valid device player for the recording '" << recordingFile.name() << "'";
			return 1;
		}

		if (!devicePlayer->initialize(recordingFile()) || !devicePlayer->start(Devices::DevicePlayer::SPEED_USE_STOP_MOTION))
		{
			Log::error() << "Failed to load input recording file";
			return 1;
		}

		const Devices::SceneTracker6DOFRef sceneTracker = Devices::Manager::get().device(Devices::SceneTracker6DOF::deviceTypeSceneTracker6DOF());

		if (!sceneTracker)
		{
			Log::error() << "The recording files does not contain a scene tracker";
			return 1;
		}

		sceneTracker->start();

		while (true)
		{
			const Timestamp timestamp = devicePlayer->playNextFrame();

			if (timestamp.isInvalid())
			{
				break;
			}

			const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sample = sceneTracker->sample();

			if (sample)
			{
				lastSceneElements = sample->sceneElements();
			}
		}
	}

	Devices::SharedDevicePlayer devicePlayer;

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER
		devicePlayer = TextureGeneratorMain_createExternalDevicePlayer();
#endif

	if (!devicePlayer->initialize(recordingFile()) || !devicePlayer->start(Devices::DevicePlayer::SPEED_USE_STOP_MOTION))
	{
		Log::error() << "Failed to load input recording file";
		return 1;
	}

	const IO::Directory outputDirectory(IO::Directory(recordingFile) + IO::Directory("output"));
	if (!outputDirectory.exists())
	{
		outputDirectory.create();
	}

	const IO::File outputFile(outputDirectory + IO::File(recordingFile.baseName() + ".x3dv"));

	unsigned int intermediateOutput = 0u;

	Value intermediateOutputValue;
	if (commandArguments.hasValue("intermediateOutput", &intermediateOutputValue) && intermediateOutputValue.isInt())
	{
		if (intermediateOutputValue.intValue() > 0)
		{
			intermediateOutput = (unsigned int)(intermediateOutputValue.intValue());
		}
	}

	if (devicePlayer->frameMediums().empty())
	{
		Log::error() << "The recording file does not contain a frame medium";
		return 1;
	}

	const Media::FrameMediumRef frameMedium = devicePlayer->frameMediums().front();
	ocean_assert(frameMedium);

	frameMedium->start();

	const Devices::SceneTracker6DOFRef sceneTracker = Devices::Manager::get().device(Devices::SceneTracker6DOF::deviceTypeSceneTracker6DOF());

	if (!sceneTracker)
	{
		Log::error() << "The recording file does not contain a scene tracker";
		return 1;
	}

	sceneTracker->start();

	Frame rgbFrame;

	Tracking::MapTexturing::TextureGenerator::TexturingMode texturingMode = Tracking::MapTexturing::TextureGenerator::TM_KEEP_RETIRED;

	Value modeValue;
	if (commandArguments.hasValue("mode", &modeValue) && modeValue.isString())
	{
		if (modeValue.stringValue() == "quality")
		{
			texturingMode = Tracking::MapTexturing::TextureGenerator::TM_KEEP_RETIRED;
		}
		else if (modeValue.stringValue() == "speed")
		{
			texturingMode = Tracking::MapTexturing::TextureGenerator::TM_REDRAW_RETIRED;
		}
		else
		{
			Log::warning() << "Unknown mode '" << modeValue.stringValue() << "'";
		}
	}

	Tracking::MapTexturing::TextureGenerator textureGenerator(texturingMode);

	const Rendering::EngineRef engine = Rendering::Manager::get().engine();
	if (!engine)
	{
		Log::error() << "Failed to create rendering engine";
		return 1;
	}

	HighPerformanceStatistic performance;

	unsigned int frameIndex = 0u;

	while (true)
	{
		const Timestamp timestamp = devicePlayer->playNextFrame();

		if (timestamp.isInvalid())
		{
			break;
		}

		const ScopedValueT<Index32> scopedFrameIndex(frameIndex, frameIndex + 1u);

		SharedAnyCamera anyCamera;
		const FrameRef frameRef = frameMedium->frame(timestamp, &anyCamera);

		if (!frameRef || !anyCamera)
		{
			Log::error() << "Failed to access frame";
			return 1;
		}

		const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sample = sceneTracker->sample(frameRef->timestamp());

		if (!sample || sample->timestamp() != frameRef->timestamp())
		{
			Log::warning() << "Missing scene sample for timestamp " << String::toAString(double(frameRef->timestamp()), 4u);
			continue;
		}

		if (!CV::FrameConverter::Comfort::convert(*frameRef, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return 1;
		}

		const HomogenousMatrix4 world_T_device(sample->positions().front(), sample->orientations().front());

		const Devices::SceneTracker6DOF::SharedSceneElements& sceneElements = lastSceneElements.empty() ? sample->sceneElements() : lastSceneElements;

		for (const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement : sceneElements)
		{
			if (!sceneElement)
			{
				continue;
			}

			if (sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_MESHES)
			{
				const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

				textureGenerator.updateMesh(sceneElement, true);
				textureGenerator.processFrame(std::move(rgbFrame), *anyCamera, world_T_device, engine);

				break;
			}
		}

		if (intermediateOutput > 0u && frameIndex % intermediateOutput == 0u)
		{
			if (!textureGenerator.exportMeshs(outputFile.base() + "_intermediate_" + String::toAString(frameIndex, 5u) + ".x3dv", true))
			{
				Log::error() << "Failed to export intermediate mesh";
			}
		}
	}

	Log::info() << "Average performance: " << performance.averageMseconds() << "ms";

	if (!textureGenerator.exportMeshs(outputFile.base() + "_final.x3dv", true))
	{
		Log::error() << "Failed to export final mesh";
	}

	return 0;
}

inline ScopedPlugin::ScopedPlugin()
{
#ifdef OCEAN_RUNTIME_STATIC

	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());

	OCEAN_APPLY_IF_WINDOWS(Rendering::GLESceneGraph::Windows::registerGLESceneGraphEngine());
	OCEAN_APPLY_IF_APPLE(Rendering::GLESceneGraph::Apple::registerGLESceneGraphEngine());

#else

	const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));

#endif
}

inline ScopedPlugin::~ScopedPlugin()
{
#ifdef OCEAN_RUNTIME_STATIC

	Rendering::GLESceneGraph::unregisterGLESceneGraphEngine();

	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());

#else

	PluginManager::get().release();

#endif
}
