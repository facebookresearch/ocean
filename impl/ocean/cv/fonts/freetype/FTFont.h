/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FONTS_FREETYPE_FT_FONT_H
#define META_OCEAN_CV_FONTS_FREETYPE_FT_FONT_H

#include "ocean/cv/fonts/freetype/FreeType.h"

#include "ocean/cv/fonts/Font.h"

namespace Ocean
{

namespace CV
{

namespace Fonts
{

/**
 * This class implements a font based on FreeType.
 * Each font object must be acquired via the `Fonts::FontManager` singleton.
 * @ingroup cvfonts
 */
class OCEAN_CV_FONTS_EXPORT FTFont : public Font
{
	friend class FTFontManager;

	protected:

		/**
		 * The index of the first supported character, which is a space in ASCII code.
		 */
		static constexpr char firstCharacter_ = 32;

		/**
		 * The index of the character after the last supported character to cover the standard ASCII code.
		 */
		static constexpr char endCharacter_ = 127;

	public:

		/**
		 * Draws a text into a given frame.
		 * @param frame The frame in which the text will be drawn, must have `dataType() == DT_UNSIGNED_INTEGER_8`, must be valid
		 * @param text The text to be drawn, can be empty
		 * @param left The horizontal start position of the text, with range (-infinity, infinity)
		 * @param top The vertical start position of the text, with range (-infinity, infinity)
		 * @param foregroundColor The foreground color to be used, must match with the pixel format of the given frame
		 * @param backgroundColor Optional explicit background color of the text, nullptr to draw the text without background color
		 * @return True, if succeeded
		 */
		bool drawText(Frame& frame, const std::string& text, const int left, const int top, const uint8_t* foregroundColor, const uint8_t* backgroundColor = nullptr) const override;

		/**
		 * Returns the bounding box a given text will occupy in pixel space when using this font.
		 * @param text The text for which the extent will be determined, can be empty
		 * @param width The width of the bounding box, in pixels, with range [0, infinity)
		 * @param height The height of the bounding box, in pixels, with range [0, infinity)
		 * @param left Optional resulting explicit horizontal start location of the bounding box (e.g., if parts of a character are drawn to the left of character's origin), nullptr otherwise
		 * @param top Optional resulting explicit vertical start location of the bounding box, nullptr otherwise
		 * @return True, if succeeded
		 */
		bool textExtent(const std::string& text, unsigned int& width, unsigned int& height, int* left = nullptr, int* top = nullptr) const override;

		/**
		 * Returns the bounding boxes in which each individual line of a given text will fit.
		 * @param text The text for which the individual line extents will be determined, can be empty
		 * @param lineBoundingBoxes The resulting bounding boxes, one for each text line
		 * @return True, if succeeded
		 */
		bool textExtent(const std::string& text, PixelBoundingBoxesI& lineBoundingBoxes) const override;

		/**
		 * Returns a frame containing all characters.
		 * @param frame The resulting frame containing all characters, the frame will own the memory
		 * @param characters Optional resulting layout information for each individual character
		 * @return True, if succeeded
		 */
		bool characterFrame(Frame& frame, SharedCharacters* characters = nullptr) const override;

	protected:

		/**
		 * Creates a new FreeType font object.
		 * @param ftFace The FreeType face object describing the font, will be release when this new font is disposed.
		 * @param size The size of the font, in dots, with range [1, infinity)
		 * @param dpi The dots per inch of the font, with range [1, infinity)
		 */
		explicit FTFont(const FT_Face& ftFace, const unsigned int size, const unsigned int dpi);

		/**
		 * Initializes the font.
		 * @param ftFace The FreeType face defining the font, must be valid
		 * @return True, if succeeded
		 */
		bool initialize(const FT_Face& ftFace);

		/**
		 * Draws the bitmap of a FreeType character to the frame storing all characters/glyphs.
		 * @param bitmap The bitmap of the character to be drawn
		 * @param frame the frame storing all characters, must be valid
		 * @param framePositionX The horizontal start position within the frame, in pixels, with range [0, frame.width() - bitmap.width - 1]
		 * @param framePositionY The horizontal start position within the frame, in pixels, with range [0, frame.width() - bitmap.rows - 1]
		 * @return True, if succeeded
		 */
		static bool drawCharacterBitmapToFrame(const FT_Bitmap& bitmap, Frame& frame, const unsigned int framePositionX, const unsigned int framePositionY);

		/**
		 * Renders a font character from the grayscale image storing all characters to a target image.
		 * @param ySourceFrameData The grayscale source image storing all characters, with pixel format FORMAT_Y8, must be valid
		 * @param targetFrameData The pointer to the top left corner within the target frame to which the character will be rendered, must be valid
		 * @param width The width of the area to be rendered, can be a sub-region of the actual character (e.g., at the boundary of the target image), with range [1, infinity)
		 * @param height The height of the area to be rendered, can be a sub-region of the actual character (e.g., at the boundary of the target image), with range [1, infinity)
		 * @param foregroundColor The foreground color of the character, must match with the pixel format of the target frame, must be valid
		 * @param ySourceFrameStrideElements The number of elements between two consecutive rows in the source image (including padding), in elements, with range [width, infinity)
		 * @param targetFrameStrideElements The number of elements between two consecutive rows in the target image (including padding), in elements, with range [width * tTargetChannels, infinity)
		 * @tparam T The data type of each target frame pixel element
		 * @tparam tTargetChannels The number of channels in the target frame, with range [1, infinity)
		 */
		template <typename T, unsigned int tTargetChannels>
		static void renderCharacterFromY8(const uint8_t* ySourceFrameData, T* targetFrameData, const unsigned int width, const unsigned int height, const T* const foregroundColor, const unsigned int ySourceFrameStrideElements, const unsigned int targetFrameStrideElements);

		/**
		 * Draws the background color into a rectangular area within the target frame.
		 * @param frameData The pointer to the top left corners of the rectangular area within the target frame, must be valid
		 * @param backgroundWidth The width of the rectangular background area, in pixels, with range [1, infinity)
		 * @param backgroundHeight The height of the rectangular background area, in pixels, with range [1, infinity)
		 * @param backgroundColor The background color to be used, must be valid
		 * @param frameStrideElements The number of elements between two consecutive frame rows (including padding), in elements, with range [backgroundWidth * tChannels, infinity)
		 * @tparam T The data type of each frame pixel element
		 * @tparam tChannels The number of frame channels, with range [1, infinity)
		 */
		template <typename T, unsigned int tChannels>
		static void renderBackground(T* frameData, const unsigned int backgroundWidth, const unsigned int backgroundHeight, const T* const backgroundColor, const unsigned int frameStrideElements);

	protected:

		/// The information for each individual character in `charactersFrame_`.
		SharedCharacters characters_;

		/// The character frame which contains all characters of the font, with pixel format FORMAT_Y8 or FORMAT_RGBA32.
		Frame charactersFrame_;

		/// The optional extra border around each glyph in the character frame to avoid that glyphs are too close together, in pixel, with range [0, infinity)
		constexpr static unsigned int extraBorder_ = 1u;
};

}

}

}

#endif // META_OCEAN_CV_FONTS_FREETYPE_FT_FONT_H
