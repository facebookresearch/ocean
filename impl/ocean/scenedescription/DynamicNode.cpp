/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/scenedescription/DynamicNode.h"

namespace Ocean
{

namespace SceneDescription
{

DynamicNode::DynamicNode() :
	Node()
{
	// nothing to do here
}

DynamicNode::~DynamicNode()
{
	for (DynamicFields::iterator i = dynamicFields_.begin(); i != dynamicFields_.end(); ++i)
		delete *i;
}

bool DynamicNode::isDynamic() const
{
	return true;
}

bool DynamicNode::addField(const std::string& name, const Field& field)
{
	ocean_assert(name.empty() == false);

	if (dynamicFieldIndices_.find(name) != dynamicFieldIndices_.end())
		return false;

	Field* newField = field.copy();

	dynamicFieldIndices_[name] = (unsigned int)(dynamicFields_.size());
	dynamicFields_.push_back(newField);

	return true;
}

bool DynamicNode::hasAnyField(const std::string& fieldName) const
{
	ocean_assert(specification_ != nullptr);
	return specification_->hasField(fieldName) || dynamicFieldIndices_.find(fieldName) != dynamicFieldIndices_.end();
}

bool DynamicNode::hasDynamicField(const std::string& name) const
{
	return dynamicFieldIndices_.find(name) != dynamicFieldIndices_.end();
}

const std::string& DynamicNode::dynamicFieldName(const unsigned int index) const
{
	if (index >= dynamicFields_.size())
	{
		throw OceanException("Invalid dynamic field index.");
	}

	for (DynamicFieldIndices::const_iterator i = dynamicFieldIndices_.begin(); i != dynamicFieldIndices_.end(); ++i)
	{
		if (i->second == index)
		{
			return i->first;
		}
	}

	ocean_assert(false && "This should never happen.");
	throw OceanException("This should never happen.");
}

const Field& DynamicNode::anyField(const std::string& fieldName) const
{
	ocean_assert(specification_ != nullptr);

	if (specification_->hasField(fieldName))
	{
		return Node::field(fieldName);
	}

	return dynamicField(fieldName);
}

Field& DynamicNode::anyField(const std::string& fieldName)
{
	ocean_assert(specification_ != nullptr);

	if (specification_->hasField(fieldName))
	{
		return Node::field(fieldName);
	}

	return dynamicField(fieldName);
}

const Field& DynamicNode::dynamicField(const std::string& fieldName) const
{
	const DynamicFieldIndices::const_iterator i = dynamicFieldIndices_.find(fieldName);
	if (i == dynamicFieldIndices_.end())
		throw OceanException("Invalid dynamic field name.");

	ocean_assert(i->second < dynamicFields_.size());
	return *dynamicFields_[i->second];
}

Field& DynamicNode::dynamicField(const std::string& fieldName)
{
	const DynamicFieldIndices::const_iterator i = dynamicFieldIndices_.find(fieldName);
	if (i == dynamicFieldIndices_.end())
		throw OceanException("Invalid dynamic field name.");

	ocean_assert(i->second < dynamicFields_.size());
	return *dynamicFields_[i->second];
}

bool DynamicNode::removeField(const std::string& name)
{
	ocean_assert(name.empty() == false);

	DynamicFieldIndices::iterator i = dynamicFieldIndices_.find(name);
	if (i == dynamicFieldIndices_.end())
		return false;

	DynamicFields::iterator iD = dynamicFields_.begin();
	iD += i->second;

	delete *iD;
	dynamicFields_.erase(iD);
	return true;
}

}

}
