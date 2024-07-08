/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_SCENEDESCRIPTION_FIELD_TYPER_H
#define META_OCEAN_SCENEDESCRIPTION_FIELD_TYPER_H

#include "ocean/scenedescription/SceneDescription.h"
#include "ocean/scenedescription/Field.h"

namespace Ocean
{

namespace SceneDescription
{

/**
 * Class providing mapping between scalar field types and real types.
 * @ingroup scenedescription
 */
template <typename T>
class TypeMapper
{
	public:

		/**
		 * Returns the scalar field type for this mapper object.
		 * @return Scalar field type
		 */
		static constexpr Field::Type type();
};

template <typename T>
constexpr Field::Type TypeMapper<T>::type()
{
	return Field::TYPE_INVALID;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<bool>::type()
{
	return Field::TYPE_BOOLEAN;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<RGBAColor>::type()
{
	return Field::TYPE_COLOR;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<Scalar>::type()
{
	return Field::TYPE_FLOAT;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<int>::type()
{
	return Field::TYPE_INT;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<SquareMatrix3>::type()
{
	return Field::TYPE_MATRIX3;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<SquareMatrix4>::type()
{
	return Field::TYPE_MATRIX4;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<NodeRef>::type()
{
	return Field::TYPE_NODE;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<Rotation>::type()
{
	return Field::TYPE_ROTATION;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<std::string>::type()
{
	return Field::TYPE_STRING;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<Timestamp>::type()
{
	return Field::TYPE_TIME;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<Vector2>::type()
{
	return Field::TYPE_VECTOR2;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<Vector3>::type()
{
	return Field::TYPE_VECTOR3;
}

/**
 * Template specialization of TypeMapper::type().
 * @see TypeMapper::type().
 */
template <>
constexpr Field::Type TypeMapper<Vector4>::type()
{
	return Field::TYPE_VECTOR4;
}

}

}

#endif // META_OCEAN_SCENEDESCRIPTION_FIELD_TYPER_H
