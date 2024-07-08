/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.rendering.glescenegraph;

/**
 * This class defines the native interface to the GLESceneGraph library of the rendering branch of the Ocean framework.
 * @ingroup renderingglescenegraph
 */
public class RenderingGLESceneGraphJni
{
	/**
	 * Java native interface function to register the GLESceneGraph Rendering component.
	 * @return True, if succeeded
	 * @see unregisterLibrary()
	 */
	public static native boolean registerLibrary();

	/**
	 * Java native interface function to unregister the GLESceneGraph Rendering component.
	 * @return True, if succeeded
	 * @see registerLibrary()
	 */
	public static native boolean unregisterLibrary();
}
