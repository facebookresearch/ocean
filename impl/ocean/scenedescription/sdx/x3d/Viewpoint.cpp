/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/Viewpoint.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

Viewpoint::Viewpoint(const SDXEnvironment* environment) :
	SDXNode(environment),
	X3DNode(environment),
	X3DChildNode(environment),
	X3DBindableNode(environment),
	X3DViewpointNode(environment),
	SDXUpdateNode(environment),
	fieldOfView_(Numeric::pi_4())
{
	static NodeSpecification specification(specifyNode());
	specification_ = &specification;
}

Viewpoint::NodeSpecification Viewpoint::specifyNode()
{
	NodeSpecification specification("Viewpoint");

	registerField(specification, "fieldOfView", fieldOfView_, ACCESS_GET_SET);

	X3DViewpointNode::registerFields(specification);

	return specification;
}

size_t Viewpoint::objectAddress() const
{
	return size_t(this);
}

Timestamp Viewpoint::onPreUpdate(const Rendering::ViewRef& view, const Timestamp timestamp)
{
	ocean_assert(view);

	const ScopedLock scopedLock(lock_);

	const NodeRefs parents(parentNodes());

	for (NodeRefs::const_iterator i = parents.begin(); i != parents.end(); ++i)
	{
		SDXNodeRef sdxNode(*i);
		ocean_assert(sdxNode);

		const Rendering::NodeRef renderingNode(sdxNode->renderingObject());

		if (renderingNode)
		{
			const HomogenousMatrices4 transformations(renderingNode->worldTransformations());

			if (transformations.empty())
			{
				continue;
			}

			view->setTransformation(transformations.front() * HomogenousMatrix4(position_.value(), orientation_.value()));
			break;
		}
	}

	return timestamp;
}

}

}

}

}
