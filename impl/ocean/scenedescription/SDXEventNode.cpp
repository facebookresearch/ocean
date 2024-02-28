// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "ocean/scenedescription/SDXEventNode.h"
#include "ocean/scenedescription/Manager.h"

namespace Ocean
{

namespace SceneDescription
{

SDXEventNode::SDXEventNode(const SDXEnvironment* environment) :
	SDXNode(environment)
{
	Manager::get().registerEventNode(*this);
}

SDXEventNode::~SDXEventNode()
{
	Manager::get().unregisterEventNode(*this);
}

}

}
