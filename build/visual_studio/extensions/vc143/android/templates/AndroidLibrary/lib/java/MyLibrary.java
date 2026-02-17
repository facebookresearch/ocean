/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.example.$safeprojectname$;

import android.util.Log;

/**
 * This class implements the main library interface for $safeprojectname$.
 */
public class $safeprojectname$
{
	private static final String TAG = "$safeprojectname$";

	/// True if the library has been initialized.
	private static boolean libraryIsInitialized_ = false;

	/**
	 * Returns the library version string.
	 * @return The version string
	 */
	public static String version()
	{
		return "1.0.0";
	}

	/**
	 * Initializes the library.
	 * @return True if initialization succeeded
	 */
	public static boolean initialize()
	{
		if (libraryIsInitialized_)
		{
			return true;
		}

		Log.i(TAG, "Initializing $safeprojectname$");

		libraryIsInitialized_ = true;

		return true;
	}

	/**
	 * Releases library resources.
	 * @return True if shutdown succeeded
	 */
	public static boolean shutdown()
	{
		if (!libraryIsInitialized_)
		{
			return true;
		}

		Log.i(TAG, "Shutting down $safeprojectname$");

		libraryIsInitialized_ = false;

		return true;
	}
}
