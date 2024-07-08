/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.platform.android;

import com.meta.ocean.devices.android.DevicesAndroidJni;

import android.Manifest;

import android.app.Activity;

import android.content.Context;
import android.content.pm.PackageManager;

import android.location.Location;
import android.location.LocationManager;

import android.os.Looper;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;

import com.google.android.gms.location.FusedLocationProviderClient;
import com.google.android.gms.location.LocationCallback;
import com.google.android.gms.location.LocationResult;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationServices;

import android.util.Log;

/**
 * This class implements a manager for locations.
 * @ingroup platformandroid
 */
public class OceanLocationManager
{
	public OceanLocationManager(Context context)
	{
		if (android.os.Build.VERSION.SDK_INT < 23)
		{
			Log.e("Ocean", "OceanLocationManager needs SDK 23+");
			return;
		}

		if (context.checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED || context.checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED)
		{
			Log.e("Ocean", "OceanLocationManager needs ACCESS_FINE/CORASE_LOCATION permissions");
			return;
		}

		if (GoogleApiAvailability.getInstance().isGooglePlayServicesAvailable(context) != ConnectionResult.SUCCESS)
		{
			Log.e("Ocean", "OceanLocationManager has no access to GooglePlayServices");
			return;
		}

		fusedLocationProviderClient_ = LocationServices.getFusedLocationProviderClient(context);

		LocationManager locationManager = (LocationManager)(context.getSystemService(Context.LOCATION_SERVICE));

        if (locationManager != null && !locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER))
		{
			Log.e("Ocean", "GPS provider is not enabled");
		}
	}

	public void startLocationUpdates()
	{
		if (fusedLocationProviderClient_ == null)
		{
			return;
		}

		if (locationCallback_ != null)
		{
			return;
		}

		locationCallback_ = new LocationCallback()
		{
			@Override
			public void onLocationResult(LocationResult locationResult)
			{
				super.onLocationResult(locationResult);

				Location location = locationResult.getLastLocation();

				if (location == null)
				{
					return;
				}

				double latitude = location.getLatitude();
				double longitude = location.getLongitude();

				float altitude = -Float.MAX_VALUE;
				if (location.hasAltitude())
				{
					altitude = (float)(location.getAltitude()); // meters above WGS 84 reference ellipsoid
				}

				float direction = -1;
				if (location.hasBearing())
				{
					direction = location.getBearing(); // horizontal direction of travel of this device, and is not related to the device orientation
				}

				float speed = -1;
				if (location.hasSpeed())
				{
					speed = location.getSpeed();
				}

				float accuracy = -1;
				if (location.hasAccuracy())
				{
					accuracy = location.getAccuracy(); // radius in meter
				}

				float altitudeAccuracy = -1;
				if (location.hasVerticalAccuracy())
				{
					altitudeAccuracy = location.getVerticalAccuracyMeters();
				}

				float directionAccuracy = -1;
				if (location.hasBearingAccuracy())
				{
					directionAccuracy = location.getBearingAccuracyDegrees();
				}

				float speedAccuracy = -1;
				if (location.hasSpeedAccuracy())
				{
					speedAccuracy = location.getSpeedAccuracyMetersPerSecond();
				}

				double timestamp = (double)(location.getTime()) * 0.001; // unix timestamp in seconds

				DevicesAndroidJni.newGPSLocation(latitude, longitude, altitude, direction, speed, accuracy, altitudeAccuracy, directionAccuracy, speedAccuracy, timestamp);
			}
		};

		LocationRequest locationRequest = LocationRequest.create();
		locationRequest.setInterval(500); // 0.5 second
		locationRequest.setFastestInterval(100); // 0.1 second
		locationRequest.setPriority(LocationRequest.PRIORITY_HIGH_ACCURACY);

		assert(fusedLocationProviderClient_ != null);
		fusedLocationProviderClient_.requestLocationUpdates(locationRequest, locationCallback_, Looper.getMainLooper());

		Log.i("Ocean", "Location client started");
	}

	public void stopLocationUpdates()
	{
		if (locationCallback_ == null)
		{
			return;
		}

		assert(fusedLocationProviderClient_ != null);
		fusedLocationProviderClient_.removeLocationUpdates(locationCallback_);
	}

	/// The location provider client.
	private FusedLocationProviderClient fusedLocationProviderClient_;

	/// The location callback object.
	private LocationCallback locationCallback_;
}
