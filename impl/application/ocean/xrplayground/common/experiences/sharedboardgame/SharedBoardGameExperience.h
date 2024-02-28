// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_SHAREDBOARDGAME_SHARED_BOARD_GAME_EXPERIENCE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_SHAREDBOARDGAME_SHARED_BOARD_GAME_EXPERIENCE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"
#include "application/ocean/xrplayground/common/experiences/XRPlaygroundSharedSpaceExperience.h"

#include "application/ocean/xrplayground/common/experiences/sharedboardgame/GamePiece.h"

#include "application/ocean/xrplayground/common/SoundManager.h"

#include "ocean/devices/Tracker6DOF.h"

#include "ocean/media/FrameMedium.h"

#include "ocean/rendering/Transform.h"

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST
	#include "ocean/platform/meta/quest/vrapi/application/VRTableMenu.h"
#endif

namespace Ocean
{

namespace XRPlayground
{

#ifdef OCEAN_PLATFORM_BUILD_MOBILE

/**
 * This experience implements an experience in which users can hangout together across individual platforms.
 * @ingroup xrplayground
 */
class SharedBoardGameExperience : protected XRPlaygroundSharedSpaceExperience
{
	friend class GamePiece;

	protected:

		/// True, to skip the game selection when starting the experience.
		static constexpr bool useDemoMode_ = false;

	protected:

		/**
		 * Definition of a map mapping game piece node ids to game piece objects.
		 */
		typedef std::unordered_map<Network::Verts::Node::NodeId, GamePiece> GamePieceMap;

		/**
		 * Definition of individual experience states.
		 */
		enum ExperienceState : uint32_t
		{
			/// The user needs to select the game.
			ES_SELECT_GAME = 0u,
			/// The experience tries to detect the board.
			ES_TRY_DETECT_REAL_BOARD,
			/// The board has been detected or the user has selected to use a real board.
			ES_USE_REAL_BOARD,
			/// The user has selected to use a virtual board.
			ES_USE_VIRTUAL_BOARD
		};

	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~SharedBoardGameExperience() override;

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
		 * Mouse press event function.
		 * @see Experience::onMousePress().
		 */
		void onMousePress(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp) override;

		/**
		 * Mouse release event function.
		 * @see Experience::onMouseRelease().
		 */
		void onMouseRelease(const Interaction::UserInterface& userInterface, const Rendering::EngineRef& engine, const std::string& button, const Vector2& screenPosition, const Line3& ray, const Timestamp timestamp) override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();

	protected:

		/**
		 * Default constructor.
		 */
		SharedBoardGameExperience();

		/**
		 * Loads a specified game.
		 * @param url The url of the game to load, must be valid
		 * @param engine The rendering engine to be used, must be valid
		 * @return True, if succeeded
		 */
		bool loadGame(const std::string& url, const Rendering::EngineRef& engine);

		/**
		 * Updates the location of the board.
		 * @param world_T_device The transformation between device and world, must be valid
		 */
		void updateBoardLocation(const HomogenousMatrix4& world_T_device);

		/**
		 * Handles the game pieces.
		 * @param engine The rendering engine to be used, must be valid
		 * @param timestamp The current timestamp
		 */
		void handleGamePieces(const Rendering::EngineRef& engine, const Timestamp& timestamp);

		/**
		 * The event function called to define the initial user location in shared space.
		 * @see XRPlaygroundSharedSpaceExperience::onInitializeLocalUser().
		 */
		void onInitializeLocalUser(HomogenousMatrix4& remoteHeadsetWorld_T_remoteAvatar) override;

		/**
		 * The event function for new entities.
		 * @param driver The driver sending the event
		 * @param entity The new entity
		 */
		void onNewEntity(Network::Verts::Driver& driver, const Network::Verts::SharedEntity& entity);

	protected:

		/// The experience's state.
		ExperienceState experienceState_ = ES_SELECT_GAME;

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/// The table menu allowing to show a simple user interface.
		Platform::Meta::Quest::VrApi::Application::VRTableMenu vrTableMenu_;

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/// The tracker used to track the board game.
		Devices::Tracker6DOFRef patternTracker_;

		/// The medium object of the pattern tracker.
		Media::FrameMediumRef frameMedium_;

		/// The rendering Transform object holding the board game.
		Rendering::TransformRef renderingTransformBoardGame_;

		/// The rendering Transform object holding the real objects of the board game.
		Rendering::TransformRef renderingTransformBoardGameReal_;

		/// The rendering Transform object holding the virtual objects of the board game.
		Rendering::TransformRef renderingTransformBoardGameVirtual_;

		/// The most recent rendering View object.
		Rendering::ViewRef renderingView_;

		/// The map holding all game pieces (also holding the local user).
		GamePieceMap gamePieceMap_;

		/// The VERTS node of the game piece of the local user.
		Network::Verts::SharedNode localUserGamePieceNode_;

		/// The object id of the pattern tracker.
		Devices::Tracker6DOF::ObjectId patternTrackerObjectId_ = Devices::Tracker6DOF::invalidObjectId();

		/// The timestamp when the game was selected.
		Timestamp gameSelectionTimestamp_;

		/// The timestamp of the last pattern tracker sample.
		Timestamp patternTrackerSampleTimestamp_;

		/// The recent transformations between cameras and world.
		HomogenousMatrices4 world_T_cameras_;

		/// The recent transformations between cameras and board.
		HomogenousMatrices4 board_T_cameras_;

		/// The recent transformation between board and world.
		HomogenousMatrix4 world_T_board_ = HomogenousMatrix4(false);

		/// The dimension of the board.
		Vector3 boardDimension_ = Vector3(0, 0, 0);

		/// The manual board rotation when using a virtual board, in radian.
		Scalar virtualBoardRotation_ = Scalar(0);

		/// The most recent scale between pattern and world.
		Scalar recentScale_ = 0;

		/// The interaction state for game piece interactions.
		GamePiece::InteractionState interactionState_;

		/// True, to hide hands in passthrough mode.
		bool hideHands_ = false;

		/// The screen position when the user pushed the screen.
		Vector2 screenPositionPressed_ = Vector2(Numeric::minValue(), Numeric::minValue());

		/// The subscription object for new entity events.
		Network::Verts::Driver::NewEntityScopedSubscription newEntityScopedSubscription_;

		/// Definition of a node type holding user game pieces.
		static constexpr const char* nodeTypeGamePiece_ = "GamePiece";

		/// The experience's sound subscriptions.
		SoundManager::SoundScopedSubscriptions soundScopedSubscriptions_;

		/// The lock for game pieces.
		Lock gamePieceLock_;
};

#else // OCEAN_PLATFORM_BUILD_MOBILE

/**
 * This experience shows how to use Avatars in an experience on a Quest.
 * @ingroup xrplayground
 */
class SharedBoardGameExperience : public XRPlaygroundExperience
{
	public:

		/**
		 * Destructs this experience, all resources will be released.
		 */
		~SharedBoardGameExperience() override;

		/**
		 * Creates a new UserInterfaceExperience object.
		 * @return The new experience
		 */
		static std::unique_ptr<XRPlaygroundExperience> createExperience();
};

#endif // OCEAN_PLATFORM_BUILD_MOBILE

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_SHAREDBOARDGAME_SHARED_BOARD_GAME_EXPERIENCE_H
