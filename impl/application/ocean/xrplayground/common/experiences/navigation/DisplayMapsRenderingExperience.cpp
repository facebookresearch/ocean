// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/navigation/DisplayMapsRenderingExperience.h"

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY

namespace Ocean
{

namespace XRPlayground
{

// START OF EXPERIENCE

DisplayMapsRenderingExperience::DisplayMapsRenderingExperience()
{
  // nothing to do here
}

DisplayMapsRenderingExperience::~DisplayMapsRenderingExperience()
{
  // nothing to do here
}

bool DisplayMapsRenderingExperience::load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties)
{
  ocean_assert(engine);

#ifdef OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
  DisplayMapsRenderingExperience::setUpIOSInterface(userInterface, engine, *this, properties);
#endif
  return true;
}

bool DisplayMapsRenderingExperience::unload(const Interaction::UserInterface& /*interface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
  return true;
}

std::unique_ptr<XRPlaygroundExperience> DisplayMapsRenderingExperience::createExperience()
{
  return std::unique_ptr<XRPlaygroundExperience>(new DisplayMapsRenderingExperience());
}

} // namespace XRPlayground

} // namespace Ocean

#endif // OCEAN_PLATFORM_BUILD_APPLE_IOS_ANY
