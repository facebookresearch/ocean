// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.app.shark.android;

/**
 * This class implements the native interface for Shark specific functions.
 * @ingroup comoceanshark
 */
public class NativeInterfaceShark
{
	/**
	 * Java native interface function telling the application view that it has been initialized successfully.
	 */
	public static native void initializedView();

	/**
	 * Java native interface function loading a new scene file.
	 * @param filename Filename of the scene to be loaded
	 * @param replace State determining whether already existing scenes will be removed first, or whether the scene will be added instead
	 * @return True, if succeeded
	 */
	public static native boolean loadScene(String filename, boolean replace);
}
