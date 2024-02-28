// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.scenedescription.sdl.assimp;

/**
 * This class defines the native interface to the (SDL) Assimp library of the SceneDescription branch of the Ocean framework.
 * @ingroup renderingglescenegraphjni
 */
public class SceneDescriptionSDLAssimpJni
{
	/**
	 * Java native interface function to register the (SDL) Assimp SceneDescription component.
	 * @return True, if succeeded
	 * @see unregisterLibrary().
	 */
	public static native boolean registerLibrary();

	/**
	 * Java native interface function to unregister the (SDL) Assimp SceneDescription component.
	 * @return True, if succeeded
	 * @see registerLibrary.
	 */
	public static native boolean unregisterLibrary();
}
