/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_PERSPECTIVE_VIEW_H
#define META_OCEAN_RENDERING_GLES_PERSPECTIVE_VIEW_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESView.h"

#include "ocean/rendering/PerspectiveView.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a perspective view for GLESceneGraph.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESPerspectiveView :
	virtual public GLESView,
	virtual public PerspectiveView
{
	friend class GLESFactory;

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

		/**
		 * Sets the projection matrix explicitly.
		 * @see PerspectiveView::setProjectionMatrix().
		 */
		void setProjectionMatrix(const SquareMatrix4& clip_T_view) override;

	protected:

		/**
		 * Creates a new perspective view.
		 */
		GLESPerspectiveView();

		/**
		 * Destructs a perspective view.
		 */
		~GLESPerspectiveView() override;

		/**
		 * (Re-)calculates the view projection matrix.
		 * @see GLESView::calculateProjectionMatrix().
		 */
		bool calculateProjectionMatrix() override;

	protected:

		/// The view's horizontal field of view in radian, with range (0, PI)
		Scalar fovX_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_PERSPECTIVE_VIEW_H
