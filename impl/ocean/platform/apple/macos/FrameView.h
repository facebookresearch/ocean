/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_MACOS_FRAME_VIEW_H
#define META_OCEAN_PLATFORM_APPLE_MACOS_FRAME_VIEW_H

#include "ocean/platform/apple/macos/MacOS.h"
#include "ocean/platform/apple/macos/FlippedView.h"
#include "ocean/platform/apple/macos/Image.h"

#include "ocean/base/Frame.h"

#ifndef __OBJC__
	#error Platform::Apple::MacOS::FrameView.h needs to be included from an ObjectiveC++ file
#endif

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

/**
 * This class implements a view displaying a frame.
 * In general, the display performance/efficiency is not very good,<br>
 * use GPU optimized solutions instead whenever performance matters.<br>
 * @ingroup platformapplemacos
 */
class FrameView : public FlippedView
{
	public:

		/**
		 * Default constructor creating an invalid media view object.
		 */
		FrameView() = default;

		/**
		 * Destructs this media view object and releases the medium object.
		 */
		~FrameView() override;

		/**
		 * Move constructor.
		 * @param view View object to move
		 */
		FrameView(FrameView&& view) noexcept;

		/**
		 * Creates a new view object with specified size and dimension.
		 * @param rect The rect defining the size and dimension
		 */
		explicit FrameView(const NSRect& rect);

		/**
		 * Creates a new view object with specified size and dimension.
		 * @param left The horizontal start position of this view, with range (-infinity, infinity)
		 * @param top The vertical start position of this view in pixel, with range (-infinity, infinity)
		 * @param width The width of the view in pixel, with range [0, infinity)
		 * @param height The height of the view in pixel, with range [0, infinity)
		 */
		FrameView(const int left, const int top, const unsigned int width, const unsigned int height);

		/**
		 * Sets the new frame to be displayed.
		 * Internally, this function will create a copy of the frame buffer in the internal image object.
		 * @param frame The frame to be displayed, an invalid frame to display no frame
		 */
		void setFrame(const Frame& frame);

		/**
		 * Sets the new image to be displayed, actually the image will be moved.
		 * @param image The image to be displayed, will be moved, an invalid image to display nothing
		 */
		void setImage(Image&& image);

		/**
		 * Converts a 2D location defined in the view's coordinate system to a location defined in the coordinate system of the frame.
		 * @param viewPoint The 2D location within this view
		 * @return The corresponding 2D location within the image frame of the medium, a negative coordinate (-1, -1) in case the provided point is outside the frame
		 */
		Vector2 view2image(const Vector2& viewPoint);

		/**
		 * The draw event function actually displaying this view.
		 */
		void onDraw() override;

		/**
		 * Move operator.
		 * @param view View object to be moved
		 * @return Reference to this object
		 */
		FrameView& operator=(FrameView&& view) noexcept;

	protected:

		/**
		 * Determines the location and size of the frame to be displayed best matching with the current size of the view.
		 * @param left The resulting horizontal start location of the frame within this view, with range [0, bounds.size.width)
		 * @param top The resulting vertical start location of the frame within this view, with range [0, bounds.size.height)
		 * @param width The resulting width of the frame within this view, with range [1, bounds.size.width]
		 * @param height The resulting height of the frame within this view, with range [1, bounds.size.height]
		 * @return True, if succeeded
		 */
		bool determineFrameSize(unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height);

	protected:

		/// The image that will be displyed.
		Image image_;

		/// The lock for this view.
		Lock lock_;
};

}

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_MACOS_FRAME_VIEW_H
