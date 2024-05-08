/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_STEREO_VIEW_H
#define META_OCEAN_RENDERING_GLES_STEREO_VIEW_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESView.h"

#include "ocean/rendering/StereoView.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a perspective stereo view for GLESceneGraph.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESStereoView :
	virtual public GLESView,
	virtual public StereoView
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the transformation between the center of both views (device) and world.
		 * @see StereoView::transformation().
		 */
		HomogenousMatrix4 transformation() const override;

		/**
		 * Returns the position and orientation of the left view in world coordinates (the left extrinsic camera data, the inverse of the known View Matrix).
		 * @see StereoView::leftTransformation().
		 */
		HomogenousMatrix4 leftTransformation() const override;

		/**
		 * Returns the position and orientation of the right view in world coordinates (the right extrinsic camera data, the inverse of the known View Matrix).
		 * @see StereoView::rightTransformation().
		 */
		HomogenousMatrix4 rightTransformation() const override;

		/**
		 * Returns the left projection matrix.
		 * @see StereoView::leftProjectionMatrix().
		 */
		SquareMatrix4 leftProjectionMatrix() const override;

		/**
		 * Returns the right projection matrix.
		 * @see StereoView::rightProjectionMatrix().
		 */
		SquareMatrix4 rightProjectionMatrix() const override;

		/**
		 * Sets the position and orientation of the center between left and right view (device) in world coordinates.
		 * @see StereoView::setTransformation().
		 */
		void setTransformation(const HomogenousMatrix4& world_T_device) override;

		/**
		 * Sets the position and orientation of the left view in world coordinates (the left extrinsic camera data, the inverse of the known View Matrix).
		 * @see StereoView::setLeftTransformation().
		 */
		void setLeftTransformation(const HomogenousMatrix4& world_T_leftView) override;

		/**
		 * Sets the position and orientation of the right view in world coordinates (the right extrinsic camera data, the inverse of the known View Matrix).
		 * @see StereoView::setRightTransformation().
		 */
		void setRightTransformation(const HomogenousMatrix4& world_T_rightView) override;

		/**
		 * Sets the left projection matrix.
		 * @see StereoView::setLeftProjectionMatrix().
		 */
		void setLeftProjectionMatrix(const SquareMatrix4& leftClip_T_leftView_) override;

		/**
		 * Sets the right projection matrix.
		 * @see StereoView::setRightProjectionMatrix().
		 */
		void setRightProjectionMatrix(const SquareMatrix4& rightClip_T_rightView_) override;

	protected:

		/**
		 * Creates a new perspective view.
		 */
		GLESStereoView();

		/**
		 * Destructs a perspective view.
		 */
		~GLESStereoView() override;

		/**
		 * Returns the projection matrix of this view.
		 * Protected function as it has no meaning in stereo views.
		 * @return The invalid 4x4 matrix
		 */
		SquareMatrix4 projectionMatrix() const override;

	protected:

		/// The transformation between device and world.
		HomogenousMatrix4 world_T_device_ = HomogenousMatrix4(false);

		/// The transformation between left view and world.
		HomogenousMatrix4 world_T_leftView_ = HomogenousMatrix4(false);

		/// The transformation between right view and world.
		HomogenousMatrix4 world_T_rightView_ = HomogenousMatrix4(false);

		/// The left projection matrix.
		SquareMatrix4 rightClip_T_leftView_ = SquareMatrix4(false);

		/// The right projection matrix.
		SquareMatrix4 rightClip_T_rightView_ = SquareMatrix4(false);
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_STEREO_VIEW_H
