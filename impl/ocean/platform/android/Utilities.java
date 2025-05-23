/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.platform.android;

import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PermissionInfo;

import java.util.ArrayList;
import java.util.List;

/**
 * This class implements utility functions for Android.
 * @ingroup platformandroid
 */
public class Utilities
{
	/**
	 * Returns a list of all required permissions (not checking whether the permissions have been granted or not).
	 * @param context The context of the application, must be valid
	 * @param onlyDangerous True, to return only dangerous permissions; False, to return all permissions
	 * @return The list all required permissions
	 */
	public List<String> requiredPermissions(Context context, boolean onlyDangerous)
	{
		List<String> result = new ArrayList<>();

		try
		{
			PackageManager packageManager = context.getPackageManager();
			PackageInfo packageInfo = packageManager.getPackageInfo(context.getPackageName(), PackageManager.GET_PERMISSIONS);

			for (String requestedPermission : packageInfo.requestedPermissions)
			{
				PermissionInfo permissionInfo = packageManager.getPermissionInfo(requestedPermission, 0);

				if (!onlyDangerous || (permissionInfo.protectionLevel & PermissionInfo.PROTECTION_DANGEROUS) != 0)
				{
					result.add(requestedPermission);
				}
			}
		}
		catch (PackageManager.NameNotFoundException exception)
		{
			// nothing to do here
		}

		return result;
	}
}
