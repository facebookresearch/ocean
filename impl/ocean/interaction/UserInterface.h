/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_INTERACTION_USER_INTERFACE_H
#define META_OCEAN_INTERACTION_USER_INTERFACE_H

#include "ocean/interaction/Interaction.h"

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY) && defined(__OBJC__)
	#import <UIKit/UIKit.h>
#endif

namespace Ocean
{

namespace Interaction
{

/**
 * This class holds UI elements of the application from which the interaction is executed.
 * @ingroup interaction
 */
class OCEAN_INTERACTION_EXPORT UserInterface
{
	public:

		/**
		 * Default constructor.
		 */
		UserInterface() = default;

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY) && defined(__OBJC__)

		/**
		 * Creates new user interface object for iOS applications.
		 * @param window The application's window, must be valid
		 * @param viewController The application's view controller, must be valid
		 */
		UserInterface(UIWindow* window, UIViewController* viewController);

		/**
		 * Returns the window of the application.
		 * @return The application's window; nullptr if not an iOS application
		 */
		UIWindow* window() const;

		/**
		 * Returns the view controller of the iOS application.
		 * @return The application's view controller; nullptr if not an iOS application
		 */
		UIViewController* viewController() const;
#endif

		/**
		 * Returns whether this object holds UI elements for an iOS application.
		 * @return True, if so
		 */
		inline bool isIOS() const;

	protected:

		/// True, in case this object holds UI elements for an iOS application.
		bool isIOS_ = false;

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY) && defined(__OBJC__)

		/// The application's window; nullptr if not an iOS application.
		UIWindow* window_ = nullptr;

		/// The application's view controller; nullptr if not an iOS application.
		UIViewController* viewController_ = nullptr;

#endif
};

inline bool UserInterface::isIOS() const
{
	return isIOS_;
}

}

}

#endif // META_OCEAN_INTERACTION_USER_INTERFACE_H
