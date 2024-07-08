/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_MACOS_FLIPPED_VIEW_H
#define META_OCEAN_PLATFORM_APPLE_MACOS_FLIPPED_VIEW_H

#include "ocean/platform/apple/macos/MacOS.h"

#include "ocean/math/Vector2.h"

#ifndef __OBJC__
	#error Platform::Apple::MacOS::FlippedView.h needs to be included from an ObjectiveC++ file
#endif

#include <AppKit/AppKit.h>

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

/**
 * This class wrapps an MacOS NSView object and flips the coordinate system.
 * @ingroup platformapplemacos
 */
class FlippedView
{
	protected:

		/**
		 * Definition of a vector holding 8 bit values.
		 */
		typedef std::vector<unsigned char> EnabledStates;

		/**
		 * Definition of a vector holding view objects.
		 */
		typedef std::vector<NSView*> NSViews;

	public:

		/**
		 * Creates an invalid object.
		 */
		FlippedView() = default;

		/**
		 * Move constructor.
		 * @param view View object to move
		 */
		FlippedView(FlippedView&& view) noexcept;

		/**
		 * Not existing copy constructor.
		 * @param view View object to copy
		 */
		FlippedView(const FlippedView& view) = delete;

		/**
		 * Creates a new view object with specified size and dimension.
		 * @param rect The rect defining the size and dimension
		 */
		explicit FlippedView(const NSRect& rect);

		/**
		 * Creates a new view object with specified size and dimension.
		 * @param left The horizontal start position of this view, with range (-infinity, infinity)
		 * @param top The vertical start position of this view in pixel, with range (-infinity, infinity)
		 * @param width The width of the view in pixel, with range [0, infinity)
		 * @param height The height of the view in pixel, with range [0, infinity)
		 */
		FlippedView(const int left, const int top, const unsigned int width, const unsigned int height);

		/**
		 * Destructs a flipped view object.
		 */
		virtual ~FlippedView();

		/**
		 * Returns whether this view is enabled.
		 * Beware: You should not modified the 'enable statements' of any child control is this view is disabled.
		 * @return True, if so
		 */
		inline bool isEnabled() const;

		/**
		 * Enables or disables this view.
		 * @param enable True, to enable the view; False, to disable the view
		 */
		void setEnabled(const bool enable);

		/**
		 * Returns the MacOS specific view object.
		 * @return The view object
		 */
		inline NSView* nsView();

		/**
		 * Returns whether this object wrapps a valid NSView object.
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * The event function before this view will be redrawn.
		 */
		virtual void onRedrawing();

		/**
		 * The draw event function actually displaying this view.
		 */
		virtual void onDraw();

		/**
		 * The function for left mouse button down events.
		 * @param mouseLocation The location of the mouse within the coordinate symste of this view
		 */
		virtual void onMouseDownLeft(const Vector2& mouseLocation);

		/**
		 * The function for left mouse button up events.
		 * @param mouseLocation The location of the mouse within the coordinate symste of this view
		 */
		virtual void onMouseUpLeft(const Vector2& mouseLocation);

		/**
		 * Returns whether this object wrapps a valid NSView object.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

		/**
		 * Move operator.
		 * @param view View object to be moved
		 * @return Reference to this object
		 */
		FlippedView& operator=(FlippedView&& view) noexcept;

		/**
		 * Not existing assign operator.
		 * @param view View object to be copied
		 * @return Reference to this object
		 */
		FlippedView& operator=(const FlippedView& view) = delete;

	protected:

		/// The MacOS specific NSView object.
		NSView* nsView_ = nullptr;

		/// True, if the view is enabled.
		bool enabled_ = false;

		/// The child controls for which a corresponding recover enabled statement has been determined.
		NSMutableArray* childControls_ = nullptr;

		/// The enable statements of all child controls which have been determined as this view got disabled.
		EnabledStates recoverChildControls_;
};

inline bool FlippedView::isEnabled() const
{
	return enabled_;
}

inline NSView* FlippedView::nsView()
{
	return nsView_;
}

inline bool FlippedView::isValid() const
{
	return nsView_ != nullptr;
}

inline FlippedView::operator bool() const
{
	return isValid();
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_MACOS_FLIPPED_VIEW_H
