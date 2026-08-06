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
	// nothing to do here
}

bool DynamicNode::isDynamic() const
{
	return true;
}

bool DynamicNode::addField(const std::string& name, const Field& field)
{
	ocean_assert(!name.empty());

	if (dynamicFieldIndices_.contains(name))
	{
		return false;
	}

	dynamicFieldIndices_.emplace(name, (unsigned int)(dynamicFields_.size()));
	dynamicFields_.emplace_back(field.copy());

	return true;
}

bool DynamicNode::hasAnyField(const std::string& fieldName) const
{
	ocean_assert(specification_ != nullptr);
	return specification_->hasField(fieldName) || dynamicFieldIndices_.contains(fieldName);
}

bool DynamicNode::hasDynamicField(const std::string& name) const
{
	return dynamicFieldIndices_.contains(name);
}

const std::string& DynamicNode::dynamicFieldName(const unsigned int index) const
{
	if (index >= dynamicFields_.size())
	{
		throw OceanException("Invalid dynamic field index.");
	}

	for (const DynamicFieldIndices::value_type& fieldIndexPair : dynamicFieldIndices_)
	{
		if (fieldIndexPair.second == index)
		{
			return fieldIndexPair.first;
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
	const DynamicFieldIndices::const_iterator iField = dynamicFieldIndices_.find(fieldName);
	if (iField == dynamicFieldIndices_.cend())
	{
		throw OceanException("Invalid dynamic field name.");
	}

	ocean_assert(iField->second < dynamicFields_.size());
	return *dynamicFields_[iField->second];
}

Field& DynamicNode::dynamicField(const std::string& fieldName)
{
	const DynamicFieldIndices::const_iterator iField = dynamicFieldIndices_.find(fieldName);
	if (iField == dynamicFieldIndices_.cend())
	{
		throw OceanException("Invalid dynamic field name.");
	}

	ocean_assert(iField->second < dynamicFields_.size());
	return *dynamicFields_[iField->second];
}

bool DynamicNode::removeField(const std::string& name)
{
	ocean_assert(!name.empty());

	const DynamicFieldIndices::const_iterator iField = dynamicFieldIndices_.find(name);
	if (iField == dynamicFieldIndices_.cend())
	{
		return false;
	}

	const unsigned int index = iField->second;
	ocean_assert(index < dynamicFields_.size());

	dynamicFields_.erase(dynamicFields_.begin() + index);

	dynamicFieldIndices_.erase(iField);

	for (DynamicFieldIndices::value_type& fieldIndexPair : dynamicFieldIndices_)
	{
		if (fieldIndexPair.second > index)
		{
			--fieldIndexPair.second;
		}
	}

	return true;
}

}

}
