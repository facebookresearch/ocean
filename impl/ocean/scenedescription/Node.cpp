/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/Node.h"

namespace Ocean
{

namespace SceneDescription
{

NodeId Node::nodeIdCounter_(0);

Lock& Node::nodeIdCounterLock()
{
	static Lock nodeIdCounterLock;
	return nodeIdCounterLock;
}

Node::NodeSpecification::NodeSpecification(const std::string& type) :
	type_(type)
{
	// nothing to do here
}

bool Node::NodeSpecification::hasField(const std::string& fieldName) const
{
	return fields_.find(fieldName) != fields_.end();
}

const std::string& Node::NodeSpecification::fieldName(const unsigned int index) const
{
	ocean_assert(index < fields_.size());

	size_t n = 0;

	for (FieldSpecificationMap::const_iterator i = fields_.begin(); i != fields_.end(); ++i, ++n)
	{
		if (n == index)
		{
			return i->first;
		}
	}

	ocean_assert(false && "Invalid field index.");
	throw OceanException("Invalid field index.");
}

Node::FieldAccessType Node::NodeSpecification::fieldAccessType(const std::string& fieldName) const
{
	FieldSpecificationMap::const_iterator i = fields_.find(fieldName);
	if (i == fields_.end())
	{
		throw OceanException("Invalid field name.");
	}

	return i->second.second;
}

const Field& Node::NodeSpecification::field(const size_t objectAddress, const std::string& fieldName) const
{
	const FieldSpecificationMap::const_iterator i = fields_.find(fieldName);
	if (i == fields_.end())
	{
		ocean_assert(false && "Invalid field name.");
		throw OceanException("Invalid field name.");
	}

	const size_t fieldAddress = objectAddress + i->second.first;
	const Field* fieldPointer = reinterpret_cast<const Field*>(fieldAddress);
	ocean_assert(fieldPointer != nullptr);

	return *fieldPointer;
}

Field& Node::NodeSpecification::field(const size_t objectAddress, const std::string& fieldName)
{
	const FieldSpecificationMap::iterator i = fields_.find(fieldName);
	if (i == fields_.end())
	{
		ocean_assert(false && "Invalid field name.");
		throw OceanException("Invalid field name.");
	}

	const size_t fieldAddress = objectAddress + i->second.first;
	Field* fieldPointer = reinterpret_cast<Field*>(fieldAddress);
	ocean_assert(fieldPointer != nullptr);

	return *fieldPointer;
}

void Node::NodeSpecification::registerField(const size_t objectAddress, const std::string& fieldName, const Field& field, const FieldAccessType accessType)
{
	if (fields_.find(fieldName) != fields_.end())
	{
		return;
	}

	ocean_assert((size_t)&field >= objectAddress);

	const size_t specificationFieldOffset((size_t)&field - objectAddress);
	fields_[fieldName] = FieldPair(specificationFieldOffset, accessType);
}

Node::Node() :
	nodeId_(invalidNodeId),
	specification_(nullptr)
{
	const ScopedLock scopedLock(nodeIdCounterLock());
	nodeId_ = ++nodeIdCounter_;
}

Node::~Node()
{
	// nothing to do here
}

Node::FieldAccessType Node::fieldAccessType(const std::string& fieldName) const
{
	ocean_assert(specification_ != nullptr);
	return specification_->fieldAccessType(fieldName);
}

void Node::setName(const std::string& name)
{
	name_ = name;
}

bool Node::hasField(const std::string& fieldName) const
{
	ocean_assert(specification_ != nullptr);
	return specification_->hasField(fieldName);
}

bool Node::hasAnyField(const std::string& fieldName) const
{
	ocean_assert(specification_ != nullptr);
	return specification_->hasField(fieldName);
}

std::string Node::originalFieldName(const std::string& fieldName) const
{
	return fieldName;
}

Field::Type Node::fieldType(const std::string& fieldName) const
{
	return field(fieldName).type();
}

unsigned int Node::fieldDimension(const std::string& fieldName) const
{
	return field(fieldName).dimension();
}

bool Node::isDynamic() const
{
	return false;
}

const Field& Node::field(const std::string& fieldName) const
{
	ocean_assert(specification_);
	return specification_->field(objectAddress(), fieldName);
}

Field& Node::field(const std::string& fieldName)
{
	ocean_assert(specification_);
	return specification_->field(objectAddress(), fieldName);
}

const Field& Node::anyField(const std::string& fieldName) const
{
	ocean_assert(specification_);
	return specification_->field(objectAddress(), fieldName);
}

Field& Node::anyField(const std::string& fieldName)
{
	ocean_assert(specification_);
	return specification_->field(objectAddress(), fieldName);
}

size_t Node::objectAddress() const
{
	ocean_assert(false && "Has to be implemented in each derivated class.");
	return size_t(this);
}

void Node::registerField(NodeSpecification& specification, const std::string& fieldName, const Field& field, const FieldAccessType accessType)
{
	return specification.registerField(objectAddress(), fieldName, field, accessType);
}

}

}
