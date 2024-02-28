// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.scenedescription.sdx.x3d;

/**
 * This class defines the native interface to the (SDX) X3D library of the SceneDescription branch of the Ocean framework.
 * @ingroup scenedescriptionsdxx3d
 */
public class SceneDescriptionSDXX3DJni
{
	/**
	 * Java native interface function to register the (SDX) X3D SceneDescription component.
	 * @return True, if succeeded
	 * @see unregisterLibrary().
	 */
	public static native boolean registerLibrary();

	/**
	 * Java native interface function to unregister the (SDX) X3D SceneDescription component.
	 * @return True, if succeeded
	 * @see registerLibrary.
	 */
	public static native boolean unregisterLibrary();
}
