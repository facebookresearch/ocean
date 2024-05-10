/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/advanced/panoramaextractor/PanoramaExtractorMain.h"

#include "ocean/base/Build.h"
#include "ocean/base/DateTime.h"
#include "ocean/base/PluginManager.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/Timestamp.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"

#include "ocean/cv/advanced/PanoramaFrame.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"
#include "ocean/io/JSONConfig.h"

#include "ocean/math/Euler.h"
#include "ocean/math/Quaternion.h"
#include "ocean/math/Random.h"

#include "ocean/media/Utilities.h"

#include "ocean/platform/System.h"

#if defined(_WINDOWS)
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

template <typename TStringType = std::string>
struct ExtractorParameters
{
	public:

		/// Width of the extracted images, range: (0, infinity)
		unsigned int outputWidth = 1280u;

		/// Height of the extracted images, range: (0, infinity)
		unsigned int outputHeight = 720u;

		/// Approximate overlap of neighboring images, range: (0, 1)
		Scalar overlap = Scalar(0.25);

		/// Horizontal field of view in degrees, range: (0, 360)
		Scalar fovX = Scalar(60);

		/// Coverage of the input image when stitching the extracted images, range: (0, 360]
		Scalar coverage = Scalar(360);

		/// Noise added to the orientation of each extracted camera image to simulate a shaky hand
		Scalar orientationNoise = Scalar(0);

		/// Maximum deviation of the generated IMU data from the true camera orientation, range: [0, 90)
		Scalar imuInaccuracy = Scalar(0);

		/// Location where the output will be stored
		TStringType outputDirectory;

		/// Filename of the 360 input image
		TStringType image360Filename;

		/// Visualize the camera frames in the panorama and save as an image, if true
		bool visualizeCameraOutlines = true;

	public:

		/**
		 * Default constructor
		 */
		ExtractorParameters() = default;

		/**
		 * Generate a string containing the usage information
		 * @param applicationName The name of the binary
		 * @return A string with the usage information
		 */
		std::string printUsage(const std::string& applicationName) const;

		/**
		 * Load parameters directly from the command-line
		 * In case of a failure this function also prints the usage information to
		 * the standard output.
		 * @param arguments The array containing the command-line arguments, e.g. the array @c argv from @c main() (the first element is the name of the application)
		 * @param count The number of command-line arguments
		 *
		 * @return True if the parameters were correctly parsed, otherwise false.
		 */
		bool loadFromCommandLine(const char** arguments, const size_t count);

		/**
		 * Load parameters directly from the command-line
		 * In case of a failure this function also prints the usage information to
		 * the standard output.
		 * @param arguments The array containing the command-line arguments without the application name as the first element, e.g. the array @c argv from @c main()
		 * @param count The number of command-line arguments
		 * @param applicationName The name of the application
		 *
		 * @return True if the parameters were correctly parsed, otherwise false.
		 */
		bool loadFromCommandLine(const char** argument, const size_t count, const std::string& applicationName);

		/** Check for valid parameter values
		 * @return True if all parameters are in the correct range of values, otherwise false.
		 */
		bool validParameterValues() const;
};

/**
 * Writes the metadata of an extracted image to a JSON file.
 * @param filename The name of the JSON file to be created
 * @param euler The orientation of the extracted frame in the framework coordinate system
 * @param innerFov The camera's inner field of view (the smaller one of both possible angles) in radian, with range (0, PI)
 * @param timestamp The timestamp of the extracted image
 * @return True, if succeeded
 */
bool writeMetadata(const std::string& filename, const Euler& euler, const Scalar innerFov, const Timestamp& timestamp);

/**
 * Adds random noise to an euler orientation.
 * @param euler The euler orientation to which the random noise will be added, must be valid
 * @param maximalNoise The maximal noise to be added to each of the three orientation parameters in radian, with range [0, PI/2)
 * @return The resulting orientation containing random noise
 */
Euler addNoise(const Euler& euler, const Scalar maximalNoise);

/**
 * Extracts images from a (full) 360 degree panorama frame and stores the individual images together with some metadata into a directory.
 * @param panoramaFile The file of the given panorama frame from which the individual images will be extracted
 * @param pixelFormat The pixel format of the result extracted images, can be e.g., FORMAT_RGB24 or FORMAT_Y8
 * @param coverage Amount of the 360 input image that is covered horizontally by the sector formed by the extracted images (in radian), range: (cameraFovX, 2*PI]
 * @param cameraWidth The width of the camera (and thus the resulting extracted images) in pixel, with range [1, infinity)
 * @param cameraHeight The height of the camera (and thus the resulting extracted images) in pixel, with range [1, infinity)
 * @param overlap The ratio of overlap of neighboring images, range: (0, 1) (no overlap at all = 0, complete overlap = 1)
 * @param cameraFovX The horizontal field of view of the camera in radian, with range (0, PI)
 * @param orientationNoise Noise that is added randomly to the orientation of the extracted image to simulate a shaky hand, added to all three axes, in radian, range: [0, PI/2) (in practice, this in the range of 0-15 degrees (converted to RAD) or otherwise stitchers will have problems with the images)
 * @param imuInaccuracy Maximum error that is added randomly to the true camera orientation in order to simulate inaccurate IMU data due to miscalibration/drift/synchronization errors, (in radian, added to all three axes), range: [0, PI/2)
 * @param resultDirectory Optional directory in which the results will be stored; if an invalid directory is provided, the results will be stored in the same directory as the panorama frame
 * @param visualizeCameraOutlines If true, draw the outlines of the cameras into the panorama and save as an image
 * @return True, if succeeded
 */
bool extractImageToDirectory(const IO::File& panoramaFile, const FrameType::PixelFormat pixelFormat, const Scalar coverage, const unsigned int cameraWidth, const unsigned int cameraHeight, const Scalar overlap, const double cameraFovX, const Scalar orientationNoise, const Scalar imuInaccuracy, const IO::Directory& resultDirectory = IO::Directory(), const bool visualizeCameraOutlines = true);

/**
 * Draw the outline of a camera frame into an image
 * @param panoramaImage A writable panorama image. Must have the same size as @c panoramaFrame, pixel format must be FORMAT_RGB24
 * @param camera A camera
 * @param orientation A orientation that is applied to the camera before drawing the outline of its frame
 * @param panoramaFrame The actual panorama that is used as a reference
 * @param imuOrientation If specified, will draw a thinner outline of the camera frame based on the generated IMU orientation, nullptr to ignore this parameter
 * @param color Four color values (RGBA), will be randomly selected if it is nullptr, range of color values: [0, 255]
 * @tparam lineWidth Width of the lines that are used to draw the outlines
 */
template <unsigned int lineWidth = 7u>
void drawCameraOutlineInPanoramaFrame(Frame& panoramaImage, const PinholeCamera& camera, const SquareMatrix3& orientation, const CV::Advanced::PanoramaFrame& panoramaFrame, const SquareMatrix3* imuOrientation = nullptr, const unsigned char* color = nullptr);

template <typename TStringType>
std::string ExtractorParameters<TStringType>::printUsage(const std::string& applicationName) const
{
	std::ostringstream oss;

	oss << applicationName << " [OPTIONS] FILE\n";
	oss << "\n";
	oss << "FILE is the file path to a true 360 image. \n";
	oss << "\n";
	oss << "OPTIONS:\n";
	oss << "\n";
	oss << "A value after an equal sign is used as default if the corresponding option is not specified.\n";
	oss << "\n";
	oss << "[--output-width|-w WIDTH=" << outputWidth << "]   - Width of the extracted images\n";
	oss << "[--output-height|-h HEIGHT=" << outputHeight << "]  - Height of the extracted images\n";
	oss << "[--overlap|-o OVERLAP=" << overlap << "]      - Approximate overlap of neighboring images, range: (0, 1)\n";
	oss << "[--fovx|-f FOVX=" << fovX << "]              - Horizontal field of view in degrees, range: (0, 360)\n";
	oss << "[--coverage|-c COV=" << coverage << "]          - Horizontal sector formed by the extracted images that covers of the input image, in degrees: (0, 360]\n";
	oss << "[--orientation-noise|-n VAL=" << orientationNoise << "]   - Noise added to the camera orientation when extracting images (shaky hand), range: [0, 90)\n";
	oss << "[--imu-inaccuracy|-i VAL=" << imuInaccuracy << "]      - Maximum deviation of the generated IMU data from the true camera orientation, range: [0, 90)\n";
	oss << "[--output-dir|-d DIRECTORY=\"" << outputDirectory << "\"]   - Location where the output will be stored; will be created\n";
	oss << "[--visualize-outlines|-v VIS=" << (visualizeCameraOutlines ? 1 : 0) << "]  - Visualize camera outlines in the panorama frame and save to image, if true.\n";
	oss << "[--help]                         - This help screen\n";

	return oss.str();
}

template <typename TStringType>
bool ExtractorParameters<TStringType>::loadFromCommandLine(const char** arguments, const size_t count)
{
	ocean_assert(arguments != nullptr && count > 0);
	return loadFromCommandLine(arguments + 1, count - 1, arguments[0]);
}

template <typename TStringType>
bool ExtractorParameters<TStringType>::loadFromCommandLine(const char** arguments, const size_t count, const std::string& applicationName)
{
	bool isValidCount = true;

	if (count < 1)
	{
		isValidCount = false;
	}
	else if (count % 2 != 1) // count = N key-value-pairs + 1 input file
	{
		Log::info() << "The number of arguments seems to be wrong\n";
		isValidCount = false;
	}

	if (!isValidCount)
	{
		Log::info() << printUsage(applicationName);
		return false;
	}

	ocean_assert(arguments);

	image360Filename = arguments[count - 1];

	if (image360Filename.empty())
	{
		Log::info() << "Please specify the filename of a 360 input image\n";
		Log::info() << printUsage(applicationName);
		return false;
	}

	for (size_t i = 0; i < count - 1; i += 2)
	{
		const std::string parameter = arguments[i];
		const std::string value = arguments[i + 1];

		if (parameter == "--output-width" || parameter == "-w")
		{
			outputWidth = (unsigned int)std::stoul(value);
		}
		else if (parameter == "--output-height" || parameter == "-h")
		{
			outputHeight = (unsigned int)std::stoul(value);
		}
		else if (parameter == "--overlap" || parameter == "-o")
		{
			overlap = Scalar(std::stod(value));
		}
		else if (parameter == "--fovx" || parameter == "-f")
		{
			fovX = Scalar(std::stod(value));
		}
		else if (parameter == "--coverage" || parameter == "-c")
		{
			coverage = Scalar(std::stod(value));
		}
		else if (parameter == "--orientation-noise" || parameter == "-n")
		{
			orientationNoise = Scalar(std::stod(value));
		}
		else if (parameter == "--imu-inaccuracy" || parameter == "-i")
		{
			imuInaccuracy = Scalar(std::stod(value));
		}
		else if (parameter == "--output-dir" || parameter == "-d")
		{
			outputDirectory = value;
		}
		else if (parameter == "--visualize-outlines" || parameter == "-v")
		{
			visualizeCameraOutlines = (value == "1" ? true : false);
		}
		else if (parameter == "--help")
		{
			Log::info() << printUsage(applicationName);
			return false;
		}
		else
		{
			Log::info() << "Unknown parameter \"" << parameter << "\"!\n";
			Log::info() << printUsage(applicationName);
			return false;
		}
	}

	if (!validParameterValues())
	{
		return false;
	}

	Log::info() << "--- Parameters ---";
	Log::info() << "Output width:      " << outputWidth;
	Log::info() << "Output height:     " << outputHeight;
	Log::info() << "Overlap:           " << overlap;
	Log::info() << "FOVX:              " << fovX;
	Log::info() << "Coverage:          " << coverage;
	Log::info() << "Orientation noise: " << orientationNoise;
	Log::info() << "IMU inaccuracy:    " << imuInaccuracy;
	Log::info() << "Output directory:  " << outputDirectory;
	Log::info() << "Input image:       " << image360Filename;
	Log::info() << "---";

	return true;
}

template <typename TStringType>
bool ExtractorParameters<TStringType>::validParameterValues() const
{
	std::vector<std::string> errorMessages;

	if (overlap <= Scalar(0) || overlap >= Scalar(1))
	{
		errorMessages.emplace_back("The overlap ratio (= " + String::toAString(overlap, 2u) + ") must be in the range (0, 1).");
	}

	if (fovX <= Scalar(0) || fovX >= Scalar(360))
	{
		errorMessages.emplace_back("The horizontal FOV (= " + String::toAString(fovX, 2u) + ") must be in the range (0, 360).");
	}

	if (coverage <= Scalar(0) || coverage > Scalar(360))
	{
		errorMessages.emplace_back("The horizontal coverage (= " + String::toAString(coverage, 2u) + ") must be in the range (0, 360].");
	}

	if (orientationNoise < Scalar(0) || orientationNoise >= Scalar(90))
	{
		errorMessages.emplace_back("The orientation noise (= " + String::toAString(orientationNoise, 2u) + ") must be in the range [0, 90).");
	}

	if (imuInaccuracy < Scalar(0) || imuInaccuracy >= Scalar(90))
	{
		errorMessages.emplace_back("The IMU inaccuracy (= " + String::toAString(imuInaccuracy, 2u) + ") must be in the range [0, 90).");
	}

	if (!errorMessages.empty())
	{
		Log::info() << "ERRORS:";
		for (size_t i = 0; i < errorMessages.size(); ++i)
		{
			Log::info() << "  " << i << ". " << errorMessages[i];
		}
	}

	return errorMessages.empty();
}

bool writeMetadata(const std::string& filename, const Euler& euler, const Scalar innerFov, const Timestamp& timestamp)
{
	const Quaternion wTq(euler);
	const Quaternion w_Tw(Vector3(1, 0, 0), Numeric::pi_2());

	const Quaternion quaternion(w_Tw * wTq);

	IO::JSONConfig config(filename, false);

	IO::JSONConfig::Value& deviceOrientationNode = config.add("DeviceOrientation");
	IO::JSONConfig::Value& quaternionNode = deviceOrientationNode.add("Quaternion");
	quaternionNode["w"] = double(quaternion.w());
	quaternionNode["x"] = double(quaternion.x());
	quaternionNode["y"] = double(quaternion.y());
	quaternionNode["z"] = double(quaternion.z());

	config["UnixTimestamp"] = double(timestamp);

	config["InnerFov"] = double(Numeric::rad2deg(innerFov));

	return config.write();
}

Euler addNoise(const Euler& euler, const Scalar maximalNoise)
{
	ocean_assert(euler.isValid());

	if (maximalNoise == 0)
		return euler;

	ocean_assert(maximalNoise > 0 && maximalNoise < Numeric::pi_2());

	const Scalar noiseYaw = Random::scalar(-maximalNoise, maximalNoise);
	const Scalar noisePitch = Random::scalar(-maximalNoise, maximalNoise);
	const Scalar noiseRoll = Random::scalar(-maximalNoise, maximalNoise);

	return Euler(Numeric::angleAdjustNull(euler.yaw() + noiseYaw), Numeric::angleAdjustNull(euler.pitch() + noisePitch), Numeric::angleAdjustNull(euler.roll() + noiseRoll));
}

bool extractImageToDirectory(const IO::File& panoramaFile, const FrameType::PixelFormat pixelFormat, const Scalar coverage, const unsigned int cameraWidth, const unsigned int cameraHeight, const Scalar overlap, const double cameraFovX, const Scalar orientationNoise, const Scalar imuInaccuracy, const IO::Directory& resultDirectory, const bool visualizeCameraOutlines)
{
	ocean_assert(panoramaFile.exists());
	ocean_assert(coverage > cameraFovX && coverage <= Numeric::pi2());
	ocean_assert(cameraWidth >= 1u && cameraHeight >= 1u);
	ocean_assert(overlap > 0 && overlap < 1);
	ocean_assert(cameraFovX > 0.0 && cameraFovX < Numeric::pi());
	ocean_assert(orientationNoise >= 0 && orientationNoise < Numeric::pi_2());
	ocean_assert(imuInaccuracy >= 0 && imuInaccuracy < Numeric::pi_2());

	Frame frame = Media::Utilities::loadImage(panoramaFile());
	if (!frame.isValid())
	{
		Log::info() << "Failed to load panorama frame \"" << panoramaFile() << "\"";
		return false;
	}

	if (!CV::FrameConverter::Comfort::change(frame, pixelFormat, WorkerPool::get().scopedWorker()()))
		return false;

	// The panorama frame holds an additional mask identifying valid and invalid pixels (pixels that are covered by visual information)
	// As we use 360 degree panorama frames, all pixels are converted with (valid) visual information so that we simply create a mask frame with same value for each pixel

	Frame mask(FrameType(frame, FrameType::FORMAT_Y8));
	mask.setValue(0x00);

	const CV::Advanced::PanoramaFrame panoramaFrame(std::move(frame), std::move(mask), 0x00, CV::Advanced::PanoramaFrame::UM_SET_ALL);

	const PinholeCamera camera(cameraWidth, cameraHeight, cameraFovX);

	Eulers orientations;
	Eulers imuOrientations;

	const Scalar overlyingAngleX = camera.fovX() * overlap;
	ocean_assert(overlyingAngleX > Numeric::eps());

	Scalar angleStep = camera.fovX() - overlyingAngleX;

	ocean_assert(Numeric::isNotEqualEps(angleStep));
	const unsigned int imageNumber = (unsigned int)(Numeric::ceil(coverage / angleStep));
	ocean_assert(imageNumber >= 1u);

	// We adjust the step width to have a equally distributed images. If the
	// input image is only covered partially, extract the image around the
	// of the input image.
	angleStep = coverage / Scalar(imageNumber);
	Scalar yaw = 0;

	if (Numeric::abs(coverage - Numeric::pi2()) > Numeric::eps())
	{
		yaw = Numeric::angleAdjustNull((-0.5 * coverage) + (0.5 * cameraFovX));
	}

	for (unsigned int n = 0u; n < imageNumber; ++n)
	{
		Euler orientation(yaw, 0, 0);

		if (orientationNoise > Numeric::eps())
		{
			orientation = addNoise(Euler(yaw, 0, 0), orientationNoise);

			// Make sure the frames i=[1,N) overlap with their previous frame i-1 in order to avoid unexpected stitching results
			if (n > 0u)
			{
				Scalar intersectionRatio = 0;
				CV::Advanced::PanoramaFrame::approximateIntersectionArea(camera, SquareMatrix3(orientations[n - 1u]), camera, SquareMatrix3(orientation), &intersectionRatio);

				Scalar closestToOverlapDiff = Numeric::abs(intersectionRatio - overlap);
				Scalar closestToOverlap = intersectionRatio;

				const Timestamp startTime(true);
				const double maxDuration = 10; // seconds

				while (!Numeric::isEqual(intersectionRatio, overlap, Scalar(0.1) * overlap) && (startTime + maxDuration >= Timestamp(true)))
				{
					const Euler currentOrientation = addNoise(Euler(yaw, 0, 0), orientationNoise);
					CV::Advanced::PanoramaFrame::approximateIntersectionArea(camera, SquareMatrix3(orientations[n - 1u]), camera, SquareMatrix3(currentOrientation), &intersectionRatio);
					const Scalar currentDifferenceToOverlap = Numeric::abs(intersectionRatio - overlap);

					if (currentDifferenceToOverlap < closestToOverlapDiff)
					{
						orientation = currentOrientation;
						closestToOverlapDiff = currentDifferenceToOverlap;
						closestToOverlap = intersectionRatio;
					}
				}
				Log::info() << n << ": closest to selected overlap ratio = " << closestToOverlap << " (" << overlap << ")";
			}
		}

		orientations.push_back(orientation);
		imuOrientations.push_back(addNoise(orientation, imuInaccuracy));

		yaw = Numeric::angleAdjustNull(yaw + angleStep);
	}

	std::string utcDateTimeString = String::replaceCharacters(DateTime::string(), '.', '-');
	utcDateTimeString = String::replaceCharacters(utcDateTimeString, ' ', '_');

	IO::Directory outputDirectory;

	if (resultDirectory.isValid())
		outputDirectory = resultDirectory;
	else
		outputDirectory = IO::Directory(panoramaFile) + IO::Directory(std::string("result_") + utcDateTimeString);

	if (!outputDirectory.create())
	{
		Log::info() << "Failed to create output directory \"" << outputDirectory() << "\"!";
		return false;
	}

	std::string panoramaName = panoramaFile.name().substr();
	panoramaName.resize(panoramaName.rfind('.'));

	if (visualizeCameraOutlines)
	{
		Frame cameraOutlines;

		if (!CV::FrameConverter::Comfort::convert(panoramaFrame.frame(), FrameType::FORMAT_RGB24, cameraOutlines))
		{
			Log::info() << "Cannot convert input image ... aborting!";
			return false;
		}

		for (size_t i = 0; i < orientations.size(); ++i)
		{
			const SquareMatrix3 imuOrientation = SquareMatrix3(imuOrientations[i]);
			drawCameraOutlineInPanoramaFrame(cameraOutlines, camera, SquareMatrix3(orientations[i]), panoramaFrame, &imuOrientation);
		}

		const IO::File cameraOutlinesFile(outputDirectory + IO::File(panoramaName + "-camera-outlines.jpg"));

		Media::Utilities::saveImage(cameraOutlines, cameraOutlinesFile(), false);
	}

	IO::Directory groundTruthDirectory = outputDirectory + IO::Directory(std::string("groundtruth"));

	if (!groundTruthDirectory.create())
	{
		Log::info() << "Failed to create the directory for the JSON groundtruth \"" << groundTruthDirectory() << "\"!";
		return false;
	}

	ocean_assert(orientations.size() == imuOrientations.size());
	for (size_t n = 0; n < orientations.size(); ++n)
	{
		const Euler& orientation = orientations[n];

		Frame extractedFrame, extractedMask;

		panoramaFrame.extractFrame(camera, SquareMatrix3(orientation), extractedFrame, extractedMask);

		const IO::File frameFile(outputDirectory + IO::File(panoramaName + std::string("-photo-") + String::toAString(n) + std::string(".jpg")));

		if (!Media::Utilities::saveImage(extractedFrame, frameFile(), false))
		{
			return false;
		}

		// first we write the metadata as it would be available from the mobile devices (containing some noise/error)

		const IO::File metadataFile = IO::File(outputDirectory + IO::File(panoramaName + "-metadata-" + String::toAString(n) + ".json"));

		if (!writeMetadata(metadataFile(), imuOrientations[n], min(camera.fovX(), camera.fovY()), Timestamp(true)))
		{
			return false;
		}

		const IO::File groundTruthMetadataFile = IO::File(groundTruthDirectory + IO::File(panoramaName + "-metadata-" + String::toAString(n) + ".json"));

		if (!writeMetadata(groundTruthMetadataFile(), orientation, min(camera.fovX(), camera.fovY()), Timestamp(true)))
		{
			return false;
		}
	}

	return true;
}

template <unsigned int lineWidth>
void drawCameraOutlineInPanoramaFrame(Frame& cameraOutlines, const PinholeCamera& camera, const SquareMatrix3& orientation, const CV::Advanced::PanoramaFrame& panoramaFrame, const SquareMatrix3* imuOrientation, const unsigned char* color)
{
	const unsigned int cameraWidth = camera.width();
	const unsigned int cameraHeight = camera.height();
	ocean_assert(cameraWidth > 0u && cameraHeight > 0u);
	ocean_assert(cameraOutlines.width() >= camera.width());
	ocean_assert(cameraOutlines.channels() <= 4u);

	const uint8_t outlineColor[4] =
	{
		(color ? color[0] : uint8_t(RandomI::random(25, 255))),
		(color ? color[1] : uint8_t(RandomI::random(25, 255))),
		(color ? color[2] : uint8_t(RandomI::random(25, 255))),
		(color ? color[3] : uint8_t(0xFF)),
	};

	if (imuOrientation)
	{
		drawCameraOutlineInPanoramaFrame<1u>(cameraOutlines, camera, *imuOrientation, panoramaFrame, nullptr, outlineColor);
	}

	const Vector3 principalRay(orientation * camera.vector(camera.undistort<true>(Vector2(camera.principalPointX(), camera.principalPointY()))));
	const Vector2 principalAngle(panoramaFrame.ray2angleStrict(principalRay));

	Vector2 from0 = panoramaFrame.cameraPixel2panoramaPixel(camera, orientation, Vector2(0, 0), principalAngle);
	Vector2 from1 = panoramaFrame.cameraPixel2panoramaPixel(camera, orientation, Vector2(0, camera.height() - 1), principalAngle);
	Vector2 to0, to1;

	for (unsigned int x = 0; x < cameraWidth; x += 10)
	{
		to0 = panoramaFrame.cameraPixel2panoramaPixel(camera, orientation, Vector2(x, 0), principalAngle);
		to1 = panoramaFrame.cameraPixel2panoramaPixel(camera, orientation, Vector2(x, cameraHeight - 1), principalAngle);

		CV::Canvas::line<lineWidth>(cameraOutlines, from0.x(), from0.y(), to0.x(), to0.y(), outlineColor);
		CV::Canvas::line<lineWidth>(cameraOutlines, from1.x(), from1.y(), to1.x(), to1.y(), outlineColor);

		from0 = to0;
		from1 = to1;
	}

	to0 = panoramaFrame.cameraPixel2panoramaPixel(camera, orientation, Vector2(cameraWidth - 1u, 0), principalAngle);
	to1 = panoramaFrame.cameraPixel2panoramaPixel(camera, orientation, Vector2(cameraWidth - 1u, cameraHeight - 1u), principalAngle);

	CV::Canvas::line<lineWidth>(cameraOutlines, from0.x(), from0.y(), to0.x(), to0.y(), outlineColor);
	CV::Canvas::line<lineWidth>(cameraOutlines, from1.x(), from1.y(), to1.x(), to1.y(), outlineColor);

	from0 = panoramaFrame.cameraPixel2panoramaPixel(camera, orientation, Vector2(0, 0), principalAngle);
	from1 = panoramaFrame.cameraPixel2panoramaPixel(camera, orientation, Vector2(cameraWidth - 1, 0), principalAngle);

	for (unsigned int y = 0; y < cameraHeight; ++y)
	{
		to0 = panoramaFrame.cameraPixel2panoramaPixel(camera, orientation, Vector2(0, y), principalAngle);
		to1 = panoramaFrame.cameraPixel2panoramaPixel(camera, orientation, Vector2(cameraWidth - 1, y), principalAngle);

		CV::Canvas::line<lineWidth>(cameraOutlines, from0.x(), from0.y(), to0.x(), to0.y(), outlineColor);
		CV::Canvas::line<lineWidth>(cameraOutlines, from1.x(), from1.y(), to1.x(), to1.y(), outlineColor);

		from0 = to0;
		from1 = to1;
	}

	to0 = panoramaFrame.cameraPixel2panoramaPixel(camera, orientation, Vector2(0, cameraHeight - 1), principalAngle);
	to1 = panoramaFrame.cameraPixel2panoramaPixel(camera, orientation, Vector2(cameraWidth - 1u, cameraHeight - 1u), principalAngle);

	CV::Canvas::line<lineWidth>(cameraOutlines, from0.x(), from0.y(), to0.x(), to0.y(), outlineColor);
	CV::Canvas::line<lineWidth>(cameraOutlines, from1.x(), from1.y(), to1.x(), to1.y(), outlineColor);
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

	const std::string frameworkPath(Platform::System::environmentVariable("OCEAN_DEVELOPMENT_PATH"));

#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::registerWICLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::registerImageIOLibrary());
#else
	PluginManager::get().collectPlugins(frameworkPath + std::string("/bin/plugins/") + Build::buildString());
	PluginManager::get().loadPlugins(PluginManager::PluginType(PluginManager::TYPE_MEDIA));
#endif

	ocean_assert(argc >= 0);

#if defined(_WINDOWS)
	ExtractorParameters<std::wstring> parameters;
	const bool loadedParameters = parameters.loadFromCommandLine(const_cast<const wchar_t**>(argv), static_cast<size_t>(argc));
#elif defined(__APPLE__) || defined(__linux__)
	ExtractorParameters<std::string> parameters;
	const bool loadedParameters = parameters.loadFromCommandLine(const_cast<const char**>(argv), static_cast<size_t>(argc));
#else
	#error Missing implementation.
#endif

	int resultValue = 1;

	if (loadedParameters)
	{
		RandomI::initialize();

		IO::File file(parameters.image360Filename);
		if (!file.exists())
		{
			Log::info() << "Cannot load file <" << file() << ">";
		}
		else
		{
			if (extractImageToDirectory(file, FrameType::FORMAT_RGB24, Numeric::deg2rad(parameters.coverage), parameters.outputWidth, parameters.outputHeight, parameters.overlap, Numeric::deg2rad(parameters.fovX), Numeric::deg2rad(parameters.orientationNoise), Numeric::deg2rad(parameters.imuInaccuracy), IO::Directory(parameters.outputDirectory), parameters.visualizeCameraOutlines))
			{
				resultValue = 0;
			}
		}
	}

#ifdef OCEAN_RUNTIME_STATIC
	OCEAN_APPLY_IF_WINDOWS(Media::WIC::unregisterWICLibrary());
	OCEAN_APPLY_IF_APPLE(Media::ImageIO::unregisterImageIOLibrary());
#else
	PluginManager::get().release();
#endif

	return resultValue;
}
