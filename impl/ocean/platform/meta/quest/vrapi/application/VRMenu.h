// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_MENU_H
#define META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_MENU_H

#include "ocean/platform/meta/quest/vrapi/application/Application.h"

#include "ocean/math/Box2.h"
#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/platform/meta/quest/vrapi/TrackedRemoteDevice.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Transform.h"

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
 * The class implements a simple menu for VR application.
 * The menu is composed of a background image and several foreground images (the menu entries).<br>
 * Based on controller tracking, the menu entries can be selected.
 * @ingroup platformmetaquestvrapiapplication
 */
class OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_EXPORT VRMenu
{
	protected:

		/**
		 * This class contains the information necessary for one menu entry.
		 */
		class MenuEntry
		{
			public:

				/**
				 * Creates an new mentry entry object.
				 * @param id The event id associated with the menu entry, with range [0, infinity) \ {invalidEntryId}
				 * @param menu_T_entry The transformation between the entry and the menu, must be valid
				 * @param width The width of the menu entry in domain of the background image (in pixel), with range [1, infinity)
				 * @param height The height of the menu entry in domain of the backgorund image (in pixel), with range [1, infinity)
				 */
				inline MenuEntry(const unsigned int id, const HomogenousMatrix4& menu_T_entry, const Scalar width, const Scalar height);

				/**
				 * Returns the event id associated with the menu entry.
				 * @return The entry's event id, with range [0, infinity) \ {invalidEntryId}.
				 */
				inline unsigned int id() const;

				/**
				 * Returns the transformation bet the entry and the menu.
				 * @return The transformation bet the entry and the menu
				 */
				inline const HomogenousMatrix4& menu_T_entry() const;

				/**
				 * Returns the width of the menu entry.
				 * @return The entry's width in domain of the background image (in pixel), with range [1, infinity)
				 */
				inline Scalar width() const;

				/**
				 * Return sthe height of the meny entry.
				 * @return The entry's height in domain of the backgorund image (in pixel), with range [1, infinity)
				 */
				inline Scalar height() const;

			protected:

				/// The event id associated with the menu entry, with range [0, infinity) \ {invalidEntryId}.
				unsigned int id_;

				/// The transformation between the entry and the menu, must be valid.
				HomogenousMatrix4 menu_T_entry_;

				/// The width of the menu entry in domain of the background image (in pixel), with range [1, infinity).
				Scalar width_;

				/// The height of the menu entry in domain of the backgorund image (in pixel), with range [1, infinity).
				Scalar height_;
		};

		/**
		 * Definition of a vector holding meny entries.
		 */
		typedef std::vector<MenuEntry> MenuEntries;

	public:

		/// Definition of an invalid menu entry id.
		static constexpr unsigned int invalidEntryId = (unsigned int)(-1);

	public:

		/**
		 * Creates a new invalid menu object.
		 */
		VRMenu();

		/**
		 * Move constructor.
		 * @param menu The menu object to be moved
		 */
		VRMenu(VRMenu&& menu);

		/**
		 * Creates a new valid menu object.
		 * @param engine The rendering engine which will be used to render the menu, must be valid
		 * @param framebuffer The framebuffer associated with the rendering engine to which the menu will be rendered
		 * @param highlightColor The color which will be used for highlighting menu entries
		 * @param highlightBorder The size of the border around a highlighted menu entry, with range (0, infinity)
		 */
		VRMenu(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer, const RGBAColor& highlightColor = RGBAColor(0, 0, 0), const Scalar highlightBorder = Scalar(0.003));

		/**
		 * Disabled copy constructor.
		 */
		VRMenu(const VRMenu&) = delete;

		/**
		 * Destructs a menu.
		 */
		~VRMenu();

		/**
		 * Shows the menu at a specified location.
		 * @param base_T_menu The transformation between menu and world, or menu and device (if 'world_T_device' is defined), must be valid
		 * @param world_T_device The optional transformation between device and world, an invalid transformation to treat `base` as `world`
		 */
		void show(const HomogenousMatrix4& base_T_menu, const HomogenousMatrix4& world_T_device = HomogenousMatrix4(false));

		/**
		 * Hides the menu.
		 */
		void hide();

		/**
		 * Returns whether the menu is shown.
		 * @return True, if succeeded
		 */
		inline bool isShown() const;

		/**
		 * Events function called before the menu is rendered.
		 * @param trackedRemoteDevice The tracked remote device providing access to the controller tracking, must be valid
		 * @param renderTimestamp The timestamp when the menu will be rendered
		 * @param id The resulting event id in case an entry was selected
		 * @return True, if an entry was selected; False, if no entry was selected
		 */
		bool onPreRender(TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& renderTimestamp, unsigned int& id);

		/**
		 * Sets the background of the menu.
		 * @param backgroundFilename The filename of the background image, must be valid
		 * @param width The width of the menu in meter, with range (0, infinity)
		 * @return True, if succeeded
		 */
		bool setBackground(const std::string& backgroundFilename, const Scalar width);

		/**
		 * Adds a new menu entry to the menu.
		 * @param id The event id of the menu, with range [0, infinity) \ {invalidEntryId}.
		 * @param entryFilename The filename of the entry image, must be valid
		 * @param relativeLeft The left horizontal start position of the entry within the entire menu (the background image), in normalized coordinates, with range [0, 1)
		 * @param relativeTop The top vertical start position of the entry within the entire menu (the background image), in normalized coordinates, with range [0, 1)
		 * @param relativeWidth The relative width of the menu entry (relative in relation to the background image), with range (0, 1)
		 * @return True, if succeeded
		 */
		bool setEntry(const unsigned int id, const std::string& entryFilename, const Scalar relativeLeft, const Scalar relativeTop, const Scalar relativeWidth);

		/**
		 * Explicitly releases the menu and all associated resources.
		 */
		void release();

		/**
		 * Returns whether this menu is valid (whether it has been initialized with a engine and framebuffer).
		 * @return True, if so
		 */
		inline bool isValid() const;

		/**
		 * Move operator.
		 * @param menu The menu object to be moved
		 * @return Reference to this object
		 */
		VRMenu& operator=(VRMenu&& menu);

		/**
		 * Disabled copy operator.
		 * @param menu The menu object which would be copied
		 * @return The reference to this object
		 */
		VRMenu& operator=(const VRMenu& menu) = delete;

	protected:

		/// The rendering engine.
		Rendering::EngineRef engine_;

		/// The framebuffer in which the menu will be rendered.
		Rendering::FramebufferRef framebuffer_;

		/// The scene holding the menu.
		Rendering::SceneRef scene_;

		/// The transformation for the selection pointer.
		Rendering::TransformRef transformSelectionPointer_;

		/// The transformation for the selection entry (the entry highlighting).
		Rendering::TransformRef transformSelectionEntry_;

		/// The width of the background in pixel, with range (0, infinity)
		Scalar backgroundWidth_;

		/// The height of the background in pixel, with range (0, infinity)
		Scalar backgroundHeight_;

		/// The menu entries.
		MenuEntries menuEntries_;

		/// The size of the border around a highlighted menu entry, with range (0, infinity).
		Scalar highlightBorder_;

		/// The transformation between menu and world (the center of the menu).
		HomogenousMatrix4 world_T_menu_;

		/// The ids of the menu entries which is currently focused with the left and right controller, first left, second right.
		unsigned int focusedEntryId_[2];

		/// The offset in z-direction between background and menu entries in meter.
		static constexpr Scalar entryOffset_ = Scalar(0.003);
};

inline VRMenu::MenuEntry::MenuEntry(const unsigned int id, const HomogenousMatrix4& menu_T_entry, const Scalar width, const Scalar height) :
	id_(id),
	menu_T_entry_(menu_T_entry),
	width_(width),
	height_(height)
{
	// nothing to do here
}

inline unsigned int VRMenu::MenuEntry::id() const
{
	return id_;
}

inline const HomogenousMatrix4& VRMenu::MenuEntry::menu_T_entry() const
{
	return menu_T_entry_;
}

inline Scalar VRMenu::MenuEntry::width() const
{
	return width_;
}

inline Scalar VRMenu::MenuEntry::height() const
{
	return height_;
}

inline bool VRMenu::isShown() const
{
	return world_T_menu_.isValid();
}

inline bool VRMenu::isValid() const
{
	return engine_ && framebuffer_;
}

}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_VRAPI_APPLICATION_VR_MENU_H
