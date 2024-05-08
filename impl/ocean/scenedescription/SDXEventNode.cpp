/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

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
