/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.scenedescription.sdl.assimp;

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
