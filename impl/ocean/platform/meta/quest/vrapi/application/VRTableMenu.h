// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_TABLE_MENU_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_TABLE_MENU_H

#include "ocean/platform/meta/quest/vrapi/application/Application.h"

#include "ocean/platform/meta/quest/application/VRTableMenuBase.h"

#include "ocean/platform/meta/quest/vrapi/TrackedRemoteDevice.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace VrApi
{

namespace Application
{

/**
 * The class implements a simple table-based menu for VR application.
 * The menu is composed of sections and menu entries.<br>
 * Based on controller tracking, the menu entries can be selected.
 * @ingroup platformmetaquestvrapiapplication
 */
class OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT VRTableMenu final : public Quest::Application::VRTableMenuBase
{
	public:

		/**
		 * Creates a new invalid menu object.
		 */
		VRTableMenu() = default;

		/**
		 * Move constructor.
		 * @param menu The menu object to be moved
		 */
		VRTableMenu(VRTableMenu&& menu) = default;

		/**
		 * Creates a new valid menu object.
		 * @param engine The rendering engine which will be used to render the menu, must be valid
		 * @param framebuffer The framebuffer associated with the rendering engine to which the menu will be rendered
		 */
		inline VRTableMenu(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer);

		/**
		 * Events function called before the menu is rendered.
		 * @param trackedRemoteDevice The tracked remote device providing access to the controller tracking, must be valid
		 * @param renderTimestamp The timestamp when the menu will be rendered
		 * @param url The resulting menu entry url in case an entry was selected
		 * @return True, if an entry was selected; False, if no entry was selected
		 */
		bool onPreRender(TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& renderTimestamp, std::string& url);

		/**
		 * Move operator.
		 * @param menu The menu object to be moved
		 * @return Reference to this object
		 */
		VRTableMenu& operator=(VRTableMenu&& menu) = default;

    protected:

        /**
		 * Disabled copy constructor.
		 */
		VRTableMenu(const VRTableMenu&) = delete;

        /**
		 * Disabled copy operator.
		 * @param menu The menu object which would be copied
		 * @return The reference to this object
		 */
		VRTableMenu& operator=(const VRTableMenu& menu) = delete;
};

inline VRTableMenu::VRTableMenu(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer) :
	VRTableMenuBase(engine, framebuffer)
{
	// nothing to do here
}

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_TABLE_MENU_H
