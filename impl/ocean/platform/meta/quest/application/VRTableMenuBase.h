/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_TABLE_MENU_BASE_H
#define META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_TABLE_MENU_BASE_H

#include "ocean/platform/meta/quest/application/Application.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/RGBAColor.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Text.h"
#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace Application
{

/**
 * The class implements the base class for a simple table-based menu for VR application.
 * The menu is composed of sections and menu entries.<br>
 * Based on controller tracking, the menu entries can be selected.<br>
 * Use the derived classes OpenXR::VRTableMenu or VrApi::VRTableMenu.
 * @ingroup platformmetaquestapplication
 */
class OCEAN_PLATFORM_META_QUEST_APPLICATION_EXPORT VRTableMenuBase
{
	public:

		/**
		 * This class holds the information of a user-defined menu entry.
		 */
		class Entry
		{
			friend class VRTableMenuBase;

			public:

				/**
				 * Creates a new entry.
				 * @param name The name of the entry (the text of the entry), must be valid
				 * @param url The url of the entry which is necessary to identify the individual entries
				 */
				inline Entry(std::string name, std::string url);

			protected:

				/// The name of the entry.
				std::string name_;

				/// The url of the entry.
				std::string url_;
		};

		/**
		 * Definition of a vector holding entries.
		 */
		typedef std::vector<Entry> Entries;

		/**
		 * Definition of a pair combining a group section name and menu entries.
		 */
		typedef std::pair<std::string, Entries> Group;

		/**
		 * Definition of a vector holding groups.
		 */
		typedef std::vector<Group> Groups;

	protected:

		/**
		 * This class contains the information necessary for one menu entry.
		 */
		class MenuEntry
		{
			public:

				/**
				 * Creates an new menu entry object.
				 * @param engine The rendering engine to be used
				 * @param textLineHeight The height of the text line, in meter, with range (0, infinity)
				 * @param name The text of the menu entry, must be valid
				 * @param url The optional URL connected with the menu entry
				 * @param isEntry True, if the entry is a menu entry; False, if the entry is a name of a group section
				 * @param backgroundColor The background color of an entry
				 */
				MenuEntry(Rendering::Engine& engine, const Scalar textLineHeight, const std::string& name, const std::string& url, bool isEntry, const RGBAColor& backgroundColor = RGBAColor(0.0f, 0.0f, 0.0f, 0.0f));

				/**
				 * Returns the extent of the actual text.
				 * @return The text's actual extent, in meter, with range [0, infinity)x[0, infinity)
				 */
				inline Vector2 extent() const;

				/**
				 * Returns the Transform node containing the Text node.
				 * @return The Transform node
				 */
				inline const Rendering::TransformRef& transform() const;

				/**
				 * Returns the name of this entry.
				 * @return The entry's name
				 */
				inline const std::string& name() const;

				/**
				 * Returns the optional URL of this entry.
				 * @return The entry's optional URL
				 */
				inline const std::string& url() const;

				/**
				 * Returns whether the entry is an actual menu entry of a group section.
				 * @return True, if the entry is a menu entry; False, if the entry is a name of a group section
				 */
				inline bool isEntry() const;

			protected:

				/// The Text node rendering the text.
				Rendering::TextRef text_;

				/// The Transform node containing the Text node.
				Rendering::TransformRef transform_;

				/// The name of the menu entry.
				std::string name_;

				/// The optional URL of the menu entry.
				std::string url_;

				/// True, if the entry is a menu entry; False, if the entry is a name of a group section
				bool isEntry_ = false;
		};

		/**
		 * Definition of a vector holding menu entries.
		 */
		typedef std::vector<MenuEntry> MenuEntries;

	public:

		/**
		 * Destructs a menu.
		 */
		virtual ~VRTableMenuBase();

		/**
		 * Sets all menu items.
		 * @param group The menu group containing all menu entries to set
		 * @param menuBackgroundColor The background color of a menu
		 * @param entryBackgroundColor The background color of individual entries
		 * @param entryHeight The height of each menu entry, in meter, with range (0, infinity)
		 * @param minWidth The minimal width of the menu, in meter, with range [0, infinity)
		 * @param minHeight The minimal height of the menu, in meter, with range [0, infinity)
		 * @return True, if succeeded
		 */
		inline bool setMenuEntries(const Group& group, const RGBAColor& menuBackgroundColor = RGBAColor(1.0f, 1.0f, 1.0f), const RGBAColor& entryBackgroundColor = RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), const Scalar entryHeight = Scalar(0.04), const Scalar minWidth = Scalar(1), const Scalar minHeight = Scalar(0.5));

        /**
		 * Sets all menu items.
		 * @param groups The menu groups containing all menu entries to set
		 * @param menuBackgroundColor The background color of menu
		 * @param entryBackgroundColor The background color of individual entries
		 * @param entryHeight The height of each menu entry, in meter, with range (0, infinity)
		 * @param minWidth The minimal width of the menu, in meter, with range [0, infinity)
		 * @param minHeight The minimal height of the menu, in meter, with range [0, infinity)
		 * @return True, if succeeded
		 */
		bool setMenuEntries(const Groups& groups, const RGBAColor& menuBackgroundColor = RGBAColor(1.0f, 1.0f, 1.0f), const RGBAColor& entryBackgroundColor = RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), const Scalar entryHeight = Scalar(0.04), const Scalar minWidth = Scalar(1), const Scalar minHeight = Scalar(0.5));

		/**
		 * Shows the menu at a specified location.
		 * @param base_T_menu The transformation between menu and world, or menu and device (if 'world_T_device' is defined), must be valid
		 * @param world_T_device The optional transformation between device and world, an invalid transformation to treat `base` as `world`
		 * @return True, if the menu was shown; False, if e.g., the menu is empty
		 */
		bool show(const HomogenousMatrix4& base_T_menu, const HomogenousMatrix4& world_T_device = HomogenousMatrix4(false));

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
		 * Returns the position of the menu
		 * @return The position of the menu in the coordinate system that was selected as base
		 * @sa show()
		 */
		inline HomogenousMatrix4 base_T_menu() const;

		/**
		 * Explicitly resets the entire menu without releasing any resources.
		 * @sa setMenuEntries()
		 */
		void reset();

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
		VRTableMenuBase& operator=(VRTableMenuBase&& menu) = default;

    protected:

		/**
		 * Creates a new invalid menu object.
		 */
		VRTableMenuBase() = default;

		/**
		 * Default move constructor.
		 */
		VRTableMenuBase(VRTableMenuBase&&) = default;

		/**
		 * Creates a new valid menu object.
		 * @param engine The rendering engine which will be used to render the menu, must be valid
		 * @param framebuffer The framebuffer associated with the rendering engine to which the menu will be rendered
		 */
		VRTableMenuBase(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer);

        /**
		 * Disabled copy constructor.
		 */
		VRTableMenuBase(const VRTableMenuBase&) = delete;

        /**
		 * Disabled copy operator.
		 * @return The reference to this object
		 */
		VRTableMenuBase& operator=(const VRTableMenuBase&) = delete;

	protected:

		/// The rendering engine.
		Rendering::EngineRef engine_;

		/// The framebuffer in which the menu will be rendered.
		Rendering::FramebufferRef framebuffer_;

		/// The scene holding the group and highlight elements.
		Rendering::SceneRef scene_;

		/// The group holding the menu.
		Rendering::GroupRef menuGroup_;

		/// The transformation for the selection pointer.
		Rendering::TransformRef transformSelectionPointer_;

		/// The transformation for the selection entry (the entry highlighting).
		Rendering::TransformRef transformSelectionEntry_;

		/// The width of the menu, in meter, with range (0, infinity)
		Scalar menuWidth_ = 0;

		/// The height of the menu, in meter, with range (0, infinity)
		Scalar menuHeight_ = 0;

		/// The height of each menu entry, in meter, with range (0, infinity)
		Scalar entryHeight_ = Scalar(0.04);

		/// The menu entries.
		MenuEntries menuEntries_;

		/// The transformation between the menu and a base coordinate system.
		HomogenousMatrix4 base_T_menu_ = HomogenousMatrix4(false);

		/// The transformation between menu and world (the center of the menu).
		HomogenousMatrix4 world_T_menu_ = HomogenousMatrix4(false);

		/// The names of the menu entries which is currently focused with the left and right controller, first left, second right.
		std::string focusedEntryNames_[2];

		/// The border around the menu entries.
		static constexpr Scalar menuBorder_ = Scalar(0.05);

		/// The offset in z-direction between background and menu entries in meter.
		static constexpr Scalar entryOffsetZ_ = Scalar(0.001);
};

inline VRTableMenuBase::Entry::Entry(std::string name, std::string url) :
	name_(std::move(name)),
	url_(std::move(url))
{
	// nothing to do here
}

Vector2 VRTableMenuBase::MenuEntry::extent() const
{
	ocean_assert(text_);

	return text_->size();
}

const Rendering::TransformRef& VRTableMenuBase::MenuEntry::transform() const
{
	return transform_;
}

inline const std::string& VRTableMenuBase::MenuEntry::name() const
{
	return name_;
}

inline const std::string& VRTableMenuBase::MenuEntry::url() const
{
	return url_;
}

inline bool VRTableMenuBase::MenuEntry::isEntry() const
{
	return isEntry_;
}

inline bool VRTableMenuBase::setMenuEntries(const Group& group, const RGBAColor& menuBackgroundColor, const RGBAColor& entryBackgroundColor, const Scalar entryHeight, const Scalar minWidth, const Scalar minHeight)
{
	const Groups groups(1, group);

	return setMenuEntries(groups, menuBackgroundColor, entryBackgroundColor, entryHeight, minWidth, minHeight);
}

inline bool VRTableMenuBase::isShown() const
{
	return world_T_menu_.isValid() && !scene_.isNull() && scene_->visible();
}

inline HomogenousMatrix4 VRTableMenuBase::base_T_menu() const
{
	return base_T_menu_;
}

inline bool VRTableMenuBase::isValid() const
{
	return engine_ && framebuffer_;
}

}

}

}

}

}

#endif // META_OCEAN_PLATFORM_META_QUEST_APPLICATION_VR_TABLE_MENU_BASE_H
