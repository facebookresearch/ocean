/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/meta/quest/application/VRTableMenuBase.h"

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

namespace Application
{

VRTableMenuBase::MenuEntry::MenuEntry(Rendering::Engine& engine, const Scalar textLineHeight, const std::string& name, const std::string& url, bool isEntry, const RGBAColor& backgroundColor) :
	name_(name),
	url_(url),
	isEntry_(isEntry)
{
	const RGBAColor foregroundColor = isEntry_ ? RGBAColor(0.0f, 0.0f, 0.0f) : RGBAColor(1.0f, 1.0f, 1.0f);

	constexpr bool shaded = true;
	constexpr Scalar fixedWidth = 0;
	constexpr Scalar fixedHeight = 0;

	constexpr Rendering::Text::AlignmentMode alignmentMode = Rendering::Text::AM_LEFT;
	constexpr Rendering::Text::HorizontalAnchor horizontalAnchor = Rendering::Text::HA_LEFT;
	constexpr Rendering::Text::VerticalAnchor verticalAnchor = Rendering::Text::VA_TOP;

	transform_ = Rendering::Utilities::createText(engine, name_, foregroundColor, backgroundColor, shaded, fixedWidth, fixedHeight, textLineHeight, alignmentMode, horizontalAnchor, verticalAnchor, std::string(), std::string(), &text_);
	ocean_assert(transform_ && text_);
}

VRTableMenuBase::VRTableMenuBase(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer) :
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

VRTableMenuBase::~VRTableMenuBase()
{
	release();
}

bool VRTableMenuBase::show(const HomogenousMatrix4& base_T_menu, const HomogenousMatrix4& world_T_device)
{
	ocean_assert(base_T_menu.isValid());

	if (menuEntries_.empty())
	{
		return false;
	}

	base_T_menu_ = base_T_menu;

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

	return true;
}

void VRTableMenuBase::hide()
{
	if (scene_)
	{
		scene_->setVisible(false);
	}
}

void VRTableMenuBase::reset()
{
	menuEntries_.clear();
	menuGroup_->clear();

	world_T_menu_.toNull();
}

void VRTableMenuBase::release()
{
	menuEntries_.clear();

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

bool VRTableMenuBase::setMenuEntries(const Groups& groups, const RGBAColor& menuBackgroundColor, const RGBAColor& entryBackgroundColor, const Scalar entryHeight, const Scalar minWidth, const Scalar minHeight)
{
	if (!engine_)
	{
		ocean_assert(false && "Menu is not initialized!");
		return false;
	}

	menuEntries_.clear();

	ocean_assert(entryHeight > Numeric::eps());
	ocean_assert(minWidth >= 0 && minHeight >= 0);

	entryHeight_ = entryHeight;

	if (groups.empty())
	{
		menuEntries_.emplace_back(*engine_, entryHeight_, "No entry", std::string(), false, entryBackgroundColor);
	}
	else
	{
		for (const Group& group : groups)
		{
			menuEntries_.emplace_back(*engine_, entryHeight_, group.first, std::string(), false);

			for (const Entry& entry : group.second)
			{
				menuEntries_.emplace_back(*engine_, entryHeight_, entry.name_, entry.url_, true, entryBackgroundColor);
			}
		}
	}

	// let's determine the width of the resulting menu

	menuWidth_ = minWidth;
	for (const MenuEntry& menuEntry : menuEntries_)
	{
		menuWidth_ = std::max(menuWidth_, menuEntry.extent().x());
	}

	menuHeight_ = (Scalar(menuEntries_.size()) + Scalar(std::max(0, int(groups.size()) - 1)) * Scalar(0.5)) * entryHeight_;
	menuHeight_ = std::max(menuHeight_, minHeight);

	menuGroup_->clear();

	Scalar yPosition = menuHeight_ * Scalar(0.5);
	for (size_t n = 0; n < menuEntries_.size(); ++n)
	{
		const MenuEntry& menuEntry = menuEntries_[n];

		if (n != 0 && !menuEntry.isEntry())
		{
			yPosition -= entryHeight_ * Scalar(0.5);
		}

		const Vector3 translation(-menuWidth_ * Scalar(0.5), yPosition, 0);

		menuEntry.transform()->setTransformation(HomogenousMatrix4(translation));
		menuGroup_->addChild(menuEntry.transform());

		if (!menuEntry.isEntry())
		{
			// we add a black box behind the group entry

			const Rendering::TransformRef highlightBox = Rendering::Utilities::createBox(engine_, Vector3(menuWidth_ + menuBorder_ * Scalar(0.5), entryHeight_, 0), RGBAColor(0.1f, 0.1f, 0.1f));
			highlightBox->setTransformation(HomogenousMatrix4(Vector3(0, yPosition - entryHeight_ * Scalar(0.5), -0.005)));

			menuGroup_->addChild(highlightBox);
		}

		yPosition -= entryHeight_;
	}

	const Rendering::TransformRef backgroundBox = Rendering::Utilities::createBox(engine_, Vector3(menuWidth_ + menuBorder_, menuHeight_ + menuBorder_, 0), menuBackgroundColor);
	backgroundBox->setTransformation(HomogenousMatrix4(Vector3(0, 0, -0.01)));

	menuGroup_->addChild(backgroundBox);

	return true;
}

}

}

}

}

}
