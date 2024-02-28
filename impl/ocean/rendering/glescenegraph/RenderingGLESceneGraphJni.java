// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

package com.facebook.ocean.rendering.glescenegraph;

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
