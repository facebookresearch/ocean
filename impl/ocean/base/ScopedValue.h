/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SCOPED_VALUE_H
#define META_OCEAN_BASE_SCOPED_VALUE_H

#include "ocean/base/Base.h"

namespace Ocean
{

/**
 * This class implements a scoped value that allows to change a specified value at the end of a scope.
 * @tparam T Data type of the value to be set
 * @ingroup base
 */
template <typename T>
class ScopedValueT
{
	public:

		/**
		 * Creates a new scoped value object.
		 * @param target The target value that will be changed at the end of the surrounding scope
		 * @param delayedValue Value that will be assigned at the end of the surrounding scope
		 */
		inline ScopedValueT(T& target, const T& delayedValue);

		/**
		 * Creates a new scoped value object.
		 * @param target The target value that will be changed at the end of the surrounding scope
		 * @param delayedValue Value that will be assigned at the end of the surrounding scope
		 */
		inline ScopedValueT(T& target, T&& delayedValue);

		/**
		 * Creates a new scoped value object.
		 * @param target The target value that will be changed at the end of the surrounding scope
		 * @param delayedValue Value that will be assigned at the end of the surrounding scope
		 * @param immediateValue Value that will be assigned directly inside the constructor
		 */
		inline ScopedValueT(T& target, const T& delayedValue, const T& immediateValue);

		/**
		 * Creates a new scoped value object.
		 * @param target The target value that will be changed at the end of the surrounding scope
		 * @param delayedValue Value that will be assigned at the end of the surrounding scope
		 * @param immediateValue Value that will be assigned directly inside the constructor
		 */
		inline ScopedValueT(T& target, T&& delayedValue, T&& immediateValue);

		/**
		 * Destructs the scoped value object.
		 */
		inline ~ScopedValueT();

		/**
		 * Changes the value that will be assigned at the end of the surrounding scope.
		 * @param value The value to be changed
		 */
		inline void setDelayed(const T& value);

		/**
		 * Changes the value that will be assigned at the end of the surrounding scope.
		 * @param value The value to be changed
		 */
		inline void setDelayed(T&& value);

		/**
		 * Immediately changes the target value, the modification is not applied at the end of the surrounding scope
		 * @param value The value to be changed
		 */
		inline void setImmediately(const T& value);

		/**
		 * Immediately changes the target value, the modification is not applied at the end of the surrounding scope
		 * @param value The value to be changed
		 */
		inline void setImmediately(T&& value);

	protected:

		/**
		 * Disabled copy constructor.
		 * @param value The value which would be copied
		 */
		ScopedValueT(const ScopedValueT<T>& value) = delete;

		/**
		 * Disabled copy operator.
		 * @param value The value which would be copied
		 */
		ScopedValueT& operator=(const ScopedValueT<T>& value) = delete;

	protected:

		/// Target value that will be changed at the end of the surrounding scope.
		T& valueTarget;

		/// Value that will be assigned at the end of the surrounding scope.
		T valueDelayed;
};

template <typename T>
inline ScopedValueT<T>::ScopedValueT(T& target, const T& delayedValue) :
	valueTarget(target),
	valueDelayed(delayedValue)
{
	// nothing to do here
}

template <typename T>
inline ScopedValueT<T>::ScopedValueT(T& target, T&& delayedValue) :
	valueTarget(target),
	valueDelayed(std::move(delayedValue))
{
	// nothing to do here
}

template <typename T>
inline ScopedValueT<T>::ScopedValueT(T& target, const T& delayedValue, const T& immediateValue) :
	valueTarget(target),
	valueDelayed(delayedValue)
{
	target = immediateValue;
}

template <typename T>
inline ScopedValueT<T>::ScopedValueT(T& target, T&& delayedValue, T&& immediateValue) :
	valueTarget(target),
	valueDelayed(std::move(delayedValue))
{
	target = std::move(immediateValue);
}

template <typename T>
inline ScopedValueT<T>::~ScopedValueT()
{
	valueTarget = valueDelayed;
}

template <typename T>
inline void ScopedValueT<T>::setDelayed(const T& value)
{
	valueDelayed = value;
}

template <typename T>
inline void ScopedValueT<T>::setDelayed(T&& value)
{
	valueDelayed = std::move(value);
}

template <typename T>
inline void ScopedValueT<T>::setImmediately(const T& value)
{
	valueTarget = value;
}

template <typename T>
inline void ScopedValueT<T>::setImmediately(T&& value)
{
	valueTarget = std::move(value);
}

}

#endif // META_OCEAN_BASE_SCOPED_VALUE_H
