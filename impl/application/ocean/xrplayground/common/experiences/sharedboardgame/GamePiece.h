// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_SHAREDBOARDGAME_GAME_PIECE_H
#define FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_SHAREDBOARDGAME_GAME_PIECE_H

#include "application/ocean/xrplayground/common/experiences/Experiences.h"

#include "ocean/network/verts/Node.h"

#include "ocean/rendering/Material.h"
#include "ocean/rendering/Transform.h"

namespace Ocean
{

namespace XRPlayground
{

/**
 * This class holds the relevant information of a game piece.
 * @ingroup xrplayground
 */
class GamePiece
{
	public:

		/**
		 * This class holds relevant information for the interaction with game pieces.
		 */
		class InteractionState
		{
			public:

				/// The most recent pinching position in the last frame, in the domain of the board.
				Vector3 previousPinchingPosition_ = Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());

				/// True, if left/right hand was pinching in the last frame.
				bool wasPinching_[2] = {false, false};

				/// True, if the local game piece is currently moving with the left hand.
				bool movingLocalGamePieceLeft_ = false;

				/// True, if the local game piece is currently moving with the right hand.
				bool movingLocalGamePieceRight_ = false;
		};

	protected:

		/// Gravity in meter per seconds squared.
		static constexpr Scalar gravity_ = Scalar(-9.8);

	public:

		/**
		 * Creates a new game piece object.
		 * @param renderingTransformParent The parent Transform node which will hold the game piece, must be valid
		 * @param isLocal True, if the game piece is local; False, if the game piece is remote
		 * @param vertsNode The VERTS node representing the game piece
		 */
		inline GamePiece(const Rendering::TransformRef& renderingTransformParent, const bool isLocal, Network::Verts::SharedNode vertsNode);

		/**
		 * Pre update interaction function which allows to adjust any rendering object before it gets rendered.
		 * @param engine The rendering engine to be used, must be valid
		 * @param useStrongShadow True, to use a strong shadow (with less transparency); False, to use a light shadow (with more transparency)
		 * @param timestamp The rendering timestamp, must be valid
		 */
		void preUpdate(const Rendering::EngineRef& engine, const bool useStrongShadow, const Timestamp timestamp);

		/**
		 * Returns whether the game piece has been deleted and whether it can be removed from the game.
		 * @return True, if so
		 */
		inline bool hasBeenDeleted() const;

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

		/**
		 * Handles the interaction with this game piece.
		 * @param board_T_world The transformation between world and board, must be valid
		 * @param timestamp The rendering timestamp, must be valid
		 * @param interactionState The current interaction state, may be changed
		 */
		void handleInteraction(const HomogenousMatrix4& board_T_world, const Timestamp& timestamp, InteractionState& interactionState);

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

	protected:

		/**
		 * Creates a game piece.
		 * @param engine The rendering engine to be used, must be valid
		 * @param color The color to be used, must be valid
		 * @param transformShadow The resulting rendering Transform node holding the shadow corresponding to the game piece
		 * @param materialShadow The resulting rendering Material object holding associated with the shadow
		 * @return The game piece, with origin at the bottom of the piece
		 */
		static Rendering::TransformRef createGamePiece(const Rendering::EngineRef& engine, const RGBAColor& color, Rendering::TransformRef& transformShadow, Rendering::MaterialRef& materialShadow);

		/**
		 * Plays a game piece drop sound.
		 * @param dropHeight The hight of the drop, in meter, with range [0, infinity)
		 */
		static void playGamePieceDropSound(const float dropHeight);

	protected:

		/// The parent Transform node which will hold the game piece.
		Rendering::TransformRef renderingTransformParent_;

		/// True, if the game piece is owned by the local user.
		bool isLocal_ = false;

		/// The VERTS node representing the game piece.
		Network::Verts::SharedNode vertsNode_;

		/// The rendering Transform node holding the game piece.
		Rendering::TransformRef renderingTransformObject_;

		/// The rendering Transform node holding the shadow of the game piece.
		Rendering::TransformRef renderingTransformShadow_;

		/// The rendering Material node associated with the shadow.
		Rendering::MaterialRef renderingMaterialShadow_;

		/// The counter counting how of the game piece has been dropped.
		uint64_t dropCounter_ = 0ull;

		/// The interaction offset in case a game piece is moved.
		Vector3 movingLocalGamePieceOffset_ = Vector3(0, 0, 0);

		/// True, if this game piece has been deleted and can be removed from the game.
		bool hasBeenDeleted_ = false;

		/// The timestamp when the game piece has been dropped, invalid if the game piece has not been dropped.
		Timestamp dropTimestamp_;
};

inline GamePiece::GamePiece(const Rendering::TransformRef& renderingTransformParent, const bool isLocal, Network::Verts::SharedNode vertsNode) :
	renderingTransformParent_(renderingTransformParent),
	isLocal_(isLocal),
	vertsNode_(std::move(vertsNode))
{
	ocean_assert(renderingTransformParent);
}

inline bool GamePiece::hasBeenDeleted() const
{
	return hasBeenDeleted_;
}

}

}

#endif // FACEBOOK_APPLICATION_OCEAN_XRPLAYGROUND_COMMON_EXPERIENCES_SHAREDBOARDGAME_GAME_PIECE_H
