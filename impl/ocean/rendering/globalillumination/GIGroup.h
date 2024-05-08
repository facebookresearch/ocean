/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_RENDERING_GI_GROUP_H
#define META_OCEAN_RENDERING_GI_GROUP_H

#include "ocean/rendering/globalillumination/GlobalIllumination.h"
#include "ocean/rendering/globalillumination/GINode.h"

#include "ocean/rendering/Group.h"

namespace Ocean
{

namespace Rendering
{

namespace GlobalIllumination
{

/**
 * This class wraps a Global Illumination group object.
 * @ingroup renderinggi
 */
class OCEAN_RENDERING_GI_EXPORT GIGroup :
	virtual public GINode,
	virtual public Group
{
	friend class GIFactory;

	public:

		/**
		 * Adds a new child node to this group.
		 * @see Group::addChild().
		 */
		void addChild(const NodeRef& node) override;

		/**
		 * Registers a light lighting all childs of this group node.
		 * @see Group::registerLight().
		 */
		void registerLight(const LightSourceRef& light) override;

		/**
		 * Removes a child node connected to this group.
		 * @see Group::removeChild().
		 */
		void removeChild(const NodeRef& node) override;

		/**
		 * Unregisters a light.
		 * @see Group::unregisterLight().
		 */
		void unregisterLight(const LightSourceRef& light) override;

		/**
		 * Returns the bounding box of this node.
		 * @see Node::boundingBox().
		 */
		BoundingBox boundingBox(const bool involveLocalTransformation) const override;

		/**
		 * Removes all cgukd nodes connected to this group.
		 * @see Group::clear().
		 */
		void clear() override;

		/**
		 * Builds the tracing object for this node and adds it to an already existing group of tracing objects.
		 * @see GINode::buildTracing().
		 */
		void buildTracing(TracingGroup& group, const HomogenousMatrix4& modelTransform, const LightSources& lightSources) const override;

	protected:

		/**
		 * Creates a new Global Illumination group object.
		 */
		GIGroup();

		/**
		 * Destructs a Global Illumination group object.
		 */
		~GIGroup() override;
};

}

}

}

#endif // META_OCEAN_RENDERING_GI_GROUP_H
