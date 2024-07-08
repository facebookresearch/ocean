/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.media.android;

import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.util.Log;

import java.util.List;
import java.util.ArrayList;

/**
 * This class implements the Android side of the Media library for Android.
 * With Android API 24+, this class is not used anymore as the cameras an be accessed in native code directly.
 * @ingroup mediaandroid
 */
public class MediaAndroidJni
{
	/**
	 * Java native interface function to register the OpenIL Media component.
	 * @return True, if succeeded
	 * @see unregisterLibrary().
	 */
	public static native boolean registerLibrary();

	/**
	 * Java native interface function to unregister the OpenIL Media component.
	 * @return True, if succeeded
	 * @see registerLibrary.
	 */
	public static native boolean unregisterLibrary();

	/**
	 * Java native interface function forwarding new new camera framebuffers to the Ocean framework.
	 * @param env JNI environment object
	 * @param javaThis JNI caller object
	 * @param imageBuffer New camera image buffer
	 * @param width The width of the new camera image buffer in pixel
	 * @param height The height of the new camera image buffer in pixel
	 * @param format Format of the camera image buffer
	 * @param fovX Horizontal field of view in radian
	 * @param bufferSize The size of the image buffer in bytes
	 * @param timestamp Unix timestamp of the frame in seconds
	 * @return True, if succeeded
	 */
	private static native boolean onNewCameraFrame(int objectId, byte[] data, int width, int height, int frametype, float fovX, int bufferSize, double timestamp);

	/**
	 * Definition of a frame callback class.
	 */
	private static final class FramePreviewCallback implements Camera.PreviewCallback
	{
		/**
		 * Creates a new frame preview callback object.
		 * @param objectId
		 */
		public FramePreviewCallback(int objectId)
		{
			callbackObjectId = objectId;
			callbackFrameWidth = 0;
			callbackFrameHeight = 0;
			callbackFrameFormat = 0;
			callbackFrameFovX = 0;
		}

		/**
		 * New camera frame callback function.
		 * @param data New frame data
		 * @param camera Camera object
		 */
		public void onPreviewFrame(byte[] data, Camera camera)
		{
			if (camera == null || callbackObjectId == -1)
			{
				return;
			}

			// we do not receive the camera parameters here as this causes problems on some devices, thus we take the frame properties from the member variables
			if (callbackFrameWidth > 0 && callbackFrameHeight > 0)
			{
				onNewCameraFrame(callbackObjectId, data, callbackFrameWidth, callbackFrameHeight, callbackFrameFormat, callbackFrameFovX, data.length, (double)(System.currentTimeMillis()) * 0.001);
			}

			camera.addCallbackBuffer(data);
		}

		/**
		 * Sets the parameters of the camera frames that will be received.
		 * @param width The width of the camera frame in pixel
		 * @param height The height of the camera frame in pixel
		 * @param format Format of the camera frame
		 * @param fovX Horizontal field of view of the camera frame in radian
		 * @return True, if succeeded
		 */
		public boolean setFrameType(int width, int height, int format, float fovX)
		{
			if (width < 0 || height < 0 || format == 0 || fovX < 0 || fovX > (float)(Math.PI / 2.0))
			{
				return false;
			}

			callbackFrameWidth = width;
			callbackFrameHeight = height;
			callbackFrameFormat = format;
			callbackFrameFovX = fovX;

			return true;
		}

		/// Id of the callback object.
		private int callbackObjectId;

		/// Width of the camera frame in pixel.
		private int callbackFrameWidth;

		/// Height of the camera frame in pixel.
		private int callbackFrameHeight;

		/// Pixel format of the camera frame.
		private int callbackFrameFormat;

		/// Horizontal field of view of the camera frame in radian.
		private float callbackFrameFovX;
	}

	/**
	 * This class covers an Android camera object.
	 * Each object holds an own camera id to allow different camera objects concurrently.<br>
	 * @ingroup comocean
	 */
	public static class CameraObject
	{
		CameraObject(String url, int preferredWidth, int preferredHeight, float preferredFrequency)
		{
			cameraInitialized = false;
			cameraIsStarted = false;

			cameraURL = url;
			cameraPreferredWidth = preferredWidth;
			cameraPreferredHeight = preferredHeight;
			cameraPreferredFrequency = preferredFrequency;

			cameraId = -1;
			objectId = -1;

			if (url.startsWith("LiveVideoId:"))
			{
				try
				{
					String urlIdString = url.substring(12);

					int urlId = Integer.parseInt(urlIdString);
					if (urlId >= 0 && urlId < Camera.getNumberOfCameras())
					{
						cameraId = urlId;
					}

					objectId = createUniqueObjectId();
				}
				catch(Exception e)
				{
					Log.e("Ocean Media Android (JAVA)", "Failed to create a Camera object.");
				}
			}
		}

		/**
		 * Initializes the camera and starts the preview request.
		 * @return True, if succeeded
		 */
		public boolean initialize()
		{
			if (camera != null)
			{
				return isValid();
			}

			if (!isValid())
			{
				return false;
			}

			cameraInitialized = true;

			try
			{
				assert(camera == null);

				reopenCamera();
			}
			catch(Exception exception)
			{
				Log.e("Ocean Media Android (JAVA)", exception.getMessage());
				return false;
			}

			return true;
		}

		/**
		 * Resets the initialization state.
		 */
		public void uninitialize()
		{
			cameraInitialized = false;
		}

		private void reopenCamera()
		{
			if (camera != null)
				return;

			try
			{
				camera = Camera.open(cameraId);

				if (cameraPreferredWidth > 0 && cameraPreferredHeight > 0)
				{
					try
					{
						Camera.Parameters cameraParameters = camera.getParameters();

						cameraParameters.setPreviewSize(cameraPreferredWidth, cameraPreferredHeight);

						camera.setParameters(cameraParameters);
					}
					catch (Exception exception)
					{
						Log.e("Ocean Media Android (JAVA)", exception.getMessage());
					}
				}

				if (cameraPreferredFrequency > 0)
				{
					try
					{
						Camera.Parameters cameraParameters = camera.getParameters();

						cameraParameters.setPreviewFpsRange((int)(cameraPreferredFrequency * 1000), (int)(cameraPreferredFrequency * 1000));

						camera.setParameters(cameraParameters);
					}
					catch (Exception exception)
					{
						Log.e("Ocean Media Android (JAVA)", exception.getMessage());
					}
				}

				try
				{
					Camera.Parameters cameraParameters = camera.getParameters();

					List<String> focusModes = cameraParameters.getSupportedFocusModes();

					if (focusModes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO))
					{
						cameraParameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
					}
					else if (focusModes.contains(Camera.Parameters.FOCUS_MODE_AUTO))
					{
						cameraParameters.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
					}

					camera.setParameters(cameraParameters);
				}
				catch (Exception exception)
				{
					Log.e("Ocean Media Android (JAVA)", exception.getMessage());
				}

				Camera.Parameters cameraParameters = camera.getParameters();

				int frameWidth = cameraParameters.getPreviewSize().width;
				int frameHeight = cameraParameters.getPreviewSize().height;
				int frameFormat = cameraParameters.getPreviewFormat();
				float frameFovX = cameraParameters.getHorizontalViewAngle() * 0.01745329252f;

				int frameBitPerPixel = ImageFormat.getBitsPerPixel(frameFormat);
				int bufferSize = frameWidth * frameHeight * frameBitPerPixel / 8;

				cameraFrameCallback = new FramePreviewCallback(objectId);
				cameraFrameCallback.setFrameType(frameWidth, frameHeight, frameFormat, frameFovX);

				camera.addCallbackBuffer(new byte[bufferSize]);
				camera.setPreviewCallbackWithBuffer(cameraFrameCallback);

				cameraSurfaceTexture = new SurfaceTexture(1000 + cameraId);
				camera.setPreviewTexture(cameraSurfaceTexture);

				Log.i("Ocean", "Starting video preview for camera: " + String.valueOf(frameWidth) + "x" + String.valueOf(frameHeight));
				Log.i("Ocean", "With horizontal field of view: " + String.valueOf(frameFovX * 57.2957795131) + "deg");

				camera.startPreview();
			}
			catch (Exception exception)
			{
				Log.e("Ocean Media Android (JAVA)", exception.getMessage());

				if (camera != null)
				{
					camera.release();
					camera = null;
				}
			}
		}

		private void releaseCamera()
		{
			if (camera != null)
			{
				try
				{
					camera.stopPreview();

					camera.setPreviewTexture(null);
					camera.setPreviewCallback(null);

					camera.release();
					camera = null;
					cameraSurfaceTexture = null;
				}
				catch(Exception exception)
				{
					Log.e("Ocean Media Android (JAVA)", exception.getMessage());
				}
			}
		}

		/**
		 * Releases the internal objects.
		 */
		protected void finalize() throws Throwable
		{
			release();
		}

		/**
		 * Releases the camera object and disposes all resources.
		 */
		public void release()
		{
			releaseCamera();
		}

		/**
		 * Starts the camera.
		 * @return True, if the camera could be started
		 */
		public boolean start()
		{
			if (cameraIsStarted)
			{
				return true;
			}

			if (camera != null)
			{
				camera.startPreview();
			}

			cameraIsStarted = true;

			Log.i("Ocean Media Android (JAVA)", "Started camera with id: " + String.valueOf(cameraId));

			return cameraIsStarted;
		}

		/**
		 * Pauses the camera.
		 * @return True, if succeeded
		 */
		public boolean pause()
		{
			if (!cameraIsStarted)
			{
				return true;
			}

			cameraIsStarted = false;

			if (camera != null)
			{
				camera.stopPreview();
			}

			Log.i("Ocean Media Android (JAVA)", "Paused camera with id: " + String.valueOf(cameraId));

			return true;
		}

		/**
		 * Stops the camera.
		 * @return True, if succeeded
		 */
		public boolean stop()
		{
			if (!cameraIsStarted)
			{
				return true;
			}

			cameraIsStarted = false;

			if (camera != null)
			{
				camera.stopPreview();
			}

			Log.i("Ocean Media Android (JAVA)", "Stopped camera with id: " + String.valueOf(cameraId));

			return true;
		}

		/**
		 * Returns the unique id of this camera.
		 * @return Unique camera id
		 */
		public int objectId()
		{
			return objectId;
		}

		/**
		 * Returns the unique URL of this camera object.
		 * @return Camera URL
		 */
		public String url()
		{
			return cameraURL;
		}

		/**
		 * Returns whether this camera object is valid.
		 * @return True, if so
		 */
		public boolean isValid()
		{
			return cameraId != -1;
		}

		/**
		 * Returns whether this camera object has been initialized already.
		 * @return True, if so
		 */
		public boolean isInitialized()
		{
			return cameraInitialized;
		}

		/// Internal camera object wrapped by this object.
		private Camera camera;

		/// Android sided camera id of the camera.
		private int cameraId;

		/// Unique object id of this camera object allowing to distinguish between individual objects.
		private int objectId;

		/// Unique url of this camera object.
		private String cameraURL;

		/// Preferred frame width in pixel, if defined.
		private int cameraPreferredWidth;

		/// Preferred frame height in pixel, if defined.
		private int cameraPreferredHeight;

		/// Preferred frame frequency in Hz, if defined.
		private float cameraPreferredFrequency;

		/// State determining whether the camera object has been initialized successfully.
		private boolean cameraInitialized;

		/// State determining whether the camera object has been started succeessfully.
		private boolean cameraIsStarted;

		/// Frame callback object forwarding the framebuffers to the Ocean framework.
		private FramePreviewCallback cameraFrameCallback;

		/// The surface texture object.
		private SurfaceTexture cameraSurfaceTexture;
	};

	/**
	 * Updates all camera objects needing an update.
	 * This function should be called before every new update or rendering call by the Java / Android implementation.
	 */
	public static synchronized void update()
	{
		if (!needInitialization)
		{
			return;
		}

		for (int n = 0; n < cameraObjects.size(); ++n)
		{
			if (!cameraObjects.get(n).isInitialized())
			{
				cameraObjects.get(n).initialize();
			}
		}

		needInitialization = false;
	}

	/**
	 * Creates a new camera object and returns the unique camera id if succeeded.
	 * Beware: This function should be called by the Ocean framework only using a native interface call.
	 * @param url URL of the camera to be created
	 * @param preferredWidth Preferred frame width in pixel, if defined
	 * @param preferredHeight Preferred frame height in pixel, if defined
	 * @param preferredFrequency Preferred frame frequency in Hz, if defined
	 * @return Unique object id allowing to distinguish between objects
	 * @see releaseCamera().
	 */
	public static synchronized int createCamera(String url, int preferredWidth, int preferredHeight, float preferredFrequency)
	{
		for (int n = 0; n < cameraObjects.size(); ++n)
		{
			if (cameraObjects.get(n).url() == url)
			{
				return -1;
			}
		}

		CameraObject newObject = new CameraObject(url, preferredWidth, preferredHeight, preferredFrequency);

		if (newObject.isValid())
		{
			cameraObjects.add(newObject);

			needInitialization = true;
			return newObject.objectId();
		}

		return -1;
	}

	/**
	 * Releases a camera object defined by it's unique object id.
	 * Beware: This function should be called by the Ocean framework only using a native interface call.
	 * @param objectId Object id of the camera to be released
	 * @return True, if suceeded
	 * @see createCamera().
	 */
	public static synchronized boolean releaseCamera(int objectId)
	{
		for (int n = 0; n < cameraObjects.size(); ++n)
		{
			if (cameraObjects.get(n).objectId() == objectId)
			{
				cameraObjects.get(n).release();
				cameraObjects.remove(n);

				return true;
			}
		}

		return false;
	}

	/**
	 * Starts a camera defined by it's unique object id.
	 * Beware: This function should be called by the Ocean framework only using a native interface call.
	 * @param objectId Unique id of the camera to be started
	 * @return True, if the camera could be started
	 * @see pauseCamera(), stopCamera(), createCamera().
	 */
	public static synchronized boolean startCamera(int objectId)
	{
		for (int n = 0; n < cameraObjects.size(); ++n)
		{
			if (cameraObjects.get(n).objectId() == objectId)
			{
				return cameraObjects.get(n).start();
			}
		}

		return false;
	}

	/**
	 * Pauses a camera defined by it's unique object id.
	 * Beware: This function should be called by the Ocean framework only using a native interface call.
	 * @param objectId Unique id of the camera to be paused
	 * @return True, if succeeded
	 * @see startCamera(), stopCamera().
	 */
	public static synchronized boolean pauseCamera(int objectId)
	{
		for (int n = 0; n < cameraObjects.size(); ++n)
		{
			if (cameraObjects.get(n).objectId() == objectId)
			{
				return cameraObjects.get(n).pause();
			}
		}

		return false;
	}

	/**
	 * Stops a camera defined by it's unique object id.
	 * Beware: This function should be called by the Ocean framework only using a native interface call.
	 * @param objectId Unique id of the camera to be stopped
	 * @return True, if succeeded
	 * @see startCamera(), pauseCamera().
	 */
	public static synchronized boolean stopCamera(int objectId)
	{
		for (int n = 0; n < cameraObjects.size(); ++n)
		{
			if (cameraObjects.get(n).objectId() == objectId)
			{
				return cameraObjects.get(n).stop();
			}
		}

		return false;
	}

	/**
	 * Creates a unique camera object id.
	 * The uniqueness is guaranteed by a synchronized id counter.
	 * @return New unique camera object id
	 */
	private static synchronized int createUniqueObjectId()
	{
		++cameraObjectIdCounter;
		return cameraObjectIdCounter;
	}

	/**
	 * Releases the camera resources e.g., because the application is paused.
	 * @see recoverResources().
	 */
	public static synchronized void releaseResources()
	{
		for (int n = 0; n < cameraObjects.size(); ++n)
		{
			cameraObjects.get(n).release();
		}
	}

	/**
	 * Recovers the camera resources e.g., because the application is resumed.
	 * @see releaseResources().
	 */
	public static synchronized void recoverResources()
	{
		if (!cameraObjects.isEmpty())
		{
			for (int n = 0; n < cameraObjects.size(); ++n)
			{
				cameraObjects.get(n).uninitialize();
			}

			needInitialization = true;
		}
	}

	/// List holding all currently created camera objects.
	private static List<CameraObject> cameraObjects = new ArrayList<CameraObject>();

	/// Id counter allowing to create unique camera object ids.
	private static int cameraObjectIdCounter = 0;

	/// State determining whether at least one camera object needs an update.
	private static boolean needInitialization = false;
}
