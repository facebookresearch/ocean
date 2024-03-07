// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/base/HighPerformanceTimer.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverterBGR24.h"
#include "ocean/cv/FrameConverterY8.h"
#include "ocean/cv/FrameInterpolatorNearestPixel.h"
#include "ocean/cv/OpenCVUtilities.h"
#include "ocean/cv/detector/HemiCube.h"

#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Random.h"

#include <featurelib/Ulf.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <array>
#include <iostream>
#include <numeric>

using namespace Ocean;
using CV::Detector::HemiCube;

namespace
{
const std::array<std::array<unsigned char, 4>, 16> colorPaletteBGR = {
	{ { { 0x00, 0xA5, 0xFF, 0xFF } }, // 0. Orange
	  { { 0xFF, 0xFF, 0xFF, 0xFF } }, // 1. White
	  { { 0x00, 0x00, 0xFF, 0xFF } }, // 2. Red
	  { { 0x00, 0xFF, 0x00, 0xFF } }, // 3. Lime
	  { { 0xFF, 0x00, 0x00, 0xFF } }, // 4. Blue
	  { { 0x00, 0xFF, 0xFF, 0xFF } }, // 5. Yellow
	  { { 0xFF, 0xFF, 0x00, 0xFF } }, // 6. Cyan / Aqua
	  { { 0xFF, 0x00, 0xFF, 0xFF } }, // 7. Magenta / Fuchsia
	  { { 0xC0, 0xC0, 0xC0, 0xFF } }, // 8. Silver
	  { { 0x80, 0x80, 0x80, 0xFF } }, // 9. Gray
	  { { 0x00, 0x00, 0x80, 0xFF } }, // 10. Maroon
	  { { 0x00, 0x80, 0x80, 0xFF } }, // 11. Olive
	  { { 0x00, 0x80, 0x00, 0xFF } }, // 12. Green
	  { { 0x80, 0x00, 0x80, 0xFF } }, // 13. Purple
	  { { 0x80, 0x80, 0x00, 0xFF } }, // 14. Teal
	  { { 0x80, 0x00, 0x00, 0xFF } } } // 15. Navy
};

void drawResults(const Frame& inputImage, Frame& resultImage, const FiniteLines2& inputLines, const FiniteLines2& mergedLines, const Indices32& mapping)
{
	ocean_assert(mapping.size() == inputLines.size());

	resultImage = Frame(inputImage, Frame::ACM_COPY_REMOVE_PADDING_LAYOUT);

	auto drawLine = [](Frame& frame, const FiniteLine2& line, const unsigned int colorIndex) {
		CV::Canvas::line<5u>(frame, line, CV::Canvas::black(frame.pixelFormat()));
		CV::Canvas::line<3u>(frame, line, colorPaletteBGR[colorIndex].data());
	};

	for (size_t i = 0; i < inputLines.size(); ++i)
	{
		drawLine(resultImage, inputLines[i], mapping[i] % colorPaletteBGR.size());
	}

	for (size_t i = 0; i < mergedLines.size(); ++i)
	{
		drawLine(resultImage, mergedLines[i], i % colorPaletteBGR.size());
	}
}

void runTestImage(const unsigned int hemiCubeBins, const Ocean::Scalar focalLength, const std::vector<std::string>& filenames)
{
	if (filenames.empty())
	{
		return;
	}

	auto extractLines = [](Frame& imageY8) -> FiniteLines2 {
		const cv::Mat cvImageY8 = CV::OpenCVUtilities::toCvMat(imageY8, false);
		std::vector<cv::Vec4f> cvLines;
		ulf::runULF(cvImageY8, cvLines);

		FiniteLines2 lines;
		lines.reserve(cvLines.size());

		for (const auto& cvLine : cvLines)
		{
			lines.emplace_back(Vector2(Scalar(cvLine[0]), Scalar(cvLine[1])), Vector2(Scalar(cvLine[2]), Scalar(cvLine[3])));
		}

		return lines;
	};

	bool keepRunning = true;
	unsigned int hemiCubeSearchRadius = 1u;
	unsigned int currentHemiCubeBins = hemiCubeBins;
	const unsigned int hemiCubeBinsStep = 5u;
	Scalar maxLineDistance = Scalar(0);
	Scalar maxGapDistance = Scalar(0);
	bool useBruteForceMerge = true;
	unsigned int filenameIndex = 0u;
	bool loadImage = true;
	bool recomputeHemiCube = true;
	bool reextractLines = true;

	HemiCube hemiCube;
	Indices32 mappingLinesToMergedLines;
	FiniteLines2 lines;
	FiniteLines2 mergedLines;
	Frame hemiCubeImage;
	std::vector<int> linesToMergedLines;

	cv::Mat cvImageBGR;
	Frame imageBGR;
	Frame imageY8;

	while (keepRunning)
	{
		if (loadImage && filenameIndex < filenames.size())
		{
			cvImageBGR = cv::imread(filenames[filenameIndex], cv::IMREAD_COLOR);

			if (cvImageBGR.empty())
			{
				std::cerr << "ERROR: Failed to load image <" << filenames[filenameIndex] << "> ... aborting!" << std::endl;
				return;
			}

			imageBGR = CV::OpenCVUtilities::toOceanFrame(cvImageBGR, false, FrameType::FORMAT_BGR24);
			imageY8 = Frame(FrameType(imageBGR.frameType(), FrameType::FORMAT_Y8));
			CV::FrameConverterBGR24::convertBGR24ToY8(imageBGR.constdata<uint8_t>(), imageY8.data<uint8_t>(), imageBGR.width(), imageBGR.height(), CV::FrameConverterBGR24::CONVERT_NORMAL, 0u, imageY8.paddingElements());

			loadImage = false;
			reextractLines = true;
			recomputeHemiCube = true;
		}

		if (reextractLines)
		{
			lines = extractLines(imageY8);
			reextractLines = false;
			recomputeHemiCube = true;
		}

		if (recomputeHemiCube)
		{
			std::cout << "-- Recomputation of the Hemi Cube --" << std::endl;
			std::cout << "Hemi Cube bins: " << currentHemiCubeBins << std::endl;
			std::cout << "Hemi Cube bin search radius: " << hemiCubeSearchRadius << std::endl;
			std::cout << "Collinearity epsilon: " << maxLineDistance << std::endl;
			std::cout << "Max. line to line distance: " << maxGapDistance << std::endl;

			HighPerformanceTimer timer;
			timer.start();
			hemiCube = HemiCube(currentHemiCubeBins, imageBGR.width(), imageBGR.height(), focalLength);
			hemiCube.merge(lines, maxLineDistance, maxGapDistance, &mappingLinesToMergedLines);
			const double mergeTimeMilliseconds = timer.mseconds();

			mergedLines = hemiCube.lines();

			auto lambdaLineLength = [](const Scalar& value, const FiniteLine2& line) { return value + line.length(); };

			const Scalar linesAverageLength = (lines.size() == 0 ? Scalar(0) : (std::accumulate(lines.begin(), lines.end(), Scalar(0), lambdaLineLength) / Scalar(lines.size())));
			std::cout << "Input lines: " << lines.size() << ", average length: " << linesAverageLength << std::endl;

			const Scalar mergedLinesAverageLength = (mergedLines.size() == 0 ? Scalar(0) : (std::accumulate(mergedLines.begin(), mergedLines.end(), Scalar(0), lambdaLineLength) / Scalar(mergedLines.size())));
			std::cout << "Merged lines: " << mergedLines.size() << ", average length: " << mergedLinesAverageLength << std::endl;
			std::cout << "Merged " << (lines.size() - mergedLines.size()) << " lines" << std::endl;
			std::cout << "Time to merge: " << String::toAString(mergeTimeMilliseconds, 3u) << " milliseconds" << std::endl;

			HighPerformanceTimer timerBruteForce;
			timerBruteForce.start();
			const FiniteLines2 mergedLines2 = HemiCube::mergeGreedyBruteForce(lines, maxLineDistance, maxGapDistance);
			const double mergeTimeBruteForceMilliseconds = timerBruteForce.mseconds();
			std::cout << "Time to merge (brute-force): " << String::toAString(mergeTimeBruteForceMilliseconds, 3u) << " milliseconds (factor: " << String::toAString((mergeTimeBruteForceMilliseconds / mergeTimeMilliseconds), 3u) << "x)" << std::endl;
			std::cout << "Merged (brute-force): " << (lines.size() - mergedLines2.size()) << " lines" << std::endl;

			if (useBruteForceMerge)
			{
				mergedLines = std::move(mergedLines2);
			}

			std::cout << "Brute-force merge: " << std::boolalpha << useBruteForceMerge << std::endl;

			std::cout << std::endl;

			recomputeHemiCube = false;
		}

		Frame inputImage(imageBGR.frameType());
		CV::FrameConverterY8::convertY8ToBGR24(imageY8.constdata<uint8_t>(), inputImage.data<uint8_t>(), imageY8.width(), imageY8.height(), CV::FrameConverterY8::CONVERT_NORMAL, imageY8.paddingElements(), inputImage.paddingElements());
		ocean_assert(inputImage.isValid());

		Frame resultImage(imageBGR.frameType());
		CV::FrameConverterY8::convertY8ToBGR24(imageY8.constdata<uint8_t>(), resultImage.data<uint8_t>(), imageY8.width(), imageY8.height(), CV::FrameConverterY8::CONVERT_NORMAL, imageY8.paddingElements(), resultImage.paddingElements());
		ocean_assert(resultImage.isValid());
		drawResults(inputImage, resultImage, lines, mergedLines, mappingLinesToMergedLines);

		cv::imshow("Input image", CV::OpenCVUtilities::toCvMat(inputImage, false));
		cv::imshow("Result image", CV::OpenCVUtilities::toCvMat(resultImage, false));
		const int keyPressed = cv::waitKey();

		switch (keyPressed)
		{
			case 63234: // Left arrow
				if (filenameIndex == 0u)
				{
					std::cerr << "Reached first image ..." << std::endl;
				}
				else
				{
					filenameIndex -= 1u;
					loadImage = true;
				}

				break;

			case 63235: // Right arrow
				if (filenameIndex >= filenames.size())
				{
					std::cerr << "Reached last image ..." << std::endl;
				}
				else
				{
					filenameIndex += 1u;
					loadImage = true;
				}

				break;

			case ';':
				maxLineDistance = std::max(Scalar(0), maxLineDistance - Scalar(1));
				recomputeHemiCube = true;
				break;

			case '\'':
				maxLineDistance += Scalar(1);
				recomputeHemiCube = true;
				break;

			case ',':
				maxGapDistance = std::max(Scalar(0), maxGapDistance - Scalar(5));
				recomputeHemiCube = true;
				break;

			case '.':
				maxGapDistance += Scalar(5);
				recomputeHemiCube = true;
				break;

			case '[':
				hemiCubeSearchRadius -= (hemiCubeSearchRadius > 1u ? 1u : 0u);
				recomputeHemiCube = true;
				break;

			case ']':
				hemiCubeSearchRadius += (hemiCubeSearchRadius < hemiCubeBins ? 1u : 0u);
				recomputeHemiCube = true;
				break;

			case '{':
			{
				const unsigned int step = (currentHemiCubeBins > 15u ? hemiCubeBinsStep : 1u);
				currentHemiCubeBins = (currentHemiCubeBins > step ? (currentHemiCubeBins - step) : currentHemiCubeBins);
				recomputeHemiCube = true;
				break;
			}

			case '}':
				currentHemiCubeBins += (currentHemiCubeBins < hemiCubeBinsStep ? 1u : hemiCubeBinsStep);
				recomputeHemiCube = true;
				break;

			case ' ': // Reset
				currentHemiCubeBins = hemiCubeBins;
				recomputeHemiCube = true;
				hemiCubeSearchRadius = 1u;
				break;

			case 'b': // Toggle: brute-force search vs. Hemi cube search
				useBruteForceMerge = !useBruteForceMerge;
				recomputeHemiCube = true;
				break;

			case 's':
			{
				ocean_assert(inputImage.frameType() == resultImage.frameType());
				ocean_assert(inputImage.isValid());
				ocean_assert(inputImage.channels() == 3u);
				cv::Mat image(inputImage.height(), 2u * inputImage.width(), CV_8UC3);

				CV::OpenCVUtilities::toCvMat(inputImage, false).copyTo(image(cv::Rect(0u, 0u, inputImage.width(), inputImage.height())));
				CV::OpenCVUtilities::toCvMat(resultImage, false).copyTo(image(cv::Rect(inputImage.width(), 0u, resultImage.width(), resultImage.height())));

				const std::string resultImageFilename = "/tmp/lines_and_planes_result.png";
				cv::imwrite(resultImageFilename, image);
				std::cout << "Saved images to:" << std::endl;
				std::cout << "  " << resultImageFilename << std::endl;
				break;
			}

			case 27: // ESC
			case 'q':
			case 'Q':
				std::cout << "Quitting ..." << std::endl;
				keepRunning = false;
				break;

			default:
				std::cout << "Key: " << keyPressed << std::endl;
				break;
		}
	}
}

} // namespace

int main(int argc, char** argv)
{
	const unsigned int bins = 10u;
	const Scalar focalLength = Scalar(1.7);

	const std::vector<std::string> filenames(argv + 1, argv + (size_t)argc);

	if (argc > 1)
	{
		runTestImage(bins, focalLength, filenames);
	}

	return 0;
}
