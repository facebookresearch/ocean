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

import com.meta.ocean.media.android.MediaAndroidJni;
import com.meta.ocean.platform.android.*;

/**
 * This class implements an OpenGLES surface view.
 * @ingroup platformandroid
 */
public class GLView extends GLSurfaceView
{
	/**
	 * Implementation of the renderer
	 */
	private class NativeRenderer implements GLSurfaceView.Renderer
	{
		/**
		 * Render the current frame.
		 * @param gl The GL interface
		 */
		@Override
		public void onDrawFrame(GL10 gl)
		{
			// we update the Android Media system with each new render call
			MediaAndroidJni.update();

			// calling the native render function
			render();
		}

		/**
		 * Changes the size of the surface.
		 * @param gl The GL interface
		 * @param width The width of the surface in pixel
		 * @param height The height of the surface in pixel
		 */
		@Override
		public void onSurfaceChanged(GL10 gl, int width, int height)
		{
			// calling the native resize function
			resize(width, height);
		}

		@Override
		public void onSurfaceCreated(GL10 gl, EGLConfig config)
		{
			// calling the native initialize function
			initialize();
		}
	}

	/**
	 * Creates a new view object.
	 * @param context Application context object
	 */
	public GLView(Context context)
	{
		super(context);
		init(false, 0, 0);

        registerInstance();
	}

	public GLView(Context context, boolean translucent, int depth, int stencil)
	{
		super(context);
		init(translucent, depth, stencil);

        registerInstance();
	}

	private void init(boolean translucent, int depth, int stencil)
	{
		/* By default, GLSurfaceView() creates a RGB_565 opaque surface.
		 * If we want a translucent one, we should change the surface's
		 * format here, using PixelFormat.TRANSLUCENT for GL Surfaces
		 * is interpreted as any 32-bit surface with alpha by SurfaceFlinger.
		 */
		if (translucent)
		{
			getHolder().setFormat(PixelFormat.TRANSLUCENT);
		}

		/* Setup the context factory for 3.0 rendering.
		 * See ContextFactory class definition below
		 */
		setEGLContextFactory(new ContextFactory());

		/* We need to choose an EGLConfig that matches the format of
		 * our surface exactly. This is going to be done in our
		 * custom config chooser. See ConfigChooser class definition below.
		 */
		setEGLConfigChooser(translucent ? new ConfigChooser(8, 8, 8, 8, depth, stencil) : new ConfigChooser(5, 6, 5, 0, depth, stencil));

		/* Set the renderer responsible for frame rendering */
		setRenderer(new NativeRenderer());
	}

	@Override
	public void onPause()
	{
		Log.d("Ocean", "GLView::onPause()");

		super.onPause();
	}

	private static class ContextFactory implements GLSurfaceView.EGLContextFactory
	{
		private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

		public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig)
		{
			Log.i(getClass().getSimpleName(), "creating OpenGL ES 3.0 context");

			checkEglError("Before eglCreateContext", egl);

			int[] attrib_list = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL10.EGL_NONE};
			EGLContext context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT, attrib_list);

			if (context == null)
			{
				Log.w(getClass().getSimpleName(), "FAILED to create EGL context");
			}

			checkEglError("After eglCreateContext", egl);
			return context;
		}

		public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context)
		{
			Log.i(getClass().getSimpleName(), "destroyed OpenGL ES 3.0 context");
			egl.eglDestroyContext(display, context);
		}
	}

	private static void checkEglError(String prompt, EGL10 egl)
	{
		int error;
		while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS)
		{
			Log.e("GLFrameView", String.format("%s: EGL error: 0x%x", prompt, error));
		}
	}

	private static class ConfigChooser implements GLSurfaceView.EGLConfigChooser
	{
		public ConfigChooser(int r, int g, int b, int a, int depth, int stencil)
		{
			mRedSize = r;
			mGreenSize = g;
			mBlueSize = b;
			mAlphaSize = a;
			mDepthSize = depth;
			mStencilSize = stencil;
		}

		/* This EGL config specification is used to specify 2.0 rendering.
		 * We use a minimum size of 4 bits for red/green/blue, but will
		 * perform actual matching in chooseConfig() below.
		 */
		private static int EGL_OPENGL_ES2_BIT = 4;
		private static int[] s_configAttribs2 =
		{
			EGL10.EGL_RED_SIZE, 4,
			EGL10.EGL_GREEN_SIZE, 4,
			EGL10.EGL_BLUE_SIZE, 4,
			EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL10.EGL_NONE
		};

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
		{

			/* Get the number of minimally matching EGL configurations
			 */
			int[] num_config = new int[1];
			egl.eglChooseConfig(display, s_configAttribs2, null, 0, num_config);

			int numConfigs = num_config[0];

			if (numConfigs <= 0)
			{
				throw new IllegalArgumentException("No configs match configSpec");
			}

			/* Allocate then read the array of minimally matching EGL configs
			 */
			EGLConfig[] configs = new EGLConfig[numConfigs];
			egl.eglChooseConfig(display, s_configAttribs2, configs, numConfigs, num_config);

			/* Now return the "best" one
			 */
			return chooseConfig(egl, display, configs);
		}

		public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs)
		{
			for(EGLConfig config : configs)
			{
				int d = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
				int s = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, 0);

				// We need at least mDepthSize and mStencilSize bits
				if (d < mDepthSize || s < mStencilSize)
				{
					continue;
				}

				// We want an *exact* match for red/green/blue/alpha
				int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
				int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
				int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
				int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);

				if (r == mRedSize && g == mGreenSize && b == mBlueSize && a == mAlphaSize)
				{
					return config;
				}
			}

			return null;
		}

		private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config, int attribute, int defaultValue)
		{
			if (egl.eglGetConfigAttrib(display, config, attribute, mValue))
			{
				return mValue[0];
			}

			return defaultValue;
		}

		// Subclasses can adjust these values:
		protected int mRedSize;
		protected int mGreenSize;
		protected int mBlueSize;
		protected int mAlphaSize;
		protected int mDepthSize;
		protected int mStencilSize;
		private int[] mValue = new int[1];
	}

	/**
	 * Touch event function.
	 * @param event Touch event
	 * @return True, if succeeded
	 */
	public boolean onTouchEvent(MotionEvent event)
	{
		super.onTouchEvent(event);

		switch (event.getAction())
		{
			case MotionEvent.ACTION_DOWN:
				onTouchDown((int)event.getX(), (int)event.getY());
				break;

			case MotionEvent.ACTION_MOVE:
				onTouchMove((int)event.getX(), (int)event.getY());
				break;

			case MotionEvent.ACTION_UP:
				onTouchUp((int)event.getX(), (int)event.getY());
				break;
		}

		return true;
	}

	/**
	 * Java native interface function to initialize the view.
	 * @return True, if succeeded
	 */
	public static native boolean initialize();

	/**
	 * Java native interface function to release the application view.
	 * @return True, if succeeded
	 */
	public static native boolean release();

    /**
	 * Native interface function to resize the framebuffer.
	 * @param width The width of the new framebuffer in pixel
	 * @param height The height of the new framebuffer in pixel
	 * @return True, if succeeded
	 */
	public static native boolean resize(int width, int height);

	/**
	 * Native interface function to render the next frame.
	 * @return True, if succeeded
	 */
	public static native boolean render();

	/**
	 * Java native interface function for touch down events.
	 * @param x Horizontal touch position in pixel
	 * @param y Vertical touch position in pixel
	 * @see touchMove(), touchUp().
	 */
	public static native void onTouchDown(float x, float y);

	/**
	 * Java native interface function for (touch) move events.
	 * @param x Horizontal (touch) move position in pixel
	 * @param y Vertical (touch) move position in pixel
	 * @see touchDown(), touchUp().
	 */
	public static native void onTouchMove(float x, float y);

	/**
	 * Java native interface function for touch up events.
	 * @param x Horizontal touch position in pixel
	 * @param y Vertical touch position in pixel
	 */
	public static native void onTouchUp(float x, float y);

	/**
	 * Java native interface function for resume event sent from the owning activity.
	 */
	public static native void onActivityResume();

	/**
	 * Java native interface function for pause event sent from the owning activity.
	 */
	public static native void onActivityPause();

	/**
	 * Java native interface function for stop event sent from the owning activity.
	 */
	public static native void onActivityStop();

	/**
	 * Java native interface function for destroy event sent from the owning activity.
	 */
	public static native void onActivityDestroy();

	/**
	 * Java native interface function for granted permission events.
	 * @param permission The permission which has been granted
	 */
	public static native void onPermissionGranted(String permission);

	/**
	 * Java native interface function to register the instance function for the corresponding native C++ class.
	 */
	private static native void registerInstance();
}
