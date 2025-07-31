/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/SDXDynamicNode.h"

namespace Ocean
{

namespace SceneDescription
{

SDXDynamicNode::SDXDynamicNode(const SDXEnvironment* environment) :
	SDXNode(environment)
{
	// nothing to do here
}

SDXDynamicNode::~SDXDynamicNode()
{
	// nothing to do here
}

bool SDXDynamicNode::setDynamicField(const std::string& fieldName, const Field& field)
{
	if (!dynamicField(fieldName).assign(field))
		return false;

	if (initialized_)
		onFieldChanged(fieldName);

	return true;
}

bool SDXDynamicNode::setAnyField(const std::string& fieldName, const Field& field)
{
	if (hasField(fieldName))
	{
		if (!Node::field(fieldName).assign(field))
			return false;
	}
	else if (hasDynamicField(fieldName))
	{
		if (!DynamicNode::dynamicField(fieldName).assign(field))
			return false;
	}
	else
		return false;

	if (initialized_)
		onFieldChanged(fieldName);

	return true;
}

}

}
