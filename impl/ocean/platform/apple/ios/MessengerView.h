// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_APPLE_IOS_MESSENGER_VIEW_H
#define META_OCEAN_PLATFORM_APPLE_IOS_MESSENGER_VIEW_H

#include "ocean/platform/apple/ios/IOS.h"

#ifndef __OBJC__
	#error Platform::Apple::IOS::MessengerView.h needs to be included from an ObjectiveC++ file
#endif

#import <UIKit/UIKit.h>

/**
 * MessengerView implements a text view showing Ocean's log messages.
 * @ingroup platformappleios
 */
@interface MessengerView : UITextView

@end

#endif // META_OCEAN_PLATFORM_APPLE_IOS_MESSENGER_VIEW_H
