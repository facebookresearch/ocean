/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_SCREEN_H
#define META_OCEAN_PLATFORM_WXWIDGETS_SCREEN_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

#include "ocean/cv/PixelBoundingBox.h"
#include "ocean/cv/PixelPosition.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class implements functions allowing to control screens and windows on screens.
 * @ingroup platformwxwidgets
 */
class OCEAN_PLATFORM_WXWIDGETS_EXPORT Screen
{
	public:

		/**
		 * Returns the width of the primary display in pixel.
		 * @return Primary display width in pixel, with range [0, infinity)
		 */
		static int primaryDisplayWidth();

		/**
		 * Returns the height of the primary display in pixel.
		 * @return Primary display height in pixel, with range [0, infinity)
		 */
		static int primaryDisplayHeight();

		/**
		 * Returns the width of the virtual display in pixel.
		 * The virtual display is bounded by all screens.
		 * @return Virtual display width in pixel, with range [0, infinity)
		 */
		static int virtualDisplayWidth();

		/**
		 * Returns the height of the virtual display in pixel.
		 * The virtual display is bounded by all screens.
		 * @return Virtual display height in pixel, with range [0, infinity)
		 */
		static int virtualDisplayHeight();

		/**
		 * Returns the bounding box of the virtual display with pixel resolution.
		 * @return The bounding box of the virtual display
		 */
		static CV::PixelBoundingBoxI virtualDisplayBoundingBox();

		/**
		 * Returns the most suitable position (top left on Windows and bottom left on OS X platforms) of a window that has to be placed on the (virtual) screen.
		 * The final position ensures that the window lies inside the bounding box of the (virtual) screen.
		 * @param windowWidth The width of the window for which the position will be determined in pixel, with range (0, infinity)
		 * @param windowHeight The height of the window for which the pose will be determined in pixel, with range (0, infinity)
		 * @param left Optional proposed left position of the window, which will be adjusted to fit into the virtual screen
		 * @param top Optional proposed top position of the window, which will be adjusted to fit into the virtual screen
		 * @param parent Optional handle of a parent (or associated) window to which the resulting position should match if no proposed position is provided
		 * @return The resulting top left position of the window
		 */
		static CV::PixelPositionI suitableWindowPosition(const unsigned int windowWidth, const unsigned int windowHeight, const int left = NumericT<int>::minValue(), const int top = NumericT<int>::minValue(), const OCEAN_WXWIDGETS_HANDLE parent = nullptr);

#ifdef __APPLE__

		/**
		 * Returns the most suitable position (top left on Windows and bottom left on OS X platforms) of a window that has to be placed on the (virtual) screen.
		 * The final position ensures that the window lies inside the bounding box of the (virtual) screen.
		 * @param windowWidth The width of the window for which the position will be determined in pixel, with range (0, infinity)
		 * @param windowHeight The height of the window for which the pose will be determined in pixel, with range (0, infinity)
		 * @param left Optional proposed left position of the window, which will be adjusted to fit into the virtual screen
		 * @param top Optional proposed top position of the window, which will be adjusted to fit into the virtual screen
		 * @param parent Optional handle of a parent (or associated) window to which the resulting position should match if no proposed position is provided
		 * @return The resulting top left position of the window
		 */
		static CV::PixelPositionI suitableWindowPositionApple(const unsigned int windowWidth, const unsigned int windowHeight, const int left = NumericT<int>::minValue(), const int top = NumericT<int>::minValue(), const OCEAN_WXWIDGETS_HANDLE parent = nullptr);

#endif

		/**
		 * Returns the scaling factor transforming the number of (backing) pixels defined in the (virtual) display coordinate system to the number of pixels defined in the (native/physical) screen coordinate system for a specified window.
		 * @param window The window for which the scaling factor is determined, nullptr to determine the scale factor of the default screen
		 * @return The resulting scale factor with range (0, infinity)
		 */
		static double scaleFactor(const wxWindow* window);

#ifdef __APPLE__

		/**
		 * Returns the scaling factor transforming the number of (backing) pixels defined in the (virtual) display coordinate system to the number of pixels defined in the (native/physical) screen coordinate system for a specified window.
		 * @param window The window for which the scaling factor is determined, nullptr to determine the scale factor of the default screen
		 * @return The resulting scale factor with range (0, infinity)
		 */
		static double scaleFactorApple(const wxWindow* window);

#endif

		/**
		 * Converts a given pixel value defined in the (virtual) display coordinate system to the pixel value defined in the (native/physical) screen coordinate system.
		 * Beware: On Apple-platforms this function does not apply any scaling, the value is returned directly.
		 * @param pixel Pixel value to convert, with range (-infinity, infinity)
		 * @param window The window for which the transformed/scaled pixel value is determined, nullptr to determine the pixel value for the default screen
		 * @return The converted pixel value for the (native/pyhsical) screen coordinate system, with range (-infinity, infinity)
		 */
		static inline int scalePixel(const int pixel, const wxWindow* window);

		/**
		 * Converts a given size value defined in the (virtual) display coordinate system to the size value defined in the (native/physical) screen coordinate system.
		 * Beware: On Apple-platforms this function does not apply any scaling, the value is returned directly.
		 * @param size Size value to convert, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param window The window for which the transformed/scaled size value is determined, nullptr to determine the size value for the default screen
		 * @return The converted size value for the (native/pyhsical) screen coordinate system, with range (-infinity, infinity)x(-infinity, infinity)
		 */
		static inline wxSize scaleSize(const wxSize& size, const wxWindow* window);

		/**
		 * Converts a given point value defined in the (virtual) display coordinate system to the point value defined in the (native/physical) screen coordinate system.
		 * Beware: On Apple-platforms this function does not apply any scaling, the value is returned directly.
		 * @param point Point value to convert, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param window The window for which the transformed/scaled point value is determined, nullptr to determine the point value for the default screen
		 * @return The converted point value for the (native/pyhsical) screen coordinate system, with range (-infinity, infinity)x(-infinity, infinity)
		 */
		static inline wxPoint scalePoint(const wxPoint& point, const wxWindow* window);

		/**
		 * Converts a given pixel value by a given scale factor.
		 * Beware: On Apple-platforms this function does not apply any scaling, the value is returned directly.
		 * @param pixel Pixel value to convert, with range (-infinity, infinity)
		 * @param scaleFactor The scale factor to be applied, with range (-infinity, infinity)
		 * @return The resulting converted pixel value (-infinity, infinity)
		 */
		static inline int scalePixelByFactor(const int pixel, const double scaleFactor);

		/**
		 * Converts a given size value by a given scale factor.
		 * Beware: On Apple-platforms this function does not apply any scaling, the value is returned directly.
		 * @param size Size value to convert, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param scaleFactor The scale factor to be applied, with range (-infinity, infinity)
		 * @return The converted size value, with range (-infinity, infinity)x(-infinity, infinity)
		 */
		static inline wxSize scaleSizeByFactor(const wxSize& size, const double scaleFactor);

		/**
		 * Converts a given point value by a given scale factor.
		 * Beware: On Apple-platforms this function does not apply any scaling, the value is returned directly.
		 * @param point Point value to convert, with range (-infinity, infinity)x(-infinity, infinity)
		 * @param scaleFactor The scale factor to be applied, with range (-infinity, infinity)
		 * @return The converted point value , with range (-infinity, infinity)x(-infinity, infinity)
		 */
		static inline wxPoint scalePointByFactor(const wxPoint& point, const double scaleFactor);
};

inline int Screen::scalePixel(const int pixel, const wxWindow* window)
{
#ifdef __APPLE__
	return pixel;
#else
	return scalePixelByFactor(pixel, scaleFactor(window));
#endif
}

inline wxSize Screen::scaleSize(const wxSize& size, const wxWindow* window)
{
#ifdef __APPLE__
	return size;
#else
	return scaleSizeByFactor(size, scaleFactor(window));
#endif
}

inline wxPoint Screen::scalePoint(const wxPoint& point, const wxWindow* window)
{
#ifdef __APPLE__
	return point;
#else
	return scalePointByFactor(point, scaleFactor(window));
#endif
}

inline int Screen::scalePixelByFactor(const int pixel, const double scaleFactor)
{
#ifdef __APPLE__
	return pixel;
#else
	return NumericD::round32(double(pixel) * scaleFactor);
#endif
}

inline wxSize Screen::scaleSizeByFactor(const wxSize& size, const double scaleFactor)
{
#ifdef __APPLE__
	return size;
#else
	return wxSize(NumericD::round32(double(size.x) * scaleFactor), NumericD::round32(double(size.y) * scaleFactor));
#endif
}

inline wxPoint Screen::scalePointByFactor(const wxPoint& point, const double scaleFactor)
{
#ifdef __APPLE__
	return point;
#else
	return wxPoint(NumericD::round32(double(point.x) * scaleFactor), NumericD::round32(double(point.y) * scaleFactor));
#endif
}

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_SCREEN_H
