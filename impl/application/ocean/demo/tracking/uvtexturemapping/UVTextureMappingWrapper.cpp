/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/uvtexturemapping/UVTextureMappingWrapper.h"

#include "ocean/base/Build.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/Processor.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/devices/Manager.h"

#include "ocean/io/CameraCalibrationManager.h"
#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/media/FiniteMedium.h"
#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#include "ocean/platform/System.h"
#include "ocean/platform/Utilities.h"

#include "ocean/tracking/Utilities.h"

#include "ocean/tracking/blob/BlobTracker6DOF.h"

#include "ocean/tracking/pattern/PatternTracker6DOF.h"

#include "ocean/tracking/orb/FeatureTracker6DOF.h"

#include "ocean/tracking/uvtexturemapping/ConeUVTextureMapping.h"
#include "ocean/tracking/uvtexturemapping/CylinderUVTextureMapping.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/directshow/DirectShow.h"
		#include "ocean/media/mediafoundation/MediaFoundation.h"
		#include "ocean/media/wic/WIC.h"
	#elif defined(__APPLE__)
		#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
			#include "ocean/devices/ios/IOS.h"
		#endif

		#include "ocean/media/avfoundation/AVFoundation.h"
		#include "ocean/media/imageio/ImageIO.h"
	#elif defined(_ANDROID)
		#include "ocean/devices/android/Android.h"
		#include "ocean/media/openimagelibraries/OpenImageLibraries.h"
	#endif
#endif

#include <array>

namespace
{

// Load a fixed number of parameters (e.g., for a cylinder or a cone pattern) either from a file or simply as a space-delimited string.
// @param parameterString Either a filename or a space-delimited string of parameters
// @parameters Fixed-size array of shape parameters that will be filled based on the input
// @return False if not all parameters could be loaded, true otherwise
template <size_t numParameters>
bool loadParametersFromStringOrFile(std::string parameterString, std::array<Scalar, numParameters>& parameters)
{
	const IO::File file(parameterString);

	if (file.exists())
	{
		std::ifstream input(file());
		if (input)
		{
			parameterString.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
		}
	}

	std::istringstream iss(parameterString);

	for (Scalar& parameter : parameters)
	{
		if (!(iss >> parameter))
		{
			Platform::Utilities::showMessageBox(
				"Error",
				"Not all parameters were specified, or the input file could not be read.\nGot parameters:\n\"" + parameterString + "\"");
			return false;
		}
	}

	return true;
}

} // namespace

UVTextureMappingWrapper::UVTextureMappingWrapper()
{
	// nothing to do here
}

UVTextureMappingWrapper::UVTextureMappingWrapper(const std::vector<std::wstring>& commandArguments)
{
#if 0
	// disable multi-core computation by forcing one CPU core
	Processor::get().forceCores(1);
#endif

	// first, we register or load the media plugin(s)
	// if we have a shared runtime we simply load all media plugins available in a specific directory
	// if we have a static runtime we explicitly need to register all plugins we want to use (at compile time)

	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		Media::DirectShow::registerDirectShowLibrary();
		Media::MediaFoundation::registerMediaFoundationLibrary();
		Media::WIC::registerWICLibrary();
	#elif defined(__APPLE__)
		Media::AVFoundation::registerAVFLibrary();
		Media::ImageIO::registerImageIOLibrary();

		#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
			Devices::IOS::registerIOSLibrary();
		#endif
	#elif defined(_ANDROID)
		Media::OpenImageLibraries::registerOpenImageLibrariesLibrary();
		Devices::Android::registerAndroidLibrary();
	#endif
#else

	// we collect all plugins located in the resource path of the application

	#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
		PluginManager::get().collectPlugins(StringOSX::toUTF8([[NSBundle mainBundle] resourcePath]));
	#else
		PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	#endif

	// Although we could use the tracking capabilities via the devices interface we invoke the trackers directly to simplify the application
	// thus, we use the media plugins only
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
#endif


	// first, we get access to the frame medium that is intended to be used for the tracking

	if (commandArguments.size() >= 1)
	{
		const std::string argument(String::toAString(commandArguments[0]));

		if (!argument.empty())
		{
			trackerFrameMedium = Media::Manager::get().newMedium(argument);

			// if we have a finite medium (e.g., a movie) we loop it

			const Media::FiniteMediumRef finiteMedium(trackerFrameMedium);
			if (finiteMedium)
				finiteMedium->setLoop(true);
		}
	}

	if (trackerFrameMedium.isNull())
	{
		// if the user did not specify a medium, first we try to get a live video with id 1 (often an external web cam - not the builtin camera of a laptop)
		trackerFrameMedium = Media::Manager::get().newMedium("LiveVideoId:1");
	}

	if (trackerFrameMedium.isNull())
	{
		// if we could not get the device with id 1 we try to get the device with id 0
		trackerFrameMedium = Media::Manager::get().newMedium("LiveVideoId:0");
	}

	if (trackerFrameMedium.isNull())
	{
		Platform::Utilities::showMessageBox("Error", "No valid input medium could be found!");

		// the device does not have an accessible live camera (or a necessary media plugin hasn't loaded successfully)
		return;
	}


	// second, we load the pattern that is intended to be used for the feature map (to be used as tracking reference)

	IO::File patternAbsoluteFile;

	if (commandArguments.size() >= 2 && !commandArguments[1].empty())
	{
		const IO::File file(String::toAString(commandArguments[1]));

		if (file.exists())
			patternAbsoluteFile = file;
		else
			Platform::Utilities::showMessageBox("Error", std::string("Could not find a valid tracking pattern!\n\nGot \"") + file() + std::string("\""));
	}
	else
	{
		const IO::File relativeFile("res/application/ocean/demo/tracking/uvtexturemapping/sift640x512.bmp");

		patternAbsoluteFile = IO::Directory(frameworkPath) + relativeFile;
	}

	if (!patternAbsoluteFile.exists())
	{
		Log::warning() << "The pattern file \"" << patternAbsoluteFile() << "\" does not exist.";

		// we do not have a valid tracking pattern, so we stop here
		return;
	}

	const Frame patternFrame = Media::Utilities::loadImage(patternAbsoluteFile());

	if (!patternFrame.isValid())
	{
		Log::warning() << "The pattern file \"" << patternAbsoluteFile() << "\" could not be loaded.";

		// we do not have a valid tracking pattern, so we stop here
		return;
	}


	// third, we check whether a desired frame dimension is specified for the input frame medium

	if (commandArguments.size() >= 3)
	{
		const std::string dimension = String::toAString(commandArguments[2]);

		if (dimension == "320x240")
		{
			trackerFrameMedium->setPreferredFrameDimension(320u, 240u);
		}
		else if (dimension == "640x480")
		{
			trackerFrameMedium->setPreferredFrameDimension(640u, 480u);
		}
		else if (dimension == "1280x720")
		{
			trackerFrameMedium->setPreferredFrameDimension(1280u, 720u);
		}
		else if (dimension == "1920x1080")
		{
			trackerFrameMedium->setPreferredFrameDimension(1920u, 1080u);
		}
	}


	// fourth, we check whether a specific tracker type is specified, if not we simply take one of the existing ones

	std::string trackerName;

	if (commandArguments.size() >= 4)
	{
		trackerName = String::toAString(commandArguments[3]);
	}

	const Scalar patternWidth = Scalar(1.0); // we simply define the width of the pattern only

	ocean_assert(patternFrame.width() != 0u);
	const Vector2 patternDimension = Vector2(patternWidth, patternWidth * Scalar(patternFrame.height()) / Scalar(patternFrame.width()));

	if (trackerName == std::string("Blob Feature Based 6DOF Tracker"))
	{
		// we want to track a simple pattern image
		visualTracker = Tracking::VisualTrackerRef(new Tracking::Blob::BlobTracker6DOF());
		visualTracker.force<Tracking::Blob::BlobTracker6DOF>().setFeatureMap(Tracking::Blob::FeatureMap(patternFrame, patternDimension, Scalar(6), true, 0, WorkerPool::get().scopedWorker()()));

		trackerObjectDimension = Box3(Vector3(0, 0, 0), Vector3(patternDimension.x(), patternDimension.length() * Scalar(0.2), patternDimension.y()));
	}

	if (visualTracker.isNull() && trackerName == std::string("Blob Feature Based 6DOF Tracker for cubes"))
	{
		// we want to track a textured cube
		if (patternFrame.width() % 3u == 0u && patternFrame.height() % 4u == 0u && patternFrame.width() * 4u == patternFrame.height() * 3u)
		{
			Frame cubeFrameY;
			if (CV::FrameConverter::Comfort::convert(patternFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, cubeFrameY, CV::FrameConverter::CP_AVOID_COPY_IF_POSSIBLE, WorkerPool::get().scopedWorker()()))
			{
				CV::Detector::Blob::BlobFeatures cubeMapFeatures;
				if (Tracking::Blob::FeatureMap::createCubeFeatureMap(cubeFrameY.constdata<uint8_t>(), cubeFrameY.width(), cubeFrameY.height(), cubeFrameY.paddingElements(), Scalar(1), cubeMapFeatures, Scalar(15), 0u, WorkerPool::get().scopedWorker()()))
				{
					visualTracker = Tracking::VisualTrackerRef(new Tracking::Blob::BlobTracker6DOF());
					visualTracker.force<Tracking::Blob::BlobTracker6DOF>().setFeatureMap(Tracking::Blob::FeatureMap(cubeMapFeatures));

					trackerObjectDimension = Box3(Vector3(-1, -1, -1), Vector3(1, 1, 1)) * Scalar(0.5);
				}
			}
		}
		else
		{
			Platform::Utilities::showMessageBox("Error", "The provided cube map cannot be interpreted.");
			return;
		}
	}

	if (visualTracker.isNull() && (
		trackerName == std::string("Blob Feature Based 6DOF Tracker for cylinders") ||
		trackerName == std::string("Pattern 6DOF Tracker for cylinders")))
	{
		if (commandArguments.size() < 6)
		{
			Platform::Utilities::showMessageBox("Error", "Cylinder parameters must be specified in a file or as a string.");
			return;
		}

		Frame yFrame;
		if (CV::FrameConverter::Comfort::convert(patternFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, false, WorkerPool::get().scopedWorker()()))
		{
			// Cylinder parameters: [0] patternCropWidth, [1] cylinderMetricHeight, [2] xAxisOffset,
			// [3] yAxisIntersection.x(), [4] yAxisIntersection.y()
			std::array<Scalar, 5u> parameters;
			if (!loadParametersFromStringOrFile(String::toAString(commandArguments[5]), parameters))
			{
				return;
			}

			// Create the cylinder UV mapping.
			Tracking::UVTextureMapping::CylinderUVTextureMapping cylinderUVTextureMapping(parameters[0], Scalar(yFrame.height()), parameters[1], parameters[2], Vector2(parameters[3], parameters[4]), true);

			if (cylinderUVTextureMapping.isValid())
			{
				// In both cases, we'll create a feature map to compute the object bounding box and obtain
				// the canonical cylinder shape with a coordinate frame. The feature map is only used
				// further for Blob tracking.
				constexpr unsigned int kMaxFeatures = 0u;
				constexpr Scalar kDetectionThreshold = Scalar(15);
				Tracking::Blob::FeatureMap featureMap(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), cylinderUVTextureMapping, kDetectionThreshold, kMaxFeatures, WorkerPool::get().scopedWorker()());

				trackerObjectDimension = featureMap.boundingBox();
				trackerObjectCylinder = featureMap.cylinder();

				if (trackerName == std::string("Blob Feature Based 6DOF Tracker for cylinders"))
				{
					visualTracker = Tracking::VisualTrackerRef(new Tracking::Blob::BlobTracker6DOF());
					visualTracker.force<Tracking::Blob::BlobTracker6DOF>().setFeatureMap(featureMap);
				}
				else // pattern-based tracking
				{
					visualTracker = Tracking::VisualTrackerRef(new Tracking::Pattern::PatternTracker6DOF());
					visualTracker.force<Tracking::Pattern::PatternTracker6DOF>().addCylinderPattern(patternFrame, cylinderUVTextureMapping, WorkerPool::get().scopedWorker()());
				}
			}
			else
			{
				Platform::Utilities::showMessageBox("Error", "The provided cylinder map cannot be interpreted.");
				return;
			}
		}
		else
		{
			Platform::Utilities::showMessageBox("Error", "The provided cylinder map cannot be interpreted.");
			return;
		}
	}

	if (visualTracker.isNull() && (
		trackerName == std::string("Blob Feature Based 6DOF Tracker for cones") ||
		trackerName == std::string("Pattern 6DOF Tracker for cones")))
	{
		if (commandArguments.size() < 6)
		{
			Platform::Utilities::showMessageBox("Error", "Cone parameters must be specified in a file or as a string.");
			return;
		}

		// We want to track a (possibly truncated) cone.
		Frame yFrame;
		if (CV::FrameConverter::Comfort::convert(patternFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, false, WorkerPool::get().scopedWorker()()))
		{
			// Cone parameters: [0] coneHeight, [1] largerDiameter, [2] smallerDiameter,
			// [3] largerArcEnd.x(), [4] largerArcEnd.y(), [5] smallerArcEnd.x(), [6] smallerArcEnd.y(),
			// [7] yAxisIntersection.x(), [8] yAxisIntersection.y(), [9] originOnSurface as 0 or 1
			std::array<Scalar, 10u> parameters;
			if (!loadParametersFromStringOrFile<10u>(String::toAString(commandArguments[5]), parameters))
			{
				return;
			}

			Tracking::UVTextureMapping::ConeUVTextureMapping coneUVTextureMapping(
				parameters[0],
				parameters[1],
				parameters[2],
				Vector2(parameters[3], parameters[4]),
				Vector2(parameters[5], parameters[6]),
				Vector2(parameters[7], parameters[8]),
				static_cast<bool>(parameters[9]));

			if (coneUVTextureMapping.isValid())
			{
				// In both cases, we'll create a feature map to compute the object bounding box and obtain
				// the canonical cone shape with a coordinate frame. The feature map is only used further
				// for Blob tracking.
				constexpr unsigned int kMaxFeatures = 0u;
				constexpr Scalar kDetectionThreshold = Scalar(15);
				Tracking::Blob::FeatureMap featureMap(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), coneUVTextureMapping, kDetectionThreshold, kMaxFeatures, WorkerPool::get().scopedWorker()());

				trackerObjectDimension = featureMap.boundingBox();
				trackerObjectCone = featureMap.cone();

				if (trackerName == std::string("Blob Feature Based 6DOF Tracker for cones"))
				{
					visualTracker = Tracking::VisualTrackerRef(new Tracking::Blob::BlobTracker6DOF());
					visualTracker.force<Tracking::Blob::BlobTracker6DOF>().setFeatureMap(featureMap);
				}
				else // pattern-based tracking
				{
					visualTracker = Tracking::VisualTrackerRef(new Tracking::Pattern::PatternTracker6DOF());
					visualTracker.force<Tracking::Pattern::PatternTracker6DOF>().addConePattern(patternFrame, coneUVTextureMapping, WorkerPool::get().scopedWorker()());
				}
			}
			else
			{
				Platform::Utilities::showMessageBox("Error", "The provided cone map cannot be interpreted.");
				return;
			}
		}
		else
		{
			Platform::Utilities::showMessageBox("Error", "The provided cone map cannot be interpreted.");
			return;
		}
	}

	if (visualTracker.isNull() && trackerName == std::string("Blob Feature Based 6DOF Tracker for meshes"))
	{
		Frame yFrame;
		if (CV::FrameConverter::Comfort::convert(patternFrame, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT, yFrame, false, WorkerPool::get().scopedWorker()()))
		{
			if (commandArguments.size() < 6)
			{
				Platform::Utilities::showMessageBox("Error", "Mesh-based tracking requires an OBJ file.");
				return;
			}

			std::ifstream meshFile(String::toAString(commandArguments[5]));

			Vectors3 vertices;
			Vectors2 textureCoordinates;
			Tracking::UVTextureMapping::MeshUVTextureMapping::TriangleFaces vertexTriangles;
			Tracking::UVTextureMapping::MeshUVTextureMapping::TriangleFaces textureTriangles;

			while (meshFile)
			{
				std::string type;
				meshFile >> type;
				if (type == "vt")
				{
					Scalar u, v;
					meshFile >> u >> v;
					textureCoordinates.emplace_back(u * patternFrame.width(), (Scalar(1.) - v) * patternFrame.height());
				}
				else if (type == "v")
				{
					Scalar x, y, z;
					meshFile >> x >> y >> z;
					vertices.emplace_back(x, y, z);
				}
				else if (type == "f")
				{
					// Face entries have three vertices, each with the format "<vertex index>/<texture index>[/<normal index>]".
					Tracking::UVTextureMapping::MeshUVTextureMapping::TriangleFace vertexTriangle;
					Tracking::UVTextureMapping::MeshUVTextureMapping::TriangleFace textureTriangle;
					for (size_t i = 0u; i < 3u; ++i) {
						std::string tmp;
						meshFile >> tmp;

						const size_t offset = tmp.find("/");
						int index = 0;
						bool valid = String::isInteger32(tmp.substr(0u, offset), &index);
						if (!valid || index <= 0)
						{
							Platform::Utilities::showMessageBox("Error", "Invalid OBJ vertex index at face " + String::toAString(vertexTriangles.size()) + ", vertex " + String::toAString(i));
							return;
						}
						vertexTriangle[i] = index - 1;

						// Ignore the part of the entry containing the vertex normal, if present.
						const size_t offset2 = tmp.rfind("/");
						index = 0;
						valid = String::isInteger32(tmp.substr(offset + 1u, (offset2 > offset) ? offset2 : tmp.size()), &index);
						if (!valid || index <= 0)
						{
							Platform::Utilities::showMessageBox("Error", "Invalid OBJ texture coordinate index at face " + String::toAString(vertexTriangles.size()) + ", vertex " + String::toAString(i));
							return;
						}
						textureTriangle[i] = index - 1;
					}

					vertexTriangles.push_back(vertexTriangle);
					textureTriangles.push_back(textureTriangle);
				}
				else
				{
					std::string tmp;
					meshFile >> tmp;
				}
			}

			Tracking::UVTextureMapping::MeshUVTextureMappingRef meshUVTextureMapping(
				new Tracking::UVTextureMapping::MeshUVTextureMapping(vertices, textureCoordinates, vertexTriangles, textureTriangles));

			if (meshUVTextureMapping->isValid())
			{
				constexpr unsigned int kMaxFeatures = 0u;
				constexpr Scalar kDetectionThreshold = Scalar(15);
				Tracking::Blob::FeatureMap featureMap(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), meshUVTextureMapping, kDetectionThreshold, kMaxFeatures, WorkerPool::get().scopedWorker()());

				visualTracker = Tracking::VisualTrackerRef(new Tracking::Blob::BlobTracker6DOF());
				visualTracker.force<Tracking::Blob::BlobTracker6DOF>().setFeatureMap(featureMap);

				trackerObjectDimension = featureMap.boundingBox();
				trackerObjectTriangles = meshUVTextureMapping->triangles3();
			}
			else
			{
				Platform::Utilities::showMessageBox("Error", "The provided mesh cannot be interpreted.");
				return;
			}
		}
		else
		{
			Platform::Utilities::showMessageBox("Error", "The provided image for the mesh cannot be loaded.");
			return;
		}
	}

	if (visualTracker.isNull() && trackerName == std::string("ORB Feature Based 6DOF Tracker"))
	{
		visualTracker = Tracking::VisualTrackerRef(new Tracking::ORB::FeatureTracker6DOF());
		visualTracker.force<Tracking::ORB::FeatureTracker6DOF>().setFeatureMap(Tracking::ORB::FeatureMap(patternFrame, patternDimension, Scalar(6), true, 0u, false, WorkerPool::get().scopedWorker()()));

		trackerObjectDimension = Box3(Vector3(0, 0, 0), Vector3(patternDimension.x(), patternDimension.length() * Scalar(0.2), patternDimension.y()));
	}

	if (visualTracker.isNull() || trackerName == std::string("Pattern 6DOF Tracker"))
	{
		visualTracker = Tracking::VisualTrackerRef(new Tracking::Pattern::PatternTracker6DOF());
		visualTracker.force<Tracking::Pattern::PatternTracker6DOF>().addPattern(patternFrame, patternDimension, WorkerPool::get().scopedWorker()());

		trackerObjectDimension = Box3(Vector3(0, 0, 0), Vector3(patternDimension.x(), patternDimension.length() * Scalar(0.2), patternDimension.y()));
	}

	if (visualTracker.isNull())
	{
		Platform::Utilities::showMessageBox("Error", std::string("No valid tracker found!\n\nGot \"") + trackerName + std::string("\""));

		// we could not create a valid tracker
		return;
	}


	// finally, we check whether an explicit camera calibration file has been provided

	IO::File cameraCalibrationFile;

	if (commandArguments.size() >= 5)
	{
		const IO::File file(String::toAString(commandArguments[4]));

		if (file.exists())
			cameraCalibrationFile = file;
	}

	if (cameraCalibrationFile.isNull())
	{
		const IO::File relativeFile("res/application/ocean/demo/tracking/uvtexturemapping/cameracalibration.occ");

		cameraCalibrationFile = IO::Directory(frameworkPath) + relativeFile;
	}

	if (cameraCalibrationFile.exists())
		IO::CameraCalibrationManager::get().registerCalibrationFile(cameraCalibrationFile());

	// we start the medium so that medium will deliver frames

	trackerFrameMedium->start();

	orientationTracker3DOF_ = Devices::Manager::get().device(Devices::OrientationTracker3DOF::deviceTypeOrientationTracker3DOF());

	if (orientationTracker3DOF_)
	{
		orientationTracker3DOF_->start();
	}
}

UVTextureMappingWrapper::~UVTextureMappingWrapper()
{
	// we do not release the tracker, this should be done by the user before the application ends
}

void UVTextureMappingWrapper::release()
{
	orientationTracker3DOF_.release();
	trackerFrameMedium.release();
	visualTracker.release();

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		Media::DirectShow::unregisterDirectShowLibrary();
		Media::MediaFoundation::unregisterMediaFoundationLibrary();
		Media::WIC::unregisterWICLibrary();
	#elif defined(__APPLE__)
		#if defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE == 1
			Devices::IOS::unregisterIOSLibrary();
		#endif

		Media::AVFoundation::unregisterAVFLibrary();
		Media::ImageIO::unregisterImageIOLibrary();
	#elif defined(_ANDROID)
		Devices::Android::unregisterAndroidLibrary();
		Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
	#endif
#else
	PluginManager::get().release();
#endif
}

bool UVTextureMappingWrapper::trackNewFrame(Frame& frame, double& time)
{
	if (visualTracker.isNull() || trackerFrameMedium.isNull())
	{
		return false;
	}

	if (!trackerCamera.isValid())
	{
		// we still need to request the correct camera profile for our input medium
		// therefore, we need to know the dimensions of the input medium (the delivered frames respectively)

		const FrameRef liveFrame = trackerFrameMedium->frame();

		if (liveFrame.isNull())
		{
			// if we cannot extract the first frame within 5 seconds since we started the medium, something must be wrong

			if (trackerFrameMedium->startTimestamp() + 5.0 < Timestamp(true))
			{
				Platform::Utilities::showMessageBox("Error", "Could not extract a valid frame from the input source!\nDefine a different source as input.");

				// we release the medium to ensure that we stop immediately the next time this function is called
				trackerFrameMedium.release();
			}

			return false;
		}

		// the camera calibration manager will either provided the calibrated profile (if existing) or will provide a default profile

		trackerCamera = IO::CameraCalibrationManager::get().camera(trackerFrameMedium->url(), liveFrame->width(), liveFrame->height(), nullptr, Numeric::deg2rad(60));
	}

	ocean_assert(trackerCamera.isValid());

	// we request the most recent frame from our input/tracking medium

	const FrameRef liveFrame = trackerFrameMedium->frame();

	if (liveFrame.isNull())
	{
		return false;
	}

	// we only handle a frame once

	if (liveFrame->timestamp() == trackerFrameTimestamp)
	{
		return false;
	}

	trackerFrameTimestamp = liveFrame->timestamp();

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(*liveFrame, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	if (trackerPerformance.measurements() % 20u == 0u)
	{
		trackerPerformance.reset();
	}

	Quaternion world_Q_camera(false);
	if (orientationTracker3DOF_)
	{
		const Devices::OrientationTracker3DOF::OrientationTracker3DOFSampleRef sample = orientationTracker3DOF_->sample(trackerFrameTimestamp, Devices::Measurement::IS_TIMESTAMP_INTERPOLATE);

		if (sample && sample->orientations().size() == 1)
		{
			const Quaternion& world_Q_device = sample->orientations().front();

			world_Q_camera = world_Q_device * Quaternion(trackerFrameMedium->device_T_camera().rotation());
		}
	}

	trackerPerformance.start();

	Tracking::VisualTracker::TransformationSamples resultingTransformationSamples;
	if (visualTracker->determinePoses(*liveFrame, trackerCamera, false, resultingTransformationSamples, world_Q_camera, WorkerPool::get().scopedWorker()()) && !resultingTransformationSamples.empty())
	{
		trackerPerformance.stop();

		// the resulting pose transforms points defined in the coordinate system of the camera to points defined in the coordinate system of the world (the pattern)

		ocean_assert(!resultingTransformationSamples.empty());
		const HomogenousMatrix4& resultingPose = resultingTransformationSamples.front().transformation();

		const HomogenousMatrix4 resultingPoseIF(PinholeCamera::standard2InvertedFlipped(resultingPose));

		Tracking::Utilities::paintBoundingBoxIF(rgbFrame, resultingPoseIF, AnyCameraPinhole(trackerCamera), trackerObjectDimension, CV::Canvas::white(), CV::Canvas::black());
		Tracking::Utilities::paintCoordinateSystemIF(rgbFrame, resultingPoseIF, AnyCameraPinhole(trackerCamera), HomogenousMatrix4(true), trackerObjectDimension.diagonal() * Scalar(0.1));

		// If a cone or cylinder is being tracked, draw a gridded representation of it.
		// Note that the axis is always [0, 0, -1].
		if (trackerObjectCone.isValid() || trackerObjectCylinder.isValid())
		{
			constexpr unsigned int kNumCircles = 6u;
			constexpr unsigned int kNumSamples = 72u;
			constexpr unsigned int kNumVerticalLines = 4u;
			constexpr Scalar kAngleStep = Numeric::pi2() / kNumSamples;

			std::vector<Vectors2> paths;
			paths.reserve(kNumCircles + kNumVerticalLines);

			const bool isCone = trackerObjectCone.isValid();

			Vector3 origin;
			Scalar minSignedDistanceAlongAxis;
			Scalar maxSignedDistanceAlongAxis;
			Scalar tanHalfApexAngle = Scalar(0.);
			if (isCone)
			{
				origin = trackerObjectCone.apex();
				minSignedDistanceAlongAxis = trackerObjectCone.minSignedDistanceAlongAxis();
				maxSignedDistanceAlongAxis = trackerObjectCone.maxSignedDistanceAlongAxis();
				tanHalfApexAngle = std::tan(Scalar(0.5) * trackerObjectCone.apexAngle());
			}
			else // cylinder
			{
				origin = trackerObjectCylinder.origin();
				minSignedDistanceAlongAxis = trackerObjectCylinder.minSignedDistanceAlongAxis();
				maxSignedDistanceAlongAxis = trackerObjectCylinder.maxSignedDistanceAlongAxis();
			}

			const auto computeRadius = [&](const Scalar z) {
				return (isCone) ? z * tanHalfApexAngle : trackerObjectCylinder.radius();
			};

			const Scalar zStep = (maxSignedDistanceAlongAxis - minSignedDistanceAlongAxis) / (kNumCircles - 1);

			// Draw circles.
			for (unsigned int i = 0u; i < kNumCircles; ++i)
			{
				paths.emplace_back();
				Vectors2& path = paths.back();
				path.reserve(kNumSamples + 1);

				const Scalar z = -minSignedDistanceAlongAxis - i * zStep; // flip to account for axis direction
				const Scalar radius = computeRadius(z);

				for (unsigned int j = 0u; j < kNumSamples; ++j)
				{
					const Scalar theta = j * kAngleStep;
					const Vector3 point(radius * Numeric::cos(theta), radius * Numeric::sin(theta), z);
					path.push_back(trackerCamera.projectToImageIF<true>(resultingPoseIF, point + origin, trackerCamera.hasDistortionParameters()));
				}

				path.push_back(path[0]);
			}

			// Draw some vertical lines down the sides of the cone.
			constexpr Scalar kVerticalLineAngleStep = Numeric::pi2() / kNumVerticalLines;
			constexpr Scalar kVerticalLineAngleOffset = Scalar(0.5) * kVerticalLineAngleStep;
			for (unsigned int i = 0u; i < kNumVerticalLines; ++i)
			{
				paths.emplace_back();
				Vectors2& path = paths.back();
				path.reserve(2);

				const Scalar theta = kVerticalLineAngleOffset + i * kVerticalLineAngleStep;

				{
					const Scalar z = -minSignedDistanceAlongAxis;
					const Scalar radius = computeRadius(z);
					const Vector3 point(radius * Numeric::cos(theta), radius * Numeric::sin(theta), z);
					path.push_back(trackerCamera.projectToImageIF<true>(resultingPoseIF, point + origin, trackerCamera.hasDistortionParameters()));
				}

				{
					const Scalar z = -maxSignedDistanceAlongAxis;
					const Scalar radius = computeRadius(z);
					const Vector3 point(radius * Numeric::cos(theta), radius * Numeric::sin(theta), z);
					path.push_back(trackerCamera.projectToImageIF<true>(resultingPoseIF, point + origin, trackerCamera.hasDistortionParameters()));
				}
			}

			Tracking::Utilities::paintPaths<3>(rgbFrame, paths.data(), paths.size(), CV::Canvas::yellow(), Ocean::WorkerPool::get().scopedWorker()());
		}

		if (trackerObjectTriangles.size() > 0u)
		{
			Tracking::Utilities::paintTrianglesIF(rgbFrame, resultingPoseIF, AnyCameraPinhole(trackerCamera), trackerObjectTriangles.data(), trackerObjectTriangles.size(), CV::Canvas::yellow());
		}
	}
	else
	{
		trackerPerformance.stop();
	}

	time = trackerPerformance.average();
	frame = std::move(rgbFrame);

	return true;
}
