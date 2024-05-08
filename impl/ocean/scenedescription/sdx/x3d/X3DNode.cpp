/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/sdx/x3d/X3DNode.h"

namespace Ocean
{

namespace SceneDescription
{

namespace SDX
{

namespace X3D
{

X3DNode::X3DNode(const SDXEnvironment* environment) :
	SDXNode(environment)
{
	// nothing to do here
}

void X3DNode::registerFields(NodeSpecification& specification)
{
	registerField(specification, "metadata", metadata_, ACCESS_GET_SET);
}

bool X3DNode::setField(const std::string& fieldName, const Field& newField)
{
	ocean_assert(specification_);

	if (initialized_)
	{
		const FieldAccessType accessType(fieldAccessType(fieldName));
		if (accessType & ACCESS_SET)
		{
			Field& localField = SDXNode::field(fieldName);

			if (accessType & ACCESS_EXPLICIT_NOTIFICATION)
			{
				if (!onFieldChanging(fieldName, newField))
				{
					return false;
				}
			}
			else
			{
				if (!localField.assign(newField))
				{
					return false;
				}

				onFieldChanged(fieldName);
			}

			if (accessType & ACCESS_GET)
			{
				forwardThatFieldHasBeenChanged(fieldName, localField);
			}

			return true;
		}

		return false;
	}

	return field(fieldName).assign(newField);
}

bool X3DNode::addConnection(const std::string& outputField, const NodeId inputNode, const std::string& inputField)
{
	const std::pair<FieldConnectionMap::const_iterator, FieldConnectionMap::const_iterator> range(fieldConnections_.equal_range(outputField));

	for (FieldConnectionMap::const_iterator i = range.first; i != range.second; ++i)
	{
		if (i->second.first == inputNode && i->second.second == inputField)
		{
			return false;
		}
	}

	fieldConnections_.insert(std::make_pair(outputField, ReceiverPair(inputNode, inputField)));
	return true;
}

std::string X3DNode::originalFieldName(const std::string& fieldName) const
{
	if (!specification_->hasField(fieldName))
	{
		if (fieldName.find("set_") == 0)
		{
			const std::string name = fieldName.substr(4u);

			if (specification_->hasField(name))
			{
				return name;
			}
		}
		else if (int(fieldName.find("_changed")) == int(fieldName.length() - 8))
		{
			const std::string name = fieldName.substr(0, fieldName.length() - 8);

			if (specification_->hasField(name))
			{
				return name;
			}
		}
	}

	return fieldName;
}

bool X3DNode::onFieldChanging(const std::string& /*fieldName*/, const Field& /*field*/)
{
	return false;
}

void X3DNode::forwardThatFieldHasBeenChanged(const std::string& fieldName, const Field& field)
{
	ocean_assert(&X3DNode::field(fieldName) == &field);
	ocean_assert(fieldAccessType(fieldName) & ACCESS_GET);

	const std::pair<FieldConnectionMap::const_iterator, FieldConnectionMap::const_iterator> range(fieldConnections_.equal_range(fieldName));

	for (FieldConnectionMap::const_iterator i = range.first; i != range.second; ++i)
	{
		SDXNodeRef sdxNode(environment_->library()->nodeManager().node(i->second.first));

		if (sdxNode)
		{
			sdxNode->setField(i->second.second, field);
		}
	}
}

}

}

}

}
