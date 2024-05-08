/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/macos/FrameView.h"

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

FrameView::FrameView(FrameView&& view) noexcept :
	FlippedView(std::move(view)),
	image_(std::move(view.image_))
{
	// nothing to do here
}

FrameView::~FrameView()
{
	// nothing to do here
}

FrameView::FrameView(const NSRect& rect) :
	FlippedView(rect)
{
	// nothing to do here
}

FrameView::FrameView(const int left, const int top, const unsigned int width, const unsigned int height) :
	FlippedView(left, top, width, height)
{
	// nothing to do here
}

void FrameView::setFrame(const Frame& frame)
{
	setImage(Image(frame, 1.0));
}

void FrameView::setImage(Image&& image)
{
	const ScopedLock scopedLock(lock_);

	image_ = std::move(image);

	dispatch_async(dispatch_get_main_queue(), ^{
		[nsView() setNeedsDisplay:true];
	});
}

Vector2 FrameView::view2image(const Vector2& viewPoint)
{
	unsigned int left = 0u;
	unsigned int top = 0u;
	unsigned int width = 0u;
	unsigned int height = 0u;

	if (determineFrameSize(left, top, width, height))
	{
		if (viewPoint.x() >= Scalar(left) && viewPoint.x() < Scalar(left + width) && viewPoint.y() >= Scalar(top) && viewPoint.y() < Scalar(top + height))
		{
			const Vector2 viewPointInImage(viewPoint.x() - Scalar(left), viewPoint.y() - Scalar(top));

			const NSSize imageSize = [image_.nsImage() size];

			ocean_assert(width != 0u && height != 0u);
			const Vector2 imagePoint(viewPointInImage.x() * Scalar(imageSize.width) / Scalar(width), viewPointInImage.y() * Scalar(imageSize.height) / Scalar(height));

			ocean_assert(imagePoint.x() >= Scalar(0) && imagePoint.x() < Scalar(imageSize.width));
			ocean_assert(imagePoint.y() >= Scalar(0) && imagePoint.y() < Scalar(imageSize.height));

			return imagePoint;
		}
	}

	return Vector2(Scalar(-1), Scalar(-1));
}

void FrameView::onDraw()
{
	const ScopedLock scopedLock(lock_);

	unsigned int left = 0u;
	unsigned int top = 0u;
	unsigned int width = 0u;
	unsigned int height = 0u;

	if (determineFrameSize(left, top, width, height))
	{
		[image_.nsImage() drawInRect:NSMakeRect(CGFloat(left), CGFloat(top), CGFloat(width), CGFloat(height)) fromRect:NSZeroRect operation:NSCompositingOperationCopy fraction:1 respectFlipped:YES hints:nil];
	}
}

FrameView& FrameView::operator=(FrameView&& view) noexcept
{
	if (this != &view)
	{
		FlippedView::operator=(std::move(view));

		image_ = std::move(view.image_);
	}

	return *this;
}

bool FrameView::determineFrameSize(unsigned int& left, unsigned int& top, unsigned int& width, unsigned int& height)
{
	if (!image_.isValid())
	{
		return false;
	}

	const NSRect viewBounds = [nsView() bounds];
	const NSSize imageSize = [image_.nsImage() size];

	if (viewBounds.size.width > 0 && viewBounds.size.height > 0 && imageSize.width > 0 && imageSize.height > 0)
	{
		CGFloat stretchWidth = 0;
		CGFloat stretchHeight = 0;

		if (viewBounds.size.width >= imageSize.width && viewBounds.size.height >= imageSize.height)
		{
			stretchWidth = imageSize.width;
			stretchHeight = imageSize.height;
		}
		else
		{
			const CGFloat imageAspectRatio = imageSize.width / imageSize.height;
			const CGFloat viewAspectRatio = viewBounds.size.width / viewBounds.size.height;
			ocean_assert(imageAspectRatio > 0 && viewAspectRatio > 0);

			if (viewAspectRatio > imageAspectRatio)
			{
				stretchHeight = viewBounds.size.height;

				// we avoid sub-pixel locations due to performance reasons
				stretchWidth = CGFloat(NumericD::round32(viewBounds.size.height * imageAspectRatio));
			}
			else
			{
				stretchWidth = viewBounds.size.width;

				// we avoid sub-pixel locations due to performance reasons
				stretchHeight = CGFloat(NumericD::round32(viewBounds.size.width / imageAspectRatio));
			}
		}

		if (stretchWidth < 1.0 || stretchHeight < 1.0)
		{
			return false;
		}

		// we avoid sub-pixel locations due to performance reasons
		const CGFloat stretchLeft = floor(viewBounds.origin.x + (viewBounds.size.width - stretchWidth) * 0.5);
		const CGFloat stretchTop = floor(viewBounds.origin.y + (viewBounds.size.height - stretchHeight) * 0.5);

		ocean_assert(stretchLeft >= viewBounds.origin.x);
		ocean_assert(stretchTop >= viewBounds.origin.y);

		ocean_assert(stretchLeft >= 0.0);
		ocean_assert(stretchTop >= 0.0);

		left = (unsigned int)stretchLeft;
		top = (unsigned int)stretchTop;

		width = (unsigned int)stretchWidth;
		height = (unsigned int)stretchHeight;

		return true;
	}

	return false;
}

}

}

}

}
