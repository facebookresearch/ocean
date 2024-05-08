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
class ScopedFunction
{
	public:

		/**
		 * Creates an object without function.
		 */
		ScopedFunction() = default;

		/**
		 * Creates a new object with a given function.
		 * @param function The function to be invoked once this object is disposed
		 */
		explicit ScopedFunction(T&& function);

		/**
		 * Move constructor.
		 * @param scopedFunction Object to be moved
		 */
		ScopedFunction(ScopedFunction<T>&& scopedFunction);

		/**
		 * Destructs this object and invoked the function if this object holds function.
		 */
		~ScopedFunction();

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
		ScopedFunction& operator=(ScopedFunction<T>&& scopedFunction);

	protected:

		/**
		 * Disabled copy constructor.
		 */
		ScopedFunction(const ScopedFunction&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		ScopedFunction<T>& operator=(const ScopedFunction&) = delete;

	protected:

		/// The function to be invoked.
		T function_ = T();
};

/**
 * Definition of a scoped function with void return parameter.
 * @ingroup base
 */
typedef ScopedFunction<std::function<void()>> ScopedFunctionVoid;

template <typename T>
ScopedFunction<T>::ScopedFunction(T&& function) :
	function_(std::move(function))
{
	// nothing to do here
}

template <typename T>
ScopedFunction<T>::ScopedFunction(ScopedFunction<T>&& scopedFunction)
{
	*this = std::move(scopedFunction);
}

template <typename T>
ScopedFunction<T>::~ScopedFunction()
{
	release();
}

template <typename T>
void ScopedFunction<T>::release()
{
	if (function_)
	{
		function_();

		function_ = T();
	}
}

template <typename T>
void ScopedFunction<T>::revoke()
{
	function_ = T();
}

template <typename T>
ScopedFunction<T>& ScopedFunction<T>::operator=(ScopedFunction<T>&& scopedFunction)
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
