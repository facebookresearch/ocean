// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/detector/qrcodes/detector2drecordingprocessor/Detector2DRecordingProcessor.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/HighPerformanceTimer.h"
#include "ocean/base/ScopedValue.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/detector/qrcodes/QRCodeDetector2D.h"
#include "ocean/cv/detector/qrcodes/LegacyQRCodeDetector2D.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/media/Manager.h"
#include "ocean/media/MovieRecorder.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/platform/System.h"

#include "metaonly/ocean/devices/vrs/DevicePlayer.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		#include "ocean/media/mediafoundation/MediaFoundation.h"
		#include "ocean/media/wic/WIC.h"
	#elif defined(__APPLE__)
		#include "ocean/media/avfoundation/AVFoundation.h"
		#include "ocean/media/imageio/ImageIO.h"
	#endif
#endif

using namespace Ocean;
using namespace Ocean::CV::Detector::QRCodes;

#if defined(_WINDOWS)
	// Main function on Windows platforms
	int wmain(int argc, wchar_t* argv[])
#elif defined(__APPLE__) || defined(__linux__)
	// Main function on OSX platforms
	int main(int argc, char* argv[])
#else
	#error Missing implementation.
#endif
{
#ifdef OCEAN_COMPILER_MSC
	// Prevent the debugger to abort the application after an assert has been catched
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

#ifdef OCEAN_DEACTIVATED_MESSENGER
	#warning The messenger is currently deactivated.
#endif

	RandomI::initialize();

	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	CommandArguments commandArguments("Generates video files from VRS files recorded by the QR code detector.");
	commandArguments.registerNamelessParameters("Optional: The first command argument is interpreted as input parameter.");
	commandArguments.registerParameter("input", "i", "The input recording which will be processed. Either a single VRS file or a directory containing one or more VRS files in sub-folders.");
	commandArguments.registerParameter("output", "o", "In case the input is a directory, the root directory for all resulting processed video files. In case the input is one VRS file, the desired filename of the resulting video.");
	commandArguments.registerParameter("fps", "f", "Optionally set the FPS of the output video.");
	commandArguments.registerParameter("justvideo", "j", "Creates a video file of the recording only, will not run the detector.");
	commandArguments.registerParameter("legacy", "l", "Will run the old detector instead (this option will be removed soon along with the old detector).");
	commandArguments.registerParameter("help", "h", "Showing this help output.");

	if (!commandArguments.parse(argv, argc))
	{
		Log::warning() << "Failed to parse the command arguments.";
	}

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		return 0;
	}

	IO::Files vrsInputFiles;
	IO::Files videoOutputFiles;

	Value inputValue;
	if (!commandArguments.hasValue("input", &inputValue, false, 0u) || !inputValue.isString())
	{
		Log::error() << "No input specified.";
		return 1;
	}

	const Value outputValue(commandArguments.value("output"));

	const IO::File inputFile(inputValue.stringValue());

	if (inputFile.exists())
	{
		vrsInputFiles.push_back(inputFile);

		if (outputValue.isString())
		{
			videoOutputFiles.push_back(IO::File(outputValue.stringValue()));
		}
		else
		{
			videoOutputFiles.push_back(IO::File(inputFile.base() + ".mp4"));
		}
	}
	else
	{
		const IO::Directory inputDirectory(inputValue.stringValue());

		if (!inputDirectory.exists())
		{
			Log::error() << "The specified input directory \"" << inputDirectory() << "\" is not an existing file or directory.";
			return 1;
		}

		if (!outputValue.isString())
		{
			Log::error() << "No output directory specified.";
			return 1;
		}

		vrsInputFiles = inputDirectory.findFiles("vrs", true /* recursive*/);

		if (vrsInputFiles.empty())
		{
			Log::error() << "The specified input directory \"" << inputDirectory() << "\" does not contain any VRS file(s).";
			return 1;
		}

		videoOutputFiles.reserve(vrsInputFiles.size());

		const IO::Directory outputDirectory(outputValue.stringValue());

		for (const IO::File& vrsInputFile : vrsInputFiles)
		{
			videoOutputFiles.emplace_back(outputDirectory + IO::File(vrsInputFile().substr(inputDirectory().size()) + ".mp4"));
		}
	}

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		Media::MediaFoundation::registerMediaFoundationLibrary();
		Media::WIC::registerWICLibrary();
	#elif defined(__APPLE__)
		Media::AVFoundation::registerAVFLibrary();
		Media::ImageIO::registerImageIOLibrary();
	#endif
#else
	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());

	// Although we could use the tracking capabilities via the devices interface we invoke the trackers directly to simplify the application
	// thus, we use the media plugins only
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
#endif

	ocean_assert(vrsInputFiles.size() == videoOutputFiles.size());

	const Value fpsValue(commandArguments.value("fps"));
	unsigned int framesPerSecond = 30u;

	if (fpsValue.isInt() && fpsValue.intValue() > 0)
	{
		framesPerSecond = (unsigned int)(fpsValue.intValue());
	}

	const bool justVideo = commandArguments.hasValue("justvideo");

	const bool useLegacyDetector = commandArguments.hasValue("legacy");

	const size_t successful = processRecordings(vrsInputFiles, videoOutputFiles, framesPerSecond, justVideo, useLegacyDetector, WorkerPool::get().scopedWorker()());

	Log::info().newLine();

	if (successful == vrsInputFiles.size())
	{
		Log::info() << "Processed all VRS recordings.";
	}
	else
	{
		Log::info() << "Processed only " << successful << " of " << vrsInputFiles.size() << " recordings!";
	}

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(_WINDOWS)
		Media::WIC::unregisterWICLibrary();
		Media::MediaFoundation::unregisterMediaFoundationLibrary();
	#elif defined(__APPLE__)
		Media::ImageIO::unregisterImageIOLibrary();
		Media::AVFoundation::unregisterAVFLibrary();
	#endif
#else
	PluginManager::get().release();
#endif

	return 0;
}

size_t processRecordings(const IO::Files& vrsFiles, const IO::Files& outputFiles, const unsigned int framesPerSecond, const bool justVideo, const bool useOldDetector, Worker* worker)
{
	ocean_assert(framesPerSecond != 0u);

	Lock lock;

	size_t successful = 0;
	size_t index = 0;

	if (worker)
	{
		worker->executeFunction(Worker::Function::createStatic(processRecordingsSubset, vrsFiles.data(), outputFiles.data(), framesPerSecond, justVideo, useOldDetector, &index, &successful, vrsFiles.size(), &lock, 0u, 0u), 0u, (unsigned int)vrsFiles.size());
	}
	else
	{
		processRecordingsSubset(vrsFiles.data(), outputFiles.data(), framesPerSecond, justVideo, useOldDetector, &index, &successful, vrsFiles.size(), &lock, 0u, 0u);
	}

	return successful;
}

void processRecordingsSubset(const IO::File* vrsFiles, const IO::File* outputFiles, const unsigned int framesPerSecond, const bool justVideo, const bool useOldDetector, size_t* index, size_t* successful, const size_t size, Lock* lock, const unsigned int /*firstIndex*/, const unsigned int /*numberIndices*/)
{
	ocean_assert(vrsFiles != nullptr);
	ocean_assert(outputFiles != nullptr);
	ocean_assert(framesPerSecond != 0u);
	ocean_assert(lock != nullptr);
	ocean_assert(index != nullptr);

	while (true)
	{
		TemporaryScopedLock temporaryScopedLock(*lock);
			const size_t localIndex = (*index)++;
		temporaryScopedLock.release();

		if (localIndex >= size)
		{
			// We are done with all VRS recordings
			break;
		}

		if (processRecording(vrsFiles[localIndex], outputFiles[localIndex], framesPerSecond, justVideo, useOldDetector))
		{
			const ScopedLock scopedLock(*lock);
			(*successful)++;
		}
	}
}

bool processRecording(const IO::File& vrsFile, const IO::File& outputFile, const unsigned int framesPerSecond, const bool justVideo, const bool useOldDetector)
{
	ocean_assert(framesPerSecond != 0u);

	if (!vrsFile.exists())
	{
		Log::warning() << "VRS file \"" << vrsFile() << "\" does not exist anymore - skipping...";
		return false;
	}

	Devices::VRS::DevicePlayer devicePlayer;
	if (!devicePlayer.loadRecording(vrsFile()) || !devicePlayer.start(0.0f))
	{
		Log::error() << "Failed to load input VRS file";
		return false;
	}

	if (devicePlayer.frameMediums().empty())
	{
		Log::error() << "VRS files does not contain a frame medium";
		return false;
	}

	// Only select the first medium and ignore all others
	const Media::FrameMediumRef frameMedium = devicePlayer.frameMediums().front();
	ocean_assert(frameMedium);

	frameMedium->start();


	Media::MovieRecorderRef movieRecorder = Media::Manager::get().newRecorder(Media::Recorder::MOVIE_RECORDER);

	if (movieRecorder.isNull())
	{
		Log::error() << "Failed to create a recorder for the output!";
		return false;
	}

	if (outputFile.exists())
	{
		if (!outputFile.remove())
		{
			Log::error() << "The output \"" << outputFile() << "\" exists already and could not be deleted - skipping...";
			return false;
		}
	}
	else
	{
		const IO::Directory outputFileDirectory(outputFile);

		if (!outputFileDirectory.exists())
		{
			outputFileDirectory.create();
		}
	}

	movieRecorder->setFilename(outputFile());
	movieRecorder->setFrameFrequency(double(framesPerSecond));
	movieRecorder->setFilenameSuffixed(false);


	Frame rgbFrame;
	Frame yFrame;

	HighPerformanceStatistic performance;

	unsigned int frameIndex = 0u;
	unsigned int frameWithDetectedCode = 0u;

	while (true)
	{
		const Timestamp timestamp = devicePlayer.playNextFrame();

		if (timestamp.isInvalid())
		{
			break;
		}

		const ScopedValue<Index32> scopedFrameIndex(frameIndex, frameIndex + 1u);

		SharedAnyCamera anyCamera;
		const FrameRef frameRef = frameMedium->frame(timestamp, &anyCamera);

		if (!frameRef || !anyCamera)
		{
			Log::error() << "Failed to access frame";
			return false;
		}

		if (!CV::FrameConverter::Comfort::convert(*frameRef, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		if (!CV::FrameConverter::Comfort::convert(rgbFrame, FrameType::FORMAT_Y8, yFrame, CV::FrameConverter::CP_ALWAYS_COPY))
		{
			ocean_assert(false && "This should never happen!");
			return false;
		}

		ocean_assert(yFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

		size_t numberDetectedCodes = 0;

		if (!useOldDetector)
		{
			CV::Detector::QRCodes::QRCodeDetector2D::Observations observations;

			HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);
			const CV::Detector::QRCodes::QRCodes codes = CV::Detector::QRCodes::QRCodeDetector2D::detectQRCodes(*anyCamera, yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), &observations, WorkerPool::get().scopedWorker()());
			scopedPerformance.release();

			numberDetectedCodes = codes.size();

			CV::Detector::QRCodes::Utilities::drawObservations(*anyCamera, rgbFrame, observations, codes);
		}
		else
		{
			CV::Detector::QRCodes::LegacyQRCodeDetector2D::Observations observations;

			HighPerformanceStatistic::ScopedStatistic scopedPerformance(performance);
			const CV::Detector::QRCodes::QRCodes codes = CV::Detector::QRCodes::LegacyQRCodeDetector2D::detectQRCodes(yFrame.constdata<uint8_t>(), yFrame.width(), yFrame.height(), yFrame.paddingElements(), WorkerPool::get().scopedWorker()(), CV::Detector::QRCodes::LegacyQRCodeDetector2D::DM_STANDARD, &observations);
			scopedPerformance.release();

			numberDetectedCodes = codes.size();

			CV::Detector::QRCodes::Utilities::drawObservations(rgbFrame, observations, codes);
		}

		frameWithDetectedCode += numberDetectedCodes != 0 ? 1u : 0u;
		ocean_assert(frameWithDetectedCode <= (frameIndex + 1u));

		const Scalar detectionRate = Scalar(frameWithDetectedCode) / Scalar(frameIndex + 1u);

		Log::info() << "Detector: " << (useOldDetector ? "legacy" : "new") << ", detected codes: " << String::toAString(numberDetectedCodes) << ", detection rate: " << String::toAString(frameWithDetectedCode) + " / " + String::toAString(frameIndex + 1u) + " (" + String::toAString(detectionRate * Scalar(100), 2u) + "%)";

		const int yOffset = int((rgbFrame.height() + 1u) / 2u);
		CV::Canvas::drawText(rgbFrame, std::string("Detector: ") + (useOldDetector ? "legacy" : "new"), 10, yOffset, CV::Canvas::white(rgbFrame.pixelFormat()), CV::Canvas::black(rgbFrame.pixelFormat()));
		CV::Canvas::drawText(rgbFrame, "Detected codes: " + String::toAString(numberDetectedCodes), 10, yOffset + 20, CV::Canvas::white(rgbFrame.pixelFormat()), CV::Canvas::black(rgbFrame.pixelFormat()));
		CV::Canvas::drawText(rgbFrame, "Detection rate: " + String::toAString(frameWithDetectedCode) + " / " + String::toAString(frameIndex + 1u) + " (" + String::toAString(detectionRate * Scalar(100), 2u) + "%)", 10, yOffset + 20, CV::Canvas::white(rgbFrame.pixelFormat()), CV::Canvas::black(rgbFrame.pixelFormat()));

		ocean_assert(movieRecorder);

		if (!movieRecorder->frameType().isValid())
		{
			movieRecorder->setPreferredFrameType(rgbFrame.frameType());
			movieRecorder->start();
		}

		Frame recorderFrame;
		if (movieRecorder->lockBufferToFill(recorderFrame, /* respectFrameFrequency */ false))
		{
			CV::FrameConverter::Comfort::convertAndCopy(rgbFrame, recorderFrame);

			movieRecorder->unlockBufferToFill();
		}
	}

	ocean_assert(movieRecorder);
	movieRecorder->stop();

	Log::info() << "Average performance: " << performance.averageMseconds() << "ms";

	return true;
}
