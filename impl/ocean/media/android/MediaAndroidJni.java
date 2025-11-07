/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

package com.meta.ocean.media.android;

import android.content.Context;
import android.media.AudioManager;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.media.AudioDeviceInfo;
import android.os.Build;
import android.util.Log;

/**
 * This class implements the Android side of the Media library for Android.
 * @ingroup mediaandroid
 */
public class MediaAndroidJni
{
	/**
	 * Java native interface function to register the Android Media component.
	 * @return True, if succeeded
	 * @see unregisterLibrary().
	 */
	public static native boolean registerLibrary();

	/**
	 * Java native interface function to unregister the Android Media component.
	 * @return True, if succeeded
	 * @see registerLibrary.
	 */
	public static native boolean unregisterLibrary();

	/**
	 * Enumerates the device's microphone capabilities.
	 * Ensure that android.permission.RECORD_AUDIO is granted before calling this function.
	 * @param context The Android application context, must be valid
	 * @return The resulting string containing all microphone capabilities information
	 */
	public static String enumerateMicrophoneCapabilities(Context context)
	{
		if (context == null)
		{
			return "Invalid context";
		}

		StringBuilder stringBuilder = new StringBuilder();

		AudioManager audioManager = (AudioManager)context.getSystemService(Context.AUDIO_SERVICE);
		if (audioManager == null)
		{
			stringBuilder.append("AudioManager is not available\n");
			return stringBuilder.toString();
		}

		stringBuilder.append("Supported Sample Rates:\n");
		int[] testSampleRates = {8000, 11025, 16000, 22050, 44100, 48000};
		int[] testChannelConfigs = {AudioFormat.CHANNEL_IN_MONO, AudioFormat.CHANNEL_IN_STEREO};
		int testEncoding = AudioFormat.ENCODING_PCM_16BIT;

		for (int sampleRate : testSampleRates)
		{
			for (int channelConfig : testChannelConfigs)
			{
				int bufferSize = AudioRecord.getMinBufferSize(sampleRate, channelConfig, testEncoding);

				if (bufferSize > 0 && bufferSize != AudioRecord.ERROR && bufferSize != AudioRecord.ERROR_BAD_VALUE)
				{
					String channelName = (channelConfig == AudioFormat.CHANNEL_IN_MONO) ? "MONO" : "STEREO";
					stringBuilder.append(sampleRate).append(" Hz (").append(channelName)
						.append("), ").append(bufferSize).append(" bytes\n");
				}
			}
		}

		stringBuilder.append("\nAudio Sources:\n");
		testAudioSource(stringBuilder, "MIC", MediaRecorder.AudioSource.MIC);
		testAudioSource(stringBuilder, "CAMCORDER", MediaRecorder.AudioSource.CAMCORDER);
		testAudioSource(stringBuilder, "VOICE_RECOGNITION", MediaRecorder.AudioSource.VOICE_RECOGNITION);
		testAudioSource(stringBuilder, "VOICE_COMMUNICATION", MediaRecorder.AudioSource.VOICE_COMMUNICATION);
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N)
		{
			testAudioSource(stringBuilder, "UNPROCESSED", MediaRecorder.AudioSource.UNPROCESSED);
		}

		// Enumerate input devices with actual capabilities
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
		{
			AudioDeviceInfo[] devices = audioManager.getDevices(AudioManager.GET_DEVICES_INPUTS);
			stringBuilder.append("\nInput Devices Found: ").append(devices.length).append("\n");

			for (int i = 0; i < devices.length; i++)
			{
				AudioDeviceInfo device = devices[i];
				stringBuilder.append("\nDevice ").append(i).append(":\n");
				stringBuilder.append("Product Name: ").append(device.getProductName()).append("\n");
				stringBuilder.append("Type: ").append(getDeviceTypeName(device.getType())).append("\n");
				stringBuilder.append("ID: ").append(device.getId()).append("\n");

				int[] sampleRates = device.getSampleRates();
				if (sampleRates != null && sampleRates.length > 0)
				{
					stringBuilder.append("Declared Sample Rates: ");
					for (int rate : sampleRates)
					{
						stringBuilder.append(rate).append(" ");
					}
					stringBuilder.append("\n");
				}
				else
				{
					stringBuilder.append("  Sample Rates: Using default (test common rates)\n");
				}

				int[] channelMasks = device.getChannelMasks();
				if (channelMasks != null && channelMasks.length > 0)
				{
					stringBuilder.append("Channel Masks: ");
					for (int mask : channelMasks)
					{
						stringBuilder.append(getChannelMaskName(mask)).append(" ");
					}
					stringBuilder.append("\n");
				}

				int[] encodings = device.getEncodings();
				if (encodings != null && encodings.length > 0)
				{
					stringBuilder.append("Encodings: ");
					for (int encoding : encodings)
					{
						stringBuilder.append(getEncodingName(encoding)).append(" ");
					}
					stringBuilder.append("\n");
				}
			}
		}

		return stringBuilder.toString();
	}

	/**
	 * Helper method to test if an audio source is actually supported.
	 * @param stringBuilder The StringBuilder to append to
	 * @param name The name of the audio source
	 * @param source The audio source constant
	 */
	private static void testAudioSource(StringBuilder stringBuilder, String name, int source)
	{
		int sampleRate = 44100;
		int channelConfig = AudioFormat.CHANNEL_IN_MONO;
		int encoding = AudioFormat.ENCODING_PCM_16BIT;

		try
		{
			int bufferSize = AudioRecord.getMinBufferSize(sampleRate, channelConfig, encoding);

			if (bufferSize > 0 && bufferSize != AudioRecord.ERROR && bufferSize != AudioRecord.ERROR_BAD_VALUE)
			{
				AudioRecord testRecord = null;

				try
				{
					testRecord = new AudioRecord(source, sampleRate, channelConfig, encoding, bufferSize);
					int state = testRecord.getState();

					String openSlPreset = getOpenSlPresetName(source);

					if (state == AudioRecord.STATE_INITIALIZED)
					{
						stringBuilder.append("Supported: ").append(name).append(" (value: ").append(source).append("), ").append(openSlPreset).append("\n");
					}
					else
					{
						stringBuilder.append("Not initialized: ").append(name).append(" (value: ").append(source).append("), ").append(openSlPreset).append("\n");
					}
				}
				catch (Exception exception)
				{
					String openSlPreset = getOpenSlPresetName(source);
					stringBuilder.append("Failed: ").append(name).append(" (value: ").append(source).append("), ").append(openSlPreset).append(" - ").append(exception.getMessage()).append("\n");
				}
				finally
				{
					if (testRecord != null)
					{
						testRecord.release();
					}
				}
			}
			else
			{
				String openSlPreset = getOpenSlPresetName(source);
				stringBuilder.append("Invalid buffer size: ").append(name).append(" (value: ").append(source).append("), ").append(openSlPreset).append("\n");
			}
		}
		catch (Exception exception)
		{
			String openSlPreset = getOpenSlPresetName(source);
			stringBuilder.append("Error: ").append(name).append(" (value: ").append(source).append("), ").append(openSlPreset).append(" - ").append(exception.getMessage()).append("\n");
		}
	}

	/**
	 * Helper method to get OpenSL ES preset name for a MediaRecorder AudioSource.
	 * @param audioSource The MediaRecorder.AudioSource constant
	 * @return The OpenSL ES preset name and value
	 */
	private static String getOpenSlPresetName(int audioSource)
	{
		switch (audioSource)
		{
			case MediaRecorder.AudioSource.MIC:
				return "SL_ANDROID_RECORDING_PRESET_GENERIC (0x01)";
			case MediaRecorder.AudioSource.CAMCORDER:
				return "SL_ANDROID_RECORDING_PRESET_CAMCORDER (0x02)";
			case MediaRecorder.AudioSource.VOICE_RECOGNITION:
				return "SL_ANDROID_RECORDING_PRESET_VOICE_RECOGNITION (0x03)";
			case MediaRecorder.AudioSource.VOICE_COMMUNICATION:
				return "SL_ANDROID_RECORDING_PRESET_VOICE_COMMUNICATION (0x04)";
			default:
				// Check for UNPROCESSED (API 24+)
				if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N && audioSource == MediaRecorder.AudioSource.UNPROCESSED)
				{
					return "SL_ANDROID_RECORDING_PRESET_UNPROCESSED (0x05)";
				}
				return "UNKNOWN_PRESET";
		}
	}

	/**
	 * Helper method to get device type name.
	 * @param type The device type constant
	 * @return The device type name
	 */
	private static String getDeviceTypeName(int type)
	{
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M)
		{
			switch (type)
			{
				case AudioDeviceInfo.TYPE_BUILTIN_MIC:
					return "BUILTIN_MIC";
				case AudioDeviceInfo.TYPE_WIRED_HEADSET:
					return "WIRED_HEADSET";
				case AudioDeviceInfo.TYPE_BLUETOOTH_SCO:
					return "BLUETOOTH_SCO";
				case AudioDeviceInfo.TYPE_USB_DEVICE:
					return "USB_DEVICE";
				case AudioDeviceInfo.TYPE_USB_HEADSET:
					return "USB_HEADSET";
				default:
					return "UNKNOWN (" + type + ")";
			}
		}

		return "UNKNOWN";
	}

	/**
	 * Helper method to get channel mask name.
	 * @param mask The channel mask constant
	 * @return The channel mask name
	 */
	private static String getChannelMaskName(int mask)
	{
		switch (mask)
		{
			case AudioFormat.CHANNEL_IN_MONO:
				return "MONO";
			case AudioFormat.CHANNEL_IN_STEREO:
				return "STEREO";
			case AudioFormat.CHANNEL_IN_BACK:
				return "BACK";
			default:
				return "MASK_" + mask;
		}
	}

	/**
	 * Helper method to get encoding name.
	 * @param encoding The encoding constant
	 * @return The encoding name
	 */
	private static String getEncodingName(int encoding)
	{
		switch (encoding)
		{
			case AudioFormat.ENCODING_PCM_16BIT:
				return "PCM_16BIT";
			case AudioFormat.ENCODING_PCM_8BIT:
				return "PCM_8BIT";
			case AudioFormat.ENCODING_PCM_FLOAT:
				return "PCM_FLOAT";
			case AudioFormat.ENCODING_INVALID:
				return "INVALID";
			default:
				if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N)
				{
					if (encoding == AudioFormat.ENCODING_PCM_24BIT_PACKED)
					{
						return "PCM_24BIT_PACKED";
					}
					if (encoding == AudioFormat.ENCODING_PCM_32BIT)
					{
						return "PCM_32BIT";
					}
				}
				return "UNKNOWN (" + encoding + ")";
		}
	}
}
