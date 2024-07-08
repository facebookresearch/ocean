/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.scenedescription.sdl.obj;

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
