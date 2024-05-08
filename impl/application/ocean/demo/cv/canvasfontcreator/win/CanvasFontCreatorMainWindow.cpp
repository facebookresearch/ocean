/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "application/ocean/demo/cv/canvasfontcreator/win/CanvasFontCreatorMainWindow.h"

#include "ocean/cv/FrameConverter.h"

#include "ocean/platform/win/Utilities.h"

CanvasFontCreatorMainWindow::CanvasFontCreatorMainWindow(HINSTANCE instance, const std::wstring& name) :
	Window(instance, name),
	BitmapWindow(instance, name),
	ApplicationWindow(instance, name)
{
	// nothing to do here
}

CanvasFontCreatorMainWindow::~CanvasFontCreatorMainWindow()
{
	// nothing to do here
}

void CanvasFontCreatorMainWindow::onPaint()
{
	HBRUSH brushRed = CreateSolidBrush(0xFF0000);

	// we iterate over each ASCII character, create a bitmap, draw the character, and extract the black pixels

	Font::Character characters[256];

	static_assert(sizeof(characters) == 256 * 34, "Invalid data type!");

	for (unsigned int n = 0u; n < 256u; ++n)
	{
		Platform::Win::Bitmap bitmap(100u, 100u, FrameType::FORMAT_RGB24, FrameType::ORIGIN_UPPER_LEFT);

		// we make a red background

		SelectObject(bitmap.dc(), brushRed);
		Rectangle(bitmap.dc(), -1, -1, bitmap.width() + 1, bitmap.height() + 1);

		// we paint the character into the bitmap

		std::string text = " ";
		text[0] = char(n);

		Platform::Win::Utilities::textOutput(bitmap.dc(), 0, 0, text);

		// we copy the pixel data from the bitmap to a frame

		Frame frame(FrameType(bitmap.width(), bitmap.height(), bitmap.pixelFormat(), bitmap.pixelOrigin()), bitmap.data(), Frame::CM_COPY_REMOVE_PADDING_LAYOUT);

		// we determine the bounding box of the character

		const CV::PixelBoundingBox boundingBox = determineCharacterBoundingBox(frame, CV::Canvas::red());
		ocean_assert(boundingBox.width() <= 16u && boundingBox.height() <= 16u);

		Frame characterFrame = frame.subFrame(0u, 0u, boundingBox.width(), boundingBox.height(), Frame::CM_USE_KEEP_LAYOUT);

		// we convert the frame to a character (the convert the black pixels to bits)

		characters[n] = Font::Character(characterFrame, CV::Canvas::black());
	}

	const std::string text = "This is a test text, with letters, numbers (12345) and some symbols @#$-!";

	unsigned int x = 0u;
	for (size_t n = 0; n < text.size(); ++n)
	{
		const Font::Character& character = characters[text[n]];

		const Frame frame = character.frame(FrameType::FORMAT_RGB24, CV::Canvas::white(), CV::Canvas::black());

		Platform::Win::Utilities::frameOutput(dc(), 5 + x, 5, frame);
		x += frame.width();
	}

	DeleteObject(brushRed);

#if 0

	// this code block can write the font data as an array of bytes

	std::vector<uint8_t> data(256 * 34);
	memcpy(data.data(), characters, 256 * 34);

	std::ofstream stream("character_data.txt");

	stream << "{";

	for (size_t n = 0; n < data.size(); ++n)
	{
		if (n % 34 == 0)
			stream << std::endl;

		stream << "0x" << String::toAStringHex(data[n]) << ", ";
	}

	stream << "}";

#endif
}

CV::PixelBoundingBox CanvasFontCreatorMainWindow::determineCharacterBoundingBox(const Frame& frame, const unsigned char* invalidColor)
{
	ocean_assert(frame.isValid());
	ocean_assert(FrameType::formatIsGeneric(frame.pixelFormat()));
	ocean_assert(frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8);

	const unsigned int channels = frame.channels();

	ocean_assert(memcmp(frame.constpixel<uint8_t>(0, 0), invalidColor, channels) != 0); // the upper left pixel must be valid

	unsigned int rightEnd = 0u;
	unsigned int bottomEnd = 0u;

	for (unsigned int x = 0u; x < frame.width(); ++x)
	{
		const uint8_t* pixel = frame.constpixel<uint8_t>(x, 0u);

		if (memcmp(pixel, invalidColor, channels) == 0)
		{
			rightEnd = x;
			break;
		}
	}

	for (unsigned int y = 0u; y < frame.width(); ++y)
	{
		const uint8_t* pixel = frame.constpixel<uint8_t>(0u, y);

		if (memcmp(pixel, invalidColor, channels) == 0)
		{
			bottomEnd = y;
			break;
		}
	}

	ocean_assert(rightEnd != 0u && bottomEnd != 0u);

	return CV::PixelBoundingBox(0u, 0u, rightEnd - 1u, bottomEnd - 1u);
}
