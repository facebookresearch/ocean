// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "application/ocean/xrplayground/common/experiences/sharedboardgame/GamePiece.h"

#include "application/ocean/xrplayground/common/experiences/XRPlaygroundExperience.h"

#include "application/ocean/xrplayground/common/SoundManager.h"

#include "ocean/base/ScopedValue.h"

#include "ocean/rendering/Utilities.h"

namespace Ocean
{

namespace XRPlayground
{

void GamePiece::preUpdate(const Rendering::EngineRef& engine, const bool useStrongShadow, const Timestamp timestamp)
{
	if (hasBeenDeleted_)
	{
		return;
	}

	ocean_assert(renderingTransformParent_);

	if (vertsNode_->hasBeenDeleted())
	{
		if (renderingTransformObject_)
		{
			renderingTransformParent_->removeChild(renderingTransformObject_);
			renderingTransformObject_.release();
		}

		if (renderingTransformShadow_)
		{
			renderingTransformParent_->removeChild(renderingTransformShadow_);
			renderingTransformShadow_.release();
		}

		renderingTransformParent_.release();

		hasBeenDeleted_ = true;

		return;
	}

	if (vertsNode_->isInitialized())
	{
		const VectorF3 position = vertsNode_->field<VectorF3>(0u);

		if (!renderingTransformObject_)
		{
			const VectorF3 color = vertsNode_->field<VectorF3>(1u);

			ocean_assert(!renderingTransformShadow_);
			renderingTransformObject_ = createGamePiece(engine, RGBAColor(color[0], color[1], color[2]), renderingTransformShadow_, renderingMaterialShadow_);
			renderingTransformParent_->addChild(renderingTransformObject_);

			renderingTransformParent_->addChild(renderingTransformShadow_);
			renderingTransformShadow_->setVisible(false);
		}

		ocean_assert(renderingTransformObject_);
		renderingTransformObject_->setTransformation(HomogenousMatrix4(Vector3(position)));

		if (position.y() == 0)
		{
			renderingTransformShadow_->setVisible(false);
		}
		else
		{
			renderingTransformShadow_->setTransformation(HomogenousMatrix4(Vector3(position.x(), Scalar(0.0001), position.z())));
			renderingTransformShadow_->setVisible(true);
		}

		ocean_assert(renderingMaterialShadow_);

		renderingMaterialShadow_->setTransparency(useStrongShadow ? 0.5f : 0.9f);

		if (!isLocal_)
		{
			const uint64_t dropCounter = vertsNode_->field<uint64_t>(2u);

			if (dropCounter != dropCounter_)
			{
				dropCounter_ = dropCounter;

				playGamePieceDropSound(vertsNode_->field<float>(3u));
			}
		}
	}
}

#ifdef XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

void GamePiece::handleInteraction(const HomogenousMatrix4& board_T_world, const Timestamp& timestamp, InteractionState& interactionState)
{
	if (!renderingTransformObject_)
	{
		return;
	}

	if (dropTimestamp_.isValid())
	{
		const double time = double(timestamp - dropTimestamp_);
		ocean_assert(time >= 0.0);

		const Scalar distance = Scalar(0.5) * gravity_ * Scalar(time * time);

		Vector3 position(vertsNode_->field<VectorF3>(0u) + VectorF3(0, float(distance), 0));

		if (position.y() <= 0)
		{
			position.y() = 0;
			dropTimestamp_.toInvalid();
		}

		vertsNode_->setField<VectorF3>(0u, position);

		return;
	}

	bool skipNextHand = false;

	for (const bool left : {true, false})
	{
		const bool right = !left;

		bool& wasPinching = interactionState.wasPinching_[left ? 0 : 1];

		Vector3 worldPosition(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());
		const bool pinching = XRPlaygroundExperience::isHandPinching(left, &worldPosition);

		const ScopedValue<bool> scopedWasPinching(wasPinching, pinching);

		if (skipNextHand)
		{
			continue;
		}

		if (wasPinching)
		{
			if ((interactionState.movingLocalGamePieceLeft_ && left) || (interactionState.movingLocalGamePieceRight_ && right))
			{
				if (!pinching || worldPosition.x() == Numeric::minValue())
				{
					ocean_assert(interactionState.previousPinchingPosition_.x() != Numeric::minValue());

					const float dropHeight = float(interactionState.previousPinchingPosition_.y());
					ocean_assert(dropHeight >= 0.0f);

					if (dropHeight > 0.0f)
					{
						playGamePieceDropSound(dropHeight);

						vertsNode_->setField<uint64_t>(2u, vertsNode_->field<uint64_t>(2u) + 1ull);
						vertsNode_->setField<float>(3u, dropHeight);
					}

					interactionState.movingLocalGamePieceLeft_ = false;
					interactionState.movingLocalGamePieceRight_ = false;

					vertsNode_->setField<VectorF3>(0u, VectorF3(interactionState.previousPinchingPosition_));

					interactionState.previousPinchingPosition_ = Vector3(Numeric::minValue(), Numeric::minValue(), Numeric::minValue());

					dropTimestamp_ = timestamp;
				}
				else
				{
					Vector3 boardPosition = board_T_world * worldPosition + movingLocalGamePieceOffset_;

					if (boardPosition.y() < 0) // ensuring that the game piece never drops below the board
					{
						boardPosition.y() = 0;
					}

					vertsNode_->setField<VectorF3>(0u, VectorF3(boardPosition));

					interactionState.previousPinchingPosition_ = boardPosition;
				}

				skipNextHand = true;
			}
		}
		else
		{
			if (worldPosition.x() != Numeric::minValue() && !interactionState.movingLocalGamePieceLeft_ && !interactionState.movingLocalGamePieceRight_)
			{
				if (pinching)
				{
					constexpr Scalar extension = Scalar(0.01);

					const Vector3 boardPosition = board_T_world * worldPosition;

					if (renderingTransformObject_->boundingBox().isInside(boardPosition, extension))
					{
						movingLocalGamePieceOffset_ = renderingTransformObject_->transformation().translation() - boardPosition; // offset in board domain

						if (left)
						{
							interactionState.movingLocalGamePieceLeft_ = true;
						}
						else
						{
							interactionState.movingLocalGamePieceRight_ = true;
						}

						skipNextHand = true;

						interactionState.previousPinchingPosition_ = boardPosition;
					}
				}
			}
		}
	}

	ocean_assert(!interactionState.movingLocalGamePieceLeft_ || !interactionState.movingLocalGamePieceRight_);
}

#endif // XR_PLAYGROUND_PLATFORM_BUILD_META_QUEST

Rendering::TransformRef GamePiece::createGamePiece(const Rendering::EngineRef& engine, const RGBAColor& color, Rendering::TransformRef& transformShadow, Rendering::MaterialRef& materialShadow)
{
	ocean_assert(engine);
	ocean_assert(color.isValid());

	constexpr Scalar radius = Scalar(0.01);
	constexpr Scalar height = Scalar(0.03);

	Rendering::TransformRef transform = engine->factory().createTransform();

	Rendering::TransformRef cone = Rendering::Utilities::createCone(engine, radius, height, color);
	cone->setTransformation(HomogenousMatrix4(Vector3(0, height * Scalar(0.5), 0)));
	transform->addChild(std::move(cone));

	Rendering::TransformRef sphere = Rendering::Utilities::createSphere(engine, radius, color);
	sphere->setTransformation(HomogenousMatrix4(Vector3(0, height, 0)));
	transform->addChild(std::move(sphere));

	transformShadow = Rendering::Utilities::createCylinder(engine, radius, Scalar(0.0001), RGBAColor(0.0f, 0.0f, 0.0f, 0.1f), nullptr /*cylinder*/, nullptr /*attributeSet*/, &materialShadow);

	return transform;
}

void GamePiece::playGamePieceDropSound(const float dropHeight)
{
	if (dropHeight <= NumericF::eps())
	{
		return;
	}

	constexpr float minDropHeight = 0.05f; // 5cm
	constexpr float maxDropHeight = 0.20f; // 20cm

	const float dropPercentage = minmax<float>(minDropHeight, dropHeight, maxDropHeight) / maxDropHeight;

	SoundManager::get().play("GAME_PIECE_DROP", dropPercentage);
}

}

}
