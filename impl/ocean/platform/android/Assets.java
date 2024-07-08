/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.platform.android;

import android.content.res.AssetManager;

import android.util.Log;

import java.io.*;

/**
 * This class implements functions allowing to access assets of an Android project/application with native code.
 * @ingroup platformandroid
 */
public class Assets
{
	/**
	 * Copies all files located in the assets folder to a specified location.
	 * @param assetManager The asset manager from which the files will be copied
	 * @param targetPath The path where the asset files will be copied
	 * @param recursive True, to copy files from sub-directories also
	 * @return True, if succeeded
	 */
	public static boolean copyFiles(AssetManager assetManager, String targetPath, boolean recusive)
	{
		return copyFiles(assetManager, "", targetPath, recusive);
	}

	/**
	 * Copies all files located in the assets folder to a specified location.
	 * @param assetManager The asset manager from which the files will be copied
	 * @param sourcePath The path in the asset folder from which the files will be copied
	 * @param targetPath The path where the asset files will be copied
	 * @param recursive True, to copy files from sub-directories also
	 * @return True, if succeeded
	 */
	public static boolean copyFiles(AssetManager assetManager, String sourcePath, String targetPath, boolean recusive)
	{
		if (targetPath.length() == 0)
		{
			Log.e("Ocean", "Invalid target path");
			return false;
		}

		String extendedTargetPath = new String(targetPath);

		if (extendedTargetPath.lastIndexOf('/') != extendedTargetPath.length() - 1)
		{
			extendedTargetPath += '/';
		}

		try
		{
			String[] assetList = assetManager.list(sourcePath);

			new File(targetPath).mkdirs();

			for (int n = 0; n < assetList.length; ++n)
			{
				String assetName = assetList[n];

				String assetSubDirectoryName = sourcePath.length() == 0 ? assetName : (sourcePath + "/" + assetName);
				String[] assetSubList = assetManager.list(assetSubDirectoryName);

				if (assetSubList.length == 0)
				{
					// we have a file
					copyFile(assetManager, assetSubDirectoryName, extendedTargetPath + assetName, false);
				}
				else if (recusive)
				{
					// we have a sub-directory
					copyFiles(assetManager, assetSubDirectoryName, extendedTargetPath + assetName, recusive);
				}
			}

			return true;
		}
		catch(Exception exception)
		{
			Log.e("Ocean", "Failed to copy asset files: " + exception.getMessage());
		}

		return false;
	}

	/**
	 * Copies one file from the assets folder of the android application to a specified location.
	 * @param assetManager The asset manager from which the files will be copied
	 * @param source The name of the source file as located in the assets folder
	 * @param target The path and name of the target file
	 * @param createTargetDirectories True, to create the directories of the target path, if not existing
	 * @return True, if succeeded
	 */
	public static boolean copyFile(AssetManager assetManager, String source, String target, boolean createTargetDirectories)
	{
		try
		{
			InputStream inputStream = assetManager.open(source);

			int size = inputStream.available();

			byte[] buffer = new byte[size];
			inputStream.read(buffer);
			inputStream.close();

			if (createTargetDirectories)
			{
				new File(target).getParentFile().mkdirs();
			}

			FileOutputStream outputStream = new FileOutputStream(target);
			outputStream.write(buffer);
			outputStream.close();

			boolean debugOutput = false;

			if (debugOutput)
			{
				Log.d("Ocean", "Copied file: " + source + " to " + target + ".");
			}

			return true;
		}
		catch(FileNotFoundException exception)
		{
			Log.e("Ocean", "Failed to copy file: " + source + " as it could not be found.");
		}
		catch(Exception exception)
		{
			Log.e("Ocean", "Failed to copy file " + source + ", due to: " + exception.getMessage());
		}

		return false;
	}
}
