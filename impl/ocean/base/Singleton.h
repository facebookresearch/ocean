/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SINGLETON_H
#define META_OCEAN_BASE_SINGLETON_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"
#include "ocean/base/OceanManager.h"

#include <cstdlib>

namespace Ocean
{

/**
 * This template class is the base class for all singleton objects.
 * Each object derived from this class can only have at most one instance at the same time within the same process unless standalone dynamic libraries attached to the process.<br>
 * By default, in the case a standalone dynamic library is attached, this library has it's own global variables and thus also provides own singleton objects.<br>
 * If only one unique process-wide singleton is desired for applications with standalone dynamic libraries, the OceanManager object needs to be initialized accordingly directly at process beginning and directly during initialization of the dynamic library.<br>
 *
 * Beware: The derived class must not be accomplished entirely with inline functions.<br>
 * Further, the derived class has to defined the Singleton base class as friend class.<br>
 * @tparam T Class type that will be extended as singleton
 * @see OceanManager.
 *
 * See this tutorial:
 * @code
 * // Any class using the thread class as base class.
 * class DerivedClass : public Singleton<DerivedClass>
 * {
 *     friend class Singleton<DerivedClass>;
 *
 *     public:
 *
 *        // Any function.
 *        void anyFunction()
 *        {
 *            // do something here
 *        }
 *
 *     private:
 *
 *        // Private constructor ensuring that this object can only be created by the singleton mechanism.
 *        DerivedClass()
 *        {
 *            // initialize something here
 *        }
 *
 *        // Private destructor ensuring that this object can only be disposed by the singleton mechanism.
 *        ~DerivedClass()
 *        {
 *            // release something here
 *        }
 * };
 *
 * void anywhereInYourCode()
 * {
 *     // accessing the singleton object anywhere in your code
 *     DerivedClass::get().anyFunction();
 * }
 * @endcode
 * @ingroup base
 */
template <typename T>
class Singleton
{
	public:

		/**
		 * Returns a reference to the unique object.
		 * @return Reference to the object
		 */
		static T& get();

	protected:

		/**
		 * Default constructor.
		 */
		Singleton() = default;

	private:

		/**
		 * Disabled copy constructor.
		 * @param singleton The singleton object that would be copied
		 */
		Singleton(const Singleton<T>& singleton) = delete;

		/**
		 * Returns a pointer to the unique object.
		 * @return Pointer to the object
		 */
		static T* internalGet();

		/**
		 * Releases the singleton object.
		 * Beware: This function should be call by the OceanManager only.
		 */
		static void releaseSingleton();

		/**
		 * Disabled assign operator.
		 * @param singleton The singleton object that would be assigned
		 */
		Singleton& operator=(const Singleton<T>& singleton) = delete;
};

template <typename T>
T& Singleton<T>::get()
{
	return *internalGet();
}

template <typename T>
T* Singleton<T>::internalGet()
{
	static T* singleton = nullptr;

	if (!singleton)
	{
		static Lock lock;
		const ScopedLock scopedLock(lock);

		if (!singleton)
		{
			singleton = new T();
			ocean_assert(singleton);

			OceanManager::get().registerSingleton(releaseSingleton);
		}
	}

	ocean_assert(singleton);
	return singleton;
}

template <typename T>
void Singleton<T>::releaseSingleton()
{
	ocean_assert(internalGet() != nullptr);
	delete internalGet();
}

}

#endif // FACEBOOK_SINGLETON_H
