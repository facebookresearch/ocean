/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Scheduler.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/io/File.h"
#include "ocean/io/Bitstream.h"

#include "ocean/media/Manager.h"
#include "ocean/media/ImageSequence.h"
#include "ocean/media/ImageSequenceFrameProviderInterface.h"

#include "ocean/tracking/Database.h"
#include "ocean/tracking/Utilities.h"

#include "ocean/tracking/offline/PointPaths.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/wic/WIC.h"
	#elif defined(__APPLE__)
		#include "ocean/media/imageio/ImageIO.h"
	#elif defined(__linux__)
		#include "ocean/media/openimagelibraries/OpenImageLibraries.h"
	#endif
#endif

using namespace Ocean;

double progressValue = -1.0;

static void onEventFunction()
{
	static double previousProgressValue = -1.0;

	if (NumericD::abs(previousProgressValue - progressValue) >= 0.05)
	{
		Log::info() << "Progress: " << String::toAString(progressValue * 100.0, 1u) << "%";

		previousProgressValue = progressValue;
	}
}

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
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

#ifdef OCEAN_RUNTIME_STATIC
		OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
		OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());
		OCEAN_APPLY_IF_LINUX(Media::OpenImageLibraries::registerOpenImageLibrariesLibrary());
#endif

	if (argc != 2)
	{
		Log::info() << "Invalid parameter number.";
		Log::info() << "Provide the filename of the first image of an image sequence";

		return 1;
	}

	RandomI::initialize();

	const IO::File imageSequenceFile(String::toAString(argv[1]));
	if (!imageSequenceFile.exists())
	{
		Log::info() << "The file of the image sequence does not exists - got \"" << imageSequenceFile() << "\"";

		return 1;
	}


	Media::ImageSequenceRef imageSequence = Media::Manager::get().newMedium(imageSequenceFile(), Media::Medium::IMAGE_SEQUENCE, true);

	if (imageSequence.isNull())
	{
		Log::info() << "Could not load the image sequence - got \"" << imageSequenceFile() << "\"";

		return 1;
	}

	Media::ImageSequenceFrameProviderInterface frameProviderInterface(imageSequence);

	while (!frameProviderInterface.isInitialized())
	{
		Log::info() << "Waiting until image sequence is initialized...";

		Thread::sleep(500u);
	}

	frameProviderInterface.setPreferredFrameType(FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT);
	const unsigned int frameNumber = frameProviderInterface.synchronFrameNumberRequest();

	Log::info() << "Total frame number: " << frameNumber << " [0, " << frameNumber - 1u << "]";

	if (frameNumber == 0u)
	{
		Log::info() << "The image sequence does not contain any images";

		return 1;
	}

	Scheduler::get().registerFunction(Scheduler::Callback::createStatic(onEventFunction));

	const Tracking::Offline::PointPaths::TrackingMethod trackingMethod = Tracking::Offline::PointPaths::TM_FIXED_PATCH_SIZE_31;
	const unsigned int binSize = 20u; // number of pixels per filtering bin (0 to avoid)
  const unsigned int strength = 2u; // minimal feature strength parameter, range[0, 256]
  const unsigned int coarsestLayerRadius = 20u; // search radius on the coarsest pyramid layer in pixels, with range [2, infinity)
  const unsigned int pyramidLayers = 5u; // number of pyramid layers, with range [1, infinity)

	const Tracking::Offline::PointPaths::TrackingConfiguration trackingConfiguration(trackingMethod, binSize, binSize, strength, coarsestLayerRadius, pyramidLayers);

	Tracking::Database database;
	if (!Tracking::Offline::PointPaths::determinePointPaths(frameProviderInterface, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT, trackingConfiguration, 0u, 0u, frameNumber - 1u, 20u, true, database, WorkerPool::get().scopedWorker()(), nullptr, &progressValue))
	{
		Log::info() << "Failed to track point paths!";

		return 1;
	}

	Scheduler::get().unregisterFunction(Scheduler::Callback::createStatic(onEventFunction));

	if (!database.isEmpty<false>())
	{
		const IO::File databaseFile = IO::File(imageSequenceFile.base() + ".database");

		std::ofstream stream;
		stream.open(databaseFile(), std::ios::binary);

		IO::OutputBitstream outputStream(stream);

		if (!Tracking::Utilities::writeDatabase(database, outputStream))
		{
			Log::info() << "Failed to write the database file \"" << databaseFile() << "\"";

			return 1;
		}

		/**
		 * The database can be read via:
		 *
		 * std::ifstream stream;
 		 * stream.open("datbasefile", std::ios::binary);
		 *
		 * IO::InputBitstream inputStream(stream);
		 *
		 * Tracking::Database database;
		 * Tracking::Utilities::readDatabase(inputStream, database);
		 */

		Log::info() << "Database file was written to \"" << databaseFile() << "\"";
	}

	frameProviderInterface.release();
	imageSequence.release();

#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
	OCEAN_APPLY_IF_LINUX(Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary());
#endif

	return 0;
}
