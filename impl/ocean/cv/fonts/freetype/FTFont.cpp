/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/cv/fonts/freetype/FTFont.h"
#include "ocean/cv/fonts/freetype/Utilities.h"

#include "ocean/cv/FrameChannels.h"

#include <freetype/ftglyph.h>

namespace Ocean
{

namespace CV
{

namespace Fonts
{

FTFont::FTFont(const FT_Face& ftFace, const unsigned int size, const unsigned int dpi) :
	Font()
{
	ocean_assert(size_ == 0u && dpi_ == 0u && height_ == 0u);

	ocean_assert(ftFace != nullptr);
	ocean_assert(size != 0u && dpi != 0u);

	const FT_Error error = FT_Set_Char_Size(ftFace, 0, FT_F26Dot6(size << 6u), FT_UInt(dpi), FT_UInt(dpi));

	if (error != FT_Err_Ok)
	{
		Log::error() << "Failed to set font size: " << Utilities::translateErrorCode(error);
		return;
	}

	if (initialize(ftFace))
	{
		size_ = size;
		dpi_ = dpi;

		ocean_assert(charactersFrame_.isValid());
		height_ = charactersFrame_.height() - extraBorder_ * 2u; // the line height does not include the upper and lower extra border

		isValid_ = true;
	}
	else
	{
		isValid_ = false;

		Log::error() << "Failed to create font " << ftFace->family_name << " with size " << size;
	}
}

bool FTFont::drawText(Frame& frame, const std::string& text, const int left, const int top, const uint8_t* foregroundColor, const uint8_t* backgroundColor) const
{
	ocean_assert(isValid());

	ocean_assert(frame.isValid());
	ocean_assert(foregroundColor != nullptr);

	if (text.empty())
	{
		return true;
	}

	if (!characters_ || characters_->size() != endCharacter_ - firstCharacter_)
	{
		ocean_assert(false && "This must never happen!");
		return false;
	}

	if (frame.numberPlanes() != 1u || frame.dataType() != FrameType::DT_UNSIGNED_INTEGER_8)
	{
		Log::warning() << "Font::drawText(): The pixel format is not supported!";
		return false;
	}

	if (charactersFrame_.pixelFormat() != FrameType::FORMAT_Y8)
	{
		ocean_assert(false && "Currently not supported!");
		return false;
	}

	if (left >= int(frame.width()))
	{
		return true;
	}

	const unsigned int targetFrameStrideElements = frame.strideElements();
	const unsigned int sourceFrameStrideElements = charactersFrame_.strideElements();

	const unsigned int targetChannels = frame.channels();

	if (backgroundColor != nullptr)
	{
		PixelBoundingBoxesI rowBoundingBoxes;

		if (!textExtent(text, rowBoundingBoxes))
		{
			return false;
		}

		for (const PixelBoundingBoxI& rowBoundingBox : rowBoundingBoxes)
		{
			if (!rowBoundingBox.isValid())
			{
				continue;
			}

			const int xStartInTargetFrame = std::max(left + rowBoundingBox.left(), 0);
			const int xEndInTargetFrame = std::min(left + int(rowBoundingBox.rightEnd()), int(frame.width()));

			const int yStartInTargetFrame = std::max(top + rowBoundingBox.top(), 0);
			const int yEndInTargetFrame = std::min(top + int(rowBoundingBox.bottomEnd()), int(frame.height()));

			if (xStartInTargetFrame >= xEndInTargetFrame || yStartInTargetFrame >= yEndInTargetFrame)
			{
				continue;
			}

			const unsigned int boundingBoxWidth = (unsigned int)(xEndInTargetFrame - xStartInTargetFrame);
			const unsigned int boundingBoxHeight = (unsigned int)(yEndInTargetFrame - yStartInTargetFrame);

			ocean_assert((unsigned int)(xStartInTargetFrame) + boundingBoxWidth <= frame.width());
			ocean_assert((unsigned int)(yStartInTargetFrame) + boundingBoxHeight <= frame.height());

			uint8_t* targetFrameData = frame.pixel<uint8_t>((unsigned int)(xStartInTargetFrame), (unsigned int)(yStartInTargetFrame));

			switch (targetChannels)
			{
				case 1u:
					renderBackground<uint8_t, 1u>(targetFrameData, boundingBoxWidth, boundingBoxHeight, backgroundColor, targetFrameStrideElements);
					break;

				case 2u:
					renderBackground<uint8_t, 2u>(targetFrameData, boundingBoxWidth, boundingBoxHeight, backgroundColor, targetFrameStrideElements);
					break;

				case 3u:
					renderBackground<uint8_t, 3u>(targetFrameData, boundingBoxWidth, boundingBoxHeight, backgroundColor, targetFrameStrideElements);
					break;

				case 4u:
					renderBackground<uint8_t, 4u>(targetFrameData, boundingBoxWidth, boundingBoxHeight, backgroundColor, targetFrameStrideElements);
					break;

				default:
					ocean_assert(false && "Invalid pixel format!");
					return false;
			}
		}
	}

	int currentX = left;
	int currentY = top;

	for (size_t n = 0; n < text.size(); ++n)
	{
		const char& value = text[n];

		if (value == '\n')
		{
			currentX = left;
			currentY += int(height_);
			continue;
		}

		if (value < firstCharacter_ || value >= endCharacter_)
		{
			// we simply skip the character
			continue;
		}

		const Character& character = (*characters_)[value - firstCharacter_];

		const int xStartInTargetFrame = std::max(currentX + character.bearingX(), 0);
		const int xEndInTargetFrame = std::min(currentX + character.bearingX() + int(character.width()), int(frame.width()));

		const int yStartInTargetFrame = std::max(currentY + int(character.linePositionY()), 0);
		const int yEndInTargetFrame = std::min(currentY + int(character.linePositionY()) + int(character.height()), int(frame.height()));

		if (xStartInTargetFrame < xEndInTargetFrame && yStartInTargetFrame < yEndInTargetFrame)
		{
			const unsigned int xStartInCharacter = (unsigned int)(xStartInTargetFrame - (currentX + character.bearingX()));
			ocean_assert(xStartInCharacter < character.width());

			const unsigned int yStartInCharacter = (unsigned int)(yStartInTargetFrame - (currentY + int(character.linePositionY())));
			ocean_assert(yStartInCharacter < character.height());

			const unsigned int outputWidth = (unsigned int)(xEndInTargetFrame - xStartInTargetFrame);
			const unsigned int outputHeight = (unsigned int)(yEndInTargetFrame - yStartInTargetFrame);

			ocean_assert(outputWidth <= character.width());
			ocean_assert(outputHeight <= character.height());

			uint8_t* targetFrameData = frame.pixel<uint8_t>((unsigned int)(xStartInTargetFrame), (unsigned int)(yStartInTargetFrame));
			const uint8_t* sourceFrameData = charactersFrame_.constpixel<uint8_t>(character.framePositionX() + xStartInCharacter, character.framePositionY() + yStartInCharacter);

			switch (targetChannels)
			{
				case 1u:
					renderCharacterFromY8<uint8_t, 1u>(sourceFrameData, targetFrameData, outputWidth, outputHeight, foregroundColor, sourceFrameStrideElements, targetFrameStrideElements);
					break;

				case 2u:
					renderCharacterFromY8<uint8_t, 2u>(sourceFrameData, targetFrameData, outputWidth, outputHeight, foregroundColor, sourceFrameStrideElements, targetFrameStrideElements);
					break;

				case 3u:
					renderCharacterFromY8<uint8_t, 3u>(sourceFrameData, targetFrameData, outputWidth, outputHeight, foregroundColor, sourceFrameStrideElements, targetFrameStrideElements);
					break;

				case 4u:
					renderCharacterFromY8<uint8_t, 4u>(sourceFrameData, targetFrameData, outputWidth, outputHeight, foregroundColor, sourceFrameStrideElements, targetFrameStrideElements);
					break;

				default:
					ocean_assert(false && "Invalid pixel format!");
					return false;
			}
		}

		currentX += int(character.advanceX());
	}

	return true;
}

bool FTFont::textExtent(const std::string& text, unsigned int& width, unsigned int& height, int* left, int* top) const
{
	if (text.empty())
	{
		width = 0u;
		height = 0u;

		if (left)
		{
			*left = 0;
		}

		if (top)
		{
			*top = 0;
		}

		return true;
	}

	if (!characters_ || characters_->size() != endCharacter_ - firstCharacter_)
	{
		ocean_assert(false && "This must never happen!");
		return false;
	}

	ocean_assert(height_ != 0u);

	int xStart = 0;
	int xEnd = 0;

	int currentX = 0;
	unsigned int currentY = 0u;

	for (size_t n = 0; n < text.size(); ++n)
	{
		const char& value = text[n];

		if (value == '\n')
		{
			currentX = 0;
			currentY += height_;
			continue;
		}

		if (value < firstCharacter_ || value >= endCharacter_)
		{
			// we simply skip the character
			continue;
		}

		const Character& character = (*characters_)[value - firstCharacter_];

		xStart = std::min(currentX + character.bearingX(), xStart); // bearing could be negative
		xEnd = std::max(xEnd, currentX + int(character.advanceX()));

		currentX += int(character.advanceX());
	}

	ocean_assert(xStart <= xEnd);

	if (xStart == xEnd)
	{
		width = 0u;
		height = 0u;
	}
	else
	{
		width = (unsigned int)(xEnd - xStart);
		height = currentY + height_;
	}

	if (left != nullptr)
	{
		*left = xStart;
	}

	if (top != nullptr)
	{
		*top = 0;
	}

	return true;
}

bool FTFont::textExtent(const std::string& text, PixelBoundingBoxesI& rowBoundingBoxes) const
{
	if (text.empty())
	{
		rowBoundingBoxes.clear();

		return true;
	}

	if (!characters_ || characters_->size() != endCharacter_ - firstCharacter_)
	{
		ocean_assert(false && "This must never happen!");
		return false;
	}

	ocean_assert(height_ != 0u);

	int xStart = 0;
	int xEnd = 0;

	int currentX = 0;
	unsigned int currentY = 0u;

	PixelBoundingBoxesI internalRowBoundingBoxes;
	internalRowBoundingBoxes.reserve(8);

	for (size_t n = 0; n < text.size(); ++n)
	{
		const char& value = text[n];

		if (value == '\n')
		{
			ocean_assert(xStart <= xEnd);

			if (xStart == xEnd)
			{
				// we have an empty row, so we set an invalid bounding box
				internalRowBoundingBoxes.emplace_back(CV::PixelBoundingBoxI());
			}
			else
			{
				internalRowBoundingBoxes.emplace_back(CV::PixelPositionI(xStart, currentY), (unsigned int)(xEnd - xStart), height_);
			}

			xStart = 0;
			xEnd = 0;

			currentX = 0;
			currentY += height_;
			continue;
		}

		if (value < firstCharacter_ || value >= endCharacter_)
		{
			// we simply skip the character
			continue;
		}

		const Character& character = (*characters_)[value - firstCharacter_];

		xStart = std::min(currentX + character.bearingX(), xStart); // bearing could be negative
		xEnd = std::max(xEnd, currentX + int(character.advanceX()));

		currentX += int(character.advanceX());
	}

	if (xEnd == 0)
	{
		ocean_assert(xStart == 0);

		// we have an empty row, so we set an invalid bounding box
		internalRowBoundingBoxes.emplace_back(CV::PixelBoundingBoxI());
	}
	else
	{
		internalRowBoundingBoxes.emplace_back(CV::PixelPositionI(xStart, currentY), (unsigned int)(xEnd - xStart), height_);
	}

	rowBoundingBoxes = std::move(internalRowBoundingBoxes);

	return true;
}

bool FTFont::characterFrame(Frame& frame, SharedCharacters* characters) const
{
	if (!charactersFrame_.isValid())
	{
		return false;
	}

	frame.copy(charactersFrame_);

	if (characters != nullptr)
	{
		*characters = characters_;
	}

	return true;
}

bool FTFont::initialize(const FT_Face& ftFace)
{
	ocean_assert(ftFace != nullptr);

	ocean_assert(!characters_);
	ocean_assert(!charactersFrame_.isValid());

	static_assert(firstCharacter_ < endCharacter_, "Invalid character definition!");
	constexpr unsigned int numberCharacters = endCharacter_ - firstCharacter_;

	characters_ = std::make_shared<Characters>();
	characters_->reserve(numberCharacters);

	unsigned int framePositionX = 0u;
	int minCharacterY = NumericT<int>::maxValue();
	int maxCharacterY = NumericT<int>::minValue();

	if (ftFace->available_sizes != 0)
	{
		FT_Select_Size(ftFace, 0);
	}

	// first, we determine the metrics of the individual glyphs

	for (unsigned int nCharacter = firstCharacter_; nCharacter < endCharacter_; ++nCharacter)
	{
		unsigned int characterWidth = 0u;
		unsigned int characterHeight = 0u;
		int characterBearingX = 0;
		int characterBearingY = 0;
		unsigned int characterAdvanceX = 0u;

		const FT_UInt charIndex = FT_Get_Char_Index(ftFace, FT_ULong(nCharacter));

		if (charIndex != FT_UInt(0))
		{
			const FT_Error loadError = FT_Load_Glyph(ftFace, charIndex, FT_LOAD_DEFAULT);

			if (loadError == FT_Err_Ok)
			{
				ocean_assert(ftFace->glyph != nullptr);

				// the glyph's metrics values are specified as 26.6 fixed-point

				const int signedWidth = int(ftFace->glyph->metrics.width >> 6);
				const int signedHeight = int(ftFace->glyph->metrics.height >> 6);

				characterBearingX = int(ftFace->glyph->metrics.horiBearingX >> 6);
				characterBearingY = int(ftFace->glyph->metrics.horiBearingY >> 6);

				const int signedAdvance = int(ftFace->glyph->metrics.horiAdvance >> 6);

				if (signedWidth >= 0 && signedHeight >= 0 && signedAdvance > 0)
				{
					characterWidth = (unsigned int)(signedWidth);
					characterHeight = (unsigned int)(signedHeight);
					characterAdvanceX = (unsigned int)(signedAdvance);

					maxCharacterY = std::max(maxCharacterY, characterBearingY);
					minCharacterY = std::min(minCharacterY, characterBearingY - signedHeight);
				}
			}
		}

		constexpr unsigned int framePositionY = 0u; // not yet known
		constexpr unsigned int linePositionY = 0u;
		characters_->emplace_back(nCharacter, framePositionX + extraBorder_, framePositionY, linePositionY, characterWidth, characterHeight, characterBearingX, characterBearingY, characterAdvanceX);

		framePositionX += (characterWidth == 0u) ? characterAdvanceX : characterWidth; // for empty characters like 'space'
		framePositionX += extraBorder_ * 2u; // left + right border
	}

	if (framePositionX == 0u)
	{
		return false;
	}

	ocean_assert(minCharacterY < maxCharacterY);

	const unsigned int maxCharacterHeight = (unsigned int)(maxCharacterY - minCharacterY) + extraBorder_ * 2u;

	// now, we can render the glyphs

	for (unsigned int nCharacter = firstCharacter_; nCharacter < endCharacter_; ++nCharacter)
	{
		Character& character = (*characters_)[nCharacter - firstCharacter_];

		const FT_UInt charIndex = FT_Get_Char_Index(ftFace, FT_ULong(nCharacter));

		if (charIndex != FT_UInt(0))
		{
			const FT_Error loadError = FT_Load_Glyph(ftFace, charIndex, FT_LOAD_RENDER | FT_LOAD_COLOR);

			if (loadError == FT_Err_Ok)
			{
				const FT_Bitmap& bitmap = ftFace->glyph->bitmap;

				if (!charactersFrame_.isValid())
				{
					FrameType::PixelFormat pixelFormat = FrameType::FORMAT_UNDEFINED;

					if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
					{
						pixelFormat = FrameType::FORMAT_Y8;
					}
					else if (bitmap.pixel_mode == FT_PIXEL_MODE_BGRA)
					{
						pixelFormat = FrameType::FORMAT_RGBA32;
					}
					else
					{
						return false;
					}

					if (!charactersFrame_.set(FrameType(framePositionX, maxCharacterHeight, pixelFormat, FrameType::ORIGIN_UPPER_LEFT), true /*forceOwner*/, true /*forceWritable*/))
					{
						ocean_assert(false && "This should never happen!");

						return false;
					}

					charactersFrame_.setValue(0x00);
				}

				ocean_assert(character.width() == ftFace->glyph->bitmap.width);
				ocean_assert(character.height() == ftFace->glyph->bitmap.rows);

				const unsigned int linePositionY = maxCharacterY - character.bearingY();
				const unsigned int framePositionY = linePositionY + extraBorder_;

				if (!drawCharacterBitmapToFrame(ftFace->glyph->bitmap, charactersFrame_, character.framePositionX(), framePositionY))
				{
					return false;
				}

				character.setPositionY(framePositionY, linePositionY);
			}
		}
	}

	return charactersFrame_.isValid();
}

bool FTFont::drawCharacterBitmapToFrame(const FT_Bitmap& bitmap, Frame& frame, const unsigned int framePositionX, const unsigned int framePositionY)
{
	static_assert(sizeof(unsigned char) == 1, "Invalid data type!");

	ocean_assert(frame.isValid());
	ocean_assert(frame.pixelOrigin() == FrameType::ORIGIN_UPPER_LEFT);

	if (bitmap.width == 0u || bitmap.rows == 0u)
	{
		// simply an empty bitmap
		return true;
	}

	if (bitmap.pitch < 0)
	{
		return false;
	}

	ocean_assert(bitmap.buffer != nullptr);
	ocean_assert(framePositionX + bitmap.width <= frame.width());
	ocean_assert(framePositionY + bitmap.rows <= frame.height());

	const unsigned int bitmapStrideBytes = (unsigned int)(bitmap.pitch);

	if (bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
	{
		ocean_assert(frame.pixelFormat() == FrameType::FORMAT_Y8);
		ocean_assert(bitmap.num_grays == 256);

		for (unsigned int y = 0u; y < bitmap.rows; ++y)
		{
			const unsigned char* bitmapRow = bitmap.buffer + y * bitmapStrideBytes;
			uint8_t* const frameRow = frame.row<uint8_t>(framePositionY + y) + framePositionX * 1u;

			memcpy(frameRow, bitmapRow, bitmap.width);
		}
	}
	else if (bitmap.pixel_mode == FT_PIXEL_MODE_BGRA)
	{
		ocean_assert(frame.pixelFormat() == FrameType::FORMAT_RGBA32);

		for (unsigned int y = 0u; y < bitmap.rows; ++y)
		{
			const unsigned char* bitmapRow = bitmap.buffer + y * bitmapStrideBytes;
			uint8_t* const frameRow = frame.row<uint8_t>(framePositionY + y) + framePositionX * 4u;

			// converting from BGRA32 to RGBA32
			CV::FrameChannels::shuffleRowChannels<unsigned char, 4u, 4u, 0x3012u>(bitmapRow, frameRow, bitmap.width);
		}
	}
	else
	{
		ocean_assert(false && "This should never happen!");
		return false;
	}

	return true;
}

template <typename T, unsigned int tTargetChannels>
void FTFont::renderCharacterFromY8(const uint8_t* ySourceFrameData, T* targetFrameData, const unsigned int width, const unsigned int height, const T* const foregroundColor, const unsigned int ySourceFrameStrideElements, const unsigned int targetFrameStrideElements)
{
	static_assert(tTargetChannels != 0u, "Invalid channel number!");

	ocean_assert(ySourceFrameData != nullptr);
	ocean_assert(targetFrameData != nullptr);
	ocean_assert(width != 0u && height != 0u);
	ocean_assert(foregroundColor != nullptr);

	ocean_assert(ySourceFrameStrideElements >= width);
	ocean_assert(targetFrameStrideElements >= width * tTargetChannels);

	for (unsigned int y = 0u; y < height; ++y)
	{
		for (unsigned int x = 0u; x < width; ++x)
		{
			for (unsigned int c = 0u; c < tTargetChannels; ++c)
			{
				targetFrameData[x * tTargetChannels + c] = (foregroundColor[c] * ySourceFrameData[x] + targetFrameData[x * tTargetChannels + c] * (uint8_t(0xFFu) - ySourceFrameData[x])) / 0xFFu;
			}
		}

		targetFrameData += targetFrameStrideElements;
		ySourceFrameData += ySourceFrameStrideElements;
	}
}

template <typename T, unsigned int tChannels>
void FTFont::renderBackground(T* frameData, const unsigned int backgroundWidth, const unsigned int backgroundHeight, const T* const backgroundColor, const unsigned int frameStrideElements)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frameData != nullptr);
	ocean_assert(backgroundWidth != 0u && backgroundHeight != 0u);
	ocean_assert(backgroundColor != nullptr);

	ocean_assert(frameStrideElements >= backgroundWidth * tChannels);

	for (unsigned int y = 0u; y < backgroundHeight; ++y)
	{
		for (unsigned int x = 0u; x < backgroundWidth; ++x)
		{
			for (unsigned int c = 0u; c < tChannels; ++c)
			{
				frameData[x * tChannels + c] = backgroundColor[c];
			}
		}

		frameData += frameStrideElements;
	}
}

} // namespace Fonts

} // namespace CV

} // namespace Ocean
