/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_DEVICES_ANDROID_JNI_DEVICESANDROIDJNI_H
#define META_OCEAN_DEVICES_ANDROID_JNI_DEVICESANDROIDJNI_H

#include "ocean/devices/android/jni/JNI.h"

/**
 * Java native interface function to register the Devices::Android component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup devicesandroid
 */
extern "C" jboolean Java_com_meta_ocean_devices_android_DevicesAndroidJni_registerLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to unregister the Devices::Android component.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @return True, if succeeded
 * @ingroup devicesandroid
 */
extern "C" jboolean Java_com_meta_ocean_devices_android_DevicesAndroidJni_unregisterLibrary(JNIEnv* env, jobject javaThis);

/**
 * Java native interface function to send a new GPS location to the native side.
 * @param env The JNI environment
 * @param javaThis The JNI object
 * @param latitude The position's latitude, in degree, always valid, with range [-90, 90]
 * @param longitude The position's longitude, in degree, must be valid, with range [-180, 180]
 * @param altitude The position's altitude, in meter, NumericF::minValue() if unknown
 * @param direction The travel direction of the device, relative to north, in degree, north is 0 degree, east is 90 degree, with range [0, 360], -1 if unknown
 * @param speed The device's speed, in meter per second, with range [0, infinity), -1 if unknown
 * @param accuracy The horizontal accuracy as radius, in meter, with range [0, infinity), -1 if unknown
 * @param altitudeAccuracy The vertical accuracy in meter, with range [0, infinity), -1 if unknown
 * @param directionAccuracy The direction accuracy, in degree, with range [0, 180], -1 if unknown
 * @param speedAccuracy The speed accuracy, in meter per second, with range [0, infinity), -1 if unknown
 * @param timestamp The unix timestamp of the GPS location
 * @return True, if succeeded
 * @ingroup devicesandroid
 */
extern "C" jboolean Java_com_meta_ocean_devices_android_DevicesAndroidJni_newGPSLocation(JNIEnv* env, jobject javaThis, jdouble latitude, jdouble longitude, jfloat altitude, jfloat direction, jfloat speed, jfloat accuracy, jfloat altitudeAccuracy, jfloat directionAccuracy, jfloat speedAccuracy, jdouble timestamp);

#endif // META_OCEAN_DEVICES_ANDROID_JNI_DEVICESANDROIDJNI_H
