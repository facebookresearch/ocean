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
 * @tparam T Data type of the value (or object) to be set
 * @tparam TValue Data type of the value to be used for the delayed (and immediate) assignment
 * @ingroup base
 */
template <typename T, typename TValue = T>
class ScopedValueT
{
	public:

		/**
		 * Creates a new scoped value object.
		 * @param target The target value that will be changed at the end of the surrounding scope
		 * @param delayedValue Value that will be assigned at the end of the surrounding scope
		 */
		inline ScopedValueT(T& target, const TValue& delayedValue);

		/**
		 * Creates a new scoped value object.
		 * @param target The target value that will be changed at the end of the surrounding scope
		 * @param delayedValue Value that will be assigned at the end of the surrounding scope
		 */
		inline ScopedValueT(T& target, TValue&& delayedValue);

		/**
		 * Creates a new scoped value object.
		 * @param target The target value that will be changed at the end of the surrounding scope
		 * @param delayedValue Value that will be assigned at the end of the surrounding scope
		 * @param immediateValue Value that will be assigned directly inside the constructor
		 */
		inline ScopedValueT(T& target, const TValue& delayedValue, const TValue& immediateValue);

		/**
		 * Creates a new scoped value object.
		 * @param target The target value that will be changed at the end of the surrounding scope
		 * @param delayedValue Value that will be assigned at the end of the surrounding scope
		 * @param immediateValue Value that will be assigned directly inside the constructor
		 */
		inline ScopedValueT(T& target, TValue&& delayedValue, TValue&& immediateValue);

		/**
		 * Destructs the scoped value object.
		 */
		inline ~ScopedValueT();

		/**
		 * Changes the value that will be assigned at the end of the surrounding scope.
		 * @param value The value to be changed
		 */
		inline void setDelayed(const TValue& value);

		/**
		 * Changes the value that will be assigned at the end of the surrounding scope.
		 * @param value The value to be changed
		 */
		inline void setDelayed(TValue&& value);

		/**
		 * Immediately changes the target value, the modification is not applied at the end of the surrounding scope
		 * @param value The value to be changed
		 */
		inline void setImmediately(const TValue& value);

		/**
		 * Immediately changes the target value, the modification is not applied at the end of the surrounding scope
		 * @param value The value to be changed
		 */
		inline void setImmediately(TValue&& value);

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
		T& target_;

		/// Value that will be assigned at the end of the surrounding scope.
		TValue delayed_;
};

template <typename T, typename TValue>
inline ScopedValueT<T, TValue>::ScopedValueT(T& target, const TValue& delayedValue) :
	target_(target),
	delayed_(delayedValue)
{
	// nothing to do here
}

template <typename T, typename TValue>
inline ScopedValueT<T, TValue>::ScopedValueT(T& target, TValue&& delayedValue) :
	target_(target),
	delayed_(std::move(delayedValue))
{
	// nothing to do here
}

template <typename T, typename TValue>
inline ScopedValueT<T, TValue>::ScopedValueT(T& target, const TValue& delayedValue, const TValue& immediateValue) :
	target_(target),
	delayed_(delayedValue)
{
	target = immediateValue;
}

template <typename T, typename TValue>
inline ScopedValueT<T, TValue>::ScopedValueT(T& target, TValue&& delayedValue, TValue&& immediateValue) :
	target_(target),
	delayed_(std::move(delayedValue))
{
	target = std::move(immediateValue);
}

template <typename T, typename TValue>
inline ScopedValueT<T, TValue>::~ScopedValueT()
{
	target_ = delayed_;
}

template <typename T, typename TValue>
inline void ScopedValueT<T, TValue>::setDelayed(const TValue& value)
{
	delayed_ = value;
}

template <typename T, typename TValue>
inline void ScopedValueT<T, TValue>::setDelayed(TValue&& value)
{
	delayed_ = std::move(value);
}

template <typename T, typename TValue>
inline void ScopedValueT<T, TValue>::setImmediately(const TValue& value)
{
	target_ = value;
}

template <typename T, typename TValue>
inline void ScopedValueT<T, TValue>::setImmediately(TValue&& value)
{
	target_ = std::move(value);
}

}

#endif // META_OCEAN_BASE_SCOPED_VALUE_H
