/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_PERSPECTIVE_VIEW_H
#define META_OCEAN_RENDERING_GI_PERSPECTIVE_VIEW_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIView.h"

#include "ocean/rendering/PerspectiveView.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class implements a perspective view.
 * @ingroup renderingi
 */
class OCEAN_RENDERING_GI_EXPORT GIPerspectiveView :
	virtual public GIView,
	virtual public PerspectiveView
{
	friend class GIFactory;

	public:

		/**
		 * Returns the horizontal field of view in radian.
		 * @see View::fovX().
		 */
		Scalar fovX() const override;

		/**
		 * Sets the horizontal field of view in radian.
		 * @see View::setFovX().
		 */
		bool setFovX(const Scalar fovx) override;

		/**
		 * Returns a viewing ray into the scene.
		 * @see View::viewingRay().
		 */
		Line3 viewingRay(const Scalar x, const Scalar y, const unsigned int width, const unsigned int height) const override;

	protected:

		/**
		 * Creates a new perspective view.
		 */
		GIPerspectiveView();

		/**
		 * Destructs a perspective view.
		 */
		~GIPerspectiveView() override;

		/**
		 * (Re-)calculates the view projection matrix.
		 */
		void calculateProjectionMatrix() override;

	protected:

		/// Horizontal field of view in radian.
		Scalar viewFovX;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_PERSPECTIVE_VIEW_H
