/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GLES_LOD_H
#define META_OCEAN_RENDERING_GLES_LOD_H

#include "ocean/rendering/glescenegraph/GLESceneGraph.h"
#include "ocean/rendering/glescenegraph/GLESGroup.h"

#include "ocean/rendering/LOD.h"

namespace Ocean
{

namespace Rendering
{

namespace GLESceneGraph
{

/**
 * This class implements a GLESceneGraph group object.
 * @ingroup renderinggles
 */
class OCEAN_RENDERING_GLES_EXPORT GLESLOD :
	virtual public GLESGroup,
	virtual public LOD
{
	friend class GLESFactory;

	public:

		/**
		 * Returns the n+1 distance ranges for n child nodes.
		 * @see LOD::distanceRanges().
		 */
		DistanceRanges distanceRanges() const override;

		/**
		 * Sets the n+1 distance ranges for n child nodes.
		 * @see LOD::setDistanceRanges().
		 */
		void setDistanceRanges(const DistanceRanges& ranges) override;

		/**
		 * Adds this node and all child node to a traverser.
		 * @see GLESNode::addToTraverser().
		 */
		void addToTraverser(const GLESFramebuffer& framebuffer, const SquareMatrix4& projectionMatrix, const HomogenousMatrix4& camera_T_object, const Lights& lights, GLESTraverser& traverser) const override;

	protected:

		/**
		 * Creates a new GLESceneGraph group object.
		 */
		GLESLOD();

		/**
		 * Destructs a GLESceneGraph group object.
		 */
		~GLESLOD() override;

	protected:

		/// The distance ranges for the individual nodes, n+1 ranges for n childs.
		DistanceRanges distanceRanges_;
};

}

}

}

#endif // META_OCEAN_RENDERING_GLES_LOD_H
