/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_NODE_H
#define META_OCEAN_SCENEDESCRIPTION_NODE_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Field.h"

#include "ocean/rendering/Engine.h"
#include "ocean/rendering/Node.h"

#include <map>

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration.
class Field;

// Forward declaration.
class Node;

/**
 * Definition of a scene description node reference with an internal reference counter.
 * @see Node.
 * @ingroup scenedescription
 */
typedef Ocean::ObjectRef<Node> NodeRef;

/**
 * Definition of a vector holding scene description node references.
 * @ingroup scenedescription
 */
typedef std::vector<NodeRef> NodeRefs;

/**
 * This class is the base class for all scene description nodes.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT Node
{
	friend class Ocean::ObjectRef<Node>;

	public:

		/**
		 * Definition of different field access types.
		 */
		enum FieldAccessType
		{
			/// No access possible, which can be a static field.
			ACCESS_NONE = 0,
			/// Read only field.
			ACCESS_GET = 1,
			/// Write only field.
			ACCESS_SET = 2,
			/// Read and write field.
			ACCESS_GET_SET = ACCESS_GET | ACCESS_SET,
			/// Field which will produce an explicit update event if it receives a new value.
			ACCESS_EXPLICIT_NOTIFICATION = 4
		};

	protected:

		/**
		 * This class implements a node specification object.
		 */
		class OCEAN_SCENEDESCRIPTION_EXPORT NodeSpecification
		{
			friend class Node;

			private:

				/**
				 * Definition of a field specification pair.
				 * The first parameter holds the field address offset.
				 * The second parameter holds the field access type.
				 */
				typedef std::pair<size_t, FieldAccessType> FieldPair;

				/**
				 * Definition of a map mapping field names to field address offsets.
				 */
				typedef std::map<std::string, FieldPair> FieldSpecificationMap;

			public:

				/**
				 * Creates a new node specification object.
				 * @param type Name of the node type.
				 */
				NodeSpecification(const std::string& type);

				/**
				 * Returns the type of this node.
				 * @return Node type
				 */
				inline const std::string& type() const;

				/**
				 * Returns the number registered fields.
				 * @return Field numbers
				 */
				inline unsigned int size() const;

				/**
				 * Returns whether the node specification holds a specific field.
				 * @param fieldName Name of the field to check
				 * @return True, if so
				 */
				bool hasField(const std::string& fieldName) const;

				/**
				 * Returns the name of the specific field.
				 * @param index Index of the field to return the name for
				 * @return Field name
				 * @exception Is thrown if the index is out of range
				 */
				const std::string& fieldName(const unsigned int index) const;

				/**
				 * Returns the access type of a specified field.
				 * @param fieldName Name of the field to check
				 * @return Field access type
				 * @exception Is thrown if the field does not exist
				 */
				FieldAccessType fieldAccessType(const std::string& fieldName) const;

				/**
				 * Returns a specific field of a specific node instance.
				 * @param objectAddress Address of the specific node instance holding the field to return
				 * @param fieldName Name of the field to be returned
				 * @return Node field
				 * @exception Is thrown if the field does not exist
				 */
				const Field& field(const size_t objectAddress, const std::string& fieldName) const;

				/**
				 * Returns a specific field of a specific node instance.
				 * @param objectAddress Address of the specific node instance holding the field to return
				 * @param fieldName Name of the field to be returned
				 * @return Node field
				 * @exception Is thrown if the field does not exist
				 */
				Field& field(const size_t objectAddress, const std::string& fieldName);

			protected:

				/**
				 * Registers a new field to a specific node type.
				 * @param objectAddress Address of an node which is used to determine the node type only
				 * @param fieldName Name of the field to register
				 * @param field Field to be register
				 * @param accessType Field access type of the field to register
				 */
				void registerField(const size_t objectAddress, const std::string& fieldName, const Field& field, const FieldAccessType accessType);

			private:

				/// Type of the node specified by this specification object.
				std::string type_;

				/// Map mapping field name to address offsets.
				FieldSpecificationMap fields_;
		};

		/**
		 * Definition of a map mapping field names to fields
		 */
		typedef std::map<std::string, Field*> FieldMap;

	public:

		/**
		 * Returns the scene description type of this node.
		 * @return Scene description type
		 */
		virtual DescriptionType descriptionType() const = 0;

		/**
		 * Returns the unique node id of this node.
		 * @return Unique node id
		 */
		inline NodeId id() const;

		/**
		 * Returns the name of this node.
		 * @return Node name
		 */
		inline const std::string& name() const;

		/**
		 * Returns the type of this node.
		 * @return Node type
		 */
		inline const std::string& type() const;

		/**
		 * Returns the field base of a specified (standard) field.
		 * @param fieldName Name of the field to return
		 * @return Field base of the specified field
		 * @exception Is thrown if the field does not exist
		 */
		const Field& field(const std::string& fieldName) const;

		/**
		 * Returns the field base of a specified (standard) field.
		 * Beware: Changing a field value using this function will not produce any field changing event functions!
		 * @param fieldName Name of the field to return
		 * @return Field base of the specified field
		 * @exception Is thrown if the field does not exist
		 */
		Field& field(const std::string& fieldName);

		/**
		 * Returns the field base of a specified standard or dynamic field.
		 * @param fieldName Name of the (standard or dynamic) field to return
		 * @return Field base of the specified field
		 * @exception Is thrown if the field does not exist
		 */
		virtual const Field& anyField(const std::string& fieldName) const;

		/**
		 * Returns the field base of a specified standard or dynamic field.
		 * Beware: Changing a field value using this function will not produce any field changing event functions!
		 * @param fieldName Name of the (standard or dynamic) field to return
		 * @return Field base of the specified field
		 * @exception Is thrown if the field does not exist
		 */
		virtual Field& anyField(const std::string& fieldName);

		/**
		 * Returns a specified (standard) field.
		 * @param fieldName Name of the field to return
		 * @return Specified field
		 * @exception Is thrown if the field does not exist
		 */
		template <typename T> const T& field(const std::string& fieldName) const;

		/**
		 * Returns a specified (standard) field.
		 * Beware: Changing a field value using this function will not produce any field changing event functions!
		 * @param fieldName Name of the field to return
		 * @return Specified field
		 * @exception Is thrown if the field does not exist
		 */
		template <typename T> T& field(const std::string& fieldName);

		/**
		 * Returns a specified standard or dynamic field.
		 * @param fieldName Name of the field to return
		 * @return Specified field
		 * @exception Is thrown if the field does not exist
		 */
		template <typename T> const T& anyField(const std::string& fieldName) const;

		/**
		 * Returns a specified standard or dynamic field.
		 * Beware: Changing a field value using this function will not produce any field changing event functions!
		 * @param fieldName Name of the field to return
		 * @return Specified field
		 * @exception Is thrown if the field does not exist
		 */
		template <typename T> T& anyField(const std::string& fieldName);

		/**
		 * Returns the access type of a specified field.
		 * @param fieldName Name of the field to check
		 * @return Field access type
		 * @exception Is thrown if the field does not exist
		 */
		FieldAccessType fieldAccessType(const std::string& fieldName) const;

		/**
		 * Sets the name of this node.
		 * @param name The name of this node to set
		 */
		virtual void setName(const std::string& name);

		/**
		 * Returns whether this node has a special (standard) field.
		 * @param fieldName Name of the field
		 * @return True, if so
		 */
		bool hasField(const std::string& fieldName) const;

		/**
		 * Returns whether this node has a special standard or dynamic field.
		 * @param fieldName Name of the field
		 * @return True, if so
		 */
		virtual bool hasAnyField(const std::string& fieldName) const;

		/**
		 * Tries to translate an alias field name to the original field name.
		 * @param fieldName Name of the alias field to return the original field name for
		 * @return Original field name if existent, otherwise the given aliasFieldName again
		 */
		virtual std::string originalFieldName(const std::string& fieldName) const;

		/**
		 * Returns the type of a special field.
		 * @param fieldName Name of the field
		 * @return Scalar of the specified field type
		 * @exception Is thrown if the field does not exist
		 */
		Field::Type fieldType(const std::string& fieldName) const;

		/**
		 * Return the dimension of a special field.
		 * @param fieldName Name of the field
		 * @return Dimension of the specified field
		 * @exception Is thrown if the field does not exist
		 */
		unsigned int fieldDimension(const std::string& fieldName) const;

		/**
		 * Returns whether this node can hold dynamic generated field.
		 * @return True, if so
		 */
		virtual bool isDynamic() const;

	protected:

		/**
		 * Creates a new node.
		 */
		Node();

		/**
		 * Disabled copy constructor.
		 * @param node Object which would be copied
		 */
		Node(const Node& node) = delete;

		/**
		 * Destructs a node.
		 */
		virtual ~Node();

		/**
		 * Returns the address of the most derived object.
		 * Each derived (not abstract) class must reimplement this function to guarantee a valid field mapping.
		 * @return Address of this object
		 */
		virtual size_t objectAddress() const;

		/**
		 * Registers a new field to a specified node type.
		 * @param specification Node specification receiving the new field type
		 * @param fieldName Name of the field  to register
		 * @param field Field pattern to register
		 * @param accessType Field access type of the field to register
		 */
		void registerField(NodeSpecification& specification, const std::string& fieldName, const Field& field, const FieldAccessType accessType = ACCESS_GET_SET);

		/**
		 * Disabled copy operator.
		 * @param node Object which would be copied
		 * @return Reference to this object
		 */
		Node& operator=(const Node& node) = delete;

		/**
		 * Returns the lock for the node id counter.
		 * @return Lock for the node id counter
		*/
		static Lock& nodeIdCounterLock();

	protected:

		/// Unique node id.
		NodeId nodeId_;

		/// Node name
		std::string name_;

		/**
		 * Pointer to the node specification, guaranteed to exist as long as the node exist.
		 * This pointer has to be set in derivated classes.
		 * Because it's not a pointer only the object is not disposed.
		 */
		NodeSpecification* specification_ = nullptr;

		/// Unique node id counter.
		static NodeId nodeIdCounter_;
};

inline const std::string& Node::NodeSpecification::type() const
{
	return type_;
}

inline unsigned int Node::NodeSpecification::size() const
{
	return (unsigned int)(fields_.size());
}

inline NodeId Node::id() const
{
	return nodeId_;
}

inline const std::string& Node::name() const
{
	return name_;
}

inline const std::string& Node::type() const
{
	ocean_assert(specification_);
	return specification_->type();
}

template <typename T>
const T& Node::field(const std::string& fieldName) const
{
	const Field& unspecificField(field(fieldName));

	ocean_assert(unspecificField.type() == T::fieldType);
	ocean_assert(unspecificField.dimension() == T::fieldDimension);

	return dynamic_cast<const T&>(unspecificField);
}

template <typename T>
T& Node::field(const std::string& fieldName)
{
	Field& unspecificField(field(fieldName));

	ocean_assert(unspecificField.type() == T::fieldType);
	ocean_assert(unspecificField.dimension() == T::fieldDimension);

	return dynamic_cast<T&>(unspecificField);
}

template <typename T>
const T& Node::anyField(const std::string& fieldName) const
{
	const Field& unspecificField(anyField(fieldName));

	ocean_assert(unspecificField.type() == T::fieldType);
	ocean_assert(unspecificField.dimension() == T::fieldDimension);

	return dynamic_cast<const T&>(unspecificField);
}

template <typename T>
T& Node::anyField(const std::string& fieldName)
{
	Field& unspecificField(anyField(fieldName));

	ocean_assert(unspecificField.type() == T::fieldType);
	ocean_assert(unspecificField.dimension() == T::fieldDimension);

	return dynamic_cast<T&>(unspecificField);
}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_NODE_H
