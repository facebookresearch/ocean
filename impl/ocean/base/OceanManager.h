/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_OCEAN_MANAGER_H
#define META_OCEAN_BASE_OCEAN_MANAGER_H

#include "ocean/base/Base.h"
#include "ocean/base/Lock.h"

namespace Ocean
{

/**
 * This class implements the basic manager for the Ocean framework.
 * The manager is implemented as singleton while it is not derived from the Singleton class of the Ocean framework.<br>
 * This manager allows to explicitly release all resources that are connected with objects derived by the Singleton class.<br>
 * However, normally there is no need to release the resources explicitly as all resources will be released automatically during the call of std::atexit.<br>
 * Therefore, use the shutdown function of this manager only if there is the explicit need for releasing all existing Ocean framework resources at a specific moment in your application.<br>
 * @see Singleton
 * @ingroup base
 */
class OCEAN_BASE_EXPORT OceanManager
{
	public:

		/**
		 * Definition of a function pointer that releases a singleton object.
		 */
		typedef void (*SingletonDestroyFunction)();

	private:

		/**
		 * Definition of a vector holding functions to release singleton objects.
		 */
		typedef std::vector<SingletonDestroyFunction> SingletonDestroyFunctions;

	public:

		/**
		 * Returns a reference to the OceanManager object.
		 * @return Reference to the manager
		 */
		static OceanManager& get();

		/**
		 * Registers a new singleton object.
		 * Every registered singleton object is released by this manager if:<br>
		 * a) the std::atexit function invokes the default shutdown of this manager<br>
		 * b) the shutdown() function of this manager object (and all other manager objects in the remaining standalone dynamic libraries) are invoked explicitly<br>
		 * @param singletonDestroyFunction The function pointer that can be invoked to release the singleton, must be valid
		 */
		void registerSingleton(const SingletonDestroyFunction& singletonDestroyFunction);

		/**
		 * Explicit shutdown of all Ocean framework resources that are handled by the Singleton class implementation.
		 * Beware: Do not call this shutdown function unless all Singletons must be released at a specific moment in the application.
		 */
		void shutdown();

	private:

		/**
		 * Creates a new manager object.
		 * This constructor is protected as it must not be created by a constructor directly.
		 */
		OceanManager();

		/**
		 * Disabled copy constructor.
		 * @param oceanManager The manager object that would be copied
		 */
		OceanManager(const OceanManager& oceanManager) = delete;

		/**
		 * Destructs this manager object.
		 */
		~OceanManager();

		/**
		 * Internal callback function for a std::atexit call.
		 */
		static void internalRelease();

		/**
		 * Disabled assign operator.
		 * @param oceanManager The manager object that would be copied
		 */
		OceanManager& operator=(const OceanManager& oceanManager) = delete;

	private:

		/// Lock of this manager.
		Lock lock_;

		/// The pointers to release functions for all registered singletons.
		SingletonDestroyFunctions singletonDestroyFunctions_;
};

}

#endif // META_OCEAN_BASE_OCEAN_MANAGER_H
