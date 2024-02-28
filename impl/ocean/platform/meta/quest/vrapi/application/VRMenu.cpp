// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/platform/meta/quest/vrapi/application/VRMenu.h"

#include "ocean/math/Line3.h"
#include "ocean/math/Plane3.h"

#include "ocean/media/FrameMedium.h"
#include "ocean/media/Manager.h"

#include "ocean/rendering/Utilities.h"

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

VRMenu::VRMenu() :
	backgroundWidth_(0),
	backgroundHeight_(0),
	highlightBorder_(0),
	world_T_menu_(false)
{
	focusedEntryId_[0] = invalidEntryId;
	focusedEntryId_[1] = invalidEntryId;
}

VRMenu::VRMenu(VRMenu&& menu) :
	VRMenu()
{
	*this = std::move(menu);
}

VRMenu::VRMenu(const Rendering::EngineRef& engine, const Rendering::FramebufferRef& framebuffer, const RGBAColor& highlightColor, const Scalar highlightBorder) :
	VRMenu()
{
	engine_ = engine;
	framebuffer_ = framebuffer;
	highlightBorder_ = highlightBorder;

	ocean_assert(engine_ && framebuffer_);
	ocean_assert(highlightBorder_ > Numeric::eps());

	scene_ = engine_->factory().createScene();
	scene_->setVisible(false);

	transformSelectionPointer_ = Rendering::Utilities::createBox(engine_, Vector3(0.004, 0.004, 0), highlightColor);
	transformSelectionPointer_->setVisible(false);
	scene_->addChild(transformSelectionPointer_);

	transformSelectionEntry_ = Rendering::Utilities::createBox(engine_, Vector3(1, 1, 0), highlightColor);
	transformSelectionEntry_->setVisible(false);
	scene_->addChild(transformSelectionEntry_);

	framebuffer_->addScene(scene_);
}

VRMenu::~VRMenu()
{
	release();
}

void VRMenu::show(const HomogenousMatrix4& base_T_menu, const HomogenousMatrix4& world_T_device)
{
	ocean_assert(base_T_menu.isValid());

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

void VRMenu::hide()
{
	if (scene_)
	{
		scene_->setVisible(false);
	}

	world_T_menu_.toNull();
}

bool VRMenu::onPreRender(TrackedRemoteDevice& trackedRemoteDevice, const Timestamp& renderTimestamp, unsigned int& id)
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

	scene_->setTransformation(world_T_menu_);
	scene_->setVisible(true);

	transformSelectionPointer_->setVisible(false);
	transformSelectionEntry_->setVisible(false);

	const unsigned int previousFocusedEntryId[2] = {focusedEntryId_[0], focusedEntryId_[1]};
	focusedEntryId_[0] = invalidEntryId;
	focusedEntryId_[1] = invalidEntryId;

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

				constexpr Scalar extraBorder = Scalar(0.02);

				if (Numeric::abs(menuIntersection.x()) <= backgroundWidth_ * Scalar(0.5) + extraBorder && Numeric::abs(menuIntersection.y()) <= backgroundHeight_ * Scalar(0.5) + extraBorder)
				{
					transformSelectionPointer_->setTransformation(HomogenousMatrix4(menuIntersection + Vector3(0, 0, entryOffset_ * Scalar(1.2))));
					transformSelectionPointer_->setVisible(true);

					for (const MenuEntry& menuEntry : menuEntries_)
					{
						const Vector3 entryIntersection = menuEntry.menu_T_entry().inverted() * menuIntersection;

						if (Numeric::abs(entryIntersection.x()) <= menuEntry.width() * Scalar(0.5) && Numeric::abs(entryIntersection.y()) <= menuEntry.height() * Scalar(0.5))
						{
							if (previousFocusedEntryId[deviceIndex] != menuEntry.id())
							{
								// we just entered this menu entry
								trackedRemoteDevice.setVibrationLevel(remoteType, 0.5f);
							}

							focusedEntryId_[deviceIndex] = menuEntry.id();

							const Vector3 selectionScale(menuEntry.width() + highlightBorder_ * Scalar(2), menuEntry.height() + highlightBorder_ * Scalar(2), 1);

							transformSelectionEntry_->setTransformation(menuEntry.menu_T_entry() * HomogenousMatrix4(Vector3(0, 0, entryOffset_ * Scalar(-0.5)), selectionScale));
							transformSelectionEntry_->setVisible(true);

							if (trackedRemoteDevice.buttonsReleased(remoteType) & ovrButton_Trigger)
							{
								id = menuEntry.id();

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

bool VRMenu::setBackground(const std::string& backgroundFilename, const Scalar width)
{
	ocean_assert(engine_ && framebuffer_);
	ocean_assert(backgroundWidth_ == 0 && backgroundHeight_ == 0);

	ocean_assert(!backgroundFilename.empty());
	ocean_assert(width > Numeric::eps());

	const Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(backgroundFilename, Media::Medium::IMAGE, true /*useExclusive*/);
	ocean_assert(frameMedium);

	if (frameMedium.isNull())
	{
		Log::error() << "Failed to load menu image '" << backgroundFilename << "'";
		return false;
	}

	frameMedium->start();

	const FrameRef frame = frameMedium->frame();

	if (frame.isNull())
	{
		return false;
	}

	const Scalar height = width * Scalar(frame->height()) / Scalar(frame->width());

	const Rendering::TransformRef backgroundTransform = Rendering::Utilities::createBox(engine_, Vector3(width, height, 0), frameMedium);

	scene_->addChild(backgroundTransform);

	backgroundWidth_ = width;
	backgroundHeight_ = height;

	return true;
}

bool VRMenu::setEntry(const unsigned int id, const std::string& entryFilename, const Scalar relativeLeft, const Scalar relativeTop, const Scalar relativeWidth)
{
	ocean_assert(engine_ && framebuffer_);
	ocean_assert(backgroundWidth_ > Numeric::eps() && backgroundHeight_ > Numeric::eps());

	ocean_assert(!entryFilename.empty());
	ocean_assert(relativeLeft >= 0 && relativeLeft + relativeWidth <= 1);
	ocean_assert(relativeTop >= 0 && relativeTop < 1);

	ocean_assert(id != invalidEntryId);

	const Media::FrameMediumRef frameMedium = Media::Manager::get().newMedium(entryFilename, Media::Medium::IMAGE, true /*useExclusive*/);
	ocean_assert(frameMedium);

	if (frameMedium.isNull())
	{
		Log::error() << "Failed to load menu image '" << entryFilename << "'";
		return false;
	}

	frameMedium->start();

	const FrameRef frame = frameMedium->frame();

	if (frame.isNull())
	{
		return false;
	}

	const Scalar left = relativeLeft * backgroundWidth_;
	const Scalar top = relativeTop * backgroundHeight_;

	const Scalar width = relativeWidth * backgroundWidth_;
	const Scalar height = width * Scalar(frame->height()) / Scalar(frame->width());

	const HomogenousMatrix4 menu_T_entry(Vector3(left + width * Scalar(0.5) - backgroundWidth_ * Scalar(0.5), backgroundHeight_ * Scalar(0.5) - (top + height * Scalar(0.5)), entryOffset_));

	const Rendering::TransformRef entryTransform = Rendering::Utilities::createBox(engine_, Vector3(width, height, 0), frameMedium);
	entryTransform->setTransformation(menu_T_entry);

	menuEntries_.emplace_back(id, menu_T_entry, width, height);

	scene_->addChild(entryTransform);

	return true;
}

void VRMenu::release()
{
	transformSelectionEntry_.release();
	transformSelectionPointer_.release();

	if (scene_)
	{
		framebuffer_->removeScene(scene_);
	}

	scene_.release();
	framebuffer_.release();
	engine_.release();
}

VRMenu& VRMenu::operator=(VRMenu&& menu)
{
	if (this != &menu)
	{
		engine_ = std::move(menu.engine_);
		framebuffer_ = std::move(menu.framebuffer_);
		scene_ = std::move(menu.scene_);
		transformSelectionPointer_ = std::move(menu.transformSelectionPointer_);
		transformSelectionEntry_ = std::move(menu.transformSelectionEntry_);

		backgroundWidth_ = menu.backgroundWidth_;
		backgroundHeight_ = menu.backgroundHeight_;
		menu.backgroundWidth_ = 0;
		menu.backgroundHeight_ = 0;

		menuEntries_ = std::move(menu.menuEntries_);

		highlightBorder_ = menu.highlightBorder_;
		menu.highlightBorder_ = 0;

		world_T_menu_ = menu.world_T_menu_;
		menu.world_T_menu_.toNull();

		focusedEntryId_[0] = menu.focusedEntryId_[0];
		focusedEntryId_[1] = menu.focusedEntryId_[1];
		menu.focusedEntryId_[0] = invalidEntryId;
		menu.focusedEntryId_[1] = invalidEntryId;
	}

	return *this;
}

}

}

}

}

}

}
