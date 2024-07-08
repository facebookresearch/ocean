/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.platform.android.application;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

/**
 * This class implements an OpenGL surface view able to visualze frames.
 * @ingroup platformandroid
 */
public class GLFrameView extends GLView
{
	/**
	 * Creates a new view object.
	 * @param context Application context object
	 */
	public GLFrameView(Context context)
	{
		super(context);

		registerInstance();
	}

	public GLFrameView(Context context, boolean translucent, int depth, int stencil)
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
	 * Java native interface function to set or change the view's background media object.
	 * @param url URL of the media object to be used as background medium
	 * @param type Hint defining the media object in more detail, possible values are "LIVE_VIDEO", "IMAGE", "MOVIE", "IMAGE_SEQUENCE" or "FRAME_STREAM", if no hint is given the first possible media object will be created
	 * @param preferredWidth Preferred width of the medium in pixel, use 0 to use the default width
	 * @param preferredHeight Preferred height of the medium in pixel, use 0 to use the default height
	 * @param adjustFov True, to adjust the view's field of view to the field of the background automatically
	 * @return True, if succeeded
	 */
	public static native boolean setFrameMedium(String url, String type, int preferredWidth, int preferredHeight, boolean adjustFov);

	/**
	 * Java native interface function to register the instance function for the corresponding native C++ class.
	 */
	private static native void registerInstance();
}
