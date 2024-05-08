/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_OBJECT_REF_H
#define META_OCEAN_BASE_OBJECT_REF_H

#include "ocean/base/Base.h"
#include "ocean/base/Callback.h"
#include "ocean/base/Lock.h"

#include <atomic>

namespace Ocean
{

/**
 * This template class implements a object reference with an internal reference counter.
 * The reference counter is thread-safe.
 *
 * Further, this implementation allows to define a callback function for release events.<br>
 * By application of the callback function, a manager can be implemented that allows to store several ObjectRef objects in a managed list.<br>
 * The manager can used this list to provide instances of specific ObjectRef objects on demand.<br>
 * Due to the callback function, the manager will be informed whenever an ObjectRef object can be removed from the managed list so that the real object (which is encapsulated by the ObjectRef) can be released automatically.<br>
 * The following code snippet demonstrates the application:
 * @code
 * // any class, struct or data type
 * class DataType
 * {
 *     public:
 *
 *         // any function
 *         int function(const double value);
 * };
 *
 * // we create a new ObjectRef instance
 * const ObjectRef<DataType> object(new DataType());
 *
 * if (!object.isNull())
 * {
 *     const int result = object->function(5.0);
 * }
 *
 * const ObjectRef<DataType> sameObject(object);
 *
 * ocean_assert(sameObject);
 * const int result2 = sameObject->function(5.0);
 * @endcode
 * @tparam T Type of the object to be encapsulated by this object reference
 * @see SmartObjectRef.
 * @ingroup base
 */
template <typename T>
class ObjectRef
{
	template <typename T2, typename TBase> friend class SmartObjectRef;
	friend class ObjectHolder;

	public:

		/**
		 * Definition of a release callback function.
		 * The first parameter determines the object for that the release event is invoked.<br>
		 */
		typedef Callback<void, const T*> ReleaseCallback;

	protected:

		/**
		 * This class implements a helper object for the actual object reference class.
		 * ObjectRef objects sharing the same encapsulated object share the same holder object of the encapsulated object.
		 * The holder is created only once for the very first ObjectRef object, while the pointer to this holder is shared to the subsequent ObjectRef objects (which access the same encapsulated object).
		 */
		class ObjectHolder
		{
			friend class ObjectRef<T>;

			public:

				/**
				 * Creates a new ObjectHolder object.
				 * @param object Pointer to the internal object
				 * @param releaseCallback Callback for release event
				 */
				explicit inline ObjectHolder(T* object, const ReleaseCallback& releaseCallback = ReleaseCallback());

				/**
				 * Increases the reference counter.
				 * @return Pointer to this object
				 */
				inline ObjectHolder* ref();

				/**
				 * Decreases the reference counter and disposes the encapsulated object and the holder itself if the reference counter reaches zero.
				 */
				void unref();

				/**
				 * Returns the number of references.
				 * @return Reference number
				 */
				inline unsigned int references() const;

			protected:

				/// Pointer to the internal object.
				T* object_ = nullptr;

				/// Reference counter of the internal object.
				std::atomic<unsigned int> atomicReferenceCounter_;

				/// Release callback
				ReleaseCallback callback_;
		};

	public:

		/**
		 * Creates an empty ObjectRef object.
		 */
		ObjectRef() = default;

		/**
		 * Copy constructor.
		 * Copies an ObjectRef object.
		 * @param objectRef ObjectRef to copy
		 */
		inline ObjectRef(const ObjectRef<T>& objectRef);

		/**
		 * Move constructor.
		 * @param object The object to be moved
		 */
		inline ObjectRef(ObjectRef<T>&& object) noexcept;

		/**
		 * Creates a new ObjectRef holding a given object.
		 * Beware: The given object will be released by this object reference!
		 * @param object The object to store
		 */
		explicit inline ObjectRef(T* object);

		/**
		 * Creates a new ObjectRef holding and managing a given object.
		 * This constructor also requests a release callback event.<br>
		 * This callback event will be invoked after the internal reference counter of this object has been decremented and is equal to 1 afterwards.<br>
		 * Thus, the release callback provides an information that in the moment of the callback only one instance of the ObjectRef exists.<br>
		 * Therefore, this callback can be used to release an ObjectRef object which is stored in e.g. a managed list so that finally the reference counter will be zero and the stored object will be released.<br>
		 * Beware: The given object will be released by this object reference!
		 * @param object The object to store
		 * @param releaseCallback Callback function which will be invoked if only one ObjectRef instance managing a specific object is left (if more than one existed before)
		 */
		inline ObjectRef(T* object, const ReleaseCallback& releaseCallback);

		/**
		 * Destructs an object reference object and releases the internal object if possible.
		 */
		inline ~ObjectRef();

		/**
		 * Returns a reference to the internal object forcing to a specified type.
		 * Beware: Check whether this reference holds a valid internal object before calling this function!<br>
		 * Beware: Make sure the forced type is matching the internal object!
		 * @return Internal object with the forced type
		 */
		template <typename T2> inline T2& force() const;

		/**
		 * Returns a point to the internal object if existing.
		 * Beware: Check whether this reference holds an internal object before calling this function!
		 * @return Pointer to the internal object
		 */
		inline T* operator->() const;

		/**
		 * Returns a reference to the internal object if existing.
		 * Beware: Check whether this reference holds an internal object before calling this function!
		 * @return Reference to the internal object
		 */
		inline T& operator*() const;

		/**
		 * Returns whether there is no other object reference but this one.
		 * @return True, if so
		 */
		inline bool isUnique() const;

		/**
		 * Returns whether this object reference holds no internal object.
		 * @return True, if so
		 */
		inline bool isNull() const;

		/**
		 * Releases the internal object, if any.
		 * Beware: After the release the object can not be accessed anymore!
		 */
		inline void release();

		/**
		 * Returns a pointer to the objects that is encapsulated by this wrapper.
		 * @return Pointer to the object or nullptr if no object is encapsulated
		 */
		inline T* pointer() const;

		/**
		 * Assign operator.
		 * @param objectRef Right object reference to assign
		 * @return Reference to this object reference
		 */
		inline ObjectRef<T>& operator=(const ObjectRef<T>& objectRef);

		/**
		 * Move operator.
		 * @param right The right object to assign
		 * @return Reference to this object
		 */
		inline ObjectRef<T>& operator=(ObjectRef<T>&& right) noexcept;

		/**
		 * Returns whether two object references are holds the same internal object.
		 * @param objectRef Right object reference
		 * @return True, if so
		 */
		inline bool operator==(const ObjectRef<T>& objectRef) const;

		/**
		 * Returns whether two object references are not equal.
		 * @param objectRef Right object reference
		 * @return True, if so
		 */
		inline bool operator!=(const ObjectRef<T>& objectRef) const;

		/**
		 * Returns whether the left object is less than the right one.
		 * @param objectRef Right operand
		 * @return True, if so
		 */
		inline bool operator<(const ObjectRef<T>& objectRef) const;

		/**
		 * Returns whether this object reference holds an internal object.
		 * @return True, if so
		 */
		explicit inline operator bool() const;

	protected:

		/**
		 * Destroys the object located in the ObjectHolder object.
		 * This function is used to get access to the protected delete operator of the internal encapsulated object.
		 * @param object the object to destroy
		 */
		static inline void destroyObject(T* object);

	protected:

		/// Pointer to the object holder.
		ObjectHolder* objectHolder_ = nullptr;
};

template <typename T>
inline ObjectRef<T>::ObjectHolder::ObjectHolder(T* newObject, const ReleaseCallback& releaseCallback) :
	object_(newObject),
	atomicReferenceCounter_(1u),
	callback_(releaseCallback)
{
	// nothing to do here
}

template <typename T>
inline typename ObjectRef<T>::ObjectHolder* ObjectRef<T>::ObjectHolder::ref()
{
	ocean_assert(atomicReferenceCounter_ != 0u);
	++atomicReferenceCounter_;

	return this;
}

template <typename T>
void ObjectRef<T>::ObjectHolder::unref()
{
	ocean_assert(atomicReferenceCounter_ != 0u);

	const unsigned int newReferenceCount = atomicReferenceCounter_.fetch_sub(1u) - 1u;

	if (newReferenceCount == 1u && callback_)
	{
		// from this point on the reference counter cannot (and also must not) be decremented from any caller but from the object which receives the callback

		ocean_assert(object_);
		callback_(object_);
		return;
	}

	if (newReferenceCount == 0u)
	{
		// from this point on there is the guarantee that the no party is interested in the encapsulated object anymore as all corresponding ObjectRef instances have been disposed already

		ObjectRef<T>::destroyObject(object_);

#ifdef OCEAN_DEBUG
		object_ = nullptr;
#endif

		delete this;
	}
}

template <typename T>
inline unsigned int ObjectRef<T>::ObjectHolder::references() const
{
	return atomicReferenceCounter_;
}

template <typename T>
inline ObjectRef<T>::ObjectRef(const ObjectRef<T>& objectRef)
{
	if (objectRef.objectHolder_ != nullptr)
	{
		objectHolder_ = objectRef.objectHolder_->ref();
	}
}

template <typename T>
inline ObjectRef<T>::ObjectRef(ObjectRef<T>&& object) noexcept :
	objectHolder_(object.objectHolder_)
{
	object.objectHolder_ = nullptr;
}

template <typename T>
inline ObjectRef<T>::ObjectRef(T* object)
{
	if (object != nullptr)
	{
		objectHolder_ = new ObjectHolder(object);
	}
}

template <typename T>
inline ObjectRef<T>::ObjectRef(T* object, const ReleaseCallback& releaseCallback)
{
	if (object != nullptr)
	{
		objectHolder_ = new ObjectHolder(object, releaseCallback);
	}
}

template <typename T>
inline ObjectRef<T>::~ObjectRef()
{
	if (objectHolder_ != nullptr)
	{
		objectHolder_->unref();
	}
}

template <typename T>
template <typename T2>
inline T2& ObjectRef<T>::force() const
{
	ocean_assert(objectHolder_ != nullptr);
	ocean_assert(objectHolder_->object_ != nullptr);
	ocean_assert(dynamic_cast<T2*>(objectHolder_->object_) != nullptr);

	return dynamic_cast<T2&>(*objectHolder_->object_);
}

template <typename T>
inline T* ObjectRef<T>::operator->() const
{
	ocean_assert(objectHolder_ != nullptr);
	ocean_assert(objectHolder_->object_);
	return objectHolder_->object_;
}

template <typename T>
inline T& ObjectRef<T>::operator*() const
{
	ocean_assert(objectHolder_ != nullptr);
	ocean_assert(objectHolder_->object_);
	return *objectHolder_->object_;
}

template <typename T>
inline bool ObjectRef<T>::isNull() const
{
	return objectHolder_ == nullptr;
}

template <typename T>
inline bool ObjectRef<T>::isUnique() const
{
	if (objectHolder_)
	{
		return objectHolder_->references() == 1u;
	}

	return true;
}

template <typename T>
inline void ObjectRef<T>::release()
{
	if (objectHolder_)
	{
		objectHolder_->unref();
		objectHolder_ = nullptr;
	}
}

template <typename T>
inline T* ObjectRef<T>::pointer() const
{
	ocean_assert(!objectHolder_ || objectHolder_->object_);
	return objectHolder_ ? objectHolder_->object_ : nullptr;
}

template <typename T>
inline ObjectRef<T>& ObjectRef<T>::operator=(const ObjectRef<T>& objectRef)
{
	if (objectHolder_)
	{
		objectHolder_->unref();
		objectHolder_ = nullptr;
	}

	if (objectRef.objectHolder_)
	{
		objectHolder_ = objectRef.objectHolder_->ref();
	}

	return *this;
}

template <typename T>
inline ObjectRef<T>& ObjectRef<T>::operator=(ObjectRef<T>&& right) noexcept
{
	if (this != &right)
	{
		if (objectHolder_)
		{
			objectHolder_->unref();
		}

		objectHolder_ = right.objectHolder_;
		right.objectHolder_ = nullptr;
	}

	return *this;
}

template <typename T>
inline bool ObjectRef<T>::operator==(const ObjectRef<T>& objectRef) const
{
	if (!objectHolder_ && !objectRef.objectHolder_)
	{
		return true;
	}

	if (objectHolder_ && objectRef.objectHolder_)
	{
		return objectHolder_->object_ == objectRef.objectHolder_->object_;
	}

	return false;
}

template <typename T>
inline bool ObjectRef<T>::operator!=(const ObjectRef<T>& objectRef) const
{
	return !(*this == objectRef);
}

template <typename T>
inline ObjectRef<T>::operator bool() const
{
	return objectHolder_ != nullptr;
}

template <typename T>
void ObjectRef<T>::destroyObject(T* object)
{
	delete object;
}

template <typename T>
inline bool ObjectRef<T>::operator<(const ObjectRef& objectRef) const
{
	if (objectHolder_ && objectRef.objectHolder_)
	{
		return objectHolder_->object_ < objectRef.objectHolder_->object_;
	}

	if (objectHolder_ && !objectRef.objectHolder_)
	{
		return false;
	}

	return !objectHolder_ && objectRef.objectHolder_;
}

}

#endif // META_OCEAN_BASE_OBJECT_REF_H
