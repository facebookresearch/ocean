/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.system.usb.android;

import android.app.PendingIntent;

import android.content.Context;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;

import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbManager;

import android.util.Log;

import java.lang.String;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * This class implements a manager for USB devices.
 * The manager has a corresponding native implementation, the Java and the C++ needs to exist at the same time.
 * @ingroup systemusbandroid
 */
public class OceanUSBManager
{
	/**
	 * Creates and initializes the manager.
	 * Only one instance of this class can exist.
	 * @param context The Android application's context, must be valid
	 * @see isvalid()
	 */
	public OceanUSBManager(Context context)
	{
		if (context == null)
		{
			Log.e("Ocean", "OceanUSBManager: Invalid context");
			return;
		}

		if (staticManager_ != null)
		{
			Log.e("Ocean", "OceanUSBManager: Only one instance of OceanUSBManager can be created");
			return;
		}

		PackageManager packageManager = context.getPackageManager();
		if (!packageManager.hasSystemFeature(PackageManager.FEATURE_USB_HOST))
		{
			Log.e("Ocean", "OceanUSBManager: The device does not support USB host mode, need to add it to the manifest?");
			return;
		}

		usbManager_ = (UsbManager)context.getSystemService(Context.USB_SERVICE);

		if (usbManager_ == null)
		{
			Log.e("Ocean", "OceanUSBManager: Failed to access USB manager");
		}

		if (initialize())
		{
			IntentFilter filter = new IntentFilter(intentUSBPermissionAction_);
			context.registerReceiver(usbReciever_, filter);
			
			context_ = context;

			staticManager_ = this;
		}
	}

	/**
	 * Returns whether this manager is valid and initialized.
	 * @return True, if so
	 */
	public boolean isValid()
	{
		return context_ != null;
	}

	/**
	 * Enumerates available USB devices with arbitrary USB classes.
	 * @return The list with the device names of available USB devices
	 */
	static public synchronized List<String> enumerateDevices()
	{
		return enumerateDevices(-1);
	}

	/**
	 * Enumerates available USB devices with a specific USB class.
	 * @param usbClass The USB class to enumerate devices for, -1 for all classes
	 * @return The list with the device names of available USB devices
	 */
	static public synchronized List<String> enumerateDevices(int usbClass)
	{
		List<String> result = new ArrayList<String>();

		if (staticManager_ == null)
		{
			Log.e("Ocean", "OceanUSBManager: No instance of OceanUSBManager available");
			return result;
		}

		HashMap<String, UsbDevice> deviceList = staticManager_.usbManager_.getDeviceList();

		for (UsbDevice usbDevice : deviceList.values())
		{
			boolean accepted = usbClass < 0;

			if (accepted == false)
			{
				if (usbDevice.getDeviceClass() == usbClass)
				{
					// the device's base class is already a match

					accepted = true;
				}
				else
				{
					// a usb base class can be e.g., Miscellaneous (0xEF), but the device may have an interface which matches the execpted class

					for (int i = 0; i < usbDevice.getInterfaceCount(); ++i)
					{
						UsbInterface usbInterface = usbDevice.getInterface(i);

						if (usbInterface.getInterfaceClass() == usbClass)
						{
							accepted = true;
							break;
						}
					}
				}
			}

			if (accepted)
			{
				result.add(usbDevice.getDeviceName());
			}
		}

		return result;
	}

	/**
	 * Returns the product name of a USB device.
	 * @param deviceName The name of the USB device for which the product name is requested, must be valid
	 * @return The product name of the USB device or null, if no such device is available or if the product name is unknown
	 */
	static public synchronized String getProductName(String deviceName)
	{
		if (deviceName == null || deviceName.isEmpty())
		{
			Log.e("Ocean", "OceanUSBManager: Invalid device name");
			return null;
		}

		if (staticManager_ == null)
		{
			Log.e("Ocean", "OceanUSBManager: No instance of OceanUSBManager available");
			return null;
		}

		UsbDevice usbDevice = getDevice(deviceName);

		if (usbDevice == null)
		{
			return null;
		}

		return usbDevice.getProductName();
	}

	/**
	 * Returns the manufacturer name of a USB device.
	 * @param deviceName The name of the USB device for which the manufacturer name is requested, must be valid
	 * @return The manufacturer name of the USB device or null, if no such device is available or if the manufacturer name is unknown
	 */
	static public synchronized String getManufacturerName(String deviceName)
	{
		if (deviceName == null || deviceName.isEmpty())
		{
			Log.e("Ocean", "OceanUSBManager: Invalid device name");
			return null;
		}

		if (staticManager_ == null)
		{
			Log.e("Ocean", "OceanUSBManager: No instance of OceanUSBManager available");
			return null;
		}

		UsbDevice usbDevice = getDevice(deviceName);

		if (usbDevice == null)
		{
			return null;
		}

		return usbDevice.getManufacturerName();
	}

	/**
	 * Retruns addtional details of a USB device.
	 * The resulting list contains (in the specified order) the vendor id, product id, device class, sub-class, protocol, and the number of interfaces.
	 * @param deviceName The name of the USB device for which the product details are requested, must be valid
	 * @return The list with six integers, the first one is vendor id, and so on
	 */
	static public synchronized List<Integer> getProductDetails(String deviceName)
	{
		List<Integer> result = new ArrayList<Integer>();

		if (deviceName == null || deviceName.isEmpty())
		{
			Log.e("Ocean", "OceanUSBManager: Invalid device name");
			return result;
		}		

		if (staticManager_ == null)
		{
			Log.e("Ocean", "OceanUSBManager: No instance of OceanUSBManager available");
			return result;
		}

		UsbDevice usbDevice = getDevice(deviceName);

		if (usbDevice == null)
		{
			return result;
		}

		result.add(usbDevice.getVendorId());
		result.add(usbDevice.getProductId());
		result.add(usbDevice.getDeviceClass());
		result.add(usbDevice.getDeviceSubclass());
		result.add(usbDevice.getDeviceProtocol());
		result.add(usbDevice.getInterfaceCount());

		return result;
	}

	/**
	 * Requests permission for a USB device.
	 * The permission request is sent to the user and can be accepted or denied.
	 * This function returns immediately, and the permission response will be sent asynchronously to the native code side.
	 * @param deviceName The name of the USB device for which permission is requested, must be valid
	 * @return True, if the permission was requested successfully (not whether the permission is granted or not); False, in case of an error
	 */
	static public synchronized boolean requestPermission(String deviceName)
	{
		if (deviceName == null || deviceName.isEmpty())
		{
			Log.e("Ocean", "OceanUSBManager: Invalid device name");
			return false;
		}

		if (staticManager_ == null)
		{
			Log.e("Ocean", "OceanUSBManager: No instance of OceanUSBManager available");
			return false;
		}
		
		UsbDevice usbDevice  = getDevice(deviceName);

		if (usbDevice == null)
		{
			return false;
		}
		
		PendingIntent permissionIntent = PendingIntent.getBroadcast(staticManager_.context_, 0, new Intent(staticManager_.intentUSBPermissionAction_), PendingIntent.FLAG_MUTABLE);

		staticManager_.usbManager_.requestPermission(usbDevice, permissionIntent);

		return true;
	}

	/**
	 * Returns whether the permission for a USB device is granted.
	 * @param deviceName The name of the USB device for which permission is requested, must be valid
	 * @return 1, if the permission is granted; 0, in case the permission is not granted; and -1, in case of an error
	 */
	static public synchronized int hasPermission(String deviceName)
	{
		if (deviceName == null || deviceName.isEmpty())
		{
			Log.e("Ocean", "OceanUSBManager: Invalid device name");
			return -1;
		}

		if (staticManager_ == null)
		{
			Log.e("Ocean", "OceanUSBManager: No instance of OceanUSBManager available");
			return -1;
		}

		UsbDevice usbDevice = getDevice(deviceName);

		if (usbDevice == null)
		{
			return -1;
		}

		if (staticManager_.usbManager_.hasPermission(usbDevice))
		{
			return 1;
		}
		
		return 0;
	}

	/**
	 * Opens a USB device.
	 * @param deviceName The name of the USB device to open, must be valid
	 * @return The file descriptor of the opened device or -1, in case of an error
	 */
	static public synchronized int openDevice(String deviceName)
	{
		if (deviceName == null || deviceName.isEmpty())
		{
			Log.e("Ocean", "OceanUSBManager: Invalid device name");
			return -1;
		}

		if (staticManager_ == null)
		{
			Log.e("Ocean", "OceanUSBManager: No instance of OceanUSBManager available");
			return -1;
		}

		UsbDeviceConnection usbDeviceConnection = staticManager_.openedDeviceList_.get(deviceName);

		if (usbDeviceConnection != null)
		{
			Log.w("Ocean", "OceanUSBManager: Device " + deviceName + " is already opened");
			return usbDeviceConnection.getFileDescriptor();
		}

		UsbDevice usbDevice = staticManager_.grantedDeviceList_.get(deviceName);

		if (usbDevice == null)
		{
			Log.w("Ocean", "OceanUSBManager: Permission for device " + deviceName + " not granted");
			return -1;
		}

		usbDeviceConnection = staticManager_.usbManager_.openDevice(usbDevice);

		if (usbDeviceConnection == null)
		{
			Log.e("Ocean", "OceanUSBManager: Failed to open device");
			return -1;
		}

		final int fileDescriptor = usbDeviceConnection.getFileDescriptor();

		if (fileDescriptor < 0)
		{
			Log.e("Ocean", "OceanUSBManager: Invalid file descriptor for device '"+ deviceName + "'");
			return -1;
		}

		staticManager_.openedDeviceList_.put(deviceName, usbDeviceConnection);

		return fileDescriptor;
	};

	/**
	 * Closes a previously opened USB device.
	 * @param deviceName The name of the USB device to close, must be valid
	 * @return True, if the device was closed successfully; False, in case of an error
	 */
	static public synchronized boolean closeDevice(String deviceName)
	{
		if (deviceName == null || deviceName.isEmpty())
		{
			Log.e("Ocean", "OceanUSBManager: Invalid device name");
			return false;
		}

		if (staticManager_ == null)
		{
			Log.e("Ocean", "OceanUSBManager: No instance of OceanUSBManager available");
			return false;
		}

		UsbDeviceConnection usbDeviceConnection = staticManager_.openedDeviceList_.get(deviceName);

		if (usbDeviceConnection == null)
		{
			Log.w("Ocean", "OceanUSBManager: Device " + deviceName + " is not opened");
			return true;
		}

		usbDeviceConnection.close();

		staticManager_.openedDeviceList_.remove(deviceName);

		return true;
	};

	/**
	 * Java native interface function to initialize the native manager.
	 * @return True, if succeeded
	 */
	private static native boolean initialize();

	/**
	 * Java native interface function for granted device permissions.
	 * @param deviceName The name of the device, must be valid
	 * @param granted True, if the permission is granted; False, if the permission is not granted
	 */
	private static native void onDevicePermission(String deviceName, boolean granted);

	/**
	 * Helper function to get a USB device from Android's USB manager.
	 * @param deviceName The name of the USB device, must be valid
	 * @return The USB device, null if the device does not exist or in case of an error
	 */
	static private UsbDevice getDevice(String deviceName)
	{
		if (staticManager_ == null)
		{
			return null;
		}

		return staticManager_.usbManager_.getDeviceList().get(deviceName);
	}

	/// The context which was used to create this instance.
	private Context context_ = null;

	/// The map with granted USB devices, mapping the device name to the USB device.
	private HashMap<String, UsbDevice> grantedDeviceList_ = new HashMap<>();

	/// The map with opened USB devices, mapping the device name to the connection.
	private HashMap<String, UsbDeviceConnection> openedDeviceList_ = new HashMap<>();

	/// The Android USB manager.
	private UsbManager usbManager_ = null;

	/// The receiver for USB permission request intents sent by Context.sendBroadcast(Intent). 
	private final BroadcastReceiver usbReciever_ = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent)
		{
			String action = intent.getAction();

			Log.d("Ocean", "OceanUSBManager: Received broadcast, action: " + action);

			if (intentUSBPermissionAction_.equals(action))
			{
				synchronized (this)
				{
					UsbDevice usbDevice = intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);

					if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false))
					{
						if (usbDevice != null)
						{
							Log.d("Ocean", "OceanUSBManager: Permission granted for device " + usbDevice.getDeviceName() + " (" + usbDevice.getProductName() + ")");

							UsbDeviceConnection usbDeviceConnection = usbManager_.openDevice(usbDevice);

							grantedDeviceList_.put(usbDevice.getDeviceName(), usbDevice);

							onDevicePermission(usbDevice.getDeviceName(), true);
						}
					}
					else
					{
						if (usbDevice != null)
						{
							onDevicePermission(usbDevice.getDeviceName(), false);

							Log.d("Ocean", "OceanUSBManager: Permission denied for device '" + usbDevice.getDeviceName() + " (" + usbDevice.getProductName() + ")");
						}
						else
						{
							Log.d("Ocean", "OceanUSBManager: Permission denied for device");
						}
					}
				}
			}
		}
	};

	/// The custom intent action for USB permission requests.
	private final String intentUSBPermissionAction_ = "com.meta.ocean.system.usb.android.ACTION_USB_PERMISSION";

	/// The static instance of this object.
	static private OceanUSBManager staticManager_ = null;
}
