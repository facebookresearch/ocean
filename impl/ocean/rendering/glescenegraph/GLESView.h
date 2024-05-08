/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_VIEW_H
#define META_OCEAN_RENDERING_GLES_VIEW_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESObject.h"

#include "ocean/rendering/LightSource.h"
#include "ocean/rendering/View.h"


namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph view object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESView :
	virtual public GLESObject,
	virtual public View
{
	friend class GLESFramebuffer;

	public:

		/**
		 * Returns the aspect ratio of this view.
		 * @see View::aspectRatio().
		 */
		Scalar aspectRatio() const override;

		/**
		 * Returns the distance to the near clipping plane.
		 * @see View::nearDistance().
		 */
		Scalar nearDistance() const override;

		/**
		 * Returns the distance to the far clipping plane.
		 * @see View::farDistance().
		 */
		Scalar farDistance() const override;

		/**
		 * Returns the view projection matrix.
		 * @return Projection matrix
		 */
		SquareMatrix4 projectionMatrix() const override;

		/**
		 * Returns the transformation between view and world (the inverse of the known View Matrix).
		 * @see View::transformation().
		 */
		HomogenousMatrix4 transformation() const override;

		/**
		 * Returns the background color of the view.
		 * @see View::backgroundColor().
		 */
		RGBAColor backgroundColor() const override;

		/**
		 * Returns whether the headlight is enabled.
		 * @see View::useHeadlight().
		 */
		bool useHeadlight() const override;

		/**
		 * Returns the headlight object of this view
		 * @return Headlight object, if any
		 */
		const LightSourceRef& headlight() const;

		/**
		 * Sets the aspect ratio of this view.
		 * @param aspectRatio Rate between width and height
		 */
		bool setAspectRatio(const Scalar aspectRatio) override;

		/**
		 * Sets the distance to the near clippling plane.
		 * @see View::setNearDistance().
		 */
		bool setNearDistance(const Scalar distance) override;

		/**
		 * Sets the distance to the far clipping plane.
		 * @see View::setFarDistance().
		 */
		bool setFarDistance(const Scalar distance) override;

		/**
		 * Sets the distance to the near and far clipping plane.
		 * @see View::setNearFarDistance().
		 */
		bool setNearFarDistance(const Scalar nearDistance, const Scalar farDistance) override;

		/**
		 * Sets the position and orientation of the view by a viewing matrix.
		 * @see View::setTransformation().
		 */
		void setTransformation(const HomogenousMatrix4& transformation) override;

		/**
		 * Sets the background color of the view.
		 * @see View::setBackgroundColor().
		 */
		bool setBackgroundColor(const RGBAColor& color) override;

		/**
		 * Sets whether the headlight should be activated.
		 * @see View::setUseHeadlight().
		 */
		void setUseHeadlight(const bool state) override;

		/**
		 * Sets the phantom mode of this view.
		 * @see View::setPhantomMode().
		 */
		bool setPhantomMode(const PhantomAttribute::PhantomMode mode) override;

		/**
		 * Returns the phantom mode of this view.
		 * @return Phantom mode
		 * @exception NotSupportedException Is thrown if this function is not supported
		 * @see setPhantomMode().
		 */
		PhantomAttribute::PhantomMode phantomMode() const override;

	protected:

		/**
		 * Creates a new view object.
		 */
		GLESView();

		/**
		 * Destructs a view object.
		 */
		~GLESView() override;

		/**
		 * (Re-)calculates the view projection matrix.
		 * @return True, if succeeded
		 */
		virtual bool calculateProjectionMatrix();

	protected:

		/// The view's projection matrix, which is clip_T_view
		SquareMatrix4 clip_T_view_ = SquareMatrix4(false);

		/// The transformation between view and world.
		HomogenousMatrix4 world_T_view_ = HomogenousMatrix4(true);

		/// Thew view's aspect ratio defined by the ratio between width and height, with range (0, infinity)
		Scalar aspectRatio_ = Scalar(1);

		/// The view's near clipping plane, with range (0, farDistance_)
		Scalar nearDistance_ = Scalar(0.01);

		/// The view's far clipping plane, with range (nearDistance_, infinity)
		Scalar farDistance_ = Scalar(1000);

		/// The view's background color.
		RGBAColor backgroundColor_ = RGBAColor(0.0f, 0.0f, 0.0f);

		/// True, if the view's headlight is enabled.
		bool useHeadlight_ = true;

		/// Thew view's headlight object.
		LightSourceRef headlight_;

		/// The phantom mode which is used in this view.
		PhantomAttribute::PhantomMode phantomMode_ = PhantomAttribute::PM_DEFAULT;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_VIEW_H
