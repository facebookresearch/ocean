/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_CV_CANVAS_H
#define META_OCEAN_CV_CANVAS_H

#include "ocean/cv/CV.h"

#include "ocean/base/DataType.h"
#include "ocean/base/Frame.h"
#include "ocean/base/Singleton.h"

#include "ocean/cv/Bresenham.h"
#include "ocean/cv/PixelPosition.h"

#include "ocean/math/Box2.h"
#include "ocean/math/FiniteLine2.h"
#include "ocean/math/Line2.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/Vector3.h"

namespace Ocean
{

namespace CV
{

/**
 * This class implements canvas functions.
 * @ingroup cv
 */
class OCEAN_CV_EXPORT Canvas
{
	public:

		/**
		 * The following comfort class provides comfortable functions simplifying prototyping applications but also increasing binary size of the resulting applications.
		 * Best practice is to avoid using these functions if binary size matters,<br>
		 * as for every comfort function a corresponding function exists with specialized functionality not increasing binary size significantly.<br>
		 */
		class OCEAN_CV_EXPORT Comfort
		{
			public:

				/**
				 * Paints a point with sub-pixel accuracy.
				 * @param frame The frame in which the point will be painted, must be valid
				 * @param position Sub-pixel position of the point, with range (-infinity, infinity)x(-infinity, infinity)
				 * @param pixelCenter The pixel center of the provided position, either 'PC_TOP_LEFT' or 'PC_CENTER'
				 * @param size The size (width) of the point, in pixel, with range [1, 15] and must be odd
				 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
				 * @return True, if succeeded
				 */
				static bool point(Frame& frame, const Vector2& position, const PixelCenter pixelCenter, const unsigned int size, const uint8_t* value = nullptr);
		};

	protected:

		/**
		 * This class implements a helper class that provides binomial filter parameters for a specific filter size.
		 * @tparam tFilterSize Size of the binomial 1D filter, with range [1, infinity) and must be odd
		 */
		template <unsigned int tFilterSize>
		class FilterFactors
		{
			public:

				/**
				 * Creates a new filter factor object.
				 */
				inline FilterFactors();

				/**
				 * Returns the filter factor for a specific index.
				 * @param index The index of the filter parameter, with range [0, tFilterSize)
				 * @return Filter parameter
				 */
				inline unsigned int factor(const unsigned int index) const;

				/**
				 * Returns the filter factor for a specific index.
				 * If the index lies outside the filter size, than zero is returned.<br>
				 * @param index The index of the filter parameter, with range (-infinity, infinity)
				 * @return Filter parameter, or zero
				 */
				inline unsigned int clampedFactor(const int index) const;

				/**
				 * Returns the maximal (center) filter parameter for the specified filter size.
				 * @return Maximal filter parameter
				 */
				inline unsigned int maximalFactor() const;

			protected:

				/// The individual filter factor.
				unsigned int factors_[tFilterSize];

				/// The maximal filter factor.
				unsigned int maximalFactor_ = 0u;
		};

		/**
		 * This class implements a standard font similar to a code-block like font.
		 */
		class OCEAN_CV_EXPORT Font : public Singleton<Font>
		{
			friend class Singleton<Font>;

			public:

				/**
				 * This class implements one character of a font for which each pixel has as size of at most 16x16.
				 */
				class OCEAN_CV_EXPORT Character
				{
					public:

						/**
						 * Creates an invalid character object.
						 */
						Character();

						/**
						 * Creates a new character from a given image in which the actual character is visible.
						 * @param frame The frame in which the character is visible, with maximal resolution 16x16
						 * @param color The color of the character, must be valid
						 */
						Character(const Frame& frame, const uint8_t* color);

						/**
						 * Creates a new frame and paints the character in the frame.
						 * The size of the frame fits with the size of the character.
						 * @param pixelFormat The pixel format of the resulting frame, must be a generic pixel format like e.g., FORMAT_Y8 or FORMAT_RGB24
						 * @param foregroundColor The foreground color to be used when painting the character, must be valid
						 * @param backgroundColor The background color to be used when paining the character, must be valid
						 * @return The frame with character
						 */
						Frame frame(const FrameType::PixelFormat pixelFormat, const uint8_t* foregroundColor, const uint8_t* backgroundColor) const;

						/**
						 * Paints the character at the specific location in a given frame.
						 * The given location can be partially outside of the frame.
						 * @param frame The frame in which the character will be painted, must have a generic pixel format like e.g., FORMAT_Y8 or FORMAT_RGB24, must be valid
						 * @param left The horizontal start position of the character within the frame, in pixel, with range (-infinity, infinity)
						 * @param top The vertical start position of the character within the frame, in pixel, with range (-infinity, infinity)
						 * @param foregroundColor The foreground color to be used when painting the character, must be valid
						 * @param backgroundColor The optional background color to be used when paining the character, nullptr to paint the foreground pixels only
						 * @return True, if succeeded
						 */
						bool paint(Frame& frame, const int left, const int top, const uint8_t* foregroundColor, const uint8_t* backgroundColor = nullptr) const;

						/**
						 * Returns the width of this character in pixel.
						 * @return The characters width in pixel, with range [0, 16]
						 */
						unsigned int width() const;

						/**
						 * Returns the height of this character in pixel.
						 * @return The characters height in pixel, with range [0, 16]
						 */
						unsigned int height() const;

						/**
						 * Returns whether this character holds valid data.
						 */
						bool isValid() const;

					protected:

						/// The data defining the character: 1 byte width, 1 byte height, 32 bytes = 16 * 16 bits for the visual information.
						uint8_t data_[2 + 32];
				};

				/**
				 * Definition of a vector holding characters.
				 */
				typedef std::vector<Character> Characters;

			public:

				/**
				 * Paints a given text into a given frame using this font.
				 * The given location can be partially outside of the frame.
				 * @param frame The frame in which the text will be painted, must have a generic pixel format like e.g., FORMAT_Y8 or FORMAT_RGB24, must be valid
				 * @param text The text to be painted, the text can contain end-of-line characters to paint the remaining the following characters in the next line, can be empty
				 * @param left The horizontal start position of the text within the frame, in pixel, with range (-infinity, infinity)
				 * @param top The vertical start position of the text within the frame, in pixel, with range (-infinity, infinity)
				 * @param foregroundColor The foreground color to be used when painting the character, must be valid
				 * @param backgroundColor The optional background color to be used when paining the character, nullptr to paint the foreground pixels only
				 * @return True, if succeeded
				 */
				bool drawText(Frame& frame, const std::string& text, const int left, const int top, const uint8_t* foregroundColor, const uint8_t* backgroundColor = nullptr) const;

				/**
				 * Returns the bounding box a given text will occupy in pixel space when using this font.
				 * @param text The text for which the extent will be determined, can be empty
				 * @param width The width of the bounding box, in pixel, with range [0, infinity)
				 * @param height The height of the bounding box, in pixel, with range [0, infinity)
				 * @return True, if succeeded
				 */
				bool textExtent(const std::string& text, unsigned int& width, unsigned int& height) const;

			protected:

				/**
				 * The protected constructor creating a new font.
				 */
				Font();

			protected:

				/**
				 * The characters of this font.
				 */
				Characters characters_;
		};

	public:

		/**
		 * Returns the color values for a white color.
		 * Values of alpha channel are set to 0xFF.
		 * @param pixelFormat The pixel format for which the color will be returned
		 * @return The requested color value matching with the pixel format
		 */
		static const uint8_t* white(const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_RGB24);

		/**
		 * Returns the color values for a black color.
		 * Values of alpha channel are set to 0xFF.
		 * @param pixelFormat The pixel format for which the color will be returned
		 * @return The requested color value matching with the pixel format
		 */
		static const uint8_t* black(const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_RGB24);

		/**
		 * Returns the color values for a gray color.
		 * Values of alpha channel are set to 0xFF.
		 * @param pixelFormat The pixel format for which the color will be returned
		 * @return The requested color value matching with the pixel format
		 */
		static const uint8_t* gray(const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_RGB24);

		/**
		 * Returns the color values for a red color.
		 * Values of alpha channel are set to 0xFF.
		 * @param pixelFormat The pixel format for which the color will be returned
		 * @return The requested color value matching with the pixel format
		 */
		static const uint8_t* red(const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_RGB24);

		/**
		 * Returns the color values for a green color.
		 * Values of alpha channel are set to 0xFF.
		 * @param pixelFormat The pixel format for which the color will be returned
		 * @return The requested color value matching with the pixel format
		 */
		static const uint8_t* green(const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_RGB24);

		/**
		 * Returns the color values for a blue color.
		 * Values of alpha channel are set to 0xFF.
		 * @param pixelFormat The pixel format for which the color will be returned
		 * @return The requested color value matching with the pixel format
		 */
		static const uint8_t* blue(const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_RGB24);

		/**
		 * Returns the color values for a yellow color.
		 * Values of alpha channel are set to 0xFF.
		 * @param pixelFormat The pixel format for which the color will be returned
		 * @return The requested color value matching with the pixel format
		 */
		static const uint8_t* yellow(const FrameType::PixelFormat pixelFormat = FrameType::FORMAT_RGB24);

		/**
		 * Paints a line with specified start and end position with pixel accuracy.
		 * The specified start and end positions will be included.
		 * @param frame The frame in which the line will be painted, must be valid
		 * @param xStart Horizontal start position, with range (-infinity, infinity)
		 * @param yStart Vertical start position, with range (-infinity, infinity)
		 * @param xEnd Horizontal end position, with range (-infinity, infinity)
		 * @param yEnd Vertical end position, with range (-infinity, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @see lines().
		 */
		static bool line(Frame& frame, const int xStart, const int yStart, const int xEnd, const int yEnd, const uint8_t* value = nullptr);

		/**
		 * Paints several lines with specified start and end positions with pixel accuracy.
		 * The specified start and end positions will be included.
		 * @param frame The frame in which the line will be painted, must be valid
		 * @param positions Interleaved start and end positions of the lines, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param numberPositions Number of provided positions, should be even - otherwise the last line will not be painted
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 * @see line().
		 */
		static bool lines(Frame& frame, const PixelPosition* positions, const unsigned int numberPositions, const uint8_t* value = nullptr);

		/**
		 * Paints a line with specified start and end position with pixel accuracy.
		 * The specified start and end positions will be included.
		 * @param frame The frame in which the line will be painted, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param xStart Horizontal start position, with range (-infinity, infinity)
		 * @param yStart Vertical start position, with range (-infinity, infinity)
		 * @param xEnd Horizontal end position, with range (-infinity, infinity)
		 * @param yEnd Vertical end position, with range (-infinity, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param framePaddingElements Optional number of padding elements at the end of each row, in elements, with range [0, infinity)
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 * @see lines8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static void line8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const int xStart, const int yStart, const int xEnd, const int yEnd, const uint8_t* value = nullptr, const unsigned int framePaddingElements = 0u);

		/**
		 * Paints several lines with specified start and end positions with pixel accuracy.
		 * The specified start and end positions will be included.
		 * @param frame The frame in which the line will be painted, must be valid
		 * @param width The width of the frame in pixel, with range [0, infinity)
		 * @param height The height of the frame in pixel, with range [0, infinity)
		 * @param positions Interleaved start and end positions of the lines, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param numberPositions Number of provided positions, should be even - otherwise the last line will not be painted
		 * @param value Color value to be used, nullptr to apply 0x00 for each channel
		 * @param framePaddingElements Optional number of padding elements at the end of each row, with range [0, infinity)
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 * @see line8BitPerChannel().
		 */
		template <unsigned int tChannels>
		static void lines8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const PixelPosition* positions, const unsigned int numberPositions, const uint8_t* value = nullptr, const unsigned int framePaddingElements = 0u);

		/**
		 * Paints a line with specified start and end position with sub-pixel accuracy.
		 * The specified start and end positions will be included.
		 * @param frame The frame in which the line will be painted, must be valid
		 * @param xStart Horizontal start position, with range (-infinity, infinity)
		 * @param yStart Vertical start position, with range (-infinity, infinity)
		 * @param xEnd Horizontal end position, with range (-infinity, infinity)
		 * @param yEnd Vertical end position, with range (-infinity, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 * @see lines().
		 */
		template <unsigned int tSize>
		static bool line(Frame& frame, const Scalar xStart, const Scalar yStart, const Scalar xEnd, const Scalar yEnd, const uint8_t* value = nullptr);

		/**
		 * Paints a line with specified start and end position with sub-pixel accuracy.
		 * The specified start and end positions will be included.
		 * @param frame The frame receiving the line, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param xStart Horizontal start position, with range (-infinity, infinity)
		 * @param yStart Vertical start position, with range (-infinity, infinity)
		 * @param xEnd Horizontal end position, with range (-infinity, infinity)
		 * @param yEnd Vertical end position, with range (-infinity, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param framePaddingElements Optional number of padding elements at the end of each row, with range [0, infinity)
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 * @see lines8BitPerChannel().
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static void line8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Scalar xStart, const Scalar yStart, const Scalar xEnd, const Scalar yEnd, const uint8_t* value = nullptr, const unsigned int framePaddingElements = 0u);

		/**
		 * Paints a line with specified start and end position with sub-pixel accuracy.
		 * The specified start and end positions will be included.
		 * @param frame The frame in which the line will be painted, must be valid
		 * @param start The start position, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param end The end position, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 * @see lines().
		 */
		template <unsigned int tSize>
		static bool line(Frame& frame, const Vector2& start, const Vector2& end, const uint8_t* value = nullptr);

		/**
		 * Paints a line with specified start and end position with sub-pixel accuracy.
		 * The specified start and end positions will be included.
		 * @param frame The frame receiving the line, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param start The start position, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param end The end position, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param framePaddingElements Optional number of padding elements at the end of each row, with range [0, infinity)
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 * @see lines8BitPerChannel().
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static void line8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2& start, const Vector2& end, const uint8_t* value = nullptr, const unsigned int framePaddingElements = 0u);

		/**
		 * Paints a finite line with sub-pixel accuracy.
		 * The specified start and end positions will be included.
		 * @param frame The frame in which the line will be painted, must be valid
		 * @param line The finite line to be drawn, must be valid
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 * @see lines8BitPerChannel().
		 */
		template <unsigned int tSize>
		static bool line(Frame& frame, const FiniteLine2& line, const uint8_t* value = nullptr);

		/**
		 * Paints several finite lines with sub-pixel accuracy.
		 * The specified start and end positions will be included.<br>
		 * @param frame The frame in which the line will be painted, must be valid
		 * @param lines The finite lines to be drawn, must be valid
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 * @see line().
		 */
		template <unsigned int tSize>
		static inline bool lines(Frame& frame, const FiniteLines2& lines, const uint8_t* value = nullptr);

		/**
		 * Paints a finite line with sub-pixel accuracy.
		 * The specified start and end positions will be included.
		 * @param frame The frame receiving the line, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param line The finite line to be drawn, must be valid
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param framePaddingElements Optional number of padding elements at the end of each row, with range [0, infinity)
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 * @see lines8BitPerChannel().
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static void line8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const FiniteLine2& line, const uint8_t* value = nullptr, const unsigned int framePaddingElements = 0u);

		/**
		 * Paints an infinite line with sub-pixel accuracy.
		 * @param frame The frame in which the line will be painted, must be valid
		 * @param line The line to be drawn, must be valid
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 * @see lines8BitPerChannel().
		 */
		template <unsigned int tSize>
		static bool line(Frame& frame, const Line2& line, const uint8_t* value = nullptr);

		/**
		 * Paints several infinite lines with sub-pixel accuracy.
		 * @param frame The frame in which the line will be painted, must be valid
		 * @param lines The lines to be drawn, must be valid
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 * @see line().
		 */
		template <unsigned int tSize>
		static inline bool lines(Frame& frame, const Lines2& lines, const uint8_t* value = nullptr);

		/**
		 * Paints an infinite line with sub-pixel accuracy.
		 * @param frame The frame receiving the line, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param line The line to be drawn, must be valid
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param framePaddingElements Optional number of padding elements at the end of each row, with range [0, infinity)
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 * @see lines8BitPerChannel().
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static void line8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Line2& line, const uint8_t* value = nullptr, const unsigned int framePaddingElements = 0u);

		/**
		 * Paints a 2D axis aligned bounding box with sub-pixel accuracy.
		 * @param frame The frame in which the line will be painted, must be valid
		 * @param box The 2D axis aligned bounding box to be drawn, must be valid
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 * @see lines8BitPerChannel().
		 */
		template <unsigned int tSize>
		static bool box(Frame& frame, const Box2& box, const uint8_t* value = nullptr);

		/**
		 * Paints a 2D axis aligned bounding box with sub-pixel accuracy.
		 * @param frame The frame receiving the line, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param box The 2D axis aligned bounding box to be drawn, must be valid
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param framePaddingElements Optional number of padding elements at the end of each row, with range [0, infinity)
		 * @tparam tChannels Number of channels of the given frame, with range [1, infinity)
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 * @see lines8BitPerChannel().
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static void box8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Box2& box, const uint8_t* value = nullptr, const unsigned int framePaddingElements = 0u);

		/**
		 * Paints an ellipse at a specified position with specified size.
		 * @param frame The frame receiving the elliptic blob-region, must be valid
		 * @param position Center position of the elliptic mask, with range [0, frame.width())x[0, frame.height())
		 * @param horizontal The horizontal size of the elliptic mask in pixel, must be odd, with range [3, infinity)
		 * @param vertical The vertical size of the elliptic mask in pixel, must be odd, with range [3, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 * @see ellipse8BitPerChannel().
		 */
		static bool ellipse(Frame& frame, const PixelPosition& position, const unsigned int horizontal, const unsigned int vertical, const uint8_t* value = nullptr);

		/**
		 * Paints an ellipse at a specified position with specified size.
		 * @param frame The frame receiving the elliptic blob-region, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param position Center position of the elliptic mask, with range [0, width)x[0, height)
		 * @param horizontal The horizontal size of the elliptic mask, must be odd, with range [3, infinity)
		 * @param vertical The vertical size of the elliptic mask, must be odd, with range [3, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param paddingElements Optional number of padding elements at the end of each row, with range [0, infinity)
		 * @tparam tChannels Number of data channels of the frame data
		 * @see ellipse().
		 */
		template <unsigned int tChannels>
		static inline void ellipse8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const PixelPosition& position, const unsigned int horizontal, const unsigned int vertical, const uint8_t* value = nullptr, const unsigned int paddingElements = 0u);

		/**
		 * Paints a rotated elliptic region at a specified position with specified size and rotation angle.
		 * @param frame The frame receiving the elliptic blob-region, must be valid
		 * @param position Center position of the elliptic mask, with range [0, frame.width())x[0, frame.height())
		 * @param horizontal The horizontal size of the elliptic mask in pixel, must be odd, with range [3, infinity)
		 * @param vertical The vertical size of the elliptic mask in pixel, must be odd, with range [3, infinity)
		 * @param angle Rotation angle, in radian
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 * @see ellipse8BitPerChannel().
		 */
		static bool rotatedEllipse(Frame& frame, const PixelPosition& position, const unsigned int horizontal, const unsigned int vertical, const Scalar angle, const uint8_t* value = nullptr);

		/**
		 * Paints a rectangle at a specified position with specified size.
		 * @param frame The frame receiving the rectangle, must be valid
		 * @param left Horizontal start position (top, left) of the rectangle in pixel, with range (-infinity, infinity)
		 * @param top Vertical start position (top, left) of the rectangle in pixel, with range (-infinity, infinity)
		 * @param xSize Width of the rectangle in pixel, with range [0u, infinity)
		 * @param ySize Height of the rectangle in pixel, with range [0u, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 */
		static bool rectangle(Frame& frame, const int left, const int top, const unsigned int xSize, const unsigned int ySize, const uint8_t* value = nullptr);

		/**
		 * Paints a rectangle at a specified position with specified size.
		 * @param frame The frame receiving the rectangle, must be valid
		 * @param width The width of the frame in pixel, with range [1u, infinity)
		 * @param height The height of the frame in pixel, with range [1u, infinity)
		 * @param left Horizontal start position (top, left) of the rectangle in pixel, with range (-infinity, infinity)
		 * @param top Vertical start position (top, left) of the rectangle in pixel, with range (-infinity, infinity)
		 * @param xSize Width of the rectangle in pixel, with range [0u, infinity)
		 * @param ySize Height of the rectangle in pixel, with range [0u, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param framePaddingElements Number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @tparam tChannels Number of data channels of the frame data, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void rectangle8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const int left, const int top, const unsigned int xSize, const unsigned int ySize, const uint8_t* value = nullptr, const unsigned int framePaddingElements = 0u);

		/**
		 * Paints a point with sub-pixel accuracy.
		 * @param frame The frame in which the point will be painted, must be valid
		 * @param position Sub-pixel position of the point, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 * @tparam tSize The size (width) of the point in pixel, with range [1, infinity) and must be odd
		 * @tparam tPixelCenter The pixel center of the provided position, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 */
		template <unsigned int tSize, PixelCenter tPixelCenter = CV::PC_CENTER>
		static bool point(Frame& frame, const Vector2& position, const uint8_t* value = nullptr);

		/**
		 * Paints a point with sub-pixel accuracy.
		 * @param frame The frame in which the point will be painted
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param position Sub-pixel position of the point, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param framePaddingElements Optional number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 * @tparam tSize The size (width) of the point in pixel, with range [1, infinity) and must be odd
		 * @tparam tPixelCenter The pixel center of the provided position, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 */
		template <unsigned int tChannels, unsigned int tSize, PixelCenter tPixelCenter = CV::PC_CENTER>
		static void point8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2& position, const uint8_t* value = nullptr, const unsigned int framePaddingElements = 0u);

		/**
		 * Paints points with sub-pixel accuracy.
		 * @param frame The frame in which the point will be painted, must be valid
		 * @param positions Sub-pixel positions of the points, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 * @tparam tSize The size (width) of the point in pixel, with range [1, infinity) and must be odd
		 * @tparam tPixelCenter The pixel center of the provided position, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 */
		template <unsigned int tSize, PixelCenter tPixelCenter = CV::PC_CENTER>
		static bool points(Frame& frame, const Vectors2& positions, const uint8_t* value = nullptr);

		/**
		 * Paints points with sub-pixel accuracy.
		 * @param frame The frame in which the point will be painted
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param positions Sub-pixel positions of the points, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param framePaddingElements Optional number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 * @tparam tSize The size (width) of the point in pixel, with range [1, infinity) and must be odd
		 * @tparam tPixelCenter The pixel center of the provided position, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 */
		template <unsigned int tChannels, unsigned int tSize, PixelCenter tPixelCenter = CV::PC_CENTER>
		static void points8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vectors2& positions, const uint8_t* value = nullptr, const unsigned int framePaddingElements = 0u);

		/**
		 * Fills an image area with a given color, an recursive seed-fill-algorithm is implemented.
		 * The fill area is determined by all pixels that lie in a successive 4-neighborhood of the given start position and have the same color value as the start pixel.
		 * @param frame The frame to be filled
		 * @param position The start position defining the color of the fill area, with range [0, width)x[0, height)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @return True, if succeeded
		 */
		static bool fill(Frame& frame, const PixelPosition& position, const uint8_t* value = nullptr);

		/**
		 * Fills an image area with a given color, an recursive seed-fill-algorithm is implemented.
		 * The fill area is determined by all pixels that lie in a successive 4-neighborhood of the given start position and have the same color value as the start pixel.
		 * @param frame The frame to be filled, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param position The start position defining the color of the fill area, with range [0, width)x[0, height)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * * @param framePaddingElements Optional number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 */
		template <unsigned int tChannels>
		static void fill8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const PixelPosition& position, const uint8_t* value = nullptr, const unsigned int framePaddingElements = 0u);

		/**
		 * Paints the outline of a polygon with sub-pixel accuracy.
		 * @param frame The frame in which the polygon will be painted, must be valid
		 * @param points The points of the polygon to be drawn, must be valid
		 * @param numberPoints The number of given polygon points, with range [0, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param closeLoop True, to close the loop of the polygon (between first and last point); False, to not paint the connection between first and last point
		 * @return True, if succeeded
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 */
		template <unsigned int tSize>
		static bool polygon(Frame& frame, const Vector2* points, size_t numberPoints, const uint8_t* value = nullptr, const bool closeLoop = true);

		/**
		 * Paints the outline of a polygon with sub-pixel accuracy.
		 * @param frame The frame in which the polygon will be painted, must be valid
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param points The points of the polygon to be drawn, must be valid
		 * @param numberPoints The number of given polygon points, with range [0, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param closeLoop True, to close the loop of the polygon (between first and last point); False, to not paint the connection between first and last point
		 * @param framePaddingElements Optional number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 * @tparam tSize Size (thickness) of the line in pixel, with range [1, infinity) and must be odd
		 */
		template <unsigned int tChannels, unsigned int tSize>
		static void polygon8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2* points, size_t numberPoints, const uint8_t* value = nullptr, const bool closeLoop = true, const unsigned int framePaddingElements = 0u);

		/**
		 * Paints a given text into a given frame using the standard (code style) font supporting only one size.
		 * Use Ocean::CV::Fonts in case arbitrary fonts are necessary.
		 * The given location can be partially outside of the frame.
		 * @param frame The frame in which the text will be painted, must have a generic pixel format like e.g., FORMAT_Y8 or FORMAT_RGB24, must be valid
		 * @param text The text to be painted, the text can contain end-of-line characters to paint the remaining the following characters in the next line, can be empty
		 * @param left The horizontal start position of the text within the frame, in pixel, with range (-infinity, infinity)
		 * @param top The vertical start position of the text within the frame, in pixel, with range (-infinity, infinity)
		 * @param foregroundColor The foreground color to be used when painting the character, must be valid
		 * @param backgroundColor The optional background color to be used when paining the character, nullptr to paint the foreground pixels only
		 * @return True, if succeeded
		 * @see textExtent(), CV::Fonts::Font::drawText().
		 */
		static bool drawText(Frame& frame, const std::string& text, const int left, const int top, const uint8_t* foregroundColor, const uint8_t* backgroundColor = nullptr);

		/**
		 * Returns the bounding box a given text will occupy in pixel space when using the standard font.
		 * @param text The text for which the extent will be determined, can be empty
		 * @param width The width of the bounding box, in pixel, with range [0, infinity)
		 * @param height The height of the bounding box, in pixel, with range [0, infinity)
		 * @return True, if succeeded
		 * @see drawText(), CV::Fonts::Font::textExtent().
		 */
		static bool textExtent(const std::string& text, unsigned int& width, unsigned int& height);

	private:

		/**
		 * Returns a memory block to 32 bytes as a backup in case a requested color value does not exist for a specific pixel format.
		 * @return The 32 byte memory block
		 */
		static const uint8_t* memoryBlock32Byte();

		/**
		 * Paints a ellipse at a specified position with specified size.
		 * @param frame The frame receiving the elliptic blob-region, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param position Center position of the elliptic mask, with range [0, width - 1]x[0, height - 1]
		 * @param horizontalHalf Half horizontal size of the elliptic mask, with range [1, infinity)
		 * @param verticalHalf Half vertical size of the elliptic mask, with range [1, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param paddingElements Optional number of padding elements at the end of each row, with range [0, infinity)
		 * @tparam T Internal data type to determine the elliptic region
		 * @tparam tChannels Number of channels of the given frame
		 */
		template <typename T, unsigned int tChannels>
		static void ellipse8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const PixelPosition& position, const unsigned int horizontalHalf, const unsigned int verticalHalf, const uint8_t* value, const unsigned int paddingElements = 0u);

		/**
		 * Paints a rotated elliptic region at a specified position with specified size.
		 * @param frame The frame receiving the elliptic blob-region, must be valid
		 * @param width The width of the mask frame in pixel, with range [1, infinity)
		 * @param height The height of the mask frame in pixel, with range [1, infinity)
		 * @param position Center position of the elliptic mask, with range [0, width)x[0, height)
		 * @param horizontalHalf Half horizontal size of the elliptic mask, with range [1, infinity)
		 * @param verticalHalf Half vertical size of the elliptic mask, with range [1, infinity)
		 * @param angle Rotation angle, in radian, with range [0, 2 * PI)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param paddingElements Optional number of padding elements at the end of each row, with range [0, infinity)
		 * @tparam tChannels Number of channels of the given frame
		 */
		template <unsigned int tChannels>
		static void rotatedEllipse8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const PixelPosition& position, const unsigned int horizontalHalf, const unsigned int verticalHalf, const Scalar angle, const uint8_t* value, const unsigned int paddingElements = 0u);

		/**
		 * Paints a point with sub-pixel accuracy.
		 * @param frame The frame in which the point will be painted
		 * @param width The width of the frame in pixel, with range [1, infinity)
		 * @param height The height of the frame in pixel, with range [1, infinity)
		 * @param position Sub-pixel position of the point, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param value Color value to be used, make sure that the provided buffer has at least as many bytes as the pixel format for one pixel, nullptr to apply 0x00 for each channel
		 * @param factors Filter factors for the specified point size
		 * @param framePaddingElements Optional number of padding elements at the end of each frame row, in elements, with range [0, infinity)
		 * @tparam tChannels Number of frame channels, with range [1, infinity)
		 * @tparam tSize The size (width) of the point in pixel, with range [1, infinity) and must be odd
		 * @tparam tPixelCenter The pixel center of the provided position, either 'PC_TOP_LEFT' or 'PC_CENTER'
		 */
		template <unsigned int tChannels, unsigned int tSize, PixelCenter tPixelCenter>
		static void point8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2& position, const uint8_t* value, const FilterFactors<tSize>& factors, const unsigned int framePaddingElements = 0u);
};

template <unsigned int tFilterSize>
inline Canvas::FilterFactors<tFilterSize>::FilterFactors()
{
	static_assert(tFilterSize % 2u == 1u, "Invalid aliasing direction filter size.");

	if constexpr (tFilterSize <= 5u)
	{
		for (unsigned int k = 0u; k < tFilterSize; ++k)
		{
			factors_[k] = NumericT<unsigned int>::binomialCoefficient(tFilterSize - 1u, k);
		}

		maximalFactor_ = NumericT<unsigned int>::binomialCoefficient(tFilterSize - 1, tFilterSize / 2u);
	}
	else
	{
		for (unsigned int k = 0u; k < tFilterSize; ++k)
		{
			if (k < 5u / 2u)
			{
				factors_[k] = NumericT<unsigned int>::binomialCoefficient(5u - 1u, k);
			}
			else if (k >= tFilterSize - 5u / 2u)
			{
				factors_[k] = NumericT<unsigned int>::binomialCoefficient(5u - 1u, 5u - (tFilterSize - k));
			}
			else
			{
				factors_[k] = NumericT<unsigned int>::binomialCoefficient(5u - 1u, 5u / 2u);
			}
		}

		maximalFactor_ = NumericT<unsigned int>::binomialCoefficient(5u - 1u, 5u / 2u);
	}
}

template <unsigned int tFilterSize>
inline unsigned int Canvas::FilterFactors<tFilterSize>::factor(const unsigned int index) const
{
	ocean_assert(index < tFilterSize);

	return factors_[index];
}

template <unsigned int tFilterSize>
inline unsigned int Canvas::FilterFactors<tFilterSize>::clampedFactor(const int index) const
{
	if (index < 0 || index >= int(tFilterSize))
	{
		return 0u;
	}

	return factors_[index];
}

template <unsigned int tFilterSize>
inline unsigned int Canvas::FilterFactors<tFilterSize>::maximalFactor() const
{
	return maximalFactor_;
}

template <unsigned int tChannels>
void Canvas::line8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const int xStart, const int yStart, const int xEnd, const int yEnd, const uint8_t* value, const unsigned int framePaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	const uint8_t zeroValue[tChannels] = {0u};
	const uint8_t* const color = value ? value : zeroValue;

	int x = int(xStart);
	int y = int(yStart);

	Bresenham bresenham(x, y, int(xEnd), int(yEnd));

	if (bresenham.isValid())
	{
		do
		{
			if ((unsigned int)x < width && (unsigned int)y < height)
			{
				uint8_t* const pixel = frame + y * frameStrideElements + x * tChannels;

				for (unsigned int n = 0u; n < tChannels; ++n)
				{
					pixel[n] = color[n];
				}
			}

			bresenham.findNext(x, y);
		}
		while (x != int(xEnd) || y != int(yEnd));
	}

	// end point
	ocean_assert(x == int(xEnd) || y == int(yEnd));

	if ((unsigned int)x < width && (unsigned int)y < height)
	{
		uint8_t* const pixel = frame + y * frameStrideElements + x * tChannels;

		for (unsigned int n = 0u; n < tChannels; ++n)
		{
			pixel[n] = color[n];
		}
	}
}

template <unsigned int tChannels>
void Canvas::lines8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const PixelPosition* positions, const unsigned int numberPositions, const uint8_t* value, const unsigned int framePaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");

	ocean_assert(frame != nullptr);
	ocean_assert(width != 0u && height != 0u);

	if (numberPositions == 0u)
	{
		return;
	}

	ocean_assert(positions != nullptr);

	for (unsigned int n = 0u; n < numberPositions - 1u; n += 2u)
	{
		ocean_assert(n + 1u < numberPositions);
		line8BitPerChannel<tChannels>(frame, width, height, positions[n].x(), positions[n].y(), positions[n + 1].x(), positions[n + 1].y(), value, framePaddingElements);
	}
}

template <unsigned int tSize>
bool Canvas::line(Frame& frame, const Scalar xStart, const Scalar yStart, const Scalar xEnd, const Scalar yEnd, const uint8_t* value)
{
	return line<tSize>(frame, Vector2(xStart, yStart), Vector2(xEnd, yEnd), value);
}

template <unsigned int tChannels, unsigned int tSize>
void Canvas::line8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Scalar xStart, const Scalar yStart, const Scalar xEnd, const Scalar yEnd, const uint8_t* value, const unsigned int framePaddingElements)
{
	line8BitPerChannel<tChannels, tSize>(frame, width, height, Vector2(xStart, yStart), Vector2(xEnd, yEnd), value, framePaddingElements);
}

template <unsigned int tSize>
bool Canvas::line(Frame& frame, const Vector2& start, const Vector2& end, const uint8_t* value)
{
	static_assert(tSize % 2u == 1u, "Invalid size parameter.");

	ocean_assert(frame);

	if (frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frame.numberPlanes() == 1u)
	{
		switch (frame.channels())
		{
			case 1u:
				line8BitPerChannel<1u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), start, end, value, frame.paddingElements());
				return true;

			case 2u:
				line8BitPerChannel<2u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), start, end, value, frame.paddingElements());
				return true;

			case 3u:
				line8BitPerChannel<3u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), start, end, value, frame.paddingElements());
				return true;

			case 4u:
				line8BitPerChannel<4u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), start, end, value, frame.paddingElements());
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

template <unsigned int tChannels, unsigned int tSize>
void Canvas::line8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2& start, const Vector2& end, const uint8_t* value, const unsigned int framePaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tSize % 2u == 1u, "Invalid size parameter.");

	ocean_assert(frame != nullptr);

	const Vector2 direction(end - start);
	const Scalar length = direction.length();

	const uint8_t zeroValue[tChannels] = {0x00};
	const uint8_t* const color = value ? value : zeroValue;

	if (length < 1)
	{
		point8BitPerChannel<tChannels, tSize, PC_CENTER>(frame, width, height, start, color, FilterFactors<tSize>(), framePaddingElements);
	}
	else
	{
		const Vector2 step = direction / length;

		for (unsigned int n = 0u; n <= (unsigned int)(length); ++n)
		{
			const Vector2 position(start + step * Scalar(n));

			if (position.x() >= -Scalar(tSize / 2u) - 1 && position.y() >= -Scalar(tSize / 2u) - 1 && position.x() <= Scalar(width + tSize / 2u) && position.y() <= Scalar(height + tSize / 2u))
			{
				point8BitPerChannel<tChannels, tSize, PC_CENTER>(frame, width, height, position, color, FilterFactors<tSize>(), framePaddingElements);
			}
		}
	}
}

template <unsigned int tSize>
bool Canvas::line(Frame& frame, const FiniteLine2& line, const uint8_t* value)
{
	return Canvas::line<tSize>(frame, line.point0(), line.point1(), value);
}

template <unsigned int tSize>
inline bool Canvas::lines(Frame& frame, const FiniteLines2& lines, const uint8_t* value)
{
	for (const FiniteLine2& line : lines)
	{
		if (!Canvas::line<tSize>(frame, line, value))
		{
			return false;
		}
	}

	return true;
}

template <unsigned int tChannels, unsigned int tSize>
void Canvas::line8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const FiniteLine2& line, const uint8_t* value, const unsigned int framePaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tSize % 2u == 1u, "Invalid size parameter!");

	ocean_assert(frame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(line.isValid());

	Canvas::line8BitPerChannel<tChannels, tSize>(frame, width, height, line.point0(), line.point1(), value, framePaddingElements);
}

template <unsigned int tSize>
bool Canvas::line(Frame& frame, const Line2& line, const uint8_t* value)
{
	static_assert(tSize % 2u == 1u, "Invalid size parameter.");

	ocean_assert(frame.isValid());

	if (frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frame.numberPlanes() == 1u)
	{
		switch (frame.channels())
		{
			case 1u:
				line8BitPerChannel<1u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), line, value, frame.paddingElements());
				return true;

			case 2u:
				line8BitPerChannel<2u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), line, value, frame.paddingElements());
				return true;

			case 3u:
				line8BitPerChannel<3u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), line, value, frame.paddingElements());
				return true;

			case 4u:
				line8BitPerChannel<4u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), line, value, frame.paddingElements());
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

template <unsigned int tSize>
inline bool Canvas::lines(Frame& frame, const Lines2& lines, const uint8_t* value)
{
	for (const Line2& line : lines)
	{
		if (!Canvas::line<tSize>(frame, line, value))
		{
			return false;
		}
	}

	return true;
}

template <unsigned int tChannels, unsigned int tSize>
void Canvas::line8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Line2& line, const uint8_t* value, const unsigned int framePaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tSize % 2u == 1u, "Invalid size parameter!");

	ocean_assert(frame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(line.isValid());

	Scalar x0, y0, x1, y1;
	if (CV::Bresenham::borderIntersection(line, -Scalar(tSize) * Scalar(0.5), -Scalar(tSize) * Scalar(0.5), Scalar(width + tSize / 2u), Scalar(height + tSize / 2u), x0, y0, x1, y1))
	{
		line8BitPerChannel<tChannels, tSize>(frame, width, height, x0, y0, x1, y1, value, framePaddingElements);
	}
}

template <unsigned int tSize>
bool Canvas::box(Frame& frame, const Box2& box, const uint8_t* value)
{
	static_assert(tSize % 2u == 1u, "Invalid size parameter.");

	ocean_assert(frame.isValid());

	if (frame.numberPlanes() == 1u && frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8)
	{
		switch (frame.channels())
		{
			case 1u:
				box8BitPerChannel<1u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), box, value, frame.paddingElements());
				return true;

			case 2u:
				box8BitPerChannel<2u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), box, value, frame.paddingElements());
				return true;

			case 3u:
				box8BitPerChannel<3u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), box, value, frame.paddingElements());
				return true;

			case 4u:
				box8BitPerChannel<4u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), box, value, frame.paddingElements());
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}

template <unsigned int tChannels, unsigned int tSize>
void Canvas::box8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Box2& box, const uint8_t* value, const unsigned int paddingElements)
{
	ocean_assert(frame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);
	ocean_assert(box.isValid());

	line8BitPerChannel<tChannels, tSize>(frame, width, height, box.left(), box.top(), box.left(), box.bottom(), value, paddingElements);
	line8BitPerChannel<tChannels, tSize>(frame, width, height, box.left(), box.bottom(), box.right(), box.bottom(), value, paddingElements);
	line8BitPerChannel<tChannels, tSize>(frame, width, height, box.right(), box.bottom(), box.right(), box.top(), value, paddingElements);
	line8BitPerChannel<tChannels, tSize>(frame, width, height, box.right(), box.top(), box.left(), box.top(), value, paddingElements);
}

template <unsigned int tChannels>
void Canvas::ellipse8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const PixelPosition& position, const unsigned int horizontal, const unsigned int vertical, const uint8_t* value, const unsigned int paddingElements)
{
	ocean_assert(frame);
	ocean_assert(position.x() < width);
	ocean_assert(position.y() < height);

	ocean_assert(horizontal >= 3u);
	ocean_assert(vertical >= 3u);
	ocean_assert(horizontal % 2u == 1u);
	ocean_assert(vertical % 2u == 1u);

	// implicit form:
	// (x / a)^2 + (y / b)^2 == 1
	//
	// x^2 * b^2 + y^2 * a^2 == a^2 * b^2

	const unsigned int horizontalHalf = horizontal >> 1u;
	const unsigned int verticalHalf = vertical >> 1u;

	if (horizontalHalf < 199u && verticalHalf < 199u)
	{
		ellipse8BitPerChannel<unsigned int, tChannels>(frame, width, height, position, horizontalHalf, verticalHalf, value, paddingElements);
	}
	else
	{
		ellipse8BitPerChannel<unsigned long long, tChannels>(frame, width, height, position, horizontalHalf, verticalHalf, value, paddingElements);
	}
}

template <unsigned int tChannels>
void Canvas::rectangle8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const int left, const int top, const unsigned int xSize, const unsigned int ySize, const uint8_t* value, const unsigned int framePaddingElements)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame != 0u);
	ocean_assert(width >= 1u && height >= 1u);

	const int clampedLeft = max(0, left);
	const int clampedTop = max(0, top);

	const int clampedRightEnd = min(left + int(xSize), int(width)); // exclusive
	const int clampedBottomEnd = min(top + int(ySize), int(height));

	if (clampedRightEnd <= clampedLeft || clampedBottomEnd <= clampedTop)
	{
		// we are entirely outside the frame
		return;
	}

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const uint8_t black[tChannels] = {0u};
	const PixelType pixelValue = value ? *(const PixelType*)value : *(const PixelType*)black;

	const unsigned int clampedWidth = (unsigned int)(clampedRightEnd - clampedLeft);
	const unsigned int clampedHeight = (unsigned int)(clampedBottomEnd - clampedTop);

	ocean_assert(clampedWidth <= xSize);
	ocean_assert(clampedHeight <= ySize);

	ocean_assert(clampedLeft + clampedWidth <= width);
	ocean_assert(clampedTop + clampedHeight <= height);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	for (unsigned int y = clampedTop; y < clampedTop + clampedHeight; ++y)
	{
		PixelType* const rowLeft = (PixelType*)(frame + y * frameStrideElements) + clampedLeft;

		for (unsigned int x = 0u; x < clampedWidth; ++x)
		{
			rowLeft[x] = pixelValue;
		}
	}
}

template <unsigned int tSize, PixelCenter tPixelCenter>
bool Canvas::point(Frame& frame, const Vector2& position, const uint8_t* value)
{
	static_assert(tSize % 2u == 1u, "Invalid size parameter.");
	static_assert(tPixelCenter == PC_TOP_LEFT || tPixelCenter == PC_CENTER, "Invalid pixel center!");

	ocean_assert(frame);

	if (frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frame.numberPlanes() == 1u)
	{
		switch (frame.channels())
		{
			case 1u:
				point8BitPerChannel<1u, tSize, tPixelCenter>(frame.data<uint8_t>(), frame.width(), frame.height(), position, value, frame.paddingElements());
				return true;

			case 2u:
				point8BitPerChannel<2u, tSize, tPixelCenter>(frame.data<uint8_t>(), frame.width(), frame.height(), position, value, frame.paddingElements());
				return true;

			case 3u:
				point8BitPerChannel<3u, tSize, tPixelCenter>(frame.data<uint8_t>(), frame.width(), frame.height(), position, value, frame.paddingElements());
				return true;

			case 4u:
				point8BitPerChannel<4u, tSize, tPixelCenter>(frame.data<uint8_t>(), frame.width(), frame.height(), position, value, frame.paddingElements());
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}


template <unsigned int tChannels, unsigned int tSize, PixelCenter tPixelCenter>
void Canvas::point8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2& position, const uint8_t* value, const unsigned int framePaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tSize % 2u == 1u, "Invalid size parameter.");
	static_assert(tPixelCenter == PC_TOP_LEFT || tPixelCenter == PC_CENTER, "Invalid pixel center!");

	ocean_assert(frame != nullptr);

	const uint8_t explicitValue[tChannels] = {0x00};

	point8BitPerChannel<tChannels, tSize, tPixelCenter>(frame, width, height, position, value ? value : explicitValue, FilterFactors<tSize>(), framePaddingElements);
}

template <unsigned int tSize, PixelCenter tPixelCenter>
bool Canvas::points(Frame& frame, const Vectors2& positions, const uint8_t* value)
{
	static_assert(tSize % 2u == 1u, "Invalid size parameter.");
	static_assert(tPixelCenter == PC_TOP_LEFT || tPixelCenter == PC_CENTER, "Invalid pixel center!");

	ocean_assert(frame);

	if (frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frame.numberPlanes() == 1u)
	{
		switch (frame.channels())
		{
			case 1u:
				points8BitPerChannel<1u, tSize, tPixelCenter>(frame.data<uint8_t>(), frame.width(), frame.height(), positions, value, frame.paddingElements());
				return true;

			case 2u:
				points8BitPerChannel<2u, tSize, tPixelCenter>(frame.data<uint8_t>(), frame.width(), frame.height(), positions, value, frame.paddingElements());
				return true;

			case 3u:
				points8BitPerChannel<3u, tSize, tPixelCenter>(frame.data<uint8_t>(), frame.width(), frame.height(), positions, value, frame.paddingElements());
				return true;

			case 4u:
				points8BitPerChannel<4u, tSize, tPixelCenter>(frame.data<uint8_t>(), frame.width(), frame.height(), positions, value, frame.paddingElements());
				return true;
		}
	}

	ocean_assert(false && "Invalid frame type!");
	return false;
}


template <unsigned int tChannels, unsigned int tSize, PixelCenter tPixelCenter>
void Canvas::points8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vectors2& positions, const uint8_t* value, const unsigned int framePaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tSize % 2u == 1u, "Invalid size parameter!");
	static_assert(tPixelCenter == PC_TOP_LEFT || tPixelCenter == PC_CENTER, "Invalid pixel center!");

	ocean_assert(frame != nullptr);

	if (value)
	{
		for (const Vector2& position : positions)
		{
			point8BitPerChannel<tChannels, tSize, tPixelCenter>(frame, width, height, position, value, FilterFactors<tSize>(), framePaddingElements);
		}
	}
	else
	{
		const uint8_t explicitValue[tChannels] = {0x00};

		for (const Vector2& position : positions)
		{
			point8BitPerChannel<tChannels, tSize, tPixelCenter>(frame, width, height, position, explicitValue, FilterFactors<tSize>(), framePaddingElements);
		}
	}
}

template <typename T, unsigned int tChannels>
void Canvas::ellipse8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const PixelPosition& position, const unsigned int horizontalHalf, const unsigned int verticalHalf, const uint8_t* value, const unsigned int paddingElements)
{
	static_assert(tChannels != 0u, "Invalid number of channels!");

	ocean_assert(frame);
	ocean_assert(verticalHalf > 0u);
	ocean_assert(horizontalHalf > 0u);
	ocean_assert(position.x() < width);
	ocean_assert(position.y() < height);

	const unsigned int frameStrideElements = width * tChannels + paddingElements;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;
	const uint8_t valueZero[tChannels] = {uint8_t(0)};

	const PixelType* const pixelValue = value == nullptr ? (PixelType*)valueZero : (PixelType*)value;

	// implicit form:
	// (x / a)^2 + (y / b)^2 == 1
	//
	// x^2 * b^2 + y^2 * a^2 == a^2 * b^2

	// We do not use the following calculation as the a single pixel would be visible at the north, west, south and east position of the ellipse:
	// const T a2 = sqr(horizontalHalf);
	// const T b2 = sqr(verticalHalf);
	// Instead we take the center between the real radius and the radius which is one pixel larger to overcome the single pixel issue

	const T a2 = (sqr(horizontalHalf) + sqr(horizontalHalf + 1u)) / 2u;
	const T b2 = (sqr(verticalHalf) + sqr(verticalHalf + 1u)) / 2u;
	const T ab2 = a2 * b2;

	for (unsigned int y = position.y(); y <= position.y() + verticalHalf; ++y)
	{
		unsigned int left = position.x();
		unsigned int right = position.x() + horizontalHalf + 1u;

		const T ySqra2 = T(sqr(y - position.y())) * a2;

		while (left + 1u < right)
		{
			ocean_assert(T(sqr(left - position.x())) * b2 + T(sqr(y - position.y())) * a2 <= ab2);

			const unsigned int mid = (left + right) / 2u;

			if (T(sqr(mid - position.x())) * b2 + ySqra2 <= ab2)
			{
				left = mid;
			}
			else
			{
				right = mid;
			}
		}

		ocean_assert(left + 1u == right);
		ocean_assert(T(sqr(left - position.x())) * b2 + T(sqr(y - position.y())) * a2 <= ab2);
		ocean_assert(T(sqr(right - position.x())) * b2 + T(sqr(y - position.y())) * a2 > ab2);

		const unsigned int frameLeft = max(0, int(2u * position.x() - left));
		const unsigned int frameRight = min(right, width);

		ocean_assert(frameLeft < width);
		ocean_assert(frameRight <= width);
		ocean_assert(frameLeft < frameRight);

		// top
		if (2u * position.y() >= y)
		{
			const unsigned int frameTop = 2u * position.y() - y;
			ocean_assert(frameTop < height);

			PixelType* pointer = (PixelType*)(frame + frameTop * frameStrideElements) + frameLeft;
			PixelType* const pointerEnd = pointer + (frameRight - frameLeft);

			while (pointer != pointerEnd)
			{
				*pointer++ = *pixelValue;
			}
		}

		// bottom
		if (y < height)
		{
			PixelType* pointer = (PixelType*)(frame + y * frameStrideElements) + frameLeft;
			PixelType* const pointerEnd = pointer + (frameRight - frameLeft);

			while (pointer != pointerEnd)
			{
				*pointer++ = *pixelValue;
			}
		}
	}
}

template <unsigned int tChannels>
void Canvas::rotatedEllipse8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const PixelPosition& position, const unsigned int horizontalHalf, const unsigned int verticalHalf, const Scalar angle, const uint8_t* value, const unsigned int paddingElements)
{
	static_assert(tChannels != 0u, "Invalid number of channels!");

	ocean_assert(frame);
	ocean_assert(position.x() < width);
	ocean_assert(position.y() < height);

	const unsigned int frameStrideElements = width * tChannels + paddingElements;

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;
	const uint8_t valueZero[tChannels] = {0u};

	const PixelType* const pixelValue = value == nullptr ? (PixelType*)valueZero : (PixelType*)value;

	// implicit form:
	// (x / a)^2 + (y / b)^2 == 1
	//
	// x^2 * b^2 + y^2 * a^2 == a^2 * b^2

	const Scalar factorA = 1 / Scalar(sqr(horizontalHalf));
	const Scalar factorB = 1 / Scalar(sqr(verticalHalf));

	const SquareMatrix3 invertedRotation(Rotation(0, 0, 1, angle));

	const uint8_t radius = uint8_t(max(horizontalHalf, verticalHalf));

	for (unsigned int y = max(0, int(position.y() - radius)); y <= min(position.y() + radius, height); ++y)
	{
		for (unsigned int x = max(0, int(position.x() - radius)); x <= min(position.x() + radius, width); ++x)
		{
			const Vector3 position3(Scalar(position.x()) - Scalar(x), Scalar(position.y()) - Scalar(y), 0);
			Vector3 invertedPosition(invertedRotation * position3);

			if (Numeric::sqr(invertedPosition.x()) * factorA + Numeric::sqr(invertedPosition.y()) * factorB <= 1)
			{
				*((PixelType*)(frame + y * frameStrideElements) + x) = *pixelValue;
			}
		}
	}
}

template <unsigned int tChannels, unsigned int tSize, PixelCenter tPixelCenter>
void Canvas::point8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2& position, const uint8_t* value, const FilterFactors<tSize>& factors, const unsigned int framePaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tSize % 2u == 1u, "Invalid size parameter.");
	static_assert(tPixelCenter == PC_TOP_LEFT || tPixelCenter == PC_CENTER, "Invalid pixel center!");

	ocean_assert(frame != nullptr);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	// the remaining function expects PC_CENTER as pixel center so shifting the position in case the coordinate is provided as PC_TOP_LEFT
	const Vector2 shiftedPosition = tPixelCenter == PC_TOP_LEFT ? position + Vector2(Scalar(0.5), Scalar(0.5)) : position;

	const int left = int(Numeric::floor(shiftedPosition.x() - Scalar(0.5)));
	const int top = int(Numeric::floor(shiftedPosition.y() - Scalar(0.5)));

	const unsigned int xFactor = (unsigned int)((Scalar(left) + Scalar(1.5) - shiftedPosition.x()) * Scalar(128) + Scalar(0.5));
	const unsigned int yFactor = (unsigned int)((Scalar(top) + Scalar(1.5) - shiftedPosition.y()) * Scalar(128) + Scalar(0.5));

	ocean_assert(xFactor <= 128u && yFactor <= 128u);

	const unsigned int sqrMaximalFilter = factors.maximalFactor() * factors.maximalFactor();

	for (unsigned int y = 0u; y <= tSize; ++y)
	{
		const unsigned int yInFrame = top + int(y) - int(tSize / 2u);

		if (yInFrame < height)
		{
			for (unsigned int x = 0u; x <= tSize; ++x)
			{
				const unsigned int xInFrame = left + int(x) - int(tSize / 2u);

				if (xInFrame < width)
				{
					const unsigned int factor = (128u - yFactor) * factors.clampedFactor(int(y - 1u)) * (128u - xFactor) * factors.clampedFactor(int(x - 1u))
													+ (128u - yFactor) * factors.clampedFactor(int(y - 1u)) * xFactor * factors.clampedFactor(x)
													+ yFactor * factors.clampedFactor(y) * (128u - xFactor) * factors.clampedFactor(int(x - 1u))
													+ yFactor * factors.clampedFactor(y) * xFactor * factors.clampedFactor(x);

					const unsigned int _factor = 16384 * sqrMaximalFilter - factor;

					uint8_t* const pixel = frame + yInFrame * frameStrideElements + xInFrame * tChannels;

					for (unsigned int n = 0u; n < tChannels; ++n)
					{
						pixel[n] = uint8_t((pixel[n] * _factor + value[n] * factor + (16384u * sqrMaximalFilter) / 2u) / (16384u * sqrMaximalFilter));
					}
				}
			}
		}
	}
}

template <unsigned int tChannels>
void Canvas::fill8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const PixelPosition& position, const uint8_t* value, const unsigned int framePaddingElements)
{
	static_assert(tChannels != 0u, "Invalid channel number!");

	ocean_assert(frame);
	ocean_assert(width >= 1u && height >= 1u);

	ocean_assert(position.x() < width && position.y() < height);

	typedef typename DataType<uint8_t, tChannels>::Type PixelType;

	const uint8_t zeroValue[tChannels] = {0u};
	const PixelType pixelValue = value ? *((PixelType*)value) : *((PixelType*)zeroValue);

	const unsigned int frameStrideElements = width * tChannels + framePaddingElements;

	const unsigned int offset = position.y() * frameStrideElements + position.x() * tChannels;

	// check whether the start position has the same color like the given replacement color
	if (*(const PixelType*)(frame + offset) == pixelValue)
	{
		return;
	}

	const PixelType areaPixelValue = *(const PixelType*)(frame + offset);
	*(PixelType*)(frame + offset) = pixelValue;

	// we use a vector and not a std::stack as the stack implementation is significant slower
	std::vector<PixelPosition> stack;
	stack.reserve((width * height) / 16u);

	// left
	if (position.x() != 0u && *((const PixelType*)(frame + offset) - 1) == areaPixelValue)
	{
		stack.push_back(position.west());
	}

	// right
	if (position.x() != width - 1u && *((const PixelType*)(frame + offset) + 1) == areaPixelValue)
	{
		stack.push_back(position.east());
	}

	// top
	if (position.y() != 0u && *((const PixelType*)(frame + offset - frameStrideElements)) == areaPixelValue)
	{
		stack.push_back(position.north());
	}

	// bottom
	if (position.y() != height - 1u && *((const PixelType*)(frame + offset + frameStrideElements)) == areaPixelValue)
	{
		stack.push_back(position.south());
	}

	while (!stack.empty())
	{
		const PixelPosition pixel(stack.back());
		stack.pop_back();

		const unsigned int testOffset = pixel.y() * frameStrideElements + pixel.x() * tChannels;

		ocean_assert(*(const PixelType*)(frame + testOffset) == areaPixelValue);

		*(PixelType*)(frame + testOffset) = pixelValue;

		// left
		if (pixel.x() != 0u && *((const PixelType*)(frame + testOffset) - 1) == areaPixelValue)
		{
			stack.push_back(pixel.west());
		}

		// right
		if (pixel.x() != width - 1u && *((const PixelType*)(frame + testOffset) + 1) == areaPixelValue)
		{
			stack.push_back(pixel.east());
		}

		// top
		if (pixel.y() != 0u && *((const PixelType*)(frame + testOffset - frameStrideElements)) == areaPixelValue)
		{
			stack.push_back(pixel.north());
		}

		// bottom
		if (pixel.y() != height - 1u && *((const PixelType*)(frame + testOffset + frameStrideElements)) == areaPixelValue)
		{
			stack.push_back(pixel.south());
		}
	}
}

template <unsigned int tSize>
bool Canvas::polygon(Frame& frame, const Vector2* points, size_t numberPoints, const uint8_t* value, const bool closeLoop)
{
	static_assert(tSize % 2u == 1u, "Invalid size parameter.");

	ocean_assert(frame.isValid());

	if (frame.dataType() == FrameType::DT_UNSIGNED_INTEGER_8 && frame.numberPlanes() == 1u)
	{
		switch (frame.channels())
		{
			case 1u:
				polygon8BitPerChannel<1u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), points, numberPoints, value, closeLoop);
				return true;

			case 2u:
				polygon8BitPerChannel<2u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), points, numberPoints, value, closeLoop);
				return true;

			case 3u:
				polygon8BitPerChannel<3u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), points, numberPoints, value, closeLoop);
				return true;

			case 4u:
				polygon8BitPerChannel<4u, tSize>(frame.data<uint8_t>(), frame.width(), frame.height(), points, numberPoints, value, closeLoop);
				return true;
		}
	}

	ocean_assert(false && "Invalid pixel format!");
	return false;
}

template <unsigned int tChannels, unsigned int tSize>
void Canvas::polygon8BitPerChannel(uint8_t* frame, const unsigned int width, const unsigned int height, const Vector2* points, size_t numberPoints, const uint8_t* value, const bool closeLoop, const unsigned int framePaddingElements)
{
	static_assert(tChannels >= 1u, "Invalid channel number!");
	static_assert(tSize % 2u == 1u, "Invalid size parameter.");

	ocean_assert(frame != nullptr);
	ocean_assert(width >= 1u && height >= 1u);

	if (numberPoints >= 2)
	{
		for (size_t n = 1; n < numberPoints; ++n)
		{
			line8BitPerChannel<tChannels, tSize>(frame, width, height, points[n - 1], points[n], value, framePaddingElements);
		}

		if (numberPoints >= 3 && closeLoop)
		{
			line8BitPerChannel<tChannels, tSize>(frame, width, height, points[numberPoints - 1], points[0], value, framePaddingElements);
		}
	}
}

}

}

#endif // META_OCEAN_CV_CANVAS_H
