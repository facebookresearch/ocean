/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_VIEW_H
#define META_OCEAN_RENDERING_GI_VIEW_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIObject.h"

#include "ocean/rendering/LightSource.h"
#include "ocean/rendering/View.h"

#include "ocean/math/HomogenousMatrix4.h"
#include "ocean/math/SquareMatrix4.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a Global Illumination view object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIView :
	virtual public GIObject,
	virtual public View
{
	friend class GIFramebuffer;

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
		 * Returns the view (extrinsic) matrix.
		 * @return View matrix
		 */
		inline HomogenousMatrix4 matrix() const;

		/**
		 * Returns the inverted view (extrinsic) matrix.
		 * @return Inverted view matrix
		 */
		inline HomogenousMatrix4 invertedMatrix() const;

		/**
		 * Returns the view projection matrix.
		 * @return Projection matrix
		 */
		SquareMatrix4 projectionMatrix() const override;

		/**
		 * Returns the viewing matrix of the viewer as homogenous transformation.
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
		 * Sets the aspect ration of the view.
		 * @param aspectRatio Rate between width and height
		 */
		bool setAspectRatio(const Scalar aspectRatio) override;

		/**
		 * Sets the distance of the near clippling plane.
		 * @see View::setNearDistance().
		 */
		bool setNearDistance(const Scalar distance) override;

		/**
		 * Sets the distance of the far clipping plane.
		 * @see View::setFarDistance().
		 */
		bool setFarDistance(const Scalar distance) override;

		/**
		 * Sets the distance of the near and far clipping plane.
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

	protected:

		/**
		 * Creates a new view object.
		 */
		GIView();

		/**
		 * Destructs a view object.
		 */
		~GIView() override;

		/**
		 * (Re-)calculates the view projection matrix.
		 */
		virtual void calculateProjectionMatrix() = 0;

	protected:

		/// View projection matrix.
		SquareMatrix4 viewProjectionMatrix;

		/// View (extrinsic camera) matrix.
		HomogenousMatrix4 viewMatrix;

		/// Inverted view (extrinsic camera) matrix.
		HomogenousMatrix4 invertedViewMatrix;

		/// View aspect ratio defined by (width / height).
		Scalar viewAspectRatio;

		/// View near clipping plane.
		Scalar viewNearDistance;

		/// View far clipping plane.
		Scalar viewFarDistance;

		/// Holds the view background color.
		RGBAColor viewBackgroundColor;

		/// Determines whether the headlight is enabled.
		bool viewUseHeadlight;

		/// View headlight object.
		LightSourceRef viewHeadlight;
};

inline HomogenousMatrix4 GIView::matrix() const
{
	ocean_assert(viewMatrix == invertedViewMatrix.inverted());
	return viewMatrix;
}

inline HomogenousMatrix4 GIView::invertedMatrix() const
{
	ocean_assert(viewMatrix.inverted() == invertedViewMatrix);
	return invertedViewMatrix;
}

}

}

}

#endif // META_OCEAN_RENDERING_GI_VIEW_H
