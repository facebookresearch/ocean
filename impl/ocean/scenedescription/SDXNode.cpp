/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/SDXNode.h"
#include "ocean/scenedescription/NodeRefManager.h"

namespace Ocean
{

namespace SceneDescription
{

SDXNode::SDXNode(const SDXEnvironment* environment) :
	environment_(environment)
{
	// nothing to do here
}

SDXNode::~SDXNode()
{
	if (parents_.empty() == false)
	{
		ocean_assert(false && "Should be empty");
	}
}

DescriptionType SDXNode::descriptionType() const
{
	return TYPE_PERMANENT;
}

const Rendering::ObjectRef& SDXNode::renderingObject() const
{
	return renderingObject_;
}

void SDXNode::setName(const std::string& name)
{
	const ScopedLock scopedLock(lock_);

	if (name == name_)
	{
		return;
	}

	ocean_assert(environment_);
	environment_->library()->nodeManager().changeRegisteredNode(id(), name_, name);

	if (renderingObject_)
	{
		renderingObject_->setName(name);
	}

	name_ = name;
}

bool SDXNode::setField(const std::string& fieldName, const Field& field)
{
	const ScopedLock scopedLock(lock_);

	if (!SDXNode::field(fieldName).assign(field))
	{
		return false;
	}

	if (initialized_)
	{
		onFieldChanged(fieldName);
	}

	return true;
}

bool SDXNode::setAnyField(const std::string& fieldName, const Field& field)
{
	const ScopedLock scopedLock(lock_);

	return setField(fieldName, field);
}

NodeRefs SDXNode::parentNodes() const
{
	ocean_assert(environment_);
	ocean_assert(environment_->library());

	const ScopedLock scopedLock(lock_);

	NodeRefs nodes;

	for (NodeIdMap::const_iterator i = parents_.begin(); i != parents_.end(); ++i)
	{
		NodeRef parent(environment_->library()->nodeManager().node(i->first));

		if (parent)
		{
			nodes.emplace_back(std::move(parent));
		}
	}

	return nodes;
}

SDXNodeSet SDXNode::ancestorNodes() const
{
	ocean_assert(environment_);
	ocean_assert(environment_->library());

	const ScopedLock scopedLock(lock_);

	SDXNodeSet nodes;

	for (NodeIdMap::const_iterator i = parents_.begin(); i != parents_.end(); ++i)
	{
		const SDXNodeRef parent(environment_->library()->nodeManager().node(i->first));

		if (parent)
		{
			const SDXNodeSet ancestor(parent->ancestorNodes());
			nodes.insert(ancestor.begin(), ancestor.end());
		}
	}

	return nodes;
}

void SDXNode::initialize(const Rendering::SceneRef& scene, const Timestamp timestamp, const bool reinitialize)
{
	if (initialized_ == false || reinitialize)
	{
		onInitialize(scene, timestamp);
		initialized_ = true;
	}
}

void SDXNode::onInitialize(const Rendering::SceneRef& /*scene*/, const Timestamp /*timestamp*/)
{
	// nothing to do here
}

void SDXNode::onFieldChanged(const std::string& /*fieldName*/)
{
	// this function should be implemented in derived classes
}

void SDXNode::registerThisNodeAsParent(const SDXNodeRef& child)
{
	if (child)
	{
		child->registerParentNode(id());
	}
}

void SDXNode::unregisterThisNodeAsParent(const SDXNodeRef& child)
{
	if (child)
	{
		child->unregisterParentNode(id());
	}
}

}

}
