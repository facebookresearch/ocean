// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_DISPLAY_MAPS_RENDERING_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_DISPLAY_MAPS_RENDERING_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

#include <string>

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)

#include <objc/runtime.h>

#if defined(__OBJC__)

#import <UIKit/UIKit.h>

#endif

#endif

#ifdef __OBJC__
#else
#endif

namespace Ocean {

namespace XRPlayground {

class DisplayMapsRenderingExperience : public XRPlaygroundExperience {

 public:
 /**
  * Destructs this experience, all resources will be released.
  */
 ~DisplayMapsRenderingExperience() override;

 /**
  * Loads this experience.
  * @see Experience::load().
  */
 bool load(
     const Interaction::UserInterface& userInterface,
     const Rendering::EngineRef& engine,
     const Timestamp timestamp,
     const std::string& properties) override;

 /**
  * Unloads this experience.
  * @see Experience::unload().
  */
 bool unload(
     const Interaction::UserInterface& userInterface,
     const Rendering::EngineRef& engine,
     const Timestamp timestamp) override;

 /**
  * Creates a new WorldLayerCircleExperience object.
  * @return The new experience
  */
 static std::unique_ptr<XRPlaygroundExperience> createExperience();

protected:
 /**
  * Protected default constructor.
  */
  DisplayMapsRenderingExperience();

#if defined(OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY)
  static void setUpIOSInterface(
      const Interaction::UserInterface& userInterface,
      const Rendering::EngineRef& engine,
      DisplayMapsRenderingExperience& experience,
      const std::string& properties);
#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

};

}

}

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_DISPLAY_MAPS_RENDERING_EXPERIENCE_H
