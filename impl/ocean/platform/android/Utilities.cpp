/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ocean/platform/android/Utilities.h"
#include "ocean/platform/android/ScopedJNIEnvironment.h"
#include "ocean/platform/android/ScopedJNIObject.h"

#include <dirent.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/system_properties.h>

namespace Ocean
{

namespace Platform
{

namespace Android
{

std::string Utilities::toAString(JNIEnv* env, jstring javaString)
{
	if (javaString == nullptr)
	{
		// e.g., if a Java String is null
		return std::string();
	}

	ocean_assert(env != nullptr);

	jboolean isCopy = false;
	const char* stringPointer = env->GetStringUTFChars(javaString, &isCopy);

	std::string result;
	if (stringPointer)
	{
		result = std::string(stringPointer);
	}

	env->ReleaseStringUTFChars(javaString, stringPointer);
	return result;
}

jstring Utilities::toJavaString(JNIEnv* env, const std::string& stdString)
{
	ocean_assert(env != nullptr);

	return env->NewStringUTF(stdString.c_str());
}

jobjectArray Utilities::toJavaStringArray(JNIEnv* env, const std::vector<std::string>& strings)
{
	ocean_assert(env != nullptr);

	jobjectArray result = env->NewObjectArray(strings.size(), env->FindClass("java/lang/String"), env->NewStringUTF(""));

	for (size_t n = 0; n < strings.size(); ++n)
	{
		env->SetObjectArrayElement(result, n, env->NewStringUTF(strings[n].c_str()));
	}

	return result;
}

bool Utilities::toVector(JNIEnv* env, jobject javaStringList, std::vector<std::string>& strings)
{
	ocean_assert(env != nullptr);
	ocean_assert(javaStringList != nullptr);

	const ScopedJClass javaClassList(*env, env->FindClass("java/util/List"));

	if (!javaClassList)
	{
		return false;
	}

	const jmethodID functionId = env->GetMethodID(javaClassList, "toArray", "()[Ljava/lang/Object;");

	if (functionId == nullptr)
	{
		return false;
	}

	const ScopedJObjectArray javaArray(*env, (jobjectArray)(env->CallObjectMethod(javaStringList, functionId)));

	if (!javaArray)
	{
		return false;
	}

	const jsize size = env->GetArrayLength(javaArray);

	if (size < 0)
	{
		return false;
	}

	strings.clear();

	if (size == 0)
	{
		return true;
	}

	strings.reserve(size_t(size));

	for (jsize n = 0; n < size; ++n)
	{
		const ScopedJString javaString(*env, (jstring)(env->GetObjectArrayElement(javaArray, n)));

		if (!javaString)
		{
			return false;
		}

		strings.emplace_back(toAString(env, javaString));
	}
	
	return true;
}

bool Utilities::toVector(JNIEnv* env, jobject javaIntegerList, std::vector<int>& values)
{
	ocean_assert(env != nullptr);
	ocean_assert(javaIntegerList != nullptr);

	const ScopedJClass javaClassList(*env, env->FindClass("java/util/List"));

	if (!javaClassList.isValid())
	{
		return false;
	}

	const jmethodID functionIdToArray = env->GetMethodID(javaClassList, "toArray", "()[Ljava/lang/Object;");

	if (functionIdToArray == nullptr)
	{
		return false;
	}

	const ScopedJObjectArray javaArray(*env, (jobjectArray)(env->CallObjectMethod(javaIntegerList, functionIdToArray)));

	if (!javaArray)
	{
		return false;
	}

	const jsize size = env->GetArrayLength(javaArray);

	if (size < 0)
	{
		return false;
	}

	values.clear();

	if (size == 0)
	{
		return true;
	}

	const ScopedJClass javaClassInteger(*env, env->FindClass("java/lang/Integer"));

	if (!javaClassInteger)
	{
		return false;
	}

	const jmethodID jFunctionIdIntValue = env->GetMethodID(javaClassInteger, "intValue", "()I");

	if (jFunctionIdIntValue == nullptr)
	{
		return false;
	}

	for (jsize n = 0; n < size; ++n)
	{
		const ScopedJObject javaObject(*env, (jstring)(env->GetObjectArrayElement(javaArray, n)));

		if (!javaObject)
		{
			return false;
		}

		values.emplace_back(env->CallIntMethod(javaObject, jFunctionIdIntValue));
	}
	
	return true;
}

bool Utilities::className(JNIEnv* env, jobject object, std::string& name)
{
	ocean_assert(env != nullptr);
	ocean_assert(object != nullptr);

	const ScopedJClass objectClass(*env, env->GetObjectClass(object));

	if (!objectClass.isValid())
	{
		return false;
	}

	jmethodID getClassMethodId =  env->GetMethodID(objectClass, "getClass", "()Ljava/lang/Class;");

	if (getClassMethodId == nullptr)
	{
		return false;
	}

	const ScopedJObject classObject(*env, env->CallObjectMethod(object, getClassMethodId));

	if (!classObject.isValid())
	{
		return false;
	}

	const ScopedJClass classDescriptor(*env, env->GetObjectClass(classObject));

	if (!classDescriptor.isValid())
	{
		return false;
	}

	jmethodID getNameMethodId =  env->GetMethodID(classDescriptor, "getName", "()Ljava/lang/String;");

	if (getNameMethodId == nullptr)
	{
		return false;
	}

	const ScopedJString constructorName(*env, (jstring)env->CallObjectMethod(classObject, getNameMethodId));

	if (!constructorName.isValid())
	{
		return false;
	}

	name = toAString(env, constructorName);

	return !name.empty();
}

bool Utilities::manifestVersion(JavaVM* javaVM, jobject activity, int& versionCode, std::string& versionName)
{
	ocean_assert(javaVM != nullptr && activity != nullptr);

	if (javaVM == nullptr || activity == nullptr)
	{
		return false;
	}

	const ScopedJNIEnvironment scopedJNIEnvironment(javaVM);

	if (!scopedJNIEnvironment)
	{
		return false;
	}

	const ScopedJClass activityClass(scopedJNIEnvironment, scopedJNIEnvironment.jniEnv()->GetObjectClass(activity));

	if (!activityClass.isValid())
	{
		return false;
	}

	jmethodID getPackageNameMethodId = scopedJNIEnvironment.jniEnv()->GetMethodID(activityClass, "getPackageName", "()Ljava/lang/String;");

	if (getPackageNameMethodId == nullptr)
	{
		return false;
	}

	jmethodID getPackageManagerMethodId =  scopedJNIEnvironment.jniEnv()->GetMethodID(activityClass, "getPackageManager", "()Landroid/content/pm/PackageManager;");

	if (getPackageManagerMethodId == nullptr)
	{
		return false;
	}

	const ScopedJClass javaClassPackageManager(scopedJNIEnvironment, scopedJNIEnvironment.jniEnv()->FindClass("android/content/pm/PackageManager"));

	if (!javaClassPackageManager.isValid())
	{
		return false;
	}

	jmethodID getPackageInfoMethodId = scopedJNIEnvironment.jniEnv()->GetMethodID(javaClassPackageManager, "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");

	if (getPackageInfoMethodId == nullptr)
	{
		return false;
	}

	const ScopedJClass javaClassPackageInfo(scopedJNIEnvironment, scopedJNIEnvironment.jniEnv()->FindClass("android/content/pm/PackageInfo"));

	if (!javaClassPackageInfo.isValid())
	{
		return false;
	}

	const jfieldID versionCodeFieldId = scopedJNIEnvironment.jniEnv()->GetFieldID(javaClassPackageInfo, "versionCode", "I");

	if (versionCodeFieldId == nullptr)
	{
		return false;
	}

	const jfieldID versionNameFieldId = scopedJNIEnvironment.jniEnv()->GetFieldID(javaClassPackageInfo, "versionName", "Ljava/lang/String;");

	if (versionNameFieldId == nullptr)
	{
		return false;
	}

	const ScopedJString packageName(scopedJNIEnvironment, (jstring)scopedJNIEnvironment.jniEnv()->CallObjectMethod(activity, getPackageNameMethodId));

	if (!packageName.isValid())
	{
		return false;
	}

	const ScopedJObject packageManager(scopedJNIEnvironment, scopedJNIEnvironment.jniEnv()->CallObjectMethod(activity, getPackageManagerMethodId));

	if (!packageManager.isValid())
	{
		return false;
	}

	const ScopedJObject packageInfo(scopedJNIEnvironment, scopedJNIEnvironment.jniEnv()->CallObjectMethod(packageManager, getPackageInfoMethodId, *packageName, 0x0));

	if (!packageInfo.isValid())
	{
		return false;
	}

	versionCode = scopedJNIEnvironment.jniEnv()->GetIntField(packageInfo, versionCodeFieldId);

	const ScopedJString versionNameFieldValue(scopedJNIEnvironment, (jstring)(scopedJNIEnvironment.jniEnv()->GetObjectField(packageInfo, versionNameFieldId)));

	if (versionNameFieldValue == nullptr)
	{
		return false;
	}

	versionName = Utilities::toAString(scopedJNIEnvironment.jniEnv(), versionNameFieldValue);

	return true;
}

bool Utilities::manifestVersionCode(JavaVM* javaVM, jobject activity, int& versionCode)
{
	std::string unusedVersionName;

	return manifestVersion(javaVM, activity, versionCode, unusedVersionName);
}

bool Utilities::manifestSdkVersions(JNIEnv* env, jobject activity, unsigned int& minSdkVersion, unsigned int& targetSdkVersion)
{
	ocean_assert(env != nullptr && activity != nullptr);

	if (env == nullptr || activity == nullptr)
	{
		return false;
	}

	const ScopedJClass jActivityClass(*env, env->GetObjectClass(activity));

	if (!jActivityClass.isValid())
	{
		return false;
	}

	// PackageManager packageManager = getActivity().getPackageManager();
	// String packageName = packageManager.getPackageName()
	// ApplicationInfo applicationInfo = getActivity().getApplicationInfo(packageName, 0);
	jmethodID jGetPackageNameMethodId = env->GetMethodID(jActivityClass, "getPackageName", "()Ljava/lang/String;");
	jmethodID jGetPackageManagerMethodId =  env->GetMethodID(jActivityClass, "getPackageManager", "()Landroid/content/pm/PackageManager;");

	if (jGetPackageNameMethodId == nullptr || jGetPackageManagerMethodId == nullptr)
	{
		return false;
	}

	const ScopedJClass jPackageManagerClass(*env, env->FindClass("android/content/pm/PackageManager"));
	const ScopedJClass jApplicationInfoClass(*env, env->FindClass("android/content/pm/ApplicationInfo"));

	if (!jPackageManagerClass.isValid() || !jApplicationInfoClass.isValid())
	{
		return false;
	}

	const ScopedJObject jPackageManagerObject(*env, env->CallObjectMethod(activity, jGetPackageManagerMethodId));

	if (!jPackageManagerObject.isValid())
	{
		return false;
	}

	jmethodID jGetApplicationInfoMethodId = env->GetMethodID(jPackageManagerClass, "getApplicationInfo", "(Ljava/lang/String;I)Landroid/content/pm/ApplicationInfo;");

	if (jGetApplicationInfoMethodId == nullptr)
	{
		return false;
	}

	const ScopedJString jPackageNameString(*env, (jstring)env->CallObjectMethod(activity, jGetPackageNameMethodId));

	if (!jPackageNameString.isValid())
	{
		return false;
	}

	const ScopedJObject jApplicationInfoObject(*env, env->CallObjectMethod(jPackageManagerObject, jGetApplicationInfoMethodId, *jPackageNameString, 0x0));

	if (!jApplicationInfoObject.isValid())
	{
		return false;
	}

	// int minSdkVersion = applicationInfo.minSdkVersion;
	// int targetSdkVersion = applicationInfo.targetSdkVersion;
	jfieldID jMinSdkVersionFieldId = env->GetFieldID(jApplicationInfoClass, "minSdkVersion", "I");
	jfieldID jTargetSdkVersionFieldId = env->GetFieldID(jApplicationInfoClass, "targetSdkVersion", "I");

	if (jMinSdkVersionFieldId == nullptr || jTargetSdkVersionFieldId == nullptr)
	{
		return false;
	}

	const jint jMinSdkVersion = env->GetIntField(jApplicationInfoObject, jMinSdkVersionFieldId);
	const jint jTargetSdkVersion = env->GetIntField(jApplicationInfoObject, jTargetSdkVersionFieldId);

	if (jMinSdkVersion < 0 || jTargetSdkVersion < 0)
	{
		ocean_assert(false && "Invalid version values");
		return false;
	}

	minSdkVersion = (unsigned int)(jMinSdkVersion);
	targetSdkVersion = (unsigned int)(jTargetSdkVersion);

	return true;
}

bool Utilities::androidSdkVersion(JNIEnv* env, unsigned int& version)
{
	ocean_assert(env != nullptr);

	const ScopedJClass jVersionClass(*env, env->FindClass("android/os/Build$VERSION"));

	if (!jVersionClass)
	{
		return false;
	}

	jfieldID jSdkIntField = env->GetStaticFieldID(jVersionClass, "SDK_INT", "I");

	if (jSdkIntField == nullptr)
	{
		return false;
	}

	const jint jVersion = env->GetStaticIntField(jVersionClass, jSdkIntField);

	if (jVersion < 0)
	{
		ocean_assert(false && "Invalid version value");
		return false;
	}

	version = (unsigned int)jVersion;

	return true;
}

bool Utilities::systemPropertyValue(const std::string& name, std::string& value)
{
	ocean_assert(!name.empty());

	char buffer[PROP_VALUE_MAX + 1];
	buffer[PROP_VALUE_MAX] = '\0';

	if (__system_property_get(name.c_str(), buffer) != 0)
	{
		value = std::string(buffer);

		return true;
	}

	return false;
}

bool Utilities::deviceBrand(JNIEnv* env, std::string& brand)
{
	ocean_assert(env != nullptr);

	const ScopedJClass javaClassBuild(*env, env->FindClass("android/os/Build"));

	if (!javaClassBuild.isValid())
	{
		return false;
	}

	jfieldID fieldId = env->GetStaticFieldID(javaClassBuild, "BRAND", "Ljava/lang/String;");

	if (fieldId == nullptr)
	{
		return false;
	}

	const ScopedJString fieldValue(*env, (jstring)(env->GetStaticObjectField(javaClassBuild, fieldId)));

	if (fieldValue == nullptr)
	{
		return false;
	}

	brand = Utilities::toAString(env, fieldValue);

	return true;
}

bool Utilities::deviceModel(JNIEnv* env, std::string& model)
{
	ocean_assert(env != nullptr);

	const ScopedJClass javaClassBuild(*env, env->FindClass("android/os/Build"));

	if (!javaClassBuild.isValid())
	{
		return false;
	}

	jfieldID fieldId = env->GetStaticFieldID(javaClassBuild, "MODEL", "Ljava/lang/String;");

	if (fieldId == nullptr)
	{
		return false;
	}

	const ScopedJString fieldValue(*env, (jstring)(env->GetStaticObjectField(javaClassBuild, fieldId)));

	if (fieldValue == nullptr)
	{
		return false;
	}

	model = Utilities::toAString(env, fieldValue);

	return true;
}

bool Utilities::determinePackageName(std::string& packageName)
{
	std::ifstream stream;
	stream.open(std::string("/proc/self/cmdline").c_str(), std::ios::binary);

	if (stream.good())
	{
		std::string line;
		std::getline(stream, line);

		line = String::trimWhitespace(line);

		if (!line.empty())
		{
			packageName = line;
			return true;
		}
	}

	return false;
}

bool Utilities::sendIntentToComponent(JNIEnv* env, jobject activity, const std::string &packageName, const std::string &className, const std::string &extraText)
{
	ocean_assert(env != nullptr && activity != nullptr);
	ocean_assert(!packageName.empty() && !className.empty());
	ocean_assert(!extraText.empty());

	const ScopedJClass jActivityClass(*env, env->GetObjectClass(activity));

	if (!jActivityClass)
	{
		return false;
	}

	const ScopedJString jPackageName(*env, env->NewStringUTF(packageName.c_str()));
	const ScopedJString jClassName(*env, env->NewStringUTF(className.c_str()));
	const ScopedJString jExtraText(*env, env->NewStringUTF(extraText.c_str()));

	const ScopedJClass jComponentNameClass(*env, env->FindClass("android/content/ComponentName"));

	if (!jComponentNameClass)
	{
		return false;
	}

	jmethodID jNewComponentNameMethod = env->GetMethodID(jComponentNameClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");

	if (!jNewComponentNameMethod)
	{
		return false;
	}

	const ScopedJObject jComponentNameObject(*env, env->NewObject(jComponentNameClass, jNewComponentNameMethod, *jPackageName, *jClassName));

	if (!jNewComponentNameMethod)
	{
		return false;
	}

	const ScopedJClass jIntentClass(*env, env->FindClass("android/content/Intent"));

	if (!jIntentClass)
	{
		return false;
	}

	jfieldID jActionSendField = env->GetStaticFieldID(jIntentClass, "ACTION_SEND", "Ljava/lang/String;");
	jfieldID jExtraTextField = env->GetStaticFieldID(jIntentClass, "EXTRA_TEXT", "Ljava/lang/String;");

	jstring jActionSendValue = (jstring)(env->GetStaticObjectField(*jIntentClass, jActionSendField));
	jstring jExtraTextValue = (jstring)(env->GetStaticObjectField(*jIntentClass, jExtraTextField));

	jmethodID jNewIntentMethod = env->GetMethodID(*jIntentClass, "<init>", "(Ljava/lang/String;)V");
	jmethodID jPutExtraMethod = env->GetMethodID(*jIntentClass, "putExtra", "(Ljava/lang/String;Ljava/lang/String;)Landroid/content/Intent;");
	jmethodID jSetTypeMethod = env->GetMethodID(*jIntentClass, "setType", "(Ljava/lang/String;)Landroid/content/Intent;");
	jmethodID jSetComponentNameMethod = env->GetMethodID(*jIntentClass, "setComponent","(Landroid/content/ComponentName;)Landroid/content/Intent;");

	if (!jNewComponentNameMethod || !jPutExtraMethod || !jSetTypeMethod || !jSetComponentNameMethod)
	{
		return false;
	}

	ScopedJObject jIntentObject(*env, env->NewObject(*jIntentClass, jNewIntentMethod, jActionSendValue));

	if (!jIntentObject)
	{
		return false;
	}

	env->CallObjectMethod(*jIntentObject, jPutExtraMethod, jExtraTextValue, *jExtraText);

	const ScopedJString jMimeType(*env, env->NewStringUTF("text/plain"));
	env->CallObjectMethod(*jIntentObject, jSetTypeMethod, *jMimeType);

	env->CallObjectMethod(*jIntentObject, jSetComponentNameMethod, *jComponentNameObject);

	jmethodID jStartActivityMethod = env->GetMethodID(*jActivityClass, "startActivity", "(Landroid/content/Intent;)V");

	if (!jStartActivityMethod)
	{
		return false;
	}

	env->CallVoidMethod(activity, jStartActivityMethod, *jIntentObject);

	return true;
}

bool Utilities::startActivity(JNIEnv* env, jobject rootActivity, const std::string& activityClassName)
{
	ocean_assert(env != nullptr && rootActivity != nullptr);
	ocean_assert(!activityClassName.empty());

	const ScopedJClass jRootActivityClass(*env, env->GetObjectClass(rootActivity));

	if (!jRootActivityClass)
	{
		return false;
	}

	jmethodID jStartActivityMethod = env->GetMethodID(*jRootActivityClass, "startActivity", "(Landroid/content/Intent;)V");

	if (!jStartActivityMethod)
	{
		return false;
	}

	const ScopedJClass jIntentClass(*env, env->FindClass("android/content/Intent"));

	if (!jIntentClass)
	{
		return false;
	}

	jmethodID jNewIntentMethod = env->GetMethodID(*jIntentClass, "<init>", "(Landroid/content/Context;Ljava/lang/Class;)V");

	if (!jNewIntentMethod)
	{
		return false;
	}

	const ScopedJClass jActivityClass(*env, env->FindClass(activityClassName.c_str()));

	ScopedJObject jIntentObject(*env, env->NewObject(*jIntentClass, jNewIntentMethod, rootActivity, *jActivityClass));

	if (!jIntentObject)
	{
		return false;
	}

	env->CallVoidMethod(rootActivity, jStartActivityMethod, *jIntentObject);

	return true;
}

bool Utilities::connectToWifi(JNIEnv* env, jobject activity, const std::string& ssid, const std::string& password)
{
	ocean_assert(env != nullptr && activity != nullptr);
	ocean_assert(!ssid.empty() &&!password.empty());

	unsigned int minSdkVersion = 0u;
	unsigned int targetSdkVersion = 0u;
	if (!manifestSdkVersions(env, activity, minSdkVersion, targetSdkVersion) || std::max(minSdkVersion, targetSdkVersion) > 28u)
	{
		Log::error() << "Functionality not available. Android SDK version 28 or lower is required (current target version: " << targetSdkVersion << ").";
		return false;
	}

	const ScopedJClass jActivityClass(*env, env->GetObjectClass(activity));

	if (!jActivityClass)
	{
		return false;
	}

	// WifiConfiguration conf = new WifiConfiguration();
	// conf.SSID = "\"" + wifiName + "\"";   // Please note the quotes. String should contain SSID in quotes
	// conf.preSharedKey = "\"" + wifiPassword + "\"";
	// conf.status = WifiConfiguration.Status.ENABLED;
	const std::string paddedSsid = "\"" + ssid + "\"";
	const std::string paddedPassword = "\"" + password + "\"";

	const ScopedJString jSsidString(*env, env->NewStringUTF(paddedSsid.c_str()));
	const ScopedJString jPasswordString(*env, env->NewStringUTF(paddedPassword.c_str()));

	if (!jSsidString.isValid() || !jPasswordString.isValid())
	{
		return false;
	}

	const ScopedJClass jWifiConfigurationClass(*env, env->FindClass("android/net/wifi/WifiConfiguration"));

	if (!jWifiConfigurationClass)
	{
		return false;
	}

	jmethodID jNewWifiConfigurationMethod = env->GetMethodID(jWifiConfigurationClass, "<init>", "()V");

	if (!jNewWifiConfigurationMethod)
	{
		return false;
	}

	const ScopedJObject jWifiConfigurationObject(*env, env->NewObject(jWifiConfigurationClass, jNewWifiConfigurationMethod));

	if (!jWifiConfigurationObject)
	{
		return false;
	}

	const ScopedJClass jStatusClass(*env, env->FindClass("android/net/wifi/WifiConfiguration$Status"));

	if (!jStatusClass)
	{
		return false;
	}

	jfieldID jEnabledField = env->GetStaticFieldID(jStatusClass, "ENABLED", "I");

	if (jEnabledField == nullptr)
	{
		return false;
	}

	const jint jEnabled = env->GetStaticIntField(jStatusClass, jEnabledField);

	if (jEnabled < 0)
	{
		ocean_assert(false && "Invalid field value in android/net/wifi/WifiConfiguration$Status");
		return false;
	}

	jfieldID jSsidField = env->GetFieldID(jWifiConfigurationClass, "SSID", "Ljava/lang/String;");
	jfieldID jPreSharedKeyField = env->GetFieldID(jWifiConfigurationClass, "preSharedKey", "Ljava/lang/String;");
	jfieldID jStatus = env->GetFieldID(jWifiConfigurationClass, "status", "I");

	if (jSsidField == nullptr || jPreSharedKeyField == nullptr ||jStatus == nullptr)
	{
		return false;
	}

	env->SetObjectField(*jWifiConfigurationObject, jSsidField, *jSsidString);
	env->SetObjectField(*jWifiConfigurationObject, jPreSharedKeyField, *jPasswordString);
	env->SetIntField(*jWifiConfigurationObject, jStatus, jEnabled);

	// conf.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);
	// conf.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
	// conf.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
	// conf.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
	// conf.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
	const ScopedJClass jGroupCipherClass(*env, env->FindClass("android/net/wifi/WifiConfiguration$GroupCipher"));
	const ScopedJClass jKeyMgmtClass(*env, env->FindClass("android/net/wifi/WifiConfiguration$KeyMgmt"));
	const ScopedJClass jPairwiseCipherClass(*env, env->FindClass("android/net/wifi/WifiConfiguration$PairwiseCipher"));

	if (!jGroupCipherClass || !jKeyMgmtClass ||!jPairwiseCipherClass)
	{
		return false;
	}

	jfieldID jGroupCipherTkipField = env->GetStaticFieldID(jGroupCipherClass, "TKIP", "I");
	jfieldID jGroupCipherCcmpField = env->GetStaticFieldID(jGroupCipherClass, "CCMP", "I");

	jfieldID jKeyMgmtWpaPskField = env->GetStaticFieldID(jKeyMgmtClass, "WPA_PSK", "I");

	jfieldID jPairwiseCipherTkipField = env->GetStaticFieldID(jPairwiseCipherClass, "TKIP", "I");
	jfieldID jPairwiseCipherCcmpField = env->GetStaticFieldID(jPairwiseCipherClass, "CCMP", "I");

	if (jGroupCipherTkipField == nullptr || jGroupCipherCcmpField == nullptr || jKeyMgmtWpaPskField == nullptr || jPairwiseCipherTkipField == nullptr || jPairwiseCipherCcmpField == nullptr)
	{
		return false;
	}

	const jint jGroupCipherTkip = env->GetStaticIntField(jGroupCipherClass, jGroupCipherTkipField);
	const jint jGroupCipherCcmp = env->GetStaticIntField(jGroupCipherClass, jGroupCipherCcmpField);

	const jint jKeyMgmtWpaPsk = env->GetStaticIntField(jKeyMgmtClass, jKeyMgmtWpaPskField);

	const jint jPairwiseCipherTkip = env->GetStaticIntField(jPairwiseCipherClass, jPairwiseCipherTkipField);
	const jint jPairwiseCipherCcmp = env->GetStaticIntField(jPairwiseCipherClass, jPairwiseCipherCcmpField);

	if (jGroupCipherTkip < 0 || jGroupCipherCcmp < 0 || jKeyMgmtWpaPsk < 0 || jPairwiseCipherTkip < 0 || jPairwiseCipherCcmp < 0)
	{
		ocean_assert(false && "Invalid field value");
		return false;
	}

	const ScopedJClass jBitSetClass(*env, env->FindClass("java/util/BitSet"));

	if (!jBitSetClass)
	{
		return false;
	}

	jmethodID jSetIntMethod = env->GetMethodID(jBitSetClass, "set", "(I)V");

	if (!jSetIntMethod)
	{
		return false;
	}

	jfieldID jAllowedGroupCiphersField = env->GetFieldID(jWifiConfigurationClass, "allowedGroupCiphers", "Ljava/util/BitSet;");
	jfieldID jAllowedKeyManagementField = env->GetFieldID(jWifiConfigurationClass, "allowedKeyManagement", "Ljava/util/BitSet;");
	jfieldID jAllowedPairwiseCiphersField = env->GetFieldID(jWifiConfigurationClass, "allowedPairwiseCiphers", "Ljava/util/BitSet;");

	if (jAllowedGroupCiphersField == nullptr || jAllowedKeyManagementField == nullptr || jAllowedPairwiseCiphersField == nullptr)
	{
		return false;
	}

	const ScopedJObject jAllowedGroupCiphersObject(*env, env->GetObjectField(jWifiConfigurationObject, jAllowedGroupCiphersField));
	const ScopedJObject jAllowedKeyManagementObject(*env, env->GetObjectField(jWifiConfigurationObject, jAllowedKeyManagementField));
	const ScopedJObject jAllowedPairwiseCiphersObject(*env, env->GetObjectField(jWifiConfigurationObject, jAllowedPairwiseCiphersField));

	if (!jAllowedGroupCiphersObject || !jAllowedKeyManagementObject ||!jAllowedPairwiseCiphersObject)
	{
		return false;
	}

	env->CallVoidMethod(jAllowedGroupCiphersObject, jSetIntMethod, jGroupCipherTkip);
	env->CallVoidMethod(jAllowedGroupCiphersObject, jSetIntMethod, jGroupCipherCcmp);

	env->CallVoidMethod(jAllowedKeyManagementObject, jSetIntMethod, jKeyMgmtWpaPsk);

	env->CallVoidMethod(jAllowedPairwiseCiphersObject, jSetIntMethod, jPairwiseCipherTkip);
	env->CallVoidMethod(jAllowedPairwiseCiphersObject, jSetIntMethod, jPairwiseCipherCcmp);

	// WifiManager wifiManager = (WifiManager)context.getSystemService(Context.WIFI_SERVICE);
	// int newNetworkID = wifiManager.addNetwork(conf);
	const ScopedJClass jContextClass(*env, env->FindClass("android/content/Context"));
	const ScopedJClass jWifiManagerClass(*env, env->FindClass("android/net/wifi/WifiManager"));

	if (!jContextClass || !jWifiManagerClass)
	{
		return false;
	}

	jfieldID jWifiServiceField = env->GetStaticFieldID(jContextClass, "WIFI_SERVICE", "Ljava/lang/String;");

	if (jWifiServiceField == nullptr)
	{
		return false;
	}

	const ScopedJString jWifiServiceString(*env, (jstring)(env->GetStaticObjectField(jContextClass, jWifiServiceField)));

	if (!jWifiServiceString.isValid())
	{
		return false;
	}

	jmethodID jGetSystemServiceMethod = env->GetMethodID(jContextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	jmethodID jAddNetworkMethod = env->GetMethodID(jWifiManagerClass, "addNetwork", "(Landroid/net/wifi/WifiConfiguration;)I");

	if (jGetSystemServiceMethod == nullptr || jAddNetworkMethod == nullptr)
	{
		return false;
	}

	const ScopedJObject jWifiManagerObject(*env, env->CallObjectMethod(activity, jGetSystemServiceMethod, *jWifiServiceString));

	if (!jWifiManagerObject)
	{
		return false;
	}

	jint newNetworkID = env->CallIntMethod(jWifiManagerObject, jAddNetworkMethod, *jWifiConfigurationObject);

	if (newNetworkID < 0)
	{
		return false;
	}

	// wifiManager.disconnect();
	// wifiManager.enableNetwork(newNetworkID, true);
	// wifiManager.reconnect();
	jmethodID jDisconnectMethod = env->GetMethodID(jWifiManagerClass, "disconnect", "()Z");
	jmethodID jEnableNetworkMethod = env->GetMethodID(jWifiManagerClass, "enableNetwork", "(IZ)Z");
	jmethodID jReconnectMethod = env->GetMethodID(jWifiManagerClass, "reconnect", "()Z");

	if (!jDisconnectMethod || !jEnableNetworkMethod || !jReconnectMethod)
	{
		return false;
	}

	if (!env->CallBooleanMethod(jWifiManagerObject, jDisconnectMethod))
	{
		return false;
	}

	if (!env->CallBooleanMethod(jWifiManagerObject, jEnableNetworkMethod, newNetworkID, /* attempt connect */ true))
	{
		return false;
	}

	if (!env->CallBooleanMethod(jWifiManagerObject, jReconnectMethod))
	{
		return false;
	}

	return true;
}

bool Utilities::sendIntentToConnectToWifi(JNIEnv* env, jobject activity, const std::string& ssid, const std::string& password)
{
	ocean_assert(env != nullptr && activity != nullptr);
	ocean_assert(!ssid.empty() &&!password.empty());

	unsigned int minSdkVersion = 0u;
	unsigned int targetSdkVersion = 0u;
	if (!manifestSdkVersions(env, activity, minSdkVersion, targetSdkVersion) || std::max(minSdkVersion, targetSdkVersion) < 29u)
	{
		Log::error() << "Functionality not available. Android SDK version 29 or higher is required (current target version: " << targetSdkVersion << ").";
		return false;
	}

	const ScopedJString jSsidString(*env, env->NewStringUTF(ssid.c_str()));
	const ScopedJString jPasswordString(*env, env->NewStringUTF(password.c_str()));

	if (!jSsidString.isValid() || !jPasswordString.isValid())
	{
		return false;
	}

	const ScopedJClass jActivityClass(*env, env->GetObjectClass(activity));

	if (!jActivityClass)
	{
		return false;
	}

	const ScopedJClass jWifiNetworkSuggestionClass(*env, env->FindClass("android/net/wifi/WifiNetworkSuggestion"));
	const ScopedJClass jBuilderClass(*env, env->FindClass("android/net/wifi/WifiNetworkSuggestion$Builder"));

	if (!jWifiNetworkSuggestionClass || !jBuilderClass)
	{
		return false;
	}

	// This follows the example of: https://developer.android.com/reference/android/provider/Settings.html#EXTRA_WIFI_NETWORK_LIST

	// final WifiNetworkSuggestion suggestion2 = new WifiNetworkSuggestion.Builder()
	//     .setSsid(ssid)
	//     .setWpa2Passphrase(password)
	//     .build();
	jmethodID jNewBuilderMethod = env->GetMethodID(jBuilderClass, "<init>", "()V");
	jmethodID jSetSsidMethod = env->GetMethodID(jBuilderClass, "setSsid", "(Ljava/lang/String;)Landroid/net/wifi/WifiNetworkSuggestion$Builder;");
	jmethodID jSetWpa2PassphraseMethod = env->GetMethodID(jBuilderClass, "setWpa2Passphrase", "(Ljava/lang/String;)Landroid/net/wifi/WifiNetworkSuggestion$Builder;");
	jmethodID jBuildMethod = env->GetMethodID(jBuilderClass, "build", "()Landroid/net/wifi/WifiNetworkSuggestion;");

	if (!jNewBuilderMethod || !jSetSsidMethod || !jSetWpa2PassphraseMethod || !jBuildMethod)
	{
		return false;
	}

	ScopedJObject jBuilderObject(*env, env->NewObject(jBuilderClass, jNewBuilderMethod));

	if (!jBuilderObject)
	{
		return false;
	}

	jBuilderObject = ScopedJObject(*env, env->CallObjectMethod(jBuilderObject, jSetSsidMethod, *jSsidString));

	if (!jBuilderObject)
	{
		return false;
	}

	jBuilderObject = ScopedJObject(*env, env->CallObjectMethod(jBuilderObject, jSetWpa2PassphraseMethod, *jPasswordString));

	if (!jBuilderObject)
	{
		return false;
	}

	const ScopedJObject jWifiNetworkSuggestionObject(*env, env->CallObjectMethod(jBuilderObject, jBuildMethod));

	if (!jWifiNetworkSuggestionObject)
	{
		return false;
	}

	// final List<WifiNetworkSuggestion> suggestionsList = new ArrayList<>;
	// suggestionsList.add(suggestion1);
	const ScopedJClass jArrayListClass(*env, env->FindClass("java/util/ArrayList"));

	if (!jArrayListClass)
	{
		return false;
	}

	jmethodID jnewArrayListMethod = env->GetMethodID(*jArrayListClass, "<init>", "()V");
	jmethodID jAddMethod = env->GetMethodID(jArrayListClass, "add", "(Ljava/lang/Object;)Z");

	if (!jnewArrayListMethod ||!jAddMethod)
	{
		return false;
	}

	const ScopedJObject jArrayListObject(*env, env->NewObject(jArrayListClass, jnewArrayListMethod));

	if (!jArrayListObject)
	{
		return false;
	}

	if (!env->CallBooleanMethod(*jArrayListObject, jAddMethod, *jWifiNetworkSuggestionObject))
	{
		return false;
	}

	// Bundle bundle = new Bundle();
	// bundle.putParcelableArrayList(Settings.EXTRA_WIFI_NETWORK_LIST,(ArrayList<? extends Parcelable>) suggestionsList);
	const ScopedJClass jBundleClass(*env, env->FindClass("android/os/Bundle"));

	if (!jBundleClass)
	{
		return false;
	}

	jmethodID jNewBundleMethod = env->GetMethodID(jBundleClass, "<init>", "()V");

	if (!jNewBundleMethod)
	{
		return false;
	}

	const ScopedJObject jBundleObject(*env, env->NewObject(jBundleClass, jNewBundleMethod));

	if (!jBundleObject)
	{
		return false;
	}

	const ScopedJClass jSettingsClass(*env, env->FindClass("android/provider/Settings"));

	if (!jSettingsClass)
	{
		return false;
	}

	jfieldID jExtraWifiNetworkListField = env->GetStaticFieldID(jSettingsClass, "EXTRA_WIFI_NETWORK_LIST", "Ljava/lang/String;");

	if (jExtraWifiNetworkListField == nullptr)
	{
		return false;
	}

	const ScopedJString jExtraWifiNetworkListString(*env, (jstring)(env->GetStaticObjectField(jSettingsClass, jExtraWifiNetworkListField)));

	if (!jExtraWifiNetworkListString.isValid())
	{
		return false;
	}

	jmethodID jPutParcelableArrayListMethod = env->GetMethodID(jBundleClass, "putParcelableArrayList", "(Ljava/lang/String;Ljava/util/ArrayList;)V");

	if (!jPutParcelableArrayListMethod)
	{
		return false;
	}

	env->CallVoidMethod(jBundleObject, jPutParcelableArrayListMethod, *jExtraWifiNetworkListString, *jArrayListObject);

	// final Intent intent = new Intent(Settings.ACTION_WIFI_ADD_NETWORKS);
	// intent.putExtras(bundle);
	jfieldID jActionWifiAddNetworksField = env->GetStaticFieldID(jSettingsClass, "ACTION_WIFI_ADD_NETWORKS", "Ljava/lang/String;");

	if (jActionWifiAddNetworksField == nullptr)
	{
		return false;
	}

	const ScopedJString jActionWifiAddNetworksString(*env, (jstring)(env->GetStaticObjectField(jSettingsClass, jActionWifiAddNetworksField)));

	if (!jActionWifiAddNetworksString.isValid())
	{
		return false;
	}

	const ScopedJClass jIntentClass(*env, env->FindClass("android/content/Intent"));

	if (!jSettingsClass)
	{
		return false;
	}

	jmethodID jNewIntentMethod = env->GetMethodID(jIntentClass, "<init>", "(Ljava/lang/String;)V");

	if (!jNewIntentMethod)
	{
		return false;
	}

	ScopedJObject jIntentObject(*env, env->NewObject(jIntentClass, jNewIntentMethod, *jActionWifiAddNetworksString));

	if (!jIntentObject)
	{
		return false;
	}

	jmethodID jPutExtrasMethod = env->GetMethodID(jIntentClass, "putExtras", "(Landroid/os/Bundle;)Landroid/content/Intent;");

	if (!jPutExtrasMethod)
	{
		return false;
	}

	jIntentObject = ScopedJObject(*env, env->CallObjectMethod(*jIntentObject, jPutExtrasMethod, *jBundleObject));

	if (!jIntentObject)
	{
		return false;
	}

	// this.startActivityForResult(intent, 0);

	jmethodID jStartActivityForResultMethod = env->GetMethodID(jActivityClass, "startActivityForResult", "(Landroid/content/Intent;I)V");

	if (!jStartActivityForResultMethod)
	{
		return false;
	}

	env->CallVoidMethod(activity, jStartActivityForResultMethod, *jIntentObject, jint(0));

	return true;
}

bool Utilities::currentWifiSsid(JNIEnv* env, jobject activity, std::string& ssid)
{
	ocean_assert(env != nullptr && activity != nullptr);

	unsigned int minSdkVersion = 0u;
	unsigned int targetSdkVersion = 0u;
	if (!manifestSdkVersions(env, activity, minSdkVersion, targetSdkVersion) || std::max(minSdkVersion, targetSdkVersion) > 28u)
	{
		Log::error() << "Functionality not available. Android SDK version 28 or lower is required (current target version: " << targetSdkVersion << ").";
		return false;
	}

	const ScopedJClass jActivityClass(*env, env->GetObjectClass(activity));

	if (!jActivityClass)
	{
		return false;
	}

	// ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
	// NetworkInfo networkInfo = connectivityManager.getActiveNetworkInfo();
	// if (networkInfo == null) {
	// 	/* no active network connection */
	// }

	const ScopedJClass jContextClass(*env, env->FindClass("android/content/Context"));
	const ScopedJClass jConnectivityManagerClass(*env, env->FindClass("android/net/ConnectivityManager"));
	const ScopedJClass jNetworkInfoClass(*env, env->FindClass("android/net/NetworkInfo"));

	if (!jContextClass || !jConnectivityManagerClass || !jNetworkInfoClass)
	{
		return false;
	}

	jfieldID jConnectivityServiceFieldId = env->GetStaticFieldID(jContextClass, "CONNECTIVITY_SERVICE", "Ljava/lang/String;");

	if (jConnectivityServiceFieldId == nullptr)
	{
		return false;
	}

	const ScopedJString jConnectivityServiceString(*env, (jstring)(env->GetStaticObjectField(jContextClass, jConnectivityServiceFieldId)));

	if (!jConnectivityServiceString.isValid())
	{
		return false;
	}

	jmethodID jGetSystemServiceMethodId = env->GetMethodID(jContextClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");

	if (jGetSystemServiceMethodId == nullptr)
	{
		return false;
	}

	const ScopedJObject jConnectivityManagerObject(*env, env->CallObjectMethod(activity, jGetSystemServiceMethodId, *jConnectivityServiceString));

	if (!jConnectivityManagerObject)
	{
		return false;
	}

	jmethodID jGetActiveNetworkInfoMethodId = env->GetMethodID(jConnectivityManagerClass, "getActiveNetworkInfo", "()Landroid/net/NetworkInfo;");

	if (jGetActiveNetworkInfoMethodId == nullptr)
	{
		return false;
	}

	const ScopedJObject jNetworkInfoObject(*env, env->CallObjectMethod(jConnectivityManagerObject, jGetActiveNetworkInfoMethodId));

	if (!jNetworkInfoObject)
	{
		// No active network connection
		ssid = "";
		return true;
	}

	// if (networkInfo.isConnected())
	// {
	//     final WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
	//     final WifiInfo wifiInfo = wifiManager.getConnectionInfo();
	//     if (wifiInfo != null && !StringUtil.isBlank(wifiInfo.getSSID()))
	//     {
	//         ssid = wifiInfo.getSSID();
	//     }
	// }
	jmethodID jIsConnectedMethodId = env->GetMethodID(jNetworkInfoClass, "isConnected", "()Z");

	if (jGetSystemServiceMethodId == nullptr)
	{
		return false;
	}

	if (!env->CallBooleanMethod(*jNetworkInfoObject, jIsConnectedMethodId))
	{
		// No active network connection
		ssid = "";
		return true;
	}

	const ScopedJClass jWifiManagerClass(*env, env->FindClass("android/net/wifi/WifiManager"));
	const ScopedJClass jWifiInfoClass(*env, env->FindClass("android/net/wifi/WifiInfo"));

	if (!jWifiManagerClass || !jWifiInfoClass)
	{
		return false;
	}

	jfieldID jWifiServiceFieldId = env->GetStaticFieldID(jContextClass, "WIFI_SERVICE", "Ljava/lang/String;");

	if (jWifiServiceFieldId == nullptr)
	{
		return false;
	}

	const ScopedJString jWifiServiceString(*env, (jstring)(env->GetStaticObjectField(jContextClass, jWifiServiceFieldId)));

	if (!jWifiServiceString.isValid())
	{
		return false;
	}

	const ScopedJObject jWifiManagerObject(*env, env->CallObjectMethod(activity, jGetSystemServiceMethodId, *jWifiServiceString));

	if (!jWifiManagerObject)
	{
		return false;
	}

	jmethodID jGetConnectionInfoMethodId = env->GetMethodID(jWifiManagerClass, "getConnectionInfo", "()Landroid/net/wifi/WifiInfo;");
	jmethodID jGetSsidMethodId = env->GetMethodID(jWifiInfoClass, "getSSID", "()Ljava/lang/String;");

	if (jGetSystemServiceMethodId == nullptr || jGetSsidMethodId == nullptr)
	{
		return false;
	}

	const ScopedJObject jWifiInfoObject(*env, env->CallObjectMethod(jWifiManagerObject, jGetConnectionInfoMethodId, *jWifiServiceString));

	if (!jWifiManagerObject)
	{
		return false;
	}

	const ScopedJString jSsidString(*env, (jstring)(env->CallObjectMethod(jWifiInfoObject, jGetSsidMethodId)));

	if (!jSsidString.isValid())
	{
		return false;
	}

	// The SSID will will be returned surrounded by double quotation marks (e.g. "metaguest"). Otherwise, it is returned as a string of hex digits.
	const std::string paddedSsid = toAString(env, jSsidString);
	ocean_assert(!paddedSsid.empty());

	if (paddedSsid.size() >= 2 && paddedSsid.front() == '"' && paddedSsid.back() == '"')
	{
		ssid = paddedSsid.substr(1, paddedSsid.size() - 2);
	}
	else
	{
		ssid = paddedSsid;
	}

	ocean_assert(!ssid.empty());

	return true;
}

}

}

}
