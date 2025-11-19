/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/calibration/offline/CameraCalibratorOfflineMain.h"

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"

#include "ocean/cv/calibration/CalibrationDebugElements.h"
#include "ocean/cv/calibration/CameraCalibrator.h"
#include "ocean/cv/calibration/Utilities.h"

#include "ocean/io/image/Image.h"

using namespace Ocean::CV::Calibration;

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

	// we forward all information/warning/error messages to the standard output
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	CommandArguments commandArguments;
	commandArguments.registerParameter("input", "i", "The input directory in which all the png images are located.");
	commandArguments.registerParameter("calibrationBoard", "cb", "The calibration board to be used, e.g., 'calibrationBoard_0_8x11");
	commandArguments.registerParameter("measuredWidth", "mw", "The measured width of the calibration board in millimeters.");
	commandArguments.registerParameter("measuredHeight", "mh", "The measured height of the calibration board in millimeters.");
	commandArguments.registerParameter("cameraModel", "cm", "The camera model to be used, e.g., 'pinhole' or 'fisheye', if not defined the model is determined automatically");
	commandArguments.registerParameter("output", "o", "The optional output directory for the calibration information");
	commandArguments.registerParameter("debugOutput", "do", "The optional output directory for the debug images");
	commandArguments.registerParameter("help", "h", "Showing this help output.");

	if (!commandArguments.parse(argv, argc))
	{
		Log::warning() << "Failure when parsing the command arguments.";
	}

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		return 0;
	}

	std::string input;
	if (!commandArguments.hasValue("input", input) || input.empty())
	{
		Log::error() << "Missing input directory.";
		return 1;
	}

	const IO::Directory inputDirectory(input);

	if (!inputDirectory.exists())
	{
		Log::error() << "The input directory '" << inputDirectory() << "' does not exist.";
		return 1;
	}

	IO::Files imageFiles = inputDirectory.findFiles("png");

	if (imageFiles.empty())
	{
		Log::error() << "The input directory '" << inputDirectory() << "' does not contain any image file.";
		return 1;
	}

	std::sort(imageFiles.begin(), imageFiles.end());

	const MetricCalibrationBoard calibrationBoard = determineCalibrationBoard(commandArguments);

	if (!calibrationBoard.isValid())
	{
		return 1;
	}

	IO::Directory outputDirectory;

	std::string output;
	if (commandArguments.hasValue("output", output))
	{
		outputDirectory = IO::Directory(output);

		if (!outputDirectory.isAbsolute())
		{
			outputDirectory = inputDirectory + outputDirectory;
		}

		if (!outputDirectory.exists() && !outputDirectory.create())
		{
			Log::error() << "The output directory '" << outputDirectory() << "' could not be created.";
			return 1;
		}
	}

	IO::Directory debugOutputDirectory;

	std::string debugOutput;
	if (commandArguments.hasValue("debugOutput", debugOutput))
	{
		debugOutputDirectory = IO::Directory(debugOutput);

		if (!debugOutputDirectory.isAbsolute())
		{
			debugOutputDirectory = inputDirectory + debugOutputDirectory;
		}

		if (!debugOutputDirectory.exists() && !debugOutputDirectory.create())
		{
			Log::error() << "The debug output directory '" << debugOutputDirectory() << "' could not be created.";
			return 1;
		}
	}

	if (debugOutputDirectory.isValid())
	{
		ocean_assert(CalibrationDebugElements::allowDebugging_);
		CalibrationDebugElements::get().activateAllElements();
	}

	AnyCameraType anyCameraType = AnyCameraType::INVALID;

	std::string cameraModelType;
	if (commandArguments.hasValue("cameraModel", cameraModelType))
	{
		if (cameraModelType == "pinhole")
		{
			anyCameraType = AnyCameraType::PINHOLE;
		}
		else if (cameraModelType == "fisheye")
		{
			anyCameraType = AnyCameraType::FISHEYE;
		}
		else
		{
			Log::error() << "Invalid camera model '" << cameraModelType << "'";
			return 1;
		}
	}

	const CameraCalibrator::InitialCameraProperties initialCameraProperties(anyCameraType);

	CameraCalibrator cameraCalibrator(calibrationBoard, initialCameraProperties);

	for (size_t imageIndex = 0; imageIndex < imageFiles.size(); ++imageIndex)
	{
		const IO::File& imageFile = imageFiles[imageIndex];

		const Frame frame = IO::Image::readImage(imageFile());

		Log::info() << "Handling image " << imageFile.name() << " ...";

		const CameraCalibrator::ImageResult imageResult = cameraCalibrator.handleImage(imageIndex, frame, WorkerPool::get().scopedWorker()());

		if (debugOutputDirectory.isValid())
		{
			writeDebugOutput(imageFile, debugOutputDirectory);
		}

		if (imageResult == CameraCalibrator::IR_BOARD_WAS_DETECTED)
		{
			const CalibrationBoardObservation& latestObservation = cameraCalibrator.latestObservation();

			const double percent = NumericD::ratio(double(latestObservation.imagePoints().size()), double(calibrationBoard.numberPoints()), 0.0);

			Log::info() << "Detected " << latestObservation.imagePoints().size() << " of " << calibrationBoard.numberPoints() << " points (" + String::toAString(percent * 100.0, 1u) + "%), visual coverage in the image: " << String::toAString(latestObservation.coverage() * 100.0f, 1u) << "%";

			const AnyCamera& camera = *latestObservation.camera();

			if (camera.name() == AnyCameraPinhole::WrappedCamera::name())
			{
				Log::info() << "Initial camera: PINHOLE, with " << String::toAString(Numeric::rad2deg(camera.fovX()), 1u) << " fovX";
			}
			else
			{
				Log::info() << "Initial camera: FISHEYE, with " << String::toAString(Numeric::rad2deg(camera.fovX()), 1u) << " fovX";
			}
		}
		else
		{
			Log::info() << "No board detected";
		}

		Log::info() << " ";
	}

	if (!cameraCalibrator.finalize())
	{
		Log::error() << "Failed to finalize the camera calibration.";
	}

	if (outputDirectory.isValid())
	{
		if (Frame outputFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_COVERAGE))
		{
			const IO::File outputFile(outputDirectory + IO::File("coverage.png"));

			if (!IO::Image::writeImage(outputFrame, outputFile()))
			{
				Log::warning() << "Failed to write output image '" << outputFile() << "'";
			}
		}

		if (Frame outputFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_PROJECTION_ERROR))
		{
			const IO::File outputFile(outputDirectory + IO::File("projection_error.png"));

			if (!IO::Image::writeImage(outputFrame, outputFile()))
			{
				Log::warning() << "Failed to write output image '" << outputFile() << "'";
			}
		}

		if (Frame outputFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_DISTORTION_GRID))
		{
			const IO::File outputFile(outputDirectory + IO::File("cameracalibrator_distortion_grid.png"));

			if (!IO::Image::writeImage(outputFrame, outputFile()))
			{
				Log::warning() << "Failed to write output image '" << outputFile() << "'";
			}
		}

		if (Frame outputFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_DISTORTION_VECTORS))
		{
			const IO::File outputFile(outputDirectory + IO::File("cameracalibrator_distortion_vectors.png"));

			if (!IO::Image::writeImage(outputFrame, outputFile()))
			{
				Log::warning() << "Failed to write output image '" << outputFile() << "'";
			}
		}

		for (const CalibrationBoardObservation& observation : cameraCalibrator.observations())
		{
			ocean_assert(observation.imageId() < imageFiles.size());
			const IO::File& imageFile = imageFiles[observation.imageId()];

			const Frame& frame = IO::Image::readImage(imageFile());

			for (const bool drawCorrespondences : {true, false})
			{
				Frame rgbFrame;
				if (!CV::FrameConverter::Comfort::convert(frame, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
				{
					ocean_assert(false && "This should never happen!");
					return 1;
				}

				CV::Calibration::Utilities::paintCalibrationBoardObservation(rgbFrame, calibrationBoard, observation, drawCorrespondences);

				CV::Canvas::drawText(rgbFrame, "Points: " + String::toAString(double(observation.imagePoints().size()) / double(calibrationBoard.numberPoints()) * 100.0, 1u) + "%", 5, 5, CV::Canvas::white(), CV::Canvas::black());

				const std::string type = drawCorrespondences ? "correspondences" : "projected_points";

				const IO::File outputFile(outputDirectory + IO::File(imageFile.baseName() + "_detected_board_" + type + ".png"));

				if (!IO::Image::writeImage(rgbFrame, outputFile()))
				{
					Log::warning() << "Failed to write image '" << outputFile() << "'";
				}
			}
		}

		const CameraProjectionChecker cameraProjectionChecker(cameraCalibrator.camera());

		if (Frame outputFrame = CV::Calibration::Utilities::visualizeDistortionValidity(cameraProjectionChecker, true))
		{
			const IO::File outputFile(outputDirectory + IO::File("cameracalibrator_distortion_validity_pixel.png"));

			if (!IO::Image::writeImage(outputFrame, outputFile()))
			{
				Log::warning() << "Failed to write outupt image '" << outputFile() << "'";
			}
		}

		if (Frame outputFrame = CV::Calibration::Utilities::visualizeDistortionValidity(cameraProjectionChecker, false))
		{
			const IO::File outputFile(outputDirectory + IO::File("cameracalibrator_distortion_validity_normalized.png"));

			if (!IO::Image::writeImage(outputFrame, outputFile()))
			{
				Log::warning() << "Failed to write outupt image '" << outputFile() << "'";
			}
		}
	}

	std::string cameraInformation;

	cameraInformation += cameraCalibrator.camera()->name();

	Scalar projectionError = Numeric::maxValue();
	const SharedAnyCamera camera = cameraCalibrator.camera(&projectionError);

	if (!camera)
	{
		Log::error() << "Invalid camera model!";
		return 1;
	}

	cameraInformation += "\nFinal projection error: " + String::toAString(projectionError, 2u);
	cameraInformation += "\nOptimized FovX: " + String::toAString(Numeric::rad2deg(camera->fovX()), 1u);
	cameraInformation += "\nNumber observations: " + String::toAString(cameraCalibrator.observations().size());
	cameraInformation += "\nNumber correspondence across all observations: " + String::toAString(cameraCalibrator.numberCorrespondences());
	cameraInformation += '\n' + String::toAString(camera->width()) + "x" + String::toAString(camera->height());

	if (camera->name() == AnyCameraFisheye::WrappedCamera::name())
	{
		const AnyCameraFisheye& anyFisheyeCamera = (const AnyCameraFisheye&)(*camera);
		const FisheyeCamera& fisheyeCamera = anyFisheyeCamera.actualCamera();

		unsigned int width;
		unsigned int height;
		FisheyeCamera::ParameterConfiguration parameterConfiguration;
		Scalars parameters;
		fisheyeCamera.copyParameters(width, height, parameters, parameterConfiguration);

		for (const Scalar& parameter : parameters)
		{
			cameraInformation += '\n' + String::toAString(parameter, 8u);
		}
	}
	else if (camera->name() == AnyCameraPinhole::WrappedCamera::name())
	{
		const AnyCameraPinhole& anyPinholeCamera = (const AnyCameraPinhole&)(*camera);
		const PinholeCamera& pinholeCamera = anyPinholeCamera.actualCamera();

		unsigned int width;
		unsigned int height;
		PinholeCamera::ParameterConfiguration parameterConfiguration;
		Scalars parameters;
		pinholeCamera.copyParameters(width, height, parameters, parameterConfiguration);

		for (const Scalar& parameter : parameters)
		{
			cameraInformation += '\n' + String::toAString(parameter, 8u);
		}
	}
	else
	{
		Log::error() << "Unknown camera model '" << camera->name() << "'!";
		return 1;
	}

	Log::info() << "Final camera: " << cameraInformation;

	if (outputDirectory.isValid())
	{
		const IO::File outputFile(outputDirectory + IO::File("camera_information.txt"));

		std::ofstream stream(outputFile().c_str(), std::ios::binary);

		stream << cameraInformation;
	}

	return 0;
}

MetricCalibrationBoard determineCalibrationBoard(const CommandArguments& commandArguments)
{
	std::string calibrationBoardType;
	if (!commandArguments.hasValue("calibrationBoard", calibrationBoardType))
	{
		Log::error() << "Missing calibration board definition.";
		return MetricCalibrationBoard();
	}

	CalibrationBoard calibrationBoard;
	if (!CV::Calibration::Utilities::createCalibrationBoardFromSeed(calibrationBoardType, calibrationBoard))
	{
		Log::error() << "The calibration board type '" << calibrationBoardType << "' could not be parsed.";
		return MetricCalibrationBoard();
	}

	double width = 0.0;
	if (!commandArguments.hasValue("measuredWidth", width) || width <= 0.0)
	{
		Log::error() << "Invalid measured width.";
		return MetricCalibrationBoard();
	}

	double height = 0.0;
	if (!commandArguments.hasValue("measuredHeight", height) || height <= 0.0)
	{
		Log::error() << "Invalid measured height.";
		return MetricCalibrationBoard();
	}

	const MetricSize measuredWidth(width, MetricSize::UT_MILLIMETER);
	const MetricSize measuredHeight(height, MetricSize::UT_MILLIMETER);

	return MetricCalibrationBoard(std::move(calibrationBoard), measuredWidth, measuredHeight);
}

void writeDebugOutput(const IO::File& inputFile, const IO::Directory& outputDirectory)
{
	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_POINT_DETECTOR_POINTS_CANDIDATES, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_00_pointdetector_candidates.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_POINT_DETECTOR_POINTS_NON_SUPPRESSED, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_01_pointdetector_candidates.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_POINT_DETECTOR_POINTS_SUPPRESSED, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_02_pointdetector_suppressed.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_POINT_DETECTOR_POINTS_OPTIMIZATION, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_03_pointdetector_optimization.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_DETECTED_POINTS, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_04_cameracalibrator_detectedpoints.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_05_cameracalibrator_markercandidates.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES_WITH_IDS, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_06_cameracalibrator_markercandidates_with_ids.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_MARKER_CANDIDATES_WITH_IDS_WITH_COORDINATES, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_07_cameracalibrator_markercandidates_with_coordinates.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_INITIAL_CAMERA_POSE_WITH_VALID_MARKER_CANDIDATES, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_08_cameracalibrator_markercandidates_with_validmarkercandidates.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_INITIAL_CAMERA_POSE_WITH_VALID_MARKER_CANDIDATES_OPTIMIZED_CAMERA, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_09_cameracalibrator_markercandidates_with_validmarkercandidates_optimizedcamera.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_OPTIMIZED_CAMERA_POSE_WITH_ADDITIONAL_POINTS, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_10_cameracalibrator_optimized_camera_pose_with_additional_points.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_OPTIMIZED_INITIAL_FOV_ITERATION_0, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_11_cameracalibrator_optimized_initial_fov_iteration0.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_OPTIMIZED_INITIAL_FOV_ITERATION_1, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_11_cameracalibrator_optimized_initial_fov_iteration1.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_OPTIMIZED_INITIAL_FOV_ITERATION_2, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_11_cameracalibrator_optimized_initial_fov_iteration2.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_ADDITIONAL_CORRESPONDENCES, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_14_cameracalibrator_additional_correspondences.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}

	if (Frame debugFrame = CalibrationDebugElements::get().element(CalibrationDebugElements::EI_CAMERA_CALIBRATOR_CAMERA_BOUNDARY, true))
	{
		const IO::File outputFile(outputDirectory + IO::File(inputFile.baseName() + "_15_cameracalibrator_camera_boundary.png"));

		if (!IO::Image::writeImage(debugFrame, outputFile()))
		{
			Log::warning() << "Failed to write debug image '" << outputFile() << "'";
		}
	}
}
