// Copyright (c) Meta Platforms, Inc. and affiliates.
// 
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

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
class ScopedObject
{
	public:

		/**
		 * Default constructor creating an object with invalid object.
		 */
		ScopedObject() = default;

		/**
		 * Move constructor.
		 * @param scopedObject The scoped object to be moved
		 */
		ScopedObject(ScopedObject<T, TReleaseValue, TReleaseFunction>&& scopedObject);

		/**
		 * Creates a new scoped object.
		 * @param object The object to be wrapped
		 * @param releaseFunction The release function
		 */
		ScopedObject(T&& object, TReleaseFunction&& releaseFunction);

		/**
		 * Creates a new scoped object.
		 * @param object The object to be wrapped
		 * @param releaseFunction The release function
		 */
		ScopedObject(const T& object, TReleaseFunction&& releaseFunction);

		/**
		 * Destructs this scoped object and releases the internal wrapped object.
		 */
		~ScopedObject();

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
		 * Explicitly releases the wrapped object.
		 */
		void release();

		/**
		 * Move operator.
		 * @param scopedObject The scoped object to be moved
		 * @return Reference to this object
		 */
		ScopedObject<T, TReleaseValue, TReleaseFunction>& operator=(ScopedObject<T, TReleaseValue, TReleaseFunction>&& scopedObject);

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
		ScopedObject(const ScopedObject<T, TReleaseValue, TReleaseFunction>&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		ScopedObject<T, TReleaseValue, TReleaseFunction>& operator=(const ScopedObject<T, TReleaseValue, TReleaseFunction>&) = delete;

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
 * @see ScopedObject.
 * @ingroup base
 */
template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue = NotVoidTyper<TReleaseReturn>::defaultValue(), bool tCheckReturnValue = true, T tInvalidValue = T()>
class ScopedObjectCompileTime
{
	public:

		/**
		 * Default constructor creating an object with invalid object.
		 */
		ScopedObjectCompileTime() = default;

		/**
		 * Move constructor.
		 * @param scopedObject The scoped object to be moved
		 */
		ScopedObjectCompileTime(ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>&& scopedObject);

		/**
		 * Creates a new scoped object.
		 * If 'object == tInvalidValue' the object will not be released once this scoped object is disposed.
		 * @param object The object to be wrapped
		 */
		explicit ScopedObjectCompileTime(T&& object);

		/**
		 * Creates a new scoped object.
		 * @param object The object to be wrapped
		 * @param needsRelease True, if the given object needs to be released once the scoped object is disposed; False, if the given object does not need to be released
		 */
		ScopedObjectCompileTime(T&& object, const bool needsRelease);

		/**
		 * Creates a new scoped object.
		 * If 'object == tInvalidValue' the object will not be released once this scoped object is disposed.
		 * @param object The object to be wrapped
		 */
		explicit ScopedObjectCompileTime(const T& object);

		/**
		 * Creates a new scoped object.
		 * @param object The object to be wrapped
		 * @param needsRelease True, if the given object needs to be released once the scoped object is disposed; False, if the given object does not need to be released
		 */
		ScopedObjectCompileTime(const T& object, const bool needsRelease);

		/**
		 * Destructs this scoped object and releases the internal wrapped object.
		 */
		~ScopedObjectCompileTime();

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
		ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>& operator=(ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>&& scopedObject);

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
		ScopedObjectCompileTime(const ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>&) = delete;

		/**
		 * Disabled assign operator.
		 * @return Reference to this object
		 */
		ScopedObjectCompileTime& operator=(const ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>&) = delete;

	protected:

		/// The wrapped reference.
		T object_ = tInvalidValue;

		/// True, if the wrapped object needs to be released.
		bool needsRelease_ = false;
};

/**
 * Template specialization for ScopedObjectCompileTime with void return value.
 * @tparam T The data type of the wrapped object
 * @tparam tReleaseFunction The data type of the release function
 * @see ScopedObjectCompileTime.
 * @ingroup base
 */
template <typename T, void (*tReleaseFunction)(T)>
using ScopedObjectCompileTimeVoid = ScopedObjectCompileTime<T, T, void, tReleaseFunction>;

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObject<T, TReleaseValue, TReleaseFunction>::ScopedObject(ScopedObject<T, TReleaseValue, TReleaseFunction>&& scopedObject)
{
	*this = std::move(scopedObject);
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObject<T, TReleaseValue, TReleaseFunction>::ScopedObject(T&& object, TReleaseFunction&& releaseFunction) :
	object_(std::move(object)),
	releaseFunction_(std::move(releaseFunction))
{
	// nothing to do here
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObject<T, TReleaseValue, TReleaseFunction>::ScopedObject(const T& object, TReleaseFunction&& releaseFunction) :
	object_(object),
	releaseFunction_(std::move(releaseFunction))
{
	// nothing to do here
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObject<T, TReleaseValue, TReleaseFunction>::~ScopedObject()
{
	release();
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
bool ScopedObject<T, TReleaseValue, TReleaseFunction>::isValid() const
{
	return releaseFunction_;
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
const T& ScopedObject<T, TReleaseValue, TReleaseFunction>::object() const
{
	return object_;
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
void ScopedObject<T, TReleaseValue, TReleaseFunction>::release()
{
	if (releaseFunction_)
	{
		releaseFunction_(TReleaseValue(object_));

		object_ = T();
		releaseFunction_ = TReleaseFunction();
	}
}

template <typename T, typename TReleaseValue, typename TReleaseFunction>
ScopedObject<T, TReleaseValue, TReleaseFunction>& ScopedObject<T, TReleaseValue, TReleaseFunction>::operator=(ScopedObject<T, TReleaseValue, TReleaseFunction>&& scopedObject)
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
ScopedObject<T, TReleaseValue, TReleaseFunction>::operator const T&() const
{
	return object_;
}

#endif

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::ScopedObjectCompileTime(ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>&& scopedObject)
{
	*this = std::move(scopedObject);
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::ScopedObjectCompileTime(T&& object) :
	object_(std::move(object))
{
	needsRelease_ = object_ != tInvalidValue;
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::ScopedObjectCompileTime(T&& object, const bool needsRelease) :
	object_(std::move(object)),
	needsRelease_(needsRelease)
{
	// nothing to do here
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::ScopedObjectCompileTime(const T& object) :
	object_(object)
{
	needsRelease_ = object_ != tInvalidValue;
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::ScopedObjectCompileTime(const T& object, const bool needsRelease) :
	object_(object),
	needsRelease_(needsRelease)
{
	// nothing to do here
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::~ScopedObjectCompileTime()
{
	release();
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
bool ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::isValid() const
{
	return object_ != tInvalidValue;
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
const T& ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::object() const
{
	return object_;
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
T& ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::resetObject(const bool needsRelease)
{
	release();
	object_ = tInvalidValue;

	needsRelease_ = needsRelease;

	return object_;
}

template <typename T, typename TReleaseValue, typename TReleaseReturn, TReleaseReturn (*tReleaseFunction)(TReleaseValue), typename NotVoidTyper<TReleaseReturn>::Type tExpectedReturnValue, bool tCheckReturnValue, T tInvalidValue>
void ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::release()
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
ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>& ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::operator=(ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>&& scopedObject)
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
ScopedObjectCompileTime<T, TReleaseValue, TReleaseReturn, tReleaseFunction, tExpectedReturnValue, tCheckReturnValue, tInvalidValue>::operator const T&() const
{
	return object_;
}

#endif

}

#endif // META_OCEAN_BASE_SCOPED_OBJECT_H
