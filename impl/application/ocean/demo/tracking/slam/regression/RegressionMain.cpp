/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/tracking/slam/regression/RegressionMain.h"
#include "application/ocean/demo/tracking/slam/regression/ReplayValidation.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/RandomI.h"

#include "ocean/devices/serialization/Serialization.h"

#include "ocean/io/Directory.h"

#if defined(OCEAN_PLATFORM_BUILD_APPLE_MACOS)
	#include "ocean/media/avfoundation/AVFoundation.h"
	#include "ocean/media/imageio/ImageIO.h"
#endif

#if defined(OCEAN_PLATFORM_BUILD_WINDOWS)
	#include "ocean/media/wic/WIC.h"
	#include "ocean/media/mediafoundation/MediaFoundation.h"
#endif

using namespace Ocean;
using namespace Ocean::Test::TestTracking::TestSLAM;

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
	commandArguments.registerNamelessParameters("Optional the first command argument is interpreted as input directory");
	commandArguments.registerParameter("input", "i", "The directory containing .osn files to validate");
	commandArguments.registerParameter("tolerancePoses", "tp", "Tolerance for valid pose count regression in percent (default: 5)");
	commandArguments.registerParameter("toleranceTrackingCorrespondences", "ttc", "Tolerance for tracking correspondences regression in percent (default: 10)");
	commandArguments.registerParameter("tolerancePoseCorrespondences", "tpc", "Tolerance for pose correspondences regression in percent (default: 10)");
	commandArguments.registerParameter("toleranceBundleAdjustment", "tba", "Tolerance for Bundle Adjustment iteration difference in percent (default: 10)");
	commandArguments.registerParameter("writeNew", "w", "If specified, writes new statistics to a 'new' subfolder");
	commandArguments.registerParameter("help", "h", "Showing this help");

	if (!commandArguments.parse(argv, argc))
	{
		Log::warning() << "Failure when parsing the command arguments";
	}

	if (commandArguments.hasValue("help"))
	{
		Log::info() << "SLAM Regression Testing Tool";
		Log::info() << " ";
		Log::info() << "This tool replays .osn recordings and compares tracking statistics";
		Log::info() << "with saved baselines to detect performance regressions.";
		Log::info() << " ";
		Log::info() << commandArguments.makeSummary();

		return 0;
	}

	std::string inputValue;
	if (!commandArguments.hasValue("input", inputValue, false, 0u) || inputValue.empty())
	{
		Log::error() << "No input directory defined";
		Log::info() << " ";
		Log::info() << commandArguments.makeSummary();

		return 1;
	}

	const IO::Directory inputDirectory(inputValue);

	if (!inputDirectory.exists())
	{
		Log::error() << "The provided input directory '" << inputDirectory() << "' does not exist";

		return 1;
	}

	ReplayValidation::Configuration configuration;

	const double tolerancePoses = commandArguments.value<double>("tolerancePoses", -1.0, false);
	if (tolerancePoses >= 0.0)
	{
		configuration.toleranceValidPoses_ = tolerancePoses / 100.0;
	}

	const double toleranceTrackingCorrespondences = commandArguments.value<double>("toleranceTrackingCorrespondences", -1.0, false);
	if (toleranceTrackingCorrespondences >= 0.0)
	{
		configuration.toleranceTrackingCorrespondences_ = toleranceTrackingCorrespondences / 100.0;
	}

	const double tolerancePoseCorrespondences = commandArguments.value<double>("tolerancePoseCorrespondences", -1.0, false);
	if (tolerancePoseCorrespondences >= 0.0)
	{
		configuration.tolerancePoseCorrespondences_ = tolerancePoseCorrespondences / 100.0;
	}

	const double toleranceBundleAdjustment = commandArguments.value<double>("toleranceBundleAdjustment", -1.0, false);
	if (toleranceBundleAdjustment >= 0.0)
	{
		configuration.toleranceBundleAdjustmentIterations_ = toleranceBundleAdjustment / 100.0;
	}

	configuration.writeNewToSubfolder_ = commandArguments.hasValue("writeNew");

	if (!configuration.isValid())
	{
		Log::error() << "Invalid configuration - tolerance values must be between 0 and 100";

		return 1;
	}

	const bool succeeded = ReplayValidation::testReplayValidation(inputDirectory(), configuration);

	Log::info() << " ";

	if (succeeded)
	{
		Log::info() << "All validations succeeded";

		return 0;
	}
	else
	{
		Log::error() << "Some validations FAILED";

		return 1;
	}
}

ScopedPlugin::ScopedPlugin()
{
#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::registerMediaFoundationLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::registerAVFLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());

	Devices::Serialization::registerSerializationLibrary();
#endif // OCEAN_RUNTIME_STATIC
}

ScopedPlugin::~ScopedPlugin()
{
#ifdef OCEAN_RUNTIME_STATIC
	Devices::Serialization::unregisterSerializationLibrary();

	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
	OCEAN_APPLY_IF_APPLE(Media::AVFoundation::unregisterAVFLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::MediaFoundation::unregisterMediaFoundationLibrary());
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
#endif // OCEAN_RUNTIME_STATIC
}
