// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/demo/cv/detector/lineevaluator/LineEvaluatorMain.h"

#include "ocean/base/Messenger.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/Thread.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/OpenCVUtilities.h"

#include "ocean/cv/detector/LineEvaluator.h"

#include "ocean/io/Directory.h"
#include "ocean/io/File.h"

#include "ocean/media/Manager.h"
#include "ocean/media/Utilities.h"

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(__APPLE__)
		#include "ocean/media/imageio/ImageIO.h"
	#elif defined(_WINDOWS)
		#include "ocean/media/wic/WIC.h"
	#else
		#include "ocean/media/openimagelibraries/OpenImageLibraries.h"
	#endif
#endif

#include <featurelib/Ulf.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace Ocean;
using namespace Ocean::CV::Detector;

/**
 * Loads the image corresponding to a given file containing lines.
 * The corresponding image has the same file name but an image extension.
 * @param lineFile The file containing the lines
 * @return The corresponding image, an invalid object if no corresponding image could be loaded
 */
static Frame loadCorrespondingImage(const IO::File& lineFile)
{
	const std::vector<std::string> imageTypes = {"png", "jpg", "bmp"};

	for (const std::string& imageType : imageTypes)
	{
		const IO::File imageFile(lineFile.base() + "." + imageType);

		if (imageFile.exists())
		{
			return Media::Utilities::loadImage(imageFile());
		}
	}

	return Frame();
}

/**
 * Loads lines form a specified file.
 * @param lineFile The file containing several lines
 * @param minimalLineLength The minimal length of lines that will be returned, with range [0, infinity), -1 to accept any line
 * @return The lines that could be loaded from the file
 */
static FiniteLinesD2 loadLines(const IO::File& lineFile, const double minimalLineLength = -1.0)
{
	std::ifstream stream(lineFile().c_str(), std::ios::binary);

	if (stream.is_open() == false)
	{
		return FiniteLinesD2();
	}

	const double minimalSqrLength = minimalLineLength * minimalLineLength;

	FiniteLinesD2 lines;

	while (stream.eof() == false)
	{
		double xPoint0 = Numeric::minValue();
		double yPoint0 = Numeric::minValue();
		double xPoint1 = Numeric::minValue();
		double yPoint1 = Numeric::minValue();

		stream >> xPoint0;

		if (stream.eof())
		{
			break;
		}

		stream >> yPoint0;
		stream >> xPoint1;
		stream >> yPoint1;

		if (stream.bad() || xPoint0 == Numeric::minValue() || yPoint0 == Numeric::minValue() || xPoint1 == Numeric::minValue() || yPoint1 == Numeric::minValue())
		{
			return FiniteLinesD2();
		}

		const FiniteLineD2 line(VectorD2(xPoint0, yPoint0), VectorD2(xPoint1, yPoint1));

		if (minimalLineLength <= 0.0 || line.sqrLength() >= minimalSqrLength)
		{
			lines.push_back(line);
		}
	}

	return lines;
}

/**
 * Draws lines into a given frame.
 * @param frame The frame in which the lines will be drawn, must be valid
 * @param lines The lines to drawn
 * @param color The explicit line color to be used, otherwise nullptr
 * @param darkenImage True, to darken the image before drawing the lines
 */
static void drawLines(Frame& frame, const FiniteLinesD2& lines, const unsigned char* color = nullptr, const bool darkenImage = false)
{
	ocean_assert(frame.isValid());

	if (darkenImage)
	{
		for (unsigned int n = 0u; n < frame.size(); ++n)
		{
			frame.data<uint8_t>()[n] /= 3u;
		}
	}

	for (const FiniteLineD2& line : lines)
	{
		CV::Canvas::line<1u>(frame, Scalar(line.point0().x()), Scalar(line.point0().y()), Scalar(line.point1().x()), Scalar(line.point1().y()), color);
	}
}

/**
 * Detects LSD lines in a given grayscale image.
 * @param yFrame The grascale image with pixel format FORMAT_Y8 and pixel origin in the upper left corner, must be valid
 * @param minimalLineLength The minimal length of lines in pixel, with range [0, infinity)
 * @param parameters Optional parameters, must be nullptr
 * @return The resulting LSD lines
 */
static FiniteLinesD2 detectLinesLSD(const Frame& yFrame, const double minimalLineLength = 10.0, const void* parameters = nullptr)
{
	ocean_assert(yFrame.isValid());
	ocean_assert(yFrame.pixelFormat() == FrameType::FORMAT_Y8);
	ocean_assert(yFrame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	cv::Mat cvImage = CV::OpenCVUtilities::toCvMat(yFrame, true);

	cv::Ptr<cv::LineSegmentDetector> lineSegmentDetector = cv::createLineSegmentDetector(cv::LSD_REFINE_STD);

	std::vector<cv::Vec4f> cvLines;
	lineSegmentDetector->detect(cvImage, cvLines);

	FiniteLinesD2 lines;
	lines.reserve(cvLines.size());

	for (const cv::Vec4f& cvLine : cvLines)
	{
		const FiniteLineD2 line(VectorD2(cvLine[0], cvLine[1]), VectorD2(cvLine[2], cvLine[3]));

		if (line.sqrLength() >= NumericD::sqr(minimalLineLength))
		{
			lines.push_back(line);
		}
	}

	return lines;
}

/**
 * Detects ULF lines in a given grayscale image.
 * @param yFrame The grascale image with pixel format FORMAT_Y8 and pixel origin in the upper left corner, must be valid
 * @param minimalLineLength The minimal length of lines in pixel, with range [0, infinity)
 * @param parameters Optional explicit ULF parameters, nullptr otherwise
 * @return The resulting ULF lines
 */
static FiniteLinesD2 detectLinesULF(const Frame& yFrame, const double minimalLineLength = 10.0, const void* parameters = nullptr)
{
	ulf::ulfParameters ulfParameters = ulf::ulfParameters();

	if (parameters != nullptr)
	{
		ulfParameters = *reinterpret_cast<const ulf::ulfParameters*>(parameters);
	}

	cv::Mat cvImage = CV::OpenCVUtilities::toCvMat(yFrame, true);

	std::vector<cv::Vec4f> cvLines;
	ulf::runULF(cvImage, cvLines, ulfParameters);

	FiniteLinesD2 lines;
	lines.reserve(cvLines.size());

	for (const cv::Vec4f& cvLine : cvLines)
	{
		const FiniteLineD2 line(VectorD2(cvLine[0], cvLine[1]), VectorD2(cvLine[2], cvLine[3]));

		if (line.sqrLength() >= NumericD::sqr(minimalLineLength))
		{
			lines.push_back(line);
		}
	}

	return lines;
}

/**
 * Definition of a function pointer for a detector.
 * First parameter: The grayscale frame in which the lines will be detected
 * Second parameter: Minimal length of lines in pixel, with range [0, infinity)
 * Third parameter: Optional explicit detector parameters, nullptr otherwise
 */
typedef FiniteLinesD2 (*DetectorFunction)(const Frame&, const double, const void*);

/**
 * Evaluates detected lines in relation to ground truth lines.
 * @param lineFile The file providing the ground truth data to evaluate
 * @param detectorFunction The function pointer for the line detector to be used, must be valid
 * @param detectorName The name of the detector, must be valid
 * @param detectorParameters Optional detection parameters, nullptr otherwise
 * @param groundTruthLines The resulting number of ground truth lines, with range [0, infinity)
 * @param detectedLines The resulting number of lines that have been detected, with range [0, infinity)
 * @param coverage The resulting overall coverage, with range [0, 1]
 * @param medianAngle The resulting median angle between detected lines and ground truth lines in radian, with range [0, PI/2]
 * @param medianDistance The resulting median distance between detected lines and ground truth lines in pixel, with range [0, infinity)
 * @param notCoveredLinesGroundTruth The number of ground truth lines for which no corresponding lines could be detected, with range [0, groundTruthLines]
 * @param notCoveredLinesDetector The number of detected lines that do not have a corresponding ground truth line, with range [0, detectedLines]
 * @param createDebugImages True, to create several debug images; False, otherwise
 * @return True, if succeeded
 */
static bool evaluateDetector(const IO::File& lineFile, const DetectorFunction detectorFunction, const std::string& detectorName, const void* const detectorParameters, size_t& groundTruthLines, size_t& detectedLines, double& coverage, double& medianAngle, double& medianDistance, size_t& notCoveredLinesGroundTruth, size_t& notCoveredLinesDetector, const bool createDebugImages = false)
{
	ocean_assert(lineFile.exists());
	ocean_assert(detectorFunction != nullptr);
	ocean_assert(!detectorName.empty());

	const Frame image = loadCorrespondingImage(lineFile);

	Frame yImage;
	if (!image.isValid() || !CV::FrameConverter::Comfort::convert(image, FrameType(image, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT), yImage, false))
	{
		return false;
	}

	// the specified threshold are defined for an image with smal image resoultion e.g, 640x480
	// therefore, we need to adjust the threshold for bigger images

	const double sizeFactor = minmax(1.0, double(std::min(yImage.width(), yImage.height())) / 600.0, 2.0);

	const double perfectMatchAngleThreshold = NumericD::deg2rad(2);
	const double perfectMatchPixelThreshold = 2.0 * sizeFactor;

	const double matchAngleThreshold = NumericD::deg2rad(5);
	const double matchCloseToLinePixelThreshold = 3.0 * sizeFactor;
	const double partialMatchNonOverlappingPixelThreshold = 25.0 * sizeFactor;
	const double complexMatchMaximalGapPixelThreshold = 15.0 * sizeFactor;

	const double minimalLineLength = 10.0 * sizeFactor;

	const FiniteLinesD2 linesGroundTruth = loadLines(lineFile, minimalLineLength);

	std::unordered_map<LineEvaluator::Id, FiniteLineD2> lineMapGroundTruth;
	for (size_t n = 0; n < linesGroundTruth.size(); ++n)
	{
		lineMapGroundTruth.insert(std::make_pair(LineEvaluator::Id(n), linesGroundTruth[n]));
	}

	ocean_assert(linesGroundTruth.size() == lineMapGroundTruth.size());
	groundTruthLines = linesGroundTruth.size();

	const FiniteLinesD2 linesDetector = detectorFunction(yImage, minimalLineLength, detectorParameters);

	if (linesDetector.empty())
	{
		return false;
	}

	detectedLines = linesDetector.size();

	std::unordered_map<LineEvaluator::Id, FiniteLineD2> lineMapDetector;
	for (size_t n = 0; n < linesDetector.size(); ++n)
	{
		lineMapDetector.insert(std::make_pair(LineEvaluator::Id(n), linesDetector[n]));
	}

	ocean_assert(linesDetector.size() == lineMapDetector.size());

	const LineEvaluator::LineMatchMap lineMatchesDetector = LineEvaluator::evaluateLineSegments(lineMapGroundTruth, lineMapDetector, perfectMatchAngleThreshold, perfectMatchPixelThreshold, matchAngleThreshold, matchCloseToLinePixelThreshold, partialMatchNonOverlappingPixelThreshold, complexMatchMaximalGapPixelThreshold);

	if (lineMatchesDetector.empty())
	{
		return false;
	}

	size_t countPerfectMatches;
	size_t countPartialMatches;
	size_t countComplexMatches;

	LineEvaluator::IdSet notCoveredGroundTruthLineIds;
	LineEvaluator::IdSet notCoveredDetectorLineIds;

	if (!LineEvaluator::evaluateLineMatches(lineMapGroundTruth, lineMapDetector, lineMatchesDetector, coverage, medianAngle, medianDistance, countPerfectMatches, countPartialMatches, countComplexMatches, notCoveredLinesGroundTruth, notCoveredLinesDetector, &notCoveredGroundTruthLineIds, &notCoveredDetectorLineIds))
	{
		return false;
	}

	if (createDebugImages)
	{
		Frame rgbImage;
		if (!CV::FrameConverter::Comfort::convert(yImage, FrameType(yImage, FrameType::FORMAT_RGB24), rgbImage, true))
		{
			return false;
		}

		FiniteLinesD2 notCoveredGroundTruthLines;
		notCoveredGroundTruthLines.reserve(notCoveredGroundTruthLineIds.size());
		for (const LineEvaluator::Id& id : notCoveredGroundTruthLineIds)
		{
			notCoveredGroundTruthLines.push_back(lineMapGroundTruth.find(id)->second);
		}

		const unsigned char green[3] = {0x00, 0xFF, 0x00};

		{
			Frame imageGroundTruthLines(rgbImage, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			drawLines(imageGroundTruthLines, linesGroundTruth, green, true);
			Media::Utilities::saveImage(imageGroundTruthLines, lineFile.base() + "-gt.png", false);
		}

		{
			Frame imageDetectorLines(rgbImage, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			drawLines(imageDetectorLines, linesDetector, green, true);
			Media::Utilities::saveImage(imageDetectorLines, lineFile.base() + "-" + detectorName + ".png", false);
		}

		{
			Frame imageNotCoveredGroundTruth(rgbImage, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			drawLines(imageNotCoveredGroundTruth, notCoveredGroundTruthLines, green, true);
			Media::Utilities::saveImage(imageNotCoveredGroundTruth, lineFile.base() + "-"  + detectorName + "-notcovered-gt.png", false);
		}

		{
			FiniteLinesD2 notCoveredDetectorLines;
			notCoveredDetectorLines.reserve(notCoveredDetectorLineIds.size());
			for (const LineEvaluator::Id& id : notCoveredDetectorLineIds)
			{
				notCoveredDetectorLines.push_back(lineMapDetector.find(id)->second);
			}

			Frame imageNotCoveredDetector(rgbImage, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);
			drawLines(imageNotCoveredDetector, notCoveredDetectorLines, green, true);
			Media::Utilities::saveImage(imageNotCoveredDetector, lineFile.base() + "-"  + detectorName + "-notcovered-" + detectorName + ".png", false);
		}
	}

	return true;
}

/**
 * Applies an evaluation for a specified line file.
 * LSD and ULF lines will be detected and compared with the ground truth data
 * @param lineFile The file providing the ground truth data to evaluate
 * @param createDebugImages True, to create several debug images; False, otherwise
 * @return True, if succeeded
 */
static bool evaluateFile(const IO::File& lineFile, const bool createDebugImages)
{
	ocean_assert(lineFile.exists());

	size_t groundTruthLines;

	size_t detectedLinesLSD;
	double coverageLSD;
	double medianAngleLSD;
	double medianDistanceLSD;
	size_t notCoveredLinesLSDGroundTruth;
	size_t notCoveredLinesLSDDetected;
	if (!evaluateDetector(lineFile, detectLinesLSD, "lsd", nullptr, groundTruthLines, detectedLinesLSD, coverageLSD, medianAngleLSD, medianDistanceLSD, notCoveredLinesLSDGroundTruth, notCoveredLinesLSDDetected, createDebugImages))
	{
		return false;
	}

	ulf::ulfParameters parameters;
	parameters.minLength = 9;

	size_t detectedLinesULF;
	double coverageULF;
	double medianAngleULF;
	double medianDistanceULF;
	size_t notCoveredLinesULFGroundTruth;
	size_t notCoveredLinesULFDetected;
	if (!evaluateDetector(lineFile, detectLinesULF, "ulf", &parameters, groundTruthLines, detectedLinesULF, coverageULF, medianAngleULF, medianDistanceULF, notCoveredLinesULFGroundTruth, notCoveredLinesULFDetected, createDebugImages))
	{
		return false;
	}

	std::cout << lineFile.baseName() << "\t\t";
	std::cout << groundTruthLines << "\t\t";

	std::cout << detectedLinesLSD << "\t" << detectedLinesULF << "\t\t";
	std::cout << coverageLSD << "\t" << coverageULF << "\t\t";
	std::cout << NumericD::rad2deg(medianAngleLSD) << "\t" << NumericD::rad2deg(medianAngleULF) << "\t\t";
	std::cout << medianDistanceLSD << "\t" << medianDistanceULF << "\t\t";
	std::cout << notCoveredLinesLSDGroundTruth << "\t" << notCoveredLinesULFGroundTruth << "\t\t";
	std::cout << notCoveredLinesLSDDetected << "\t" << notCoveredLinesULFDetected << std::endl;

	return true;
}

/**
 * Applies a permutation of ULF prameters for one image.
 * @param lineFile The file providing the ground truth data to evaluate
 * @return True, if succeeded
 */
static bool testParameterPermutationsULF(const IO::File& lineFile)
{
	ocean_assert(lineFile.exists());

	/*
	 * ULF's default values
	 * responseScaleBar = 20.0f;          // intensity scaling of bar measure
	 * responseScaleStep = 20.0f / 4.0f;  // intensity scaling of step measure
	 * winSize = 4;          // half-window size for detecting bars / steps
	 * minLength = 20;       // minimum length of detected lines
	 * straightThresh = 1.6f;// max distance from segment connecting endpoints
	 *
	 * oldnew = false;        // old / new version (for experiments)
	 * group = true;          // group lines into segments
	 * filterStraight = true; // only keep straight lines
	 * dirMode = 0;           // 0 - both, 1 - horiz, 2 - vertical
	 * residMAD = false;      // residual is mean abs diff (RMS if false)
	 * refine = true;         // use refined least-squares line fit
	 */

	ulf::ulfParameters parameters;
	parameters.minLength = 9;

	for (int winSize = 2; winSize <= 5; ++winSize)
	{
		parameters.winSize = winSize;

		for (unsigned int residMAD = 0u; residMAD <= 1u; ++residMAD)
		{
			parameters.residMAD = residMAD == 1u;

			for (float responseScaleBar = 16.0f; responseScaleBar <= 24.0f; responseScaleBar += 1.0f)
			{
				parameters.responseScaleBar = responseScaleBar;

				for (float responseScaleStep= 4.0f; responseScaleStep <= 6.0f; responseScaleStep += 0.25f)
				{
					parameters.responseScaleStep = responseScaleStep;

					size_t groundTruthLines;
					size_t detectedLinesULF;
					double coverageULF;
					double medianAngleULF;
					double medianDistanceULF;
					size_t notCoveredLinesULFGroundTruth;
					size_t notCoveredLinesULFDetected;
					if (!evaluateDetector(lineFile, detectLinesULF, "ulf", &parameters, groundTruthLines, detectedLinesULF, coverageULF, medianAngleULF, medianDistanceULF, notCoveredLinesULFGroundTruth, notCoveredLinesULFDetected))
					{
						return false;
					}

					std::cout << lineFile.baseName() << "\t\t";
					std::cout << winSize << "\t";
					std::cout << residMAD << "\t";
					std::cout << responseScaleBar << "\t";
					std::cout << responseScaleStep << "\t\t";

					std::cout << groundTruthLines << "\t";

					std::cout << detectedLinesULF << "\t";
					std::cout << coverageULF << "\t";
					std::cout << NumericD::rad2deg(medianAngleULF) << "\t";
					std::cout << medianDistanceULF << "\t";
					std::cout << notCoveredLinesULFGroundTruth << "\t";
					std::cout << notCoveredLinesULFDetected << std::endl;
				}
			}
		}
	}

	return true;
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
#ifdef _MSC_VER
	// prevent the debugger to abort the application after an assert has been caught
	_set_error_mode(_OUT_TO_MSGBOX);
#endif

	RandomI::initialize();

	// first we have to register the media plugins

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(__APPLE__)
		Media::ImageIO::registerImageIOLibrary();
	#elif defined(_WINDOWS)
		Media::WIC::registerWICLibrary();
	#else
		Media::OpenImageLibraries::registerOpenImageLibrariesLibrary();
	#endif
#endif

	if (argc >= 2)
	{
		const IO::Directory directory(String::trim(String::toAString(argv[1])));

		if (!directory.exists())
		{
			std::cout << "The provided directory \"" << directory() << "\" does not exist." << std::endl;
			return -1;
		}

		bool createDebugImages = false;

		bool parameterPermutationsULF = false;

		if (argc == 3)
		{
			createDebugImages = String::trim(String::toAString(argv[2])) == "debugoutput";

			parameterPermutationsULF = String::trim(String::toAString(argv[2])) == "tweakulf";
		}

		std::cout << std::endl;

		const IO::Files lineFiles = directory.findFiles("lns");

		if (parameterPermutationsULF)
		{
			std::cout << "Permutation of ULF parameters:" << std::endl;

			std::cout << "Dataset\t\t";

			std::cout << "WinSize\t";
			std::cout << "ResidMAD\t";
			std::cout << "ResponseScaleBar\t";
			std::cout << "ResponseScaleStep\t\t";

			std::cout << "Ground truth lines\t";
			std::cout << "Lines\t";
			std::cout << "Overall coverage (%)\t";
			std::cout << "Median angle (deg)\t";
			std::cout << "Median distance (px)\t";
			std::cout << "Unmatched GT\t";
			std::cout << "Unmatched ULF" << std::endl;

			for (const IO::File& lineFile : lineFiles)
			{
				if (!testParameterPermutationsULF(lineFile))
				{
					std::cout << "Error: Could not evaluate \"" << lineFile() << "\"" << std::endl;
				}
			}
		}
		else
		{
			std::cout << "Evaluation results for LSD and ULF line detector:" << std::endl;

			std::cout << "Dataset\t\t";
			std::cout << "Ground truth lines\t\t";

			std::cout << "LSD lines\t" << "ULF lines\t\t";
			std::cout << "LSD overall coverage (%)\t" << "ULF overall coverage (%)\t\t";
			std::cout << "LSD median angle (deg)\t" << "ULF median angle (deg)\t\t";
			std::cout << "LSD median distance (px)\t" << "ULF median distance (px)\t\t";
			std::cout << "LSD unmatched GT\t" << "ULF unmatched GT\t\t";
			std::cout << "LSD unmatched LSD\t" << "ULF unmatched ULF" << std::endl;

			for (const IO::File& lineFile : lineFiles)
			{
				if (!evaluateFile(lineFile, createDebugImages))
				{
					std::cout << "Error: Could not evaluate \"" << lineFile() << "\"" << std::endl;
				}
			}
		}

		std::cout << std::endl << std::endl << std::endl;
	}
	else
	{
		std::cout << "Error: Please provide one directory!" << std::endl;
		return -1;
	}

#ifdef OCEAN_RUNTIME_STATIC
	#if defined(__APPLE__)
		Media::ImageIO::unregisterImageIOLibrary();
	#elif defined(_WINDOWS)
		Media::WIC::unregisterWICLibrary();
	#else
		Media::OpenImageLibraries::unregisterOpenImageLibrariesLibrary();
	#endif
#endif

  return 0;
}
