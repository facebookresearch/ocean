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

-(void)menuItemSelected:(id)sender
{
	ocean_assert(menuOwner != nullptr);

	if (menuOwner)
		menuOwner->onItemSelected((NSMenuItem*)sender);
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

ContextMenu::ContextMenu() :
	contextMenuNSMenu(nullptr),
	contextSelectedIndex(-1)
{
	contextMenuNSMenu = [[OceanPlatformAppleMacOSContextMenu alloc] initWithTitle:@"" andOwner:this];
}

int ContextMenu::addItem(const std::string& text, const bool enabled)
{
	ocean_assert(contextMenuNSMenu != nullptr);

	const int index = int(contextMenuItemTexts.size());

	if (text == "-")
	{
		[contextMenuNSMenu insertItem:[NSMenuItem separatorItem] atIndex:index];
	}
	else
	{
		NSMenuItem* item = [(OceanPlatformAppleMacOSContextMenu*)contextMenuNSMenu insertItemWithTitle:StringApple::toNSString(text) action:@selector(menuItemSelected:) keyEquivalent:@"" atIndex:index];

		if (enabled)
		{
			[item setTarget:(OceanPlatformAppleMacOSContextMenu*)contextMenuNSMenu];
		}
	}

	contextMenuItemTexts.push_back(text);

	return index;
}

int ContextMenu::popup()
{
	contextSelectedIndex = -1;

	[(OceanPlatformAppleMacOSContextMenu*)contextMenuNSMenu popUpMenuPositioningItem:nil atLocation:[NSEvent mouseLocation] inView:nil];

	return contextSelectedIndex;
}

void ContextMenu::onItemSelected(NSMenuItem* item)
{
	ocean_assert(contextSelectedIndex == -1);
	ocean_assert(item != nullptr);

	const std::string itemTitle = StringApple::toUTF8([item title]);

	for (size_t n = 0; n < contextMenuItemTexts.size(); ++n)
		if (contextMenuItemTexts[n] == itemTitle)
		{
			contextSelectedIndex = int(n);
			break;
		}
}

ContextMenu& ContextMenu::operator=(ContextMenu&& contextMenu) noexcept
{
	if (this != &contextMenu)
	{
		contextMenuNSMenu = contextMenu.contextMenuNSMenu;
		contextMenu.contextMenuNSMenu = nullptr;

		contextMenuItemTexts = std::move(contextMenu.contextMenuItemTexts);

		contextSelectedIndex = contextMenu.contextSelectedIndex;
		contextMenu.contextSelectedIndex = -1;
	}

	return *this;
}

}

}

}

}
