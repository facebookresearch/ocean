// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/interactivehome/InteractiveHomeExperience.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/platformsdk/Application.h"

	#include "application/ocean/xrplayground/common/PlatformSpecific.h"
#endif

#include "ocean/rendering/Utilities.h"

namespace Ocean
{

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	using namespace Platform::Meta::Quest;
	using namespace Platform::Meta::Quest::VrApi;
	using namespace Platform::Meta::Quest::VrApi::Application;
#endif

namespace XRPlayground
{

InteractiveHomeExperience::~InteractiveHomeExperience()
{
	// nothing to do here
}

bool InteractiveHomeExperience::load(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& /*properties*/)
{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	const HomogenousMatrix4 world_T_floor = PlatformSpecific::get().world_T_floor();

	Rendering::TransformRef table = createTable(engine, RGBAColor(0.5f, 0.5f, 0.5f), Scalar(2), Scalar(0.75), Scalar(1));
	table->setTransformation(world_T_floor * HomogenousMatrix4(Vector3(0, 0, -1)));
	actions_.emplace_back(Action::AT_NONE, 0ull, "", table);

	Rendering::TransformRef rotaryFiles = createRotaryFiles(engine, RGBAColor(0.5f, 0.5f, 0.5f), RGBAColor(1.0f, 1.0f, 1.0f), Scalar(0.2), "I Expect You To Die");
	rotaryFiles->setTransformation(HomogenousMatrix4(Vector3(Scalar(0.45), Scalar(0.775), Scalar(-0.1)), Quaternion(Vector3(0, 1, 0), Numeric::deg2rad(-25))));
	actions_.emplace_back(Action::AT_PROXIMITY_OR_RAY, 1987283631365460ull, "Do you want to play the game?", rotaryFiles);
	table->addChild(std::move(rotaryFiles));

	Rendering::TransformRef screen = createScreen(engine, RGBAColor(0.2f, 0.2f, 0.2f), RGBAColor(1.0f, 0.0f, 0.0f), Scalar(0.75), "Netflix");
	screen->setTransformation(HomogenousMatrix4(Vector3(Scalar(-0.4), Scalar(0.775), Scalar(-0.15)), Quaternion(Vector3(0, 1, 0), Numeric::deg2rad(20))));
	actions_.emplace_back(Action::AT_PROXIMITY_OR_RAY, 2184912004923042ull, "Do you want to start Netflix?", screen);
	table->addChild(std::move(screen));

	Rendering::TransformRef tablet = createTablet(engine, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f), Scalar(0.25), "Play\nSupernatural");
	tablet->setTransformation(HomogenousMatrix4(Vector3(Scalar(-0.1), Scalar(0.775), Scalar(0.25)), Quaternion(Vector3(0, 1, 0), Numeric::deg2rad(-5))));
	actions_.emplace_back(Action::AT_PROXIMITY_OR_RAY, 1830168170427369ull, "Want to play Supernatural? ", tablet);
	table->addChild(std::move(tablet));

	Rendering::TransformRef racket = createRacket(engine, RGBAColor(0.2f, 0.2f, 0.2f), RGBAColor(0.4f, 0.0f, 0.0f), Scalar(0.1), "ELEVEN\nTable Tennis");
	racket->setTransformation(HomogenousMatrix4(Vector3(Scalar(-0.65), Scalar(0.775), Scalar(0.25)), Quaternion(Vector3(0, 1, 0), Numeric::deg2rad(30))));
	actions_.emplace_back(Action::AT_PROXIMITY_OR_RAY, 1995434190525828ull, "Do you want to place tennis?", racket);
	table->addChild(std::move(racket));

	experienceScene()->addChild(std::move(table));

	Rendering::TransformRef door = createDoor(engine, RGBAColor(0.58f, 0.29f, 0.0f), RGBAColor(0.5f, 0.5f, 0.5f), Scalar(1.2), Scalar(2.2), "Portal to Horizon");
	door->setTransformation(world_T_floor * HomogenousMatrix4(Vector3(2, 0, 0), Quaternion(Vector3(0, 1, 0), -Numeric::pi_2())));
	actions_.emplace_back(Action::AT_PROXIMITY_BODY, 3562519657091839ull, "Ready for Horizon?", door, Box3(Vector3(0, 0, Scalar(-1.25)), 2, 10, 2));
	experienceScene()->addChild(std::move(door));

	Rendering::TransformRef whiteBoard = createWhiteBoard(engine, RGBAColor(0.7f, 0.7f, 0.7f), RGBAColor(1.0f, 1.0f, 1.0f), Scalar(1.2), Scalar(1.8), "Work in Workplace");
	whiteBoard->setTransformation(world_T_floor * HomogenousMatrix4(Vector3(-2, 0, 0), Quaternion(Vector3(0, 1, 0), Numeric::pi_2())));
	actions_.emplace_back(Action::AT_PROXIMITY_BODY, 2514011888645651ull, "Do you want to start Workplace?", whiteBoard, Box3(Vector3(0, 0, Scalar(0.5)), 1, 10, 1));
	experienceScene()->addChild(std::move(whiteBoard));


	renderingTransformMovingSelectionBox_ = Rendering::Utilities::createBox(engine, Vector3(1, 0, 1), RGBAColor(1.0f, 0.0f, 0.0f));
	renderingTransformMovingSelectionBox_->setVisible(false);
	experienceScene()->addChild(renderingTransformMovingSelectionBox_);

	renderingTransformMovingSelectionPoint_ = Rendering::Utilities::createSphere(engine, Scalar(0.015), RGBAColor(1.0f, 1.0f, 1.0f));
	renderingTransformMovingSelectionPoint_->setVisible(false);
	experienceScene()->addChild(renderingTransformMovingSelectionPoint_);

	renderingTransformActivableSelectionBox_ = createTransparentBox(engine, RGBAColor(0.2f, 0.2f, 1.0f, 0.5f));
	renderingTransformActivableSelectionBox_->setVisible(false);
	experienceScene()->addChild(renderingTransformActivableSelectionBox_);

	vrTableMenu_ = VRTableMenu(engine, engine->framebuffers().front());

	PlatformSpecific::get().vrControllerVisualizer().setControllerRayLength(Scalar(0.2));
#endif

	return true;
}

bool InteractiveHomeExperience::unload(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Timestamp timestamp)
{
	renderingTransformMovingSelectionBox_.release();
	renderingTransformMovingSelectionPoint_.release();

	renderingTransformFloor_.release();

	actions_.clear();

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	vrTableMenu_.release();
#endif

	return true;
}

Timestamp InteractiveHomeExperience::preUpdate(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp)
{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	const HomogenousMatrix4 world_T_device = view->transformation();

	renderingTransformMovingSelectionBox_->setVisible(false);
	renderingTransformMovingSelectionPoint_->setVisible(false);

	if (!vrTableMenu_.isShown())
	{
		renderingTransformActivableSelectionBox_->setVisible(false);
	}

	const TrackedRemoteDevice& trackedRemoteDevice = PlatformSpecific::get().trackedRemoteDevice();

	if (movingAction_ != nullptr)
	{
		if (trackedRemoteDevice.buttonsReleased(movingRemoteType_) & ovrButton_Trigger)
		{
			movingPlaneIntersection_ = Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());
			movingRemoteType_ = TrackedRemoteDevice::RT_UNDEFINED;

			movingAction_->world_T_actionInteractionStarted_.toNull();
			movingAction_->parent_T_actionInteractionStarted_.toNull();
			movingAction_->rotationAngle_ = 0;
			movingAction_ = nullptr;
		}
		else
		{
			moveAction(world_T_device, timestamp);
		}
	}
	else if (activeAction_ == nullptr)
	{
		determineMovableAction(timestamp);

		if (movingAction_ == nullptr && !renderingTransformMovingSelectionBox_->visible())
		{
			determineActivableAction(world_T_device, timestamp);
		}
	}
	else
	{
		std::string entryUrl;
		if (vrTableMenu_.onPreRender(PlatformSpecific::get().trackedRemoteDevice(), timestamp, entryUrl))
		{
			vrTableMenu_.hide();

			if (activeAction_->actionType_ & Action::AT_PROXIMITY_BODY)
			{
				// disabling the action for at least 2 seconds
				activeAction_->disabledUntilTimestamp_ = timestamp + 2.0;
			}

			if (entryUrl == "YES")
			{
				launchApp(activeAction_->appId_);
			}

			activeAction_ = nullptr;
		}
	}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	return timestamp;
}

void InteractiveHomeExperience::onKeyPress(const Interaction::UserInterface& /*userInterface*/, const Rendering::EngineRef& /*engine*/, const std::string& key, const Timestamp /*timestamp*/)
{
#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	if (key == "A" || key == "X")
	{
		Rendering::NodeRef table;

		if (!actions_.empty())
		{
			table = actions_.front().renderingTransform_->child(0u);
		}

		if (PlatformSpecific::get().mrPassthroughVisualizer().isPassthroughRunning())
		{
			PlatformSpecific::get().mrPassthroughVisualizer().pausePassthrough();

			if (table)
			{
				table->setVisible(true);
			}
		}
		else
		{
			PlatformSpecific::get().mrPassthroughVisualizer().resumePassthrough();

			if (table)
			{
				table->setVisible(false);
			}
		}
	}
#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
}

std::unique_ptr<XRPlaygroundExperience> InteractiveHomeExperience::createExperience()
{
	return std::unique_ptr<XRPlaygroundExperience>(new InteractiveHomeExperience());
}

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

void InteractiveHomeExperience::determineMovableAction(const Timestamp& timestamp)
{
	ocean_assert(activeAction_ == nullptr);

	const TrackedRemoteDevice& trackedRemoteDevice = PlatformSpecific::get().trackedRemoteDevice();

	Action* candidateAction = nullptr;
	Scalar candidateActionSqrDistance = Numeric::maxValue();
	Vector3 candidateWorldPlaneIntersection;

	for (const TrackedRemoteDevice::RemoteType& remoteType : trackedRemoteDevice.activeHandheldDevices())
	{
		if (remoteType == TrackedRemoteDevice::RT_LEFT) // we use the left controller to move actions
		{
			HomogenousMatrix4 world_T_controller(false);
			if (!trackedRemoteDevice.pose(remoteType, &world_T_controller, nullptr, timestamp))
			{
				return;
			}

			const Line3 laserRay = Line3(world_T_controller.translation(), -world_T_controller.zAxis());

			for (Action& action : actions_)
			{
				const Plane3 worldActionPlane = action.plane();

				Vector3 worldPlaneIntersection;
				if (worldActionPlane.intersection(laserRay, worldPlaneIntersection) && ((worldPlaneIntersection - laserRay.point()) * laserRay.direction() > Scalar(0))) // intersection in front of user
				{
					const Vector3 actionPlaneIntersection = action.world_T_action().inverted() * worldPlaneIntersection;

					const BoundingBox actionBoundingBox = action.renderingBoundingBox();

					if (actionBoundingBox.isInside(actionPlaneIntersection, Scalar(0.2)))
					{
						const Scalar actionSqrDistance = laserRay.point().sqrDistance(worldPlaneIntersection);

						if (actionSqrDistance < candidateActionSqrDistance)
						{
							candidateAction = &action;
							candidateActionSqrDistance = actionSqrDistance;
							candidateWorldPlaneIntersection = worldPlaneIntersection;
						}
					}
				}
			}

			if (candidateAction != nullptr)
			{
				const BoundingBox actionBoundingBox = candidateAction->renderingBoundingBox();

				const Vector3 boxPosition(actionBoundingBox.center().x(), 0, actionBoundingBox.center().z());
				const Vector3 boxScale(actionBoundingBox.xDimension() + selectionExtension_, 1, actionBoundingBox.zDimension() + selectionExtension_);

				renderingTransformMovingSelectionBox_->setTransformation(candidateAction->world_T_action() * HomogenousMatrix4(boxPosition, boxScale));
				renderingTransformMovingSelectionBox_->setVisible(true);

				renderingTransformMovingSelectionPoint_->setTransformation(HomogenousMatrix4(candidateWorldPlaneIntersection));
				renderingTransformMovingSelectionPoint_->setVisible(true);

				if (trackedRemoteDevice.buttonsPressed(remoteType) & ovrButton_Trigger)
				{
					movingPlaneIntersection_ = candidateAction->world_T_action().inverted() * candidateWorldPlaneIntersection;
					movingRemoteType_ = remoteType;

					ocean_assert(movingAction_ == nullptr);
					movingAction_ = candidateAction;

					movingAction_->world_T_actionInteractionStarted_ = movingAction_->world_T_action();
					movingAction_->parent_T_actionInteractionStarted_ = movingAction_->parent_T_action();
				}
			}

			break;
		}
	}
}

void InteractiveHomeExperience::moveAction(const HomogenousMatrix4& world_T_device, const Timestamp& timestamp)
{
	ocean_assert(movingAction_ != nullptr);
	ocean_assert(movingRemoteType_ != TrackedRemoteDevice::RT_UNDEFINED);

	const TrackedRemoteDevice& trackedRemoteDevice = PlatformSpecific::get().trackedRemoteDevice();

	HomogenousMatrix4 world_T_controller(false);
	if (trackedRemoteDevice.pose(movingRemoteType_, &world_T_controller, nullptr, timestamp))
	{
		const Line3 laserRay = Line3(world_T_controller.translation(), -world_T_controller.zAxis());

		const Plane3 worldActionPlane = movingAction_->plane();

		Vector3 worldPlaneIntersection;
		if (worldActionPlane.intersection(laserRay, worldPlaneIntersection) && ((worldPlaneIntersection - laserRay.point()) * laserRay.direction() > Scalar(0))) // intersection in front of user
		{
			constexpr Scalar maxDistance = 10;

			if (worldPlaneIntersection.sqrDistance(world_T_device.translation()) > Numeric::sqr(maxDistance))
			{
				worldPlaneIntersection = worldActionPlane.projectOnPlane(laserRay.point(maxDistance));
			}

			movingAction_->rotationAngle_ += trackedRemoteDevice.joystickTilt(movingRemoteType_).x() * Scalar(0.05);

			const Vector3 actionPlaneIntersection = movingAction_->world_T_action().inverted() * worldPlaneIntersection;

			const Vector3 started_t_current = actionPlaneIntersection - movingPlaneIntersection_;

			ocean_assert(movingAction_->parent_T_actionInteractionStarted_.isValid());
			const HomogenousMatrix4 parent_T_movedAction(movingAction_->parent_T_actionInteractionStarted_ * HomogenousMatrix4(started_t_current, Quaternion(Vector3(0, 1, 0), movingAction_->rotationAngle_)));
			const HomogenousMatrix4 world_T_movedAction(movingAction_->world_T_actionInteractionStarted_ * HomogenousMatrix4(started_t_current, Quaternion(Vector3(0, 1, 0), movingAction_->rotationAngle_)));

			movingAction_->renderingTransform_->setTransformation(parent_T_movedAction);

			const BoundingBox actionBoundingBox = movingAction_->renderingBoundingBox();

			const Vector3 boxPosition(actionBoundingBox.center().x(), 0, actionBoundingBox.center().z());
			const Vector3 boxScale(actionBoundingBox.xDimension() + selectionExtension_, 1, actionBoundingBox.zDimension() + selectionExtension_);

			renderingTransformMovingSelectionBox_->setTransformation(world_T_movedAction * HomogenousMatrix4(boxPosition, boxScale));
			renderingTransformMovingSelectionBox_->setVisible(true);

			renderingTransformMovingSelectionPoint_->setTransformation(HomogenousMatrix4(worldPlaneIntersection));
			renderingTransformMovingSelectionPoint_->setVisible(true);
		}
	}
}

void InteractiveHomeExperience::determineActivableAction(const HomogenousMatrix4& world_T_device, const Timestamp& timestamp)
{
	ocean_assert(activeAction_ == nullptr);

	const TrackedRemoteDevice& trackedRemoteDevice = PlatformSpecific::get().trackedRemoteDevice();

	for (Action& action : actions_)
	{
		if (timestamp < action.disabledUntilTimestamp_)
		{
			continue;
		}

		const HomogenousMatrix4 world_T_action = action.renderingTransform_->worldTransformation();
		const HomogenousMatrix4 action_T_world = world_T_action.inverted();

		bool activateAction = false;

		const Box3 box = action.box_.isValid() ? action.box_ : action.renderingBoundingBox();

		Action::ActionType highlightActionType = Action::AT_NONE;

		if (action.actionType_ & Action::AT_PROXIMITY_BODY)
		{
			const HomogenousMatrix4 action_T_device = action_T_world * world_T_device;

			const Box3 extendedBox(box.center(), box.xDimension(), 10, box.zDimension());

			if (extendedBox.isInside(action_T_device.translation()))
			{
				activateAction = true;

				highlightActionType = Action::AT_PROXIMITY_BODY;
			}
		}

		if (!activateAction && action.actionType_ & Action::AT_PROXIMITY_CONTROLLERS)
		{
			for (const TrackedRemoteDevice::RemoteType remoteType : {TrackedRemoteDevice::RT_LEFT, TrackedRemoteDevice::RT_RIGHT})
			{
				HomogenousMatrix4 world_T_controller(false);
				if (trackedRemoteDevice.pose(remoteType, &world_T_controller, nullptr, timestamp))
				{
					const HomogenousMatrix4 action_T_controller = action_T_world * world_T_controller;

					if (box.expanded(Vector3(selectionExtension_, selectionExtension_, selectionExtension_)).isInside(action_T_controller.translation()))
					{
						activateAction = true;
						highlightActionType = Action::AT_PROXIMITY_CONTROLLERS;

						break;
					}
				}
			}

			if (!activateAction)
			{
				for (const bool left : {true, false})
				{
					HomogenousMatrix4 world_T_hand(false);
					if (PlatformSpecific::get().handPoses().getHandRoot(left, world_T_hand))
					{
						const HomogenousMatrix4 action_T_hand = action_T_world * world_T_hand;

						if (box.expanded(Vector3(selectionExtension_, selectionExtension_, selectionExtension_)).isInside(action_T_hand.translation()))
						{
							activateAction = true;
							break;
						}
					}
				}
			}
		}

		if (!activateAction && action.actionType_ & Action::AT_RAY_CONTROLLERS)
		{
			constexpr TrackedRemoteDevice::RemoteType remoteType = TrackedRemoteDevice::RT_RIGHT; // we use the right controller to activate actions

			HomogenousMatrix4 world_T_controller(false);
			if (trackedRemoteDevice.pose(remoteType, &world_T_controller, nullptr, timestamp))
			{
				const Line3 laserRay = Line3(world_T_controller.translation(), -world_T_controller.zAxis());

				if (box.expanded(Vector3(selectionExtension_, selectionExtension_, selectionExtension_)).hasIntersection(laserRay, action_T_world))
				{
					highlightActionType = Action::AT_RAY_CONTROLLERS;

					if (trackedRemoteDevice.buttonsPressed(remoteType) & ovrButton_Trigger)
					{
						activateAction = true;
					}
				}
			}
		}

		if (highlightActionType != Action::AT_NONE && (highlightActionType & Action::AT_PROXIMITY_BODY) != Action::AT_PROXIMITY_BODY)
		{
			const Box3 highlightBox = action.renderingBoundingBox();

			HomogenousMatrix4 transformation(action.world_T_action());
			transformation *= HomogenousMatrix4(highlightBox.center());
			transformation.applyScale(highlightBox.dimension() + Vector3(Scalar(0.05), Scalar(0.05), Scalar(0.05)));

			renderingTransformActivableSelectionBox_->setTransformation(transformation);
			renderingTransformActivableSelectionBox_->setVisible(true);
		}

		if (activateAction)
		{
			if (action.disabledUntilTimestamp_.isInvalid())
			{
				VRTableMenu::Entries menuEntries =
				{
					VRTableMenu::Entry("Yes", "YES"),
					VRTableMenu::Entry("No", "NO")
				};

				const VRTableMenu::Group menuGroup(action.description_, std::move(menuEntries));

				vrTableMenu_.setMenuEntries(menuGroup, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), Scalar(0.02), Scalar(0), Scalar(0));
				vrTableMenu_.show(HomogenousMatrix4(Vector3(0, 0, Scalar(-0.65))), world_T_device);

				activeAction_ = &action;
				break;
			}
		}
		else if (timestamp >= action.disabledUntilTimestamp_)
		{
			// enabling the action again
			action.disabledUntilTimestamp_.toInvalid();
		}
	}
}

bool InteractiveHomeExperience::launchApp(const uint64_t appId)
{
	return PlatformSDK::Application::get().launchAnotherApplication(appId).valid();
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

Rendering::TransformRef InteractiveHomeExperience::createDoor(const Rendering::EngineRef& engine, const RGBAColor& doorColor, const RGBAColor& frameColor, const Scalar width, const Scalar height, const std::string& text)
{
	ocean_assert(engine);
	ocean_assert(doorColor.isValid() && frameColor.isValid());
	ocean_assert(width > 0 && height > 0);

	constexpr Scalar frameSize = Scalar(0.1);

	const Scalar width_2 = width * Scalar(0.5);
	const Scalar height_2 = height * Scalar(0.5);
	const Scalar frameSize_2 = frameSize * Scalar(0.5);

	Rendering::TransformRef transform = engine->factory().createTransform();

	Rendering::TransformRef transformLeftFrame = Rendering::Utilities::createBox(engine, Vector3(frameSize, height, frameSize), frameColor);
	transformLeftFrame->setTransformation(HomogenousMatrix4(Vector3(-width_2, height_2, 0)));
	transform->addChild(std::move(transformLeftFrame));

	Rendering::TransformRef transformRightFrame = Rendering::Utilities::createBox(engine, Vector3(frameSize, height, frameSize), frameColor);
	transformRightFrame->setTransformation(HomogenousMatrix4(Vector3(width_2, height_2, 0)));
	transform->addChild(std::move(transformRightFrame));

	Rendering::TransformRef transformTopFrame = Rendering::Utilities::createBox(engine, Vector3(width + frameSize, frameSize, frameSize), frameColor);
	transformTopFrame->setTransformation(HomogenousMatrix4(Vector3(0, height, 0)));
	transform->addChild(std::move(transformTopFrame));

	Rendering::TransformRef transformCore = engine->factory().createTransform();
	transformCore->setTransformation(HomogenousMatrix4(Vector3(width_2, 0, 0)) * HomogenousMatrix4(Quaternion(Vector3(0, 1, 0), Numeric::deg2rad(-60))) * HomogenousMatrix4(Vector3(-width_2, 0, 0)));
	transform->addChild(transformCore);

	Rendering::TransformRef transformDoor = Rendering::Utilities::createBox(engine, Vector3(width, height, frameSize_2), doorColor);
	transformDoor->setTransformation(HomogenousMatrix4(Vector3(0, height_2, 0)));
	transformCore->addChild(std::move(transformDoor));

	Rendering::TransformRef transformKnob = Rendering::Utilities::createSphere(engine, frameSize_2, doorColor);
	transformKnob->setTransformation(HomogenousMatrix4(Vector3(-width * Scalar(0.35), height_2, frameSize_2)));
	transformCore->addChild(std::move(transformKnob));

	Rendering::TransformRef transformText =  Rendering::Utilities::createText(*engine, text, RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false, 0, 0, frameSize - Scalar(0.005), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string());
	transformText->setTransformation(HomogenousMatrix4(Vector3(0, height, frameSize_2 + Scalar(0.005))));
	transform->addChild(std::move(transformText));

	return transform;
}

Rendering::TransformRef InteractiveHomeExperience::createTable(const Rendering::EngineRef& engine, const RGBAColor color, const Scalar width, const Scalar height, const Scalar depth)
{
	ocean_assert(engine);
	ocean_assert(color.isValid());
	ocean_assert(width > 0 && height > 0 && depth > 0);

	constexpr Scalar radius = Scalar(0.04);

	Rendering::TransformRef transform = engine->factory().createTransform();

	const Scalar height_2 = height * Scalar(0.5);
	const Scalar xLegOffset = width * Scalar(0.5) - radius * Scalar(2);
	const Scalar zLegOffset = depth * Scalar(0.5) - radius * Scalar(2);

	Rendering::TransformRef transformLeg0 = Rendering::Utilities::createCylinder(engine, radius, height, color);
	transformLeg0->setTransformation(HomogenousMatrix4(Vector3(-xLegOffset, height_2, -zLegOffset)));
	transform->addChild(std::move(transformLeg0));

	Rendering::TransformRef transformLeg1 = Rendering::Utilities::createCylinder(engine, radius, height, color);
	transformLeg1->setTransformation(HomogenousMatrix4(Vector3(xLegOffset, height_2, -zLegOffset)));
	transform->addChild(std::move(transformLeg1));

	Rendering::TransformRef transformLeg2 = Rendering::Utilities::createCylinder(engine, radius, height, color);
	transformLeg2->setTransformation(HomogenousMatrix4(Vector3(xLegOffset, height_2, zLegOffset)));
	transform->addChild(std::move(transformLeg2));

	Rendering::TransformRef transformLeg3 = Rendering::Utilities::createCylinder(engine, radius, height, color);
	transformLeg3->setTransformation(HomogenousMatrix4(Vector3(-xLegOffset, height_2, zLegOffset)));
	transform->addChild(std::move(transformLeg3));

	Rendering::TransformRef transformTop = Rendering::Utilities::createBox(engine, Vector3(width, radius, depth), color);
	transformTop->setTransformation(HomogenousMatrix4(Vector3(0, height, 0)));
	transform->addChild(std::move(transformTop));

	Rendering::TransformRef table = engine->factory().createTransform();
	table->addChild(transform);

	return table;
}

Rendering::TransformRef InteractiveHomeExperience::createRotaryFiles(const Rendering::EngineRef& engine, const RGBAColor& frameColor, const RGBAColor& fileColor, const Scalar size, const std::string& text)
{
	ocean_assert(engine);
	ocean_assert(frameColor.isValid() && fileColor.isValid());

	Rendering::TransformRef transform = engine->factory().createTransform();

	const Scalar size_5 = size * Scalar(0.5);
	const Scalar size_6 = size * Scalar(0.6);

	Rendering::TransformRef boxLeft = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.005), size_6, size_6), frameColor);
	boxLeft->setTransformation(HomogenousMatrix4(Vector3(-size_5, size_6 * Scalar(0.5), -size_6 * Scalar(0.3))));
	transform->addChild(std::move(boxLeft));

	Rendering::TransformRef boxRight = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.005), size_6, size_6), frameColor);
	boxRight->setTransformation(HomogenousMatrix4(Vector3(size_5, size_6 * Scalar(0.5), -size_6 * Scalar(0.3))));
	transform->addChild(std::move(boxRight));

	Rendering::TransformRef axis = Rendering::Utilities::createCylinder(engine, Scalar(0.01), size * Scalar(1.05), frameColor.damped(0.2f));
	axis->setTransformation(HomogenousMatrix4(Vector3(0, size_5, 0), Quaternion(Vector3(0, 0, 1), Numeric::pi_2())));
	transform->addChild(std::move(axis));

	constexpr unsigned int numberFiles = 12u;

	for (unsigned int n = 1u; n < numberFiles; ++n)
	{
		Rendering::TransformRef file = Rendering::Utilities::createBox(engine, Vector3(size * Scalar(0.9), size_5, Scalar(0.001)), fileColor);
		file->setTransformation(HomogenousMatrix4(Vector3(0, size_5, 0)) * HomogenousMatrix4(Quaternion(Vector3(1, 0, 0), -Numeric::pi() * Scalar(n) / Scalar(numberFiles - 1u))) * HomogenousMatrix4(Vector3(0, size_5 * Scalar(0.5), 0)));

		if (n == 1u)
		{
			Rendering::TransformRef transformText =  Rendering::Utilities::createText(*engine, text, RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false, 0, 0, size * Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string());
			transformText->setTransformation(HomogenousMatrix4(Vector3(0, size_5 * Scalar(0.25), Scalar(0.001))));
			file->addChild(std::move(transformText));
		}

		transform->addChild(std::move(file));
	}

	return transform;
}

Rendering::TransformRef InteractiveHomeExperience::createScreen(const Rendering::EngineRef& engine, const RGBAColor& frameColor, const RGBAColor& screenColor, const Scalar size, const std::string& text)
{
	ocean_assert(engine);
	ocean_assert(frameColor.isValid() && screenColor.isValid());

	Rendering::TransformRef transform = engine->factory().createTransform();

	const Scalar size_5 = size * Scalar(0.5);

	Rendering::TransformRef boxBottom = Rendering::Utilities::createBox(engine, Vector3(size * Scalar(0.4), Scalar(0.02), size * Scalar(0.3)), frameColor);
	boxBottom->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.01), 0)));
	transform->addChild(std::move(boxBottom));

	Rendering::TransformRef boxBack = Rendering::Utilities::createBox(engine, Vector3(Scalar(0.05), size_5, Scalar(0.05)), frameColor);
	boxBack->setTransformation(HomogenousMatrix4(Vector3(0, size_5 * Scalar(0.5), Scalar(-0.05))));
	transform->addChild(std::move(boxBack));

	Rendering::TransformRef boxScreenInner = Rendering::Utilities::createBox(engine, Vector3(size - Scalar(0.02), size * Scalar(0.6) - Scalar(0.02), Scalar(0.02)), screenColor);
	boxScreenInner->setTransformation(HomogenousMatrix4(Vector3(0, 0, Scalar(0.001))));

	Rendering::TransformRef transformText =  Rendering::Utilities::createText(*engine, text, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false, 0, 0, size * Scalar(0.2), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string());
	transformText->setTransformation(HomogenousMatrix4(Vector3(0, size_5 * Scalar(0.25), Scalar(0.0101))));
	boxScreenInner->addChild(std::move(transformText));

	Rendering::TransformRef boxScreen = Rendering::Utilities::createBox(engine, Vector3(size, size * Scalar(0.6), Scalar(0.02)), frameColor);
	boxScreen->addChild(std::move(boxScreenInner));
	boxScreen->setTransformation(HomogenousMatrix4(Vector3(0, size_5, Scalar(-0.02))));
	transform->addChild(std::move(boxScreen));

	return transform;
}

Rendering::TransformRef InteractiveHomeExperience::createTablet(const Rendering::EngineRef& engine, const RGBAColor& frameColor, const RGBAColor& screenColor, const Scalar size, const std::string& text)
{
	ocean_assert(engine);
	ocean_assert(frameColor.isValid() && screenColor.isValid());

	Rendering::TransformRef transform = engine->factory().createTransform();

	Rendering::TransformRef boxTabletInner = Rendering::Utilities::createBox(engine, Vector3(size * Scalar(0.75) - Scalar(0.005), Scalar(0.01), size - Scalar(0.005)), screenColor);
	boxTabletInner->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.001), 0)));

	Rendering::TransformRef transformText =  Rendering::Utilities::createText(*engine, text, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false, 0, 0, size * Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_TOP, std::string(), std::string());
	transformText->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.006), size * Scalar(-0.3)), Quaternion(Vector3(1, 0, 0), -Numeric::pi_2())));
	boxTabletInner->addChild(std::move(transformText));

	Rendering::TransformRef boxTablet = Rendering::Utilities::createBox(engine, Vector3(size * Scalar(0.75), Scalar(0.01), size), frameColor);
	boxTablet->addChild(std::move(boxTabletInner));
	boxTablet->setTransformation(HomogenousMatrix4(Vector3(0, 0, Scalar(-0.02))));
	transform->addChild(std::move(boxTablet));

	return transform;
}

Rendering::TransformRef InteractiveHomeExperience::createRacket(const Rendering::EngineRef& engine, const RGBAColor& handleColor, const RGBAColor& plateColor, const Scalar size, const std::string& text)
{
	ocean_assert(engine);
	ocean_assert(handleColor.isValid() && plateColor.isValid());

	Rendering::TransformRef transform = engine->factory().createTransform();

	Rendering::TransformRef cylinerPlate = Rendering::Utilities::createCylinder(engine, size, Scalar(0.01), plateColor);
	transform->addChild(std::move(cylinerPlate));

	Rendering::TransformRef cylinerHandle = Rendering::Utilities::createCylinder(engine, Scalar(0.015), size, handleColor);
	cylinerHandle->setTransformation(HomogenousMatrix4(Vector3(0, 0, size * Scalar(1.5)), Quaternion(Vector3(1, 0, 0), -Numeric::pi_2()), Vector3(1, 1, Scalar(0.65))));
	transform->addChild(std::move(cylinerHandle));

	Rendering::TransformRef transformText = Rendering::Utilities::createText(*engine, text, RGBAColor(1.0f, 1.0f, 1.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false, 0, 0, size * Scalar(0.3), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string());
	transformText->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.006), size * Scalar(-0.3)), Quaternion(Vector3(1, 0, 0), -Numeric::pi_2())));
	transform->addChild(std::move(transformText));

	return transform;
}

Rendering::TransformRef InteractiveHomeExperience::createWhiteBoard(const Rendering::EngineRef& engine, const RGBAColor& frameColor, const RGBAColor& boardColor, const Scalar width, const Scalar height, const std::string& text)
{
	ocean_assert(engine);
	ocean_assert(frameColor.isValid() && boardColor.isValid());
	ocean_assert(width > 0 && height > 0);

	constexpr Scalar frameSize = Scalar(0.05);
	constexpr Scalar depth = Scalar(0.4);

	const Scalar width_2 = width * Scalar(0.5);
	const Scalar height_2 = height * Scalar(0.5);
	const Scalar frameSize_2 = frameSize * Scalar(0.5);

	Rendering::TransformRef transform = engine->factory().createTransform();

	Rendering::TransformRef transformLeftFrame = Rendering::Utilities::createBox(engine, Vector3(frameSize, height, frameSize), frameColor);
	transformLeftFrame->setTransformation(HomogenousMatrix4(Vector3(-width_2, height_2, 0)));
	transform->addChild(std::move(transformLeftFrame));

	Rendering::TransformRef transformRightFrame = Rendering::Utilities::createBox(engine, Vector3(frameSize, height, frameSize), frameColor);
	transformRightFrame->setTransformation(HomogenousMatrix4(Vector3(width_2, height_2, 0)));
	transform->addChild(std::move(transformRightFrame));

	Rendering::TransformRef transformLeftFrameBottom = Rendering::Utilities::createBox(engine, Vector3(frameSize, frameSize, depth), frameColor);
	transformLeftFrameBottom->setTransformation(HomogenousMatrix4(Vector3(-width_2, frameSize_2, 0)));
	transform->addChild(std::move(transformLeftFrameBottom));

	Rendering::TransformRef transformRightFrameBottom = Rendering::Utilities::createBox(engine, Vector3(frameSize, frameSize, depth), frameColor);
	transformRightFrameBottom->setTransformation(HomogenousMatrix4(Vector3(width_2, frameSize_2, 0)));
	transform->addChild(std::move(transformRightFrameBottom));

	Rendering::TransformRef transformBoard = Rendering::Utilities::createBox(engine, Vector3(width, height * Scalar(0.5), Scalar(0.01)), boardColor);
	transformBoard->setTransformation(HomogenousMatrix4(Vector3(0, height * Scalar(0.70), 0)));
	transform->addChild(std::move(transformBoard));

	Rendering::TransformRef transformText =  Rendering::Utilities::createText(*engine, text, RGBAColor(0.0f, 0.0f, 0.0f), RGBAColor(0.0f, 0.0f, 0.0f, 0.0f), false, 0, 0, Scalar(0.1), Rendering::Text::AM_CENTER, Rendering::Text::HA_CENTER, Rendering::Text::VA_MIDDLE, std::string(), std::string());
	transformText->setTransformation(HomogenousMatrix4(Vector3(0, height * Scalar(0.85), Scalar(0.015))));
	transform->addChild(std::move(transformText));

	return transform;
}

Rendering::TransformRef InteractiveHomeExperience::createTransparentBox(const Rendering::EngineRef& engine, const RGBAColor& color)
{
	ocean_assert(engine);
	ocean_assert(color.isValid());

	Rendering::TransformRef transform = engine->factory().createTransform();

	Rendering::TransformRef front = Rendering::Utilities::createBox(engine, Vector3(1, 1, 0), color);
	front->setTransformation(HomogenousMatrix4(Vector3(0, 0, Scalar(0.5))));

	Rendering::TransformRef back = Rendering::Utilities::createBox(engine, Vector3(1, 1, 0), color);
	back->setTransformation(HomogenousMatrix4(Vector3(0, 0, Scalar(-0.5))));

	Rendering::TransformRef left = Rendering::Utilities::createBox(engine, Vector3(0, 1, 1), color);
	left->setTransformation(HomogenousMatrix4(Vector3(Scalar(-0.5), 0, 0)));

	Rendering::TransformRef right = Rendering::Utilities::createBox(engine, Vector3(0, 1, 1), color);
	right->setTransformation(HomogenousMatrix4(Vector3(Scalar(0.5), 0, 0)));

	Rendering::TransformRef top = Rendering::Utilities::createBox(engine, Vector3(1, 0, 1), color);
	top->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(0.5), 0)));

	Rendering::TransformRef bottom = Rendering::Utilities::createBox(engine, Vector3(1, 0, 1), color);
	bottom->setTransformation(HomogenousMatrix4(Vector3(0, Scalar(-0.5), 0)));

	transform->addChild(std::move(front));
	transform->addChild(std::move(back));

	transform->addChild(std::move(left));
	transform->addChild(std::move(right));

	transform->addChild(std::move(top));
	transform->addChild(std::move(bottom));

	return transform;
}

}

}
