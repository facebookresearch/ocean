/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SCHEDULER_H
#define META_OCEAN_BASE_SCHEDULER_H

#include "ocean/base/Base.h"
#include "ocean/base/Callback.h"
#include "ocean/base/Singleton.h"
#include "ocean/base/Thread.h"

#include <map>

namespace Ocean
{

/**
 * This class represents a round robin scheduler.
 * The scheduler is based on one single thread and executes all registered functions in a round robin manner.<br>
 * The registered functions are called with high frequency (e.g., each 10-50 ms).<br>
 * A registered function should return immediately so that a high call frequency for all registered function can be ensured.
 * @see ThreadPool, TaskQueue.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Scheduler :
	public Singleton<Scheduler>,
	protected Thread
{
	friend class Singleton<Scheduler>;

	public:

		/**
		 * Definition of a callback function.
		 */
		typedef Ocean::Callback<void> Callback;

	private:

		/**
		 * Definition of a callback pair holding a callback function and an active flag.
		 */
		typedef ConcurrentCallbacks<Callback> Callbacks;

	public:

		/**
		 * Registers a scheduler function.
		 * Each registered function must be unregistered by the caller if the scheduler event is not needed anymore.
		 * @param callback Callback function to register
		 * @see unregisterFunction().
		 */
		void registerFunction(const Callback& callback);

		/**
		 * Unregisters a scheduler function.
		 * This function may be called from a registered scheduler function.
		 * @param callback Callback function to unregister
		 * @see registerFunction().
		 */
		void unregisterFunction(const Callback& callback);

	protected:

		/**
		 * Creates a new scheduler.
		 */
		Scheduler();

		/**
		 * Destructs a scheduler.
		 */
		virtual ~Scheduler();

		/**
		 * Derived thread run function.
		 */
		virtual void threadRun();

	protected:

		/// Scheduler callbacks.
		Callbacks callbacks;
};

}

#endif // META_OCEAN_BASE_SCHEDULER_H
