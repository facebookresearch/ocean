/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_FIELD_1D_H
#define META_OCEAN_SCENEDESCRIPTION_FIELD_1D_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Field.h"
#include "ocean/scenedescription/FieldTyper.h"

#include <vector>

namespace Ocean
{

namespace SceneDescription
{

// Forward declaration.
template <typename T> class Field1D;

/**
 * Definition of a multi field with boolean values.
 * @ingroup scenedescription
 */
typedef Field1D<bool> MultiBool;

/**
 * Definition of a multi field with color values.
 * @ingroup scenedescription
 */
typedef Field1D<RGBAColor> MultiColor;

/**
 * Definition of a multi field with float values.
 * @ingroup scenedescription
 */
typedef Field1D<Scalar> MultiFloat;

/**
 * Definition of a multi field with integer values.
 * @ingroup scenedescription
 */
typedef Field1D<int> MultiInt;

/**
 * Definition of a multi field with node values.
 * @ingroup scenedescription
 */
typedef Field1D<NodeRef> MultiNode;

/**
 * Definition of a multi field with 3x3 matrix values.
 * @ingroup scenedescription
 */
typedef Field1D<SquareMatrix3> MultiMatrix3;

/**
 * Definition of a multi field with 4x4 matrix values.
 * @ingroup scenedescription
 */
typedef Field1D<SquareMatrix4> MultiMatrix4;

/**
 * Definition of a multi field with rotation values.
 * @ingroup scenedescription
 */
typedef Field1D<Rotation> MultiRotation;

/**
 * Definition of a multi field with string values.
 * @ingroup scenedescription
 */
typedef Field1D<std::string> MultiString;

/**
 * Definition of a multi field with time values.
 * @ingroup scenedescription
 */
typedef Field1D<Timestamp> MultiTime;

/**
 * Definition of a multi field with 2D vector values.
 * @ingroup scenedescription
 */
typedef Field1D<Vector2> MultiVector2;

/**
 * Definition of a multi field with 3D vector values.
 * @ingroup scenedescription
 */
typedef Field1D<Vector3> MultiVector3;

/**
 * Definition of a multi field with 4D vector values.
 * @ingroup scenedescription
 */
typedef Field1D<Vector4> MultiVector4;

/**
 * This class implements all 1D fields (fields holding an array of single values).
 * @ingroup scenedescription
 */
template <typename T>
class Field1D : public Field
{
	public:

		/// Scalar type of this multi field.
		static constexpr Type fieldType = TypeMapper<T>::type();

		/// Dimension of this multi field.
		static constexpr unsigned int fieldDimension = 1u;

		/**
		 * Definition of a vector holding the single values.
		 */
		typedef std::vector<T> Values;

	public:

		/**
		 * Creates a new multi value field with no value.
		 */
		Field1D() = default;

		/**
		 * Creates a new multi value field by a given initialization value.
		 * The modification timestamp will be set to zero.
		 * @param value Initialization value
		 */
		inline Field1D(const T& value);

		/**
		 * Creates a new multi value field by a given initialization value.
		 * @param value Initialization value
		 * @param timestamp Explicit field timestamp
		 */
		inline Field1D(const T& value, const Timestamp timestamp);

		/**
		 * Creates a new multi value field by given initilization values.
		 * The modification timestamp will be set to zero.
		 * @param values Initialization values
		 */
		inline Field1D(const Values& values);

		/**
		 * Creates a new multi value field by given initilization values.
		 * @param values Initialization values
		 * @param timestamp Explicit field timestamp
		 */
		inline Field1D(const Values& values, const Timestamp timestamp);

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
		 * Returns the values of this field.
		 * @return Field values
		 */
		inline const Values& values() const;

		/**
		 * Returns the values of this field.
		 * @return Field values
		 */
		inline Values& values();

		/**
		 * Sets the values of this field and changes the timestamp to the current time.
		 * @param value Values to set
		 */
		inline void setValues(const Values& value);

		/**
		 * Sets the values of this field and defines an explicit modification timestamp.
		 * @param value Values to set
		 * @param timestamp Explicit modification timestamp
		 */
		inline void setValues(const Values& value, const Timestamp timestamp);

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

		/// Field values.
		Values values_;
};

template <typename T>
inline Field1D<T>::Field1D(const T& value) :
	values_(1, value)
{
	// nothing to do here
}

template <typename T>
inline Field1D<T>::Field1D(const T& value, const Timestamp timestamp) :
	Field(timestamp),
	values_(1, value)
{
	// nothing to do here
}

template <typename T>
inline Field1D<T>::Field1D(const Values& values) :
	values_(values)
{
	// nothing to do here
}

template <typename T>
inline Field1D<T>::Field1D(const Values& values, const Timestamp timestamp) :
	Field(timestamp),
	values_(values)
{
	// nothing to do here
}

template <typename T>
Field::Type Field1D<T>::type() const
{
	return TypeMapper<T>::type();
}

template <typename T>
unsigned int Field1D<T>::dimension() const
{
	return 1;
}

template <typename T>
inline const typename Field1D<T>::Values& Field1D<T>::values() const
{
	return values_;
}

template <typename T>
inline typename Field1D<T>::Values& Field1D<T>::values()
{
	return values_;
}

template <typename T>
inline void Field1D<T>::setValues(const Values& values)
{
	values_ = values;
	timestamp_.toNow();
}

template <typename T>
inline void Field1D<T>::setValues(const Values& values, const Timestamp timestamp)
{
	values_ = values;
	timestamp_ = timestamp;
}

template <typename T>
Field* Field1D<T>::copy() const
{
	return new Field1D<T>(values_);
}

template <typename T>
bool Field1D<T>::assign(const Field& field)
{
	if (fieldType != field.type() || fieldDimension != field.dimension())
	{
		return false;
	}

	values_ = dynamic_cast<const Field1D<T>&>(field).values_;
	timestamp_ = field.timestamp();

	return true;
}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_FIELD_1D_H
