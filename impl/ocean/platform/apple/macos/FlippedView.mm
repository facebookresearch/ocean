/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/macos/FlippedView.h"

using namespace Ocean;
using namespace Ocean::Platform::Apple::MacOS;

/**
 * This class implements a flipped view.
 */
@interface OceanPlatformAppleMacOSFlippedView : NSView
{
	/// The owner of this MacOS specific object.
	FlippedView* owner_;
}

/**
 * Initializes the view object.
 * @param frameRect The initial rect of the view
 * @param owner The owner of the view
 */
-(id)initWithFrame:(NSRect)frameRect andOwner:(FlippedView*)owner;

/**
 * Sets or changes the owner of this outline view object.
 * @param owner The owner to set
 */
-(void)setOwner:(FlippedView*)owner;

@end

@implementation OceanPlatformAppleMacOSFlippedView

-(id)initWithFrame:(NSRect)frameRect
{
	self = [self initWithFrame:frameRect andOwner:nullptr];
	return self;
}

-(id)initWithFrame:(NSRect)frameRect andOwner:(FlippedView*)owner
{
	owner_ = nullptr;

	if (self = [super initWithFrame:frameRect])
	{
		owner_ = owner;
	}

	return self;
}

-(void)setOwner:(FlippedView*)owner
{
	ocean_assert((owner_ == nullptr || owner_ != owner) && "Never change the owner!");
	owner_ = owner;
}

- (BOOL)isFlipped
{
	return true;
}

- (void)viewWillDraw
{
	[super viewWillDraw];

	if (owner_ != nullptr)
	{
		owner_->onRedrawing();
	}
}

- (void)drawRect:(NSRect)dirtyRect
{
	[super drawRect:dirtyRect];

	if (owner_ != nullptr)
	{
		owner_->onDraw();
	}
}

- (void)mouseDown:(NSEvent*)theEvent
{
	[super mouseDown:theEvent];

	NSPoint locationInWindow = theEvent.locationInWindow;
	NSPoint locationInView = [self convertPoint:locationInWindow fromView:nil];

	if (owner_ != nullptr)
	{
		owner_->onMouseDownLeft(Vector2(Scalar(locationInView.x), Scalar(locationInView.y)));
	}
}

- (void)mouseUp:(NSEvent*)theEvent
{
	[super mouseUp:theEvent];

	NSPoint locationInWindow = theEvent.locationInWindow;
	NSPoint locationInView = [self convertPoint:locationInWindow fromView:nil];

	if (owner_ != nullptr)
	{
		owner_->onMouseUpLeft(Vector2(Scalar(locationInView.x), Scalar(locationInView.y)));
	}
}

@end

namespace Ocean
{

namespace Platform
{

namespace Apple
{

namespace MacOS
{

FlippedView::FlippedView(FlippedView&& view) noexcept :
	nsView_(view.nsView_),
	enabled_(view.enabled_),
	childControls_(view.childControls_),
	recoverChildControls_(std::move(view.recoverChildControls_))
{
	if (nsView_ != nullptr)
	{
		[(OceanPlatformAppleMacOSFlippedView*)(nsView_) setOwner:this];
	}

	view.nsView_ = nullptr;
	view.enabled_ = false;
	view.childControls_ = nullptr;
}

FlippedView::FlippedView(const NSRect& rect) :
	nsView_(nullptr),
	enabled_(true)
{
	nsView_ = [[OceanPlatformAppleMacOSFlippedView alloc] initWithFrame:rect andOwner:this];
}

FlippedView::FlippedView(const int left, const int top, const unsigned int width, const unsigned int height) :
	nsView_(nullptr),
	enabled_(true)
{
	nsView_ = [[OceanPlatformAppleMacOSFlippedView alloc] initWithFrame:NSMakeRect(CGFloat(left), CGFloat(top), CGFloat(width), CGFloat(height)) andOwner:this];
}

FlippedView::~FlippedView()
{
	// we reset the owner
	[(OceanPlatformAppleMacOSFlippedView*)(nsView_) setOwner:nullptr];

	nsView_ = nullptr;
	childControls_ = nullptr;
}

void FlippedView::setEnabled(const bool enable)
{
	ocean_assert(nsView_ != nullptr);
	if (nsView_ == nullptr)
	{
		return;
	}

	if (enabled_ == enable)
		return;

	if (enable)
	{
		// we recover the 'enabled statements' of all previously enumerated controls

		ocean_assert(childControls_ != nullptr);
		ocean_assert(childControls_.count == recoverChildControls_.size());

		for (size_t n = 0; n < recoverChildControls_.size(); ++n)
		{
			NSControl* control = [childControls_ objectAtIndex:n];
			ocean_assert(control);

			ocean_assert([control isEnabled] == false);
			[control setEnabled:recoverChildControls_[n]];
		}

		childControls_ = nullptr;
		recoverChildControls_.clear();
	}
	else
	{
		// we enumerate all child controls (recursively), store their 'enabled statements' and disable all controls
		ocean_assert(childControls_ == nullptr);
		ocean_assert(recoverChildControls_.empty());

		childControls_ = [[NSMutableArray alloc] init];
		recoverChildControls_.clear();

		NSViews viewStack(1, nsView());

		while (!viewStack.empty())
		{
			NSView* viewObjectBack = viewStack.back();
			viewStack.pop_back();

			NSArray* subviews = [viewObjectBack subviews];

			for (size_t n = 0; n < subviews.count; ++n)
			{
				NSObject* object = [subviews objectAtIndex:n];
				ocean_assert(object);

				if ([object isKindOfClass:[NSControl class]])
				{
					NSControl* control = (NSControl*)object;
					ocean_assert(control);

					[childControls_ addObject:control];
					recoverChildControls_.push_back([control isEnabled] ? 1u : 0u);

					[control setEnabled:false];
				}
				else if ([object isKindOfClass:[NSView class]])
				{
					NSControl* view = (NSControl*)object;
					ocean_assert(view);

					viewStack.push_back(view);
				}
			}
		}

		ocean_assert(childControls_.count == recoverChildControls_.size());
	}

	enabled_ = enable;
}

void FlippedView::onRedrawing()
{
	// should be implemented in derived classes
}

void FlippedView::onDraw()
{
	// should be implemented in derived classes
}

void FlippedView::onMouseDownLeft(const Vector2& mouseLocation)
{
	// should be implemented in derived classes
}

void FlippedView::onMouseUpLeft(const Vector2& mouseLocation)
{
	// should be implemented in derived classes
}

FlippedView& FlippedView::operator=(FlippedView&& view) noexcept
{
	if (this != &view)
	{
		// we do not release the MacOS specific object

		nsView_ = view.nsView_;
		view.nsView_ = nullptr;

		if (nsView_ != nullptr)
		{
			[(OceanPlatformAppleMacOSFlippedView*)(nsView_) setOwner:this];
		}

		enabled_ = view.enabled_;
		view.enabled_ = false;

		childControls_ = view.childControls_;
		view.childControls_ = nullptr;

		recoverChildControls_ = std::move(view.recoverChildControls_);
	}

	return *this;
}

}

}

}

}
