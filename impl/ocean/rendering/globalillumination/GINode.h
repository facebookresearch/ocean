/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_NODE_H
#define META_OCEAN_RENDERING_GI_NODE_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GIFramebuffer.h"
#include "ocean/rendering/globalillumination/GIObject.h"
#include "ocean/rendering/globalillumination/GILightSource.h"
#include "ocean/rendering/globalillumination/RayIntersection.h"
#include "ocean/rendering/globalillumination/TracingGroup.h"

#include "ocean/rendering/Node.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class wraps a Global Illumination node object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GINode :
	virtual public GIObject,
	virtual public Node
{
	public:

		/**
		 * Returns whether the node is visible.
		 * @see Node::visible().
		 */
		bool visible() const override;

		/**
		 * Sets whether the node is visible.
		 * @see Node::setVisible().
		 */
		void setVisible(const bool visible) override;

		/**
		 * Builds the tracing object for this node and adds it to an already existing group of tracing objects.
		 * @param group The existing group of tracing objects to which the new tracing object will be added
		 * @param modelTransform The model transformation matrix for this node
		 * @param lightSources The light sources which will be used to light this node
		 */
		virtual void buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const LightSources& lightSources) const = 0;

	protected:

		/**
		 * Creates a new Global Illumination node object.
		 */
		GINode();

		/**
		 * Destructs a Global Illumination node object.
		 */
		~GINode() override;

	protected:

		/// Specifies whether this frame is visible.
		bool nodeVisible;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_NODE_H
