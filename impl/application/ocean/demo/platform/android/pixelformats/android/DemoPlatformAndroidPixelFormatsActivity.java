/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.app.demo.platform.android.pixelformats.android;

import com.meta.ocean.base.BaseJni;
import com.meta.ocean.platform.android.*;
import com.meta.ocean.platform.android.application.*;

import android.graphics.*;
import android.util.Log;
import android.view.WindowManager;

import java.io.*;

/**
 * This class implements the main Activity of the demo application.
 *
 * This application is mainly intended to proof the matchings of pixel formats:
 *
 * Android Java pixel format,    Android Native pixel format,         Ocean pixel format
 * Bitmap.Config.ARGB_8888       ANDROID_BITMAP_FORMAT_RGBA_8888      FORMAT_RGBA32
 * Bitmap.Config.RGB_565         ANDROID_BITMAP_FORMAT_RGB_565        FORMAT_BGR565
 *
 * @ingroup applicationdemoplatformandroidpixelforamts
 */
public class DemoPlatformAndroidPixelFormatsActivity extends TextActivity
{
	// load the library
	static
	{
		System.loadLibrary("OceanDemoPlatformAndroidPixelFormats");
	}

	/**
	 * Native interface function to set a channel of the bitmap to a specified value.
	 * @param bitmap The bitmap for which the channel will be set, must be valid
	 * @param channel The channel to be modified, with range [0, 4], depending on the actual pixel format
	 * @param value The value to be set, with range [0, 255]
	 */
	private native boolean setChannelToValue(Bitmap bitmap, int channel, int value);

	/**
	 * Called when the activity is becoming visible to the user.
	 */
	public void onStart()
	{
		super.onStart();
		android.os.Process.setThreadPriority(-20);

		// ensuring that the screenn does not turn off during execution
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		// The output to which the application's log will be written
		// use "STANDARD" to show the log output in 'adb logcat -s "Ocean"'
		// use "QUEUED" to queue the logs and to allow popping them later
		// use "filename" to write the log output into a file (e.g., with getExternalFilesDir(null) + "/filename.txt")
		final String applicationOutput = "STANDARD, QUEUED";

		BaseJni.initialize(applicationOutput);

		modifyImageRGBA888();

		modifyImageRGB565();
	}

	private void modifyImageRGBA888()
	{
		final String resourcePath = getExternalFilesDir(null) + "/";
		BaseJni.information("resourcePath: " + resourcePath);
		final String imageFilename = "tropical-island-with-toucans_640x640.jpg";

		Assets.copyFiles(getAssets(), resourcePath, true);

		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inPreferredConfig = Bitmap.Config.ARGB_8888;
		Bitmap bitmap = BitmapFactory.decodeFile(resourcePath + "/" + imageFilename, options);

		if (bitmap != null && bitmap.getConfig() == Bitmap.Config.ARGB_8888)
		{
			BaseJni.information("Loaded bitmap with resolution " + Integer.toString(bitmap.getWidth()) + "x" + Integer.toString(bitmap.getHeight()));
			BaseJni.information("The image has the following pixel format \"" + bitmap.getConfig().toString() + "\"");

			{
				// let's modify the red channel

				Bitmap bitmapCopy = bitmap.copy(bitmap.getConfig(), true);

				if (setChannelToValue(bitmapCopy, 0, 255))
				{
					try (FileOutputStream outputStream = new FileOutputStream(resourcePath + "/" + "rgba32_channel0_255.png"))
					{
						bitmapCopy.compress(Bitmap.CompressFormat.PNG, 100, outputStream);
						BaseJni.information("Writing image to file");
					}
					catch (IOException exception)
					{
						BaseJni.error(exception.getMessage());
					}
				}
			}

			{
				// let's modify the green channel

				Bitmap bitmapCopy = bitmap.copy(bitmap.getConfig(), true);

				if (setChannelToValue(bitmapCopy, 1, 255))
				{
					try (FileOutputStream outputStream = new FileOutputStream(resourcePath + "/" + "rgba32_channel1_255.png"))
					{
						bitmapCopy.compress(Bitmap.CompressFormat.PNG, 100, outputStream);
						BaseJni.information("Writing image to file");
					}
					catch (IOException exception)
					{
						BaseJni.error(exception.getMessage());
					}
				}
			}

			{
				// let's modify the blue channel

				Bitmap bitmapCopy = bitmap.copy(bitmap.getConfig(), true);

				if (setChannelToValue(bitmapCopy, 2, 255))
				{
					try (FileOutputStream outputStream = new FileOutputStream(resourcePath + "/" + "rgba32_channel2_255.png"))
					{
						bitmapCopy.compress(Bitmap.CompressFormat.PNG, 100, outputStream);
						BaseJni.information("Writing image to file");
					}
					catch (IOException exception)
					{
						BaseJni.error(exception.getMessage());
					}
				}
			}

			{
				// let's modify the alpha channel

				Bitmap bitmapCopy = bitmap.copy(bitmap.getConfig(), true);

				if (setChannelToValue(bitmapCopy, 3, 128))
				{
					try (FileOutputStream outputStream = new FileOutputStream(resourcePath + "/" + "rgba32_channel3_128.png"))
					{
						bitmapCopy.compress(Bitmap.CompressFormat.PNG, 100, outputStream);
						BaseJni.information("Writing image to file");
					}
					catch (IOException exception)
					{
						BaseJni.error(exception.getMessage());
					}
				}
			}
		}
		else
		{
			BaseJni.error("Failed to load image \"" + imageFilename + "\"");
		}
	}

	private void modifyImageRGB565()
	{
		final String resourcePath = getExternalFilesDir(null).getAbsolutePath();
		BaseJni.information("resourcePath: " + resourcePath);
		final String imageFilename = "tropical-island-with-toucans_640x640.jpg";

		Assets.copyFiles(getAssets(), resourcePath, true);

		BitmapFactory.Options options = new BitmapFactory.Options();
		options.inPreferredConfig = Bitmap.Config.RGB_565;
		Bitmap bitmap = BitmapFactory.decodeFile(resourcePath + "/" + imageFilename, options);

		if (bitmap != null && bitmap.getConfig() == Bitmap.Config.RGB_565)
		{
			BaseJni.information("Loaded bitmap with resolution " + Integer.toString(bitmap.getWidth()) + "x" + Integer.toString(bitmap.getHeight()));
			BaseJni.information("The image has the following pixel format \"" + bitmap.getConfig().toString() + "\"");

			{
				// let's modify the red channel

				Bitmap bitmapCopy = bitmap.copy(bitmap.getConfig(), true);

				if (setChannelToValue(bitmapCopy, 0, 255))
				{
					try (FileOutputStream outputStream = new FileOutputStream(resourcePath + "/" + "rgb565_channel0_255.png"))
					{
						bitmapCopy.compress(Bitmap.CompressFormat.PNG, 100, outputStream);
						BaseJni.information("Writing image to file");
					}
					catch (IOException exception)
					{
						BaseJni.error(exception.getMessage());
					}
				}
			}

			{
				// let's modify the green channel

				Bitmap bitmapCopy = bitmap.copy(bitmap.getConfig(), true);

				if (setChannelToValue(bitmapCopy, 1, 255))
				{
					try (FileOutputStream outputStream = new FileOutputStream(resourcePath + "/" + "rgb565_channel1_255.png"))
					{
						bitmapCopy.compress(Bitmap.CompressFormat.PNG, 100, outputStream);
						BaseJni.information("Writing image to file");
					}
					catch (IOException exception)
					{
						BaseJni.error(exception.getMessage());
					}
				}
			}

			{
				// let's modify the blue channel

				Bitmap bitmapCopy = bitmap.copy(bitmap.getConfig(), true);

				if (setChannelToValue(bitmapCopy, 2, 255))
				{
					try (FileOutputStream outputStream = new FileOutputStream(resourcePath + "/" + "rgb565_channel2_255.png"))
					{
						bitmapCopy.compress(Bitmap.CompressFormat.PNG, 100, outputStream);
						BaseJni.information("Writing image to file");
					}
					catch (IOException exception)
					{
						BaseJni.error(exception.getMessage());
					}
				}
			}
		}
		else
		{
			BaseJni.error("Failed to load image \"" + imageFilename + "\"");
		}
	}

	/**
	 * Called when the activity is no longer visible to the user, because another activity has been resumed and is covering this one.
	 */
	public void onStop()
	{
		super.onStop();

		BaseJni.exit(0);
	}
}
