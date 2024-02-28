// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#ifndef FACEBOOK_NETWORK_VERTS_NODE_SPECIFICATION_H
#define FACEBOOK_NETWORK_VERTS_NODE_SPECIFICATION_H

#include "ocean/network/verts/Verts.h"

#include "ocean/base/Lock.h"

#include "ocean/math/Vector3.h"
#include "ocean/math/Quaternion.h"

// Forward declaration
struct verts_component;
struct verts_driver;
struct verts_field_set;

// Forward declaration.
enum class VertsFieldType : int;

namespace Ocean
{

namespace Network
{

namespace Verts
{

/**
 * This class holds the specification node types.
 * The specification of a node contains e.g., field names and field types but not the actual field values.<br>
 * Nodes are equivalent to VERTS components.
 * @ingroup networkverts
 */
class OCEAN_NETWORK_VERTS_EXPORT NodeSpecification
{
	friend class Driver;
	friend class Manager;
	friend class Node;
	friend class std::default_delete<NodeSpecification>;

	public:

		/**
		 * Definition of individual field types.
		 */
		enum FieldType : uint32_t
		{
			/// Unknown field type.
			FT_UNKNOWN = 0u,
			/// The field is a boolean.
			FT_BOOL,
			/// The field is a signed integer with 32 bit precision.
			FT_INT_32,
			/// The field is an unsigned integer with 64 bit precision.
			FT_UINT_64,
			/// The field is a float with 32 bit precision.
			FT_FLOAT_32,
			/// The field is a 3D vector with 32 bit precision.
			FT_VECTOR_32,
			/// The field is a 4D quaternion with 32 bit precision.
			FT_QUATERNION_32,
			/// The field is a string with 8 bit per character.
			FT_STRING,
			/// The field is a RPC (Remote procedure call) bradcasting to everyone.
			FT_RPC_BROADCAST,
			/// The field is a RPC (Remote procedure call) bradcasting to the owner.
			FT_RPC_OWNER,
			/// The field is a reference to an entity.
			FT_ENTITY_REFERENCE
		};

		/**
		 * Definition of a data type which can be used for string fields (as well as std::string).
		 */
		typedef std::vector<uint8_t> StringBuffer;

		/**
		 * The class is holding a pointer to a string (not owning the memory) which can be used for string fields (as well as std:string).
		 */
		class StringPointer
		{
			public:

				/**
				 * Creates an empty object not wrapping any memory.
				 */
				StringPointer() = default;

				/**
				 * Creates a new memory object.
				 * @param data The data of the memory, will not be copied
				 * @param size The size of the memory, in bytes, with range [0, infinity)
				 */
				inline StringPointer(const void* data, const size_t size);

				/**
				 * Returns the memory of this object.
				 * @return The object's memory, nullptr if no memory is wrapped
				 */
				inline const void* data() const;

				/**
				 * Returns the size of the memory wrapped in this object.
				 * @return The memory size in bytes, with range [0, infinity)
				 */
				inline size_t size() const;

				/**
				 * Returns whether this object holds valid data.
				 * @return True, if so
				 */
				explicit inline operator bool() const;

			protected:

				/// The memory, not owned.
				const void* data_ = nullptr;

				/// The size of the memory, in bytes.
				size_t size_ = 0;
		};

		/**
		 * Definition of a id identifying node specifications (VERTS field sets).
		 */
		typedef uint32_t NodeSpecificationId;

	protected:

		/**
		 * This class implemenets a manager for node specifications.
		 */
		class Manager : public Singleton<Manager>
		{
			friend class Singleton<Manager>;

			protected:

				/// Definition of an unordered map mapping names of node specifications to node specifications.
				typedef std::unordered_map<std::string, std::unique_ptr<NodeSpecification>> NodeSpecificationMap;

			public:

				/**
				 * Returns an existing node specification.
				 * @param name The name of the node specification, must be valid
				 * @return The specification of the node, nullptr if the specification does not exist
				 */
				const NodeSpecification* nodeSpecification(const std::string& name) const;

				/**
				 * Adds a new node specification to which fields can be registered afterwards.
				 * @param name The name of the new specification, must be valid
				 * @return The new node specification, or the existing specification in case the specification exists already
				 */
				NodeSpecification& newNodeSpecification(const std::string& name);

				/**
				 * Creates a new node specification for an existing VERTS field set.
				 * @param vertsDriver The VERTS driver to which the field set belongs, must be valid
				 * @param vertsComponent The VERTS component to which the field set belongs, must be valid
				 * @param vertsFieldSet The VERTS field set for which the new node specification will be created, must be valid
				 * @return The new node specification, nullptr if the specification could not be created
				 */
				const NodeSpecification* newNodeSpecification(verts_driver* vertsDriver, verts_component* vertsComponent, verts_field_set* vertsFieldSet);

				/**
				 * Returns whether a specific node specification exists.
				 * @param name The name of the node specification to check, must be valid
				 * @return True, if so
				 */
				inline bool hasNodeSpecification(const std::string& name) const;

				/**
				 * Registers all node specifications with a specified VERTS driver.
				 * @param vertsDriver The VERTS driver, must be valid
				 */
				void registerAllNodeSpecificationsWithDriver(verts_driver* vertsDriver);

				/**
				 * Releases all node specifications.
				 */
				void release();

			protected:

				/// The map mapping names to specifications.
				NodeSpecificationMap nodeSpecificationMap_;

				/// The mananger's lock.
				mutable Lock lock_;
		};

		/**
		 * This class holds the relevant information specifying a field.
		 */
		class FieldSpecification
		{
			public:

				/**
				 * Creates a new field specification.
				 * @param name The name of the field, must be valid
				 * @param fieldType The type of the field, must be valid
				 */
				inline FieldSpecification(std::string name, const FieldType fieldType);

			public:

				/// The field's name.
				std::string name_;

				/// The field's type.
				FieldType fieldType_ = FT_UNKNOWN;

				/// The index of the field in VERTS.
				uint16_t vertsFieldIndex_ = uint16_t(-1);
		};

		/**
		 * Definition of a pair combining the field's index with the field's type.
		 */
		typedef std::pair<Index32, FieldType> FieldPair;

		/**
		 * Definition of an unordered map mapping field names to field pairs.
		 */
		typedef std::unordered_map<std::string, FieldPair> FieldMap;

		/**
		 * Definition of a vector holding field specifications.
		 */
		typedef std::vector<FieldSpecification> FieldSpecifications;

		/**
		 * Definitionp of an unordered set holding VERTS drivers.
		 */
		typedef std::unordered_set<verts_driver*> VertsDriverSet;

		/// Definition of an internal node type holding user ids.
		static constexpr const char* internalNodeTypeSession_ = "_OCEAN_NODE_SESSION";

		/// Definition of an internal node type holding container identifier.
		static constexpr const char* internalNodeTypeContainer_ = "_OCEAN_NODE_CONTAINER";

	public:

		/**
		 * Returns the name of this node specification.
		 * @return The node's name
		 */
		inline const std::string& name() const;

		/**
		 * Registers a new field in this specification.
		 * The specification must not yet be finalized.
		 * @param fieldName The name of the field to register, must be valid
		 * @param fieldType The type of the field to register
		 * @return The index of the registered field if succeeded, invalidFieldIndex() otherwise
		 * @see isFinialized().
		 */
		Index32 registerField(std::string fieldName, const FieldType fieldType);

		/**
		 * Registers a new field in this specification.
		 * The specification must not yet be finalized.
		 * @param fieldName The name of the field to register, must be valid
		 * @return The index of the registered field if succeeded, invalidFieldIndex() otherwise
		 * @tparam T The data type of the field to register
		 * @see isFinialized().
		 */
		template <typename T>
		Index32 registerField(std::string fieldName);

		/**
		 * Returns the number of fields this node specification contains.
		 * @return The number of fields
		 */
		inline size_t fields() const;

		/**
		 * Retrns the field name of a specific field.
		 * @param fieldIndex The index of the field for which the name will be returned, with range [0, fields() - 1]
		 * @return The field's name
		 */
		inline std::string fieldName(const Index32 fieldIndex) const;

		/**
		 * Returns whether this node specification has a specific field.
		 * @param name The name of the field to check
		 * @param fieldType Optional field type to verify that the field also has a specific field type, FT_UNKNOWN if the field type is not of interest
		 * @return True, if so
		 */
		inline bool hasField(const std::string& name, const FieldType fieldType = FT_UNKNOWN) const;

		/**
		 * Returns the type of a specific field.
		 * @param name The name of the field for which the type will be returned, must be valid
		 * @return The field's type, FT_UNKNOWN if the field does not exist
		 */
		inline FieldType fieldType(const std::string& name) const;

		/**
		 * Returns the type of a specific field.
		 * @param fieldIndex The index of the field for which the type will be returned, with range [0, fields() - 1]
		 * @return The field's type, FT_UNKNOWN if the field does not exist
		 */
		inline FieldType fieldType(const Index32 fieldIndex) const;

		/**
		 * Returns the index of a specific field.
		 * @param name The name of the field for which the index will be returned
		 * @return The field's index, invalidFieldIndex() if the field does not exist
		 */
		inline Index32 fieldIndex(const std::string& name) const;

		/**
		 * Returns whether a specified field has a specific index and optional also a specific type.
		 * @param name The name of the field to check, must be valid
		 * @param fieldIndex The expected field index, with range [0, field() - 1]
		 * @param fieldType Optional field type to verify that the field also has a specific field type, FT_UNKNOWN if the field type is not of interest
		 * @return True, if so
		 */
		bool fieldHasIndex(const std::string& name, const Index32 fieldIndex, const FieldType fieldType = FT_UNKNOWN) const;

		/**
		 * Returns whether a specified field has a specific index and also a specific type.
		 * @param name The name of the field to check, must be valid
		 * @param fieldIndex The expected field index, with range [0, field() - 1]
		 * @return True, if so
		 * @tparam T The data type the field must have
		 */
		template <typename T>
		inline bool fieldHasIndex(const std::string& name, const Index32 fieldIndex) const;

		/**
		 * Returns whether this specification is finalized.
		 * The specification is finalized when the field has been registered with at least one driver.
		 * @return True, if so
		 */
		inline bool isFinalized() const;

		/**
		 * Returns an existing node specification.
		 * @param name The name of the node specification, must be valid
		 * @return The specification of the node, nullptr if the specification does not exist
		 */
		static inline const NodeSpecification* nodeSpecification(const std::string& name);

		/**
		 * Adds a new node specification to which fields can be registered afterwards.
		 * @param name The name of the new specification, must be valid
		 * @return The new node specification, or the existing specification in case the specification exists already
		 */
		static inline NodeSpecification& newNodeSpecification(const std::string& name);

		/**
		 * Returns whether a specific node specification exists.
		 * @param name The name of the node specification to check, must be valid
		 * @return True, if so
		 */
		static inline bool hasNodeSpecification(const std::string& name);

		/**
		 * Returns an invalid node specification id.
		 * @return Invalid node specification id
		 */
		static constexpr NodeSpecificationId invalidId();

		/**
		 * Returns an invalid index of a field.
		 * @return Invalid field index
		 */
		static constexpr Index32 invalidFieldIndex();

		/**
		 * Returns the field type value for a field type.
		 * @tparam T The data type of the field for which the value will be returned
		 * @return The field type value, FT_UNKNOWN if the data type is not supported
		 */
		template <typename T>
		static constexpr FieldType translateFieldType();

		/**
		 * Returns the VERTS field type for a given field type.
		 * @param fieldType The field type for which the VERTS field type will be returned
		 * @return The VERTS field type, -1 if unknown
		 */
		static VertsFieldType translateFieldType(const FieldType fieldType);

		/**
		 * Returns the field type for a given VERTS field type.
		 * @param vertsFieldType The VERTS field type for which the field type will be returned
		 * @return The field type, FT_UNKNOWN if unknown
		 */
		static FieldType translateFieldType(const VertsFieldType vertsFieldType);

	protected:

		/**
		 * Creates a new node specification with given name.
		 * @param name The name of the specification, must be valid
		 */
		explicit NodeSpecification(std::string name);

		/**
		 * Creates a new node specification for an existing VERTS field set.
		 * @param vertsDriver The VERTS driver to which the field set belongs, must be valid
		 * @param name The name of the specification, must be valid
		 * @param vertsComponent The VERTS component to which the field set belongs, must be valid
		 * @param vertsFieldSet The VERTS field set for which the new node specification will be created, must be valid
		 */
		NodeSpecification(verts_driver* vertsDriver, std::string name, verts_component* vertsComponent, verts_field_set* vertsFieldSet);

		/**
		 * Destructs a node specification.
		 */
		~NodeSpecification();

		/**
		 * Returns the VERTS field index of a given field index.
		 * @param fieldIndex The index of the field for which the corresponding VERTS field index will be returned, with range [0, fields() - 1]
		 * @param vertsFieldIndex The resulting VERTS field index
		 * @return True, if succeeded
		 */
		inline bool vertsFieldIndex(const Index32 fieldIndex, uint16_t& vertsFieldIndex) const;

		/**
		 * Registers this node specification with a specified VERTS driver.
		 * @param vertsDriver The VERTS driver, must be valid
		 */
		void registerWithDriver(verts_driver* vertsDriver) const;

		/**
		 * Registers all node specifications with a specified VERTS driver.
		 * @param vertsDriver The VERTS driver, must be valid
		 */
		static void registerAllNodeSpecificationsWithDriver(verts_driver* vertsDriver);

		/**
		 * Creates a new node specification for an existing VERTS field set.
		 * @param vertsDriver The VERTS driver to which the field set belongs, must be valid
		 * @param vertsComponent The VERTS component to which the field set belongs, must be valid
		 * @param vertsFieldSet The VERTS field set for which the new node specification will be created, must be valid
		 * @return The new node specification, nullptr if the specification could not be created
		 */
		static inline const NodeSpecification* newNodeSpecification(verts_driver* vertsDriver, verts_component* vertsComponent, verts_field_set* vertsFieldSet);

		/**
		 * Registers all internal node specifications.
		 * This should be done once VERTS is initialized.
		 */
		static void registerInternalNodeSpecifications();

		/**
		 * Returns whether a node is an internal node.
		 * @param nodeType The type of the node to check
		 * @return True, if so
		 */
		static bool isInternalNodeType(const std::string& nodeType);

		/**
		 * Disabled copy constructor.
		 * @param nodeSpecification The node specification which would be copied
		 */
		NodeSpecification(const NodeSpecification& nodeSpecification) = delete;

		/**
		 * Disabled copy operator.
		 * @param nodeSpecification The node specification which would be copied
		 * @return Reference to this object
		 */
		NodeSpecification& operator=(const NodeSpecification& nodeSpecification) = delete;

	protected:

		/// The node's name, e.g., describing the fields or the purpose of the node.
		std::string name_;

		/// The map mapping field names to field indices and types.
		FieldMap fieldMap_;

		/// The field specifications of all registered fields.
		mutable FieldSpecifications fieldSpecifications_;

		/// The corresponding VERTS field set.
		verts_field_set* vertsFieldSet_ = nullptr;

		/// The unique id of the specification (the id of the VERTS field set).
		NodeSpecificationId id_ = invalidId();

		/// True, if the specification is finalized.
		mutable bool isFinalized_ = false;

		/// The set containing all VERTS drivers to which this specification has been registered.
		mutable VertsDriverSet registeredVertsDrivers_;
};

inline NodeSpecification::StringPointer::StringPointer(const void* data, const size_t size) :
	data_(data),
	size_(size)
{
	// nothing to do here
}

inline const void* NodeSpecification::StringPointer::data() const
{
	return data_;
}

inline size_t NodeSpecification::StringPointer::size() const
{
	return size_;
}

inline NodeSpecification::StringPointer::operator bool() const
{
	return size_ != 0;
}

inline bool NodeSpecification::Manager::hasNodeSpecification(const std::string& name) const
{
	const ScopedLock scopedLock(lock_);

	return nodeSpecificationMap_.find(name) != nodeSpecificationMap_.cend();
}

inline NodeSpecification::FieldSpecification::FieldSpecification(std::string name, const FieldType fieldType) :
	name_(std::move(name)),
	fieldType_(fieldType)
{
	// nothing to do here
}

inline const std::string& NodeSpecification::name() const
{
	return name_;
}

template <typename T>
Index32 NodeSpecification::registerField(std::string fieldName)
{
	const FieldType fieldType = translateFieldType<T>();

	return registerField(std::move(fieldName), fieldType);
}

constexpr Index32 NodeSpecification::invalidFieldIndex()
{
	return Index32(-1);
}

inline size_t NodeSpecification::fields() const
{
	return fieldMap_.size();
}

inline std::string NodeSpecification::fieldName(const Index32 fieldIndex) const
{
	if (fieldIndex < fieldSpecifications_.size())
	{
		return fieldSpecifications_[fieldIndex].name_;
	}

	ocean_assert(false && "Invalid field index");
	return std::string();
}

inline bool NodeSpecification::hasField(const std::string& name, const FieldType fieldType) const
{
	const FieldMap::const_iterator iField = fieldMap_.find(name);

	if (iField == fieldMap_.cend())
	{
		return false;
	}

	const FieldPair& fieldPair = iField->second;

	return fieldType == FT_UNKNOWN || fieldType == fieldPair.second;
}

inline NodeSpecification::FieldType NodeSpecification::fieldType(const std::string& name) const
{
	const FieldMap::const_iterator iField = fieldMap_.find(name);

	if (iField == fieldMap_.cend())
	{
		ocean_assert(false && "Unknown field");
		return FT_UNKNOWN;
	}

	const FieldPair& fieldPair = iField->second;

	return fieldPair.second;
}

inline NodeSpecification::FieldType NodeSpecification::fieldType(const Index32 fieldIndex) const
{
	if (fieldIndex < fieldSpecifications_.size())
	{
		return fieldSpecifications_[fieldIndex].fieldType_;
	}

	ocean_assert(false && "Unknown field");
	return FT_UNKNOWN;
}

inline Index32 NodeSpecification::fieldIndex(const std::string& name) const
{
	const FieldMap::const_iterator iField = fieldMap_.find(name);

	if (iField == fieldMap_.cend())
	{
		ocean_assert(false && "Unknown field");
		return FT_UNKNOWN;
	}

	const FieldPair& fieldPair = iField->second;

	return fieldPair.first;
}

template <typename T>
inline bool NodeSpecification::fieldHasIndex(const std::string& name, const Index32 fieldIndex) const
{
	constexpr FieldType fieldType = translateFieldType<T>();

	return fieldHasIndex(name, fieldIndex, fieldType);
}

template <>
constexpr inline NodeSpecification::FieldType NodeSpecification::translateFieldType<bool>()
{
	return FT_BOOL;
}

template <>
constexpr inline NodeSpecification::FieldType NodeSpecification::translateFieldType<int32_t>()
{
	return FT_INT_32;
}

template <>
constexpr inline NodeSpecification::FieldType NodeSpecification::translateFieldType<uint64_t>()
{
	return FT_UINT_64;
}

template <>
constexpr inline NodeSpecification::FieldType NodeSpecification::translateFieldType<float>()
{
	return FT_FLOAT_32;
}

template <>
constexpr inline NodeSpecification::FieldType NodeSpecification::translateFieldType<VectorF3>()
{
	return FT_VECTOR_32;
}

template <>
constexpr inline NodeSpecification::FieldType NodeSpecification::translateFieldType<QuaternionF>()
{
	return FT_QUATERNION_32;
}

template <>
constexpr inline NodeSpecification::FieldType NodeSpecification::translateFieldType<std::string>()
{
	return FT_STRING;
}

template <>
constexpr NodeSpecification::FieldType NodeSpecification::translateFieldType<NodeSpecification::StringPointer>()
{
	return FT_STRING;
}

template <>
constexpr NodeSpecification::FieldType NodeSpecification::translateFieldType<NodeSpecification::StringBuffer>()
{
	return FT_STRING;
}

inline bool NodeSpecification::vertsFieldIndex(const Index32 fieldIndex, uint16_t& vertsFieldIndex) const
{
	if (fieldIndex < fieldSpecifications_.size())
	{
		vertsFieldIndex = fieldSpecifications_[fieldIndex].vertsFieldIndex_;
		ocean_assert(vertsFieldIndex != uint16_t(-1));

		return true;
	}

	return false;
}

inline bool NodeSpecification::isFinalized() const
{
	return isFinalized_;
}

constexpr NodeSpecification::NodeSpecificationId NodeSpecification::invalidId()
{
	return NodeSpecificationId(-1);
}

inline const NodeSpecification* NodeSpecification::nodeSpecification(const std::string& name)
{
	return Manager::get().nodeSpecification(name);
}

inline NodeSpecification& NodeSpecification::newNodeSpecification(const std::string& name)
{
	return Manager::get().newNodeSpecification(name);
}

inline const NodeSpecification* NodeSpecification::newNodeSpecification(verts_driver* vertsDriver, verts_component* vertsComponent, verts_field_set* vertsFieldSet)
{
	return Manager::get().newNodeSpecification(vertsDriver, vertsComponent, vertsFieldSet);
}

inline bool NodeSpecification::hasNodeSpecification(const std::string& name)
{
	return Manager::get().hasNodeSpecification(name);
}

}

}

}

#endif // FACEBOOK_NETWORK_VERTS_NODE_H
