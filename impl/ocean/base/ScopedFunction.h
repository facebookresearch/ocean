/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SCOPED_FUNCTION_H
#define META_OCEAN_BASE_SCOPED_FUNCTION_H

#include "ocean/base/Base.h"

#include <functional>

namespace Ocean
{

/**
 * This class holds a function which will be invoked once the object is disposed.
 * @tparam T The data type of the function
 * @ingroup base
 */
template <typename T>
class ScopedFunctionT
{
	public:

		/**
		 * Creates an object without function.
		 */
		ScopedFunctionT() = default;

		/**
		 * Creates a new object with a given function.
		 * @param function The function to be invoked once this object is disposed
		 */
		explicit ScopedFunctionT(T&& function) noexcept;

		/**
		 * Move constructor.
		 * @param scopedFunction Object to be moved
		 */
		ScopedFunctionT(ScopedFunctionT<T>&& scopedFunction) noexcept;

		/**
		 * Destructs this object and invoked the function if this object holds function.
		 */
		~ScopedFunctionT();

		/**
		 * Explicitly released this object.
		 * In case this object holds a valid function, the function is invoked and then invalidated so that the function will not be invoked again when this object is disposed.
		 */
		void release();

		/**
		 * Revokes the function call.
		 * In case this object holds a valid function, the function is invalidated without calling the function.
		 */
		void revoke();

		/**
		 * Move operator.
		 * @param scopedFunction The object to be moved
		 * @return Reference to this object
		 */
		ScopedFunctionT& operator=(ScopedFunctionT<T>&& scopedFunction) noexcept;

	protected:

		/**
		 * Disabled copy constructor.
		 */
		ScopedFunctionT(const ScopedFunctionT&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		ScopedFunctionT<T>& operator=(const ScopedFunctionT&) = delete;

	protected:

		/// The function to be invoked.
		T function_ = T();
};

/**
 * Definition of a scoped function with void return parameter.
 * @ingroup base
 */
typedef ScopedFunctionT<std::function<void()>> ScopedFunctionVoid;

template <typename T>
ScopedFunctionT<T>::ScopedFunctionT(T&& function) noexcept :
	function_(std::move(function))
{
	// nothing to do here
}

template <typename T>
ScopedFunctionT<T>::ScopedFunctionT(ScopedFunctionT<T>&& scopedFunction) noexcept
{
	*this = std::move(scopedFunction);
}

template <typename T>
ScopedFunctionT<T>::~ScopedFunctionT()
{
	release();
}

template <typename T>
void ScopedFunctionT<T>::release()
{
	if (function_)
	{
		function_();

		function_ = T();
	}
}

template <typename T>
void ScopedFunctionT<T>::revoke()
{
	function_ = T();
}

template <typename T>
ScopedFunctionT<T>& ScopedFunctionT<T>::operator=(ScopedFunctionT<T>&& scopedFunction) noexcept
{
	if (this != &scopedFunction)
	{
		release();

		function_ = std::move(scopedFunction.function_);
		scopedFunction.function_ = T();
	}

	return *this;
}

}

#endif // META_OCEAN_BASE_SCOPED_OBJECT_H
