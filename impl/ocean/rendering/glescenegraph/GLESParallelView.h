/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_PARALLEL_VIEW_H
#define META_OCEAN_RENDERING_GLES_PARALLEL_VIEW_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESView.h"

#include "ocean/rendering/ParallelView.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a parallel (orthographic) view for GLESceneGraph.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESParallelView :
	virtual public GLESView,
	virtual public ParallelView
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the width of the orthographic viewing box in world units.
		 * @return The view width in world units
		 */
		Scalar width() const override;

		/**
		 * Sets the width of the orthographic viewing box in world units.
		 * The height is automatically derived from the width and aspect ratio.
		 * @param width The view width in world units, with range (0, infinity)
		 * @return True if succeeded
		 */
		bool setWidth(const Scalar width) override;

		/**
		 * Returns a viewing ray into the scene.
		 * For parallel projection, all rays have the same direction (parallel to camera's forward axis).
		 * @see View::viewingRay().
		 */
		Line3 viewingRay(const Scalar x, const Scalar y, const unsigned int width, const unsigned int height) const override;

	protected:

		/**
		 * Creates a new parallel view.
		 */
		GLESParallelView();

		/**
		 * Destructs a parallel view.
		 */
		~GLESParallelView() override;

		/**
		 * (Re-)calculates the view projection matrix using orthographic projection.
		 * @see GLESView::calculateProjectionMatrix().
		 */
		bool calculateProjectionMatrix() override;

	protected:

		/// The view's width in world units, with range (0, infinity)
		Scalar width_ = 0;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_PARALLEL_VIEW_H
