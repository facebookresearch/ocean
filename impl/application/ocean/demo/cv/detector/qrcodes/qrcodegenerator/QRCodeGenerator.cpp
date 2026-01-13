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
#include "ocean/base/RandomI.h"
#include "ocean/base/String.h"
#include "ocean/base/WorkerPool.h"

#include "ocean/cv/FrameInterpolatorNearestPixel.h"

#include "ocean/cv/detector/qrcodes/MicroQRCodeEncoder.h"
#include "ocean/cv/detector/qrcodes/QRCodeEncoder.h"
#include "ocean/cv/detector/qrcodes/Utilities.h"

#include "ocean/io/File.h"

#include "ocean/media/openimagelibraries/Image.h"

using namespace Ocean;
using namespace Ocean::CV::Detector::QRCodes;

int main(int argc, char** argv)
{
	// Direct all messages to the standard output
	Messenger::get().setOutputType(Messenger::OUTPUT_STANDARD);

	CommandArguments commandArguments("Generates a QR code for user-provided messages");
	commandArguments.registerNamelessParameters("The message that will be encoded into a QR code, must be valid");
	commandArguments.registerParameter("output", "o", "Location where the generated QR code will be stored, default: ./qrcode.png");
	commandArguments.registerParameter("size", "s", "Size in pixels of the image of the QR code that will be generated, will set to the closest multiple of the actual QR code size.");
	commandArguments.registerParameter("ecc", "e", "Level of error correction, standard QR codes: 7, 15, 25, 30 (default: 7), micro QR codes: 0, 7, 15, 25 (default: 0)");
	commandArguments.registerParameter("no-quiet-zone", "q", "If specified, no quiet zone will be added around the generated QR code.");
	commandArguments.registerParameter("micro", "m", "If specified, a micro QR code will be generated.");
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
	const Strings& namelessValues = commandArguments.namelessValues();

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

	const bool generateMicroQRCode = commandArguments.hasValue("micro");

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

	QRCode::ErrorCorrectionCapacity errorCorrectionCapacity = generateMicroQRCode ? QRCode::ECC_DETECTION_ONLY : QRCode::ECC_07;

	if (commandArguments.hasValue("ecc"))
	{
		const Value eccValue(commandArguments.value("ecc"));

		errorCorrectionCapacity = QRCode::ECC_INVALID;

		if (eccValue.isInt())
		{
			switch (eccValue.intValue())
			{
				case 0:
					errorCorrectionCapacity = QRCode::ECC_DETECTION_ONLY;
					break;

				case 7:
					errorCorrectionCapacity = QRCode::ECC_07;
					break;

				case 15:
					errorCorrectionCapacity = QRCode::ECC_15;
					break;

				case 25:
					errorCorrectionCapacity = QRCode::ECC_25;
					break;

				case 30:
					errorCorrectionCapacity = QRCode::ECC_30;
					break;

				default:
					Log::error() << "Invalid error-correction value: " << eccValue.intValue();
					return 1;
			}
		}
	}

	if (errorCorrectionCapacity == QRCode::ECC_INVALID)
	{
		Log::error() << "Invalid error-correction value";
		return 1;
	}

	if (generateMicroQRCode)
	{
		if (errorCorrectionCapacity == QRCode::ECC_30)
		{
			Log::error() << "Error correction level 30 is not supported for micro QR codes";
			return 1;
		}
	}
	else
	{
		if (errorCorrectionCapacity == QRCode::ECC_DETECTION_ONLY)
		{
			Log::error() << "Error correction level 0 is not supported for standard QR codes";
			return 1;
		}
	}

	const bool noQuietZone = commandArguments.hasValue("no-quiet-zone");

	ocean_assert(message.empty() == false);
	ocean_assert(outputFilename.empty() == false);
ocean_assert(errorCorrectionCapacity != QRCode::ECC_INVALID);

	using QRCodeBaseRef = ObjectRef<QRCodeBase>;

	QRCodeBaseRef codeRef;
	QRCodeEncoderBase::StatusCode encodeTextStatus;

	if (generateMicroQRCode)
	{
		MicroQRCode code;
		encodeTextStatus = MicroQRCodeEncoder::encodeText(message, errorCorrectionCapacity, code);
		codeRef = QRCodeBaseRef(new MicroQRCode(code));
	}
	else
	{
		QRCode code;
		encodeTextStatus = QRCodeEncoder::encodeText(message, errorCorrectionCapacity, code);
		codeRef = QRCodeBaseRef(new QRCode(code));
	}

	switch(encodeTextStatus)
	{
		case QRCodeEncoderBase::SC_SUCCESS:
			break;

		case QRCodeEncoderBase::SC_CODE_CAPACITY_EXCEEDED:
			Log::error() << "The message is too long for the QR code type selected";
			return 1;

		case QRCodeEncoderBase::SC_INVALID_DATA:
			Log::error() << "The message contains invalid characters";
			return 1;

		case QRCodeEncoderBase::SC_INVALID_ERROR_CORRECTION_CAPACITY:
			Log::error() << "The error correction capacity is invalid";
			return 1;

		case QRCodeEncoderBase::SC_UNKNOWN_ERROR:
			Log::error() << "An unknown error occurred";
			return 1;
	}

	ocean_assert(!codeRef.isNull() && codeRef->isValid());

	Log::info() << " ";
	Log::info() << "ASCII version of the QR code:" << CV::Detector::QRCodes::Utilities::toString(*codeRef, /* border */ 4u);

	const bool quietZoneBorder = noQuietZone ? 0u : 4u;
	const Frame frame = CV::Detector::QRCodes::Utilities::draw(*codeRef, imageSize, /* allowTrueMultiple */ true, quietZoneBorder, WorkerPool::get().scopedWorker()());

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
	Log::info() << " * type:    " << codeRef->translateCodeType(codeRef->codeType());
	Log::info() << " * version: " << codeRef->versionString();
	Log::info() << " * modules: " << codeRef->modulesPerSide();

	return 0;
}
