/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_SCREEN_H
#define META_OCEAN_PLATFORM_WIN_SCREEN_H

#include "ocean/platform/win/Win.h"

#include "ocean/cv/PixelBoundingBox.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class provides screen functionalities.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT Screen
{
	public:

		/**
		 * Returns the width of the primary display in pixel.
		 * @return Primary display width in pixel
		 */
		static int primaryDisplayWidth();

		/**
		 * Returns the height of the primary display in pixel.
		 * @return Primary display height in pixel
		 */
		static int primaryDisplayHeight();

		/**
		 * Returns the width of the virtual display in pixel.
		 * The virtual display is bounded by all screens.
		 * @return Virtual display width in pixel
		 */
		static int virtualDisplayWidth();

		/**
		 * Returns the height of the virtual display in pixel.
		 * The virtual display is bounded by all screens.
		 * @return Virtual display height in pixel
		 */
		static int virtualDisplayHeight();

		/**
		 * Returns the bounding box of the virtual display with pixel resolution.
		 * @return The bounding box of the virtual display
		 */
		static CV::PixelBoundingBoxI virtualDisplayBoundingBox();

		/**
		 * Returns the most suitable top left position of a window that has to be placed on the (virtual) screen.
		 * The final position ensures that the window lies inside the bounding box of the (virtual) screen.
		 * @param windowWidth The width of the window for which the position will be determined in pixel, with range (0, infinity)
		 * @param windowHeight The height of the window for which the pose will be determined in pixel, with range (0, infinity)
		 * @param left Optional proposed left position of the window, which will be adjusted to fit into the virtual screen
		 * @param top Optional proposed top position of the window, which will be adjusted to fit into the virtual screen
		 * @param parent Optional handle of a parent (or associated) window to which the resulting position should match if no proposed position is provided
		 * @return The resulting top left position of the window
		 */
		static CV::PixelPositionI suitableWindowPosition(const unsigned int windowWidth, const unsigned int windowHeight, const int left = NumericT<int>::minValue(), const int top = NumericT<int>::minValue(), const HWND parent = nullptr);

		/**
		 * Returns the number of installed screens.
		 * @return Screen number
		 */
		static unsigned int screens();

		/**
		 * Returns the position and dimension of the screen most suitable for a given cursor point.
		 * @param positionX Horizontal cursor position to receive the screen for
		 * @param positionY Vertical cursor position to receive the screen for
		 * @param left Left screen position, pixel in virtual coordinate space
		 * @param top Top screen position, pixel in virtual coordinate space
		 * @param width Screen width, in pixel
		 * @param height Screen height, in pixel
		 * @return True, if succeeded
		 */
		static bool screen(const unsigned int positionX, const unsigned int positionY, unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height);

		/**
		 * Returns the position and dimension of the screen most suitable for a given window handle.
		 * @param window Window handle to receive the screen for
		 * @param left Left screen position, pixel in virtual coordinate space
		 * @param top Top screen position, pixel in virtual coordinate space
		 * @param width Screen width, in pixel
		 * @param height Screen height, in pixel
		 * @return True, if succeeded
		 */
		static bool screen(const HWND window, unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height);

		/**
		 * Returns the current DPI value of a given device context.
		 * If no context is given, the desktop DC is used instead.
		 * A DPI value of 96 complies with a 100 percent scaling, 192 DPI complies with 200 percent scaling
		 * @param dc The device context, nullptr to determine the scale factor of the default screen
		 * @return The resulting DPI value, with range (0, infinity)
		 */
		static int dpi(const HDC dc);

		/**
		 * Returns the scaling factor transforming the number of (backing) pixels defined in the (virtual) display coordinate system to the number of pixels defined in the (native/physical) screen coordinate system for a specified device context.
		 * @param dc The device context, nullptr to determine the scale factor of the default screen
		 * @return The resulting scale factor with range (0, infinity)
		 */
		static double scaleFactor(const HDC dc);

		/**
		 * Converts a given pixel value defined in the (virtual) display coordinate system to the pixel value defined in the (native/physical) screen coordinate system.
		 * @param pixel Pixel value to convert, with range (-infinity, infinity)
		 * @param dc The device context, nullptr to determine the scale factor of the default screen
		 * @return The converted pixel value for the (native/pyhsical) screen coordinate system, with range (-infinity, infinity)
		 */
		static inline int scalePixel(const int pixel, const HDC dc);

		/**
		 * Converts a given pixel value by a given scale factor.
		 * @param pixel Pixel value to convert, with range (-infinity, infinity)
		 * @param scaleFactor The scale factor to be applied, with range (-infinity, infinity)
		 * @return The resulting converted pixel value (-infinity, infinity)
		 */
		static inline int scalePixelByFactor(const int pixel, const double scaleFactor);
};

inline int Screen::scalePixel(const int pixel, const HDC dc)
{
	return scalePixelByFactor(pixel, scaleFactor(dc));
}

inline int Screen::scalePixelByFactor(const int pixel, const double scaleFactor)
{
	return NumericD::round32(double(pixel) * scaleFactor);
}

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_SCREEN_H
