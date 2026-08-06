/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_DYNAMIC_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_DYNAMIC_NODE_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Node.h"

#include <memory>
#include <vector>

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration.
class DynamicNode;

/**
 * Definition of a smart object reference for X scene description nodes.
 * @see DynamicNode, Node.
 * @ingroup scenedescription
 */
using DynamicNodeRef = SmartObjectRef<DynamicNode, Node>;

/**
 * This class is the base class for all nodes able to handle fields loaded during runtime.
 * A dynamic field is identified by its name. The index of a dynamic field is not an identity but an enumeration handle
 * with range [0, dynamicFields()), valid only until the next addField() or removeField() call.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT DynamicNode : virtual public Node
{
	protected:

		/**
		 * Definition of a vector holding fields.
		 */
		using DynamicFields = std::vector<std::unique_ptr<Field>>;

		/**
		 * Definition of a map mapping field names to field indices.
		 */
		using DynamicFieldIndices = std::unordered_map<std::string, unsigned int>;

	public:

		/**
		 * Adds a new field during runtime.
		 * The field is appended, so it receives the highest index and the indices of all existing fields stay valid.
		 * @param name The name of the field, must be valid
		 * @param field Field to add, a copy is stored
		 * @return True, if no field has be added with the same name before
		 */
		bool addField(const std::string& name, const Field& field);

		/**
		 * Returns the number of dynamic fields.
		 * Together with dynamicFieldName() this allows to enumerate all dynamic fields of this node.
		 * @return Number of dynamic fields
		 */
		inline unsigned int dynamicFields() const;

		/**
		 * Returns whether this node has a special standard or dynamic field.
		 * @see Node::hasAnyField().
		 */
		bool hasAnyField(const std::string& fieldName) const override;

		/**
		 * Returns whether this node holds a specific dynamic field.
		 * @param name The name of the dynamic field to check
		 * @return True, if so
		 */
		bool hasDynamicField(const std::string& name) const;

		/**
		 * Returns the name of a dynamic field by a given index.
		 * The index reflects the order in which the fields have been added, it is not stable across a removeField() call.
		 * @param index Index of the dynamic field to return, with range [0, dynamicFields())
		 * @return The name of the dynamic field
		 * @exception OceanException Is thrown if the index does not exist
		 */
		const std::string& dynamicFieldName(const unsigned int index) const;

		/**
		 * Returns the field base of a specified standard or dynamic field.
		 * @see Node::anyField().
		 */
		const Field& anyField(const std::string& fieldName) const override;

		/**
		 * Returns the field base of a specified standard or dynamic field.
		 * @see Node::anyField().
		 */
		Field& anyField(const std::string& fieldName) override;

		/**
		 * Returns the field base of a specified dynamic field.
		 * @param fieldName Name of the dynamic field to return
		 * @return Field base of the specified dynamic field
		 */
		const Field& dynamicField(const std::string& fieldName) const;

		/**
		 * Returns the field base of a specified dynamic field.
		 * Beware: Changing a field value using this function will not produce any field changing event functions!
		 * @param fieldName Name of the dynamic field to return
		 * @return Field base of the specified dynamic field
		 */
		Field& dynamicField(const std::string& fieldName);

		/**
		 * Returns a specified dynamic field.
		 * @param fieldName Name of the dynamic field to return
		 */
		template <typename T> const T& dynamicField(const std::string& fieldName) const;

		/**
		 * Returns a specified dynamic field.
		 * Beware: Changing a field value using this function will not produce any field changing event functions!
		 * @param fieldName Name of the dynamic field to return
		 */
		template <typename T> T& dynamicField(const std::string& fieldName);

		/**
		 * Removes a field added during runtime.
		 * The remaining fields keep their relative order but are re-indexed, so every index above the removed one is invalidated.
		 * @param name The name of the field to remove, must be valid
		 * @return True, if the field could be remove
		 */
		bool removeField(const std::string& name);

		/**
		 * Returns whether this node can hold dynamic generated field.
		 * @see Node::isDynamic().
		 */
		bool isDynamic() const override;

	protected:

		/**
		 * Creates a new dynamic node object.
		 */
		DynamicNode();

		/**
		 * Destructs a dynamic node object.
		 */
		~DynamicNode() override;

	protected:

		/// Vector holding the dynamic fields.
		DynamicFields dynamicFields_;

		/// Map mapping names of dynamic fields to indices.
		DynamicFieldIndices dynamicFieldIndices_;
};

inline unsigned int DynamicNode::dynamicFields() const
{
	return (unsigned int)(dynamicFields_.size());
}

template <typename T> const T& DynamicNode::dynamicField(const std::string& fieldName) const
{
	const DynamicFieldIndices::const_iterator i = dynamicFieldIndices_.find(fieldName);
	if (i == dynamicFieldIndices_.end())
	{
		throw OceanException("Invalid dynamic field name.");
	}

	ocean_assert(i->second < dynamicFields_.size());
	return Field::cast<const T&>(*dynamicFields_[i->second]);
}

template <typename T> T& DynamicNode::dynamicField(const std::string& fieldName)
{
	const DynamicFieldIndices::const_iterator i = dynamicFieldIndices_.find(fieldName);
	if (i == dynamicFieldIndices_.end())
	{
		throw OceanException("Invalid dynamic field name.");
	}

	ocean_assert(i->second < dynamicFields_.size());
	return Field::cast<T&>(*dynamicFields_[i->second]);
}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_DYNAMIC_NODE_H
