/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_WXWIDGETS_WX_SCOPED_PARENT_WINDOW_H
#define META_OCEAN_PLATFORM_WXWIDGETS_WX_SCOPED_PARENT_WINDOW_H

#include "ocean/platform/wxwidgets/WxWidgets.h"

namespace Ocean
{

namespace Platform
{

namespace WxWidgets
{

/**
 * This class implements a parent window taking on an already existing window handle as long as the instance of the scoped window exists.
 * @ingroup platformwxwidgets
 */
class OCEAN_PLATFORM_WXWIDGETS_EXPORT WxScopedParentWindow
{
	public:

		/**
		 * Creates a new scoped window.
		 */
		WxScopedParentWindow();

		/**
		 * Creates a new scoped window.
		 * @param handle The handle of an already existing window which will be taken over as long as the scoped window exists, may be nullptr which is then equivalent to the default constructor
		 */
		explicit WxScopedParentWindow(OCEAN_WXWIDGETS_HANDLE handle);

		/**
		 * Move constructor.
		 * @param window Window to move
		 */
		WxScopedParentWindow(WxScopedParentWindow&& window) noexcept;

		/**
		 * Destructs a scoped window object and gives the external window handle back.
		 */
		~WxScopedParentWindow();

		/**
		 * Explicitly releases the internal window and gives the external window handle back.
		 * Use this function to release the internal handle before the scope finishes.
		 */
		void release();

		/**
		 * Move operator.
		 * @param window Window to move
		 * @return Reference to this object
		 */
		WxScopedParentWindow& operator=(WxScopedParentWindow&& window) noexcept;

		/**
		 * Dereferencing operator providing access to the internal window object.
		 * Beware: Ensure that this object has not been released before!
		 * @return The internal window object
		 * @see release().
		 */
		wxWindow& operator*() const;

	protected:

		/**
		 * Disabled copy constructor.
		 */
		WxScopedParentWindow(const WxScopedParentWindow&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		WxScopedParentWindow& operator=(const WxScopedParentWindow&) = delete;

	protected:

		/// True, if the internal window is associated.
		bool assocated_ = false;

		/// The internal window object.
		wxWindow* window_ = nullptr;
};

}

}

}

#endif // META_OCEAN_PLATFORM_WXWIDGETS_WX_SCOPED_PARENT_WINDOW_H
