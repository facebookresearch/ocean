/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/base/CommandArguments.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Messenger.h"
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameInterpolatorNearestPixel.h"

#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/io/File.h"

#include "ocean/media/openimagelibraries/Image.h"

using namespace Ocean;

int main(int argc, char** argv)
{
	// Direct all messages to the standard output
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	CommandArguments commandArguments("Generates a QR code for user-provided messages");
	commandArguments.registerNamelessParameters("The message that will be encoded into a QR code, must be valid");
	commandArguments.registerParameter("output", "o", "Location where the generated QR code will be stored, default: ./qrcode.png");
	commandArguments.registerParameter("size", "s", "Size in pixels of the image of the QR code that will be generated, will set to the closest multiple of the actual QR code size.");
	commandArguments.registerParameter("ecc", "e", "Level of error correction, accepted values: 7, 15, 25, 30 (default: 7)");
	commandArguments.registerParameter("no-quiet-zone", "q", "If specified, no quiet zone will be added around the generated QR code.");
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

	std::string message;
	const std::vector<std::string>& namelessValues = commandArguments.namelessValues();

	if (!namelessValues.empty())
	{
		message = namelessValues.front();

		for (size_t i = 1; i < namelessValues.size(); ++i)
		{
			message += " " + namelessValues[i];
		}
	}

	if (message.empty())
	{
		Log::error() << "No data to create a QR code.";

		return 1;
	}

	const Value outputValue(commandArguments.value("output"));
	std::string outputFilename = "./qrcode.png";

	if (outputValue.isString() && !outputValue.stringValue().empty())
	{
		outputFilename = outputValue.stringValue();
	}

	const Value imageSizeValue(commandArguments.value("size"));
	unsigned int imageSize = 1000u;

	if (imageSizeValue.isInt() && imageSizeValue.intValue() > 0)
	{
		imageSize = (unsigned int)(imageSizeValue.intValue());
	}

	CV::Detector::QRCodes::QRCode::ErrorCorrectionCapacity errorCorrectionCapacity = CV::Detector::QRCodes::QRCode::ECC_07;
	if (commandArguments.hasValue("ecc"))
	{
		const Value eccValue(commandArguments.value("ecc"));
		errorCorrectionCapacity = CV::Detector::QRCodes::QRCode::ECC_INVALID;

		if (eccValue.isInt() && eccValue.intValue() > 0)
		{
			switch (eccValue.intValue())
			{
				case 7:
					errorCorrectionCapacity = CV::Detector::QRCodes::QRCode::ECC_07;
					break;

				case 15:
					errorCorrectionCapacity = CV::Detector::QRCodes::QRCode::ECC_15;
					break;

				case 25:
					errorCorrectionCapacity = CV::Detector::QRCodes::QRCode::ECC_25;
					break;

				case 30:
					errorCorrectionCapacity = CV::Detector::QRCodes::QRCode::ECC_30;
					break;
			}
		}
	}
	
	if (errorCorrectionCapacity == CV::Detector::QRCodes::QRCode::ECC_INVALID)
	{
		Log::error() << "Invalid error-correction value";
		return 1;
	}

	const bool noQuietZone = commandArguments.hasValue("no-quiet-zone");

	ocean_assert(message.empty() == false);
	ocean_assert(outputFilename.empty() == false);
	ocean_assert(errorCorrectionCapacity != CV::Detector::QRCodes::QRCode::ECC_INVALID);

	CV::Detector::QRCodes::QRCode code;

	if (!CV::Detector::QRCodes::QRCodeEncoder::encodeText(message, errorCorrectionCapacity, code))
	{
		Log::error() << "Failed to generate a QR code";
		return 1;
	}

	ocean_assert(code.isValid());

	Log::info() << " ";
	Log::info() << "ASCII version of the QR code:" << CV::Detector::QRCodes::Utilities::toString(code, /* border */ 4u);

	const bool quietZoneBorder = noQuietZone ? 0u : 4u;
	const Frame frame = CV::Detector::QRCodes::Utilities::draw(code, imageSize, /* allowTrueMultiple */ true, quietZoneBorder, WorkerPool::get().scopedWorker()());

	ocean_assert(frame.isValid());

	if (Media::OpenImageLibraries::Image::writeImage(frame, outputFilename) == false)
	{
		Log::error() << "Failed to save to file \'" << outputFilename << "\'";
	}
	else
	{
		Log::info() << "Saved image of QR code to \'" << outputFilename << "\'";
	}

	Log::info() << " ";
	Log::info() << "Message:    \'" << message << "\'";
	Log::info() << "Output file: " << outputFilename;
	Log::info() << "Image size:  " << imageSize << ", final size: " << frame.width();
	Log::info() << " ";
	Log::info() << "QR code:";
	Log::info() << " * version: " << code.version();
	Log::info() << " * modules: " << code.modulesPerSide();
	Log::info() << " ";

	return 0;
}
