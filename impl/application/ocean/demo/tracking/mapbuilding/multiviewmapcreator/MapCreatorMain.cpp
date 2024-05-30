/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/mapbuilding/multiviewmapcreator/MapCreatorMain.h"

#include "ocean/base/Build.h"
#include "ocean/base/CommandArguments.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameInterpolator.h"

#include "ocean/io/File.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/Tracker6DOF.h"

#include "ocean/math/RGBAColor.h"

#include "ocean/media/Manager.h"

#include "ocean/tracking/Utilities.h"

#include "ocean/tracking/mapbuilding/MultiViewMapCreator.h"
#include "ocean/tracking/mapbuilding/RelocalizerMono.h"
#include "ocean/tracking/mapbuilding/RelocalizerStereo.h"

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
	commandArguments.registerParameter("input", "i", "The recording file to be used as input to create the map");
	commandArguments.registerParameter("testMono", "tm", "The optional recording file with mono camera to be used as test for the resulting map");
	commandArguments.registerParameter("testStereo", "ts", "The optional recording file with stereo camera to be used as test for the resulting map");
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

	Tracking::MapBuilding::MultiViewMapCreator multiViewMapCreator;

	{
		const IO::File recordingFile(inputValue);

		Devices::SharedDevicePlayer devicePlayer;

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER
		devicePlayer = MapCreatorMain_createExternalDevicePlayer();
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

		if (devicePlayer->frameMediums().empty())
		{
			Log::error() << "Recording files does not contain a frame medium";
			return 1;
		}

		const std::vector<Media::FrameMediumRef> frameMediums = devicePlayer->frameMediums();

		for (const Media::FrameMediumRef& frameMedium : frameMediums)
		{
			frameMedium->start();
		}

		HighPerformanceStatistic performance;

		while (true)
		{
			const Timestamp timestamp = devicePlayer->playNextFrame();

			if (timestamp.isInvalid())
			{
				break;
			}

			SharedAnyCameras cameras;
			Frames yFrames;
			HomogenousMatrices4 device_T_cameras;

			Timestamp frameTimestamp(false);

			for (const Media::FrameMediumRef& frameMedium : frameMediums)
			{
				SharedAnyCamera anyCamera;
				const FrameRef frameRef = frameMedium->frame(timestamp, &anyCamera);

				if (frameRef && anyCamera)
				{
					if (frameTimestamp.isValid() && frameRef->timestamp() != frameTimestamp)
					{
						Log::warning() << "Not consistent timestamp";
						continue;
					}

					Frame yFrame;
					if (!CV::FrameConverter::Comfort::convert(*frameRef, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_ALWAYS_COPY, nullptr, CV::FrameConverter::Options(0.6f)))
					{
						ocean_assert(false && "This should never happen!");
						return 1;
					}

					frameTimestamp = yFrame.timestamp();

					yFrames.emplace_back(std::move(yFrame));
					cameras.emplace_back(std::move(anyCamera));
					device_T_cameras.emplace_back(frameMedium->device_T_camera());
				}
			}

			HomogenousMatrixD4 world_T_device;
			if (devicePlayer->transformation("world_T_device", frameTimestamp, world_T_device) == Devices::DevicePlayer::TR_PRECISE)
			{
				const HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);

				multiViewMapCreator.processFrame(yFrames, cameras, HomogenousMatrix4(world_T_device), device_T_cameras);
			}
			else
			{
				Log::warning() << "Missing device transformation for timestamp " << double(frameTimestamp);
			}

			Vectors3 objectPoints;
			multiViewMapCreator.latestFeatureMap(objectPoints, nullptr, nullptr, 20);

			Log::info() << "Feature map size: " << objectPoints.size();

#ifdef DEBUG_OUTPUT_ON_WINDOWS
#ifdef _WINDOWS
			{
				Frames rgbFrames;
				for (size_t nFrame = 0; nFrame < yFrames.size(); ++nFrame)
				{
					Frame rgbFrame;
					CV::FrameConverter::Comfort::convert(yFrames[nFrame], FrameType::FORMAT_RGB24, rgbFrame);
					rgbFrames.emplace_back(std::move(rgbFrame));
				}

				Vectors3 objectPoints;
				Scalars stabilityFactors;
				multiViewMapCreator.latestFeatureMap(objectPoints, nullptr, &stabilityFactors, 20);
				ocean_assert(objectPoints.size() == stabilityFactors.size());

				for (size_t n = 0; n < objectPoints.size(); ++n)
				{
					const Vector3& objectPoint = objectPoints[n];
					const float stabilityFactor = float(stabilityFactors[n]);

					const RGBAColor color = RGBAColor(0.0f, 1.0f, 0.0f).damped(stabilityFactor).combined(RGBAColor(1.0f, 0.0f, 0.0f).damped(1.0f - stabilityFactor));

					const uint8_t pointColor[3] =
					{
						uint8_t(color.red() * 255.0f),
						uint8_t(color.green() * 255.0f),
						uint8_t(color.blue() * 255.0f)
					};

					for (size_t nFrame = 0; nFrame < yFrames.size(); ++nFrame)
					{
						const HomogenousMatrix4 world_T_camera(world_T_device * device_T_cameras[nFrame]);
						const HomogenousMatrix4 flippedCamera_T_world(PinholeCamera::standard2InvertedFlipped(world_T_camera));

						if (PinholeCamera::isObjectPointInFrontIF(flippedCamera_T_world, objectPoint))
						{
							const Vector2 projectedImagePoint = cameras[nFrame]->projectToImageIF(flippedCamera_T_world, objectPoint);

							if (cameras[nFrame]->isInside(projectedImagePoint))
							{
								CV::Canvas::point<5u>(rgbFrames[nFrame], projectedImagePoint, pointColor);
							}
						}
					}
				}

				for (size_t nFrame = 0; nFrame < yFrames.size(); ++nFrame)
				{
					Platform::Win::Utilities::desktopFrameOutput(int(rgbFrames[nFrame].width() * nFrame), rgbFrames[nFrame].height() * 2 + 10, rgbFrames[nFrame]);
				}
			}
#endif
#endif // DEBUG_OUTPUT_ON_WINDOWS

		}

		Vectors3 objectPoints;
		if (multiViewMapCreator.latestFeatureMap(objectPoints))
		{
			Log::info() << "Finished with a feature map containing " << objectPoints.size() << " feature points";
		}

		Log::info() << "P90 performance: " << performance.percentileMseconds(0.9) << "ms";
	}

	std::string testMonoValue;
	if (commandArguments.hasValue("testMono", testMonoValue) && !testMonoValue.empty())
	{
		RandomGenerator randomGenerator;

		size_t validPoses = 0;
		size_t usedCoorespondences = 0;

		Tracking::MapBuilding::RelocalizerMono relocalizerMono(std::bind(Tracking::MapBuilding::Relocalizer::detectFreakFeatures, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

		Vectors3 objectPoints;
		Scalars objectPointStabilityFactors;
		std::vector<CV::Detector::FREAKDescriptors32> multiDescriptors;

		if (multiViewMapCreator.latestFeatureMap(objectPoints, &multiDescriptors, &objectPointStabilityFactors, 20, 20))
		{
			const Indices32 objectPointIds = createIndices<Index32>(objectPoints.size(), 0u);

			Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap descriptorMap;
			descriptorMap.reserve(objectPoints.size() * 2);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				descriptorMap.emplace(objectPointIds[n], multiDescriptors[n]);
			}

			using ImagePointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptor256;
			using ObjectPointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptors256;
			using ObjectPointVocabularyDescriptor = Tracking::MapBuilding::UnifiedDescriptor::BinaryDescriptor<256u>;

			using UnifiedFeatureMap = Tracking::MapBuilding::UnifiedFeatureMapT<ImagePointDescriptor, ObjectPointDescriptor, ObjectPointVocabularyDescriptor>;

			Tracking::MapBuilding::SharedUnifiedDescriptorMap unifiedDescriptorMap = std::make_shared<Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256>(std::move(descriptorMap));

			if (!relocalizerMono.setFeatureMap(std::make_shared<UnifiedFeatureMap>(Vectors3(objectPoints), Indices32(objectPointIds), std::move(unifiedDescriptorMap), randomGenerator, UnifiedFeatureMap::VocabularyForest::TVocabularyTree::determineClustersMeanForBinaryDescriptor<256u>, Tracking::MapBuilding::UnifiedHelperFreakMultiDescriptor256::extractVocabularyDescriptorsFromMap)))
			{
				Log::error() << "Failed to set feature map";
				return 1;
			}
		}

		Devices::SharedDevicePlayer monoDevicePlayer;

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER
		monoDevicePlayer = MapCreatorMain_createExternalDevicePlayer();
#endif

		if (!monoDevicePlayer)
		{
			Log::error() << "No valid mono device player for the recording '" << testMonoValue << "'";
			return 1;
		}

		if (!monoDevicePlayer->initialize(testMonoValue) || !monoDevicePlayer->start(Devices::DevicePlayer::SPEED_USE_STOP_MOTION))
		{
			Log::error() << "Failed to load input recording file";
			return 1;
		}

		if (monoDevicePlayer->frameMediums().empty())
		{
			Log::error() << "Invalid recording";
			return 1;
		}

		const Media::FrameMediumRef medium = monoDevicePlayer->frameMediums().front();

		const Devices::Tracker6DOFRef slamTracker = Devices::Manager::get().device(Devices::Tracker6DOF::deviceTypeTracker6DOF());

		if (slamTracker)
		{
			slamTracker->start();
		}

		HomogenousMatrix4 slamCamera_T_camera(false);

		while (true)
		{
			const Timestamp timestamp = monoDevicePlayer->playNextFrame();

			if (timestamp.isInvalid())
			{
				break;
			}

			SharedAnyCamera camera;
			FrameRef frame = medium->frame(timestamp, &camera);

			if (frame.isNull() || !camera)
			{
				Log::warning() << "Invalid frame!";
				continue;
			}

			Frame yFrame;
			if (!CV::FrameConverter::Comfort::convert(*frame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_ALWAYS_COPY, nullptr))
			{
				ocean_assert(false && "This should never happen!");
				return 1;
			}

			CV::FrameInterpolator::resize(yFrame, yFrame.width() * 375u / 1000u, yFrame.height() * 375u / 1000u);

			camera = camera->clone(yFrame.width(), yFrame.height());

			HomogenousMatrix4 world_T_roughCamera(false);
			HomogenousMatrix4 world_T_slamCamera(false);

			if (slamTracker)
			{
				const Devices::Tracker6DOF::Tracker6DOFSampleRef sample = slamTracker->sample(timestamp);

				if (sample)
				{
					world_T_slamCamera = HomogenousMatrix4(sample->positions().front(), sample->orientations().front());

					if (slamCamera_T_camera.isValid())
					{
						world_T_roughCamera = world_T_slamCamera * slamCamera_T_camera;
					}
				}
			}

			HomogenousMatrix4 world_T_camera(false);

			constexpr unsigned int minimalNumberCorrespondence = 65u;
			constexpr Scalar maximalProjectionError = Scalar(3.5);
			constexpr Scalar inlierRate = Scalar(0.15);

			Indices32 usedObjectPointIds;
			Vectors2 usedImagePoints;
			if (relocalizerMono.relocalize(*camera, yFrame, world_T_camera, minimalNumberCorrespondence,maximalProjectionError, inlierRate,world_T_roughCamera, WorkerPool::get().scopedWorker()(), &usedObjectPointIds, &usedImagePoints) && usedImagePoints.size() >= 65)
			{
				if (world_T_slamCamera.isValid())
				{
					slamCamera_T_camera = world_T_slamCamera.inverted() * world_T_camera;
				}

				++validPoses;
				usedCoorespondences += usedImagePoints.size();

#ifdef DEBUG_OUTPUT_ON_WINDOWS
#ifdef _WINDOWS
				{
					Frame rgbFrame;
					CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame);

					Tracking::Utilities::paintObjectPoints<3u>(rgbFrame, *camera, world_T_camera, objectPoints.data(), objectPoints.size(), CV::Canvas::red());

					for (size_t n = 0; n < usedObjectPointIds.size(); ++n)
					{
						const Vector3& objectPoint = objectPoints[usedObjectPointIds[n]];
						const Vector2& imagePoint = usedImagePoints[n];

						const Vector2 projectedObjectPoint = camera->projectToImage(world_T_camera, objectPoint);

						CV::Canvas::point<5u>(rgbFrame, projectedObjectPoint, CV::Canvas::blue());
						CV::Canvas::point<3u>(rgbFrame, imagePoint, CV::Canvas::green());
					}

					Platform::Win::Utilities::desktopFrameOutput(0, 0, rgbFrame);
				}
#endif
#endif // DEBUG_OUTPUT_ON_WINDOWS
			}
			else
			{
				slamCamera_T_camera.toNull();
			}
		}

		Log::info() << "Valid poses: " << validPoses;

		if (validPoses != 0u)
		{
			Log::info() << "Used correspondences: " << double(usedCoorespondences) / double(validPoses);
		}
	}

	std::string testStereoValue;
	if (commandArguments.hasValue("testStereo", testStereoValue) && !testStereoValue.empty())
	{
		RandomGenerator randomGenerator;

		size_t validPoses = 0;
		size_t usedCoorespondences = 0;

		Tracking::MapBuilding::RelocalizerStereo relocalizerStereo(std::bind(Tracking::MapBuilding::Relocalizer::detectFreakFeatures, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

		Vectors3 objectPoints;
		Scalars objectPointStabilityFactors;
		std::vector<CV::Detector::FREAKDescriptors32> multiDescriptors;

		if (multiViewMapCreator.latestFeatureMap(objectPoints, &multiDescriptors, &objectPointStabilityFactors, 20, 20))
		{
			const Indices32 objectPointIds = createIndices<Index32>(objectPoints.size(), 0u);

			Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256::DescriptorMap descriptorMap;
			descriptorMap.reserve(objectPoints.size() * 2);

			for (size_t n = 0; n < objectPoints.size(); ++n)
			{
				descriptorMap.emplace(objectPointIds[n], multiDescriptors[n]);
			}

			using ImagePointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptor256;
			using ObjectPointDescriptor = Tracking::MapBuilding::UnifiedDescriptor::FreakMultiDescriptors256;
			using ObjectPointVocabularyDescriptor = Tracking::MapBuilding::UnifiedDescriptor::BinaryDescriptor<256u>;

			using UnifiedFeatureMap = Tracking::MapBuilding::UnifiedFeatureMapT<ImagePointDescriptor, ObjectPointDescriptor, ObjectPointVocabularyDescriptor>;

			Tracking::MapBuilding::SharedUnifiedDescriptorMap unifiedDescriptorMap = std::make_shared<Tracking::MapBuilding::UnifiedDescriptorMapFreakMultiLevelMultiViewDescriptor256>(std::move(descriptorMap));

			relocalizerStereo.setFeatureMap(std::make_shared<UnifiedFeatureMap>(Vectors3(objectPoints), Indices32(objectPointIds), std::move(unifiedDescriptorMap), randomGenerator, UnifiedFeatureMap::VocabularyForest::TVocabularyTree::determineClustersMeanForBinaryDescriptor<256u>, Tracking::MapBuilding::UnifiedHelperFreakMultiDescriptor256::extractVocabularyDescriptorsFromMap));
		}

		Devices::SharedDevicePlayer stereoDevicePlayer;

#ifdef OCEAN_USE_EXTERNAL_DEVICE_PLAYER
		stereoDevicePlayer = MapCreatorMain_createExternalDevicePlayer();
#endif

		if (!stereoDevicePlayer)
		{
			Log::error() << "No valid stereo device player for the recording '" << testMonoValue << "'";
			return 1;
		}

		if (!stereoDevicePlayer->initialize(testStereoValue) || !stereoDevicePlayer->start(Devices::DevicePlayer::SPEED_USE_STOP_MOTION))
		{
			Log::error() << "Failed to load input recording file";
			return 1;
		}

		if (stereoDevicePlayer->frameMediums().empty())
		{
			Log::error() << "Invalid recording";
			return 1;
		}

		const std::vector<Media::FrameMediumRef> frameMediums = stereoDevicePlayer->frameMediums();

		for (const Media::FrameMediumRef& frameMedium : frameMediums)
		{
			frameMedium->start();
		}

		HomogenousMatrix4 slamDevice_T_relocalizedDevice(false);

		while (true)
		{
			const Timestamp timestamp = stereoDevicePlayer->playNextFrame();

			if (timestamp.isInvalid())
			{
				break;
			}

			SharedAnyCameras cameras;
			Frames yFrames;
			HomogenousMatrices4 device_T_cameras;

			Timestamp frameTimestamp(false);

			for (const Media::FrameMediumRef& frameMedium : frameMediums)
			{
				SharedAnyCamera anyCamera;
				const FrameRef frameRef = frameMedium->frame(timestamp, &anyCamera);

				if (frameRef && anyCamera)
				{
					if (frameTimestamp.isValid() && frameRef->timestamp() != frameTimestamp)
					{
						Log::warning() << "Not consistent timestamp";
						continue;
					}

					Frame yFrame;
					if (!CV::FrameConverter::Comfort::convert(*frameRef, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_ALWAYS_COPY, nullptr, CV::FrameConverter::Options(0.6f)))
					{
						ocean_assert(false && "This should never happen!");
						return 1;
					}

					frameTimestamp = yFrame.timestamp();

					yFrames.emplace_back(std::move(yFrame));
					cameras.emplace_back(std::move(anyCamera));
					device_T_cameras.emplace_back(frameMedium->device_T_camera());
				}
			}

			IndexPair32 stereoCameraIndices;
			if (!Tracking::MapBuilding::MultiViewMapCreator::determineLowerStereoCameras(device_T_cameras, stereoCameraIndices))
			{
				Log::error() << "Failed to determine stereo cameras";
				return 1;
			}

			const AnyCamera& cameraA = *cameras[stereoCameraIndices.first];
			const AnyCamera& cameraB = *cameras[stereoCameraIndices.second];

			const HomogenousMatrix4& device_T_cameraA = device_T_cameras[stereoCameraIndices.first];
			const HomogenousMatrix4& device_T_cameraB = device_T_cameras[stereoCameraIndices.second];

			const Frame& yFrameA = yFrames[stereoCameraIndices.first];
			const Frame& yFrameB = yFrames[stereoCameraIndices.second];

			HomogenousMatrix4 world_T_roughDevice(false);

			HomogenousMatrixD4 world_T_slamDevice(false);
			if (stereoDevicePlayer->transformation("world_T_device", frameTimestamp, world_T_slamDevice) == Devices::DevicePlayer::TR_PRECISE)
			{
				if (slamDevice_T_relocalizedDevice.isValid())
				{
					world_T_roughDevice = HomogenousMatrix4(world_T_slamDevice) * slamDevice_T_relocalizedDevice;
				}
			}
			else
			{
				Log::warning() << "Missing device transformation for timestamp " << double(frameTimestamp);
			}

			HomogenousMatrix4 world_T_relocalizedDevice(false);

			constexpr unsigned int minimalNumberCorrespondence = 100u;
			constexpr Scalar maximalProjectionError = Scalar(2.5);
			constexpr Scalar inlierRate = Scalar(0.15);

			Indices32 usedObjectPointIdsA;
			Indices32 usedObjectPointIdsB;
			Vectors2 usedImagePointsA;
			Vectors2 usedImagePointsB;
			if (relocalizerStereo.relocalize(cameraA, cameraB, device_T_cameraA, device_T_cameraB, yFrameA, yFrameB, world_T_relocalizedDevice, minimalNumberCorrespondence, maximalProjectionError, inlierRate, world_T_roughDevice, WorkerPool::get().scopedWorker()(), nullptr, &usedObjectPointIdsA, &usedObjectPointIdsB, &usedImagePointsA, &usedImagePointsB) && usedImagePointsA.size() + usedImagePointsB.size() >= 100)
			{
				if (world_T_slamDevice.isValid())
				{
					slamDevice_T_relocalizedDevice = HomogenousMatrix4(world_T_slamDevice.inverted()) * world_T_relocalizedDevice;
				}

				++validPoses;
				usedCoorespondences += usedImagePointsA.size() + usedImagePointsB.size();

#ifdef DEBUG_OUTPUT_ON_WINDOWS
#ifdef _WINDOWS
				for (unsigned int nImage = 0u; nImage < 2u; ++nImage)
				{
					const Frame& yFrame = nImage == 0u ? yFrameA : yFrameB;
					const AnyCamera& camera = nImage == 0u ? cameraA : cameraB;
					const HomogenousMatrix4& world_T_camera = world_T_relocalizedDevice * (nImage == 0u ? device_T_cameraA :device_T_cameraB);
					const Indices32& usedObjectPointIds = nImage == 0u ? usedObjectPointIdsA : usedObjectPointIdsB;
					const Vectors2& usedImagePoints = nImage == 0u ? usedImagePointsA : usedImagePointsB;

					Frame rgbFrame;
					CV::FrameConverter::Comfort::convert(yFrame, FrameType::FORMAT_RGB24, rgbFrame);

					Tracking::Utilities::paintObjectPoints<3u>(rgbFrame, camera, world_T_camera, objectPoints.data(), objectPoints.size(), CV::Canvas::red());

					for (size_t n = 0; n < usedObjectPointIds.size(); ++n)
					{
						const Vector3& objectPoint = objectPoints[usedObjectPointIds[n]];
						const Vector2& imagePoint = usedImagePoints[n];

						const Vector2 projectedObjectPoint = camera.projectToImage(world_T_camera, objectPoint);

						CV::Canvas::point<5u>(rgbFrame, projectedObjectPoint, CV::Canvas::blue());
						CV::Canvas::point<3u>(rgbFrame, imagePoint, CV::Canvas::green());
					}

					Platform::Win::Utilities::desktopFrameOutput(int(rgbFrame.width()) * int(nImage), 0, rgbFrame);
				}
#endif
#endif // DEBUG_OUTPUT_ON_WINDOWS
			}
			else
			{
				slamDevice_T_relocalizedDevice.toNull();
			}
		}

		Log::info() << "Valid poses: " << validPoses;

		if (validPoses != 0u)
		{
			Log::info() << "Used correspondences: " << double(usedCoorespondences) / double(validPoses);
		}
	}

	return 0;
}

inline ScopedPlugin::ScopedPlugin()
{
#ifdef OCEAN_RUNTIME_STATIC

	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());

#else

	const std::string frameworkPath(Platform::Win::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));

#endif
}

inline ScopedPlugin::~ScopedPlugin()
{
#ifdef OCEAN_RUNTIME_STATIC

	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());

#else

	PluginManager::get().release();

#endif
}
