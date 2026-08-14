/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef META_OCEAN_PLATFORM_ANDROID_UTILITIES_H
#define META_OCEAN_PLATFORM_ANDROID_UTILITIES_H

#include "ocean/platform/android/Android.h"
#include "ocean/platform/android/ScopedJNIObject.h"

namespace Ocean
{

namespace Platform
{

namespace Android
{

/**
 * This class implements helper functions for android platforms.
 * @ingroup platformandroid
 */
class OCEAN_PLATFORM_ANDROID_EXPORT Utilities
{
	public:

		/**
		 * Checks whether a Java exception is pending on the calling thread, and clears it.
		 * A pending exception makes almost every following JNI call undefined, and it terminates the process in case the thread detaches while the exception is still pending.
		 * An exception must therefore be cleared before the calling thread makes any further JNI call, returns to Java, or ends.
		 * The description of the exception is written to the error log, as Ocean cannot handle a Java exception in any other way.
		 * This function is safe to call at any time, also when no exception is pending.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @return True, if an exception was pending and has been cleared; False, if no exception was pending
		 */
		static bool clearPotentialException(JNIEnv& jniEnvironment);

		/**
		 * Returns the class with a given name, and handles a potential exception.
		 * Beware: A thread which has been attached by Ocean has no Java frames on the stack, so that the class is resolved with the bootstrap class loader and an application class cannot be found.
		 * Resolve application classes on a thread which has been called from Java, and keep them as a global reference.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param className The name of the class to be returned, e.g., "android/content/Intent", must be valid
		 * @return The requested class, invalid if the class could not be found
		 */
		static ScopedJClass findClass(JNIEnv& jniEnvironment, const std::string& className);

		/**
		 * Returns the id of a non-static method of a class, and handles a potential exception.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param javaClass The class providing the method, must be valid
		 * @param name The name of the method, must be valid
		 * @param signature The signature of the method, e.g., "(Ljava/lang/String;)I", must be valid
		 * @return The id of the method, nullptr if the method does not exist
		 */
		static jmethodID getMethodId(JNIEnv& jniEnvironment, jclass javaClass, const std::string& name, const std::string& signature);

		/**
		 * Returns the id of a static method of a class, and handles a potential exception.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param javaClass The class providing the method, must be valid
		 * @param name The name of the method, must be valid
		 * @param signature The signature of the method, e.g., "(Ljava/lang/String;)I", must be valid
		 * @return The id of the method, nullptr if the method does not exist
		 */
		static jmethodID getStaticMethodId(JNIEnv& jniEnvironment, jclass javaClass, const std::string& name, const std::string& signature);

		/**
		 * Returns the id of a non-static field of a class, and handles a potential exception.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param javaClass The class providing the field, must be valid
		 * @param name The name of the field, must be valid
		 * @param signature The signature of the field, e.g., "Ljava/lang/String;", must be valid
		 * @return The id of the field, nullptr if the field does not exist
		 */
		static jfieldID getFieldId(JNIEnv& jniEnvironment, jclass javaClass, const std::string& name, const std::string& signature);

		/**
		 * Returns the id of a static field of a class, and handles a potential exception.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param javaClass The class providing the field, must be valid
		 * @param name The name of the field, must be valid
		 * @param signature The signature of the field, e.g., "Ljava/lang/String;", must be valid
		 * @return The id of the field, nullptr if the field does not exist
		 */
		static jfieldID getStaticFieldId(JNIEnv& jniEnvironment, jclass javaClass, const std::string& name, const std::string& signature);

		/**
		 * Calls a non-static Java method returning an object, and handles a potential exception.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param object The object on which the method will be called, must be valid
		 * @param methodId The id of the method to be called, must be valid
		 * @param args The arguments of the method, must match the signature of the method
		 * @return The resulting object, invalid if the method has thrown an exception or has returned null
		 * @tparam T The data type of the returned reference, e.g., jobject or jstring
		 * @tparam TArgs The data types of the arguments of the method
		 */
		template <typename T = jobject, typename... TArgs>
		static ScopedJNILocalObject<T> callObjectMethod(JNIEnv& jniEnvironment, jobject object, jmethodID methodId, TArgs&&... args);

		/**
		 * Calls a static Java method returning an object, and handles a potential exception.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param javaClass The class providing the method, must be valid
		 * @param methodId The id of the method to be called, must be valid
		 * @param args The arguments of the method, must match the signature of the method
		 * @return The resulting object, invalid if the method has thrown an exception or has returned null
		 * @tparam T The data type of the returned reference, e.g., jobject or jstring
		 * @tparam TArgs The data types of the arguments of the method
		 */
		template <typename T = jobject, typename... TArgs>
		static ScopedJNILocalObject<T> callStaticObjectMethod(JNIEnv& jniEnvironment, jclass javaClass, jmethodID methodId, TArgs&&... args);

		/**
		 * Creates a new instance of a class, and handles a potential exception.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param javaClass The class to be instantiated, must be valid
		 * @param methodId The id of the constructor to be called, must be valid
		 * @param args The arguments of the constructor, must match the signature of the constructor
		 * @return The resulting object, invalid if the constructor has thrown an exception
		 * @tparam T The data type of the returned reference, e.g., jobject or jstring
		 * @tparam TArgs The data types of the arguments of the constructor
		 */
		template <typename T = jobject, typename... TArgs>
		static ScopedJNILocalObject<T> newObject(JNIEnv& jniEnvironment, jclass javaClass, jmethodID methodId, TArgs&&... args);

		/**
		 * Calls a non-static Java method without return value, and handles a potential exception.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param object The object on which the method will be called, must be valid
		 * @param methodId The id of the method to be called, must be valid
		 * @param args The arguments of the method, must match the signature of the method
		 * @return True, if the method did not throw an exception
		 * @tparam TArgs The data types of the arguments of the method
		 */
		template <typename... TArgs>
		static bool callVoidMethod(JNIEnv& jniEnvironment, jobject object, jmethodID methodId, TArgs&&... args);

		/**
		 * Calls a non-static Java method returning a boolean, and handles a potential exception.
		 * The return value is provided as a parameter, as a thrown exception and a returned 'false' cannot be distinguished otherwise.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param object The object on which the method will be called, must be valid
		 * @param methodId The id of the method to be called, must be valid
		 * @param returnValue The resulting return value of the method, only valid if this function succeeds
		 * @param args The arguments of the method, must match the signature of the method
		 * @return True, if the method did not throw an exception
		 * @tparam TArgs The data types of the arguments of the method
		 */
		template <typename... TArgs>
		static bool callBooleanMethod(JNIEnv& jniEnvironment, jobject object, jmethodID methodId, bool& returnValue, TArgs&&... args);

		/**
		 * Calls a static Java method returning a boolean, and handles a potential exception.
		 * The return value is provided as a parameter, as a thrown exception and a returned 'false' cannot be distinguished otherwise.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param javaClass The class providing the method, must be valid
		 * @param methodId The id of the method to be called, must be valid
		 * @param returnValue The resulting return value of the method, only valid if this function succeeds
		 * @param args The arguments of the method, must match the signature of the method
		 * @return True, if the method did not throw an exception
		 * @tparam TArgs The data types of the arguments of the method
		 */
		template <typename... TArgs>
		static bool callStaticBooleanMethod(JNIEnv& jniEnvironment, jclass javaClass, jmethodID methodId, bool& returnValue, TArgs&&... args);

		/**
		 * Calls a non-static Java method returning an integer, and handles a potential exception.
		 * The return value is provided as a parameter, as a thrown exception and a returned '0' cannot be distinguished otherwise.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param object The object on which the method will be called, must be valid
		 * @param methodId The id of the method to be called, must be valid
		 * @param returnValue The resulting return value of the method, only valid if this function succeeds
		 * @param args The arguments of the method, must match the signature of the method
		 * @return True, if the method did not throw an exception
		 * @tparam TArgs The data types of the arguments of the method
		 */
		template <typename... TArgs>
		static bool callIntMethod(JNIEnv& jniEnvironment, jobject object, jmethodID methodId, int32_t& returnValue, TArgs&&... args);

		/**
		 * Calls a static Java method returning an integer, and handles a potential exception.
		 * The return value is provided as a parameter, as a thrown exception and a returned '0' cannot be distinguished otherwise.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param javaClass The class providing the method, must be valid
		 * @param methodId The id of the method to be called, must be valid
		 * @param returnValue The resulting return value of the method, only valid if this function succeeds
		 * @param args The arguments of the method, must match the signature of the method
		 * @return True, if the method did not throw an exception
		 * @tparam TArgs The data types of the arguments of the method
		 */
		template <typename... TArgs>
		static bool callStaticIntMethod(JNIEnv& jniEnvironment, jclass javaClass, jmethodID methodId, int32_t& returnValue, TArgs&&... args);

		/**
		 * Calls a non-static Java method returning a float, and handles a potential exception.
		 * The return value is provided as a parameter, as a thrown exception and a returned '0' cannot be distinguished otherwise.
		 * @param jniEnvironment The JNI environment of the calling thread
		 * @param object The object on which the method will be called, must be valid
		 * @param methodId The id of the method to be called, must be valid
		 * @param returnValue The resulting return value of the method, only valid if this function succeeds
		 * @param args The arguments of the method, must match the signature of the method
		 * @return True, if the method did not throw an exception
		 * @tparam TArgs The data types of the arguments of the method
		 */
		template <typename... TArgs>
		static bool callFloatMethod(JNIEnv& jniEnvironment, jobject object, jmethodID methodId, float& returnValue, TArgs&&... args);

		/**
		 * Converts a Java native string to a std string.
		 * @param env The Java environment, must be valid
		 * @param javaString Java string to be converted, may be nullptr
		 * @return Resulting std string
		 */
		static std::string toAString(JNIEnv* env, jstring javaString);

		/**
		 * Converts a std string to a Java native string.
		 * @param env The Java environment, must be valid
		 * @param stdString Std string to be converted
		 * @return Resulting Java native string
		 */
		static jstring toJavaString(JNIEnv* env, const std::string& stdString);

		/**
		 * Converts a vector of std strings to a Java array with native strings.
		 * @param env The Java environment, must be valid
		 * @param strings Std strings to be converted, can be empty
		 * @return Resulting Java native string
		 */
		static jobjectArray toJavaStringArray(JNIEnv* env, const Strings& strings);

		/**
		 * Converts a Java native list with string to a vector of strings.
		 * @param env The Java environment, must be valid
		 * @param javaStringList Java list with strings to be converted, must be valid
		 * @param strings The resulting vector of strings
		 * @return True, if succeeded
		 */
		static bool toVector(JNIEnv* env, jobject javaStringList, Strings& strings);

		/**
		 * Converts a Java native list with integer to a vector of integers.
		 * @param env The Java environment, must be valid
		 * @param javaIntegerList Java list with integers to be converted, must be valid
		 * @param values The resulting vector of integers
		 * @return True, if succeeded
		 */
		static bool toVector(JNIEnv* env, jobject javaIntegerList, std::vector<int>& values);

		/**
		 * Returns the class name of an object.
		 * @param env The Java environment, must be valid
		 * @param object The object for which the class name will be returned, must be valid
		 * @param name The resulting class name
		 * @return True, if succeeded
		 */
		static bool className(JNIEnv* env, jobject object, std::string& name);

		/**
		 * Returns the version code stored in the application manifest.
		 * @param javaVM The Java virtual machine, must be valid
		 * @param activity The Android main activity, must be valid
		 * @param versionCode The resulting version code of the application
		 * @param versionName The resulting version name of the application
		 * @return True, if succeeded
		 */
		static bool manifestVersion(JavaVM* javaVM, jobject activity, int& versionCode, std::string& versionName);

		/**
		 * Deprecated.
		 *
		 * Returns the version code stored in the application manifest.
		 * @param javaVM The Java virtual machine, must be valid
		 * @param activity The Android main activity, must be valid
		 * @param versionCode The version code of the application
		 * @return True, if succeeded
		 */
		static bool manifestVersionCode(JavaVM* javaVM, jobject activity, int& versionCode);

		/**
		 * Returns the minimum and target SDK versions stored in the application manifest.
		 * @param env The Java environment, must be valid
		 * @param activity The Android main activity, must be valid
		 * @param minSdkVersion The resulting minimum SDK version of the application as defined in its manifest
		 * @param targetSdkVersion The resulting target SDK version of the application as defined in its manifest
		 * @return True, if succeeded
		 */
		static bool manifestSdkVersions(JNIEnv* env, jobject activity, unsigned int& minSdkVersion, unsigned int& targetSdkVersion);

		/**
		 * Return the Android release version of the system (android.os.Build.VERSION.RELEASE).
		 * @param env The Java environment, must be valid
		 * @param version The resulting Android release version, only valid if this function returns 'true'
		 * @return True, if succeeded, otherwise false
		 */
		 static bool androidReleaseVersion(JNIEnv* env, std::string& version);

		/**
		 * Return the Android SDK version of the system (android.os.Build.VERSION.SDK).
		 * @param env The Java environment, must be valid
		 * @param version The resulting Android SDK version, only valid if this function returns 'true'
		 * @return True, if succeeded, otherwise false
		 */
		static bool androidSdkVersion(JNIEnv* env, unsigned int& version);

		/**
		 * Returns the value of a specific system property.
		 * @param name The name of the system property, must be valid
		 * @param value The resulting value
		 * @return True, if succeeded
		 */
		static bool systemPropertyValue(const std::string& name, std::string& value);

		/**
		 * Returns the brand string of the device.
		 * @param env The Java environment, must be valid
		 * @param brand The resulting model string
		 * @return True, if succeeded
		 */
		static bool deviceBrand(JNIEnv* env, std::string& brand);

		/**
		 * Returns the model string of the device.
		 * @param env The Java environment, must be valid
		 * @param model The resulting model string
		 * @return True, if succeeded
		 */
		static bool deviceModel(JNIEnv* env, std::string& model);

		/**
		 * Returns the serial number of the device.
		 * On Android 8.0 (API 26) and above, access to the serial number is restricted to system apps.
		 * @param serialNumber The resulting device's serial number
		 * @return True, if succeeded
		 */
		static bool deviceSerialNumber(std::string& serialNumber);

		/**
		 * Returns the name of the package.
		 * @param packageName The resulting package name
		 * @return True, if succeeded
		 */
		static bool determinePackageName(std::string& packageName);

		/**
		 * Sends an explicit intent to another component or application.
		 * @param env The Java environment, must be valid
		 * @param activity The Android main activity, must be valid
		 * @param packageName The name of the package to which the intent will be sent, must be valid
		 * @param className The name of the class or activity to which the intent will be sent, must be valid
		 * @param extraText The data that will be sent with the intent, must be valid
		 * @return True, if the intent was sent successfully, otherwise false
		 */
		static bool sendIntentToComponent(JNIEnv* env, jobject activity, const std::string& packageName, const std::string& className, const std::string& extraText);

		/**
		 * Starts an new activity from a root activity.
		 * @param env The Java environment, must be valid
		 * @param rootActivity The root activity from which the new activity will be started, must be valid
		 * @param activityClassName The name of the class of the activity to start, must be valid
		 * @return True, if succeeded
		 */
		static bool startActivity(JNIEnv* env, jobject rootActivity, const std::string& activityClassName);

		/**
		 * Joins a Wi-Fi network (WPA2-PSK).
		 * This only works with Android SDK version 28 or below.
		 * @param env The Java environment, must be valid
		 * @param activity The root activity from which the new activity will be started, must be valid
		 * @param ssid The Wi-Fi network name (SSID), the network must be using WPA2, must be valid
		 * @param password The Wi-Fi network password, must be valid
		 * @return True, if joined successfully, otherwise false
		 **/
		static bool connectToWifi(JNIEnv* env, jobject activity, const std::string& ssid, const std::string& password);

		/**
		 * Sends an intent to the OS to request it to join a Wi-Fi network (WPA2-PSK).
		 * The intent is send to the system settings app to save and connect to a new Wi-Fi network. This will display a notification to user to accept or reject the new Wi-Fi network.
		 * Additional notes:
		 *  * The Wi-Fi network must use the security protocol WPA2-PSK.
		 *  * This function requires Android SDK version 29 or higher.
		 *  * This function requires the permissions `android.permission.CHANGE_NETWORK_STATE` and `android.permission.WRITE_SETTINGS` to be set in the app manifest to work.
		 *  * Rejecting a new network does NOT revoke the permission `android.permission.CHANGE_NETWORK_STATE` again. Re-tries are possible.
		 *  * The newly added networks do NOT have to reachable at the time of adding them (they will be stored for when they become available).
		 *  * The system app receiving the intent sent here must implement a handler for the action type `Settings.ACTION_WIFI_ADD_NETWORKS`. Otherwise this function will have no effect (and still return `true`).
		 * @param env The Java environment, must be valid
		 * @param activity The root activity from which the new activity will be started, must be valid
		 * @param ssid The Wi-Fi network name (SSID), the network must be using WPA2, must be valid
		 * @param password The Wi-Fi network password, must be valid
		 * @return True, if the intent was sent successfully, otherwise false
		 **/
		static bool sendIntentToConnectToWifi(JNIEnv* env, jobject activity, const std::string& ssid, const std::string& password);

		/**
		 * Returns current Wi-Fi network name (SSID).
		 * @param env The Java environment, must be valid
		 * @param activity The root activity from which the new activity will be started, must be valid
		 * @param ssid The resulting Wi-Fi network name (SSID), will be empty if there is no Wi-Fi connection
		 * @return True, if succeeded
		 */
		static bool currentWifiSsid(JNIEnv* env, jobject activity, std::string& ssid);

		/**
		 * Triggers a vibration.
		 * This function needs permission 'android.permission.VIBRATE'.
		 * @param env The Java environment, must be valid
		 * @param activity The root activity from which the new activity will be started, must be valid
		 * @param intensity The intensity of the vibration, the higher the stronger, with range [0, 2]
		 * @param duration The duration of the vibration in milliseconds, with range [1, infinity)
		 * @return True, if succeeded
		 */
		static bool triggerVibration(JNIEnv* env, jobject activity, unsigned int intensity = 1u, const unsigned int duration = 50u);

		/**
		 * Returns the refresh rate of the display in Hz.
		 * On Android 30 (API 30) and above, uses Activity.getDisplay().
		 * On older versions, uses WindowManager.getDefaultDisplay().
		 * @param env The Java environment, must be valid
		 * @param activity The Android main activity, must be valid
		 * @param refreshRateHz The resulting refresh rate in Hz, will be 0 if the display could not be determined
		 * @return True, if succeeded
		 */
		static bool displayRefreshRate(JNIEnv* env, jobject activity, float& refreshRateHz);

	protected:

		/**
		 * Returns whether all given data types can be handed to a JNI function call with variable arguments.
		 * A JNI argument is either a reference like jobject or jstring, or a primitive like jint or jboolean, as any other type would be pushed onto the variable argument list of the runtime.
		 * Beware: This does not verify that the arguments match the signature of the Java method, e.g., an jint handed to a method expecting a jlong is still wrong.
		 * @return True, if so
		 * @tparam TArgs The data types to be checked, can be empty
		 */
		template <typename... TArgs>
		static constexpr bool isValidArgumentTypes();
};

template <typename... TArgs>
constexpr bool Utilities::isValidArgumentTypes()
{
	return ((std::is_pointer<typename std::decay<TArgs>::type>::value || std::is_arithmetic<typename std::decay<TArgs>::type>::value) && ...);
}

template <typename T, typename... TArgs>
ScopedJNILocalObject<T> Utilities::callObjectMethod(JNIEnv& jniEnvironment, jobject object, jmethodID methodId, TArgs&&... args)
{
	static_assert(isValidArgumentTypes<TArgs...>(), "A JNI argument must be a JNI reference or a JNI primitive!");

	ocean_assert(object != nullptr);
	ocean_assert(methodId != nullptr);

	ScopedJNILocalObject<T> result(jniEnvironment, T(jniEnvironment.CallObjectMethod(object, methodId, std::forward<TArgs>(args)...)));

	if (clearPotentialException(jniEnvironment))
	{
		// the return value is undefined in case the method has thrown an exception

		return ScopedJNILocalObject<T>();
	}

	return result;
}

template <typename T, typename... TArgs>
ScopedJNILocalObject<T> Utilities::callStaticObjectMethod(JNIEnv& jniEnvironment, jclass javaClass, jmethodID methodId, TArgs&&... args)
{
	static_assert(isValidArgumentTypes<TArgs...>(), "A JNI argument must be a JNI reference or a JNI primitive!");

	ocean_assert(javaClass != nullptr);
	ocean_assert(methodId != nullptr);

	ScopedJNILocalObject<T> result(jniEnvironment, T(jniEnvironment.CallStaticObjectMethod(javaClass, methodId, std::forward<TArgs>(args)...)));

	if (clearPotentialException(jniEnvironment))
	{
		return ScopedJNILocalObject<T>();
	}

	return result;
}

template <typename T, typename... TArgs>
ScopedJNILocalObject<T> Utilities::newObject(JNIEnv& jniEnvironment, jclass javaClass, jmethodID methodId, TArgs&&... args)
{
	static_assert(isValidArgumentTypes<TArgs...>(), "A JNI argument must be a JNI reference or a JNI primitive!");

	ocean_assert(javaClass != nullptr);
	ocean_assert(methodId != nullptr);

	ScopedJNILocalObject<T> result(jniEnvironment, T(jniEnvironment.NewObject(javaClass, methodId, std::forward<TArgs>(args)...)));

	if (clearPotentialException(jniEnvironment))
	{
		return ScopedJNILocalObject<T>();
	}

	return result;
}

template <typename... TArgs>
bool Utilities::callVoidMethod(JNIEnv& jniEnvironment, jobject object, jmethodID methodId, TArgs&&... args)
{
	static_assert(isValidArgumentTypes<TArgs...>(), "A JNI argument must be a JNI reference or a JNI primitive!");

	ocean_assert(object != nullptr);
	ocean_assert(methodId != nullptr);

	jniEnvironment.CallVoidMethod(object, methodId, std::forward<TArgs>(args)...);

	return !clearPotentialException(jniEnvironment);
}

template <typename... TArgs>
bool Utilities::callBooleanMethod(JNIEnv& jniEnvironment, jobject object, jmethodID methodId, bool& returnValue, TArgs&&... args)
{
	static_assert(isValidArgumentTypes<TArgs...>(), "A JNI argument must be a JNI reference or a JNI primitive!");

	ocean_assert(object != nullptr);
	ocean_assert(methodId != nullptr);

	const jboolean result = jniEnvironment.CallBooleanMethod(object, methodId, std::forward<TArgs>(args)...);

	if (clearPotentialException(jniEnvironment))
	{
		return false;
	}

	returnValue = result == JNI_TRUE;

	return true;
}

template <typename... TArgs>
bool Utilities::callStaticBooleanMethod(JNIEnv& jniEnvironment, jclass javaClass, jmethodID methodId, bool& returnValue, TArgs&&... args)
{
	static_assert(isValidArgumentTypes<TArgs...>(), "A JNI argument must be a JNI reference or a JNI primitive!");

	ocean_assert(javaClass != nullptr);
	ocean_assert(methodId != nullptr);

	const jboolean result = jniEnvironment.CallStaticBooleanMethod(javaClass, methodId, std::forward<TArgs>(args)...);

	if (clearPotentialException(jniEnvironment))
	{
		return false;
	}

	returnValue = result == JNI_TRUE;

	return true;
}

template <typename... TArgs>
bool Utilities::callIntMethod(JNIEnv& jniEnvironment, jobject object, jmethodID methodId, int32_t& returnValue, TArgs&&... args)
{
	static_assert(isValidArgumentTypes<TArgs...>(), "A JNI argument must be a JNI reference or a JNI primitive!");

	ocean_assert(object != nullptr);
	ocean_assert(methodId != nullptr);

	const jint result = jniEnvironment.CallIntMethod(object, methodId, std::forward<TArgs>(args)...);

	if (clearPotentialException(jniEnvironment))
	{
		return false;
	}

	returnValue = int32_t(result);

	return true;
}

template <typename... TArgs>
bool Utilities::callStaticIntMethod(JNIEnv& jniEnvironment, jclass javaClass, jmethodID methodId, int32_t& returnValue, TArgs&&... args)
{
	static_assert(isValidArgumentTypes<TArgs...>(), "A JNI argument must be a JNI reference or a JNI primitive!");

	ocean_assert(javaClass != nullptr);
	ocean_assert(methodId != nullptr);

	const jint result = jniEnvironment.CallStaticIntMethod(javaClass, methodId, std::forward<TArgs>(args)...);

	if (clearPotentialException(jniEnvironment))
	{
		return false;
	}

	returnValue = int32_t(result);

	return true;
}

template <typename... TArgs>
bool Utilities::callFloatMethod(JNIEnv& jniEnvironment, jobject object, jmethodID methodId, float& returnValue, TArgs&&... args)
{
	static_assert(isValidArgumentTypes<TArgs...>(), "A JNI argument must be a JNI reference or a JNI primitive!");

	ocean_assert(object != nullptr);
	ocean_assert(methodId != nullptr);

	const jfloat result = jniEnvironment.CallFloatMethod(object, methodId, std::forward<TArgs>(args)...);

	if (clearPotentialException(jniEnvironment))
	{
		return false;
	}

	returnValue = float(result);

	return true;
}

}

}

}

#endif // META_OCEAN_PLATFORM_ANDROID_UTILITIES_H
