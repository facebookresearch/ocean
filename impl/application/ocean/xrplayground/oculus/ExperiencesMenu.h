// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_OCULUS_EXPERIENCES_MENU_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_OCULUS_EXPERIENCES_MENU_H

#include "application/ocean/xrplayground/oculus/XRPlaygroundOculus.h"

#include "ocean/math/HomogenousMatrix4.h"

#include "ocean/platform/meta/quest/vrapi/TrackedRemoteDevice.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Framebuffer.h"
#include "ocean/rendering/Scene.h"
#include "ocean/rendering/Text.h"
#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace XRPlayground
{

namespace Oculus
{

/**
 * This class implements the VR menu for experiences.
 * @ingroup xrplaygroundoculus
 */
class ExperiencesMenu
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
				 * @param engine The rendering engine to be used
				 * @param textLineHeight The height of the text line, in meter, with range (0, infinity)
				 * @param name The text of the menu entry, must be valid
				 * @param url The optional URL connected with the menu entry
				 * @param isExperience True, if the entry contains an experience; False, if the entry contains a group name
				 */
				MenuEntry(Rendering::Engine& engine, const Scalar textLineHeight, const std::string& name, const std::string& url, const bool isExperience);

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
				 * Returns whether the entry is an experience or not.
				 * @return True, if the entry contains an experience; False, if the entry contains a group name
				 */
				inline bool isExperience() const;

			protected:

				/// The Text node rendering the text.
				Rendering::TextRef text_;

				/// The Transform node containing the Text node.
				Rendering::TransformRef transform_;

				/// The name of the menu entry.
				std::string name_;

				/// The optional URL of the menu entry.
				std::string url_;

				/// True, if the entry contains an experience; False, if the entry contains a group name
				bool isExperience_ = false;
		};

		/**
		 * Definition of a vector holding meny entries.
		 */
		typedef std::vector<MenuEntry> MenuEntries;

	public:

		/**
		 * Creates a new invalid menu object.
		 */
		ExperiencesMenu() = default;

		/**
		 * Move constructor.
		 * @param menu The menu object to be moved
		 */
		ExperiencesMenu(ExperiencesMenu&& menu) = default;

		/**
		 * Creates a new valid menu object.
		 * @param engine The rendering engine which will be used to render the menu, must be valid
		 * @param framebuffer The framebuffer associated with the rendering engine to which the menu will be rendered
		 */
		ExperiencesMenu(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer);

		/**
		 * Destructs a menu.
		 */
		~ExperiencesMenu();

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
		 * @param url The resulting menu entry url in case an entry was selected
		 * @return True, if an entry was selected; False, if no entry was selected
		 */
		bool onPreRender(Platform::Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& renderTimestamp, std::string& url);

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
		ExperiencesMenu& operator=(ExperiencesMenu&& menu) = default;

    protected:

		/**
		 * Creates all menu items.
		 */
		void createMenuEntries();

        /**
		 * Disabled copy constructor.
		 */
		ExperiencesMenu(const ExperiencesMenu&) = delete;

        /**
		 * Disabled copy operator.
		 * @param menu The menu object which would be copied
		 * @return The reference to this object
		 */
		ExperiencesMenu& operator=(const ExperiencesMenu& menu) = delete;

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

		/// The experience group which will be displayed in the menu, empty to display all experiences.
		std::string group_ = "Main Experiences";

		/// The menu entries.
		MenuEntries menuEntries_;

		/// The transformation between menu and world (the center of the menu).
		HomogenousMatrix4 world_T_menu_;

		/// The names of the menu entries which is currently focused with the left and right controller, first left, second right.
		std::string focusedEntryNames_[2];

		/// The border around the menu entries.
		static constexpr Scalar menuBorder_ = Scalar(0.05);

		/// The offset in z-direction between background and menu entries in meter.
		static constexpr Scalar entryOffsetZ_ = Scalar(0.003);

		/// The menu url prefix to show other experience groups.
		std::string urlShowOtherPrefix_ = "SHOW_OTHER_";

		/// The timestamp until the menu will be hidden, invalid to ignore this timestamp.
		Timestamp hideTimestamp_ = Timestamp(false);
};

Vector2 ExperiencesMenu::MenuEntry::extent() const
{
	ocean_assert(text_);

	return text_->size();
}

const Rendering::TransformRef& ExperiencesMenu::MenuEntry::transform() const
{
	return transform_;
}

inline const std::string& ExperiencesMenu::MenuEntry::name() const
{
	return name_;
}

inline const std::string& ExperiencesMenu::MenuEntry::url() const
{
	return url_;
}

inline bool ExperiencesMenu::MenuEntry::isExperience() const
{
	return isExperience_;
}

inline bool ExperiencesMenu::isShown() const
{
	return world_T_menu_.isValid();
}

inline bool ExperiencesMenu::isValid() const
{
	return engine_ && framebuffer_;
}

}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_OCULUS_EXPERIENCES_MENU_H
