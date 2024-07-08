/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_APPLE_MACOS_FRAME_MEDIUM_VIEW_H
#define META_OCEAN_PLATFORM_APPLE_MACOS_FRAME_MEDIUM_VIEW_H

#include "ocean/platform/apple/macos/MacOS.h"
#include "ocean/platform/apple/macos/FrameView.h"

#include "ocean/base/Timestamp.h"
#include "ocean/base/Thread.h"

#include "ocean/media/FrameMedium.h"

#ifndef __OBJC__
	#error Platform::Apple::MacOS::FrameMediumView.h needs to be included from an ObjectiveC++ file
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
 * This class implements a view displaying a frame medium.
 * In general, the display performance/efficiency is not very good,<br>
 * use GPU optimized solutions instead whenever performance matters.<br>
 * This class can be seen as a OpenGLFrameMediumViewController equivalent not using OpenGLES.
 * @ingroup platformapplemacos
 */
class FrameMediumView :
	public FrameView,
	protected Thread
{
	public:

		/**
		 * Default constructor creating an invalid media view object.
		 */
		FrameMediumView() = default;

		/**
		 * Destructs this media view object and releases the medium object.
		 */
		~FrameMediumView() override;

		/**
		 * Move constructor.
		 * @param view View object to move
		 */
		FrameMediumView(FrameMediumView&& view) noexcept;

		/**
		 * Creates a new view object with specified size and dimension.
		 * @param rect The rect defining the size and dimension
		 */
		explicit FrameMediumView(const NSRect& rect);

		/**
		 * Creates a new view object with specified size and dimension.
		 * @param left The horizontal start position of this view, with range (-infinity, infinity)
		 * @param top The vertical start position of this view in pixel, with range (-infinity, infinity)
		 * @param width The width of the view in pixel, with range [0, infinity)
		 * @param height The height of the view in pixel, with range [0, infinity)
		 */
		FrameMediumView(const int left, const int top, const unsigned int width, const unsigned int height);

		/**
		 * Sets or replaces the frame medium to be displayed in the view.
		 * @param frameMedium The frame medium to be displayed, an invalid object to remove the current frame medium
		 */
		void setFrameMedium(const Media::FrameMediumRef& frameMedium);

		/**
		 * Move operator.
		 * @param view View object to be moved
		 * @return Reference to this object
		 */
		inline FrameMediumView& operator=(FrameMediumView&& view) noexcept;

	protected:

		/**
		 * Thread run function.
		 */
		void threadRun() override;

	protected:

		/// The frame medium delivering the frames to be displayed.
		Media::FrameMediumRef frameMedium_;

		/// The timestamp of the most recent frame that has been displyed.
		Timestamp frameTimestamp_;
};

inline FrameMediumView& FrameMediumView::operator=(FrameMediumView&& view) noexcept
{
	if (this != &view)
	{
		setFrameMedium(view.frameMedium_);
		frameTimestamp_ = view.frameTimestamp_;

		FrameView::operator=(std::move(view));

		view.frameMedium_.release();
		view.frameTimestamp_.toInvalid();
	}

	return *this;
}

}

}

}

}

#endif // META_OCEAN_PLATFORM_APPLE_MACOS_FRAME_MEDIUM_VIEW_H
