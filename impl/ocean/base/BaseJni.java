/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.base;

/**
 * This class defines the native interface to the Base library of the Ocean framework.
 * @ingroup basejni
 */
public class BaseJni
{
	/**
	 * Definition of different message output types.
	 * The values need to be in sync with Ocean::Messenger::MessageOutput.
	 */
	public enum MessageOutput
	{
		/// All messages will be discarded.
		OUTPUT_DISCARDED(0),		
		/// All messages are directed to the standard output immediately.
		OUTPUT_STANDARD(1 << 0),
		/// All messages are queued and must be popped from the message stack explicitly.
		OUTPUT_QUEUED(1 << 1),
		/// All messages are directed to a debug window.
		OUTPUT_DEBUG_WINDOW(1 << 2),
		/// All messages are directed to a file immediately.
		OUTPUT_FILE(1 << 3),
		/// All messages are directed to an arbitrary stream immediately.
		OUTPUT_STREAM(1 << 4),
		/// All messages are directed to the maintenance manager.
		OUTPUT_MAINTENANCE(1 << 5);

		/**
		 * Constructor for an integer value.
		 * @param value The integer value
		 */
		MessageOutput(int value)
		{
			value_ = value;
		}

		/**
		 * Returns the value of this enum.
		 * @return The enum's value
		 */
		public int value()
		{
			return value_;
		}

		/// The enum value.
		private final int value_;
	};

	/**
	 * Java native interface function to initialize the Ocean Framework.
	 * Several individual message output types can be specified and combined:
	 * - "OUTPUT_STANDARD": To write all messages to the standard output (e.g., std::cout on desktop platforms, or Android logcat on Android platforms).
	 * - "OUTPUT_QUEUED": To queue all messages and to explicitly pop the messages later (e.g., to display messages in a debug window).
	 * - <filename>: To write all messages to a define file.
	 * @param messageOutputType The type of the message output to be used, empty to use 'OUTPUT_STANDARD'
	 * @return True, if succeeded
	 * @see release().
	 */
	public static native boolean initialize(String messageOutputType);

	/**
	 * Java native interface function to initialize the Ocean Framework.
	 * @param messageOutputType The type of the message output to be used.
 	 * @param outputFile The name of the file to which messages will be written, 'messageOutputType' must contain 'OUTPUT_FILE', empty otherwise
	 * @return True, if succeeded
	 * @see release().
	 */
	public static native boolean initializeWithMessageOutput(int messageOutputType, String outputFile);

	/**
	 * Java native interface function to set the current activity.
	 * @param activity The current activity to be set or updated, can be nullptr to remove the previously set activity
	 * @return True, if succeeded
	 */
	public static native boolean setCurrentActivity(Object activity);

	/**
	 * Forces a specific number of processor cores.
	 * @param cores CPU cores to be forced during initialization
	 * @return True, if succeeded
	 */
	public static native boolean forceProcessorCoreNumber(int cores);

	/**
	 * Sets or changes the maximal capacity of the worker pool.
	 * @param capacity Maximal number of worker objects the worker pool may provide
	 * @return True, if succeeded
	 */
	public static native boolean setWorkerPoolCapacity(int capacity);

	/**
	 * Java native interface function to forward an debug message to the framework.
	 * @param message Debug message to forward
	 * @see warning(), error().
	 */
	public static native void debug(String message);

	/**
	 * Java native interface function to forward an information message to the framework.
	 * @param message Information message to forward
	 * @see warning(), error().
	 */
	public static native void information(String message);

	/**
	 * Java native interface function to forward an information message to the framework.
	 * @param message Information message to forward
	 * @see information(), error().
	 */
	public static native void warning(String message);

	/**
	 * Java native interface function to forward an information message to the framework.
	 * @param message Information message to forward
	 * @return True, if succeeded
	 * @see information(), warning().
	 */
	public static native void error(String message);

	/**
	 * Pops all messages that a currently waiting in the message queue.
	 * @param env JNI environment
	 * @param javaThis JNI object
	 * @return Message that have been poped.
	 */
	public static native String popMessages();

	/**
	 * Exits the application by invoking the _exit() command.
	 * Beware: Commonly Android decides whether an application will be terminated or not so that this function should be a workaround only.
	 * @param exitValue The exit value to be used
	 */
	public static native void exit(int exitValue);
}
