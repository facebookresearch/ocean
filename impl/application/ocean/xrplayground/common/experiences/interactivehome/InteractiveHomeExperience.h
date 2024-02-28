// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_INTERACTIVEHOME_INTERACTIVE_HOME_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_INTERACTIVEHOME_INTERACTIVE_HOME_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "ocean/math/BoundingBox.h"

#include "ocean/rendering/Text.h"
#include "ocean/rendering/Transform.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/vrapi/application/VRTableMenu.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

/**
 * This experience shows how to realize a Horizon home-like environment with interactive elements.
 * @ingroup xrplayground
 */
class InteractiveHomeExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * This class holds the relevant information of an action.
		 */
		class Action
		{
			public:

				/**
				 * Definition of individual types of actions.
				 */
				enum ActionType : uint32_t
				{
					/// No action.
					AT_NONE = 0u,
					/// A proximity action triggered with the body.
					AT_PROXIMITY_BODY = (1u << 0u),
					/// A proximity action triggered with controllers/hands.
					AT_PROXIMITY_CONTROLLERS = (1u << 1u),
					/// An action which can be triggered with a controller ray.
					AT_RAY_CONTROLLERS = (1u << 2u),
					/// An action which can be triggered via proximity or rays using hands or controllers.
					AT_PROXIMITY_OR_RAY = AT_PROXIMITY_CONTROLLERS | AT_RAY_CONTROLLERS
				};

			public:

				/**
				 * Creates a new action.
				 * @param actionType The type of the action
				 * @param appId The id of the app associated with the action
				 * @param description The description of the action
				 * @param renderingTransform The Transform node associated with the action, must be valid
				 * @param box Optional explicit interaction bounding box.
				 */
				inline Action(const ActionType actionType, const uint64_t appId, std::string&& description, const Rendering::TransformRef& renderingTransform, const Box3& box = Box3());

				/**
				 * Returns the plane of the action.
				 * @return The action's plane
				 */
				inline Plane3 plane() const;

				/**
				 * Returns the transformation between action and world.
				 * @return The action's transformation
				 */
				inline HomogenousMatrix4 world_T_action() const;

				/**
				 * Returns the transformation between action and the parent object.
				 * @return The action's transformation
				 */
				inline HomogenousMatrix4 parent_T_action() const;

				/**
				 * Returns the rendering bounding box of this action.
				 * @return The action's counding box.
				 */
				inline BoundingBox renderingBoundingBox() const;

			public:

				/// The type of the action.
				ActionType actionType_ = AT_NONE;

				/// The id of the app associated with the action.
				uint64_t appId_ = 0ull;

				/// The description of the action.
				std::string description_;

				/// The Transform node associated with the action.
				Rendering::TransformRef renderingTransform_;

				/// Optional explicit interaction bounding box.
				Box3 box_;

				/// The timestamp until this action is disabled.
				Timestamp disabledUntilTimestamp_;

				/// The transformation between action and world, when an interaction stated.
				HomogenousMatrix4 world_T_actionInteractionStarted_ = HomogenousMatrix4(false);

				/// The transformation between action and the parent object, when an interaction stated.
				HomogenousMatrix4 parent_T_actionInteractionStarted_ = HomogenousMatrix4(false);

				/// The explicit rotation angle of this action, in degree.
				Scalar rotationAngle_ = 0;
		};

		/**
		 * Definition of a vector holding actions.
		 */
		typedef std::vector<Action> Actions;

	protected:

		/// The extention of a selection bounding box.
		static constexpr Scalar selectionExtension_ = Scalar(0.1);

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~InteractiveHomeExperience() override;

		/**
		 * Loads this experience.
		 * @see Experience::load().
		 */
		bool load(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp, const std::string& properties) override;

		/**
		 * Unloads this experience.
		 * @see Experience::unload().
		 */
		bool unload(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Timestamp timestamp) override;

		/**
		 * Pre update interaction function which allows to adjust any rendering object before it gets rendered.
		 * @see Experience::preUpdate().
		 */
		Timestamp preUpdate(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const Rendering::ViewRef& view, const Timestamp timestamp) override;

		/**
		 * Key press function.
		 * @see Experience::onKeyPress().
		 */
		void onKeyPress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& key, const Timestamp timestamp) override;

		/**
		 * Creates a new UserProfileExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Protected default constructor.
		 */
		InteractiveHomeExperience() = default;

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/**
		 * Determines a movable action.
		 * @param timestamp The current timestamp
		 */
		void determineMovableAction(const Timestamp& timestamp);

		/**
		 * Moves the currently movable action.
		 * @param world_T_device The transformation between device and world, must be valid
		 * @param timestamp The current timestamp
		 */
		void moveAction(const HomogenousMatrix4& world_T_device, const Timestamp& timestamp);

		/**
		 * Determines an action which can be activated.
		 * @param world_T_device The transformation between device and world, must be valid
		 * @param timestamp The current timestamp
		 */
		void determineActivableAction(const HomogenousMatrix4& world_T_device, const Timestamp& timestamp);

		/**
		 * Launches a specified app.
		 * @param appId The id of the app to launch, must be valid
		 * @return True, if succeeded
		 */
		static bool launchApp(const uint64_t appId);

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/**
		 * Creates an open door.
		 * @param engine The rendering engine to be used, must be valid
		 * @param doorColor The color of the door, must be valid
		 * @param frameColor The color of the frame frame, must be valid
		 * @param width The width of the door, in meter, with range (0, infinity)
		 * @param height The height of the door, in meter, with range (0, infinity)
		 * @param text The text above the door
		 * @return The Transform node holding the door
		 */
		static Rendering::TransformRef createDoor(const Rendering::EngineRef& engine, const RGBAColor& doorColor, const RGBAColor& frameColor, const Scalar width, const Scalar height, const std::string& text);

		/**
		 * Creates a table object.
		 * @param engine The rendering engine to be used, must be valid
		 * @param color The color of the table, must be valid
		 * @param width The width of the table, in meter, with range (0, infinity)
		 * @param height The height of the table, in meter, with range (0, infinity)
		 * @param depth The depth of the table, in meter, with range (0, infinity)
		 * @return The Transform node holding the table
		 */
		static Rendering::TransformRef createTable(const Rendering::EngineRef& engine, const RGBAColor color, const Scalar width, const Scalar height, const Scalar depth);

		/**
		 * Creates a rotary files object.
		 * @param engine The rendering engine to be used, must be valid
		 * @param frameColor The color of the frame, must be valid
		 * @param fileColor The color of the file, must be valid
		 * @param size The size of the resulting object, in meter, with range (0, infinity)
		 * @param text The text on top of the file
		 * @return The Transform node holding the object
		 */
		static Rendering::TransformRef createRotaryFiles(const Rendering::EngineRef& engine, const RGBAColor& frameColor, const RGBAColor& fileColor, const Scalar size, const std::string& text);

		/**
		 * Creates a screen object.
		 * @param engine The rendering engine to be used, must be valid
		 * @param frameColor The color of the frame, must be valid
		 * @param screenColor The color if the screen, must be valid
		 * @param size The size of the screen, in meter, with range (0, infinity)
		 * @param text The text on the screen
		 * @return The Transform node holding the object
		 */
		static Rendering::TransformRef createScreen(const Rendering::EngineRef& engine, const RGBAColor& frameColor, const RGBAColor& screenColor, const Scalar size, const std::string& text);

		/**
		 * Creates a tablet object.
		 * @param engine The rendering engine to be used, must be valid
		 * @param frameColor The color of the frame, must be valid
		 * @param screenColor The color of the screen, must be valid
		 * @param size The size of the tablet, in meter, with range (0, infinity)
		 * @param text The text on the tablet
		 * @return The Transform node holding the object
		 */
		static Rendering::TransformRef createTablet(const Rendering::EngineRef& engine, const RGBAColor& frameColor, const RGBAColor& screenColor, const Scalar size, const std::string& text);

		/**
		 * Creates a racket object.
		 * @param engine The rendering engine to be used, must be valid
		 * @param handleColor The color of the handle, must be valid
		 * @param plateColor The color of the plate, must be valid
		 * @param size The size of the tablet, in meter, with range (0, infinity)
		 * @param text The text on the tablet
		 * @return The Transform node holding the object
		 */
		static Rendering::TransformRef createRacket(const Rendering::EngineRef& engine, const RGBAColor& handleColor, const RGBAColor& plateColor, const Scalar size, const std::string& text);

		/**
		 * Creates a white board.
		 * @param engine The rendering engine to be used, must be valid
		 * @param frameColor The color of the frame, must be valid
		 * @param boardColor The color of the board frame, must be valid
		 * @param width The width of the baord, in meter, with range (0, infinity)
		 * @param height The height of the board, in meter, with range (0, infinity)
		 * @param text The text on the board
		 * @return The Transform node holding the board
		 */
		static Rendering::TransformRef createWhiteBoard(const Rendering::EngineRef& engine, const RGBAColor& frameColor, const RGBAColor& boardColor, const Scalar width, const Scalar height, const std::string& text);

		/**
		 * Creates a transparent box.
		 * @param engine The rendering engine to be used, must be valid
		 * @param color The color to be used
		 * @return The Transform node holding the box
		 */
		static Rendering::TransformRef createTransparentBox(const Rendering::EngineRef& engine, const RGBAColor& color);

	protected:

		/// The rendering Transform node at floor level.
		Rendering::TransformRef renderingTransformFloor_;

		/// The actions of the experience.
		Actions actions_;

		/// The Transform node holding a box for moving actions.
		Rendering::TransformRef renderingTransformMovingSelectionBox_;

		/// The transform node holding a sphere for moving actions.
		Rendering::TransformRef renderingTransformMovingSelectionPoint_;

		/// The transform node holding a box for activable actions.
		Rendering::TransformRef renderingTransformActivableSelectionBox_;

		/// The action which is currently moved.
		Action* movingAction_ = nullptr;

		/// The action which is currently active.
		Action* activeAction_ = nullptr;

		/// The 3D point wihtin the plane of the moving action.
		Vector3 movingPlaneIntersection_ = Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/// The table menu allowing to show a simple user interface.
		Platform::Meta::Quest::VrApi::Application::VRTableMenu vrTableMenu_;

		/// The remote type which is currently used to move an action.
		Platform::Meta::Quest::VrApi::TrackedRemoteDevice::RemoteType movingRemoteType_ = Platform::Meta::Quest::VrApi::TrackedRemoteDevice::RT_UNDEFINED;

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
};

inline InteractiveHomeExperience::Action::Action(const ActionType actionType, const uint64_t appId, std::string&& description, const Rendering::TransformRef& renderingTransform, const Box3& box) :
	actionType_(actionType),
	appId_(appId),
	description_(std::move(description)),
	renderingTransform_(renderingTransform),
	box_(box)
{
	// nothing to do here
}

inline Plane3 InteractiveHomeExperience::Action::plane() const
{
	const BoundingBox boundingBox = renderingBoundingBox();

	const Vector3 boundingBoxBottom(boundingBox.center().x(), boundingBox.center().y() - boundingBox.yDimension() * Scalar(0.5), boundingBox.center().z());

	const Vector3 planePoint = world_T_action() * boundingBoxBottom;

	return Plane3(planePoint, Vector3(0, 1, 0));
}

inline HomogenousMatrix4 InteractiveHomeExperience::Action::world_T_action() const
{
	if (world_T_actionInteractionStarted_.isValid())
	{
		return world_T_actionInteractionStarted_;
	}

	ocean_assert(renderingTransform_);
	return renderingTransform_->worldTransformation();
}

inline HomogenousMatrix4 InteractiveHomeExperience::Action::parent_T_action() const
{
	ocean_assert(renderingTransform_);
	return renderingTransform_->transformation();
}

inline BoundingBox InteractiveHomeExperience::Action::renderingBoundingBox() const
{
	ocean_assert(renderingTransform_);

	// determining the inner bounding box, not including the local transformation of the transform node

	BoundingBox boundingBox;

	for (unsigned int n = 0u; n < renderingTransform_->numberChildren(); ++n)
	{
		const BoundingBox localBoundingBox = renderingTransform_->child(n)->boundingBox();

		if (localBoundingBox.isValid())
		{
			boundingBox += localBoundingBox;
		}
	}

	return boundingBox;
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_INTERACTIVEHOME_INTERACTIVE_HOME_EXPERIENCE_H
