/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SMART_OBJECT_REF_H
#define META_OCEAN_BASE_SMART_OBJECT_REF_H

#include "ocean/base/Base.h"
#include "ocean/base/ObjectRef.h"

namespace Ocean
{

/**
 * This template class implements a smart object reference which is a specialization of an ObjectRef object.
 * In the following the application of the SmartObjectRef class together with the ObjectRef class is shown:<br>
 * @code
 * // any kind of base class
 * class Base
 * {
 *     public:
 *
 *         void baseFunction();
 *
 *         virtual void virtualFunction();
 * };
 *
 * // any kind of derived class
 * class Derived : public Base
 * {
 *     public:
 *
 *         void derivedFunction();
 *
 *         virtual void virtualFunction();
 * };
 *
 * void main()
 * {
 *     // create a new object reference of type 'Base' holding an instance of type 'Base'
 *     ObjectRef<Base> base(new Base());
 *
 *     // check whether 'base' really holds a valid instance
 *     if (base)
 *     {
 *         // call both function of the instance
 *         base->baseFunction();
 *         base->virtualFunction();
 *     }
 *
 *     // create another object reference of type 'Base' holding an instance of type 'Derived'
 *     ObjectRef<Base> derived(new Derived());
 *
 *     // check whether 'derived' really holds a valid instance
 *     if (derived)
 *     {
 *         // call the base function
 *         derived->baseFunction();
 *
 *         // call the virtual function of the class 'Derived'
 *         derived->virtualFunction();
 *
 *         // try to call the specific 'derivedFunction' of class 'Derived'
 *         derived->derivedFunction(); // we will receive a compiler error as the object reference 'derived' has no knowledge about the class 'Derived'
 *     }
 *
 *     // we create a specialization of 'derived' so that we can finally access the specific function of class 'Derived'
 *     SmartObjectRef<Derived, Base> smartDerived(derived);
 *
 *     // check whether the provided object reference 'derived' could be specialized
 *     if (smartDerived)
 *     {
 *         // three valid function calls
 *         smartDerived->baseFunction();
 *         smartDerived->virtualFunction();
 *         smartDerived->derivedFunction();
 *     }
 * }
 * @endcode
 * @tparam T Type of the encapsulated object of the smart object reference, must be derived from TBase
 * @tparam TBase Base type of the object to be encapsulated
 * @see ObjectRef.
 * @ingroup base
 */
template <typename T, typename TBase>
class SmartObjectRef : public ObjectRef<TBase>
{
	public:

		/**
		 * Redefinition of the release callback function defined in ObjectRef.
		 */
		typedef typename ObjectRef<TBase>::ReleaseCallback ReleaseCallback;

	public:

		/**
		 * Creates a new SmartObjectRef with no internal object.
		 */
		SmartObjectRef() = default;

		/**
		 * Creates a new SmartObjectRef by a given object.
		 * This given object will be released by the smart object reference itself.
		 * @param object Internal object
		 */
		explicit inline SmartObjectRef(T* object);

		/**
		 * Creates a new SmartObjectRef by a given object.
		 * This given object will be released by the smart object reference itself.
		 * @param object Internal object
		 * @param releaseCallback Callback function for the release event
		 */
		inline SmartObjectRef(T* object, const ReleaseCallback& releaseCallback);

		/**
		 * Copy constructor.
		 * @param smartObjectRef SmartObjectRef object to copy
		 */
		inline SmartObjectRef(const SmartObjectRef<T, TBase>& smartObjectRef);

		/**
		 * Move constructor.
		 * @param smartObjectRef SmartObjectRef object to move
		 */
		inline SmartObjectRef(SmartObjectRef<T, TBase>&& smartObjectRef);

		/**
		 * Creates a new SmartObjectRef by a given ObjectRef.
		 * @param objectRef Given object reference
		 */
		inline SmartObjectRef(const ObjectRef<TBase>& objectRef);

		/**
		 * Copies a SmartObjectRef object.
		 * @param smartObjectRef SmartObjectRef object to copy
		 * @tparam T2 Type of the encapsulated object of the given object, must be derived from TBase
		 */
		template <typename T2>
		explicit inline SmartObjectRef(const SmartObjectRef<T2, TBase>& smartObjectRef);

		/**
		 * Assign operator.
		 * @param smartObjectRef Smart object reference to assign
		 * @return Reference to this object
		 */
		inline SmartObjectRef<T, TBase>& operator=(const SmartObjectRef<T, TBase>& smartObjectRef);

		/**
		 * Moves a smart object reference object to this smart object reference.
		 * @param smartObjectRef Smart object reference to move
		 * @return Reference to this object
		 */
		inline SmartObjectRef<T, TBase>& operator=(SmartObjectRef<T, TBase>&& smartObjectRef);

		/**
		 * Assigns a ObjectRef to this smart object reference
		 * @param objectRef ObjectRef to assign
		 * @return Reference to this SmartObjectRef object
		 */
		inline SmartObjectRef<T, TBase>& operator=(const ObjectRef<TBase>& objectRef);

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

	protected:

		/// Pointer to the internal object.
		T* objectPointer_ = nullptr;
};

template <typename T, typename TBase>
inline SmartObjectRef<T, TBase>::SmartObjectRef(T* object) :
	ObjectRef<TBase>(dynamic_cast<TBase*>(object))
{
	if (ObjectRef<TBase>::objectHolder_)
	{
		objectPointer_ = object;
	}
}

template <typename T, typename TBase>
inline SmartObjectRef<T, TBase>::SmartObjectRef(T* object, const ReleaseCallback& releaseCallback) :
	ObjectRef<TBase>(dynamic_cast<TBase*>(object, releaseCallback))
{
	if (ObjectRef<TBase>::objectHolder_ != nullptr)
	{
		objectPointer_ = object;
	}
}

template <typename T, typename TBase>
inline SmartObjectRef<T, TBase>::SmartObjectRef(const SmartObjectRef<T, TBase>& smartObjectRef) :
	ObjectRef<TBase>()
{
	if (smartObjectRef)
	{
		objectPointer_ = dynamic_cast<T*>(&*smartObjectRef);

		if (objectPointer_ != nullptr)
		{
			ObjectRef<TBase>::objectHolder_ = smartObjectRef.objectHolder_->ref();
		}
	}
}

template <typename T, typename TBase>
inline SmartObjectRef<T, TBase>::SmartObjectRef(SmartObjectRef<T, TBase>&& smartObjectRef) :
	ObjectRef<TBase>()
{
	if (smartObjectRef.objectPointer_ != nullptr)
	{
		objectPointer_ = smartObjectRef.objectPointer_;
		ObjectRef<TBase>::objectHolder_ = smartObjectRef.objectHolder_;

		smartObjectRef.objectPointer_ = nullptr;
		smartObjectRef.objectHolder_ = nullptr;
	}
}

template <typename T, typename TBase>
inline SmartObjectRef<T, TBase>::SmartObjectRef(const ObjectRef<TBase>& objectRef) :
	ObjectRef<TBase>()
{
	if (objectRef)
	{
		objectPointer_ = dynamic_cast<T*>(&*objectRef);

		if (objectPointer_ != nullptr)
		{
			ObjectRef<TBase>::objectHolder_ = objectRef.objectHolder_->ref();
		}
	}
}

template <typename T, typename TBase>
template <typename T2>
inline SmartObjectRef<T, TBase>::SmartObjectRef(const SmartObjectRef<T2, TBase>& smartObjectRef) :
	ObjectRef<TBase>()
{
	if (smartObjectRef)
	{
		objectPointer_ = dynamic_cast<T*>(&*smartObjectRef);

		if (objectPointer_ != nullptr)
		{
			ObjectRef<TBase>::objectHolder_ = smartObjectRef.objectHolder_->ref();
		}
	}
}

template <typename T, typename TBase>
inline SmartObjectRef<T, TBase>& SmartObjectRef<T, TBase>::operator=(const SmartObjectRef<T, TBase>& smartObjectRef)
{
	if (this != &smartObjectRef)
	{
		if (ObjectRef<TBase>::objectHolder_ != nullptr)
		{
			ocean_assert(objectPointer_ != nullptr);

			ObjectRef<TBase>::objectHolder_->unref();
			ObjectRef<TBase>::objectHolder_ = nullptr;
			objectPointer_ = nullptr;
		}

		ocean_assert(objectPointer_ == nullptr);

		if (smartObjectRef.objectPointer_ != nullptr)
		{
			objectPointer_ = smartObjectRef.objectPointer_;
			ObjectRef<TBase>::objectHolder_ = smartObjectRef.objectHolder_->ref();
		}
	}

	return *this;
}

template <typename T, typename TBase>
inline SmartObjectRef<T, TBase>& SmartObjectRef<T, TBase>::operator=(SmartObjectRef<T, TBase>&& smartObjectRef)
{
	if (this != &smartObjectRef)
	{
		if (ObjectRef<TBase>::objectHolder_ != nullptr)
		{
			ocean_assert(objectPointer_);

			ObjectRef<TBase>::objectHolder_->unref();
			ObjectRef<TBase>::objectHolder_ = nullptr;
			objectPointer_ = nullptr;
		}

		ocean_assert(objectPointer_ == nullptr);

		objectPointer_ = smartObjectRef.objectPointer_;
		ObjectRef<TBase>::objectHolder_ = smartObjectRef.objectHolder_;

		smartObjectRef.objectPointer_ = nullptr;
		smartObjectRef.objectHolder_ = nullptr;
	}

	return *this;
}

template <typename T, typename TBase>
inline SmartObjectRef<T, TBase>& SmartObjectRef<T, TBase>::operator=(const ObjectRef<TBase>& objectRef)
{
	if (ObjectRef<TBase>::objectHolder_ != nullptr)
	{
		ocean_assert(objectPointer_ != nullptr);

		ObjectRef<TBase>::objectHolder_->unref();
		ObjectRef<TBase>::objectHolder_ = nullptr;
		objectPointer_ = nullptr;
	}

	if (objectRef)
	{
		objectPointer_ = dynamic_cast<T*>(&*objectRef);

		if (objectPointer_ != nullptr)
		{
			ObjectRef<TBase>::objectHolder_ = objectRef.objectHolder_->ref();
		}
	}

	return *this;
}

template <typename T, typename TBase>
inline void SmartObjectRef<T, TBase>::release()
{
	ObjectRef<TBase>::release();
	objectPointer_ = nullptr;
}

template <typename T, typename TBase>
inline T* SmartObjectRef<T, TBase>::pointer() const
{
	return objectPointer_;
}

template <typename T, typename TBase>
inline T* SmartObjectRef<T, TBase>::operator->() const
{
	ocean_assert(objectPointer_ != nullptr);

	return objectPointer_;
}

template <typename T, typename TBase>
inline T& SmartObjectRef<T, TBase>::operator*() const
{
	ocean_assert(objectPointer_ != nullptr);

	return *objectPointer_;
}

}

#endif
