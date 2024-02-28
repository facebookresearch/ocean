// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/SDXUpdateNode.h"
#include "ocean/scenedescription/Manager.h"

namespace Ocean
{

namespace SceneDescription
{

SDXUpdateNode::SDXUpdateNode(const SDXEnvironment* environment) :
	SDXNode(environment)
{
	Manager::get().registerUpdateNode(*this);
}

SDXUpdateNode::~SDXUpdateNode()
{
	Manager::get().unregisterUpdateNode(*this);
}

Timestamp SDXUpdateNode::onPreUpdate(const Rendering::ViewRef& /*view*/, const Timestamp timestamp)
{
	return timestamp;
}

void SDXUpdateNode::onUpdate(const Rendering::ViewRef& /*view*/, const Timestamp /*timestamp*/)
{
	// nothing to do here
}

}

}
