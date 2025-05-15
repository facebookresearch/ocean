/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.platform.android.application;

import android.content.Context;

/**
 * This class implements an OpenGL surface view able to visualze frames.
 * @ingroup platformandroid
 */
public class GLRendererView extends GLView
{
	/**
	 * Creates a new view object.
	 * @param context Application context object
	 */
	public GLRendererView(Context context)
	{
		super(context);

		registerInstance();
	}

	public GLRendererView(Context context, boolean translucent, int depth, int stencil)
	{
		super(context, translucent, depth, stencil);

		registerInstance();
	}

	/**
	 * Java native interface function to set or change the view's horizontal field of view.
	 * @param angle New horizontal field of view in radian
	 * @return True, if succeeded
	 */
	public static native boolean setFovX(double angle);

	/**
	 * Java native interface functio to set or change the view's background color
	 * @param red Red color value, with range [0.0, 1.0]
	 * @param green Green color value, with range [0.0, 1.0]
	 * @param blue Blue color value, with range [0.0, 1.0]
	 * @return True, if succeeded
	 */
	public static native boolean setBackgroundColor(double red, double green, double blue);

	/**
	 * Java native interface function to register the instance function for the corresponding native C++ class.
	 */
	private static native void registerInstance();
}
