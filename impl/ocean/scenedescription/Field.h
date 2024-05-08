/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_FIELD_H
#define META_OCEAN_SCENEDESCRIPTION_FIELD_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Node.h"

#include "ocean/base/ObjectRef.h"
#include "ocean/base/Timestamp.h"

#include "ocean/math/RGBAColor.h"
#include "ocean/math/Rotation.h"
#include "ocean/math/SquareMatrix3.h"
#include "ocean/math/SquareMatrix4.h"
#include "ocean/math/Vector2.h"
#include "ocean/math/Vector3.h"
#include "ocean/math/Vector4.h"

namespace Ocean
{

namespace SceneDescription
{

/**
 * This class is the base class for all scene description fields.
 * @ingroup scenedescription
 */
class OCEAN_SCENEDESCRIPTION_EXPORT Field
{
	public:

		/**
		 * Definition of scalar field types.
		 */
		enum Type : uint32_t
		{
			/// Invalid field type.
			TYPE_INVALID = 0u,
			/// Field with a boolean value.
			TYPE_BOOLEAN,
			/// Field with a color value.
			TYPE_COLOR,
			/// Field with a float value.
			TYPE_FLOAT,
			/// Field with an integer value.
			TYPE_INT,
			/// Field with a 3x3 matrix as value.
			TYPE_MATRIX3,
			/// Field with a 4x4 matrix as value.
			TYPE_MATRIX4,
			/// Field with a node as value.
			TYPE_NODE,
			/// Field with a rotation value.
			TYPE_ROTATION,
			/// Field with a string value.
			TYPE_STRING,
			/// Field with a time value.
			TYPE_TIME,
			/// Field with a 2D vector as value.
			TYPE_VECTOR2,
			/// Field with a 3D vector as value.
			TYPE_VECTOR3,
			/// Field with a 4D vector as value.
			TYPE_VECTOR4
		};

	public:

		/**
		 * Destructs a field object.
		 */
		inline virtual ~Field();

		/**
		 * Returns the most recent field modification timestamp.
		 * @return Field timestamp
		 */
		inline Timestamp timestamp() const;

		/**
		 * Returns the type of this field.
		 * @return Field type
		 */
		virtual Type type() const = 0;

		/**
		 * Returns the dimension of this field.
		 * @return Field dimension
		 */
		virtual unsigned int dimension() const = 0;

		/**
		 * Returns whether this field is a 0D field.
		 * @return True, if so
		 */
		inline bool is0D() const;

		/**
		 * Returns whether this field is a 1D field.
		 * @return True, if so
		 */
		inline bool is1D() const;

		/**
		 * Returns whether this field has a given type.
		 * @param type Type to check
		 * @return True, if so
		 */
		inline bool isType(const Type type) const;

		/**
		 * Returns whether this field has a given type and dimension.
		 * @param type Type to check
		 * @param dimension Dimension to check
		 * @return True, if so
		 */
		inline bool isType(const Type type, const unsigned int dimension) const;

		/**
		 * Sets the field timestamp explicitly.
		 * @param timestamp Field timestamp to set
		 */
		inline void setTimestamp(const Timestamp timestamp);

		/**
		 * Casts an unspecific field object to the real field object.
		 * @param field Unspecific field object
		 * @return Specified field object
		 */
		template <typename T> static const T& cast(const Field& field);

		/**
		 * Returns a new instance of this field.
		 * @return Copy of this field
		 */
		virtual Field* copy() const = 0;

		/**
		 * Assigns a field to this field if both field have the identical field type
		 * @param field Field to assign the value from
		 * @return True, if the field type was identical
		 */
		virtual bool assign(const Field& field) = 0;

		/**
		 * Assigns a field to this field.
		 * The field value and field timestamp is copied only if the field types are identical.
		 * @param field Field to assign
		 * @return Reference to this object
		 */
		inline Field& operator=(const Field& field);

	protected:

		/**
		 * Creates a new field.
		 * The field timestamp will be set to invalid.
		 */
		Field() = default;

		/**
		 * Disabled copy constructor.
		 * @param field Object which would be copied
		 */
		Field(const Field& field) = delete;

		/**
		 * Creates a new field with an explicit modification timestamp.
		 * @param timestamp Explicit field timestamp
		 */
		inline explicit Field(const Timestamp timestamp);

	protected:

		/// Field timestamp.
		Timestamp timestamp_;
};

inline Field::Field(const Timestamp timestamp) :
	timestamp_(timestamp)
{
	// nothing to do here
}

inline Field::~Field()
{
	// nothing to do here
}

inline Timestamp Field::timestamp() const
{
	return timestamp_;
}

inline bool Field::is0D() const
{
	return dimension() == 0;
}

inline bool Field::is1D() const
{
	return dimension() == 1;
}

inline bool Field::isType(const Type checkType) const
{
	return type() == checkType;
}

inline bool Field::isType(const Type checkType, const unsigned int checkDimension) const
{
	return isType(checkType) && dimension() == checkDimension;
}

inline void Field::setTimestamp(const Timestamp timestamp)
{
	timestamp_ = timestamp;
}

template <typename T>
const T& Field::cast(const Field& field)
{
	ocean_assert(field.isType(T::fieldType, T::fieldDimension));

	return dynamic_cast<const T&>(field);
}

inline Field& Field::operator=(const Field& field)
{
	assign(field);
	return *this;
}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_FIELD_H
