/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SCOPED_OBJECT_H
#define META_OCEAN_BASE_SCOPED_OBJECT_H

#include "ocean/base/Base.h"
#include "ocean/base/DataType.h"

namespace Ocean
{

/**
 * This class wraps an unmanaged object (or reference) which needs to be released after usage.
 * The release function can be defined at runtime.
 * @tparam T The data type of the wrapped object
 * @tparam TReleaseValue The optional explicit data type of the object to be released
 * @tparam TReleaseFunction The data type of the release function
 * @see ScopedObjectCompileTime.
 * @ingroup base
 */
template <typename T, typename TReleaseValue = T, typename TReleaseFunction = void(*)(TReleaseValue)>
class ScopedObjectT
{
	public:

		/**
		 * Default constructor creating an object with invalid object.
		 */
		ScopedObjectT() = default;

		/**
		 * Move constructor.
		 * @param scopedObject The scoped object to be moved
		 */
		ScopedObjectT(ScopedObjectT<T, TReleaseValue, TReleaseFunction>&& scopedObject) noexcept;

		/**
		 * Creates a new scoped object.
		 * @param object The object to be wrapped
		 * @param releaseFunction The release function
		 */
		ScopedObjectT(T&& object, TReleaseFunction&& releaseFunction) noexcept;

		/**
		 * Creates a new scoped object.
		 * This constructor allows to decide at runtime whether the release function will be used or not.
		 * @param object The object to be wrapped
		 * @param releaseFunction The release function
		 * @param useReleaseFunction True, to use the provided release function; False, to ignore the provided release function (so that the wrapped object will never be released)
		 */
		ScopedObjectT(T&& object, TReleaseFunction&& releaseFunction, const bool useReleaseFunction) noexcept;

		/**
		 * Creates a new scoped object.
		 * @param object The object to be wrapped
		 * @param releaseFunction The release function
		 */
		ScopedObjectT(const T& object, TReleaseFunction&& releaseFunction) noexcept;

		/**
		 * Creates a new scoped object.
		 * This constructor allows to decide at runtime whether the release function will be used or not.
		 * @param object The object to be wrapped
		 * @param releaseFunction The release function
		 * @param useReleaseFunction True, to use the provided release function; False, to ignore the provided release function (so that the wrapped object will never be released)
		 */
		ScopedObjectT(const T& object, TReleaseFunction&& releaseFunction, const bool useReleaseFunction) noexcept;

		/**
		 * Destructs this scoped object and releases the internal wrapped object.
		 */
		~ScopedObjectT();

		/**
		 * Returns whether this scoped object holds a valid release function (which will be invoked once the object is released).
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns the wrapped object.
		 * @return The wrapped object
		 */
		const T& object() const;

		/**
		 * Arrow operator returning the wrapped object.
		 * Ensure that the object is valid before calling this operator.
		 * @return The wrapped object
		 * @see isValid().
		 */
		const T& operator->() const;

		/**
		 * De-reference operator returning the wrapped object.
		 * Ensure that the object is valid before calling this operator.
		 * @return The wrapped object
		 * @see isValid().
		 */
		const T& operator*() const;

		/**
		 * Explicitly releases the wrapped object.
		 */
		void release();

		/**
		 * Move operator.
		 * @param scopedObject The scoped object to be moved
		 * @return Reference to this object
		 */
		ScopedObjectT<T, TReleaseValue, TReleaseFunction>& operator=(ScopedObjectT<T, TReleaseValue, TReleaseFunction>&& scopedObject) noexcept;

#ifdef OCEAN_ENABLE_CAST_OPERATOR_FOR_SCOPED_OBJECT

		/**
		 * Returns the wrapped object.
		 * @return The wrapped object
		 */
		operator const T&() const;

#endif // OCEAN_ENABLE_CAST_OPERATOR_FOR_SCOPED_OBJECT

	protected:

		/**
		 * Disabled copy constructor.
		 */
		ScopedObjectT(const ScopedObjectT<T, TReleaseValue, TReleaseFunction>&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		ScopedObjectT<T, TReleaseValue, TReleaseFunction>& operator=(const ScopedObjectT<T, TReleaseValue, TReleaseFunction>&) = delete;

	protected:

		/// The wrapped reference.
		T object_ = T();

		/// The function used to release the wrapped object.
		TReleaseFunction releaseFunction_ = TReleaseFunction();
};

/**
 * This class wraps an unmanaged object (or reference) which needs to be released after usage.
 * The release function needs to be defined at compile time.
 * @tparam T The data type of the wrapped object
 * @tparam TReleaseValue The optional explicit data type of the object to be released
 * @tparam TReleaseReturn The data type of the return value of the release function
 * @tparam tReleaseFunction The data type of the release function
 * @tparam tExpectedReturnValue The expected return value of the release function
 * @tparam tCheckReturnValue True, to check the return value when calling the release function; False, to ignore the return value
 * @tparam tInvalidValue The value of an invalid object
 * @see ScopedObjectT.
 * @ingroup base
 */
template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue = NotVoidTyper<TReleaseReturn>::defaultValue(), bool tCheckReturnValue = true, T tInvalidValue = T()>
class ScopedObjectCompileTimeT
{
	public:

		/**
		 * Default constructor creating an object with invalid object.
		 */
		ScopedObjectCompileTimeT() = default;

		/**
		 * Move constructor.
		 * @param scopedObject The scoped object to be moved
		 */
		ScopedObjectCompileTimeT(ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>&& scopedObject) noexcept;

		/**
		 * Creates a new scoped object.
		 * If 'object == tInvalidValue' the object will not be released once this scoped object is disposed.
		 * @param object The object to be wrapped
		 */
		explicit ScopedObjectCompileTimeT(T&& object) noexcept;

		/**
		 * Creates a new scoped object.
		 * @param object The object to be wrapped
		 * @param needsRelease True, if the given object needs to be released once the scoped object is disposed; False, if the given object does not need to be released
		 */
		ScopedObjectCompileTimeT(T&& object, const bool needsRelease) noexcept;

		/**
		 * Creates a new scoped object.
		 * If 'object == tInvalidValue' the object will not be released once this scoped object is disposed.
		 * @param object The object to be wrapped
		 */
		explicit ScopedObjectCompileTimeT(const T& object);

		/**
		 * Creates a new scoped object.
		 * @param object The object to be wrapped
		 * @param needsRelease True, if the given object needs to be released once the scoped object is disposed; False, if the given object does not need to be released
		 */
		ScopedObjectCompileTimeT(const T& object, const bool needsRelease);

		/**
		 * Destructs this scoped object and releases the internal wrapped object.
		 */
		~ScopedObjectCompileTimeT();

		/**
		 * Returns whether this scoped object holds a valid object.
		 * @return True, if so
		 */
		bool isValid() const;

		/**
		 * Returns the wrapped object.
		 * @return The wrapped object
		 */
		const T& object() const;

		/**
		 * Returns the wrapped object.
		 * Ensure that the object is valid before calling this operator.
		 * @return The wrapped object
		 * @see isValid().
		 */
		const T& operator->() const;

		/**
		 * De-reference operator returning the wrapped object.
		 * Ensure that the object is valid before calling this operator.
		 * @return The wrapped object
		 * @see isValid().
		 */
		const T& operator*() const;

		/**
		 * Releases the current wrapped object and returns a new wrapped object.
		 * @param needsRelease True, if the new object needs to be released once the scoped object is disposed; False, if the new object does not need to be released
		 * @return The new wrapped object
		 */
		T& resetObject(const bool needsRelease = true);

		/**
		 * Explicitly releases the wrapped object.
		 */
		void release();

		/**
		 * Move operator.
		 * @param scopedObject The scoped object to be moved
		 * @return Reference to this object
		 */
		ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>& operator=(ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>&& scopedObject) noexcept;

#ifdef OCEAN_ENABLE_CAST_OPERATOR_FOR_SCOPED_OBJECT

		/**
		 * Returns the wrapped object.
		 * @return The wrapped object
		 */
		operator const T&() const;

#endif // OCEAN_ENABLE_CAST_OPERATOR_FOR_SCOPED_OBJECT

	protected:

		/**
		 * Disabled copy constructor.
		 */
		ScopedObjectCompileTimeT(const ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		ScopedObjectCompileTimeT& operator=(const ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>&) = delete;

	protected:

		/// The wrapped reference.
		T object_ = tInvalidValue;

		/// True, if the wrapped object needs to be released.
		bool needsRelease_ = false;
};

/**
 * Template specialization for ScopedObjectCompileTimeT with void return value.
 * @tparam T The data type of the wrapped object
 * @tparam tReleaseFunction The data type of the release function
 * @see ScopedObjectCompileTimeT.
 * @ingroup base
 */
template <typename T, void (*tReleaseFunction)(T)>
using ScopedObjectCompileTimeVoidT = ScopedObjectCompileTimeT<T, T, void, tReleaseFunction>;

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObjectT<T, TReleaseValue, TReleaseFunction>::ScopedObjectT(ScopedObjectT<T, TReleaseValue, TReleaseFunction>&& scopedObject) noexcept
{
	*this = std::move(scopedObject);
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObjectT<T, TReleaseValue, TReleaseFunction>::ScopedObjectT(T&& object, TReleaseFunction&& releaseFunction) noexcept :
	object_(std::move(object)),
	releaseFunction_(std::move(releaseFunction))
{
	// nothing to do here
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObjectT<T, TReleaseValue, TReleaseFunction>::ScopedObjectT(T&& object, TReleaseFunction&& releaseFunction, const bool useReleaseFunction) noexcept :
	object_(std::move(object))
{
	if (useReleaseFunction)
	{
		releaseFunction_ = std::move(releaseFunction);
	}
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObjectT<T, TReleaseValue, TReleaseFunction>::ScopedObjectT(const T& object, TReleaseFunction&& releaseFunction) noexcept :
	object_(object),
	releaseFunction_(std::move(releaseFunction))
{
	// nothing to do here
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObjectT<T, TReleaseValue, TReleaseFunction>::ScopedObjectT(const T& object, TReleaseFunction&& releaseFunction, const bool useReleaseFunction) noexcept :
	object_(object)
{
	if (useReleaseFunction)
	{
		releaseFunction_ = std::move(releaseFunction);
	}
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObjectT<T, TReleaseValue, TReleaseFunction>::~ScopedObjectT()
{
	release();
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
bool ScopedObjectT<T, TReleaseValue, TReleaseFunction>::isValid() const
{
	return bool(releaseFunction_);
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
const T& ScopedObjectT<T, TReleaseValue, TReleaseFunction>::object() const
{
	return object_;
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
const T& ScopedObjectT<T, TReleaseValue, TReleaseFunction>::operator->() const
{
	ocean_assert(isValid());

	return object();
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
const T& ScopedObjectT<T, TReleaseValue, TReleaseFunction>::operator*() const
{
	ocean_assert(isValid());

	return object();
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
void ScopedObjectT<T, TReleaseValue, TReleaseFunction>::release()
{
	if (releaseFunction_)
	{
		releaseFunction_(TReleaseValue(object_));

		object_ = T();
		releaseFunction_ = TReleaseFunction();
	}
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObjectT<T, TReleaseValue, TReleaseFunction>& ScopedObjectT<T, TReleaseValue, TReleaseFunction>::operator=(ScopedObjectT<T, TReleaseValue, TReleaseFunction>&& scopedObject) noexcept
{
	if (this != &scopedObject)
	{
		release();

		object_ = std::move(scopedObject.object_);
		scopedObject.object_ = T();

		releaseFunction_ = std::move(scopedObject.releaseFunction_);
		scopedObject.releaseFunction_ = TReleaseFunction();
	}

	return *this;
}

#ifdef OCEAN_ENABLE_CAST_OPERATOR_FOR_SCOPED_OBJECT

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObjectT<T, TReleaseValue, TReleaseFunction>::operator const T&() const
{
	return object_;
}

#endif

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::ScopedObjectCompileTimeT(ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>&& scopedObject) noexcept
{
	*this = std::move(scopedObject);
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::ScopedObjectCompileTimeT(T&& object) noexcept :
	object_(std::move(object))
{
	needsRelease_ = object_ != tInvalidValue;
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::ScopedObjectCompileTimeT(T&& object, const bool needsRelease) noexcept :
	object_(std::move(object)),
	needsRelease_(needsRelease)
{
	// nothing to do here
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::ScopedObjectCompileTimeT(const T& object) :
	object_(object)
{
	needsRelease_ = object_ != tInvalidValue;
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::ScopedObjectCompileTimeT(const T& object, const bool needsRelease) :
	object_(object),
	needsRelease_(needsRelease)
{
	// nothing to do here
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::~ScopedObjectCompileTimeT()
{
	release();
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
bool ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::isValid() const
{
	return object_ != tInvalidValue;
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
const T& ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::object() const
{
	return object_;
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
const T& ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::operator->() const
{
	ocean_assert(isValid());

	return object();
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
const T& ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::operator*() const
{
	ocean_assert(isValid());

	return object();
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
T& ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::resetObject(const bool needsRelease)
{
	release();
	object_ = tInvalidValue;

	needsRelease_ = needsRelease;

	return object_;
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
void ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::release()
{
	if (needsRelease_)
	{
		if constexpr (std::is_same<TReleaseReturn, void>::value)
		{
			tReleaseFunction(TReleaseValue(object_));
		}
		else
		{
			if constexpr (tCheckReturnValue)
			{
				const TReleaseReturn returnValue = tReleaseFunction(TReleaseValue(object_));
				ocean_assert_and_suppress_unused(returnValue == tExpectedReturnValue, returnValue);
			}
			else
			{
				const TReleaseReturn returnValue = tReleaseFunction(TReleaseValue(object_));
				OCEAN_SUPPRESS_UNUSED_WARNING(returnValue);
			}
		}

		object_ = tInvalidValue;
		needsRelease_ = false;
	}
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>& ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::operator=(ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>&& scopedObject) noexcept
{
	if (this != &scopedObject)
	{
		release();

		object_ = std::move(scopedObject.object_);
		scopedObject.object_ = tInvalidValue;

		needsRelease_ = scopedObject.needsRelease_;
		scopedObject.needsRelease_ = false;
	}

	return *this;
}

#ifdef OCEAN_ENABLE_CAST_OPERATOR_FOR_SCOPED_OBJECT

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTimeT<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::operator const T&() const
{
	return object_;
}

#endif

}

#endif // META_OCEAN_BASE_SCOPED_OBJECT_H
