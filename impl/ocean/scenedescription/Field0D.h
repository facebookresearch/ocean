/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_FIELD_0D_H
#define META_OCEAN_SCENEDESCRIPTION_FIELD_0D_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Field.h"
#include "ocean/scenedescription/Node.h"
#include "ocean/scenedescription/FieldTyper.h"

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration.
template <typename T> class Field0D;

/**
 * Definition of a single field with boolean value.
 * @ingroup scenedescription
 */
typedef Field0D<bool> SingleBool;

/**
 * Definition of a single field with color value.
 * @ingroup scenedescription
 */
typedef Field0D<RGBAColor> SingleColor;

/**
 * Definition of a single field with float value.
 * @ingroup scenedescription
 */
typedef Field0D<Scalar> SingleFloat;

/**
 * Definition of a single field with integer value.
 * @ingroup scenedescription
 */
typedef Field0D<int> SingleInt;

/**
 * Definition of a single field with node value.
 * @ingroup scenedescription
 */
typedef Field0D<NodeRef> SingleNode;

/**
 * Definition of a single field with 3x3 matrix value.
 * @ingroup scenedescription
 */
typedef Field0D<SquareMatrix3> SingleMatrix3;

/**
 * Definition of a single field with 4x4 matrix value.
 * @ingroup scenedescription
 */
typedef Field0D<SquareMatrix4> SingleMatrix4;

/**
 * Definition of a single field with rotation value.
 * @ingroup scenedescription
 */
typedef Field0D<Rotation> SingleRotation;

/**
 * Definition of a single field with string value.
 * @ingroup scenedescription
 */
typedef Field0D<std::string> SingleString;

/**
 * Definition of a single field with time value.
 * @ingroup scenedescription
 */
typedef Field0D<Timestamp> SingleTime;

/**
 * Definition of a single field with 2D vector value.
 * @ingroup scenedescription
 */
typedef Field0D<Vector2> SingleVector2;

/**
 * Definition of a single field with 3D vector value.
 * @ingroup scenedescription
 */
typedef Field0D<Vector3> SingleVector3;

/**
 * Definition of a single field with 4D vector value.
 * @ingroup scenedescription
 */
typedef Field0D<Vector4> SingleVector4;

/**
 * This class implements all 0D fields (fields holding a single value only).
 * @ingroup scenedescription
 */
template <typename T>
class Field0D : public Field
{
	public:

		/// Scalar type of this single field.
		static constexpr Type fieldType = TypeMapper<T>::type();

		/// Dimension of this single field.
		static constexpr unsigned int fieldDimension = 0u;

	public:

		/**
		 * Creates a new single value field with an undefined value.
		 */
		Field0D() = default;

		/**
		 * Creates a new single value field by a given initialization value.
		 * The modification timestamp will be set to zero.
		 * @param value Initialization value
		 */
		inline Field0D(const T& value);

		/**
		 * Creates a new single value field by a given initialization value and an explicit timestamp.
		 * @param value Initialization value
		 * @param timestamp Explicit field timestamp
		 */
		inline Field0D(const T& value, const Timestamp timestamp);

		/**
		 * Returns the type of this field.
		 * @see Field::type().
		 */
		Type type() const override;

		/**
		 * Returns the dimension of this field.
		 * @see Field::dimension().
		 */
		unsigned int dimension() const override;

		/**
		 * Returns the value of this field.
		 * @return Field value
		 */
		inline const T& value() const;

		/**
		 * Sets the value of this field and changes the modification timestamp to the current time.
		 * @param value Value to set
		 */
		inline void setValue(const T& value);

		/**
		 * Sets the value of this field and defines an explicit modification timestamp.
		 * @param value Value to set
		 * @param timestamp Explicit modification timestamp
		 */
		inline void setValue(const T& value, const Timestamp timestamp);

		/**
		 * Assigns a field to this field if both field have the identical field type
		 * @see Field::assignField().
		 */
		bool assign(const Field& field) override;

	protected:

		/**
		 * Returns a new instance of this field.
		 * @see Field::copy().
		 */
		Field* copy() const override;

	protected:

		/// Field (single) value.
		T value_;
};

template <typename T>
inline Field0D<T>::Field0D(const T& value) :
	value_(value)
{
	// nothing to do here
}

template <typename T>
inline Field0D<T>::Field0D(const T& value, const Timestamp timestamp) :
	Field(timestamp),
	value_(value)
{
	// nothing to do here
}

template <typename T>
Field::Type Field0D<T>::type() const
{
	return TypeMapper<T>::type();
}

template <typename T>
unsigned int Field0D<T>::dimension() const
{
	return 0u;
}

template <typename T>
inline const T& Field0D<T>::value() const
{
	return value_;
}

template <typename T>
inline void Field0D<T>::setValue(const T& value)
{
	value_ = value;
	timestamp_.toNow();
}

template <typename T>
inline void Field0D<T>::setValue(const T& value, const Timestamp timestamp)
{
	value_ = value;
	timestamp_ = timestamp;
}

template <typename T>
Field* Field0D<T>::copy() const
{
	return new Field0D<T>(value_);
}

template <typename T>
bool Field0D<T>::assign(const Field& field)
{
	if (fieldType != field.type() || fieldDimension != field.dimension())
	{
		return false;
	}

	value_ = dynamic_cast<const Field0D<T>&>(field).value_;
	timestamp_ = field.timestamp();

	return true;
}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_FIELD_0D_H
