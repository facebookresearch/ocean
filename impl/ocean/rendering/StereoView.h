/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_STEREO_VIEW_H
#define META_OCEAN_RENDERING_STEREO_VIEW_H

#include "ocean/rendering/Rendering.h"
#include "ocean/rendering/PerspectiveView.h"

namespace Ocean
{

namespace Rendering
{

// Forward declaration
class StereoView;

/**
 * Definition of a smart object reference holding a stereo view node.
 * @see SmartObjectRef, StereoView.
 * @ingroup rendering
 */
typedef SmartObjectRef<StereoView> StereoViewRef;

/**
 * This class is the base class for all stereo views.
 * @ingroup rendering
 */
class OCEAN_RENDERING_EXPORT StereoView : virtual public PerspectiveView
{
	public:

		/**
		 * Definition of different stereo visualization types.
		 */
		enum StereoType
		{
			/// Invalid stereo type.
			STEREO_TYPE_INVALID,
			/// Quadbuffered stereo visualization type.
			STEREO_TYPE_QUADBUFFERED,
			/// Anaglyph stereo visualization type.
			STEREO_TYPE_ANAGLYPH,
		};

	public:

		/**
		 * Returns the focus distance of this view.
		 * @return View focus distance
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual Scalar focusDistance() const;

		/**
		 * Returns whether left and right image is reversed.
		 * @return True, if so
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool eyesReversed() const;

		/**
		 * Returns the stereo visualization type of this stereo view.
		 * @return Stereo visualization type
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual StereoType stereoType() const;

		/**
		 * Returns the transformation between the center of both views (device) and world.
		 * @return The device's transformation, which is world_T_device
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setTransformation().
		 */
		HomogenousMatrix4 transformation() const override;

		/**
		 * Returns the position and orientation of the left view in world coordinates (the left extrinsic camera data, the inverse of the known View Matrix).
		 * @return The transformation converting the left view to world, which is world_T_leftView
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see rightTransformation().
		 */
		virtual HomogenousMatrix4 leftTransformation() const;

		/**
		 * Returns the position and orientation of the right view in world coordinates (the right extrinsic camera data, the inverse of the known View Matrix).
		 * @return The transformation converting the right view to world, which is world_T_rightView
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see leftTransformation().
		 */
		virtual HomogenousMatrix4 rightTransformation() const;

		/**
		 * Returns the left projection matrix.
		 * @return The left projection matrix
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see rightProjectionMatrix().
		 */
		virtual SquareMatrix4 leftProjectionMatrix() const;

		/**
		 * Returns the right projection matrix.
		 * @return The right projection matrix
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see leftProjectionMatrix().
		 */
		virtual SquareMatrix4 rightProjectionMatrix() const;

		/**
		 * Sets the focus distance of this view.
		 * @param focus View focus distance to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setFocusDistance(const Scalar focus);

		/**
		 * Sets whether left and right image will be reversed.
		 * @param reverse True, if the left image will be displayed for the right eye and vice versa
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual void setEyesReversed(const bool reverse);

		/**
		 * Sets the stereo visualization type of this stereo view.
		 * @param type Stereo visualization type to set
		 * @return True, if succeeded
		 * @exception NotSupportedException Is thrown if this function is not supported
		 */
		virtual bool setStereoType(const StereoType type);

		/**
		 * Sets the position and orientation of the center between left and right view (device) in world coordinates.
		 * @param world_T_device The transformation converting the device to world, must be valid
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see transformation().
		 */
		void setTransformation(const HomogenousMatrix4& world_T_device) override;

		/**
		 * Sets the position and orientation of the left view in world coordinates (the left extrinsic camera data, the inverse of the known View Matrix).
		 * @param world_T_leftView The transformation converting the left view to world, must be valid
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setRightTransformation().
		 */
		virtual void setLeftTransformation(const HomogenousMatrix4& world_T_leftView);

		/**
		 * Sets the position and orientation of the right view in world coordinates (the right extrinsic camera data, the inverse of the known View Matrix).
		 * @param world_T_rightView The transformation converting the right view to world, must be valid
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setLeftTransformation().
		 */
		virtual void setRightTransformation(const HomogenousMatrix4& world_T_rightView);

		/**
		 * Sets the left projection matrix.
		 * @param leftClip_T_leftView_ Left projection matrix to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setRightProjectionMatrix().
		 */
		virtual void setLeftProjectionMatrix(const SquareMatrix4& leftClip_T_leftView_);

		/**
		 * Sets the right projection matrix.
		 * @param rightClip_T_rightView_ Right projection matrix to set
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setLeftProjectionMatrix().
		 */
		virtual void setRightProjectionMatrix(const SquareMatrix4& rightClip_T_rightView_);

		/**
		 * Returns the type of this object.
		 * @see Object::type().
		 */
		ObjectType type() const override;

	protected:

		/**
		 * Creates a new stereo view object.
		 */
		StereoView();

		/**
		 * Destructs a stereo view object.
		 */
		~StereoView() override;

		/**
		 * Returns the projection matrix of this view.
		 * Protected function as it has no meaning in stereo views.
		 * @return The invalid 4x4 matrix
		 */
		SquareMatrix4 projectionMatrix() const override;
};

}

}

#endif // META_OCEAN_RENDERING_STEREO_VIEW_H
