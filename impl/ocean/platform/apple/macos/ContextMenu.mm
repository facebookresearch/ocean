/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/apple/macos/ContextMenu.h"

#include "ocean/base/StringApple.h"

using namespace Ocean::Platform::Apple::MacOS;

@interface OceanPlatformAppleMacOSContextMenu : NSMenu
{
	ContextMenu* menuOwner;
}

/**
 * Sets or changes the owner of this context menu object.
 * @param owner The owner to set, must be valid
 */
-(void)setOwner:(ContextMenu*)owner;

@end

@implementation OceanPlatformAppleMacOSContextMenu

-(id)initWithTitle:(NSString *)title andOwner:(ContextMenu*)owner
{
	menuOwner = nullptr;

	ocean_assert(owner != nullptr);

	if (self = [super initWithTitle:title])
	{
		menuOwner = owner;
	}

	return self;
}

-(void)setOwner:(ContextMenu*)owner
{
	ocean_assert(owner != nullptr);

	menuOwner = owner;
}

-(void)menuItemSelected:(id)sender
{
	ocean_assert(menuOwner != nullptr);

	if (menuOwner)
	{
		menuOwner->onItemSelected((NSMenuItem*)sender);
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

ContextMenu::ContextMenu()
{
	nsMenu_ = [[OceanPlatformAppleMacOSContextMenu alloc] initWithTitle:@"" andOwner:this];
}

int ContextMenu::addItem(const std::string& text, const bool enabled)
{
	ocean_assert(nsMenu_ != nullptr);

	const int index = int(itemTexts_.size());

	if (text == "-")
	{
		[nsMenu_ insertItem:[NSMenuItem separatorItem] atIndex:index];
	}
	else
	{
		NSMenuItem* item = [(OceanPlatformAppleMacOSContextMenu*)nsMenu_ insertItemWithTitle:StringApple::toNSString(text) action:@selector(menuItemSelected:) keyEquivalent:@"" atIndex:index];

		if (enabled)
		{
			[item setTarget:(OceanPlatformAppleMacOSContextMenu*)nsMenu_];
		}
	}

	itemTexts_.push_back(text);

	return index;
}

int ContextMenu::popup()
{
	selectedIndex_ = -1;

	[(OceanPlatformAppleMacOSContextMenu*)nsMenu_ popUpMenuPositioningItem:nil atLocation:[NSEvent mouseLocation] inView:nil];

	return selectedIndex_;
}

void ContextMenu::onItemSelected(NSMenuItem* item)
{
	ocean_assert(selectedIndex_ == -1);
	ocean_assert(item != nullptr);

	const std::string itemTitle = StringApple::toUTF8([item title]);

	for (size_t n = 0; n < itemTexts_.size(); ++n)
	{
		if (itemTexts_[n] == itemTitle)
		{
			selectedIndex_ = int(n);
			break;
		}
	}
}

ContextMenu& ContextMenu::operator=(ContextMenu&& contextMenu) noexcept
{
	if (this != &contextMenu)
	{
		nsMenu_ = contextMenu.nsMenu_;
		contextMenu.nsMenu_ = nullptr;

		if (nsMenu_ != nullptr)
		{
			// the menu calls back into its owner, so the callback has to follow the menu
			[(OceanPlatformAppleMacOSContextMenu*)(nsMenu_) setOwner:this];
		}

		itemTexts_ = std::move(contextMenu.itemTexts_);

		selectedIndex_ = contextMenu.selectedIndex_;
		contextMenu.selectedIndex_ = -1;
	}

	return *this;
}

}

}

}

}
