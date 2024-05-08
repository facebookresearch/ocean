/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WIN_TEXT_WINDOW_H
#define META_OCEAN_PLATFORM_WIN_TEXT_WINDOW_H

#include "ocean/platform/win/Win.h"
#include "ocean/platform/win/Window.h"

namespace Ocean
{

namespace Platform
{

namespace Win
{

/**
 * This class implements a text window.
 * @ingroup platformwin
 */
class OCEAN_PLATFORM_WIN_EXPORT TextWindow : virtual public Window
{
	public:

		/**
		 * Creates a new text window.
		 * @param instance Application instance
		 * @param name The name of the window
		 * @param isChild True, if the window is intended to be a child window
		 */
		TextWindow(HINSTANCE instance, const std::wstring& name, const bool isChild = false);

		/**
		 * Destructs the text window.
		 */
		~TextWindow() override;

		/**
		 * Returns the text of this window.
		 * @return The text of this window
		 */
		virtual std::string text() const;

		/**
		 * Sets the text of this text window.
		 * @param text The text to be set
		 * @return True, if succeeded
		 */
		virtual bool setText(const std::string& text);

		/**
		 * Appends text to this text window.
		 * @param text The text to be appended
		 * @return True, if succeeded
		 */
		virtual bool appendText(const std::string& text);

		/**
		 * Clears the text of this text window.
		 */
		virtual bool clearText();

	protected:

		/**
		 * Creates the window itself using the registered window class.
		 * @return True, if succeeded
		 */
		bool createWindow() override;
};

}

}

}

#endif // META_OCEAN_PLATFORM_WIN_TEXT_WINDOW_H
