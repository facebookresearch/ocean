// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/oceantrackerdemos/RoomPlanTrackerExperience.h"

#include "ocean/devices/Manager.h"
#include "ocean/devices/VisualTracker.h"

#include "ocean/rendering/AbsoluteTransform.h"
#include "ocean/rendering/UndistortedBackground.h"
#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

RoomPlanTrackerExperience::~RoomPlanTrackerExperience()
{
	// nothing to do here
}

bool RoomPlanTrackerExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
	assert(engine);

	renderingWorldTransform_ = engine->factory().createTransform();

	renderingViewTransform_ = engine->factory().createTransform();

	{
		const Rendering::AbsoluteTransformRef absoluteTransform = engine->factory().createAbsoluteTransform();
		absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_VIEW);
		experienceScene()->addChild(absoluteTransform);

		renderingViewTransform_->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(-0.5), -1), Vector3(Scalar(0.04), Scalar(0.04), Scalar(0.04))));

		absoluteTransform->addChild(renderingViewTransform_);
	}

	{
		const Rendering::AbsoluteTransformRef absoluteTransform = engine->factory().createAbsoluteTransform();
		absoluteTransform->setTransformationType(Rendering::AbsoluteTransform::TT_HEAD_UP); // the head-up transformation allows to place content as "head-up display"
		absoluteTransform->setHeadUpRelativePosition(Vector2(Scalar(0.5), Scalar(0.08)));
		experienceScene()->addChild(absoluteTransform);

		const Rendering::TransformRef textTransform = Rendering::Utilities::createText(*engine, " Scanning started ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, Scalar(0.0004), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, "", "", &renderingText_);

		absoluteTransform->addChild(textTransform);
	}

	sceneTracker6DOF_ = Devices::Manager::get().device("ARKit 6DOF Room Plan Tracker");

	if (sceneTracker6DOF_.isNull())
	{
		Log::error() << "Failed to access Room Plan tracker";
		return false;
	}

	const Devices::VisualTrackerRef visualTracker(sceneTracker6DOF_);
	if (visualTracker && !engine->framebuffers().empty())
	{
		const Rendering::ViewRef view = engine->framebuffers().front()->view();

		if (view && view->background())
		{
			const Rendering::UndistortedBackgroundRef undistortedBackground(view->background());

			const Media::FrameMediumRef frameMedium(undistortedBackground->medium());

			if (frameMedium)
			{
				visualTracker->setInput(frameMedium);
			}
		}
	}

	sceneTrackerSampleEventSubscription_ = sceneTracker6DOF_->subscribeSampleEvent(Devices::Measurement::SampleCallback::create(*this, &RoomPlanTrackerExperience::onSceneTrackerSample));

	sceneTracker6DOF_->start();

	if (!anchoredContentManager_.initialize(std::bind(&RoomPlanTrackerExperience::onRemovedContent, this, std::placeholders::_1), experienceScene()))
	{
		return false;
	}

	const Devices::Tracker6DOF::ObjectId trackerObjectId = sceneTracker6DOF_->objectId("World");

	if (trackerObjectId != Devices::Tracker6DOF::invalidObjectId())
	{
		constexpr Scalar visibilityRadius = Scalar(10);
		constexpr Scalar engagementRadius = Scalar(1000);
		anchoredContentManager_.addContent(renderingWorldTransform_, sceneTracker6DOF_, trackerObjectId, visibilityRadius, engagementRadius);
	}

	return true;
}

bool RoomPlanTrackerExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	sceneTrackerSampleEventSubscription_.release();

	anchoredContentManager_.release();

	renderingViewTransform_.release();
	renderingWorldTransform_.release();
	sceneTracker6DOF_.release();

	return true;
}

Timestamp RoomPlanTrackerExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
	if (!anchoredContentManager_)
	{
		return timestamp;
	}

	TemporaryScopedLock scopedLock(lock_);
		Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap roomObjectMap;
		std::swap(roomObjectMap, roomObjectMap_);
	scopedLock.release();

	if (!roomObjectMap.empty())
	{
		renderingWorldTransform_->clear();

		Rendering::TransformRef viewTransform = engine->factory().createTransform();

		for (Devices::SceneTracker6DOF::SceneElementRoom::RoomObjectMap::const_iterator iRoomObject = roomObjectMap.cbegin(); iRoomObject != roomObjectMap.cend(); ++iRoomObject)
		{
			const Devices::SceneTracker6DOF::SceneElementRoom::SharedRoomObject& roomObject = iRoomObject->second;

			Rendering::TransformRef transform;

			if (roomObject->objectType() == Devices::SceneTracker6DOF::SceneElementRoom::RoomObject::OT_PLANAR)
			{
				const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject& planarRoomObject = (const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject&)(*roomObject);

				Vector3 dimension = roomObject->dimension();

				const Scalar thickness = planarThickness(planarRoomObject.planarType());
				dimension = Vector3(std::max(dimension.x(), thickness), std::max(dimension.y(), thickness), std::max(dimension.z(), thickness));

				transform = Rendering::Utilities::createBox(engine, dimension, planarColor(planarRoomObject.planarType()));
			}
			else
			{
				const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject& volumetricRoomObject = (const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject&)(*roomObject);

				const Vector3 dimension = adjustedVolumetricDimension(volumetricRoomObject.volumetricType(), roomObject->dimension());

				transform = Rendering::Utilities::createBox(engine, dimension, volumetricColor(volumetricRoomObject.volumetricType()));

				const std::string volumeticType = Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::translateVolumetricType(volumetricRoomObject.volumetricType());
				const float confidence = volumetricRoomObject.confidence();

				const Rendering::TransformRef text = Rendering::Utilities::createText(*engine, " " + volumeticType + ", " + String::toAString(confidence, 1u) + " ", RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false /*shaded*/, 0, 0, 0.075, Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE);
				text->setTransformation(HomogenousMatrix4(Vector3(0, dimension.y() * Scalar(0.5) + Scalar(0.05), 0)));

				transform->addChild(text);

				const Rendering::TransformRef backsideText = engine->factory().createTransform();
				backsideText->addChild(text);
				backsideText->setTransformation(HomogenousMatrix4(Quaternion(Vector3(0, 1, 0), Numeric::pi())));

				transform->addChild(backsideText);
			}

			transform->setTransformation(roomObject->world_T_object());

			renderingWorldTransform_->addChild(transform);
			viewTransform->addChild(transform);
		}

		renderingViewTransform_->clear();

		const BoundingBox boundingBox = viewTransform->boundingBox();

		viewTransform->setTransformation(HomogenousMatrix4(-boundingBox.center()));

		renderingViewTransform_->addChild(viewTransform);
	}

	renderingViewTransform_->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(-0.5), -1), Quaternion(Vector3(0, 1, 0), previewViewRotationAngle_.load()), Vector3(Scalar(0.04), Scalar(0.04), Scalar(0.04))));

	const Timestamp updatedTimestamp = anchoredContentManager_.preUpdate(engine, view, timestamp);

	return updatedTimestamp;
}

void RoomPlanTrackerExperience::onMousePress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp)
{
	previousScreenPosition_ = screenPosition;
}

void RoomPlanTrackerExperience::onMouseMove(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp)
{
	if (previousScreenPosition_.x() != Numeric::minValue())
	{
		ocean_assert(previousScreenPosition_.y() != Numeric::minValue());
		const Vector2 offset = screenPosition - previousScreenPosition_;

		const float angle = previewViewRotationAngle_.load();

		previewViewRotationAngle_ = angle + float(offset.x() * 0.005f);

		previousScreenPosition_ = screenPosition;
	}
}

void RoomPlanTrackerExperience::onMouseRelease(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp)
{
	previousScreenPosition_ = Vector2(Numeric::minValue(), Numeric::minValue());
}

std::unique_ptr<XRPlaygroundExperience> RoomPlanTrackerExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new RoomPlanTrackerExperience());
}

void RoomPlanTrackerExperience::onSceneTrackerSample(const Devices::Measurement* measurement, const Devices::Measurement::SampleRef& sample)
{
	const Devices::SceneTracker6DOF::SceneTracker6DOFSampleRef sceneSample(sample);
	ocean_assert(sceneSample);

	const Devices::SceneTracker6DOF::SharedSceneElements& sceneElements = sceneSample->sceneElements();

	if (sceneElements.empty())
	{
		return;
	}

	const Devices::Measurement::Metadata::const_iterator iInstruction = sceneSample->metadata().find("instruction");
	if (iInstruction != sceneSample->metadata().cend())
	{
		const std::string instruction = iInstruction->second.stringValue();

		if (!instruction.empty())
		{
			if (instruction == "normal")
			{
				renderingText_->setText(" Continue scanning ");
			}
			else
			{
				renderingText_->setText(" Guidance: \n " + instruction + " ");
			}
		}
	}

	ocean_assert(sceneElements.size() == 1);
	const Devices::SceneTracker6DOF::SharedSceneElement& sceneElement = sceneElements.front();

	if (sceneElement->sceneElementType() == Devices::SceneTracker6DOF::SceneElement::SET_ROOM)
	{
		const Devices::SceneTracker6DOF::SceneElementRoom& sceneElementRoom = dynamic_cast<const Devices::SceneTracker6DOF::SceneElementRoom&>(*sceneElement);

		const ScopedLock scopedLock(lock_);

		if (!sceneElementRoom.addedRoomObjects().empty() || !sceneElementRoom.removedRoomObjects().empty() || !sceneElementRoom.changedRoomObjects().empty() || !sceneElementRoom.updatedRoomObjects().empty())
		{
			roomObjectMap_ = sceneElementRoom.roomObjectMap();
		}
	}
}

void RoomPlanTrackerExperience::onRemovedContent(AnchoredContentManager::SharedContentObjectSet&& removedObjects)
{
	ocean_assert(false && "Objects should never be removed by the anchor manager, as we have a very large engagement radius");
}

RGBAColor RoomPlanTrackerExperience::planarColor(const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PlanarType planarType, const float alpha)
{
	switch (planarType)
	{
		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_UNKNOWN:
			break;

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_WALL:
			return RGBAColor(0.7f, 0.7f, 0.7f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_DOOR:
			return RGBAColor(0.9f, 0.25f, 0.25f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_WINDOW:
			return RGBAColor(0.25f, 0.25f, 0.9f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_OPENING:
			return RGBAColor(0.25f, 0.9f, 0.25f, 0.5f);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_FLOOR:
			return RGBAColor(0.25f, 0.25f, 0.25f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_END:
			ocean_assert(false && "Must not be used!");
			break;
	}

	ocean_assert(false && "Invalid type!");
	return RGBAColor(0.7f, 0.7f, 0.7f, alpha);
}

Scalar RoomPlanTrackerExperience::planarThickness(const Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PlanarType planarType)
{
	switch (planarType)
	{
		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_UNKNOWN:
			break;

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_WALL:
		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_FLOOR:
			return Scalar(0);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_DOOR:
			return Scalar(0.1);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_WINDOW:
			return Scalar(0.05);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_OPENING:
			return Scalar(0.025);

		case Devices::SceneTracker6DOF::SceneElementRoom::PlanarRoomObject::PT_END:
			ocean_assert(false && "Must not be used!");
			break;
	}

	ocean_assert(false && "Invalid type!");
	return Scalar(0);
}

RGBAColor RoomPlanTrackerExperience::volumetricColor(const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VolumetricType volumetricType, const float alpha)
{
	switch (volumetricType)
	{
		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_UNKNOWN:
			break;

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_STORAGE:
			return RGBAColor(0.7f, 0.25f, 0.25f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_REFRIGERATOR:
			return RGBAColor(0.7f, 0.7f, 0.7f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_STOVE:
			return RGBAColor(0.25f, 0.7f, 0.25f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_BED:
			return RGBAColor(0.25f, 0.25f, 0.7f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_SINK:
			return RGBAColor(0.9f, 0.9f, 0.9f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_WASHER_DRYER:
			return RGBAColor(0.0f, 0.0f, 1.0f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_TOILET:
			return RGBAColor(1.0f, 1.0f, 1.0f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_BATHTUB:
			return RGBAColor(1.0f, 1.0f, 1.0f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_OVEN:
			return RGBAColor(0.25f, 0.7f, 0.25f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_DISHWASHER:
			return RGBAColor(0.0f, 0.0f, 1.0f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_TABLE:
			return RGBAColor(0.5f, 0.5f, 0.5f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_SOFA:
			return RGBAColor(0.7f, 0.7f, 0.25f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_CHAIR:
			return RGBAColor(0.25f, 0.7f, 0.7f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_FIREPLACE:
			return RGBAColor(0.7f, 0.25f, 0.7f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_TELEVISION:
			return RGBAColor(0.2f, 0.2f, 0.2f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_STAIRS:
			return RGBAColor(0.6f, 0.6f, 0.6f, alpha);

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_END:
			ocean_assert(false && "Must not be used!");
			break;
	}

	ocean_assert(false && "Invalid type!");
	return RGBAColor(0.7f, 0.7f, 0.7f, alpha);
}

Vector3 RoomPlanTrackerExperience::adjustedVolumetricDimension(const Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VolumetricType volumetricType, const Vector3& dimension)
{
	switch (volumetricType)
	{
		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_UNKNOWN:
			ocean_assert(false && "Invalid type!");
			break;

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_SINK:
		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_STOVE:
			return Vector3(dimension.x(), dimension.y() + Scalar(0.01), dimension.z());

		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_REFRIGERATOR:
		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_OVEN:
		case Devices::SceneTracker6DOF::SceneElementRoom::VolumetricRoomObject::VT_DISHWASHER:
			return Vector3(dimension.x(), dimension.y(), dimension.z() + Scalar(0.01));

		default:
			break;
	}

	return dimension;
}

}

}
