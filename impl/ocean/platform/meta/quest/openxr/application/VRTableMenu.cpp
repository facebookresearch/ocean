/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/openxr/application/VRTableMenu.h"

#include "ocean/math/Line3.h"
#include "ocean/math/Plane3.h"

#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace Platform
{

namespace Meta
{

namespace Quest
{

namespace OpenXR
{

namespace Application
{

bool VRTableMenu::onPreRender(TrackedController& trackedController, const Timestamp& renderTimestamp, std::string& url)
{
	if (engine_.isNull())
	{
		return false;
	}

	if (!world_T_menu_.isValid())
	{
		scene_->setVisible(false);
		return false;
	}

	if (!scene_->visible())
	{
		return false;
	}

	scene_->setTransformation(world_T_menu_);

	transformSelectionPointer_->setVisible(false);
	transformSelectionEntry_->setVisible(false);

	const std::string previousFocusedEntryNames[2] = {focusedEntryNames_[0], focusedEntryNames_[1]};
	focusedEntryNames_[0].clear();
	focusedEntryNames_[1].clear();

	const HomogenousMatrix4 menu_T_world(world_T_menu_.inverted());

	const Plane3 menuWorldPlane(world_T_menu_.translation(), world_T_menu_.zAxis());

	for (const TrackedController::ControllerType& controllerType : trackedController.activeHandheldControllers())
	{
		const unsigned int controllerIndex = controllerType == TrackedController::CT_LEFT ? 0u : 1u;

		HomogenousMatrix4 world_T_controllerAim;
		if (trackedController.pose(controllerType, &world_T_controllerAim))
		{
			const Line3 ray(world_T_controllerAim.translation(), -world_T_controllerAim.zAxis());

			Vector3 worldIntersection;
			if (menuWorldPlane.intersection(ray, worldIntersection) && (worldIntersection - ray.point()) * ray.direction() > 0)
			{
				const Vector3 menuIntersection = menu_T_world * worldIntersection;

				constexpr Scalar extraBorder = menuBorder_ * Scalar(2);

				if (Numeric::abs(menuIntersection.x()) <= menuWidth_ * Scalar(0.5) + extraBorder && Numeric::abs(menuIntersection.y()) <= menuHeight_ * Scalar(0.5) + extraBorder)
				{
					transformSelectionPointer_->setTransformation(HomogenousMatrix4(menuIntersection + Vector3(0, 0, entryOffsetZ_ * Scalar(1.2))));
					transformSelectionPointer_->setVisible(true);

					for (const MenuEntry& menuEntry : menuEntries_)
					{
						if (!menuEntry.isEntry())
						{
							continue;
						}

						const Vector3 entryIntersection = menuEntry.transform()->transformation().inverted() * menuIntersection;

						if (entryIntersection.x() >= 0 && entryIntersection.x() <= menuWidth_ && entryIntersection.y() >= -entryHeight_ && entryIntersection.y() <= 0)
						{
							if (previousFocusedEntryNames[controllerIndex] != menuEntry.name())
							{
								// we just entered this menu entry
								trackedController.setVibration(controllerType, 0.0, 0.0f, 0.5f);
							}

							focusedEntryNames_[controllerIndex] = menuEntry.name();

							const Vector3 selectionScale(menuWidth_ + menuBorder_ * Scalar(0.5), entryHeight_, 1);

							transformSelectionEntry_->setTransformation(menuEntry.transform()->transformation() * HomogenousMatrix4(Vector3(menuWidth_ * Scalar(0.5), -entryHeight_ * Scalar(0.5), entryOffsetZ_ * Scalar(-0.5)), selectionScale));
							transformSelectionEntry_->setVisible(true);

							constexpr TrackedController::ButtonType anyTrigger = TrackedController::ButtonType(TrackedController::BT_LEFT_TRIGGER | TrackedController::BT_RIGHT_TRIGGER);

							if (trackedController.buttonsReleased(controllerType) & anyTrigger)
							{
								url = menuEntry.url();

								trackedController.setVibration(controllerType, 0.0, 0.0f, 1.0f);

								return true;
							}

							break;
						}
					}

					break;
				}
			}
		}
	}

	return false;
}

}

}

}

}

}

}
