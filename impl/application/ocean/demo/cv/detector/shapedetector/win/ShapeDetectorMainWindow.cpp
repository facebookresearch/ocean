/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/detector/shapedetector/win/ShapeDetectorMainWindow.h"

#include "ocean/base/WorkerPool.h"

#include "ocean/cv/Canvas.h"
#include "ocean/cv/FrameConverter.h"
#include "ocean/cv/FrameConverterColorMap.h"
#include "ocean/cv/FrameInterpolatorBilinear.h"
#include "ocean/cv/FrameNormalizer.h"
#include "ocean/cv/FrameTransposer.h"

#include "ocean/cv/detector/Utilities.h"

#include "ocean/media/Utilities.h"

#include "ocean/platform/win/Keyboard.h"
#include "ocean/platform/win/Utilities.h"

ShapeDetectorMainWindow::GradientBasedParameters::GradientBasedParameters()
{
	detectorResponseFactor_ = 1.0;
	detectorThreshold_ = 0.05;

	shapeSign_ = -1;
	detectorPenaltyFactor_ = 3.0;

	detectorEdgeResponseStrategy_ = GradientBasedDetector::ERS_GRADIENT_TO_CENTER;
	detectorMinimalResponseStrategy_ = GradientBasedDetector::MRS_SEPARATE_HORIZONTAL_VERTICAL;
	detectorPenaltyUsage_ = GradientBasedDetector::PU_SUBTRACT;
}

bool ShapeDetectorMainWindow::GradientBasedParameters::onKeyDown(const std::string& keyString)
{
	if (keyString == "=")
	{
		detectorResponseFactor_ *= 10.0;
	}
	else if (keyString == "-")
	{
		detectorResponseFactor_ = std::max(1.0, detectorResponseFactor_ * 0.1);
	}

	else if (keyString == "]")
	{
		detectorThreshold_ *= 1.25;
	}
	else if (keyString == "[")
	{
		detectorThreshold_ = std::max(0.0, detectorThreshold_ / 1.25);
	}

	if (keyString == "Z")
	{
		shapeSign_ = shapeSign_ < 0 ? 1 : -1;
	}

	else if (keyString == "M")
	{
		detectorPenaltyFactor_ *= 1.25;
	}
	else if (keyString == "N")
	{
		detectorPenaltyFactor_ = std::max(0.0, detectorPenaltyFactor_ / 1.25);
	}

	else if (keyString == "X")
	{
		detectorEdgeResponseStrategy_ = GradientBasedDetector::EdgeResponseStrategy((detectorEdgeResponseStrategy_ + 1u) % 2u);
	}
	else if (keyString == "C")
	{
		detectorMinimalResponseStrategy_ = GradientBasedDetector::MinimalResponseStrategy((detectorMinimalResponseStrategy_ + 1u) % 3u);
	}
	else if (keyString == "V")
	{
		detectorPenaltyUsage_ = GradientBasedDetector::PenaltyUsage((detectorPenaltyUsage_ + 1u) % 2u);
	}
	else
	{
		return false;
	}

	return true;
}

std::string ShapeDetectorMainWindow::GradientBasedParameters::translateEdgeResponseStrategy(const GradientBasedDetector::EdgeResponseStrategy edgeResponseStrategy)
{
	switch (edgeResponseStrategy)
	{
		case GradientBasedDetector::ERS_GRADIENT_TO_CENTER:
			return "Gradient to center";

		case GradientBasedDetector::ERS_GRADIENT_TO_NEIGHBOR:
			return "Gradient to neibhbor";
	}

	ocean_assert(false && "Invalid strategy!");
	return "Invalid";
}

std::string ShapeDetectorMainWindow::GradientBasedParameters::translateMinimalResponseStrategy(const GradientBasedDetector::MinimalResponseStrategy minimalResponseStrategy)
{
	switch (minimalResponseStrategy)
	{
		case GradientBasedDetector::MRS_IGNORE:
			return "Ignore";

		case GradientBasedDetector::MRS_SEPARATE_HORIZONTAL_VERTICAL:
			return "Separate horizontal/vertical";

		case GradientBasedDetector::MRS_SEPARATE_OPPOSITE_SIDE:
			return "Separate opposite side";
	}

	ocean_assert(false && "Invalid strategy!");
	return "Invalid";
}

std::string ShapeDetectorMainWindow::GradientBasedParameters::translatePenaltyUsage(const GradientBasedDetector::PenaltyUsage penaltyUsage)
{
	switch (penaltyUsage)
	{
		case GradientBasedDetector::PU_DIVISION:
			return "Division";

		case GradientBasedDetector::PU_SUBTRACT:
			return "Subtract";
	}

	ocean_assert(false && "Invalid strategy!");
	return "Invalid";
}

ShapeDetectorMainWindow::VarianceBasedParameters::VarianceBasedParameters()
{
	detectorResponseFactor_ = 1.0;
	detectorThreshold_ = 4.5;

	detectorMinimalVariance_ = 1.0;
	detectorMaximalRatio_ = 4.0;
	detectorForegroundVarianceFactor_ = 1.0;

	detectorThresholdStrategy_ = VarianceBasedDetector::TS_VALUE;

	detectorGradientResponseStrategy_ = VarianceBasedDetector::GRS_SUM_ABSOLUTE_DIFFERENCES;
	detectorBandStrategy_ = VarianceBasedDetector::BS_JOINED;
}

bool ShapeDetectorMainWindow::VarianceBasedParameters::onKeyDown(const std::string& keyString)
{
	if (keyString == "=")
	{
		detectorResponseFactor_ *= 10.0;
	}
	else if (keyString == "-")
	{
		detectorResponseFactor_ = std::max(1.0, detectorResponseFactor_ * 0.1);
	}

	else if (keyString == "]")
	{
		detectorThreshold_ *= 1.25;
	}
	else if (keyString == "[")
	{
		detectorThreshold_ = std::max(0.0, detectorThreshold_ / 1.25);
	}

	if (keyString == "M")
	{
		detectorMinimalVariance_ *= 1.25;
	}
	else if (keyString == "N")
	{
		detectorMinimalVariance_ = std::max(1.0, detectorMinimalVariance_ / 1.25);
	}

	if (keyString == "K")
	{
		detectorMaximalRatio_ *= 1.05;
	}
	else if (keyString == "J")
	{
		detectorMaximalRatio_ = std::max(1.0, detectorMaximalRatio_ / 1.05);
	}

	if (keyString == "Y")
	{
		detectorForegroundVarianceFactor_ *= 1.05;
	}
	else if (keyString == "H")
	{
		detectorForegroundVarianceFactor_ = std::max(0.1, detectorForegroundVarianceFactor_ / 1.05);
	}

	else if (keyString == "X")
	{
		detectorGradientResponseStrategy_ = VarianceBasedDetector::GradientResponseStrategy((detectorGradientResponseStrategy_ + 1u) % 2u);
	}

	else if (keyString == "C")
	{
		detectorBandStrategy_ = VarianceBasedDetector::BandStrategy((detectorBandStrategy_ + 1u) % 4u);
	}
	else if (keyString == "V")
	{
		detectorThresholdStrategy_ = VarianceBasedDetector::ThresholdStrategy((detectorThresholdStrategy_ + 1u) % 3u);
	}
	else
	{
		return false;
	}

	return true;
}

std::string ShapeDetectorMainWindow::VarianceBasedParameters::translateThresholdStrategy(const VarianceBasedDetector::ThresholdStrategy detectorThresholdStrategy)
{
	switch (detectorThresholdStrategy)
	{
		case VarianceBasedDetector::TS_VALUE:
			return "Value";

		case VarianceBasedDetector::TS_BASED_ON_TOP_100_65:
			return "65% of top 100 shapes";

		case VarianceBasedDetector::TS_BASED_ON_TOP_75_55:
			return "55% of top 75 shapes";
	}

	ocean_assert(false && "Invalid strategy!");
	return "Invalid";
}

std::string ShapeDetectorMainWindow::VarianceBasedParameters::translateGradientResponseStrategy(const VarianceBasedDetector::GradientResponseStrategy foregroundResponseStrategy)
{
	switch (foregroundResponseStrategy)
	{
		case VarianceBasedDetector::GRS_MAX_ABSOLUTE_DIFFERENCE:
			return "Max absolute difference";

		case VarianceBasedDetector::GRS_SUM_ABSOLUTE_DIFFERENCES:
			return "Sum absolute differences";
	}

	ocean_assert(false && "Invalid strategy!");
	return "Invalid";
}

std::string ShapeDetectorMainWindow::VarianceBasedParameters::translateBandStrategy(const VarianceBasedDetector::BandStrategy bandStrategy)
{
	switch (bandStrategy)
	{
		case VarianceBasedDetector::BS_JOINED:
			return "Joined";

		case VarianceBasedDetector::BS_SEPARATE_AVERAGE:
			return "Separate averaged";

		case VarianceBasedDetector::BS_SEPARATE_MAX:
			return "Separate max";

		case VarianceBasedDetector::BS_SKIP:
			return "Skip";
	}

	ocean_assert(false && "Invalid strategy!");
	return "Invalid";
}

ShapeDetectorMainWindow::GradientVarianceBasedParameters::GradientVarianceBasedParameters()
{
	detectorResponseFactor_ = 1.0;
	detectorThreshold_ = 4.5;
	shapeSign_ = -1;

	detectorMaximalRatio_ = 4.0;

	detectorBandStrategy_ = GradientVarianceBasedDetector::BS_SUBTRACT_AND_DIVIDE;
	detectorOptimizationStrategy_ = GradientVarianceBasedDetector::OS_NONE;
}

bool ShapeDetectorMainWindow::GradientVarianceBasedParameters::onKeyDown(const std::string& keyString)
{
	if (keyString == "=")
	{
		detectorResponseFactor_ *= 10.0;
	}
	else if (keyString == "-")
	{
		detectorResponseFactor_ = std::max(0.01, detectorResponseFactor_ * 0.1);
	}

	else if (keyString == "]")
	{
		detectorThreshold_ *= 1.25;
	}
	else if (keyString == "[")
	{
		detectorThreshold_ = std::max(0.0, detectorThreshold_ / 1.25);
	}

	if (keyString == "Z")
	{
		shapeSign_ = ((shapeSign_ + 2) % 3) - 1;
	}

	if (keyString == "K")
	{
		detectorMaximalRatio_ *= 1.05;
	}
	else if (keyString == "J")
	{
		detectorMaximalRatio_ = std::max(1.0, detectorMaximalRatio_ / 1.05);
	}

	else if (keyString == "C")
	{
		detectorBandStrategy_ = GradientVarianceBasedDetector::BandStrategy((detectorBandStrategy_ + 1u) % GradientVarianceBasedDetector::BS_END);
	}
	else if (keyString == "V")
	{
		detectorOptimizationStrategy_ = GradientVarianceBasedDetector::OptimizationStrategy((detectorOptimizationStrategy_ + 1u) % GradientVarianceBasedDetector::OS_END);
	}
	else
	{
		return false;
	}

	return true;
}

std::string ShapeDetectorMainWindow::GradientVarianceBasedParameters::translateBandStrategy(const GradientVarianceBasedDetector::BandStrategy bandStrategy)
{
	switch (bandStrategy)
	{
		case GradientVarianceBasedDetector::BS_SKIP:
			return "Skip";

		case GradientVarianceBasedDetector::BS_DIVIDE:
			return "Divide";

		case GradientVarianceBasedDetector::BS_SUBTRACT_AND_DIVIDE:
			return "Subtract and divide";

		case GradientVarianceBasedDetector::BS_END:
			break;
	}

	ocean_assert(false && "Invalid strategy!");
	return "Invalid";
}

std::string ShapeDetectorMainWindow::GradientVarianceBasedParameters::translateOptimizationStrategy(const GradientVarianceBasedDetector::OptimizationStrategy optimizationStrategy)
{
	switch (optimizationStrategy)
	{
		case GradientVarianceBasedDetector::OS_NONE:
			return "None";

		case GradientVarianceBasedDetector::OS_SYMMETRIC_RESPONSES_FOUR_HORIZONTAL_SAME_VERTICAL:
			return "Symmetric responses, four horizontal, same vertical";

		case GradientVarianceBasedDetector::OS_SYMMETRIC_RESPONSES_FOUR_HORIZONTAL_DIFFERENT_VERTICAL:
			return "Symmetric responses, four horizontal, different vertical";

		case GradientVarianceBasedDetector::OS_SYMMETRIC_RESPONSES_TWO_HORIZONTAL_SAME_VERTICAL:
			return "Symmetric responses, two horizontal, same vertical";

		case GradientVarianceBasedDetector::OS_SYMMETRIC_RESPONSES_TWO_HORIZONTAL_DIFFERENT_VERTICAL:
			return "Symmetric responses, two horizontal, different vertical";

		case GradientVarianceBasedDetector::OS_END:
			break;
	}

	ocean_assert(false && "Invalid strategy!");
	return "Invalid";
}

ShapeDetectorMainWindow::ModifiedGradientVarianceBasedParameters::ModifiedGradientVarianceBasedParameters()
{
	detectorResponseFactor_ = 1.0;
	detectorThreshold_ = 4.5;
	shapeSign_ = -1;

	detectorMaximalRatio_ = 4.0;

	detectorBandStrategy_ = GradientVarianceBasedDetector::BS_SUBTRACT_AND_DIVIDE;
}

bool ShapeDetectorMainWindow::ModifiedGradientVarianceBasedParameters::onKeyDown(const std::string& keyString)
{
	if (keyString == "=")
	{
		detectorResponseFactor_ *= 10.0;
	}
	else if (keyString == "-")
	{
		detectorResponseFactor_ = std::max(0.01, detectorResponseFactor_ * 0.1);
	}

	else if (keyString == "]")
	{
		detectorThreshold_ *= 1.25;
	}
	else if (keyString == "[")
	{
		detectorThreshold_ = std::max(0.0, detectorThreshold_ / 1.25);
	}

	if (keyString == "Z")
	{
		shapeSign_ = ((shapeSign_ + 2) % 3) - 1;
	}

	if (keyString == "K")
	{
		detectorMaximalRatio_ *= 1.05;
	}
	else if (keyString == "J")
	{
		detectorMaximalRatio_ = std::max(1.0, detectorMaximalRatio_ / 1.05);
	}

	else if (keyString == "C")
	{
		detectorBandStrategy_ = GradientVarianceBasedDetector::BandStrategy((detectorBandStrategy_ + 1u) % GradientVarianceBasedDetector::BS_END);
	}
	else
	{
		return false;
	}

	return true;
}

std::string ShapeDetectorMainWindow::ModifiedGradientVarianceBasedParameters::translateBandStrategy(const GradientVarianceBasedDetector::BandStrategy bandStrategy)
{
	switch (bandStrategy)
	{
		case GradientVarianceBasedDetector::BS_SKIP:
			return "Skip";

		case GradientVarianceBasedDetector::BS_DIVIDE:
			return "Divide";

		case GradientVarianceBasedDetector::BS_SUBTRACT_AND_DIVIDE:
			return "Subtract and divide";

		case GradientVarianceBasedDetector::BS_END:
			break;
	}

	ocean_assert(false && "Invalid strategy!");
	return "Invalid";
}

ShapeDetectorMainWindow::ShapeDetectorMainWindow(HINSTANCE instance, const std::wstring& name, const std::vector<std::string>& filenames) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name),
	currentInputIndex_(0),
	shapeDetector_(SD_GRADIENT_VARIANCE_BASED),
	windowResponsesTopDown_(instance, L"Top-down responses"),
	windowResponsesBottomUp_(instance, L"Bottom-up responses"),
	windowParameters_(instance, L"Parameters")
{
	inputFiles_.reserve(filenames.size());

	for (const std::string& filename : filenames)
	{
		IO::File file(filename);

		if (file.exists())
		{
			inputFiles_.emplace_back(std::move(file));
		}
	}

	resetParameters();
}

ShapeDetectorMainWindow::~ShapeDetectorMainWindow()
{
	// nothing to do here
}

void ShapeDetectorMainWindow::onInitialized()
{
	windowResponsesTopDown_.setParent(handle());
	windowResponsesTopDown_.initialize();
	windowResponsesTopDown_.show();

	windowResponsesBottomUp_.setParent(handle());
	windowResponsesBottomUp_.initialize();
	windowResponsesBottomUp_.show();

	windowParameters_.setParent(handle());
	windowParameters_.initialize();
	windowParameters_.show();

	setEnableDropAndDrop(true);

	if (currentInputIndex_ < inputFiles_.size())
	{
		if (updateImage(inputFiles_[currentInputIndex_]()))
		{
			invokeShapeDetector();
		}
	}
}

void ShapeDetectorMainWindow::onKeyDown(const int key)
{
	std::string keyString;

	if (!Platform::Win::Keyboard::translateVirtualkey(key, keyString))
	{
		return;
	}

	if (keyString == "2")
	{
		shapeDetector_ = ShapeDetector((shapeDetector_ + 1u) % SD_END);
	}
	else if (keyString == "1")
	{
		shapeDetector_ = (shapeDetector_ > 0u) ? ShapeDetector(shapeDetector_ - 1u) : ShapeDetector(SD_END - 1u);
	}

	if (keyString == "8")
	{
		showResponses_ = !showResponses_;
	}

	if (!inputFiles_.empty())
	{
		const size_t previousInputIndex = currentInputIndex_;

		if (keyString == "left")
		{
			if (currentInputIndex_ == 0)
			{
				currentInputIndex_ = inputFiles_.size() - 1;
			}
			else
			{
				--currentInputIndex_;
			}
		}
		else if (keyString == "right")
		{
			currentInputIndex_ = (currentInputIndex_ + 1) % inputFiles_.size();
		}

		if (previousInputIndex != currentInputIndex_)
		{
			ocean_assert(currentInputIndex_ < inputFiles_.size());
			updateImage(inputFiles_[currentInputIndex_]());
		}
	}

	if (keyString == "up")
	{
		visibilityLinearGain_ *= 1.1;
	}
	else if (keyString == "down")
	{
		if (visibilityLinearGain_ > 0.05)
		{
			visibilityLinearGain_ /= 1.1;
		}
	}
	if (keyString == "6")
	{
		rotation_ += Numeric::deg2rad(1);
	}
	else if (keyString == "5")
	{
		rotation_ -= Numeric::deg2rad(1);
	}
	else if (keyString == "0")
	{
		resetParameters();
	}

	else if (keyString == "Q")
	{
		shapeWidth_ += 2u;
	}
	else if (keyString == "A")
	{
		shapeWidth_ = std::max(shapeBottomBand_ * 2u + shapeStepSize_, shapeWidth_ - 2u);
	}
	else if (keyString == "W")
	{
		++shapeHeight_;
	}
	else if (keyString == "S")
	{
		shapeHeight_ = std::max(shapeStepSize_ + shapeBottomBand_, shapeHeight_ - 1u);
	}
	else if (keyString == "E")
	{
		if (shapeStepSize_ + 2u + shapeBottomBand_ * 2 <= shapeWidth_)
		{
			shapeStepSize_ += 2u;
		}
	}
	else if (keyString == "D")
	{
		if (shapeStepSize_ >= 3u)
		{
			shapeStepSize_ -= 2u;
		}
	}
	else if (keyString == "R")
	{
		++shapeTopBand_;
	}
	else if (keyString == "F")
	{
		shapeTopBand_ = std::max(1u, shapeTopBand_ - 1u);
	}
	else if (keyString == "T")
	{
		if ((shapeBottomBand_ + 1u) * 2u + shapeStepSize_ <= shapeWidth_)
		{
			++shapeBottomBand_;
		}
	}
	else if (keyString == "G")
	{
		shapeBottomBand_ = std::max(1u, shapeBottomBand_ - 1u);
	}

	else if (keyString == "'")
	{
		++detectorNonMaximumSuppressionRadius_;
	}
	else if (keyString == ";")
	{
		detectorNonMaximumSuppressionRadius_ = std::max(1.0, detectorNonMaximumSuppressionRadius_ - 1.0);
	}

	else if (keyString == "7")
	{
		transposed_ = !transposed_;
	}

	else if (keyString == "/")
	{
		detectorMinimalEdgeResponse_ += 0.1;
	}
	else if (keyString == ".")
	{
		detectorMinimalEdgeResponse_ = std::max(0.0, detectorMinimalEdgeResponse_ - 0.1);
	}

	else if (keyString == "3")
	{
		responseVisualization_ = ResponseVisualization((responseVisualization_ + 1u) % 2u);
	}

	else if (keyString == "4")
	{
		responseType_ = ResponseType((responseType_ + 1u) % 3u);
	}

	if (shapeDetector_ == SD_GRADIENT_BASED)
	{
		gradientBasedParameters_.onKeyDown(keyString);
	}

	if (shapeDetector_ == SD_VARIANCE_BASED)
	{
		varianceBasedParameters_.onKeyDown(keyString);
	}

	if (shapeDetector_ == SD_GRADIENT_VARIANCE_BASED)
	{
		gradientVarianceBasedParameters_.onKeyDown(keyString);
	}

	if (shapeDetector_ == SD_MODIFIED_GRADIENT_VARIANCE_BASED)
	{
		modifiedGradientVarianceBasedParameters_.onKeyDown(keyString);
	}

	invokeShapeDetector();
}

void ShapeDetectorMainWindow::onDragAndDrop(const Files& files)
{
	if (!files.empty())
	{
		inputFiles_.clear();
		inputFiles_.reserve(files.size());

		for (const std::wstring& filename : files)
		{
			IO::File file(String::toAString(filename));

			if (file.exists())
			{
				inputFiles_.emplace_back(std::move(file));
			}
		}

		currentInputIndex_ = 0;

		if( currentInputIndex_ < inputFiles_.size())
		{
			if (updateImage(inputFiles_[currentInputIndex_]()))
			{
				invokeShapeDetector();
			}
		}
	}
}

void ShapeDetectorMainWindow::resetParameters()
{
	visibilityLinearGain_ = 1.0;
	rotation_ = 0.0;
	showResponses_ = true;

	shapeWidth_ = 15u;
	shapeHeight_ = 15u;
	shapeStepSize_ = 3u;
	shapeTopBand_ = 3u;
	shapeBottomBand_ = 4u;

	detectorMinimalEdgeResponse_ = 0.5;
	detectorNonMaximumSuppressionRadius_ = 9.0;

	transposed_ = false;

	responseType_ = RT_HORIZONTAL_AND_VERTICAL;
	responseVisualization_ = RV_LINEAR;

	gradientBasedParameters_ = GradientBasedParameters();
	varianceBasedParameters_ = VarianceBasedParameters();
	gradientVarianceBasedParameters_ = GradientVarianceBasedParameters();
	modifiedGradientVarianceBasedParameters_ = ModifiedGradientVarianceBasedParameters();
}

bool ShapeDetectorMainWindow::updateImage(const std::string& filename)
{
	ocean_assert(!filename.empty());

	const Frame image = Media::Utilities::loadImage(filename);

	return CV::FrameConverter::Comfort::convert(image, FrameType::FORMAT_Y8, yImage_, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()());
}

void ShapeDetectorMainWindow::invokeShapeDetector()
{
	if (!yImage_.isValid())
	{
		return;
	}

	Frame yImageRotated = Frame(yImage_, Frame::ACM_USE_KEEP_LAYOUT);

	if (NumericD::isNotEqualEps(rotation_))
	{
		yImageRotated.set(yImage_.frameType(), true, true);
		CV::FrameInterpolatorBilinear::Comfort::rotate(yImage_, yImageRotated, Scalar(yImageRotated.width()) * Scalar(0.5), Scalar(yImageRotated.height()) * Scalar(0.5), Scalar(rotation_), WorkerPool::get().scopedWorker()());
	}

	Frame yFrameTransposed = Frame(yImageRotated, Frame::ACM_USE_KEEP_LAYOUT);

	if (transposed_)
	{
		CV::FrameTransposer::transpose(yFrameTransposed, WorkerPool::get().scopedWorker()());
	}

	Frame yParameters(FrameType(540u, 560u, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	yParameters.setValue(0xFFu);

	const uint8_t* const black = CV::Canvas::black(yParameters.pixelFormat());

	int yPosition = 5;

	CV::Canvas::drawText(yParameters, "Detector (1 / 2): " + translateShapeDetector(shapeDetector_), 5, yPosition, black);

	CV::Canvas::drawText(yParameters, "Switch images (left / right): " + String::toAString(currentInputIndex_), 5, yPosition += 30, black);
	CV::Canvas::drawText(yParameters, "Gain for visibility (down / up): " + String::toAString(visibilityLinearGain_, 2u), 5, yPosition += 20, black);
	CV::Canvas::drawText(yParameters, "Rotation (5 / 6): " + String::toAString(Numeric::rad2deg(Scalar(rotation_)), 1u), 5, yPosition += 20, black);
	CV::Canvas::drawText(yParameters, std::string("Show responses (8): ") + (showResponses_ ? std::string("true") : std::string("false")), 5, yPosition += 20, black);
	CV::Canvas::drawText(yParameters, "Reset (0)", 5, yPosition += 20, black);

	LShapes lShapes;
	TShapes tShapes;
	XShapes xShapes;

	Frame fResponsesTopDown;
	Frame fResponsesBottomUp;

	CV::Canvas::drawText(yParameters, "Width (a / q): " + String::toAString(shapeWidth_), 5, yPosition += 30, black);
	CV::Canvas::drawText(yParameters, "Height (s / w): " + String::toAString(shapeHeight_), 5, yPosition += 20, black);
	CV::Canvas::drawText(yParameters, "Step size (d / e): " + String::toAString(shapeStepSize_), 5, yPosition += 20, black);
	CV::Canvas::drawText(yParameters, "Top band (f / r): " + String::toAString(shapeTopBand_), 5, yPosition += 20, black);
	CV::Canvas::drawText(yParameters, "Bottom band (g / t): " + String::toAString(shapeBottomBand_), 5, yPosition += 20, black);

	CV::Canvas::drawText(yParameters, "Non-maximum suppression radius (; / '): " + String::toAString(detectorNonMaximumSuppressionRadius_, 1u), 5, yPosition += 30, black);
	CV::Canvas::drawText(yParameters, "Min edge response (. / /): " + String::toAString(detectorMinimalEdgeResponse_, 1u), 5, yPosition += 20, black);

	CV::Canvas::drawText(yParameters, "Transposed: " + String::toAString(transposed_), 5, yPosition += 20, black);

	CV::Canvas::drawText(yParameters, "Response visualization (3): " + translateResponseVisualization(responseVisualization_), 5, yPosition += 30, black);
	CV::Canvas::drawText(yParameters, "Response type (4): " + translateResponseType(responseType_), 5, yPosition += 20, black);

	if (shapeDetector_ == SD_GRADIENT_BASED)
	{
		GradientBasedDetector::detectShapes(yFrameTransposed, gradientBasedParameters_.detectorThreshold_, gradientBasedParameters_.detectorResponseFactor_, lShapes, tShapes, xShapes, gradientBasedParameters_.shapeSign_, shapeWidth_, shapeHeight_, shapeStepSize_, shapeTopBand_, shapeBottomBand_, responseType_, gradientBasedParameters_.detectorPenaltyFactor_, (unsigned int)(detectorMinimalEdgeResponse_ + 0.5), detectorNonMaximumSuppressionRadius_, gradientBasedParameters_.detectorEdgeResponseStrategy_, gradientBasedParameters_.detectorMinimalResponseStrategy_, gradientBasedParameters_.detectorPenaltyUsage_, &fResponsesTopDown, &fResponsesBottomUp);

		CV::Canvas::drawText(yParameters, "Response factor (- / =): " + String::toAString(gradientBasedParameters_.detectorResponseFactor_, 1u), 5, yPosition += 30, black);
		CV::Canvas::drawText(yParameters, "Threshold ([ / ]): " + String::toAString(gradientBasedParameters_.detectorThreshold_, 3u), 5, yPosition += 20, black);
		CV::Canvas::drawText(yParameters, "Sign (z): " + String::toAString(gradientBasedParameters_.shapeSign_), 5, yPosition += 20, black);

		CV::Canvas::drawText(yParameters, "Edge response strategy (x): " + GradientBasedParameters::translateEdgeResponseStrategy(gradientBasedParameters_.detectorEdgeResponseStrategy_), 5, yPosition += 20, black);
		CV::Canvas::drawText(yParameters, "Minimal response strategy (c): " + GradientBasedParameters::translateMinimalResponseStrategy(gradientBasedParameters_.detectorMinimalResponseStrategy_), 5, yPosition += 20, black);
		CV::Canvas::drawText(yParameters, "Penality usage (v): " + GradientBasedParameters::translatePenaltyUsage(gradientBasedParameters_.detectorPenaltyUsage_), 5, yPosition += 20, black);

		CV::Canvas::drawText(yParameters, "Penalty factor (n / m): " + String::toAString(gradientBasedParameters_.detectorPenaltyFactor_, 1u), 5, yPosition += 20, black);
	}
	else if (shapeDetector_ == SD_VARIANCE_BASED)
	{
		VarianceBasedDetector::detectShapes(yFrameTransposed, varianceBasedParameters_.detectorThreshold_, varianceBasedParameters_.detectorResponseFactor_, lShapes, tShapes, xShapes, shapeWidth_, shapeHeight_, shapeStepSize_, shapeTopBand_, shapeBottomBand_, responseType_, detectorMinimalEdgeResponse_, varianceBasedParameters_.detectorForegroundVarianceFactor_, varianceBasedParameters_.detectorMinimalVariance_, varianceBasedParameters_.detectorMaximalRatio_, detectorNonMaximumSuppressionRadius_, varianceBasedParameters_.detectorThresholdStrategy_, varianceBasedParameters_.detectorGradientResponseStrategy_, varianceBasedParameters_.detectorBandStrategy_, &fResponsesTopDown, &fResponsesBottomUp);

		CV::Canvas::drawText(yParameters, "Response factor (- / =): " + String::toAString(varianceBasedParameters_.detectorResponseFactor_, 1u), 5, yPosition += 30, black);
		CV::Canvas::drawText(yParameters, "Threshold ([ / ]): " + String::toAString(varianceBasedParameters_.detectorThreshold_, 3u), 5, yPosition += 20, black);

		CV::Canvas::drawText(yParameters, "Foreground response strategy (x): " + VarianceBasedParameters::translateGradientResponseStrategy(varianceBasedParameters_.detectorGradientResponseStrategy_), 5, yPosition += 30, black);
		CV::Canvas::drawText(yParameters, "Band strategy (c): " + VarianceBasedParameters::translateBandStrategy(varianceBasedParameters_.detectorBandStrategy_), 5, yPosition += 20, black);
		CV::Canvas::drawText(yParameters, "Threshold strategy (v): " + VarianceBasedParameters::translateThresholdStrategy(varianceBasedParameters_.detectorThresholdStrategy_), 5, yPosition += 20, black);
		CV::Canvas::drawText(yParameters, "Minimal variance value (n / m): " + String::toAString(varianceBasedParameters_.detectorMinimalVariance_, 1u), 5, yPosition += 20, black);
		CV::Canvas::drawText(yParameters, "Maximal ratio (j / k): " + String::toAString(varianceBasedParameters_.detectorMaximalRatio_, 2u), 5, yPosition += 20, black);
		CV::Canvas::drawText(yParameters, "Foreground variance factor (h / y): " + String::toAString(varianceBasedParameters_.detectorForegroundVarianceFactor_, 2u), 5, yPosition += 20, black);
	}
	else if (shapeDetector_ == SD_GRADIENT_VARIANCE_BASED)
	{
		if (gradientVarianceBasedParameters_.detectorOptimizationStrategy_ != GradientVarianceBasedDetector::OS_NONE)
		{
			if (shapeTopBand_ != shapeBottomBand_)
			{
				MessageBoxA(handle(), "Bottom and top band need to be identical, updating both values.", "Warning", 0);

				shapeTopBand_ = (shapeTopBand_ + shapeBottomBand_ + 1u) / 2u;
				shapeBottomBand_ = shapeTopBand_;
			}
		}

		GradientVarianceBasedDetector::detectShapes(yFrameTransposed, gradientVarianceBasedParameters_.detectorThreshold_, gradientVarianceBasedParameters_.detectorResponseFactor_, lShapes, tShapes, xShapes, gradientVarianceBasedParameters_.shapeSign_, shapeWidth_, shapeHeight_, shapeStepSize_, shapeTopBand_, shapeBottomBand_, responseType_, detectorMinimalEdgeResponse_, gradientVarianceBasedParameters_.detectorMaximalRatio_, gradientVarianceBasedParameters_.detectorBandStrategy_, gradientVarianceBasedParameters_.detectorOptimizationStrategy_, detectorNonMaximumSuppressionRadius_, &fResponsesTopDown, &fResponsesBottomUp);

		CV::Canvas::drawText(yParameters, "Response factor (- / =): " + String::toAString(gradientVarianceBasedParameters_.detectorResponseFactor_, 1u), 5, yPosition += 30, black);
		CV::Canvas::drawText(yParameters, "Threshold ([ / ]): " + String::toAString(gradientVarianceBasedParameters_.detectorThreshold_, 3u), 5, yPosition += 20, black);
		CV::Canvas::drawText(yParameters, "Sign (z): " + String::toAString(gradientVarianceBasedParameters_.shapeSign_), 5, yPosition += 20, black);

		CV::Canvas::drawText(yParameters, "Maximal ratio (j / k): " + String::toAString(gradientVarianceBasedParameters_.detectorMaximalRatio_, 2u), 5, yPosition += 30, black);
		CV::Canvas::drawText(yParameters, "Band strategy (c): " + GradientVarianceBasedParameters::translateBandStrategy(gradientVarianceBasedParameters_.detectorBandStrategy_), 5, yPosition += 20, black);
		CV::Canvas::drawText(yParameters, "Optimization strategy (v): " + GradientVarianceBasedParameters::translateOptimizationStrategy(gradientVarianceBasedParameters_.detectorOptimizationStrategy_), 5, yPosition += 20, black);
	}
	else
	{
		ocean_assert(shapeDetector_ == SD_MODIFIED_GRADIENT_VARIANCE_BASED);

		GradientVarianceBasedDetector::detectShapesModified(yFrameTransposed, modifiedGradientVarianceBasedParameters_.detectorThreshold_, modifiedGradientVarianceBasedParameters_.detectorResponseFactor_, lShapes, tShapes, xShapes, modifiedGradientVarianceBasedParameters_.shapeSign_, shapeWidth_, shapeHeight_, shapeStepSize_, shapeTopBand_, shapeBottomBand_, responseType_, detectorMinimalEdgeResponse_, modifiedGradientVarianceBasedParameters_.detectorMaximalRatio_, modifiedGradientVarianceBasedParameters_.detectorBandStrategy_, detectorNonMaximumSuppressionRadius_, &fResponsesTopDown, &fResponsesBottomUp);

		CV::Canvas::drawText(yParameters, "Response factor (- / =): " + String::toAString(modifiedGradientVarianceBasedParameters_.detectorResponseFactor_, 1u), 5, yPosition += 30, black);
		CV::Canvas::drawText(yParameters, "Threshold ([ / ]): " + String::toAString(modifiedGradientVarianceBasedParameters_.detectorThreshold_, 3u), 5, yPosition += 20, black);
		CV::Canvas::drawText(yParameters, "Sign (z): " + String::toAString(modifiedGradientVarianceBasedParameters_.shapeSign_), 5, yPosition += 20, black);

		CV::Canvas::drawText(yParameters, "Maximal ratio (j / k): " + String::toAString(modifiedGradientVarianceBasedParameters_.detectorMaximalRatio_, 2u), 5, yPosition += 30, black);
		CV::Canvas::drawText(yParameters, "Band strategy (c): " + GradientVarianceBasedParameters::translateBandStrategy(modifiedGradientVarianceBasedParameters_.detectorBandStrategy_), 5, yPosition += 20, black);
	}

	windowParameters_.setFrame(yParameters);
	windowParameters_.repaint();

	Frame rgbFrame;
	if (!CV::FrameConverter::Comfort::convert(yFrameTransposed, FrameType::FORMAT_RGB24, rgbFrame, CV::FrameConverter::CP_ALWAYS_COPY, WorkerPool::get().scopedWorker()()))
	{
		ocean_assert(false && "Should never happen!");
		return;
	}

	uint8_t* const rgbData = rgbFrame.data<uint8_t>();
	for (unsigned int n = 0u; n < rgbFrame.size(); ++n)
	{
		rgbData[n] = uint8_t(std::min((unsigned int)(double(rgbData[n]) * visibilityLinearGain_ + 0.5f), 255u));
	}

	Frame rgbFrameWithShapes(rgbFrame, Frame::ACM_COPY_KEEP_LAYOUT_DO_NOT_COPY_PADDING_DATA);

	CV::Detector::Utilities::paintShapes(rgbFrameWithShapes, lShapes, tShapes, xShapes, 5);

	if (transposed_)
	{
		CV::FrameTransposer::transpose(rgbFrameWithShapes, WorkerPool::get().scopedWorker()());
		CV::FrameTransposer::transpose(rgbFrame, WorkerPool::get().scopedWorker()());
		CV::FrameTransposer::transpose(fResponsesTopDown, WorkerPool::get().scopedWorker()());
		CV::FrameTransposer::transpose(fResponsesBottomUp, WorkerPool::get().scopedWorker()());
	}

	if (showResponses_)
	{
		Frame yResponseTopDown;
		Frame yResponseBottomUp;

		if (!fResponsesTopDown.isValid() || !fResponsesBottomUp.isValid())
		{
			yResponseTopDown.set(FrameType(yFrameTransposed.frameType(), FrameType::FORMAT_Y8), true /*forceOwner*/, true /*forceWritable*/);
			yResponseTopDown.setValue(0x00u);

			yResponseBottomUp.set(FrameType(yFrameTransposed.frameType(), FrameType::FORMAT_Y8), true /*forceOwner*/, true /*forceWritable*/);
			yResponseBottomUp.setValue(0x00u);
		}
		else
		{
			if (responseVisualization_ == RV_LOGARITHMIC)
			{
				CV::FrameNormalizer::normalizeLogarithmToFloat32(fResponsesTopDown, Scalar(5));
				CV::FrameNormalizer::normalizeLogarithmToFloat32(fResponsesBottomUp, Scalar(5));
			}

			CV::FrameConverterColorMap::Comfort::convertFloat1ChannelToRGB24(fResponsesTopDown, yResponseTopDown, CV::FrameConverterColorMap::CM_INFERNO);
			CV::FrameConverterColorMap::Comfort::convertFloat1ChannelToRGB24(fResponsesBottomUp, yResponseBottomUp, CV::FrameConverterColorMap::CM_INFERNO);
		}

		windowResponsesTopDown_.setFrame(yResponseTopDown);
		windowResponsesBottomUp_.setFrame(yResponseBottomUp);
	}
	else
	{
		windowResponsesTopDown_.setFrame(rgbFrame);
		windowResponsesBottomUp_.setFrame(rgbFrame);
	}

	windowResponsesTopDown_.repaint();
	windowResponsesBottomUp_.repaint();

	setFrame(rgbFrameWithShapes);
	repaint();
}

std::string ShapeDetectorMainWindow::translateShapeDetector(const ShapeDetector shapeDetector)
{
	switch (shapeDetector)
	{
		case SD_GRADIENT_BASED:
			return "Gradient-based";

		case SD_VARIANCE_BASED:
			return "Variance-based";

		case SD_GRADIENT_VARIANCE_BASED:
			return "Gradient & Variance-based";

		case SD_MODIFIED_GRADIENT_VARIANCE_BASED:
			return "Modified gradient & Variance-based";

		case SD_END:
			break;
	}

	ocean_assert(false && "Invalid strategy!");
	return "Invalid";
}

std::string ShapeDetectorMainWindow::translateResponseType(const ResponseType responseType)
{
	switch (responseType)
	{
		case RT_HORIZONTAL_AND_VERTICAL:
			return "Horizontal and vertical";

		case RT_HORIZONTAL:
			return "Horizontal";

		case RT_VERTICAL:
			return "Vertical";
	}

	ocean_assert(false && "Invalid strategy!");
	return "Invalid";
}

std::string ShapeDetectorMainWindow::translateResponseVisualization(const ResponseVisualization responseVisualization)
{
	switch (responseVisualization)
	{
		case RV_LINEAR:
			return "Linear";

		case RV_LOGARITHMIC:
			return "Logarithmic";
	}

	ocean_assert(false && "Invalid strategy!");
	return "Invalid";
}
