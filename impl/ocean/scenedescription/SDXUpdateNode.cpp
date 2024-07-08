/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
