/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_FONTS_FONT_H
#define META_OCEAN_CV_FONTS_FONT_H

#include "ocean/cv/fonts/Fonts.h"

#include "ocean/base/Frame.h"

#include "ocean/cv/PixelBoundingBox.h"

namespace Ocean
{

namespace CV
{

namespace Fonts
{

/// Forward declaration.
class Font;

/**
 * Definition of a shared font pointer.
 * @see Font.
 * @ingroup cvfonts
 */
typedef std::shared_ptr<Font> SharedFont;

/**
 * This class is the base class for all font implementations.
 * Each font object must be acquired via the `FontManager` singleton.
 * @see FontManager.
 * @ingroup cvfonts
 */
class OCEAN_CV_FONTS_EXPORT Font
{
	public:

		/**
		 * This class stores font-specific layout information about each individual character.
		 * The class does not store the actual visual information (appearance) of each character.<br>
		 * The visual information of all characters is stored in a lookup image.
		 */
		class Character
		{
			public:

				/**
				 * Creates a new invalid character object.
				 */
				inline Character();

				/**
				 * Creates the a new character object.
				 * @param characterIndex The index of the new character, with range [0, infinity)
				 * @param framePositionX The character's left pixel in 'charactersFrame', in pixels, with range [0, charactersFrame_.width() - 1]
				 * @param framePositionY The character's top pixel in 'charactersFrame', in pixels, with range [0, charactersFrame_.height() - 1]
				 * @param linePositionY The character's vertical start pixel inside the target text line, in pixels, with range [0, framePositionY]
				 * @param width The character's width, in pixels, with range [0, charactersFrame_.width()]
				 * @param height The character's hieght, in pixels, with range [0, charactersFrame_.height()]
				 * @param bearingX The character's horizontal offset, in pixels, with range (-infinity, infinity)
				 * @param bearingY The character's vertical offset, in pixels, with range (-infinity, infinity)
				 * @param advanceX The horizontal distance between this character and the next character (between the origins of both characters), in pixels, with range [0, infinity)
				 */
				inline Character(const unsigned int characterIndex, const unsigned int framePositionX, const unsigned int framePositionY, const unsigned int linePositionY, const unsigned int width, const unsigned int height, const int bearingX, const int bearingY, const unsigned int advanceX);

				/**
				 * Returns the index of this character.
				 * @return The character's index, with range [0, infinity)
				 */
				inline unsigned int characterIndex() const;

				/**
				 * Returns the horizontal start pixel of the character inside 'charactersFrame'.
				 * @return The character's left pixel in 'charactersFrame', in pixels, with range [0, charactersFrame_.width() - 1]
				 */
				inline unsigned int framePositionX() const;

				/**
				 * Returns the vertical start pixel of the character inside 'charactersFrame'.
				 * @return The character's top pixel in 'charactersFrame', in pixels, with range [0, charactersFrame_.height() - 1]
				 */
				inline unsigned int framePositionY() const;

				/**
				 * Returns the vertical start pixel of the character inside the target text line.
				 * This vertical start position may be different from 'framePositionY()' in case the 'charactersFrame_' contains some extra border around each character.
				 * @return The character's top pixel in the target text line, in pixel, with range [0, framePositionY()]
				 */
				inline unsigned int linePositionY() const;

				/**
				 * The width of the character.
				 * @return The character's width, in pixels, with rang [0, charactersFrame_.width()]
				 */
				inline unsigned int width() const;

				/**
				 * The height of the character.
				 * @return The character's hieght, in pixels, with rang [0, charactersFrame_.height()]
				 */
				inline unsigned int height() const;

				/**
				 * Returns the horizontal offset of the character in relation to the origin.
				 * @return The character's horizontal offset, in pixels, with range (-infinity, infinity)
				 */
				inline int bearingX() const;

				/**
				 * Returns the vertical offset of the character in relation to the origin.
				 * @return The character's vertical offset, in pixels, with range (-infinity, infinity)
				 */
				inline int bearingY() const;

				/**
				 * Returns the horizontal distance between this character and the next character (between the origins of both characters).
				 * @return The character's horizontal distance, in pixels, with range [0, infinity)
				 */
				inline unsigned int advanceX() const;

				/**
				 * Sets the vertical start pixel of the character inside 'charactersFrame'.
				 * @param framePositionY The vertical start position, in pixels, with range [0, charactersFrame_.height() - 1]
				 * @param linePositionY The character's top pixel in the target text line, in pixel, with range [0, framePositionY]
				 */
				inline void setPositionY(const unsigned int framePositionY, const unsigned int linePositionY);

			protected:

				/// The character's index, with range [0, infinity).
				unsigned int characterIndex_;

				/// The character's left pixel in 'charactersFrame', in pixels, with range [0, charactersFrame_.width() - 1].
				unsigned int framePositionX_;

				/// The character's top pixel in 'charactersFrame', in pixels, with range [0, charactersFrame_.height() - 1].
				unsigned int framePositionY_;

				/// The character's top pixel in the target text line, in pixel, with range [0, font.height() - 1]
				unsigned int linePositionY_;

				/// The character's width, in pixels, with rang [0, charactersFrame_.width()].
				unsigned int width_;

				/// The character's hieght, in pixels, with rang [0, charactersFrame_.height()].
				unsigned int height_;

				///  The character's horizontal offset, in pixels, with range (-infinity, infinity).
				int bearingX_;

				//// The character's vertical offset, in pixels, with range (-infinity, infinity).
				int bearingY_;

				/// The horizontal distance between this character and the next character (between the origins of both characters), in pixels, with range [0, infinity).
				unsigned int advanceX_;
		};

		/// Definition of a vector holding information about characters.
		typedef std::vector<Character> Characters;

		/// Definition of a shared pointers holding characters.
		typedef std::shared_ptr<Characters> SharedCharacters;

	public:

		/**
		 * Destructs the font object.
		 */
		virtual ~Font() = default;

		/**
		 * Draws a text into a given frame.
		 * @param frame The frame in which the text will be drawn, must have `dataType() == DT_UNSIGNED_INTEGER_8`, must be valid
		 * @param text The text to be drawn, can be empty
		 * @param left The horizontal start position of the text, with range (-infinity, infinity)
		 * @param top The vertical start positoin of the text, with range (-infinity, infinity)
		 * @param foregroundColor The foreground color to be used, must match with the pixel format of the given frame
		 * @param backgroundColor Optional explicit background color of the text, nullptr to draw the text without background color
		 * @return True, if succeeded
		 */
		virtual bool drawText(Frame& frame, const std::string& text, const int left, const int top, const uint8_t* foregroundColor, const uint8_t* backgroundColor = nullptr) const = 0;

		/**
		 * Returns the bounding box in which a given text will fit.
		 * In case the text contains multiple lines, the maximal bounding box is determined.
		 * @param text The text for which the extent will be determined, can be empty
		 * @param width The width of the bounding box, in pixel, with range [0, infinity)
		 * @param height The height of the bounding box, in pixel, with range [0, infinity)
		 * @param left Optional resulting explicit horizontal start location of the bounding box (e.g., if parts of a character are drawn to the left of character's origin), nullptr otherwise
		 * @param top Optional resulting explicit vertical start location of the bounding box, nullptr otherwise
		 * @return True, if succeeded
		 */
		virtual bool textExtent(const std::string& text, unsigned int& width, unsigned int& height, int* left = nullptr, int* top = nullptr) const = 0;

		/**
		 * Returns the bounding boxes in which each individual line of a given text will fit.
		 * @param text The text for which the individual line extents will be determined, can be empty
		 * @param lineBoundingBoxes The resulting bounding boxes, one for each text line
		 * @return True, if succeeded
		 */
		virtual bool textExtent(const std::string& text, PixelBoundingBoxesI& lineBoundingBoxes) const = 0;

		/**
		 * Returns a frame containing all characters.
		 * @param frame The resulting frame containing all characters, the frame will own the memory
		 * @param characters Optional resulting layout information for each individual character
		 * @return True, if succeeded
		 */
		virtual bool characterFrame(Frame& frame, SharedCharacters* characters = nullptr) const = 0;

		/**
		 * Returns the size of the font in dots.
		 * @return The font's size in dots, with range [0, infinity)
		 */
		inline unsigned int size() const;

		/**
		 * Returns the height of the font in pixels.
		 * @return The font's height in pixel, with range [0, infinity)
		 */
		inline unsigned int height() const;

		/**
		 * Returns the dpi of the font.
		 * @return The font's dots per inch, with range [0, infinity)
		 */
		inline unsigned int dpi() const;

		/**
		 * Returns whether the font is valid and whether it can be used.
		 * @return True, if so
		 */
		inline bool isValid() const;

	protected:

		/**
		 * Creates a new font.
		 */
		inline Font();

	protected:

		/// The font's size in dots, with range [0, infinity).
		unsigned int size_;

		/// The font's height in pixel, with range [0, infinity).
		unsigned int height_;

		/// The font's dots per inch, with range [0, infinity).
		unsigned int dpi_;

		/// True, if the font is valid.
		bool isValid_;
};

inline Font::Character::Character() :
	characterIndex_((unsigned int)(-1)),
	framePositionX_(0u),
	framePositionY_(0u),
	linePositionY_(0u),
	width_(0u),
	height_(0u),
	bearingX_(0),
	bearingY_(0),
	advanceX_(0u)
{
	// nothing to do here
}

inline Font::Character::Character(const unsigned int characterIndex, const unsigned int framePositionX, const unsigned int framePositionY, const unsigned int linePositionY, const unsigned int width, const unsigned int height, const int bearingX, const int bearingY, const unsigned int advanceX) :
	characterIndex_(characterIndex),
	framePositionX_(framePositionX),
	framePositionY_(framePositionY),
	linePositionY_(linePositionY),
	width_(width),
	height_(height),
	bearingX_(bearingX),
	bearingY_(bearingY),
	advanceX_(advanceX)
{
	// nothing to do here
}

inline unsigned int Font::Character::characterIndex() const
{
	ocean_assert(characterIndex_ != (unsigned int)(-1));
	return characterIndex_;
}

inline unsigned int Font::Character::framePositionX() const
{
	return framePositionX_;
}

inline unsigned int Font::Character::framePositionY() const
{
	return framePositionY_;
}

inline unsigned int Font::Character::linePositionY() const
{
	return linePositionY_;
}

inline unsigned int Font::Character::width() const
{
	return width_;
}

inline unsigned int Font::Character::height() const
{
	return height_;
}

inline int Font::Character::bearingX() const
{
	return bearingX_;
}

inline int Font::Character::bearingY() const
{
	return bearingY_;
}

inline unsigned int Font::Character::advanceX() const
{
	return advanceX_;
}

inline void Font::Character::setPositionY(const unsigned int framePositionY, const unsigned int linePositionY)
{
	ocean_assert(linePositionY <= framePositionY);

	framePositionY_ = framePositionY;
	linePositionY_ = linePositionY;
}

inline Font::Font() :
	size_(0u),
	height_(0u),
	dpi_(0u),
	isValid_(false)
{
	// nothing to do here
}

inline unsigned int Font::size() const
{
	ocean_assert(isValid());

	return size_;
}

inline unsigned int Font::height() const
{
	ocean_assert(isValid());

	return height_;
}

inline unsigned int Font::dpi() const
{
	ocean_assert(isValid());

	return dpi_;
}

inline bool Font::isValid() const
{
	return isValid_;
}

}

}

}

#endif // META_OCEAN_CV_FONTS_FONT_H
