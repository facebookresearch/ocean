// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.scenedescription.sdl.obj;

/**
 * This class defines the native interface to the (SDL) OBJ library of the SceneDescription branch of the Ocean framework.
 * @ingroup scenedescriptionsdlobjjni
 */
public class SceneDescriptionSDLOBJJni
{
	/**
	 * Java native interface function to register the (SDL) OBJ SceneDescription component.
	 * @return True, if succeeded
	 * @see unregisterLibrary().
	 */
	public static native boolean registerLibrary();

	/**
	 * Java native interface function to unregister the (SDL) OBJ SceneDescription component.
	 * @return True, if succeeded
	 * @see registerLibrary.
	 */
	public static native boolean unregisterLibrary();
}
