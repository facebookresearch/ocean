/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/ObjectRef.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameInterpolatorNearestPixel.h"

#include "ocean/io/File.h"

#include "ocean/media/openimagelibraries/Image.h"

using namespace Ocean;

int main(int argc, char** argv)
{
	// Direct all messages to the standard output
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	CommandArguments commandArguments("Generates a calibration pattern");
	commandArguments.registerParameter("horizontal", "x", "Number of horizontal boxes in the pattern, default: 5");
	commandArguments.registerParameter("vertical", "y", "Number of vertical boxes in the pattern, default: 7");
	commandArguments.registerParameter("output", "o", "Location where the generated pattern will be stored, default: ./pattern.png");
	commandArguments.registerParameter("size", "s", "Size in pixels of the image of the pattern that will be generated, will set to the closest multiple of the actual pattern size.");
	commandArguments.registerParameter("help", "h", "Showing this help output.");

	if (!commandArguments.parse(argv, argc))
	{
		Log::warning() << "Failed to parse the command arguments.";

		return 1;
	}

	if (commandArguments.hasValue("help"))
	{
		Log::info() << commandArguments.makeSummary();
		return 0;
	}

	const Value horizontalValue(commandArguments.value("horizontal"));
	unsigned int horizontalBoxes = 5u;

	if (horizontalValue.isInt() && horizontalValue.intValue() > 0)
	{
		horizontalBoxes = (unsigned int)(horizontalValue.intValue());
	}

	const Value verticalValue(commandArguments.value("vertical"));
	unsigned int verticalBoxes = 7u;

	if (verticalValue.isInt() && verticalValue.intValue() > 0)
	{
		verticalBoxes = (unsigned int)(verticalValue.intValue());
	}

	const Value outputValue(commandArguments.value("output"));
	std::string outputFilename = "./pattern.png";

	if (outputValue.isString() && !outputValue.stringValue().empty())
	{
		outputFilename = outputValue.stringValue();
	}

	const Value imageSizeValue(commandArguments.value("size"));
	unsigned int targetSize = 1000u;

	if (imageSizeValue.isInt() && imageSizeValue.intValue() > 0)
	{
		targetSize = (unsigned int)(imageSizeValue.intValue());
	}

	ocean_assert(outputFilename.empty() == false);

	Frame unscaledFrame(FrameType(horizontalBoxes * 2u + 1u, verticalBoxes * 2u + 1, FrameType::FORMAT_Y8, FrameType::ORIGIN_UPPER_LEFT));
	unscaledFrame.setValue(0xFFu);
	
	for (unsigned int y = 0u; y < verticalBoxes; ++y)
	{
		for (unsigned int x = 0u; x < horizontalBoxes; ++x)
		{
			unscaledFrame.pixel<uint8_t>(x * 2u + 1u, y * 2u + 1u)[0] = 0x00u;
		}
	}

	unsigned int scale = targetSize / unscaledFrame.width();

	if (targetSize % unscaledFrame.width() != 0u)
	{
		scale += 1u;
	}

	Frame frame(FrameType(unscaledFrame, scale * unscaledFrame.width(), scale * unscaledFrame.height()));
	CV::FrameInterpolatorNearestPixel::resize<uint8_t, 1u>(unscaledFrame.constdata<uint8_t>(), frame.data<uint8_t>(), unscaledFrame.width(), unscaledFrame.height(), frame.width(), frame.height(), unscaledFrame.paddingElements(), frame.paddingElements(), WorkerPool::get().scopedWorker()());

	ocean_assert(frame.isValid());

	if (Media::OpenImageLibraries::Image::writeImage(frame, outputFilename) == false)
	{
		Log::error() << "Failed to save to file \'" << outputFilename << "\'";
	}
	else
	{
		Log::info() << "Saved image of pattern to \'" << outputFilename << "\'";
	}

	Log::info() << " ";
	Log::info() << "Pattern:     " << horizontalBoxes << " x " << verticalBoxes;
	Log::info() << "Output file: " << outputFilename;
	Log::info() << "Target size: " << targetSize << ", final size: " << frame.width() << " x " << frame.height();

	return 0;
}
