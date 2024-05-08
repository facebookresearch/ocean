/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_BASE_SIGNAL_H
#define META_OCEAN_BASE_SIGNAL_H

#include "ocean/base/Base.h"

#if defined(_ANDROID) || defined(__linux__) || defined(__EMSCRIPTEN__)
	#include <semaphore.h>
#endif

#ifdef __APPLE__
	#include <dispatch/dispatch.h>
#endif

#include <atomic>

namespace Ocean
{

/**
 * This class implements a signal.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Signal
{
	friend class Signals;

	public:

		/**
		 * Creates a new signal object.
		 */
		Signal();

		/**
		 * Destructs a signal object.
		 */
		~Signal();

		/**
		 * Returns after the signal has been pulsed.
		 */
		void wait() const;

		/**
		 * Returns after the signal has been pulsed or the specified time elapsed.
		 * @param time Maximal wait time in ms
		 * @return True, if the signal has been pulsed, False otherwise
		 */
		bool wait(const unsigned int time) const;

		/**
		 * Pulses this signal.
		 * @return True, if succeeded
		 */
		bool pulse();

		/**
		 * Resets the signal.
		 * @return True, if succeeded
		 */
		bool reset();

		/**
		 * Pulses the signal for the last time to allow a waiting process to proceed and releases the signal.
		 */
		void release();

#ifdef _WINDOWS

		/**
		 * Returns the signal handle.
		 * @return Signal handle
		 */
		inline void* handle() const;

#endif

	protected:

		/**
		 * Disabled copy constructor.
		 * @param otherSignal Object which would be copied
		 */
		Signal(const Signal& otherSignal) = delete;

		/**
		 * Disabled assign operator.
		 * @param otherSignal Object which would be copied
		 * @return Reference to this object
		 */
		Signal& operator=(const Signal& otherSignal) = delete;

#if defined(_WINDOWS)

		/// Signal handle for Windows platforms.
		void* signalHandle;

#elif defined(__APPLE__)

		/// Semaphore object for Apple platforms.
		dispatch_semaphore_t semaphoreObject;

		/// True, if the semaphore object has been released and should not be used anymore.
		std::atomic<bool> semaphoreObjectReleased;

#else

		/// Semaphore object for e.g., Android platforms.
		mutable sem_t semaphoreObject;

		/// True, if the semaphore object (for e.g., Android platforms) is valid and has been initialized.
		bool semaphoreObjectState;

		/// True, if the semaphore object (for e.g., Android platforms) has been released and should not be used anymore.
		std::atomic<bool> semaphoreObjectReleased;

#endif
};

/**
 * This class implements a signal array.
 * @ingroup base
 */
class OCEAN_BASE_EXPORT Signals
{
	public:

		/**
		 * Creates an empty signal array.
		 */
		Signals();

		/**
		 * Creates a new signal array object by the number of requested signals.
		 * @param size Number of requested signals
		 */
		explicit Signals(const unsigned int size);

		/**
		 * Destructs a signal array.
		 */
		~Signals();

		/**
		 * Returns after all signals has been pulsed.
		 */
		void wait() const;

		/**
		 * Returns after a subset of signals has been pulsed.
		 * @param signalsCount Number of the first signals to wait for
		 */
		void waitSubset(const unsigned int signalsCount) const;

		/**
		 * Returns after all signals has been pulsed or the specified time has been elapsed.
		 * @param time Maximal wait time in ms
		 * @return True, if all signals has been pulsed
		 */
		bool wait(const unsigned int time) const;

		/**
		 * Returns after a subset of signals has been pulsed or the specified time has been elapsed.
		 * @param signalsCount Number of the first signals to wait for
		 * @param time Maximal wait time in ms
		 * @return True, if all specified signals has been pulsed
		 */
		bool waitSubset(const unsigned int signalsCount, const unsigned int time);

		/**
		 * Pulses all signals.
		 * @return True, if succeeded
		 */
		bool pulse();

		/**
		 * Resets all the signal.
		 * @return True, if succeeded
		 */
		bool reset();

		/**
		 * Returns the number of signals.
		 * @return Signal number
		 */
		inline unsigned int size() const;

		/**
		 * Changes the size of the signal array.
		 * All existing signals will be released before the new array is created.
		 * @param size New signal array size
		 */
		void setSize(const unsigned int size);

		/**
		 * Returns one specific signal of this array.
		 * @param index Signal index to return
		 * @return Specified signal
		 */
		Signal& operator[](const unsigned int index) const;

	private:

		/**
		 * Disabled copy constructor.
		 * @param otherSignals Object which would be copied
		 */
		Signals(const Signals& otherSignals) = delete;

		/**
		 * Disabled copy operator.
		 * @param otherSignals Object which would be copied
		 * @return Reference to this object
		 */
		Signals& operator=(const Signals& otherSignals) = delete;

	private:

		/// Array of stored signals.
		Signal* signalObjects;

		/// Number of signals stored in the array.
		unsigned int signalsSize;
};

#ifdef _WINDOWS

inline void* Signal::handle() const
{
	return signalHandle;
}

#endif

inline unsigned int Signals::size() const
{
	return signalsSize;
}

}

#endif // META_OCEAN_BASE_SIGNAL_H
