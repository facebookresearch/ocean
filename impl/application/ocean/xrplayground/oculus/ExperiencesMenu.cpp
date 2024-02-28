// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/oculus/ExperiencesMenu.h"

#include "application/ocean/xrplayground/common/experiences/ExperiencesManager.h"

#include "ocean/math/Line3.h"
#include "ocean/math/Plane3.h"

#include "ocean/rendering/Utilities.h"

namespace Ocean
{

using namespace Platform::Meta::Quest::VrApi;

namespace XRPlayground
{

namespace Oculus
{

ExperiencesMenu::MenuEntry::MenuEntry(Rendering::Engine& engine, const Scalar textLineHeight, const std::string& name, const std::string& url, const bool isExperience) :
	name_(name),
	url_(url),
	isExperience_(isExperience)
{
	const RGBAColor foregroundColor = isExperience ? RGBAColor(0.0f, 0.0f, 0.0f) : RGBAColor(1.0f, 1.0f, 1.0f);
	const RGBAColor backgroundColor = RGBAColor(0.0f, 0.0f, 0.0f, 0.0f);

	constexpr bool shaded = true;
	constexpr Scalar fixedWidth = 0;
	constexpr Scalar fixedHeight = 0;

	constexpr Rendering::Text::AlignmentMode alignmentMode = Rendering::Text::AM_LEFT;
	constexpr Rendering::Text::HorizontalAnchor horizontalAnchor = Rendering::Text::HA_LEFT;
	constexpr Rendering::Text::VerticalAnchor verticalAnchor = Rendering::Text::VA_TOP;

	transform_ = Rendering::Utilities::createText(engine, name_, foregroundColor, backgroundColor, shaded, fixedWidth, fixedHeight, textLineHeight, alignmentMode, horizontalAnchor, verticalAnchor, std::string(), std::string(), &text_);
	ocean_assert(transform_ && text_);
}

ExperiencesMenu::ExperiencesMenu(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer) :
	engine_(engine),
	framebuffer_(framebuffer)
{
	ocean_assert(engine_ && framebuffer_);

	scene_ = engine_->factory().createScene();
	scene_->setVisible(false);

	const RGBAColor pointerHighlightColor(0.0f, 0.0f, 1.0f);
	const RGBAColor entryHighlightColor(0.5f, 0.5f, 1.0f);

	transformSelectionPointer_ = Rendering::Utilities::createBox(engine_, Vector3(0.0075, 0.0075, 0), pointerHighlightColor);
	transformSelectionPointer_->setVisible(false);
	scene_->addChild(transformSelectionPointer_);

	transformSelectionEntry_ = Rendering::Utilities::createBox(engine_, Vector3(1, 1, 0), entryHighlightColor);
	transformSelectionEntry_->setVisible(false);
	scene_->addChild(transformSelectionEntry_);

	menuGroup_ = engine_->factory().createGroup();
	scene_->addChild(menuGroup_);

	framebuffer_->addScene(scene_);
}

ExperiencesMenu::~ExperiencesMenu()
{
	release();
}

void ExperiencesMenu::show(const HomogenousMatrix4& base_T_menu, const HomogenousMatrix4& world_T_device)
{
	ocean_assert(base_T_menu.isValid());

	createMenuEntries();

	if (world_T_device.isValid())
	{
		const Vector3 yAxis(0, 1, 0);

		Vector3 zAxis = world_T_device.xAxis().cross(yAxis);
		Vector3 xAxis = yAxis.cross(zAxis);

		if (zAxis.normalize() && xAxis.normalize())
		{
			world_T_menu_ = HomogenousMatrix4(xAxis, yAxis, zAxis, world_T_device.translation()) * base_T_menu;

		}
		else
		{
			world_T_menu_ = world_T_device * base_T_menu;
		}
	}
	else
	{
		world_T_menu_ = base_T_menu;
	}

	if (scene_)
	{
		scene_->setTransformation(world_T_menu_);
		scene_->setVisible(true);
	}
}

void ExperiencesMenu::hide()
{
	if (scene_)
	{
		scene_->setVisible(false);

		menuEntries_.clear();
		menuGroup_->clear();
	}

	world_T_menu_.toNull();
}

bool ExperiencesMenu::onPreRender(Platform::Meta::Quest::VrApi::TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& renderTimestamp, std::string& url)
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

	if (hideTimestamp_.isValid())
	{
		if (renderTimestamp < hideTimestamp_)
		{
			scene_->setVisible(false);
			return false;
		}

		hideTimestamp_.toInvalid();
	}

	scene_->setTransformation(world_T_menu_);
	scene_->setVisible(true);

	transformSelectionPointer_->setVisible(false);
	transformSelectionEntry_->setVisible(false);

	const std::string previousFocusedEntryNames[2] = {focusedEntryNames_[0], focusedEntryNames_[1]};
	focusedEntryNames_[0].clear();
	focusedEntryNames_[1].clear();

	const HomogenousMatrix4 menu_T_world(world_T_menu_.inverted());

	const Plane3 menuWorldPlane(world_T_menu_.translation(), world_T_menu_.zAxis());

	for (const TrackedRemoteDevice::RemoteType& remoteType : trackedRemoteDevice.activeHandheldDevices())
	{
		const unsigned int deviceIndex = remoteType == TrackedRemoteDevice::RT_LEFT ? 0u : 1u;

		HomogenousMatrix4 world_T_remoteDevice;
		if (trackedRemoteDevice.pose(remoteType, &world_T_remoteDevice))
		{
			const Line3 ray(world_T_remoteDevice.translation(), -world_T_remoteDevice.zAxis());

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
						if (!menuEntry.isExperience())
						{
							continue;
						}

						const Vector3 entryIntersection = menuEntry.transform()->transformation().inverted() * menuIntersection;

						if (entryIntersection.x() >= 0 && entryIntersection.x() <= menuWidth_ && entryIntersection.y() >= -entryHeight_ && entryIntersection.y() <= 0)
						{
							if (previousFocusedEntryNames[deviceIndex] != menuEntry.name())
							{
								// we just entered this menu entry
								trackedRemoteDevice.setVibrationLevel(remoteType, 0.5f);
							}

							focusedEntryNames_[deviceIndex] = menuEntry.name();

							const Vector3 selectionScale(menuWidth_ + menuBorder_ * Scalar(0.5), entryHeight_, 1);

							transformSelectionEntry_->setTransformation(menuEntry.transform()->transformation() * HomogenousMatrix4(Vector3(menuWidth_ * Scalar(0.5), -entryHeight_ * Scalar(0.5), entryOffsetZ_ * Scalar(-0.5)), selectionScale));
							transformSelectionEntry_->setVisible(true);

							if (trackedRemoteDevice.buttonsReleased(remoteType) & ovrButton_Trigger)
							{
								trackedRemoteDevice.setVibrationLevel(remoteType, 1.0f);

								if (menuEntry.url().find(urlShowOtherPrefix_) == 0)
								{
									group_ = menuEntry.url().substr(urlShowOtherPrefix_.length());

									createMenuEntries();

									hideTimestamp_ = renderTimestamp + 0.2;

									return false;
								}

								url = menuEntry.url();

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

void ExperiencesMenu::release()
{
	transformSelectionEntry_.release();
	transformSelectionPointer_.release();

	if (scene_)
	{
		framebuffer_->removeScene(scene_);
	}

	menuGroup_.release();
	scene_.release();
	framebuffer_.release();
	engine_.release();
}

void ExperiencesMenu::createMenuEntries()
{
	menuEntries_.clear();

	const ExperiencesManager::SelectableExperienceGroups selectableExperienceGroups(ExperiencesManager::selectableExperienceGroups(ExperiencesManager::PT_HMD_QUEST_INDOOR));

	if (selectableExperienceGroups.empty())
	{
		menuEntries_.emplace_back(*engine_, entryHeight_, "No Experiences available", std::string(), false);
	}
	else
	{
		for (size_t groupIndex = 0; groupIndex < selectableExperienceGroups.size(); ++groupIndex)
		{
			const ExperiencesManager::SelectableExperienceGroup& selectableExperienceGroup = selectableExperienceGroups[groupIndex];

			if (group_.empty() || selectableExperienceGroup.first == group_)
			{
				menuEntries_.emplace_back(*engine_, entryHeight_, selectableExperienceGroup.first, std::string(), false);

				for (const ExperiencesManager::SelectableExperience& selectableExperience : selectableExperienceGroup.second)
				{
					menuEntries_.emplace_back(*engine_, entryHeight_, selectableExperience.name(), selectableExperience.url(), true);
				}
			}
		}

		if (!group_.empty())
		{
			menuEntries_.emplace_back(*engine_, entryHeight_, "Show Other", std::string(), false);

			for (size_t groupIndex = 0; groupIndex < selectableExperienceGroups.size(); ++groupIndex)
			{
				const ExperiencesManager::SelectableExperienceGroup& selectableExperienceGroup = selectableExperienceGroups[groupIndex];

				if (selectableExperienceGroup.first != group_)
				{
					menuEntries_.emplace_back(*engine_, entryHeight_, selectableExperienceGroup.first + " ...", urlShowOtherPrefix_ + selectableExperienceGroup.first, true);
				}
			}
		}
	}

	// let's determine the width of the resulting menu

	menuWidth_ = Scalar(1); // at least 1 meter
	for (const MenuEntry& menuEntry : menuEntries_)
	{
		menuWidth_ = std::max(menuWidth_, menuEntry.extent().x());
	}

	menuHeight_ = (Scalar(menuEntries_.size()) + Scalar(std::max(0, int(selectableExperienceGroups.size()) - 1)) * Scalar(0.5)) * entryHeight_;
	menuHeight_ = std::max(menuHeight_, Scalar(0.5)); // at least 0.5 meter

	menuGroup_->clear();

	Scalar yPosition = menuHeight_ * Scalar(0.5);
	for (size_t n = 0; n < menuEntries_.size(); ++n)
	{
		const MenuEntry& menuEntry = menuEntries_[n];

		if (n != 0 && !menuEntry.isExperience())
		{
			yPosition -= entryHeight_ * Scalar(0.5);
		}

		const Vector3 translation(-menuWidth_ * Scalar(0.5), yPosition, 0);

		menuEntry.transform()->setTransformation(HomogenousMatrix4(translation));
		menuGroup_->addChild(menuEntry.transform());

		if (!menuEntry.isExperience())
		{
			// we add a black box behind the group entry

			const Rendering::TransformRef highlightBox = Rendering::Utilities::createBox(engine_, Vector3(menuWidth_ + menuBorder_ * Scalar(0.5), entryHeight_, 0), RGBAColor(0.1f, 0.1f, 0.1f));
			highlightBox->setTransformation(HomogenousMatrix4(Vector3(0, yPosition - entryHeight_ * Scalar(0.5), -0.005)));

			menuGroup_->addChild(highlightBox);
		}

		yPosition -= entryHeight_;
	}

	const Rendering::TransformRef backgroundBox = Rendering::Utilities::createBox(engine_, Vector3(menuWidth_ + menuBorder_, menuHeight_ + menuBorder_, 0), RGBAColor(1.0f, 1.0f, 1.0f));
	backgroundBox->setTransformation(HomogenousMatrix4(Vector3(0, 0, -0.01)));

	menuGroup_->addChild(backgroundBox);
}

}

}

}
